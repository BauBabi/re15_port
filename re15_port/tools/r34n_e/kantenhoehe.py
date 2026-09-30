# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Hoehe einer Moebel-Oberseite ueber die KANTENLAGE gegen die SCA-Zelle.

Wenn nur EIN Cut die Ablagestelle zeigt (ROOM1000 elliot.bmp: nur Cut 0), geht die
Flaechenkorrelation zweier Cuts nicht. Dann traegt die Kollisionszelle: RE1.5s Masken-Tiefen SIND
die Entfernung zum Kollisionsquader (Memory reai-v2-quader-modell: 31 Cuts, 260858 Punkte, Faktor
1,00), die SCA-Zelle liegt also auf dem gemalten Moebel. Ihre Kanten auf der Hoehe h projiziert
muessen auf den gemalten Kanten liegen - fuer JEDE Kante und JEDEN Cut dieselbe Hoehe.

Je Kante: gemalte Kantenpunkte = staerkster Sprung der "Braeune" R-B (Holz gegen Boden) entlang
Spalten (waagerechte Kanten) bzw. Zeilen (schraege Kanten); Ausreisser (> 2 px von der
Ausgleichsgeraden) verworfen und gezaehlt. Fuer jedes h wird die Weltkante (fester Wert der
Achse, gleichmaessig abgetastet) mit der ENGINE-Matrix projiziert (geom.py = Sonde `kamera`) und
der mittlere senkrechte Abstand der Kantenpunkte zur projizierten Geraden gemessen.
Minimum = Hoehe dieser Kante. Kein Schwellwert fuer h.

Aufruf: python re15_port/tools/r34n_e/kantenhoehe.py 1000
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402

# Kanten je Raum: (Name, Cut, Welt-Kante, Suchfenster, Suchrichtung, Vorzeichen)
#   Welt-Kante: ("z", wert, von, bis) = Linie z = wert fuer x von..bis; ("x", wert, von, bis) = x = wert, z von..bis
#   Suchrichtung "spalte": je Bildspalte sx in [a..b] die Zeile in [c..d] mit dem staerksten Sprung;
#                "zeile": je Bildzeile sy in [c..d] die Spalte in [a..b]
#   Vorzeichen +1: Braeune nimmt in Suchrichtung ZU (Boden -> Holz), -1: nimmt AB
KANTEN = {
    "1000": [
        # SCA-Zelle 7 x[18800..20650] z[-12450..-2750] (Sonde `sca 1000`)
        ("Cut 0 Suedende z=-12450", 0, ("z", -12450, 18800, 20650), (150, 206, 140, 180), "spalte", +1, "braun"),
        ("Cut 0 Ostkante x=20650", 0, ("x", 20650, -12400, -9200), (95, 160, 158, 238), "zeile", +1, "braun"),
        ("Cut 2 Nordende z=-2750", 2, ("z", -2750, 18800, 20650), (152, 222, 195, 222), "spalte", +1, "braun"),
        ("Cut 1 Nordende z=-2750", 1, ("z", -2750, 18800, 20650), (147, 180, 120, 140), "spalte", +1, "braun"),
    ],
    # Marvins Schreibtisch: SCA-Zelle 2 x[-10893..-9197] z[-17864..-15064] (floor 2), Westseite
    # x = -10893 zeigt zu beiden Kameras (Cut 6 bei x -18972, Cut 3 bei x -26622). Vorderkante der
    # Platte = staerkster HELLIGKEITS-Abfall nach unten (helle Platte -> dunkle Fuge unter der Kante).
    "1020": [
        ("Cut 6 Westkante x=-10893", 6, ("x", -10893, -17864, -15064), (130, 180, 106, 118), "spalte", -1, "hell"),
        ("Cut 3 Westkante x=-10893", 3, ("x", -10893, -17864, -15064), (128, 160, 93, 103), "spalte", -1, "hell"),
    ],
    # Verhoertisch: SCA-Zelle 5 x[-50..2450] z[1550..5900] (Sonde `sca 1010`); Cut 0 steht westlich
    # (x -1893) - die Westkante x = -50 ist die linke Tischkante im Bild (erstes Holz von links).
    "1010": [
        ("Cut 0 Westkante x=-50", 0, ("x", -50, 1600, 5850), (140, 200, 138, 186), "zeile", +1, "braun"),
    ],
}


