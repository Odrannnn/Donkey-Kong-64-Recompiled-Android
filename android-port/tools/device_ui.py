"""Small ADB helper for this port's manual smoke tests (no coordinates guessed)."""
from pathlib import Path
import argparse
import os
import re
import subprocess
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser()
parser.add_argument("--serial", help="Explicit ADB device serial; use when phone and tablet are both connected")
parser.add_argument("action", choices=["inspect", "tap", "screenshot"])
parser.add_argument("value", nargs="?")
args = parser.parse_args()
sdk = Path(os.environ.get("ANDROID_HOME", Path(os.environ.get("LOCALAPPDATA", "")) / "Android/Sdk"))
adb = str(sdk / "platform-tools/adb.exe")

def run(*command):
    target = ["-s", args.serial] if args.serial else []
    return subprocess.check_output([adb, *target, *command], timeout=30)

if args.action == "screenshot":
    if not args.value:
        parser.error("Provide the screenshot output path.")
    Path(args.value).write_bytes(run("exec-out", "screencap", "-p"))
else:
    run("shell", "uiautomator", "dump", "/data/local/tmp/dk64-window.xml")
    tree = ET.fromstring(run("shell", "cat", "/data/local/tmp/dk64-window.xml"))
    allowed = {"io.github.dk64port", "com.google.android.documentsui", "com.android.documentsui"}
    nodes = [node for node in tree.iter("node") if node.get("package") in allowed]
    if args.action == "inspect":
        for node in nodes:
            if node.get("text") or node.get("content-desc") or node.get("clickable") == "true":
                print({key: node.get(key) for key in ["text", "content-desc", "bounds", "enabled"]})
    else:
        matches = [node for node in nodes if args.value in (node.get("text"), node.get("content-desc"))]
        if len(matches) != 1:
            raise RuntimeError(f"Expected exactly one visible match for {args.value!r}, found {len(matches)}")
        node = matches[0]
        if node.get("enabled") != "true":
            raise RuntimeError("Control is disabled.")
        x1,y1,x2,y2 = map(int, re.findall(r"\d+", node.get("bounds")))
        run("shell", "input", "tap", str((x1+x2)//2), str((y1+y2)//2))
