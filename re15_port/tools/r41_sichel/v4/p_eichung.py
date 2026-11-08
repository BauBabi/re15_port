"""Pruefer V4: Eichung Video <-> Port-Schirm ueber den gemeinsamen Raumhintergrund (50A0 Cut 12).
Video: Median n1442..1472 (1-basiert; unabgeblendet). Port: rechte Haelfte von port_V3r2_h135.png
(Leser-A-Lauf V3 r2). Port vorher mit Gauss sigma S geglaettet (Video ist unscharf). Fit je Kanal
Video = a + b*Port ueber statische Felder ausserhalb des Wesens (x<270 oder x>540, y 60..440).
Aufruf: python p_eichung.py FRAMES_NPY PORT_PNG OUTDIR"""
import sys, json, numpy as np, cv2
from PIL import Image
fr = np.load(sys.argv[1]).astype(np.float32)
vid = np.median(fr[1441 - 1300:1472 - 1300], axis=0)
port = np.asarray(Image.open(sys.argv[2]).convert("RGB")).astype(np.float32)[:, 640:]
m = np.zeros((480, 640), bool); m[60:440, :270] = True; m[60:440, 540:] = True
res = {}
for s in (0, 1.5, 2.5):
    p = cv2.GaussianBlur(port, (0, 0), s) if s else port
    out = []
    for c in range(3):
        x = p[:, :, c][m]; y = vid[:, :, c][m]
        A = np.vstack([x, np.ones_like(x)]).T
        (b, a), *_ = np.linalg.lstsq(A, y, rcond=None)
        r = float(np.sqrt(((y - (a + b * x)) ** 2).mean()))
        out.append({"a": round(float(a), 2), "b": round(float(b), 4), "rmse": round(r, 2)})
    res[f"sigma{s}"] = out
    print("sigma", s, out)
# Felder-Mittel (robust gegen Unschaerfe)
felder = {"Wand links": (40, 150, 120, 300), "Boden links": (30, 380, 200, 440), "Wand rechts": (560, 230, 630, 330),
          "Decke links": (20, 60, 140, 95), "Tuerrahmen gelb": (470, 85, 490, 120)}
fl = {}
for k, (x0, y0, x1, y1) in felder.items():
    fl[k] = {"video": vid[y0:y1, x0:x1].reshape(-1, 3).mean(0).round(1).tolist(),
             "port": port[y0:y1, x0:x1].reshape(-1, 3).mean(0).round(1).tolist()}
    print(k, fl[k])
res["felder"] = fl
json.dump(res, open(sys.argv[3] + "/P_eichung.json", "w"), indent=1)
