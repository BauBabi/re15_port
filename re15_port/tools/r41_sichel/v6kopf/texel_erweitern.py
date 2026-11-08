"""SF V6 Kopf: Kopftextur des ORIGINAL RE1.5-Birkin (EM30 Mesh 9, TPage 128, CLUT-Zeile 0) erweitern - nur Indizes der
Original-Textur, keine neue Farbe. Koordinaten = TIM-Texel (x = u, da TPage 128 bei x 0 beginnt; y = v).

Lesart der Original-Kopftextur (Texelkarte sfv6/tex_grid.png, tex_unten_x10.png; L*-Raster sfv6/r3.py / r4.py, Original-Palette Zeile 0):
  * Die Textur liegt aufrecht und ungespiegelt: Bild-links der Textur = (W)-rechte Gesichtshaelfte = Bild-links in der Vorderansicht
    (Pose 135: sichtbare Texel u 65-126, die Bild-rechte Kopfhaelfte zeigt das violette Feld u 96-127; Pose P2 (Foto 2, (W)-rechts zur
    Kamera): sichtbare Texel u 59-113).
  * Haar (blond): b* 10-25 (gelblich), oben u 56-127 v 136-165 und am linken Rand u 56-70 bis v ~190.
  * Grauer Fleck (W)-rechts = Bild-links ("links in seinem Gesicht"): dunkelgrauer Streifen unter dem (W)-rechten Auge, u 58-72
    v 188-226, L* 1-30, b* < 10.
  * Hellgrau in der Mitte: Stirn/Nasenruecken/Wange u 72-100 v 166-221, L* 55-84, a* 2-8, b* 0-5 (Adern des violetten Felds ab u 93).
  * (W)-rechtes Auge (Texturbild links): Lid/Weiss u 70-88 v 186-200 (Augenweiss L* 60-64 bei u 74-84 v 196).
  * (W)-linkes Auge (Texturbild rechts): rotes Auge im violetten Aderfeld u 100-127 v 170-212 - unveraendert (das war die "Haar"-Lesung).
  * Mund: offener Mund u 80-104 v 222-238 (Hoehle L* < 15, Zaehne L* 50-68, rote Lippen a* > 20), darunter rotes Kinnfleisch
    v 238-254; rechts davon (u 104-127 v 200-254) aufgerissene Wange (rot) - unveraendert.
Erweiterung (Nutzer 2026-11-08): (a) grauer Fleck -> die (W)-rechte Gesichtshaelfte bis zur Nase (Zone A); (b) Hellgrau der Mitte
-> ueber den Mund (Zone B), Mund geschlossen; (c) "Haare" (violettes Feld) unveraendert; (d) Augen unveraendert.
Verfahren (Machart der Original-Textur: Flecken mit weichem, ausgefranstem Rand, Texelrauschen; keine geraden Kanten):
  Gewicht je Zone: w = Rampe des vorzeichenbehafteten Abstands zum Zonenrand (Breite 2 x rand Texel), Rand um ein geglaettetes
  Rauschfeld (+-wackel Texel) verschoben; ein Texel wird ersetzt, wenn w > Schwelle (zweites Rauschfeld, gleichverteilt) -
  der Rand franst aus wie der Rand des Original-Flecks. Rauschen mit festem Startwert (PARAM 'saat'), Bau deterministisch.
  A: Rangabbildung. Jeder ersetzte Texel behaelt seinen RANG der Helligkeit (L* der Original-Palette) unter den ersetzten Texeln und
     bekommt den Index des grauen Flecks mit demselben Rang (Quantil der L*-sortierten Fleck-Texel): Struktur der Original-Textur
     (Wangenschatten, Falten), Farben des Flecks.
  B: Fortsetzen des Hellgrau. Ersetzter Texel = Texel des Hellgrau-Felds S an der entsprechenden Stelle (S als ein Stueck auf
     die Box von B gelegt, naechster Texel).
"""
import numpy as np, cv2
from skimage.color import rgb2lab

