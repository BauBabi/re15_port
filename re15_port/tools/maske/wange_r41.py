"""Runde 41 Spur Q, Nachbesserung 1 (Abnahme 0 Mangel M1): Lage der Treppenwange ROOM6010 aus zwei Kameras.

Dossier: analysis/befunde_runde41/Q_restpunkte.md, Abschnitt "Nachbesserung 1".

BEFUND: Teil 8 in Cut 5 (R113..) ist die Aussenflaeche der Westwange der suedlichen Treppe. Seine Modell-Zeile war die Kollisionszelle @0x0092C
(Westflaeche x -19919, Band 1). Die Zelle ist nicht die Wange: die Wange steht weiter westlich, eine Figur zwischen Wange und Zellflaeche blieb sichtbar.
Fuer die Wange gibt es im RDT keine Zelle. Ihre Lage kommt aus dem Bild: Kameras 4 und 5 (RDT-Kamerasatz, Kamera des Spiels platzieren_r41.ansicht_spiel)
sehen dieselbe Aussenflaeche von Norden und von Sueden, Basislinie rund 16 000.

REGEL: je Kamera die Bildkante als Gerade durch die Proben (Hauptachse), Sehebene = Kamera + Gerade, 3D-Kante = Schnitt der zwei Sehebenen. Das gilt nur, wenn die
beiden Bildkanten dieselbe Kante sind; Gegenprobe ist dx/dz ~ 0 (alle Treppen des Raums laufen achsparallel) und dy/dz ~ 5400/6445 (Laenge der Zelle @0x0092C).
Proben: je Bildzeile der Sprung im Helligkeitsprofil (R+G+B)/3 des Hintergrunds (--ppm prueft sie nach), je Kante zwei Faelle:
  UNTEN = Dunkel -> Oliv, OBEN = Oliv -> Schwarz.
EBENE der Wange = Mittel der beiden 3D-Kanten auf der Hoehe von Teil 8 (z -20500, die Sehstrahlen von Teil 8 treffen z -20963..-20069).

Aufruf:
  python re15_port/tools/maske/wange_r41.py [--ppm <Ordner mit ROOM6010<c>.ppm aus probe_bg_dump>]
"""
import argparse
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import platzieren_r41 as P      # noqa: E402
import schliessen_r41 as SR     # noqa: E402

RAUM = 0x6010
# Bildkanten (u, v), Bildzeilen v wie gelesen; die Zeilen 216 links in Cut 5 und unter 110 / ueber 134 in Cut 4 kreuzt ein fremder Streifen (Zug, Gitter), sie fehlen.
UNTEN5 = [(147.5, 204), (149.5, 208), (150.7, 212), (156.5, 220), (158.5, 224), (160.7, 228), (162.7, 232), (165.7, 236)]
OBEN5 = [(158.5, 204), (160.5, 208), (162.6, 212), (165.5, 216), (167.6, 220), (170.5, 224), (172.5, 228), (174.5, 232), (177.6, 236)]
UNTEN4 = [(259.5, 110), (260.7, 113), (261.7, 116), (263.6, 119), (265.5, 122), (266.7, 125), (268.6, 128), (270.5, 131), (271.6, 134)]
OBEN4 = [(265.5, 110), (267.5, 113), (268.5, 116), (271.5, 119), (271.5, 122), (273.5, 125), (274.5, 128), (275.5, 131), (277.6, 134)]
Z_TEIL8 = -20500.0


def kamera(rdt, cut):
    r, t, h = P.ansicht_spiel(rdt, cut)
    R = np.array(r, float).reshape(3, 3) / 4096.0
    t = np.array(t, float)
    return R, t, h, -np.linalg.inv(R).dot(t)


def strahl(k, u, v):
    R, _t, h, _c = k
    return R.T.dot(np.array([(u - 160.0) / h, (v - 120.0) / h, 1.0]))


