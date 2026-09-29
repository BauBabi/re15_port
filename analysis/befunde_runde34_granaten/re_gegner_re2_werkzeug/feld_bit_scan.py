#!/usr/bin/env python3
"""feld_bit_scan.py - Runde 34, re_gegner_re2_familie.
Listet in einem RE2-Overlay (roh @0x80100000) jede Stelle, an der ein Entity-Feld (Offset)
per lhu/lbu/lh/lb/lw geladen und in den naechsten 3 Instruktionen mit `andi rX,rX,MASKE`
geprueft bzw. mit `ori` gesetzt wird. Aufruf:
  python feld_bit_scan.py <overlay.BIN> <offset_dez> <maske_hex>[,<maske_hex>...]
Beispiel: feld_bit_scan.py EMZ0.BIN 538 0x800,0x1000,0x2000
"""
import sys, struct, re, importlib.util, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
spec = importlib.util.spec_from_file_location("d", os.path.join(REPO, ".claude/skills/re15-psx-disasm/scripts/re2_disasm.py"))
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
path, off = sys.argv[1], int(sys.argv[2])
masks = [int(x, 16) for x in sys.argv[3].split(",")]
data = open(path, "rb").read()
ins = []
for i in range(len(data) // 4):
    w = struct.unpack_from("<I", data, i * 4)[0]; a = 0x80100000 + i * 4
    try: t, _ = d.dis_one(w, a)
    except Exception: t = "?"
    ins.append((a, t.strip()))
ld = re.compile(r"^(lhu|lbu|lh|lb|lw) (\w+),%d\((\w+)\)$" % off)
for k, (a, t) in enumerate(ins):
    m = ld.match(t)
    if not m: continue
    r = m.group(2)
    for (b, u) in ins[k + 1:k + 5]:
        mm = re.match(r"^(andi|ori) (\w+),%s,(0x[0-9a-f]+)$" % r, u)
        if mm and int(mm.group(3), 16) in masks:
            print("%08x: %-28s %08x: %s" % (a, t, b, u))
