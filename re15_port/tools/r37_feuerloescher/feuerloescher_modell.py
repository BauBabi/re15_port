#!/usr/bin/env python3
"""feuerloescher_modell.py - baut das Welt-Modell des FEUERLOESCHERS (Item 0x31) fuer den Port.

Runde 37, Spur I (Nachtrag 4 des Nutzers, analysis/befunde_runde37/AUFTRAG.md):
    "Bitte erstelle mir zusaetzlich auch ein Modell des Feuerloeschers."
Weder RE1.5 noch RE2 haben eines (analysis/befunde_2026-09-21/re2-feuerloescher-modell.md:
570/570 RE1.5- und 923/923 RE2-Prop-Plaetze angesehen, Aufnahme-Datensatz ROOM1000.RDT @0x0C24
traegt tk_prop 0xFF). Das Modell ist deshalb NEU GEBAUT (PORT-WAHL); die einzige Original-
Vorlage ist das Gegenstands-Bild ITEM/ITPS.ITP Block 0x31 (file@0x093000 = 0x31 * 0x3000,
Schrittweite aus engine/src/itps_common.c ITPS_STRIDE; 112x72 8bpp mit eigener CLUT).

WAS AUS DEM BILD GEMESSEN WIRD (nicht gewaehlt):
  * Achse des Koerpers = Hauptachse der Koerperpixel (PCA), Laengen/Breiten in Bildpunkten
    -> Verhaeltnis Zylinderlaenge / Durchmesser (Dossier I_feuerloescher.md, Abschnitt Modell).
  * Textur des Mantels = das Bild selbst, entlang der Achse abgewickelt (Etikett, Flecken,
    Baender), quer zur Achse ent-schattiert (Helligkeitsprofil je Querlage normiert).
  * Farben von Griff/Hebel (Chrom) = Mittel der grauen Hebelpixel des Bilds.
  * Helligkeit: Weiss des Etiketts auf das Weiss der Original-Item-Textur ROOM4050 Prop 3
    (rote Spraydose, MD1 @0x3788 / TIM @0x2DDA8, 99,5-Perzentil Summe 656) gezogen — der Port
    beleuchtet Props mit dem Raumlicht (NCCT), die Textur traegt also die EIGENFARBE in
    derselben Skala wie die Original-Gegenstaende.

MASSSTAB (PORT-WAHL, Herleitung im Dossier):
  Leon PL00.PLD Nullpose 3022 Einheiten hoch (tor/tor_massstab.py spielermodell; Trefferkasten
  @0x80073e94 = 3060). Leon = 1,78 m -> 1698 Einheiten je Meter. Durchmesser des Koerpers
  13 cm (10-lb-Klasse) -> R = 110. Laenge des Zylinders = gemessenes Bildverhaeltnis * D.

FORMAT 1:1 WIE DIE BISHERIGEN EINGEBACKENEN PROPS (tools/sicherung_engine_export.py):
  MD1-Kopf 65/0/2, ein Mesh, Normalen Q12 (4096) nach AUSSEN, Vierecke in Z-Ordnung
  (FUN_800256b0 / Primitiv 0x3C, Port teilt (0,1,3)+(0,3,2)); TIM 8bpp+CLUT, Bild @VRAM(0,0),
  CLUT @VRAM(0,480), UV-clut 0x7800, page 0x80; CLUT-Index 0 frei (PSX-Farbschluessel).

ZWEI LAGEN DESSELBEN NETZES (damit keine Prop-Drehung um 90 Grad noetig ist, Lehre der
Sicherung: "liegend exportiert ist die Prop-Position schlicht die MITTE"):
  stehend  Achse senkrecht, Ursprung = Mitte der Bodenflaeche (y=0), Hebel nach -X,
           Etikett nach +Z, Schlauch nach +X.   (Wandhalterung, Leons Fundort)
  liegend  Achse waagerecht entlang X (Kopf +X), Ursprung = Mitte auf dem Boden (y=0 = Auflage),
           Etikett nach oben, Hebel nach -Z.      (leerer Loescher ROOM1090/1091)

Aufruf:
  python re15_port/tools/r37_feuerloescher/feuerloescher_modell.py            # .inc + Bilder
  python re15_port/tools/r37_feuerloescher/feuerloescher_modell.py --nur-mass # nur Messwerte
Schreibt engine/src/gen/feuerloescher_r37.inc und analysis/befunde_runde37/I_belege/modell_*.png.
"""
import io
import math
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
ITPS = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "ITEM", "ITPS.ITP")
ITPS_STRIDE = 0x3000          # engine/src/itps_common.c ITPS_STRIDE
ITEM_ID = 0x31                # "Fire Extinguisher" (DEBUG.BIN @0x4CDC, ITPS file@0x093000)
REF_TIM = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE4", "ROOM4050.RDT")
REF_TIM_OFF = 0x2DDA8         # Prop-3-TIM der roten Spraydose (Vorarbeit 2026-09-21 Tabelle 3)
ZIEL_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "feuerloescher_r37.inc")
BELEGE = os.path.join(REPO, "analysis", "befunde_runde37", "I_belege")

# ---- Massstab (PORT-WAHL, Dossier) ------------------------------------------------------
LEON_EINHEITEN = 3022.0       # PL00.PLD Nullpose (tor_massstab.spielermodell)
LEON_METER = 1.78
EINHEITEN_JE_M = LEON_EINHEITEN / LEON_METER          # 1697.8
DURCHMESSER_M = 0.13                                   # 10-lb-Klasse
R = int(round(DURCHMESSER_M * EINHEITEN_JE_M / 2))    # 110

TEX_W, TEX_H = 128, 128
CLUT_WORT = 0x7800            # (480 << 6) | 0 — wie gen/sicherung_prop.inc
PAGE_WORT = 0x0080            # 8bpp, VRAM (0,0)
SEITEN = 10                   # Zehnkant (RE2 ROOM60D0 Prop 2 Laborflasche: 10-seitig)
WEISS_ZIEL = 656.0            # 99,5-Perzentil R+G+B der ROOM4050-Spraydose (gemessen hier)


