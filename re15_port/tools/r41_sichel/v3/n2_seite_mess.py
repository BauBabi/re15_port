# SF V3 Runde 2: Seitenansicht des Kopfs, wie birkin_formen.seite_nah (Spielkamera 50A0/12, Pose Clip 1 Bild 26 Gier 556,
# Parallelprojektion, 0,9 px/E., Mitte T[9]), neutrales Licht (EA.render, Farbe x 1,35). Markiert die Anker aus BAU_INFO.
# Aufruf: python n2_seite_mess.py EMD name [--nur9]
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2')); sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import b2, birkin_buehne as BB, emd_ansichtsblatt as EA, birkin_formen as BF
OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/n2/'


def bild(mo, nur9=False, p=None):
    pn = BF.seite_nah(p or BF.KOPF_V3)
    kf = BB.kf_von_clip(mo, pn['clip'], pn['bild'])
    V, N, F = mo.geometrie(kf); R, T = mo.pose(kf)
    if nur9:
        F = [f for f in F if f['bone'] == 9]
    ri, un, de = pn['achsen']
    col, alp = EA.render(mo, V - T[9].astype(float), N, F, ri, un, de, pn['breite'], pn['hoehe'], pn['massstab'], pn['cx'], pn['cy'])
    return (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8), alp


if __name__ == '__main__':
    mo = b2.modell(sys.argv[1]); name = sys.argv[2]
    for nur9 in (False, True):
        b, a = bild(mo, nur9)
        cv2.imwrite(OUT + 'seitemess%s_%s.png' % ('9' if nur9 else '', name), cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
