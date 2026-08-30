"""Save local source-only diffs and pin every initialized dependency. Never includes ROMs or generated game code."""
from pathlib import Path
import json
import subprocess

ROOT = Path(__file__).resolve().parents[1]
PATCHES = ROOT / "patches"
EXTRA = {"upstream/lib/rt64/src/contrib/nativefiledialog-extended": ["src/nfd_null.cpp"]}

def git(repo, *args):
    return subprocess.check_output(["git", "-c", f"safe.directory={repo.as_posix()}", "-C", str(repo), *args])

def snapshot(repo):
    relative = repo.relative_to(ROOT).as_posix()
    head = git(repo, "rev-parse", "HEAD").decode().strip()
    url = git(repo, "remote", "get-url", "origin").decode().strip()
    children = []
    for line in git(repo, "ls-files", "-s", "-z").split(b"\0"):
        if line.startswith(b"160000 "):
            children.append(line.split(b"\t", 1)[1].decode())
    patch = git(repo, "diff", "--binary", "--no-ext-diff", "HEAD", "--", ".", *[f":(exclude){path}" for path in children])
    for name in EXTRA.get(relative, []):
        contents = (repo / name).read_bytes().replace(b"\r\n", b"\n")
        lines = contents.decode().splitlines()
        patch += (f"diff --git a/{name} b/{name}\nnew file mode 100644\n--- /dev/null\n+++ b/{name}\n@@ -0,0 +1,{len(lines)} @@\n"
                  + "".join("+" + line + "\n" for line in lines)).encode()
    record = {"path": relative, "url": url, "commit": head}
    if patch:
        patch_path = PATCHES / (relative.replace("/", "__").replace(".local__deps__", "dependency__") + ".patch")
        patch_path.write_bytes(patch)
        record["patch"] = patch_path.relative_to(ROOT).as_posix()
    records = [record]
    for child in children:
        if (repo / child / ".git").exists(): records.extend(snapshot(repo / child))
    return records

def main():
    PATCHES.mkdir(exist_ok=True)
    repos = []
    for name in ["upstream", ".local/deps/SDL", ".local/deps/freetype", ".local/deps/adrenotools"]:
        repos.extend(snapshot(ROOT / name))
    (ROOT / "upstream.lock.json").write_text(json.dumps({"schema": 1, "repositories": repos}, indent=2) + "\n")
    for item in repos:
        if "patch" in item:
            git(ROOT / item["path"], "apply", "--reverse", "--check", str(ROOT / item["patch"]))
    print(f"Pinned {len(repos)} repositories; verified {sum('patch' in r for r in repos)} source patches against the working tree.")

if __name__ == "__main__": main()
