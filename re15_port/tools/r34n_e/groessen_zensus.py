# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: welche Rechteckgroesse haben die Item_aot_set des Originals?

Grundlage der PORT-WAHL "Aufhebe-Rechteck 1000 x 1000, mittig" (E_dokumente.md 5.2). Walker wie
zensus_item_nachricht.py (Laengen = scd_vm.c s_opcode_sizes; 18 Bloecke brechen ab und fehlen, die
Zahlen sind Untergrenzen). Gezaehlt: 0x50 Item_aot_set mit sce 9 (pc[2]), Id < 0x70 (schliesst
Fehlgriffe in Nachrichtentext aus, s. ROOM2030 @0x204c), Breite u16 @+10, Tiefe u16 @+12.

Aufruf: python re15_port/tools/r34n_e/groessen_zensus.py
"""
import glob
import os
import sys
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import zensus_item_nachricht as Z  # noqa: E402


def main():
    paare, ws, ds = Counter(), Counter(), Counter()
    n = 0
    for p in sorted(glob.glob(os.path.join(Z.CD, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x60:
            continue
        for sec in (Z.u32(d, 0x40), Z.u32(d, 0x44)):
            for a, e, idx in Z.regionen(d, sec):
                recs, ok = Z.walk(d, a, e)
                for r in recs:
                    if r[0] == "ITEM" and r[6] < 0x70 and d[r[7] + 2] == 0x09:
                        n += 1
                        paare[(r[4], r[5])] += 1
                        ws[r[4]] += 1
                        ds[r[5]] += 1
    print("Item_aot_set (sce 9) gezaehlt: %d" % n)
    print("haeufigste (Breite, Tiefe): %s" % paare.most_common(6))
    print("haeufigste Breite: %s" % ws.most_common(4))
    print("haeufigste Tiefe:  %s" % ds.most_common(4))


if __name__ == "__main__":
    main()
