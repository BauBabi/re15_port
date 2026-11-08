"""Pruefung V4 r2 Sicht (Wiederholung): Kopf vorn x8 - Vorlage 1 (Nutzerpunkte Auge/Haare/Nase) | V4 Buehne mit Id-9-Kontur und
Riegel-Augenpunkt | Spielbild h135 (gleiche Box) | V4 Buehne im Videoraum (gleiche Unschaerfe wie das Video)."""
import sys, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2, OUT = sys.argv[1], sys.argv[2]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(np.uint8)
o1 = L(R2 + '/vorlage1_entbl.png').copy(); v4 = L(R2 + '/V4_135.png').copy(); sp = cv2.cvtColor(cv2.imread(R2 + '/spiel_V4r2_h135.png'), cv2.COLOR_BGR2RGB)[:480].copy()
Vv = np.clip(G.videoraum(v4.astype(float)), 0, 255).astype(np.uint8).copy(); ids = np.load(R2 + '/V4_135_ids.npy')[:480]
def cr(im, b, f=8): x0, y0, x1, y1 = b; return cv2.resize(im[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
a = cr(o1, (360, 132, 424, 200))
for (x, y), c in (((382, 165), (255, 255, 0)), ((403, 160), (0, 255, 255)), ((394, 177), (0, 255, 255))): cv2.circle(a, ((x-360)*8+4, (y-132)*8+4), 6, c, 2)
bx = (320, 150, 384, 226)
cs, _ = cv2.findContours((ids == 9).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
b = cr(v4, bx); c = cr(sp, bx); d = cr(Vv, bx)
for im in (b, c, d):
    for cc in cs: cv2.polylines(im, [((cc[:, 0, :] - np.array(bx[:2])) * 8 + 4).astype(np.int32)], True, (0, 255, 0), 1)
    cv2.circle(im, ((333-bx[0])*8+4, (190-bx[1])*8+4), 8, (255, 255, 0), 2)
def t(im, s): im = im.copy(); cv2.putText(im, s, (6, 20), 0, 0.6, (255, 255, 0), 2); return im
h = max(i.shape[0] for i in (a, b, c, d)); row = np.full((h, sum(i.shape[1] for i in (a, b, c, d)) + 24, 3), 40, np.uint8); x = 0
for im, s in ((a, 'Vorlage1 x8 gelb Auge cyan Haare/Nase'), (b, 'V4 Buehne x8 gruen Id9 gelb Auge'), (c, 'Spiel h135 x8'), (d, 'V4 Videoraum x8')):
    im = t(im, s); row[:im.shape[0], x:x+im.shape[1]] = im; x += im.shape[1] + 8
Image.fromarray(row).save(OUT + '/PS2_kopf_vorn_x8.png'); print(row.shape)
