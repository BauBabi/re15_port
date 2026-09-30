# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: welche AOT-Slots und obj_ids beruehrt IRGENDEIN Skriptblock eines Raums?

Die Sonde (`aots`, `modelle`) sieht nur den Zustand nach dem Raumstart. Ein Slot, den erst ein
spaeteres Ereignis setzt (Aot_on/Aot_reset/Aot_set in einer sub), waere dort frei und kollidierte
trotzdem. Dieses Werkzeug liest ALLE Bloecke (main + subs) und listet je Raum jeden Zugriff:

  0x2C Aot_set        slot = pc[1]   (20/28 B nach pc[3]&0x80)
  0x3B Door_aot_set   slot = pc[1]   (32/40 B)
  0x50 Item_aot_set   slot = pc[1]   (22/30 B)
  0x46 Aot_reset      slot = pc[1]   (10 B, scd_vm.c op_aot_reset)
  0x47 Aot_on         slot = pc[1]   (scd_vm.c s_op_table[0x47] = op_aot_on)
  0x2D Obj_model_set  obj  = pc[1]   (34 B, scd_vm.c op_obj_model_set)

Laengen wie zensus_item_nachricht.py (= scd_vm.c s_opcode_sizes). Unbekannter Opcode -> der Block
wird als ABGEBROCHEN gemeldet (nicht still).

Aufruf: python re15_port/tools/r34n_e/slot_zensus.py ROOM1000 ROOM1001 ...
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import zensus_item_nachricht as Z  # noqa: E402

CD = os.path.join(Z.REPO, "re15_port", "shared_assets", "PSX")
NAMEN = {0x2C: "Aot_set", 0x3B: "Door_aot_set", 0x50: "Item_aot_set", 0x46: "Aot_reset",
         0x47: "Aot_on", 0x2D: "Obj_model_set"}


def main():
    for name in sys.argv[1:]:
        d = open(os.path.join(CD, "STAGE%s" % name[4], name + ".RDT"), "rb").read()
        slots = {}
        objs = {}
        abbruch = []
        for teil, sec in (("main", Z.u32(d, 0x40)), ("sub", Z.u32(d, 0x44))):
            for a, e, idx in Z.regionen(d, sec):
                pc = a
                while pc < e:
                    sz = Z.op_size(d, pc)
                    if sz is None or pc + sz > e:
                        abbruch.append("%s%02d@0x%05X op 0x%02X" % (teil, idx, pc, d[pc]))
                        break
                    op = d[pc]
                    if op in (0x2C, 0x3B, 0x50, 0x46, 0x47):
                        slots.setdefault(d[pc + 1], []).append("%s %s%02d@0x%05X" % (NAMEN[op], teil, idx, pc))
                    elif op == 0x2D:
                        objs.setdefault(d[pc + 1], []).append("%s%02d@0x%05X" % (teil, idx, pc))
                    pc += sz
        print("%s: Slots %s" % (name, sorted(slots)))
        for s in sorted(slots):
            print("   slot %2d: %s" % (s, "; ".join(slots[s])))
        print("   obj_ids %s" % sorted(objs))
        for o in sorted(objs):
            print("   obj %2d: %s" % (o, "; ".join(objs[o])))
        print("   abgebrochene Bloecke: %s" % (", ".join(abbruch) if abbruch else "keine"))


if __name__ == "__main__":
    main()
