"""Direct full-campaign protocol reconnect/replay test. No game or relay."""
import asyncio
import json
from pathlib import Path
import sys
import tempfile
from websockets.legacy.server import serve
from websockets.exceptions import ConnectionClosed

ITEMS = [[14041104, 14041180, 1, 1], [14041108, 14041181, 1, 1]]

def manifest():
    return {
        "format": 1, "world_version": "1.5.8",
        "revision": "66d0dc90064a572e9bf2a2eada53ef81a7f47eb4", "game": "Donkey Kong 64",
        "seed_name": "campaign-network", "player": "DK64Test", "team": 0, "slot": 1,
        "locations": [14041180, 14041181], "starting_inventory": [],
        "blockers": [{"item": "keys", "count": i + 1} for i in range(8)],
        "boss_bananas": [60, 120, 200, 250, 300, 350, 400], "edits": {"entrances": []},
    }

async def main(binary):
    with tempfile.TemporaryDirectory(prefix="dkap-campaign-network-") as directory:
        root = Path(directory); connections = 0; got_checks = False
        room = {"cmd": "RoomInfo", "seed_name": "campaign-network", "games": ["Donkey Kong 64"]}
        connected = {"cmd": "Connected", "team": 0, "slot": 1,
            "slot_data": {"Version": "1.5.8"}, "checked_locations": [],
            "missing_locations": [14041180, 14041181]}
        async def server(ws):
            nonlocal connections, got_checks
            connections += 1
            try:
                await ws.send(json.dumps([room]))
                login = json.loads(await ws.recv())[0]
                assert login["cmd"] == "Connect" and login["game"] == "Donkey Kong 64"
                assert login["items_handling"] == 1 and login["name"] == "DK64Test"
                assert login["uuid"].startswith("dk64recomp-campaign-")
                await ws.send(json.dumps([connected]))
                if connections == 1:
                    await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS[:1]}]))
                    await asyncio.sleep(.3); await ws.close(); return
                await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS}]))
                await ws.send(json.dumps([{"cmd": "ReceivedItems", "index": 0, "items": ITEMS}]))
                async for raw in ws:
                    for command in json.loads(raw):
                        if command["cmd"] == "LocationChecks":
                            got_checks |= command["locations"] == [14041180, 14041181]
            except ConnectionClosed:
                pass
        config_path, manifest_path = root / "config.json", root / "manifest.json"
        manifest_path.write_text(json.dumps(manifest()))
        async with serve(server, "127.0.0.1", 0) as websocket_server:
            config_path.write_text(json.dumps({"server": f"ws://127.0.0.1:{websocket_server.sockets[0].getsockname()[1]}"}))
            process = await asyncio.create_subprocess_exec(str(binary), str(config_path), str(manifest_path),
                str(root / "campaign.json"), "2")
            assert await asyncio.wait_for(process.wait(), 45) == 0
        journal = json.loads((root / "campaign.json").read_text())
        assert connections >= 2 and got_checks and len(journal["items"]) == 2
        assert journal["checks"] == [14041180, 14041181]
        print("PASS: direct campaign authentication, durable checks/items and reconnect replay")

if __name__ == "__main__":
    asyncio.run(main(Path(sys.argv[1]).resolve()))