def kantenpunkte(A, fenster, richtung, vz, kanal="braun"):
    br = (A[:, :, 0] - A[:, :, 2]) if kanal == "braun" else A.sum(axis=2) / 3.0
    a, b, c, d = fenster
    pts = []
    if richtung == "spalte":
        for sx in range(a, b + 1):
            g = np.diff(br[c:d + 1, sx]) * vz
            i = int(np.argmax(g))
            if g[i] >= 20:
                pts.append((sx + 0.5, c + i + 1.0))
    else:
        # schraege Laengskante: die ERSTE Stelle (in Suchrichtung), ab der die Braeune >= 30
        # bleibt (drei Folgepixel) - das Holz beginnt; davor Boden (R-B < 20). Der staerkste
        # Sprung taugt hier nicht: die Fase der Kante ist ein heller Streifen, und Muetze/Buegel
        # liegen IM Holz.
        for sy in range(c, d + 1):
            row = br[sy, a:b + 1] * vz
            for i in range(1, len(row) - 3):
                if row[i - 1] < 30 and all(row[i + k] >= 30 for k in range(4)):
                    pts.append((a + i + 0.0, sy + 0.5))
                    break
    p = np.array(pts)
    if richtung == "spalte":
        k, m = np.polyfit(p[:, 0], p[:, 1], 1)
        r = np.abs(p[:, 1] - (k * p[:, 0] + m))
    else:
        k, m = np.polyfit(p[:, 1], p[:, 0], 1)
        r = np.abs(p[:, 0] - (k * p[:, 1] + m))
    return p[r <= 2.0], int((r > 2.0).sum())


def abstand(cam, kante, h, pts):
    achse, wert, von, bis = kante
    s = np.linspace(von, bis, 60)
    q = []
    for t in s:
        w = (wert, h, t) if achse == "x" else (t, h, wert)
        r = geom.projiziere(cam, w)
        if r is not None:
            q.append((r[0], r[1]))
    q = np.array(q)
    if len(q) < 2:
        return float("nan")
    # Gerade durch die projizierten Punkte (Projektion einer Weltgeraden = Bildgerade)
    p0, p1 = q[0], q[-1]
    n = np.array([-(p1[1] - p0[1]), p1[0] - p0[0]])
    n /= np.linalg.norm(n)
    return float(np.mean(np.abs((pts - p0) @ n)))


def main():
    raum = sys.argv[1] if len(sys.argv) > 1 else "1000"
    cams = geom.lade_kameras(raum)
    hs = list(range(-2500, 1, 5))
    ergebnis = []
    for name, cut, kante, fenster, richtung, vz, kanal in KANTEN[raum]:
        A = geom.bg(raum, cut)
        pts, weg = kantenpunkte(A, fenster, richtung, vz, kanal)
        werte = [(h, abstand(cams[cut], kante, h, pts)) for h in hs]
        werte = [w for w in werte if w[1] == w[1]]
        best = min(werte, key=lambda t: t[1])
        nb = dict(werte)
        ergebnis.append((name, best[0], best[1]))
        print("%-26s %2d Kantenpunkte (verworfen %d): Minimum h = %d, mittlerer Abstand %.2f px "
              "(h-100: %.2f, h+100: %.2f)" % (name, len(pts), weg, best[0], best[1],
                                             nb.get(best[0] - 100, float("nan")), nb.get(best[0] + 100, float("nan"))))
    hh = [e[1] for e in ergebnis]
    print("Hoehen je Kante: %s -> Mittel %.0f, Spanne %d..%d" % (hh, np.mean(hh), min(hh), max(hh)))


if __name__ == "__main__":
    main()
