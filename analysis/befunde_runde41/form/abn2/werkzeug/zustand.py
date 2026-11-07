# Zeitleiste aus state.log/debug.log eines Abnahme-Laufs (eigenes Werkzeug der Abnahme 2).
#   python zustand.py <laufdir> [--alle]
import re, sys, os
d = sys.argv[1]
alle = '--alle' in sys.argv
RX_PL = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RX_MS = re.compile(r' gr=(\d+) hk=(\d+)/(\d+) io=(\d+) hr=([0-9a-f]+) ms=(\d+)')
RX_EN = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
seg = 0; last_f = -1
alt = {}
zeilen = []
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = RX_PL.match(line)
    if not m: continue
    f = int(m.group(1))
    if f <= last_f: seg += 1
    last_f = f
    plx, plz, plhp, cam = int(m.group(3)), int(m.group(4)), int(m.group(6)), int(m.group(7))
    ms = RX_MS.search(line)
    gr, hk, hkph, io, hr, mst = (ms.groups() if ms else ('?',)*6)
    form = None; g5 = None
    for e in RX_EN.finditer(line):
        slot, t = int(e.group(1)), e.group(2)
        rec = dict(slot=slot, st=int(e.group(3)), s1=int(e.group(4)), s2=int(e.group(5)), s3=int(e.group(6)), mo=int(e.group(8)),
                   af=int(e.group(9)), x=int(e.group(12)), z=int(e.group(13)), r=int(e.group(14)), hp=int(e.group(15)))
        if t == '30' and form is None: form = rec
        if t == '36' and g5 is None: g5 = rec
    key = (seg, cam, gr, hk, mst, plhp,
           (form['st'], form['s1'], form['mo'], form['hp']) if form else None,
           (g5['st'], g5['s1'], g5['hp']) if g5 else None)
    kurz = 'S%d F%-5d PL(%6d,%6d) hp=%-4d cam=%-2d gr=%s hk=%s/%s ms=%s' % (seg, f, plx, plz, plhp, cam, gr, hk, hkph, mst)
    if form: kurz += ' | FORM s%d st=%d ss1=%d ss2=%d mo=%d af=%d @(%d,%d,r%d) hp=%d' % (form['slot'], form['st'], form['s1'], form['s2'], form['mo'], form['af'], form['x'], form['z'], form['r'], form['hp'])
    if g5: kurz += ' | G5 st=%d ss1=%d hp=%d @(%d,%d)' % (g5['st'], g5['s1'], g5['hp'], g5['x'], g5['z'])
    if alle or key != alt.get('k'):
        print(kurz)
    alt['k'] = key
