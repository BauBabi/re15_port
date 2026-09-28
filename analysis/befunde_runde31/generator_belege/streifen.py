import sys, re
from PIL import Image, ImageDraw
# streifen.py <lauf-dir> <out.png> <frame,frame,...>
d, out, frames = sys.argv[1], sys.argv[2], [int(x) for x in sys.argv[3].split(",")]
log = {}
for ln in open(d + "/panel.log"):
    m = re.match(r"F(\d+) raum=11F0 cut=(\d+) .*wert=(\d+) geloest=(\d) strom=(\d) padsperre=(\d) msg=(\d) bestaet=(\d+)", ln)
    if m: log[int(m.group(1))] = m.groups()[1:]
W, H = 320, 240
img = Image.new("RGB", (W * len(frames), H + 28), (0, 0, 0))
dr = ImageDraw.Draw(img)
for i, f in enumerate(frames):
    im = Image.open("%s/fd%06d.ppm" % (d, f)).convert("RGB")
    if im.size != (W, H): im = im.resize((W, H))
    img.paste(im, (i * W, 28))
    cut, wert, gel, strom, ps, msg, best = log.get(f, ("?",) * 7)
    dr.text((i * W + 4, 2), "F%d cut=%s wert=%s" % (f, cut, wert), fill=(255, 255, 0))
    dr.text((i * W + 4, 14), "geloest=%s msg=%s ton=%s sperre=%s" % (gel, msg, best, ps), fill=(255, 255, 0))
img.save(out)
print(out, img.size)
