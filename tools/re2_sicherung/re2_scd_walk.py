#!/usr/bin/env python3
"""re2_scd_walk.py - opcode-exakter Walk ueber die ROHEN RE2-SCD-Bloecke.

Warum roh: das im Repo liegende Decompilat
info/re2leon/PL0/RDT/room*/scd/*.c ist an Stellen DESYNCHRON — room60D0/scd/sub02.c
enthaelt wortwoertlich `Item_aot_set_4p(/* WIP */);` und gibt fuer den davor
stehenden Item_aot_set `sat=0, x=125, z=125, w=256, d=256` aus (unplausibel;
alle intakten Eintraege haben sat=49 und Welt-Koordinaten). Also wird hier
byteweise gelaufen.

Laengentabelle: primaer aus den 143 Handlern ab 0x800A74C8 selbst gezogen
(re2_scd_lens.py, PC-Vorschub 'addiu rX,rPC,N / sw rX,28(a0)'), fuer die 15
Handler ohne eindeutigen Vorschub aus der RE2-Opcode-Referenz
info/Resident_Evil_und_Playstation_Information/information293.txt ergaenzt.
Beide Quellen werden gegeneinander geprueft; Abweichungen werden gemeldet.

Item_aot_set (0x4E, 22 Byte) — Feldlage aus dem Handler FUN_80054CD4:
  80054CF4: lhu a1,18(s0)  -> +18 flag (Argument des Flag-Tests FUN_80077360)
  80054CF8: lbu s2,20(s0)  -> +20 md1  (Objekt-/Modell-Slot, Schranke < 0x20)
  80054CFC: addiu v0,s0,22 -> Laenge 22
  80054DD4: lbu s1,21(s0)  -> +21 action
Daraus (und aus der Belegung der Koordinaten) folgt:
  +0 op  +1 aot  +2 sce  +3 sat  +4 nFloor  +5 super
  +6 x(s16) +8 z(s16) +10 w(s16) +12 d(s16)
  +14 i_item(u16)  +16 n_item(u16)  +18 flag(u16)  +20 md1  +21 action
"""
import struct, sys, os, re, glob, collections

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import re2_scd_lens
DOC = os.path.join(REPO, "info", "Resident_Evil_und_Playstation_Information",
                   "information293.txt")
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

ROW = re.compile(r"^0x([0-9a-f]{2})\t([A-Za-z_0-9]+)\t(\d+)\t", re.M)

# Handler, deren PC-Vorschub der automatische Scan NICHT eindeutig findet
# (mehrere sw-28-Pfade oder der Vorschub laeuft ueber den Block-Stack).
# Jeder Wert ist einzeln am Handler disassembliert:
OVERRIDE = {
    0x01: 2,   # Evt_end   @0x800537FC: kein PC-Vorschub, poppt den Block-Stack
               #            (lb a2,2(a3) = Task+2). Record {op, ret_value} = 2.
    0x03: 4,   # Evt_chain @0x80053878: {op, u0, u1, event} = 4
    0x07: 4,   # Else_ck   @0x80053964: lhu v0,2(v1) -> u16 Blocklaenge bei +2,
               #            PC += Blocklaenge. Record selbst 4 Byte.
    0x10: 2,   # Ewhile    @0x80053E0C
    0x17: 6,   # Goto      @0x8005415C: lh t0,4(a1) -> s16 rel_offset bei +4 => 6
    0x18: 2,   # Gosub     @0x800541A8: addiu v1,v1,2 (Ruecksprung = PC+2) => 2
    0x19: 2,   # Return    @0x80054210
    0x1A: 2,   # Break     @0x80054268
    0x44: 22,  # Sce_em_set: der Scan trifft einen falschen sw-28-Pfad und liefert 1.
               #   @0x80057170 lw v0,28(s5) / @0x8005720C addiu a1,v0,2 (Nutzlast ab +2).
               #   22 deckt sich mit analysis/re2_ermittlung_2026-08-30/STRAENGE_ROH.md:170
               #   und mit information293.txt; validiert durch 0 Desyncs im Vollwalk.
    0x8E: 24,  # dito (Scan liefert 1); 24 aus STRAENGE_ROH.md:170 + Vollwalk-Validierung.
}


def doc_table():
    txt = open(DOC, "r", errors="replace").read()
    out = {}
    for m in ROW.finditer(txt):
        op = int(m.group(1), 16)
        out.setdefault(op, (m.group(2), int(m.group(3))))
    return out


