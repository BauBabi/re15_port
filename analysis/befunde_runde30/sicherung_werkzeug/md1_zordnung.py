#!/usr/bin/env python3
"""Prueft und berichtigt die ECKEN-REIHENFOLGE der Vierecke im Sicherungs-MD1.

BEFUND: Die 40 Vierecke in gen/sicherung_prop.inc stehen im UMLAUF (0-1-2-3 rund um die
Flaeche). Alle ausgelieferten Vierecke stehen in Z-ORDNUNG (0-1 obere Kante, 2-3 untere
Kante) — das ist die Eckenfolge des PSX-Primitivs POLY_GT4, und der Port teilt danach:
    Dreieck (0,1,3) + Dreieck (0,3,2)        platform/pc/main.c:9735-9746
Auf ein Umlauf-Viereck angewandt ueberlappen die beiden Dreiecke, und ein Viertel der
Flaeche (das Dreieck Ecke1-Ecke2-Mitte) bleibt OFFEN.

Das Werkzeug
  1. zaehlt die Ordnung fuer Original-Props und fuer die Sicherung,
  2. misst die Deckung: gezeichnete Flaeche / Vierecksflaeche,
  3. schreibt eine berichtigte Fassung (Ecke 2 <-> Ecke 3, samt Normalen-Index und UV-Satz)
     nach build/r30_sicherung/sicherung_zordnung.md1 — NUR dorthin.

    python analysis/befunde_runde30/sicherung_werkzeug/md1_zordnung.py
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import REPO, inc_bytes, md1_lesen, props, rdt_laden  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")


def ordnung(p):
    p = np.array(p, float)
    n = np.cross(p[1] - p[0], p[2] - p[0])
    if np.abs(n).max() == 0:
        n = np.cross(p[1] - p[0], p[3] - p[0])
    ax = int(np.argmax(np.abs(n)))
    k = [i for i in range(3) if i != ax]
    q = p[:, k]

    def ccw(u, v, w):
        return (w[1] - u[1]) * (v[0] - u[0]) - (v[1] - u[1]) * (w[0] - u[0])

    def schneiden(a, b, c, d):
        return ccw(a, b, c) * ccw(a, b, d) < 0 and ccw(c, d, a) * ccw(c, d, b) < 0
    if schneiden(q[0], q[3], q[1], q[2]):
        return "Z"
    if schneiden(q[0], q[2], q[1], q[3]):
        return "UMLAUF"
    return "entartet"


def flaeche3(a, b, c):
    return 0.5 * np.linalg.norm(np.cross(np.array(b, float) - a, np.array(c, float) - a))


def deckung(p):
    """Anteil der Vierecksflaeche, den (0,1,3)+(0,3,2) wirklich bedecken (Abtastung)."""
    p = np.array(p, float)
    n = np.cross(p[1] - p[0], p[2] - p[0])
    if np.abs(n).max() == 0:
        n = np.cross(p[1] - p[0], p[3] - p[0])
    ax = int(np.argmax(np.abs(n)))
    k = [i for i in range(3) if i != ax]
    q = p[:, k]
    lo, hi = q.min(0), q.max(0)
    N = 120
    xs = np.linspace(lo[0], hi[0], N)
    ys = np.linspace(lo[1], hi[1], N)
    X, Y = np.meshgrid(xs, ys)

    def im(a, b, c):
        d = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])
        if abs(d) < 1e-9:
            return np.zeros(X.shape, bool)
        w0 = ((b[0] - X) * (c[1] - Y) - (c[0] - X) * (b[1] - Y)) / d
        w1 = ((c[0] - X) * (a[1] - Y) - (a[0] - X) * (c[1] - Y)) / d
        return (w0 >= 0) & (w1 >= 0) & (1 - w0 - w1 >= 0)
    port = im(q[0], q[1], q[3]) | im(q[0], q[3], q[2])
    # die wahre Vierecksflaeche: konvexe Huelle der vier Ecken = Vereinigung ALLER vier
    # moeglichen Eck-Dreiecke
    huelle = im(q[0], q[1], q[2]) | im(q[0], q[1], q[3]) | im(q[0], q[2], q[3]) | im(q[1], q[2], q[3])
    return port.sum() / max(1, huelle.sum())


def bericht(name, mesh):
    z = {"Z": 0, "UMLAUF": 0, "entartet": 0}
    d = []
    for q in mesh["quads"]:
        p = [mesh["qv"][i] for i in q]
        o = ordnung(p)
        z[o] += 1
        if o != "entartet":
            d.append(deckung(p))
    print("%-36s %3d Vierecke: Z-Ordnung %3d | Umlauf %3d | entartet %d | Deckung Mittel %.3f"
          % (name, len(mesh["quads"]), z["Z"], z["UMLAUF"], z["entartet"],
             float(np.mean(d)) if d else 0))
    return z


def main():
    os.makedirs(ZIEL, exist_ok=True)
    for raum in ("ROOM1150.RDT", "ROOM1170.RDT", "ROOM11F0.RDT"):
        rdt = rdt_laden(raum)
        for p in props(rdt):
            if p["md1"]:
                bericht("%s Prop %d" % (raum[:8], p["idx"]), md1_lesen(p["md1"])["meshes"][0])
    re2 = os.path.join(REPO, "extracted_re2_sicherung", "item_077_fuse_case", "weltmodell.md1")
    bericht("RE2 ROOM60D0 Prop 1 (Fuse Case)", md1_lesen(open(re2, "rb").read())["meshes"][0])
    roh = bytearray(inc_bytes("re15_sicherung_md1"))
    m = md1_lesen(bytes(roh))["meshes"][0]
    bericht("SICHERUNG gen/sicherung_prop.inc", m)

    (tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu) = m["kopf"]
    for k in range(qfc):
        o = 12 + qf + k * 16
        n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from("<8H", roh, o)
        struct.pack_into("<8H", roh, o, n0, v0, n1, v1, n3, v3, n2, v2)
        o = 12 + qu + k * 16
        a = bytes(roh[o + 8:o + 12])
        b = bytes(roh[o + 12:o + 16])
        roh[o + 8:o + 12] = b
        roh[o + 12:o + 16] = a
    bericht("SICHERUNG berichtigt (2 <-> 3)", md1_lesen(bytes(roh))["meshes"][0])
    ziel = os.path.join(ZIEL, "sicherung_zordnung.md1")
    open(ziel, "wb").write(roh)
    print("geschrieben:", os.path.relpath(ziel, REPO), len(roh), "B")


if __name__ == "__main__":
    main()
