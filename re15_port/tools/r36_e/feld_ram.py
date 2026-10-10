# Liest das Gegner-Feld (0x800acc2c, Stride 0x1F4) und den Aktiv-Zaehler 0x800aca4e aus Savestates des
# Auslieferungsstands (nicht die PATCHED-EXE-Staende), plus Pruefung des Stubs @0x80026e4c.
import sys, os
sys.path.insert(0, sys.argv[1])
import re15_ss as S
for sav in sys.argv[2:]:
    r = S.Ram(sav)
    stub = r.u32(0x80026e4c)

    n = r.u8(0x800aca4e)
    akt = []
    for s in range(22):
        e = 0x800acc2c + s * 0x1F4
        w0 = r.u32(e); typ = r.u8(e + 8)
        if w0 & 1:
            akt.append((s, hex(typ), r.s32(e + 0x34), r.s32(e + 0x3c)))
    print(os.path.basename(sav), "stub@0x80026e4c=%08x" % stub, "zaehler 0x800aca4e =", n, "aktiv:", akt)
    print("   0x800af33c..: ", r.bytes(0x800af33c, 16).hex())
