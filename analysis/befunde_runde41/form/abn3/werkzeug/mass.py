# Abnahme 3 Stufe 3 (eigenes Werkzeug des Abnehmers): je Bild Kamera (state.log cam=), Lagen von Spieler und Form (t=30),
# gezeichnete Flaechen aus der Messschiene deck.log (s<slot>=<sichtbar>/<bedeckt>, 320x240) und "fast schwarz" aus dem
# Framedump (640x480 P6; PIL 'L' < 24, Promille, wie abn2/werkzeug/hell.py).
#   python mass.py <laufdir> --szene            Fenster = Rueckstoss-Szene (Form st 2/3, hp 1, Clip 13 Taumeln / 12 Sturz)
#   python mass.py <laufdir> --von N --bis M    beliebiges Fenster (Kampf / Wache)
# Schreibt <laufdir>/mass_<tag>.txt (je Bild) und gibt Abschnitte je Schnitt sowie das Pruefmass aus:
#   Form sichtbar >= 768 BP, Spieler sichtbar <= 25600 BP, Form verdeckt <= 500 Promille, fast schwarz <= 600 Promille.
import sys, os, re, glob
import numpy as np
from PIL import Image
RX = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RXE = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')


def lese_state(d):
    z = {}
    for line in open(os.path.join(d, 'state.log'), errors='replace'):
        m = RX.match(line)
        if not m:
            continue
        f = int(m.group(1))
        if f in z:
            continue
        form = None
        for e in RXE.finditer(line):
            if e.group(2) == '30' and form is None:
                form = dict(slot=int(e.group(1)), st=int(e.group(3)), ss1=int(e.group(4)), mo=int(e.group(8)), af=int(e.group(9)),
                            x=int(e.group(12)), z=int(e.group(13)), hp=int(e.group(15)))
        z[f] = dict(px=int(m.group(3)), pz=int(m.group(4)), php=int(m.group(6)), cam=int(m.group(7)), form=form)
    return z


def lese_deck(d):
    deck = {}
    p = os.path.join(d, 'deck.log')
    if not os.path.exists(p):
        return deck
    for line in open(p):
        m = re.match(r'F(\d+) cam=(-?\d+)(.*)', line)
        if m:
            deck[int(m.group(1))] = {int(a): (int(b), int(c)) for a, b, c in re.findall(r' s(\d+)=(\d+)/(\d+)', m.group(3))}
    return deck


