"""Wie hoch ist ein Buerostuhl in ROOM10F0 wirklich? — an der KUNST gemessen.

⛔ WARUM NEU GEMESSEN: die ausgelieferte Quaderhoehe -1950 stammt aus auswahl.json
   _warum, dritte Runde 2026-09-09: "die schwarzen Lehnen laufen bis h~-1950 (an den
   SCHWARZ-SAEULEN aufgeloest)". Gemessen wurde also am TIEFSCHWARZ - derselben Groesse,
   die die Regel danach als Stuhl fuehrt. Das ist ein Zirkelschluss: wenn die Wand
   faelschlich als Stuhl zaehlt, misst man die Wandhoehe.

VERFAHREN (nicht zirkulaer): drei der Stuehle sind WEISS und vom Nutzer freigestellt.
Ihre Freistellung ist die Kunst, nicht das Dunkel. Ueber der MITTE ihrer Kollisionszelle
wird eine senkrechte Weltstrecke projiziert; gesucht ist die Hoehe, deren Bildzeile die
OBERSTE Zeile der Freistellung trifft.

EICHUNG des Verfahrens: dieselbe Strecke bei Hoehe 0 muss die UNTERSTE Zeile der
Freistellung treffen (der Stuhl steht auf dem Boden). Steht die Eichung nicht, taugt
das Verfahren nicht und das sagt die Ausgabe.
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import geom
import maske_aus_png
import raum
from geom import load_rdt
from scipy import ndimage as nd

CD = "re15_port/shared_assets/PSX"


def zeile_von(v, x, y, z):
    """Bildzeile eines Weltpunkts im Kamerasatz v = (R, t, H)."""
    R, t, H = v
    M = np.array(R, float).reshape(3, 3) / 4096.0
    c = M.dot(np.array([x, y, z], float)) + np.array(t, float)
    if c[2] <= 0:
        return None
    return H * c[1] / c[2] + 120.0


def main():
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    sp = geom.sca_sperrzellen(rdt, 0) or []
    print("Stuhl   Zelle   Kunst-Kasten    Eichung (Hoehe 0 -> Zeile)   gemessene Hoehe")
    werte = []
    for cut in (4, 5):
        v = geom.cut_view(rdt, cam, cut)
        for o in aus[str(cut)]["objekte"]:
            if "png" not in o:
                continue
            r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
            if r is None:
                continue
            lab, n = nd.label(r)
            for k in range(1, n + 1):
                comp = lab == k
                if comp.sum() < 20:
                    continue
                bf, bki = 0.0, None
                for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
                    if typ != 3:
                        continue
                    _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
                    f = float((tr & comp).sum()) / float(comp.sum())
                    if f > bf:
                        bf, bki = f, ki
                if bki is None:
                    continue
                zx, zz, zw, zd, _ = sp[bki]
                mx, mz = zx + zw / 2.0, zz + zd / 2.0
                ys, xs = np.nonzero(comp)
                yo, yu = int(ys.min()), int(ys.max())
                z0 = zeile_von(v, mx, 0, mz)
                # Hoehe suchen, deren Zeile == yo
                best, bh = 1e9, None
                for h in range(0, -2601, -5):
                    zz_ = zeile_von(v, mx, h, mz)
                    if zz_ is None:
                        continue
                    d = abs(zz_ - yo)
                    if d < best:
                        best, bh = d, h
                abgeschnitten = (yu >= 239) or (yo <= 0)
                print("c%d %-18s Z%-2d  x%d..%d y%d..%d   Boden y=%6.1f (Kunst %3d, "
                      "Abweichung %5.1f)   h=%5d %s"
                      % (cut, o["name"][9:24], bki, xs.min(), xs.max(), yo, yu,
                         z0, yu, z0 - yu, bh,
                         "⛔ Kunst am Bildrand abgeschnitten" if abgeschnitten else ""))
                if not abgeschnitten:
                    werte.append((cut, bki, bh, z0 - yu))
    print()
    gut = [w for w in werte if abs(w[3]) <= 3.0]
    print("Eichung bestanden (|Abweichung| <= 3 Bildzeilen) bei %d von %d Stuehlen"
          % (len(gut), len(werte)))
    if gut:
        hs = [w[2] for w in gut]
        print("gemessene Stuhlhoehen: %s" % ", ".join("%d" % h for h in hs))
        print("Median %d | Mittel %.0f | Spanne %d..%d"
              % (int(np.median(hs)), float(np.mean(hs)), max(hs), min(hs)))


if __name__ == "__main__":
    main()
