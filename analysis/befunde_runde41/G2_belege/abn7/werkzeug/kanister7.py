# Abnahme 7 (K): Lebenslauf des Szenen-Kanisters je Lauf: Liegestelle (gator_boss.log "RE2 KANISTER gesetzt"), Aufnahme Stufe 1 (F, laengs, quer),
# angehaengt F, Zeilen RUNDE / ANSCHWIMMEN / Zielmodus 1, Treffer bis zur Aufnahme und erster Treffer (state.log, letzter Besuch).
import re, sys, os


def lebenslauf(d):
    gb = open(os.path.join(d, 'gator_boss.log'), errors='ignore').read() if os.path.exists(os.path.join(d, 'gator_boss.log')) else ''
    dbg = open(os.path.join(d, 'debug.log'), errors='ignore').read() if os.path.exists(os.path.join(d, 'debug.log')) else ''
    ges = re.search(r'RE2 KANISTER gesetzt \((-?\d+),(-?\d+),(-?\d+)\)', gb)
    s1 = re.search(r'KANISTER Stufe 1 laengs=(-?\d+) quer=(-?\d+) sub \d -> 1 F(\d+)', gb)
    ang = re.search(r'KANISTER angehaengt .*?F(\d+)', gb)
    runde = len(re.findall(r'RUNDE zum Kanister', dbg))
    ansch = len(re.findall(r'ANSCHWIMMEN an', dbg))
    ziel = len(re.findall(r'Zielmodus 1', gb))
    intro = re.search(r'KAMERA INTRO frei[^\n]*? F(\d+)', gb)
    sl = [l for l in open(os.path.join(d, 'state.log'), errors='replace') if l.startswith('F')]
    start = 0
    prev = None
    for i, l in enumerate(sl):
        f = int(l[1:l.index(' ')])
        if prev is not None and f < prev:
            start = i
        prev = f
    hp_prev = 100
    tr = []
    for l in sl[start:]:
        m = re.match(r'F(\d+) .*?hp=(-?\d+)\)', l)
        if not m:
            continue
        f = int(m.group(1))
        hp = int(m.group(2))
        if hp < 100 and hp_prev >= 100:
            tr.append(f)
        hp_prev = hp
    auf = int(s1.group(3)) if s1 else None
    tr_vor = [f for f in tr if auf is None or f < auf]
    return dict(name=os.path.basename(d.rstrip('/')), liege=ges.groups() if ges else None, auf=auf, laengs=s1.group(1) if s1 else '-',
                quer=s1.group(2) if s1 else '-', ang=ang.group(1) if ang else '-', runde=runde, ansch=ansch, ziel=ziel,
                intro=intro.group(1) if intro else '-', treffer=len(tr), tr_vor=len(tr_vor), erst=tr[0] if tr else None, bilder=prev)


for d in sys.argv[1:]:
    if not os.path.exists(os.path.join(d, 'state.log')):
        continue
    r = lebenslauf(d)
    print('%-28s Liegestelle %-26s | Aufnahme Stufe 1 %-6s (laengs %s quer %s) angehaengt F%-5s | RUNDE %d ANSCHWIMMEN %d Zielmodus1 %d | Intro frei F%s | Treffer %d (vor der Aufnahme %d, erster F%s) | Bilder %s' % (
        r['name'], r['liege'], ('F%d' % r['auf']) if r['auf'] else 'KEINE', r['laengs'], r['quer'], r['ang'], r['runde'], r['ansch'], r['ziel'], r['intro'],
        r['treffer'], r['tr_vor'], r['erst'], r['bilder']))
