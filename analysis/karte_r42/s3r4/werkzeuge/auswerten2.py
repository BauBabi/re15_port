# Stufe 3 Runde 2: Auswertung UNABHAENGIG von der Marker-Rechnung der Engine.
#  (1) Steht der Punkt auf dem ROTEN Raum?  Mass des Pruefers (pruef_s3_r1/py/eigen.py): Anteil roter Pixel im
#      2-px-Rand um den 5x5-Punkt, Grenze 0.3.  Zusaetzlich: Mittelpunkt-Umgebung 3x3 ohne Punkt-Pixel.
#  (2) Raeume mit AUSGELIEFERTER Zeile (@0x800768b0, direkt aus PSX.EXE gelesen): Punkt == Original-Formel
#      FUN_800473f8 @0x8004741c-0x80047528 an der Lage aus dem Spiel-Log (Abstand in px, nur gemeldet, da der
#      Port danach ins Rechteck klemmt und die Tuer anzieht).
#  (3) Blatt: bestes Sweep-Blatt (Wandpixel-Vergleich).
# Aufruf: auswerten2.py <kampagne.txt> <schussordner> <aus.json>
import os, re, sys, json, struct, collections
from PIL import Image
S = os.path.dirname(os.path.abspath(__file__))
SW = os.path.join(S, '..', 's3', 'sweep')
EXE = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/info/Re1.5/PSX.EXE', 'rb').read()
BASIS = [0, 38, 50, 65, 77, 98]
ring = [(1,0),(2,0),(3,0),(0,1),(4,1),(0,2),(4,2),(0,3),(4,3),(1,4),(2,4),(3,4)]
innen = [(1,1),(2,1),(3,1),(1,2),(3,2),(1,3),(2,3),(3,3)]
def finde(p, W, H):
    hits = []
    for y in range(0, H - 5):
        for x in range(0, W - 5):
            c = p[x + 1, y]
            if c[0] < 120 or c[2] > 60: continue
            if all(p[x+dx, y+dy] == c for dx, dy in ring) and all(p[x+dx, y+dy] != c for dx, dy in innen):
                hits.append((x + 3, y + 3))
    return hits
def rot(c): return 40 <= c[0] <= 56 and c[1] <= 12 and 40 <= c[2] <= 72
def zeile(rid):
    st = (rid >> 12) - 1; va = 0x800768b0 + 8 * (BASIS[st] + ((rid >> 4) & 0xFF)); o = 0x800 + va - 0x80010000
    ox, oy, sx, sy = struct.unpack_from('<hhHH', EXE, o)
    return (ox, oy, sx, sy, va) if sx > 1 and sy > 1 else None
def formel(x, z, r):
    ox, oy, sx, sy, _ = r
    t = (((x + 32000) * 10 * sx) >> 20) + 5; t2 = -((((z + 32000) * 10 * sy) >> 20) + 5)
    return int(t / 10) + ox, int(t2 / 10) + oy
def main():
    liste, sch, aus = sys.argv[1], sys.argv[2], sys.argv[3]
    sweep = {pg: Image.open(os.path.join(SW, 'blatt%02d.bmp' % pg)).convert('RGB').load() for pg in range(13)}
    erg = []
    for l in open(liste):
        t = l.split()
        if not t: continue
        tag, rid = t[0], t[1]
        r = {'tag': tag, 'rid': rid, 'soll': (int(t[2]), int(t[4])), 'aus': t[7], 'geh': t[6] != '0'}
        d = os.path.join(sch, tag); f = os.path.join(d, 'karte.bmp')
        pos = None
        if os.path.exists(os.path.join(d, 'debug.log')):
            for ln in open(os.path.join(d, 'debug.log'), errors='replace'):
                m = re.search(r'\[walk\] F(\d+) .*pl pos=\((-?\d+),(-?\d+),(-?\d+)\)', ln)
                if m: pos = (int(m.group(2)), int(m.group(4)))
        r['lage'] = pos
        if not os.path.exists(f): r['urteil'] = 'KEIN ABZUG'; erg.append(r); continue
        im = Image.open(f).convert('RGB'); p = im.load(); W, H = im.size
        wand = [(x, y) for y in range(H) for x in range(W) if p[x, y] == (176, 176, 176)]
        if wand:
            sc = {pg: sum(1 for x, y in wand if sweep[pg][x, y] == (176, 176, 176)) / len(wand) for pg in sweep}
            r['blatt'] = max(sc, key=sc.get)
        r['rot_px'] = sum(1 for y in range(H) for x in range(W) if rot(p[x, y]))
        h = finde(p, W, H)
        if not h: r['urteil'] = 'KEIN PUNKT'; erg.append(r); continue
        m = h[0]; r['marker'] = m
        pts = [(m[0]-3+dx, m[1]-3+dy) for dx in range(-2, 7) for dy in range(-2, 7) if not (0 <= dx <= 4 and 0 <= dy <= 4)]
        um = sum(1 for x, y in pts if 0 <= x < W and 0 <= y < H and rot(p[x, y])) / len(pts)
        r['um'] = round(um, 2)
        z = zeile(int(rid, 16))
        if z and pos:
            fx, fy = formel(pos[0], pos[1], z); r['formel'] = (fx, fy); r['formel_va'] = '0x%08X' % z[4]
            r['formel_px'] = max(abs(fx - m[0]), abs(fy - m[1]))
        if r['rot_px'] == 0: r['urteil'] = 'RAUM NICHT ROT'
        elif um < 0.3: r['urteil'] = 'NEBEN DEM ROTEN RAUM'
        else: r['urteil'] = 'ok'
        erg.append(r)
    json.dump(erg, open(aus, 'w'), indent=0)
    c = collections.Counter(r['urteil'] for r in erg)
    print('Abzuege', len(erg), dict(c))
    for r in erg:
        if r['urteil'] != 'ok': print('  ', r['tag'], r['urteil'], 'lage', r.get('lage'), 'punkt', r.get('marker'), 'um', r.get('um'), 'blatt', r.get('blatt'))
if __name__ == '__main__': main()
