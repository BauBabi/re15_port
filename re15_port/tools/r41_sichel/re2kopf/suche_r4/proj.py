from r4lib import *
import birkin_buehne as BB
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
def proj(blob, h):
    mo = b2.modell(blob); pose = P[h]
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild']); R, Tt = mo.pose(kf)
    V = np.array([v[:3] for v in mo.meshes[9].tv], float)
    W = (R[9].astype(float) / 4096.0 @ V.T).T + Tt[9].astype(float)
    return b2.projiziere(pose, W)
for nm, blob in (('r3', open(SP + 'EM30_V5r3.EMD', 'rb').read()), ('r4a', bauen(dict(sx=1.0, ratio=1.0))[0]), ('r4b', bauen(dict(sx=0.85, ratio=1.0))[0])):
    for h in ('85', '135'):
        xy = proj(blob, h)
        o = np.argsort(-xy[:, 0])[:8] if h == '85' else np.argsort(xy[:, 0])[:8]
        print(nm, h, [(int(i), round(float(xy[i, 0]), 1), round(float(xy[i, 1]), 1)) for i in o])
        print('    auge', [(i, round(float(xy[i, 0]), 1), round(float(xy[i, 1]), 1)) for i in (2, 3, 5, 17, 39)])
