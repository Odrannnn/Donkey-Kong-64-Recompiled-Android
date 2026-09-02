"""Package standard companion ZIPs without rewriting the RecompModTool NRM."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
import zipfile

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--nrm", type=Path, default=root / "dist" / "dk64_lan_coop.nrm")
parser.add_argument("--android-library", type=Path, default=root / "build" / "android" / "dk64_coop_bridge.so")
parser.add_argument("--windows-library", type=Path, default=root / "build" / "windows" / "dk64_coop_bridge.dll")
parser.add_argument("--apk", type=Path, help="Verified compatible Android application APK to include in checksums")
parser.add_argument("--dist", type=Path, default=root / "dist")
args = parser.parse_args()
dist = args.dist.resolve()
dist.mkdir(parents=True, exist_ok=True)
nrm = args.nrm.resolve()
with zipfile.ZipFile(nrm) as archive:
    try:
        manifest = json.loads(archive.read("mod.json"))
    except (KeyError, json.JSONDecodeError) as error:
        raise ValueError("NRM must contain a valid mod.json") from error
    version = manifest.get("version")
    if not isinstance(version, str) or not re.fullmatch(r"0\.[0-9]+\.[0-9]+", version):
        raise ValueError(f"NRM has an unsupported semantic version: {version!r}")
    try:
        protocol = int(version.split(".")[1])
    except ValueError as error:
        raise ValueError(f"NRM version does not encode a protocol minor version: {version!r}") from error
    expected_functions = ["dk64_coop_start", "dk64_coop_local_ipv4"]
    if protocol >= 52:
        expected_functions += [
            f"dk64_coop_recovery_configure_v{protocol}",
            f"dk64_coop_recovery_status_v{protocol}",
            f"dk64_coop_recovery_promote_v{protocol}",
        ]
    expected_functions += [f"dk64_coop_tick_v{protocol}", "dk64_coop_stop"]
    if manifest.get("native_libraries") != {"dk64_coop_bridge": expected_functions}:
        raise ValueError("NRM native_libraries must contain exactly the derived start/address/tick/stop exports")
    if any(name.endswith((".dll", ".so")) for name in archive.namelist()):
        raise ValueError("NRM must not embed companion libraries")
    options = {option["id"]: option for option in manifest["config_schema"]["options"]}
    if (options["role"]["default"], options["combat"]["default"], options["same_area_events"]["default"],
            options["shared_items"]["default"], options["automatic_world_refresh"]["default"]) != ("Off", "Off", "Off", "Off", "Off"):
        raise ValueError("NRM experimental options must default to Off")
    if options["shared_items"]["options"] != ["Off", "Unique collectibles"] or options["combat"]["options"] != ["Off", "Shots and enemy defeats", "Shots, defeats and enemy movement", "Shots, defeats, movement and enemy pose"]:
        raise ValueError("NRM option schema is malformed")
    if options["port"]["default"] != 6464 or options["room"]["default"] != 123456:
        raise ValueError("NRM LAN defaults are malformed")
    if options["save_profile"]["default"] != "Campaign 1 (existing)" or len(options["save_profile"]["options"]) != 8:
        raise ValueError("NRM co-op campaign save schema is malformed")
    if options["save_conflict"]["default"] != "Stop safely" or options["save_conflict"]["options"] != ["Stop safely", "Merge guest progress into host"]:
        raise ValueError("NRM save-conflict policy schema is malformed")

source_files = [root / name for name in ("CMakeLists.txt", "mod.toml", ".gitignore", "README.md", "BUILDING.md", "TECHNICAL-NOTES.md", "WORLD-UNLOCKS.md", "THIRD_PARTY.md", "LICENSE")]
for directory in ("mod", "native", "tests", "tools", "vendor", "licenses"):
    source_files.extend(file for file in sorted((root / directory).rglob("*")) if file.is_file() and "__pycache__" not in file.parts and file.suffix != ".pyc")

artifacts = []
for platform, library in (("android", args.android_library.resolve()), ("windows", args.windows_library.resolve())):
    raw = library.read_bytes()
    if platform == "android":
        assert raw[:7] == b"\x7fELF\x02\x01\x01"
        assert struct.unpack_from("<HH", raw, 16) == (3, 183)
        offset = struct.unpack_from("<Q", raw, 32)[0]
        stride, count = struct.unpack_from("<HH", raw, 54)
        loads = [struct.unpack_from("<IIQQQQQQ", raw, offset + i * stride) for i in range(count)]
        assert all(p[7] >= 16384 and p[2] % 16384 == p[3] % 16384 for p in loads if p[0] == 1)
    else:
        assert raw[:2] == b"MZ"
        pe = struct.unpack_from("<I", raw, 60)[0]
        assert raw[pe:pe + 4] == b"PE\0\0" and struct.unpack_from("<H", raw, pe + 4)[0] == 0x8664
    path = dist / f"DK64-LAN-Coop-{version}-{platform}-prototype.zip"
    with zipfile.ZipFile(path, "w", zipfile.ZIP_DEFLATED) as bundle:
        for file in (nrm, library, root / "README.md", root / "BUILDING.md", root / "TECHNICAL-NOTES.md", root / "WORLD-UNLOCKS.md", root / "LICENSE", root / "THIRD_PARTY.md"):
            bundle.write(file, file.name)
        for notice in sorted((root / "licenses").glob("*")):
            if notice.is_file(): bundle.write(notice, "licenses/" + notice.name)
        for file in source_files:
            bundle.write(file, "source/coop/" + file.relative_to(root).as_posix())
    with zipfile.ZipFile(path) as bundle:
        assert bundle.read(nrm.name) == nrm.read_bytes()
        assert bundle.read(library.name) == raw
        if "BUILDING.md" not in bundle.namelist() or any("source/coop/" + file.relative_to(root).as_posix() not in bundle.namelist() for file in source_files):
            raise ValueError("companion archive is missing the current build instructions or source snapshot")
    artifacts.append(path)
source = dist / f"DK64-LAN-Coop-{version}-source.zip"
with zipfile.ZipFile(source, "w", zipfile.ZIP_DEFLATED) as bundle:
    for file in source_files:
        bundle.write(file, "coop/" + file.relative_to(root).as_posix())
artifacts.append(source)
with zipfile.ZipFile(source) as bundle:
    if any("coop/" + file.relative_to(root).as_posix() not in bundle.namelist() for file in source_files):
        raise ValueError("source archive is missing current build, native, or test sources")
checksum_artifacts = artifacts + [nrm]
if args.apk:
    apk = args.apk.resolve()
    if not apk.is_file() or apk.parent != dist:
        raise ValueError("Verified APK must already be a file in the selected dist directory")
    checksum_artifacts.append(apk)
lines = [f"{hashlib.sha256(path.read_bytes()).hexdigest()}  {path.name}" for path in checksum_artifacts]
(dist / "SHA256SUMS.txt").write_text("\n".join(lines) + "\n")
print("\n".join(lines))
