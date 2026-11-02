# -*- coding: utf-8 -*-
"""Bausteine des Panel-Bilds ROOM2030 (Runde 41 Spur N, Nachtrag 37, Entwurf A "Druckspeicher"): entzerrter Echt-Ausschnitt des Panels aus
ROOM2030 Cut 1/2 + Rahmen aus ROOM2050 Cut 15 + Cursor aus dem Spielbild. Alle Masse in Schirmpixeln (320x240); gezeichnet wird 4-fach
ueberabgetastet. Dossier analysis/befunde_runde41/N_nachtrag30.md §2. Eingabe: Bild-Dumps von tests/unit/probe_bg_dump.c (Verzeichnis QUELLE)."""
import os, sys, io
import numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont, ImageFilter

HIER = os.path.dirname(os.path.abspath(__file__))
TREE = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
QUELLE = [os.environ.get("R41N_BG", ".")]     # Verzeichnisse mit ROOM203xx.ppm, ROOM20515.ppm, ROOM11F10.ppm
SS = 4
W, H = 320, 240
F_BD = r'C:/Windows/Fonts/arialbd.ttf'
F_RG = r'C:/Windows/Fonts/arial.ttf'


def bg(name):
    for d in QUELLE:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return p
    raise SystemExit("Bild-Dump fehlt: %s (R41N_BG = Verzeichnis der probe_bg_dump-Ausgabe, mehrere mit ';')" % name)


# --- Kamera-Abbildung (ROOM2030.RDT @0x80 Cut 1, @0xA0 Cut 2; H = 26684 >> 7 = 208; Tafelebene z -10550 trianguliert) ---
CAMS = {1: ((-3858, -8101, -3691), (11593, -5157, -12615)), 2: ((3883, -8101, -7091), (-11568, -5157, -9778))}
HS = 26684 >> 7
ZW = -10550.0
S, XC, Y0 = 0.14, -116.0, -9040.0   # Schirmpixel je Welteinheit, Welt-x der Bildmitte, Welt-y der Oberkante der Oeffnung


def _basis(c):
    p, t = CAMS[c]; p = np.array(p, float); t = np.array(t, float)
    f = t - p; f /= np.linalg.norm(f)
    r = np.cross([0, 1, 0], f); r /= np.linalg.norm(r)
    d = np.cross(f, r)
    return p, r, d, f


def _warp(c):
    u, v = np.meshgrid((np.arange(W * SS) + .5) / SS, (np.arange(H * SS) + .5) / SS)
    X_ = XC - (u - 160) / S; Y_ = Y0 + (v - 20) / S
    P = np.stack([X_, Y_, np.full_like(X_, ZW)], -1)
    p, r, d, f = _basis(c)
    Q = P - p
    z = Q @ f
    mx = (160 + HS * (Q @ r) / z).astype(np.float32); my = (120 + HS * (Q @ d) / z).astype(np.float32)
    im = np.array(Image.open(bg('ROOM203%02d.ppm' % c)).convert('RGB'))
    ok = (mx >= 0) & (mx <= 319) & (my >= 0) & (my <= 239)
    return cv2.remap(im, mx - .5, my - .5, cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE).astype(np.float32), ok


def echtbild():
    """Tafel aus Cut 1 (links, deckt T1 und die Wand) und Cut 2 (rechts, schaerfer), weiche Naht im freien Blech."""
    a, oa = _warp(1); b, ob = _warp(2)
    box = (102 * SS, 40 * SS, 118 * SS, 62 * SS)
    r1 = a[box[1]:box[3], box[0]:box[2]].reshape(-1, 3).mean(0); r2 = b[box[1]:box[3], box[0]:box[2]].reshape(-1, 3).mean(0)
    a = a * (r2 / r1)
    u = (np.arange(W * SS) + .5) / SS
    t = np.clip((u - 94) / 12, 0, 1)[None, :] * np.ones((H * SS, 1))
    t = cv2.GaussianBlur(np.where(ob, t, 0.0).astype(np.float32), (0, 0), 6)
    return (a * (1 - t[..., None]) + b * t[..., None]).clip(0, 255)