# ============================================================================================
# 1. Das Original-Bild
# ============================================================================================
def tim_lesen(b, o=0):
    """TIM (8bpp+CLUT) -> (index[h,w], clut[256] u16)."""
    magic, flag = struct.unpack_from("<II", b, o)
    assert magic == 0x10 and (flag & 0x0B) == 0x09, (hex(magic), hex(flag))
    q = o + 8
    bl, _cx, _cy, cw, ch = struct.unpack_from("<IHHHH", b, q)
    clut = np.frombuffer(b[q + 12:q + bl], dtype="<u2").reshape(ch, cw)[0]
    q += bl
    bl, _ix, _iy, iw, ih = struct.unpack_from("<IHHHH", b, q)
    idx = np.frombuffer(b[q + 12:q + 12 + iw * 2 * ih], dtype=np.uint8).reshape(ih, iw * 2)
    return idx, clut


def clut_rgb(c):
    c = int(c)
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def itps_bild():
    d = open(ITPS, "rb").read()
    off = ITEM_ID * ITPS_STRIDE
    idx, clut = tim_lesen(d, off)
    pal = np.array([clut_rgb(c) for c in clut], np.float64)
    return pal[idx], off


def ref_weiss():
    """99,5-Perzentil der R+G+B-Summe der Original-Spraydosen-Textur (ROOM4050 Prop 3)."""
    d = open(REF_TIM, "rb").read()
    idx, clut = tim_lesen(d, REF_TIM_OFF)
    pal = np.array([clut_rgb(c) for c in clut], np.float64)
    s = pal[idx].sum(-1).reshape(-1)
    s = s[s > 0]
    return float(np.percentile(s, 99.5))


def koerper_messen(rgb):
    """Hauptachse der Koerperpixel. Hintergrund = (0,0,56) (haeufigste Farbe, 6394 von 8064)."""
    h, w, _ = rgb.shape
    bg = (rgb[..., 0] == 0) & (rgb[..., 1] == 0) & (rgb[..., 2] == 56)
    m = ~bg
    m[:2, :] = m[-2:, :] = False          # grauer Rahmen der Bildkachel
    m[:, :2] = m[:, -2:] = False
    ys, xs = np.nonzero(m)
    # Hebel/Griff: die grauen Pixel links oben (x <= 41 und y <= 38, siehe Dossier-Karte)
    hebel = (xs <= 37) & (ys <= 38) | (xs <= 41) & (ys <= 24)
    B = np.stack([xs[~hebel], ys[~hebel]], 1).astype(float)
    L = np.stack([xs[hebel], ys[hebel]], 1).astype(float)
    c = B.mean(0)
    _u, _s, vt = np.linalg.svd(B - c)
    ax, pe = vt[0], vt[1]
    if ax[0] < 0:
        ax = -ax
    if pe[1] < 0:
        pe = -pe
    t = (B - c) @ ax
    s = (B - c) @ pe
    # Breite QUER zur Achse: je Achsenlage t eine Linie entlang `pe` in 0,05-px-Schritten
    # abtasten (naechstes Pixel in der Koerpermaske ohne Hebel); Breite = Laenge des
    # zusammenhaengenden Abschnitts durch die Achse. (Eine Scheiben-Huelle |s|max-|s|min+1
    # ueberschaetzt die Breite um die Schraege und das Pixelraster.)
    kmask = np.zeros_like(m)
    kmask[ys[~hebel], xs[~hebel]] = True
    breite = []
    for k in np.arange(math.floor(t.min()), math.ceil(t.max()) + 1, 2.0):
        lo = hi = None
        for sgn in (-1, 1):
            st = 0.0
            while True:
                q = c + k * ax + sgn * (st + 0.05) * pe
                xi, yi = int(round(q[0])), int(round(q[1]))
                if not (0 <= yi < h and 0 <= xi < w) or not kmask[yi, xi]:
                    break
                st += 0.05
            if sgn < 0:
                lo = st
            else:
                hi = st
        q0 = c + k * ax
        if kmask[int(round(q0[1])), int(round(q0[0]))]:
            breite.append((float(k), lo + hi + 1.0, 100))   # +1: Pixelbreite der Randpixel
    hebel_rgb = rgb[ys[hebel], xs[hebel]]
    grau = [p for p in hebel_rgb if abs(p[0] - p[1]) < 25 and abs(p[1] - p[2]) < 25]
    # Chrom = die HELLERE Haelfte der grauen Hebelpixel (die dunklere ist Eigenschatten des
    # gerenderten Bilds; der Port schattiert selbst)
    if grau:
        g = np.array(grau, float)
        grau = list(g[g.sum(1) >= np.median(g.sum(1))])
    return dict(zentrum=c, achse=ax, quer=pe, t_min=float(t.min()), t_max=float(t.max()),
                breiten=breite, hebel_t=(float(((L - c) @ ax).min()), float(((L - c) @ ax).max())),
                hebel_s=(float(((L - c) @ pe).min()), float(((L - c) @ pe).max())),
                chrom=np.mean(np.array(grau, float), 0) if grau else np.array([170.0, 170, 165]),
                maske=m)


