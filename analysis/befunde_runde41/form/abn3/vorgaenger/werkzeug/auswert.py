# Abnahme 3 (eigenes Werkzeug): Zeitleiste der Rueckstoss-Szene je Bild aus state.log + deck.log + Framedump.
#   python auswert.py <laufdir> [--tabelle]
# Szene = Bilder mit Form (t=30) st 2/3, hp 1 (Taumeln mo 13, Sturz mo 12) bis die Form nicht mehr im state.log steht.
# fast schwarz: Anteil Bildpunkte mit PIL-'L' < 24 (wie abn2/werkzeug/hell.py), Promille.
import sys, os, re, glob
import numpy as np
from PIL import Image
d = sys.argv[1]; tab = '--tabelle' in sys.argv
RX = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RXE = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
zeilen = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = RX.match(line)
    if not m: continue
    f = int(m.group(1))
    form = None
    for e in RXE.finditer(line):
        if e.group(2) == '30':
            form = dict(slot=int(e.group(1)), st=int(e.group(3)), mo=int(e.group(8)), af=int(e.group(9)), x=int(e.group(12)), z=int(e.group(13)), hp=int(e.group(15)))
            break
    zeilen[f] = dict(px=int(m.group(3)), pz=int(m.group(4)), php=int(m.group(6)), cam=int(m.group(7)), form=form)
szene = [f for f in sorted(zeilen) if zeilen[f]['form'] and zeilen[f]['form']['st'] in (2, 3) and zeilen[f]['form']['hp'] == 1 and zeilen[f]['form']['mo'] in (12, 13)]
if not szene:
    print('keine Szene'); sys.exit(1)
deck = {}
if os.path.exists(os.path.join(d, 'deck.log')):
    for line in open(os.path.join(d, 'deck.log')):
        m = re.match(r'F(\d+) cam=(-?\d+)(.*)', line)
        if m: deck[int(m.group(1))] = (int(m.group(2)), {int(a): (int(b), int(c)) for a, b, c in re.findall(r' s(\d+)=(\d+)/(\d+)', m.group(3))})
summe = {}
for f in szene:
    if f in deck:
        for s, (v, b) in deck[f][1].items():
            if 1 <= s <= 18: summe[s] = summe.get(s, 0) + b
fs = max(summe, key=summe.get) if summe else -1
dunkel = {}
for p in sorted(glob.glob(os.path.join(d, 'fd', 'f*.ppm'))):
    f = int(os.path.basename(p)[1:7])
    if szene[0] - 2 <= f <= szene[-1] + 2:
        a = np.asarray(Image.open(p).convert('L'), dtype=np.int32)
        dunkel[f] = (int((a < 24).sum() * 1000 // a.size), float(a.mean()))
segs = []
for f in szene:
    c = zeilen[f]['cam']; mo = zeilen[f]['form']['mo']
    if not segs or segs[-1][0] != c or segs[-1][3] != mo: segs.append([c, f, f, mo])
    else: segs[-1][2] = f
out = []
mx_d = (0, 0); mn_v = (10**9, 0); mx_p = (0, 0); mx_k = (0, 0); n600 = 0; nd = 0
for f in szene:
    z = zeilen[f]; fo = z['form']
    v = b = pv = -1; k = -1
    if f in deck:
        v, b = deck[f][1].get(fs, (0, 0)); pv = deck[f][1].get(0, (0, 0))[0]
        k = (b - v) * 1000 // b if b else 1000
        if fo['mo'] == 13:
            if v < mn_v[0]: mn_v = (v, f)
            if pv > mx_p[0]: mx_p = (pv, f)
            if k > mx_k[0]: mx_k = (k, f)
    dd = dunkel.get(f)
    if dd:
        nd += 1
        if dd[0] > mx_d[0]: mx_d = (dd[0], f)
        if dd[0] > 600: n600 += 1
    out.append('F%d cam=%d mo=%d af=%d form=(%d,%d) pl=(%d,%d) hp=%d deck form %d/%d verdeckt %d spieler %d dunkel %s mittel %s' % (
        f, z['cam'], fo['mo'], fo['af'], fo['x'], fo['z'], z['px'], z['pz'], z['php'], v, b, k, pv,
        dd[0] if dd else '-', '%.1f' % dd[1] if dd else '-'))
print('%s: Szene F%d-F%d (%d Bilder), Form-Slot %d, Spieler bei Szenenbeginn (%d,%d), Form (%d,%d)' % (
    os.path.basename(d.rstrip('/')), szene[0], szene[-1], len(szene), fs, zeilen[szene[0]]['px'], zeilen[szene[0]]['pz'],
    zeilen[szene[0]]['form']['x'], zeilen[szene[0]]['form']['z']))
print('  Schnitte: ' + ', '.join('Cut %d F%d-F%d (mo %d)' % (c, a, b, mo) for c, a, b, mo in segs))
print('  Taumeln: Form min sichtbar %d (F%d), Spieler max %d (F%d), Form max verdeckt %d Promille (F%d); fast schwarz max %d Promille (F%d), > 600: %d von %d Bildern' % (
    mn_v[0], mn_v[1], mx_p[0], mx_p[1], mx_k[0], mx_k[1], mx_d[0], mx_d[1], n600, nd))
if tab:
    open(os.path.join(d, 'szene.txt'), 'w').write('\n'.join(out) + '\n')
