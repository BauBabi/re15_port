"""Runde 3, Befund 11: Kopf 22 + Unterkiefer 23 (+ Hals 21) im Knochenraum des Kopfes, ohne Kamera, mit Tiefenpuffer (je Bildpunkt das
vorderste Dreieck). Farben: die 6 Hals-Streifen-Flaechen, die uebrigen 15 Zone-30-Flaechen, Zaehne (Zone 41), Haut. Ansichten von unten,
von der Seite, von vorn und schraeg von unten. Dazu Kennzahlen je Flaeche: Lage zu den Zaehnen (x-Bereich) und zur Kieferlinie.
Pose: Aufruf  python r3_kopfbild.py <clip> <bild> <name>  (Standard 27 20 griff)."""
import json
import sys
import cv2
from s_basis import *  # noqa: F401,F403
import r3_lage as RL

SECHS = set(RL.SECHS)
Z30 = set(RL.Z30)
ZAEHNE = set(RL.zone_liste('41'))
LIPPE = set(RL.zone_liste('42'))
FARBE = {'sechs': (220, 20, 30), 'z30': (255, 160, 0), 'zahn': (250, 250, 235), 'lippe': (190, 120, 200),
         'kopf': (120, 170, 120), 'kiefer': (90, 140, 230), 'hals': (175, 175, 160)}


def gruppe(f, b):
    if f in SECHS:
        return 'sechs'
    if f in Z30:
        return 'z30'
    if f in ZAEHNE:
        return 'zahn'
    if f in LIPPE:
        return 'lippe'
    return {22: 'kopf', 23: 'kiefer', 21: 'hals'}[b]


def kopfraum(mo, kf, teile=(21, 22, 23)):
    F, tris, teil, schl = RL.flaechen(mo, kf)
    R, T = mo.pose(kf)
    Rk = R[22].astype(float) / 4096.0; Tk = T[22].astype(float)
    sel = np.isin(teil, teile)
    tris = tris[sel]; teil = teil[sel]; schl = schl[sel]
    K = (tris - Tk) @ Rk          # Zeilenvektor: (X - T) R = R^T (X - T)
    return K, teil, schl


def ansicht(K, teil, schl, blick, oben, B=520, rand=40, ausschnitt=None):
    """Orthografisch: Blickrichtung 'blick' (vom Betrachter in die Szene), 'oben' = Bild-oben. Tiefenpuffer, einfache Schattierung.
    Rueckseiten werden wie im Spiel weggelassen: gezeichnet nur, wenn die Aussen-Normale (-Kreuzprodukt) zum Betrachter zeigt."""
    w = np.asarray(blick, float); w /= np.linalg.norm(w)
    u = np.cross(np.asarray(oben, float), w); u /= np.linalg.norm(u)   # Bild-rechts
    v = np.cross(w, u)                                                # Bild-oben
    P = K.reshape(-1, 3)
    x = P @ u; y = P @ v; z = P @ w
    if ausschnitt is None:
        ausschnitt = (x.min(), x.max(), y.min(), y.max())
    x0, x1, y0, y1 = ausschnitt
    s = (B - 2 * rand) / max(x1 - x0, y1 - y0)
    X = ((x - x0) * s + rand).reshape(-1, 3); Y = ((y1 - y) * s + rand).reshape(-1, 3); Z = z.reshape(-1, 3)
    img = np.full((B, B, 3), 255, np.uint8); zb = np.full((B, B), np.inf); idb = np.full((B, B), -1, np.int32)
    nrm0 = -np.cross(K[:, 1] - K[:, 0], K[:, 2] - K[:, 0])
    vorder = (nrm0 @ w) < 0
    for i in range(len(X)):
        if not vorder[i]:
            continue
        xa, ya, za = X[i], Y[i], Z[i]
        fl = (xa[1] - xa[0]) * (ya[2] - ya[0]) - (xa[2] - xa[0]) * (ya[1] - ya[0])
        if abs(fl) < 1e-9:
            continue
        bx0 = max(int(np.floor(xa.min())), 0); bx1 = min(int(np.ceil(xa.max())) + 1, B)
        by0 = max(int(np.floor(ya.min())), 0); by1 = min(int(np.ceil(ya.max())) + 1, B)
        if bx0 >= bx1 or by0 >= by1:
            continue
        PX, PY = np.meshgrid(np.arange(bx0, bx1) + 0.5, np.arange(by0, by1) + 0.5)
        w1 = ((PX - xa[0]) * (ya[2] - ya[0]) - (xa[2] - xa[0]) * (PY - ya[0])) / fl
        w2 = ((xa[1] - xa[0]) * (PY - ya[0]) - (PX - xa[0]) * (ya[1] - ya[0])) / fl
        w0 = 1 - w1 - w2
        inn = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        zz = w0 * za[0] + w1 * za[1] + w2 * za[2]
        sl = (slice(by0, by1), slice(bx0, bx1))
        m = inn & (zz < zb[sl])
        zb[sl][m] = zz[m]; idb[sl][m] = i
    nrm = -np.cross(K[:, 1] - K[:, 0], K[:, 2] - K[:, 0])
    nrm /= np.maximum(np.linalg.norm(nrm, axis=1, keepdims=True), 1e-9)
    hell = np.clip(0.55 + 0.45 * np.abs(nrm @ w), 0, 1)
    farb = np.array([FARBE[gruppe(schl[i], teil[i])] for i in range(len(K))], float)
    ok = idb >= 0
    img[ok] = (farb[idb[ok]] * hell[idb[ok], None]).astype(np.uint8)
    # Kanten der Dreiecke duenn nachziehen (Flaechengrenzen = Wechsel der Flaechenkennung)
    fk = np.full(idb.shape, -1, np.int64)
    namen = {s_: j for j, s_ in enumerate(sorted(set(schl.tolist())))}
    fk[ok] = np.array([namen[schl[i]] for i in idb[ok]])
    kante = np.zeros(idb.shape, bool)
    kante[:, 1:] |= fk[:, 1:] != fk[:, :-1]; kante[1:, :] |= fk[1:, :] != fk[:-1, :]
    img[kante & ok] = (img[kante & ok] * 0.55).astype(np.uint8)
    return img, idb, (X, Y), ausschnitt


