"""Pruefung V4 r5 Sicht: Laufposen h85..h135 aus debug.log des Laufs sfv4/r/V4r5 (riegel_v3.lauf_posen) mit b2 gezeichnet,
mit Id-Bild - um Flecken im Spielbild einem Mesh zuzuordnen (Lichtgegenprobe b2 gegen FRAMEDUMP laut r5_probe dE00 <= 0,9)."""
import sys, os, numpy as np
H = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, H + '/v4'); sys.path.insert(0, H + '/nb2neu')
import v4_gemein as G, b2, riegel_v3 as R3
from PIL import Image
OUT = sys.argv[1]
for run, emd in (('V4r5', G.EMD_V4), ('V4r4', os.path.join(G.SFV4, 'emd', 'EM30_V4r4.EMD'))):
    PO = R3.lauf_posen(os.path.join(G.SFV4, 'r', run, 'debug.log')); mo = b2.modell(emd)
    for h in (85, 90, 100, 118, 135):
        if h not in PO: print(run, h, 'keine Pose'); continue
        img, ids = G.zeichne(mo, PO[h])
        Image.fromarray(np.ascontiguousarray(img).astype(np.uint8)).save(OUT + '/lp_%s_h%d.png' % (run, h)); np.save(OUT + '/lp_%s_h%d_ids.npy' % (run, h), ids)
        print(run, h, 'ok', sorted(set(np.unique(ids).tolist())))
