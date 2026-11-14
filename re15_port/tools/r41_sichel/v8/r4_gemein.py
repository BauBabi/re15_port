"""SF V8 Runde 4 (Bauer, 2026-11-09): Messhilfen fuer die Befunde der Pruefung V8 r3 (Schlitz von vorn, rote Kappe, rote Kacheln, Band).

Kein RE-Befund; keine Zahl hier ist eine @0x-Konstante.
Schlitz (Zahlen-Pruefer r3 O3: "In F344-F378 klafft links neben dem Kopf ein Loch durch die Silhouette: 807 Loch-Pixel ... Offline in P1 sind
es 0 px, deshalb faellt der Fehler durch S9"): gemessen sfv8/r4 ana3.py - die Huegel-Flaeche v14 von Mesh 8 ist von vorn eine Rueckseite
(Culling wie im Port, BB._mac0), hinter ihr liegt keine andere Flaeche -> Loch. Mass hier: Bildpunkte, die Knochen 0-15 OHNE Rueckseiten-
Culling belegen und MIT Culling nicht, und die bei V7 in derselben Pose nicht so sind (V7 hat solche Stellen an den offenen Mesh-Raendern).
"""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import birkin_buehne as BB            # noqa: E402

SCHLITZ_TAKTE = tuple(range(336, 373, 2))     # Szene von vorn (Clip 1, Gier 1513-1529) = Framedump F342-F378 (Takt + 6, r3_gemein)


def ohne_culling(mo, pose, knochen):
    orig = BB._mac0
    BB._mac0 = lambda a, b, c: abs(orig(a, b, c)) + 1
    try:
        return V.b2.umriss_schnell(mo, pose, knochen=knochen)
    finally:
        BB._mac0 = orig


def schlitz_menge(mo, pose):
    k = list(range(16))
    return ohne_culling(mo, pose, k) & ~V.b2.umriss_schnell(mo, pose, knochen=k)


def vorn_posen():
    sp = R3.szene_posen()
    return [('P1', R3.P1)] + [('F%d' % f, sp[f]) for f in SCHLITZ_TAKTE]


def schlitz(mo, mo7, posen=None):
    """Je Ansicht: Loch-Bildpunkte, die V7 dort nicht hat."""
    out = []
    for nm, pose in (posen or vorn_posen()):
        out.append((nm, int((schlitz_menge(mo, pose) & ~schlitz_menge(mo7, pose)).sum())))
    return out
