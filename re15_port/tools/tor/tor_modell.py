#!/usr/bin/env python3
"""tor_modell.py - Tuermodell des Gelaendertors von ROOM1170 im Format der DOOR-Archive.

Das Tor steht am Hubschrauberlandeplatz zwischen zwei Bereichen desselben Raums
(ROOM1170.RDT Door_aot_set Slot 0 @Datei 0x1206 und Slot 6 @Datei 0x135A). RE1.5 und
RE2 haben dafuer kein Modell; es wird hier aus den Hintergrundpixeln gebaut, im selben
Format wie die Tuermodelle der DO2-Archive, damit die Tuersequenz es spaeter aufstellen
und drehen kann.

Aufruf
    python re15_port/tools/tor/tor_modell.py              # bauen + Abnahme
    python re15_port/tools/tor/tor_modell.py --ziel DIR   # Ausgabe (Default build/tor_1170/modell)

Ausgabe
    TOR1170.md1  TOR1170.tim  TOR1170.DO2 (RE1.5-Container)
    TOR1170.obj/.mtl + TOR1170_tex.png (Ansicht in Blender; Y nach oben)
    modell.json  (Kennzahlen, Aufstellwerte fuer die Sequenz, Abnahme)
    abnahme_cut00/11/12.png, vorschau_tuerszene.png

WAS BELEG IST UND WAS NACHBAU
  BELEG    Alle Masse stammen aus der Vermessung (analysis/tor_1170/01_vermessung.md,
           Werkzeug tor_vermessung.py; die Aussagen-Kennungen M1..M9 stehen an jeder Zahl).
           Jeder Texel ist ein Originalpixel aus ROOM1170 Cut 12 (NEAREST, keine
           Zwischenwerte), entzerrt ueber die im Bild gemessene Ebenen-Abbildung Ebene12.
           Das Dateiformat ist do2_format.py (analysis/tor_1170/02_tuerformat.md; 56 von 56
           Originalarchiven byte-gleich zurueckgeschrieben).
           Der Massstab Raum -> Tuerszene ist k = 1,90 (analysis/tor_1170/06_massstab.md,
           berichtigt in 06_massstab.skeptiker.md).
  NACHBAU  (1) Ein Rohrdurchmesser fuer den ganzen Rahmen: das Rohr ist gebogen, also
               EIN Rohr; gewaehlt ist das einzige mit beiden Kanten gegen den Himmel (M3).
           (2) Die lo-Aussenkante (schwarz vor schwarz): Innenkante gemessen, Rohr angesetzt.
           (3) Schild, Laschen und Fuesse als Flaechen ohne Dicke, beidseitig angelegt
               (Gegenflaechen wie in DOOR04/DOOR10/DOOR1F): die Dicke ist in keinem Cut
               messbar (01_vermessung.md, offen).
           (4) Die Landeplatz-Seite traegt denselben Druck wie die Laufsteg-Seite, lesbar
               von beiden Seiten. Cut 0 zeigt Schraffurrand und Tafel auch dort, loest
               aber die Schrift nicht auf (01_vermessung.md D1).
           (5) Die Rohrrueckseite traegt das gespiegelte Querprofil der Vorderseite, das
               lo-Rohr das Profil des hi-Rohrs (die lo-Seite liegt im Original im Schatten).
           (6) Der Pfosten ist ein Rohr desselben Durchmessers (Cut 12 misst 147, M9) und
               endet am oberen Holm (P1); er traegt das Profil des hi-Rohrs.
"""
import argparse
import json
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import do2_format as fmt            # noqa: E402
import tor_kamera as tk             # noqa: E402
from tor_vermessung import Ebene12  # noqa: E402

WURZEL = tk.WURZEL
ZIEL = os.path.join(WURZEL, "build", "tor_1170", "modell")
NAME = "TOR1170"
DOOR00 = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "DOOR", "DOOR00.DO2")

# =============================================================================
# 1. Masse in Torebenen-Koordinaten (Raumwelt-Einheiten)
#    u: von der Achse des hi-Rahmenrohrs nach lo (Welt -x), v: ueber dem Boden
#    y = -7200 (Welt -y), w: senkrecht zur Ebene (Welt +z; +w = Laufsteg-Seite).
#    Quelle aller Werte: analysis/tor_1170/01_vermessung.md bzw. build/tor_1170/
#    vermessung.json (python re15_port/tools/tor/tor_vermessung.py alles).
# =============================================================================
ROHR_D = 151.97                 # M3: Rohr oben, beide Kanten gegen Himmel, 8,28 px * s
ROHR_R = ROHR_D / 2.0
U_HI = 0.0                      # M2: Achse hi-Rahmenrohr (-85,20..85,20) = Ursprung
U_LO = 1747.87 + ROHR_R         # M2: lo-Innenkante gemessen (NACHBAU 2)
V_OBEN = 1467.49                # M1: Mitte Rohr oben (1391,51..1543,48)
V_UNTEN = 327.90                # M1/E2: Mitte Rohr unten (268,85..386,95)
ECKE_R_AUSSEN = 307.85          # M4: Aussenkontur Ecke oben/hi, 21 Punkte, rms 0,44 px
ECKE_R = ECKE_R_AUSSEN - ROHR_R  # Mittellinie des Rohrbogens
SCHILD_U = (293.66, 1536.87)    # M5
SCHILD_V = (470.55, 1229.34)    # M5
LASCHEN_U = ((369.92, 554.96), (1281.56, 1429.18))   # M8
LASCHE_V = (1229.34, V_OBEN)    # M8: Schild-Oberkante bis Rohr; ab der Rohrinnenkante
                                # (1391,51) verschwindet sie im Rohr
