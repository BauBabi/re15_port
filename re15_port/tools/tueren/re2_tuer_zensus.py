#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_zensus.py - alle Door_aot_set der RE2-Leon-Raeume, gepaart zu physischen Tueren.

Runde 31, Teil T2 (analysis/befunde_runde31/tueren_02_re2.md, Abschnitt 2).
Liest NUR: info/re2leon/PL0/RDT/ROOM*.RDT (Walker analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py).
Schreibt NUR: build/r31_tueren/t2/re2_tueren.json

Satzformat (RE2 Leon PSX.EXE, selbst disassembliert):
  0x3B Door_aot_set    Handler 0x80054be4 (Tabelle 0x800a74c8[59]): Satz = pc+2 (@0x80054c30 addiu v0,v0,2),
                       Laenge 32 (@0x80054c40 addiu v0,v0,32). Rechteck pc+6..13 (x,z s16, w,d u16).
                       Nutzlast = Satz+12 = pc+14 (Dispatch @0x80051444 addiu a0,s0,12).
  0x68 Door_aot_set_4p Handler 0x80054c50 (Tabelle [104]): Satz = pc+2 (@0x80054c9c), sat |= 0x80
                       (@0x80054cb4 ori v0,v0,0x80), Laenge 40 (@0x80054cc4 addiu v0,v0,40). Vier Punkte
                       pc+6..21; Nutzlast = Satz+20 = pc+22 (@0x8005141c addiu a0,s0,20).
  Nutzlast (FUN_80026b7c / LAB_80051514 / FUN_80015064 / FUN_80013c1c, 03_tuersequenz.md 4):
    +0,+2,+4 Ziel x,y,z  +6 Zielrichtung  +8 Stage +9 Raum +10 Cut +11 Etage
    +12 Tuertextur-Typ = Archiv (@0x80015088 lbu v1,12(v0))
    +13 Tuertyp = Variante, Bit 7 extra (@0x80013e5c lbu v0,13(a0); @0x80013e6c andi 0xff7f; @0x80013e84 andi 0x80)
    +14 Klopf-Typ (nicht gelesen)  +15 Schluessel-Flag (@0x800515a8)  +16 Schluesselart (@0x800515d0)

