#!/usr/bin/env python3
"""Nachbesserung 1 Spur H R38: Zensus aller RE2-Skript-Se_on mit Muendungsfeuer-ESP 0x18 ("GUN").
Walk opcode-exakt (tools/re2_sicherung/re2_scd_walk.py). Se_on-Lage laut Handler @0x80056428:
 [1]=Bank [2]=Satz [3]=Byte3 [4]=Art (Tabelle @0x80011458: 0 Welt,1 Spieler,2 Partner @0x800cfe18,
 3 Gegner @0x800cfe1c[idx], 4 Objekt) [5]=Index [6..11]=Versatz.
Je Treffer: Raum, Block, Offset, Satz, Art/Index, Schuetze (Sce_em_set-Typ), Welle sha1/Laenge."""
import os, sys, struct, glob, hashlib, collections
BAUM = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(BAUM, "tools", "re2_sicherung"))
sys.path.insert(0, os.path.join(BAUM, "re15_port", "tools", "r36_l"))
os.environ.setdefault("RE15_INFO_DIR", "C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information")
import re2_scd_walk as W
import re2_raum_se_zensus as Z
RDT = os.path.join(BAUM, "info", "re2leon", "PL0", "RDT")
ART = {0: "Welt", 1: "Spieler", 2: "Partner", 3: "Gegner", 4: "Objekt"}
treffer = []
for p in sorted(glob.glob(os.path.join(RDT, "ROOM*.RDT"))):
    raw = open(p, "rb").read()
    room = os.path.basename(p)[4:8]
    ems = {}
    blocks = list(W.rdt_blocks(raw))
    alle = []
    for name, s, e in blocks:
        recs, st = W.walk(raw[s:e])
        for off, op, r in recs:
            alle.append((name, s + off, op, r))
            if op == 0x44 and len(r) >= 4:
                ems.setdefault(r[2], set()).add(r[3])
    for i, (name, a, op, r) in enumerate(alle):
        if op != 0x36 or len(r) < 12:
            continue
        # Muendungsfeuer: Sce_espr_on (0x3A) mit Id 0x18 in den naechsten 4 Saetzen desselben Blocks
        gun = None
        for j in range(i + 1, min(i + 1 + int(os.environ.get("FENSTER", "4")), len(alle))):
            n2, a2, op2, r2 = alle[j]
            if n2 != name:
                break
            if op2 == 0x3A and len(r2) > 2 and r2[2] == 0x18:
                gun = a2
                break
        if gun is None:
            continue
        bank, satz, b3, art, idx = r[1], r[2], r[3], r[4], r[5]
        w = Z.satz_welle(raw, satz) if bank == 2 else None
        if art == 3:
            sch = "Typ " + ",".join("0x%02X" % t for t in sorted(ems.get(idx, [])))
        elif art == 2:
            sch = "Typ " + ",".join("0x%02X" % t for t in sorted(ems.get(0xFF, [])))
        else:
            sch = "-"
        treffer.append((room, name, a, satz, b3, art, idx, sch, gun, w))
fam = collections.defaultdict(list)
for t in treffer:
    room, name, a, satz, b3, art, idx, sch, gun, w = t
    print("ROOM%s %-6s @0x%05X Se_on Bank2 Satz 0x%02X Byte3 %d Art %d(%s) Idx %d  Schuetze %-14s GUN @0x%05X  Welle %s %s B" % (
        room, name, a, satz, b3, art, ART.get(art, "?"), idx, sch, gun,
        w["sha"][:12] if w else "-", w["vag_len"] if w else "-"))
    if w:
        fam[w["sha"][:12]].append("%s/%s(Art %d)" % (room, name, art))
print()
for k, v in fam.items():
    print("Welle %s: %d Schuesse: %s" % (k, len(v), " ".join(v)))
