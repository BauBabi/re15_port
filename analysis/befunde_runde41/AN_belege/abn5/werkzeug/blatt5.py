#!/usr/bin/env python3
"""blatt5.py - Abnahme 5 Spur AN: Bildleiste aus einer RE15_FRAMEDUMP-Serie (f<NNNNNN>.ppm) eines Laufordners. Je Feld die
Bildnummer und (falls im state.log des letzten Raumabschnitts vorhanden) Spielerlage und Abstand zur 0x4D.
  python blatt5.py <laufdir> <ausgabe.jpg> <spalten> <bild1,bild2,...|alle|von-bis/schritt> [feldbreite=320] [titel]
  Umgebung BLATT_LABEL = Name der Spielfigur in der Feldzeile (Vorgabe Leon)."""
import sys, os, glob, re, math
from PIL import Image, ImageDraw, ImageFont
d, out, cols, wahl = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
fw = int(sys.argv[5]) if len(sys.argv) > 5 else 320
titel = sys.argv[6] if len(sys.argv) > 6 else ""
files = sorted(glob.glob(os.path.join(d, "f*.ppm")))
nr = {int(re.search(r"f(\d+)\.ppm$", f).group(1)): f for f in files}
if wahl == "alle": sel = sorted(nr)
elif "-" in wahl and "/" in wahl:
    a, rest = wahl.split("-"); b, s = rest.split("/"); sel = [k for k in range(int(a), int(b) + 1, int(s)) if k in nr]
else: sel = [int(x) for x in wahl.split(",") if int(x) in nr]
if not sel: sys.exit("keine Bilder")
# state.log: letzter Abschnitt
info = {}
sl = os.path.join(d, "state.log")
if os.path.exists(sl):
    segs, cur, last = [], [], -1
    for l in open(sl, errors="replace"):
        if not l.startswith("F"): continue
        f = int(l.split(" ", 1)[0][1:])
        if f < last: segs.append(cur); cur = []
        last = f; cur.append((f, l))
    segs.append(cur)
    pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
    an = re.compile(r"\[(\d+) t=4d [^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
    for f, l in segs[-1]:
        p, a = pl.search(l), an.search(l)
        if p:
            s = "%s (%s,%s)" % (os.environ.get("BLATT_LABEL", "Leon"), p.group(1), p.group(2))
            if a: s += " 0x4D %.0f" % math.hypot(int(p.group(1)) - int(a.group(2)), int(p.group(2)) - int(a.group(3)))
            info[f] = s
im0 = Image.open(nr[sel[0]]); w, h = im0.size; fh = int(h * fw / w)
rows = (len(sel) + cols - 1) // cols
top = 22 if titel else 0
sheet = Image.new("RGB", (cols * fw, top + rows * (fh + 18)), (20, 20, 20))
dr = ImageDraw.Draw(sheet)
try: font = ImageFont.truetype("arial.ttf", 13)
except Exception: font = ImageFont.load_default()
if titel: dr.text((6, 4), titel, fill=(255, 255, 255), font=font)
for i, f in enumerate(sel):
    im = Image.open(nr[f]).convert("RGB").resize((fw, fh))
    x, y = (i % cols) * fw, top + (i // cols) * (fh + 18)
    sheet.paste(im, (x, y + 18))
    dr.text((x + 4, y + 2), "F%d %s" % (f, info.get(f, "")), fill=(255, 255, 0), font=font)
sheet.save(out, quality=85)
print(out, len(sel), "Bilder", sheet.size)
