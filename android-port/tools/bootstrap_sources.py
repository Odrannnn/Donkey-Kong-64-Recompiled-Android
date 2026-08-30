"""Restore pinned upstream/dependencies and apply the source-only Android patches without overwriting local work."""
from pathlib import Path
import json
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def git(repo, *args, check=True):
    return subprocess.run(["git", "-c", f"safe.directory={repo.as_posix()}", "-C", str(repo), *args], check=check)

def main():
    lock = json.loads((ROOT / "upstream.lock.json").read_text())
    for item in lock["repositories"]:
        repo = ROOT / item["path"]
        if not (repo / ".git").exists():
            if repo.exists() and any(repo.iterdir()): raise RuntimeError(f"Refusing to replace nonempty directory: {repo}")
            repo.parent.mkdir(parents=True, exist_ok=True)
            subprocess.run(["git", "clone", "--no-checkout", "--filter=blob:none", item["url"], str(repo)], check=True)
            git(repo, "checkout", "--detach", item["commit"])
        current = subprocess.check_output(["git", "-c", f"safe.directory={repo.as_posix()}", "-C", str(repo), "rev-parse", "HEAD"]).decode().strip()
        if current != item["commit"]: raise RuntimeError(f"{repo} is on {current}; expected {item['commit']}. Existing work was preserved.")
    for item in lock["repositories"]:
        if "patch" not in item: continue
        repo, patch = ROOT / item["path"], str(ROOT / item["patch"])
        if git(repo, "apply", "--reverse", "--check", patch, check=False).returncode == 0:
            print(f"Already patched: {item['path']}")
        else:
            git(repo, "apply", "--check", patch)
            git(repo, "apply", patch)
    print("Pinned Android sources are ready. Supply your own supported ROM for code generation.")

if __name__ == "__main__": main()
