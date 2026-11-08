# Leser B v3: Farbvergleich Schwanz gegen Koerperteile (KI-Video 4 _slp, Seitenansicht).
# Schwanzmaske aus b_schwanz.py (tail<n>.npy), 2 px erodiert (5x5). Koerperteile als Rechtecke INNERHALB der Teile
# (an der Lupe g<n>.png gewaehlt, im Belegbild eingezeichnet), zusaetzlich 2 px erodiert.
# Lab = OpenCV float Lab (sRGB, D65), L 0..100. Delta-E = CIE76. "rot" = HSV-Farbton <= 20 oder >= 340 Grad, Saettigung >= 90/255.
# Aufruf: python b_farben.py <sfv3-dir> <schwanz-dir> <beleg.png>
import sys, json, cv2, numpy as np
S, T, BEL = sys.argv[1], sys.argv[2], sys.argv[3]
RECHTECKE = {  # n: {teil: (x0,y0,x1,y1)}
    1435: {"Ruecken/Buckel": (770, 395, 840, 460), "Oberarm vorn (W-rechts)": (705, 510, 750, 560),
           "Bein (Unterschenkel)": (825, 640, 855, 700), "Jeans": (800, 580, 840, 620)},
    1438: {"Ruecken/Buckel": (770, 395, 840, 455), "Oberarm vorn (W-rechts)": (705, 510, 750, 555)},
    1442: {"Ruecken/Buckel": (750, 385, 810, 440), "Oberarm vorn (W-rechts)": (690, 485, 735, 540),
           "Bein (Unterschenkel)": (825, 650, 855, 710), "Jeans": (760, 585, 830, 620), "Sichel": (650, 650, 690, 740)},
    1445: {"Ruecken/Buckel": (750, 385, 810, 440), "Sichel": (650, 650, 690, 740)},
}


def lab(i):
    return cv2.cvtColor(i.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)


def rot(raw):
    hsv = cv2.cvtColor(raw, cv2.COLOR_BGR2HSV); h = hsv[..., 0].astype(int) * 2
    return ((h <= 20) | (h >= 340)) & (hsv[..., 1] >= 90)


def stat(raw, f, sel):
    return dict(rgb=[round(float(v), 1) for v in raw[sel][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[sel].mean(0)],
                L_sd=round(float(f[sel][:, 0].std()), 2), rot=round(float(rot(raw)[sel].mean()), 3), px=int(sel.sum()))


out = {}
bilder = []
for n, rects in RECHTECKE.items():
    raw = cv2.imread(f"{S}/ki/n{n}.png"); f = lab(raw)
    tail = np.load(f"{T}/tail{n}.npy"); te = cv2.erode(tail, np.ones((5, 5), np.uint8)) > 0
    r = {"Schwanz": stat(raw, f, te)}
    o = raw.copy(); cs, _ = cv2.findContours(te.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(o, cs, -1, (0, 255, 0), 1)
    for k, (x0, y0, x1, y1) in rects.items():
        sel = np.zeros(tail.shape, bool); sel[y0 + 2:y1 - 2, x0 + 2:x1 - 2] = True
        r[k] = stat(raw, f, sel)
        cv2.rectangle(o, (x0, y0), (x1, y1), (0, 255, 255), 1); cv2.putText(o, k.split(" ")[0][:7], (x0, y0 - 2), 0, 0.33, (0, 255, 255), 1)
    ts = np.array(r["Schwanz"]["lab"])
    for k in rects:
        r[k]["dE_zu_Schwanz"] = round(float(np.linalg.norm(np.array(r[k]["lab"]) - ts)), 2)
    out[n] = r
    cv2.putText(o, f"KI n{n} h{n - 1350}: gruen = Schwanz (2px erodiert), gelb = Vergleichsflaechen", (500, 365), 0, 0.4, (0, 255, 255), 1)
    bilder.append(o[350:800, 500:980])
    print(n, json.dumps(r, ensure_ascii=False))
cv2.imwrite(BEL, np.vstack([np.hstack(bilder[:2]), np.hstack(bilder[2:4])]))
json.dump(out, open(f"{T}/farben.json", "w"), indent=1, ensure_ascii=False)
