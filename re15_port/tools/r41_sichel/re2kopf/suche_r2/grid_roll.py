"""Runde 2: Suche (pitch, yaw, roll) -> Augenfaecher im Licht (Lichtfaktor p50/p90 auf T19/T20/T41), Faechermitte (K4), K10, S2, KS2, Zaehne."""
import os, sys, json, time
import numpy as np
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM
OUT = os.path.dirname(os.path.abspath(__file__))
P = G.posen()
FAN = [19, 20, 41]
grid = [(pi, ya, ro) for pi in (10, 20) for ya in (-25, -34, -45) for ro in (-10, -15, -20, -25)]
if len(sys.argv) > 1:
    grid = [tuple(float(x) for x in g.split(',')) for g in sys.argv[1:]]
res = []
for pi, ya, ro in grid:
    p = json.loads(json.dumps(KB.PARAM)); p['pitch'] = pi; p['yaw'] = ya; p['roll'] = ro
    neu, prot, _ = KB.bauen(p)
    mo = b2.modell(neu)
    r = dict(pitch=pi, yaw=ya, roll=ro)
    for h in ('135', '85'):
        il, ids, iv = G.zeichne(mo, P[h], voll=True); ir, _ = G.zeichne(mo, P[h], licht=False)
        Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
        ok, so, H = G.h_port(iv); kb = G.kopfbox(ids)
        if not kb:
            r[h] = 'kein Kopf'; continue
        x0, y0, x1, y1 = kb
        _i, idf = G.zeichne(KM.teil(mo, FAN), P[h], licht=False)
        m = (idf == 9) & (ids == 9)
        d = dict(K10=((x1 - x0 + 1) / H, (y1 - y0 + 1) / H), kopf_L=float(np.median(Ll[ids == 9])))
        if m.any():
            f = Ll[m] / np.maximum(Lr[m], 1); ys, xs = np.nonzero(m)
            d['fan'] = dict(px=int(m.sum()), f=[float(v) for v in np.percentile(f, [10, 50, 90]).round(2)], wh=((xs.ptp() + 1) / H, (ys.ptp() + 1) / H))
            if h == '135':
                d['K4'] = ((0.5 * (x0 + x1) - xs.mean()) / H, (ys.mean() - y0) / H)
            else:
                d['S2'] = ((x1 - xs.mean()) / H, (so - ys.mean()) / H)
        mz = KM.marker_render(mo, P[h], KM.zahn_texel(mo), KB.ZEILE_G3)
        d['zaehne'] = (int(mz.sum()), float(np.median(Ll[mz])) if mz.any() else None)
        if h == '85':
            import riegel_v4 as RV
            V = G.videoraum(il); l3, bx3 = RV.fenster_lab(V, kb, RV.FENSTER['KS2'])
            km = ids == 9; sub = V[bx3[1]:bx3[3] + 1, bx3[0]:bx3[2] + 1]; mk = km[bx3[1]:bx3[3] + 1, bx3[0]:bx3[2] + 1]
            d['KS2'] = (float(RV.lab1(sub[mk].mean(0))[0]) if mk.any() else None, float(mk.mean()), float(np.median(G.lab(V)[km][:, 0])))
        r[h] = d
    res.append(r); print(json.dumps(r, default=str)); sys.stdout.flush()
json.dump(res, open(os.path.join(OUT, 'grid_roll.json'), 'w'), indent=1, default=str)