# ============================================================================================
# 2. Abmessungen (aus der Messung abgeleitet)
# ============================================================================================
def abmessungen(mess):
    br = [b for (_k, b, _n) in mess["breiten"]]
    d_px = float(np.median(br))                                  # Durchmesser im Bild
    # Zylinder = die Lagen mit mindestens 90 % der Mittelbreite; Schulter / Boden = die
    # schmaleren Enden (Kuppel oben, Rundung unten).
    voll = [k for (k, b, _n) in mess["breiten"] if b >= 0.9 * d_px]
    t0, t1 = min(voll), max(voll)                                # Zylinderanfang / -ende im Bild
    l_zyl_px = t1 - t0
    verh = l_zyl_px / d_px                                       # Laenge / Durchmesser
    schulter_px = t0 - mess["t_min"]
    boden_px = mess["t_max"] - t1
    e = 2.0 * R / d_px                                           # Einheiten je Bildpunkt (quer gemessen)
    # Boden: die Haelfte der schmaleren Bodenlage ist die Fase, die andere Haelfte die (geneigt
    # gesehene) Bodenscheibe selbst -> zaehlt zur Mantellaenge. PORT-WAHL, Dossier.
    fase = int(round(0.5 * boden_px * e))
    l_zyl = int(round((l_zyl_px + 0.5 * boden_px) * e)) + fase
    kopf_px = mess["t_min"] - mess["hebel_t"][0]                 # Kuppel-Ende bis Hebelende auf der Achse
    return dict(d_px=d_px, l_zyl_px=l_zyl_px, verh=verh, t_zyl=(t0, t1), L=l_zyl, fase=fase,
                kuppel=int(round(schulter_px * e)), kopf=int(round(kopf_px * e)),
                schulter_px=schulter_px, boden_px=boden_px, kopf_px=kopf_px)


# ============================================================================================
# 3. Textur (128x128): Mantel-Abwicklung + Farbfelder
# ============================================================================================
FELD = {            # Farbfelder (x0, y0) je 8x8, Mitte = (x0+4, y0+4)
    "rot":     (80, 0),
    "rot_d":   (88, 0),
    "chrom":   (96, 0),
    "metall":  (104, 0),
    "schwarz": (112, 0),
    "chrom_h": (120, 0),
}
MANTEL_W, MANTEL_H = 65, 96          # Spalten 0..64 (Spalte 64 = Spalte 0, Naht), Zeilen 0..95


def bilinear(rgb, x, y):
    h, w, _ = rgb.shape
    x = min(max(x, 0.0), w - 1.001)
    y = min(max(y, 0.0), h - 1.001)
    x0, y0 = int(x), int(y)
    fx, fy = x - x0, y - y0
    a = rgb[y0, x0] * (1 - fx) + rgb[y0, x0 + 1] * fx
    b = rgb[y0 + 1, x0] * (1 - fx) + rgb[y0 + 1, x0 + 1] * fx
    return a * (1 - fy) + b * fy


def textur_bauen(rgb, mess, mass):
    c, ax, pe = mess["zentrum"], mess["achse"], mess["quer"]
    t0, t1 = mass["t_zyl"]
    r_px = 0.5 * mass["d_px"]
    q = 0.80 * r_px                     # Randpixel (Kantenglaettung mit dem blauen Grund) nicht abtasten
    mantel = np.zeros((MANTEL_H, MANTEL_W, 3))
    quer = np.zeros((MANTEL_H, MANTEL_W))
    for row in range(MANTEL_H):
        # Zeile 0 = oben (Schulter, Bild-t0), Zeile 95 = unten (Boden, Bild-t1)
        t = t0 + (row + 0.5) / MANTEL_H * (t1 - t0)
        for col in range(MANTEL_W):
            phi = (col % (MANTEL_W - 1)) / float(MANTEL_W - 1) * 2 * math.pi
            s = q * math.sin(phi)
            p = c + t * ax + s * pe
            mantel[row, col] = bilinear(rgb, p[0], p[1])
            quer[row, col] = s
    # ENT-SCHATTEN: das Bild ist gerendert (Glanzlicht oben links). Helligkeit je Querlage s
    # ueber alle Zeilen mitteln (nur ROTE Pixel, Etikett/Flecken haben andere Helligkeit) und
    # auf das Gesamtmittel normieren.
    lum = mantel.sum(-1)
    rot = (mantel[..., 0] > mantel[..., 1] * 1.25) & (mantel[..., 0] > 90)
    bins = np.linspace(-q, q, 9)
    prof = np.ones_like(lum)
    gesamt = lum[rot].mean() if rot.any() else lum.mean()
    for i in range(len(bins) - 1):
        sel = (quer >= bins[i]) & (quer <= bins[i + 1])
        selr = sel & rot
        m = lum[selr].mean() if selr.sum() > 10 else gesamt
        prof[sel] = m / gesamt
    mantel = mantel / prof[..., None]
    # HELLIGKEIT auf die Original-Item-Skala (Weiss = Weiss der ROOM4050-Spraydose)
    ref = ref_weiss()
    s = mantel.sum(-1).reshape(-1)
    weiss = float(np.percentile(s, 99.5))
    faktor = ref / weiss
    mantel = np.clip(mantel * faktor, 0, 255)

    tex = np.zeros((TEX_H, TEX_W, 3))
    tex[:MANTEL_H, :MANTEL_W] = mantel
    # Farbfelder: Rot = Mittel der roten Mantelpixel (Schulter/Boden), Chrom aus den Hebelpixeln
    rotmittel = mantel[rot].mean(0) if rot.any() else np.array([150.0, 40, 30])
    chrom = np.clip(mess["chrom"] * faktor, 0, 255)
    farben = {
        "rot": rotmittel,
        "rot_d": rotmittel * 0.70,
        "chrom": chrom,
        "metall": chrom * 0.62,
        "schwarz": np.array([34.0, 34.0, 36.0]),
        "chrom_h": np.clip(chrom * 1.15, 0, 255),
    }
    for name, (x0, y0) in FELD.items():
        tex[y0:y0 + 8, x0:x0 + 8] = farben[name]
    return np.clip(tex, 0, 255).astype(np.uint8), dict(faktor=faktor, weiss_vorher=weiss, ref=ref,
                                                         rot=rotmittel, chrom=chrom)


def feld_uv(name):
    x0, y0 = FELD[name]
    return (x0 + 4, y0 + 4)


# ============================================================================================
# 4. Netz (stehend: y nach UNTEN, Boden bei y=0, Achse = -Y)
# ============================================================================================
class Netz:
    def __init__(self):
        self.v = []          # (x,y,z)
        self.n = []          # (nx,ny,nz) normiert (float)
        self.tris = []       # [(vi,ni,(u,v)) x3]
        self.quads = []      # [(vi,ni,(u,v)) x4] im UMLAUF a-b-c-d

    def pv(self, p):
        self.v.append(tuple(float(c) for c in p))
        return len(self.v) - 1

    def pn(self, n):
        n = np.asarray(n, float)
        ln = np.linalg.norm(n)
        self.n.append(tuple(n / ln) if ln > 1e-9 else (0.0, -1.0, 0.0))
        return len(self.n) - 1

    def quad(self, ecken):
        self.quads.append(ecken)

    def tri(self, ecken):
        self.tris.append(ecken)


