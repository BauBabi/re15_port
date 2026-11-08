# r7.py <laufordner> <aus.jpg> <titel> F1 F2 ...: Bildleiste aus einer RE15_FRAMEDUMP-Serie (fd_NNNNNN.ppm) des letzten Besuchs, 4 Spalten je 320x240,
# Beschriftung je Bild: F, Figur (x,y,z), Spieler-Clip mo, hp (state.log + climb.log Zeile fuer Zeile, letzter Besuch). Aufhellung env HELL (PIL Brightness).
import sys, os, re
from PIL import Image, ImageDraw, ImageEnhance
run, aus, titel = sys.argv[1], sys.argv[2], sys.argv[3]
Fs = [int(f) for f in sys.argv[4:]]
hell = float(os.environ.get('HELL', '1'))
rs = re.compile(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+) ")
rc = re.compile(r"T(\d+) room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\)")
sl = [l for l in open(os.path.join(run, 'state.log'), errors='replace') if l.startswith('F')]
cl = [l for l in open(os.path.join(run, 'climb.log'), errors='replace') if l.startswith('T')]
start = 0
prev = None
for i, l in enumerate(sl):
    f = int(l[1:l.index(' ')])
    if prev is not None and f < prev:
        start = i
    prev = f
st = {}
for i in range(start, len(sl)):
    m = rs.match(sl[i])
    c = rc.match(cl[i]) if i < len(cl) else None
    if m:
        st[int(m.group(1))] = (m.group(2), c.group(4) if c else '?', m.group(3), m.group(6), m.group(5))
W, H, C = 320, 240, 4
R = (len(Fs) + C - 1) // C
img = Image.new('RGB', (C * (W + 4) + 4, 22 + R * (H + 18)), (20, 20, 20))
d = ImageDraw.Draw(img)
d.text((4, 4), titel, fill=(230, 230, 230))
for i, F in enumerate(Fs):
    p = os.path.join(run, 'fd_%06d.ppm' % F)
    x = 4 + (i % C) * (W + 4)
    y = 22 + (i // C) * (H + 18)
    if os.path.exists(p):
        im = Image.open(p).convert('RGB').resize((W, H))
        if hell != 1:
            im = ImageEnhance.Brightness(im).enhance(hell)
        img.paste(im, (x, y))
    s = st.get(F)
    lab = 'F%d' % F + (' Figur (%s,%s,%s) mo=%s hp=%s' % s if s else '')
    d.text((x, y + H + 2), lab, fill=(255, 255, 0))
img.save(aus, quality=88)
print(aus, img.size)
