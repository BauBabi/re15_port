# Abnahme 2: Rand der Masse (Mesh 2) gemorpht (RE2-RAM s09, gruen) und UNgemorpht (EMD-Basis, magenta), RE2-Kamera 9, auf RE2 und Port
import numpy as np, sys
from PIL import Image, ImageDraw
def lese(p):
    L = open(p).read().split("\n"); n = int(L[0].split()[0])
    return np.array([[int(t) for t in L[1+k].split()] for k in range(n)], float)
E = np.array([2700,-882,-29052.]); T = np.array([-1584,-4608,-14850.]); H = 231
f = T - E; f /= np.linalg.norm(f); up = np.array([0,-1.,0]); r = np.cross(f, up); r /= np.linalg.norm(r); u = np.cross(r, f)
M2 = np.array([[4096,25,0],[-25,4096,0],[0,0,4096]], float) / 4096.0
polys = []
for z in open("sonde/spur_mesh2_poly.txt"):
    p = z.split(); idx = [int(v) for v in p[1:]]
    if p[0] == "Q": idx = [idx[0], idx[1], idx[3], idx[2]]
    polys.append(idx)
def rand(vl, col, a):
    w = (M2 @ vl.T).T + np.array([-959, 1274, -23400]); d = w - E; X, Y, Z = d @ r, d @ u, d @ f
    S = np.stack([2 * (160 + H * X / Z), 2 * (120 - H * Y / Z)], 1)
    mk = Image.new("L", (640, 480), 0); dm = ImageDraw.Draw(mk)
    for ix in polys:
        P = S[ix]; q1 = P[1]-P[0]; q2 = P[2]-P[0]
        if q1[0]*q2[1]-q1[1]*q2[0] < 0: dm.polygon([tuple(q) for q in P], fill=255)
    m = np.array(mk) > 0
    rd = m & ~(np.roll(m,1,0) & np.roll(m,-1,0) & np.roll(m,1,1) & np.roll(m,-1,1))
    a[rd] = col
bild = sys.argv[1]; re2src = sys.argv[2]; portsrc = sys.argv[3]; ziel = sys.argv[4]
outs = []
for nm, src in (("RE2 echt", re2src), ("Port Leon", portsrc)):
    a = np.array(Image.open(src).convert("RGB").resize((640, 480)))
    rand(lese("sonde/re2_v_s09.txt"), (0, 255, 0), a)
    rand(lese("sonde/spur_mesh2_basis.txt"), (255, 0, 255), a)
    im = Image.fromarray(a); ImageDraw.Draw(im).text((6, 126), nm + " Bild " + bild + ": gruen = Masse gemorpht (RE2-RAM s09), magenta = ungemorpht (Basis)", fill=(255, 255, 0))
    outs.append(im.crop((0, 120, 640, 480)))
c = Image.new("RGB", (1280, 360)); c.paste(outs[0], (0, 0)); c.paste(outs[1], (640, 0)); c.save(ziel, quality=90)
