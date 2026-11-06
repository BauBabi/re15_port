"""Pruefer V4: Port-Ist V3 r2 (Lauf port_V3r2_h135.png, rechte Haelfte, Spielkamera 50A0 Cut 12)
gegen die Vorlage-1-Ziele (P_farben.json, entblendet), verglichen im Videoraum:
Port -> Gauss sigma 1,5 -> V' = a + b*P (p_eichung) -> Lab (D65) -> dE00 (skimage).
Regionsboxen von Hand auf die Port-Pose gesetzt (Port steht aufrechter, Kopf x338-392 y158-222).
Aufruf: python p_port_ist.py PORT_PNG OUTDIR"""
import sys, json, numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab, deltaE_ciede2000
EA = np.array([9.42, 11.96, 10.94]); EB = np.array([0.9979, 0.9901, 0.9964])
p = np.asarray(Image.open(sys.argv[1]).convert("RGB")).astype(np.float32)[:, 640:]
pv = np.clip(EA + EB * cv2.GaussianBlur(p, (0, 0), 1.5), 0, 255)
ziele = {r["region"]: r for r in json.load(open(sys.argv[2] + "/P_farben.json", encoding="utf-8")) if r["bild"] == 1}
def lab(c): return rgb2lab(np.array(c, float).reshape(1, 1, 3) / 255.0)[0, 0]
L_ = lambda a: a @ np.array([0.299, 0.587, 0.114])
REG = [("Kopf rote Kuppe", (345, 158, 385, 170), None),
       ("Auge vorn (heller Fleck)", (353, 175, 359, 181), None),
       ("Haare vorn (dunkles Feld)", (366, 168, 382, 186), None),
       ("Nase", (362, 182, 370, 188), None),
       ("Kinn/Maulzone dunkel", (345, 198, 380, 212), None),
       ("Brust rot", (330, 225, 400, 270), lambda a: (a[..., 0] - a[..., 1]) >= 15),
       ("Jeans", (355, 315, 420, 345), None),
       ("Bezug Wand links", (40, 150, 119, 299), None)]
res = []
for name, (x0, y0, x1, y1), bed in REG:
    sub = pv[y0:y1 + 1, x0:x1 + 1]
    m = np.ones(sub.shape[:2], bool) if bed is None else bed(sub)
    rgb = sub[m].mean(0)
    lp = lab(rgb); lz = np.array(ziele[name]["lab"])
    d = float(deltaE_ciede2000(lp.reshape(1, 1, 3), lz.reshape(1, 1, 3))[0, 0])
    r = {"region": name, "port_box": [x0, y0, x1, y1], "px": int(m.sum()), "port_videoraum_rgb": rgb.round(1).tolist(),
         "port_lab": lp.round(1).tolist(), "ziel_lab": lz.tolist(), "dE00": round(d, 1), "dL": round(float(lp[0] - lz[0]), 1)}
    res.append(r); print(f"{name:30s} port {r['port_lab']} ziel {r['ziel_lab']} dE00 {r['dE00']} dL {r['dL']}")
json.dump(res, open(sys.argv[2] + "/P_port_ist.json", "w"), indent=1, ensure_ascii=False)
