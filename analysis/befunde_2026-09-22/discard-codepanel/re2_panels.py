"""Sucht in den RE2-Retail-RDTs nach einem mehrstufigen Code-Panel wie in RE1.5:
die Textreihe "Enter the first number." / "Wrong code" / "You've opened the lock"
und die Pad-Abfrage Sce_key_ck (0x51/0x52). Damit laesst sich beantworten, ob RE2 ein
Vorbild fuer "Karte einstecken UND Code eingeben" hat."""
import os, sys, glob, collections

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.normpath(os.path.join(HERE, "..", "..", "..", "re15_port", "tools"))
sys.path.insert(0, TOOLS)
from scd_walk_lib import (u16, u32, op_size, fwd_target, rdt_section_end,
                          section_regions, walk_ops, regionen, messages, NAMES)


ROOT = os.path.normpath(os.path.join(HERE, "..", "..", "..", "info", "re2leon", "PL0", "RDT"))
MUSTER = ["enter the", "wrong code", "opened the lock", "card reader", "four digit",
          "digits", "keypad", "access code", "code is"]

treffer = collections.defaultdict(list)
keyck = collections.Counter()
n = 0
for p in sorted(glob.glob(os.path.join(ROOT, "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x48:
        continue
    n += 1
    room = os.path.basename(p).split(".")[0]
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec in (ms, ss):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            for (pc, op, sz) in walk_ops(d, sec + o, sec + e):
                if op in (0x51, 0x52):
                    keyck[room] += 1
    try:
        msgs = messages(d)
    except Exception:
        msgs = {}
    for mid, (t, c) in msgs.items():
        low = t.lower()
        for m in MUSTER:
            if m in low:
                treffer[room].append((mid, m, t[:110]))
                break

print("RE2-Retail (Leon A), %d RDTs gelesen" % n)
print("\n-- Raeume mit Sce_key_ck (eigene Pad-Abfrage im Skript):")
if keyck:
    for r, c in sorted(keyck.items()):
        print("   %s  %dx" % (r, c))
else:
    print("   KEINER")
print("\n-- Nachrichten, die auf ein Code-/Ziffern-Panel deuten:")
if treffer:
    for r in sorted(treffer):
        for (mid, m, t) in treffer[r]:
            print("   %s msg %-3d [%s] %r" % (r, mid, m, t))
else:
    print("   KEINE")
