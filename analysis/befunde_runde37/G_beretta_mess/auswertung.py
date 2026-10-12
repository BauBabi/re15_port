# Runde 37 Spur G — Auswertung eines Messlaufs (state.log + wf.log):
# je Bild Munition (mg), Gegner-HP/Zustand; Ausgabe: jede HP-Aenderung (= ein Treffer), jeder Zustandswechsel,
# jeder Munitionswechsel, die SPAWN-Zeilen des wf.log (Muendung/Rauch/Huelse) mit Bildnummer.
import re, sys, os
wd = sys.argv[1]
st = open(os.path.join(wd, 'state.log'), encoding='utf-8', errors='replace').read().splitlines()
rx_f = re.compile(r'^F(\d+) pad=([0-9a-f]+) .*? mg=(-?\d+)')
rx_e = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=[0-9a-f]+ mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
seg = 0; prevf = -1
last = {}; lastmg = None; lastpad = None
events = []
for line in st:
    m = rx_f.match(line)
    if not m: continue
    f = int(m.group(1));
    if f < prevf: seg += 1; last = {}
    prevf = f
    pad = m.group(2); mg = int(m.group(3))
    if seg == 0: continue          # vor dem Raumwechsel
    if pad != lastpad:
        events.append((f, 'pad %s -> %s' % (lastpad, pad))); lastpad = pad
    if mg != lastmg:
        events.append((f, 'mg %s -> %d' % (lastmg, mg))); lastmg = mg
    for e in rx_e.finditer(line):
        sl = int(e.group(1)); hp = int(e.group(14)); stt = int(e.group(3)); ss1 = int(e.group(4)); stun = int(e.group(9))
        d = int(e.group(10))
        key = (hp, stt, ss1)
        if sl in last:
            php, pst, pss1 = last[sl]
            if hp != php:
                events.append((f, 'slot %d HP %d -> %d (dHP %d) st=%d ss1=%d d=%d stun=%d' % (sl, php, hp, hp - php, stt, ss1, d, stun)))
            elif (stt, ss1) != (pst, pss1):
                events.append((f, 'slot %d st %d/%d -> %d/%d d=%d hp=%d' % (sl, pst, pss1, stt, ss1, d, hp)))
        else:
            events.append((f, 'slot %d t=%s erscheint hp=%d st=%d ss1=%d d=%d @(%s,%s)' % (sl, e.group(2), hp, stt, ss1, d, e.group(11), e.group(12))))
        last[sl] = (hp, stt, ss1)
# wf.log: SPAWN-Zeilen dem Bild zuordnen
wf = os.path.join(wd, 'wf.log')
if os.path.exists(wf):
    cur = None; wseg = 0; wprev = -1
    for line in open(wf, encoding='utf-8', errors='replace'):
        m = re.match(r'^F(\d+) ', line)
        if m:
            fr = int(m.group(1))
            if fr < wprev: wseg += 1
            wprev = fr; cur = fr
            continue
        if 'SPAWN' in line and cur is not None:
            events.append((cur, 'wf ' + line.strip()))
for f, t in sorted(events, key=lambda x: x[0]):
    print('F%-5d %s' % (f, t))
# fx.log (RE15_FX_LOG): Blut-/Treffer-Effekte id=0 mit Alter 1 (frame=1) je Bild = im Bild gespawnt
fx = os.path.join(wd, 'fx.log')
if os.path.exists(fx):
    per = {}
    for line in open(fx, encoding='utf-8', errors='replace'):
        m = re.match(r'^id=0 sub=(\d+) eidx=\d+ frame=(\d+) w\((-?\d+),(-?\d+),(-?\d+)\) phys=0 .*F=(\d+)$', line.strip())
        if not m: continue
        F = int(m.group(6))
        per.setdefault(F, set()).add((int(m.group(2)), m.group(3), m.group(4), m.group(5)))
    neu = [(F, sum(1 for v in per[F] if v[0] == 1)) for F in sorted(per)]
    neu = [(F, n) for F, n in neu if n]
    print('Blut-Effekte (id=0, Alter 1) je Bild:', neu, '-> gesamt', sum(n for _, n in neu))