def dunkel(d, f):
    p = os.path.join(d, 'fd', 'f%06d.ppm' % f)
    if not os.path.exists(p):
        return None
    a = np.asarray(Image.open(p).convert('L'), dtype=np.int32)
    return int((a < 24).sum() * 1000 // a.size), float(a.mean())


def main():
    d = sys.argv[1]
    args = sys.argv[2:]
    z = lese_state(d)
    deck = lese_deck(d)
    if '--szene' in args:
        fr = [f for f in sorted(z) if z[f]['form'] and z[f]['form']['st'] in (2, 3) and z[f]['form']['hp'] == 1 and z[f]['form']['mo'] in (12, 13)]
        if not fr:
            print('%s: keine Rueckstoss-Szene' % d)
            return 1
        von, bis = fr[0], fr[-1]
        tag = 'szene'
    else:
        von = int(args[args.index('--von') + 1])
        bis = int(args[args.index('--bis') + 1])
        tag = '%d_%d' % (von, bis)
    fr = [f for f in sorted(z) if von <= f <= bis]
    # Form-Slot = Figuren-Slot 1..18 mit der groessten bedeckten Flaeche im Fenster (wie der Riegel), nie Slot 0 (Spieler)
    summe = {}
    for f in fr:
        for s, (v, b) in deck.get(f, {}).items():
            if 1 <= s <= 18:
                summe[s] = summe.get(s, 0) + b
    fs = max(summe, key=summe.get) if summe else -1
    rows = []
    segs = []
    for f in fr:
        r = z[f]
        fo = r['form']
        dk = deck.get(f)
        fv = fb = pv = pb = -1
        if dk is not None:
            fv, fb = dk.get(fs, (0, 0))
            pv, pb = dk.get(0, (0, 0))
        fverd = (fb - fv) * 1000 // fb if fb > 0 else (0 if fb == 0 else -1)
        pverd = (pb - pv) * 1000 // pb if pb > 0 else (0 if pb == 0 else -1)
        dn = dunkel(d, f)
        row = dict(f=f, cam=r['cam'], mo=fo['mo'] if fo else -1, af=fo['af'] if fo else -1, fx=fo['x'] if fo else 0, fz=fo['z'] if fo else 0,
                   fst=fo['st'] if fo else -1, fhp=fo['hp'] if fo else 0, px=r['px'], pz=r['pz'], php=r['php'], fv=fv, fb=fb, fverd=fverd,
                   pv=pv, pb=pb, pverd=pverd, dn=dn[0] if dn else -1, mittel=dn[1] if dn else -1)
        rows.append(row)
        key = (row['cam'], row['mo'] if tag == 'szene' else None)
        if not segs or segs[-1]['key'] != key:
            segs.append(dict(key=key, rows=[row]))
        else:
            segs[-1]['rows'].append(row)
    with open(os.path.join(d, 'mass_%s.txt' % tag), 'w') as fp:
        fp.write('# %s Fenster F%d-F%d, Form-Slot %d; Spalten: F cam mo af form(x,z) st hp | pl(x,z) hp | form sichtbar/bedeckt verdeckt-Promille | spieler sichtbar/bedeckt verdeckt-Promille | dunkel-Promille mittel\n' % (os.path.basename(d.rstrip('/')), von, bis, fs))
        for r in rows:
            fp.write('F%d cam=%d mo=%d af=%d form=(%d,%d) st=%d hp=%d | pl=(%d,%d) hp=%d | form %d/%d %d | pl %d/%d %d | dunkel %d mittel %.1f\n' % (
                r['f'], r['cam'], r['mo'], r['af'], r['fx'], r['fz'], r['fst'], r['fhp'], r['px'], r['pz'], r['php'], r['fv'], r['fb'], r['fverd'], r['pv'], r['pb'], r['pverd'], r['dn'], r['mittel']))
    name = os.path.basename(d.rstrip('/'))
    print('%s: Fenster F%d-F%d (%d Bilder), Form-Slot %d; Beginn: Spieler (%d,%d) Form (%d,%d)' % (name, von, bis, len(fr), fs, rows[0]['px'], rows[0]['pz'], rows[0]['fx'], rows[0]['fz']))
    for s in segs:
        rr = s['rows']
        dns = [r['dn'] for r in rr if r['dn'] >= 0]
        fvs = [r['fv'] for r in rr if r['fv'] >= 0]
        pvs = [r['pv'] for r in rr if r['pv'] >= 0]
        fvd = [r['fverd'] for r in rr if r['fverd'] >= 0]
        pvd = [r['pverd'] for r in rr if r['pverd'] >= 0]
        print('  Cut %2d%s F%d-F%d (%3d Bilder): Form x %6d..%6d, Spieler x %6d..%6d | dunkel %s..%s (>600: %d) | Form sichtbar min %s, verdeckt max %s | Spieler sichtbar max %s, verdeckt max %s' % (
            s['key'][0], (' mo%d' % s['key'][1]) if s['key'][1] is not None else '', rr[0]['f'], rr[-1]['f'], len(rr), rr[0]['fx'], rr[-1]['fx'], rr[0]['px'], rr[-1]['px'],
            min(dns) if dns else '-', max(dns) if dns else '-', sum(1 for x in dns if x > 600),
            min(fvs) if fvs else '-', max(fvd) if fvd else '-', max(pvs) if pvs else '-', max(pvd) if pvd else '-'))
    # Pruefmass ueber die Taumelbilder (Szene: mo 13) bzw. alle Bilder des Fensters
    pr = [r for r in rows if (r['mo'] == 13 if tag == 'szene' else True)]
    verst = []
    for r in pr:
        g = []
        if 0 <= r['fv'] < 768 and tag == 'szene':
            g.append('Form sichtbar %d < 768' % r['fv'])
        if r['pv'] > 25600:
            g.append('Spieler %d > 25600' % r['pv'])
        if r['fverd'] > 500 and tag == 'szene':
            g.append('Form verdeckt %d > 500' % r['fverd'])
        if r['dn'] > 600:
            g.append('fast schwarz %d > 600' % r['dn'])
        if g:
            verst.append((r['f'], r['cam'], '; '.join(g)))
    dns = [r['dn'] for r in pr if r['dn'] >= 0]
    fvs = [r for r in pr if r['fv'] >= 0]
    print('  Pruefmass (%d Bilder, %d mit Bild): Form sichtbar min %s (F%s), Spieler sichtbar max %s (F%s), Form verdeckt max %s (F%s), fast schwarz max %s (F%s), > 600: %d; Verstoesse: %d' % (
        len(pr), len(dns),
        min((r['fv'] for r in fvs), default='-'), min(fvs, key=lambda r: r['fv'])['f'] if fvs else '-',
        max((r['pv'] for r in fvs), default='-'), max(fvs, key=lambda r: r['pv'])['f'] if fvs else '-',
        max((r['fverd'] for r in fvs), default='-'), max(fvs, key=lambda r: r['fverd'])['f'] if fvs else '-',
        max(dns) if dns else '-', max(pr, key=lambda r: r['dn'])['f'] if dns else '-', sum(1 for x in dns if x > 600), len(verst)))
    for v in verst[:12]:
        print('    F%d Cut %d: %s' % v)
    if len(verst) > 12:
        print('    ... %d weitere' % (len(verst) - 12))
    return 0


sys.exit(main())
