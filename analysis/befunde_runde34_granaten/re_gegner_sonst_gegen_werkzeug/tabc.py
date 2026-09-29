#!/usr/bin/env python3
"""Gegenpruefung R34: kompakte Zeigertabelle ueber load() von re15_disasm.py / re2_disasm.py.
tabc.py [--re2] <addr> <n> [--bin X.BIN] [--rowbytes N --cols C]  -> gruppiert gleiche Ziele."""
import sys, os, struct, argparse, importlib.util
ap = argparse.ArgumentParser(); ap.add_argument("addr"); ap.add_argument("n", type=int)
ap.add_argument("--bin"); ap.add_argument("--re2", action="store_true")
ap.add_argument("--rowbytes", type=int, default=4); ap.add_argument("--cols", type=int, default=1)
a = ap.parse_args()
here = r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts"
mod = "re2_disasm.py" if a.re2 else "re15_disasm.py"
spec = importlib.util.spec_from_file_location("d", os.path.join(here, mod)); d = importlib.util.module_from_spec(spec)
sys.argv = [mod]; spec.loader.exec_module(d)
base = int(a.addr, 16)
data, fo, path = d.load(base, a.bin)
print("; %s" % path)
groups = {}
for i in range(a.n):
    row = []
    for c in range(a.cols):
        adr = base + i*a.rowbytes + c*4
        row.append(struct.unpack_from("<I", data, fo(adr))[0])
    key = tuple(row)
    groups.setdefault(key, []).append(i)
for key, idx in groups.items():
    print("  [%s] -> %s" % (",".join(str(x) for x in idx), " | ".join("0x%08x" % v for v in key)))
