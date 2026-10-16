#!/usr/bin/env python3
"""fahrstuhl_archiv_bauen.py - Runde 37 Spur C "fahrstuhl": Port-Archive der Revier-Fahrstuhltuer ROOM1080.

Auftrag (analysis/befunde_runde37/AUFTRAG.md #6): "Die Fahrstuhl Tuer in der Tuer Animation ist noch nicht korrekt
... wenn es keine passende Fahrstuhl Tuer gibt, muessen wir die wieder bauen". Keine der 55 RE2-Tueren ist die
glatte graue zweifluegelige Revier-Fahrstuhltuer (Bogen C_belege/c0_re2_alle_tuerarchive.jpg). Deshalb zwei
Port-Archive im RE2-Aufbau:

  P27F  Flurseite  (Einstieg ROOM1040/10C0/1120 -> Kabine, Seiten S013/S040/S057; gemalt ROOM10C00/ROOM11200)
  P27C  Kabinenseite (Ausstieg Kabine ROOM1080 -> Etage, Seiten S027/S028/S029; gemalt ROOM10800 Cut 0)

Aufbau je Archiv (Basis = RE2 DOOR27 = die RE2-FAHRSTUHLtuer, RE2 ROOM6010 @0x004C2/@0x004E2/@0x00502 V0):
  * Tonteil  BYTEGLEICH DOOR27 (Schiebe-Ton der RE2-Fahrstuhltuer, Se_on Skript 1 @+0x44 nach Bild 70).
  * SCD      BYTEGLEICH DOOR27 (10 Skripte; Skript 1 @Datei 0x6040: obj 0 mesh 0 bei z -3000, obj 1 mesh 1 bei
             z +3000, Skript 2/3 schieben die Fluegel gegenlaeufig in z, 2240 je Fluegel - Fahrstuhl-Mechanik RE2).
  * MD1 NEU  (md1_eigen = 2): Mesh 0/1 = glatte Fluegel mit GERADER Mittelfuge statt der verzahnten Labor-Fluegel;
             Mesh 2 = DOOR27 Mesh 2 bytegleich (von keinem Skript aufgestellt: einziger Door_model_set-Satz ist
             Skript 1, obj 0 mesh 0 / obj 1 mesh 1).
  * TIM NEU  128x256/8 bit/1 CLUT wie DOOR27: gemaltes Fluegelfeld (gemalt_fluegel.py) * 128/c, c = 73.

MASSE (PORT-WAHL, Herkunft je Zahl):
  FLUEGEL_B 1860 = 2000 / 2 * 1,86: Tueroeffnung 2000 Raumeinheiten = Door_aot_set-Breite ROOM10C0.RDT @0x00E82 /
            ROOM1120.RDT @0x00CB6 (w 0x07D0); die Runde-31-Verfeinerung am gemalten Bild fand 2150 (S040) und
            1850 (S057), Mittel 2000. Massstab Raum -> Tuerszene 1,86 = 6602 / 3549 (Runde 31 06_massstab 1:
            Blatthoehe RE2 6602 Szene = 3549 Raum).
  FLUEGEL_H 6600 = Blatthoehe des RE2-Standardblatts (DOOR07/DOOR2A Mesh 0 y -6600..2).
  OBJ_Z     3000 = Lage obj 0 / obj 1 in DOOR27 Skript 1 (Door_model_set pos z -3000 / +3000, Bytes 48 f4 / b8 0b):
            Fluegel 0 liegt lokal z 1140..3000 (Welt -1860..0), Fluegel 1 lokal -3000..-1140 (Welt 0..1860) - die
            Fuge liegt genau in der Mitte, beim Oeffnen faehrt jeder Fluegel 2240 > 1860 ganz aus der Oeffnung.
  DICKE_HALB 100 (Stahlblech-Fluegel; DOOR27 +-245/+-300, Standardblatt +-145) - nur die Kante beim Aufschieben.
  Teilung der Vorderseite 4 x 8 Felder: RE2 zeichnet ohne Flag 0x20 (DOOR27 obj-Flags 0x0a80/0x0280) ungeteilt,
            die Textur wird affin interpoliert -> kleine Dreiecke wie in den RE2-Modellen (DOOR27 Mesh 0: 180).

TEXTUR (PORT-WAHL, Rechenweg wie Runde 33 rezept_glattes_blech / Runde 36 P0CD "gezeigt = gemalt"):
  Fluegelflaeche u 0..119 (u 0 = Aussenkante, u 119 = Fuge; Fluegel 1 gespiegelt gemappt), v 0..255 oben -> unten:
    gemaltes Feld (60 x 128, Mittel der entzerrten Fluegel) bikubisch auf 120 x 256, Gauss 1,5 Texel,
    * 128 / 73 (Texel = gemalt * 128 / c, c = NCCT der Vorderseite, tuer_archiv_bauen.C_BLATT),
    * (1 + 0,5 * Korn): Feinkorn des RE2-Stahlblatts DOOR07 (tuer_archiv_bauen.blech_korn, KORN_GLATT 0,5;
      Schlitze und Druecker-Schild geflickt wie Runde 33).
  Fuge: Spalte 119 * 0,75 (gemalte Fuge = Helligkeitsminimum: ROOM11200 Zeile 60 29 gegen 33..40 daneben,
        ROOM10C00 Zeile 50 21..24 gegen 30..36 -> 0,7..0,8).
  Kantenstreifen u 120..127 (Stirnseiten, Ober-/Unterkante, Rueckseite): Median des Feldes * 0,6 (dunkle Kante).

Aufruf:  python re15_port/tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py [--pruefen]
Eingabe: analysis/befunde_runde37/C_belege/feld/feld_flur.png, feld_kabine.png (gemalt_fluegel.py)
Ausgabe: re15_port/shared_assets/RE15DOOR/P27F.DO2, P27C.DO2; engine/src/gen/re15_tuer_eigen.inc (Eintraege 31/32
         + Zeilen S013/S040/S057 -> P27F); analysis/befunde_runde33/tueren_rest/plan.json (T011/T017/T018 archiv
         P27F); Protokoll analysis/befunde_runde37/C_belege/archiv_protokoll.json; Vorschau build/r37_fahrstuhl/.
"""
import argparse
import hashlib
import json
import os
import re
import struct
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tueren"))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
import do2_format as fmt              # noqa: E402
import tuer_archiv_bauen as tab       # noqa: E402

