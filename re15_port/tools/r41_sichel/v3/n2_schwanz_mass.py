# SF V3 Runde 2: Schwanz in der Seitenkamera (P_port.messen) fuer h83 (keine Riegel-Pose) und die Riegel-Posen h85/h92.
# Aufruf: python n2_schwanz_mass.py EMD [EMD ...]
import os, sys, json
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import P_port
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
P = json.load(open(SC + '/sfv2/posen_seite.json'))
for emd in sys.argv[1:]:
    for h in ('83', '85', '92'):
        d, vis = P_port.messen(emd, int(h), P[h])
        f, c = d['form'], d['farbe']
        print(os.path.basename(emd), h, 'Dicke 0,2/0,5/0,9 %.3f/%.3f/%.3f' % (f['dicke_H'][0.2], f['dicke_H'][0.5], f['dicke_H'][0.9]),
              'max %.1f Grad, Knicke %d' % (f['max_dwinkel_005H'], f['knicke']), 'Austritt %.3f' % d['austritt_unter_OK_H'],
              'L/C %.3f/%.3f' % (c['L_verh'], c['C_verh']))
