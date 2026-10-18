#!/usr/bin/env python3
"""grundriss_10f1.py - Runde 39 Spur I: Draufsicht ROOM10F1 (= ROOM10F0, bytegleich) aus der RDT.

Zeichnet die Kollisionszellen (SCA @RDT+0x20, Satz 12 B: w d x z typ u0 u1 floor — Leser wie
tools/r39_treffer_zellen_bild.py), die Text-/Item-/Tuer-Zonen aus main00 (scd_dump_room.py), die
Kamerapositionen (RDT+0x24, 32 B je Cut, Position @+4, Ziel @+0x10) und frei waehlbare Wegpunkte.
Achsen: x nach rechts, z nach OBEN (Norden). Gierung 0 = +X, 1024 = -Z, 2048 = -X, 3072 = +Z.

Aufruf: grundriss_10f1.py <out.png> [x,z[,name] ...]
"""
import os, struct, sys
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
RDT = os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM10F1.RDT")
sys.path.insert(0, os.path.join(PORT, "tools"))


def main():
    out = sys.argv[1]
    wege = []
    for a in sys.argv[2:]:
        t = a.split(",")
        wege.append((int(t[0]), int(t[1]), t[2] if len(t) > 2 else ""))
    d = open(RDT, "rb").read()
    sca = struct.unpack_from("<I", d, 0x20)[0]
    cnt = struct.unpack_from("<5I", d, sca + 4)
    zellen = [struct.unpack_from("<HHhhBBBB", d, sca + 24 + i * 12) for i in range(cnt[0])]
    cam_off = struct.unpack_from("<I", d, 0x24)[0]
    ncut = d[1] if False else 12
    kams = []
    for c in range(ncut):
        rec = cam_off + c * 32
        P = struct.unpack_from("<iii", d, rec + 4)
        T = struct.unpack_from("<iii", d, rec + 0x10)
        kams.append((c, P, T))
    X0, X1, Z0, Z1 = -10000, 10000, -6000, 16000
    S_ = 0.04
    W, H = int((X1 - X0) * S_), int((Z1 - Z0) * S_)
    im = Image.new("RGB", (W, H), (250, 250, 250))
    g = ImageDraw.Draw(im)
    def p(x, z):
        return ((x - X0) * S_, (Z1 - z) * S_)
    for x in range(-10000, 10001, 1000):
        g.line([p(x, Z0), p(x, Z1)], fill=(230, 230, 230))
    for z in range(-6000, 16001, 1000):
        g.line([p(X0, z), p(X1, z)], fill=(230, 230, 230))
    for (w, dd, x, z, t, u0, u1, fl) in zellen:
        g.rectangle([p(x, z + dd), p(x + w, z)], outline=(60, 60, 60), fill=(200, 200, 205))
    # Zonen aus main00 (scd_dump_room-Zeilen, als Unterprozess)
    import subprocess
    txt = subprocess.run([sys.executable, os.path.join(PORT, "tools", "scd_dump_room.py"), RDT],
                         capture_output=True, text=True).stdout
    import re
    for z in txt.splitlines():
        m = re.search(r"(Aot_set|Door_aot_set|Item_aot_set)\s.*slot=(\d+) sce=(\d+).*rect=\((-?\d+),(-?\d+),(\d+),(\d+)\)", z)
        if not m or "sub" in z[:3]:
            continue
        op, slot, sce, x, zz, w, dd = m.group(1), int(m.group(2)), int(m.group(3)), *[int(m.group(i)) for i in range(4, 8)]
        col = {"Aot_set": (0, 120, 220), "Door_aot_set": (220, 40, 40), "Item_aot_set": (0, 160, 60)}[op]
        g.rectangle([p(x, zz + dd), p(x + w, zz)], outline=col, width=2)
        g.text((p(x, zz + dd)[0] + 2, p(x, zz + dd)[1] + 2), "s%d" % slot, fill=col)
    for (c, P, T) in kams:
        a, b = p(P[0], P[2]), p(T[0], T[2])
        g.ellipse([a[0] - 4, a[1] - 4, a[0] + 4, a[1] + 4], fill=(240, 140, 0))
        g.line([a, (a[0] + (b[0] - a[0]) * 0.15, a[1] + (b[1] - a[1]) * 0.15)], fill=(240, 140, 0), width=2)
        g.text((a[0] + 5, a[1] - 5), "C%d" % c, fill=(200, 100, 0))
    for (x, z, n) in wege:
        a = p(x, z)
        g.ellipse([a[0] - 5, a[1] - 5, a[0] + 5, a[1] + 5], fill=(200, 0, 200))
        g.text((a[0] + 6, a[1] + 2), n or "(%d,%d)" % (x, z), fill=(150, 0, 150))
    im.save(out)
    print(out, im.size, "Zellen", len(zellen))


if __name__ == "__main__":
    main()
