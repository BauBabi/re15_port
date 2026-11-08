"""SF V4 Runde 2: Kopf nah (x6) Original | Port | Port mit Fleckmaske, vorn und seitlich -> sfv4/r2/kopf_<ansicht>_r2.png"""
import sys, os
import numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2, riegel_v4 as RV


def bild(emd, aus):
    mo = b2.modell(emd); P = G.posen()
    for nm, fn, b_ in (('vorn', RV.kopf_vorn, 1), ('seite', RV.kopf_seite, 2)):
        Z, info, (img, ids, V, b) = fn(mo, P)
        x0, y0, x1, y1 = info['kopfbox']
        o = G.vorlage(b_).astype(np.uint8); ko = G.KB_ORIG[b_]
        c1 = cv2.resize(o[ko[1] - 15:ko[3] + 15, ko[0] - 25:ko[2] + 15], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)
        im2 = img.copy(); im2[b['maske']] = (0, 255, 0)
        c2 = cv2.resize(img[y0 - 15:y1 + 15, x0 - 25:x1 + 15], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)
        c3 = cv2.resize(im2[y0 - 15:y1 + 15, x0 - 25:x1 + 15], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)
        h = max(c1.shape[0], c2.shape[0])
        pad = lambda c: np.vstack([c, np.zeros((h - c.shape[0], c.shape[1], 3), np.uint8)])
        cv2.imwrite(os.path.join(aus, 'kopf_%s_r2.png' % nm), cv2.cvtColor(np.hstack([pad(c1), pad(c2), pad(c3)]), cv2.COLOR_RGB2BGR))
        print(nm, {k: v for k, v in info['blob'].items() if k in ('wh', 'sp', 'n', 'bbox')})


if __name__ == '__main__':
    bild(sys.argv[1], sys.argv[2])
