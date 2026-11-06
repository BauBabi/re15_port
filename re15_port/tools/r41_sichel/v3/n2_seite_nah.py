# SF V3 Runde 2: "Kopf seitlich nah" - Parallelprojektion in der Blickrichtung der Spielkamera 50A0/12 bei der Seitenpose
# (sfv2/posen_seite.json h83/h85/h92), Mitte T[9], 1,2 px/E., neutrales Licht (EA.render). Zeigt, was die Spielkamera vom Kopf sieht.
# Aufruf: python n2_seite_nah.py <EMD> <name> [h]
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2, birkin_buehne as BB, emd_ansichtsblatt as EA
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
OUT = SC + '/sfv3/n2/'


def achsen(pose):
    r, kam, M, li = b2.raum()
    g = int(pose['gier']); s, c = EA.sin_q12(g) / 4096., EA.cos_q12(g) / 4096.
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    Mm = (kam.V.astype(float) / 4096.) @ Y           # Modellraum -> Kamera
    return Mm[0], Mm[1], -Mm[2]


def seite_nah(mo, pose, sc=1.2, W=900, H=900, knochen=None):
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    V, N, F = mo.geometrie(kf); R, T = mo.pose(kf)
    if knochen is not None:
        F = [f for f in F if f['bone'] in knochen]
    ri, do, de = achsen(pose)
    mitte = T[9].astype(float)
    col, alp = EA.render(mo, V - mitte, N, F, ri, do, de, W, H, sc, W // 2, H // 2)
    return (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8), alp


if __name__ == '__main__':
    mo = b2.modell(sys.argv[1]); name = sys.argv[2]
    P = json.load(open(SC + '/sfv2/posen_seite.json'))
    for h in (sys.argv[3:] or ['85']):
        b, a = seite_nah(mo, P[h])
        b9, a9 = seite_nah(mo, P[h], knochen=(9,))
        cv2.imwrite(OUT + 'seitenah_%s_h%s.png' % (name, h), cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
        cv2.imwrite(OUT + 'seitenah9_%s_h%s.png' % (name, h), cv2.cvtColor(b9, cv2.COLOR_RGB2BGR))
        ys, xs = np.nonzero(a9 > 0); print(h, 'Kopf allein bbox', xs.min(), xs.max(), ys.min(), ys.max())