# --- Koordinaten + Primitive ------------------------------------------------------------------------------------------
_y, _x = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32)
X = (_x + .5) / SS; Y = (_y + .5) / SS
RNG = np.random.default_rng(2030)


def col(*c): return np.array(c, np.float32)
def aa(sd, soft=.5): return np.clip(.5 - sd / soft, 0, 1)


def blend(img, c, a):
    a = np.clip(a, 0, 1)
    img[:] = img * (1 - a[..., None]) + c * a[..., None]


def sd_kreis(cx, cy, r): return np.hypot(X - cx, Y - cy) - r


def sd_box(x0, y0, x1, y1, r=0.):
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    qx = np.abs(X - cx) - ((x1 - x0) / 2 - r); qy = np.abs(Y - cy) - ((y1 - y0) / 2 - r)
    return np.hypot(np.maximum(qx, 0), np.maximum(qy, 0)) + np.minimum(np.maximum(qx, qy), 0) - r


def sd_seg(x0, y0, x1, y1, r):
    px, py = X - x0, Y - y0
    bx, by = x1 - x0, y1 - y0
    h = np.clip((px * bx + py * by) / (bx * bx + by * by), 0, 1)
    return np.hypot(px - bx * h, py - by * h) - r


def rausch(sigma_px, seed=None):
    """Rauschen 0-mittig, Streuung 1, Korngroesse sigma_px Schirmpixel."""
    r = RNG if seed is None else np.random.default_rng(seed)
    n = r.standard_normal((H * SS, W * SS)).astype(np.float32)
    if sigma_px > 0:
        n = cv2.GaussianBlur(n, (0, 0), sigma_px * SS)
    return n / (n.std() + 1e-6)


def weich(m, sigma_px): return cv2.GaussianBlur(m.astype(np.float32), (0, 0), sigma_px * SS)


def text(img, s, x, y, size, c, alpha=1., font=F_BD, anchor='mm', blur=.25):
    m = Image.new('L', (W * SS, H * SS), 0)
    f = ImageFont.truetype(font, int(round(size * SS)))
    ImageDraw.Draw(m).text((x * SS, y * SS), s, font=f, fill=255, anchor=anchor)
    a = np.asarray(m, np.float32) / 255
    if blur: a = weich(a, blur)
    blend(img, c, a * alpha)


def schatten(img, sd, stark=.55, sigma=1.4, dx=1.0, dy=1.3):
    m = np.roll(np.roll(aa(sd, 1.), int(dy * SS), 0), int(dx * SS), 1)
    img *= (1 - stark * weich(m, sigma))[..., None]


# --- Elemente ----------------------------------------------------------------------------------------------------------
def tafel_licht(img):
    """Nahaufnahme ist heller ausgeleuchtet als der Raum (Massstab 2050 Cut 15): Gamma + Verstaerkung, dann Nachschaerfen."""
    a = 255 * np.power(img / 255, .78) * 1.18
    gross = cv2.GaussianBlur(a, (0, 0), 2.0 * SS)
    a = a + (a - gross) * .9
    return a.clip(0, 255)


def korn_blech(img, maske):
    """Blech-Kleinstruktur: Flecken, Kratzer, feines Korn (die Quelle hat nur 3 px je 100 Einheiten)."""
    fl = rausch(9) * 4.5 + rausch(2.2) * 3.5 + rausch(.5) * 3.2 + rausch(.2) * 2.2
    img += (fl * maske)[..., None]
    sk = np.zeros((H * SS, W * SS), np.uint8)
    r = np.random.default_rng(77)
    for _ in range(170):
        x0, y0 = r.integers(0, W * SS), r.integers(0, H * SS)
        l, w = r.uniform(10, 90), r.uniform(-.35, .35) + (np.pi / 2 if r.random() < .75 else 0)
        cv2.line(sk, (int(x0), int(y0)), (int(x0 + l * np.cos(w)), int(y0 + l * np.sin(w))), int(r.integers(60, 200)), 1, cv2.LINE_AA)
    sk = cv2.GaussianBlur(sk.astype(np.float32) / 255, (0, 0), 1.)
    img += (sk * 26 * maske)[..., None]


