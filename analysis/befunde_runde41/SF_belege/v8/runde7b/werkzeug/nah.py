# Offline-Nahansicht Schwanz: PV / P2 / PV von unten / Drehungen, je Modell eine Zeile
import sys, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, cv2
import v8_gemein as V, tafeln_v8 as TV, r3_gemein as R3, f_rast as FR
modelle = [(os.path.basename(p).split('.')[0], open(p, 'rb').read()) for p in sys.argv[2:]]
def box(blob, pose, rand=10, nur=None):
    mo = V.modell(blob)
    b = FR.zeichne(mo, pose, None, licht=False)['bone'][:480]
    m = ((b >= 16) & (b <= 22) if nur == "s" else b >= 0)
    if os.environ.get("ALLES"): m = b >= 0
    ys, xs = np.nonzero(m)
    return (max(int(xs.min()) - rand, 0), max(int(ys.min()) - rand, 0), min(int(xs.max()) + rand, 640), min(int(ys.max()) + rand, 480))
ans = []
for nm in sys.argv[1].split(','):
    if nm == 'PV': ans.append(('PV', V.PV))
    elif nm == 'P2': ans.append(('P2', R3.P2))
    elif nm == 'unten':
        p = dict(V.PV); p['y'] = -1800; ans.append(('PV von unten', p))
    elif nm.startswith('g'):
        p = dict(V.PV); p['gier'] = (V.PV['gier'] + int(nm[1:])) % 4096; ans.append(('PV gier+%s' % nm[1:], p))
    elif nm.startswith('h'):
        p = dict(R3.P2); p['gier'] = (R3.P2['gier'] + int(nm[1:])) % 4096; ans.append(('P2 gier+%s' % nm[1:], p))
zeilen = []
for mn, blob in modelle:
    ks = []
    for an, pose in ans:
        bx = box(modelle[0][1], pose, nur='s')
        f = float(os.environ.get("H", 300)) / max(bx[3] - bx[1], 1)
        ks.append(TV.kachel(TV.render(blob, pose), bx, f, '%s %s' % (mn, an)))
    zeilen.append(TV.reihe(ks))
w = max(z.shape[1] for z in zeilen)
im = np.vstack([np.hstack([z, np.zeros((z.shape[0], w - z.shape[1], 3), np.uint8)]) for z in zeilen])
cv2.imwrite(os.environ.get('OUT', 'nah.png'), im[:, :, ::-1])
