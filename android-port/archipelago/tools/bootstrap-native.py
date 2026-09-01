"""Fetch pinned native dependencies and apply the reviewed receive-size limits."""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess

ROOT = Path(__file__).resolve().parents[1]
DEPS = ROOT.parent / ".local"
DEPENDENCIES = {
    "ap-ixwebsocket": ("https://github.com/machinezone/IXWebSocket.git", "64fae7676bd8fe31f7cb4bcde7a6841892dad65e"),
    "ap-mbedtls": ("https://github.com/Mbed-TLS/mbedtls.git", "068ff080b369adfac81509f9b57b2afabaf82dc5"),
}

def patch(folder, name, old, new):
    path = folder / "ixwebsocket" / name
    text = path.read_text()
    if new in text:
        return
    if text.count(old) != 1:
        raise RuntimeError(f"Unexpected dependency contents: {name}")
    path.write_text(text.replace(old, new), newline="\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--offline", action="store_true", help="Verify existing checkouts; do not fetch")
    args = parser.parse_args()
    lock = json.loads((ROOT / "native-dependencies.json").read_text(encoding="utf-8"))
    for name, digest in lock["files"].items():
        if hashlib.sha256((ROOT / name).read_bytes()).hexdigest() != digest:
            raise RuntimeError(f"Vendored source checksum mismatch: {name}")
    DEPS.mkdir(exist_ok=True)
    for name, (url, commit) in DEPENDENCIES.items():
        path = DEPS / name
        if not path.exists():
            if args.offline:
                raise RuntimeError(f"Missing {path}")
            subprocess.run(["git", "clone", "--no-checkout", url, str(path)], check=True)
            subprocess.run(["git", "-C", str(path), "checkout", "--detach", commit], check=True)
        actual = subprocess.check_output(["git", "-C", str(path), "rev-parse", "HEAD"], text=True).strip()
        if actual != commit:
            raise RuntimeError(f"{name}: expected {commit}, found {actual}; refusing to replace an existing checkout")
        if name == "ap-mbedtls" and not args.offline:
            subprocess.run(["git", "-C", str(path), "submodule", "update", "--init", "--recursive"], check=True)
        if name == "ap-mbedtls":
            framework = subprocess.check_output(["git", "-C", str(path / "framework"), "rev-parse", "HEAD"], text=True).strip()
            if framework != lock["mbedtls"]["framework_commit"]:
                raise RuntimeError("Unexpected mbedTLS framework revision")
    ix = DEPS / "ap-ixwebsocket"
    patch(ix, "IXWebSocketTransport.cpp", "const uint64_t maxFrameSize(1ULL << 63);", "const uint64_t maxFrameSize(1ULL << 20); // DK64 AP: bounded frames")
    patch(ix, "IXWebSocketTransport.cpp", "if (ws.N > maxFrameSize)\n            {\n                return;", "if (ws.N > maxFrameSize)\n            {\n                close(1009, \"Message too large\");\n                return;")
    patch(ix, "IXWebSocketTransport.cpp", "            unmaskReceiveBuffer(ws);", """            // DK64 AP: bound fragmented messages before allocating or merging.
            uint64_t aggregate = ws.N;
            for (const auto& chunk : _chunks) aggregate += chunk.size();
            if (aggregate > (1ULL << 20) || _chunks.size() >= 256)
            {
                close(1009, "Message too large");
                return;
            }
            unmaskReceiveBuffer(ws);""")
    patch(ix, "IXWebSocketPerMessageDeflateCodec.cpp", "            out.append(reinterpret_cast<char*>(&_compressBuffer.front()),", """            // DK64 AP: compressed messages must also respect the decoded limit.
            if (out.size() + _compressBuffer.size() - _inflateState.avail_out > (1ULL << 20)) return false;
            out.append(reinterpret_cast<char*>(&_compressBuffer.front()),""")
    patch(ix, "IXSocket.cpp", "            if (!readByte(&c, isCancellationRequested))", "            if (line.size() >= 8192 || !readByte(&c, isCancellationRequested)) // DK64 AP: bounded status line")
    patch(ix, "IXWebSocketHttpHeaders.cpp", "        while (true)\n        {\n            int colon = 0;", """        size_t totalBytes = 0, headerCount = 0;
        while (true)
        {
            if (++headerCount > 128) return std::make_pair(false, headers);
            int colon = 0;""")
    patch(ix, "IXWebSocketHttpHeaders.cpp", "            if (line[0] == '\\r' && line[1] == '\\n')", """            totalBytes += i;
            if (totalBytes > 65536 || i >= 1023) return std::make_pair(false, headers);
            if (line[0] == '\\r' && line[1] == '\\n')""")
    print("Pinned IXWebSocket and mbedTLS ready; frame, fragment, inflate and HTTP bounds applied.")

if __name__ == "__main__":
    main()