def skala_winkel(v): return np.radians(-135 + 270 * v / 100)  # 0 unten links ... 100 unten rechts, im Uhrzeigersinn


def strich(img, cx, cy, phi, r0, r1, b, c, alpha=1.):
    s, k = np.sin(phi), np.cos(phi)
    blend(img, c, aa(sd_seg(cx + r0 * s, cy - r0 * k, cx + r1 * s, cy - r1 * k, b), .45) * alpha)


def scheibe(img, cx, cy, R, ring=3.9):
    """Rundes Instrument: Lippe, Metallring, Ziffernblatt (Aufbau wie die Scheiben im Echtbild)."""
    d = np.hypot(X - cx, Y - cy)
    phi = np.arctan2(Y - cy, X - cx)
    schatten(img, sd_kreis(cx, cy, R + ring), .6, 1.5, 1.1, 1.5)
    blend(img, col(6, 6, 6), aa(sd_kreis(cx, cy, R + ring + .5), .8))
    licht = .5 + .5 * np.cos(phi + np.radians(125))          # Licht von oben links
    ringf = (46 + 150 * licht ** 2.4)[..., None] * col(1., 1., .96) + (rausch(.6, 5) * 5)[..., None]
    a = aa(sd_kreis(cx, cy, R + ring - .4), .6)
    img[:] = img * (1 - a[..., None]) + ringf * a[..., None]
    blend(img, col(10, 10, 9), aa(sd_kreis(cx, cy, R + .7), .6))
    u = (d / R)
    grund = col(219, 214, 190)
    verlauf = (1.02 - .22 * u ** 3 + .06 * ((cx - X) + (cy - Y)) / R)[..., None]
    schmutz = (rausch(1.2, 9) * 7 + rausch(.3, 10) * 3.2)[..., None]
    f = grund * verlauf + schmutz
    a = aa(sd_kreis(cx, cy, R), .5)
    img[:] = img * (1 - a[..., None]) + f * a[..., None]


def glas(img, cx, cy, R):
    d1 = sd_kreis(cx - R * .28, cy - R * .34, R * .82)
    d2 = sd_kreis(cx - R * .02, cy - R * .02, R * .90)
    a = aa(d1, 1.) * (1 - aa(d2, 2.2)) * aa(sd_kreis(cx, cy, R - .4), .5)
    blend(img, col(255, 255, 248), a * .34)


def instrument(img, cx, cy, R, wert):
    """Hauptanzeige: Skala 0..100 wie die Skala 2050/11F0, 80 rot, rotes Band 80..100."""
    scheibe(img, cx, cy, R)
    tinte = col(30, 28, 26)
    for v in range(0, 101, 10):
        ph = skala_winkel(v)
        lang = v % 20 == 0
        strich(img, cx, cy, ph, R * (.70 if lang else .78), R * .92, .36 if lang else .24, tinte, .9)
    for v in np.arange(80, 100.01, 1.):
        ph = skala_winkel(v)
        strich(img, cx, cy, ph, R * .76, R * .92, .42, col(178, 28, 22), .95)
    rot = col(172, 22, 18)
    for v, s, c, gr in ((0, '0', tinte, 4.6), (50, '50', tinte, 4.6), (80, '80', rot, 6.0)):
        ph = skala_winkel(v)
        rr = R * (.45 if v == 80 else .50)
        text(img, s, cx + rr * np.sin(ph), cy - rr * np.cos(ph) + (0 if v != 50 else .4), gr, c, .96, F_BD, blur=.12)
    ph = skala_winkel(wert)
    # Zeiger: Schatten, Spitze, Stummel, Nabe
    s, k = np.sin(ph), np.cos(ph)
    blend(img, col(0, 0, 0), weich(aa(sd_seg(cx - R * .22 * s + .8, cy + R * .22 * k + 1., cx + R * .86 * s + .8, cy - R * .86 * k + 1., .5), .5), .4) * .34)
    blend(img, col(16, 15, 15), aa(sd_seg(cx - R * .24 * s, cy + R * .24 * k, cx + R * .58 * s, cy - R * .58 * k, .62), .4))
    blend(img, col(16, 15, 15), aa(sd_seg(cx + R * .50 * s, cy - R * .50 * k, cx + R * .88 * s, cy - R * .88 * k, .34), .35))
    a = aa(sd_kreis(cx, cy, 2.3), .5)
    nabe = (40 + 120 * np.clip(.5 + .5 * ((cx - X) + (cy - Y)) / 2.6, 0, 1))[..., None] * col(1, 1, 1)
    img[:] = img * (1 - a[..., None]) + nabe * a[..., None]
    glas(img, cx, cy, R)


