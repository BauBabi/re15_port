# Abnahme 7 (eigener Auswerter, unabhaengig von den GTR-Zeilen des Bauers): Trefferreaktion der Figur aus state.log + climb.log (Zeile i = Zeile i, letzter Besuch).
# Je Treffer (hp-Flanke, Messhaken RE15_R39_MESS_SPIELER_HP=100): Unterzustand aus dem ersten Reaktions-Clip (18 = Sub 2, 20 = Sub 3), Dauer = Bilder mit
# Spieler-Clip 18/19 (Sub 2) bzw. 20/17/11 (Sub 3) ab dem Treffer, Luftlinie (Lage vor dem Treffer -> Lage im letzten Reaktionsbild), groesster Bildschritt,
# Sturz (y-Folge aus climb.log), RE1.5-Niederwurf-Clips 12..16 nach dem Treffer, Art aus gator_boss.log (RAMMEN F) / gator_trag.log (Phase 3 Biss, 5 Hochbiss, 7 CROSS).
import re, sys, os, math
from collections import Counter
rs = re.compile(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+) .*?cam=(-?\d+)")
rc = re.compile(r"T(\d+) room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\) rot=(-?\d+) band=(\d+) sub=(\d+)")
rt = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\)")
S2 = {18, 19}
S3 = {20, 17, 11}
RE = {18, 19, 20, 17, 11}


def lade(run):
    sl = [l for l in open(run + '/state.log', errors='replace') if l.startswith('F')]
    cl = [l for l in open(run + '/climb.log', errors='replace') if l.startswith('T')]
    start = 0
    prev = None
    for i, l in enumerate(sl):
        f = int(l[1:l.index(' ')])
        if prev is not None and f < prev:
            start = i
        prev = f
    st = []
    for i in range(start, len(sl)):
        m = rs.match(sl[i])
        c = rc.match(cl[i]) if i < len(cl) else None
        if m and c:
            st.append(dict(F=int(m.group(1)), x=int(m.group(2)), z=int(m.group(3)), rot=int(m.group(4)), hp=int(m.group(5)), mo=int(m.group(6)),
                           cam=int(m.group(7)), y=int(c.group(4)), band=int(c.group(7)), sub=int(c.group(8)), room=c.group(2)))
    ramm = set()
    trag = {}
    if os.path.exists(run + '/gator_boss.log'):
        for l in open(run + '/gator_boss.log', errors='replace'):
            m = re.match(r'RAMMEN F(\d+)', l)
            if m:
                ramm.add(int(m.group(1)))
    if os.path.exists(run + '/gator_trag.log'):
        for l in open(run + '/gator_trag.log', errors='replace'):
            m = rt.match(l)
            if m:
                trag[int(m.group(1))] = int(m.group(2))
    return st, ramm, trag


def oben(s):
    return s['y'] < -900 and 1850 < s['x'] < 7200 and -18100 < s['z'] < -14500


