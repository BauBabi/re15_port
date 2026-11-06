import sys, os, numpy as np, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G, b2
from PIL import Image
P = G.posen()
for name, emd in (('V4', G.EMD_V4), ('V3r2', G.EMD_V3), ('REF2', G.REF2)):
    m = b2.modell(emd)
    for h in ('135', '85'):
        img, ids, idsv = G.zeichne(m, P[h], voll=True)
        Image.fromarray(np.ascontiguousarray(img).astype(np.uint8)).save('%s_%s.png' % (name, h))
        np.save('%s_%s_ids.npy' % (name, h), idsv)
        ok, so, hh = G.h_port(idsv)
        print(name, h, 'H_port', ok, so, hh, 'ids', sorted(set(np.unique(ids).tolist())))
    # neutrales Licht rundum (Texelfarbe)
    img, ids = b2.zeichne(m, P['85'], None, licht=False)
    Image.fromarray(np.ascontiguousarray(img).astype(np.uint8)).save('%s_85_neutral.png' % name)
for b in (1, 2):
    Image.fromarray(np.clip(G.vorlage(b), 0, 255).astype(np.uint8)).save('vorlage%d_entbl.png' % b)
print('ok')
