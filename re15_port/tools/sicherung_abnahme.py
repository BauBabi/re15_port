#!/usr/bin/env python3
"""ABNAHME des Sicherungs-Modells gegen die Originalkunst.

Nicht "sieht richtig aus": das Modell wird mit der ECHTEN RDT-Kamera von ROOM1050
Cut 8 projiziert und seine Silhouette Zeile fuer Zeile gegen die des Originals
gemessen.

DIE ORIGINAL-SILHOUETTE wird als DIFFERENZ Cut8 - Cut7 bestimmt: die Sicherung ist
genau das, was in Cut 8 an dieser Stelle hinzukommt. Eine Helligkeitsschwelle auf
Cut 8 allein taugt nicht — sie verliert die untere Metallkappe, die im Schatten liegt
(das war der erste Anlauf und hat die Hoehe um 14 px zu kurz gemessen).

Kamera (ROOM1050.RDT Kameratabelle @0x60 + 8*32):
    fov=26684 -> H = fov>>7 = 208   (render_pc.c:30-38)
    pos=(16171,-2733,-8185)  tgt=(18240,-1974,-8706)
Projektion sx = 160 + H*x/z, sy = 120 + H*y/z (main.c:5172-5173).
PSX-Konvention: +Y zeigt nach UNTEN — die Kamerabasis wird deshalb unten GEPRUEFT,
nicht angenommen.

    python re15_port/tools/sicherung_abnahme.py
"""
import os
import sys

import numpy as np

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sicherung_modell import bg_laden, modell_bauen  # noqa: E402

CAM = np.array([16171.0, -2733.0, -8185.0])
TGT = np.array([18240.0, -1974.0, -8706.0])
H = 26684 >> 7
EBENE_X = 17100.0      # Montageplatte: vor der SCA-Wand x=17200
FENSTER = (185, 216)   # x-Fenster der rechten (in Cut 8 neuen) Sicherung


def basis():
    """Orthonormale Kamerabasis. u ist die BILDSCHIRM-UNTEN-Richtung: sy waechst
    nach unten, und in PSX-Weltkoordinaten ist +Y ebenfalls unten — u[1] muss
    also positiv sein. Genau das wird hier erzwungen statt angenommen."""
    f = TGT - CAM
    f /= np.linalg.norm(f)
    r = np.cross(np.array([0.0, 1.0, 0.0]), f)
    r /= np.linalg.norm(r)
    u = np.cross(f, r)
    u /= np.linalg.norm(u)
    if u[1] < 0:                      # Haendigkeit so drehen, dass u nach unten zeigt
        r, u = -r, -u
    assert u[1] > 0, "u muss nach Welt-unten zeigen"
    return f, r, u


def rueckprojekt(sx, sy, X):
    f, r, u = basis()
    d = f + r * ((sx - 160.0) / H) + u * ((sy - 120.0) / H)
    return CAM + d * ((X - CAM[0]) / d[0])


def projiziere(p):
    f, r, u = basis()
    v = p - CAM
    z = float(v @ f)
    if z <= 1.0:
        return None
    return 160.0 + H * float(v @ r) / z, 120.0 + H * float(v @ u) / z, z


def silhouette_original():
    """Was kommt in Cut 8 hinzu? = die eingesetzte Sicherung."""
    a = bg_laden("ROOM10507.ppm").astype(np.int32).sum(2)
    b = bg_laden("ROOM10508.ppm").astype(np.int32).sum(2)
    aus = {}
    for y in range(80, 180):
        xs = [x for x in range(*FENSTER) if b[y, x] - a[y, x] > 30]
        if not xs:
            continue
        runs, s, p = [], xs[0], xs[0]
        for x in xs[1:]:
            if x == p + 1:
                p = x
            else:
                runs.append((s, p)); s = x; p = x
        runs.append((s, p))
        lo, hi = max(runs, key=lambda t: t[1] - t[0])
        if hi - lo + 1 >= 4:
            aus[y] = (lo, hi)
    return aus