ANSICHTEN = {
    'von unten': ((0, -1, 0), (1, 0, 0)),          # Blick nach oben (y unten positiv -> Betrachter unter dem Kopf), Schnauze nach oben im Bild
    'von links (Wesen)': ((0, 0, -1), (0, -1, 0)),
    'von vorn (Schnauze)': ((-1, 0, 0), (0, -1, 0)),
    'schraeg von unten-hinten': ((0.6, -0.8, 0.0), (0.8, 0.6, 0.0)),
}

if __name__ == '__main__':
    clip, bild, name = (int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]) if len(sys.argv) > 3 else (27, 20, 'griff')
    k = kand(ASSET); mo = k.mo
    kf = k.kf(clip, bild)
    out = {}
    for teile, tn in (((22, 23), 'kopf_kiefer'), ((21, 22, 23), 'mit_hals')):
        K, teil, schl = kopfraum(mo, kf, teile)
        for an, (bl, ob) in ANSICHTEN.items():
            img, idb, _xy, _a = ansicht(K, teil, schl, bl, ob)
            fn = AUS + '/r3_kopf_%s_%s_%s.png' % (name, tn, an.replace(' ', '_').replace('(', '').replace(')', ''))
            cv2.imwrite(fn, img[..., ::-1])
            sicht = {}
            for i in np.unique(idb[idb >= 0]):
                g = gruppe(schl[i], teil[i])
                if g in ('sechs', 'z30'):
                    sicht[str(schl[i])] = sicht.get(str(schl[i]), 0) + int((idb == i).sum())
            out['%s | %s' % (tn, an)] = sicht
    # Lage zu den Zaehnen im Knochenraum des Kopfes (Pose-unabhaengig fuer Teil 22; Kiefer in dieser Pose in Kopfraum gerechnet)
    K, teil, schl = kopfraum(mo, kf, (22, 23))
    M = K.mean(1)
    def bereich(menge):
        s = np.array([x in menge for x in schl])
        return [[int(M[s, a].min()), int(M[s, a].max())] for a in range(3)]
    lage = {'zaehne_xyz_bereich': bereich(ZAEHNE), 'lippe_xyz_bereich': bereich(LIPPE),
            'kopf_xyz_bereich': bereich(set(x for x in schl if x.startswith('22|'))),
            'kiefer_xyz_bereich': bereich(set(x for x in schl if x.startswith('23|')))}
    for f in sorted(Z30):
        s = schl == f
        lage[f] = [int(round(c)) for c in M[s].mean(0)]
    out['lage_kopfraum'] = lage
    json.dump(out, open(ZIEL + '/messung/r3_kopf_%s.json' % name, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps(out, ensure_ascii=False, indent=1))
