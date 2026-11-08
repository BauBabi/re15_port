# Leser A V3: Port V1 Schwanz ohne Verdeckung (nur Knochen 16-22 gezeichnet, Spielkamera, RDT-Licht) fuer S2/S3/S4;
# H aus dem vollen Render derselben Pose (Knochen 0-15). Ausgabe sfv3/A_v1_frei_h<h>.png und Werte auf stdout/JSON.
import os, sys, json
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v2')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
import b2
from A_common import SCR, lab
from A_schwanz import mittellinie, form, muster, groesste
SFV2 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2'
mo = b2.modell(os.path.join(SFV2, 'emd', 'EM30_V1_SPITZ.EMD'))
P_ = json.load(open(os.path.join(SFV2, 'posen_seite.json')))
out = {}
for h in (83, 85, 92):
    pose = P_[str(h)]
    im, ids = b2.zeichne(mo, pose, None, knochen=range(16, 23))
    full = np.load(SCR + 'A_v1_seite_h%d_ids.npy' % h)
    body = (full >= 0) & ((full < 16) | (full > 22))
    ys, xs = np.nonzero(body); top, sole = ys.min(), ys.max(); H = int(sole - top + 1)
    m = groesste(ids >= 0)
    tx = xs[ys <= top + 2].mean()
    my, mx = np.nonzero(m); i = np.argmin(np.hypot(mx - tx, my - top))
    P = mittellinie(m, np.array([mx[i], my[i]], float))
    fo = form(m, P, H); mu = muster(im, m, P, H)
    cv2.imwrite(SCR + 'A_v1_frei_h%d.png' % h, cv2.cvtColor(im, cv2.COLOR_RGB2BGR))
    np.save(SCR + 'A_v1_frei_P%d.npy' % h, P); np.save(SCR + 'A_v1_frei_m%d.npy' % h, m)
    out[h] = dict(H=H, form=fo, muster=mu, wurzel=P[0].tolist(), spitze=P[-1].tolist(), wurzel_unter_OK_H=round((P[0][1] - top) / H, 3))
    print('V1 frei h%d H%d Laenge %.2fH Dicke %s Knicke %d %s max %.0f bei %.2f | Wurzel %s unterOK %.3f | AK a %s lag %s H | AK L %s lag %s px std L %s' % (
        h, H, fo['laenge_H'], fo['dicke_H'], fo['knicke_gt20'], fo['knick_pos'], fo['max_dwinkel_005H'], fo['max_bei'], [round(v, 1) for v in P[0]],
        out[h]['wurzel_unter_OK_H'], mu['ak_a_max'], mu['ak_a_lag_H'], mu['ak_L_max'], mu['ak_L_lag_px'], mu['L_rest_std']))
    print('   L*prof', mu['L_profil_21']); print('   Breiten', fo['breiten_profil']); print('   Winkel', fo['winkel_profil'])
json.dump(out, open(SCR + 'A_port_v1_frei.json', 'w'), indent=1, default=lambda o: o.item() if hasattr(o, 'item') else str(o))