Aufruf:  python re15_port/tools/tueren/re2_tuer_zensus.py [--liste] [--paare]
"""
import argparse
import json
import math
import os
import struct
import sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
import re2_scd_walk  # noqa: E402

OUT = os.path.join(REPO, "build", "r31_tueren", "t2")
RDTDIR = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")


def reihe_und_stage(fn):
    """ROOMsrrp.RDT: s = '1'..'7' (Reihe A) oder 'A'..'G' (Reihe B, dieselben Stages)."""
    c = fn[4]
    if c.isdigit():
        return "1", int(c) - 1
    return "A", ord(c) - ord("A")


def dateiname(reihe, stage, raum):
    c = chr(ord(reihe) + stage)
    return "ROOM%s%02X0.RDT" % (c, raum)


def lies_tueren():
    tueren = []
    for fn, d, blk, base, sub, s, e, last in re2_scd_walk.all_subs():
        ops, st = re2_scd_walk.walk(d, s, e, last)
        for (pc, op, ln) in ops:
            if op not in (0x3B, 0x68):
                continue
            rec = {"datei": fn, "block": blk, "sub": sub, "pc": pc, "op": op,
                   "slot": d[pc + 1], "sce": d[pc + 2], "sat": d[pc + 3], "etage": d[pc + 4],
                   "super": d[pc + 5]}
            if op == 0x3B:
                x, z, w, dd = struct.unpack_from("<hhHH", d, pc + 6)
                rec["rechteck"] = [x, z, w, dd]
                rec["punkte"] = [[x, z], [x + w, z], [x + w, z + dd], [x, z + dd]]
                p = pc + 14
            else:
                pts = struct.unpack_from("<8h", d, pc + 6)
                rec["punkte"] = [[pts[0], pts[1]], [pts[2], pts[3]], [pts[4], pts[5]], [pts[6], pts[7]]]
                xs = [q[0] for q in rec["punkte"]]
                zs = [q[1] for q in rec["punkte"]]
                rec["rechteck"] = [min(xs), min(zs), max(xs) - min(xs), max(zs) - min(zs)]
                p = pc + 22
            nx, ny, nz, nd = struct.unpack_from("<hhhh", d, p)
            rec.update({"ziel_pos": [nx, ny, nz], "ziel_dir": nd & 0xFFFF,
                        "ziel_stage": d[p + 8], "ziel_raum": d[p + 9], "ziel_cut": d[p + 10],
                        "ziel_etage": d[p + 11], "archiv": d[p + 12], "typ_roh": d[p + 13],
                        "variante": d[p + 13] & 0x7F, "bit7": d[p + 13] >> 7,
                        "klopf": d[p + 14], "schl_flag": d[p + 15], "schl_art": d[p + 16], "frei": d[p + 17],
                        "nutzlast_off": p, "bytes": d[pc:pc + ln].hex()})
            reihe, stg = reihe_und_stage(fn)
            rec["reihe"] = reihe
            rec["stage"] = stg
            rec["raum"] = int(fn[5:7], 16)
            rec["ziel_datei"] = dateiname(reihe, rec["ziel_stage"], rec["ziel_raum"])
            tueren.append(rec)
    return tueren


def punkt_rechteck_abstand(pt, rec):
    """Abstand eines Punktes (x,z) zum AOT-Viereck (0 = innen). Fuer Rechtecke exakt."""
    x, z = pt
    rx, rz, rw, rd = rec["rechteck"]
    dx = max(rx - x, 0, x - (rx + rw))
    dz = max(rz - z, 0, z - (rz + rd))
    return math.hypot(dx, dz)


def mitte(rec):
    xs = [q[0] for q in rec["punkte"]]
    zs = [q[1] for q in rec["punkte"]]
    return (sum(xs) / 4.0, sum(zs) / 4.0)


def seiten_bilden(tueren):
    """Mehrere Saetze fuer DIESELBE Tuerseite (gleiches Rechteck, gleiches Ziel) zusammenfassen:
    RE2 setzt je nach Szenario-Flag verschiedene Saetze in verschiedenen Subs."""
    seiten = {}
    for i, t in enumerate(tueren):
        k = (t["datei"], tuple(map(tuple, t["punkte"])), t["ziel_datei"], tuple(t["ziel_pos"]))
        s = seiten.setdefault(k, {"datei": t["datei"], "punkte": t["punkte"], "rechteck": t["rechteck"],
                                  "ziel_datei": t["ziel_datei"], "ziel_pos": t["ziel_pos"],
                                  "ziel_dir": t["ziel_dir"], "ziel_cut": t["ziel_cut"],
                                  "saetze": []})
        s["saetze"].append(i)
    return list(seiten.values())


def paaren(tueren, seiten):
    """Physische Tuer = Seite A in Raum R1 mit Ziel R2 + Seite B in R2 mit Ziel R1, wobei das Ziel
    von A im Rechteck von B liegt (bzw. nahe) und umgekehrt. Beste gegenseitige Wahl."""
    nach_datei = defaultdict(list)
    for j, s in enumerate(seiten):
        nach_datei[s["datei"]].append(j)
    kand = {}
    for i, a in enumerate(seiten):
        best = None
        for j in nach_datei.get(a["ziel_datei"], []):
            b = seiten[j]
            if b["ziel_datei"] != a["datei"]:
                continue
            d1 = punkt_rechteck_abstand(a["ziel_pos"][0::2], b)   # A-Ziel im Rechteck von B
            d2 = punkt_rechteck_abstand(b["ziel_pos"][0::2], a)
            sc = d1 + d2
            if best is None or sc < best[0]:
                best = (sc, j, d1, d2)
        kand[i] = best
    paare = []
    genommen = set()
    for i, best in sorted(kand.items(), key=lambda kv: (kv[1][0] if kv[1] else 1e18)):
        if best is None or i in genommen:
            continue
        sc, j, d1, d2 = best
        if j in genommen:
            continue
        rb = kand.get(j)
        if rb is None or rb[1] != i:
            continue
        if i == j:
            continue
        genommen.add(i)
        genommen.add(j)
        paare.append({"a": i, "b": j, "abstand": [round(d1), round(d2)]})
    einzel = [i for i in range(len(seiten)) if i not in genommen]
    return paare, einzel


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--liste", action="store_true")
    ap.add_argument("--paare", action="store_true")
    a = ap.parse_args()
    tueren = lies_tueren()
    seiten = seiten_bilden(tueren)
    for s in seiten:
        vs = sorted({(tueren[k]["archiv"], tueren[k]["variante"], tueren[k]["bit7"]) for k in s["saetze"]})
        s["archiv_varianten"] = vs
    paare, einzel = paaren(tueren, seiten)
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "re2_tueren.json"), "w") as f:
        json.dump({"saetze": tueren, "seiten": seiten, "paare": paare, "einzel": einzel}, f, indent=1)
    print("Door_aot_set-Saetze: %d (0x3B: %d, 0x68: %d), Raeume mit Tueren: %d von %d RDT" % (
        len(tueren), sum(t["op"] == 0x3B for t in tueren), sum(t["op"] == 0x68 for t in tueren),
        len({t["datei"] for t in tueren}), len([x for x in os.listdir(RDTDIR) if x.endswith(".RDT")])))
    print("Tuerseiten (gleiches Viereck + Ziel): %d; Paare: %d; ungepaart: %d" % (len(seiten), len(paare), len(einzel)))
    print("Variante (& 0x7f):", sorted(Counter(t["variante"] for t in tueren).items()))
    print("Bit 7:", sum(t["bit7"] for t in tueren))
    if a.liste:
        for t in tueren:
            print("%s %s sub%-2d @0x%05X op%02X slot%-2d rect=%s -> %s pos=%s cut=%d arch=%02X var=%d b7=%d key=%02X/%02X" % (
                t["datei"], t["block"], t["sub"], t["pc"], t["op"], t["slot"], t["rechteck"], t["ziel_datei"],
                t["ziel_pos"], t["ziel_cut"], t["archiv"], t["variante"], t["bit7"], t["schl_flag"], t["schl_art"]))
    if a.paare:
        for p in paare:
            A, B = seiten[p["a"]], seiten[p["b"]]
            print("%s %s  <->  %s %s  d=%s" % (A["datei"], A["archiv_varianten"], B["datei"], B["archiv_varianten"], p["abstand"]))


if __name__ == "__main__":
    main()
