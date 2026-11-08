"""Krallen vorn (Bild 2): Spitzen je Klauen-Mesh (projiziert, sichtbar?) gegen Spitzen der Nutzer-Umrisse right/left_arm_front.
Spitze im Umriss = Konturpunkt, dessen Winkel zwischen den Nachbarn im Abstand 6 px < 75 Grad ist und der eine lokale Abstandsspitze zum
Handschwerpunkt bildet (Nicht-Maxima-Unterdrueckung 10 px)."""
import json, sys, math
import numpy as np, cv2
import v4f

NT = v4f.FORM + '/nutzer_teile/maske_%s.png'
HAND = {'r (W)': ('right_arm_front', 18, (19, 20, 29, 31)), 'l (W)': ('left_arm_front', 13, (14, 15, 28, 30))}


def umriss_spitzen(m, mitte, schritt=6, winkel=75.0, nms=10):
    cs, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    c = max(cs, key=len)[:, 0, :].astype(float)
    n = len(c)
    kand = []
    for i in range(n):
        a, b, p = c[(i - schritt) % n], c[(i + schritt) % n], c[i]
        v1, v2 = a - p, b - p
        w = math.degrees(math.acos(np.clip(v1 @ v2 / (np.linalg.norm(v1) * np.linalg.norm(v2) + 1e-9), -1, 1)))
        d = np.linalg.norm(p - mitte)
        da, db = np.linalg.norm(a - mitte), np.linalg.norm(b - mitte)
        if w < winkel and d > da and d > db:
            kand.append((w, tuple(int(x) for x in p)))
    kand.sort()
    aus = []
    for w, p in kand:
        if all(math.hypot(p[0] - q[0], p[1] - q[1]) > nms for _w, q in aus):
            aus.append((round(w, 1), p))
    return aus


def modell_spitzen(ordner):
    k = v4f.kand(ordner)
    lage, gier, clip, bild = k.stand(2)
    kam = v4f.P.kamera(2)
    tris = v4f.dreiecke_id(k.mo, k.kf(clip, bild), lage, gier, kam, None)
    pts = {}
    for _t, p, _uv, _z, _f, tag in tris:
        x, y = v4f.FB.abbilden(np.array([q[0] + v4f.FB.PHASE for q in p], float), np.array([q[1] + v4f.FB.PHASE for q in p], float), kam.abbildung)
        pts.setdefault(tag[0], []).extend(zip(x, y))
    return {b: np.array(v) for b, v in pts.items()}


def main(cache, aus_json):
    import pickle
    res = {}
    for nm in ('K4', 'Asset K5c'):
        K = pickle.load(open(cache + '/cache_%s.pkl' % nm.replace(' ', '_'), 'rb'))[2]['K']
        pts = modell_spitzen(v4f.ST[nm])
        for hand, (maske, hm, klauen) in HAND.items():
            um = cv2.imread(NT % maske, 0) > 0
            hp = pts.get(hm)
            mitte = hp.mean(0) if hp is not None else np.argwhere(um)[:, ::-1].mean(0)
            r = {'handmitte': [round(float(x), 1) for x in mitte], 'klauen': {}}
            for kb in klauen:
                if kb not in pts:
                    r['klauen'][kb] = None
                    continue
                q = pts[kb]
                i = int(np.argmax(np.linalg.norm(q - mitte, axis=1)))
                tx, ty = q[i]
                xi, yi = int(tx), int(ty)
                sicht = int((K['mesh'] == kb).sum())
                im_umriss = bool(0 <= xi < 640 and 0 <= yi < 480 and um[min(yi, 479), min(xi, 639)])
                # sichtbare Laenge: groesste Ausdehnung der sichtbaren Pixel des Meshes
                ys, xs = np.nonzero(K['mesh'] == kb)
                laenge = float(np.hypot(xs.max() - xs.min(), ys.max() - ys.min())) if sicht else 0.0
                r['klauen'][kb] = dict(spitze=[round(float(tx), 1), round(float(ty), 1)], sichtbar_px=sicht, spitze_im_nutzerumriss=im_umriss,
                                       sichtbare_bbox_diagonale_px=round(laenge, 1))
            res['%s|%s' % (nm, hand)] = r
    for hand, (maske, hm, klauen) in HAND.items():
        um = cv2.imread(NT % maske, 0) > 0
        mitte = np.array(res['Asset K5c|%s' % hand]['handmitte'])
        res['Nutzerumriss|%s' % hand] = dict(maske=maske, spitzen=umriss_spitzen(um, mitte))
    json.dump(res, open(aus_json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    for k_, v in res.items():
        print(k_, json.dumps(v, ensure_ascii=False))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
