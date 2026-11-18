# Messbild je Raum: Sweep-Blatt (alles aufgedeckt) um die Kachel(n) des Raums, darauf die Engine-Markerlagen
# der FOLGE-Rasterpunkte (klein, gruen = auf eigener Kunst, rot = daneben) und der Ankunftslagen (Kreis, Name).
import sys, os, subprocess, collections
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kunst
S = os.path.dirname(os.path.abspath(__file__))
SW = os.path.join(S, '..', 's3', 'sweep')
f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 13); fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 18)
raeume = sys.argv[1].split(','); aus = sys.argv[2]; best = sys.argv[3] if len(sys.argv) > 3 else os.path.join(S, 'bestand.txt')
vor = sys.argv[4] if len(sys.argv) > 4 else os.path.join(S, 'vorpruef_detail.txt')
Z = kunst.zonen()
folge = collections.defaultdict(list)
for l in open(best):
    t = l.split()
    if t and t[0] == 'FOLGE' and t[1] in raeume: folge[t[1]].append((int(t[6]), int(t[7]), int(t[8])))
ein = collections.defaultdict(list)
for l in open(vor):
    t = l.split()
    if len(t) > 8 and t[1] in raeume: ein[t[1]].append((t[0], int(t[5]), int(t[6]), int(t[7]), t[8]))
SK = 4; tiles = []
for r in raeume:
    rid = int(r, 16)
    pgs = sorted(set([p for p, _, _ in folge[r]] + [p for _, p, _, _, _ in ein[r]]))
    for pg in pgs:
        if pg < 0 or pg > 12: continue
        own = set()
        for z in Z:
            if z['room'] == rid and z['page'] == pg and z['rect'] != 255: own |= kunst.flaeche(pg, z['rect'])
        pts = [(x, y) for p, x, y in folge[r] if p == pg] + [(x, y) for _, p, x, y, _ in ein[r] if p == pg] + list(own)
        if not pts: continue
        x0 = max(0, min(p[0] for p in pts) - 14); x1 = min(319, max(p[0] for p in pts) + 14)
        y0 = max(0, min(p[1] for p in pts) - 14); y1 = min(239, max(p[1] for p in pts) + 14)
        im = Image.open(os.path.join(SW, 'blatt%02d.bmp' % pg)).convert('RGB').crop((x0, y0, x1 + 1, y1 + 1))
        im = im.resize((im.width * SK, im.height * SK), Image.NEAREST); d = ImageDraw.Draw(im)
        for p, x, y in folge[r]:
            if p != pg: continue
            c = (0, 255, 0) if (x, y) in own else (255, 60, 60)
            X, Y = (x - x0) * SK + 2, (y - y0) * SK + 2; d.rectangle([X - 1, Y - 1, X + 1, Y + 1], fill=c)
        for tag, p, x, y, u in ein[r]:
            if p != pg: continue
            X, Y = (x - x0) * SK + 2, (y - y0) * SK + 2
            c = (255, 255, 0) if u in ('auf',) or u.startswith('rand') else (255, 0, 255)
            d.ellipse([X - 7, Y - 7, X + 7, Y + 7], outline=c, width=2); d.text((X + 8, Y - 7), tag.split('_')[1] + ' ' + u, fill=c, font=f)
        tiles.append((im, 'ROOM%s Blatt %d (x %d..%d, y %d..%d)' % (r, pg, x0, x1, y0, y1)))
W = max(i.width for i, _ in tiles) + 20; H = sum(i.height + 26 for i, _ in tiles) + 44
out = Image.new('RGB', (max(W, 700), H), (10, 10, 10)); d = ImageDraw.Draw(out)
d.rectangle([0, 0, out.width, 36], fill=(160, 0, 0)); d.text((8, 7), 'Messbild - kein Ergebnis   Engine-Marker auf der Kunst (gruen=auf, rot=daneben; Kreis=Ankunft)', fill=(255, 255, 255), font=fb)
y = 44
for im, lab in tiles:
    d.text((10, y), lab, fill=(255, 255, 160), font=f); out.paste(im, (10, y + 18)); y += im.height + 26
out.save(aus)
