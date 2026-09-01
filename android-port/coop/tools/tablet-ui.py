"""Local smoke-test helper; controls only the explicitly named tablet."""
from pathlib import Path
import os
import subprocess
import sys
import xml.etree.ElementTree as ET
import argparse

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--serial", required=True, help="Explicit adb tablet serial; never auto-select a device")
parser.add_argument("input", nargs=argparse.REMAINDER, help="Optional Android input command")
args = parser.parse_args()
adb = [str(Path(os.environ["LOCALAPPDATA"]) / "Android/Sdk/platform-tools/adb.exe"), "-s", args.serial]
out = Path(__file__).resolve().parents[1] / "build/verification"
out.mkdir(parents=True, exist_ok=True)
if args.input:
    subprocess.run(adb + ["shell", "input"] + args.input, check=True)
subprocess.run(adb + ["shell", "uiautomator", "dump", "/data/local/tmp/dk64-coop-ui.xml"], check=True, stdout=subprocess.DEVNULL)
xml = subprocess.run(adb + ["exec-out", "cat", "/data/local/tmp/dk64-coop-ui.xml"], capture_output=True, check=True).stdout
(out / "ui.xml").write_bytes(xml)
for node in ET.fromstring(xml).iter("node"):
    if node.get("text") or node.get("content-desc"):
        print(repr((node.get("text"), node.get("content-desc"), node.get("bounds"))))
with (out / "tablet.png").open("wb") as image:
    subprocess.run(adb + ["exec-out", "screencap", "-p"], check=True, stdout=image)
