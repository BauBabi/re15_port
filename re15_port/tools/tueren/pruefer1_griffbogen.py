# Pruefer 1 (Runde 31, Tueren): Messwerkzeug, kein Spielcode. Dossier analysis/befunde_runde31/tueren_04_pruefer1.md
# Pruefer-1-Griffbogen: groessere Ausschnitte je Seite (RE1.5 aus + entz | Sequenz Anfang/Mitte, auf Tuer beschnitten)
import json, glob, os, sys
from PIL import Image, ImageDraw, ImageFont
W = 'C:/workspace/git/reAi_v2/.claude/worktrees/r31_tueren'
BILDER = sys.argv[1] if len(sys.argv) > 1 else W + '/build/r31_tueren/t4/bilder'
OUT = sys.argv[2] if len(sys.argv) > 2 else W + '/build/r31_tueren/t4p1/griff'
os.makedirs(OUT, exist_ok=True)
d = json.load(open(W + '/analysis/befunde_runde31/tueren_03/zuordnung.json', encoding='utf-8'))
zen = {s['id']: s for s in json.load(open(W + '/build/r31_tueren/t1/zensus.json', encoding='utf-8'))['seiten']}
SEITEN = W + '/build/r31_tueren/t1/re15_seiten'
liste = []
for t in d['tueren']:
    for s in t['seiten']:
        if s.get('abgedeckt'):
            liste.append((t, s))
liste.sort(key=lambda ts: int(ts[1]['id'][1:]))
H = 300
font = ImageFont.truetype('arial.ttf', 15)
def best(sid):
    z = zen.get(sid); b = None
    if z:
        a = z.get('ausschnitte') or {}
        if isinstance(a, dict):
            for c in a.get('cuts', []):
                if c.get('ok') and c.get('px_h') and c.get('px_w'):
                    f = glob.glob(os.path.join(SEITEN, '%s_*_c%02d_aus.png' % (sid, c['cut'])))
                    if f and (b is None or c['px_h'] * c['px_w'] > b[0]):
                        b = (c['px_h'] * c['px_w'], f[0])
    if b: return b[1]
    f = sorted(glob.glob(os.path.join(SEITEN, '%s_*_aus.png' % sid)))
    return f[0] if f else None
def skal(im, h=H, maxw=420):
    s = h / im.height
    w = int(im.width * s)
    if w > maxw:
        s = maxw / im.width; w = maxw; h = int(im.height * s)
    return im.resize((max(1, w), max(1, h)), Image.LANCZOS)
def beschnitt(p):
    im = Image.open(p).convert('RGB')
    g = im.convert('L').point(lambda v: 255 if v > 14 else 0)
    bb = g.getbbox()
    if not bb: return im
    x0, y0, x1, y1 = bb
    x0 = max(0, x0 - 6); y0 = max(0, y0 - 6); x1 = min(im.width, x1 + 6); y1 = min(im.height, y1 + 6)
    return im.crop((x0, y0, x1, y1))
N = 6
for b in range(0, len(liste), N):
    teil = liste[b:b + N]
    zeilen = []
    for t, s in teil:
        sid = s['id']
        zellen = []
        a = best(sid)
        if a:
            zellen.append(skal(Image.open(a).convert('RGB')))
            e = a.replace('_aus.png', '_entz.png')
            if os.path.exists(e): zellen.append(skal(Image.open(e).convert('RGB')))
        for k in ('anfang', 'mitte'):
            p = os.path.join(BILDER, '%s_%s.ppm' % (sid, k))
            if os.path.exists(p): zellen.append(skal(beschnitt(p)))
        zeilen.append((t, s, zellen))
    breite = 230 + max(sum(z.width + 6 for z in zs) for _, _, zs in zeilen)
    bogen = Image.new('RGB', (breite, len(zeilen) * (H + 8)), (20, 20, 20))
    dr = ImageDraw.Draw(bogen)
    for i, (t, s, zs) in enumerate(zeilen):
        y = i * (H + 8)
        k = s['schluessel']
        txt = ['%s %s' % (s['id'], t['id']), '%s->%s' % (s['raum'], k.get('ziel')), '%s V%d' % (s['archiv'], s['variante']),
               'Griff %s' % s.get('griff_seite'), '%s' % s.get('griff_form'), 'Tausch %s' % (s.get('griff_tausch') or '-')]
        for j, z in enumerate(txt):
            dr.text((4, y + 4 + j * 19), z, fill=(255, 230, 120) if j == 0 else (230, 230, 230), font=font)
        x = 230
        for z in zs:
            bogen.paste(z, (x, y)); x += z.width + 6
    bogen.save(os.path.join(OUT, 'g%02d.png' % (b // N + 1)))
print(len(liste), 'Seiten')
