# SF Messblatt V3 Nachtrag (Pruefer): Port-Render in der Spielkamera 50A0/12, vorn h118/h125 (Posen sfv2/posen_vorn.json),
# 640er b2.zeichne x2 NEAREST = 1280er Massstab wie die KI-Videobilder; speichert Bild + Knochen-Ids als .npy.
# Aufruf: python N_render.py EMD NAME   -> sfv3/N/<NAME>_h118.npz, _h125.npz (im, ids, H, ok)
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
PV = json.load(open(SC + '/sfv2/posen_vorn.json'))
emd, name = sys.argv[1], sys.argv[2]
mo = b2.modell(emd)
for h in (118, 125):
    im, ids = b2.zeichne(mo, PV[str(h)], None)
    ok, so = b2.ok_sohle(ids, ausser=tuple(range(16, 23)))
    im2 = cv2.resize(im, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    ids2 = cv2.resize(ids.astype(np.int16), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    np.savez_compressed(SC + '/sfv3/N/%s_h%d.npz' % (name, h), im=im2, ids=ids2, H=2 * (so - ok), ok=2 * ok)
    cv2.imwrite(SC + '/sfv3/N/%s_h%d.png' % (name, h), cv2.cvtColor(im2, cv2.COLOR_RGB2BGR))
    print(name, h, 'H', 2 * (so - ok), 'ok', 2 * ok)