FUESSE_U = ((383.21, 551.17), (1329.60, 1459.72))    # M8
FUSS_V = (V_UNTEN, 470.55)      # M8: Rohr bis Schild-Unterkante, s.o.
PFOSTEN_U = (-207.39, -213.27, -191.51)  # M9: Achse Cut 0 / Cut 11 / Cut 12 (-265,09..-117,93)
PFOSTEN_H = 1802.0              # P1: oberer Gelaenderholm ueber dem Boden

# Rohrquerschnitt: Sechskant wie die Staebe von DOOR2E (04_tuerkatalog.md K18);
# Ecken bei 0/60/../300 Grad, 0 Grad = in der Ebene nach aussen. Die Silhouette von vorn
# ist damit genau 2R = ROHR_D breit.
SEITEN = 6
# Rahmenbogen: 2 gerade Abschnitte je Ecke wie DOOR2E (04_tuerkatalog.md K19).
ECK_TEILE = 2

# =============================================================================
# 2. Massstab und Achsen der Tuerszene
# =============================================================================
K = 1.90    # Tuerszenen-Einheiten je Raumwelt-Einheit, k aus der Blatthoehe an 11 Tueren,
            # berichtigt (06_massstab.skeptiker.md: 1,90 +0,05/-0,03).
# Tuerszene: Blick entlang -x, Bild rechts = +z, Bild unten = +y (03_tuersequenz.md K05).
# Blatt-Konvention: Ursprung auf der Angelachse, Blatt laeuft nach -z, oben ist -y
# (02_tuerformat.md "angel", 04_tuerkatalog.md K20).
# Abbildung (u,v,w) -> (X,Y,Z) = (-w, -v, -u) * K. Determinante +1: keine Spiegelung
# gegenueber der Raumwelt (Raumwelt -> Tuerszene ist eine Drehung um y um 90 Grad).
# Folge: Drehung 0 zeigt der Tuerkamera (+X) die Landeplatz-Seite (-w) mit der Angel
# rechts - wie Cut 0 (01_vermessung.md A1: "Cut 0: rechter Pfosten").


def zur_tuerszene(u, v, w):
    return (int(round(-w * K)), int(round(-v * K)), int(round(-u * K)))


# =============================================================================
# 3. Textur-Aufteilung (128 x 256, 8 bit, eine CLUT - wie alle 56 Tuertexturen)
# =============================================================================
TEX_W, TEX_H = 128, 256
BEREICH = {                      # (x0, y0, breite, hoehe) im Texturblatt
    "schild": (0, 0, 128, 78),       # 1243 x 759 -> Seitenverhaeltnis 1,64
    "rohr_oben": (0, 80, 128, 16),
    "rohr_hi": (0, 96, 128, 16),
    "rohr_unten": (0, 112, 128, 16),
    "lasche_hi": (0, 128, 32, 16),
    "lasche_lo": (32, 128, 32, 16),
    "fuss_hi": (64, 128, 32, 16),
    "fuss_lo": (96, 128, 32, 16),
}

# Ebene12: Bild <-> Torebene in Cut 12 aus den im Bild gemessenen Fluchtpunkten
# (vermessung.json -> cut12.ebene12; 01_vermessung.md S1/K3).
E12 = dict(ref=(164.5, 155.0), vx=(1231.83, 190.41), vy=(173.5, -1202.74),
           s=17.49, roh_u0=-47.76, roh_v0=-33.12, v0=1467.49)
# Cut 0 / Cut 11: Torebene und Weltlage der Angelachse (01_vermessung.md E1, V2)
LAGE = {0: dict(z=15110.0, x_hi=3226.70), 11: dict(z=-27590.91, x_hi=-10586.05)}
BODEN_Y = -7200.0


def e12_objekt():
    return Ebene12(E12["vx"], E12["vy"], ref=E12["ref"])


def uv_zu_bild12(E, u, v):
    a = u / E12["s"] + E12["roh_u0"]
    b = (E12["v0"] - v) / E12["s"] + E12["roh_v0"]
    return E.bild(a, b)


