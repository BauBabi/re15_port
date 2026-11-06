"""Pruefer V4: Schwanzfarbe Vorlage 2 (n1435) mit Bewegungsmaske gegen den Hintergrund.
Hintergrund = Median n1449..1473 (1-basiert; Schwanz dort nicht im Fenster, Stichprobe: (281,220)/(284,235) sind dort
und in n1353-n1425 L 76-85 = heller Rahmen im Hintergrund, NICHT Schwanz). Maske |L - L_bg| > 12, 1 px erodiert,
Fenster x270-345 y195-350; Teil 'oben/Bogen' y<=300, 'unter dem Knick' y>=312. Unterseite = Chroma R-(G+B)/2 >= 6,
Aussenseite = Chroma < 6. Aufruf: python p_schwanzfarbe.py FRAMES_NPY OUTDIR"""
import sys, json, numpy as np, cv2
fr = np.load(sys.argv[1]).astype(np.float32)
f = fr[1434 - 1300]; bg = np.median(fr[1448 - 1300:1473 - 1300], axis=0)
Lf = lambda a: a @ np.array([0.299, 0.587, 0.114], np.float32)
d = np.abs(Lf(f) - Lf(bg))
C = f[..., 0] - (f[..., 1] + f[..., 2]) / 2
m = np.zeros(d.shape, np.uint8); m[195:351, 270:346] = (d[195:351, 270:346] > 12)
m = cv2.erode(m, np.ones((3, 3), np.uint8)).astype(bool)
def lab(c):
    x = np.clip(np.array(c, float).reshape(1, 1, 3) / 255.0, 0, 1).astype(np.float32)
    return [round(float(v), 1) for v in cv2.cvtColor(x, cv2.COLOR_RGB2Lab)[0, 0]]
yy, xx = np.mgrid[:480, :640]
TEILE = {"Bogen gesamt (y<=300)": m & (yy <= 300), "Bogen Unterseite (Chroma>=6)": m & (yy <= 300) & (C >= 6),
         "Bogen Aussenseite (Chroma<6)": m & (yy <= 300) & (C < 6), "unter dem Knick (y>=312)": m & (yy >= 312)}
res = {}
for k, mm in TEILE.items():
    rgb = f[mm].mean(0); r = {"px": int(mm.sum()), "rgb": rgb.round(1).tolist(), "lab": lab(rgb), "lab_sw17": lab(np.clip(rgb - 17, 0, 255)),
                              "port_ziel_rgb": np.clip((rgb - np.array([9.42, 11.96, 10.94])) / np.array([0.9979, 0.9901, 0.9964]), 0, 255).round(1).tolist()}
    r["port_ziel_lab"] = lab(r["port_ziel_rgb"]); res[k] = r; print(k, r)
# Breiten der Bewegungsmaske je Zeile (waagerecht) im senkrechten Teil und unter dem Knick
br = {int(y): int(m[y, 270:300].sum()) for y in range(256, 346, 4)}
res["waagerechte_breite_x270_299"] = br; print("Breite je Zeile", br)
json.dump(res, open(sys.argv[2] + "/P_schwanzfarbe.json", "w"), indent=1, ensure_ascii=False)
cv2.imwrite(sys.argv[2] + "/P_schwanz_maske.png", cv2.resize((m * 255).astype(np.uint8)[190:355, 265:350], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST))
