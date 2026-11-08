"""SF V4 Runde 2: Augenpunkt auf der Kopfoberflaeche per Sehstrahl der Vorderkamera (Pose 135) an der K4-Stelle, dazu S2 und Sichtbarkeit."""
import sys, json, numpy as np
import r2_expt as X, b2, v4_gemein as G, kopf3d_v4 as K, augenpunkt_v4 as AP


def suche(kp, kxs=(0.045, 0.05, 0.055), kys=(0.095, 0.10, 0.105, 0.11)):
    blob = X.bauen(kp=kp); mo = b2.modell(blob); P = G.posen()
    img, ids, iv = G.zeichne(mo, P['135'], voll=True); ok, so, Hv = G.h_port(iv); kv = G.kopfbox(ids)
    img2, ids2, iv2 = G.zeichne(mo, P['85'], voll=True); ok2, so2, Hs = G.h_port(iv2); ks = G.kopfbox(ids2)
    xm = 0.5 * (kv[0] + kv[2])
    out = []
    for kx in kxs:
        for ky in kys:
            t = K.treffer(mo, P['135'], (xm - kx * Hv, kv[1] + ky * Hv))
            if t is None:
                continue
            pk = t[1]
            xs, ws = AP.proj(mo, P['85'], pk[None])
            ts = K.treffer(mo, P['85'], tuple(xs[0]))
            sicht = ts is not None and np.linalg.norm(ts[0] - ws[0]) < 25
            out.append((kx, ky, pk, (ks[2] - xs[0][0]) / Hs, (so2 - xs[0][1]) / Hs, sicht))
    return out, dict(K10=((kv[2] - kv[0] + 1) / Hv, (kv[3] - kv[1] + 1) / Hv), seite=((ks[2] - ks[0] + 1) / Hs, (ks[3] - ks[1] + 1) / Hs))


if __name__ == '__main__':
    kp = json.loads(sys.argv[1])
    out, info = suche(kp)
    print(info)
    for kx, ky, pk, s2x, s2y, sicht in out:
        print('K4 %.3f/%.3f -> Kopfraum %s  S2 %.3f / %.3f  sichtbar seitlich %s' % (kx, ky, np.round(pk, 0), s2x, s2y, sicht))