def main():
    f, r, u = basis()
    print("Kamerabasis geprueft: f=%s r=%s u=%s (u[1]>0 = Bildschirm-unten)"
          % (np.round(f, 3), np.round(r, 3), np.round(u, 3)))

    orig = silhouette_original()
    oy = sorted(orig)
    y_oben, y_unten = min(oy), max(oy)
    x_mitte = (orig[y_unten][0] + orig[y_unten][1]) / 2.0

    # Massstab NICHT vorrechnen, sondern aus der Rueckprojektion ABLEITEN:
    p_fuss = rueckprojekt(x_mitte, y_unten, EBENE_X)
    p_kopf = rueckprojekt(x_mitte, y_oben, EBENE_X)
    laenge_ist = float(np.linalg.norm(p_kopf - p_fuss))
    br = np.mean([orig[y][1] - orig[y][0] + 1 for y in oy])
    p_l = rueckprojekt(orig[y_unten][0], y_unten, EBENE_X)
    p_r = rueckprojekt(orig[y_unten][1], y_unten, EBENE_X)
    print()
    print("ORIGINAL auf der Ebene x=%.0f zurueckprojiziert:" % EBENE_X)
    print("  Silhouette y %d..%d = %d px, mittlere Breite %.1f px"
          % (y_oben, y_unten, y_unten - y_oben + 1, br))
    print("  -> Laenge  %.0f units (%.1f cm)" % (laenge_ist, laenge_ist / 10))
    print("  -> Breite  %.0f units (%.1f cm) an der Fusslinie"
          % (np.linalg.norm(p_r - p_l), np.linalg.norm(p_r - p_l) / 10))
    print("  -> %.2f Welteinheiten je Bildpixel" % (laenge_ist / (y_unten - y_oben)))

    v, vt, faces = modell_bauen()
    laenge_modell = max(p[1] for p in v)
    print()
    print("MODELL: Laenge %.0f units -> Abweichung %+.0f units (%+.1f %%)"
          % (laenge_modell, laenge_modell - laenge_ist,
             100.0 * (laenge_modell - laenge_ist) / laenge_ist))

    # Modell an den Fusspunkt setzen: +Y des Modells = nach Welt-oben (-Y)
    welt = [np.array([p_fuss[0] + p[2], p_fuss[1] - p[1], p_fuss[2] + p[0]]) for p in v]
    # ⛔ Das Modell wird GERASTERT (Flaechen gefuellt) und dann mit DERSELBEN
    # Lauf-Messung wie das Original ausgewertet. Ein Vergleich von Kanten-Extremwerten
    # gegen eine Pixel-Schwelle misst zwei verschiedene Dinge und ueberschaetzt die
    # Modellbreite um rund 1 px.
    # 4x ueberabgetastet und danach mit einer 50-%-Deckungsschwelle ausgewertet:
    # die Original-Silhouette zaehlt nur Pixel, die die Schwelle reissen, also
    # Randpixel mit halber Deckung NICHT. Eine ganzzahlige Polygonfuellung tut das
    # doch und macht das Modell rund 1 px zu breit.
    from PIL import Image, ImageDraw
    SS = 4
    bild = Image.new("L", (320 * SS, 240 * SS), 0)
    zeich = ImageDraw.Draw(bild)
    proj = [projiziere(w) for w in welt]
    flaechen = []
    for fc in faces:
        pts = [proj[a - 1] for a, _ in fc if proj[a - 1]]
        if len(pts) < 3:
            continue
        zm = sum(p[2] for p in pts) / len(pts)
        flaechen.append((zm, [(p[0] * SS, p[1] * SS) for p in pts]))
    for _, pts in sorted(flaechen, key=lambda t: -t[0]):
        zeich.polygon(pts, fill=255)
    px = np.asarray(bild.resize((320, 240), Image.BOX))   # BOX = echte Deckung
    modell = {}
    for y in range(240):
        xs = np.nonzero(px[y] > 127)[0]
        if len(xs):
            modell[y] = (float(xs.min()), float(xs.max()))
    my = sorted(modell)

    print("        projiziert y %d..%d = %d px (Original %d px) -> Abw %+d px"
          % (min(my), max(my), max(my) - min(my) + 1,
             y_unten - y_oben + 1, (max(my) - min(my) + 1) - (y_unten - y_oben + 1)))
    print()
    print("  y   Original    Modell            Breite o/m     Abw")
    diffs, versatz = [], []
    for y in range(min(min(my), y_oben), max(max(my), y_unten) + 1):
        o, m = orig.get(y), modell.get(y)
        bo = (o[1] - o[0] + 1) if o else 0
        bm = (m[1] - m[0] + 1) if m else 0
        d = (bm - bo) if (o and m) else None
        if d is not None:
            diffs.append(d)
            versatz.append(((m[0] + m[1]) / 2) - ((o[0] + o[1]) / 2))
        print("  %3d  %s  %s   %2d / %5.1f   %s"
              % (y, ("%3d..%3d" % o) if o else "   -    ",
                 ("%6.1f..%6.1f" % m) if m else "       -      ",
                 bo, bm, ("%+.1f" % d) if d is not None else "-"))
    if diffs:
        a, s = np.array(diffs, float), np.array(versatz, float)
        print()
        print("Breite:  Median %+.1f px, Mittel %+.2f px, Betrag-Mittel %.2f px (%d Zeilen)"
              % (np.median(a), a.mean(), np.abs(a).mean(), len(a)))
        # ⛔ Zeilen, in denen das Original vom Klemmenblock/Kabel ANGESCHNITTEN ist,
        # sind kein Modellfehler. Sie werden getrennt ausgewiesen, nicht weggelassen.
        voll = np.array([d for d, o in zip(diffs, [orig[y] for y in sorted(orig)
                                                   if y in modell])
                         if (o[1] - o[0] + 1) >= 10], float)
        if len(voll):
            print("         nur unverdeckte Zeilen (Originalbreite >= 10 px): "
                  "Median %+.1f px, Betrag-Mittel %.2f px (%d Zeilen)"
                  % (np.median(voll), np.abs(voll).mean(), len(voll)))
        print("Versatz: Mittel %+.2f px (Mittelachse Modell gegen Original)" % s.mean())


if __name__ == "__main__":
    main()
