# Kontaktblatt aus einer RE15_FRAMEDUMP-Serie (eigenes Werkzeug der Abnahme 2).
#   python blatt.py <laufdir> <ausgabe.png> <spalten> <skala> <bild1,bild2,...|von-bis/schritt> [titel]
# Beschriftung je Kachel: F<n>, cam, Form-Zustand (st/ss1/mo/af/hp) aus state.log, Spieler-HP.
import re, sys, os
from PIL import Image, ImageDraw, ImageFont
d, out, cols, sk, sel = sys.argv[1], sys.argv[2], int(sys.argv[3]), float(sys.argv[4]), sys.argv[5]
titel = sys.argv[6] if len(sys.argv) > 6 else os.path.basename(d.rstrip('/'))
if '-' in sel and '/' in sel:
    a, rest = sel.split('-'); b, st = rest.split('/')
    frames = list(range(int(a), int(b) + 1, int(st)))
else:
    frames = [int(x) for x in sel.split(',')]
RX_PL = re.compile(r'^F(\d+) pad=[0-9A-Fa-f]+ PL\((-?\d+),(-?\d+),rot=-?\d+,hp=(-?\d+)\).*? cam=(\d+)')
RX_EN = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=[0-9a-f]+ mo=(\d+) af=(\d+) [^]]*@\((-?\d+),(-?\d+),r-?\d+\)\] hp=(-?\d+)')
RX_MS = re.compile(r' hk=(\d+)/\d+ io=\d+ hr=[0-9a-f]+ ms=(\d+)')
info = {}
seg, last = 0, -1
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = RX_PL.match(line)
    if not m: continue
    f = int(m.group(1))
    if f <= last: seg += 1
    last = f
    t = 'cam%s pl%s' % (m.group(5), m.group(4))
    for e in RX_EN.finditer(line):
        if e.group(2) == '30':
            t += ' FORM st%s s%s mo%s/%s x%s hp%s' % (e.group(3), e.group(4), e.group(6), e.group(7), e.group(8), e.group(10)); break
        if e.group(2) == '36':
            t += ' G5 st%s s%s hp%s' % (e.group(3), e.group(4), e.group(10)); break
    ms = RX_MS.search(line)
    if ms: t += ' hk%s ms%s' % (ms.group(1), ms.group(2))
    info[f] = t          # letzte Segment-Fassung gewinnt (Raum-Segment)
W, H = int(640 * sk), int(480 * sk)
tiles = []
for f in frames:
    p = os.path.join(d, 'fd', 'f%06d.ppm' % f)
    if os.path.exists(p): tiles.append((f, Image.open(p).convert('RGB').resize((W, H), Image.LANCZOS)))
rows = (len(tiles) + cols - 1) // cols
LH = 26
sheet = Image.new('RGB', (cols * W, rows * (H + LH) + 30), (20, 20, 20))
dr = ImageDraw.Draw(sheet)
try: font = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 11)
except Exception: font = ImageFont.load_default()
try: tfont = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 16)
except Exception: tfont = font
dr.text((6, 6), titel, fill=(255, 255, 255), font=tfont)
for i, (f, im) in enumerate(tiles):
    x, y = (i % cols) * W, 30 + (i // cols) * (H + LH)
    sheet.paste(im, (x, y))
    lab = 'F%d %s' % (f, info.get(f, ''))
    dr.text((x + 2, y + H + 1), lab[:int(W / 6.2)], fill=(255, 230, 120), font=font)
    if len(lab) > int(W / 6.2): dr.text((x + 2, y + H + 13), lab[int(W / 6.2):int(2 * W / 6.2)], fill=(255, 230, 120), font=font)
sheet.save(out, optimize=True)
print(out, sheet.size, len(tiles), 'Kacheln')