def ring(netz, h, r, n_ring=None):
    """SEITEN+1 Punkte (Naht doppelt) auf Hoehe h (y = -h). Normalen radial (n_ring = (radial, hoch))."""
    vi, ni = [], []
    for k in range(SEITEN + 1):
        phi = 2 * math.pi * (k % SEITEN) / SEITEN
        sx, cz = math.sin(phi), math.cos(phi)
        vi.append(netz.pv((r * sx, -h, r * cz)))
        if n_ring is None:
            ni.append(netz.pn((sx, 0.0, cz)))
        else:
            rad, hoch = n_ring
            ni.append(netz.pn((rad * sx, -hoch, rad * cz)))
    return vi, ni


def band(netz, a, b, uv_a, uv_b):
    """Viereck-Band zwischen Ring a (unten) und Ring b (oben). uv_x(k) -> (u,v)."""
    (va, na), (vb, nb) = a, b
    for k in range(SEITEN):
        netz.quad([(vb[k], nb[k], uv_b(k)), (vb[k + 1], nb[k + 1], uv_b(k + 1)),
                   (va[k + 1], na[k + 1], uv_a(k + 1)), (va[k], na[k], uv_a(k))])


def kasten(netz, x0, x1, y0, y1, z0, z1, uv):
    """Quader (achsparallel), Flaechennormalen, alle Ecken auf demselben Farbfeld."""
    P = lambda x, y, z: (x, y, z)
    flaechen = [
        ((1, 0, 0),  [P(x1, y0, z0), P(x1, y0, z1), P(x1, y1, z1), P(x1, y1, z0)]),
        ((-1, 0, 0), [P(x0, y0, z1), P(x0, y0, z0), P(x0, y1, z0), P(x0, y1, z1)]),
        ((0, -1, 0), [P(x0, y0, z0), P(x0, y0, z1), P(x1, y0, z1), P(x1, y0, z0)]),
        ((0, 1, 0),  [P(x0, y1, z1), P(x0, y1, z0), P(x1, y1, z0), P(x1, y1, z1)]),
        ((0, 0, -1), [P(x0, y0, z0), P(x1, y0, z0), P(x1, y1, z0), P(x0, y1, z0)]),
        ((0, 0, 1),  [P(x1, y0, z1), P(x0, y0, z1), P(x0, y1, z1), P(x1, y1, z1)]),
    ]
    for nrm, ecken in flaechen:
        ni = netz.pn(nrm)
        netz.quad([(netz.pv(p), ni, uv) for p in ecken])


def rohr(netz, punkte, rh, uv):
    """Vierkant-Rohr (Schlauch) entlang einer Polylinie; Querschnitt-Ebene senkrecht zur Strecke."""
    pts = [np.asarray(p, float) for p in punkte]
    ringe = []
    for i, p in enumerate(pts):
        d = (pts[min(i + 1, len(pts) - 1)] - pts[max(i - 1, 0)])
        d /= np.linalg.norm(d)
        a = np.cross(d, [0.0, 0.0, 1.0])
        if np.linalg.norm(a) < 1e-6:
            a = np.cross(d, [1.0, 0.0, 0.0])
        a /= np.linalg.norm(a)
        b = np.cross(d, a)
        ecken = [p + rh * (a * math.cos(k * math.pi / 2 + math.pi / 4) + b * math.sin(k * math.pi / 2 + math.pi / 4))
                 for k in range(4)]
        ringe.append([(netz.pv(e), netz.pn(e - p)) for e in ecken])
    for i in range(len(ringe) - 1):
        r0, r1 = ringe[i], ringe[i + 1]
        for k in range(4):
            k2 = (k + 1) % 4
            netz.quad([(r0[k][0], r0[k][1], uv), (r0[k2][0], r0[k2][1], uv),
                       (r1[k2][0], r1[k2][1], uv), (r1[k][0], r1[k][1], uv)])
    # Endkappen
    for ring_, vorzeichen in ((ringe[0], -1), (ringe[-1], 1)):
        p0 = pts[0] if vorzeichen < 0 else pts[-1]
        p1 = pts[1] if vorzeichen < 0 else pts[-2]
        nrm = (p0 - p1)
        ni = netz.pn(nrm)
        netz.quad([(ring_[k][0], ni, uv) for k in (0, 1, 2, 3)])


