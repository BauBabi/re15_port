#!/usr/bin/env python3
"""Textur M1: 1) UV-Abdeckung je Texel (welche Texel lesen Flaechen ueberhaupt), Wolverine-Rechteck.
2) Suche des Wolverine-Bilds in allen TIMs der Mod-EMS und in RE2.
3) Mehrskalige Suche je 24x24-Kachel gegen RE2 G2/G1, D0 (cv2.matchTemplate CCOEFF_NORMED auf Luminanz+Farbe), Karte als PNG."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from blobs_kette import kette
from PIL import Image, ImageDraw
import cv2
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
M1 = zerlege(blob('M1'))
D0 = zerlege(blob('D0'))
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2); G2 = zerlege(e, t)
e, *_ = re2_rec(0x30, 3); t, *_ = re2_rec(0x30, 2); G1 = zerlege(e, t)


def rgb(q):
    w = q.tex.shape[1] // q.pal.shape[0]
    return np.concatenate([q.pal[i][q.tex[:, i*w:(i+1)*w]] for i in range(q.pal.shape[0])], axis=1)


R1 = rgb(M1)
h, w = R1.shape[:2]
# UV-Abdeckung
mask = np.zeros((h, w), np.uint8)
for i, m in enumerate(M1.meshes[:16]):
    for u in m.tuv:
        p = (u[5] & 0xF) * 128
        pts = np.array([[u[0]+p, u[1]], [u[3]+p, u[4]], [u[6]+p, u[7]]], np.int32)
        cv2.fillConvexPoly(mask, pts, 1)
    for u in m.quv:
        p = (u[5] & 0xF) * 128
        pts = np.array([[u[0]+p, u[1]], [u[3]+p, u[4]], [u[9]+p, u[10]], [u[6]+p, u[7]]], np.int32)
        cv2.fillConvexPoly(mask, pts, 1)
print('UV-Abdeckung gesamt:', mask.sum(), 'von', mask.size, f'= {100*mask.mean():.1f}%')
x0, y0, x1, y1 = 301, 7, 323, 29
print('Wolverine-Rechteck x', x0, x1, 'y', y0, y1, 'abgedeckte Texel:', mask[y0:y1, x0:x1].sum(), 'von', (y1-y0)*(x1-x0))
vkachel = R1[y0:y1, x0:x1]
print('mittlere Farbe Kachel', vkachel.reshape(-1, 3).mean(0).round(1))
idx = M1.tex[y0:y1, x0:x1]
print('Index-Bytes Zeile 7 (TIM-Pixel):', idx[0][:22].tolist())
# Offsets
tim_off = M1.dir[8]
pix = 8 + u32(M1.tim, 8) + 12
print('EM-Blob: TIM @', tim_off, 'Pixel @', tim_off + pix, ' Texel (301,7) @ Blob', tim_off + pix + 7*384 + 301, ' = Archiv-Offset 0x%X' % (2015*2048 + tim_off + pix + 7*384 + 301))

# Wolverine in anderen TIMs
tpl = cv2.cvtColor(vkachel, cv2.COLOR_RGB2BGR)
treffer = []
for nm, pf in (('Mod0', MOD+'CDEMD0.EMS'), ('Mod1', MOD+'CDEMD1.EMS'), ('Disc0', DISC+'CDEMD0.EMS'), ('Disc1', DISC+'CDEMD1.EMS')):
    a = open(pf, 'rb').read()
    for s, ln, c in kette(a):
        try:
            q = zerlege(a[s*2048:s*2048+ln])
            if q.tex is None:
                continue
            bild = rgb(q) if q.pal.shape[0] > 1 else q.pal[0][q.tex]
            r = cv2.matchTemplate(cv2.cvtColor(bild, cv2.COLOR_RGB2BGR), tpl, cv2.TM_SQDIFF)
            mn = r.min() / tpl.size
            treffer.append((mn, nm, s, cv2.minMaxLoc(r)[2]))
        except Exception as ex:
            pass
for kind in range(0x10, 0x5B):
    e, _, _ = re2_rec(kind, 3)
    t, _, _ = re2_rec(kind, 2)
    if not e or not t:
        continue
    try:
        q = zerlege(e, t)
        bild = rgb(q)
        r = cv2.matchTemplate(cv2.cvtColor(bild, cv2.COLOR_RGB2BGR), tpl, cv2.TM_SQDIFF)
        treffer.append((r.min() / tpl.size, 'RE2', kind, cv2.minMaxLoc(r)[2]))
    except Exception:
        pass
treffer.sort(key=lambda x: x[0])
print('Beste Treffer Wolverine (mittl. Quadratfehler je Kanal):')
for x in treffer[:8]:
    print('  ', round(float(x[0]), 1), x[1], x[2] if x[1] != 'RE2' else hex(x[2]), x[3])

# Mehrskalige Suche
G = 24
quellen = {'G2': rgb(G2), 'G1': rgb(G1), 'D0': rgb(D0)}
Q8 = {k: cv2.cvtColor(v, cv2.COLOR_RGB2BGR) for k, v in quellen.items()}
ZI = cv2.cvtColor(R1, cv2.COLOR_RGB2BGR)
kart = Image.fromarray(R1).convert('RGB').resize((w*3, h*3), Image.NEAREST)
ov = Image.new('RGBA', kart.size, (0, 0, 0, 0))
dr = ImageDraw.Draw(ov)
FARBE = {'G2': (60, 230, 90, 80), 'G2s': (240, 220, 40, 80), 'G1': (60, 160, 255, 80), 'D0': (255, 60, 60, 110), '?': (0, 0, 0, 150)}
zaehl = {}
for y in range(0, h - G + 1, G):
    for x in range(0, w - G + 1, G):
        tt = ZI[y:y+G, x:x+G]
        if tt.mean() < 8 or tt.std() < 6:
            continue
        if mask[y:y+G, x:x+G].mean() < .3:
            k = 'unbenutzt'
            zaehl[k] = zaehl.get(k, 0) + 1
            continue
        best = (-1, None, None)
        for k, Qq in Q8.items():
            for s in (1.0, .9, .8, .75, .67, .6, .5, 1.25, 1.5):
                tw = max(8, int(round(G*s)))
                tr = cv2.resize(tt, (tw, tw), interpolation=cv2.INTER_AREA if s < 1 else cv2.INTER_LINEAR)
                r = cv2.matchTemplate(Qq, tr, cv2.TM_CCOEFF_NORMED)
                v = float(r.max())
                if v > best[0]:
                    best = (v, k, s)
        v, k, s = best
        if v < .80:
            lab = '?'
        elif k == 'G2' and s != 1.0:
            lab = 'G2s'
        else:
            lab = k
        zaehl[lab] = zaehl.get(lab, 0) + 1
        dr.rectangle([x*3, y*3, (x+G)*3-1, (y+G)*3-1], fill=FARBE[lab])
print('Kacheln 24x24, von Flaechen genutzt:', zaehl)
kart = kart.convert('RGBA')
kart.alpha_composite(ov)
kart.convert('RGB').save(os.path.join(AUS, 'tex_M1_herkunft.png'))
Image.fromarray((mask*255)).resize((w*2, h*2), Image.NEAREST).save(os.path.join(AUS, 'tex_M1_uvmaske.png'))
print('ok')
