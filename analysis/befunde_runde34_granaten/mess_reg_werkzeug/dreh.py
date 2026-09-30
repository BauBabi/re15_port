# Drehung je Bild aus dem Zustandslog (Segment NACH dem Debug-Sprung), zusammengefasst zu Laeufen gleicher (pad, ac, delta).
import sys, re
def seg(path):
    L = [l for l in open(path, encoding='latin1') if l.startswith('F')]
    # zweites Segment: ab dem Rueckgang des Bildzaehlers
    out = []; prev = -1; cur = []
    for l in L:
        m = re.match(r'F(\d+) pad=([0-9a-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+) ac=(\d+)', l)
        if not m: continue
        f = int(m.group(1))
        if f < prev: out.append(cur); cur = []
        prev = f
        cur.append(dict(F=f, pad=int(m.group(2), 16), rot=int(m.group(5)), pst=int(m.group(7)), ps1=int(m.group(8)), ps2=int(m.group(9)), mo=int(m.group(10)), ac=int(m.group(11))))
    out.append(cur)
    return out[-1]
def names(p):
    n = []
    for b, s in ((0x0800,'R1'),(0x0080,'L'),(0x0020,'R'),(0x0010,'U'),(0x0040,'D'),(0x8000,'SQ'),(0x0400,'L1')):
        if p & b: n.append(s)
    return '+'.join(n) or '-'
def runs(rows):
    res = []
    for i in range(1, len(rows)):
        d = rows[i]['rot'] - rows[i-1]['rot']
        if d > 32767: d -= 65536
        if d < -32768: d += 65536
        key = (names(rows[i]['pad']), rows[i]['ac'], d)
        if res and res[-1][0] == key and res[-1][2] == rows[i]['F'] - 1:
            res[-1][2] = rows[i]['F']
        else:
            res.append([key, rows[i]['F'], rows[i]['F']])
    return res
if __name__ == '__main__':
    rows = seg(sys.argv[1])
    for key, a, b in runs(rows):
        if key[0] == '-' and key[2] == 0: continue
        print('F%d-%d (%d) pad=%s ac=%d d=%+d' % (a, b, b - a + 1, key[0], key[1], key[2]))
