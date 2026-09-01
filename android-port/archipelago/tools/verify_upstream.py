"""Verify all generated catalogs against a full pinned checkout; execute no upstream code."""
import importlib.util
from pathlib import Path
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    spec = importlib.util.spec_from_file_location("dk64_catalog_compiler", ROOT / "tools/import-randomizer.py")
    compiler = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(compiler)  # Our local compiler, never upstream generator/client code.
    with tempfile.TemporaryDirectory(prefix="dk64-catalog-") as scratch:
        output = Path(scratch)
        compiler.generate(Path(sys.argv[1]), output)
        for name in ("catalog.json", "catalog.h", "port-coverage.json"):
            expected = (ROOT / "generated" / name).read_text(encoding="utf-8")
            if (output / name).read_text(encoding="utf-8") != expected:
                raise ValueError(f"Generated source is stale or altered: {name}")
    print("PASS: all generated location/item definitions and source coverage match pinned upstream inputs")


if __name__ == "__main__": main()