def textur_bauen():
    """Texturblatt aus den Originalpixeln von Cut 12 (NEAREST).

    Jeder Texel bekommt den Bildpunkt, in den seine Mitte auf der Torebene faellt."""
    bg = tk.hintergrund(12)
    E = e12_objekt()
    blatt = np.zeros((TEX_H, TEX_W, 4), np.uint8)
    herkunft = {}

    def fuelle(name, ort):
        x0, y0, bw, bh = BEREICH[name]
        pts = []
        for j in range(bh):
            for i in range(bw):
                u, v = ort((i + 0.5) / bw, (j + 0.5) / bh)
                q = uv_zu_bild12(E, u, v)
                px, py = int(math.floor(q[0])), int(math.floor(q[1]))
                blatt[y0 + j, x0 + i, :3] = bg[py, px]
                blatt[y0 + j, x0 + i, 3] = 255
                pts.append((px, py))
        xs, ys = [p[0] for p in pts], [p[1] for p in pts]
        herkunft[name] = dict(bildpunkte_cut12=[min(xs), min(ys), max(xs), max(ys)],
                              verschiedene=len(set(pts)), texel=bw * bh)

    # Schild: s nach rechts = hi -> lo, t nach unten = oben -> unten (Ansicht von +w)
    fuelle("schild", lambda s, t: (SCHILD_U[0] + s * (SCHILD_U[1] - SCHILD_U[0]),
                                   SCHILD_V[1] - t * (SCHILD_V[1] - SCHILD_V[0])))
    # Rohrstreifen: s laengs des geraden Stuecks, t quer von aussen (0) nach innen (1)
    gu = (U_HI + ECKE_R, U_LO - ECKE_R)
    gv = (V_UNTEN + ECKE_R, V_OBEN - ECKE_R)
    fuelle("rohr_oben", lambda s, t: (gu[0] + s * (gu[1] - gu[0]), V_OBEN + ROHR_R - t * ROHR_D))
    fuelle("rohr_unten", lambda s, t: (gu[0] + s * (gu[1] - gu[0]), V_UNTEN - ROHR_R + t * ROHR_D))
    fuelle("rohr_hi", lambda s, t: (U_HI - ROHR_R + t * ROHR_D, gv[0] + s * (gv[1] - gv[0])))
    # Laschen und Fuesse ueber denselben v-Bereich wie ihre Flaeche (der Teil im Rohr
    # traegt Rohrpixel, ist aber vom Rohr verdeckt)
    for n, (a, b) in zip(("lasche_hi", "lasche_lo"), LASCHEN_U):
        fuelle(n, lambda s, t, a=a, b=b: (a + s * (b - a), LASCHE_V[1] - t * (LASCHE_V[1] - LASCHE_V[0])))
    for n, (a, b) in zip(("fuss_hi", "fuss_lo"), FUESSE_U):
        fuelle(n, lambda s, t, a=a, b=b: (a + s * (b - a), FUSS_V[1] - t * (FUSS_V[1] - FUSS_V[0])))
    return Image.fromarray(blatt), herkunft


# =============================================================================
# 4. Geometrie
# =============================================================================
def _norm(a):
    a = np.asarray(a, float)
    return a / np.linalg.norm(a)


class Sammler(object):
    """Dreiecke in Tuerszenen-Koordinaten mit Texturkoordinaten.

    Jedes Dreieck wird so geordnet, dass (p2-p0) x (p1-p0) in die gewuenschte
    Aussenrichtung zeigt: das ist die Seite, die der Tuer-Renderer zeichnet
    (NCLIP >= 0, RE1.5 @0x80016d24 bltz v0 / RE2 @0x800148f8 bgez v0; do2_format._normale)."""

    def __init__(self):
        self.dreiecke = []

    def dreieck(self, e0, e1, e2, aussen):
        (p0, t0), (p1, t1), (p2, t2) = e0, e1, e2
        a, b, c = (np.array(p, float) for p in (p0, p1, p2))
        n = np.cross(c - a, b - a)
        if np.linalg.norm(n) < 1e-9:
            return
        if n.dot(aussen) < 0:
            e1, e2 = e2, e1
        self.dreiecke.append((e0, e1, e2))

    def viereck(self, e0, e1, e2, e3, aussen):
        self.dreieck(e0, e1, e2, aussen)
        self.dreieck(e0, e2, e3, aussen)


def _tex(name, s, t):
    """Anteil (s, t) in 0..1 -> ganzzahlige Texelkoordinate im Bereich `name`."""
    x0, y0, bw, bh = BEREICH[name]
    s = min(max(s, 0.0), 1.0)
    t = min(max(t, 0.0), 1.0)
    return (int(round(x0 + s * (bw - 1))), int(round(y0 + t * (bh - 1))))


def rahmenpfad():
    """Mittellinie des Rohrrahmens (u, v): 4 Boegen zu je ECK_TEILE Abschnitten."""
    r = ECKE_R
    ecken = [((U_HI + r, V_OBEN - r), 180.0, 90.0),
             ((U_LO - r, V_OBEN - r), 90.0, 0.0),
             ((U_LO - r, V_UNTEN + r), 0.0, -90.0),
             ((U_HI + r, V_UNTEN + r), -90.0, -180.0)]
    pfad = []
    for (cu, cv), a0, a1 in ecken:
        for k in range(ECK_TEILE + 1):
            a = math.radians(a0 + (a1 - a0) * k / ECK_TEILE)
            pfad.append((cu + r * math.cos(a), cv + r * math.sin(a)))
    return pfad


def _streifen(p, q):
    """Welcher Rohrstreifen und welche Laengsrichtung fuer den Abschnitt p -> q."""
    du, dv = q[0] - p[0], q[1] - p[1]
    mu, mv = 0.5 * (p[0] + q[0]), 0.5 * (p[1] + q[1])
    if abs(du) >= abs(dv):
        name = "rohr_oben" if mv > 0.5 * (V_OBEN + V_UNTEN) else "rohr_unten"
        a, b = U_HI + ECKE_R, U_LO - ECKE_R
        return name, (lambda pt: (pt[0] - a) / (b - a))
    a, b = V_UNTEN + ECKE_R, V_OBEN - ECKE_R
    # lo-Rohr: Profil des hi-Rohrs (NACHBAU 5)
    return "rohr_hi", (lambda pt: (pt[1] - a) / (b - a))


