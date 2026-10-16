#!/usr/bin/env python3
"""kamera.py - Kamera-Mathematik der RE1.5-Raeume fuer Spur I (Runde 37, feuerloescher).

Python-Fassung von re15_camera_build_view (FUN_80053ca4) — dieselbe Formel wie
re15_port/tools/maske/geom.py build_view (LookAt mit GANZZAHLIGER Normalisierung, H = fov>>7,
sx = 160 + H*vx/vz). Kamerasatz 32 B ab RDT-Zeiger +0x24: u16 flag, u16 fov, s32 ort[3],
s32 ziel[3], u32 pri (tor_massstab.py-Kopf). Gegengeprueft gegen probe_r34n_e_dokumente
`projekt` (Dossier I_feuerloescher.md).

Nur lesend gegenueber allen Originaldaten.
"""
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "maske"))
import geom as G  # noqa: E402

CD = os.path.join(REPO, "re15_port", "shared_assets", "PSX")


def rdt_laden(raum):
    d, st = G.load_rdt(CD, "ROOM%04X" % raum if isinstance(raum, int) else raum)
    if d is None:
        raise SystemExit("RDT fehlt: %s" % raum)
    return d


class Kamera:
    def __init__(self, rdt, cut):
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        rec = cam + cut * 32
        self.fov = struct.unpack_from("<H", rdt, rec + 2)[0]
        self.P = struct.unpack_from("<iii", rdt, rec + 4)
        self.T = struct.unpack_from("<iii", rdt, rec + 16)
        self.R, self.t, self.H = G.build_view(self.fov, *self.P, *self.T)

    def sicht(self, p):
        R, t = self.R, self.t
        x, y, z = p
        vx = (R[0] * x + R[1] * y + R[2] * z) / 4096.0 + t[0]
        vy = (R[3] * x + R[4] * y + R[5] * z) / 4096.0 + t[1]
        vz = (R[6] * x + R[7] * y + R[8] * z) / 4096.0 + t[2]
        return vx, vy, vz

    def bild(self, p):
        vx, vy, vz = self.sicht(p)
        if vz <= 1:
            return None
        return 160.0 + self.H * vx / vz, 120.0 + self.H * vy / vz, vz
