import json, sys, numpy as np
for p in sys.argv[1:]:
    d = json.load(open(p))['je_bild']
    hell = {f: r for f, r in d.items() if 'unterseite' in r and r['unterseite']['L'] >= 10}
    out = {}
    for k in ('unterseite', 'rumpf', 'beinhaut', 'oberseite'):
        v = [r[k] for r in hell.values() if k in r]
        if v:
            out[k] = (len(v), round(float(np.median([x['sd'] for x in v])), 2), round(float(np.median([x['rel'] for x in v])), 4), round(float(np.median([x['L'] for x in v])), 1))
    fr = sorted(int(f) for f in hell)
    print(p, 'Bilder mit Unterseite L*>=10:', len(hell), fr[:5], '...', fr[-5:])
    for k, v in out.items(): print('   %-10s n %3d sd %.2f rel %.4f L* %.1f' % ((k,) + v))
