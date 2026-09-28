#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_bericht.py - Markdown-Tabellen fuer analysis/befunde_runde31/tueren_02_re2.md aus den JSONs von
re2_tuer_varianten.py (varianten.json), re2_tuer_ton.py (ton.json) und re2_tuer_zensus.py (re2_tueren.json).

Liest/Schreibt NUR build/r31_tueren/t2/ (bericht_*.md). Keine eigene Messung - nur Umformatierung.
Aufruf: python re15_port/tools/tueren/re2_tuer_bericht.py
"""
import hashlib
import json
import os
import sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
T2 = os.path.join(REPO, "build", "r31_tueren", "t2")
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "tor"))
import tuerkatalog as tk  # noqa: E402

GRIFF_FORM = {
    # [BILD] build/r31_tueren/t2/bogen_griffe.png, selbst angesehen; Mesh-Gleichheit aus re2_tuer_varianten (GEO+UV)
    ("DOOR00", 1): "Knauf A", ("DOOR03", 1): "Knauf A", ("DOOR05", 1): "Knauf A", ("DOOR0D", 1): "Knauf A",
    ("DOOR18", 1): "Knauf A", ("DOOR01", 3): "Knauf A",
    ("DOOR02", 1): "Knauf B (Schild)", ("DOOR08", 1): "Knauf B (Schild)", ("DOOR09", 1): "Knauf B (Schild)",
    ("DOOR11", 1): "Knauf B (Langschild)", ("DOOR13", 1): "Knauf B (Schild)", ("DOOR1C", 1): "Knauf B (oval)",
    ("DOOR01", 1): "Druecker", ("DOOR01", 2): "Druecker (Rueckseite)", ("DOOR05", 2): "Druecker",
    ("DOOR05", 3): "Druecker (Rueckseite)",
    ("DOOR07", 1): "Druecker flach", ("DOOR1D", 1): "Druecker flach (Schild)",
    ("DOOR1A", 1): "Druecker + Kartenleser", ("DOOR24", 1): "Druecker schraeg",
    ("DOOR04", 1): "Stangengriff lang", ("DOOR06", 1): "Buegelgriff", ("DOOR0B", 1): "Stangengriff",
    ("DOOR0C", 1): "Stangengriff", ("DOOR2F", 1): "Stangengriff",
    ("DOOR1B", 1): "Druckstange quer", ("DOOR23", 1): "Riegelstange quer",
    ("DOOR17", 1): "Ring", ("DOOR0A", 1): "Riegelkasten", ("DOOR2E", 1): "Riegelkasten",
    ("DOOR26", 3): "Handrad", ("DOOR31", 3): "Handrad",
    ("DOOR19", 1): "Griffmulde", ("DOOR19", 3): "Griffmulde (Rueckseite)", ("DOOR2C", 2): "Griffleiste",
    ("DOOR14", 1): "Griffplatte", ("DOOR29", 1): "Bedientafel", ("DOOR29", 2): "Bedientafel (Rueckseite)",
    ("DOOR15", 1): "Beschlag (Strebe)",
    ("DOOR1F", 1): "zweite Platte (faehrt)", ("DOOR1F", 2): "Tafel", ("DOOR1F", 3): "Schild", ("DOOR29", 3): "Schild",
}


def main():
    V = json.load(open(os.path.join(T2, "varianten.json")))
    TON = json.load(open(os.path.join(T2, "ton.json")))
    ton = {r["archiv"]: r for r in TON["archive"]}
    out = []
    out.append("| Archiv | V | Skr | Bilder | Bewegung | Richtung | Angel x (Seite) | freie Kante | Griff: Mesh, x, Seite (vorn) | Objekte (Rolle:Mesh) | Se_on Bild | Schliesston |")
    out.append("|---|---|---|---|---|---|---|---|---|---|---|---|")
    for name, r in V.items():
        for v in r["varianten"]:
            gr = [g for g in v.get("griffe", []) if g["sichtbar"] == "vorn"]
            gtxt = "; ".join("m%d %.0f %s" % (g["mesh"], g["bild_x"], g["seite"]) for g in gr) or "-"
            objs = " ".join("%s:%d" % ({"Blatt": "B", "Blatt fest": "Bf", "Griff/Beschlag": "G",
                                         "Anbauteil (Kind)": "Ak", "Anbauteil (Wurzel)": "Aw"}.get(o["rolle"], "?"),
                                        o["mesh"]) for o in v["objekte"])
            se = ",".join(str(s["tick"]) for s in v.get("toene", [])) or "-"
            out.append("| %s | %d%s | %s | %d | %s | %s | %s | %s | %s | %s | %s | %s |" % (
                name, v["variante"], " b7" if v.get("bit7") else "", v.get("aufbau_skript"), v["bilder"],
                v.get("bewegung", "-"), v.get("richtung", "-"),
                ("%.0f (%s)" % (v["angel_bild_x"], v["angel_seite"])) if v.get("angel_bild_x") is not None else "-",
                v.get("freie_kante_seite", "-"), gtxt, objs, se, "ja" if v.get("schliesston") else "nein"))
    open(os.path.join(T2, "bericht_varianten.md"), "w", encoding="utf-8").write("\n".join(out) + "\n")

    # Griff-Katalog
    g = ["| Archiv | Mesh | Dreiecke | Form | Groesse x,y,z | Anhaengepunkt im Blatt | Drehung beim Oeffnen (rot x) | UV v | gleich (GEO+UV) wie |",
         "|---|---|---|---|---|---|---|---|---|"]
    fam = defaultdict(list)
    uv, hsh = {}, {}
    for i in range(tk.N_DOORS):
        d = tk.Door(i)
        for m in d.meshes:
            uv[(d.name, m["index"])] = "%d..%d" % (m["uv_min"][1], m["uv_max"][1])
            hsh[(d.name, m["index"])] = hashlib.md5(repr((m["verts"], [(t["v"], t["uv"]) for t in m["tris"]])).encode()).hexdigest()
    gleich = defaultdict(list)
    for k, h in hsh.items():
        gleich[h].append("%s.m%d" % k)
    for name, r in V.items():
        seen = set()
        for v in r["varianten"]:
            for gg in v.get("griffe", []):
                if gg["mesh"] in seen:
                    continue
                seen.add(gg["mesh"])
                m = r["meshes"][gg["mesh"]]
                form = GRIFF_FORM.get((name, gg["mesh"]), "?")
                fam[(m["dreiecke"], tuple(m["groesse"]))].append("%s.m%d" % (name, gg["mesh"]))
                andere = [x for x in gleich[hsh[(name, gg["mesh"])]] if x != "%s.m%d" % (name, gg["mesh"])]
                g.append("| %s | %d | %d | %s | %s | %s | %d | %s | %s |" % (
                    name, gg["mesh"], m["dreiecke"], form, m["groesse"], gg["lage_im_blatt"], gg["dreht"][0],
                    uv[(name, gg["mesh"])], ", ".join(andere) or "-"))
    open(os.path.join(T2, "bericht_griffe.md"), "w", encoding="utf-8").write("\n".join(g) + "\n")

    # Ton
    t = ["| Archiv | Tonteil B | Ton 0: VAG, Dauer s (Bilder) | Ton 1: VAG, Dauer s | Se_on je Variante (Satz@Bild) | Door_exit spielt Ton 1 | Familie |",
         "|---|---|---|---|---|---|---|"]
    famidx = {}
    for i, f in enumerate(TON["familien_tonteil"]):
        for a in f:
            famidx[a] = ("F%d" % (i + 1)) if len(f) > 1 else "-"
    for name in V:
        r = ton[name]
        e0, e1 = r["eintraege"][0], r["eintraege"][1]
        def ex(e):
            if e["leer"] or not e["vag_bytes"]:
                return "stumm (VAG %d leer)" % e.get("vag", -1)
            return "VAG %d %s, %.2f (%.0f)" % (e["vag"], e["vag_sha1"][:6], e["dauer_s"], e["dauer_bilder"])
        vs = "; ".join("V%d %s" % (v["variante"], ",".join("S%d@%d" % (s["satz"], s["bild"]) for s in v["se_on"]) or "-")
                       for v in r["varianten"])
        zu = sorted({v["schliesston_door_exit"] for v in r["varianten"]})
        t.append("| %s | %d | %s | %s | %s | %s | %s |" % (name, r["tonteil"], ex(e0), ex(e1), vs,
                                                          "/".join("ja" if z else "nein" for z in zu), famidx[name]))
    open(os.path.join(T2, "bericht_ton.md"), "w", encoding="utf-8").write("\n".join(t) + "\n")
    print("geschrieben: bericht_varianten.md (%d Zeilen), bericht_griffe.md, bericht_ton.md" % (len(out) - 2))


if __name__ == "__main__":
    main()
