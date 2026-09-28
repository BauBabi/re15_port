"""Kamera-, Projektions- und RDT-Grundlage fuer die Vermessung des Tors in ROOM1170.

Nur LESEND. Jede Formel hier ist die Python-Fassung einer Stelle im Port:

  Blickmatrix   re15_port/engine/src/camera_common.c:65-115  re15_camera_build_view
                (= RE1.5 setupCameraLookAtMatrix FUN_80053ca4), GANZZAHLIG:
                dist/horiz ueber SquareRoot0, sin/cos als abschneidende Division.
  SquareRoot0   re15_port/engine/src/re15_math.c:53-65, Tabelle :22-39 (wird zur
                Laufzeit AUS DIESER DATEI gelesen - eine Quelle, keine Abschrift).
  H             camera_common.c:36-61  fov_to_screen_dist: H = fov >> 7
  Projektion    sx = 160 + H*vx/vz, sy = 120 + H*vy/vz
                (platform/pc/main.c:5277-5278; camera_common.c:13)
  Kamerasatz    re15_port/include/re15_camera.h:28-38 (32 B: u16 flag, u16 fov,
                s32 pos[3], s32 target[3], u32 pri_offset)
  SCA-Zelle     re15_port/include/re15_rdt.h:62-71 (12 B)
  Band -> y     y = -band * 0x708 (tools/maske/geom.py:168, BAND_HOEHE)
"""
import os
import re
import struct

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RDT_PFAD = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "STAGE1", "ROOM1170.RDT")
BG_MUSTER = os.path.join(WURZEL, "extracted", "PSX", "STAGE1", "ROOM117", "ROOM117%02d.bmp")
MATH_C = os.path.join(WURZEL, "re15_port", "engine", "src", "re15_math.c")

BILD_CX, BILD_CY = 160, 120      # platform/pc/main.c:5277-5278
BAND_HOEHE = 0x708               # re15_collision_band_from_y: band = -(Y / 0x708)


# ----------------------------------------------------------------------------
# SquareRoot0 (BIOS 0x80065f60) - Tabelle aus re15_math.c gelesen
# ----------------------------------------------------------------------------
_SQRT_TAB = None


def _sqrt_tab():
    global _SQRT_TAB
    if _SQRT_TAB is None:
        src = open(MATH_C, encoding="utf-8", errors="replace").read()
        m = re.search(r"re15_sqrt0_tab\[192\]\s*=\s*\{(.*?)\};", src, re.S)
        tab = [int(v) for v in re.findall(r"\d+", m.group(1))]
        assert len(tab) == 192 and tab[0] == 4096 and tab[64] == 5792 and tab[191] == 8175, \
            "SquareRoot0-Tabelle nicht wie erwartet (re15_math.c:22-39)"
        _SQRT_TAB = tab
    return _SQRT_TAB


def squareroot0(x):
    """re15_math.c:53-65, bitgleich."""
    x &= 0xFFFFFFFF
    if x == 0:
        return 0
    y = (~x & 0xFFFFFFFF) if (x & 0x80000000) else x
    if y == 0:
        return 0
    lz = 0
    while (y & 0x80000000) == 0:
        y = (y << 1) & 0xFFFFFFFF
        lz += 1
    t2 = lz & ~1
    t1 = (31 - t2) >> 1
    t4 = ((x << (t2 - 24)) & 0xFFFFFFFF) if t2 >= 24 else (x >> (24 - t2))
    return (_sqrt_tab()[t4 - 64] << t1) >> 12


