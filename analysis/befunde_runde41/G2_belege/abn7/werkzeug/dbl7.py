# Abnahme 7: Doppelbisse - zwei Treffer (hp-Flanken, letzter Besuch) im Abstand <= 25 Bilder, bei denen der Gator-Clip (state.log [35 ...] mo=/af=) zwischen beiden
# Treffern durchlaeuft (gleicher Clip, af steigt ohne Ruecksetzung) = zweiter Biss im selben Angriffs-Clip. RE2: der Treffer-Einstieg 0x80101578 feuert nur in den
# Schliess-Stufen der Biss-Maschine +0x224 (0x80101388), und ein neuer Biss braucht +0x238 == 0 (70 Bilder, @0x80100cb4-c4) - ein zweiter Treffer 20-22 Bilder nach
# dem ersten ist dort nicht moeglich.
import re, sys, os
from collections import Counter
rs = re.compile(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*?\[35 .*? mo=(-?\d+) af=(-?\d+) ")


def lade(run):
    sl = [l for l in open(run + '/state.log', errors='replace') if l.startswith('F')]
    start = 0
    prev = None
    for i, l in enumerate(sl):
        f = int(l[1:l.index(' ')])
        if prev is not None and f < prev:
            start = i
        prev = f
    st = []
    for l in sl[start:]:
        m = rs.match(l)
        if m:
            st.append(dict(F=int(m.group(1)), hp=int(m.group(5)), gmo=int(m.group(6)), gaf=int(m.group(7))))
    ph = {}
    if os.path.exists(run + '/gator_trag.log'):
        for l in open(run + '/gator_trag.log', errors='replace'):
            m = re.match(r'TRAG F(\d+) ph=(\d+)', l)
            if m:
                ph[int(m.group(1))] = int(m.group(2))
    ramm = set()
    if os.path.exists(run + '/gator_boss.log'):
        for l in open(run + '/gator_boss.log', errors='replace'):
            m = re.match(r'RAMMEN F(\d+)', l)
            if m:
                ramm.add(int(m.group(1)))
    for s_ in st:
        s_['art'] = 'Rammen' if s_['F'] in ramm else {3: 'Biss', 5: 'Hochbiss', 7: 'CROSS', 12: 'Dash'}.get(ph.get(s_['F']), '?')
    return st


tot = Counter()
for run in sys.argv[1:]:
    if not os.path.exists(run + '/state.log'):
        continue
    st = lade(run)
    hits = [i for i in range(1, len(st)) if st[i]['hp'] < 100 and st[i - 1]['hp'] >= 100]
    paare = []
    for a, b in zip(hits, hits[1:]):
        d = st[b]['F'] - st[a]['F']
        if d > 25:
            continue
        gleich = all(st[i]['gmo'] == st[a]['gmo'] for i in range(a, b + 1)) and all(st[i + 1]['gaf'] >= st[i]['gaf'] for i in range(a, b))
        paare.append((st[a]['F'], st[b]['F'], d, st[a]['gmo'], st[a]['gaf'], st[b]['gaf'], gleich, st[a]['art'], st[b]['art']))
    n_gl = sum(1 for p in paare if p[6])
    tot['treffer'] += len(hits)
    tot['paare'] += len(paare)
    tot['paare_gleicher_clip'] += n_gl
    for p in paare:
        tot['abstand %d' % p[2]] += 1
        tot['Art %s->%s' % (p[7], p[8])] += 1
    print('%-26s Treffer %3d, Paare <= 25 Bilder: %3d, davon im selben Gator-Clip: %3d | %s' % (
        os.path.basename(run.rstrip('/')), len(hits), len(paare), n_gl,
        ' '.join('F%d/F%d(+%d %s clip %d af %d->%d%s)' % (p[0], p[1], p[2], p[7], p[3], p[4], p[5], '' if p[6] else ' NEU') for p in paare[:12]) + (' ...' if len(paare) > 12 else '')))
print('SUMME:', dict(tot))
