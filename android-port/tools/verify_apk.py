"""Verify package contents, ARM64 ELF architecture and 16 KiB native load alignment."""
from pathlib import Path
import struct
import sys
import zipfile

required = {"libmain.so", "libSDL2.so", "libc++_shared.so", "libdk64vulkan.so", "libhook_impl.so",
            "libmain_hook.so", "libfile_redirect_hook.so", "libgsl_alloc_hook.so"}
with zipfile.ZipFile(sys.argv[1]) as apk:
    names = apk.namelist()
    native = {Path(name).name for name in names if name.startswith("lib/") and name.endswith(".so")}
    assert native == required, f"Unexpected native libraries: {native ^ required}"
    assert not any(Path(name).suffix.lower() in {".z64", ".n64", ".v64", ".rom"} for name in names), "ROM in APK"
    assert not any("gpu-drivers" in name or "freedreno" in name or "VkLayer" in name for name in names), "Imported driver or diagnostic layer in normal APK"
    for name in names:
        if not name.startswith("lib/") or not name.endswith(".so"): continue
        assert name.startswith("lib/arm64-v8a/"), name
        data = apk.read(name)
        assert data[:6] == b"\x7fELF\x02\x01" and struct.unpack_from("<H", data, 18)[0] == 183, name
        offset = struct.unpack_from("<Q", data, 32)[0]
        entry_size, count = struct.unpack_from("<HH", data, 54)
        for i in range(count):
            header = struct.unpack_from("<IIQQQQQQ", data, offset + i * entry_size)
            if header[0] == 1: assert header[7] >= 16384, f"{name} needs 16 KiB alignment"
    assert "assets/program/assets/launcher.rml" in names or any(name.startswith("assets/program/assets/") for name in names)
print("APK verified: 8 ARM64 libraries, 16 KiB load alignment, program assets, no ROM, imported driver or validation layer.")
