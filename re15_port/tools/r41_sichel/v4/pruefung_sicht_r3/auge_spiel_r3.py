"""Pruefung V4 r3 Sicht: Augenfleck im Spielbild h135/h118 (RE15_FRAMEDUMP) wie PS2_auge_spiel_b (Box 338-370 x 188-218 bei h135,
ohne den Koerper-Fleck links oben), Kern 3x3 / Ring 15x15, Blob; Flaeche auf H normiert gegen das Original (52 px bei H1 302)."""
import sys, json, numpy as np, cv2
R = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def lum(a): return a @ np.array([0.299, 0.587, 0.114])
def mess(img, bx):
    Lm = lum(img); b = cv2.blur(Lm.astype(np.float32), (5, 5)); m = np.full_like(b, -1); x0, y0, x1, y1 = bx; m[y0:y1, x0:x1] = b[y0:y1, x0:x1]
    iy, ix = np.unravel_index(np.argmax(m), m.shape); k = Lm[iy-1:iy+2, ix-1:ix+2].mean(); r = Lm[iy-7:iy+8, ix-7:ix+8].copy(); r[4:11, 4:11] = np.nan; rr = np.nanmean(r); thr = (k+rr)/2
    mm = np.zeros_like(Lm, np.uint8); mm[y0:y1, x0:x1] = (Lm[y0:y1, x0:x1] > thr); n, lab, st, _ = cv2.connectedComponentsWithStats(mm, connectivity=4); c = lab[iy, ix]
    return dict(punkt=(int(ix), int(iy)), kern=round(float(k), 1), ring=round(float(rr), 1), sprung=round(float(k-rr), 1), blob_px=int(st[c, 4]), blob_wh=(int(st[c, 2]), int(st[c, 3])))
out = {}
for k in ('V4r3', 'V4r2'):
    d = mess(S(R + '/spiel_%s_h135.png' % k), (338, 188, 370, 218)); d['flaeche_norm_x_original'] = round((d['blob_px'] / 364.0 ** 2) / (52 / 302.0 ** 2), 2)
    d['wh_H'] = [round(d['blob_wh'][0] / 364, 3), round(d['blob_wh'][1] / 364, 3)]; out[k + '_h135'] = d
out['original'] = dict(blob_px=52, blob_wh=(8, 10), wh_H=[0.026, 0.033], sprung=45.0)
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
