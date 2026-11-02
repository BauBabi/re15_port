# Abnahme 2: Rand der Masse gemorpht (RE2-RAM) gruen / ungemorpht (EMD-Basis) magenta mit RE2-Kamera auf RE2- und Port-Bild
import numpy as np, sys
from PIL import Image, ImageDraw
sav, bild, kam, p2, re2src, portsrc, ziel = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], sys.argv[7]
K = {"9": ((2700,-882,-29052), (-1584,-4608,-14850), 231), "10": ((8946,-936,-25902), (-1710,-3546,-21690), 208)}[kam]
P2 = np.array([float(v) for v in p2.split(",")])
def lese(p):
    L = open(p).read().split("\n"); n = int(L[0].split()[0])
    return np.array([[int(t) for t in L[1+k].split()] for k in range(n)], float)
E = np.array(K[0], float); T = np.array(K[1], float); H = K[2]
f = T - E; f /= np.linalg.norm(f); up = np.array([0,-1.,0]); r = np.cross(f, up); r /= np.linalg.norm(r); u = np.cross(r, f)
polys = []
for z in open("sonde/spur_mesh2_poly.txt"):
    p = z.split(); idx = [int(v) for v in p[1:]]
    if p[0] == "Q": idx = [idx[0], idx[1], idx[3], idx[2]]
    polys.append(idx)
def rand(vl, col, a):
    w = vl + P2; d = w - E; X, Y, Z = d @ r, d @ u, d @ f
    S = np.stack([2 * (160 + H * X / Z), 2 * (120 - H * Y / Z)], 1)
    mk = Image.new("L", (640, 480), 0); dm = ImageDraw.Draw(mk)
    for ix in polys:
        if np.any(Z[ix] < 50): continue
        P = S[ix]; q1 = P[1]-P[0]; q2 = P[2]-P[0]
        if q1[0]*q2[1]-q1[1]*q2[0] < 0: dm.polygon([tuple(q) for q in P], fill=255)
    m = np.array(mk) > 0
    rd = m & ~(np.roll(m,1,0) & np.roll(m,-1,0) & np.roll(m,1,1) & np.roll(m,-1,1))
    a[rd] = col
outs = []
for nm, src in (("RE2 echt", re2src), ("Port Leon", portsrc)):
    a = np.array(Image.open(src).convert("RGB").resize((640, 480)))
    rand(lese("sonde/re2_v_%s.txt" % sav), (0, 255, 0), a)
    rand(lese("sonde/spur_mesh2_basis.txt"), (255, 0, 255), a)
    im = Image.fromarray(a); ImageDraw.Draw(im).text((6, 6), "%s Bild %s (RE2-RAM %s): gruen = Masse gemorpht, magenta = ungemorpht (EMD-Basis)" % (nm, bild, sav), fill=(255, 255, 0))
    outs.append(im)
c = Image.new("RGB", (1280, 480)); c.paste(outs[0], (0, 0)); c.paste(outs[1], (640, 0)); c.save(ziel, quality=90)
