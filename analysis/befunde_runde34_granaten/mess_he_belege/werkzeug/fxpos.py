# fxpos.py <laufdir> <id> <sub> [F0 F1] : Bildschirmlage (PSX 320x240) je gezeichnetem Platz aus fx.log
import re, sys, os
d, fid, fsub = sys.argv[1], sys.argv[2], sys.argv[3]
f0 = int(sys.argv[4]) if len(sys.argv) > 4 else 0
f1 = int(sys.argv[5]) if len(sys.argv) > 5 else 99999
cur = None
for l in open(os.path.join(d, 'fx.log')):
    l = l.rstrip()
    m = re.match(r'id=(\d+) sub=(\d+) eidx=(-?\d+) frame=(\d+) .* wpos=\((-?\d+),(-?\d+),(-?\d+)\) A=(\d+) B=(\d+) zuender=(\d+) zaehler=(\d+) fl=(\w+) art=(\d+) F=(\d+)$', l)
    if m: cur = m.groups(); continue
    m = re.match(r'\s+-> sx=(-?\d+) sy=(-?\d+) S=(\d+) n=(\d+) c0=(\d+) w16=(\d+)', l)
    if m and cur and cur[0] == fid and cur[1] == fsub and f0 <= int(cur[13]) <= f1:
        print('F%s satz%s fl=%s wpos=(%s,%s,%s) -> sx=%s sy=%s n=%s c0=%s px=%.1f' % (cur[13], cur[3], cur[11], cur[4], cur[5], cur[6],
              m.group(1), m.group(2), m.group(4), m.group(5), int(m.group(6)) / 65536))