def drehscheibe(img, cx, cy, R, grad):
    """Scheibe mit Querbalken wie im Echtbild (weiss, schwarzer Balken ueber den ganzen Durchmesser)."""
    scheibe(img, cx, cy, R)
    tinte = col(30, 28, 26)
    for k in range(12):
        strich(img, cx, cy, np.radians(30 * k), R * .84, R * .95, .22, tinte, .6)
    ph = np.radians(grad)
    s, c = np.sin(ph), np.cos(ph)
    x0, y0, x1, y1 = cx - R * .93 * s, cy + R * .93 * c, cx + R * .93 * s, cy - R * .93 * c
    blend(img, col(0, 0, 0), weich(aa(sd_seg(x0 + .8, y0 + 1., x1 + .8, y1 + 1., 1.25), .5), .45) * .32)
    blend(img, col(14, 14, 13), aa(sd_seg(x0, y0, x1, y1, 1.15), .4))
    a = aa(sd_seg(x0 - .35 * c, y0 - .35 * s, x1 - .35 * c, y1 - .35 * s, .22), .4)
    blend(img, col(120, 120, 116), a * .5)
    glas(img, cx, cy, R)


def koerper(img, x0, x1, y0, y1):
    """Metallgehaeuse eines Roehrenelements (Kopf oben grau, unten Fuss mit Loch, Lage wie im Echtbild)."""
    schatten(img, sd_box(x0, y0, x1, y1, 2.), .62, 1.5, 1.2, 1.5)
    blend(img, col(7, 7, 7), aa(sd_box(x0 - .9, y0 - .9, x1 + .9, y1 + .9, 2.6), .8))
    u = np.clip((X - x0) / (x1 - x0), 0, 1)
    z = np.clip(np.sin(np.pi * u), 0, 1) ** .7                # Zylinderprofil
    gl = np.exp(-((u - .30) / .13) ** 2)                      # Glanz
    f = (58 + 92 * z + 62 * gl)[..., None] * col(1., .99, .92) + (rausch(1.0, 21) * 5 + rausch(.25, 22) * 3)[..., None]
    a = aa(sd_box(x0, y0, x1, y1, 2.), .5)
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(14, 14, 13), aa(sd_box(x0, y0 + 13.5, x1, y0 + 14.6), .4) * .75)
    blend(img, col(230, 230, 216), aa(sd_box(x0, y0 + 14.7, x1, y0 + 15.3), .4) * .22)


def roehre(img, x0, x1, ytop, ybot, level, voll=col(218, 212, 170)):
    """Schauglas in x0..x1, ytop (Wert 100) .. ybot (Wert 0); level 0..100."""
    blend(img, col(6, 6, 6), aa(sd_box(x0 - .6, ytop - .6, x1 + .6, ybot + .6, 1.1), .6))
    u = np.clip((X - x0) / (x1 - x0), 0, 1)
    z = np.clip(np.sin(np.pi * u), 0, 1) ** .8
    gl = np.exp(-((u - .28) / .11) ** 2)
    ly = ybot - (ybot - ytop) * level / 100.
    a = aa(sd_box(x0, ytop, x1, ybot, .7), .5)
    leer = (16 + 26 * z + 40 * gl)[..., None] * col(.92, 1., .98)
    blend(img, leer, a)
    voll_f = (voll[None, None, :] * (.58 + .46 * z + .10 * gl)[..., None]) + (rausch(1.5, 23) * 5)[..., None]
    a2 = a * aa(ly - Y, .5)
    img[:] = img * (1 - a2[..., None]) + voll_f * a2[..., None]
    if level > 0:
        blend(img, col(250, 248, 224), aa(np.abs(Y - ly) - .32, .4) * a * .8)
        blend(img, col(40, 34, 18), aa(np.abs(Y - ly - 1.1) - .5, .6) * a * .22)
    return ly


