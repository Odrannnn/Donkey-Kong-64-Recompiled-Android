"""Read-only backup before the local gameplay smoke test; never includes the ROM."""
import os
from pathlib import Path
import subprocess
import hashlib
import tarfile
import json
import argparse

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--serial", required=True, help="Explicit adb tablet serial; never auto-select a device")
args = parser.parse_args()
adb = Path(os.environ["LOCALAPPDATA"]) / "Android/Sdk/platform-tools/adb.exe"
target = [str(adb), "-s", args.serial]
active = subprocess.run(target + ["shell", "pidof", "io.github.dk64port:game"], capture_output=True, check=False).stdout.strip()
if active:
    raise SystemExit("Refusing test setup: DK64 game is active. Save and close it first.")
backup = root / "build/verification/tablet-before-coop.tar"
backup.parent.mkdir(parents=True, exist_ok=True)
if backup.exists():
    raise SystemExit("Backup already exists; refusing to overwrite the pre-test copy.")
with backup.open("wb") as output:
    subprocess.run(target + ["exec-out", "run-as", "io.github.dk64port", "tar", "-cf", "-", "-C", "files/data",
        "saves", "mods", "mod_config", "mods.json", "general.json", "graphics.json", "graphics-driver.json"], stdout=output, check=True)
hashes = {}
with tarfile.open(backup) as archive:
    for member in archive:
        if member.isfile(): hashes[member.name] = hashlib.sha256(archive.extractfile(member).read()).hexdigest()
(backup.parent / "tablet-before-hashes.json").write_text(json.dumps(hashes, indent=2) + "\n")
print(f"Backed up {len(hashes)} save/mod/config files, excluding ROMs; SHA256 {hashlib.sha256(backup.read_bytes()).hexdigest()}")
