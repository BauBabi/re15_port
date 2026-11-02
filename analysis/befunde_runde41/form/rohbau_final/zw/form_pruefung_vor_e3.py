#!/usr/bin/env python3
"""Gemeinsames Messwerkzeug der Bahnsteig-Form: jeder Kandidat wird gleich geprueft.

    python re15_port/tools/form_pruefung.py ziele                     Zielmasken, Hintergruende, Grenzen, Lichteichung (einmalig)
    python re15_port/tools/form_pruefung.py ziele2 [--logo-neu]       Logo-Maske aus dem Video, Gesamtziele ziel2_bild*.png aus den Teilen des Nutzers
    python re15_port/tools/form_pruefung.py pruefe KANDIDAT.json --aus DIR
    python re15_port/tools/form_pruefung.py teile KANDIDAT.json --aus DIR   nur die Pruefung gegen die Teile des Nutzers, schneller
    python re15_port/tools/form_pruefung.py vergleiche DIR1 DIR2 ...  eine Zeile je Kandidat
    python re15_port/tools/form_pruefung.py dummy --aus DIR [--gebacken]   Pruefmodell aus formblatt.json als EMD + TIM + JSON
    python re15_port/tools/form_pruefung.py selbstprobe               Soll-Umriss als Render, Versatzprobe
    python re15_port/tools/form_pruefung.py teileprobe                Probe der Teile-Messung mit kleinem Dummy

KANDIDAT.json (Pfade relativ zur Datei):
    {"name": "A", "emd": "A.EMD", "tim": "A.TIM",
     "bild1": {"lage": [-18514, 0, -18726], "gier": 97},
     "bild2": {"lage": [2625, 0, -23625], "gier": 3129},
     "teile": {"tail": [12, 13, 14], "left_leg": [3, 4], "right_arm": [7, 8], "right_arm_front": [7, 8], ...}}
Optional je Bild "clip" und "bild" (Vorgabe Clip 0 / 1, Bild 0), oben "ruhe": {"clip": 2, "bild": 0} und "licht": "mess" | "disc".
"teile" ordnet Knochen den Teilen des Nutzers zu (nutzer_teile/lage.json). Derselbe Knochen darf in einem Teil von Bild 1 und einem von Bild 2 stehen,
right_arm und right_arm_front sind derselbe Arm. Weitere Namen sind erlaubt, sie haben keine Maske und zaehlen als fremde Teile.

Ausgabe in DIR: bericht.txt, bericht.json, blatt_bild1.png, blatt_bild2.png, ausschnitt_bild1.png, ausschnitt_bild2.png, rundum.png, rundum_hell.png,
mit "teile" zusaetzlich teile_bild1.png, teile_bild2.png. Gemessen wird gegen ziel2_bild*.png, sobald ziele2 lief, sonst gegen ziel_bild*.png.

⛔ KEIN RE-WERT. Das Werkzeug misst Vertraeglichkeit mit zwei Videostandbildern. Nichts hier gehoert in die Engine. Soll-Werte kommen nur aus
formblatt.json und lesung_zeichnung.json, Bildwerte aus bild1.png und bild2.png, Herleitung analysis/befunde_runde41/form/pruefung.md.
Vorrang hat die Lesung des Nutzers (nutzer_teile/). Die Anatomie in FORMBLATT.md ist ueberholt, seine Messungen gelten weiter, pruefung.md Abschnitt 14.
Das Werkzeug kennt keinen Knochenbau: es liest Ecken, Flaechen und Textur ueber form_emd und form_buehne und misst nur am Bild. Was ein Knochen darstellt, sagt allein "teile".
"""
import argparse, colorsys, json, math, os, shutil, sys
import numpy as np
import cv2
from PIL import Image, ImageDraw
from scipy.ndimage import map_coordinates
from skimage.graph import MCP_Geometric

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402
import form_emd as FE                   # noqa: E402
import form_buehne as FB                # noqa: E402
import form_kamera as FK                # noqa: E402
import form_video as FV                 # noqa: E402

FORM = FV.FORM
DATEN = os.path.join(FORM, 'pruefung')
BREITE, HOEHE = 640, 480
YK = np.array([.299, .587, .114], np.float32)

# Ecken liegen ganzzahlig im Port: GTE schneidet ab (form_buehne._gte), mit Phase +0,375 ist der Fehler -0,375..+0,625 Port-Pixel, B streckt rund 2fach.
# Gemessen Buehne gegen form_kamera: bis 2,08 px schraeg, also rund 1,25 px je Achse.
RASTER_PX = 1.25
# Formblatt Abschnitt 5: "Umriss x=250-300 oben in Bild 1 dunkel auf dunkel, +-10 px". Gilt auch sonst, wo keine Kante messbar ist.
OHNE_KANTE_PX = 10.0
# Unter diesem Hub im besten Kanal gilt eine Stelle als "ohne Kante". Kleinerer Hub liegt im Filterrauschen (farbe.md: MAE 15-21).
MIN_HUB = 18.0
# Lage-Spielraum der Zeichnung: halbe Kante 10-90 (Median 4,5 / 5,1 px, lesung_zeichnung.json bildtechnik) aufgerundet.
ZEICHNUNG_SPIEL_PX = 3
ZEICHNUNG_MIN_ANTEIL = 0.6      # Festlegung des Messwerkzeugs, Probe in pruefung.md, keine Bildaussage
FIGUR_SUCHE_PX = 45             # weiter weg zaehlt eine Umrisskante nicht mehr als dieselbe Kante
# Handschnitt gegen gemessene Kanten Abschnitt 7, Werte siehe pruefung.md 14.3 und teileprobe. Gleich der halben Kante oben.
TEIL_SAUM_PX = ZEICHNUNG_SPIEL_PX
SPITZE_MIN_PX = 12              # Festlegung: eine Spitze ragt so weit aus ihrem Sattel, kleinere Zacken sind Schnittrauschen des Handumrisses
# Punkte aus der alten Anatomie: bleiben im Bericht, zaehlen nicht mehr (pruefung.md 14.5). Die Masse selbst sind nicht widerlegt, nur ihre Deutung.
STILLGELEGT = {'b1.rundling_1': 'C4 und C5 sind getrennt gelesen, liegt 8 px innen in right_leg',
               'b1.rundling_2': 'C4 und C5 sind getrennt gelesen, liegt 18 px innen in right_leg',
               'HAKEN': 'Haken ist ueberholte Anatomie, die Stelle gehoert zu face_and_upper_body'}
STILL_GELESEN = 'gelesene Kuppelkante, der Nutzerumriss liegt 5-28 px weiter aussen'


# ═══════════════════════════════════════════════════ Quellen
_ablage = {}


def _json(pfad):
    if pfad not in _ablage:
        with open(pfad, encoding='utf-8') as f:
            _ablage[pfad] = json.load(f)
    return _ablage[pfad]


def formblatt():
    return _json(os.path.join(FORM, 'formblatt.json'))


def merkmale():
    return {m['id']: m for m in _json(os.path.join(FORM, 'lesung_zeichnung.json'))['merkmale']}


def bauteil(bild, name):
    for t in _json(os.path.join(FORM, 'lesung_gesamtbau.json'))['teile']:
        if t['bild'] == bild and t['name'] == name:
            return t
    raise KeyError(name)


def teil_b1(i):
    return next(t for t in formblatt()['teile_bild1'] if t['id'] == i)


def teil_b2(i):
    return next(t for t in formblatt()['teile_bild2'] if t['id'] == i)


def nicht_figur(i):
    return next(t for t in formblatt()['nicht_figur_bild1'] if t['id'] == i)


def lade_rgb(pfad):
    b = cv2.imread(pfad)
    if b is None:
        raise SystemExit('fehlt: ' + pfad)
    return b[:, :, ::-1].copy()


def original(n):
    return lade_rgb(os.path.join(FORM, 'bild%d.png' % n))


def daten(name):
    return os.path.join(DATEN, name)


def poly_maske(punkte, form=(HOEHE, BREITE)):
    m = np.zeros(form, np.uint8)
    cv2.fillPoly(m, [np.round(np.array(punkte, float)).astype(np.int32)], 1)
    return m.astype(bool)


def rechteck(x0, y0, x1, y1):
    m = np.zeros((HOEHE, BREITE), bool)
    m[max(y0, 0):min(y1 + 1, HOEHE), max(x0, 0):min(x1 + 1, BREITE)] = True
    return m


def scheibe(mx, my, r):
    m = np.zeros((HOEHE, BREITE), np.uint8)
    cv2.circle(m, (int(mx), int(my)), int(r), 1, -1)
    return m.astype(bool)


def weite(m, r):
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1, 2 * r + 1))
    return cv2.dilate(m.astype(np.uint8), k).astype(bool)


def enge(m, r):
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1, 2 * r + 1))
    return cv2.erode(m.astype(np.uint8), k).astype(bool)


# ═══════════════════════════════════════════════════ Kantenmessung im Original
def kanaele(rgb, rest=None):
    b = rgb.astype(np.float32)
    k = {'Y': b @ YK, 'R': b[..., 0], 'G': b[..., 1], 'B': b[..., 2]}
    if rest is not None:
        k['Rest'] = rest
    return k


def profil(y, p, n, r, st=0.25):
    t = np.arange(-r, r + st, st)
    n = np.asarray(n, float) / (np.linalg.norm(n) or 1.0)
    return t, map_coordinates(y, [p[1] + t * n[1], p[0] + t * n[0]], order=1, mode='nearest')


def kante(y, p, n, r=18):
    """Querprofil durch p. Staerkste Flanke nahe der Mitte, Pegel = Extrem je Seite der Flanke im Fenster +-r. Damit bleiben zweistufige,
    weiche Flanken ganz drin; die Fassung mit Gradientenende schnitt sie ab (gemessen an den Kanten aus formblatt_gegenprobe.md 3.1).
    Rueckgabe: Breite 10-90, Versatz der 50-%-Kante gegen p, tief, hoch; None ohne Flanke."""
    t, v = profil(y, p, n, r)
    g = np.gradient(v, t)
    i = int(np.argmax(np.abs(g) * np.exp(-(t / 9.0) ** 2)))
    if i < 1 or i > len(t) - 2 or g[i] == 0:
        return None
    s = 1 if g[i] > 0 else -1
    links, rechts = v[:i + 1], v[i:]
    if s > 0:
        lo, hi = float(links.min()), float(rechts.max())
    else:
        hi, lo = float(links.max()), float(rechts.min())
    d = hi - lo
    if d <= 0:
        return None

    def geh(z, ri):
        k = i
        while 0 < k < len(v) - 1:
            if (v[k] - z) * (v[k + ri] - z) <= 0:
                a = v[k + ri] - v[k]
                return t[k] + (z - v[k]) / (a if a else 1e-9) * (t[k + ri] - t[k])
            k += ri
        return None
    q10, q90 = geh(lo + .1 * d, -s), geh(lo + .9 * d, s)
    if q10 is None or q90 is None:
        return None
    k50 = lo + .5 * d
    q50 = geh(k50, s) if v[i] < k50 else geh(k50, -s)       # s zeigt immer bergauf, gleich ob die Flanke steigt oder faellt
    if q50 is None:
        q50 = t[i]
    return dict(breite=abs(q90 - q10), versatz=float(q50), tief=lo, hoch=hi, hub=d, steigt=s > 0)


def beste_kante(kan, p, n, r=18):
    """Luma zuerst: Farbgrenzen sind im Video nur auf 5-10 px genau, Luma traegt (lesung_zeichnung.json bildtechnik). Hat Luma dort keinen Hub (Scheitel Bild 1:
    nur Farbwechsel, Gegenprobe 2; Bild 2: Rot und Restbild), zaehlt der Kanal mit dem groessten Hub, gewichtet nach Naehe zum Soll. Reiner Hub fing die
    Raumstufe bei Bild 2 (394,187), +13,0 px im Rotkanal gegen 0,0 im Restbild, und mass Bein FR rechts y=324 in Blau 11,3 px breit gegen 4,9 in Luma."""
    y = kante(kan['Y'], p, n, r)
    if y and y['hub'] >= 2 * MIN_HUB and abs(y['versatz']) <= 9:
        return dict(y, kanal='Y')
    beste, wert = None, -1.0
    for nm, b in kan.items():
        e = kante(b, p, n, r)
        if e:
            w = e['hub'] * math.exp(-(e['versatz'] / 9.0) ** 2)
            if w > wert:
                beste, wert = dict(e, kanal=nm), w
    return beste


def grenze_aus(e):
    """Grenze in Pixeln: Wurzel aus (halbe Kante 10-90)^2 + RASTER_PX^2 + (Abstand Soll zu gemessener Kante)^2.
    Innerhalb der halben Kante kann das Video die Lage nicht mehr trennen; der Versatz deckt die Ablesung des Solls."""
    if e is None or e['hub'] < MIN_HUB:
        return OHNE_KANTE_PX, None
    return float(math.sqrt((e['breite'] / 2) ** 2 + RASTER_PX ** 2 + e['versatz'] ** 2)), e


def normale_aus_bild(kan, p, s=3.0):
    """Normale aus dem geglaetteten Bildgradienten. Von Hand gesetzte Normalen lagen am Rundling daneben und taeuschten -6,8 px Versatz vor."""
    y = cv2.GaussianBlur(kan['Y'], (0, 0), s)
    x0, y0 = int(round(p[0])), int(round(p[1]))
    gx = float(y[y0, min(x0 + 1, BREITE - 1)] - y[y0, max(x0 - 1, 0)])
    gy = float(y[min(y0 + 1, HOEHE - 1), x0] - y[max(y0 - 1, 0), x0])
    return (gx, gy) if abs(gx) + abs(gy) > 1e-3 else (1.0, 0.0)


# ═══════════════════════════════════════════════════ Kameras, Hintergrund, Licht
def kamera(n):
    """Buehnen-Kamera aus kamera_bildN.json. B ist fuer Pixelmitte auf ganzer Zahl gemessen (form_kamera), rastern tastet bei i + 0,5."""
    k = FK.lade_kamera(n)
    a = np.array([[1, 0, .5], [0, 1, .5], [0, 0, 1]], float)
    b = np.array([[1, 0, -.5], [0, 1, -.5], [0, 0, 1]], float)
    return FB.Kamera.aus_rid(k['auge'], k['ziel'], h=int(k['H']), abbildung=dict(matrix=(a @ k['B'] @ b).tolist()), name='Bild %d' % n)


def raum_bild2(neu=False):
    """ROOM509.BSS Cut 11, Engine-Dekoder (tests/unit/probe_bg_dump.c), in Video-Bildlage ueber B derselben Kamera wie die Figur.
    Die Ablage aus `ziele` macht `pruefe` unabhaengig vom Build-Baum; neu=True liest wieder von der Quelle."""
    if not neu and os.path.exists(daten('hintergrund_bild2.png')):
        return lade_rgb(daten('hintergrund_bild2.png'))
    pfad = FB.hintergrund_bss('5090', 11)
    roh = lade_rgb(pfad) if pfad else lade_rgb(os.path.join(FORM, 'lesehilfen', 'farbe_port_ROOM50911.png'))
    roh = roh if roh.shape[:2] == (240, 320) else cv2.resize(roh, (320, 240), interpolation=cv2.INTER_NEAREST)
    return cv2.warpPerspective(roh, FK.lade_kamera(2)['B'], (BREITE, HOEHE), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REPLICATE)


def raum_rest2():
    """Video minus gefilterter Raum, Rotkanal: die Beleg-Groesse fuer Verdeckung in Bild 2."""
    fil = FV.video_filter(raum_bild2(), bild=2, entzerren=False, rauschen=False).astype(np.float32)
    return original(2).astype(np.float32)[..., 0] - fil[..., 0], fil


def licht_von_disc(n):
    """ROOM5090 Cut 11 ist von der Disc. Bild 1 hat keinen Schnitt: Cut 2 liegt ihm am naechsten (kamera.md), sein L0 steht ueber der Figur."""
    return FB.Raum('5090').licht(11) if n == 2 else FB.Raum('5080').licht(2)


def licht_skaliert(c, k):
    return dict(c, umgebung=tuple(int(round(a * q)) for a, q in zip(c['umgebung'], k)),
                farbe=[tuple(int(round(a * q)) for a, q in zip(f, k)) for f in c['farbe']])


def licht_fuer(n, art):
    c = licht_von_disc(n)
    if art == 'disc':
        return c, (1.0, 1.0, 1.0)
    with open(daten('licht.json'), encoding='utf-8') as f:
        k = json.load(f)['bild%d' % n]['k']
    return licht_skaliert(c, k), tuple(k)


# ═══════════════════════════════════════════════════ Zielmasken
def band_um(punkte, kan, zu, fest=None):
    """Grauer Saum um eine Kante: je Stelle Scheibe mit Radius aus der gemessenen Kantenbreite, entlang der Linie geglaettet.
    fest(x, y) -> Radius ersetzt die Messung an Stellen, die das Formblatt selbst als unlesbar fuehrt. Rueckgabe: Maske + Messreihe."""
    p = np.asarray(punkte, float)
    if zu:
        p = np.vstack([p, p[:1]])
    d = np.r_[0, np.cumsum(np.linalg.norm(np.diff(p, axis=0), axis=1))]
    s = np.arange(0, d[-1], 4.0)
    xy = np.c_[np.interp(s, d, p[:, 0]), np.interp(s, d, p[:, 1])]
    tg = np.gradient(xy, axis=0)
    r = np.zeros(len(xy))
    for i, q in enumerate(xy):
        e = beste_kante(kan, q, (-tg[i, 1], tg[i, 0]))
        r[i], _ = grenze_aus(None if e is None else dict(e, versatz=0.0))
    r = np.clip(r, 3.0, 13.0)
    if len(r) >= 5:                                         # Median ueber 5 Stellen: Einzelausreisser durch Nachbarkanten
        r = np.array([np.median(r[max(i - 2, 0):i + 3]) for i in range(len(r))])
    if fest:
        for i, q in enumerate(xy):
            v = fest(q[0], q[1])
            if v is not None:
                r[i] = v
    m = np.zeros((HOEHE, BREITE), np.uint8)
    for q, a in zip(xy, r):
        cv2.circle(m, (int(round(q[0])), int(round(q[1]))), int(math.ceil(a)), 1, -1)
    return m.astype(bool), r


def _konturen(m):
    c, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    return [x[:, 0, :] for x in c if len(x) >= 8]


def zone_b1(i):
    """Boden-frei-Zone Bild 1. L2 traegt die Spitze aus A20 ((293,313), Y 77 bei y=312, 145 bei y=316); das Polygon in formblatt.json endet bei y=320,
    dann gaebe es die Kante von Bein NL bei y=318 aus Abschnitt 7 gar nicht."""
    if i == 'L2':
        return [(293, 313), (300, 322), (304, 326), (312, 346), (320, 350), (300, 364), (286, 372), (283, 358), (286, 338), (287, 318)]
    return nicht_figur(i)['polygon']