def netz_bauen(mass):
    """Stehendes Netz. Rueckgabe: Netz + Kennzahlen."""
    L = mass["L"]
    netz = Netz()

    def uv_mantel(h_rel):
        # h_rel 0 = Boden, 1 = Zylinderende oben -> Zeile 95..0
        v = int(round((1.0 - h_rel) * (MANTEL_H - 1)))
        return lambda k: (int(round(k * (MANTEL_W - 1) / SEITEN)), v)

    rot = lambda k: feld_uv("rot")
    rot_d = lambda k: feld_uv("rot_d")

    # Boden: Scheibe (Dreiecksfaecher) + Fase (gemessen, abmessungen())
    fase = mass["fase"]
    r_boden = ring(netz, 0.0, R - fase, (0.55, -0.83))
    r_unten = ring(netz, fase, R)
    mitte = netz.pv((0.0, 0.0, 0.0))
    n_mitte = netz.pn((0.0, 1.0, 0.0))
    for k in range(SEITEN):
        netz.tri([(mitte, n_mitte, feld_uv("rot_d")),
                  (r_boden[0][k + 1], n_mitte, feld_uv("rot_d")),
                  (r_boden[0][k], n_mitte, feld_uv("rot_d"))])
    band(netz, r_boden, r_unten, rot_d, uv_mantel(fase / L))
    # Mantel in 3 Baendern (Lichtverlauf je Ecke)
    stufen = [fase, L * 0.36, L * 0.70, L]
    ringe = [r_unten] + [ring(netz, h, R) for h in stufen[1:]]
    for i in range(len(stufen) - 1):
        band(netz, ringe[i], ringe[i + 1], uv_mantel(stufen[i] / L), uv_mantel(stufen[i + 1] / L))
    # Schulter (Kuppel) zum Hals
    K = float(mass["kuppel"])            # Kuppelhoehe (gemessen: Schulterlage im Bild)
    profil = [(L, 1.00), (L + 0.31 * K, 0.93), (L + 0.61 * K, 0.76), (L + 0.87 * K, 0.50),
              (L + 1.00 * K, 0.30)]
    vor = ringe[-1]
    for i in range(1, len(profil)):
        h, f = profil[i]
        hp, fp = profil[i - 1]
        # Normale der Schulter: senkrecht auf das Profil (radial nach aussen, nach oben)
        dr = (f - fp) * R
        dh = h - hp
        nr = ring(netz, h, f * R, (dh, -dr))
        band(netz, vor, nr, rot, rot)
        vor = nr
    h_hals0 = profil[-1][0]
    r_hals = 0.30 * R
    h_hals1 = h_hals0 + 0.30 * R
    hals = ring(netz, h_hals1, r_hals)
    band(netz, vor, hals, lambda k: feld_uv("metall"), lambda k: feld_uv("metall"))
    # Ventil-Koerper (Kasten), darauf Griff (fest) und Hebel (beweglich) nach -X, Schlauch nach +X.
    # Seite gemessen: im Bild zeigt der Hebel (Pixel s -13,8..11,3, Spitze bei (34,13)) bei
    # zugewandtem Etikett auf die Seite RECHTS-oberhalb der Achse; von +Z (Etikett) aus gesehen
    # ist das Welt -X (Bildrechts = -X, Vorschau `blick`).
    sx = -1.0
    def kx(a, b):
        return (min(sx * a, sx * b), max(sx * a, sx * b))
    vb = 0.36 * R
    y_v0, y_v1 = -h_hals1, -(h_hals1 + 0.62 * R)
    kasten(netz, -vb, vb, y_v1, y_v0, -0.30 * R, 0.30 * R, feld_uv("chrom"))
    lang = 1.55 * R                      # Hebel-/Grifflaenge ab Achse (Bild: s -13,8..11,3 px bei d 17 px)
    # Griff (unten, waagerecht)
    gy = y_v0 - 0.18 * R
    kasten(netz, *kx(vb * 0.5, lang), gy - 0.13 * R, gy, -0.16 * R, 0.16 * R, feld_uv("chrom"))
    # Hebel (oben, leicht angestellt): zwei Kaesten
    hy = y_v1 + 0.05 * R
    kasten(netz, *kx(vb * 0.5, lang * 0.55), hy - 0.12 * R, hy, -0.15 * R, 0.15 * R, feld_uv("chrom_h"))
    kasten(netz, *kx(lang * 0.55, lang * 0.98), hy - 0.12 * R - 0.20 * R, hy - 0.20 * R, -0.15 * R, 0.15 * R,
           feld_uv("chrom_h"))
    # Schlauch (schwarz): aus dem Ventil zur Gegenseite, Bogen nach unten, am Mantel entlang, Duese
    sy = (y_v0 + y_v1) / 2
    hs = -sx
    pfad = [(hs * vb, sy, 0.0), (hs * (vb + 0.55 * R), sy, 0.0), (hs * 1.12 * R, sy + 0.45 * R, 0.0),
            (hs * 1.18 * R, -(L * 0.80), 0.0), (hs * 1.16 * R, -(L * 0.58), 0.0)]
    rohr(netz, pfad, 0.13 * R, feld_uv("schwarz"))
    # Duese (dickeres Endstueck)
    rohr(netz, [(hs * 1.16 * R, -(L * 0.58), 0.0), (hs * 1.15 * R, -(L * 0.50), 0.0)], 0.20 * R,
         feld_uv("schwarz"))
    hoehe = -(min(p[1] for p in netz.v))
    return netz, dict(L=L, R=R, hoehe=hoehe, hals=h_hals1, hebel=lang)


# ============================================================================================
# 5. Lagen + MD1 + TIM
# ============================================================================================
def lage(netz, art):
    """stehend: unveraendert. liegend: (x,y,z) -> (h - H/2, -(z + R), x) mit h = -y."""
    if art == "stehend":
        return [tuple(v) for v in netz.v], [tuple(n) for n in netz.n]
    H = -(min(p[1] for p in netz.v))
    vs = [(-y - H / 2.0, -(z + R), x) for (x, y, z) in netz.v]
    ns = [(-ny, -nz, nx) for (nx, ny, nz) in netz.n]
    return vs, ns


