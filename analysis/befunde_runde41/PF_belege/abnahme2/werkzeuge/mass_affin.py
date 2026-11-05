import numpy as np, json, sys
import kasten_mass as km, referenz, affin
vp = float(sys.argv[1]) if len(sys.argv) > 1 else 131.3
r = referenz.rueckwand(); f, d = referenz.fluegel(vp)
ref = {'innen': r, 'fluegel': f}
out = {}
for fig in ('leon', 'elza'):
    for st in ('vor', 'nach'):
        b = np.load('k_k_%s_%s_auf.npy' % (st, fig))
        i, fl = km.port_flaechen(b, '2010' if fig == 'leon' else '2011')
        for n, img in (('innen', i), ('fluegel', fl)):
            if fig == 'elza':
                bl = np.load('k_k_%s_leon_auf.npy' % st)
                il, fll = km.port_flaechen(bl)
                same = np.array_equal(img, il if n == 'innen' else fll)
                print('elza %s %s entzerrt bildgleich mit leon: %s' % (st, n, same))
                if same:
                    out['elza_%s_%s' % (st, n)] = out['leon_%s_%s' % (st, n)]
                    continue
            e = affin.registriere(img, ref[n])
            out['%s_%s_%s' % (fig, st, n)] = e
            print('VP %.1f %s %s %-8s NCC %.3f  V %.3f  Senkrechte %+.2f  Waagrechte %+.2f  Achsenwinkel %.2f' % (vp, fig, st, n, e['ncc'], e['V'], e['senkrechte'], e['waagrechte'], e['achsenwinkel']), flush=True)
            if fig == 'leon' and st == 'nach':
                for gp in (5.0,):
                    g = affin.registriere(affin.scher_gegenprobe(img, gp), ref[n])
                    print('   Gegenprobe Port %+.0f Grad geschert: Senkrechte %+.2f (Differenz %+.2f)  V %.3f' % (gp, g['senkrechte'], g['senkrechte'] - e['senkrechte'], g['V']), flush=True)
json.dump(out, open('mass_affin_%s.json' % vp, 'w'), indent=1)
