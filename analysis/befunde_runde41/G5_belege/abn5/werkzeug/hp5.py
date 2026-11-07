# Abnahme 5: Spieler-HP-Verlauf und G5-HP aus state.log (eigene Laeufe), Szenenmarken aus debug.log
import re, sys
for L in sys.argv[1:]:
    pl = []; g5 = []
    for z in open(L + '/state.log', encoding='latin-1'):
        m = re.match(r'F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)', z)
        if not m: continue
        f = int(m.group(1)); pl.append((f, int(m.group(5)), int(m.group(2))))
        g = re.search(r'\[\d+ t=36 st=(\d+)[^\]]*@\((-?\d+),(-?\d+)[^\]]*\] hp=(-?\d+)', z)
        if g: g5.append((f, int(g.group(1)), int(g.group(4)), int(g.group(2))))
    marken = [z.strip()[:110] for z in open(L + '/debug.log', encoding='latin-1') if '[g5auftritt] F' in z]
    print('==', L.split('/')[-1]); [print('  ', m) for m in marken]
    # HP-Wechsel des Spielers
    alt = None; wechsel = []
    for f, hp, x in pl:
        if hp != alt: wechsel.append((f, hp, x)); alt = hp
    print('   Spieler-HP-Wechsel (Bild, HP, x):', wechsel[:40])
    alt = None; gw = []
    for f, st, hp, x in g5:
        if (st, hp) != alt: gw.append((f, st, hp, x)); alt = (st, hp)
    print('   G5 (Bild, Zustand, HP, x) bei Wechsel:', gw[:40])
    print('   letztes Bild', pl[-1] if pl else None)
