from r4lib import *
import birkin_buehne as BB
f = json.loads(sys.argv[2]); v = f.pop('versatz', None)
blob, prot = bauen(f, versatz=v)
mo = b2.modell(blob)
Z = 7
tiles = []
for h, box in (('135', (305, 140, 395, 230)), ('85', (425, 170, 495, 250))):
    img, ids = G.zeichne(mo, P[h])
    pose = P[h]; kf = BB.kf_von_clip(mo, pose['clip'], pose['bild']); R, Tt = mo.pose(kf)
    Vv = np.array([q[:3] for q in mo.meshes[9].tv], float)
    W = (R[9].astype(float) / 4096.0 @ Vv.T).T + Tt[9].astype(float)
    xy = b2.projiziere(pose, W)
    x0, y0, x1, y1 = box
    t = cv2.resize(img[y0:y1, x0:x1], None, fx=Z, fy=Z, interpolation=cv2.INTER_NEAREST)
    for i, (x, y) in enumerate(xy):
        px, py = int((x - x0) * Z), int((y - y0) * Z)
        cv2.circle(t, (px, py), 3, (0, 255, 0), -1)
        cv2.putText(t, str(i), (px + 4, py - 2), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 0), 1)
    tiles.append(t)
hh = max(t.shape[0] for t in tiles)
tiles = [np.vstack([t, np.zeros((hh - t.shape[0], t.shape[1], 3), np.uint8)]) for t in tiles]
cv2.imwrite(sys.argv[1], cv2.cvtColor(np.hstack(tiles), cv2.COLOR_RGB2BGR))
