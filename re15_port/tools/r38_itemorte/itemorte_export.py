#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur K "itemorte": Feuerloescher nach ROOM1030, Parking Lot Key / Prison Key an die Pinnwand ROOM10F0/10F1.

Erzeugt re15_port/engine/src/gen/itemorte_r38.inc (KEIN Asset-Patch, shared_assets bleibt unangetastet).
Dossier: analysis/befunde_runde38/K_itemorte.md.

NUTZER-VORGABE (AUFTRAG.md Runde 38, woertlich):
  "Ich haette den fire extinguisher gerne in room 1030 haengen an der stelle von fire_extinguisher.bmp, anstatt in
   room 1050."
  "Ich moechte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location
   platzierst, die ich im Bild key_location.bmp markiert habe."

WAS GEMESSEN IST (Dossier Abschnitt 1/2, alle Zahlen dort mit Herkunft):
  * Cut der Bilder: Bildvergleich gegen ALLE 428 STAGE1-Hintergruende (Engine-Dekoder probe_bg_dump), mittlere
    Abweichung je Pixel ausserhalb der Marke: fire_extinguisher.bmp = ROOM1030 Cut 0 (15,94; Cut 9 = dieselbe
    Kamera 22,29; Rest >= 62), key_location.bmp = ROOM10F0 Cut 5 (4,34; Rest >= 90,9). Marken-Huelle hier aus den
    BMP gelesen (exakte Markenfarbe (237,28,36), wie re15_port/tools/r34n_e/marken.py).
  * Kameras: `probe_r34n_e_dokumente kamera 1030|10F0` (VIEW-Zeilen = re15_camera_build_view, FUN_80053ca4).
  * ROOM1030 Pfeiler: Kanten trianguliert (Cut 0/1/3, Strahlen der Engine-Matrix, Senkrechte als Gerade in der
    Draufsicht): (-x,+z)-Ecke (-2668,-17564), (-x,-z)-Ecke (-2709,-18459) (5 Beobachtungen, Rest <= 8),
    (+x,+z)-Ecke (-1790,-17606). Die Marke liegt auf der -x-Flaeche (Bildspalten 35,3..49,4 in Cut 0; die +z-Flaeche
    mit dem Aushang liegt bei 24..35), unter dem Notruf-Monitor (ROOM1030 msg 14 "...emergency broadcast...",
    Monitor-Anzeige trianguliert (-3225,-3940,-18026) aus Cut 0/1/2).
  * ROOM10F0 Pinnwand: Rueckwand z = -3400 (SCA-Zelle 0 x -600..11300 z -5400..-3400; Zettel der Pinnwand aus
    Cut 5 + Cut 6 trianguliert z -3481 / -3352).

PORT-WAHL (die FORM ist jeweils belegt, die Zahlen kommen aus der Messung):
  FEUERLOESCHER: dasselbe stehende Netz wie Runde 37 (gen/feuerloescher_r37.inc, Masse aus dem Netz gelesen:
    Hoehe 792, Radius 110), dieselbe Wandregel wie ROOM1050 (Achse R + 15 vor der Wand, Runde 37 Dossier 3.2),
    Etikett zum Raum (rot_y so, dass Modell -z in die Wand zeigt). MITTE = Sehstrahl der Markenmitte geschnitten
    mit der Achsenebene (Runde 36 N: "Mitte = Sehstrahl der Markenmitte x Ebene"); Groesse NICHT an die Marke
    angepasst (es ist derselbe Loescher, den Leon spaeter in ROOM1090 ablegt).
  SCHLUESSEL: flach an der Pinnwand (Modellebene parallel zur Wand). Sichtseite = Wicklungs-Vorderseite = die Seite,
    auf der die Textur NICHT gespiegelt erscheint (P. Room Key +y, Cabin Key -y: Rechnung im Dossier 2.6), Textur-u
    waagerecht nach rechts (wie im TIM gezeichnet; P. Room Key Kopf rechts wie im RE2-Itembild ITPS 95).
    "gut sichtbar" = NUTZER-VORGABE; MASSSTAB s = groesster Wert, bei dem alle SICHTBAREN Texel (CLUT-Farbe != 0)
    im Nutzer-Cut innerhalb der Marken-Huelle liegen (+-0,5 / +1,5 px wie karten_export.py, Runde 36 N 3.3),
    Mitte des sichtbaren Rechtecks auf dem Sehstrahl der Markenmitte. Das MD1 wird gleichmaessig skaliert und in der
    Modellebene verschoben (nur die Ecken @0x44, Normalen/UV/Topologie unveraendert — wie karten_export.py).

