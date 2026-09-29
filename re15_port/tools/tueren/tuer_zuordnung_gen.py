#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuer_zuordnung_gen.py - Port-Tabelle "RE1.5-Tuerseite -> RE2-Tuerarchiv" aus zuordnung.json.

Runde 31, Stufe 4 (analysis/befunde_runde31/tueren_04_bau.md Abschnitt 1). Quelle ist die
Zuordnung der Stufe 3 (analysis/befunde_runde31/tueren_03/zuordnung.json, Dossier
tueren_03_zuordnung.md): je abgedeckter Tuerseite Archiv, Variante, Griff-Tausch und der
Port-Schluessel (alle Raumdateien xxx0/xxx1, Band pc[4], Rechteck bzw. vier Punkte).

Die Wahl Tuer -> Archiv/Variante ist PORT-WAHL aus dem Bildvergleich (RE1.5 fuehrt kein Archiv:
Payload+12/+13 = 0 in 649 von 653 Door_aot_set) - so steht es in jeder Zeile. RE2-Angleichung
(Beta -> Retail): RE1.5 hat die Tuermaschine, aber nur DOOR00 mit Evt_end.

Schluessel = Raum + Flaeche + Band, NICHT der Slot (tueren_02_re2.md 6.2). Die Flaeche wird so
umgerechnet, wie der Port sie haelt (engine/src/scd_vm.c op_door_aot_set): Rechteck (x,z,w,d) ->
Mitte x + w/2, z + d/2 (C-Division), Halbmass |w|/2, |d|/2; Viereck -> die vier Punkte pc+6..21.

Ausserdem je Griff-Tausch-Paar (Archiv <- Spender) die Werte, die der Zeichner braucht, gemessen
mit dem Katalog-Simulator (re15_port/tools/tor/tuerkatalog.py, [SIM]): Griff-Mesh des Archivs und
des Spenders (Kind-Objekt des Blatts, Flag 0x10), Grund-Drehung des Spender-Griffs vorn/hinten
(Door_model_set, erstes Bild), Ausschlag rot x des Archiv- und des Spendergriffs beim Oeffnen.

    python re15_port/tools/tueren/tuer_zuordnung_gen.py            # schreibt engine/src/gen/tuer_zuordnung.inc
    python re15_port/tools/tueren/tuer_zuordnung_gen.py --pruefen  # nur zaehlen/pruefen
"""
import argparse
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
AUS = os.path.join(PORT, "engine", "src", "gen", "tuer_zuordnung.inc")

sys.path.insert(0, os.path.join(PORT, "tools", "tor"))


def c_div(a, b):
    """C-Division (Richtung 0), wie op_door_aot_set rechnet."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def raum_id(name):
    return int(name[4:], 16)


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def griff_daten(archiv, spender):
    """[SIM] Griff-Mesh, Grund-Drehungen, Ausschlag - fuer das Paar Archiv <- Spender."""
    import tuerkatalog as tk

    def lauf(idx, v):
        return tk.VM(tk.Door(idx), variant=v, sound_ready_tick=0).run()

    def griffe(vm):
        """{obj: [(bild, pos, rot, mesh)]} fuer Kind-Objekte mit Mesh != 0 (Flag 0x10)."""
        out = {}
        for i, fr in enumerate(vm.frames):
            for o in fr:
                if o["parent"] >= 0 and o["mesh"] != 0:
                    out.setdefault(o["obj"], []).append((i, o["pos"], [r & 0xFFFF for r in o["rot"]], o["mesh"]))
        return out

    a0 = griffe(lauf(archiv, 0))
    s0 = griffe(lauf(spender, 0))
    s1 = griffe(lauf(spender, 1))
    vorn_a = [l for l in a0.values() if l[0][1][0] > 0][0]
    vorn_s = [l for l in s0.values() if l[0][1][0] > 0][0]
    hinten_s = [l for l in s1.values() if l[0][1][0] < 0][0]

    def ausschlag(lst):
        r0 = lst[0][2][0]
        best = 0
        for _, _, r, _ in lst:
            d = s16(r[0] - r0)
            if abs(d) > abs(best):
                best = d
        return best

    return {
        "mesh_archiv": vorn_a[0][3],
        "mesh_spender": vorn_s[0][3],
        "rot_vorn": vorn_s[0][2],
        "rot_hinten": hinten_s[0][2],
        "aus_archiv": ausschlag(vorn_a),
        "aus_spender": ausschlag(vorn_s),
        "anker_archiv": vorn_a[0][1],
        "anker_spender": vorn_s[0][1],
    }


