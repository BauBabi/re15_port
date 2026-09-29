#!/usr/bin/env python3
"""ram_vs_disc.py - vergleicht das RAM eines Savestates mit dem Auslieferungsstand.

(1) EXE-Text: RAM[t_addr .. t_addr+t_size] gegen PSX.EXE[0x800:0x800+t_size] -> Abweichungs-Bereiche.
    Abweichungen in Code-Bereichen = gepatchte EXE; in Daten/BSS = Laufzeitwerte (erwartet).
(2) Overlay: RAM[0x80100000 .. +len(STAGEn.BIN)] gegen jedes STAGEn.BIN -> beste Stage + Bereiche.
(3) Stichproben: 0x80026e4c (Save-Patch-Stub), Code der Absturzkette, Tabellen 0x8011feac/0x8011f7b4.
Aufruf: ram_vs_disc.py SAV [SAV ...]
"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gp_ss import SS, EXE, EXE_TADDR, EXE_TSIZE, REPO

BIN = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN")


def ranges(a, b, base, gap=16):
    out = []
    n = min(len(a), len(b))
    i = 0
    while i < n:
        if a[i] != b[i]:
            j = i
            last = i
            while j < n and j - last <= gap:
                if a[j] != b[j]:
                    last = j
                j += 1
            out.append((base + i, base + last + 1))
            i = last + 1
        else:
            i += 1
    return out


def main():
    for p in sys.argv[1:]:
        s = SS(p)
        print("==", p)
        print("   titel:", s.title, "| medium:", os.path.basename(s.media))
        ram_exe = s.mem(EXE_TADDR, EXE_TSIZE)
        disk = EXE[0x800:0x800 + EXE_TSIZE]
        r = ranges(ram_exe, disk, EXE_TADDR)
        nb = sum(e - a for a, e in r)
        print("   EXE t_addr=%08x t_size=%x: %d Abweichungs-Bereiche, %d Byte" % (EXE_TADDR, EXE_TSIZE, len(r), nb))
        # Code-Bereich: bis zum Anfang der Daten. Wir melden Bereiche < 0x80066000 einzeln.
        code = [(a, e) for a, e in r if a < 0x80068000]
        print("   davon unter 0x80068000 (Code):", ", ".join("%08x-%08x" % x for x in code[:40]) or "keine")
        print("   @0x80026e4c:", s.mem(0x80026e4c, 8).hex(" "), "(Auslieferung: 08 00 e0 03 ...)")
        best = None
        for f in sorted(os.listdir(BIN)):
            if not f.startswith("STAGE"):
                continue
            d = open(os.path.join(BIN, f), "rb").read()
            m = s.mem(0x80100000, len(d))
            same = sum(1 for x, y in zip(m[:0x4000], d[:0x4000]) if x == y)
            if best is None or same > best[1]:
                best = (f, same, d, m)
        f, same, d, m = best
        rr = ranges(m, d, 0x80100000)
        print("   Overlay = %s (erste 0x4000 B: %d gleich); %d Abweichungs-Bereiche, %d Byte" % (f, same, len(rr), sum(e - a for a, e in rr)))
        print("   Bereiche:", ", ".join("%08x-%08x" % x for x in rr[:60]))
        for a, n in ((0x80106ba4, 0x74), (0x80100424, 0x170), (0x8011feac, 0x2c0), (0x8011f7b4, 0x30), (0x8011fb90, 0x2c0)):
            ok = m[a - 0x80100000:a - 0x80100000 + n] == d[a - 0x80100000:a - 0x80100000 + n]
            print("   %s @%08x+%x: RAM == %s ? %s" % ("Overlay", a, n, f, ok))
        for a, n in ((0x80012d60, 0x30c), (0x8001854c, 0x198), (0x8001a50c, 0xd4), (0x8001cdec, 0x50), (0x80019e20, 0x120),
                     (0x80033640, 0x17c), (0x80033b38, 0x60), (0x80074100, 0x50), (0x80071d40, 0xc0), (0x8006f418, 0x24)):
            fo = 0x800 + a - EXE_TADDR
            ok = s.mem(a, n) == EXE[fo:fo + n]
            print("   EXE @%08x+%x: RAM == PSX.EXE ? %s" % (a, n, ok))
        print("   RAM 0x00000000..0x10:", " ".join("%08x" % s.u32(0x80000000 + 4 * k) for k in range(4)))
        print("   Wort [9][1] 0x8011ffd0 = %08x ; Zeile 9 = %s" % (s.u32(0x8011ffd0), [hex(s.u32(0x8011ffcc + 4 * k)) for k in range(8)]))
        print("   Typ-Tabelle 0x80072bac[0x10] = %08x" % s.u32(0x80072bac + 0x40))


if __name__ == "__main__":
    main()