Aufruf: python re15_port/tools/r38_itemorte/itemorte_export.py [--schreiben]
"""
import hashlib
import math
import os
import re
import struct
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RE2 = os.path.join(REPO, "info", "re2leon")
NUTZER = os.path.join(REPO, "analysis", "befunde_runde38", "nutzer")
FL37_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "feuerloescher_r37.inc")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "itemorte_r38.inc")

# ---- Kameras: `probe_r34n_e_dokumente kamera <raum>` VIEW-Zeilen (R Q12 zeilenweise, T, H) -----------------------
CAMS = {
    (0x1030, 0): ((-3810, 0, -1515, -102, 4084, 255, 1510, 275, -3799), (-15554, 3393, -4125), 208),
    (0x1030, 1): ((3508, 0, -2147, -180, 4078, -294, 2137, 343, 3492), (-334, 863, 26824), 208),
    (0x1030, 2): ((-1520, 0, -3823, -718, 4027, 285, 3758, 769, -1495), (-13985, 2547, 3264), 208),
    (0x1030, 3): ((-3946, 0, 1099, 89, 4096, 320, -1099, 333, -3946), (4697, 3905, -9501), 208),
    (0x10F0, 5): ((-4082, 0, 353, 88, 3992, 1025, -345, 1029, -3979), (303, 847, 10437), 208),
    (0x10F0, 6): ((-3805, 0, 1553, 536, 3833, 1314, -1454, 1415, -3561), (-539, 1151, 7613), 208),
    (0x10F0, 7): ((-3738, 0, -1681, -538, 3879, 1195, 1591, 1310, -3540), (-3804, 945, 9768), 208),
}
MARKENFARBE = (237, 28, 36)

# ---- ROOM1030: Pfeiler (Dossier 2.2) -----------------------------------------------------------------------------
PFEILER_XA_ZB = (-2668, -17564)      # (-x,+z)-Ecke: Cut 3 Spalte 228,6 + Cut 1 Spalte 296,6
PFEILER_XA_ZA = (-2709, -18459)      # (-x,-z)-Ecke: Cut 0 Spalte 49,4 + Cut 1 Spalten 318,3/317,5/314,6/313,8
FLAECHE_X = int(round((PFEILER_XA_ZB[0] + PFEILER_XA_ZA[0]) / 2.0))   # -x-Flaeche, achsparallel (Mittel der Ecken)
WAND_ABSTAND_ZUSATZ = 15             # Runde 37 Dossier 3.2: "Prop z -23775 (R + 15 vor der Wand)"
FL_ROT_Y = 3072                      # Modell -z (Wandseite) -> Welt +x (in den Pfeiler), Etikett +z -> Welt -x (Raum)
# Aufhebe-Rechteck: Klemmgrenze des Spielers vor den Tischen x = -4100 (probe_r34n_e_dokumente stand 1030
# -6000 -1000 -19000 -16800 100: begehbar x[..-4100] fuer z -19000..-16800), Pruefpunkt 620 voraus (@0x80042bd0
# `ori v0,zero,0x26c`) -> Blick +x landet bei x -3480; Rechteck 1000 x 1000 (r35-Muster, 69 von 162 Item_aot_set)
# mit dieser Mitte. z: um die Loescher-Mitte, aber nicht in Slot 13 "Desks..." (z bis -18372, ROOM1030 @0x01D92;
# der kleinere Slot gewaenne dort, `j 0x80043028` @0x80042f94).
KLEMME_X = -4100
VORAUS = 620
SLOT13_Z1 = -18372

# ---- ROOM10F0: Pinnwand (Dossier 2.3) ---------------------------------------------------------------------------
WAND_Z = -3400                       # SCA-Zelle 0 (+z-Flaeche), Zettel trianguliert -3481/-3352
ZONE_VOR_WAND = 300                  # Runde 36 N (karten_export.py): "Aufhebe-Rechteck 1000 x 1000 um Mitte + 300*n"
SLOT12_X0 = 1800                     # ROOM10F0 Slot 12 (30 Patronen, Item_aot_set @0x01028) beginnt bei x 1800: das
                                     # Rechteck endet davor (sonst naehme der kleinere Slot 8 dort die Patronen-Zone)

# ---- RE2-Schluessel (wie re15_port/tools/r36_schluessel/schluessel_export.py, md5-geprueft) -----------------------
SCHLUESSEL = {
    "park":   dict(item=0x45, rdt="PL0/RDT/ROOM6150.RDT", md1=(0x004FBC, 236, "3c9176ee9bbb7e91b7c9433d6fe5c371"),
                   tim=(0x030B74, 34848, "0d43d30f076929dbc396075df3e051f6")),
    "prison": dict(item=0x42, rdt="PL0/RDT/ROOM1050.RDT", md1=(0x001558, 140, "569def980f500800614301d40744b741"),
                   tim=(0x00C650, 8736, "be1f46e9490e070787f6e33fcae8faef")),
}
VERT_OFF = 0x44                      # MD1: Ecken ab Datei 12 + t_vertex_offset 56 (beide Schluessel)


def marke(datei):
    a = np.array(Image.open(os.path.join(NUTZER, datei)).convert("RGB")).astype(int)
    m = (a[:, :, 0] == MARKENFARBE[0]) & (a[:, :, 1] == MARKENFARBE[1]) & (a[:, :, 2] == MARKENFARBE[2])
    ys, xs = np.nonzero(m)
    x0, x1, y0, y1 = int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())
    assert len(xs) == (x1 - x0 + 1) * (y1 - y0 + 1), "Marke ist kein volles Rechteck"
    return x0, y0, x1, y1


def kamera(raum, cut):
    R, T, H = CAMS[(raum, cut)]
    return np.array(R, float).reshape(3, 3) / 4096.0, np.array(T, float), float(H)


def projiziere(raum, cut, p):
    R, T, H = kamera(raum, cut)
    v = R @ np.asarray(p, float) + T
    return 160.0 + H * v[0] / v[2], 120.0 + H * v[1] / v[2], v[2]


def strahl(raum, cut, sx, sy):
    R, T, H = kamera(raum, cut)
    Ri = np.linalg.inv(R)
    return Ri @ (-T), Ri @ np.array([(sx - 160.0) / H, (sy - 120.0) / H, 1.0])


def auf_ebene(raum, cut, sx, sy, n, c):
    o, d = strahl(raum, cut, sx, sy)
    return o + (c - n @ o) / (n @ d) * d


def euler(m):
    """Zerlegung nach pc_prop_rot_q12 (platform/pc/main.c, Ry*Rx*Rz) — wie karten_export.py."""
    sy = max(-1.0, min(1.0, m[0, 2]))
    ry = math.asin(sy)
    if abs(math.cos(ry)) > 1e-9:
        rx = math.atan2(-m[1, 2], m[2, 2])
        rz = math.atan2(-m[0, 1], m[0, 0])
    else:
        rx = 0.0
        rz = math.atan2(m[1, 0], m[1, 1])
    return rx, ry, rz


def wieder(rx, ry, rz):
    sx, cx, sy, cy, sz, cz = math.sin(rx), math.cos(rx), math.sin(ry), math.cos(ry), math.sin(rz), math.cos(rz)
    return np.array([[cz * cy, -sz * cy, sy],
                     [sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx],
                     [sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]])


def q12(a):
    return int(round(a / (2 * math.pi) * 4096)) & 0xFFF


def q12_matrix(rx, ry, rz):
    f = lambda a: a / 4096.0 * 2 * math.pi
    return wieder(f(rx), f(ry), f(rz))


# ---- MD1 / TIM lesen ----------------------------------------------------------------------------------------------
def md1_lesen(b):
    n_obj = struct.unpack_from("<I", b, 8)[0]
    assert n_obj == 2, "ein Netz erwartet"
    h = struct.unpack_from("<14I", b, 12)
    tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = h
    assert tvo == qvo == VERT_OFF - 12, "Ecken nicht bei @0x44"
    V = [struct.unpack_from("<4h", b, 12 + tvo + 8 * i)[:3] for i in range(tvc)]
    faces = []
    for i in range(tfc):
        f = struct.unpack_from("<6H", b, 12 + tfo + 12 * i)
        uv = struct.unpack_from("<BBHBBHBB", b, 12 + tuo + 12 * i)
        faces.append(([f[1], f[3], f[5]], [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], uv[2]))
    for i in range(qfc):
        f = struct.unpack_from("<8H", b, 12 + qfo + 16 * i)
        uv = struct.unpack_from("<BBHBBHBBHBB", b, 12 + quo + 16 * i)
        faces.append(([f[1], f[3], f[5], f[7]], [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])], uv[2]))
    return V, faces, tvc


def tim_texel_sichtbar(tb, clut_feld):
    """Sichtbarkeit je Texel (8 bpp): CLUT-Farbe != 0x0000 (PSX: 0x0000 = durchsichtig)."""
    magic, flags = struct.unpack_from("<II", tb, 0)
    assert flags & 8 and (flags & 3) == 1, "8bpp mit CLUT erwartet"
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", tb, 8)
    clut = np.frombuffer(tb[8 + 12:8 + 12 + cw * ch * 2], dtype="<u2").reshape(ch, cw)
    o = 8 + ln
    _, px, py, pw, ph = struct.unpack_from("<IHHHH", tb, o)
    idx = np.frombuffer(tb[o + 12:o + 12 + pw * ph * 2], dtype=np.uint8).reshape(ph, pw * 2)
    zeile = (clut_feld >> 6) - cy
    assert 0 <= zeile < ch and (clut_feld & 0x3F) * 16 == cx, "CLUT-Feld passt nicht zum TIM"
    return clut[zeile][idx] != 0


def affin_uv_nach_modell(faces, V):
    """(u,v) -> Modell (x,z) als affine Abbildung aus allen Ecken (kleinste Quadrate, Rest geprueft)."""
    A, B = [], []
    for vi, uvs, _c in faces:
        for i, (u, v) in zip(vi, uvs):
            A.append([u, v, 1.0]); B.append([V[i][0], V[i][2]])
    A = np.array(A); B = np.array(B)
    X, res, *_ = np.linalg.lstsq(A, B, rcond=None)
    rest = np.abs(A @ X - B).max()
    assert rest < 1.0, "UV-Abbildung nicht affin (Rest %.2f)" % rest
    return X       # [u v 1] @ X = [x z]


def schluessel_rechnen(name, raum, cut, mk):
    q = SCHLUESSEL[name]
    rdt = open(os.path.join(RE2, q["rdt"]), "rb").read()
    off, n, md5 = q["md1"]; md1 = rdt[off:off + n]
    assert hashlib.md5(md1).hexdigest() == md5, "MD1 md5 " + name
    off, n, md5 = q["tim"]; tim = rdt[off:off + n]
    assert hashlib.md5(tim).hexdigest() == md5, "TIM md5 " + name
    V, faces, nv = md1_lesen(md1)
    assert all(abs(v[1]) == 0 for v in V), "Schluessel-Netz ist nicht flach (y != 0)"
    clut_feld = faces[0][2]
    sicht = tim_texel_sichtbar(tim, clut_feld)
    X = affin_uv_nach_modell(faces, V)
    # u-Richtung im Modell (x,z): d(x,z)/du = X[0]; v-Richtung = X[1]
    du = np.array([X[0][0], 0.0, X[0][1]]); dv = np.array([X[1][0], 0.0, X[1][1]])
    # Sichtseite: die Seite (+y / -y), von der aus die Textur NICHT gespiegelt erscheint. Blick von -y (oben):
    # Bild-rechts = +x, Bild-unten = -z; von +y (unten): Bild-rechts = +x, Bild-unten = +z. Nicht gespiegelt heisst
    # det [[du/dsx, du/dsy],[dv/dsx, dv/dsy]] > 0 (Textur-v waechst nach Bild-unten).
    J = np.array([[X[0][0], X[0][1]], [X[1][0], X[1][1]]])        # (x,z) je (u,v)
    Jinv = np.linalg.inv(J.T)                                      # (u,v) je (x,z)
    det_oben = np.linalg.det(Jinv @ np.array([[1.0, 0.0], [0.0, -1.0]]))
    seite_y = -1.0 if det_oben > 0 else 1.0                        # Modell-Achse, die zum Raum zeigt
    # Wicklung der Flaechen von dieser Seite (Kreuzprodukt wie IS_BACKFACE, platform/pc/main.c)
    wick = []
    for vi, _uv, _c in faces:
        p = [(V[i][0], -V[i][2] if seite_y < 0 else V[i][2]) for i in vi[:3]]
        wick.append((p[1][0] - p[0][0]) * (p[2][1] - p[0][1]) - (p[1][1] - p[0][1]) * (p[2][0] - p[0][0]))
    # Drehung: u -> Bild-rechts (Welt -x vor der Wand z = -3400, Blick -z), Sichtseite -> Welt +z (zum Raum)
    eu = du / np.linalg.norm(du)
    ey = np.array([0.0, seite_y, 0.0])
    ew = np.cross(eu, ey)
    Bm = np.column_stack([eu, ey, ew])
    A_w = np.column_stack([np.array([-1.0, 0, 0]), np.array([0.0, 0, 1.0]), np.cross([-1.0, 0, 0], [0.0, 0, 1.0])])
    M = A_w @ Bm.T
    assert abs(np.linalg.det(M) - 1.0) < 1e-9
    rx, ry, rz = euler(M)
    RX, RY, RZ = q12(rx), q12(ry), q12(rz)
    Mq = q12_matrix(RX, RY, RZ)
    # sichtbare Texel -> Modell (x,z), Ecken je Texel; Huelle in Modellkoordinaten
    us, vs = [], []
    for vi, uvs, _c in faces:
        u0 = min(u for u, v in uvs); u1 = max(u for u, v in uvs); v0 = min(v for u, v in uvs); v1 = max(v for u, v in uvs)
        for vv in range(v0, v1 + 1):
            for uu in range(u0, u1 + 1):
                if vv < sicht.shape[0] and uu < sicht.shape[1] and sicht[vv, uu]:
                    us.append(uu); vs.append(vv)
    pts = set(zip(us, vs))
    ecken = []
    for (uu, vv) in pts:
        for du_, dv_ in ((0, 0), (1, 0), (0, 1), (1, 1)):
            x, z = np.array([uu + du_, vv + dv_, 1.0]) @ X
            ecken.append((x, z))
    ecken = np.array(ecken)
    # nur Texel INNERHALB der Flaechen zaehlen (der UV-Bereich der Flaechen ist das Rechteck/Parallelogramm)
    xs, zs = ecken[:, 0], ecken[:, 1]
    vx = [v[0] for v in V]; vz = [v[2] for v in V]
    innen = (xs >= min(vx) - 1e-6) & (xs <= max(vx) + 1e-6) & (zs >= min(vz) - 1e-6) & (zs <= max(vz) + 1e-6)
    ecken = ecken[innen]
    mx = (ecken[:, 0].min() + ecken[:, 0].max()) / 2.0
    mz = (ecken[:, 1].min() + ecken[:, 1].max()) / 2.0
    x0, y0, x1, y1 = mk
    n = np.array([0.0, 0.0, 1.0]); c = float(WAND_Z)
    mitte = auf_ebene(raum, cut, (x0 + x1 + 1) / 2.0, (y0 + y1 + 1) / 2.0, n, c)

    def welt(s, ex, ez):
        return mitte + Mq @ np.array([(ex - mx) * s, 0.0, (ez - mz) * s])

    def passt(s):
        for ex, ez in ecken:
            sx, sy, _ = projiziere(raum, cut, welt(s, ex, ez))
            if sx < x0 - 0.5 or sx > x1 + 1.5 or sy < y0 - 0.5 or sy > y1 + 1.5:
                return False
        return True
    lo, hi = 0.01, 20.0
    for _ in range(60):
        mi = (lo + hi) / 2
        if passt(mi):
            lo = mi
        else:
            hi = mi
    s = lo
    # skaliertes + verschobenes MD1 (nur die Ecken @0x44)
    sk = bytearray(md1)
    neu = []
    for i, v in enumerate(V):
        nx = int(round((v[0] - mx) * s)); nz = int(round((v[2] - mz) * s))
        assert -32768 <= nx <= 32767 and -32768 <= nz <= 32767
        struct.pack_into("<3h", sk, VERT_OFF + 8 * i, nx, 0, nz)
        neu.append((nx, 0, nz))
    # Kontrolle mit den GERUNDETEN Werten: sichtbare Huelle im Bild
    pos = np.round(mitte).astype(int)
    sxs, sys_ = [], []
    for ex, ez in ecken:
        w = pos + Mq @ np.array([(ex - mx) * s, 0.0, (ez - mz) * s])
        sx, sy, _ = projiziere(raum, cut, w)
        sxs.append(sx); sys_.append(sy)
    breite = (ecken[:, 0].max() - ecken[:, 0].min()) * s
    tiefe = (ecken[:, 1].max() - ecken[:, 1].min()) * s
    return dict(name=name, item=q["item"], md1=bytes(sk), md1_md5=hashlib.md5(md1).hexdigest(), s=s, pos=pos,
                rot=(RX, RY, RZ), seite_y=seite_y, wick=wick, mitte=mitte, sichtbar=len(pts),
                huelle=(min(sxs), max(sxs), min(sys_), max(sys_)), ecken_neu=neu, breite=breite, tiefe=tiefe,
                det_oben=det_oben)


def loescher_rechnen(mk):
    src = open(FL37_INC).read()
    m = re.search(r"k_fl_md1_stehend\[\d*\]\s*=\s*\{(.*?)\};", src, re.S)
    b = bytes(int(x, 16) for x in re.findall(r"0x[0-9a-fA-F]+", m.group(1)))
    h = struct.unpack_from("<14I", b, 12)
    V = [struct.unpack_from("<4h", b, 12 + h[0] + 8 * i)[:3] for i in range(h[1])]
    hoehe = -min(v[1] for v in V)
    radius = max(abs(v[2]) for v in V)
    assert max(v[1] for v in V) == 0, "Ursprung nicht Bodenmitte"
    achse_x = FLAECHE_X - (radius + WAND_ABSTAND_ZUSATZ)
    x0, y0, x1, y1 = mk
    c = auf_ebene(0x1030, 0, (x0 + x1 + 1) / 2.0, (y0 + y1 + 1) / 2.0, np.array([1.0, 0, 0]), float(achse_x))
    urs = np.array([achse_x, c[1] + hoehe / 2.0, c[2]])
    pos = np.round(urs).astype(int)
    fp = KLEMME_X + VORAUS
    rx0 = fp - 500
    rz0 = max(int(round(c[2])) - 500, SLOT13_Z1 + 2)
    # Kontrolle: Netz mit rot_y projizieren -> Huelle im Nutzer-Cut
    M = q12_matrix(0, FL_ROT_Y, 0)
    sxs, sys_ = [], []
    for v in V:
        sx, sy, _ = projiziere(0x1030, 0, pos + M @ np.array(v, float))
        sxs.append(sx); sys_.append(sy)
    return dict(hoehe=hoehe, radius=radius, achse_x=achse_x, mitte=c, pos=pos, rot_y=FL_ROT_Y,
                rect=(rx0, rz0, 1000, 1000), huelle=(min(sxs), max(sxs), min(sys_), max(sys_)), n_ecken=len(V))


def main():
    schreiben = "--schreiben" in sys.argv
    mk_fl = marke("fire_extinguisher.bmp")
    mk_ks = marke("key_location.bmp")
    print("Marke fire_extinguisher.bmp x%d..%d y%d..%d (ROOM1030 Cut 0)" % (mk_fl[0], mk_fl[2], mk_fl[1], mk_fl[3]))
    print("Marke key_location.bmp      x%d..%d y%d..%d (ROOM10F0 Cut 5)" % (mk_ks[0], mk_ks[2], mk_ks[1], mk_ks[3]))
    fl = loescher_rechnen(mk_fl)
    print("LOESCHER: Netz Hoehe %d Radius %d (%d Ecken) | Pfeiler -x-Flaeche x %d -> Achse x %d | Mitte (%.0f,%.0f,%.0f)"
          % (fl["hoehe"], fl["radius"], fl["n_ecken"], FLAECHE_X, fl["achse_x"], *fl["mitte"]))
    print("          Ursprung (Bodenmitte) (%d,%d,%d) rot_y %d | Huelle Cut 0 x %.1f..%.1f y %.1f..%.1f | Rechteck (%d,%d) %dx%d"
          % (*fl["pos"], fl["rot_y"], *fl["huelle"], *fl["rect"]))
    ks = []
    for name in ("park", "prison"):
        k = schluessel_rechnen(name, 0x10F0, 5, mk_ks)
        ks.append(k)
        print("SCHLUESSEL %-6s Item 0x%02X: Sichtseite Modell %+dy (Wicklung %s, det_oben %.3f) | %d sichtbare Texel | s %.4f"
              " -> sichtbar %.0f x %.0f | Ursprung (%d,%d,%d) rot (%d,%d,%d) | Huelle Cut 5 x %.2f..%.2f y %.2f..%.2f"
              % (name, k["item"], int(k["seite_y"]), ["%.0f" % w for w in k["wick"]], k["det_oben"], k["sichtbar"],
                 k["s"], k["breite"], k["tiefe"], *k["pos"], *k["rot"], *k["huelle"]))
        print("           Ecken neu", k["ecken_neu"])
    rx1 = min(int(round(ks[0]["pos"][0])) + 500, SLOT12_X0 - 2)
    rect_ks = (rx1 - 1000, WAND_Z + ZONE_VOR_WAND - 500)
    print("SCHLUESSEL-Zone Rechteck (%d,%d) 1000x1000 (Mitte (x_Marke, %d + %d), rechts vor Slot 12 x %d)"
          % (rect_ks[0], rect_ks[1], WAND_Z, ZONE_VOR_WAND, SLOT12_X0))
    if not schreiben:
        return

    def blob(name, b):
        out = ["static const unsigned char %s[%d] = {" % (name, len(b))]
        for i in range(0, len(b), 16):
            out.append("    " + ",".join("0x%02x" % v for v in b[i:i + 16]) + ",")
        out.append("};")
        return "\n".join(out)

    L = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r38_itemorte/itemorte_export.py --schreiben - nicht von Hand.",
         " * Runde 38 Spur K (Dossier analysis/befunde_runde38/K_itemorte.md). Herleitung jeder Zahl: Kopf des Skripts.",
         " * Nutzer-Marken: fire_extinguisher.bmp x%d..%d y%d..%d (ROOM1030 Cut 0), key_location.bmp x%d..%d y%d..%d"
         % (mk_fl[0], mk_fl[2], mk_fl[1], mk_fl[3], mk_ks[0], mk_ks[2], mk_ks[1], mk_ks[3]),
         " * (ROOM10F0 Cut 5). */",
         "",
         "/* FEUERLOESCHER ROOM1030 (stehendes Netz Runde 37: Hoehe %d, Radius %d): Pfeiler -x-Flaeche x %d, Achse x %d"
         % (fl["hoehe"], fl["radius"], FLAECHE_X, fl["achse_x"]),
         " * (R + %d), Mitte auf dem Sehstrahl der Markenmitte (%.0f,%.0f,%.0f); Huelle im Nutzer-Cut x %.1f..%.1f y %.1f..%.1f."
         % (WAND_ABSTAND_ZUSATZ, fl["mitte"][0], fl["mitte"][1], fl["mitte"][2], *fl["huelle"]),
         " * Aufhebe-Rechteck: Mitte x %d = Klemmgrenze %d + %d voraus, z ab %d (Slot 13 endet bei %d). */"
         % (KLEMME_X + VORAUS, KLEMME_X, VORAUS, fl["rect"][1], SLOT13_Z1),
         "#define RE15_R38K_FL_X        (%d)" % fl["pos"][0],
         "#define RE15_R38K_FL_Y        (%d)" % fl["pos"][1],
         "#define RE15_R38K_FL_Z        (%d)" % fl["pos"][2],
         "#define RE15_R38K_FL_ROT_Y    %d" % fl["rot_y"],
         "#define RE15_R38K_FL_RECT_X   (%d)" % fl["rect"][0],
         "#define RE15_R38K_FL_RECT_Z   (%d)" % fl["rect"][1],
         "#define RE15_R38K_FL_RECT_W   %d" % fl["rect"][2],
         "#define RE15_R38K_FL_RECT_D   %d" % fl["rect"][3],
         ""]
    for k in ks:
        N = k["name"].upper()
        L += ["/* SCHLUESSEL %s (Item 0x%02X, RE2-MD1 md5 %s): s = %.4f -> sichtbar %.0f x %.0f Einheiten;"
              % (N, k["item"], k["md1_md5"], k["s"], k["breite"], k["tiefe"]),
              " * Sichtseite Modell %+dy, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x %.2f..%.2f y %.2f..%.2f. */"
              % (int(k["seite_y"]), *k["huelle"]),
              "#define RE15_R38K_%s_X        (%d)" % (N, k["pos"][0]),
              "#define RE15_R38K_%s_Y        (%d)" % (N, k["pos"][1]),
              "#define RE15_R38K_%s_Z        (%d)" % (N, k["pos"][2]),
              "#define RE15_R38K_%s_ROT_X    %d" % (N, k["rot"][0]),
              "#define RE15_R38K_%s_ROT_Y    %d" % (N, k["rot"][1]),
              "#define RE15_R38K_%s_ROT_Z    %d" % (N, k["rot"][2]),
              blob("k_r38k_%s_md1" % k["name"], k["md1"]), ""]
    L += ["/* Aufhebe-Rechteck der Schluessel (beide Varianten derselbe Ort): 1000 x 1000 um (x_Marke, Wand %d + %d),"
          % (WAND_Z, ZONE_VOR_WAND),
          " * rechts vor Slot 12 (x ab %d) beendet. */" % SLOT12_X0,
          "#define RE15_R38K_KS_RECT_X   (%d)" % rect_ks[0],
          "#define RE15_R38K_KS_RECT_Z   (%d)" % rect_ks[1],
          "#define RE15_R38K_KS_RECT_W   1000",
          "#define RE15_R38K_KS_RECT_D   1000", ""]
    open(ZIEL, "w", newline="\n").write("\n".join(L) + "\n")
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