def lesen():
    d = json.load(open(ZUORDNUNG, encoding="utf-8"))
    zeilen = []
    for t in d["tueren"]:
        for s in t["seiten"]:
            if not s.get("abgedeckt"):
                continue
            k = s["schluessel"]
            archiv = int(s["archiv"][4:], 16)
            spender = int(s["griff_tausch"][4:], 16) if s.get("griff_tausch") else 0xFF
            sz = sorted({(x["datei"], x["off"], x["slot"], x["skript"]) for x in k["saetze"]})
            for rn in k["raeume"]:
                z = {
                    "seite": s["id"], "tuer": t["id"], "raum": raum_id(rn), "band": k["band"],
                    "archiv": archiv, "variante": s["variante"], "bit7": 0, "spender": spender,
                    "herkunft": s.get("variante_herkunft") or "", "ziel": k.get("ziel"),
                    "saetze": [x for x in sz if x[0].startswith(rn)],
                }
                if k["form"] == "rechteck":
                    x, zz, w, dd = k["rect"]
                    assert w != 0 or dd != 0, "Null-Rechteck ist Skript-Uebergang: %s" % s["id"]
                    z.update(form=0, x=x + c_div(w, 2), z=zz + c_div(dd, 2), hw=abs(w) // 2, hh=abs(dd) // 2,
                             qx=[0, 0, 0, 0], qz=[0, 0, 0, 0], rect=k["rect"])
                    # zuordnung.json fuehrt die Mitte exakt (x + w/2 als Bruch), der Port mit C-Division
                    assert abs(z["x"] - k["mitte"][0]) <= 0.5 and abs(z["z"] - k["mitte"][1]) <= 0.5, (s["id"], z["x"], z["z"], k["mitte"])
                else:
                    pts = k["pts"]
                    z.update(form=1, x=0, z=0, hw=0, hh=0, qx=[p[0] for p in pts], qz=[p[1] for p in pts])
                zeilen.append(z)
    return zeilen


