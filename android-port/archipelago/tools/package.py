"""Build the direct Android mod, corresponding source and separate developer AP world."""
from pathlib import Path
import hashlib
import json
import struct
import subprocess
import zipfile

root = Path(__file__).resolve().parents[1]
dist = root / "dist"
dist.mkdir(exist_ok=True)
# The direct native client supersedes the 0.1 desktop bridge. Do not leave its
# old bundle beside current artifacts after packaging a direct-mod release.
(dist / "DK64-Archipelago-0.1.0-desktop-relay.zip").unlink(missing_ok=True)
nrm = dist / "dk64_archipelago.nrm"
library = root / "build/android/dk64_ap_bridge.so"
with zipfile.ZipFile(nrm) as archive:
    manifest = json.loads(archive.read("mod.json"))
    if manifest["native_libraries"] != {"dk64_ap_bridge": ["dk64_ap_configure_v2", "dk64_ap_start_v2", "dk64_ap_tick_v2", "dk64_ap_labels_v2", "dk64_ap_stop_v2", "dk64_ap_campaign_configure_v3", "dk64_ap_campaign_start_v3", "dk64_ap_campaign_connect_v3", "dk64_ap_campaign_tick_v3", "dk64_ap_campaign_stage_v3", "dk64_ap_campaign_stop_v3"]}:
        raise ValueError("Unexpected companion ABI")
    if manifest["version"] != "0.2.0" or manifest["game_id"] != "dk64":
        raise ValueError("Unexpected manifest")
    options = {o["id"]: o for o in manifest["config_schema"]["options"]}
    if options["enabled"]["default"] != "Off": raise ValueError("Test must default Off")
    if set(options) != {"enabled", "server", "slot_name", "password", "seed", "team", "slot"}:
        raise ValueError("Unexpected direct client settings")
raw = library.read_bytes()
if raw[:7] != b"\x7fELF\x02\x01\x01" or struct.unpack_from("<HH", raw, 16) != (3, 183):
    raise ValueError("Not an Android ARM64 shared library")
offset = struct.unpack_from("<Q", raw, 32)[0]
stride, count = struct.unpack_from("<HH", raw, 54)
loads = [struct.unpack_from("<IIQQQQQQ", raw, offset+i*stride) for i in range(count)]
if not all(p[7] >= 16384 and p[2] % 16384 == p[3] % 16384 for p in loads if p[0] == 1):
    raise ValueError("ELF load segments must be aligned for 16 KiB pages")
android = dist / "DK64-Archipelago-0.2.0-android-direct-test.zip"
with zipfile.ZipFile(android, "w", zipfile.ZIP_DEFLATED) as bundle:
    for path in (nrm, library, root / "README.md", root / "FULL-PORT.md", root / "LICENSE", root / "THIRD_PARTY.md", root / "upstream.json", root / "native-dependencies.json"):
        bundle.write(path, path.name)
    for path in sorted((root / "licenses").glob("*")):
        bundle.write(path, "licenses/" + path.name)
world = dist / "dk64_recomp_test.apworld"
with zipfile.ZipFile(world, "w", zipfile.ZIP_DEFLATED) as bundle:
    for name in ("__init__.py", "archipelago.json"):
        bundle.write(root / "test-world" / name, "dk64_recomp_test/" + name)
    bundle.write(root / "licenses/DK64-Randomizer.txt", "dk64_recomp_test/LICENSE-DK64-Randomizer.txt")
    bundle.write(root / "LICENSE", "dk64_recomp_test/LICENSE")
source = dist / "DK64-Archipelago-0.2.0-source.zip"
with zipfile.ZipFile(source, "w", zipfile.ZIP_DEFLATED) as bundle:
    for name in ("CMakeLists.txt", "mod.toml", "README.md", "FULL-PORT.md", "THIRD_PARTY.md", "LICENSE", "upstream.json", "native-dependencies.json", ".gitignore"):
        bundle.write(root / name, "archipelago/" + name)
    for folder in ("mod", "native", "generated", "tools", "tests", "test-world", "vendor", "licenses"):
        for path in sorted((root / folder).rglob("*")):
            if path.is_file() and "__pycache__" not in path.parts and path.suffix != ".pyc":
                bundle.write(path, "archipelago/" + path.relative_to(root).as_posix())
    # Include the complete pinned dependency sources, including our bounded-input patch.
    for name in ("ap-ixwebsocket", "ap-mbedtls"):
        dependency = root.parent / ".local" / name
        files = subprocess.check_output(["git", "-C", str(dependency), "ls-files", "--recurse-submodules", "-z"]).decode().split("\0")
        for name_in_repo in files:
            path = dependency / name_in_repo
            if name_in_repo and path.is_file(): bundle.write(path, ".local/" + name + "/" + name_in_repo)
lines = [f"{hashlib.sha256(p.read_bytes()).hexdigest()}  {p.name}" for p in (android, world, source, nrm)]
(dist / "SHA256SUMS.txt").write_text("\n".join(lines) + "\n")
print("\n".join(lines))