FELD_DIR = os.path.join(REPO, "analysis", "befunde_runde37", "C_belege", "feld")
PROTOKOLL = os.path.join(REPO, "analysis", "befunde_runde37", "C_belege", "archiv_protokoll.json")
VORSCHAU = os.path.join(REPO, "build", "r37_fahrstuhl")

MASSSTAB = 6602.0 / 3549.0            # Runde 31 06_massstab 1 (Szene / Raum)
OEFFNUNG_RAUM = 2000                  # Door_aot_set w 0x07D0 ROOM10C0 @0x00E82 / ROOM1120 @0x00CB6
FLUEGEL_B = int(round(OEFFNUNG_RAUM * MASSSTAB / 2.0 / 10.0)) * 10   # 1860
FLUEGEL_H = 6600                      # RE2-Standardblatt (DOOR07/DOOR2A Mesh 0 y -6600..2)
OBJ_Z = 3000                          # DOOR27 Skript 1: obj 0 pos z -3000 / obj 1 pos z +3000
DICKE_HALB = 100                      # PORT-WAHL
TEIL_Z, TEIL_Y = 4, 8                 # Teilung der Vorderseite
U_FLUEGEL = 119                       # u 0..119 Fluegelflaeche
U_KANTE = (120, 127)                  # Kantenstreifen
FUGE_FAKTOR = 0.75                    # gemalte Fuge 0,7..0,8 (Kopf)
KANTE_FAKTOR = 0.6                    # PORT-WAHL dunkle Stirnseite
KORN_GLATT = tab.KORN_GLATT           # 0,5 (Runde 33 "glattes Blatt")

ARCHIVE = (
    ("P27F", "feld_flur.png", "Flurseite: gemalte Fluegel ROOM10C00 + ROOM11200 (je zwei, entzerrt, gemittelt)"),
    ("P27C", "feld_kabine.png", "Kabinenseite: gemalte Fluegel ROOM10800 Cut 0 (zwei, entzerrt, gemittelt)"),
)


# ------------------------------------------------------------------------------------------------
# Netz
# ------------------------------------------------------------------------------------------------
def _sichtnormale(p0, p1, p2):
    a = [p2[i] - p0[i] for i in range(3)]
    b = [p1[i] - p0[i] for i in range(3)]
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def _dreieck(e0, e1, e2, richtung):
    """Ecken so ordnen, dass die SICHTBARE Seite (do2_format._normale: (p2-p0) x (p1-p0), NCLIP >= 0) in
    `richtung` zeigt."""
    n = _sichtnormale(e0[0], e1[0], e2[0])
    if sum(n[i] * richtung[i] for i in range(3)) < 0:
        e1, e2 = e2, e1
    return (e0, e1, e2)