def auswerten(run, zeig=True):
    name = os.path.basename(run.rstrip('/'))
    st, ramm, trag = lade(run)
    if not st:
        print('%s: keine Daten' % name)
        return None
    hits = []
    kd_total = sum(1 for s in st if 12 <= s['mo'] <= 16)
    for k in range(1, len(st)):
        if not (st[k]['hp'] < 100 and st[k - 1]['hp'] >= 100):
            continue
        F = st[k]['F']
        p0 = st[k - 1]
        j = k + 1
        if j < len(st) and st[j]['mo'] in RE:
            sub = 2 if st[j]['mo'] in S2 else 3
            menge = S2 if sub == 2 else S3
            while j + 1 < len(st) and st[j + 1]['mo'] in menge and st[j + 1]['hp'] >= 100:
                j += 1
            dauer = j - k
            e = j
        else:
            sub = 0
            dauer = 0
            e = k
        luft = int(math.hypot(st[e]['x'] - p0['x'], st[e]['z'] - p0['z']))
        step = max((math.hypot(st[u + 1]['x'] - st[u]['x'], st[u + 1]['z'] - st[u]['z']) for u in range(k - 1, e)), default=0)
        ys = [st[u]['y'] for u in range(k - 1, e + 1)]
        sturz = p0['y'] < -900 and any(-1800 < yy < 0 for yy in ys)
        gelandet = sturz and st[e]['y'] == 0
        yfolge = [yy for yy in ys if yy != -1800][:7] if sturz else []
        kd = sum(1 for u in range(k, min(e + 1, len(st))) if 12 <= st[u]['mo'] <= 16)
        art = 'Rammen' if F in ramm else {3: 'Biss', 5: 'Hochbiss', 7: 'CROSS-Biss', 12: 'Dash'}.get(trag.get(F), '?')
        clips = []
        for u in range(k, e + 1):
            if not clips or clips[-1][0] != st[u]['mo']:
                clips.append((st[u]['mo'], st[u]['F']))
        abbruch = j + 1 < len(st) and st[j + 1]['hp'] < 100 and st[j + 1]['mo'] in RE
        hits.append(dict(F=F, art=art, sub=sub, dauer=dauer, luft=luft, step=int(step), oben=oben(p0), sturz=sturz, gelandet=gelandet, yfolge=yfolge, kd=kd,
                         von=(p0['x'], p0['z']), nach=(st[e]['x'], st[e]['y'], st[e]['z']), clips=clips, abbruch=abbruch, hp=st[k]['hp'], cam=p0['cam']))
    ob = [oben(s) for s in st]
    hp_idx = set(i for i in range(1, len(st)) if st[i]['hp'] < 100 and st[i - 1]['hp'] >= 100)
    pause = 0
    erst_tr_oben = None
    a = None
    letzt = None
    abschn = []
    for i, s in enumerate(st):
        if ob[i] and a is None:
            a = i
            letzt = None
        if ob[i] and i in hp_idx:
            g = i - (letzt if letzt is not None else a)
            pause = max(pause, g)
            letzt = i
            if erst_tr_oben is None:
                erst_tr_oben = i - a
        if not ob[i] and a is not None:
            abschn.append((st[a]['F'], st[i - 1]['F'], i - a, letzt is not None))
            a = None
    if a is not None:
        abschn.append((st[a]['F'], st[-1]['F'], len(st) - a, letzt is not None))
        rest = len(st) - 1 - (letzt if letzt is not None else a)
        pause = max(pause, rest)
    if zeig:
        print('== %s: %d Bilder (F%d..F%d), Treffer %d, RE1.5-Niederwurf-Clips 12..16 gesamt %d Bilder' % (name, len(st), st[0]['F'], st[-1]['F'], len(hits), kd_total))
        for h in hits:
            print('   F%-5d %-10s %-6s hp %3d Sub %d Dauer %2d%s Luft %5d Schritt max %4d  von %s nach %s%s  Clips %s' % (
                h['F'], h['art'], 'oben' if h['oben'] else 'Wasser', h['hp'], h['sub'], h['dauer'], ' (abgebr.)' if h['abbruch'] else '', h['luft'], h['step'],
                h['von'], h['nach'], (' STURZ y%s%s' % (h['yfolge'], ' gelandet' if h['gelandet'] else ' NICHT gelandet')) if h['sturz'] else '',
                ' '.join('%d@F%d' % c for c in h['clips'][:6])))
        print('   oben-Abschnitte (F von..bis, Bilder, Treffer darin):', abschn, '| erster Treffer oben nach', erst_tr_oben, 'Bildern, laengste Pause oben', pause)
    return dict(name=name, n=len(st), hits=hits, kd_total=kd_total, abschn=abschn, erst_tr_oben=erst_tr_oben, pause=pause)


if __name__ == '__main__':
    alle = []
    for run in sys.argv[1:]:
        r = auswerten(run)
        if r:
            alle.append(r)
    print('\n== SUMME ueber %d Laeufe ==' % len(alle))
    H = [h for r in alle for h in r['hits']]
    if H:
        print('Treffer %d; Sub 2 %d, Sub 3 %d, ohne Reaktion %d; RE1.5-Niederwurf-Clips nach Treffern %d, gesamt %d' % (
            len(H), sum(h['sub'] == 2 for h in H), sum(h['sub'] == 3 for h in H), sum(h['sub'] == 0 for h in H), sum(h['kd'] for h in H), sum(r['kd_total'] for r in alle)))
        for art in sorted(set(h['art'] for h in H)):
            hh = [h for h in H if h['art'] == art]
            print('  %-10s %3d: Dauer %s, Luftlinie max %d (Mittel %d), Schritt max %d, oben %d, Stuerze %d' % (
                art, len(hh), sorted(Counter(h['dauer'] for h in hh).items()), max(h['luft'] for h in hh), sum(h['luft'] for h in hh) // len(hh),
                max(h['step'] for h in hh), sum(h['oben'] for h in hh), sum(h['sturz'] for h in hh)))
        voll = [h for h in H if not h['abbruch']]
        print('vollstaendige Reaktionen: Sub 2 Dauer', sorted(Counter(h['dauer'] for h in voll if h['sub'] == 2).items()),
              '| Sub 3 Dauer', sorted(Counter(h['dauer'] for h in voll if h['sub'] == 3).items()))
        print('Luftlinie > 3735: %d, > 3080 bei Sub 3: %d; Stuerze %d, davon gelandet %d; y-Folgen: %s' % (
            sum(h['luft'] > 3735 for h in H), sum(h['luft'] > 3080 and h['sub'] == 3 for h in H), sum(h['sturz'] for h in H), sum(h['gelandet'] for h in H),
            Counter(tuple(h['yfolge']) for h in H if h['sturz'])))