def tdiv(a, b):
    """C-Division (gegen null abschneiden)."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


# ----------------------------------------------------------------------------
# Kamera
# ----------------------------------------------------------------------------
class Kamera(object):
    """Ein Cut. R = 3x3 (float, bereits /4096), t = 3 (Welteinheiten), H = fov>>7."""

    def __init__(self, cut, off, roh, exakt=False):
        self.cut = cut
        self.off = off
        self.roh = bytes(roh)
        self.flag, self.fov = struct.unpack_from("<HH", roh, 0)
        self.pos = struct.unpack_from("<iii", roh, 4)
        self.ziel = struct.unpack_from("<iii", roh, 16)
        self.pri = struct.unpack_from("<I", roh, 28)[0]
        self.H = self.fov >> 7
        self.exakt = exakt
        if exakt:
            self._bau_ideal()
        else:
            self._bau_port()

    def _bau_port(self):
        px, py, pz = self.pos
        tx, ty, tz = self.ziel
        dx, dy, dz = tx - px, ty - py, tz - pz
        # (uint32_t)-Kuerzung wie im Port (camera_common.c:83/85)
        dist = squareroot0((dx * dx + dy * dy + dz * dz) & 0xFFFFFFFF)
        horiz = squareroot0((dx * dx + dz * dz) & 0xFFFFFFFF)
        self.dist_q, self.horiz_q = dist, horiz
        sp = s16(tdiv(-dy * 4096, dist))
        cp = s16(tdiv(horiz * 4096, dist))
        sy = s16(tdiv(dx * 4096, horiz))
        cy = s16(tdiv(dz * 4096, horiz))
        self.sp, self.cp, self.sy, self.cy = sp, cp, sy, cy
        Ri = [cy, 0, -sy,
              (sp * sy) >> 12, cp, (sp * cy) >> 12,
              (cp * sy) >> 12, -sp, (cp * cy) >> 12]
        ti = [(Ri[0] * -px + Ri[1] * -py + Ri[2] * -pz) >> 12,
              (Ri[3] * -px + Ri[4] * -py + Ri[5] * -pz) >> 12,
              (Ri[6] * -px + Ri[7] * -py + Ri[8] * -pz) >> 12]
        self.Ri, self.ti = Ri, ti
        self.R = np.array(Ri, float).reshape(3, 3) / 4096.0
        self.t = np.array(ti, float)
        # Kameraort, wie ihn DIESE Matrix sieht (nicht exakt self.pos, weil R nicht
        # exakt orthonormal ist)
        self.Rinv = np.linalg.inv(self.R)
        self.ort = self.Rinv.dot(-self.t)

    def _bau_ideal(self):
        """Dieselbe Konstruktion in Gleitkomma - nur als GEGENPROBE, wie gross der
        Ganzzahlfehler der Port-Matrix ist."""
        p = np.array(self.pos, float)
        d = np.array(self.ziel, float) - p
        dist = np.linalg.norm(d)
        horiz = np.hypot(d[0], d[2])
        sp, cp = -d[1] / dist, horiz / dist
        sy, cy = d[0] / horiz, d[2] / horiz
        self.R = np.array([[cy, 0, -sy],
                           [sp * sy, cp, sp * cy],
                           [cp * sy, -sp, cp * cy]])
        self.t = self.R.dot(-p)
        self.Rinv = self.R.T
        self.ort = p

    # -- Welt -> Bild --------------------------------------------------------
    def sicht(self, w):
        w = np.asarray(w, float)
        return w @ self.R.T + self.t

    def bild(self, w):
        """Welt (…,3) -> (sx, sy, vz). Keine Rundung: Messwerkzeug, nicht Zeichner."""
        v = self.sicht(w)
        vz = v[..., 2]
        with np.errstate(divide="ignore", invalid="ignore"):
            sx = BILD_CX + self.H * v[..., 0] / vz
            sy = BILD_CY + self.H * v[..., 1] / vz
        return sx, sy, vz

    # -- Bild -> Welt --------------------------------------------------------
    def strahl(self, sx, sy):
        """Richtung des Sehstrahls durch den Bildpunkt (sx, sy) in Weltkoordinaten."""
        d = np.array([sx - BILD_CX, sy - BILD_CY, float(self.H)])
        return self.Rinv.dot(d)

    def auf_ebene(self, sx, sy, n, c):
        """Schnitt des Sehstrahls mit der Ebene n.w = c -> (Weltpunkt, vz) oder None."""
        n = np.asarray(n, float)
        d = self.strahl(sx, sy)
        nd = n.dot(d)
        if abs(nd) < 1e-12:
            return None
        s = (c - n.dot(self.ort)) / nd
        if s <= 0:
            return None
        w = self.ort + s * d
        return w, float(self.sicht(w)[2])


def lade_rdt(pfad=RDT_PFAD):
    return open(pfad, "rb").read()


def kamera(rdt, cut, exakt=False):
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    off = cam + 32 * cut
    return Kamera(cut, off, rdt[off:off + 32], exakt=exakt)


# ----------------------------------------------------------------------------
# SCA
# ----------------------------------------------------------------------------
def sca_zellen(rdt):
    """-> (kopf, [dict]) - alle Eintraege mit Datei-Offset und rohen Bytes."""
    s = struct.unpack_from("<I", rdt, 0x20)[0]
    cx, cz = struct.unpack_from("<HH", rdt, s)
    cnt = struct.unpack_from("<5I", rdt, s + 4)
    aus = []
    for i in range(sum(cnt)):
        o = s + 24 + 12 * i
        w, d, x, z, typ, u0, u1, flr = struct.unpack_from("<HHhhBBBB", rdt, o)
        aus.append(dict(i=i, off=o, roh=rdt[o:o + 12].hex(" "), w=w, d=d, x=x, z=z,
                        typ=typ, u0=u0, u1=u1, floor=flr, band=flr >> 4))
    return dict(off=s, ceil=(cx, cz), counts=cnt), aus


def sca_eindeutig(rdt):
    """Die fuenf Gruppen tragen in ROOM1170 dieselben 30 Zellen - nur die erste Gruppe."""
    kopf, z = sca_zellen(rdt)
    gesehen, aus = set(), []
    for c in z:
        k = (c["w"], c["d"], c["x"], c["z"], c["typ"], c["u0"], c["u1"], c["floor"])
        if k in gesehen:
            continue
        gesehen.add(k)
        aus.append(c)
    return kopf, aus


# ----------------------------------------------------------------------------
# Hintergrund
# ----------------------------------------------------------------------------
def _bg_pfad(cut):
    """extracted/ ist unversioniert (Java-Extraktor) und liegt nur im Hauptbaum. Aus einem
    Arbeitsbaum heraus dorthin zurueckfallen: git-common-dir/.. = Wurzel des Hauptbaums."""
    p = BG_MUSTER % cut
    if os.path.exists(p):
        return p
    import subprocess
    try:
        common = subprocess.check_output(["git", "-C", WURZEL, "rev-parse", "--git-common-dir"],
                                         text=True).strip()
        haupt = os.path.dirname(os.path.abspath(os.path.join(WURZEL, common)))
        q = os.path.join(haupt, "extracted", "PSX", "STAGE1", "ROOM117", "ROOM117%02d.bmp" % cut)
        if os.path.exists(q):
            return q
    except Exception:
        pass
    return p


def hintergrund(cut):
    from PIL import Image
    return np.asarray(Image.open(_bg_pfad(cut)).convert("RGB"), np.uint8)
