#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - je DuckStation-Stand in ROOM1150/1151: Blink-Zustand im RAM UND im Bild.

Nur LESEN. Je Stand:
  * EXE-Sauberkeit (Wort @0x80026e4c == 0x03e00008), Raum DAT_800b0fe2, Cut DAT_800b0fe4,
    VBlank-Zaehler 0x800787dc, VSync-Modus DAT_800b5456, Doppelpuffer DAT_800aca34.
  * Masken-Records (DAT_800b2584, RDT[0] Stueck, 4 Byte): Byte0 der Gruppen 6..11.
    FUN_800392d4 schreibt Byte1 = Gruppenindex+1, d.h. Gruppe g (1-basiert wie im Zensus
    col_chg_zensus.py) <-> Byte1 == g; Opcode 0x45 op1 trifft Byte1 == op1+1.
  * SCD-Faden von sub05: Fadenpool 0x800b2b4c, Stride 0x170 (FUN_8003ef6c), pc = +0x1C
    (lw 28(a0) in allen Handlern), Tiefe = +2 (lb 2(a0)), Sleep-Zaehlerindex = +8+Tiefe
    (lbu 8(a1) @0x8003f3fc), Zaehler = +160+Tiefe*8+Index*2 (@0x8003f400-0x8003f41c).
    sub05 liegt in ROOM1150 bei RDT+0x10B6..0x10E7 (ROOM1151 +0x1094..0x10C5).
  * Bildspeicher: Schrift-Rechteck x139..216 y25..40 in BEIDEN Puffern (VRAM y=0 und y=240,
    RECT-y = -(buf!=0)&0xF0 @0x8002156c-80), gegen die AN- und AUS-Referenz aus
    schrift1150_masken.py (15-Bit-quantisiert). Zaehlt Pixel, die EXAKT einer Atlasfarbe der
    Masken 6..11 an ihrer Zielstelle entsprechen (die AN-Pixel sind CLUT-Farben, bitgenau).

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_schrift1150.py <bss-ppm-praefix> <stand.sav> [...]
"""
import os
import struct
import sys
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
sys.path.insert(0, HERE)
import re15_ss  # noqa: E402
import schrift1150_masken as SM  # noqa: E402

BOX = (139, 25, 216, 40)
POOL, STRIDE, NFADEN = 0x800b2b4c, 0x170, 16
# sub05 je Raumvariante (Raumindex 0x15 ist fuer 1150 und 1151 gleich): (von, bis, Sleeping#1, Sleeping#2)
SUB05 = {"ROOM1150": (0x10B6, 0x10E8, 0x10C9, 0x10DF), "ROOM1151": (0x1094, 0x10C6, 0x10A7, 0x10BD)}


def ref_bilder(room, bss_praefix):
    rdt, _ = SM.original.load_rdt(SM.CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    _, recs = SM.records(rdt, cam, 2)
    idx, clut = SM.lade_atlas("ROOM1150", 2)
    from PIL import Image
    bg = np.asarray(Image.open("%s_cut02.ppm" % bss_praefix).convert("RGB"), np.uint8)
    an = SM.blit(bg, recs, idx, clut).astype(np.int16)
    aus = SM.blit(bg, [m for m in recs if not (6 <= m["gruppe"] <= 11)], idx, clut).astype(np.int16)
    # Maske der Pixel, die im AN-Bild aus dem Atlas kommen (Gruppen 6..11)
    nur = np.zeros((240, 320), bool)
    for m in recs:
        if not (6 <= m["gruppe"] <= 11):
            continue
        for yy in range(m["h"]):
            for xx in range(m["w"]):
                if idx[(m["sy"] + yy) & 0xFF, (m["sx"] + xx) & 0xFF]:
                    nur[m["Y"] + yy, m["X"] + xx] = True
    return an, aus, nur


def fb_box(r, ybase):
    x0, y0, x1, y1 = BOX
    vb = r.vram_base
    rows = []
    for y in range(y0, y1 + 1):
        o = vb + ((ybase + y) * 1024 + x0) * 2
        rows.append(np.frombuffer(r.blob[o:o + (x1 - x0 + 1) * 2], dtype="<u2"))
    a = np.stack(rows).astype(np.int32)
    return np.stack([(a & 31) << 3, ((a >> 5) & 31) << 3, ((a >> 10) & 31) << 3], -1).astype(np.int16)


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    bss = sys.argv[1]
    an, aus, nur = ref_bilder("ROOM1150", bss)
    x0, y0, x1, y1 = BOX
    q = lambda im: ((im >> 3) << 3)  # noqa: E731
    an_b, aus_b, nur_b = q(an[y0:y1 + 1, x0:x1 + 1]), q(aus[y0:y1 + 1, x0:x1 + 1]), nur[y0:y1 + 1, x0:x1 + 1]
    for f in sys.argv[2:]:
        r = re15_ss.Ram(f)
        stub = r.u32(0x80026e4c)
        room = r.u16(0x800b0fe2)
        cut = r.u16(0x800b0fe4)
        rdt = r.u32(0x800ac778)
        nmask = r.u8(rdt)
        tab = r.u32(0x800b2584)
        recs = [r.bytes(tab + 4 * i, 4) for i in range(nmask)]
        g = {b[1]: b[0] for b in recs if 6 <= b[1] <= 11}
        print("%s\n  EXE %s  Raum %02x Cut %d  Vcount %d  VSync-Modus %d  Puffer(aca34) %d  RDT[0] %d"
              % (os.path.basename(f), "sauber" if stub == 0x03e00008 else "GEPATCHT %08x" % stub, room, cut,
                 r.u32(0x800787dc), r.u8(0x800b5456), r.u8(0x800aca34), nmask))
        print("  Records Gruppen 6..11 Byte0: %s" % (" ".join("g%d=%d" % (k, g[k]) for k in sorted(g)) or "-"))
        for name, (lo, hi, s1, s2) in SUB05.items():
            for i in range(NFADEN):
                base = POOL + i * STRIDE
                pc = r.u32(base + 0x1C)
                if rdt + lo <= pc < rdt + hi:
                    d = r.s8(base + 2)
                    cnt = r.s8(base + 8 + d)
                    ctr = r.u16(base + 160 + d * 8 + cnt * 2) if cnt >= 0 else None
                    wo = {s1: "Sleep#1 (nach :=0)", s2: "Sleep#2 (nach :=1)"}.get(pc - rdt, "?")
                    print("  sub05-Faden %d @%08x (falls %s): pc=RDT+0x%04X %s  Tiefe %d  Zaehlerindex %d  Rest %s"
                          % (i, base, name, pc - rdt, wo, d, cnt, ctr))
        if cut == 2:
            for yb in (0, 240):
                fb = fb_box(r, yb)
                d_an = int(np.abs(fb - an_b).max())
                d_aus = int(np.abs(fb - aus_b).max())
                exakt = int((np.all(fb == an_b, axis=-1) & nur_b).sum())
                print("  Bildspeicher y=%3d: max|d| zu AN %3d, zu AUS %3d; Maskenpixel bitgleich AN %d/%d"
                      % (yb, d_an, d_aus, exakt, int(nur_b.sum())))
    return 0


if __name__ == "__main__":
    sys.exit(main())
