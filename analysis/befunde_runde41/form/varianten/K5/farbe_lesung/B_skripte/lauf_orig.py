import json
import numpy as np, cv2
import fl, zonen as Z

aus = {}
for n in (1, 2):
    v = cv2.imread(fl.FORM + '/bild%d.png' % n)[:, :, ::-1].copy()
    e, T, clip = Z.rueckrechnen(v, n)
    res, alle = Z.zonen_messen(e, T, clip, Z.regionen(n))
    aus['bild%d' % n] = res
    np.save(fl.S + '/T_orig%d.npy' % n, T)
    np.save(fl.S + '/e_orig%d.npy' % n, e)
    np.save(fl.S + '/clip_orig%d.npy' % n, clip)
    np.save(fl.S + '/fig_orig%d.npy' % n, alle)
    for r in res:
        print(n, r['zone'], r.get('px'), r.get('anteil_figur'), r.get('klassen'), 'e', r.get('e_mittel'), 'T', r.get('T_mittel'), r.get('T_lab'), r.get('mehrton'))
        if 'klassen_farbe' in r:
            print('    ', r['klassen_farbe'])
json.dump(aus, open(fl.S + '/zonen_orig.json', 'w'), indent=1)
