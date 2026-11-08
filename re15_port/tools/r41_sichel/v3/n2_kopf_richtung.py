# SF V3 Runde 2: Blickrichtung der Spielkamera 50A0/12 im Kopfraum (Mesh 9) fuer die Posen seite h83/h85/h92 und vorn h118/h125;
# Einteilung der Mesh-9-Flaechen (vorn = Kopfbild V3, seitlich sichtbar, Rest).
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2, birkin_buehne as BB, emd_ansichtsblatt as EA, birkin_formen as BF
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
mo = b2.modell(sys.argv[1])
r, kam, M, li = b2.raum()
def d_kopf(pose):
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    R, T = mo.pose(kf)
    g = int(pose['gier']); s, c = EA.sin_q12(g) / 4096., EA.cos_q12(g) / 4096.
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    Mk = (kam.V.astype(float) / 4096.) @ Y @ (R[9].astype(float) / 4096.)
    d = Mk.T @ np.array([0, 0, 1.0])
    return d / np.linalg.norm(d)
m = mo.meshes[9]
fl = []
for f in m.tf: fl.append([m.tn[f[2 * k]][:3] for k in range(3)])
for f in m.qf: fl.append([m.qn[f[2 * k]][:3] for k in range(4)])
Nf = np.array([np.sum(np.array(x, float), 0) for x in fl]); Nf /= np.linalg.norm(Nf, axis=1, keepdims=True)
dv = np.array(BF.KOPF_V2['blick']); dv /= np.linalg.norm(dv)
vorn = Nf @ dv <= -0.15
print('Flaechen Mesh 9', len(Nf), 'vorn (Kopfbild)', vorn.sum())
for art in ('seite', 'vorn'):
    P = json.load(open(SC + '/sfv2/posen_%s.json' % art))
    for h in sorted(P, key=int):
        d = d_kopf(P[h]); sicht = Nf @ d <= -0.15
        print(art, h, 'blick Kopfraum', np.round(d, 3), 'sichtbar', sicht.sum(), 'davon vorn', (sicht & vorn).sum(), 'nur seitlich', (sicht & ~vorn).sum())
V = np.array([v[:3] for v in m.tv] + [v[:3] for v in m.qv], float)
print('Kopf Bbox lokal min', V.min(0), 'max', V.max(0))