def fluegel_bauen(S):
    pfad = rahmenpfad()
    n = len(pfad)
    mitte = np.array([0.5 * (U_HI + U_LO), 0.5 * (V_OBEN + V_UNTEN)])
    W = np.array([0.0, 0.0, 1.0])
    ringe = []
    for i in range(n):
        p = np.array(pfad[i])
        a = np.array(pfad[i - 1])
        b = np.array(pfad[(i + 1) % n])
        t1, t2 = _norm(p - a), _norm(b - p)
        tb = _norm(t1 + t2)
        nb = np.array([tb[1], -tb[0]])
        if nb.dot(p - mitte) < 0:
            nb = -nb
        gehrung = 1.0 / max(tb.dot(t1), 1e-6)
        ring = []
        for j in range(SEITEN):
            th = 2 * math.pi * j / SEITEN
            du, dv = ROHR_R * math.cos(th) * gehrung * nb
            ring.append(((p[0] + du, p[1] + dv, ROHR_R * math.sin(th)), (1 - math.cos(th)) / 2))
        ringe.append((p, nb, ring))
    for i in range(n):
        p, nb, ra = ringe[i]
        q, nq, rb = ringe[(i + 1) % n]
        name, laengs = _streifen(p, q)
        sa, sb = laengs(p), laengs(q)
        nm = _norm(nb + nq)
        for j in range(SEITEN):
            k = (j + 1) % SEITEN
            thc = 2 * math.pi * (j + 0.5) / SEITEN
            aussen_uvw = np.array([math.cos(thc) * nm[0], math.cos(thc) * nm[1], math.sin(thc)])
            ecken = [(ra[j][0], sa, ra[j][1]), (ra[k][0], sa, ra[k][1]),
                     (rb[k][0], sb, rb[k][1]), (rb[j][0], sb, rb[j][1])]
            e = [(zur_tuerszene(*pt), _tex(name, s, t)) for pt, s, t in ecken]
            S.viereck(e[0], e[1], e[2], e[3], _aussen(aussen_uvw))
    # Flaechen ohne Dicke, beidseitig (NACHBAU 3). +w-Seite: hi links (wie Cut 12);
    # -w-Seite: lo links, Textur so gelegt, dass sie von dort lesbar ist (NACHBAU 4).
    platte(S, "schild", SCHILD_U, SCHILD_V)
    for name, uu in zip(("lasche_hi", "lasche_lo"), LASCHEN_U):
        platte(S, name, uu, LASCHE_V)
    for name, uu in zip(("fuss_hi", "fuss_lo"), FUESSE_U):
        platte(S, name, uu, FUSS_V)


def _aussen(n_uvw):
    """Richtung (u,v,w) -> Tuerszene (ohne Massstab)."""
    return np.array([-n_uvw[2], -n_uvw[1], -n_uvw[0]])


def platte(S, name, uu, vv):
    (u0, u1), (v0, v1) = uu, vv
    for seite in (+1, -1):
        def tx(u, v):
            s = (u - u0) / (u1 - u0)
            if seite < 0:
                s = 1.0 - s
            return _tex(name, s, (v1 - v) / (v1 - v0))
        e = [(zur_tuerszene(u, v, 0.0), tx(u, v)) for u, v in ((u0, v1), (u1, v1), (u1, v0), (u0, v0))]
        S.viereck(e[0], e[1], e[2], e[3], _aussen(np.array([0.0, 0.0, seite])))


def pfosten_bauen(S):
    """Pfosten: Sechskantrohr, Ursprung = Fuss auf der Achse (NACHBAU 6)."""
    ringe = []
    for h in (0.0, PFOSTEN_H):
        ring = []
        for j in range(SEITEN):
            th = 2 * math.pi * j / SEITEN
            ring.append(((ROHR_R * math.cos(th), h, ROHR_R * math.sin(th)), (1 - math.cos(th)) / 2))
        ringe.append(ring)
    for j in range(SEITEN):
        k = (j + 1) % SEITEN
        thc = 2 * math.pi * (j + 0.5) / SEITEN
        aussen = _aussen(np.array([math.cos(thc), 0.0, math.sin(thc)]))
        ecken = [(ringe[0][j], 0.0), (ringe[0][k], 0.0), (ringe[1][k], 1.0), (ringe[1][j], 1.0)]
        e = [(zur_tuerszene(*pt[0]), _tex("rohr_hi", s, pt[1])) for pt, s in ecken]
        S.viereck(e[0], e[1], e[2], e[3], aussen)
    oben = ringe[1]
    mitte_tex = _tex("rohr_hi", 1.0, 0.5)
    for j in range(1, SEITEN - 1):
        e = [(zur_tuerszene(*oben[0][0]), mitte_tex), (zur_tuerszene(*oben[j][0]), mitte_tex),
             (zur_tuerszene(*oben[j + 1][0]), mitte_tex)]
        S.dreieck(e[0], e[1], e[2], _aussen(np.array([0.0, 1.0, 0.0])))


