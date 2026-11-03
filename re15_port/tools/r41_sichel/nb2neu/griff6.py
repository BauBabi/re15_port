"""NB2neu Griff-Synchronitaet: Abstand Sichelhand Birkin (Typ 0x30 Knochen 15) zu Griffknochen Spider-Man (Typ 0x2B Knochen 20)
je gezeichnetem Bild aus dem Messhaken RE15_GRIFF_LOG (main.c), in Welt-Einheiten und im Bild (Video-Koordinaten 640x480,
c6.proj durch Cut 12 und die Registrierung). Zuordnung Bild h ueber die Letterbox (c6.off). python griff6.py lauf[,lauf..] [h,h..]"""
import sys, os, re, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
def lese(run):
    R = {}
    for L in open(os.path.join(rundir(run), 'griff.log'), errors='ignore'):
        m = re.match(r'\[griff\] F(\d+) typ(\w+) mo=(\d+) af=(\d+)(.*)', L)
        if not m: continue
        P = {int(b): np.array([int(x), int(y), int(z)], float) for b, x, y, z in re.findall(r' (\d+)\((-?\d+),(-?\d+),(-?\d+)\)', m.group(5))}
        R.setdefault(int(m.group(1)), {})[m.group(2)] = dict(mo=int(m.group(3)), af=int(m.group(4)), P=P)
    return R
def abstaende(run):
    L = lese(run); o = off(run); out = {}
    for F, e in L.items():
        if '30' not in e or '2B' not in e: continue
        a, b = e['30']['P'][15], e['2B']['P'][20]
        pa, pb = np.array(proj(a)), np.array(proj(b))
        out[F - o] = dict(welt=int(np.linalg.norm(a - b)), bild=int(np.linalg.norm(pa - pb)), hand=[int(v) for v in pa], griff=[int(v) for v in pb],
                          sm_mo=e['2B']['mo'], b_mo=e['30']['mo'], b_af=e['30']['af'])
    return out
if __name__ == '__main__':
    runs = sys.argv[1].split(','); hs = [int(x) for x in sys.argv[2].split(',')] if len(sys.argv) > 2 else list(range(0, 80, 2))
    A = {r: abstaende(r) for r in runs}
    for h in hs:
        z = 'h%-3d' % h
        for r in runs:
            d = A[r].get(h)
            z += ' | %-4s ' % r + ('-' if d is None else 'Welt %5d Bild %4d px  Hand %s Griff %s B-clip %d/%d' % (d['welt'], d['bild'], d['hand'], d['griff'], d['b_mo'], d['b_af']))
        print(z)
    for r in runs:
        json.dump(A[r], open(os.path.join(RUNS, 'griff_%s.json' % r), 'w'))
