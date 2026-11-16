"""Framedump-Diff vorher -> nachher (F225-F400): jeder Bildpunkt, der sich aendert (max. Kanaldifferenz > 8), muss im Schwanz liegen -
Vereinigung der Knochen-Id-Masken 16-22 (b2.zeichne mit der gezeichneten Pose aus pose.log) von vorher und nachher, um 4 Videopunkte
erweitert, in den Framedump abgebildet (Videopunkt = 2 X/3 + 1,97, 2,079 Y/3 + 1,3). Ausgabe je Bild: geaenderte Punkte, davon ausserhalb.

    python fd_diff.py <vorher> <nachher> <ausgabe.json>
"""
import sys, os, json
import numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import messen as Ms
import b2
vl, nl, aus = sys.argv[1], sys.argv[2], sys.argv[3]
mo = b2.modell(Ms.EMD)
PV, PN = Ms.pose_log(os.path.join(vl, 'pose.log')), Ms.pose_log(os.path.join(nl, 'pose.log'))
H, W = 720, 960
Y, X = np.mgrid[0:H, 0:W]
vx = np.clip(np.round(2.0 * X / 3.0 + 1.97).astype(int), 0, 639)
vy = np.clip(np.round(2.079 * Y / 3.0 + 1.3).astype(int), 0, 479)
kern = np.ones((9, 9), np.uint8)


def schwanz(P, F):
    if F not in P:
        return np.zeros((480, 640), bool)
    p = P[F]
    pose = dict(clip=p['clip'], bild=p['bild'], gier=p['gier'], x=p['x'], y=p['y'], z=p['z'])
    _b, ids = b2.zeichne(mo, pose, R_T=(p['R'], p['T']), licht=False)
    return (ids >= 16) & (ids <= 22)


erg = {}
ges_anders = ges_aussen = 0
for F in range(225, 401):
    a = cv2.imread(os.path.join(vl, 'fd', 'f%06d.ppm' % F)); b = cv2.imread(os.path.join(nl, 'fd', 'f%06d.ppm' % F))
    if a is None or b is None:
        continue
    d = np.abs(a.astype(int) - b.astype(int)).max(2) > 8
    n = int(d.sum())
    if not n:
        erg[F] = (0, 0)
        continue
    m = cv2.dilate((schwanz(PV, F) | schwanz(PN, F)).astype(np.uint8), kern).astype(bool)
    aussen = d & ~m[vy, vx]
    k = int(aussen.sum())
    erg[F] = (n, k)
    ges_anders += n; ges_aussen += k
    if k:
        ys, xs = np.nonzero(aussen)
        print('F%d h%d: %d anders, %d ausserhalb (Framedump x %d-%d y %d-%d)' % (F, F - 243, n, k, xs.min(), xs.max(), ys.min(), ys.max()))
json.dump(erg, open(aus, 'w'))
print('Summe: %d Bilder verglichen, %d mit Aenderung, %d Punkte geaendert, davon %d ausserhalb des Schwanzes' % (
    len(erg), sum(1 for v in erg.values() if v[0]), ges_anders, ges_aussen))
