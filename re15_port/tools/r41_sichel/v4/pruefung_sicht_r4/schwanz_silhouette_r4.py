"""Pruefung V4 r4 Sicht: Schwanz-Silhouette V4 (Ids 16-22, Pose 85) gegen die Original-Schwanzmaske (Leser A,
sfv4/schwanz_maske*.npy auf Vorlage 2 n1434/1435): Ueberdeckung, Zeilenbreiten, Oberkonturen, Kantenwinkel, Overlay-Bild."""
import sys, os, json, numpy as np, cv2
from PIL import Image
SF = sys.argv[1]; R2 = SF + '/sicht_r4'; OUT = sys.argv[2]; os.makedirs(OUT, exist_ok=True)
out = {}
M = {k: np.load(SF + '/schwanz_maske%s.npy' % k) for k in ('', '_hi', '_lo')}
for name in ('V4', 'V4r3', 'REF2'):
    ids = np.load(R2 + '/%s_85_ids.npy' % name)[:480]; sw = np.isin(ids, range(16, 23))
    d = {'px': int(sw.sum())}
    for k, m in M.items():
        i = (sw & m).sum(); u = (sw | m).sum(); d['IoU' + k] = round(float(i / u), 3); d['orig_gedeckt' + k] = round(float(i / m.sum()), 3); d['port_in_orig' + k] = round(float(i / max(sw.sum(), 1)), 3)
    # Oberkontur je Spalte (oberster Schwanzpixel) x 280..360 fuer Port und Original
    ok = {}
    for x in range(276, 362, 4):
        a = np.nonzero(sw[:, x])[0]; b = np.nonzero(M[''][:, x])[0]
        ok[x] = (int(a.min()) if len(a) else None, int(b.min()) if len(b) else None)
    d['oberkontur_port_orig'] = ok
    # Zeilenbreiten
    zb = {}
    for y in range(200, 350, 10):
        a = np.nonzero(sw[y, :380])[0]; b = np.nonzero(M[''][y, :380])[0]
        zb[y] = ((int(a.min()), int(a.max()), len(a)) if len(a) else None, (int(b.min()), int(b.max()), len(b)) if len(b) else None)
    d['zeilen_port_orig'] = zb
    out[name] = d
# Kruemmung der Aussenkontur: Polygon-Approximation (Douglas-Peucker eps 2 px), Eckwinkel
def ecken(m):
    cs, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); c = max(cs, key=cv2.contourArea)
    p = cv2.approxPolyDP(c, 2.0, True)[:, 0, :]; n = len(p); res = []
    for i in range(n):
        a, b, cc = p[i-1], p[i], p[(i+1) % n]; v1 = a - b; v2 = cc - b
        ang = np.degrees(np.arccos(np.clip(np.dot(v1, v2) / (np.linalg.norm(v1)*np.linalg.norm(v2) + 1e-9), -1, 1)))
        res.append((int(b[0]), int(b[1]), int(round(ang))))
    return len(p), res
ids = np.load(R2 + '/V4_85_ids.npy')[:480]; sw4 = np.isin(ids, range(16, 23))
out['dp_ecken_V4'] = ecken(sw4); out['dp_ecken_orig'] = ecken(M[''])
idr = np.load(R2 + '/V4r3_85_ids.npy')[:480]; out['dp_ecken_V4r3'] = ecken(np.isin(idr, range(16, 23)))
# Overlay
o2 = np.asarray(Image.open(R2 + '/vorlage2_entbl.png').convert('RGB')).copy(); v4 = np.asarray(Image.open(R2 + '/V4_85.png').convert('RGB')).copy()
for im in (o2, v4):
    cs, _ = cv2.findContours(M[''].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(im, cs, -1, (255, 0, 255), 1)
    cs, _ = cv2.findContours(sw4.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(im, cs, -1, (0, 255, 255), 1)
def cr(im): return cv2.resize(im[180:360, 250:400], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
a, b = cr(o2), cr(v4); pad = np.full((a.shape[0], 8, 3), 40, np.uint8)
t = np.concatenate([a, pad, b], 1); cv2.putText(t, 'Vorlage 2 (magenta Orig-Maske, cyan V4 Ids 16-22) x4', (6, 18), 0, 0.5, (255, 255, 0), 1); cv2.putText(t, 'V4 r4 Pose 85', (a.shape[1]+14, 18), 0, 0.5, (255, 255, 0), 1)
Image.fromarray(t).save(OUT + '/PS4_schwanz_silhouette_x4.png')
json.dump(out, open(OUT + '/PS4_schwanz_silhouette.json', 'w'), indent=1, default=str)
for k, v in out.items(): print(k, v)
