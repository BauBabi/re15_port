#!/usr/bin/env python3
"""r36_rezept_p0cd.py - Runde 36 Spur G (Punkt 2): neue Textur fuer das Port-Archiv P0CD (Tuer T025 ROOM10C0 <-> ROOM10D0).

Nutzer: "Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich zum
Rest." Ursache (gemessen, analysis/befunde_runde36/G_tueren.md G2): das Runde-33-Rezept (tuer_rezepte.rezept_P0CD)
zog 68x80 Texel DOOR0C-Holz BIKUBISCH auf 128x256 hoch und legte eine EINZIGE Flaechenfarbe darunter -> Flecken statt
Faser, kein Licht, kein Relief; Fenster/Gitter/Stange als flache Rechtecke.

Dieses Rezept (PORT-WAHL, Textur ist Bauentscheidung; belegt sind Lage und Farbe aus dem RE1.5-Hintergrund):
  * FASER aus RE2 bei 1:1-Schaerfe: die drei Fuellungen von DOOR0D (helles Holz, u 23..102 x v 18..60 / 89..131 /
    160..202, gemessen: dort gx > gy = senkrechte Faser) als relatives Feinkorn L/Tiefpass(L)-1, um 90 Grad gedreht
    (RE1.5 malt WAAGERECHTE Faser, ROOM10C04) und NUR entlang der Faser gestreckt (quer zur Faser bleibt 1 Texel =
    1 Texel), die drei Stuecke uebereinander (Fugen laufen MIT der Faser).
  * FARBE + LICHT = das gemalte Blatt: Zeilenprofil des linken Fluegels in ROOM10C04 (Port-MDEC-Dekoder
    probe_bg_dump, x 115..134, y 70..150 -> v 0..175), Texel = gemalt * 128 / 73 (Blattflaeche c = 73, tuer_archiv_bauen
    C_BLATT; "gezeigt = gemalt" wie Runde 32/33).
  * LAGEN wie Runde 33 gemessen (S041 c03): Fenster u 26..57 v 17..137, Panikstange v 147..153 u 14..115 (Fuge = u
    klein); dazu aus ROOM10C04 (frontal): Trittblech ab 87 % der Blatthoehe (v 190..217).
  * RELIEF wie die RE2-Tuertexturen (Licht von links oben): Fensterrahmen vertieft, Glas mit Spiegelverlauf,
    Drahtgitter als Rauten aus 1-Texel-Draht, Stange als Zylinder (Glanz oben, Schatten unten, Wurfschatten aufs Holz),
    Halter an beiden Enden, Trittblech gebuerstet (Feinkorn quer aus DOOR07) mit Schrauben, dunkle Blattkanten.
"""
import numpy as np
from PIL import Image

import tuer_archiv_bauen as tab
import tuer_rezepte as R

BV = R.BV   # 218

# Gemessen: ROOM10C04 (Port-Dekoder), linker Fluegel x 115..134, Zeilen y -> Median-RGB (4er-Baender). Blatt y 69..164
# (Oberkante = erste Holzzeile nach dem Rahmen, Unterkante = Ende Trittblech) -> v = (y - 69) * 218 / 95.
PROFIL_Y = [70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 134, 138, 142, 146, 150]
PROFIL_RGB = [(99, 84, 68), (98, 79, 54.5), (97, 77, 51), (95, 73, 49), (89, 67, 43), (87, 65, 41), (80.5, 60.5, 34),
              (75, 55, 33), (78, 56, 29), (76, 56, 29), (75, 55, 28), (72, 52, 25), (69, 49, 24), (65, 48, 24),
              (56, 39, 17), (46, 31, 13), (45, 31, 11), (45, 31, 11), (41, 30, 10.5), (32, 24.5, 10)]
BLATT_Y0, BLATT_H = 69, 95
GLAS = np.array([34.0, 36.0, 32.0])        # ROOM10C04 x 142..148 y 80..119 (Median)
TRITT = np.array([26.0, 29.0, 26.0])       # ROOM10C04 x 115..159 y 153..162 (Median)
FASER_K = 1.3                              # Faserkontrast (relativ); DOOR0D selbst: fein 0.1517
GLAS_BASIS = 0.88                          # Median-Abgleich Fenster (s. rezept), r36_textur_mass / probe 'textur'

FENSTER = (26, 17, 57, 137)                # Runde 33 (S041 c03): u0, v0, u1, v1
STANGE_V = (147, 153)                      # Runde 33
STANGE_U = (14, 116)
TRITT_V0 = 190                             # 87 % (ROOM10C04: Blechkante y 152 -> (152-69)/95)


def _v_aus_y(y):
    return (y - BLATT_Y0) * BV / float(BLATT_H)