def _rechteck(ecken, uvs, richtung):
    """ecken = (A, B, C, D) im Umlauf, uvs dazu -> zwei Dreiecke mit Sichtseite `richtung`."""
    (A, B, C, D), (a, b, c, d) = ecken, uvs
    return [_dreieck((A, a), (B, b), (D, d), richtung), _dreieck((B, b), (C, c), (D, d), richtung)]


def fluegel_mesh(z_aussen, z_fuge):
    """Fluegel lokal: x -DICKE_HALB..+DICKE_HALB (Vorderseite +x zur Tuerkamera, Auge (10000,0,0)),
    y -FLUEGEL_H..0, z von der Aussenkante z_aussen bis zur Fuge z_fuge."""
    T, H = DICKE_HALB, FLUEGEL_H
    tris = []
    zs = [z_aussen + (z_fuge - z_aussen) * i / TEIL_Z for i in range(TEIL_Z + 1)]
    ys = [-H + H * j / TEIL_Y for j in range(TEIL_Y + 1)]
    us = [int(round(U_FLUEGEL * i / TEIL_Z)) for i in range(TEIL_Z + 1)]
    vs = [int(round(255 * j / TEIL_Y)) for j in range(TEIL_Y + 1)]
    for i in range(TEIL_Z):
        for j in range(TEIL_Y):
            ecken = ((T, ys[j], zs[i]), (T, ys[j], zs[i + 1]), (T, ys[j + 1], zs[i + 1]), (T, ys[j + 1], zs[i]))
            uv = ((us[i], vs[j]), (us[i + 1], vs[j]), (us[i + 1], vs[j + 1]), (us[i], vs[j + 1]))
            tris += _rechteck(ecken, uv, (1, 0, 0))
    k0, k1 = U_KANTE
    kuv = ((k0, 0), (k1, 0), (k1, 255), (k0, 255))
    sz_fuge = 1 if z_fuge > z_aussen else -1
    # Stirnseite an der Fuge (zeigt zum anderen Fluegel), Stirnseite aussen, Ober-/Unterkante, Rueckseite
    tris += _rechteck(((T, -H, z_fuge), (-T, -H, z_fuge), (-T, 0, z_fuge), (T, 0, z_fuge)), kuv, (0, 0, sz_fuge))
    tris += _rechteck(((T, -H, z_aussen), (-T, -H, z_aussen), (-T, 0, z_aussen), (T, 0, z_aussen)), kuv,
                      (0, 0, -sz_fuge))
    tris += _rechteck(((T, -H, z_aussen), (T, -H, z_fuge), (-T, -H, z_fuge), (-T, -H, z_aussen)), kuv, (0, -1, 0))
    tris += _rechteck(((T, 0, z_aussen), (T, 0, z_fuge), (-T, 0, z_fuge), (-T, 0, z_aussen)), kuv, (0, 1, 0))
    tris += _rechteck(((-T, -H, z_aussen), (-T, -H, z_fuge), (-T, 0, z_fuge), (-T, 0, z_aussen)), kuv, (-1, 0, 0))
    m = fmt.mesh_aus_dreiecken([[(tuple(int(round(c)) for c in p), uv) for (p, uv) in t] for t in tris])
    return m


def md1_bauen(basis_md1):
    m0 = fluegel_mesh(OBJ_Z - FLUEGEL_B, OBJ_Z)          # obj 0 bei z -3000: Welt -1860..0
    m1 = fluegel_mesh(-(OBJ_Z - FLUEGEL_B), -OBJ_Z)      # obj 1 bei z +3000: Welt  1860..0
    md1 = fmt.md1_bauen([m0, m1, basis_md1.meshes[2]])
    return md1, dict(fluegel_b=FLUEGEL_B, fluegel_h=FLUEGEL_H, dicke=2 * DICKE_HALB,
                     dreiecke=[len(m.tris) for m in md1.meshes], ecken=[len(m.vertices) for m in md1.meshes],
                     mesh2="DOOR27 Mesh 2 bytegleich (unbenutzt)")


# ------------------------------------------------------------------------------------------------
# Textur
# ------------------------------------------------------------------------------------------------
def _hoch(feld, w, h):
    """float-Feld (H, W, 3) bikubisch auf (h, w) + Gauss 1,5 (gegen die Blockstufen des Vergroesserns)."""
    kan = []
    for c in range(3):
        im = Image.fromarray(feld[..., c].astype(np.float32)).resize((w, h), Image.BICUBIC)
        kan.append(np.asarray(im, np.float64))
    return tab.blur(np.stack(kan, -1), 1.5)


