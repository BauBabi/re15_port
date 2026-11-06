# Leser B v3: Schwanz gegen Koerperhaut im ORIGINAL-Video 4 (ohne _slp, 640x480), Bilder n1435/n1438/n1442 (h85/h88/h92).
# Schwarzwert: 17 je Kanal abgezogen (Vorgabe), danach auf 0..255 gestreckt (x 255/238).
# Ausrichtung: KI-Bild (1280x960) auf 640x480 verkleinert und per Phasenkorrelation (Graubild) gegen das Original verschoben.
# Schwanzmaske = KI-Schwanzmaske (b_schwanz.py) auf 640 verkleinert, verschoben, 2 px erodiert (5x5 bei 640).
# Hautflaechen = dieselben Rechtecke wie b_farben.py, halbiert und verschoben, 2 px eingerueckt.
# Aufruf: python b_original.py <sfv3-dir> <schwanz-dir> <beleg.png>
import sys, json, cv2, numpy as np
S, T, BEL = sys.argv[1], sys.argv[2], sys.argv[3]
REC = {1435: {"Ruecken/Buckel": (770, 395, 840, 460), "Oberarm vorn (W-rechts)": (705, 510, 750, 560), "Bein (Unterschenkel)": (825, 640, 855, 700), "Jeans": (800, 580, 840, 620)},
       1438: {"Ruecken/Buckel": (770, 395, 840, 455), "Oberarm vorn (W-rechts)": (705, 510, 750, 555)},
       1442: {"Ruecken/Buckel": (750, 385, 810, 440), "Oberarm vorn (W-rechts)": (690, 485, 735, 540), "Bein (Unterschenkel)": (825, 650, 855, 710), "Jeans": (760, 585, 830, 620), "Sichel": (650, 650, 690, 740)}}


def lab(i):
    return cv2.cvtColor(i.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)


def st(raw, f, sel):
    return dict(rgb=[round(float(v), 1) for v in raw[sel][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[sel].mean(0)], px=int(sel.sum()))


out = {}; bilder = []
for n, rects in REC.items():
    o = cv2.imread(f"{S}/orig/n{n}.png").astype(np.float32)
    o = np.clip((o - 17) * 255 / 238, 0, 255).astype(np.uint8)
    k = cv2.resize(cv2.imread(f"{S}/ki/n{n}.png"), (640, 480), interpolation=cv2.INTER_AREA)
    g1 = cv2.cvtColor(o, cv2.COLOR_BGR2GRAY).astype(np.float32); g2 = cv2.cvtColor(k, cv2.COLOR_BGR2GRAY).astype(np.float32)
    (dx, dy), resp = cv2.phaseCorrelate(g2[100:420, 150:500], g1[100:420, 150:500])
    M = np.float32([[1, 0, dx], [0, 1, dy]])
    tail = cv2.resize(np.load(f"{T}/tail{n}.npy").astype(np.uint8), (640, 480), interpolation=cv2.INTER_NEAREST)
    tail = cv2.warpAffine(tail, M, (640, 480), flags=cv2.INTER_NEAREST)
    te = cv2.erode(tail, np.ones((5, 5), np.uint8)) > 0
    f = lab(o)
    r = {"verschiebung_px640": [round(dx, 2), round(dy, 2)], "Schwanz": st(o, f, te)}
    vis = o.copy(); cs, _ = cv2.findContours(te.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(vis, cs, -1, (0, 255, 0), 1)
    for nm, (x0, y0, x1, y1) in rects.items():
        a = (int(round(x0 / 2 + dx)), int(round(y0 / 2 + dy)), int(round(x1 / 2 + dx)), int(round(y1 / 2 + dy)))
        sel = np.zeros((480, 640), bool); sel[a[1] + 2:a[3] - 2, a[0] + 2:a[2] - 2] = True
        r[nm] = st(o, f, sel); r[nm]["dE_zu_Schwanz"] = round(float(np.linalg.norm(np.array(r[nm]["lab"]) - np.array(r["Schwanz"]["lab"]))), 2)
        cv2.rectangle(vis, a[:2], a[2:], (0, 255, 255), 1)
    out[n] = r
    c = cv2.resize(vis[170:400, 240:490], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    cv2.putText(c, f"ORIGINAL n{n} h{n - 1350} (Schwarzwert -17)", (4, 14), 0, 0.45, (0, 255, 255), 1)
    bilder.append(c)
    print(n, json.dumps(r, ensure_ascii=False))
cv2.imwrite(BEL, np.hstack(bilder))
json.dump(out, open(f"{T}/original.json", "w"), indent=1, ensure_ascii=False)
