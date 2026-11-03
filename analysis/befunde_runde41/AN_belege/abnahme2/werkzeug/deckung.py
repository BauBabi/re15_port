"""Gegenprobe zum Bildanteil (Abnahme 2): ID-Puffer der Autoren (AN_belege/werkzeug/n3/szmass.py) fuer ein Spielbild aus MEINEM
state.log rechnen und als Umriss auf MEIN Fensterbild (RE15_FRAMEDUMP) legen. Passt der Umriss auf die gezeichneten Figuren, ist
die Messung des Bildanteils (ohne Feuer/Masken) belastbar. Zusaetzlich: Anteil der Form-Flaeche, die im echten Bild von Feuer
(gesaettigtes Orange/Gelb) bedeckt ist. Aufruf: deckung.py <laufdir> <ausgabe.png> F [F ...]"""
import sys, os, re
import numpy as np
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', '..', 'werkzeug', 'n3'))
import szmass as S

def zeile(log, f):
    last = None
    for l in open(log, errors='replace'):
        if l.startswith('F%d ' % f):
            last = l
    return last

d, aus = sys.argv[1], sys.argv[2]
bilder = []
for f in map(int, sys.argv[3:]):
    l = zeile(os.path.join(d, 'state.log'), f)
    cam = int(re.search(r'cam=(\d+)', l).group(1))
    objs = []
    for typ, mo, oid in (('30', S.FORM, 1), ('4d', S.FRAU, 2)):
        m = re.search(r't=%s st=\d+ ss1=\d+ ss2=\d+ ss3=\d+ g=\d+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(\d+)' % typ, l)
        if m:
            clip, bild, x, z, r = map(int, m.groups())
            if oid == 2 and clip == 2:
                clip, bild = 1, 0
            objs.append((mo, clip, bild, (x, 0, z), r, oid))
    buf, al = S.idpuffer(cam, objs)
    m = S.mass(buf, al)
    im = np.array(Image.open(os.path.join(d, 'f%06d.ppm' % f)).convert('RGB')).astype(int)
    r, g, b = im[..., 0], im[..., 1], im[..., 2]
    feuer = (r > 200) & (g > 70) & (b < 120) & (r - b > 120)
    fm = buf == 1
    feuer_auf_form = float((feuer & fm).sum()) / max(1, fm.sum())
    print('F%d Cut %d: Form %.1f %% des Bandes, Annette %.0f %% frei (gegen die Form), Form-Flaeche mit Feuer bedeckt %.0f %%' % (
        f, cam, 100 * m['form'], 100 * m.get('frau_sichtbar', 0), 100 * feuer_auf_form))
    o = im.copy()
    for oid, farbe in ((1, (0, 255, 255)), (2, (0, 255, 0))):
        mk = buf == oid
        rand = mk & ~(np.roll(mk, 1, 0) & np.roll(mk, -1, 0) & np.roll(mk, 1, 1) & np.roll(mk, -1, 1))
        o[rand] = farbe
    bilder.append(Image.fromarray(o.astype(np.uint8)))
w, h = bilder[0].size
blatt = Image.new('RGB', (w * min(2, len(bilder)), h * ((len(bilder) + 1) // 2)))
for i, b in enumerate(bilder):
    blatt.paste(b, ((i % 2) * w, (i // 2) * h))
blatt.save(aus)
