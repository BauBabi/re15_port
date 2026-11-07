"""V3P_4: Vorhersage der Ketten fuer Leser As (b2)-Probe neben dem Original (roh, gleicher Filter, Saat 15)."""
import cv2, numpy as np
from PIL import Image, ImageDraw
O = {1: r'C:/workspace/git/reAi_v2/Version_80%/vlcsnap-2026-10-31-00h28m45s774.png', 2: r'C:/workspace/git/reAi_v2/Version_80%/vlcsnap-2026-10-31-00h29m02s830.png'}
box = {1: (180, 170, 470, 480), 2: (120, 130, 480, 480)}
b = Image.new('RGB', (3 * 366 + 24, 2 * 380 + 40), (18, 19, 22)); dr = ImageDraw.Draw(b)
dr.text((6, 4), 'Ketten-Vorhersage (kein Bau): Original | K5c heute -> video_filter | Leser-A-(b2)-Probe (G4-Texel 1:1, Kopf aus F1) -> video_filter. Roh, Saat 15.', fill=(240, 200, 90))
y = 24
for n in (1, 2):
    x0, y0, x1, y1 = box[n]
    ims = [cv2.imread(O[n]), cv2.imread('kette_b2_bild%d_K5c_heute.png' % n), cv2.imread('kette_b2_bild%d_b2_probe_A.png' % n)]
    for j, (t, im) in enumerate(zip(('Original Bild %d' % n, 'K5c heute', '(b2)-Probe A'), ims)):
        c = cv2.cvtColor(im[y0:y1, x0:x1], cv2.COLOR_BGR2RGB)
        s = 360 / max(c.shape[:2]); c = cv2.resize(c, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
        dr.text((6 + j * 372, y), t, fill=(220, 220, 220)); b.paste(Image.fromarray(c), (6 + j * 372, y + 14))
    y += 380
b.save('V3P_4_ketten_vorhersage_b2.png'); print('ok')
