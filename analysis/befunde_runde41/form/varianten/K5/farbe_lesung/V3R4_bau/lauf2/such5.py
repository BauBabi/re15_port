"""Suche Lauf 2 (parallel): python such5.py start.json aus.json [sweeps]"""
import sys, json, copy, math
from concurrent.futures import ProcessPoolExecutor
sys.path.insert(0, r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5")
from worker5 import *

if __name__ == "__main__":
    POOL = ProcessPoolExecutor(2)
    par = json.load(open(sys.argv[1], encoding='utf-8'))
    par = par.get('par', par)
    for g in par:
        for k in GRENZEN:
            par[g].setdefault(k, 1.0 if k in ('q', 'kappa', 'a', 'g') else 0.0)
            par[g][k] = grenz(g, k, par[g][k])
    aus = sys.argv[2]
    sweeps = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    r, gn, n, f = bewerte(par)
    print('Start %d/%d  f %.3f' % (gn, n, sum(f.values())), flush=True)
    verlauf = []
    for sw in range(sweeps):
        for k in ('C', 'h', 'g', 'beta', 'a', 'q', 'kappa'):
            kand = []
            for sgn in (+1, -1):
                p2 = copy.deepcopy(par)
                for g in p2:
                    x = p2[g][k]
                    x = x * math.exp(sgn * SCHRITT[k]) if k == 'C' else x + sgn * SCHRITT[k]
                    p2[g][k] = round(grenz(g, k, x), 4)
                if p2 == par:
                    continue
                kand.append(p2)
            kand = [(p2,) + e for p2, e in zip(kand, POOL.map(bewerte, kand))]
            for g in par:
                best = (f[g], None)
                for p2, r2, g2, n2, f2 in kand:
                    if f2[g] < best[0] - 1e-9:
                        best = (f2[g], p2[g][k])
                if best[1] is not None:
                    par[g][k] = best[1]
            r, gn, n, f = bewerte(par)
            verlauf.append(dict(sweep=sw, k=k, gruen=gn, n=n, f=round(sum(f.values()), 4), par=copy.deepcopy(par)))
            json.dump(par, open(aus, 'w'), indent=1)
            json.dump(verlauf, open(aus.replace('.json', '_verlauf.json'), 'w'), indent=1)
            rot = [kk for kk, v in r.items() if v['b'] and not v['ok']]
            print('sweep %d %-5s %d/%d f %.3f  ROT %s' % (sw, k, gn, n, sum(f.values()), rot), flush=True)
        for k in SCHRITT:
            SCHRITT[k] *= 0.6
    print(json.dumps(par))
