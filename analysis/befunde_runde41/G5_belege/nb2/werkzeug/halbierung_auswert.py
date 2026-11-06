import re, sys, os, glob
# Auswertung der NB2-Diagnose: Kampfbeginn, Spielerlage, Treffer (HP-Abfall) mit Quelle, G5-Tod.
pat = re.compile(r"n(\d+) pl=\((-?\d+),(-?\d+)\) hp=(-?\d+) hr=(\d+) g5 sub=(\d+) ph=(\d+) t=(-?\d+) blob=(\d+)/(\d+) cd=(-?\d+) mo=(-?\d+) af=(\d+) x=(-?\d+) z=(-?\d+) hp=(-?\d+) dist=(-?\d+) arc=(-?\d+) hb=(-?\d+)(.*)")
for d in sys.argv[1:]:
    f = os.path.join(d, 'diag.log')
    if not os.path.exists(f):
        continue
    rows = []
    for ln in open(f):
        m = pat.match(ln)
        if m: rows.append(m)
    fertig = open(os.path.join(d, 'fertig.txt')).read().strip() if os.path.exists(os.path.join(d, 'fertig.txt')) else 'laeuft'
    print('==', os.path.basename(d), fertig, 'zeilen', len(rows))
    kampf = None
    prev = None
    g5tot = None
    for m in rows:
        n = int(m.group(1)); sub = int(m.group(6)); g5hp = int(m.group(16)); hp = int(m.group(4))
        if kampf is None and sub != 2:
            kampf = n
            print('  Kampfbeginn n%d pl=(%s,%s) hp=%s g5 x=%s' % (n, m.group(2), m.group(3), m.group(4), m.group(14)))
        if prev is not None:
            php = int(prev.group(4))
            if hp < php:
                print('  TREFFER n%d hp %d->%d pl=(%s,%s) g5 sub=%s ph=%s blob=%s/%s x=%s dist=%s arc=%s hb=%s %s' % (
                    n, php, hp, prev.group(2), prev.group(3), prev.group(6), prev.group(7), prev.group(9), prev.group(10),
                    prev.group(14), prev.group(17), prev.group(18), prev.group(19), prev.group(20).strip()))
            pz, z = int(prev.group(3)), int(m.group(3))
            if abs(z - pz) > 200 or abs(int(m.group(2)) - int(prev.group(2))) > 400:
                print('  SCHUB n%d pl (%s,%s)->(%s,%s) g5 sub=%s ph=%s x=%s %s' % (n, prev.group(2), prev.group(3), m.group(2), m.group(3),
                      m.group(6), m.group(7), m.group(14), m.group(20).strip()))
        if g5tot is None and g5hp < 0:
            g5tot = n
            print('  G5 tot n%d, Spieler hp %d' % (n, hp))
        prev = m
    if rows:
        m = rows[-1]
        print('  Ende n%s pl=(%s,%s) hp=%s g5 hp=%s' % (m.group(1), m.group(2), m.group(3), m.group(4), m.group(16)))