# =============================================================================
# 5. Rastern (fuer Abnahme und Vorschau)
# =============================================================================
def rastern(tris2d, tex, W, H, ueber=4, bg=None, nclip=True):
    """tris2d: [(sx[3], sy[3], z[3], uv[3])]. Zeichnet nur NCLIP >= 0 wie der
    Tuer-Renderer, Texel NEAREST, Tiefenpuffer. Rueckgabe (rgb, deckung, gezeichnet)."""
    Wu, Hu = W * ueber, H * ueber
    farbe = np.zeros((Hu, Wu, 3), np.float32)
    tiefe = np.full((Hu, Wu), np.inf, np.float32)
    maske = np.zeros((Hu, Wu), bool)
    gez = 0
    for sx, sy, z, uv in tris2d:
        x = np.array(sx, float) * ueber
        y = np.array(sy, float) * ueber
        ncl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if nclip and ncl < 0:
            continue
        if abs(ncl) < 1e-9:
            continue
        gez += 1
        x0, x1 = max(int(math.floor(x.min())), 0), min(int(math.ceil(x.max())), Wu - 1)
        y0, y1 = max(int(math.floor(y.min())), 0), min(int(math.ceil(y.max())), Hu - 1)
        if x0 > x1 or y0 > y1:
            continue
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (y[1] - y[2]) * (x[0] - x[2]) + (x[2] - x[1]) * (y[0] - y[2])
        l0 = ((y[1] - y[2]) * (gx - x[2]) + (x[2] - x[1]) * (gy - y[2])) / d
        l1 = ((y[2] - y[0]) * (gx - x[2]) + (x[0] - x[2]) * (gy - y[2])) / d
        l2 = 1 - l0 - l1
        drin = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        if not drin.any():
            continue
        zz = l0 * z[0] + l1 * z[1] + l2 * z[2]
        uu = l0 * uv[0][0] + l1 * uv[1][0] + l2 * uv[2][0]
        vv = l0 * uv[0][1] + l1 * uv[1][1] + l2 * uv[2][1]
        sub = (slice(y0, y1 + 1), slice(x0, x1 + 1))
        vor = drin & (zz < tiefe[sub])
        ti = np.clip(np.floor(uu).astype(int), 0, tex.shape[1] - 1)
        tj = np.clip(np.floor(vv).astype(int), 0, tex.shape[0] - 1)
        texel = tex[tj, ti]
        vor &= texel[..., 3] > 0          # Index 0 = 0x0000 wird nie gezeichnet
        f = farbe[sub]
        f[vor] = texel[vor][:, :3]
        tiefe[sub][vor] = zz[vor]
        maske[sub][vor] = True
    # auf Bildpunkte mitteln
    m = maske.reshape(H, ueber, W, ueber).mean(axis=(1, 3))
    s = (farbe * maske[..., None]).reshape(H, ueber, W, ueber, 3).sum(axis=(1, 3))
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(m[..., None] > 0, s / (m[..., None] * ueber * ueber), 0)
    if bg is not None:
        rgb = rgb * m[..., None] + bg.astype(np.float32) * (1 - m[..., None])
    return rgb, m, gez


def dreiecke_uvw(S):
    """Tuerszene -> (u, v, w) zurueck (Umkehrung von zur_tuerszene)."""
    aus = []
    for tri in S.dreiecke:
        pts = [(-p[2] / K, -p[1] / K, -p[0] / K) for p, _ in tri]
        aus.append((pts, [t for _, t in tri]))
    return aus


def raum_projektion(S, cut, su=1.0, dx=0.0, dy=0.0):
    """Modell in den Raum-Cut: Cut 0/11 ueber RDT-Kamera + Torebene, Cut 12 ueber Ebene12."""
    tris = []
    if cut == 12:
        E = e12_objekt()
        for pts, uv in dreiecke_uvw(S):
            q = [uv_zu_bild12(E, p[0] * su, p[1]) for p in pts]
            # Tiefe: w zur Kamera hin (Cut 12 sieht die +w-Seite) -> kleiner
            z = [-p[2] for p in pts]
            tris.append(([a[0] + dx for a in q], [a[1] + dy for a in q], z, uv))
        return tris
    rdt = tk.lade_rdt()
    kam = tk.kamera(rdt, cut)
    L = LAGE[cut]
    for pts, uv in dreiecke_uvw(S):
        w = np.array([[L["x_hi"] - p[0] * su, BODEN_Y - p[1], L["z"] + p[2]] for p in pts])
        sx, sy, vz = kam.bild(w)
        tris.append((list(sx + dx), list(sy + dy), list(vz), uv))
    return tris


# =============================================================================
# 6. Abnahme
# =============================================================================
AUSSCHNITT = {0: (133, 81, 30, 28), 11: (73, 60, 44, 36), 12: (107, 115, 111, 76)}