def holzprofil():
    """(218, 3) gemalte Holzfarbe je Zeile (lineare Interpolation des Messprofils, Rand fortgesetzt)."""
    vs = np.array([_v_aus_y(y + 1.5) for y in PROFIL_Y])
    rgb = np.array(PROFIL_RGB, np.float64)
    out = np.zeros((BV, 3))
    for k in range(3):
        out[:, k] = np.interp(np.arange(BV), vs, rgb[:, k])
    # Glaetten (das Profil ist aus 4er-Baendern, Stufen vermeiden)
    return tab.blur(out[:, None, :], 3.0)[:, 0, :]


def faser():
    """Relatives Feinkorn (218, 128), waagerechte Faser, aus den drei DOOR0D-Fuellungen (RE2 1:1).
    Je Fuellung nur das Innere ohne Profilkanten (u 27..98, v +2..-2), um 90 Grad gedreht (72 Zeilen quer zur Faser),
    nur laengs (u) auf 128 gestreckt; vier Stuecke (das vierte = erstes gespiegelt) mit 4 Zeilen linearer
    Ueberblendung untereinander - die Fugen laufen MIT der Faser (gemessen vorher: helles Band an harten Fugen)."""
    rgba, _ = R.re2_rgba("DOOR0D")
    L = R.lum(rgba).astype(np.float64)
    stuecke = []
    for v0, v1 in ((20, 59), (91, 130), (162, 201)):
        f = L[v0:v1, 27:99]                                    # 39 x 72, senkrechte Faser
        rel = f / np.maximum(tab.blur(f, 4.0), 1.0) - 1.0      # Feinkorn ohne Licht
        rel = rel - rel.mean()
        rot = np.rot90(rel, 1)                                 # 72 x 39, waagerechte Faser
        h, w = rot.shape
        x = np.linspace(0, w - 1, 128)
        stuecke.append(np.stack([np.interp(x, np.arange(w), rot[r]) for r in range(h)]))
    stuecke.append(stuecke[0][::-1, ::-1])
    UEB = 4
    g = stuecke[0]
    for st in stuecke[1:]:
        a = np.linspace(1.0, 0.0, UEB + 2)[1:-1][:, None]
        mitte = g[-UEB:] * a + st[:UEB] * (1 - a)
        g = np.concatenate([g[:-UEB], mitte, st[UEB:]], 0)
    g = g[:BV]
    s = g.std()
    return np.clip(g, -3 * s, 3 * s)


