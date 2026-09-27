#!/usr/bin/env python3
"""Vollzensus ueber ALLE 240 RE1.5-RDTs: welcher Raum pulst bank1 bit0x1C / bit0x1D?
Das ist der Fahrstuhl-Anker UNABHAENGIG von der 32-Byte-Signatur des Ports."""
import glob, os

P1C1 = bytes([0x22,1,0x1c,1]); P1C0 = bytes([0x22,1,0x1c,0])
P1D1 = bytes([0x22,1,0x1d,1]); P1D0 = bytes([0x22,1,0x1d,0])
SIG  = bytes([0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
              0x22,0x01,0x1c,0x00, 0x09,0x0a,0x5a,0x00,
              0x22,0x01,0x1c,0x01, 0x09,0x0a,0x08,0x00,
              0x22,0x01,0x1c,0x00, 0x09,0x0a,0x14,0x00])

def occ(d, p):
    return [i for i in range(len(d)-len(p)+1) if d[i:i+len(p)] == p]

rows = []
for f in sorted(glob.glob("re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT")):
    d = open(f, "rb").read()
    c1c, c1d = occ(d, P1C1), occ(d, P1D1)
    if not c1c and not c1d:
        continue
    rows.append((os.path.basename(f)[:-4], len(c1c), len(occ(d, P1C0)),
                 len(c1d), len(occ(d, P1D0)), len(occ(d, SIG)),
                 [hex(x) for x in c1c[:6]]))

print(f"{'RAUM':10s} {'1C=1':>4s} {'1C=0':>4s} {'1D=1':>4s} {'1D=0':>4s} {'SIG':>4s}  Offsets 1C=1")
for r in rows:
    print(f"{r[0]:10s} {r[1]:4d} {r[2]:4d} {r[3]:4d} {r[4]:4d} {r[5]:4d}  {r[6]}")
print(f"\nRaeume mit bit0x1C/0x1D-Pulsen: {len(rows)}")