def einrasten(figur, kan):
    """Umriss auf die gemessene 50-%-Kante ziehen, wo das Bild eine hat. Das Polygon lag bis 5 px neben Kanten, die Abschnitt 7 und die Bildmessung auf 1,5 px
    uebereinstimmend nennen. Nur mit Luma-Hub >= 2 * MIN_HUB, nur innen dunkel, aussen hell, nur +-6 px; dunkel auf dunkel bleibt die Lesung."""
    k, hier = cv2.findContours(figur.astype(np.uint8), cv2.RETR_CCOMP, cv2.CHAIN_APPROX_NONE)
    aus = np.zeros((HOEHE, BREITE), np.uint8)
    wege = []
    for i, c in sorted(enumerate(k), key=lambda t: hier[0][t[0]][3] >= 0):      # aeussere Ringe zuerst, Loecher danach
        q = c[:, 0, :].astype(float)
        if len(q) < 12:
            continue
        q = q[::3]
        tg = np.roll(q, -2, 0) - np.roll(q, 2, 0)
        n = np.c_[tg[:, 1], -tg[:, 0]] / np.maximum(np.linalg.norm(tg, axis=1, keepdims=True), 1e-6)
        # Normale nach aussen drehen: ein Stueck weiter darf keine Figur mehr sein
        pr = q + 3 * n
        drin = figur[np.clip(np.round(pr[:, 1]).astype(int), 0, HOEHE - 1), np.clip(np.round(pr[:, 0]).astype(int), 0, BREITE - 1)]
        if drin.mean() > 0.5:
            n = -n
        d = np.zeros(len(q))
        for j, (a, m) in enumerate(zip(q, n)):
            e = kante(kan['Y'], a, m, 12)
            if e and e['hub'] >= 2 * MIN_HUB and e['steigt'] and abs(e['versatz']) <= 6:
                d[j] = e['versatz']
        d = np.array([np.median(np.take(d, range(j - 2, j + 3), mode='wrap')) for j in range(len(d))])
        wege += list(np.abs(d[d != 0]))
        neu = np.round(q + d[:, None] * n).astype(np.int32)
        cv2.fillPoly(aus, [neu], 0 if hier[0][i][3] >= 0 else 1)
    return aus.astype(bool), wege


def ziel_bild1():
    J = formblatt()
    kan = kanaele(original(1))
    um = list(J['umriss_bild1'])
    um.insert(1, tuple(J['pruefpunkte']['bild1']['punkte']['stange_sichtbar_bis']))   # 1.4: Stange x=280 endet an der Kante bei y=191; das Polygon lief bei y=195
    u = poly_maske(um)
    frei = {i: poly_maske(zone_b1(i)) for i in ('L1', 'L2', 'SCH')}   # W7, Abschnitt 7 Boden frei
    figur, wege = einrasten(u & ~frei['L1'] & ~frei['L2'] & ~frei['SCH'], kan)

    def fest(x, y):
        if 250 <= x <= 300 and y <= 225:                    # Abschnitt 5: dunkel auf dunkel
            return OHNE_KANTE_PX
        return None
    band = np.zeros_like(figur)
    reihen = []
    for c in _konturen(figur):
        b, r = band_um(c[::2], kan, True, fest)
        band |= b
        reihen += list(r)

    offen = np.zeros_like(figur)
    for i in ('B5', 'C5', 'X'):                             # Abschnitt 1.4: offen
        offen |= poly_maske(teil_b1(i)['polygon'])
        offen |= rechteck(*teil_b1(i)['bereich_px'])
    # Sohlen: "Letzte harte Stellen: Bein NL (201,400), Bein NR (312,414). Tiefer ist eine Sohle moeglich, aber nicht gelesen." (1.2)
    offen |= figur & rechteck(0, 401, 289, HOEHE - 1)
    offen |= figur & rechteck(290, 415, BREITE - 1, HOEHE - 1)
    for i in ('K2', 'K3', 'K4'):
        k = next(c for c in J['kontakte'] if c['id'] == i)
        offen |= scheibe(k['bild'][0], k['bild'][1], 16)
    offen |= rechteck(195, 400, 253, 436)                   # 1.2: Streifen y 430-436 reicht bis x=195, Kante endet (201,400)
    c4 = poly_maske(teil_b1('C4')['polygon'])
    offen |= weite(c4, 12) & ~enge(c4, 12) & rechteck(0, 396, BREITE - 1, HOEHE - 1) & ~rechteck(0, 0, 317, HOEHE - 1)  # A5, W7: Aussenkante weich
    offen |= rechteck(0, 436, 253, HOEHE - 1)               # Abschnitt 5: Zone unter dem Logo
    offen |= rechteck(306, 188, 345, 206) & (u | weite(u, 8))   # W16: Oberkante erst ab x=348 sicher
    offen |= rechteck(250, 150, 276, 225) & ~(u & rechteck(0, 225, BREITE, HOEHE))  # 1.4 und 5: Figur davor bei x 250-276 nicht ausgeschlossen
    offen |= rechteck(412, 278, 436, 302)                   # A10: rechte Kante nur y 238-278, darunter Wandsockel ohne Kante

    grau = band | offen
    weiss = figur & ~grau
    z = np.zeros((HOEHE, BREITE), np.uint8)
    z[grau] = 128
    z[weiss] = 255
    return z, figur, dict(saum_px=[float(np.min(reihen)), float(np.median(reihen)), float(np.max(reihen))],
                          eingerastet=[len(wege), float(np.median(wege)), float(np.max(wege))])


def ziel_bild2():
    J = formblatt()
    rest, fil = raum_rest2()
    kan = kanaele(original(2), rest)
    R = teil_b2('R')
    gelesen, belegt = R['kante_gelesen'], R['kante_belegt']
    # Lesekanten sind Untergrenzen: "Silhouette kann weiter aussen liegen" (1.4). Rechts Mindestausdehnung x=413 (W9), dann die Grenze der
    # Verdeckung vor den hellen Zargen (Video gegen Raum, pruefung.md 2.2), links unter x=262 Band und Untermasse (W15, 1.4 U).
    rand = [(413, 225), (413, 262), (384, 266), (382, 284), (366, 298), (345, 300), (338, 318)]
    g = teil_b2('G')['polygon']
    unten = [(326, 319), (314, 334), (298, 346), (286, 352), (240, 372), (232, 347), (222, 292), (224, 272), (225, 258), (252, 258), (262, 250), (262, 232)]
    rumpf = poly_maske(list(gelesen) + list(belegt) + rand + unten)
    v3 = rechteck(340, 312, 364, 345)                       # W10: bis mindestens y=345
    weiss = rumpf | v3 | poly_maske(g) | poly_maske(teil_b2('P')['polygon']) | poly_maske(teil_b2('H')['polygon'])
    figur = weiss.copy()

    sichtbar = cv2.GaussianBlur(fil[..., 0], (0, 0), 2) >= 60   # Raum hell genug, dass Verdeckung sichtbar waere
    offen = weite(~sichtbar & rechteck(165, 115, 354, 300), 4)  # A2, Abschnitt 5: vor schwarzer Tuer keine Silhouette
    offen |= rechteck(190, 200, 262, 300)                   # W8, W15: links oben nicht begrenzt, Anschluss Haken offen
    offen |= rechteck(205, 262, 232, 372)                   # A12: U ist Mindestausdehnung, der Balken endet selbst bei x rund 218
    offen |= rechteck(405, 200, 425, 300)                   # W9: rechter Rand x 413-420
    for x0, y0, x1, y1 in ((160, 325, 228, 388), (415, 225, 462, 300), (415, 310, 505, 400)):
        offen |= rechteck(x0, y0, x1, y1)                   # Feuer: Pixel sind nicht Figur, aber es kann vor ihr stehen (W9, 6B)
    offen |= rechteck(345, 262, 420, 381)                   # 1.4 Unterkante rechts: weicher Uebergang, Lage offen
    offen |= rechteck(205, 340, 345, 394)                   # F, Band, Bereich unter G: offen (1.4, H)
    offen |= poly_maske(teil_b2('F')['polygon'])
    e = teil_b2('Y')['ellipse']
    y = np.zeros((HOEHE, BREITE), np.uint8)
    cv2.ellipse(y, tuple(e['mitte']), tuple(e['halbachsen']), 0, 0, 360, 1, -1)
    offen |= weite(y.astype(bool), 6)                       # W13: deckend sicher, Figur offen
    offen |= rechteck(0, 436, 253, HOEHE - 1)               # Abschnitt 5: Zone unter dem Logo
    loch = rechteck(300, 300, 345, 320)                     # Abschnitt 5: Figur dort nicht pruefbar
    offen |= loch | rechteck(325, 320, 345, 345)            # rechts neben G: nichts belegt, V3 beginnt erst bei x=340

    band, r = band_um(belegt, kan, False)
    grau = (offen | band) & ~(enge(weiss, 3) & ~loch & ~band)
    weiss_kern = enge(weiss, 3) & ~loch & ~band
    z = np.zeros((HOEHE, BREITE), np.uint8)
    z[grau] = 128
    z[weiss_kern] = 255
    z[(z == 0) & weiss] = 128                               # Rand der Sollflaeche ist nie sicher nicht Figur
    for x0, y0, x1, y1 in J['pruefpunkte']['bild2']['boden_frei']:  # Abschnitt 7: Boden frei
        z[y0:y1 + 1, x0:x1 + 1] = 0
    z[395:476, 278:451] = 0                                 # A13: Boden, vom Raum erklaert
    return z, figur, dict(saum_px=[float(r.min()), float(np.median(r)), float(r.max())])


def hintergrund_bild1(ziel):
    """Spielbild-Schaetzung aus dem Video, Figur grob ausgemalt, damit der Filter dasselbe Streulicht sieht. Aussen bleibt der Raum, wie
    video_umkehr ihn liefert (Hin und zurueck MAE 6,6, farbe.md); unter der Figur steht eine weiche Fortsetzung der Umgebung."""
    e = np.clip(FV.video_umkehr(original(1), 1) + .5, 0, 255).astype(np.uint8)
    logo = rechteck(0, 436, 253, HOEHE - 1)
    fig = (ziel > 0) & ~logo
    m = weite(fig, 7)
    klein = cv2.resize(e, (320, 240), interpolation=cv2.INTER_AREA)
    mk = cv2.resize(m.astype(np.uint8), (320, 240), interpolation=cv2.INTER_NEAREST)
    fuell = cv2.resize(cv2.inpaint(klein, mk, 7, cv2.INPAINT_TELEA), (BREITE, HOEHE), interpolation=cv2.INTER_LINEAR)
    fuell = cv2.GaussianBlur(fuell, (0, 0), 3)
    a = cv2.GaussianBlur(m.astype(np.float32), (0, 0), 2)[..., None]
    return np.clip(e * (1 - a) + fuell * a + .5, 0, 255).astype(np.uint8)


def ziel_uebersicht(n, z, name='ziel_bild%d_auf_original.png'):
    """Beleg zum Ansehen: Ziel auf dem Original, 2-fach. Gruen = weiss, blau getoent = grau, ungetoent = schwarz."""
    s = 2
    o = cv2.resize(np.power(original(n) / 255.0, 0.7) * 255, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
    g = cv2.resize((z == 128).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST).astype(bool)
    o[g] = o[g] * 0.62 + np.array([30, 80, 170]) * 0.38
    o = np.clip(o, 0, 255).astype(np.uint8)
    kontur_auf(o, z == 255, (70, 240, 90), s)
    kontur_auf(o, z > 0, (90, 200, 255), s)
    for x in range(0, BREITE, 40):
        cv2.putText(o, str(x), (x * s + 2, 11), cv2.FONT_HERSHEY_SIMPLEX, .36, (255, 255, 0), 1, cv2.LINE_AA)
    for y in range(40, HOEHE, 40):
        cv2.putText(o, str(y), (2, y * s + 4), cv2.FONT_HERSHEY_SIMPLEX, .36, (255, 255, 0), 1, cv2.LINE_AA)
    cv2.imwrite(daten(name % n), o[:, :, ::-1])


def befehl_ziele(_a=None):
    os.makedirs(DATEN, exist_ok=True)
    z1, f1, s1 = ziel_bild1()
    z2, f2, s2 = ziel_bild2()
    cv2.imwrite(os.path.join(FORM, 'ziel_bild1.png'), z1)
    cv2.imwrite(os.path.join(FORM, 'ziel_bild2.png'), z2)
    ziel_uebersicht(1, z1)
    ziel_uebersicht(2, z2)
    cv2.imwrite(daten('figur_bild1.png'), f1.astype(np.uint8) * 255)
    cv2.imwrite(daten('figur_bild2.png'), f2.astype(np.uint8) * 255)
    for n, z, s in ((1, z1, s1), (2, z2, s2)):
        print('ziel_bild%d.png: weiss %d, grau %d, schwarz %d Pixel; Saum min/Median/max %.1f / %.1f / %.1f px'
              % (n, (z == 255).sum(), (z == 128).sum(), (z == 0).sum(), *s['saum_px']))
        if 'eingerastet' in s:
            print('  Umriss eingerastet: %d Stellen, Weg Median %.1f, groesster %.1f px' % tuple(s['eingerastet']))
    h1 = hintergrund_bild1(z1)
    h2 = raum_bild2(neu=True)
    cv2.imwrite(daten('hintergrund_bild1.png'), h1[:, :, ::-1])
    cv2.imwrite(daten('hintergrund_bild2.png'), h2[:, :, ::-1])
    g = grenzen_rechnen()
    with open(daten('grenzen.json'), 'w', encoding='utf-8') as f:
        json.dump(g, f, ensure_ascii=False, indent=1)
    print('grenzen.json: %d Punkte, Grenze %.1f..%.1f px' % (len(g), min(x['grenze'] for x in g.values()), max(x['grenze'] for x in g.values())))
    k = licht_eichen()
    with open(daten('licht.json'), 'w', encoding='utf-8') as f:
        json.dump(k, f, ensure_ascii=False, indent=1)
    print('licht.json: Bild 1 k %s, Bild 2 k %s' % (k['bild1']['k'], k['bild2']['k']))


# ═══════════════════════════════════════════════════ Pruefpunkte Abschnitt 7
def punkte_abschnitt7():
    """Alle Punkte aus formblatt.json pruefpunkte, als Liste. art: spalte_oben/unten (Kante in Spalte x, Soll y), zeile_links/rechts (Kante in
    Zeile y, Soll x), kontur (kuerzester Abstand zum Umriss), mat_* wie spalte_*, aber am Material statt am Umriss."""
    J = formblatt()['pruefpunkte']
    a = J['bild1']['punkte']
    P = []

    def neu(i, bild, art, text, x, y, zaehlt=True, sicherheit='sicher'):
        P.append(dict(id=i, bild=bild, art=art, text=text, x=x, y=y, zaehlt=zaehlt, sicherheit=sicherheit))
    neu('b1.sohle_fr', 1, 'spalte_unten', 'Unterkante right_arm', a['sohle_fr'][0], a['sohle_fr'][1])
    neu('b1.kappe_spitze', 1, 'mat_spalte_oben', 'Z5 Spitze', *a['kappe_spitze'])
    neu('b1.kappe_basis', 1, 'mat_spalte_unten', 'Z5 Basis', *a['kappe_basis'])
    neu('b1.schulterkuppe', 1, 'kontur', 'Oberkante right_arm', *a['schulterkuppe'], sicherheit='wahrscheinlich')
    for x in (300, 305, 311, 316, 322):
        neu('b1.scheitel_x%d' % x, 1, 'spalte_oben', 'Oberkante tail', x, a['scheitel_y_bei_x_300_322'])
    for i, p in enumerate(a['flanke_rechts']):
        neu('b1.flanke_%d' % (i + 1), 1, 'kontur', 'Kante part_of_Body', p[0], p[1])
    for y in (224, 233, 242):
        neu('b1.rand_links_y%d' % y, 1, 'zeile_links', 'Kante links tail', a['rand_links_x_bei_y_224_242'], y)
    neu('b1.rand_rechts', 1, 'zeile_rechts', 'Kante rechts neben right_arm', a['rand_rechts'][0], a['rand_rechts'][1], sicherheit='wahrscheinlich')
    for nm, t, y, sl, sr in (('fr', 'right_arm', 324, *a['bein_fr_x_bei_y_324']), ('fr', 'right_arm', 344, *a['bein_fr_x_bei_y_344']),
                             ('nr', 'right_leg', 322, *a['bein_nr_x_bei_y_322']), ('nr', 'right_leg', 346, *a['bein_nr_x_bei_y_346'])):
        neu('b1.%s_y%d_links' % (nm, y), 1, 'zeile_links', t + ' Kante links', sl, y)
        neu('b1.%s_y%d_rechts' % (nm, y), 1, 'zeile_rechts', t + ' Kante rechts', sr, y)
    sl, sr = a['bein_nl_x_bei_y_318_342']
    for y in (318, 330, 342):
        neu('b1.nl_y%d_links' % y, 1, 'zeile_links', 'left_leg Kante links', sl, y)
        neu('b1.nl_y%d_rechts' % y, 1, 'zeile_rechts', 'tail Kante rechts', sr, y)
    for i, p in enumerate(a['rundling_linke_kante']):
        neu('b1.rundling_%d' % (i + 1), 1, 'kontur', 'Rundling linke Kante', p[0], p[1], sicherheit='wahrscheinlich')
    neu('b1.stange', 1, 'spalte_oben', 'tail Oberkante vor der Stange', *a['stange_sichtbar_bis'])
    R = teil_b2('R')
    for i, p in enumerate(R['kante_belegt']):
        neu('b2.kante_%d' % (i + 1), 2, 'kontur', 'Kante face_and_upper_body', p[0], p[1], sicherheit='sicher' if p[0] <= 378 else 'wahrscheinlich')
    for i, p in enumerate(R['kante_gelesen']):
        neu('b2.gelesen_%d' % (i + 1), 2, 'kontur', 'Kante gelesen, nur Anhalt', p[0], p[1], zaehlt=False, sicherheit='wahrscheinlich')
    for p in P:
        if p['id'] in STILLGELEGT:
            p['zaehlt'], p['grund'] = False, STILLGELEGT[p['id']]
        elif p['id'].startswith('b2.gelesen'):
            p['grund'] = STILL_GELESEN
    return P


def grenzen_rechnen():
    rest, _ = raum_rest2()
    kan = {1: kanaele(original(1)), 2: kanaele(original(2), rest)}
    aus = {}
    for p in punkte_abschnitt7():
        k = kan[p['bild']]
        q = (p['x'], p['y'])
        if p['art'].endswith('spalte_oben') or p['art'].endswith('spalte_unten'):
            n = (0.0, 1.0)
        elif p['art'].startswith('zeile'):
            n = (1.0, 0.0)
        else:
            n = normale_aus_bild(k, q)
        e = beste_kante(k, q, n)
        g, e2 = grenze_aus(e)
        aus[p['id']] = dict(grenze=round(g, 1), breite=None if e2 is None else round(e2['breite'], 1), versatz=None if e2 is None else round(e2['versatz'], 1),
                            kanal=None if e2 is None else e2['kanal'], hub=None if e2 is None else round(e2['hub'], 0), normale=[round(float(x), 3) for x in n],
                            ohne_kante=e2 is None)
    return aus


# ═══════════════════════════════════════════════════ Messung am Render
def geschlossen(m):
    """Stichloecher zwischen Dreiecken sind keine Umrisskante."""
    return cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8)).astype(bool)


