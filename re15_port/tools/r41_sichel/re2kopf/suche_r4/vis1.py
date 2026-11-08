from r4lib import *
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
import sys
var = json.loads(sys.argv[2])
rows = []
blobs = [('r3', open(SP + 'EM30_V5r3.EMD', 'rb').read())]
for f in var:
    blob, prot = bauen(f); blobs.append((json.dumps(f), blob))
for nm, blob in blobs:
    mo = b2.modell(blob)
    t = []
    for h, box in (('85', (420, 170, 500, 250)), ('135', (300, 135, 400, 235))):
        img, ids = G.zeichne(mo, P[h])
        x0, y0, x1, y1 = box
        c = cv2.resize(img[y0:y1, x0:x1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
        c = cv2.resize(c, (300, 300)); t.append(c)
    for a in ('seite', 'vorn'):
        im, _ = profil(mo, a); t.append(im)
    row = np.hstack(t)
    cv2.putText(row, nm[:80], (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 0), 1)
    rows.append(row)
cv2.imwrite(sys.argv[1], cv2.cvtColor(np.vstack(rows), cv2.COLOR_RGB2BGR))
