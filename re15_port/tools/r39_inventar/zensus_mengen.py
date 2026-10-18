#!/usr/bin/env python3
"""Runde 39 Spur C (inventar) - Zensus: welche Item-Ids zeigen im Inventar eine ZAHL,
welche davon kann der Spieler GLEICH mehrfach bekommen, und welche stapeln heute nicht.

Quellen (alles ausgelieferte Bytes, nichts gewaehlt):
  * Ziffern-Gate RE1.5 FUN_80048f28: `sltiu id,0x22` @0x80049124 -> Ids 0x01..0x21 zeigen eine Zahl.
  * Eigenschaftstabelle @0x80074DA8 (Schritt 12): +0 Obergrenze (u8 gelesen @0x8004e338),
    +4 Kombinations-Satz (Leser @0x8004e9d8), +9 Anzahl der Saetze; Satzform 4 Byte
    {Partner, Ergebnis, Aktion, Bild} (Aktion 5 = SELF-STACK, Executor @0x8004e664).
  * RE2 (info/re2leon/PSX.EXE): Tabelle @0x800a9e1c Schritt 8 {u8 max, u8, u8, u8 Anzahl, u32 Satz},
    Satzform {Partner, Aktion, Ergebnis, Bild}; Kisten-Gate @0x80070490-94 (Id 0x14..0x1F).
  * Ausgaben im Spiel: alle Item_aot_set (0x50) aller RDTs (opcode-exakter Walk, scd_walk_lib).

Aufruf: python zensus_mengen.py [ASSET_ROOT]   (Default re15_port/shared_assets/PSX)
"""
import os, sys, glob, struct, collections
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, ".."))
import scd_walk_lib as W

REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
ROOT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "re15_port", "shared_assets", "PSX")
EXE  = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
EXE2 = os.path.join(REPO, "info", "re2leon", "PSX.EXE")

def lade(p):
    d = open(p, "rb").read()
    t = struct.unpack_from("<I", d, 0x18)[0]
    return d, (lambda a: 0x800 + a - t)

d1, o1 = lade(EXE)
d2, o2 = lade(EXE2)

def namen_re15():
    # Namens-Blob 0x800c4a28 + u16[0x800c495c + id*2] liegt in DEBUG.BIN (@0x800c0000)
    dbg = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
    out = {}
    for i in range(0x48):
        off = struct.unpack_from("<H", dbg, 0x495c + i * 2)[0]
        p = 0x4a28 + off; s = ""
        while dbg[p] != 0x07 and len(s) < 40:
            c = W.glyph(dbg[p]); s += c if c is not None else "?"; p += 1
        out[i] = s
    return out

NAMEN = namen_re15()

def eig15(i):
    a = 0x80074DA8 + i * 12
    mx, satz = struct.unpack_from("<II", d1, o1(a))
    anz = d1[o1(a) + 9]
    saetze = []
    for k in range(anz):
        s = d1[o1(satz) + 4 * k: o1(satz) + 4 * k + 4]
        saetze.append(tuple(s))
    return a, mx & 0xFF, satz, anz, saetze

# Ausgaben im Spiel
ausg = collections.defaultdict(list)
for p in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x48: continue
    room = os.path.basename(p)[4:8]
    for sec in (W.u32(d, 0x40), W.u32(d, 0x44)):
        if sec == 0 or sec >= len(d): continue
        se = W.rdt_section_end(d, sec)
        for (o, e, idx) in W.section_regions(d, sec, se):
            for (pc, op, sz) in W.walk_ops(d, sec + o, sec + e):
                if op == 0x50:
                    lf = (d[pc + 3] & 0x80) != 0
                    t = d[pc + 22] if lf else d[pc + 14]
                    a = d[pc + 24] if lf else d[pc + 16]
                    ausg[t].append("%s@0x%05X x%d" % (room, pc, a))

print("Id   Name                       Zahl max  Satz       n  Self-Stack(05) Ausgaben (Leon xxx0 / Elza xxx1)")
for i in range(1, 0x22):
    a, mx, satz, anz, saetze = eig15(i)
    selbst = [s for s in saetze if s[0] == i and s[2] == 5]
    print("0x%02X %-26s ja  %3d  0x%08X %d  %-14s %s" % (
        i, NAMEN[i][:26], mx, satz, anz, ("ja " + " ".join("%02x" % b for b in selbst[0])) if selbst else "NEIN",
        ", ".join(ausg[i]) if ausg[i] else "-"))
print()
print("RE2 (Leon) @0x800a9e1c: Id 0x1E Ink Ribbon / Munition 0x14..0x1D")
for i in list(range(0x14, 0x20)):
    a = 0x800a9e1c + i * 8
    b = d2[o2(a): o2(a) + 8]
    mx, anz = b[0], b[3]
    satz = struct.unpack_from("<I", b, 4)[0]
    s = [tuple(d2[o2(satz) + 4 * k: o2(satz) + 4 * k + 4]) for k in range(anz)]
    selbst = [x for x in s if x[0] == i and x[1] == 5]
    print("  0x%02X @0x%08X max %3d Saetze %d @0x%08X self-stack %s" % (
        i, a, mx, anz, satz, ("%02x %02x %02x %02x" % selbst[0]) if selbst else "NEIN"))
