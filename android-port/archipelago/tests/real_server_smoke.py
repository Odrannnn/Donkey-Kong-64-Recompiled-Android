"""Real AP server -> production native client, no relay or UDP.

Run with the isolated AP server's Python: tests/real_server_smoke.py AP_SOURCE SEED_ZIP NATIVE_PEER
Only creates temporary localhost server data; never touches the game or installed AP.
"""
import asyncio
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import tempfile
from websockets.legacy.client import connect

async def peer(binary, config, journal, checks, expected=0):
    process = await asyncio.create_subprocess_exec(str(binary), str(config), str(journal), str(checks),
        stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
    try:
        output, _ = await asyncio.wait_for(process.communicate(), 50)
    except BaseException:
        process.kill(); await process.wait(); raise
    assert process.returncode == expected, (process.returncode, output.decode())
    return output.decode()

async def main(source, seed, binary):
    with tempfile.TemporaryDirectory(prefix="ap-direct-server-") as directory:
        scratch = Path(directory)
        with socket.socket() as probe:
            probe.bind(("127.0.0.1", 0)); port = probe.getsockname()[1]
        env = dict(os.environ, SKIP_REQUIREMENTS_UPDATE="1")
        with (scratch / "server.log").open("w") as logfile:
            process = subprocess.Popen([sys.executable, str(source / "MultiServer.py"), str(seed),
                "--host", "127.0.0.1", "--port", str(port), "--password", "disposable-test-password",
                "--disable_save", "--loglevel", "info"], cwd=source, env=env, stdin=subprocess.PIPE,
                stdout=logfile, stderr=subprocess.STDOUT, creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
            try:
                for _ in range(150):
                    if process.poll() is not None:
                        raise RuntimeError((scratch / "server.log").read_text())
                    try:
                        async with connect(f"ws://127.0.0.1:{port}") as ws:
                            info = json.loads(await ws.recv())[0]
                        break
                    except OSError:
                        await asyncio.sleep(0.1)
                else:
                    raise TimeoutError("AP server did not start")
                config = {"server": f"ws://127.0.0.1:{port}", "seed": info["seed_name"], "team": 0,
                    "slot": 1, "name": "DK64Test", "password": "disposable-test-password", "expect_scouts": 2}
                config_path = scratch / "config.json"
                config_path.write_text(json.dumps(config))
                journal = scratch / "state.json"
                output = await peer(binary, config_path, journal, 3)
                assert "65539" in output and "scouts=2 labels=0" in output, output
                saved = json.loads(journal.read_text())
                assert len(saved["items"]) == 2 and saved["checks"] == 3
                assert "password" not in journal.read_text()
                assert "has completed their goal" in (scratch / "server.log").read_text()
                output = await peer(binary, config_path, journal, 0)
                assert "65539" in output and "scouts=2 labels=0" in output and json.loads(journal.read_text()) == saved
                config["password"] = "wrong"
                config_path.write_text(json.dumps(config))
                await peer(binary, config_path, scratch / "bad-password.json", 0, expected=3)
                config["seed"] = "wrong-seed"
                config_path.write_text(json.dumps(config))
                await peer(binary, config_path, scratch / "bad-seed.json", 0, expected=3)
                print("PASS: real AP 0.6.7 direct authentication, scouts/names, checks, items, goal, journal replay, wrong-password and wrong-seed rejection")
            finally:
                if process.poll() is None:
                    process.stdin.write(b"/exit\n"); process.stdin.flush()
                    try: process.wait(timeout=5)
                    except subprocess.TimeoutExpired: process.terminate(); process.wait(timeout=5)

if __name__ == "__main__":
    asyncio.run(main(*(Path(arg).resolve() for arg in sys.argv[1:4])))
