# Leser A V3: Port-Ist V1 (EM30_V1_SPITZ.EMD) in der Spielkamera 50A0/12 mit Knochen-Kennung je Bildpunkt rendern
# (Werkzeuge v2: b2.zeichne mit RDT-Licht, Posen posen_seite/posen_vorn.json aus sfv2), dazu Kopf vorn nah (neutrales Licht,
# wie leisten_v2.leiste_rundum). Ausgabe: sfv3/A_v1_<art>_h<h>.png + .npy (ids), A_v1_kopfnah.png. Nur lesen/rendern.
import os, sys, json
import numpy as np, cv2
V2 = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v2'
sys.path.insert(0, V2)
import b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA
SFV2 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2'
OUT = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/'
mo = b2.modell(os.path.join(SFV2, 'emd', 'EM30_V1_SPITZ.EMD'))
for art, hs in (('seite', (83, 85, 92)), ('vorn', (118, 125))):
    P = json.load(open(os.path.join(SFV2, 'posen_%s.json' % art)))
    for h in hs:
        im, ids = b2.zeichne(mo, P[str(h)], None)
        cv2.imwrite(OUT + 'A_v1_%s_h%d.png' % (art, h), cv2.cvtColor(im, cv2.COLOR_RGB2BGR))
        np.save(OUT + 'A_v1_%s_h%d_ids.npy' % (art, h), ids)
        print(art, h, 'Knochen im Bild:', sorted(set(np.unique(ids)) - {-1}))
kf = BB.kf_von_clip(mo, 1, 0)
V, N, F = mo.geometrie(kf)
R, T = mo.pose(kf)
for titel, mitte, right, depth, sk, nm in (
        ('Kopf vorn nah', T[9].astype(float) + np.array([150.0, -150, 0]), np.array([0, 0, 1.0]), np.array([1.0, 0, 0]), 0.9, 'kopfnah'),
        ('Schwanz rechte Flanke nah', (b2.kette_punkte(mo, kf).mean(0)), np.array([1.0, 0, 0]), np.array([0, 0, -1.0]), 0.17, 'schwanznah')):
    col, alp = EA.render(mo, V - mitte, N, F, right, np.array([0.0, 1.0, 0.0]), depth, 960, 960, sk, 480, 480)
    b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
    b[alp == 0] = (92, 96, 104)
    cv2.imwrite(OUT + 'A_v1_%s.png' % nm, cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
    np.save(OUT + 'A_v1_%s_alp.npy' % nm, alp)
print('ok')