PARAM = dict(
    # Zone A: (W)-rechte Gesichtshaelfte bis zur Nase. Oben bis unter das (W)-rechte Auge (v 200 ab u 70), links ab dem Kopfrand
    # u 58 (Haarstraehnen u 56-57), rechts bis an die Nase (Nasenruecken/-fluegel u 88-96 v 196-214 bleiben hell), unten bis v 254,
    # ab v 222 bis zum Mundwinkel (Zone B ab u 82).
    zone_a=[(58, 186), (68, 186), (70, 200), (86, 201), (87, 221), (81, 223), (80, 254), (58, 254)],
    # grauer Fleck (Quelle der Indizes fuer A): Box u 58-74 v 186-226 ohne Haar (b* <= 10) bis L* 50 = der dunkelgraue Streifen mit
    # der grauen Haut dazwischen (L* P10/50/90 der Box 10 / 28 / 55, s. sfv6/r4.py); nur der dunkle Kern (L* <= 30) wirkte als glatte
    # dunkle Flaeche (Versuch sfv6/tex_vgl_a30.png)
    fleck_box=(58, 186, 74, 226), fleck_l=50.0, haar_b=10.0,
    # Zone B: Mund + rotes Kinn in der Mitte; oben die Oberlippe (v 222), rechts bis vor die aufgerissene Wange
    zone_b=[(81, 222), (100, 221), (105, 224), (106, 254), (81, 254)],
    # Hellgrau-Feld S (Stirn) ohne die Adern des violetten Felds (ab u 93) und ohne die Brauen (ab v 187): u 74-92, v 166-185
    # (L* 55-84, a* 2-8, b* 0-5). B = S als EIN zusammenhaengendes Stueck auf die Box von B gelegt (naechster Texel, Massstab
    # Breite/Hoehe von B zu S): die Hautstruktur der Stirn laeuft ueber den Mund weiter. Verworfen: Spiegelkachel (Rorschach-Muster
    # an den Spiegelachsen, sfv6/tex_vgl_spiegel.png), Zufallsflicken 3 x 3 (Schachbrett, sfv6/tex_vgl_flicken.png).
    hell_box=(74, 166, 92, 185),
    rand=1.5,                    # halbe Breite der Randrampe (Texel)
    wackel=1.5,                  # Randverschiebung durch das geglaettete Rauschfeld (+- Texel)
    saat=4108,                   # Startwert der Rauschfelder
)


def _poly_maske(poly, shape):
    m = np.zeros(shape, np.uint8)
    cv2.fillPoly(m, [np.array(poly, np.int32)], 1)
    return m.astype(bool)


def pal_lab(clut_zeile):
    c = np.asarray(clut_zeile, np.int64)
    rgb = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1).astype(float) / 255.0
    return rgb2lab(rgb[None])[0]


def _abstand(maske):
    """vorzeichenbehafteter Abstand (Texel) zum Rand: > 0 innen, < 0 aussen."""
    m = maske.astype(np.uint8)
    di = cv2.distanceTransform(m, cv2.DIST_L2, 3)
    da = cv2.distanceTransform(1 - m, cv2.DIST_L2, 3)
    return di - da


def zonen_gewicht(shape, p, rng):
    """(wA, wB, Schwelle): Gewichte 0..1 je Zone mit ausgefranstem Rand, Schwelle gleichverteilt."""
    w = []
    for z in ('zone_a', 'zone_b'):
        s = _abstand(_poly_maske(p[z], shape))
        n = cv2.GaussianBlur(rng.standard_normal(shape).astype(np.float32), (0, 0), 1.5)
        n = n / max(float(np.abs(n).max()), 1e-6) * p['wackel']
        w.append(np.clip((s + n + p['rand']) / (2 * p['rand']), 0.0, 1.0))
    return w[0], w[1], rng.random(shape)


