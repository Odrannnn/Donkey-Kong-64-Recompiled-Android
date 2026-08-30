"""Host-side equivalent of RT64's file_to_c for cross-compilation."""
from pathlib import Path
import re
import sys

source, symbol, c_path, h_path = sys.argv[1:]
if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", symbol):
    raise ValueError("Invalid C symbol")
data = Path(source).read_bytes()
if not data:
    raise ValueError(f"Empty shader/input: {source}")
for path in (c_path, h_path):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
declarations = f"extern const char {symbol}[{len(data)}];\nextern const size_t {symbol}_size;\n"
Path(h_path).write_text('#include <stddef.h>\n#ifdef __cplusplus\nextern "C" {\n#endif\n'
                        + declarations + '#ifdef __cplusplus\n}\n#endif\n', encoding="utf-8")
with Path(c_path).open("w", encoding="utf-8") as output:
    output.write('#include <stddef.h>\n' + declarations + f"const char {symbol}[{len(data)}] = {{\n")
    for index in range(0, len(data), 32):
        output.write(','.join(str(value if value < 128 else value - 256) for value in data[index:index+32]) + ',\n')
    output.write(f"}};\nconst size_t {symbol}_size = sizeof({symbol});\n")
