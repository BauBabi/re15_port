#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur F "dining1141": Zensus fuer die Ressourcen-Vergabe (VERTRAG 1.1-1.3).

  python re15_port/tools/r38_dining/zensus.py

1. ITEM-IDS: jedes Item_aot_set (0x50, Id u16 @+14, Aufnahme-Bit @+18) und jedes Aot_reset (0x46) mit
   sce 9 (Id u16 @+4) in ALLEN RDTs unter shared_assets/PSX/STAGE*/ (opcode-exakter Walk ueber
   scd_walk_lib = die EINE Laengentabelle des Ports). Ausgabe: je Id 0x22..0x47 die Zahl der
   Platzierungen + Eigenschaftssatz aus PSX.EXE @0x80074DA8 + Id*12 (RAM-Basis aus dem EXE-Kopf).
2. BANK 9: jedes Bit, das ein Item_aot_set als Aufnahme-Bit fuehrt (pc[18]) und jedes Ck/Set/Cmp
   der Bank 9 in den Skripten — die Ressourcen 64/66/67 muessen dort fehlen.
3. NACHRICHTEN: Zahl der RDT-Nachrichten je Raum 1011/1021/1131/1141 (Port-Ids liegen dahinter).
"""
import os, sys, struct, glob, collections
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")

def rdts():
    for p in sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x100:   # Platzhalter-Dateien (4 B) tragen kein SCD
            continue
        yield os.path.basename(p)[4:8], d

def ops(d):
    for (tag, idx), liste in sorted(L.regionen(d).items()):
        for (pc, op, sz) in liste:
            yield tag, idx, pc, op, sz

def main():
    platz = collections.defaultdict(list)
    bank9_item = collections.defaultdict(list)
    bank9_skript = collections.defaultdict(list)
    nmsg = {}
    for rid, d in rdts():
        for tag, idx, pc, op, sz in ops(d):
            if op == 0x50:
                item = L.u16(d, pc + 14); bit = L.u16(d, pc + 18)
                platz[item].append("%s %s%02d @0x%05X" % (rid, tag, idx, pc))
                bank9_item[bit].append("%s @0x%05X Item 0x%02X" % (rid, pc, item))
            elif op == 0x46 and d[pc + 2] == 9:
                item = L.u16(d, pc + 4)
                platz[item].append("%s %s%02d @0x%05X (Aot_reset sce9)" % (rid, tag, idx, pc))
            elif op in (0x21, 0x22) and d[pc + 1] == 9:
                bank9_skript[d[pc + 2]].append("%s @0x%05X %s" % (rid, pc, L.NAMES.get(op, "?")))
        if rid in ("1011", "1021", "1131", "1141", "1140", "1171"):
            try:
                nmsg[rid] = len(L.messages(d))
            except Exception as e:  # noqa: BLE001
                nmsg[rid] = "?(%s)" % e
    exe = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
    taddr = struct.unpack_from("<I", exe, 0x18)[0]
    def ram(a, n):
        o = 0x800 + (a - taddr)
        return exe[o:o + n]
    print("== 1. Item-Ids 0x22..0x47: Platzierungen + Eigenschaftssatz @0x80074DA8+Id*12")
    for i in range(0x22, 0x48):
        satz = ram(0x80074DA8 + i * 12, 12).hex(" ")
        print("  0x%02X  %2d Platz.  Satz %s  %s" % (i, len(platz[i]), satz,
              "; ".join(platz[i][:3]) + (" ..." if len(platz[i]) > 3 else "")))
    print("== 2. Bank 9: Bits 60..70 (Item-Aufnahme-Bit / Skript-Ck/Set)")
    for b in range(60, 71):
        print("  (9,%3d) Item %-3d Skript %-3d %s" % (b, len(bank9_item[b]), len(bank9_skript[b]),
              "; ".join((bank9_item[b] + bank9_skript[b])[:3])))
    print("== 3. RDT-Nachrichten je Raum:", nmsg)

if __name__ == "__main__":
    main()
