import sys, json
sys.path.insert(0, '.')
from b_geom import *
import b_teile as T
import numpy as np
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_emd as FE
ordner = sys.argv[1] if len(sys.argv) > 1 else 'zw/v3'
f = FE.lese_emd(open(ordner + '/FORM.EMD', 'rb').read())
for i, (k, m) in enumerate(zip(f['knochen'], f['meshes'])):
    v = np.array(m['ecken']) if m['ecken'] else np.zeros((1, 3))
    print(i, T.KNOCHEN[i][0], 'el', k['eltern'], 'ver', k['versatz'], 'E', len(m['ecken']), 'T', len(m['dreiecke']), 'Q', len(m['vierecke']), 'lo', v.min(0), 'hi', v.max(0))
for c in f['clips']:
    b = c['bilder'][0]
    k = f['pool'][b['kf']]
    print(c['name'], k['wurzel'], k['winkel'][:2], k['winkel'][9:11])
