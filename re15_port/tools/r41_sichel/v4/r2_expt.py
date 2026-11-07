"""SF V4 Runde 2 (Bauer): Versuchsbau mit geaenderten SCHWANZ_V4 / KOPF_V4-Werten, Render Pose 85/135, Schwanzmasse (riegel_v4).

    python r2_expt.py '<json schwanz-ueberschreibung>' [name]
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import riegel_v4 as RV

AUS = os.path.join(G.SFV4, 'r2', 'expt')


def bauen(sw=None, kp=None, farben=None):
    BF.V4_FARBEN = farben or os.path.join(G.SFV4, 'nicht_da.npz')
    if sw:
        BF.SCHWANZ_V4.update(sw)
    if kp:
        BF.KOPF_V4.update(kp)
    BF.BAU_INFO.clear()
    return BF.baue(BF.lade_em30(), BF._variante_v4())


def schwanz_messen(blob, name=None):
    mo = b2.modell(blob)
    P = G.posen()
    Z, s, (img, ids, V, fr, rm) = RV.schwanz(mo, P)
    if name:
        os.makedirs(AUS, exist_ok=True)
        im = img.copy()
        for (x, y) in ((357, 243), (290, 308), (295, 343)):
            cv2.circle(im, (x, y), 3, (255, 255, 0), 1)
        for k in ('beginn', 'biegung', 'knick', 'ende'):
            x, y = s[k]; cv2.circle(im, (int(x), int(y)), 2, (0, 255, 255), -1)
        farb = np.array([[255, 0, 0], [255, 128, 0], [255, 255, 0], [0, 255, 0], [0, 255, 255], [0, 0, 255], [255, 0, 255]])
        idb = np.where((ids[..., None] >= 0), 90, 30).astype(np.uint8).repeat(3, 2)
        idb[np.isin(ids, (7, 8))] = (150, 150, 150)
        for j in range(7):
            idb[ids == 16 + j] = farb[j]
        for (x, y) in ((357, 243), (290, 308), (295, 343)):
            cv2.circle(idb, (x, y), 3, (255, 255, 255), 1)
        both = np.hstack([im[150:420, 220:520], idb[150:420, 220:520]])
        cv2.imwrite(os.path.join(AUS, name + '_85.png'), cv2.cvtColor(both, cv2.COLOR_RGB2BGR).repeat(2, 0).repeat(2, 1))
        img2, ids2 = G.zeichne(mo, P['135'])
        cv2.imwrite(os.path.join(AUS, name + '_135.png'), cv2.cvtColor(img2, cv2.COLOR_RGB2BGR))
    return Z, s


if __name__ == '__main__':
    sw = json.loads(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1] else None
    nm = sys.argv[2] if len(sys.argv) > 2 else None
    blob = bauen(sw)
    Z, s = schwanz_messen(blob, nm)
    for z in Z:
        print('%-4s %-22s %s' % ('OK' if z['ok'] else 'ROT', z['mass'], z['ist']))
    print('H', s['H'], 'beginn', s['beginn'], 'scheitel', s['scheitel'], 'sohle', s['sohle'])
    print('T5 alle', {k: round(v, 3) for k, v in s['T5'].items()})
