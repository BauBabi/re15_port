# Abnahme 8 (eigenes Werkzeug): Bildleiste aus einer RE15_FRAMEDUMP-Serie (fd_NNNNNN.ppm = Bildzaehler des Raums) des letzten Besuchs.
# Beschriftung je Bild aus state.log + climb.log (zeilengleich): F, Figur (x,y,z), Spieler-Zustand pst/ps1 + Clip, hp, Gator-Clip/Bild.
# Aufruf: python lst8.py <lauf> <aus.jpg> <titel> <F1> <F2> ...   (env HELL = Aufhellung, SPALTEN = Spalten, Standard 4)
import sys, os, re
from PIL import Image, ImageDraw, ImageEnhance
run, aus, titel = sys.argv[1], sys.argv[2], sys.argv[3]
Fs = [int(f) for f in sys.argv[4:]]
hell = float(os.environ.get('HELL', '1')); C = int(os.environ.get('SPALTEN', '4'))
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+)')
rg = re.compile(r' mo=(\d+) af=(\d+) stun=(\d+) d=\S+ @\((-?\d+),(-?\d+),r(-?\d+)\)')
rc = re.compile(r'^T(\d+) room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\).*?band=(-?\d+)')
sl = open(os.path.join(run, 'state.log'), encoding='latin-1').read().splitlines()
cl = open(os.path.join(run, 'climb.log'), encoding='latin-1').read().splitlines()
start = 0; prev = None
for i, l in enumerate(sl):
    m = rs.match(l)
    if not m: continue
    f = int(m.group(1))
    if prev is not None and f <= prev: start = i
    prev = f
info = {}
for i in range(start, len(sl)):
    m = rs.match(sl[i]); g = rg.search(sl[i]); c = rc.match(cl[i]) if i < len(cl) else None
    if not m: continue
    F = int(m.group(1))
    info[F] = 'F%d (%s,%s,%s) b%s pst%s/%s mo%s hp%s | G mo%s af%s' % (F, m.group(2), c.group(4) if c else '?', m.group(3), c.group(6) if c else '?',
              m.group(6), m.group(7), m.group(9), m.group(5), g.group(1) if g else '-', g.group(2) if g else '-')
W, H = 320, 240
R = (len(Fs) + C - 1) // C
img = Image.new('RGB', (C * (W + 4) + 4, 22 + R * (H + 18)), (16, 16, 16))
d = ImageDraw.Draw(img)
d.text((4, 4), titel, fill=(235, 235, 235))
for i, F in enumerate(Fs):
    p = os.path.join(run, 'fd_%06d.ppm' % F)
    x = 4 + (i % C) * (W + 4); y = 22 + (i // C) * (H + 18)
    if os.path.exists(p):
        im = Image.open(p).convert('RGB').resize((W, H))
        if hell != 1: im = ImageEnhance.Brightness(im).enhance(hell)
        img.paste(im, (x, y))
    else:
        d.text((x + 8, y + 100), 'fehlt: fd_%06d.ppm' % F, fill=(255, 80, 80))
    d.text((x, y + H + 2), info.get(F, 'F%d ?' % F), fill=(255, 230, 0))
img.save(aus, quality=88)
print(aus, img.size)
