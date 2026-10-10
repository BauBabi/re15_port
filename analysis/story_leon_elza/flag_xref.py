#!/usr/bin/env python3
"""flag_xref.py - Schreiber/Leser je Flag (Bank,Bit) aus census.json, getrennt nach Variante.
Aufruf: python flag_xref.py [bank:bit ...]  (hex-Bit), ohne Argument -> flag_xref.txt"""
import json, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
c = json.load(open(os.path.join(HERE, "census.json"), encoding="utf-8"))
W, R = {}, {}
for rid, ev in c.items():
    for e in ev:
        if e["k"] in ("set", "ck"):
            key = (e["bank"], e["bit"])
            (W if e["k"] == "set" else R).setdefault(key, []).append((rid, e["reg"], e["pc"], e["val"], e["cond"]))
def show(key, out):
    out.append("FLAG (%d,0x%02X)" % key)
    for tag, D in (("  SET", W), ("  CK ", R)):
        for (rid, reg, pc, val, cond) in sorted(D.get(key, [])):
            out.append("%s ROOM%s %s @0x%05X =%d%s" % (tag, rid, reg, pc, val, ("  [wenn " + " & ".join(cond) + "]") if cond else ""))
out = []
if len(sys.argv) > 1:
    for a in sys.argv[1:]:
        b, bit = a.split(":"); show((int(b), int(bit, 16)), out)
    print("\n".join(out))
else:
    for key in sorted(set(W) | set(R)):
        show(key, out)
    open(os.path.join(HERE, "flag_xref.txt"), "w", encoding="utf-8").write("\n".join(out) + "\n")
    print(len(set(W) | set(R)), "Flags")
