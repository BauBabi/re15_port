from r4lib import *
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
var = json.loads(sys.argv[2])
blobs = [('r3', open(SP + 'EM30_V5r3.EMD', 'rb').read())]
for f in var:
    v = f.pop('versatz', None); blob, prot = bauen(f, versatz=v); blobs.append((json.dumps(f)[:60], blob))
Z = 6; x0, y0, x1, y1 = 315, 150, 395, 230
fo = G.vorlage(1).astype(np.uint8)
t = [cv2.resize(fo[135:195, 360:420], (x1 - x0) * Z // 1 and None, fx=Z * 80 / 60, fy=Z * 80 / 60, interpolation=cv2.INTER_NEAREST)]
for nm, blob in blobs:
    mo = b2.modell(blob); img, ids = G.zeichne(mo, P['135'])
    c = cv2.resize(img[y0:y1, x0:x1], None, fx=Z, fy=Z, interpolation=cv2.INTER_NEAREST)
    cv2.putText(c, nm, (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 0), 1)
    t.append(c)
t = [x[:480, :480] for x in t]
rows = [np.hstack(t[:3]), np.hstack(t[3:6] + [np.zeros((480, 480, 3), np.uint8)] * (6 - len(t)))]
cv2.imwrite(sys.argv[1], cv2.cvtColor(np.vstack(rows), cv2.COLOR_RGB2BGR))
