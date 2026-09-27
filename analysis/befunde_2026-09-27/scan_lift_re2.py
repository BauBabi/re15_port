#!/usr/bin/env python3
"""Gegenstueck-Zensus in RE2-RETAIL: welche RE2-RDTs tragen die Fahrt-Signatur bzw.
bank1-bit0x1C/0x1D-Pulse — und haben sie ein Se_on daneben?"""
import glob, os

SIG  = bytes([0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
              0x22,0x01,0x1c,0x00, 0x09,0x0a,0x5a,0x00,
              0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
              0x22,0x01,0x1c,0x00, 0x09,0x0a,0x14,0x00])
P1C1 = bytes([0x22,1,0x1c,1]); P1D1 = bytes([0x22,1,0x1d,1])

def occ(d, p):
    return [i for i in range(len(d)-len(p)+1) if d[i:i+len(p)] == p]

rows = []
for f in sorted(glob.glob("info/re2leon/**/*.RDT", recursive=True)):
    d = open(f, "rb").read()
    s, a, b = occ(d, SIG), occ(d, P1C1), occ(d, P1D1)
    if not (s or a or b):
        continue
    # Se_on (0x36) in einem Fenster von +-0x60 um den ersten Puls
    near = 0
    for o in (s or a or b)[:1]:
        w = d[max(0, o-0x60):o+0x60]
        near = sum(1 for i in range(len(w)) if w[i] == 0x36)
    rows.append((os.path.basename(f), len(s), len(a), len(b), near,
                 [hex(x) for x in (s or a)[:3]]))

print(f"{'RDT':16s} {'SIG':>3s} {'1C=1':>4s} {'1D=1':>4s} {'Se_on nah':>9s}  Offsets")
for r in rows:
    print(f"{r[0]:16s} {r[1]:3d} {r[2]:4d} {r[3]:4d} {r[4]:9d}  {r[5]}")
print(f"\nRE2-RDTs mit Fahrt-/Puls-Spur: {len(rows)}")
