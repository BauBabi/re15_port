# Message-Id -> Bildbereich (msg a=1) aus state.log; nur lesen.
import re, sys
ids = []
cur = None
for line in open(sys.argv[1], encoding='latin-1'):
    m = re.match(r'F(\d+) .*cam=(\d+) msg\(a=(\d) fsm=(\d) id=(\d+)', line)
    if not m: continue
    f, cam, a, i = int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(5))
    if a == 1:
        if cur and cur[0] == i and f - cur[2] <= 2: cur[2] = f
        else:
            if cur: ids.append(cur)
            cur = [i, f, f, cam]
if cur: ids.append(cur)
for i, s, e, c in ids:
    print(f"id={i:2d} F{s}-F{e} ({e-s+1} Bilder) cam={c}")
