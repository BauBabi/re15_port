from r4lib import *
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
import sys
var = json.loads(sys.argv[2])
rows = []
blobs = [('r3', open(SP + 'EM30_V5r3.EMD', 'rb').read(), None)]
for f in var:
    v = f.pop('versatz', None)
    blob, prot = bauen(f, versatz=v); blobs.append((json.dumps(f) + ' v%s' % (v,), blob, v))
Z = 4
fotos = [G.vorlage(2).astype(np.uint8), G.vorlage(1).astype(np.uint8)]
boxes = (('85', (425, 175, 495, 260)), ('135', (310, 130, 400, 230)))
t = []
for (h, box), fo in zip(boxes, fotos):
    x0, y0, x1, y1 = box
    t.append(cv2.resize(fo[y0:y1, x0:x1], None, fx=Z, fy=Z, interpolation=cv2.INTER_NEAREST))
t.append(np.full((300, 300, 3), 40, np.uint8)); t.append(np.full((300, 300, 3), 40, np.uint8))
def pad(a, h):
    return np.vstack([a, np.full((h - a.shape[0], a.shape[1], 3), 40, np.uint8)]) if a.shape[0] < h else a[:h]
HH = max(Z * 85, Z * 100, 300)
rows.append(np.hstack([pad(x, HH) for x in t]))
for nm, blob, v in blobs:
    mo = b2.modell(blob)
    t = []
    for h, box in boxes:
        img, ids = G.zeichne(mo, P[h])
        x0, y0, x1, y1 = box
        t.append(cv2.resize(img[y0:y1, x0:x1], None, fx=Z, fy=Z, interpolation=cv2.INTER_NEAREST))
    for a in ('seite', 'vorn'):
        p = json.loads(json.dumps(KB.PARAM_R3))
        if v is not None: p['versatz'] = list(v)
        im, _ = profil(mo, a, p=p); t.append(im)
    row = np.hstack([pad(x, HH) for x in t])
    cv2.putText(row, nm[:110], (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 0), 1)
    rows.append(row)
cv2.imwrite(sys.argv[1], cv2.cvtColor(np.vstack(rows), cv2.COLOR_RGB2BGR))
