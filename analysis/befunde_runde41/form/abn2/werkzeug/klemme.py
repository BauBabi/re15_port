# M6/M7 aus state.log: (a) laengste Folge von Bildern mit Lauf-Clip (mo 5) und Wurzelweg < 4 je Bild,
# (b) kleinster Abstand Spieler - Form-Wurzel ohne Griff (gr=0), Form aktiv st=1.  Eigenes Werkzeug der Abnahme 2.
import re, sys, math
RX_PL = re.compile(r'^F(\d+) pad=[0-9A-Fa-f]+ PL\((-?\d+),(-?\d+),rot=-?\d+,hp=(-?\d+)\).*? gr=(\d+)')
RX_EN = re.compile(r'\[(\d+) t=30 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=[0-9a-f]+ mo=(\d+) af=(\d+) [^]]*@\((-?\d+),(-?\d+),r-?\d+\)\] hp=(-?\d+)')
for d in sys.argv[1:]:
    seg, last = 0, -1; prev = None; run = 0; best = (0, None); dmin = (1e9, None); rows = []
    for line in open(d + '/state.log', errors='replace'):
        m = RX_PL.match(line)
        if not m: continue
        f = int(m.group(1))
        if f <= last: seg += 1
        last = f
        if seg > 1: break
        e = RX_EN.search(line)
        if not e: prev = None; continue
        st, s1, mo, x, z = int(e.group(2)), int(e.group(3)), int(e.group(5)), int(e.group(7)), int(e.group(8))
        if prev is not None and mo == 5 and st == 1:
            w = math.hypot(x - prev[0], z - prev[1])
            if w < 4: run += 1
            else: run = 0
            if run > best[0]: best = (run, f)
        else: run = 0
        prev = (x, z)
        if st == 1 and int(m.group(5)) == 0:
            dd = math.hypot(int(m.group(2)) - x, int(m.group(3)) - z)
            if dd < dmin[0]: dmin = (dd, f, s1, mo)
    print('%s: laengste Standphase im Lauf-Clip 5 = %d Bilder (endet F%s); kleinster Abstand Spieler-Wurzel ohne Griff = %.0f (F%s, ss1=%s mo=%s)' % (d.split('/')[-1], best[0], best[1], dmin[0], dmin[1], dmin[2] if len(dmin) > 2 else '-', dmin[3] if len(dmin) > 3 else '-'))