def md1_bauen(netz, art):
    vs, ns = lage(netz, art)
    verts = [tuple(int(round(c)) for c in p) for p in vs]
    norms = [tuple(int(round(c * 4096)) for c in n) for n in ns]
    tris, quads = netz.tris, netz.quads
    base, hdr = 12, 56
    off_v = hdr
    off_n = off_v + len(verts) * 8
    off_tf = off_n + len(norms) * 8
    off_tuv = off_tf + len(tris) * 12
    off_qf = off_tuv + len(tris) * 12
    off_quv = off_qf + len(quads) * 16
    ende = off_quv + len(quads) * 16
    d = bytearray(base + ende)
    struct.pack_into("<III", d, 0, 65, 0, 2)
    struct.pack_into("<14I", d, 12, off_v, len(verts), off_n, len(norms), off_tf, len(tris), off_tuv,
                     off_v, len(verts), off_n, len(norms), off_qf, len(quads), off_quv)
    for i, p in enumerate(verts):
        struct.pack_into("<4h", d, base + off_v + i * 8, p[0], p[1], p[2], 0)
    for i, n in enumerate(norms):
        struct.pack_into("<4h", d, base + off_n + i * 8, n[0], n[1], n[2], 0)
    for i, f in enumerate(tris):
        (va, na, ua), (vb, nb, ub), (vc, nc, uc) = f
        struct.pack_into("<6H", d, base + off_tf + i * 12, na, va, nb, vb, nc, vc)
        struct.pack_into("<BBHBBHBBH", d, base + off_tuv + i * 12, ua[0], ua[1], CLUT_WORT,
                         ub[0], ub[1], PAGE_WORT, uc[0], uc[1], 0)
    # ⛔ Vierecke in Z-ORDNUNG (Umlauf a-b-c-d -> a-b-d-c), sicherung_engine_export.py §3.3
    for i, f in enumerate(quads):
        a, b, c, e = f
        z = (a, b, e, c)
        struct.pack_into("<8H", d, base + off_qf + i * 16, z[0][1], z[0][0], z[1][1], z[1][0],
                         z[2][1], z[2][0], z[3][1], z[3][0])
        struct.pack_into("<BBHBBHBBHBBH", d, base + off_quv + i * 16,
                         z[0][2][0], z[0][2][1], CLUT_WORT, z[1][2][0], z[1][2][1], PAGE_WORT,
                         z[2][2][0], z[2][2][1], 0, z[3][2][0], z[3][2][1], 0)
    return bytes(d), verts, norms


