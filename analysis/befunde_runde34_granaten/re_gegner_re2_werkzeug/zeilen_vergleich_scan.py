#!/usr/bin/env python3
"""zeilen_vergleich_scan.py - Runde 34 (Granaten), re_gegner_re2_familie.

Sucht in einem RE2-Overlay (roh @0x80100000) alle Stellen, an denen die Treffer-Zeile +0x5
(`lbu rX,5(rY)`) gelesen und innerhalb der naechsten N Instruktionen gegen eine Konstante
verglichen wird (addiu/ori rZ,zero,K + beq/bne, oder slti/sltiu rZ,rX,K, oder xori rX,K).
Zweck: finden, ob ein Gegner-Overlay die GL-Zeilen 9/10/11 (Explosiv/Brand/Saeure) oder eine
andere Zeile gesondert behandelt. Nutzt den Disassembler aus re2_disasm.py (dieselbe dis_one).

Aufruf:
  python zeilen_vergleich_scan.py <overlay.BIN> [fenster=8]
  (Overlay-Pfad absolut oder relativ; die Datei wird roh @0x80100000 gelesen.)
"""
import os, sys, struct, re, importlib.util

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
spec = importlib.util.spec_from_file_location(
    "re2dis", os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts", "re2_disasm.py"))
re2dis = importlib.util.module_from_spec(spec)
spec.loader.exec_module(re2dis)

path = sys.argv[1]
win = int(sys.argv[2]) if len(sys.argv) > 2 else 8
data = open(path, "rb").read()
BASE = 0x80100000
n = len(data) // 4
ins = []
for i in range(n):
    w = struct.unpack_from("<I", data, i * 4)[0]
    a = BASE + i * 4
    try:
        txt, _ = re2dis.dis_one(w, a)
    except Exception:
        txt = ".word 0x%08x" % w
    ins.append((a, txt))

lbu5 = re.compile(r"^lbu (\w+),5\((\w+)\)$")
hits = 0
for k, (a, t) in enumerate(ins):
    m = lbu5.match(t.strip())
    if not m:
        continue
    reg = m.group(1)
    ctx = ins[k + 1:k + 1 + win]
    cmp = []
    for (b, u) in ctx:
        u = u.strip()
        if re.match(r"^(slti|sltiu) \w+,%s,(0x[0-9a-f]+|-?\d+)$" % reg, u) or \
           re.match(r"^xori %s,%s,(0x[0-9a-f]+|\d+)$" % (reg, reg), u) or \
           re.match(r"^(beq|bne) (%s,\w+|\w+,%s),0x" % (reg, reg), u) or \
           re.match(r"^(addiu|ori) \w+,zero,(0x[0-9a-f]+|-?\d+)$", u):
            cmp.append("%08x: %s" % (b, u))
    if any(re.match(r"^\w+: (beq|bne|slti|sltiu|xori)", c) for c in cmp):
        hits += 1
        print("%08x: %s" % (a, t.strip()))
        for c in cmp:
            print("      " + c)
print("# %d Kandidaten in %s (%d Instruktionen)" % (hits, os.path.basename(path), n))
