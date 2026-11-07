"""Pruefer V3 Belegbilder: V3P_2 Bild 1 Ort F1 (Original | Knochenkarte K5c | Kopf unverdeckt | Vorwaerts-Probe mit Ziel-Texel),
V3P_3 Bild 2 Unterkiefer (Original | Klassen | Knochen | Vorwaerts-Probe). Nichts aufgehellt; Knochenkarten in Falschfarben, beschriftet."""
from szene import *
import json
from PIL import Image, ImageDraw
import gegenprobe as G

GP = json.load(open('gegenprobe.json', encoding='utf-8'))
F1 = np.load('F1_pruef.npy'); band = np.load('band2_pruef.npy'); zahn = np.load('zahn2_pruef.npy')
kn1 = np.load('kn1.npy'); kn2 = np.load('kn2.npy')
rng = np.random.default_rng(3); pal = rng.integers(60, 255, (40, 3)).astype(np.uint8)


def kontur(img, m, f):
    c, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    o = img.copy(); cv2.drawContours(o, c, -1, f, 1); return o


def knk(kn, hv):
    o = np.zeros((480, 640, 3), np.uint8)
    o[kn >= 0] = pal[kn[kn >= 0] % 40] // 3
    for b, f in hv.items():
        o[kn == b] = f
    return o


def sim_bild(k, T):
    n, m = G.REG[k]
    img = G.E[n].copy(); L = G.licht_in(k)
    img[m] = np.clip(np.asarray(T, np.float32)[None] * L[m], 0, 255)
    return FV.video_filter(np.clip(img + 0.5, 0, 255).astype(np.uint8), bild=n, entzerren=False, rauschen=False)


def blatt(name, titel, kacheln, box, s):
    x0, y0, x1, y1 = box
    w, h = (x1 - x0) * s, (y1 - y0) * s
    pad, kopf = 6, 34
    b = Image.new('RGB', (pad + len(kacheln) * (w + pad), 26 + kopf + h + pad), (18, 19, 22)); dr = ImageDraw.Draw(b)
    dr.text((pad, 6), titel, fill=(240, 200, 90))
    for i, (t, a) in enumerate(kacheln):
        x = pad + i * (w + pad)
        for j, z in enumerate(t.split('|')):
            dr.text((x, 26 + 12 * j), z, fill=(220, 220, 220))
        b.paste(Image.fromarray(cv2.resize(a[y0:y1, x0:x1], (w, h), interpolation=cv2.INTER_NEAREST)), (x, 26 + kopf))
    b.save(name)


V1 = P.original(1); V2 = P.original(2)
aK, LK, mK, _ = render(1, [22, 23]); a21, L21, m21, _ = render(1, [21])
k1 = knk(kn1, {0: (255, 160, 0), 23: (0, 200, 255)})
k1 = kontur(k1, F1, (255, 255, 255)); k1 = kontur(k1, mK, (0, 255, 255)); k1 = kontur(k1, m21, (0, 255, 0))
T1 = GP['fit_F1']['T']
blatt('V3P_2_bild1_F1_x4.png', 'Bild 1 Stelle F1 (Pruefer V3): weiss = F1 (Video L*>=30, a*>=20, 646 px); orange = Knochen 0 sichtbar; cyan = Kopf 22+23 unverdeckt; gruen = Hals 21 unverdeckt',
      [('Original Video (roh)|F1 weiss', kontur(V1, F1, (255, 255, 255))),
       ('K5c Bild-1-Pose: Knochenkarte|F1 liegt zu 646/646 px auf Knochen 0', k1),
       ('Vorwaerts-Probe: F1 = Ziel-Texel %s|x Licht je Pixel -> video_filter' % str([round(v) for v in T1]), kontur(sim_bild('F1', T1), F1, (255, 255, 255))),
       ('Vorwaerts-Probe: F1 = G4-F8-Feuer|(Lab 65,6/25,4/30,0) gleicher Weg', kontur(sim_bild('F1', GP['kandidaten']['G4 F8 Feueranteil']['T']), F1, (255, 255, 255)))],
      (270, 200, 410, 300), 4)
k2 = knk(kn2, {23: (255, 160, 0), 22: (255, 80, 200), 18: (0, 200, 255)})
kl = V2.copy() // 3; kl[band] = (230, 70, 60); kl[zahn] = (235, 235, 235)
TB = GP['fit_band']['T']
blatt('V3P_3_bild2_maul_x3.png', 'Bild 2 Unterkiefer (Pruefer V3): Klassen im e-Raum (rot = Fleischband, weiss = Zaehne); Knochen orange 23, magenta 22, cyan 18',
      [('Original Video (roh)', kontur(V2, band | zahn, (255, 255, 255))), ('Klassen e-Raum|rot Band 66 %, weiss Zaehne 12 %', kl),
       ('K5c Bild-2-Pose: Knochenkarte', kontur(k2, band, (255, 255, 255))),
       ('Vorwaerts-Probe: Band = Ziel-Texel %s' % str([round(v) for v in TB]), kontur(sim_bild('band', TB), band, (255, 255, 255))),
       ('Vorwaerts-Probe: Band = Orange (255,140,40)', kontur(sim_bild('band', (255, 140, 40)), band, (255, 255, 255)))],
      (150, 290, 360, 425), 3)
print('ok')
