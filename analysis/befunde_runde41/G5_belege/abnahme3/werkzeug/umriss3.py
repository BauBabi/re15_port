# Abnahme 3: eigene Umriss-Probe. Masse Mesh 2 aus RE2-RAM (re2_ram.py: Vertexpuffer 0x80132be0 + Teil-2-Weltlage), Kamera aus RID
# ROOM7040 (@0x184 Schnitt 9 / @0x1A4 Schnitt 10, selbst gelesen), Topologie aus der EMD (spur_mesh2_poly.txt), Grundform spur_mesh2_basis.txt.
# Ausgabe: Bild RE2 | Leon | Elza mit Rand gruen (gemorpht) / magenta (Grundform) + Flaechenmasse je Region.
import numpy as np, sys
from PIL import Image, ImageDraw
K = {9: ((2700,-882,-29052), (-1584,-4608,-14850), 231), 10: ((8946,-936,-25902), (-1710,-3546,-21690), 208)}
def lese_ram(ss):
    L = open("re2ram_v_s%s.txt" % ss).read().split("\n")
    w = np.array([float(t) for t in L[1].split()])
    return np.array([[int(t) for t in L[2 + k].split()] for k in range(635)], float), w, L[0]
basis = np.array([[int(t) for t in z.split()] for z in open("spur_mesh2_basis.txt").read().split("\n")[1:636]], float)
polys = []
for z in open("spur_mesh2_poly.txt"):
    p = z.split(); idx = [int(v) for v in p[1:]]
    if p[0] == "Q": idx = [idx[0], idx[1], idx[3], idx[2]]
    polys.append(idx)
def maske(vl, P2, kam):
    E = np.array(K[kam][0], float); T = np.array(K[kam][1], float); H = K[kam][2]
    f = T - E; f /= np.linalg.norm(f); up = np.array([0, -1., 0]); r = np.cross(f, up); r /= np.linalg.norm(r); u = np.cross(r, f)
    d = vl + P2 - E; X, Y, Z = d @ r, d @ u, d @ f
    S = np.stack([2 * (160 + H * X / Z), 2 * (120 - H * Y / Z)], 1)
    mk = Image.new("L", (640, 480), 0); dm = ImageDraw.Draw(mk)
    for ix in polys:
        if np.any(Z[ix] < 50): continue
        P = S[ix]; q1 = P[1] - P[0]; q2 = P[2] - P[0]
        if q1[0] * q2[1] - q1[1] * q2[0] < 0: dm.polygon([tuple(q) for q in P], fill=255)
    return np.array(mk) > 0
def rand(m): return m & ~(np.roll(m, 1, 0) & np.roll(m, -1, 0) & np.roll(m, 1, 1) & np.roll(m, -1, 1))
bild, ss, kam = int(sys.argv[1]), sys.argv[2], int(sys.argv[3])
vm, P2, kopf = lese_ram(ss)
M = maske(vm, P2, kam); B = maske(basis, P2, kam)
A = M & ~B; C = B & ~M
quellen = [("RE2 echt", "re2bilder/re2_%+05d.png" % bild), ("Port Leon 5090", "lauf/fd_leon/fd/f%06d.ppm" % (1613 + bild)),
           ("Port Elza 5091", "lauf/fd_elza/fd/f%06d.ppm" % (1613 + bild))]
bilder = [np.array(Image.open(p).convert("RGB").resize((640, 480), Image.BILINEAR)).astype(float) for _, p in quellen]
print("Bild %d RE2-RAM s%s (%s) Teil2 %s: |M|=%d |B|=%d nur-gemorpht %d nur-Grundform %d" % (bild, ss, kopf, P2.astype(int).tolist(), M.sum(), B.sum(), A.sum(), C.sum()))
for (nm, _), a in zip(quellen, bilder):
    lum = a.mean(2)
    print("  %-15s L(nur gemorpht) %5.1f  L(nur Grundform) %5.1f  L(beide) %5.1f  RGB(nur gemorpht) %s RGB(nur Grundform) %s" % (
        nm, lum[A].mean() if A.any() else -1, lum[C].mean() if C.any() else -1, lum[M & B].mean(),
        a[A].mean(0).astype(int).tolist() if A.any() else "-", a[C].mean(0).astype(int).tolist() if C.any() else "-"))
r2 = bilder[0]
for (nm, _), a in list(zip(quellen, bilder))[1:]:
    print("  %-15s |Port-RE2| nur gemorpht %5.1f, nur Grundform %5.1f, ganze Masse %5.1f, Bild ausserhalb %5.1f" % (nm,
        np.abs(a - r2)[A].mean() if A.any() else -1, np.abs(a - r2)[C].mean() if C.any() else -1, np.abs(a - r2)[M | B].mean(), np.abs(a - r2)[~(M | B)].mean()))
outs = []
for (nm, _), a in zip(quellen, bilder):
    a = a.astype(np.uint8).copy(); a[rand(M)] = (0, 255, 0); a[rand(B)] = (255, 0, 255)
    im = Image.fromarray(a); ImageDraw.Draw(im).text((6, 6), "%s  Bild %d  (RE2-RAM s%s)  gruen = gemorpht (RE2-RAM), magenta = Grundform" % (nm, bild, ss), fill=(255, 255, 0))
    outs.append(im)
c = Image.new("RGB", (1920, 480)); [c.paste(o, (640 * i, 0)) for i, o in enumerate(outs)]
c.save("umriss/umriss_bild%03d.jpg" % bild, quality=88)
