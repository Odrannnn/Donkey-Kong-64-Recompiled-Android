"""Local fault-injection and TLS tests for the direct native client. No game files."""
import asyncio
import json
from pathlib import Path
import ssl
import struct
import subprocess
import sys
import tempfile
from websockets.legacy.server import serve
from websockets.exceptions import ConnectionClosed
from real_server_smoke import peer

CHECKSUM = "1111111111111111111111111111111111111111"
ROOM = {"cmd": "RoomInfo", "seed_name": "seed", "games": ["DK64 Recomp Integration Test"],
    "datapackage_checksums": {"DK64 Recomp Integration Test": CHECKSUM}}
CONNECTED = {"cmd": "Connected", "team": 0, "slot": 1,
    "slot_data": {"recomp_profile": "dk64-recomp-first-moves-japes-v1", "Version": "1.5.8"},
    "missing_locations": [14041180, 14041181], "checked_locations": [],
    "slot_info": {"1": {"name": "DK64Test", "game": "DK64 Recomp Integration Test", "type": 1, "class": "NetworkSlot"}},
    "players": [{"team": 0, "slot": 1, "alias": "DK Test Alias", "name": "DK64Test", "class": "NetworkPlayer"}]}
ITEMS = [[14041104, 14041180, 1, 1], [14041108, 14041181, 1, 1]]

async def main(binary, tls_probe):
    with tempfile.TemporaryDirectory(prefix="ap-direct-network-") as directory:
        root = Path(directory)
        connections, got_checks, got_goal, got_scouts = 0, False, False, False
        async def reconnect_server(ws):
            nonlocal connections, got_checks, got_goal, got_scouts
            connections += 1
            try:
                await ws.send(json.dumps([ROOM]))
                connect = json.loads(await ws.recv())[0]
                assert connect["cmd"] == "Connect" and connect["items_handling"] == 7
                await ws.send(json.dumps([CONNECTED]))
                if connections == 1:
                    await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS[:1]}]))
                    await asyncio.sleep(.3)
                    await ws.close()
                    return
                await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS}]))
                await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS}]))
                async for raw in ws:
                    for command in json.loads(raw):
                        if command["cmd"] == "LocationChecks": got_checks |= command["locations"] == [14041180, 14041181]
                        if command["cmd"] == "StatusUpdate": got_goal |= command["status"] == 30
                        if command["cmd"] == "LocationScouts":
                            got_scouts |= command["locations"] == [14041180, 14041181] and command["create_as_hint"] == 0
                            await ws.send(json.dumps([{"cmd": "LocationInfo", "locations": [
                                {"item": 14041104, "location": 14041180, "player": 1, "flags": 1, "class": "NetworkItem"},
                                {"item": 14041108, "location": 14041181, "player": 1, "flags": 1, "class": "NetworkItem"}]}]))
                        if command["cmd"] == "GetDataPackage":
                            assert command["games"] == ["DK64 Recomp Integration Test"]
                            await ws.send(json.dumps([{"cmd": "DataPackage", "data": {"games": {
                                "DK64 Recomp Integration Test": {"checksum": CHECKSUM, "item_name_to_id": {
                                    "Baboon Blast": 14041104, "Chimpy Charge": 14041108}}}}}]))
            except ConnectionClosed:
                pass
        config = {"server": "", "name": "DK64Test", "password": "", "seed": "seed", "expect_scouts": 2}
        config_path = root / "config.json"
        async with serve(reconnect_server, "127.0.0.1", 0) as server:
            config["server"] = f"ws://127.0.0.1:{server.sockets[0].getsockname()[1]}"
            config_path.write_text(json.dumps(config))
            await peer(binary, config_path, root / "reconnect.json", 3)
        assert connections >= 2 and got_checks and got_goal and got_scouts
        assert len(json.loads((root / "reconnect.json").read_text())["items"]) == 2

        async def malformed(ws):
            await ws.send("[" * 40 + "0" + "]" * 40)
            await ws.wait_closed()
        async with serve(malformed, "127.0.0.1", 0) as server:
            config["server"] = f"ws://127.0.0.1:{server.sockets[0].getsockname()[1]}"
            config_path.write_text(json.dumps(config))
            await peer(binary, config_path, root / "malformed.json", 0, expected=3)

        # Reject a huge advertised length before allocation, and excessive tiny fragments.
        for case, wire in (("huge-frame", struct.pack("!BBQ", 0x81, 127, 1 << 40)),
                           ("fragments", b"\x01\x00" + b"\x00\x00" * 256)):
            rejected = asyncio.Event()
            async def oversized(ws):
                ws.transport.write(wire)
                try:
                    await ws.recv()
                except ConnectionClosed as error:
                    if error.code == 1009: rejected.set()
            async with serve(oversized, "127.0.0.1", 0) as server:
                config["server"] = f"ws://127.0.0.1:{server.sockets[0].getsockname()[1]}"
                config_path.write_text(json.dumps(config))
                proc = await asyncio.create_subprocess_exec(str(binary), str(config_path), str(root / f"{case}.json"), "0",
                    stdout=asyncio.subprocess.PIPE)
                try:
                    await asyncio.wait_for(rejected.wait(), 8)
                    assert proc.returncode is None
                finally:
                    if proc.returncode is None: proc.terminate()
                    await proc.wait()

        # Local certificate trusted only by the test probe, with hostname localhost.
        cert, key = root / "cert.pem", root / "key.pem"
        subprocess.run(["openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes", "-days", "1",
            "-subj", "/CN=localhost", "-addext", "subjectAltName=DNS:localhost", "-keyout", str(key), "-out", str(cert)],
            check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER); context.load_cert_chain(cert, key)
        authenticated = 0
        async def tls_server(ws):
            nonlocal authenticated
            authenticated += 1
            await ws.wait_closed()
        async with serve(tls_server, "127.0.0.1", 0, ssl=context) as server:
            port = server.sockets[0].getsockname()[1]
            for host, expected in (("localhost", 1), ("127.0.0.1", 2)):
                proc = await asyncio.create_subprocess_exec(str(tls_probe), f"wss://{host}:{port}", str(cert), str(expected))
                assert await asyncio.wait_for(proc.wait(), 15) == 0
            assert authenticated == 1, "Mismatched hostname accepted"
            config["server"] = f"wss://localhost:{port}"; config_path.write_text(json.dumps(config))
            proc = await asyncio.create_subprocess_exec(str(binary), str(config_path), str(root / "untrusted.json"), "0",
                stdout=asyncio.subprocess.PIPE)
            try:
                await asyncio.sleep(4)
                assert authenticated == 1, "Production client trusted a self-signed certificate"
            finally:
                proc.terminate(); await proc.wait()
        print("PASS: direct reconnect/replay, scouts/names, checks, goal, malformed JSON, frame/fragment bounds, TLS trust and hostname rejection")

if __name__ == "__main__":
    asyncio.run(main(*(Path(arg).resolve() for arg in sys.argv[1:3])))
