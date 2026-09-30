#!/usr/bin/env python3
"""Gegenpruefung Spur G2 (Runde 34 Nacht) - reproduzierbarer Byte-Zensus, nur LESEN.

Prueft an info/Re1.5/PSX.EXE (+ allen info/Re1.5/PSX/BIN/*.BIN):
  1. jal-Aufrufstellen der Masken-Funktionen (Aufbau FUN_800392d4, Zeichnen FUN_80039590,
     Setzen FUN_800396a8, Cut-Apply FUN_80021bbc, SCD-Laeufer FUN_8003f038).
  2. ALLE Byte-Stores auf das Dirty-Flag DAT_800b5457 samt geschriebenem Wert
     (Wert aus dem letzten `ori rt,zero,imm` / `addu rt,zero,zero` davor).
  3. Den Kopf von FUN_80021bbc: bei Dirty == 2 Sprung hinter den Aufbau-Block.

  C:/Python310/python.exe re15_port/tools/r34n_g/g2_gegen_zensus.py
"""
import glob
import os
import struct

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
EXE = os.path.join(ROOT, "info", "Re1.5", "PSX.EXE")
ZIELE = {0x800392d4: "Aufbau FUN_800392d4", 0x80039590: "Zeichnen FUN_80039590",
         0x800396a8: "Setzen FUN_800396a8", 0x80021bbc: "Cut-Apply FUN_80021bbc",
         0x8003f038: "SCD-Laeufer FUN_8003f038"}


def lade(pfad, basis, kopf):
    d = open(pfad, "rb").read()
    return d, basis, kopf


def wort(bild, adr):
    d, basis, kopf = bild
    return struct.unpack_from("<I", d, kopf + adr - basis)[0]


def main():
    d = open(EXE, "rb").read()
    t_addr = struct.unpack_from("<I", d, 0x18)[0]
    bilder = [("PSX.EXE", (d, t_addr, 0x800))]
    for p in sorted(glob.glob(os.path.join(ROOT, "info", "Re1.5", "PSX", "BIN", "*.BIN"))):
        bilder.append((os.path.basename(p), lade(p, 0x80100000, 0)))

    print("== 1. jal-Aufrufstellen")
    for name, (dd, basis, kopf) in bilder:
        for off in range(kopf, len(dd) - 3, 4):
            w = struct.unpack_from("<I", dd, off)[0]
            if (w >> 26) == 3:
                ziel = 0x80000000 | ((w & 0x3FFFFFF) << 2)
                if ziel in ZIELE:
                    print("  %-10s %08x jal %s" % (name, basis + off - kopf, ZIELE[ziel]))

    print("== 2. Stores auf DAT_800b5457 (sb rt,0x5457(at)) mit Wert")
    bild = bilder[0][1]
    for off in range(0x800, len(d) - 3, 4):
        w = struct.unpack_from("<I", d, off)[0]
        if (w >> 26) == 0x28 and (w & 0xFFFF) == 0x5457:
            adr = t_addr + off - 0x800
            rt = (w >> 16) & 31
            wert = "?"
            if rt == 0:
                wert = "0 (zero)"
            else:
                for k in range(1, 12):
                    v = wort(bild, adr - 4 * k)
                    if (v >> 26) == 0x0D and ((v >> 16) & 31) == rt and ((v >> 21) & 31) == 0:
                        wert = "%d (ori @%08x)" % (v & 0xFFFF, adr - 4 * k)
                        break
            print("  %08x  sb r%d -> Dirty := %s" % (adr, rt, wert))

    print("== 3. Kopf FUN_80021bbc")
    for adr in (0x80021bc4, 0x80021bc8, 0x80021bd4):
        print("  %08x  %08x" % (adr, wort(bild, adr)))
    w = wort(bild, 0x80021bd4)
    if (w >> 26) == 4:
        ziel = 0x80021bd4 + 4 + (((w & 0xFFFF) ^ 0x8000) - 0x8000) * 4
        print("  beq v1(Dirty),v0(=2) -> %08x  (Aufbau-jal liegt bei 80021c28 DAZWISCHEN: %s)"
              % (ziel, 0x80021bd8 < 0x80021c28 < ziel))


if __name__ == "__main__":
    main()