def _spalte(m, x, soll, oben):
    s = m[:, int(x)].astype(np.int8)
    d = np.diff(s)
    k = (np.nonzero(d == 1)[0] + 0.5) if oben else (np.nonzero(d == -1)[0] + 0.5)
    return _naechste(k, soll)


def _zeile(m, y, soll, links):
    s = m[int(y), :].astype(np.int8)
    d = np.diff(s)
    k = (np.nonzero(d == 1)[0] + 0.5) if links else (np.nonzero(d == -1)[0] + 0.5)
    return _naechste(k, soll)


def _naechste(k, soll):
    if not len(k):
        return None
    i = int(np.argmin(np.abs(k - soll)))
    return float(k[i]) if abs(k[i] - soll) <= FIGUR_SUCHE_PX else None


def konturabstand(m, x, y):
    """Vorzeichen: + Soll liegt ausserhalb des Renders (Render zu klein), - innerhalb (Render zu gross)."""
    if not m.any():
        return None
    ix, iy = int(round(x)), int(round(y))
    if m[iy, ix]:
        return -float(cv2.distanceTransform(m.astype(np.uint8), cv2.DIST_L2, 5)[iy, ix]) + 0.5
    return float(cv2.distanceTransform((~m).astype(np.uint8), cv2.DIST_L2, 5)[iy, ix]) - 0.5


def punkt_messen(p, g, m, band):
    a = p['art']
    if a == 'kontur':
        d = konturabstand(m, p['x'], p['y'])
        ist = None
    elif a in ('spalte_oben', 'mat_spalte_oben', 'spalte_unten', 'mat_spalte_unten'):
        q = band if a.startswith('mat') else m
        ist = _spalte(q, p['x'], p['y'], a.endswith('oben'))
        d = None if ist is None else ist - p['y']
    else:
        ist = _zeile(m, p['y'], p['x'], a.endswith('links'))
        d = None if ist is None else ist - p['x']
    ok = d is not None and abs(d) <= g['grenze']
    return dict(id=p['id'], text=p['text'], soll=[p['x'], p['y']], art=a, ist=ist, abstand=None if d is None else round(d, 1),
                grenze=g['grenze'], ok=bool(ok), zaehlt=p['zaehlt'], sicherheit=p['sicherheit'], ohne_kante=g['ohne_kante'], grund=p.get('grund'))


def ueberdeckung(m, z):
    w, s, gr = z == 255, z == 0, z == 128
    ri = m & ~gr
    vereinigt = int((ri | w).sum())
    t = int((m & w).sum())
    return dict(iou=round(t / vereinigt, 4) if vereinigt else 0.0, weiss_gedeckt=round(t / int(w.sum()), 4), weiss=int(w.sum()),
                schwarz_getroffen=int((m & s).sum()), render_pixel=int(m.sum()), render_auf_grau=int((m & gr).sum()))


def flecken(m, n=3):
    """Groesste zusammenhaengende Flaechen: (Pixel, x0, y0, x1, y1). Sagt dem Modellbau, WO es fehlt."""
    k, lab, st, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    z = sorted(((int(st[i, 4]), int(st[i, 0]), int(st[i, 1]), int(st[i, 0] + st[i, 2] - 1), int(st[i, 1] + st[i, 3] - 1)) for i in range(1, k)), reverse=True)
    return [list(x) for x in z[:n]]


def zone_frei(m, zone, z, i, text, sicherheit):
    """Pruefflaeche = Zone minus Grau: der Saum um die Kante ist schon die Grenze aus der Unschaerfe."""
    kern = zone & (z == 0)
    if not kern.any():
        kern = enge(zone, 3)
    tref = m & kern
    tief = 0.0
    if tref.any():
        tief = float(cv2.distanceTransform(kern.astype(np.uint8), cv2.DIST_L2, 5)[tref].max())
    return dict(id=i, text=text, flaeche=int(kern.sum()), treffer=int(tref.sum()), eindringen_px=round(tief, 1), ok=not tref.any(), zaehlt=True, sicherheit=sicherheit)


def linie_gedeckt(m, a, b, i, text, kan, zaehlt=True, sicherheit='sicher'):
    """Gedeckt = Render liegt auf der Linie. Die Enden zaehlen erst innerhalb der Grenze aus ihrer Kante; das Video kann das Ende nicht genauer zeigen."""
    a, b = np.array(a, float), np.array(b, float)
    n = int(np.linalg.norm(b - a)) + 1
    r = (b - a) / max(np.linalg.norm(b - a), 1)
    ge = [grenze_aus(beste_kante(kan, p, r))[0] for p in (a, b)]
    ge = [min(x, 8.0) for x in ge]
    tt = np.linspace(0, 1, n)
    pp = a + tt[:, None] * (b - a)
    x, y = np.round(pp[:, 0]).astype(int), np.round(pp[:, 1]).astype(int)
    drauf = m[y, x]
    lg = n
    innen = (tt * lg >= ge[0]) & ((1 - tt) * lg >= ge[1])
    luecke = best = 0
    for v in drauf[innen]:
        luecke = 0 if v else luecke + 1
        best = max(best, luecke)
    return dict(id=i, text=text, a=a.tolist(), b=b.tolist(), anteil=round(float(drauf.mean()), 3), anteil_innen=round(float(drauf[innen].mean()) if innen.any() else 0.0, 3),
                luecke_px=int(best), grenze_enden=[round(x, 1) for x in ge], ok=bool(innen.any() and drauf[innen].all()), zaehlt=zaehlt, sicherheit=sicherheit)


def boden_und_verdeckung(n, m, z, kan):
    J = formblatt()
    aus = []
    if n == 1:
        for i, t in (('L1', 'Boden frei L1'), ('L2', 'Boden frei L2'), ('SCH', 'Boden frei (345-378,362-402)')):
            aus.append(zone_frei(m, poly_maske(zone_b1(i)), z, i, t, nicht_figur(i)['sicherheit']))
        return aus
    (x0, y0, x1, y1), (a0, b0, a1, b1) = J['pruefpunkte']['bild2']['boden_frei']
    aus.append(zone_frei(m, rechteck(x0, y0, x1, y1), z, 'BO', 'Boden frei (345-405,382-396)', 'sicher'))
    aus.append(zone_frei(m, rechteck(a0, b0, a1, b1), z, 'FUGE', 'Fuge x>=278, y 402-416', 'sicher'))
    for v in J['verdeckungen_bild2']:
        if v['id'] == 'BO':
            continue
        if v['id'] == 'V6':                                 # Kein Deckungsstreifen: die Figurkante liegt in der Strecke (R 194 bei y=204, 117 bei y=208), also Kantenpunkt
            g = _json(daten('grenzen.json'))['b2.kante_7']['grenze']
            ist = _spalte(m, 411, 206, True)
            aus.append(dict(id='V6', text=v['aussage'], abstand=None if ist is None else round(ist - 206, 1), grenze=g, ok=ist is not None and abs(ist - 206) <= g,
                            zaehlt=True, sicherheit=v['sicherheit']))
            continue
        aus.append(linie_gedeckt(m, v['stelle'][0], v['stelle'][1], v['id'], v['aussage'], kan, sicherheit=v['sicherheit']))   # V5: Flaeche Y ist die Hand, W13 ist damit entschieden
    h = enge(poly_maske(teil_b2('H')['polygon']), 2)        # Abschnitt 7: Haken, gefuellte Flaeche
    d = float((m & h).sum()) / max(int(h.sum()), 1)
    aus.append(dict(id='HAKEN', text='Haken gefuellt (205-245,215-262)', flaeche=int(h.sum()), anteil=round(d, 3), ok=d >= ZEICHNUNG_MIN_ANTEIL, zaehlt=False, sicherheit='wahrscheinlich',
                    grund=STILLGELEGT['HAKEN']))
    rr = teil_b2('rand_rechts')
    tief = []
    for y in range(rr['y'][0], rr['y'][1] + 1, 6):
        q = np.nonzero(m[y])[0]
        tief.append(int(q.max()) if len(q) else -1)
    aus.append(dict(id='RAND413', text='Rand rechts mindestens x=413 bei y 225-262', kleinstes_x=min(tief), ok=min(tief) >= rr['mindestens_x'] - 3, zaehlt=True, sicherheit='wahrscheinlich'))
    return aus


# ═══════════════════════════════════════════════════ Zeichnung
def _pz(ids):
    M = merkmale()
    return [M[i]['polygon'] for i in ids]


def stellen():
    """Sollflaechen: gemessene Polygone aus lesung_zeichnung.json und lesung_gesamtbau.json, Zuordnung und Material aus FORMBLATT 2.2.
    zaehlt=False: das Formblatt fuehrt die Stelle als offen."""
    b = lambda n, nm: [bauteil(n, nm)['polygon']]     # noqa: E731
    return [dict(id='Z1', bild=1, polygone=_pz(['A01']), material='rosa_stelle', sicherheit='wahrscheinlich'),
            dict(id='Z2', bild=1, polygone=_pz(['A02']), material='helle_baender', sicherheit='wahrscheinlich'),
            dict(id='Z3', bild=1, polygone=_pz(['A03']), material='weisskerne', sicherheit='Form wahrscheinlich, Farbe offen'),
            dict(id='Z4', bild=1, polygone=_pz(['A04']), material='helle_baender', sicherheit='wahrscheinlich'),
            dict(id='Z5', bild=1, polygone=b(1, 'dreieck_gelenk_A'), material='helle_baender', sicherheit='sicher als Stelle'),
            dict(id='Z6a', bild=1, polygone=_pz(['A14']), material='helle_baender', sicherheit='wahrscheinlich'),
            dict(id='Z6b', bild=1, polygone=_pz(['A15']), material='helle_baender', sicherheit='offen', zaehlt=False),
            dict(id='Z6c', bild=1, polygone=_pz(['A16']), material='helle_baender', sicherheit='offen', zaehlt=False),
            dict(id='Z7', bild=1, polygone=_pz(['A20', 'A22', 'A23']), material='feine_linien', sicherheit='wahrscheinlich'),
            dict(id='Z8', bild=1, polygone=_pz(['A09', 'A08', 'A07']), material='feine_linien', sicherheit='Lage wahrscheinlich, Bedeutung offen'),
            dict(id='Z9', bild=1, polygone=b(1, 'schraegstrich_beule'), material='z9', sicherheit='wahrscheinlich'),
            dict(id='Q1', bild=2, polygone=_pz(['B01', 'B02']), material='feine_linien', sicherheit='wahrscheinlich'),
            dict(id='Q2', bild=2, polygone=b(2, 'scheitel_sichel'), material='rotbraun_mittelton', sicherheit='wahrscheinlich'),
            dict(id='Q3', bild=2, polygone=_pz(['B06']), material='feine_linien', sicherheit='wahrscheinlich'),
            dict(id='Q4', bild=2, polygone=_pz(['B07']), material='helle_baender', sicherheit='wahrscheinlich'),
            dict(id='Q5', bild=2, polygone=b(2, 'innenlinie_rechts_unten'), material='feine_linien', sicherheit='wahrscheinlich'),
            dict(id='G', bild=2, polygone=[teil_b2('G')['polygon']], material='untere_streifen', sicherheit='sicher als Stelle'),
            dict(id='P', bild=2, polygone=[teil_b2('P')['polygon']], material='weisskerne', sicherheit='wahrscheinlich')]


def material_maske(alb, m, name):
    """Render-Albedo (Textur ohne Licht) gegen das Material. Spanne aus der Palette, 8 Stufen Rand fuer die 5-Bit-CLUT."""
    M = formblatt()['zeichnung']['materialien']
    a = alb.astype(np.int32)
    Y = alb.astype(np.float32) @ YK
    if name == 'weisskerne':                                # Formblatt 2.1: nur "mindestens so hell wie Band"
        t = float(np.array(M['helle_baender']['oberflaeche_rgb'], np.float32) @ YK)
        return m & (Y >= t)
    if name == 'untere_streifen':                           # Obergrenze, echte Farbe dunkler: heller als Rotbraun, nicht ueber die Grenze
        t = float(np.array(M['rotbraun_mittelton']['oberflaeche_von'], np.float32) @ YK)
        return m & (Y >= t) & (a <= np.array(M['untere_streifen']['oberflaeche_bis']) + 8).all(-1)
    if name == 'z9':                                        # Formblatt 2.2: Textur rund (85,80,60), kein Fehlerband angegeben
        return m & (np.abs(a - np.array([85, 80, 60])) <= 30).all(-1)
    return m & (a >= np.array(M[name]['oberflaeche_von']) - 8).all(-1) & (a <= np.array(M[name]['oberflaeche_bis']) + 8).all(-1)


def zeichnung_messen(n, alb, m, gef, orig):
    aus = []
    for s in stellen():
        if s['bild'] != n:
            continue
        p = np.zeros((HOEHE, BREITE), bool)
        for q in s['polygone']:
            p |= poly_maske(q)
        fl = int(p.sum())
        mm = material_maske(alb, m, s['material'])
        anteil = float((weite(mm, ZEICHNUNG_SPIEL_PX) & p).sum()) / max(fl, 1)
        fig = float((m & p).sum()) / max(fl, 1)
        fenster = weite(p, ZEICHNUNG_SPIEL_PX + 3)
        groesse = float((mm & fenster).sum()) / max(fl, 1)
        ys, xs = np.nonzero(p)
        sm = [float(xs.mean()), float(ys.mean())]
        lage = None
        if (mm & weite(p, 14)).any():
            ly, lx = np.nonzero(mm & weite(p, 14))
            lage = round(float(math.hypot(lx.mean() - sm[0], ly.mean() - sm[1])), 1)
        aus.append(dict(id=s['id'], material=s['material'], sicherheit=s['sicherheit'], flaeche=fl, anteil_material=round(anteil, 3), anteil_figur=round(fig, 3),
                        groesse=round(groesse, 2), lage_px=lage, sollmitte=[round(x) for x in sm],
                        y_video=round(float((orig.astype(np.float32) @ YK)[p].mean()), 0), y_render=round(float((gef.astype(np.float32) @ YK)[p].mean()), 0),
                        ok=anteil >= ZEICHNUNG_MIN_ANTEIL, zaehlt=s.get('zaehlt', True)))
    return aus


def gegenproben(n, alb, m):
    """Formblatt 2.3 als Verbote: dort darf keine helle Zeichnung sitzen."""
    t = float(np.array(formblatt()['zeichnung']['materialien']['rotbraun_mittelton']['oberflaeche_von'], np.float32) @ YK)
    Y = alb.astype(np.float32) @ YK
    aus = []
    for i, nb, zone, text in (('N1', 2, rechteck(290, 225, 340, 280), 'Rumpfinneres ohne Zeichnung (290-340,225-280)'),
                              ('N2', 1, rechteck(384, 302, 411, 354), 'right_arm unter Z5 ohne Band (384-411,302-354)')):
        if n != nb:
            continue
        hell = m & zone & (Y >= t)
        q = float(hell.sum()) / max(int((m & zone).sum()), 1)
        aus.append(dict(id=i, text=text, anteil_hell=round(q, 3), ok=q <= 0.05, zaehlt=True, sicherheit='sicher'))
    return aus


# ═══════════════════════════════════════════════════ Render
class Kandidat:
    def __init__(self, pfad):
        with open(pfad, encoding='utf-8') as f:
            self.j = json.load(f)
        o = os.path.dirname(os.path.abspath(pfad))
        self.name = self.j.get('name', os.path.splitext(os.path.basename(pfad))[0])
        self.emd = open(os.path.join(o, self.j['emd']), 'rb').read()
        self.tim = open(os.path.join(o, self.j['tim']), 'rb').read()
        self.mo = FE.modell(self.emd, self.tim)
        self.tex = FB.Textur(self.tim)
        self.licht = self.j.get('licht', 'mess')

    def stand(self, n):
        s = self.j['bild%d' % n]
        return tuple(int(v) for v in s['lage']), int(s['gier']), int(s.get('clip', n - 1)), int(s.get('bild', 0))

    def kf(self, clip, bild):
        if not 0 <= clip < len(self.mo.clips):
            clip = 0
        f0, k = self.mo.clips[clip]
        return self.mo.frames[f0 + min(bild, k - 1)] & 0xFFF

    def rendern(self, n):
        lage, gier, clip, bild = self.stand(n)
        hg = lade_rgb(daten('hintergrund_bild%d.png' % n))
        kam = kamera(n)
        kf = self.kf(clip, bild)
        licht, k = licht_fuer(n, self.licht)
        sauber = hg.copy()
        roh = FB.rastern(FB.dreiecke(self.mo, kf, lage, gier, kam, licht), self.tex, sauber, kam.abbildung)
        alb = np.zeros((HOEHE, BREITE, 3), np.uint8)
        FB.rastern(FB.dreiecke(self.mo, kf, lage, gier, kam, None), self.tex, alb, kam.abbildung)
        return sauber, alb, geschlossen(roh), k


# ═══════════════════════════════════════════════════ Rundumblatt
# Name = sichtbare Seite. Figur blickt nach +X, Y unten: ihr rechts ist -Z (Weltregel rechts = (fz, 0, -fx), form_buehne V). Mit Gier "Ende F vorn" ist -Z die Seite R.
ANSICHTEN = (('vorn, Seite +X', 90, 0), ('hinten, Seite -X', -90, 0), ('links, Seite +Z', 180, 0), ('rechts, Seite -Z', 0, 0),
             ('oben, Seite -Y', 0, 89.9), ('unten, Seite +Y', 0, -89.9), ('3/4 vorn rechts', 45, 22), ('3/4 hinten links', -135, 22))