def korn_256():
    """Feinkorn des RE2-Stahlblatts DOOR07 (218 x 128, Runde 33) auf 256 x 120: Zeilen 0..217, dann 0..37.
    Geflickt wie Runde 33: Lueftungsschlitze (D07_SCHLITZE) UND der gemalte Druecker-Schild-Umriss
    (D07_SCHILD_FLICKEN) - die Fahrstuhltuer hat keinen Druecker."""
    _, _, _, tim07 = tab.re2_lesen("DOOR07")
    rgba07, _ = tab.rgba_aus_tim(tim07)
    korn, _ = tab.blech_korn(rgba07, (tab.D07_SCHLITZE, tab.D07_SCHILD_FLICKEN))
    korn = korn[:, :U_FLUEGEL + 1]
    return np.concatenate([korn, korn[:256 - korn.shape[0]]], 0)


def textur_bauen(feld_png, korn):
    feld = np.asarray(Image.open(os.path.join(FELD_DIR, feld_png)).convert("RGB"), np.float64)
    gemalt = _hoch(feld, U_FLUEGEL + 1, 256)
    texel, gekappt = tab.texel_fuers_tuerlicht(gemalt)
    texel = np.clip(texel * (1.0 + KORN_GLATT * korn)[..., None], 0, 255)
    texel[:, U_FLUEGEL] *= FUGE_FAKTOR
    rgba = np.zeros((256, 128, 4), np.float64)
    rgba[:, :U_FLUEGEL + 1, :3] = texel
    med = np.median(feld.reshape(-1, 3), 0)
    rgba[:, U_KANTE[0]:, :3] = np.clip(med * KANTE_FAKTOR * 128.0 / tab.C_BLATT, 0, 255)
    rgba[..., 3] = 255
    proto = dict(feld=feld_png, gemalt_median=[round(float(x), 1) for x in med],
                 gemalt_mittel=[round(float(x), 1) for x in gemalt.reshape(-1, 3).mean(0)],
                 texel_mittel=[round(float(x), 1) for x in texel.reshape(-1, 3).mean(0)],
                 gezeigt_soll=[round(float(x), 1) for x in (texel * tab.C_BLATT / 128.0).reshape(-1, 3).mean(0)],
                 c_blatt=tab.C_BLATT, korn=KORN_GLATT, fuge=FUGE_FAKTOR, kante=KANTE_FAKTOR, gekappt=gekappt)
    return np.round(rgba).astype(np.uint8), proto


# ------------------------------------------------------------------------------------------------
# Archiv
# ------------------------------------------------------------------------------------------------
def archiv_bauen(kennung, feld_png, md1, korn, schreiben):
    d_basis, o, md1_basis, tim = tab.re2_lesen("DOOR27")
    _, wert = tab.rgba_aus_tim(tim)
    rgba, proto = textur_bauen(feld_png, korn)
    md1_b = md1.schreiben()
    benutzt = tab.uv_maske(fmt.Md1.lesen(md1_b), range(3))
    neu_tim, qinfo = tab.tim_bauen(rgba, tim, wert, np.zeros((256, 128), bool), ~benutzt)
    tim_b = neu_tim.schreiben()
    if len(tim_b) != len(o.tim):
        raise SystemExit("%s: TIM %d B statt %d" % (kennung, len(tim_b), len(o.tim)))
    neu = fmt.Do2("re2", md1=md1_b, skripte=list(o.skripte), tim=tim_b, ton_vorspann=o.ton_vorspann,
                  vh=o.vh, ton_nachspann=o.ton_nachspann, vb=o.vb)
    datei = neu.schreiben()
    bef = fmt.lader_pruefung(datei)
    if not bef["lesbar"]:
        raise SystemExit("%s: Lader-Pruefung %s" % (kennung, bef["fehler"]))
    rueck = fmt.Do2.lesen(datei)
    la, ln = o.lage(), rueck.lage()
    assert datei[:la["ton_ende"]] == d_basis[:la["ton_ende"]], "%s: Tonteil nicht bytegleich" % kennung
    assert datei[ln["scd"]:ln["md1"]] == d_basis[la["scd"]:la["md1"]], "%s: SCD nicht bytegleich" % kennung
    assert rueck.md1 == md1_b and rueck.tim == tim_b
    m_rueck = fmt.Md1.lesen(rueck.md1)
    assert m_rueck.meshes[2].vertices == md1_basis.meshes[2].vertices, "Mesh 2 nicht DOOR27"
    t2, _ = fmt.Tim.lesen(rueck.tim)
    assert (t2.farbtiefe, t2.breite_px, t2.hoehe_px) == (8, 128, 256)
    te = neu.re2_tabelleneintrag()
    e = dict(kennung=kennung, basis="DOOR27", basis_nr=0x27, ton=te["ton_groesse"], modell=te["modell_groesse"],
             sektor=te["modell_sektor"], datei=len(datei), fnv=tab.fnv1a(datei), sha1=hashlib.sha1(datei).hexdigest(),
             basis_sha1=hashlib.sha1(d_basis).hexdigest(), md1_eigen=2, textur=proto, palette=qinfo)
    if schreiben:
        with open(os.path.join(tab.AUS_DIR, kennung + ".DO2"), "wb") as f:
            f.write(datei)
        os.makedirs(VORSCHAU, exist_ok=True)
        vor, _ = tab.rgba_aus_tim(t2)
        Image.fromarray(vor[..., :3]).resize((128 * 3, 256 * 3), Image.NEAREST).save(
            os.path.join(VORSCHAU, kennung + "_tim.png"))
    return e


