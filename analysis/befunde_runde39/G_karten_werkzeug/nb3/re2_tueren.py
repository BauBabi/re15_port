"""Nachbesserung 3 (N2): Wie fuehrt RE2 Retail eine Tuer zwischen zwei Raeumen auf der Karte?
re2_tueren.py <area>   - liest die Area-Tabelle @0x800aaa38 (20 x 8 B {u32 roomTablePtr; u8 roomCount; u8 mapItemBit;
u16 cdFileNo}) und die Raum-Eintraege (0x10 B {u8 u,v,w,h; s16 x,y; s16 mx,my; u8 visBit,altBit,hideL,hideC}) aus
info/re2leon/PSX.EXE (t_addr 0x80010000, Datei = addr - 0x80010000 + 0x800) und die Kartenseite
COMMON/DATA/MAPS/MAPS_<cdFileNo+1>.TIM (256x256 4 bpp, Pixel ab Datei-Offset 0x620 = 8 + 1548 + 12).
Je Raum-Sprite: Tuerpixel (Index 3 / 11 = Goldgelb 0x12dc), und fuer jede Tuer: steht sie AUCH im Sprite des
Nachbarn, dessen Rechteck dieselben Schirmpixel deckt (gemeinsame Wand)?"""
import sys, struct, os
BAUM = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
EXE = os.path.join(BAUM, 'info', 're2leon', 'PSX.EXE')
d = open(EXE, 'rb').read()
fo = lambda a: a - 0x80010000 + 0x800
area = int(sys.argv[1]) if len(sys.argv) > 1 else 2
ptr, cnt, mib, cdf = struct.unpack_from('<IBBH', d, fo(0x800aaa38 + 8 * area))
tim = open(os.path.join(BAUM, 'info', 're2leon', 'COMMON', 'DATA', 'MAPS', 'MAPS_%03d.TIM' % (cdf + 1)), 'rb').read()
px = tim[0x620:0x620 + 32768]
def tex(u, v):
    b = px[v * 128 + u // 2]
    return (b >> 4) if (u & 1) else (b & 15)
print('Area %d: RoomTable 0x%08x, %d Raeume, mapItemBit %d, MAPS_%03d.TIM' % (area, ptr, cnt, mib, cdf + 1))
R = []
for i in range(cnt):
    u, v, w, h, x, y, mx, my, vb, ab, hl, hc = struct.unpack_from('<BBBBhhhhBBBB', d, fo(ptr + 16 * i))
    R.append((u, v, w, h, x, y, vb))
    print('[%2d] @0x%08x uv=(%3d,%3d) wh=(%3d,%3d) xy=(%3d,%3d) visBit=%d' % (i, ptr + 16 * i, u, v, w, h, x, y, vb))
def schirm(i, X, Y):
    u, v, w, h, x, y, vb = R[i]
    if not (x <= X < x + w and y <= Y < y + h): return None
    return tex(u + X - x, v + Y - y)
TUER = (3, 11)
for i in range(cnt):
    u, v, w, h, x, y, vb = R[i]
    tp = [(X, Y) for Y in range(y, y + h) for X in range(x, x + w) if schirm(i, X, Y) in TUER]
    if not tp: continue
    # Tuerpixel zu Gruppen (8-Nachbarschaft)
    rest = set(tp); gruppen = []
    while rest:
        s = rest.pop(); st = [s]; g = [s]
        while st:
            cx, cy = st.pop()
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    q = (cx + dx, cy + dy)
                    if q in rest: rest.remove(q); st.append(q); g.append(q)
        gruppen.append(sorted(g))
    for g in gruppen:
        xs = [p[0] for p in g]; ys = [p[1] for p in g]
        anderer = []
        for j in range(cnt):
            if j == i: continue
            vals = [schirm(j, X, Y) for X, Y in g]
            if all(t is None for t in vals): continue
            anderer.append('r%d:%s' % (j, ''.join('-' if t is None else ('%x' % t) for t in vals)))
        print('Raum %2d Tuer x%d..%d y%d..%d (%d px); dieselben Schirmpixel in: %s' % (
            i, min(xs), max(xs), min(ys), max(ys), len(g), ' '.join(anderer) if anderer else '(kein anderes Sprite)'))

# Ausschnitt: re2_tueren.py <area> <x0> <x1> <y0> <y1> <raum> [<raum> ...] - je Raum-Sprite die Texel (Schirmlage)
if len(sys.argv) > 6:
    x0, x1, y0, y1 = map(int, sys.argv[2:6])
    for ri in map(int, sys.argv[6:]):
        print('Raum %d (Schirm x%d..%d y%d..%d; . = 0, Leerzeichen = ausserhalb des Sprites):' % (ri, x0, x1, y0, y1))
        print('      ' + ''.join(str(X % 10) for X in range(x0, x1 + 1)))
        for Y in range(y0, y1 + 1):
            print('%5d ' % Y + ''.join(' ' if schirm(ri, X, Y) is None else ('.' if schirm(ri, X, Y) == 0 else '%x' % schirm(ri, X, Y))
                                       for X in range(x0, x1 + 1)))
