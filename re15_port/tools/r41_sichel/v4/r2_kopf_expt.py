"""SF V4 Runde 2 (Bauer): Kopfform-Versuch (KOPF_V4 skal / vorbau) - Seitenansicht Pose 85: Vorderkante, Spaltenhoehen an der Kante,
Kopfbox; Vorderansicht Pose 135: K10; gemeinsamer Augenpunkt (augenpunkt_v4.suche) fuer K4 + S2.

    python r2_kopf_expt.py '<json KOPF_V4-Ueberschreibung>' [name]
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import r2_expt as X
import augenpunkt_v4 as AP


def kopf_form(blob, name=None):
    mo = b2.modell(blob)
    P = G.posen()
    out = {}
    for k in ('135', '85'):
        img, ids, iv = G.zeichne(mo, P[k], voll=True)
        ok, so, H = G.h_port(iv)
        kb = G.kopfbox(ids)
        m = ids == 9
        out[k] = dict(H=H, kb=kb, bw=(kb[2] - kb[0] + 1) / H, bh=(kb[3] - kb[1] + 1) / H)
        if k == '85':
            x1 = kb[2]
            sp = [int(m[:, x].sum()) for x in range(x1 - 14, x1 + 1)]
            out[k]['spalten_vorn'] = sp            # Spaltenhoehen der letzten 15 px (Vorderkante)
            out[k]['kante_hoehe'] = max(sp[-4:]) / H
        if name:
            os.makedirs(X.AUS, exist_ok=True)
            x0, y0, x1, y1 = kb
            c = img[max(0, y0 - 20):y1 + 20, max(0, x0 - 20):x1 + 20]
            cv2.imwrite(os.path.join(X.AUS, '%s_kopf%s.png' % (name, k)), cv2.cvtColor(c, cv2.COLOR_RGB2BGR).repeat(6, 0).repeat(6, 1))
    return out


if __name__ == '__main__':
    kp = json.loads(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1] else None
    nm = sys.argv[2] if len(sys.argv) > 2 else None
    farben = sys.argv[3] if len(sys.argv) > 3 else None
    blob = X.bauen(kp=kp, farben=farben)
    pfad = os.path.join(X.AUS, 'kopf_probe.EMD'); os.makedirs(X.AUS, exist_ok=True); open(pfad, 'wb').write(blob)
    f = kopf_form(blob, nm)
    print('vorn K10 %.3f x %.3f H' % (f['135']['bw'], f['135']['bh']))
    print('seite Kopfbox %.3f x %.3f H; Vorderkante Spaltenhoehen %s; Kante %.3f H' % (f['85']['bw'], f['85']['bh'], f['85']['spalten_vorn'], f['85']['kante_hoehe']))
    import birkin_formen as BF
    if BF.KOPF_V4.get('auge') is not None:
        mo = b2.modell(blob); P = G.posen()
        pa = np.array(BF.KOPF_V4['auge'], float)[None]
        xv, wv = AP.proj(mo, P['135'], pa); xs, ws = AP.proj(mo, P['85'], pa)
        kv, ks = f['135']['kb'], f['85']['kb']
        img, ids, iv = G.zeichne(mo, P['85'], voll=True); ok, so, Hs = G.h_port(iv)
        print('Auge KOPF_V4[auge]: K4 %.3f / %.3f  S2 %.3f / %.3f' % ((0.5 * (kv[0] + kv[2]) - xv[0][0]) / f['135']['H'],
              (xv[0][1] - kv[1]) / f['135']['H'], (ks[2] - xs[0][0]) / Hs, (so - xs[0][1]) / Hs))
    for sc, p, k4, s2, sicht in AP.suche(pfad)[:3]:
        print('Augenpunkt Kopfraum (%.0f, %.0f, %.0f)  K4 %.3f / %.3f  S2 %.3f / %.3f  Abstand %.2f  sichtbar %s' % (*p, *k4, *s2, sc, sicht))