# ------------------------------------------------------------------------------------------------
# gen/re15_tuer_eigen.inc + plan.json (gezielt, Muster tools/tueren/r36_tueren_bauen.py)
# ------------------------------------------------------------------------------------------------
SEITEN_P27F = (13, 40, 57)
HERKUNFT = ("Runde 37 Spur C: Port-Archiv P27F (Basis DOOR27 = RE2-Fahrstuhltuer, RE2 ROOM6010 @0x004C2 V0; "
            "MD1 neu: zwei glatte Fluegel mit gerader Fuge wie RE1.5 ROOM10C00/ROOM11200/ROOM10800)")
KOPF_R37 = (" * Runde 37 Spur C (tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py, analysis/befunde_runde37/C_fahrstuhl.md):\n"
            " * Eintraege 31 P27F / 32 P27C (Revier-Fahrstuhl, Basis DOOR27, MD1 NEU = md1_eigen 2) und die Zeilen\n"
            " * S013/S040/S057 -> P27F gezielt nachgefuehrt. P27C spielen die Kabinen-Ausstiege (fahrstuhl_r37.c).")


def inc_nachfuehren(text, eintraege):
    m = re.search(r"static const re15_tuer_eigen_t re15_tuer_eigen\[(\d+)\] = \{\n", text)
    if not m:
        raise SystemExit("re15_tuer_eigen[] nicht gefunden")
    kennungen = re.findall(r'^    \{ "(\w+)", 0x', text, re.M)
    basis_zahl = len([k for k in kennungen if k not in ("P27F", "P27C")])
    ende = text.index("\n};\n", m.end())
    block = text[m.end():ende]
    zeilen = [l for l in block.split("\n") if l]
    rein = []
    for l in zeilen:
        if '"P27F"' in l or '"P27C"' in l or re.match(r"    /\* 3[12]: P27[FC] ", l):
            continue
        rein.append(l)
    for i, e in enumerate(eintraege):
        nr = basis_zahl + i + 1
        rein.append("    /* %d: %s = DOOR27 + MD1 NEU (zwei glatte Fluegel, gerade Fuge) + Textur '%s' (Ton eigen DOOR27 "
                    "(RE2-Fahrstuhltuer, bytegleich)) */" % (nr, e["kennung"], e["textur_text"]))
        rein.append("    { \"%s\", 0x27, %d, %d, %d, %d, 0x%08Xu, 2 }," % (e["kennung"], e["ton"], e["modell"], e["sektor"],
                                                                       e["datei"], e["fnv"]))
    n = basis_zahl + len(eintraege)
    text = text[:m.start()] + "static const re15_tuer_eigen_t re15_tuer_eigen[%d] = {\n" % n + "\n".join(rein) + text[ende:]
    idx = {e["kennung"]: basis_zahl + i + 1 for i, e in enumerate(eintraege)}
    # Zeilen S013/S040/S057 (xxx0 + xxx1): Basis 0x27, V0, eigen = P27F
    tl = text.split("\n")
    ersetzt = 0
    for i, l in enumerate(tl):
        mm = re.match(r"^    /\* S(\d{3}) (T\d{3}) ROOM([0-9A-F]{4}) -> (ROOM[0-9A-F]{4}) \| ([^|]+)\| ", l)
        if not mm or int(mm.group(1)) not in SEITEN_P27F:
            continue
        tl[i] = "    /* S%s %s ROOM%s -> %s | %s| P27F (Basis DOOR27) V0 (%s) */" % (
            mm.group(1), mm.group(2), mm.group(3), mm.group(4), mm.group(5), HERKUNFT)
        d = tl[i + 1]
        dm = re.match(r"^(    \{ 0x%s, .*, 0x27, 0, 0, 0xFF, %d, \d+, 0x[0-9A-F]+, )(\d+)( \},)$" % (mm.group(3),
                      int(mm.group(1))), d)
        if not dm:
            raise SystemExit("Datenzeile S%s ROOM%s unerwartet: %s" % (mm.group(1), mm.group(3), d))
        tl[i + 1] = dm.group(1) + str(idx["P27F"]) + dm.group(3)
        ersetzt += 1
    if ersetzt != 6:
        raise SystemExit("erwartet 6 Zeilen (S013/S040/S057 x xxx0/xxx1), ersetzt %d" % ersetzt)
    text = "\n".join(tl)
    if KOPF_R37.split("\n")[0] not in text:
        marke = " * Textur) gezielt nachgefuehrt; plan.json traegt dieselbe Wahl (archiv \"RE2\")."
        if marke not in text:
            raise SystemExit("Kopf-Marke der Runde 36 fehlt")
        text = text.replace(marke, marke + "\n" + KOPF_R37, 1)
    return text, idx


