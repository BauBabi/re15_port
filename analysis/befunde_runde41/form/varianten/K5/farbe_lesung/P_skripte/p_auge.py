import numpy as np, cv2, sys
from p_lade import K5, lab
from p_zonen import rueck, P, FV
k = P.Kandidat(K5 + '/K5c/form.json')
sauber, alb, m, kk = k.rendern(2)
vid = FV.video_filter(sauber, bild=2, entzerren=False, rauschen=True)
T, clip = rueck(vid, 2)
yy, xx = np.mgrid[:480, :640]
for nm, (x, y) in (('auge_r_W', (223, 271)), ('auge_l_W', (288, 269))):
    mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
    L = lab(np.clip(T[mm], 0, 255)); print(nm, L.mean(0).round(1), 'Lmax', L[:, 0].max().round(1))
# hellste magenta Albedo-Pixel (Augenfarbe) im Render
A = lab(alb.reshape(-1, 3)).reshape(480, 640, 3)
mag = (A[..., 1] > 40) & (A[..., 2] < -5)
ys, xs = np.nonzero(mag); print('magenta Albedo px', mag.sum(), 'Lage', list(zip(xs[:5], ys[:5])))
if mag.sum():
    L = lab(np.clip(T[mag], 0, 255)); print('Kette dort', L.mean(0).round(1), 'Lmax', L[:, 0].max().round(1))