def buerste(h, w, seed_quelle="DOOR07"):
    """Gebuerstetes Blech: Feinkorn einer RE2-Stahltextur (DOOR07, u 20..108 v 20..120), quer zur Buerstrichtung
    gemittelt -> waagerechte Riefen (relativ, mean 0)."""
    rgba, _ = R.re2_rgba(seed_quelle)
    L = R.lum(rgba).astype(np.float64)[20:120, 20:108]
    rel = L / np.maximum(tab.blur(L, 3.0), 1.0) - 1.0
    # Riefen: jede Zeile = Zeilenmittel des Korns + etwas Korn
    zeilen = rel.mean(1, keepdims=True)
    feld = 0.6 * zeilen + 0.4 * rel
    feld = np.tile(feld, (h // feld.shape[0] + 1, w // feld.shape[1] + 2))[:h, :w]
    return feld - feld.mean()


def rezept(basis):
    rgba = basis["rgba"]
    proto = {}
    prof = holzprofil()                                       # gemalt je Zeile
    g = np.zeros((256, 128, 3))
    g[:BV] = prof[:, None, :]
    # leichte waagerechte Lichtwoelbung wie die RE2-Blaetter (Mitte heller, Kanten -12 %)
    uu = np.arange(128) / 127.0
    woelbung = 1.0 - 0.12 * (2 * uu - 1) ** 2
    # normiert auf die Messstelle (ROOM10C04 x 115..134 = u ~94 von der Fuge): dort gezeigt = gemalt
    g[:BV] *= (woelbung / woelbung[94])[None, :, None]
    # Faser (RE2 DOOR0D, 1:1)
    fa = faser()
    g[:BV] *= (1.0 + FASER_K * fa)[:, :, None]
    protokoll = dict(faser_std=round(float(fa.std()), 4))

    # Blattkanten: Fuge (u 0) und Bandseite (u 127) dunkel, Ober-/Unterkante dunkel (Spalt im Rahmen)
    for u, k in ((0, 0.45), (1, 0.75), (2, 0.9), (127, 0.5), (126, 0.8)):
        g[:BV, u] *= k
    for v, k in ((0, 0.5), (1, 0.8), (BV - 1, 0.5), (BV - 2, 0.8)):
        g[v, :] *= k

    # ---- Fenster: vertiefter Metallrahmen, Glas mit Spiegelverlauf, Drahtglas-Rauten ----
    u0, v0, u1, v1 = FENSTER
    rahmen_farbe = np.array([62.0, 60.0, 54.0])                # dunkles Metall (wie die Stange gemalt, aufgehellt)
    g = R.auftragen(g, R.m_rechteck(u0 - 3, v0 - 3, u1 + 3, v1 + 3, 1), rahmen_farbe)
    g = R.relief_rechteck(g, u0 - 3, v0 - 3, u1 + 3, v1 + 3, 2, 1.45, 0.55, True, 1)      # Rahmen erhaben
    vv, uu2 = np.mgrid[0:256, 0:128].astype(np.float64)
    innen = (uu2 >= u0) & (uu2 < u1) & (vv >= v0) & (vv < v1)
    # Glas: gemalte Glasfarbe, Spiegelung = schraeger heller Streifen + Verlauf oben hell
    t = (vv - v0) / float(v1 - v0)
    spiegel = 1.0 + 0.30 * np.exp(-((uu2 - u0) * 1.0 + (vv - v0) * 0.35 - 22.0) ** 2 / 60.0) + 0.15 * (1 - t)
    # Basis so, dass der MEDIAN des Fensters (Draht + Spiegelung eingerechnet) = gemaltes Glas (ROOM10C04-Median)
    glas = GLAS[None, None, :] * (GLAS_BASIS * spiegel)[:, :, None]
    g = np.where(innen[:, :, None], glas, g)
    # Drahtgitter: Rauten aus 1-Texel-Draht (hell auf dunklem Glas, ROOM10C04 zeigt helle Schraeglinien), Abstand 6
    a1 = np.mod(uu2 - u0 + (vv - v0), 6.0)
    a2 = np.mod(uu2 - u0 - (vv - v0), 6.0)
    draht = innen & ((a1 < 0.9) | (a2 < 0.9))
    g = np.where(draht[:, :, None], g * 1.0 + np.array([12.0, 12.0, 11.0])[None, None, :], g)
    # Innenschatten oben/links im Glas (Rahmen wirft Schatten, Licht von links oben)
    sch = innen & (((uu2 - u0) < 2) | ((vv - v0) < 2))
    g = np.where(sch[:, :, None], g * 0.55, g)

    # ---- Panikstange: Zylinder + Halter + Wurfschatten ----
    s0, s1 = STANGE_V
    su0, su1 = STANGE_U
    stahl = np.array([150.0, 150.0, 146.0]) * float(prof[150] @ R.LUM) / 120.0
    # Wurfschatten aufs Holz (unten rechts versetzt)
    sw = (uu2 >= su0 + 1) & (uu2 < su1 + 1) & (vv >= s1) & (vv < s1 + 3)
    g = np.where(sw[:, :, None], g * 0.55, g)
    for v in range(s0, s1):
        tt = (v - s0) / float(s1 - s0 - 1)                     # 0 oben .. 1 unten
        k = 1.55 - 1.1 * tt if tt < 0.35 else 1.17 - 0.75 * tt  # Glanz oben, Schatten unten
        g[v, su0:su1] = stahl * k
    # Halter (Platten an beiden Enden, ueber die Stange hinaus)
    for hu in (su0 - 2, su1 - 4):
        g = R.auftragen(g, R.m_rechteck(hu, s0 - 6, hu + 6, s1 + 5, 1), stahl * 0.8)
        g = R.relief_rechteck(g, hu, s0 - 6, hu + 6, s1 + 5, 1, 1.4, 0.55, True, 1)

    # ---- Trittblech: gebuerstet, Oberkante hell, Schrauben ----
    tb = buerste(BV - TRITT_V0, 128)
    blech = TRITT[None, None, :] * (1.0 + 1.2 * tb)[:, :, None]
    # Licht: oben etwas heller
    tv = np.linspace(1.15, 0.85, BV - TRITT_V0)[:, None, None]
    g[TRITT_V0:BV] = blech * tv
    g[TRITT_V0, :] = TRITT * 2.2                               # Kante (Licht von oben)
    g[TRITT_V0 + 1, :] = TRITT * 1.5
    for su in (6, 64, 121):
        for sv in (TRITT_V0 + 5, BV - 6):
            g = R.auftragen(g, R.m_ellipse(su - 1.5, sv - 1.5, su + 1.5, sv + 1.5), TRITT * 2.0)
            g[int(sv) + 1, su] = TRITT * 0.6
    g[BV - 1, :] *= 0.5

    out, gek = R.zusammensetzen(rgba, g)
    proto.update(protokoll)
    proto.update(fenster=list(FENSTER), panikstange=[su0, s0, su1, s1], trittblech_v0=TRITT_V0, gekappt=gek,
                 herkunft="Faser RE2 DOOR0D (3 Fuellungen, gedreht, nur laengs gestreckt); Farbe/Licht ROOM10C04 "
                          "linker Fluegel (Port-Dekoder); Lagen Runde 33 S041 c03 + ROOM10C04")
    return out, proto
