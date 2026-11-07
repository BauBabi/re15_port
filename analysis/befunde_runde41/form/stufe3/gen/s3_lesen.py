"""Stufe 3: Gerippe von J und Clip-Daten des RE2-G4 lesen (nur Ausgabe)."""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '../../../../..'))
sys.path.insert(0, os.path.join(WURZEL, 're15_port/tools'))
import form_emd as FE

def j_lesen():
    p = os.path.join(WURZEL, 'analysis/befunde_runde41/form/varianten/J_dicker/FORM.EMD')
    return FE.lese_emd(open(p, 'rb').read())

if __name__ == '__main__':
    f = j_lesen()
    print('Knochen:')
    for i, k in enumerate(f['knochen']):
        v = np.array(f['meshes'][i]['ecken'], float) if f['meshes'][i]['ecken'] else np.zeros((1, 3))
        print('%2d eltern %2d versatz %-20s ecken %4d  min %s max %s' % (i, k['eltern'], k['versatz'], len(f['meshes'][i]['ecken']),
              v.min(0).astype(int).tolist(), v.max(0).astype(int).tolist()))
    for ci, c in enumerate(f['clips']):
        for b in c['bilder']:
            k = f['pool'][b['kf']]
            print('Clip %d wurzel %s spur %s' % (ci, k['wurzel'], k['spur']))
            print('   ', ' '.join('%d:%s' % (i, a) for i, a in enumerate(k['winkel']) if any(a)))
            print('    tiefster', FE.tiefster_punkt(f, b))
