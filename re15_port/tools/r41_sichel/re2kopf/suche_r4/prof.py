from r4lib import *
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
var = json.loads(sys.argv[2])
blobs = [('r3', open(SP + 'EM30_V5r3.EMD', 'rb').read(), None)]
for f in var:
    v = f.pop('versatz', None); blob, prot = bauen(f, versatz=v); blobs.append((json.dumps(f)[:60], blob, v))
t = []
for nm, blob, v in blobs:
    mo = b2.modell(blob)
    p = json.loads(json.dumps(KB.PARAM_R3))
    if v is not None: p['versatz'] = list(v)
    row = []
    for a in ('seite', 'vorn'):
        im, al = profil(mo, a, scale=1.0, W=560, H=560, cx=290, cy=330, p=p)
        cv2.putText(im, nm + ' ' + a, (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 0), 1)
        row.append(im)
    t.append(np.hstack(row))
cv2.imwrite(sys.argv[1], cv2.cvtColor(np.vstack(t), cv2.COLOR_RGB2BGR))
