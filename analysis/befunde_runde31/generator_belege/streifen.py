import sys, re
from PIL import Image, ImageDraw
# streifen.py <lauf-dir> <out.png> <frame,frame,...>
# Oben das volle Bild (RE15_FRAMEDUMP), darunter die Skala (x 250..300, y 45..185) dreifach.
d, out, frames = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(",")]
log = {}
for ln in open(d + "/panel.log"):
    m = re.match(r"F(\d+) raum=11F0 cut=(\d+) .*wert=(\d+) geloest=(\d) strom=(\d) padsperre=(\d) msg=(\d) bestaet=(\d+)(?: ruhe=(\d+) panelsperre=(\d))?", ln)
    if m: log[int(m.group(1))] = tuple("-" if x is None else x for x in m.groups()[1:])
W, H = 320, 240
Z = (250, 45, 300, 185); ZF = 3
zw, zh = (Z[2] - Z[0]) * ZF, (Z[3] - Z[1]) * ZF
img = Image.new("RGB", (W * len(frames), 40 + H + zh), (0, 0, 0))
dr = ImageDraw.Draw(img)
for i, f in enumerate(frames):
    im = Image.open("%s/fd%06d.ppm" % (d, f)).convert("RGB")
    if im.size != (W, H): im = im.resize((W, H))
    img.paste(im, (i * W, 40))
    img.paste(im.crop(Z).resize((zw, zh), Image.NEAREST), (i * W + (W - zw) // 2, 40 + H))
    g = log.get(f, ("?",) * 9)
    cut, wert, gel, strom, ps, msg, best, ruhe, pans = g
    dr.text((i * W + 4, 2), "F%d cut=%s wert=%s ruhe=%s" % (f, cut, wert, ruhe), fill=(255, 255, 0))
    dr.text((i * W + 4, 14), "geloest=%s msg=%s ton=%s strom=%s" % (gel, msg, best, strom), fill=(255, 255, 0))
    dr.text((i * W + 4, 26), "sperre: pad=%s panel=%s" % (ps, pans), fill=(255, 255, 0))
img.save(out)
print(out, img.size)
