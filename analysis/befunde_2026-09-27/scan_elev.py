#!/usr/bin/env python3
"""scan_elev.py — Fahrstuhl-Zensus ueber ALLE RE1.5-RDTs.

Sucht UNABHAENGIG von der 32-Byte-Signatur nach der Fahrt-GESTALT:
    22 bb ii 01 | 09 0a s1 00 | 22 bb ii 00 | 09 0a s2 00
(Set bank/bit=1, Sleep, Set bank/bit=0, Sleep) — also den Puls-Paaren, mit
denen ROOM1080 seine Etagenfahrt baut. Gibt Bank/Bit und beide Schlafzeiten aus,
damit "andere Fahrskripte" auffallen, die die feste Signatur verfehlen.
Zaehlt ausserdem Se_on (0x36) je Raum.
"""
import os, sys, glob, collections

ROOT = sys.argv[1] if len(sys.argv) > 1 else "re15_port/shared_assets/PSX"
SIG = bytes([0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
             0x22,0x01,0x1c,0x00, 0x09,0x0a,0x5a,0x00,
             0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
             0x22,0x01,0x1c,0x00, 0x09,0x0a,0x14,0x00])

def pulses(d):
    """alle Puls-Paare (off, bank, bit, sleep1, sleep2)"""
    out = []
    for i in range(len(d) - 16):
        if (d[i] == 0x22 and d[i+3] == 0x01 and
            d[i+4] == 0x09 and d[i+5] == 0x0a and d[i+7] == 0x00 and
            d[i+8] == 0x22 and d[i+9] == d[i+1] and d[i+10] == d[i+2] and d[i+11] == 0x00 and
            d[i+12] == 0x09 and d[i+13] == 0x0a and d[i+15] == 0x00):
            out.append((i, d[i+1], d[i+2], d[i+6], d[i+14]))
    return out

rows = []
for p in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    name = os.path.basename(p)
    sig = [i for i in range(len(d)-32) if d[i:i+32] == SIG]
    pu  = pulses(d)
    # "Fahrt-artig": ein Puls-Paar, dessen ZWEITE Schlafzeit lang ist (>= 0x30)
    longp = [x for x in pu if x[4] >= 0x30]
    if sig or longp:
        rows.append((name, len(sig), sig[:4], pu, longp))

print("RAUM      SIG  SIG-Offsets            PULSPAARE (off,bank,bit,sl1,sl2)")
for name, ns, so, pu, longp in rows:
    print(f"{name:12s} {ns:2d}  {[hex(x) for x in so]}")
    agg = collections.Counter((b, i, s1, s2) for (_o, b, i, s1, s2) in pu)
    for (b, i, s1, s2), n in sorted(agg.items()):
        mark = "  <== LANG" if s2 >= 0x30 else ""
        print(f"             bank{b} bit{i:#04x}  sleep {s1:3d}/{s2:3d}  x{n}{mark}")