def tim_bauen(tex):
    """128x128 8bpp + 256er-CLUT, Index 0 frei (Farbschluessel)."""
    im = Image.fromarray(tex)
    pal = im.convert("P", palette=Image.ADAPTIVE, colors=255)
    idx = np.asarray(pal, np.uint8).astype(np.int32) + 1
    roh = np.asarray(pal.getpalette()[:255 * 3], np.uint8).reshape(255, 3)
    clut = [0x0000]
    for r, g, b in roh:
        w = ((int(b) >> 3) << 10) | ((int(g) >> 3) << 5) | (int(r) >> 3)
        clut.append(w if w != 0 else 0x0421)      # echtes Schwarz NIE als 0x0000 (waere durchsichtig)
    while len(clut) < 256:
        clut.append(0x0421)
    aus = bytearray(struct.pack("<II", 0x10, 0x09))
    aus += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + b"".join(struct.pack("<H", c) for c in clut)
    bild = idx.astype(np.uint8).tobytes()
    aus += struct.pack("<IHHHH", 12 + len(bild), 0, 0, TEX_W // 2, TEX_H) + bild
    return bytes(aus)


# ============================================================================================
# 6. Vorschau (Software-Rasterer, nur fuer das Dossier)
# ============================================================================================
def tim_zu_rgb(tim):
    idx, clut = tim_lesen(tim)
    pal = np.array([clut_rgb(c) for c in clut], np.uint8)
    return pal[idx]


def raster(verts, norms, netz, tex, R3, t3, f, groesse, licht=(0.4, -0.7, -0.6), hg=None, H=None,
           zentrum=None):
    """Projiziert und rastert das Netz. R3 = Welt->Kamera (3x3), t3 Verschiebung; f = Brennweite
    (H); zentrum = Bildmitte. Rueckgabe RGB-Bild (+ Tiefe). Viereck in (0,1,3)+(0,3,2) wie der Port."""
    W, Hh = groesse
    bild = np.array(hg, np.float64) if hg is not None else np.full((Hh, W, 3), 60.0)
    zb = np.full((Hh, W), np.inf)
    cx, cy = zentrum if zentrum else (W / 2.0, Hh / 2.0)
    L = np.asarray(licht, float)
    L /= np.linalg.norm(L)
    P = np.array(verts, float) @ np.asarray(R3, float).T + np.asarray(t3, float)
    N = np.array(norms, float) @ np.asarray(R3, float).T
    scr = np.stack([cx + f * P[:, 0] / P[:, 2], cy + f * P[:, 1] / P[:, 2], P[:, 2]], 1)

    def dreieck(ecken):
        (va, na, ua), (vb, nb, ub), (vc, nc, uc) = ecken
        pa, pb, pc = scr[va], scr[vb], scr[vc]
        if min(pa[2], pb[2], pc[2]) < 10:
            return
        x0 = int(max(0, math.floor(min(pa[0], pb[0], pc[0]))))
        x1 = int(min(W - 1, math.ceil(max(pa[0], pb[0], pc[0]))))
        y0 = int(max(0, math.floor(min(pa[1], pb[1], pc[1]))))
        y1 = int(min(Hh - 1, math.ceil(max(pa[1], pb[1], pc[1]))))
        den = (pb[1] - pc[1]) * (pa[0] - pc[0]) + (pc[0] - pb[0]) * (pa[1] - pc[1])
        if abs(den) < 1e-9:
            return
        sh = [max(0.25, 0.35 + 0.85 * max(0.0, -float(np.dot(N[n] / (np.linalg.norm(N[n]) + 1e-9), L))))
              for n in (na, nb, nc)]
        for yy in range(y0, y1 + 1):
            for xx in range(x0, x1 + 1):
                px, py = xx + 0.5, yy + 0.5
                l0 = ((pb[1] - pc[1]) * (px - pc[0]) + (pc[0] - pb[0]) * (py - pc[1])) / den
                l1 = ((pc[1] - pa[1]) * (px - pc[0]) + (pa[0] - pc[0]) * (py - pc[1])) / den
                l2 = 1 - l0 - l1
                if l0 < -1e-6 or l1 < -1e-6 or l2 < -1e-6:
                    continue
                z = l0 * pa[2] + l1 * pb[2] + l2 * pc[2]
                if z >= zb[yy, xx]:
                    continue
                zb[yy, xx] = z
                u = l0 * ua[0] + l1 * ub[0] + l2 * uc[0]
                v = l0 * ua[1] + l1 * ub[1] + l2 * uc[1]
                col = tex[min(TEX_H - 1, max(0, int(v))), min(TEX_W - 1, max(0, int(u)))].astype(float)
                bild[yy, xx] = np.clip(col * (l0 * sh[0] + l1 * sh[1] + l2 * sh[2]), 0, 255)

    for t in netz.tris:
        dreieck(t)
    for q in netz.quads:
        a, b, c, e = q
        dreieck((a, b, c))
        dreieck((a, c, e))
    return bild.astype(np.uint8), zb


def blick(mitte, az, el, dist):
    """Kamera auf einer Kugel um `mitte` (Azimut/Elevation in Grad). PSX: +y nach UNTEN.
    Sichtkoordinaten x rechts, y unten, z vorwaerts: rechts = (0,1,0) x f, unten = f x rechts."""
    a, e = math.radians(az), math.radians(el)
    d = np.array([math.sin(a) * math.cos(e), -math.sin(e), math.cos(a) * math.cos(e)])
    C = np.asarray(mitte, float) + d * dist
    f = -d
    rechts = np.cross([0.0, 1.0, 0.0], f)
    rechts /= np.linalg.norm(rechts)
    unten = np.cross(f, rechts)
    R3 = np.stack([rechts, unten, f])
    return R3, -(R3 @ C)


def vorschau(netz, tex, art, name, groesse=(240, 300)):
    vs, ns = lage(netz, art)
    P = np.array(vs, float)
    mitte = (P.max(0) + P.min(0)) / 2
    ausd = float(np.max(P.max(0) - P.min(0)))
    bilder = []
    for az, el in ((0, 10), (60, 25), (90, 5), (180, 10), (35, 60)):
        R3, t3 = blick(mitte, az, el, ausd * 1.5)
        img, _ = raster(vs, ns, netz, tex, R3, t3, groesse[1] * 1.0, groesse)
        bilder.append(Image.fromarray(img))
    blatt = Image.new("RGB", (groesse[0] * len(bilder), groesse[1] + 16), (30, 30, 30))
    d = ImageDraw.Draw(blatt)
    for i, b in enumerate(bilder):
        blatt.paste(b, (i * groesse[0], 16))
    d.text((4, 2), "Feuerloescher-Modell (%s) - Ansichten 0/60/90/180 Grad, Aufsicht 60 Grad" % art,
           fill=(255, 255, 0))
    os.makedirs(BELEGE, exist_ok=True)
    blatt.save(os.path.join(BELEGE, name))


def vergleich_bild(rgb, tex, netz, mess, name):
    """Original-Bild (ITPS 0x31) neben dem STEHENDEN Modell in derselben Lage: Etikett zur
    Kamera, Achse wie im Bild (Kopf oben links, gemessene Achse), gleiche Bildgroesse."""
    vs, ns = lage(netz, "stehend")
    P = np.array(vs, float)
    mitte = (P.max(0) + P.min(0)) / 2
    ausd = float(np.max(P.max(0) - P.min(0)))
    R3, t3 = blick(mitte, 0, 0, ausd * 2.0)
    kopf = -np.asarray(mess["achse"], float)                       # Bildrichtung Boden -> Kopf
    th = math.atan2(kopf[1], kopf[0]) - math.atan2(-1.0, 0.0)      # Modell-oben (0,-1) -> Kopf
    c_, s_ = math.cos(th), math.sin(th)
    r_, d_ = R3[0].copy(), R3[1].copy()
    R3 = np.stack([c_ * r_ - s_ * d_, s_ * r_ + c_ * d_, R3[2]])
    t3 = np.array([c_ * t3[0] - s_ * t3[1], s_ * t3[0] + c_ * t3[1], t3[2]])
    W, Hh = 224, 144
    # Massstab so, dass der Koerperdurchmesser im Bild dem gemessenen (x2) entspricht
    f = 2.0 * 17.05 * (ausd * 2.0) / (2 * R)
    img, _ = raster(vs, ns, netz, tex, R3, t3, f, (W, Hh), hg=np.full((Hh, W, 3), (0, 0, 56)),
                    zentrum=(2 * mess["zentrum"][0], 2 * mess["zentrum"][1]))
    links = Image.fromarray(rgb.astype(np.uint8)).resize((W, Hh), Image.NEAREST)
    blatt = Image.new("RGB", (W * 2 + 8, Hh + 16), (30, 30, 30))
    blatt.paste(links, (0, 16))
    blatt.paste(Image.fromarray(img), (W + 8, 16))
    ImageDraw.Draw(blatt).text((4, 2), "ITPS 0x31 Original (2x)  |  Modell, gleiche Achse + Groesse",
                               fill=(255, 255, 0))
    blatt.save(os.path.join(BELEGE, name))


# ============================================================================================
# 7. Untersuchen-Text (PORT-WAHL, Wortlaut zur Freigabe im Dossier)
# ============================================================================================
TEXT_LEER = "It's an empty fire extinguisher."


def text_bauen():
    """Einseitiger Untersuchungstext `04 02 <Text> 01 00` = Form von ROOM1000 msg 0 @0x0D22
    (`04 02 2a 4b 50 44 45 4a 43 00 51 4a 51 4f 51 3d 48 57 01 00` "Nothing unusual.").
    Glyphen/Breiten aus tools/r36_p/texte_bauen.py (Umkehrung von msg_common.c re15_msg_glyph,
    Breiten include/font_width.h); Zeile <= 271 px, Laenge <= 128 B."""
    sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "r36_p"))
    import texte_bauen as T    # noqa: E402
    roh = T.enc(TEXT_LEER)
    breite = T.px(roh)
    if breite > T.MAX_PX:
        raise SystemExit("Text zu breit: %d px" % breite)
    b = bytes([0x04, 0x02]) + roh + bytes([0x01, 0x00])
    if len(b) > T.MAX_LEN:
        raise SystemExit("Text zu lang: %d B" % len(b))
    return b, breite