def lengths(verbose=False):
    code = re2_scd_lens.table()
    doc = doc_table()
    lens, names, src, mismatch = {}, {}, {}, []
    for op in range(re2_scd_lens.N_OPS):
        h, n, why = code[op]
        dn = doc.get(op)
        if n is not None and dn is not None and dn[1] != n:
            mismatch.append((op, n, dn[1], dn[0]))
        if op in OVERRIDE:
            lens[op] = OVERRIDE[op]; src[op] = "override"
        elif n is not None:
            lens[op] = n; src[op] = "handler"
        elif dn is not None:
            lens[op] = dn[1]; src[op] = "doku"
        names[op] = dn[0] if dn else "op%02X" % op
    if verbose:
        print("Laengen: %d aus dem Handler, %d aus der Doku, %d fehlen" % (
            sum(1 for v in src.values() if v == "handler"),
            sum(1 for v in src.values() if v == "doku"),
            re2_scd_lens.N_OPS - len(lens)))
        print("Abweichungen Handler<->Doku: %d %s" % (len(mismatch), mismatch))
    return lens, names, src, mismatch


LENS, NAMES, SRC, MISMATCH = lengths()


def walk(buf):
    """[(offset, opcode, recordbytes)], plus Status 'ok'/'desync'."""
    out, p = [], 0
    while p < len(buf):
        op = buf[p]
        n = LENS.get(op)
        if n is None or p + n > len(buf):
            return out, "desync@0x%X op=0x%02X" % (p, op)
        out.append((p, op, buf[p:p + n]))
        p += n
    return out, "ok"


ITEM_OPS = {0x4E: 22, 0x69: 30}


def item_records(buf):
    recs, st = walk(buf)
    out = []
    for off, op, r in recs:
        if op == 0x4E:
            out.append(dict(off=off, op=op, aot=r[1], sce=r[2], sat=r[3],
                            nfloor=r[4], super=r[5],
                            x=struct.unpack_from("<h", r, 6)[0],
                            z=struct.unpack_from("<h", r, 8)[0],
                            w=struct.unpack_from("<h", r, 10)[0],
                            d=struct.unpack_from("<h", r, 12)[0],
                            i_item=struct.unpack_from("<H", r, 14)[0],
                            n_item=struct.unpack_from("<H", r, 16)[0],
                            flag=struct.unpack_from("<H", r, 18)[0],
                            md1=r[20], action=r[21], raw=r))
        elif op == 0x69:
            # 30 Byte: 6 Kopf + 4 Punkte a 2 s16 (16 Byte) = +22 Nutzlast
            out.append(dict(off=off, op=op, aot=r[1], sce=r[2], sat=r[3],
                            nfloor=r[4], super=r[5],
                            i_item=struct.unpack_from("<H", r, 22)[0],
                            n_item=struct.unpack_from("<H", r, 24)[0],
                            flag=struct.unpack_from("<H", r, 26)[0],
                            md1=r[28], action=r[29], raw=r))
    return out, st


if __name__ == "__main__":
    lengths(verbose=True)
    import re2_items
    if len(sys.argv) > 1 and sys.argv[1] == "one":
        buf = open(sys.argv[2], "rb").read()
        recs, st = walk(buf)
        print("%s: %d Records, %s" % (sys.argv[2], len(recs), st))
        for off, op, r in recs:
            print("  +0x%04X  0x%02X %-18s %s" % (
                off, op, NAMES.get(op, "?"), " ".join("%02x" % b for b in r)))
        sys.exit(0)

    hist = collections.Counter()
    desync = []
    total = 0
    files = sorted(glob.glob(os.path.join(RDT, "room*", "scd", "*.scd")))
    print("SCD-Bloecke: %d" % len(files))
    for f in files:
        room = os.path.basename(os.path.dirname(os.path.dirname(f)))
        ent = os.path.splitext(os.path.basename(f))[0]
        buf = open(f, "rb").read()
        items, st = item_records(buf)
        if st != "ok":
            desync.append((room, ent, st))
        for it in items:
            total += 1
            hist[it["i_item"]] += 1
            nm = re2_items.name(it["i_item"])[1] if it["i_item"] < re2_items.N_NAMES else "??"
            print("%-9s %-8s +0x%04X op=0x%02X id=%3d %-22s n=%-3d flag=%-4d md1=%-3d act=%-3d %s"
                  % (room, ent, it["off"], it["op"], it["i_item"], nm,
                     it["n_item"], it["flag"], it["md1"], it["action"], st))
    print()
    print("--- %d Item-AOTs, %d verschiedene Ids, %d Bloecke desynchron ---"
          % (total, len(hist), len(desync)))
    for iid in sorted(hist):
        nm = re2_items.name(iid)[1] if iid < re2_items.N_NAMES else "??"
        print("  id %3d  x%-3d %s" % (iid, hist[iid], nm))
    if desync:
        print("desynchron:", desync[:40])
