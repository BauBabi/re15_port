#!/usr/bin/env python3
"""Runde 34 / Granate: Anim-Saetze eines CORE00.ESP-Effekts als Sprite-Verbund rendern
(Teilsprites an ihren s8-Versaetzen dx/dy, Kante = Satz-Byte 3, wie FUN_800534c4), aus einem
4bpp-TIM-Schnitt von DATA/TEX.TIM (tex_tim_effect_slice.py --word1 ...).

Aufruf: python wolken_bild.py <CORE00.ESP> <effekt-id> <von> <bis> <out.png> <tim> [<tim> ...]
Jede TIM ergibt eine Zeile (z.B. CLUT 0x78D1 = sub 0x19, 0x7851 = sub 0x0B).
"""
import struct, sys, os, importlib.util
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("e", os.path.join(HERE, "esp_effekt.py"))
E = importlib.util.module_from_spec(spec); spec.loader.exec_module(E)
spec2 = importlib.util.spec_from_file_location("f", os.path.join(HERE, "flugzellen_bild.py"))
F = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(F)

def main():
    esp, eid, a, z, out = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
    tims = sys.argv[6:]
    b = open(esp, "rb").read()
    e = [x for x in E.parse(b) if x["id"] == eid][0]
    recs = []
    for i in range(a, z + 1):
        o = e["start"] + 8 + 8 * i
        r = b[o:o + 8]
        cells = []
        for c in range(r[1]):
            co = e["start"] + 8 + 8 * e["ca"] + 4 * (r[0] + c)
            cc = b[co:co + 4]
            cells.append((cc[0], cc[1], struct.unpack("b", cc[2:3])[0], struct.unpack("b", cc[3:4])[0]))
        recs.append((i, r[3], cells))
    CW = 96
    rows = []
    for t in tims:
        clut, idx, W, H = F.load(t)
        img = Image.new("RGBA", (CW * len(recs), CW), (40, 40, 40, 255))
        for k, (i, S, cells) in enumerate(recs):
            cx, cy = k * CW + CW // 2, CW // 2
            for (u, v, dx, dy) in cells:
                for y in range(S):
                    for x in range(S):
                        if v + y >= H or u + x >= W:
                            continue
                        c = clut[idx[v + y][u + x]]
                        if c:
                            img.putpixel((cx + dx + x, cy + dy + y), F.rgba(c))
        rows.append(img)
    sc = 3
    outimg = Image.new("RGBA", (rows[0].width * sc, (CW * sc + 8) * len(rows)), (0, 0, 0, 255))
    for n, im in enumerate(rows):
        outimg.paste(im.resize((im.width * sc, im.height * sc), Image.NEAREST), (0, n * (CW * sc + 8)))
    outimg.save(out)
    print("geschrieben", out, outimg.size, "Saetze", a, "..", z)

if __name__ == "__main__":
    main()
