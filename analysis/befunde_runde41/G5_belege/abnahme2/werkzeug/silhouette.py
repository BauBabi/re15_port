# Abnahme 2: Silhouette der Masse (Mesh 2, RE2-RAM s09) mit RE2-Kamera Schnitt 9, Maler-Sortierung nach Tiefe,
# Vorderseiten (PSX NCLIP: Bildschirm-Umlauf) gelb, Rueckseiten blau - gegen RE2 und Port gelegt.
import numpy as np, sys
from PIL import Image, ImageDraw
def lese(p):
    L = open(p).read().split("\n"); n = int(L[0].split()[0])
    return np.array([[int(t) for t in L[1+k].split()] for k in range(n)], float)
m2l = lese("sonde/re2_v_s09.txt")
M2 = np.array([[4096,25,0],[-25,4096,0],[0,0,4096]], float) / 4096.0
m2 = (M2 @ m2l.T).T + np.array([-959, 1274, -23400])
E = np.array([2700,-882,-29052.]); T = np.array([-1584,-4608,-14850.]); H = 231
f = T - E; f /= np.linalg.norm(f); up = np.array([0,-1.,0]); r = np.cross(f, up); r /= np.linalg.norm(r); u = np.cross(r, f)
d = m2 - E; X, Y, Z = d @ r, d @ u, d @ f
S = np.stack([2 * (160 + H * X / Z), 2 * (120 - H * Y / Z)], 1); Zs = Z
polys = []
for z in open("sonde/spur_mesh2_poly.txt"):
    p = z.split()
    idx = [int(v) for v in p[1:]]
    if p[0] == "Q": idx = [idx[0], idx[1], idx[3], idx[2]]      # PSX-Quad-Reihenfolge 0,1,3,2 als Umriss
    polys.append(idx)
def zeichne(img, modus):
    dr = ImageDraw.Draw(img, "RGBA")
    order = sorted(polys, key=lambda ix: -np.mean(Zs[ix]))
    for ix in order:
        P = S[ix]
        a = P[1] - P[0]; b = P[2] - P[0]; cr = a[0] * b[1] - a[1] * b[0]
        vorn = cr < 0
        if modus == "vorn" and not vorn: continue
        dr.polygon([tuple(q) for q in P], fill=(255, 220, 0, 90) if vorn else (0, 120, 255, 90), outline=None)
    return img
out = []
for nm, src in (("RE2 echt", "re2/bilder/re2_0413.png"), ("Port Leon", "lauf/leon_steh/bilder/port_0413.png")):
    for modus in ("vorn", "alle"):
        im = Image.open(src).convert("RGB").resize((640, 480))
        zeichne(im, modus)
        ImageDraw.Draw(im).text((6, 6), "%s Bild 313 - Mesh 2 aus RE2-RAM s09, %s" % (nm, "nur Umlauf A" if modus == "vorn" else "Umlauf A gelb / B blau"), fill=(255, 255, 0))
        out.append(im)
c = Image.new("RGB", (1280, 960))
for i, im in enumerate(out): c.paste(im, ((i % 2) * 640, (i // 2) * 480))
c.save(sys.argv[1], quality=88)