def pruefen(zeilen):
    schl = {}
    for z in zeilen:
        key = (z["raum"], z["form"], z["x"], z["z"], z["hw"], z["hh"], tuple(z["qx"]), tuple(z["qz"]), z["band"])
        if key in schl and schl[key] != (z["archiv"], z["variante"], z["spender"]):
            raise SystemExit("Schluessel doppelt mit anderer Wahl: %s / %s" % (z["seite"], key))
        schl[key] = (z["archiv"], z["variante"], z["spender"])
    seiten = sorted({z["seite"] for z in zeilen})
    return seiten


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pruefen", action="store_true")
    ap.add_argument("--aus", default=AUS)
    a = ap.parse_args()
    zeilen = lesen()
    seiten = pruefen(zeilen)
    archive = sorted({z["archiv"] for z in zeilen} | {z["spender"] for z in zeilen if z["spender"] != 0xFF})
    print("Seiten %d, Zeilen %d, Archive %s" % (len(seiten), len(zeilen), " ".join("%02X" % x for x in archive)))
    paare = sorted({(z["archiv"], z["spender"]) for z in zeilen if z["spender"] != 0xFF})
    griffe = {p: griff_daten(*p) for p in paare}
    for p, g in griffe.items():
        print("Griff-Tausch DOOR%02X <- DOOR%02X: %s" % (p[0], p[1], g))
    if a.pruefen:
        return
    zeilen.sort(key=lambda z: (z["raum"], z["seite"]))
    L = []
    L.append("/* Erzeugt von re15_port/tools/tueren/tuer_zuordnung_gen.py - NICHT von Hand aendern.")
    L.append(" * Quelle: analysis/befunde_runde31/tueren_03/zuordnung.json (Dossier tueren_03_zuordnung.md).")
    L.append(" * ⛔ PORT-WAHL aus dem Bildvergleich, KEINE Original-Adresse: RE1.5 waehlt kein Archiv")
    L.append(" * (Payload+12/+13 = 0 in 649 von 653 Door_aot_set); RE2-Angleichung (Beta -> Retail).")
    L.append(" * %d Tuerseiten, %d Zeilen (je Raumdatei xxx0/xxx1 eine). Schluessel = Raum + Flaeche + Band" % (len(seiten), len(zeilen)))
    L.append(" * (NICHT der Slot): Rechteck als Mitte/Halbmass wie op_door_aot_set, Viereck als Punkte pc+6..21.")
    L.append(" * Spalten: raum, form (0 Rechteck/1 Viereck), band, x, z, hw, hh, qx[4], qz[4], re2_nr,")
    L.append(" *          variante (var 12), bit7 (var 14), spender (Griff-Tausch, 0xFF = keiner), seite, tuer,")
    L.append(" *          off = RDT-Datei-Offset eines Door_aot_set dieser Seite in dieser Raumdatei. */")
    L.append("static const re15_tuer_zeile_t re15_tuer_zeilen[%d] = {" % len(zeilen))
    for z in zeilen:
        herk = z["herkunft"].replace("*/", "* /")
        ofs = ", ".join("%s@0x%X %s Slot %d" % (x[0], x[1], x[3], x[2]) for x in z["saetze"]) or "-"
        tausch = (" Griff-Tausch DOOR%02X" % z["spender"]) if z["spender"] != 0xFF else ""
        L.append("    /* %s %s ROOM%04X -> %s | %s | DOOR%02X V%d (%s)%s */" % (
            z["seite"], z["tuer"], z["raum"], z["ziel"], ofs, z["archiv"], z["variante"], herk, tausch))
        L.append("    { 0x%04X, %d, %d, %6d, %6d, %5d, %5d, {%d, %d, %d, %d}, {%d, %d, %d, %d}, 0x%02X, %d, %d, 0x%02X, %d, %d, 0x%05X }," % (
            z["raum"], z["form"], z["band"], z["x"], z["z"], z["hw"], z["hh"], *z["qx"], *z["qz"],
            z["archiv"], z["variante"], z["bit7"], z["spender"], int(z["seite"][1:]), int(z["tuer"][1:]),
            z["saetze"][0][1]))
    L.append("};")
    L.append("")
    L.append("/* Griff-Tausch (tueren_02_re2.md 2.5/3): Griff-Mesh des Spenders am Anhaengepunkt des Archivs,")
    L.append(" * mit Spender-Textur/CLUT. ⛔ PORT-WAHL: RE2 tauscht Griffe nur archivintern per Bit 7 (DOOR01/05).")
    L.append(" * Werte [SIM] (tools/tor/tuerkatalog.py): mesh_archiv = Griff-Mesh des Archivs (Kind des Blatts),")
    L.append(" * mesh_spender = Griff-Mesh des Spenders, rot_vorn/rot_hinten = Grund-Drehung des Spendergriffs")
    L.append(" * (x > 0 vorn, x < 0 hinten; Door_model_set im ersten Bild), aus_archiv/aus_spender = groesster")
    L.append(" * Ausschlag rot x beim Oeffnen (Knauf dreht, Druecker kippt, Stange steht). */")
    L.append("static const re15_griff_tausch_t re15_griff_tausche[%d] = {" % max(1, len(griffe)))
    for (ar, sp), g in sorted(griffe.items()):
        L.append("    /* DOOR%02X <- DOOR%02X: Anker Archiv %s, Anker Spender %s */" % (ar, sp, g["anker_archiv"], g["anker_spender"]))
        L.append("    { 0x%02X, 0x%02X, %d, %d, {%d, %d, %d}, {%d, %d, %d}, %d, %d }," % (
            ar, sp, g["mesh_archiv"], g["mesh_spender"], *g["rot_vorn"], *g["rot_hinten"], g["aus_archiv"], g["aus_spender"]))
    L.append("};")
    os.makedirs(os.path.dirname(a.aus), exist_ok=True)
    with open(a.aus, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(L) + "\n")
    print("geschrieben:", a.aus)


if __name__ == "__main__":
    main()