def plan_nachfuehren(plan, eintraege):
    for tid in ("T011", "T017", "T018"):
        e = plan["tueren"][tid]
        e["archiv"] = "P27F"
        e["basis"] = "DOOR27"
        e["runde37"] = ("Fahrstuhl ROOM1080: vorher DOOR27 V0 unveraendert (Runde 36, verzahnte Labortuer) - jetzt Port-"
                        "Archiv P27F mit NEUEM MD1, gebaut von tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py (nicht ueber "
                        "tuer_archiv_bauen: dessen Pruefung verlangt MD1 = Basis-Meshes als Anfang)")
        for sp in e["seiten"]:
            sp["variante"] = 0
            sp["herkunft"] = HERKUNFT
    for e in eintraege:
        plan["archive"][e["kennung"]] = dict(basis="DOOR27", rezept=e["textur_text"], ton="eigen DOOR27",
                                             griff="keiner", bau="tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py",
                                             md1_eigen=2)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pruefen", action="store_true", help="nichts schreiben")
    a = ap.parse_args()
    _, o, md1_basis, _ = tab.re2_lesen("DOOR27")
    md1, minfo = md1_bauen(md1_basis)
    print("MD1:", minfo)
    korn = korn_256()
    eintraege = []
    for kennung, feld, text in ARCHIVE:
        e = archiv_bauen(kennung, feld, md1, korn, not a.pruefen)
        e["textur_text"] = text
        e["md1"] = minfo
        eintraege.append(e)
        print("%s: %d B (Ton %d, Modell %d, Sektor %d), fnv %08X, Palette %d, gemalt %s -> Texel %s (gezeigt %s)" % (
            kennung, e["datei"], e["ton"], e["modell"], e["sektor"], e["fnv"], e["palette"]["palette_belegt"],
            e["textur"]["gemalt_mittel"], e["textur"]["texel_mittel"], e["textur"]["gezeigt_soll"]))
    text = open(tab.AUS_INC, encoding="utf-8").read()
    text, idx = inc_nachfuehren(text, eintraege)
    print("eigen-Index:", idx)
    plan = json.load(open(tab.PLAN, encoding="utf-8"))
    plan_nachfuehren(plan, eintraege)
    if a.pruefen:
        print("(--pruefen: nichts geschrieben)")
        return 0
    with open(tab.AUS_INC, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    with open(tab.PLAN, "w", encoding="utf-8", newline="\n") as f:
        json.dump(plan, f, ensure_ascii=False, indent=1)
        f.write("\n")
    with open(PROTOKOLL, "w", encoding="utf-8", newline="\n") as f:
        json.dump(dict(archive=eintraege, eigen_index=idx), f, ensure_ascii=False, indent=1)
    print("geschrieben:", tab.AUS_INC, tab.PLAN, PROTOKOLL)
    return 0


if __name__ == "__main__":
    sys.exit(main())
