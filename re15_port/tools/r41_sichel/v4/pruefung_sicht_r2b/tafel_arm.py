"""Pruefung V4 r2 Sicht (Wiederholung): (B)-linke Seite vorn x3 - Vorlage 1 entblendet mit Box 298-325 x 262-340 | V4 Buehne Id 14
(gruen) | Spiel V4 r2 h135 | Video n1468 | Spiel V4 r2 h118 (alle Box 270-400 x 230-460)."""
import sys, numpy as np, cv2
from PIL import Image
R2, OUT = sys.argv[1], sys.argv[2]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(np.uint8)
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)[:480]
bx = (270, 230, 400, 460)
def cr(im): return cv2.resize(im[bx[1]:bx[3], bx[0]:bx[2]], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST).copy()
o = cr(L(R2 + '/vorlage1_entbl.png')); cv2.rectangle(o, ((298-bx[0])*3, (262-bx[1])*3), ((325-bx[0])*3, (340-bx[1])*3), (255, 255, 0), 2)
b = cr(L(R2 + '/V4_135.png')); ids = np.load(R2 + '/V4_135_ids.npy')[:480]
cs, _ = cv2.findContours((ids == 14).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
for c in cs: cv2.polylines(b, [((c[:, 0, :] - np.array(bx[:2])) * 3).astype(np.int32)], True, (0, 255, 0), 1)
ims = [(o, 'Vorlage1 n1485 entbl.'), (b, 'V4 Buehne Id14 gruen'), (cr(S(R2 + '/spiel_V4r2_h135.png')), 'Spiel V4r2 h135'), (cr(S(R2 + '/video_h118.png')), 'Video n1468 roh'), (cr(S(R2 + '/spiel_V4r2_h118.png')), 'Spiel V4r2 h118')]
row = np.full((ims[0][0].shape[0], sum(i.shape[1] for i, _ in ims) + 8*4, 3), 40, np.uint8); x = 0
for im, t in ims:
    im = im.copy(); cv2.putText(im, t, (5, 18), 0, 0.55, (255, 255, 0), 2); row[:, x:x+im.shape[1]] = im; x += im.shape[1] + 8
Image.fromarray(row).save(OUT + '/PS2_unterarm_links_x3.png'); print(row.shape)