def teilung(img, x0, x1, ytop, ybot, schritt=10):
    for v in range(0, 101, schritt):
        y = ybot - (ybot - ytop) * v / 100
        l = 3.2 if v % 50 == 0 else 2.0
        blend(img, col(24, 22, 20), aa(sd_box(x0, y - .22, x0 + l, y + .22), .35) * .72)


def bund(img, x0, x1, ytop, ybot, wert):
    """Roter Anschlag (Fassungsvermoegen) quer ueber das Schauglas."""
    y = ybot - (ybot - ytop) * wert / 100
    blend(img, col(170, 26, 20), aa(sd_box(x0 - 1., y - .6, x1 + 1., y + .6, .3), .4))
    blend(img, col(235, 120, 100), aa(sd_box(x0 - .5, y - .55, x1 + .5, y - .25), .4) * .5)


def loch(img, cx, cy, r=1.9):
    blend(img, col(190, 190, 176), aa(sd_kreis(cx + .25, cy + .3, r + .35), .5) * .4)
    blend(img, col(6, 6, 6), aa(sd_kreis(cx, cy, r), .5))


def schildchen(img, x0, y0, x1, y1, s, size=4.8, grund=col(30, 30, 28), tinte=col(226, 222, 206)):
    """Graviertes Plaettchen (dunkel, wie die schwarzen Felder unter den Tasten des Echtbilds)."""
    schatten(img, sd_box(x0, y0, x1, y1, .6), .4, .7, .5, .7)
    a = aa(sd_box(x0, y0, x1, y1, .6), .5)
    f = grund[None, None, :] + (rausch(.7, 31) * 3)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(120, 118, 110), aa(np.abs(sd_box(x0, y0, x1, y1, .6)) - .1, .4) * .42)
    text(img, s, (x0 + x1) / 2, (y0 + y1) / 2 + .15, size, tinte, .95, F_BD, blur=.12)


def lampe_unten(img, x0, y0, w=8, h=3, an=True):
    """Lampen-Marke wie 2050 (Nachtrag 18: 'das mit den Lampen unten gut')."""
    blend(img, col(8, 10, 8), aa(sd_box(x0 - .8, y0 - .8, x0 + w + .8, y0 + h + .8, .6), .5))
    if an:
        blend(img, col(0, 255, 90), weich(aa(sd_box(x0, y0, x0 + w, y0 + h, .3), .5), .9) * .35)
        blend(img, col(40, 212, 80), aa(sd_box(x0, y0, x0 + w, y0 + h, .4), .4))
        blend(img, col(190, 255, 200), aa(sd_box(x0 + 1, y0 + .6, x0 + w - 1, y0 + 1.2), .4) * .6)
    else:
        blend(img, col(20, 36, 22), aa(sd_box(x0, y0, x0 + w, y0 + h, .4), .4))


# --- Schluss: verkleinern, Bildkoernung wie MDEC, Rahmen, Cursor ------------------------------------------------------
def rahmen():
    """Rahmenpixel aus ROOM2050 Cut 15 (zeilengleich mit ROOM11F0 Cut 10): alles ausserhalb der Oeffnung x 12..303, y 20..219,
    dazu die Eckkerben, wo die Rahmenschraube in die Oeffnung ragt."""
    a = np.array(Image.open(bg('ROOM20515.ppm')).convert('RGB')).astype(np.float32)
    b = np.array(Image.open(bg('ROOM11F10.ppm')).convert('RGB')).astype(np.float32)
    m = np.ones((H, W), bool)
    m[20:220, 12:304] = False
    d = np.abs(a - b).sum(2)
    for (x0, x1, y0, y1) in ((12, 18, 20, 26), (298, 304, 20, 26), (12, 18, 214, 220), (298, 304, 214, 220)):
        m[y0:y1, x0:x1] = d[y0:y1, x0:x1] < 24
    return a, m


