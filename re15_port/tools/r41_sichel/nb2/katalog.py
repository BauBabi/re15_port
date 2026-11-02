"""Runde 41 SF NB2: Clip-Katalog einer EMD-Datei (wie tools/r41_hangar/clipkatalog.py, aber aus der Datei statt aus der EMS),
optional zwei Dateien zeilenweise abwechselnd (vorher/nachher).  python katalog.py <aus.png> <emd> [<emd2>] [--von a --bis b]"""
import argparse, math, os, sys
import numpy as np
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HIER, '..', '..'))
import emd_ansichtsblatt as ea   # noqa: E402
import birkin_buehne as BB       # noqa: E402

ap = argparse.ArgumentParser(); ap.add_argument('aus'); ap.add_argument('emd', nargs='+')
ap.add_argument('--von', type=int, default=0); ap.add_argument('--bis', type=int, default=20)
ap.add_argument('--spalten', type=int, default=8); ap.add_argument('--kachel', type=int, default=110)
ap.add_argument('--gier', type=float, default=35.0)
a = ap.parse_args()
mos = [BB.modell(open(e, 'rb').read()) for e in a.emd]
g = math.radians(a.gier)
depth = np.array([-math.cos(g), 0.0, -math.sin(g)]); down = np.array([0.0, 1.0, 0.0])
right = np.cross(down, depth); right /= np.linalg.norm(right)
K = a.kachel
zeilen = [(c, i) for c in range(a.von, a.bis + 1) for i in range(len(mos))]
S = Image.new('RGB', (60 + K * a.spalten, K * len(zeilen)), (18, 19, 22)); d = ImageDraw.Draw(S)
for r, (c, i) in enumerate(zeilen):
    mo = mos[i]; first, n = mo.clips[c]
    d.text((4, r * K + 4), 'clip\n0x%02X\n%d B.\n%s' % (c, n, 'vorher' if i == 0 else 'neu'), fill=(230, 230, 230))
    for s in range(a.spalten):
        fi = round(s * (n - 1) / (a.spalten - 1)); kf = mo.frames[first + fi] & 0xfff
        V, N, F = mo.geometrie(kf)
        col, alp = ea.render(mo, V, N, F, right, down, depth, K, K, K / 5600.0, K / 2, K * 0.80)
        img = Image.fromarray((np.clip(col * 2.4, 0, 1) * 255).astype(np.uint8))
        bg = Image.new('RGB', (K, K), (28, 30, 35)); bg.paste(img, (0, 0), Image.fromarray((alp * 255).astype(np.uint8)))
        dd = ImageDraw.Draw(bg); dd.line([(0, K * 0.80), (K, K * 0.80)], fill=(60, 64, 72)); dd.text((3, K - 12), 'B%d' % fi, fill=(214, 138, 74))
        S.paste(bg, (60 + s * K, r * K))
S.save(a.aus); print(a.aus, S.size)
