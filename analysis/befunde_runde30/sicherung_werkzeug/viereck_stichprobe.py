#!/usr/bin/env python3
"""Stichprobe (Fortsetzungs-Agent): die Ecken EINES Vierecks im Sicherungs-MD1 und EINES
Vierecks im ausgelieferten ROOM1150 Prop 0 ausdrucken, samt Datei-Offset des Face-Records.

    python analysis/befunde_runde30/sicherung_werkzeug/viereck_stichprobe.py
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import inc_bytes, props, rdt_laden  # noqa: E402


def zeig(name, d, basis, k):
    (tv, tvc, tn, tnc, tf, tfc, tu,
     qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, 12)
    o = 12 + qf + k * 16
    n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from("<8H", d, o)
    Q = [struct.unpack_from("<3h", d, 12 + qv + i * 8) for i in (v0, v1, v2, v3)]
    print("%s  Viereck %d, Face-Record @MD1+0x%X%s: %s" %
          (name, k, o, (" = Datei 0x%X" % (basis + o)) if basis else "",
           d[o:o + 16].hex(" ")))
    for i, p in enumerate(Q):
        print("    Ecke %d = %s" % (i, p))
    u = struct.unpack_from("<BBHBBHBBHBBH", d, 12 + qu + k * 16)
    print("    UV   0=(%d,%d) 1=(%d,%d) 2=(%d,%d) 3=(%d,%d)" %
          (u[0], u[1], u[3], u[4], u[6], u[7], u[9], u[10]))


rdt = rdt_laden("ROOM1150.RDT")
p0 = props(rdt)[0]
zeig("ROOM1150 Prop 0 (ausgeliefert)", p0["md1"], p0["md1_off"], 79)
zeig("Sicherung gen/sicherung_prop.inc", inc_bytes("re15_sicherung_md1"), 0, 0)
