"""Zustandswechsel der Figuren (Typ t=..) und des Spielers aus einem state.log, ab einer Zeilennummer. Nur lesen.
zustand.py <state.log> <ab-zeile> <typ[,typ]> [von-bild]"""
import sys, re
log, ab, typen = sys.argv[1], int(sys.argv[2]), sys.argv[3].split(',')
von = int(sys.argv[4]) if len(sys.argv) > 4 else 0
alt = {}
for n, z in enumerate(open(log, errors='replace'), 1):
    if n < ab: continue
    m = re.match(r'F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)', z)
    if not m: continue
    f = int(m.group(1))
    if f < von: continue
    cam = re.search(r'cam=(\d+)', z).group(1)
    k = ('PL', m.group(5), cam)
    if alt.get('PL') != k:
        print('F%d Spieler hp=%s cam=%s @(%s,%s)' % (f, m.group(5), cam, m.group(2), m.group(3))); alt['PL'] = k
    for e in re.finditer(r'\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=\w+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)', z):
        slot, t, st, s1, s2, mo, af, x, zz, r, hp = e.groups()
        if t not in typen: continue
        k = (t, st, s1, mo, hp)
        if alt.get(slot) != k:
            print('F%d  Slot %s t=%s st=%s ss1=%s ss2=%s mo=%s af=%s hp=%s @(%s,%s)' % (f, slot, t, st, s1, s2, mo, af, hp, x, zz))
            alt[slot] = k
