"""Runde 2: Suche Drehung (pitch, yaw) -> Auge im Licht (Lichtfaktor am Augen-Marker), K4, K10, S2, KS2, Zaehne. Ohne Palettenaenderung."""
import os, sys, json, time
import numpy as np
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM
OUT = os.path.dirname(os.path.abspath(__file__))
P = G.posen()
res = []
grid = [(pi, ya) for pi in (10, 20, 30) for ya in (-34, -45, -55, -65)]
if len(sys.argv) > 1:
    grid = [tuple(float(x) for x in g.split(',')) for g in sys.argv[1:]]
for pi, ya in grid:
    t0 = time.time()
    p = json.loads(json.dumps(KB.PARAM)); p['pitch'] = pi; p['yaw'] = ya
    neu, prot, _ = KB.bauen(p)
    mo = b2.modell(neu)
    m = KM.messen(mo, P, bilder=True)
    r = dict(pitch=pi, yaw=ya, kurz=KM.kurz(m))
    for h in ('135', '85'):
        a = m[h]
        if 'img' not in a:
            continue
        il = a['img']; ir, _ = G.zeichne(mo, P[h], licht=False)
        Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
        ma = a['ma']
        if ma.any():
            r['auge_%s' % h] = dict(px=int(ma.sum()), L_lit=float(np.median(Ll[ma])), L_raw=float(np.median(Lr[ma])), Lmax_lit=float(Ll[ma].max()),
                                    fak=float(np.median(Ll[ma] / np.maximum(Lr[ma], 1))))
        mz = a['mz']
        if mz.any():
            r['zaehne_%s' % h] = dict(px=int(mz.sum()), L_lit=float(np.median(Ll[mz])), Lmax_lit=float(Ll[mz].max()))
        k = a['ids'] == 9
        r['kopf_L_lit_%s' % h] = float(np.median(Ll[k])); r['K10_%s' % h] = a.get('K10')
    r['K4'] = m['135'].get('K4'); r['S2'] = m['85'].get('S2'); r['KS2'] = (m['85']['KS2']['kopf'][0], m['85']['kopf_p50_v']) if 'KS2' in m['85'] else None
    r['s'] = time.time() - t0
    res.append(r)
    print(json.dumps(r, default=str)); sys.stdout.flush()
json.dump(res, open(os.path.join(OUT, 'grid_rot.json'), 'w'), indent=1, default=str)
