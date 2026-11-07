import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
K = os.path.join(FORM, 'rohbau_final', 'form.json')
k = P.Kandidat(K)
m = k.mo.meshes[1]
print(type(m), [a for a in dir(m) if not a.startswith('_')])
print('tv', len(m.tv), m.tv[:3], 'qv', len(m.qv), m.qv[:3])
print('tf', m.tf[:2], 'qf', m.qf[:2])
for b, nm in ((1, 's1'), (26, 'uschenkel_r'), (21, 'hals')):
    m = k.mo.meshes[b]
    for art, vv in (('tv', m.tv), ('qv', m.qv)):
        v = np.array([x[:3] for x in vv], float)
        if not len(v):
            continue
        # Stationen ueber die laengste Achse
        ach = int(np.argmax(v.max(0) - v.min(0)))
        st = {}
        for p in v:
            st.setdefault(int(round(p[ach] / 10) * 10), []).append(p)
        print(nm, art, 'Achse', 'xyz'[ach], 'Stationen:')
        for s in sorted(st):
            a = np.array(st[s]); c = a.mean(0)
            print('   %6d  Ecken %2d  Radius %4.0f  Mitte %s' % (s, len(a), np.linalg.norm(a - c, axis=1).max(), c.round(0)))