def _lum(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def abnahme_cut(S, tex, cut, ziel, suche):
    """Modell in den Cut rastern und gegen das Original stellen.

    Bild: Original | Modell eingesetzt | Original mit den gezeichneten Dreieckskanten.
    Mass (nur mit suche=True): normierte Kreuzkorrelation der Helligkeit im Ausschnitt
    (+3 px Rand) zwischen Original und 'Hintergrund mit eingesetztem Modell'. Nullmodell:
    dasselbe Modell um dx, dy in -3..3 px verschoben und laengs (u) um su gestreckt.
    Nur fuer Cut 12 sinnvoll: in Cut 0 und Cut 11 traegt die Textur das Licht von Cut 12,
    die Korrelation misst dort den Lichtunterschied mit (erster Lauf: Optimum am Rand des
    Suchraums in BEIDEN Cuts, und ein Kantenmass streute ohne Optimum). Dort gilt
    abnahme_merkmale(). In Cut 12 ist die Textur selbstbestaetigend; was prueft, sind
    die Rohrkanten gegen den Himmel."""
    bg = tk.hintergrund(cut).astype(np.float32)
    x, y, w, h = AUSSCHNITT[cut]
    x0, y0, x1, y1 = max(x - 3, 0), max(y - 3, 0), min(x + w + 3, 320), min(y + h + 3, 240)
    orig = _lum(bg[y0:y1, x0:x1])

    def wert(su, dx, dy):
        rgb, m, _ = rastern(raum_projektion(S, cut, su, dx, dy), tex, 320, 240, 4, bg)
        a = _lum(rgb[y0:y1, x0:x1])
        a, b = a - a.mean(), orig - orig.mean()
        return float((a * b).sum() / math.sqrt((a * a).sum() * (b * b).sum())), rgb, m
    ncc0, rgb0, m0 = wert(1.0, 0, 0)
    erg = dict(ncc_null=round(ncc0, 4))
    if suche:
        raster = []
        for su in (0.92, 0.96, 1.00, 1.04, 1.08):
            for dx in range(-3, 4):
                for dy in range(-3, 4):
                    raster.append(dict(su=su, dx=dx, dy=dy, ncc=round(wert(su, dx, dy)[0], 4)))
        folge = sorted(raster, key=lambda r: -r["ncc"])
        erg.update(bester=folge[0], varianten=len(raster), rang_der_nulllage=1 + next(
            i for i, r in enumerate(folge) if r["su"] == 1.0 and r["dx"] == 0 and r["dy"] == 0))
    F = 12 if cut == 0 else (9 if cut == 11 else 4)
    bw, bh = (x1 - x0) * F, (y1 - y0) * F
    kanten = Image.fromarray(bg[y0:y1, x0:x1].astype(np.uint8)).resize((bw, bh), Image.NEAREST)
    d = ImageDraw.Draw(kanten)
    for sx, sy, z, uv in raum_projektion(S, cut):
        if (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0]) < 0:
            continue
        p = [((a - x0) * F, (b - y0) * F) for a, b in zip(sx, sy)]
        d.line(p + [p[0]], fill=(255, 0, 255), width=1)
    teile = [Image.fromarray(np.clip(t, 0, 255).astype(np.uint8)).resize((bw, bh), Image.NEAREST)
             for t in (bg[y0:y1, x0:x1], rgb0[y0:y1, x0:x1])] + [kanten]
    blatt = Image.new("RGB", (bw * 3 + 20, bh + 18), (30, 30, 34))
    for i, im in enumerate(teile):
        blatt.paste(im, (i * (bw + 10), 18))
    text = "Cut %d: Original | Modell eingesetzt | Dreieckskanten auf dem Original" % cut
    if suche:
        text += "   NCC %.3f, Rang %d von %d" % (ncc0, erg["rang_der_nulllage"], erg["varianten"])
    ImageDraw.Draw(blatt).text((2, 2), text, fill=(230, 230, 230))
    pfad = os.path.join(ziel, "abnahme_cut%02d.png" % cut)
    blatt.save(pfad)
    erg["bild"] = os.path.relpath(pfad, WURZEL).replace("\\", "/")
    return erg


def abnahme_merkmale():
    """Cut 0 / Cut 11: Abweichung des Modells in BILDPUNKTEN gegen die Linien, die die
    Vermessung in genau diesen Cuts gemessen hat (vermessung.json -> cutN.welt).
    Beide Werte an derselben Stelle der Torebene durch die RDT-Kamera projiziert."""
    pfad = os.path.join(WURZEL, "build", "tor_1170", "vermessung.json")
    if not os.path.exists(pfad):
        return dict(fehlt=pfad + " (python re15_port/tools/tor/tor_vermessung.py alles)")
    j = json.load(open(pfad, encoding="utf-8"))
    mv = {"rohr_oben": V_OBEN, "rohr_unten": V_UNTEN,
          "rohr_oben_oberkante": V_OBEN + ROHR_R, "rohr_oben_unterkante": V_OBEN - ROHR_R,
          "rohr_unten_oberkante": V_UNTEN + ROHR_R, "rohr_unten_unterkante": V_UNTEN - ROHR_R,
          "schild_oben_kante": SCHILD_V[1], "schild_unten_kante": SCHILD_V[0]}
    mu = {"schild_hi_kante": SCHILD_U[0], "schild_lo_kante": SCHILD_U[1], "lo_dunkel": U_LO,
          "tafel_hi_kante": 444.78, "tafel_lo_kante": 1353.69}   # Tafel: M6
    rdt = tk.lade_rdt()
    aus = {}
    for cut, key in ((0, "cut0"), (11, "cut11")):
        k, L, wv = tk.kamera(rdt, cut), LAGE[cut], j[key]["welt"]
        zeilen = []
        for n, m in mv.items():
            if n in wv["v"]:
                a = k.bild(np.array([L["x_hi"] - 900.0, BODEN_Y - wv["v"][n], L["z"]]))[1]
                b = k.bild(np.array([L["x_hi"] - 900.0, BODEN_Y - m, L["z"]]))[1]
                zeilen.append(dict(achse="v", name=n, gemessen=round(wv["v"][n], 1), modell=round(m, 1),
                                   px=round(float(b - a), 2)))
        for n, m in mu.items():
            if n in wv["u"]:
                a = k.bild(np.array([L["x_hi"] - wv["u"][n], BODEN_Y - 900.0, L["z"]]))[0]
                b = k.bild(np.array([L["x_hi"] - m, BODEN_Y - 900.0, L["z"]]))[0]
                zeilen.append(dict(achse="u", name=n, gemessen=round(wv["u"][n], 1), modell=round(m, 1),
                                   px=round(float(b - a), 2)))
        aus["cut%02d" % cut] = dict(zeilen=zeilen, groesste_px=max(abs(z["px"]) for z in zeilen))
    return aus


