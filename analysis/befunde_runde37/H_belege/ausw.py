#!/usr/bin/env python3
"""Auswertung state.log (RE15_STATE_LOG): je Raumbesuch (cam/room-Wechsel nicht im Log -> Besuch = F-Zaehler faellt)
je Zombie: Zustandsfolge (st/ss1/ss2 mit Bildern), Weg je 30-Bilder-Fenster, kleinster Abstand, erstes Bild Gang (ss1==1)."""
import re, sys, math, collections
RX_F = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)')
RX_E = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
ZTYP = {0x10, 0x11, 0x12, 0x13, 0x16, 0x18}
def lese(path):
    besuche = []; cur = None; last = -1
    for line in open(path, errors='replace'):
        m = RX_F.match(line)
        if not m: continue
        f = int(m.group(1))
        if f < last or cur is None:
            cur = []; besuche.append(cur)
        last = f
        pl = tuple(int(m.group(i)) for i in range(2, 6))
        ens = {}
        for e in RX_E.finditer(line):
            slot = int(e.group(1)); t = int(e.group(2), 16)
            ens[slot] = dict(t=t, st=int(e.group(3)), ss1=int(e.group(4)), ss2=int(e.group(5)), g=int(e.group(7), 16),
                             mo=int(e.group(8)), d=int(e.group(11)), x=int(e.group(12)), z=int(e.group(13)), r=int(e.group(14)), hp=int(e.group(15)))
        cur.append((f, pl, ens))
    return besuche
def zusammenfassung(besuch, von=0, bis=10**9, takt=30):
    zs = collections.OrderedDict()
    for (f, pl, ens) in besuch:
        if f < von or f > bis: continue
        for s, e in ens.items():
            if e['t'] not in ZTYP: continue
            zs.setdefault(s, []).append((f, pl, e))
    out = []
    for s, rows in zs.items():
        e0 = rows[0][2]; e1 = rows[-1][2]
        # Zustandsfolge
        folge = []; prev = None; start = rows[0][0]
        for (f, pl, e) in rows:
            k = (e['st'], e['ss1'], e['ss2'] if e['ss1'] == 0 else -1)
            if k != prev:
                if prev is not None: folge.append('%s@%d' % ('%d/%d%s' % (prev[0], prev[1], '' if prev[2] < 0 else '/%d' % prev[2]), start))
                prev = k; start = f
        folge.append('%s@%d' % ('%d/%d%s' % (prev[0], prev[1], '' if prev[2] < 0 else '/%d' % prev[2]), start))
        weg = 0.0; px, pz = e0['x'], e0['z']
        for (f, pl, e) in rows:
            weg += math.hypot(e['x'] - px, e['z'] - pz); px, pz = e['x'], e['z']
        dmin = min(r[2]['d'] for r in rows)
        gang = next((f for (f, pl, e) in rows if e['st'] == 1 and e['ss1'] != 0), None)
        # Annaeherung: Abstand am Anfang/Ende
        out.append("  slot %2d t=%02x g=%02x  start(%d,%d) ende(%d,%d) weg=%d  d %d->%d (min %d)  erstes Nicht-Stehen F%s\n     Folge: %s" % (
            s, e0['t'], e0['g'], e0['x'], e0['z'], e1['x'], e1['z'], weg, e0['d'], e1['d'], dmin, gang, ' '.join(folge[:14]) + (' ...' if len(folge) > 14 else '')))
    return out
if __name__ == '__main__':
    p = sys.argv[1]
    bs = lese(p)
    sel = int(sys.argv[2]) if len(sys.argv) > 2 else -1
    for i, b in enumerate(bs):
        if sel >= 0 and i != sel: continue
        if not b: continue
        n = max((len([1 for e in r[2].values() if e['t'] in ZTYP]) for r in b), default=0)
        print("Besuch %d: F%d..F%d, Zombies max %d, Leon %s -> %s" % (i, b[0][0], b[-1][0], n, b[0][1][:2], b[-1][1][:2]))
        for l in zusammenfassung(b): print(l)