def cursor_stempel():
    """Cursor 'unser Cursor' aus einem echten Spielbild: C_belege/pult2050_loesung_sheet.png, Kachel F540 (Cursor bei 160,119)."""
    s = np.array(Image.open(os.path.join(TREE, 'analysis/befunde_runde41/C_belege/pult2050_loesung_sheet.png')).convert('RGB')).astype(np.int32)
    t = s[100:140, 320 + 140:320 + 182]
    bg_ = np.array(Image.open(bg('ROOM20515.ppm')).convert('RGB')).astype(np.int32)[100:140, 140:182]
    px = []
    for y in range(40):
        for x in range(42):
            r, g, b = t[y, x]
            diff = np.abs(t[y, x] - bg_[y, x]).sum()
            hell_gruen = g > 110 and r < 90 and b < 110
            gelb = r > 100 and g > 95 and b < 170
            if hell_gruen or gelb or (diff > 12 and ((g > r + 14 and g > b + 14) or (r > b + 18 and g > b + 14))):
                px.append((x - 20, y - 19, int(r), int(g), int(b)))
    return px


def fertig(img, cursor=None, ausgabe=None):
    klein = cv2.resize(img.clip(0, 255), (W, H), interpolation=cv2.INTER_AREA)
    r = np.random.default_rng(4)
    klein = klein + r.standard_normal((H, W, 1)).astype(np.float32) * 1.9
    bild = Image.fromarray(klein.clip(0, 255).astype(np.uint8))
    # BSS-Hintergruende sind MDEC-Bilder (4:2:0, Makrobloecke): dieselbe Kornstruktur ueber eine JPEG-Kette
    puffer = io.BytesIO()
    bild.save(puffer, 'JPEG', quality=84, subsampling=2)
    bild = Image.open(io.BytesIO(puffer.getvalue())).convert('RGB')
    arr = np.array(bild).astype(np.float32)
    # Kontrast der echten Nahansichten (2050 Cut 15 / 11F0 Cut 10): tiefe Schatten, Mitten etwas dunkler
    arr = 255 * np.power(arr / 255, 1.16)
    # Rahmen
    a, m = rahmen()
    # Schatten des Rahmens in die Oeffnung (oben/links staerker, wie 2050 Cut 15)
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
    dd = np.minimum.reduce([xx - 12, 303 - xx, yy - 20, 219 - yy]).clip(0, 40)
    arr *= (1 - .62 * np.exp(-dd / 3.2))[..., None]
    # EXIT-Feld, pixelgleich aus ROOM11F0 Cut 10 (x 210..238, y 175..204), rechts unten auf die Tafel
    ex = np.array(Image.open(bg('ROOM11F10.ppm')).convert('RGB')).astype(np.float32)[175:205, 210:239]
    ey, ex0 = 180, 240
    arr[ey + 1:ey + 32, ex0 + 1:ex0 + 31] *= .55
    arr[ey:ey + 30, ex0:ex0 + 29] = ex
    arr[m] = a[m]
    out = arr.clip(0, 255).astype(np.uint8)
    if cursor is not None:
        cx, cy = cursor
        for dx, dy, r_, g_, b_ in cursor_stempel():
            xx_, yy_ = int(cx + dx), int(cy + dy)
            if 0 <= xx_ < W and 0 <= yy_ < H:
                out[yy_, xx_] = (r_, g_, b_)
    im = Image.fromarray(out)
    if ausgabe:
        im.resize((W * 2, H * 2), Image.NEAREST).save(ausgabe)
    return im