# =============================================================================
# 7. Vorschau in der Tuerszene (RE1.5-Tuerkamera)
# =============================================================================
# RE1.5 Tuerkamera: Auge (30000,0,0), Ziel (22000,0,0), H = 1000, Bildmitte (160,120)
# (03_tuersequenz.md K32, im Skeptiker bestaetigt). Blickmatrix daraus nach
# tor_kamera.Kamera._bau_ideal: Bild-x = z, Bild-y = y, Tiefe = 30000 - x.
TUERKAMERA = dict(auge=30000.0, H=1000.0)


def rot_y(winkel):
    """RotMatrix fuer Drehung nur um y: m[0][2] = +sin (03_tuersequenz.md K22,
    RE1.5 @0x8006816c)."""
    a = 2 * math.pi * winkel / 4096.0
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def tuerszene_projektion(dreiecke, lage, winkel):
    R = rot_y(winkel)
    P = np.array(lage, float)
    tris = []
    for tri in dreiecke:
        pw = [R.dot(np.array(p, float)) + P for p, _ in tri]
        z = [TUERKAMERA["auge"] - q[0] for q in pw]
        sx = [160 + TUERKAMERA["H"] * q[2] / zz for q, zz in zip(pw, z)]
        sy = [120 + TUERKAMERA["H"] * q[1] / zz for q, zz in zip(pw, z)]
        tris.append((sx, sy, z, [t for _, t in tri]))
    return tris


