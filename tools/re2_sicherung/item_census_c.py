#!/usr/bin/env python3
"""item_census_c.py - Item-Zensus aus den vorhandenen RE2-SCD-Decompilaten.

Zweite, UNABHAENGIGE Quelle neben dem Roh-Byte-Walk (re2_scd_items.py):
info/re2leon/PL0/RDT/room*/scd/*.c enthaelt fertige Item_aot_set(...)-Aufrufe.
Die vier letzten Argumente sind dort als BIG-ENDIAN-Halbwoerter ausgegeben
(Werkzeug-Eigenheit), daher wird byteweise gedreht:

  Item_aot_set(aot, sce, sat, nFloor, super, x, z, w, d, i_item, n_item, flag, md1_action)
                                                          ^^^^^^ ^^^^^^ ^^^^  ^^^^^^^^^^^
  i_item = byteswap16(arg[9]) = Item-Id     (Record-Offset +14)
  n_item = byteswap16(arg[10]) = Anzahl     (Record-Offset +16)
  flag   = byteswap16(arg[11]) = Flag-Bit   (Record-Offset +18)
  md1    = arg[12] & 0xFF, action = arg[12] >> 8

Record-Layout aus dem Handler FUN_80054CD4 (Opcode 0x4E, 22 Byte):
  80054CF4: lhu a1,18(s0)   -> +18 = flag  (Argument fuer FUN_80077360 = Flag-Test)
  80054CF8: lbu s2,20(s0)   -> +20 = md1   (Modell-/Objekt-Slot, < 0x20)
  80054CFC: addiu v0,s0,22  -> Recordlaenge 22
  80054DD4: lbu s1,21(s0)   -> +21 = action
"""
import re, os, sys, glob, collections

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

CALL = re.compile(r"(Item_aot_set(?:_4p)?)\s*\(([^)]*)\)")


def bswap16(v):
    v &= 0xFFFF
    return ((v & 0xFF) << 8) | (v >> 8)


def parse():
    rows = []
    for f in sorted(glob.glob(os.path.join(RDT, "room*", "scd", "*.c"))):
        room = os.path.basename(os.path.dirname(os.path.dirname(f)))
        ent = os.path.splitext(os.path.basename(f))[0]
        txt = open(f, "r", errors="replace").read()
        for m in CALL.finditer(txt):
            fn = m.group(1)
            args = [a.strip() for a in m.group(2).split(",")]
            vals = []
            for a in args:
                try:
                    vals.append(int(a, 0))
                except ValueError:
                    vals.append(None)
            rows.append((room, ent, fn, vals, m.start()))
    return rows


if __name__ == "__main__":
    rows = parse()
    hist = collections.Counter()
    sys.path.insert(0, HERE)
    import re2_items
    print("%-9s %-8s %-18s %-4s %-24s %-5s %-6s %s" % (
        "room", "entry", "opcode", "id", "name(EN)", "cnt", "flag", "md1/act"))
    for room, ent, fn, v, _ in rows:
        if fn == "Item_aot_set" and len(v) >= 13:
            iid, cnt, flg, ma = bswap16(v[9]), bswap16(v[10]), bswap16(v[11]), v[12]
        elif fn == "Item_aot_set_4p" and len(v) >= 17:
            iid, cnt, flg, ma = bswap16(v[13]), bswap16(v[14]), bswap16(v[15]), v[16]
        else:
            print("!! unerwartete Argzahl", room, ent, fn, len(v)); continue
        hist[iid] += 1
        nm = re2_items.name(iid)[1] if iid < re2_items.N_NAMES else "??"
        print("%-9s %-8s %-18s %3d  %-24s %-5d 0x%04X 0x%04X" % (
            room, ent, fn, iid, nm, cnt, flg, ma))
    print()
    print("--- Histogramm: %d Platzierungen, %d verschiedene Item-Ids ---" % (
        sum(hist.values()), len(hist)))
    for iid in sorted(hist):
        nm = re2_items.name(iid)[1] if iid < re2_items.N_NAMES else "??"
        print("  id %3d  x%-3d %s" % (iid, hist[iid], nm))
