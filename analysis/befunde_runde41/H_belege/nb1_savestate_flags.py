#!/usr/bin/env python3
# Runde 41 Spur H, Nachbesserung 1: Entity-Wort +0x0 (Gate-Bits von FUN_8002aec4) aus einem DuckStation-Savestate des
# Auslieferungsstands. Liest den Pool 0x800acc2c (Stride 500), Raum 0x800b0fe0/fe2, EXE-Pruefung @0x80026e4c.
# Aufruf: python nb1_savestate_flags.py <savestate.sav>
import os, sys
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
from re15_ss import Ram
r = Ram(sys.argv[1])
def s32(a):
    v = r.u32(a); return v - (1 << 32) if v & 0x80000000 else v
print("@0x80026e4c = %08x (Auslieferung: 03e00008 = jr ra)" % r.u32(0x80026e4c))
print("Stage/Raum 0x800b0fe0/fe2 = %d/%d, g_active_count 0x800aca4e = %d" % (r.u16(0x800b0fe0), r.u16(0x800b0fe2), r.u8(0x800aca4e)))
for k in range(16):
    b = 0x800acc2c + 500 * k
    fl = r.u32(b)
    if fl == 0: continue
    box = r.u32(b + 0x78)
    gates = [n for n, m in (("0x2", 0x2), ("0x4", 0x4), ("0x1000", 0x1000)) if fl & m]
    print("E%d typ=0x%02x Wort0=%08x Gate-Bits=%s Zustand=%d/%d +0x9=%02x Lage=(%d,%d,%d) Kasten r=%d" % (
        k, r.u8(b + 8), fl, ",".join(gates) or "keine", r.u8(b + 4), r.u8(b + 5), r.u8(b + 9),
        s32(b + 0x34), s32(b + 0x38), s32(b + 0x3c), r.u16(box + 6) if box else 0))