def sehebene(k, pts):
    """Ebene durch die Kamera und die Bildgerade der Proben -> (Normale, Punkt, mittlerer Abstand der Proben in Bildpunkten)."""
    p = np.array(pts, float)
    m = p.mean(0)
    _u, _s, vt = np.linalg.svd(p - m)
    a, b = m - 20 * vt[0], m + 20 * vt[0]
    n = np.cross(strahl(k, *a), strahl(k, *b))
    return n / np.linalg.norm(n), k[3], float(np.abs((p - m).dot(vt[1])).mean())


def schnitt(e1, e2):
    """Schnittgerade zweier Ebenen -> Funktion z -> Punkt, Richtung (dx/dz, dy/dz)."""
    d = np.cross(e1[0], e2[0])
    d = d / np.linalg.norm(d)
    p0 = np.linalg.solve(np.array([e1[0], e2[0], d]), np.array([e1[0].dot(e1[1]), e2[0].dot(e2[1]), 0.0]))

    def bei(z):
        return p0 + (z - p0[2]) / d[2] * d
    return bei, d[0] / d[2], d[1] / d[2]


def kante(k5, k4, a5, a4):
    e5, e4 = sehebene(k5, a5), sehebene(k4, a4)
    bei, dxdz, dydz = schnitt(e5, e4)
    return bei, dxdz, dydz, e5[2], e4[2]


def ppm_lesen(p):
    with open(p, "rb") as f:
        b = f.read()
    t = b.split(b"\n", 3)
    w, h = map(int, t[1].split())
    return np.frombuffer(t[3], np.uint8).reshape(h, w, 3).astype(float).sum(2) / 3.0


def proben_pruefen(ordner):
    """Jede Probe muss auf dem steilsten Stueck des Helligkeitsprofils ihrer Bildzeile liegen (+-1,5 Bildpunkte um die Probe). -> Zahl der Abweichler."""
    fehl = 0
    for name, cut, pts, zeichen in (("UNTEN5", 5, UNTEN5, 1), ("OBEN5", 5, OBEN5, -1), ("UNTEN4", 4, UNTEN4, 1), ("OBEN4", 4, OBEN4, -1)):
        img = ppm_lesen(os.path.join(ordner, "ROOM6010%d.ppm" % cut))
        spruenge = []
        for u, v in pts:
            z = img[int(v)]
            x0 = int(np.floor(u))
            best = max(zeichen * (z[x + 1] - z[x]) for x in range(x0 - 1, x0 + 2))
            spruenge.append(best)
            if best < 15:
                fehl += 1
        print("  %s: Sprung je Probe %s" % (name, " ".join("%3d" % s for s in spruenge)))
    return fehl


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ppm", help="Ordner mit den Hintergruenden ROOM60104.ppm und ROOM60105.ppm (probe_bg_dump)")
    a = ap.parse_args()
    rdt = open(os.path.join(SR.PSX, "STAGE%d" % (RAUM >> 12), "ROOM%04X.RDT" % RAUM), "rb").read()
    k5, k4 = kamera(rdt, 5), kamera(rdt, 4)
    print("Kamera 5 %s, Kamera 4 %s" % (k5[3].round(0), k4[3].round(0)))
    u = kante(k5, k4, UNTEN5, UNTEN4)
    o = kante(k5, k4, OBEN5, OBEN4)
    xs = []
    for nm, q in (("Unterkante", u), ("Oberkante ", o)):
        p = q[0](-20000.0)
        t = q[0](Z_TEIL8)
        xs.append(t[0])
        print("%s: bei z -20000 x %.0f y %.0f, bei z %.0f x %.0f y %.0f, dx/dz %.3f, dy/dz %.3f, Rest der Proben %.2f / %.2f Bildpunkte" % (
            nm, p[0], p[1], Z_TEIL8, t[0], t[1], q[1], q[2], q[3], q[4]))
    print("Ebene der Wange: x = %.0f (Mittel der beiden Kanten auf der Hoehe von Teil 8)" % (sum(xs) / 2.0))
    if a.ppm:
        n = proben_pruefen(a.ppm)
        print("Proben neben dem Sprung: %d" % n)
        return 1 if n else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
