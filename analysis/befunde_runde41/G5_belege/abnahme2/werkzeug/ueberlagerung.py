# Abnahme 2: RE2-Geometrie aus dem RAM (Savestate s09: Mesh 0 Welt-Vertexpuffer 0x80132588, Mesh 2 Teil-2-lokal 0x80132be0 + Teil-2-Matrix/Lage)
# mit der RE2-Kamera Schnitt 9 (RID @0x184: Auge (2700,-882,-29052), Ziel (-1584,-4608,-14850), H 231) projiziert und auf RE2- und Port-Bild gelegt.
import numpy as np, sys
from PIL import Image, ImageDraw
def lese(p):
    L = open(p).read().split("\n"); n = int(L[0].split()[0])
    return np.array([[int(t) for t in L[1+k].split()] for k in range(n)], float)
m0 = lese("sonde/re2_m0_s09.txt")
m2l = lese("sonde/re2_v_s09.txt")
M2 = np.array([[4096,25,0],[-25,4096,0],[0,0,4096]], float) / 4096.0
m2 = (M2 @ m2l.T).T + np.array([-959, 1274, -23400])
E = np.array([2700,-882,-29052.]); T = np.array([-1584,-4608,-14850.]); H = 231
f = T - E; f /= np.linalg.norm(f); up = np.array([0,-1.,0]); r = np.cross(f, up); r /= np.linalg.norm(r); u = np.cross(r, f)
def proj(P):
    d = P - E; x, y, z = d @ r, d @ u, d @ f
    return np.stack([160 + H * x / z, 120 - H * y / z, z], 1)
out = []
for nm, src, dy in (("RE2 echt Bild ~313", "re2/bilder/re2_0413.png", 0), ("Port Leon Bild 313", "lauf/leon_steh/bilder/port_0413.png", 0)):
    im = Image.open(src).convert("RGB").resize((640, 480)); dr = ImageDraw.Draw(im)
    for P, col in ((m2, (0, 255, 0)), (m0, (0, 200, 255))):
        for x, y, z in proj(P):
            if z > 100: dr.ellipse((2*x-1.5, 2*y-1.5+dy, 2*x+1.5, 2*y+1.5+dy), fill=col)
    dr.text((6, 6), nm + "  gruen = Mesh 2 (Masse), blau = Mesh 0 (Kopf/Rumpf), RE2-RAM s09 projiziert", fill=(255, 255, 0))
    out.append(im)
c = Image.new("RGB", (1280, 480)); c.paste(out[0], (0, 0)); c.paste(out[1], (640, 0)); c.save(sys.argv[1], quality=90)
