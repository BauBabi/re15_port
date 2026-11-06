# Zahlen-Pruefer V3 r1: Schwanzgelenke je Szenentakt aus debug.log (eigener Parser), Gelenkwinkel im Raum und im Bild der
# Spielkamera 50A0/12 (Projektion b2.projiziere = Zeichen-Infrastruktur), dazu Boden-Durchstoss (Gelenk-y > Birkin-y).
# python PZ_gang.py debug.log [NAME]
import os, sys, re, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2

def lesen(p):
    G, P = {}, {}
    for z in open(p, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)$', z)
        if m:
            pts = re.findall(r'(\d)\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))
            G[int(m.group(1))] = (int(m.group(2)), np.array([[int(a), int(b), int(c)] for _, a, b, c in pts], float))
        m = re.search(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+) bild (\d+) gier (-?\d+) \((-?\d+),(-?\d+)\)', z)
        if m:
            P[int(m.group(1))] = dict(F=int(m.group(2)), clip=int(m.group(3)), bild=int(m.group(4)), gier=int(m.group(5)))
    return G, P

def winkel(Q):
    d = np.diff(Q, axis=0); n = np.linalg.norm(d, axis=1)
    u = d / n[:, None]
    return np.degrees(np.arccos(np.clip((u[:-1] * u[1:]).sum(1), -1, 1))), n

if __name__ == '__main__':
    G, P = lesen(sys.argv[1])
    welt = dict(gier=0, x=0, z=0, y=0)
    je = {}
    for h, (by, Q) in sorted(G.items()):
        w3, l3 = winkel(Q)
        q = b2.projiziere(welt, Q)
        w2, l2 = winkel(np.c_[q, np.zeros(len(q))])
        boden = Q[:, 1] - by                     # PSX: y nach unten; > 0 = unter dem Boden
        je[h] = dict(raum=[round(x, 1) for x in w3], bild=[round(x, 1) for x in w2], bild_glied_px=[round(x, 1) for x in l2],
                     unter_boden_max=float(boden.max()), clip=P.get(h, {}).get('clip'))
    def zus(lo, hi):
        hs = [h for h in je if lo <= h <= hi]
        return dict(takte=len(hs), raum_max=max(max(je[h]['raum']) for h in hs),
                    raum_ueber20_max=max(sum(1 for x in je[h]['raum'] if x > 20) for h in hs),
                    bild_max=max(max(je[h]['bild']) for h in hs),
                    bild_max_ohne_kurz=max(max([w for i, w in enumerate(je[h]['bild'])
                                                if min(je[h]['bild_glied_px'][i], je[h]['bild_glied_px'][i + 1]) >= 0.4 * np.median(je[h]['bild_glied_px'])] or [0]) for h in hs),
                    bild_ueber20_max=max(sum(1 for x in je[h]['bild'] if x > 20) for h in hs),
                    unter_boden_max=max(je[h]['unter_boden_max'] for h in hs))
    out = dict(gang_h84_h132=zus(84, 132), halten_h16_h70=zus(16, 70), alle=zus(min(je), max(je)), je_takt=je)
    print(json.dumps({k: v for k, v in out.items() if k != 'je_takt'}))
    for h in (84, 90, 96, 102, 108, 114, 120, 126, 132):
        if h in je:
            print(h, je[h])
    if len(sys.argv) > 2:
        json.dump(out, open(os.path.join('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/pz', 'gang_%s.json' % sys.argv[2]), 'w'), indent=1)
