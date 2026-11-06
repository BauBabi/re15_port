"""Pruefer V4: Fleck-Messung (Auge vorn/seitlich hell, Haare dunkel) per Regionwachstum ab dem
Extrem im Suchfenster; Schwelle = Mitte zwischen Extremwert (3x3) und Umgebung (Median 21x21).
Bild 1 wird vorher entblendet: I' = 17 + (I-17)/k, k = 0,766/0,764/0,768 (Fit an Wand/Boden gegen
Median n1441..1471, Schwarzwert 17). Aufruf: python p_blob.py OUTDIR"""
import sys, json, numpy as np, cv2
from PIL import Image
OUT = sys.argv[1]; V = r"C:/workspace/git/reAi_v2/Version_80%/"
K1 = np.array([0.766, 0.764, 0.768])
def lade(b):
    a = np.asarray(Image.open(V + f"Beschreibung{b}_Vorlage.png").convert("RGB")).astype(float)
    return 17 + (a - 17) / K1 if b == 1 else a
def lum(a): return a @ np.array([0.299, 0.587, 0.114])
def lab(rgb):
    x = np.clip(np.array(rgb, float).reshape(1, 1, 3) / 255.0, 0, 1).astype(np.float32)
    l = cv2.cvtColor(x, cv2.COLOR_RGB2Lab)[0, 0]
    return [round(float(v), 1) for v in l]
FAELLE = [(1, "Auge vorn", "hell", (370, 155, 392, 178)),
          (1, "Haare vorn", "dunkel", (390, 148, 412, 178)),
          (2, "Auge seitlich", "hell", (440, 225, 462, 248)),
          (2, "Haare seitlich", "dunkel", (455, 228, 469, 250))]
res = []
for b, t, art, (x0, y0, x1, y1) in FAELLE:
    a = lade(b); L = lum(a)
    win = L[y0:y1, x0:x1]
    sm = cv2.blur(win.astype(np.float32), (3, 3))
    iy, ix = np.unravel_index(np.argmax(sm) if art == "hell" else np.argmin(sm), sm.shape)
    px, py = ix + x0, iy + y0
    ext = float(sm[iy, ix]); umg = float(np.median(L[py-10:py+11, px-10:px+11]))
    thr = (ext + umg) / 2
    m = (L > thr) if art == "hell" else (L < thr)
    m = m.astype(np.uint8)
    if b == 2 and art == "dunkel":  # Pfosten rechts ausschliessen: nur x<=468
        m[:, 469:] = 0
    n, labm, st, cen = cv2.connectedComponentsWithStats(m, connectivity=4)
    k = labm[py, px]
    ys, xs = np.nonzero(labm == k)
    rgb = a[ys, xs].mean(0)
    rgb_sw = np.clip(rgb - 17, 0, 255)
    r = {"bild": b, "text": t, "extrem": [int(px), int(py)], "extrem_L": round(ext, 1), "umgebung_L": round(umg, 1),
         "schwelle": round(thr, 1), "px": int(len(xs)), "bbox": [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
         "w_h": [int(xs.max() - xs.min() + 1), int(ys.max() - ys.min() + 1)], "sp": [round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
         "rgb": [round(float(v), 1) for v in rgb], "lab": lab(rgb), "lab_sw17": lab(rgb_sw)}
    res.append(r); print(r)
json.dump(res, open(OUT + "/P_kopf_flecken.json", "w"), indent=1)
