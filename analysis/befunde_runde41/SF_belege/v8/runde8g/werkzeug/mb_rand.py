"""Messbild - kein Ergebnis: Spielbild-Ausschnitte (Szene 50A0, exe d8ef12963, 960x720) F325 / F336 (Nutzerbild): 8c | 8d | 8d mit Naht-Texeln
u 54-55 markiert (magenta, aus dem Naht-Markerlauf) | 8g."""
import sys, os, numpy as np
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rand as R
L, aus = sys.argv[1], sys.argv[2]
try:
    fk = ImageFont.truetype('arial.ttf', 26); fs = ImageFont.truetype('arial.ttf', 17)
except OSError:
    fk = fs = ImageFont.load_default()
zeilen = []
for f in (325, 336):
    s_ = R.bild(L + '/s', f); mk = R.bild(L + '/mk_klasse', f)
    T_ = np.abs(mk - s_).sum(2) > 0
    ys, xs = np.nonzero(T_)
    y0, y1, x0, x1 = max(ys.min() - 12, 0), min(ys.max() + 12, 719), max(xs.min() - 12, 0), min(xs.max() + 12, 959)
    sp = []
    for name, d in (('8c d31f9534', 'n_8c'), ('8d 547acc80 (Video 30)', 'n_8d'), ('8d, Naht u 54-55 magenta', 'nahtmark'), ('8g a02a6df3', 'n_8g')):
        if d == 'nahtmark':
            n = R.bild(L + '/n_8d', f); nh = R.bild(L + '/naht_8d', f)
            m = T_ & (np.abs(nh - n).sum(2) > 0)
            im = n.copy(); im[m] = (255, 0, 255)
        else:
            im = R.bild(L + '/' + d, f)
        c = Image.fromarray(im[y0:y1 + 1, x0:x1 + 1].astype(np.uint8))
        c = c.resize((c.width * 2, c.height * 2), Image.NEAREST)
        sp.append((name, c))
    zeilen.append(('F%d' % f, sp))
PAD, KOPF = 12, 70
cw = max(c.width for _, sp in zeilen for _, c in sp); ch = [max(c.height for _, c in sp) for _, sp in zeilen]
BW = PAD + 4 * (cw + PAD); BH = KOPF + sum(h + 30 for h in ch) + PAD
B = Image.new('RGB', (BW, BH), (22, 23, 27)); dr = ImageDraw.Draw(B)
dr.rectangle([0, 0, BW, KOPF - 8], fill=(150, 30, 30))
dr.text((PAD, 12), 'Messbild - kein Ergebnis', font=fk, fill=(255, 255, 255))
dr.text((PAD + 360, 18), 'Spielbild 960x720 (exe d8ef12963, Szene 50A0), Ausschnitt 2x; F336 = Nutzerbild vlcsnap 07h32m52s078', font=fs, fill=(240, 220, 220))
y = KOPF
for (fn, sp), h in zip(zeilen, ch):
    for i, (name, c) in enumerate(sp):
        x = PAD + i * (cw + PAD)
        dr.text((x, y), '%s  %s' % (fn, name), font=fs, fill=(220, 220, 225))
        B.paste(c, (x, y + 24))
    y += h + 30
B.save(aus); print('ok', aus, B.size)
