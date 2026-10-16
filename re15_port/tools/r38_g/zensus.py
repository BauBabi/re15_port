# -*- coding: utf-8 -*-
"""zensus.py - Runde 38 Spur G: Ressourcen-Zensus fuer Roys Diary (VERTRAG 1.1/1.3).

  (a) Item-Id 0x4D (= Dokument 5, RE15_FILES_FIRST_ITEM_ID 0x48 + 5): legt irgendein ausgeliefertes Skript eine
      Item-Zone mit dieser Id an? Brute-Force ueber JEDE Byte-Position aller 240 RDT-Dateien wie
      tools/r34n_e/id_zensus.py: Item_aot_set `50 slot 09 ...` Id u16 @+14, Aot_reset `46 slot 09 ...` Id u16 @+4.
  (b) Bank 9 Bit 68 (0x44): Set/Ck im Skript (`22 09 44 vv` / `21 09 44 vv`) und als Aufnahme-Bit einer Item-Zone
      (Item_aot_set u8 @+18, z.B. ROOM1011.RDT @0x00954 `50 02 09 31 .. 39 00 01 00 8a 00` -> Bit 0x8A).
  (c) Port-Dokumente: Saetze FILEnn unter shared_assets/RE2/FILES (frei = kein FILE30_*).
Gegenprobe: ROOM1011 liefert die drei Item-Zonen @0x00954/@0x0096A/@0x00980 (Bits 0x8A/0x8D/0x8E).

Aufruf: C:/Python310/python.exe re15_port/tools/r38_g/zensus.py
"""
import glob
import os
import struct

HIER = os.path.dirname(os.path.abspath(__file__))
CD = os.path.join(HIER, "..", "..", "shared_assets", "PSX")
FILES = os.path.join(HIER, "..", "..", "shared_assets", "RE2", "FILES")
ID, BIT = 0x4D, 68


def main():
    dateien = sorted(glob.glob(os.path.join(CD, "STAGE*", "ROOM*.RDT")))
    n_set = n_reset = 0
    id_treffer, bit_skript, bit_zone = [], [], []
    for p in dateien:
        d = open(p, "rb").read()
        name = os.path.basename(p)
        for pc in range(len(d) - 22):
            b0 = d[pc]
            if b0 == 0x50 and d[pc + 2] == 0x09 and d[pc + 1] < 48:
                n_set += 1
                if struct.unpack_from("<H", d, pc + 14)[0] == ID:
                    id_treffer.append("%s@0x%05X Item_aot_set" % (name, pc))
                if d[pc + 18] == BIT:
                    bit_zone.append("%s@0x%05X" % (name, pc))
            if b0 == 0x46 and d[pc + 2] == 0x09 and d[pc + 1] < 48:
                n_reset += 1
                if struct.unpack_from("<H", d, pc + 4)[0] == ID:
                    id_treffer.append("%s@0x%05X Aot_reset" % (name, pc))
            if b0 in (0x21, 0x22) and d[pc + 1] == 9 and d[pc + 2] == BIT and d[pc + 3] in (0, 1):
                bit_skript.append("%s@0x%05X %02x 09 %02x %02x" % (name, pc, b0, BIT, d[pc + 3]))
    print("RDT-Dateien: %d; Item_aot_set-Muster %d, Aot_reset-sce9-Muster %d" % (len(dateien), n_set, n_reset))
    print("(a) Item-Id 0x%02X in Item-Zonen: %d %s" % (ID, len(id_treffer), id_treffer))
    print("(b) Bank 9 Bit %d: Skript Set/Ck %d %s; als Aufnahme-Bit einer Item-Zone %d %s"
          % (BIT, len(bit_skript), bit_skript[:8], len(bit_zone), bit_zone[:8]))
    saetze = sorted(set(os.path.basename(f)[4:6] for f in glob.glob(os.path.join(FILES, "FILE*_*.TIM"))))
    print("(c) Bildsaetze unter shared_assets/RE2/FILES: %s" % " ".join(saetze))
    d = open(os.path.join(CD, "STAGE1", "ROOM1011.RDT"), "rb").read()
    k = ["0x%05X Bit 0x%02X" % (pc, d[pc + 18]) for pc in range(len(d) - 22)
         if d[pc] == 0x50 and d[pc + 2] == 0x09 and d[pc + 1] < 48]
    print("Gegenprobe ROOM1011: %s (erwartet 0x00954/0x0096A/0x00980, Bits 0x8A/0x8D/0x8E)" % k)


if __name__ == "__main__":
    main()
