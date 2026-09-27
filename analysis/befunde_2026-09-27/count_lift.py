#!/usr/bin/env python3
"""Zaehlt je Raum die Fahrt-Pulse (Set bank1 bit0x1C/0x1D) und Se_on-Opcodes."""
import glob, sys

PAT = {"1C=1": bytes([0x22,1,0x1c,1]), "1C=0": bytes([0x22,1,0x1c,0]),
       "1D=1": bytes([0x22,1,0x1d,1]), "1D=0": bytes([0x22,1,0x1d,0])}

for r in sys.argv[1:]:
    p = glob.glob(f"re15_port/shared_assets/PSX/STAGE*/{r}.RDT")
    if not p:
        print(f"{r}: FEHLT"); continue
    d = open(p[0], "rb").read()
    parts = []
    for k, v in PAT.items():
        c = sum(1 for i in range(len(d)-3) if d[i:i+4] == v)
        parts.append(f"{k}x{c}")
    print(f"{r}: " + "  ".join(parts))
