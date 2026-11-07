# Abnahme 3 (eigenes Werkzeug): HP der Form beim ersten Bild in Wagen A/B/C, Treffer bis zur Rueckstoss-Szene, Rueckstoss-Beginn,
# Sturz (Clip 12), G5 (t=36), Spieler-HP-Verlauf, Kampfstart (Clip 10 erstes Bild).
import sys, re
RX = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RXE = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
ph = 0
erst = {}
alt = None
treffer = 0
ersttr = None
rs = None
sturz = None
g5 = None
plhp = []
plalt = None
kampf = None
gesehen = set()
for line in open(sys.argv[1], errors='replace'):
    m = RX.match(line)
    if not m:
        continue
    f = int(m.group(1))
    if f in gesehen:
        continue
    gesehen.add(f)
    px, pz, php = int(m.group(3)), int(m.group(4)), int(m.group(6))
    form = None
    g = None
    for e in RXE.finditer(line):
        if e.group(2) == '30' and form is None:
            form = (int(e.group(3)), int(e.group(8)), int(e.group(15)), int(e.group(12)))
        if e.group(2) == '36' and g is None:
            g = int(e.group(15))
    if form and ph == 0:
        ph = 1
        erst['A'] = (f, form[2])
    if ph == 1 and (px, pz) == (24700, -9250):
        ph = 2
        erst['B'] = (f, form[2] if form else None)
    if ph == 2 and (px, pz) == (24800, -23550):
        ph = 3
        erst['C'] = (f, form[2] if form else None)
        alt = form[2] if form else None
    if ph == 3 and form:
        if form[1] == 10 and kampf is None:
            kampf = f
        if form[0] in (2, 3) and form[1] == 13 and form[2] == 1 and rs is None:
            rs = (f, form[3])
        if rs is None and alt is not None and form[2] < alt:
            treffer += 1
            ersttr = ersttr or f
        if rs is None:
            alt = form[2]
        if form[1] == 12 and form[2] == 1 and sturz is None:
            sturz = (f, form[3])
    if g is not None and g5 is None:
        g5 = (f, g)
    if plalt is not None and php != plalt:
        plhp.append('F%d %d->%d' % (f, plalt, php))
    plalt = php
print('HP Form: A F%s %s | B F%s %s | C F%s %s; Kampfstart (Clip 10) F%s; Treffer bis Rueckstoss %d (ab F%s); Rueckstoss %s; Sturz %s; G5 %s; Spieler-HP %s' % (
    erst.get('A', ('-', None))[0], erst.get('A', ('-', None))[1], erst.get('B', ('-', None))[0], erst.get('B', ('-', None))[1],
    erst.get('C', ('-', None))[0], erst.get('C', ('-', None))[1], kampf, treffer, ersttr, rs, sturz, g5, ', '.join(plhp) or '-'))
