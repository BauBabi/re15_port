"""Pruefung V4 r2 Sicht (Wiederholung): Augenfleck im Spielbild h135/h118 (RE15_FRAMEDUMP) - hellste 5x5-Stelle in der
Kopfregion (Box 325-375 x 165-225, ohne Lampe), Kern 3x3 / Ring 15x15, Blob (Schwelle Mitte Kern/Ring), gegen Video roh n1485."""
import sys, json, numpy as np, cv2
from PIL import Image
R2 = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def lum(a): return a @ np.array([0.299, 0.587, 0.114])
def mess(img, bx):
    Lm = lum(img); b = cv2.blur(Lm.astype(np.float32), (5, 5)); m = np.full_like(b, -1); x0, y0, x1, y1 = bx; m[y0:y1, x0:x1] = b[y0:y1, x0:x1]
    iy, ix = np.unravel_index(np.argmax(m), m.shape); k = Lm[iy-1:iy+2, ix-1:ix+2].mean(); r = Lm[iy-7:iy+8, ix-7:ix+8].copy(); r[4:11, 4:11] = np.nan; rr = np.nanmean(r); thr = (k+rr)/2
    mm = np.zeros_like(Lm, np.uint8); mm[y0:y1, x0:x1] = (Lm[y0:y1, x0:x1] > thr); n, lab, st, _ = cv2.connectedComponentsWithStats(mm, connectivity=4); c = lab[iy, ix]
    return dict(punkt=(int(ix), int(iy)), kern=round(float(k), 1), ring=round(float(rr), 1), sprung=round(float(k-rr), 1), blob_px=int(st[c, 4]), blob_wh=(int(st[c, 2]), int(st[c, 3])))
out = {'video_n1485': mess(S(R2 + '/video_h135.png'), (366, 150, 412, 192)), 'spiel_h135': mess(S(R2 + '/spiel_V4r2_h135.png'), (325, 165, 375, 225)), 'spiel_h118': mess(S(R2 + '/spiel_V4r2_h118.png'), (320, 160, 380, 230)), 'spiel_V3r2_h135': mess(S(R2 + '/spiel_V3r2_h135.png'), (325, 165, 375, 225))}
print(json.dumps(out, indent=1)); json.dump(out, open(sys.argv[2] + '/PS2_auge_spiel.json', 'w'), indent=1)