def vorschau(Sf, Sp, tex, ziel, lage, pfosten_z):
    bilder = []
    zeilen = []
    for winkel, titel in ((0, "Drehung 0: Landeplatz-Seite (wie Cut 0)"),
                          (570, "Drehung +570: aufgedrueckt (DOOR2E-Winkel)"),
                          (2048, "Drehung 2048: Laufsteg-Seite (wie Cut 12)"),
                          (2048 + 570, "Drehung 2618: aufgezogen")):
        # Gegenseite: Lage in z gespiegelt, wie DOOR2E Variante 1 (z 1714 -> -1374,
        # 04_tuerkatalog.md K21/K22), damit das Blatt im Bild bleibt
        basis = np.array(lage, float) if winkel < 1024 else np.array([lage[0], lage[1], -lage[2]], float)
        t = tuerszene_projektion(Sf.dreiecke, basis, winkel)
        pl = basis + (np.array([0.0, 0.0, pfosten_z]) if winkel < 1024 else
                      rot_y(2048).dot(np.array([0.0, 0.0, pfosten_z])))
        t += tuerszene_projektion(Sp.dreiecke, pl, 0)
        rgb, m, gez = rastern(t, tex, 320, 240, 2, np.full((240, 320, 3), 24, np.float32))
        zeilen.append(dict(winkel=winkel, titel=titel, dreiecke=len(t), gezeichnet=gez))
        im = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8)).resize((640, 480), Image.NEAREST)
        ImageDraw.Draw(im).text((6, 6), titel, fill=(240, 240, 120))
        bilder.append(im)
    blatt = Image.new("RGB", (1290, 970), (10, 10, 10))
    for i, im in enumerate(bilder):
        blatt.paste(im, ((i % 2) * 650, (i // 2) * 490))
    pfad = os.path.join(ziel, "vorschau_tuerszene.png")
    blatt.save(pfad)
    return dict(bild=os.path.relpath(pfad, WURZEL).replace("\\", "/"), ansichten=zeilen)


# =============================================================================
# 8. Ausgabe
# =============================================================================
def obj_schreiben(ziel, meshes_dreiecke, texname):
    """OBJ fuer Blender: (X, -Y, -Z) = Drehung um x um 180 Grad (Y nach oben, keine
    Spiegelung, der Drehsinn der Flaechen bleibt gueltig)."""
    zeilen = ["# %s - Tuermodell Gelaendertor ROOM1170 (Tuerszenen-Einheiten)" % NAME,
              "mtllib %s.mtl" % NAME]
    vi = 1
    for name, tris in meshes_dreiecke:
        zeilen.append("o %s" % name)
        zeilen.append("usemtl tor")
        for tri in tris:
            for p, t in tri:
                zeilen.append("v %d %d %d" % (p[0], -p[1], -p[2]))
                zeilen.append("vt %.5f %.5f" % ((t[0] + 0.5) / TEX_W, 1 - (t[1] + 0.5) / TEX_H))
            zeilen.append("f %d/%d %d/%d %d/%d" % (vi, vi, vi + 1, vi + 1, vi + 2, vi + 2))
            vi += 3
    with open(os.path.join(ziel, NAME + ".obj"), "w") as f:
        f.write("\n".join(zeilen) + "\n")
    with open(os.path.join(ziel, NAME + ".mtl"), "w") as f:
        f.write("newmtl tor\nKd 1 1 1\nmap_Kd %s\n" % texname)


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--ziel", default=ZIEL)
    ap.add_argument("--ohne-abnahme", action="store_true")
    a = ap.parse_args(argv)
    ziel = a.ziel
    os.makedirs(ziel, exist_ok=True)

    tex_bild, herkunft = textur_bauen()
    tim, tim_info = fmt.tim_aus_bild(tex_bild)
    tex_rgba = np.asarray(tim.als_bild().convert("RGBA"))   # zurueckgelesen: das, was die GPU sieht
    Image.fromarray(tex_rgba).save(os.path.join(ziel, NAME + "_tex.png"))

    Sf, Sp = Sammler(), Sammler()
    fluegel_bauen(Sf)
    pfosten_bauen(Sp)
    m_fluegel = fmt.mesh_aus_dreiecken(Sf.dreiecke)
    m_pfosten = fmt.mesh_aus_dreiecken(Sp.dreiecke)
    md1 = fmt.md1_bauen([m_fluegel, m_pfosten])
    md1_b = md1.schreiben()
    tim_b = tim.schreiben()
    ton = fmt.Do2.lesen(open(DOOR00, "rb").read())
    do2 = fmt.do2_bauen(md1, tim, [fmt.evt_end_skript()], ton=ton, variante="re15")
    do2_b = do2.schreiben()
    for endung, daten in (("md1", md1_b), ("tim", tim_b), ("DO2", do2_b)):
        with open(os.path.join(ziel, "%s.%s" % (NAME, endung)), "wb") as f:
            f.write(daten)
    obj_schreiben(ziel, [("fluegel", Sf.dreiecke), ("pfosten", Sp.dreiecke)], NAME + "_tex.png")

    # Rundlauf: zuruecklesen und vergleichen
    zurueck = fmt.Md1.lesen(md1_b)
    rundlauf = zurueck.schreiben() == md1_b
    lader = fmt.lader_pruefung(do2_b)

    def ausdehnung(m):
        v = np.array([p[:3] for p in m.vertices])
        return dict(min=v.min(axis=0).tolist(), max=v.max(axis=0).tolist())

    pfosten_z = [round(-u * K) for u in PFOSTEN_U]
    E = dict(
        werkzeug="python re15_port/tools/tor/tor_modell.py",
        massstab_k=K,
        meshes=[dict(name="fluegel", vertices=len(m_fluegel.vertices), normalen=len(m_fluegel.normals),
                     dreiecke=len(m_fluegel.tris), vierecke=0, ausdehnung=ausdehnung(m_fluegel)),
                dict(name="pfosten", vertices=len(m_pfosten.vertices), normalen=len(m_pfosten.normals),
                     dreiecke=len(m_pfosten.tris), vierecke=0, ausdehnung=ausdehnung(m_pfosten))],
        dreiecke_mit_zwei_pfosten=len(m_fluegel.tris) + 2 * len(m_pfosten.tris),
        md1_bytes=len(md1_b), tim_bytes=len(tim_b), do2_bytes=len(do2_b),
        md1_rundlauf_byte_gleich=rundlauf,
        lader_pruefung=lader,
        tim=tim_info, texturherkunft=herkunft,
        aufstellung_fuer_die_sequenz=dict(
            fluegel_ursprung="Angelachse (Achse hi-Rahmenrohr) auf Bodenhoehe",
            pfosten_hi_z_je_cut=dict(zip(("cut0", "cut11", "cut12"), pfosten_z)),
            pfosten_lo="Lage OFFEN (01_vermessung.md M9: Cut 0 und Cut 11 widersprechen sich)",
            drehung_0="Landeplatz-Seite zur Kamera, Angel rechts",
            drehung_2048="Laufsteg-Seite zur Kamera, Angel links"),
    )
    if not a.ohne_abnahme:
        E["abnahme"] = {("cut%02d" % c): abnahme_cut(Sf, tex_rgba, c, ziel, c == 12) for c in (0, 11, 12)}
        E["abnahme"]["merkmale_cut0_cut11"] = abnahme_merkmale()
        # Vorschau: Aufstellung NUR fuer das Bild, keine Festlegung fuer die Sequenz
        lage = (12000, 1700, 1800)
        E["vorschau"] = vorschau(Sf, Sp, tex_rgba, ziel, lage, pfosten_z[2])
        E["vorschau"]["lage_nur_fuer_das_bild"] = lage
    with open(os.path.join(ziel, "modell.json"), "w", encoding="utf-8") as f:
        json.dump(E, f, ensure_ascii=False, indent=1)
    print(json.dumps({k: E[k] for k in ("meshes", "dreiecke_mit_zwei_pfosten", "md1_bytes", "tim_bytes",
                                         "do2_bytes", "md1_rundlauf_byte_gleich")}, ensure_ascii=False))
    print("TIM:", tim_info)
    print("Lader:", {k: lader[k] for k in lader if k in ("lesbar", "fehler")})
    if "abnahme" in E:
        for c, r in E["abnahme"].items():
            if c.startswith("merkmale"):
                for cc, rr in r.items():
                    print(cc, "groesste Abweichung %.2f px" % rr["groesste_px"] if "groesste_px" in rr else rr)
                    for z in rr.get("zeilen", []):
                        print("   %s %-22s gemessen %7.1f  Modell %7.1f  %+5.2f px" % (
                            z["achse"], z["name"], z["gemessen"], z["modell"], z["px"]))
            else:
                print(c, r)
        print(json.dumps(E["vorschau"]["ansichten"], ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