def rundum_dreiecke(mo, kf, az, el, gross):
    """Freie Kamera um die Figur, sonst wie form_buehne.dreiecke: gleiche Flaechenwahl, gleiche Rueckseiten, Gouraud je Ecke. Licht aus Blickrichtung
    oben links, nur fuer dieses Blatt, damit die Form lesbar wird; Mischung wie re15_light: Textur mal (Umgebung + Farbe * max(0, N*L))."""
    R, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])     # Blickrichtung
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    pts = []
    tl = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = R[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ W.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
            pts.append(v)
            tl.append((v, nn, faces, uvs, viereck))
    if not pts:
        return []
    alle = np.vstack(pts)
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max()) or 1.0
    s = gross * 0.46 / rad
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))              # Richtung ZUM Licht, im Blickraum oben links vorn
    aus = []
    for v, nn, faces, uvs, viereck in tl:
        q = (v - mitte) @ A.T
        sx = gross / 2 + q[:, 0] * s
        sy = gross / 2 + q[:, 1] * s
        hell = np.clip(0.46 + 0.86 * np.maximum(nn @ L, 0), 0, 1) * 255.0
        for g, uv in zip(faces, uvs):
            ie, inn = g[1::2], g[0::2]
            p = [(float(sx[i]), float(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if viereck else (m0 <= 0):
                continue
            po = (uv[5] & 0xF) * 128
            zeile = (uv[2] >> 6) & 0x1FF
            c = [np.full(3, hell[i]) for i in inn]
            if viereck:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                    aus.append((float(q[ie[x], 2] + q[ie[y], 2] + q[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), zeile, (c[x], c[y], c[zz])))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((float(q[ie[0], 2] + q[ie[1], 2] + q[ie[2], 2]) / 3, tuple(p), tuple(tx), zeile, tuple(c)))
    return aus


def pose_punkte(mo, kf):
    """Alle Ecken im Aktorraum, Gleitkomma, ohne Gier und Lage."""
    R, T = mo.pose(kf)
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        for verts in (m.tv, m.qv):
            if verts:
                aus.append(np.array([x[:3] for x in verts], float) @ (R[b].astype(float).T / 4096.0) + T[b].astype(float))
    return np.vstack(aus) if aus else np.zeros((0, 3))


def spiel_reihe(k, n, gross):
    """Spielperspektive: Kamera von Bild n als Spielfenster ohne Video-Abbildung, Pose und Licht von Bild n, Figur viermal um je 90 Grad gedreht. Hoehe, Neigung
    und Abstand bleiben die der Kamera, die Drehachse steht senkrecht durch die Mitte der Figur. Erste Tafel = Sicht aus Bild n. Massstab gleich in der Reihe."""
    lage, gier, clip, bild = k.stand(n)
    f = FK.lade_kamera(n)
    kam = FB.Kamera.aus_rid(f['auge'], f['ziel'], h=int(f['H']), name='Spiel Bild %d' % n)
    kf = k.kf(clip, bild)
    licht = licht_fuer(n, k.licht)[0]
    p = pose_punkte(k.mo, kf)
    if not len(p):
        return []
    mx, mz = (p[:, 0].min() + p[:, 0].max()) / 2, (p[:, 2].min() + p[:, 2].max()) / 2

    def welt(g):
        s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
        return c * mx + s * mz, -s * mx + c * mz
    wx, wz = welt(gier)
    tafeln = []
    for i in range(4):
        g = (gier + i * 1024) & 0xFFF
        qx, qz = welt(g)
        lg = (int(round(lage[0] + wx - qx)), lage[1], int(round(lage[2] + wz - qz)))
        tafeln.append((i, FB.dreiecke(k.mo, kf, lg, g, kam, licht)))
    tafeln = [(i, tr, np.array([q for t in tr for q in t[1]], float)) for i, tr in tafeln if tr]
    if not tafeln:
        return []
    spanne = max(float((q.max(0) - q.min(0)).max()) for _i, _t, q in tafeln) or 1.0
    s = gross * 0.9 / spanne
    aus = []
    for i, tr, q in tafeln:
        mitte = (q.min(0) + q.max(0)) / 2
        a = np.full((gross, gross, 3), 92, np.uint8)
        m = FB.rastern(tr, k.tex, a, dict(ax=s, bx=gross / 2 - mitte[0] * s, ay=s, by=gross / 2 - mitte[1] * s))
        aus.append(('Spiel Bild %d: Augenhoehe %d, Gier %+d Grad' % (n, -f['auge'][1], i * 90), a, m))
    return aus


def rundum(k, ziel, ziel_hell, clip, bild):
    gross = 480
    kf = k.kf(clip, bild)
    reihe, hell = [], []

    def dazu(t, a, m):
        reihe.append((t, a))
        h = a.copy()
        h[m] = np.clip(np.power(a[m] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
        hell.append((t, h))
    for t, az, el in ANSICHTEN:
        a = np.full((gross, gross, 3), 92, np.uint8)
        dazu(t, a, FB.rastern(rundum_dreiecke(k.mo, kf, az, el, gross), k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0)))
    for n in (1, 2):
        for t, a, m in spiel_reihe(k, n, gross):
            dazu(t, a, m)
    kopf = '%s  Clip %d Bild %d  Vorderseite +X, Y nach unten; Spielart: Textur, Gouraud, Licht nur fuers Blatt; untere Reihen: Spielperspektive, Pose und Licht je Bild' % (k.name, clip, bild)
    FB.montage(reihe, 4, ziel, kopf)
    FB.montage(hell, 4, ziel_hell, kopf + '; Figur aufgehellt, Gamma 0,5')


# ═══════════════════════════════════════════════════ Blaetter
def schrift(g, fett=False):
    return EA.font(g, fett)


def tafel_rahmen(tafeln, spalten, ziel, kopf):
    h, w = tafeln[0][1].shape[:2]
    pad, zeile, oben = 12, 26, 34
    zl = (len(tafeln) + spalten - 1) // spalten
    b = Image.new('RGB', (pad + spalten * (w + pad), oben + zl * (h + zeile + pad)), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), kopf, font=schrift(16), fill=(170, 173, 180))
    for i, (t, a) in enumerate(tafeln):
        x, y = pad + (i % spalten) * (w + pad), oben + (i // spalten) * (h + zeile + pad)
        b.paste(Image.fromarray(a), (x, y))
        d.text((x, y + h + 3), t, font=schrift(15, True), fill=(214, 138, 74))
    b.save(ziel)


def kontur_auf(a, m, f, s=1):
    q = cv2.resize(m.astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST).astype(bool)
    k = q & ~enge(q, 1)
    a[k] = f


def ueberlagerung(orig, m, z, punkte, s=1, nummern=False):
    o = cv2.resize(orig, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST).copy()
    kontur_auf(o, z == 255, (70, 230, 90), s)
    kontur_auf(o, z > 0, (70, 200, 255), s)
    kontur_auf(o, m, (255, 245, 0), s)
    for i, p in enumerate(punkte):
        x, y = int((p['soll'][0] + .5) * s), int((p['soll'][1] + .5) * s)
        f = (60, 255, 90) if p['ok'] else ((255, 170, 60) if not p['zaehlt'] else (255, 50, 50))
        cv2.drawMarker(o, (x, y), f, cv2.MARKER_CROSS, 5 + 3 * s, 1)
        if nummern:
            cv2.putText(o, str(i + 1), (x + 4, y - 3), cv2.FONT_HERSHEY_SIMPLEX, 0.36, f, 1, cv2.LINE_AA)
    return o


def zahlentafel(zeilen, w=640, h=480):
    b = Image.new('RGB', (w, h), (26, 27, 31))
    d = ImageDraw.Draw(b)
    y = 8
    for z in zeilen:
        t, f = (z if isinstance(z, tuple) else (z, (214, 216, 222)))
        d.text((10, y), t, font=schrift(14), fill=f)
        y += 18
        if y > h - 18:
            break
    return np.array(b)


def differenz(gef, orig):
    return np.clip(128 + (gef.astype(np.int16) - orig.astype(np.int16)), 0, 255).astype(np.uint8)


def ausschnitt_box(n):
    return (180, 170, 450, 460) if n == 1 else (190, 140, 440, 420)


def gross(a, box, s):
    x0, y0, x1, y1 = box
    return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)


def blaetter(n, k, sauber, alb, m, z, orig, gef_bild, gef_mess, B, aus):
    punkte = B['punkte']
    zeilen = kurz_zeilen(n, B)
    ueb = ueberlagerung(orig, m, z, punkte)
    dif = differenz(gef_mess, orig)
    kopf = '%s  Bild %d  Lage %s Gier %d  Clip %d Bild %d  Licht %s k=%s' % (k.name, n, k.stand(n)[0], k.stand(n)[1], k.stand(n)[2], k.stand(n)[3], k.licht, B['licht_k'])
    tafel_rahmen([('Original', orig), ('Render sauber', sauber), ('Render durch video_filter', gef_bild), ('Differenz Filter - Original, grau = 0', dif),
                  ('Umriss: gelb Render, gruen Ziel weiss, blau Ziel grau; Kreuze Pruefpunkte', ueb), ('Kennzahlen', zahlentafel(zeilen))], 3,
                 os.path.join(aus, 'blatt_bild%d.png' % n), kopf)
    box = ausschnitt_box(n)
    tafel_rahmen([('Original', gross(orig, box, 3)), ('Render sauber', gross(sauber, box, 3)), ('Render durch video_filter', gross(gef_bild, box, 3)),
                  ('Differenz, grau = 0', gross(dif, box, 3)),
                  ('Umriss und Pruefpunkte, Nummern siehe bericht.txt', ueberlagerung(orig, m, z, punkte, 3, True)[box[1] * 3:box[3] * 3, box[0] * 3:box[2] * 3]),
                  ('Render ohne Licht (Albedo)', gross(alb, box, 3))], 3, os.path.join(aus, 'ausschnitt_bild%d.png' % n), kopf + '  Ausschnitt 3-fach')


def zaehle(liste):
    z = [x for x in liste if x['zaehlt']]
    return sum(1 for x in z if x['ok']), len(z)


def kurz_zeilen(n, B):
    gut = (120, 225, 140)
    schl = (255, 110, 100)
    u = B['umriss']
    z = [('Umriss auf nicht-grauen Pixeln', (255, 225, 120)),
         'Ueberdeckung (IoU) %.3f   Weiss gedeckt %.3f' % (u['iou'], u['weiss_gedeckt']),
         'Schwarz getroffen %d px, Render %d px, davon grau %d' % (u['schwarz_getroffen'], u['render_pixel'], u['render_auf_grau'])]
    for t, nm in (('Pruefpunkte', 'punkte'), ('Boden, Verdeckung', 'flaechen'), ('Zeichnung', 'zeichnung'), ('Verbote', 'verbote')):
        a, b = zaehle(B[nm])
        z.append(('%s %d/%d' % (t, a, b), gut if a == b and b else schl))
    z.append(('Nicht bestanden:', (255, 225, 120)))
    for nm in ('punkte', 'flaechen', 'zeichnung', 'verbote'):
        for x in B[nm]:
            if x['zaehlt'] and not x['ok']:
                d = x.get('abstand')
                z.append(('  %s  %s' % (x['id'], '' if d is None else '%+.1f / %.1f' % (d, x['grenze'])), schl))
    return z


def bericht_text(R):
    L = ['Kandidat %s' % R['name'], '']
    K = R['kennzahlen']
    L.append('(g) Kennzahlen: %d Flaechen (%d Dreiecke, %d Vierecke), %d Knochen, %d Meshes, Textur %dx%d %d bpp, %d CLUT-Zeilen, %d Clips, Bilder je Clip %s'
             % (K['flaechen'], K['dreiecke'], K['vierecke'], K['knochen'], K['meshes'], K['textur'][0], K['textur'][1], K['bpp'], K['clut_zeilen'], K['clips'], K['clip_bilder']))
    L.append('Rahmen: ' + ('alles eingehalten' if not K['rahmen'] else '; '.join(K['rahmen'])))
    L.append('Boden je Pose, tiefster Punkt y: %s' % K['boden_y'])
    L.append('FE.pruefe: %d Fehler, %d Warnungen' % (len(K['fehler']), len(K['warnungen'])))
    for x in K['fehler']:
        L.append('  FEHLER ' + x)
    for x in K['warnungen']:
        L.append('  Warnung ' + x)
    for n in (1, 2):
        B = R['bild%d' % n]
        u = B['umriss']
        L += ['', '=== Bild %d, Lage %s Gier %d Clip %d Bild %d, Licht %s k=%s' % (n, B['lage'], B['gier'], B['clip'], B['bild'], R['licht'], B['licht_k']),
              '(a) Umriss gegen %s, auf nicht-grauen Pixeln: IoU %.3f, Weiss gedeckt %.3f, Schwarz getroffen %d px, Render %d px, davon auf Grau %d'
              % (B.get('ziel', 'ziel_bild%d.png' % n), u['iou'], u['weiss_gedeckt'], u['schwarz_getroffen'], u['render_pixel'], u['render_auf_grau']),
              '    Weiss fehlt (Pixel, x0,y0,x1,y1): %s' % B['weiss_fehlt'], '    Schwarz getroffen: %s' % B['schwarz_treffer']]
        a, b = zaehle(B['punkte'])
        L.append('(b) Pruefpunkte %d/%d bestanden  (+ = rechts/unten vom Soll; kontur: + = Soll ausserhalb, Render zu klein)' % (a, b))
        for i, p in enumerate(B['punkte']):
            s = '-' if p['abstand'] is None else '%+6.1f' % p['abstand']
            L.append('  %2d %-4s %-22s %-30s Soll (%g,%g)  Abstand %6s  Grenze %4.1f%s%s'
                     % (i + 1, 'ok' if p['ok'] else ('--' if not p['zaehlt'] else 'NEIN'), p['id'], p['text'], p['soll'][0], p['soll'][1], s, p['grenze'],
                        '  ohne Kante' if p['ohne_kante'] else '', ('  zaehlt nicht' + (': ' + p['grund'] if p.get('grund') else '')) if not p['zaehlt'] else ''))
        a, b = zaehle(B['flaechen'])
        L.append('(c) Boden frei, Verdeckung %d/%d' % (a, b))
        for f in B['flaechen']:
            w = dict(f)
            kz = 'ok' if w['ok'] else ('--' if not w['zaehlt'] else 'NEIN')
            if 'abstand' in w:
                L.append('  %-4s %-5s %-46s Kante Abstand %s, Grenze %.1f' % (kz, w['id'], w['text'][:46], w['abstand'], w['grenze']))
            elif 'treffer' in w:
                L.append('  %-4s %-5s %-46s Treffer %d von %d px, Eindringen %.1f px' % (kz, w['id'], w['text'], w['treffer'], w['flaeche'], w['eindringen_px']))
            elif 'luecke_px' in w:
                L.append('  %-4s %-5s %-46s gedeckt %.3f, innen %.3f, Luecke %d px, Enden +-%s%s' % (kz, w['id'], w['text'][:46], w['anteil'], w['anteil_innen'], w['luecke_px'], w['grenze_enden'], '  zaehlt nicht' if not w['zaehlt'] else ''))
            elif 'anteil' in w:
                L.append('  %-4s %-5s %-46s gedeckt %.3f%s' % (kz, w['id'], w['text'], w['anteil'], '  stillgelegt: ' + w['grund'] if w.get('grund') else ''))
            else:
                L.append('  %-4s %-5s %-46s kleinstes x %d' % (kz, w['id'], w['text'], w['kleinstes_x']))
        a, b = zaehle(B['zeichnung'])
        L.append('(d) Zeichnung %d/%d, Anteil = Sollflaeche mit passendem Material, Y = Mittel im Video gegen gefilterten Render' % (a, b))
        for z in B['zeichnung']:
            L.append('  %-4s %-4s %-19s Material %.2f  Figur %.2f  Groesse %.2f  Lage %s  Y %3.0f/%3.0f%s'
                     % ('ok' if z['ok'] else ('--' if not z['zaehlt'] else 'NEIN'), z['id'], z['material'], z['anteil_material'], z['anteil_figur'], z['groesse'],
                        '-' if z['lage_px'] is None else '%.1f' % z['lage_px'], z['y_video'], z['y_render'], '  zaehlt nicht' if not z['zaehlt'] else ''))
        for v in B['verbote']:
            L.append('  %-4s %-4s %s: heller Anteil %.3f' % ('ok' if v['ok'] else 'NEIN', v['id'], v['text'], v['anteil_hell']))
        L.append('Bild im Original gegen Filter: MAE in Weiss %.1f, NCC in Weiss %.3f' % (B['mae_weiss'], B['ncc_weiss']))
    L += ['', *teile_text(R['teile'])] if R.get('teile') else ['', 'Teile des Nutzers: NICHT geprueft, Kandidat ohne Eintrag "teile"']
    return '\n'.join(L) + '\n'


def kennzahlen(k):
    f = FE.lese_emd(k.emd)
    F, W = FE.pruefe(f)
    t = FE.tim_lesen(k.tim)
    d = sum(len(m['dreiecke']) for m in f['meshes'])
    v = sum(len(m['vierecke']) for m in f['meshes'])
    h, w = t['pixel'].shape
    bilder = [len(c['bilder']) for c in f['clips']]
    rahmen = []
    if d + v > 1900:
        rahmen.append('%d Flaechen, Rahmen rund 1900' % (d + v))
    if len(f['knochen']) > 32:
        rahmen.append('%d Knochen, Rahmen 32' % len(f['knochen']))
    if w > 384 or h > 256:
        rahmen.append('Textur %dx%d, Rahmen 384x256' % (w, h))
    if len(bilder) < 3:
        rahmen.append('nur %d Clips, Clip 0, 1, 2 gefordert' % len(bilder))
    for c in (0, 1):
        if c < len(bilder) and bilder[c] != 1:
            rahmen.append('Clip %d hat %d Bilder, gefordert 1' % (c, bilder[c]))
    boden = []
    for c in f['clips'][:3]:
        boden.append(None if not c['bilder'] else round(float(FE.tiefster_punkt(f, c['bilder'][0]) or 0), 1))
    for c, y in enumerate(boden):
        if y is not None and abs(y) > 2:                    # werkzeug.md Abschnitt 3: tiefster Punkt auf y = 0 je Pose, sonst schwebt oder steckt die Figur
            rahmen.append('Clip %d tiefster Punkt y=%g, gefordert 0' % (c, y))
    return dict(flaechen=d + v, dreiecke=d, vierecke=v, knochen=len(f['knochen']), meshes=len(f['meshes']), textur=[w, h], bpp=8, clut_zeilen=len(t['cluts']), clips=len(bilder),
                clip_bilder=bilder[:8], rahmen=rahmen, boden_y=boden, fehler=F, warnungen=W)


def pruefe_kandidat(pfad, aus):
    os.makedirs(aus, exist_ok=True)
    k = Kandidat(pfad)
    with open(daten('grenzen.json'), encoding='utf-8') as f:
        G = json.load(f)
    rest, _ = raum_rest2()
    R = dict(name=k.name, licht=k.licht, kennzahlen=kennzahlen(k))
    for n in (1, 2):
        zname, z = ziel_laden(n)
        orig = original(n)
        kan = kanaele(orig, rest if n == 2 else None)
        sauber, alb, m, lk = k.rendern(n)
        gef_mess = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
        gef_bild = FV.video_filter(sauber, bild=n, entzerren=False)
        band = material_maske(alb, m, 'helle_baender')
        punkte = [punkt_messen(p, G[p['id']], m, band) for p in punkte_abschnitt7() if p['bild'] == n]
        w = z == 255
        a, b = (gef_mess.astype(np.float32) @ YK)[w], (orig.astype(np.float32) @ YK)[w]
        ncc = float(np.corrcoef(a, b)[0, 1]) if a.std() > 0 and b.std() > 0 else 0.0
        lage, gier, clip, bild = k.stand(n)
        B = dict(lage=list(lage), gier=gier, clip=clip, bild=bild, ziel=zname, licht_k=[round(float(x), 3) for x in lk], umriss=ueberdeckung(m, z), weiss_fehlt=flecken((z == 255) & ~m), schwarz_treffer=flecken((z == 0) & m),
                 punkte=punkte, flaechen=boden_und_verdeckung(n, m, z, kan), zeichnung=zeichnung_messen(n, alb, m, gef_mess, orig), verbote=gegenproben(n, alb, m),
                 mae_weiss=round(float(np.abs(a - b).mean()), 1), ncc_weiss=round(ncc, 3))
        R['bild%d' % n] = B
        blaetter(n, k, sauber, alb, m, z, orig, gef_bild, gef_mess, B, aus)
    r = k.j.get('ruhe', {})
    rundum(k, os.path.join(aus, 'rundum.png'), os.path.join(aus, 'rundum_hell.png'), int(r.get('clip', 2)), int(r.get('bild', 0)))
    if k.j.get('teile'):
        R['teile'] = teile_pruefen(k, aus)
    with open(os.path.join(aus, 'bericht.json'), 'w', encoding='utf-8') as f:
        json.dump(R, f, ensure_ascii=False, indent=1)
    t = bericht_text(R)
    with open(os.path.join(aus, 'bericht.txt'), 'w', encoding='utf-8') as f:
        f.write(t)
    return R, t


# ═══════════════════════════════════════════════════ Teile des Nutzers: Quellen
TEILE_ORDNER = os.path.join(FORM, 'nutzer_teile')
VIDEO = ('Version_80%', 'Biohazard 1.5 Final Build.mp4')
# Farben wie teilekarte_bild*.png, damit Blatt und Karte sich lesen wie eines. Weitere Namen aus "teile" nehmen FREMDFARBEN, Knochen ohne Teil Grau.
TEILFARBE = {'tail': (125, 255, 120), 'left_leg': (255, 150, 40), 'right_leg': (40, 225, 255), 'right_arm': (255, 40, 255), 'part_of_Body': (255, 165, 185),
             'face_and_upper_body': (255, 235, 40), 'left_arm_front': (60, 210, 60), 'right_arm_front': (50, 130, 255)}
FREMDFARBEN = ((150, 110, 240), (240, 120, 90), (100, 200, 170), (200, 200, 120), (170, 130, 110), (120, 170, 240))
RESTFARBE = (150, 152, 158)
ENDNAME = {'tail': ('Ansatz', 'Spitze'), 'left_leg': ('oben', 'Fuss'), 'right_leg': ('oben', 'Fuss'), 'right_arm': ('Schulter', 'Hand'), 'part_of_Body': ('oben', 'unten'),
           'face_and_upper_body': ('oben', 'unten'), 'left_arm_front': ('Schulter', 'Klaue'), 'right_arm_front': ('Schulter', 'Klauenhand')}
OHNE_SPITZEN = ('part_of_Body', 'face_and_upper_body', 'tail')  # keine Klauen: Flaechen, und der Schwanz hat nur sein Ende


def teile_lage():
    return _json(os.path.join(TEILE_ORDNER, 'lage.json'))


def teile_von(n):
    return [t for t, l in teile_lage().items() if l['bild'] == n]


def teil_roh(name):
    """Maske, wie der Nutzer sie geschnitten hat, 640x480. Handschnitt: legt fest, WAS WO ist, nicht jede Kante auf den Pixel."""
    k = 'teil:' + name
    if k not in _ablage:
        m = cv2.imread(os.path.join(TEILE_ORDNER, 'maske_%s.png' % name), 0)
        if m is None:
            raise SystemExit('fehlt: maske_%s.png' % name)
        _ablage[k] = m > 0
    return _ablage[k]


def teil_maske(name):
    """Maske ohne Logo-Schrift. Gilt fuer jedes Teil: Die Schrift liegt in Bild 1 genauso ueber tail und left_leg wie in Bild 2 ueber right_arm_front."""
    return teil_roh(name) & ~logo_maske()


def teil_merkmale(name):
    """Stellen, die der Auftrag einem Teil zuschreibt: Gegenstand X ist die Schwanzspitze, die deckende Flaeche Y ist die Hand. Lage aus formblatt.json, nur ihre Deutung ist neu."""
    if name == 'tail':
        return [('X Schwanzspitze', poly_maske(teil_b1('X')['polygon']))]
    if name == 'right_arm_front':
        e = teil_b2('Y')['ellipse']
        y = np.zeros((HOEHE, BREITE), np.uint8)
        cv2.ellipse(y, tuple(e['mitte']), tuple(e['halbachsen']), 0, 0, 360, 1, -1)
        return [('Y Hand', y.astype(bool))]
    return []


def video_pfad():
    """Das Video liegt nur im Hauptbaum. Vom Werkzeug aus aufwaerts suchen, auch aus einem Arbeitsbaum unter .claude/worktrees."""
    p = HIER
    while True:
        q = os.path.join(p, *VIDEO)
        if os.path.exists(q):
            return q
        o = os.path.dirname(p)
        if o == p:
            return None
        p = o


def logo_maske(neu=False):
    """Schrift des Videos, gemessen statt geschwellt. Das Logo steht ueber allen Bildern still, das Bild darunter wechselt: Zeitstreuung je Pixel ueber jedes fuenfte
    Videobild, Otsu trennt beide Haeufungen. Bild 1 und 2 sind Bilder dieses Videos in gleicher Lage. Abgelegt, damit pruefe das Video nicht braucht."""
    pfad = daten('logo_maske.png')
    if not neu:
        if 'logo' not in _ablage:
            if not os.path.exists(pfad):
                raise SystemExit('fehlt: pruefung/logo_maske.png, erst ziele2 laufen lassen')
            _ablage['logo'] = cv2.imread(pfad, 0) > 0
        return _ablage['logo']
    v = video_pfad()
    if v is None:
        raise SystemExit('Video nicht gefunden: ' + '/'.join(VIDEO))
    c = cv2.VideoCapture(v)
    y0, x1 = 420, 320                                       # Schrift endet bei x rund 255, Abschnitt 5: Zone (0-253,436-480)
    st, i = [], 0
    while c.grab():
        i += 1
        if i % 5 == 0:
            st.append(cv2.cvtColor(c.retrieve()[1], cv2.COLOR_BGR2GRAY)[y0:, :x1])
    c.release()
    s = np.stack(st).astype(np.float32).std(0)
    t = cv2.threshold(np.clip(s * 2, 0, 255).astype(np.uint8), 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[0] / 2
    m = s < t
    m[:, :2] = False                                        # schwarzer Bildrand, steht auch still
    k, lab, sta, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    for j in range(1, k):
        if sta[j, 4] < 20:                                  # Einzelpixel im Bild, die zufaellig still blieben
            m[lab == j] = False
    aus = np.zeros((HOEHE, BREITE), bool)
    aus[y0:, :x1] = m
    cv2.imwrite(pfad, aus.astype(np.uint8) * 255)
    _ablage['logo'] = aus
    beleg = []
    for n in (1, 2):                                        # Beleg: Umriss auf beiden Bildern, 4-fach
        a = cv2.resize(original(n)[y0:, :x1], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
        umriss_auf(a, aus, (0, 255, 0), (0, y0), 4)
        beleg.append(a)
    cv2.imwrite(daten('logo_beleg.png'), np.vstack(beleg)[:, :, ::-1])
    print('logo_maske.png: %d Videobilder, Otsu Streuung %.1f, %d Pixel, x %d-%d, y %d-%d'
          % (len(st), t, aus.sum(), np.nonzero(aus.any(0))[0][0], np.nonzero(aus.any(0))[0][-1], np.nonzero(aus.any(1))[0][0], np.nonzero(aus.any(1))[0][-1]))
    return aus


def ziel_laden(n):
    """Gesamtziel fuer pruefe: ziel2_bildN.png, die Lesung des Nutzers. Das alte Ziel nur, solange ziele2 nicht lief."""
    for nm in ('ziel2_bild%d.png' % n, 'ziel_bild%d.png' % n):
        p = os.path.join(FORM, nm)
        if os.path.exists(p):
            return nm, cv2.imread(p, 0)
    raise SystemExit('fehlt: ziel_bild%d.png, erst ziele laufen lassen' % n)


def ziel2_bauen(n):
    """Weiss = alle Teile des Nutzers. Schwarz = sicher Boden oder Hintergrund: Schwarz aus ziel_bildN.png, dazu der Sockel (FORMBLATT 1.4 sicher, im alten Ziel nur wegen A10
    grau). Der Rest grau, auch altes Weiss ausserhalb der Teile: Die Teile sind keine Vollstaendigkeitsaussage. Logo-Schrift grau, was unter ihr liegt, zeigt das Bild nicht.
    Weiss schlaegt Schwarz: Vorrang der Lesung. Die Boden-frei-Pruefung nimmt Schwarz aus diesem Ziel, wer dem Nutzerumriss folgt, wird also nie bestraft."""
    alt = cv2.imread(os.path.join(FORM, 'ziel_bild%d.png' % n), 0)
    z = np.where(alt == 0, 0, 128).astype(np.uint8)
    if n == 1:
        z[rechteck(*nicht_figur('SOCKEL')['bereich_px'])] = 0
    nutzer = np.zeros((HOEHE, BREITE), bool)
    for t in teile_von(n):
        nutzer |= teil_roh(t)
    z[nutzer] = 255
    z[logo_maske()] = 128
    return z, alt


def ziel2_zonen(n):
    """Sichere Zonen aus FORMBLATT 1.4 und Abschnitt 7, zur Kontrolle gegen ziel2. Feuer ist grau (P1), Saeume bleiben grau."""
    if n == 1:
        return [(i, poly_maske(zone_b1(i))) for i in ('L1', 'L2', 'SCH')] + [('SOCKEL', rechteck(*nicht_figur('SOCKEL')['bereich_px']))]
    (x0, y0, x1, y1), (a0, b0, a1, b1) = formblatt()['pruefpunkte']['bild2']['boden_frei']
    return [('BO', rechteck(x0, y0, x1, y1)), ('FUGE', rechteck(a0, b0, a1, b1)), ('A13 Boden', rechteck(278, 395, 450, 475)), ('Tuerplatte', rechteck(350, 112, 395, 172)),
            ('Kisten', rechteck(540, 265, 639, 450)), ('Feuer 1', rechteck(160, 325, 228, 388)), ('Feuer 2', rechteck(415, 225, 462, 300)), ('Feuer 3', rechteck(415, 310, 505, 400))]


def befehl_ziele2(a=None):
    os.makedirs(DATEN, exist_ok=True)
    if (a is not None and a.logo_neu) or not os.path.exists(daten('logo_maske.png')):
        logo_maske(neu=True)
    stand = {}
    for n in (1, 2):
        z, alt = ziel2_bauen(n)
        cv2.imwrite(os.path.join(FORM, 'ziel2_bild%d.png' % n), z)
        ziel_uebersicht(n, z, 'ziel2_bild%d_auf_original.png')
        S = dict(weiss=int((z == 255).sum()), grau=int((z == 128).sum()), schwarz=int((z == 0).sum()), teile={}, zonen={})
        for t in teile_von(n):
            r = teil_roh(t)
            S['teile'][t] = dict(flaeche=int(r.sum()), logo=int((r & logo_maske()).sum()), auf_altem_schwarz=int((r & (alt == 0)).sum()))
        for i, m in ziel2_zonen(n):
            S['zonen'][i] = dict(flaeche=int(m.sum()), weiss=int((m & (z == 255)).sum()), grau=int((m & (z == 128)).sum()), schwarz=int((m & (z == 0)).sum()))
        stand['bild%d' % n] = S
        print('ziel2_bild%d.png: weiss %d, grau %d, schwarz %d Pixel' % (n, S['weiss'], S['grau'], S['schwarz']))
        for t, w in S['teile'].items():
            print('  %-20s %6d px, Logo %5d, auf altem Schwarz %5d' % (t, w['flaeche'], w['logo'], w['auf_altem_schwarz']))
        for i, w in S['zonen'].items():
            print('  Zone %-11s %6d px: weiss %5d, grau %5d, schwarz %6d' % (i, w['flaeche'], w['weiss'], w['grau'], w['schwarz']))
    with open(daten('ziel2_stand.json'), 'w', encoding='utf-8') as f:
        json.dump(stand, f, ensure_ascii=False, indent=1)


# ═══════════════════════════════════════════════════ Teile: Render mit Kennung
KODE = 7        # Knochen b im Rotkanal als KODE * (b + 1) + 1: rastern schneidet die Farbe ab, der Rest 1 faengt das auf


def knochen_zahl(k):
    return min(k.mo.skel.bone_count, len(k.mo.meshes))


def teil_zuordnung(k, n):
    """Knochen -> Teil fuer Bild n. Namen dieses Bildes zuerst: So kann derselbe Arm in Bild 1 right_arm und in Bild 2 right_arm_front heissen. Danach alle uebrigen
    Namen, ein Bein bleibt also auch in Bild 2 ein Bein und wird nicht zum Rest. Rueckgabe: {Knochen: Teil}, Warnungen."""
    T = k.j.get('teile') or {}
    nk = knochen_zahl(k)
    hier = teile_von(n)
    zu, warn = {}, []
    for t in hier + [x for x in T if x not in hier]:
        for b in T.get(t, []):
            if not isinstance(b, int) or not 0 <= b < nk:
                if t in hier:
                    warn.append('%s: Knochen %s gibt es nicht, Modell hat %d' % (t, b, nk))
            elif b not in zu:
                zu[b] = t
            elif t in hier and zu[b] != t:
                warn.append('Knochen %d steht in %s und %s, zaehlt fuer %s' % (b, zu[b], t, zu[b]))
    for t in hier:
        if t not in T:
            warn.append('%s: fehlt in "teile"' % t)
    return zu, warn


def teil_farbe(t, namen):
    if t in TEILFARBE:
        return TEILFARBE[t]
    if t == 'Rest':
        return RESTFARBE
    f = [x for x in namen if x not in TEILFARBE and x != 'Rest']
    return FREMDFARBEN[f.index(t) % len(FREMDFARBEN)]


def knochen_farben(zu, namen, nk):
    """Je Knochen eine eigene Farbe: Farbton des Teils, Schritte in Ton und Helligkeit innerhalb des Teils."""
    lut = np.zeros((nk, 3), float)
    z = {}
    for b in range(nk):
        t = zu.get(b, 'Rest')
        i = z[t] = z.get(t, -1) + 1
        h, s, v = colorsys.rgb_to_hsv(*(c / 255.0 for c in teil_farbe(t, namen)))
        h = (h + ((i + 1) // 2) * (1 if i % 2 else -1) * 0.018) % 1.0
        lut[b] = np.array(colorsys.hsv_to_rgb(h, s, v * (1.0, 0.74, 0.88, 0.62)[i % 4])) * 255
    return lut


def kopflicht(f):
    """Formlicht nur fuers Blatt: von schraeg oben hinter der Kamera. Formbeschreibung aus form_buehne.licht_kontext, Typ 1 = Richtungslicht, lage = Laufrichtung des Lichts."""
    d = f['R'].T @ np.array([0.35, 0.5, 1.0])
    return dict(skala=1, typ=(1, 1, 1), farbe=[(255, 255, 255), (0, 0, 0), (0, 0, 0)], umgebung=(0, 0, 0),
                lage=[tuple(int(round(x * 4096)) for x in d), (0, 0, 0), (0, 0, 0)], hell=(0, 0, 0))


def teilbild(k, n, zu):
    """Render ohne Video-Filter, Kennung statt Textur. Dieselbe Zeichenfolge wie Kandidat.rendern: dreiecke je Knochen in Knochenfolge, rastern sortiert stabil, durchsichtige
    Texel bleiben durchsichtig. Dazu jedes Teil allein: Nur so laesst sich sagen, was von ihm verdeckt ist."""
    lage, gier, clip, bild = k.stand(n)
    kam = kamera(n)
    kf = k.kf(clip, bild)
    weiss = FB.Textur(k.tim)
    weiss.lut = weiss.lut.copy()
    weiss.lut[..., :3] = 255
    nk = knochen_zahl(k)
    if KODE * (nk + 1) + 1 > 255:
        raise SystemExit('Teil-Kennung traegt hoechstens 35 Knochen, Modell hat %d' % nk)
    je = []
    for b in range(nk):
        c = np.full(3, float(KODE * (b + 1) + 1))
        je.append([(t[0], t[1], t[2], t[3], (c, c, c)) for t in FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile={b})])
    puffer = np.zeros((HOEHE, BREITE, 3), np.uint8)
    FB.rastern([t for g in je for t in g], weiss, puffer, kam.abbildung)
    kn = puffer[..., 0].astype(np.int32) // KODE - 1
    loch = geschlossen(kn >= 0) & (kn < 0)                  # Stichloecher wie geschlossen() beim Umriss, aber mit dem Knochen der Nachbarn
    for y, x in zip(*np.nonzero(loch)):
        f = kn[max(y - 1, 0):y + 2, max(x - 1, 0):x + 2]
        kn[y, x] = np.bincount(f[f >= 0]).argmax()
    hier = teile_von(n)
    namen = hier + [t for t in dict.fromkeys(zu.values()) if t not in hier] + ['Rest']
    lut = np.array([namen.index(zu.get(b, 'Rest')) for b in range(nk)], np.int32)
    karte = np.full((HOEHE, BREITE), -1, np.int32)
    karte[kn >= 0] = lut[kn[kn >= 0]]
    knochen = {t: sorted(b for b, x in zu.items() if x == t) for t in namen if t != 'Rest' and any(x == t for x in zu.values())}
    rest = sorted(b for b in range(nk) if b not in zu)
    if rest:
        knochen['Rest'] = rest
    allein = {}
    for t in hier:
        if t in knochen:
            allein[t] = geschlossen(FB.rastern([d for b in knochen[t] for d in je[b]], weiss, np.zeros((HOEHE, BREITE, 3), np.uint8), kam.abbildung))
    form = np.zeros((HOEHE, BREITE, 3), np.uint8)
    FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, kopflicht(FK.lade_kamera(n))), weiss, form, kam.abbildung)
    farbe = knochen_farben(zu, namen, nk)
    b = np.full((HOEHE, BREITE, 3), 30, np.uint8)
    m = kn >= 0
    b[m] = np.clip(farbe[kn[m]] * (0.42 + 0.58 * form[m, 0:1] / 255.0), 0, 255).astype(np.uint8)
    naht = m & ((kn != np.roll(kn, 1, 0)) | (kn != np.roll(kn, 1, 1)))
    b[naht] = (b[naht] * 0.55).astype(np.uint8)
    return dict(namen=namen, karte=karte, allein=allein, knochen=knochen, kn=kn, bild=b)


# ═══════════════════════════════════════════════════ Teile: Messung
def achse(m):
    """Schwerpunkt und Hauptachse (Pixel gleich gewichtet). Grad: 0 = waagerecht, 90 = senkrecht, Bild-y nach unten. Laenge und Breite 1-99 %, damit ein Ausreisser nicht zaehlt."""
    ys, xs = np.nonzero(m)
    if len(xs) < 8:
        return None
    p = np.c_[xs, ys].astype(float)
    c = p.mean(0)
    q = p - c
    w, v = np.linalg.eigh(q.T @ q / len(q))
    e = v[:, 1]
    if e[1] < 0 or (e[1] == 0 and e[0] < 0):
        e = -e
    n = np.array([-e[1], e[0]])
    a, b = q @ e, q @ n
    return dict(mitte=c, e=e, n=n, grad=math.degrees(math.atan2(e[1], e[0])), laenge=float(np.percentile(a, 99) - np.percentile(a, 1)),
                breite=float(np.percentile(b, 99) - np.percentile(b, 1)), streckung=float(math.sqrt(w[1] / max(w[0], 1e-9))))


def groesste(m):
    k, lab, st, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    return None if k <= 1 else lab == 1 + int(np.argmax(st[1:, 4]))


def geodaetisch(q, start):
    """Weg innerhalb der Maske von start (y, x) zu jedem Pixel; -1 ausserhalb."""
    d = MCP_Geometric(np.where(q, 1.0, -1.0), fully_connected=True).find_costs([tuple(int(c) for c in start)])[0]
    d = np.where(q & np.isfinite(d), d, -1.0)
    return d


def enden(m):
    """Die beiden Enden = laengster Weg innerhalb der Form (zweimal hin und her). Bei einem gebogenen Schwanz traegt das, wo die Hauptachse aus der Form laeuft.
    Rueckgabe: Form, Ende oben, Ende unten (y, x), Weglaenge; None ohne Form."""
    q = groesste(m)
    if q is None:
        return None
    dt = cv2.distanceTransform(q.astype(np.uint8), cv2.DIST_L2, 5)
    d = geodaetisch(q, np.unravel_index(int(np.argmax(dt)), dt.shape))
    a = np.unravel_index(int(np.argmax(d)), d.shape)
    d = geodaetisch(q, a)
    b = np.unravel_index(int(np.argmax(d)), d.shape)
    if a[0] > b[0]:
        a, b = b, a
    return q, (int(a[0]), int(a[1])), (int(b[0]), int(b[1])), float(d.max())


def spitzen(q, wurzel, mindest=SPITZE_MIN_PX):
    """Spitzen einer Form: Gipfel des Wegs von der Wurzel, die um mindest Pixel aus ihrem Sattel ragen, Schartenhoehe wie bei Bergen. So zaehlt eine Klaue, die Zacke im Handschnitt nicht.
    Rueckgabe: Liste (y, x, Ueberragung), weiteste zuerst."""
    d = geodaetisch(q, wurzel)
    ys, xs = np.nonzero(q)
    lab = np.full(q.shape, -1, np.int32)
    gipfel, wer, tot = [], [], {}

    def finde(i):
        while wer[i] != i:
            wer[i] = wer[wer[i]]
            i = wer[i]
        return i
    for j in np.argsort(-d[ys, xs], kind='stable'):
        y, x = int(ys[j]), int(xs[j])
        nb = set()
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                v, u = y + dy, x + dx
                if 0 <= v < HOEHE and 0 <= u < BREITE and lab[v, u] >= 0:
                    nb.add(finde(int(lab[v, u])))
        if not nb:
            lab[y, x] = len(gipfel)
            wer.append(len(gipfel))
            gipfel.append((float(d[y, x]), y, x))
            continue
        nb = sorted(nb, key=lambda i: -gipfel[i][0])
        lab[y, x] = nb[0]
        for i in nb[1:]:
            wer[i] = nb[0]
            tot[i] = gipfel[i][0] - float(d[y, x])
    aus = [(g[1], g[2], float('inf') if i not in tot else tot[i]) for i, g in sorted(enumerate(gipfel), key=lambda t: -t[1][0])]
    return [a for a in aus if a[2] >= mindest]


def _in_zone(m, y, x, r=2):
    return bool(m[max(y - r, 0):y + r + 1, max(x - r, 0):x + r + 1].any())


def ende_zeile(name, soll, ist, sichtbar, logo):
    return dict(name=name, soll=[soll[1], soll[0]], ist=[ist[1], ist[0]], dx=ist[1] - soll[1], dy=ist[0] - soll[0], abstand=round(math.hypot(ist[1] - soll[1], ist[0] - soll[0]), 1),
                sichtbar=_in_zone(sichtbar, *ist), unter_logo=_in_zone(logo, *soll))


def teil_enden(name, A, V):
    """Enden und Spitzen von Soll und Modell. Am Modell zaehlt das ganze Teil, auch verdeckt: Wo die Schwanzspitze im Bild liegt, haengt nicht davon ab, was davor steht; ob man sie sieht, sagt
    sichtbar. Am Soll gelten die Enden ohne Logo-Schrift. Sonst endete der Schwanz in der Ecke (209,478) unter ".COM" statt am rosa Ende. Die Spitzen nehmen den rohen Handschnitt: Unter der Schrift ist er die
    einzige Aussage zur Hand, und die Luecken zwischen den Buchstaben ergaeben falsche Finger. unter_logo kennzeichnet das."""
    logo = logo_maske()
    s, i = enden(teil_maske(name)), enden(A)
    if s is None or i is None:
        return [], []
    _q, oben, unten, _l = s
    qi, c, d, _m = i
    if math.dist(oben, c) + math.dist(unten, d) > math.dist(oben, d) + math.dist(unten, c):
        c, d = d, c
    nm = ENDNAME[name]
    e = [ende_zeile(nm[0], oben, c, V, logo), ende_zeile(nm[1], unten, d, V, logo)]
    if name in OHNE_SPITZEN:
        return e, []
    ss, si = spitzen(groesste(teil_roh(name)), oben), spitzen(qi, c)
    frei = list(range(len(si)))
    paare = sorted(((math.dist(a[:2], b[:2]), ia, ib) for ia, a in enumerate(ss) for ib, b in enumerate(si)), key=lambda t: t[0])
    zu = {}
    for dd, ia, ib in paare:
        if ia not in zu and ib in frei:
            zu[ia] = ib
            frei.remove(ib)
    aus = []
    for ia, a in enumerate(ss):
        w = dict(soll=[a[1], a[0]], unter_logo=_in_zone(logo, a[0], a[1]), ist=None, abstand=None, sichtbar=None)
        if ia in zu:
            b = si[zu[ia]]
            w.update(ist=[b[1], b[0]], abstand=round(math.dist(a[:2], b[:2]), 1), sichtbar=_in_zone(V, b[0], b[1]))
        aus.append(w)
    for ib in frei:
        b = si[ib]
        aus.append(dict(soll=None, unter_logo=False, ist=[b[1], b[0]], abstand=None, sichtbar=_in_zone(V, b[0], b[1])))
    return e, aus


def teil_messen(name, tb):
    """Ein Nutzer-Teil gegen den gleichnamigen Modell-Teil. tb = teilbild() oder eine Karte aus der Probe. Logo-Pixel zaehlen auf keiner Seite. M Maske, V sichtbarer Teil, A Teil allein.
    Schnitte des Nutzers ueberlappen sich (part_of_Body und right_leg 1316 px). Dort darf jeder der beiden Teile stehen: Steht der andere, faellt das Pixel aus der Wertung, damit kein Modell daran scheitert."""
    ro, M = teil_roh(name), teil_maske(name)
    logo = logo_maske()
    namen, karte = tb['namen'], tb['karte']
    A = tb['allein'].get(name)
    i = namen.index(name) if name in namen else -2
    geteilt, neutral = {}, np.zeros((HOEHE, BREITE), bool)
    for o in teile_von(teile_lage()[name]['bild']):
        q = M & teil_roh(o)
        if o != name and q.any():
            geteilt[o] = int(q.sum())
            if o in namen:
                neutral |= q & (karte == namen.index(o))
    W = M & ~neutral
    fl = max(int(W.sum()), 1)
    fr = {nm: round(int(((karte == j) & W).sum()) / fl, 3) for j, nm in enumerate(namen) if j != i and ((karte == j) & W).any()}
    aus = dict(teil=name, knochen=tb['knochen'].get(name, []), maske_px=int(M.sum()), wertung_px=int(W.sum()), logo_px=int((ro & logo).sum()), geteilt=geteilt, treffer_px=0, iou=None, fremd=fr,
               leer=round(float(((karte < 0) & W).sum()) / fl, 3))
    am = achse(M)
    merk = [(t, m & ~logo) for t, m in teil_merkmale(name)]
    aus['merkmale'] = [dict(name=t, flaeche=int(m.sum()), gedeckt=round(float((m & (karte == i)).sum()) / max(int(m.sum()), 1), 3)) for t, m in merk]
    if A is None or not A.any():
        aus['hinweis'] = 'kein Knochen zugeordnet' if not aus['knochen'] else 'liegt nicht im Bild'
        return aus
    Vg = karte == i
    V = Vg & ~logo
    t = int((V & W).sum())
    K, S = enge(M, TEIL_SAUM_PX) & ~neutral, weite(M, TEIL_SAUM_PX)
    vp = max(int(V.sum()), 1)
    ap = max(int(A.sum()), 1)
    aus.update(render_px=int(V.sum()), allein_px=int(A.sum()), treffer_px=t, iou=round(t / max(int((V | W).sum()), 1), 3), deckung=round(t / fl, 3), kern_deckung=round(int((V & K).sum()) / max(int(K.sum()), 1), 3),
               innen=round(t / vp, 3), innen_saum=round(int((V & S).sum()) / vp, 3), verdeckt=round(max(1.0 - int(Vg.sum()) / ap, 0.0), 3),
               verdeckt_von={nm: round(int(((karte == j) & A).sum()) / ap, 3) for j, nm in enumerate(namen) if j != i and ((karte == j) & A).any()})
    av = achse(V)
    aus['schwerpunkt'] = aus['achse'] = None
    if av is not None and am is not None:
        d = av['mitte'] - am['mitte']
        w = abs(av['grad'] - am['grad']) % 180
        aus['schwerpunkt'] = dict(soll=[round(float(x), 1) for x in am['mitte']], ist=[round(float(x), 1) for x in av['mitte']], dx=round(float(d[0]), 1), dy=round(float(d[1]), 1),
                                  abstand=round(float(np.linalg.norm(d)), 1))
        aus['achse'] = dict(soll_grad=round(am['grad'], 1), ist_grad=round(av['grad'], 1), abweichung_grad=round(min(w, 180 - w), 1), quer_px=round(float(d @ am['n']), 1), laengs_px=round(float(d @ am['e']), 1),
                            laenge=[round(av['laenge'], 1), round(am['laenge'], 1)], breite=[round(av['breite'], 1), round(am['breite'], 1)], streckung_soll=round(am['streckung'], 2))
    aus['enden'], aus['spitzen'] = teil_enden(name, A, Vg)
    return aus


def teile_pruefen(k, aus):
    os.makedirs(aus, exist_ok=True)
    R = {}
    for n in (1, 2):
        zu, warn = teil_zuordnung(k, n)
        tb = teilbild(k, n, zu)
        zl = [teil_messen(t, tb) for t in teile_von(n)]
        tr = sum(z['treffer_px'] for z in zl)
        fl = sum(z['wertung_px'] for z in zl)
        ious = [z['iou'] for z in zl if z['iou'] is not None]
        R['bild%d' % n] = dict(zuordnung={str(b): t for b, t in sorted(zu.items())}, knochen=tb['knochen'], warnungen=warn, teile=zl, trefferquote=round(tr / max(fl, 1), 3),
                               iou_mittel=round(float(np.mean(ious)), 3) if ious else None)
        teile_blatt(n, k, tb, R['bild%d' % n], os.path.join(aus, 'teile_bild%d.png' % n))
    return R


def _knochen_kurz(b):
    return ','.join(str(x) for x in b) if b else '-'


def _anteile(d):
    return ', '.join('%s %.2f' % (k, v) for k, v in sorted(d.items(), key=lambda t: -t[1]))


def teile_text(T):
    L = ['=== Teile des Nutzers. Soll: nutzer_teile, Logo-Schrift zaehlt nirgends, Saum %d px; Abstaende: + = rechts/unten vom Soll' % TEIL_SAUM_PX,
         'IoU und Deckung am sichtbaren Teil. Deckung = Maske gedeckt, Kern = Maske minus Saum, Innen = Teil in der Maske, +Saum = Maske plus Saum. Verdeckt = vom Teil allein durch andere Modell-Teile.',
         'Leer = Maske ohne jedes Modell. Laenge, Breite: Ist/Soll laengs und quer zur Hauptachse. Spalte Wertung = Maskenpixel, die zaehlen.']
    kopf = '  %-20s %-16s %7s %5s %5s %5s %5s %5s %5s %5s  %-20s %-13s %-9s %s'
    for n in (1, 2):
        B = T['bild%d' % n]
        L.append('--- Bild %d: Anteil aller Nutzerpixel, die der richtige Teil deckt %.3f, mittlere IoU %s' % (n, B['trefferquote'], '-' if B['iou_mittel'] is None else '%.3f' % B['iou_mittel']))
        for w in B['warnungen']:
            L.append('  Warnung ' + w)
        L.append(kopf % ('Teil', 'Knochen', 'Wertung', 'IoU', 'Deck', 'Kern', 'Innen', '+Saum', 'Verd', 'Leer', 'Schwerpunkt dx,dy', 'Achse', 'Laenge', 'Breite'))
        for z in B['teile']:
            if z['iou'] is None:
                L.append('  %-20s %-16s %6d %s; Leer %.2f; in der Maske: %s' % (z['teil'], _knochen_kurz(z['knochen']), z['wertung_px'], z['hinweis'], z['leer'], _anteile(z['fremd']) or '-'))
                continue
            s, a = z['schwerpunkt'], z['achse']
            L.append(kopf % (z['teil'], _knochen_kurz(z['knochen'])[:16], z['wertung_px'], '%.2f' % z['iou'], '%.2f' % z['deckung'], '%.2f' % z['kern_deckung'], '%.2f' % z['innen'], '%.2f' % z['innen_saum'],
                             '%.2f' % z['verdeckt'], '%.2f' % z['leer'], '-' if s is None else '%+.0f,%+.0f (%.1f)' % (s['dx'], s['dy'], s['abstand']),
                             '-' if a is None else '%.0f Grad %+.0f px' % (a['abweichung_grad'], a['quer_px']), '-' if a is None else '%.0f/%.0f' % tuple(a['laenge']), '-' if a is None else '%.0f/%.0f' % tuple(a['breite'])))
            if z['geteilt']:
                L.append('      Schnitt ueberlappt ' + ', '.join('%s %d px' % x for x in z['geteilt'].items()) + ', dort zaehlt jeder der beiden Teile')
            if z['fremd']:
                L.append('      in der Maske statt des Teils: ' + _anteile(z['fremd']))
            if z['verdeckt_von']:
                L.append('      verdeckt von: ' + _anteile(z['verdeckt_von']))
            for e in z['merkmale']:
                L.append('      Merkmal %s: %.2f vom Teil gedeckt, %d px' % (e['name'], e['gedeckt'], e['flaeche']))
            for e in z['enden']:
                L.append('      Ende %-10s Soll (%d,%d)  Ist (%d,%d)  %+d,%+d  Abstand %.1f%s%s' % (e['name'], *e['soll'], *e['ist'], e['dx'], e['dy'], e['abstand'], '' if e['sichtbar'] else '  verdeckt', '  Soll unter Logo' if e['unter_logo'] else ''))
            for e in z['spitzen']:
                if e['soll'] is None:
                    L.append('      Spitze ueberzaehlig: Ist (%d,%d)%s' % (*e['ist'], '' if e['sichtbar'] else '  verdeckt'))
                elif e['ist'] is None:
                    L.append('      Spitze fehlt: Soll (%d,%d)%s' % (*e['soll'], '  unter Logo' if e['unter_logo'] else ''))
                else:
                    L.append('      Spitze Soll (%d,%d)  Ist (%d,%d)  Abstand %.1f%s%s' % (*e['soll'], *e['ist'], e['abstand'], '' if e['sichtbar'] else '  verdeckt', '  Soll unter Logo' if e['unter_logo'] else ''))
    return L


def umriss_auf(a, m, f, box, s, stark=False):
    k = cv2.findContours(m.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)[0]
    for q in k:
        p = np.round((q[:, 0, :] + 0.5 - np.array(box[:2])) * s).astype(np.int32)
        if stark:
            cv2.polylines(a, [p], True, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.polylines(a, [p], True, f, 1, cv2.LINE_AA)


def schrift_auf(a, t, x, y, f, g=0.42):
    cv2.putText(a, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, f, 1, cv2.LINE_AA)


def teile_blatt(n, k, tb, B, ziel):
    """Original mit Nutzerumrissen neben Render mit Teil-Farben ohne Filter, darueber dieselben Umrisse. Ausschnitt = alle Masken plus Render, damit ein Teil, das weit daneben liegt, sichtbar bleibt."""
    namen = teile_von(n)
    alle = tb['karte'] >= 0
    for t in namen:
        alle = alle | teil_roh(t)
    ys, xs = np.nonzero(alle)
    box = (max(int(xs.min()) - 14, 0), max(int(ys.min()) - 14, 0), min(int(xs.max()) + 15, BREITE), min(int(ys.max()) + 15, HOEHE))
    s = 3 if max(box[2] - box[0], box[3] - box[1]) <= 340 else 2
    o = gross(original(n), box, s).copy()
    r = gross(tb['bild'], box, s).copy()
    for a in (o, r):
        umriss_auf(a, logo_maske(), (120, 122, 128), box, s)
        for t in namen:
            umriss_auf(a, teil_roh(t), TEILFARBE[t], box, s, stark=True)
    for b in np.unique(tb['kn'][tb['kn'] >= 0]):                # Knochennummern: Wer die Zuordnung pflegt, muss die Knochen im Bild finden
        q = achse(tb['kn'] == b)
        if q is not None:
            schrift_auf(r, str(int(b)), int((q['mitte'][0] - box[0]) * s), int((q['mitte'][1] - box[1]) * s), (255, 255, 255), 0.36)
    for t in namen:
        q = achse(teil_roh(t))
        schrift_auf(o, t, max(int((q['mitte'][0] - box[0]) * s) - 30, 2), int((q['mitte'][1] - box[1]) * s), TEILFARBE[t])
    zl = [(('Bild %d  Trefferquote %.3f  mittlere IoU %s  Lage %s Gier %d Clip %d Bild %d' % (n, B['trefferquote'], B['iou_mittel'], *k.stand(n)),), (214, 216, 222)),
          (('Teil', 'Knochen', 'IoU', 'Deck', 'Kern', 'Innen', 'Verd', 'Schwerpunkt', 'Achse', 'Enden'), (150, 153, 160))]
    for z in B['teile']:
        if z['iou'] is None:
            zl.append(((z['teil'], '-', z['hinweis']), TEILFARBE[z['teil']]))
            continue
        s0, a = z['schwerpunkt'], z['achse']
        en = '  '.join('%s %.0f%s' % (e['name'], e['abstand'], '' if e['sichtbar'] else ' verd.') for e in z['enden'])
        zl.append(((z['teil'], _knochen_kurz(z['knochen'])[:14], '%.2f' % z['iou'], '%.2f' % z['deckung'], '%.2f' % z['kern_deckung'], '%.2f' % z['innen'], '%.2f' % z['verdeckt'],
                    '-' if s0 is None else '%+.0f,%+.0f' % (s0['dx'], s0['dy']), '-' if a is None else '%.0f Grad %+.0f px' % (a['abweichung_grad'], a['quer_px']), en), TEILFARBE[z['teil']]))
    for t in tb['namen']:
        if t not in namen and t in tb['knochen']:
            zl.append(((t, _knochen_kurz(tb['knochen'][t])[:14], 'ohne Maske in diesem Bild'), teil_farbe(t, tb['namen'])))
    h, w = o.shape[:2]
    pad, zeile = 12, 19
    b = Image.new('RGB', (pad + 2 * (w + pad), 34 + h + 26 + zeile * len(zl) + pad), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), '%s  Teile des Nutzers  Ausschnitt %s, %d-fach' % (k.name, box, s), font=schrift(16), fill=(170, 173, 180))
    for i, (t, a) in enumerate((('Original mit Nutzerumrissen, grau = Logo-Schrift', o), ('Render ohne Filter: je Knochen eine Farbe im Ton des Teils, gleiche Umrisse', r))):
        x = pad + i * (w + pad)
        b.paste(Image.fromarray(a), (x, 34))
        d.text((x, 34 + h + 3), t, font=schrift(15, True), fill=(214, 138, 74))
    y = 34 + h + 26
    tabs = (0, 170, 300, 350, 400, 450, 500, 555, 650, 765)
    for sp, f in zl:
        for t, x in zip(sp, tabs):
            d.text((pad + x, y), t, font=schrift(14), fill=tuple(f))
        y += zeile
    b.save(ziel)


def befehl_teile(a):
    k = Kandidat(a.kandidat)
    if not k.j.get('teile'):
        raise SystemExit('Kandidat ohne Eintrag "teile"')
    T = teile_pruefen(k, a.aus)
    with open(os.path.join(a.aus, 'teile.json'), 'w', encoding='utf-8') as f:
        json.dump(T, f, ensure_ascii=False, indent=1)
    t = '\n'.join(teile_text(T)) + '\n'
    with open(os.path.join(a.aus, 'teile.txt'), 'w', encoding='utf-8') as f:
        f.write(t)
    print(t)


# ═══════════════════════════════════════════════════ Pruefmodell als Dummy
PALETTE = {'grundhaut_dunkel': 1, 'rotbraun_mittelton': 2, 'rosa_stelle': 3, 'helle_baender': 4, 'feine_linien': 5, 'untere_streifen': 6, 'weisskerne': 7, 'z9': 8}


def dummy_palette():
    M = formblatt()['zeichnung']['materialien']
    f = [(0, 0, 0)] * 9
    for n, i in PALETTE.items():
        f[i] = (85, 80, 60) if n == 'z9' else ((180, 165, 180) if n == 'untere_streifen' else tuple(M[n]['oberflaeche_rgb']))
    return f


ZEL = (96, 80)        # Zelle je Teil im Atlas


def _ellipsoid(a, nl=14, nb=10):
    """UV-Kugel mit Naht: Punkte, Normalen, Atlas-Lage je Punkt (0..1)."""
    p, n, t = [], [], []
    for j in range(nb + 1):
        th = math.pi * j / nb
        for i in range(nl + 1):
            ph = 2 * math.pi * i / nl
            e = np.array([math.sin(th) * math.cos(ph), -math.cos(th), math.sin(th) * math.sin(ph)])
            p.append(e * a)
            n.append(e / np.asarray(a, float))
            t.append((i / nl, j / nb))
    q = []
    w = nl + 1
    for j in range(nb):
        for i in range(nl):
            a0, b0, c0, d0 = j * w + i, j * w + i + 1, (j + 1) * w + i + 1, (j + 1) * w + i
            if j == 0:
                q.append((a0, c0, d0))
            elif j == nb - 1:
                q.append((a0, b0, d0))
            else:
                q.append((a0, b0, c0, d0))
    return np.array(p), np.array(n), np.array(t), q


def dummy_form():
    """Jedes Ellipsoid ein Knochen, Kuppel als Wurzel. Das Modell ist nur Messhilfe (formblatt.json pruefmodell), keine Figur."""
    J = formblatt()['pruefmodell']['teile']
    f = FE.neu()
    k = J[0]['mitte']
    FE.knochen_dazu(f, J[0]['name'], -1, (0, 0, 0))
    for t in J[1:]:
        FE.knochen_dazu(f, t['name'], 0, tuple(a - b for a, b in zip(t['mitte'], k)))
    for i, t in enumerate(J):
        h = [t['halbachsen'][0], t['halbachsen'][1], t['halbachsen'][2]]
        p, n, uv, q = _ellipsoid(h)
        x0, y0 = (i % 4) * ZEL[0], (i // 4) * ZEL[1]
        tex = [(x0 + 1 + (ZEL[0] - 3) * a, y0 + 1 + (ZEL[1] - 3) * b) for a, b in uv]
        FE.netz(f, i, p, q, tex, normalen=n)
    FE.orientiere(f)
    w = (k[0], k[1], k[2])
    for nm in ('pose1', 'pose2', 'ruhe'):
        FE.clip_dazu(f, nm, [FE.bild(f, wurzel=w)])
    return f        # Bewusst ohne boden_nachfuehren: rundling_nr reicht bis y=+118, das Formblatt rechnet das Modell in genau dieser Lage


def dummy_textur(backen):
    """Grundhaut. Gebacken: Sollflaechen aus beiden Kameras auf die Ellipsoide projiziert, damit Prüfung (d) auch den Treffer-Fall sieht."""
    J = formblatt()['pruefmodell']['teile']
    px = np.full((ZEL[1] * 3, ZEL[0] * 4), PALETTE['grundhaut_dunkel'], np.uint8)
    if backen:
        karte = {1: np.zeros((HOEHE, BREITE), np.uint8), 2: np.zeros((HOEHE, BREITE), np.uint8)}
        for s in stellen():
            for q in s['polygone']:
                cv2.fillPoly(karte[s['bild']], [np.round(np.array(q, float)).astype(np.int32)], PALETTE[s['material']])
        st = {1: (np.array(DUMMY_LAGE[1], float), DUMMY_GIER[1], FK.lade_kamera(1)), 2: (np.array(DUMMY_LAGE[2], float), DUMMY_GIER[2], FK.lade_kamera(2))}
        for i, t in enumerate(J):
            h = np.array(t['halbachsen'], float)
            m = np.array(t['mitte'], float)
            x0, y0 = (i % 4) * ZEL[0], (i // 4) * ZEL[1]
            for ty in range(ZEL[1] - 1):
                for tx in range(ZEL[0] - 1):
                    th = math.pi * ty / (ZEL[1] - 3)
                    ph = 2 * math.pi * tx / (ZEL[0] - 3)
                    e = np.array([math.sin(th) * math.cos(ph), -math.cos(th), math.sin(th) * math.sin(ph)])
                    for n in (1, 2):
                        lage, gier, kam = st[n]
                        s_, c_ = math.sin(gier * 2 * math.pi / 4096), math.cos(gier * 2 * math.pi / 4096)
                        Y = np.array([[c_, 0, s_], [0, 1, 0], [-s_, 0, c_]])
                        w = Y @ (m + e * h) + lage
                        nrm = Y @ (e / h)
                        if nrm @ (np.array(kam['auge'], float) - w) <= 0:
                            continue
                        b = FK.projiziere(kam, w)
                        if not np.isfinite(b).all():
                            continue
                        bx, by = int(round(b[0])), int(round(b[1]))
                        if 0 <= bx < BREITE and 0 <= by < HOEHE and karte[n][by, bx]:
                            px[y0 + ty + 1, x0 + tx + 1] = karte[n][by, bx]
    return FE.tim_schreiben(px, [FE.clut_aus_rgb(dummy_palette())])


DUMMY_LAGE = {1: (-18514, 0, -18726), 2: (2625, 0, -23625)}      # koerperrahmen, Abschnitt 3.2 Probe Sicht Flanke
DUMMY_GIER = {1: 97, 2: 3129}                                    # Abschnitt 3.1, 3.2: Ende F vorn


def befehl_dummy(a):
    os.makedirs(a.aus, exist_ok=True)
    f = dummy_form()
    nm = 'DUMMY_GEBACKEN' if a.gebacken else 'DUMMY'
    with open(os.path.join(a.aus, nm + '.EMD'), 'wb') as h:
        h.write(FE.schreibe_emd(f))
    with open(os.path.join(a.aus, nm + '.TIM'), 'wb') as h:
        h.write(dummy_textur(a.gebacken))
    with open(os.path.join(a.aus, nm + '.json'), 'w', encoding='utf-8') as h:
        json.dump(dict(name=nm, emd=nm + '.EMD', tim=nm + '.TIM', bild1=dict(lage=DUMMY_LAGE[1], gier=DUMMY_GIER[1]), bild2=dict(lage=DUMMY_LAGE[2], gier=DUMMY_GIER[2])), h, indent=1)
    print('geschrieben: ' + os.path.join(a.aus, nm + '.json'))


def licht_eichen():
    """Palette = Schirm / gemessener Faktor (farbe.md 5), Disc-Licht ergibt weniger. Damit eine palettentreue Textur im Vergleichsblatt die Helligkeit des
    Videos trifft, wird das Disc-Licht einmal geeicht, am Pruefmodell in der Lage aus Abschnitt 3. Fest fuer alle Kandidaten, nie je Kandidat."""
    M = formblatt()['zeichnung']['licht']
    emd = FE.schreibe_emd(dummy_form(), pruefen=False)
    tim = dummy_textur(False)
    mo = FE.modell(emd, tim)
    aus = {}
    for n in (1, 2):
        kam = kamera(n)
        c = licht_von_disc(n)
        kf = mo.frames[mo.clips[n - 1][0]] & 0xFFF
        weiss = np.zeros((HOEHE, BREITE, 3), np.uint8)
        tr = FB.dreiecke(mo, kf, DUMMY_LAGE[n], DUMMY_GIER[n], kam, c)
        # Faktor je Pixel ohne Textur: alle Texel neutral weiss
        tw = FB.Textur(tim)
        tw.lut = np.full_like(tw.lut, 255)
        m = FB.rastern(tr, tw, weiss, kam.abbildung)
        f = weiss[m].reshape(-1, 3).mean(0) / 255.0
        ziel = np.array(M['bild%d' % n], float)
        aus['bild%d' % n] = dict(disc_faktor=[round(float(x), 3) for x in f], gemessen=ziel.tolist(), k=[round(float(x), 3) for x in ziel / f])
    aus['hinweis'] = 'k = gemessener Faktor (farbe.md) / mittlerer Disc-Faktor am Pruefmodell. Gilt fuer alle Kandidaten gleich. licht="disc" schaltet ihn ab.'
    return aus


# ═══════════════════════════════════════════════════ Selbstprobe
def befehl_selbstprobe(_a=None):
    """Der Soll-Umriss selbst als Render: Was hier faellt, ist ein Widerspruch zwischen Abschnitt 7 und Umriss, nicht das Modell. Dann Versatz: bei welchem
    Fehler kippen die Punkte? Zeigt, ob die Grenzen scharf genug sind."""
    with open(daten('grenzen.json'), encoding='utf-8') as f:
        G = json.load(f)
    P = punkte_abschnitt7()
    for n in (1, 2):
        fig = cv2.imread(daten('figur_bild%d.png' % n), 0) > 0
        z = cv2.imread(os.path.join(FORM, 'ziel_bild%d.png' % n), 0)
        tp = [p for p in P if p['bild'] == n and p['zaehlt'] and not p['art'].startswith('mat')]
        print('--- Bild %d: Soll-Umriss als Render, %d Umrisspunkte' % (n, len(tp)))
        rest = raum_rest2()[0] if n == 2 else None
        kan = kanaele(original(n), rest)
        for dx, dy in ((0, 0), (3, 0), (-3, 0), (0, 3), (0, -3), (6, 0), (-6, 0), (0, 6), (0, -6), (10, 0), (-10, 0), (0, 10), (0, -10)):
            q = np.roll(np.roll(fig, dy, 0), dx, 1)
            r = [punkt_messen(p, G[p['id']], q, q) for p in tp]
            u = ueberdeckung(q, z)
            fl = [x for x in boden_und_verdeckung(n, q, z, kan) if x['zaehlt']]
            fehl = [x['id'] for x in r if not x['ok']] + [x['id'] for x in fl if not x['ok']]
            print('  Versatz %+3d,%+3d: %2d/%2d Punkte, %d/%d Flaechen, IoU %.3f, Schwarz %5d  %s'
                  % (dx, dy, sum(1 for x in r if x['ok']), len(r), sum(1 for x in fl if x['ok']), len(fl), u['iou'], u['schwarz_getroffen'], ' '.join(fehl) if (dx, dy) == (0, 0) else ''))


def soll_durch_filter(n):
    """Soll-Umriss in Grundhaut-Farbe auf dem Hintergrund, durch den Filter. Prueft Hintergrund und Filter zugleich: Aussen muss das Original herauskommen."""
    M = formblatt()['zeichnung']
    fig = cv2.imread(daten('figur_bild%d.png' % n), 0) > 0
    b = lade_rgb(daten('hintergrund_bild%d.png' % n))
    b[fig] = (np.array(M['materialien']['grundhaut_dunkel']['oberflaeche_rgb'], float) * np.array(M['licht']['bild%d' % n])).astype(np.uint8)
    return fig, FV.video_filter(b, bild=n, entzerren=False, rauschen=False)


def befehl_abschluss():
    """Kantenlage: gleiche Messung auf Original und gefiltertem Soll-Umriss. Das ist der Prueffall fuer die Annahme, der Umriss treffe die sichtbare Kante."""
    for n in (1, 2):
        fig, gef = soll_durch_filter(n)
        o = original(n)
        z = cv2.imread(os.path.join(FORM, 'ziel_bild%d.png' % n), 0)
        fern = ~weite(z > 0, 8) & ~rechteck(0, 430, 262, HOEHE - 1)
        ring = weite(z > 0, 28) & ~weite(z > 0, 8) & ~rechteck(0, 430, 262, HOEHE - 1)
        ya, yb = (gef.astype(np.float32) @ YK), (o.astype(np.float32) @ YK)
        print('Bild %d Hintergrund durch Filter gegen Original: fern MAE %.1f, Ring 8-28 px um die Figur MAE %.1f, mittlere Abweichung %+.1f'
              % (n, np.abs(ya - yb)[fern].mean(), np.abs(ya - yb)[ring].mean(), (ya - yb)[ring].mean()))
        d, dg = [], []
        for p in punkte_abschnitt7():
            if p['bild'] != n or p['art'].startswith('mat') or not p['zaehlt']:
                continue
            q = (p['x'], p['y'])
            kn = (0.0, 1.0) if 'spalte' in p['art'] else ((1.0, 0.0) if 'zeile' in p['art'] else normale_aus_bild(kanaele(o), q))
            a, b = kante(ya, q, kn), kante(yb, q, kn)
            if a and b and min(a['hub'], b['hub']) >= 2 * MIN_HUB and a['breite'] <= 9 and b['breite'] <= 9:
                d.append(a['versatz'] - b['versatz'])
                # Kante im sauberen Soll-Umriss gegen dieselbe Kante nach dem Filter: Verschiebung durch Blur und Gamma
                m = kante(fig.astype(np.float32) * 255, q, kn)
                if m:
                    dg.append((a['versatz'] - m['versatz']) * (1 if m['steigt'] else -1))
        if d:
            print('  harte Kanten, %d Stellen: Filter gegen Original Median %+.1f, |Betrag| Median %.1f, max %.1f px' % (len(d), np.median(d), np.median(np.abs(d)), np.max(np.abs(d))))
        if dg:
            print('  Verschiebung durch Filter gegen saubere Kante, + = zur hellen Seite: Median %+.1f, Spanne %+.1f..%+.1f px' % (np.median(dg), np.min(dg), np.max(dg)))


# ═══════════════════════════════════════════════════ Probe der Teile-Messung
# Tiefen nur fuer den Probe-Dummy, keine Messung: Beine vor dem Schwanz, Koerper und Arm hinten, damit Verdeckung vorkommt.
PROBE_TIEFE = {'tail': 5200, 'left_leg': 4700, 'right_leg': 4800, 'part_of_Body': 6300, 'right_arm': 6800, 'face_and_upper_body': 6500, 'left_arm_front': 5600, 'right_arm_front': 5000}
PROBE_INSEL = 20000     # Die Teile von Bild 2 liegen im Modell so weit abseits, dass jede Kamera nur ihre Teile sieht


def probe_glied(name):
    """Hauptachse der Maske in die Ebene der Probetiefe zurueckgerechnet: drei Stuetzpunkte laengs, Halbbreite in Einheiten."""
    n = teile_lage()[name]['bild']
    f = FK.lade_kamera(n)
    a = achse(teil_maske(name))
    h = a['laenge'] / 2
    auge, d = FK.strahl(f, np.array([a['mitte'] - a['e'] * h, a['mitte'], a['mitte'] + a['e'] * h]))
    p = auge + (PROBE_TIEFE[name] / (d @ f['R'][2]))[:, None] * d
    return dict(bild=n, p=p, halb=a['breite'] / 2 * np.linalg.norm(p[2] - p[0]) / (2 * h))


def probe_dummy(aus):
    """Kleiner Dummy: je Nutzer-Teil zwei Ellipsoide = zwei Knochen auf der Hauptachse der Maske. Legt die wahren Enden je Teil in Welt-Koordinaten mit ab."""
    os.makedirs(aus, exist_ok=True)
    f = FE.neu()
    FE.knochen_dazu(f, 'wurzel', -1, (0, 0, 0))
    gl = {t: probe_glied(t) for t in teile_lage()}
    urs, teile, enden_welt = {}, {}, {}
    for n in (1, 2):
        p = np.vstack([gl[t]['p'] for t in teile_von(n)])
        urs[n] = np.round([p[:, 0].mean(), 0.0, p[:, 2].mean()])
    insel = {1: np.zeros(3), 2: np.array([0.0, 0.0, float(PROBE_INSEL)])}
    for t, g in gl.items():
        n = g['bild']
        teile[t] = []
        for i, (a, b) in enumerate(((g['p'][0], g['p'][1]), (g['p'][1], g['p'][2]))):
            e = (b - a) / np.linalg.norm(b - a)
            u = np.cross(e, (0.0, 0.0, 1.0) if abs(e[2]) < 0.9 else (1.0, 0.0, 0.0))
            u /= np.linalg.norm(u)
            R = np.c_[u, e, np.cross(e, u)]                 # Pole des Ellipsoids auf seiner y-Achse
            lang = np.linalg.norm(b - a) * 0.56
            pu, nr, uv, q = _ellipsoid((g['halb'], lang, g['halb']), 8, 6)
            c = (a + b) / 2
            j = FE.knochen_dazu(f, '%s_%d' % (t, i), 0, tuple(int(round(x)) for x in c - urs[n] + insel[n]))
            FE.netz(f, j, pu @ R.T, q, [(1 + 28 * s, 1 + 28 * r) for s, r in uv], normalen=nr @ R.T)
            teile[t].append(j)
            if i == 0:
                enden_welt[t] = [(c - e * lang).tolist()]
            else:
                enden_welt[t].append((c + e * lang).tolist())
    for nm in ('pose1', 'pose2', 'ruhe'):
        FE.clip_dazu(f, nm, [FE.bild(f)])
    FE.orientiere(f)
    with open(os.path.join(aus, 'PROBE.EMD'), 'wb') as h:
        h.write(FE.schreibe_emd(f, pruefen=False))
    with open(os.path.join(aus, 'PROBE.TIM'), 'wb') as h:
        h.write(FE.tim_schreiben(np.ones((32, 32), np.uint8), [FE.clut_aus_rgb([(0, 0, 0), (120, 95, 105)])]))
    j = dict(name='PROBE', emd='PROBE.EMD', tim='PROBE.TIM', licht='disc', bild1=dict(lage=[int(x) for x in urs[1]], gier=0), bild2=dict(lage=[int(x) for x in urs[2] - insel[2]], gier=0), teile=teile)
    pfad = os.path.join(aus, 'PROBE.json')
    with open(pfad, 'w', encoding='utf-8') as h:
        json.dump(j, h, indent=1)
    return pfad, gl, enden_welt


def tb_aus(V, A=None):
    """Teilbild ohne Render fuer die Probe: V = {Teil: sichtbare Pixel}, A = {Teil: Teil allein}, Vorgabe A = V. Frueher genannte Teile liegen vorn."""
    namen = list(V)
    karte = np.full((HOEHE, BREITE), -1, np.int32)
    for i, t in enumerate(namen):
        karte[V[t] & (karte < 0)] = i
    return dict(namen=namen, karte=karte, allein=dict(A or V), knochen={t: [i] for i, t in enumerate(namen)})


def nutzer_union(n):
    u = np.zeros((HOEHE, BREITE), bool)
    for t in teile_von(n):
        u |= teil_roh(t)
    return u


def handschnitt_messung():
    """Abstand der Nutzerumrisse zu den gemessenen Umrisspunkten aus Abschnitt 7. Grundlage fuer TEIL_SAUM_PX."""
    aus = []
    for n in (1, 2):
        u = nutzer_union(n)
        di, da = cv2.distanceTransform(u.astype(np.uint8), cv2.DIST_L2, 5), cv2.distanceTransform((~u).astype(np.uint8), cv2.DIST_L2, 5)
        for p in punkte_abschnitt7():
            if p['bild'] == n and not p['art'].startswith('mat') and p['zaehlt']:
                y, x = int(round(p['y'])), int(round(p['x']))
                aus.append((float(di[y, x] if u[y, x] else da[y, x]) - 0.5, p['id']))
    return aus


def befehl_teileprobe():
    aus = daten('teile')
    os.makedirs(aus, exist_ok=True)
    L = []

    def sag(t=''):
        print(t)
        L.append(t)
    logo = logo_maske()
    y1, y2 = original(1).astype(np.float32) @ YK, original(2).astype(np.float32) @ YK
    zone = rechteck(0, 434, 255, HOEHE - 1)
    sag('--- Logo: %d Pixel. Bild 1 gegen Bild 2, mittlerer Y-Abstand in der Schrift %.1f, in der Zone sonst %.1f' % (logo.sum(), np.abs(y1 - y2)[logo].mean(), np.abs(y1 - y2)[zone & ~logo].mean()))

    h = handschnitt_messung()
    w = np.array([x[0] for x in h])
    sag('--- Handschnitt gegen gemessene Kanten: %d Punkte, Median %.1f, 80 %% %.1f, 90 %% %.1f, groesster %.1f px; ueber 8 px: %s'
        % (len(w), np.median(w), np.percentile(w, 80), np.percentile(w, 90), w.max(), ', '.join('%s %.1f' % (i, v) for v, i in h if v > 8) or '-'))

    sag('--- Maske als Render: alle Werte muessen 1 oder 0 sein')
    for t in teile_lage():
        m = teil_roh(t)
        z = teil_messen(t, tb_aus({t: m}))
        sag('  %-20s IoU %.3f Deck %.3f Kern %.3f Innen %.3f +Saum %.3f Verd %.3f Leer %.3f SP %.1f Achse %.1f Grad %+.1f px Enden %s Logo aus Maske %d'
            % (t, z['iou'], z['deckung'], z['kern_deckung'], z['innen'], z['innen_saum'], z['verdeckt'], z['leer'], z['schwerpunkt']['abstand'], z['achse']['abweichung_grad'], z['achse']['quer_px'],
               [e['abstand'] for e in z['enden']], z['logo_px']))

    sag('--- Versatz +7,-4 und Drehung 20 Grad um den Schwerpunkt: Soll SP +7,-4 und Abweichung 20 Grad bei quer 0')
    for t in ('part_of_Body', 'right_arm', 'left_arm_front', 'face_and_upper_body'):
        m = teil_roh(t)
        z = teil_messen(t, tb_aus({t: np.roll(np.roll(m, -4, 0), 7, 1)}))
        a = achse(m)
        M = cv2.getRotationMatrix2D((float(a['mitte'][0]), float(a['mitte'][1])), 20.0, 1.0)
        r = cv2.warpAffine(m.astype(np.uint8) * 255, M, (BREITE, HOEHE), flags=cv2.INTER_NEAREST) > 127
        q = teil_messen(t, tb_aus({t: r}))
        sag('  %-20s SP %+.1f,%+.1f  IoU %.3f   Drehung: Abweichung %.1f Grad, quer %+.1f px, Streckung %.2f' % (t, z['schwerpunkt']['dx'], z['schwerpunkt']['dy'], z['iou'], q['achse']['abweichung_grad'], q['achse']['quer_px'], a['streckung']))

    sag('--- Verdeckung: tail allein = Maske, Streifen y 250-300 von right_leg verdeckt')
    t = teil_roh('tail')
    streifen = rechteck(0, 250, BREITE - 1, 300)
    z = teil_messen('tail', tb_aus({'right_leg': streifen & t, 'tail': t}, {'tail': t}))
    sag('  Soll verdeckt %.3f, gemessen %.3f, von %s; Fremd in der Maske %s' % ((t & streifen).sum() / t.sum(), z['verdeckt'], z['verdeckt_von'], z['fremd']))

    sag('--- Ueberlappung der Schnitte: part_of_Body als Render, im Ueberlapp steht right_leg. Soll: nichts fehlt')
    a, b = teil_roh('part_of_Body'), teil_roh('right_leg')
    z = teil_messen('part_of_Body', tb_aus({'right_leg': a & b, 'part_of_Body': a}, {'part_of_Body': a}))
    q = teil_messen('part_of_Body', tb_aus({'tail': a & b, 'part_of_Body': a}, {'part_of_Body': a}))
    sag('  Maske %d px, Ueberlapp %d: mit right_leg davor IoU %.3f Deck %.3f; mit tail davor IoU %.3f Deck %.3f, das muss fehlen' % (z['maske_px'], (a & b).sum(), z['iou'], z['deckung'], q['iou'], q['deckung']))

    sag('--- Logo zaehlt nicht: right_arm_front mit und ohne Schrift als Render')
    m = teil_roh('right_arm_front')
    for nm, v in (('Maske ohne Schrift', m & ~logo), ('Maske plus ganze Schrift', m | logo)):
        z = teil_messen('right_arm_front', tb_aus({'right_arm_front': v}))
        sag('  %-26s IoU %.3f Deck %.3f Innen %.3f' % (nm, z['iou'], z['deckung'], z['innen']))
    sag('  Schrift in den Masken: ' + ', '.join('%s %d' % (t, (teil_roh(t) & logo).sum()) for t in teile_lage() if (teil_roh(t) & logo).any()))

    sag('--- Enden und Spitzen der Masken')
    for t in teile_lage():
        q, o, u, g = enden(teil_maske(t))
        sp = '' if t in OHNE_SPITZEN else ' Spitzen ' + ' '.join('(%d,%d)%s' % (s[1], s[0], '' if s[2] == float('inf') else ' +%.0f' % s[2]) for s in spitzen(groesste(teil_roh(t)), o))
        sag('  %-20s %s (%d,%d)  %s (%d,%d)  Weg %.0f%s' % (t, ENDNAME[t][0], o[1], o[0], ENDNAME[t][1], u[1], u[0], g, sp))

    sag('--- Zeichnung und Verdeckungen in den Teilen: Anteil der Stelle in der Maske')
    for s_ in stellen():
        p = np.zeros((HOEHE, BREITE), bool)
        for q in s_['polygone']:
            p |= poly_maske(q)
        w = {t: round(float((p & teil_roh(t)).sum()) / max(int(p.sum()), 1), 2) for t in teile_von(s_['bild'])}
        sag('  %-4s Bild %d %s' % (s_['id'], s_['bild'], {t: v for t, v in w.items() if v > 0} or 'in keinem Teil'))
    for v in formblatt()['verdeckungen_bild2']:
        if v['id'] != 'BO':
            a, b = np.array(v['stelle'], float)
            q = a + np.linspace(0, 1, int(np.linalg.norm(b - a)) + 1)[:, None] * (b - a)
            yy, xx = np.round(q[:, 1]).astype(int), np.round(q[:, 0]).astype(int)
            w = {t: round(float(teil_roh(t)[yy, xx].mean()), 2) for t in teile_von(2)}
            sag('  %-4s Bild 2 %s' % (v['id'], {t: x for t, x in w.items() if x > 0} or 'in keinem Teil'))

    sag('--- Nutzer-Soll als Render gegen ziel2 und die verbliebenen Pruefungen')
    G = _json(daten('grenzen.json'))
    rest = raum_rest2()[0]
    for n in (1, 2):
        z = ziel_laden(n)[1]
        u = nutzer_union(n)
        q = u & ~logo
        v = ueberdeckung(q, z)
        pk = [punkt_messen(p, G[p['id']], u, u) for p in punkte_abschnitt7() if p['bild'] == n and p['zaehlt'] and not p['art'].startswith('mat')]
        fl = [x for x in boden_und_verdeckung(n, u, z, kanaele(original(n), rest if n == 2 else None)) if x['zaehlt']]
        sag('  Bild %d: IoU %.3f, Weiss gedeckt %.3f, Schwarz %d; Punkte %d/%d, nein: %s; Flaechen %d/%d, nein: %s'
            % (n, v['iou'], v['weiss_gedeckt'], v['schwarz_getroffen'], sum(x['ok'] for x in pk), len(pk), ' '.join('%s(%+.1f/%.1f)' % (x['id'], x['abstand'] if x['abstand'] is not None else 99, x['grenze']) for x in pk if not x['ok']) or '-',
               sum(x['ok'] for x in fl), len(fl), ' '.join(x['id'] for x in fl if not x['ok']) or '-'))

    sag('--- Dummy: je Teil zwei Ellipsoide auf der Hauptachse der Maske, Tiefen frei gewaehlt')
    d = os.path.join(aus, 'dummy')
    pf, gl, ew = probe_dummy(d)
    k = Kandidat(pf)
    sag('  %d Knochen, %d Flaechen' % (knochen_zahl(k), sum(len(m['dreiecke']) + len(m['vierecke']) for m in FE.lese_emd(k.emd)['meshes'])))
    tbs = {}
    for n in (1, 2):
        zu, warn = teil_zuordnung(k, n)
        tb = tbs[n] = teilbild(k, n, zu)
        m = k.rendern(n)[2]
        fremd = [t for t in tb['namen'] if t not in teile_von(n) and t != 'Rest' and (tb['karte'] == tb['namen'].index(t)).any()]
        sag('  Bild %d: Kennung gegen Kandidat.rendern, abweichende Pixel %d von %d; Warnungen %s; Teile des anderen Bildes im Bild: %s' % (n, ((tb['kn'] >= 0) != m).sum(), m.sum(), warn, fremd or '-'))
        f = FK.lade_kamera(n)
        for t in teile_von(n):
            z = teil_messen(t, tb)
            b = FK.projiziere(f, np.array(ew[t]))
            if b[0, 1] > b[1, 1]:
                b = b[::-1]
            sag('    %-20s IoU %.2f SP %+.0f,%+.0f Achse %.0f Grad; wahres Ende oben (%.0f,%.0f) gefunden %s, unten (%.0f,%.0f) gefunden %s'
                % (t, z['iou'], z['schwerpunkt']['dx'], z['schwerpunkt']['dy'], z['achse']['abweichung_grad'], b[0, 0], b[0, 1], z['enden'][0]['ist'], b[1, 0], b[1, 1], z['enden'][1]['ist']))

    A = tbs[1]['allein']
    soll = (A['part_of_Body'] & A['right_leg']).sum() / A['part_of_Body'].sum()
    z = teil_messen('part_of_Body', tbs[1])
    r = teil_messen('right_leg', tbs[1])
    sag('  Verdeckung im Render: part_of_Body hinter right_leg, Soll %.3f, gemessen %s; right_leg selbst verdeckt von %s' % (soll, z['verdeckt_von'], r['verdeckt_von']))

    T0 = teile_pruefen(k, os.path.join(d, 'lauf'))
    T1 = teile_pruefen(Kandidat(pf), os.path.join(d, 'lauf'))
    sag('  Wiederholung: zweiter Lauf %s' % ('gleich' if json.dumps(T0, sort_keys=True) == json.dumps(T1, sort_keys=True) else 'VERSCHIEDEN'))

    with open(pf, encoding='utf-8') as f:
        j = json.load(f)
    tausch = json.loads(json.dumps(j))
    tausch['teile']['left_leg'], tausch['teile']['right_leg'] = tausch['teile']['right_leg'], tausch['teile']['left_leg']
    weg = os.path.join(d, 'PROBE_TAUSCH.json')
    with open(weg, 'w', encoding='utf-8') as f:
        json.dump(tausch, f, indent=1)
    Tt = teile_pruefen(Kandidat(weg), os.path.join(d, 'lauf_tausch'))
    gl1 = {x['teil']: x for x in T0['bild1']['teile']}
    gl2 = {x['teil']: x for x in Tt['bild1']['teile']}
    sag('  Tausch left_leg gegen right_leg: IoU ' + ', '.join('%s %.2f -> %.2f' % (t, gl1[t]['iou'], gl2[t]['iou']) for t in gl1))

    sch = json.loads(json.dumps(j))
    sch['bild1']['lage'][0] += 300
    sch['bild1']['lage'][2] -= 200
    weg = os.path.join(d, 'PROBE_SCHUB.json')
    with open(weg, 'w', encoding='utf-8') as f:
        json.dump(sch, f, indent=1)
    Ts = teile_pruefen(Kandidat(weg), os.path.join(d, 'lauf_schub'))
    f1 = FK.lade_kamera(1)
    for x, y in zip(T0['bild1']['teile'], Ts['bild1']['teile']):
        w = np.mean(gl[x['teil']]['p'], axis=0)
        e = FK.projiziere(f1, w + (300.0, 0.0, -200.0)) - FK.projiziere(f1, w)
        sag('  Schub 300,-200 %-14s Schwerpunkt verschoben %+.1f,%+.1f, projiziert %+.1f,%+.1f' % (x['teil'], y['schwerpunkt']['ist'][0] - x['schwerpunkt']['ist'][0], y['schwerpunkt']['ist'][1] - x['schwerpunkt']['ist'][1], e[0], e[1]))

    pruefe_kandidat(pf, os.path.join(d, 'lauf'))
    with open(os.path.join(d, 'lauf', 'bericht.json'), encoding='utf-8') as f:
        J = json.load(f)
    sag('  pruefe durchgelaufen: Ziel %s, %s; Blaetter in pruefung/teile/dummy/lauf' % (J['bild1']['ziel'], J['bild2']['ziel']))
    for x in ('PROBE_TAUSCH.json', 'PROBE_SCHUB.json'):
        os.remove(os.path.join(d, x))
    for x in ('lauf_tausch', 'lauf_schub'):
        shutil.rmtree(os.path.join(d, x))
    with open(os.path.join(aus, 'selbstprobe.txt'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(L) + '\n')


def befehl_vergleiche(a):
    print('%-22s %7s %7s %8s %7s %8s %7s %7s %8s %7s %6s' % ('Kandidat', 'IoU1', 'Weiss1', 'Punkte1', 'Flaech1', 'Zeich1', 'IoU2', 'Weiss2', 'Punkte2', 'Flaech2', 'Zeich2'))
    for d in a.ordner:
        with open(os.path.join(d, 'bericht.json'), encoding='utf-8') as f:
            R = json.load(f)
        z = []
        for n in (1, 2):
            B = R['bild%d' % n]
            z += ['%.3f' % B['umriss']['iou'], '%.3f' % B['umriss']['weiss_gedeckt']]
            z += ['%d/%d' % zaehle(B[k]) for k in ('punkte', 'flaechen', 'zeichnung')]
        print('%-22s %7s %7s %8s %7s %8s %7s %7s %8s %7s %6s' % (R['name'][:22], *z))
        if R.get('teile'):
            print('  Teile IoU: ' + '  '.join('%s %s' % (t['teil'], '-' if t.get('iou') is None else '%.2f' % t['iou']) for n in (1, 2) for t in R['teile']['bild%d' % n]['teile']))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0], formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    sp = ap.add_subparsers(dest='befehl', required=True)
    sp.add_parser('ziele')
    a = sp.add_parser('ziele2')
    a.add_argument('--logo-neu', action='store_true', help='Logo-Maske neu aus dem Video messen')
    sp.add_parser('selbstprobe')
    sp.add_parser('teileprobe')
    a = sp.add_parser('pruefe')
    a.add_argument('kandidat')
    a.add_argument('--aus', required=True)
    a = sp.add_parser('teile')
    a.add_argument('kandidat')
    a.add_argument('--aus', required=True)
    a = sp.add_parser('dummy')
    a.add_argument('--aus', required=True)
    a.add_argument('--gebacken', action='store_true')
    a = sp.add_parser('vergleiche')
    a.add_argument('ordner', nargs='+')
    arg = ap.parse_args()
    if arg.befehl == 'ziele':
        befehl_ziele()
    elif arg.befehl == 'ziele2':
        befehl_ziele2(arg)
    elif arg.befehl == 'teile':
        befehl_teile(arg)
    elif arg.befehl == 'teileprobe':
        befehl_teileprobe()
    elif arg.befehl == 'selbstprobe':
        befehl_selbstprobe()
        befehl_abschluss()
    elif arg.befehl == 'dummy':
        befehl_dummy(arg)
    elif arg.befehl == 'vergleiche':
        befehl_vergleiche(arg)
    else:
        _r, t = pruefe_kandidat(arg.kandidat, arg.aus)
        print(t)
    return 0


if __name__ == '__main__':
    sys.exit(main())