# ============================================================================================
# 8. C-Einbindung
# ============================================================================================
def carr(f, name, b):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    rgb, off = itps_bild()
    mess = koerper_messen(rgb)
    mass = abmessungen(mess)
    print("ITPS.ITP Block 0x%02X @file 0x%06X: Achse %.1f Grad, t %.1f..%.1f" % (
        ITEM_ID, off, math.degrees(math.atan2(mess["achse"][1], mess["achse"][0])), mess["t_min"], mess["t_max"]))
    print("  Breiten je 4-px-Scheibe:", ", ".join("%.1f" % b for (_k, b, _n) in mess["breiten"]))
    print("  Durchmesser %.2f px, Zylinder %.1f px (t %.1f..%.1f) -> Laenge/Durchmesser %.3f" % (
        mass["d_px"], mass["l_zyl_px"], mass["t_zyl"][0], mass["t_zyl"][1], mass["verh"]))
    print("  Schulter %.1f px, Boden %.1f px, Hebel t %.1f..%.1f s %.1f..%.1f, Chrom %s" % (
        mass["schulter_px"], mass["boden_px"], mess["hebel_t"][0], mess["hebel_t"][1],
        mess["hebel_s"][0], mess["hebel_s"][1], np.round(mess["chrom"]).astype(int).tolist()))
    print("Massstab: %.1f Einheiten/m, R = %d, Mantel L = %d (Fase %d), Kuppel %d, Kopf ueber der Kuppel %d" % (
        EINHEITEN_JE_M, R, mass["L"], mass["fase"], mass["kuppel"], mass["kopf"]))
    if "--nur-mass" in sys.argv:
        return
    tex, tinfo = textur_bauen(rgb, mess, mass)
    print("Textur: Weiss vorher %.0f, Referenz ROOM4050 %.0f -> Faktor %.3f; Rot %s, Chrom %s" % (
        tinfo["weiss_vorher"], tinfo["ref"], tinfo["faktor"], np.round(tinfo["rot"]).astype(int).tolist(),
        np.round(tinfo["chrom"]).astype(int).tolist()))
    netz, kz = netz_bauen(mass)
    md1_s, vs_s, _ = md1_bauen(netz, "stehend")
    md1_l, vs_l, _ = md1_bauen(netz, "liegend")
    tim = tim_bauen(tex)
    def bbox(vs):
        a = np.array(vs)
        return a.min(0).tolist(), a.max(0).tolist()
    print("Netz: %d Punkte, %d Dreiecke, %d Vierecke; Hoehe %.0f (%.2f m), Hebel %.0f" % (
        len(netz.v), len(netz.tris), len(netz.quads), kz["hoehe"], kz["hoehe"] / EINHEITEN_JE_M, kz["hebel"]))
    print("  stehend bbox", bbox(vs_s), " liegend bbox", bbox(vs_l))
    print("MD1 stehend %d B, liegend %d B, TIM %d B" % (len(md1_s), len(md1_l), len(tim)))
    os.makedirs(os.path.dirname(ZIEL_INC), exist_ok=True)
    with io.open(ZIEL_INC, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/r37_feuerloescher/feuerloescher_modell.py — NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Welt-Modell des FEUERLOESCHERS (Item 0x31 \"Fire Extinguisher\"), Runde 37 Spur I.\n"
                " * Neu gebaut (PORT-WAHL): weder RE1.5 noch RE2 haben ein Modell (Vorarbeit\n"
                " * analysis/befunde_2026-09-21/re2-feuerloescher-modell.md). Vorlage = Gegenstands-Bild\n"
                " * ITEM/ITPS.ITP Block 0x31 @file 0x%06X (Achse, Verhaeltnis, Mantel-Textur, Chrom).\n"
                " * Massstab: Leon PL00.PLD 3022 Einheiten = 1,78 m -> %.1f/m; Durchmesser 13 cm -> R = %d;\n"
                " * Zylinder L = %d (Bildverhaeltnis %.3f), Gesamthoehe %.0f (%.2f m).\n"
                " * Format wie gen/sicherung_prop.inc: MD1-Kopf 65/0/2, Normalen Q12 nach aussen, Vierecke in\n"
                " * Z-Ordnung, TIM 8bpp+CLUT 128x128 @VRAM(0,0), CLUT @VRAM(0,480), clut 0x7800, page 0x80.\n"
                " * Textur-Helligkeit x%.3f = Weiss der Original-Spraydose ROOM4050 TIM @0x2DDA8 (%.0f).\n"
                " * stehend: Ursprung Bodenmitte, Achse -Y, Hebel +X, Etikett +Z, Schlauch -X.\n"
                " * liegend: Ursprung Mitte auf der Auflage (y=0), Achse X, Etikett oben (-Y), Hebel +Z.\n"
                " * %d Punkte, %d Dreiecke, %d Vierecke.\n"
                " */\n\n" % (off, EINHEITEN_JE_M, R, mass["L"], mass["verh"], kz["hoehe"],
                             kz["hoehe"] / EINHEITEN_JE_M, tinfo["faktor"], tinfo["ref"],
                             len(netz.v), len(netz.tris), len(netz.quads)))
        carr(f, "k_fl_md1_stehend", md1_s)
        carr(f, "k_fl_md1_liegend", md1_l)
        carr(f, "k_fl_tim", tim)
        txt, breite = text_bauen()
        f.write("/* msg %s: \"%s\" (%d px, Form ROOM1000 msg 0 @0x0D22) */\n" % (
            "RE15_FL37_MSG_LEER", TEXT_LEER, breite))
        carr(f, "k_fl_msg_leer", txt)
    print("geschrieben:", os.path.relpath(ZIEL_INC, REPO))
    os.makedirs(BELEGE, exist_ok=True)
    texb = tim_zu_rgb(tim)
    Image.fromarray(texb).resize((TEX_W * 3, TEX_H * 3), Image.NEAREST).save(os.path.join(BELEGE, "modell_textur.png"))
    vorschau(netz, texb, "stehend", "modell_stehend.png")
    vorschau(netz, texb, "liegend", "modell_liegend.png")
    vergleich_bild(rgb, texb, netz, mess, "modell_vergleich_itps.png")
    print("Bilder:", os.path.relpath(BELEGE, REPO))


if __name__ == "__main__":
    main()
