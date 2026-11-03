import numpy as np
eye = np.array([2700,-882,-29052.]); at = np.array([-1584,-4608,-14850.]); H = 231
f = at - eye; f /= np.linalg.norm(f)
up = np.array([0,1.,0])   # PSX: y nach unten
r = np.cross(up, f); r /= np.linalg.norm(r)   # Bild-x
u = np.cross(f, r)
def proj(p):
    d = np.array(p, float) - eye
    x, y, z = d @ r, d @ u, d @ f
    return 160 + H*x/z, 120 + H*y/z, z
print("Tuermitte", proj([-5683,-1500,-23482]))
print("Nordpfosten unten/oben", proj([-5683,0,-21990]), proj([-5683,-3000,-21990]))
for x0 in range(-1000, -20001, -1000):
    c = proj([x0,-4400,-16800])
    l = proj([x0+2549,-4400,-16800]); rr = proj([x0-2549,-4400,-16800])
    print("x0 %6d Lampe sx %6.1f sy %6.1f z %6.0f | Wand x %6.1f..%6.1f" % (x0, c[0], c[1], c[2], l[0], rr[0]))
