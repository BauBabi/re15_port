"""Pruefer V4: Zielwerte der Riegel-Fenster (kopfrelativ, ohne Farbmaske) im Videoraum.
Kopfbox vorn (n1485, entblendet) x368-416 y139-191; seitlich (n1435) x412-467 y190-263.
Fenster als Anteile der Kopfbox (x von (B)-links, y von oben). Dieselben Anteile gelten am Port auf der
projizierten Bbox von Mesh 9 (b2-ID-Render, nach Gauss sigma 1,5 und V' = a + b*P).
Aufruf: python p_riegelfenster.py OUTDIR"""
import sys, json, numpy as np, cv2
from PIL import Image
OUT = sys.argv[1]; V = r"C:/workspace/git/reAi_v2/Version_80%/"
K1 = np.array([0.766, 0.764, 0.768])
def lade(b):
    a = np.asarray(Image.open(V + f"Beschreibung{b}_Vorlage.png").convert("RGB")).astype(float)
    return 17 + (a - 17) / K1 if b == 1 else a
def lab(c):
    x = np.clip(np.array(c, float).reshape(1, 1, 3) / 255.0, 0, 1).astype(np.float32)
    return [round(float(v), 1) for v in cv2.cvtColor(x, cv2.COLOR_RGB2Lab)[0, 0]]
KB = {1: (368, 139, 416, 191), 2: (412, 190, 467, 263)}
F = [(1, "KV1 Kuppe", (0.40, 0.02, 0.80, 0.32)), (1, "KV2 Haarfeld", (0.52, 0.25, 0.85, 0.75)),
     (1, "KV3 Nase", (0.44, 0.66, 0.62, 0.81)), (1, "KV4 Kinn/Maulzone", (0.21, 0.88, 0.62, 1.05)),
     (1, "KV5 Kopf (B)-links unter dem Auge", (0.05, 0.62, 0.35, 0.85)),
     (2, "KS1 rote Zone oben", (0.11, 0.04, 0.53, 0.29)), (2, "KS2 Haarkante vorn", (0.84, 0.52, 0.96, 0.80)),
     (2, "KS3 Kopf unter dem Auge", (0.44, 0.82, 0.89, 0.99))]
res = []
for b, name, (u0, v0, u1, v1) in F:
    x0, y0, x1, y1 = KB[b]; w, h = x1 - x0, y1 - y0
    bx = (int(round(x0 + u0 * w)), int(round(y0 + v0 * h)), int(round(x0 + u1 * w)), int(round(y0 + v1 * h)))
    a = lade(b); sub = a[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1].reshape(-1, 3)
    rgb = sub.mean(0)
    r = {"bild": b, "fenster": name, "anteile": [u0, v0, u1, v1], "box_px": bx, "px": int(len(sub)),
         "rgb": rgb.round(1).tolist(), "lab": lab(rgb)}
    res.append(r); print(r)
json.dump(res, open(OUT + "/P_riegelfenster.json", "w"), indent=1, ensure_ascii=False)
