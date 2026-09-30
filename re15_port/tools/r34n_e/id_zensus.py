# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: nutzt irgendein ausgeliefertes Skript die Item-Ids 0x48..0x4C?

Sobald die Dokument-Tabelle (re15_files.c) Eintraege 1..4 fuehrt, macht aot_item_dokument JEDE
Item-Zone mit Id 0x49..0x4C zum Dokument (Leser statt Item-Modal). Legt ein Original-Skript eine
solche Zone an, aenderte sich dort das Verhalten. Brute-Force ueber JEDE Byte-Position aller
240 RDT-Dateien (unabhaengig vom Block-Walker, der 18 Bloecke nicht zu Ende liest):
  Item_aot_set  `50 slot 09 ...`, Id = u16 @+14  (ROOM1010.RDT @0x00996 `50 02 09 31 .. 22 00 ..`)
  Aot_reset     `46 slot 09 ...`, Id = p0 u16 @+4 (scd_vm.c op_aot_reset: slot, sce, flags, p0..)
Gegenprobe: dieselbe Suche findet in ROOM1010 genau die drei bekannten Items (Slot 2/3/4).

Aufruf: python re15_port/tools/r34n_e/id_zensus.py
"""
import glob
import os
import struct

HIER = os.path.dirname(os.path.abspath(__file__))
CD = os.path.join(HIER, "..", "..", "shared_assets", "PSX")


def main():
    dateien = sorted(glob.glob(os.path.join(CD, "STAGE*", "ROOM*.RDT")))
    item_set = []
    reset = []
    n_set = n_reset = 0
    for p in dateien:
        d = open(p, "rb").read()
        name = os.path.basename(p)
        for pc in range(len(d) - 22):
            if d[pc] == 0x50 and d[pc + 2] == 0x09 and d[pc + 1] < 48:
                n_set += 1
                if 0x48 <= struct.unpack_from("<H", d, pc + 14)[0] <= 0x4C:
                    item_set.append("%s@0x%05X" % (name, pc))
            if d[pc] == 0x46 and d[pc + 2] == 0x09 and d[pc + 1] < 48:
                n_reset += 1
                if 0x48 <= struct.unpack_from("<H", d, pc + 4)[0] <= 0x4C:
                    reset.append("%s@0x%05X" % (name, pc))
    print("RDT-Dateien: %d" % len(dateien))
    print("Item_aot_set-Muster (50 slot 09): %d, davon Id 0x48..0x4C: %d %s" % (n_set, len(item_set), item_set))
    print("Aot_reset-Muster (46 slot 09):    %d, davon Id 0x48..0x4C: %d %s" % (n_reset, len(reset), reset))
    d = open(os.path.join(CD, "STAGE1", "ROOM1010.RDT"), "rb").read()
    k = [hex(pc) for pc in range(len(d) - 22) if d[pc] == 0x50 and d[pc + 2] == 0x09 and d[pc + 1] < 48]
    print("Gegenprobe ROOM1010: %s (erwartet 0x996, 0x9ac, 0x9c2)" % k)


if __name__ == "__main__":
    main()
