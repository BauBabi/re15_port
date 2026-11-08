import sys, json, copy
B = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/bau4'
sys.path.insert(0, B)
import such4 as Q
Q.setze_f1(Q.K.K5 + '/farbe/K5c_farbe_v3')
p0 = json.load(open(sys.argv[1], encoding='utf-8'))
p0 = p0.get('par', p0)
N_MIN = 500
def lauf(kap, boden=False):
    p = copy.deepcopy(p0)
    for g in p: p[g].update(kappa=kap, boden=boden)
    return Q.ader_ring(p, drucken=False)
boden = lauf(0.0, True)
erg = {}
for kap in (1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0):
    o = lauf(kap)
    for g, d in o.items():
        ok = True; grund = []
        for k, v in d.items():
            if k.startswith('texel'):
                if not (v['L'][0] <= v['L'][1] and v['a'][0] <= v['a'][1]): ok = False; grund.append(k)
            elif v['n_ader'] >= N_MIN:
                da = v['a'][0] - v['a'][1]
                db = boden[g][k]['a'][0] - boden[g][k]['a'][1]
                if v['L'][0] > v['L'][1] or da > max(0.0, db): ok = False; grund.append('%s dL %.1f da %.2f boden %.2f' % (k, v['L'][0] - v['L'][1], da, db))
        if ok and g not in erg:
            erg[g] = kap
        print('kappa %.1f %-16s %s %s' % (kap, g, 'ok' if ok else 'nein', grund), flush=True)
    if len(erg) == len(o):
        break
print('KAPPA', json.dumps(erg))
json.dump(dict(kappa=erg, boden={g: {k: v for k, v in d.items()} for g, d in boden.items()}), open(B + '/kappa4_end.json', 'w'), indent=1)
