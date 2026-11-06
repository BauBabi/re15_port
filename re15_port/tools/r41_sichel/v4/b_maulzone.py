# Leser B L5: dunkle Zone unter der Nase (Bild 1, n1485) frei abgegrenzt: L*(abblende-korr) < Schwelle, Komponente durch (388,190),
# Suchfenster x 366..415 (Kopfbox), y 178..205; Schwellen 11/13/15 als Empfindlichkeit.
import sys, json, numpy as np, cv2
from scipy import ndimage as ndi
import b_common as C
S = sys.argv[1]
I1k = np.load(f'{S}/B_I1k.npy'); L = C.lab(I1k.reshape(-1, 3)).reshape(480, 640, 3)[..., 0]
out = {}
for t in (11, 13, 15):
    m = np.zeros((480, 640), bool); m[178:206, 366:416] = L[178:206, 366:416] < t
    lab, _ = ndi.label(m, structure=np.ones((3, 3))); k = lab[190, 388]
    if k == 0: out[t] = 'Startpunkt nicht dunkel'; continue
    R = lab == k; ys, xs = np.nonzero(R); hull = cv2.convexHull(np.c_[xs, ys].astype(np.int32))
    out[t] = dict(px=int(R.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], breite=int(xs.ptp() + 1), hoehe=int(ys.ptp() + 1),
                  huelle=float(cv2.contourArea(hull)), fuellgrad=round(float(R.sum()) / ((xs.ptp() + 1) * (ys.ptp() + 1)), 2))
    print(t, out[t])
json.dump(out, open(f'{S}/B_L5_maulzone.json', 'w'), indent=1)
