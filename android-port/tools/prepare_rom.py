"""Validate and normalize a user's ROM, then run the pinned upstream decompressor.

Never modifies the supplied ROM. ROM data stays in ignored local build directories.
"""
from pathlib import Path
import argparse
import hashlib
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_SHA1 = "cf806ff2603640a748fca5026ded28802f1f4a50"


def normalize(data: bytes) -> bytes:
    if len(data) != 0x2000000:
        raise ValueError("Expected a 32 MiB US DK64 ROM (not a ZIP or decompressed ROM).")
    magic = data[:4]
    if magic == bytes.fromhex("37804012"):
        result = bytearray(len(data))
        result[0::2], result[1::2] = data[1::2], data[0::2]
        data = bytes(result)
    elif magic == bytes.fromhex("40123780"):
        result = bytearray(len(data))
        for index in range(4):
            result[index::4] = data[3 - index::4]
        data = bytes(result)
    elif magic != bytes.fromhex("80371240"):
        raise ValueError("Unrecognized N64 byte order.")
    digest = hashlib.sha1(data).hexdigest()
    if digest != EXPECTED_SHA1:
        raise ValueError(f"Unsupported ROM: normalized SHA1 {digest}; expected {EXPECTED_SHA1}.")
    return data


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("rom", type=Path)
    args = parser.parse_args()
    # Reject oversize input before allocating; normalization also checks exact size.
    if args.rom.stat().st_size != 0x2000000:
        parser.error("Expected a 32 MiB ROM.")
    data = normalize(args.rom.read_bytes())
    work = ROOT / ".local" / "rom-source"
    work.mkdir(parents=True, exist_ok=True)
    (work / "baserom.us.z64").write_bytes(data)
    script = ROOT / "upstream/lib/dk64_decomp/tools/generate_decompressed_rom.py"
    subprocess.run([sys.executable, str(script)], cwd=work, check=True)
    destination = ROOT / "upstream/donkeykong64.decompressed.us.z64"
    shutil.copyfile(work / "baserom.us.decompressed.z64", destination)
    print(f"Verified US DK64 SHA1: {EXPECTED_SHA1}")
    print(f"Build input ready: {destination}")


if __name__ == "__main__":
    main()