def erweitern(pix, mesh9, frei, clut0, param=None):
    """pix: TIM-Indizes (256 x 384), frei: Texel, die nur Mesh 9 nutzt; clut0: Original-Zeile 0 (15 Bit). Rueckgabe (pix_neu, prot)."""
    p = dict(PARAM, **(param or {}))
    rng = np.random.default_rng(p['saat'])
    pix = pix.copy()
    alt = pix.copy()
    lab = pal_lab(clut0)
    L, B_ = lab[alt][..., 0], lab[alt][..., 2]
    wA, wB, schwelle = zonen_gewicht(pix.shape, p, rng)
    ersB = frei & (wB > schwelle) & (wB > 0)
    ersA = frei & (wA > schwelle) & (wA > 0) & ~ersB
    # --- Quelle A: grauer Fleck
    x0, y0, x1, y1 = p['fleck_box']
    fb = np.zeros(pix.shape, bool); fb[y0:y1 + 1, x0:x1 + 1] = True
    fleck = fb & (L <= p['fleck_l']) & (B_ <= p['haar_b']) & frei
    f_idx = alt[fleck]
    f_sort = f_idx[np.argsort(L[fleck], kind='stable')]
    ersA &= ~fleck                                   # der Fleck selbst bleibt
    ya, xa = np.nonzero(ersA)
    rang = np.argsort(np.argsort(L[ya, xa], kind='stable'), kind='stable')
    q = (rang + 0.5) / max(len(rang), 1)
    pix[ya, xa] = f_sort[np.minimum((q * len(f_sort)).astype(int), len(f_sort) - 1)]
    # --- Quelle B: Hellgrau-Feld gespiegelt fortsetzen
    hx0, hy0, hx1, hy1 = p['hell_box']
    nw, nh = hx1 - hx0 + 1, hy1 - hy0 + 1
    bx0 = min(x for x, _y in p['zone_b']); by0 = min(y for _x, y in p['zone_b'])
    yb, xb = np.nonzero(ersB)
    bx1 = max(x for x, _y in p['zone_b']) + 2; by1 = max(y for _x, y in p['zone_b']) + 2     # Box von B inkl. Randrampe
    bx0_, by0_ = bx0 - 2, by0 - 2
    sx = hx0 + np.minimum(((xb - bx0_) * nw) // (bx1 - bx0_ + 1), nw - 1)
    sy = hy0 + np.minimum(((yb - by0_) * nh) // (by1 - by0_ + 1), nh - 1)
    pix[yb, xb] = alt[sy, sx]
    geaendert = pix != alt
    quellen = set(int(i) for i in f_idx) | set(int(i) for i in alt[hy0:hy1 + 1, hx0:hx1 + 1].ravel())
    prot = dict(param={k: (list(v) if isinstance(v, tuple) else v) for k, v in p.items()},
                fleck_texel=int(fleck.sum()), fleck_indizes=sorted(set(int(i) for i in f_idx)),
                fleck_l_p10_50_90=[round(float(v), 1) for v in np.percentile(L[fleck], [10, 50, 90])],
                hell_indizes=sorted(set(int(i) for i in alt[hy0:hy1 + 1, hx0:hx1 + 1].ravel())),
                zone_a_ersetzt=int(len(xa)), zone_b_ersetzt=int(len(xb)),
                geaendert=int(geaendert.sum()), nicht_frei_geaendert=int((geaendert & ~frei).sum()),
                indizes_ausserhalb_quellen=sorted(set(int(i) for i in pix[geaendert]) - quellen),
                box_geaendert=[int(v) for v in (np.nonzero(geaendert)[1].min(), np.nonzero(geaendert)[0].min(),
                                                 np.nonzero(geaendert)[1].max(), np.nonzero(geaendert)[0].max())] if geaendert.any() else None,
                maske_a=ersA, maske_b=ersB)
    return pix, prot
