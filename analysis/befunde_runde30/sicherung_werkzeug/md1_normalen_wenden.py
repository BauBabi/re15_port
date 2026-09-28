#!/usr/bin/env python3
"""Schreibt build/r30_sicherung/sicherung_zord_normal.md1 = die auf Z-Ordnung berichtigte
Sicherung (sicherung_zordnung.md1) mit GEWENDETEN Normalen (nach aussen).

Dazu die Gegenprobe an konvexen Original-Props: zeigen DEREN Normalen nach aussen?

    python analysis/befunde_runde30/sicherung_werkzeug/md1_normalen_wenden.py
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import REPO, props, rdt_laden  # noqa: E402

Z = os.path.join(REPO, "build", "r30_sicherung")


def zaehlen(name, d):
    (tv, tvc, tn, tnc, tf, tfc, tu,
     qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, 12)
    V = np.array([struct.unpack_from("<3h", d, 12 + qv + i * 8) for i in range(qvc)], float)
    N = np.array([struct.unpack_from("<3h", d, 12 + qn + i * 8) for i in range(qnc)], float)
    aus = ein = 0
    # je Viereck: Flaechenmitte relativ zum Schwerpunkt des Modells gegen die Eck-Normalen
    c = V.mean(0)
    for i in range(qfc):
        f = struct.unpack_from("<8H", d, 12 + qf + i * 16)
        m = np.mean([V[f[2 * k + 1]] for k in range(4)], 0) - c
        for k in range(4):
            s = float(N[f[2 * k]] @ m)
            if s > 0:
                aus += 1
            elif s < 0:
                ein += 1
    print("%-40s Eck-Normalen gegen (Flaechenmitte - Schwerpunkt): aussen %3d | innen %3d"
          % (name, aus, ein))


rdt = rdt_laden("ROOM1150.RDT")
pr = props(rdt)
zaehlen("ROOM1150 Prop 1 (Deckelhaelfte)", pr[1]["md1"])
zaehlen("ROOM1150 Prop 2 (Deckelhaelfte)", pr[2]["md1"])
re2 = os.path.join(REPO, "extracted_re2_sicherung", "item_077_fuse_case", "weltmodell.md1")
zaehlen("RE2 ROOM60D0 Prop 1 (Fuse Case)", open(re2, "rb").read())
re2 = os.path.join(REPO, "extracted_re2_sicherung", "item_076_main_fuse", "weltmodell.md1")
zaehlen("RE2 ROOM60D0 Prop 3 (Main Fuse)", open(re2, "rb").read())
d = bytearray(open(os.path.join(Z, "sicherung_zordnung.md1"), "rb").read())
zaehlen("Sicherung (Z-Ordnung, Normalen Bestand)", bytes(d))
(tv, tvc, tn, tnc, tf, tfc, tu,
 qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, 12)
assert tn == qn and tnc == qnc, "geteilte Normalenliste erwartet"
for i in range(qnc):
    o = 12 + qn + i * 8
    x, y, z, w = struct.unpack_from("<4h", d, o)
    struct.pack_into("<4h", d, o, -x, -y, -z, w)
zaehlen("Sicherung (Z-Ordnung, Normalen GEWENDET)", bytes(d))
ziel = os.path.join(Z, "sicherung_zord_normal.md1")
open(ziel, "wb").write(d)
print("geschrieben:", ziel, len(d), "B")
