#!/usr/bin/env python3
"""Tonmodell zum Beurteilen der Form: flach schattiert, Maler von hinten nach vorn, drei Posen, sechs Seiten. Dazu Umriss ueber beiden Bildern.
python ton.py [--aus DATEI] [--bilder] """
import argparse, json, math, os, sys
import numpy as np, cv2
from kn import knochen, lade_posen, fk, ry, E, FORM, HIER
import entwurf
import netz as NZ
import form_kamera as FK

FARBE = {'rumpf': (150, 150, 165), 'kopf': (120, 170, 190), 'arm': (120, 120, 200), 'bein': (120, 190, 130), 'schwanz': (170, 130, 110), 'klaue': (210, 215, 220)}


def farbe(nm):
    if nm.startswith(('klaue', 'zeh', 'zahn', 'horn')):
        return FARBE['klaue']
    if nm.startswith('schwanz'):
        return FARBE['schwanz']
    if nm.startswith(('oberarm', 'unterarm', 'hand')):
        return FARBE['arm']
    if nm.startswith(('oberschenkel', 'unterschenkel', 'fuss')):
        return FARBE['bein']
    if nm.startswith(('kopf', 'kiefer', 'hals')):
        return FARBE['kopf']
    return FARBE['rumpf']


def weltquads(N, R, T):
    """N: Liste Netze (punkte lokal, flaechen, knochen). -> Dreiecke[n,3,3], Farbindex"""
    Q, F = [], []
    for ni, m in enumerate(N):
        p = m['V'] @ R[m['knochen']].T + T[m['knochen']]
        for f in m['F']:
            if len(f) == 3:
                Q.append(p[[f[0], f[1], f[2], f[2]]])
            else:
                Q.append(p[list(f)])
            F.append(ni)
    return np.array(Q), np.array(F)


def tafel(Q, F, N, ex, ey, ez, W, H, sk, mitte, titel, boden=None):
    p = Q - mitte
    u = p @ ex * sk + W / 2
    v = p @ ey * sk + H / 2
    d = p @ ez
    n = np.cross(Q[:, 2] - Q[:, 0], Q[:, 3] - Q[:, 1])
    n /= np.maximum(1e-6, np.linalg.norm(n, axis=1, keepdims=True))
    ansicht = -ez
    # Aussen zeigt die Normale weg von der Mitte des Netzes: Vorzeichen je Dreieck aus der Windung (orientiert in netz.py)
    L1 = ex * -0.45 + ey * -0.65 + ez * -0.6
    L1 /= np.linalg.norm(L1)
    li = np.clip(n @ L1, 0, 1) * 0.85 + 0.2 + 0.1 * np.clip(n @ (-ey), 0, 1)
    sicht = (n @ ansicht) > 0
    SS = 2
    b = np.full((H * SS, W * SS, 3), 30, np.uint8)
    if boden is not None:
        yb = int((boden - mitte[1]) * sk * SS + H * SS / 2) if False else None
    ord_ = np.argsort(-d.mean(1))
    for j in ord_:
        if not sicht[j]:
            continue
        q = np.stack([u[j], v[j]], -1)
        c = np.array(farbe(N[F[j]]['name']), float) * li[j]
        cv2.fillConvexPoly(b, np.round(q * SS * 16).astype(np.int32), tuple(float(x) for x in np.clip(c, 0, 255)), lineType=cv2.LINE_AA, shift=4)
    b = cv2.resize(b, (W, H), interpolation=cv2.INTER_AREA)
    cv2.putText(b, titel, (6, 16), 0, .45, (255, 255, 255), 1, cv2.LINE_AA)
    return b


def blatt(N, posen, ks, pfad, W=520, H=400):
    zeilen = []
    Seiten = (('rechts', (1, 0, 0), (0, 1, 0), (0, 0, 1)), ('links', (-1, 0, 0), (0, 1, 0), (0, 0, -1)), ('vorn', (0, 0, 1), (0, 1, 0), (-1, 0, 0)), ('hinten', (0, 0, -1), (0, 1, 0), (1, 0, 0)),
              ('3/4 vorn', (0.7, 0, 0.7), (0, 1, 0), (-0.7, 0, 0.7)), ('oben', (1, 0, 0), (0, 0, 1), (0, 1, 0)))
    for pn, p in posen:
        R, T, _Rw, _Tw = fk(ks, p)
        Q, F = weltquads(N, R, T)
        mitte = 0.5 * (Q.reshape(-1, 3).min(0) + Q.reshape(-1, 3).max(0))
        sp = float((Q.reshape(-1, 3).max(0) - Q.reshape(-1, 3).min(0)).max())
        sk = (W - 30) / sp
        z = []
        for nm, ex, ey, ez in Seiten:
            z.append(tafel(Q, F, N, np.array(ex, float), np.array(ey, float), np.array(ez, float), W, H, sk, mitte, pn + ' von ' + nm))
        zeilen.append(np.hstack(z[:3]))
        zeilen.append(np.hstack(z[3:]))
    cv2.imwrite(pfad, np.vstack(zeilen))


def umriss(N, ks, posen, pfad):
    """Tonmodell ueber dem Bild, Kamera des Bildes, Nutzer-Umrisse."""
    import glob
    aus = []
    for nr in (1, 2):
        p = posen['bild%d' % nr]
        _R, _T, Rw, Tw = fk(ks, p)
        kam = FK.lade_kamera(nr)
        Q, F = weltquads(N, Rw, Tw)
        b = FK.projiziere(kam, Q.reshape(-1, 3)).reshape(-1, 4, 2)
        z = FK.tiefe(kam, Q.reshape(-1, 3)).reshape(-1, 4).mean(1)
        n = np.cross(Q[:, 2] - Q[:, 0], Q[:, 3] - Q[:, 1])
        n /= np.maximum(1e-6, np.linalg.norm(n, axis=1, keepdims=True))
        auge = np.array(kam['auge'], float)
        v = auge - Q.mean(1)
        v /= np.linalg.norm(v, axis=1, keepdims=True)
        li = np.clip((n * v).sum(1), 0, 1)
        im = cv2.imread(os.path.join(FORM, 'bild%d.png' % nr)).astype(np.float32) / 255
        orig = (np.clip(im, 0, 1) ** 0.6 * 255).astype(np.uint8)
        r = orig.copy()
        for j in np.argsort(-z):
            if li[j] <= 0 or not np.isfinite(b[j]).all():
                continue
            c = np.array(farbe(N[F[j]]['name']), float) * (0.25 + 0.75 * li[j])
            cv2.fillConvexPoly(r, np.round(b[j] * 16).astype(np.int32), tuple(float(x) for x in c), lineType=cv2.LINE_AA, shift=4)
        for mp in glob.glob(os.path.join(FORM, 'nutzer_teile', 'maske_*.png')):
            import json as J
            nm = os.path.basename(mp)[6:-4]
            if J.load(open(os.path.join(FORM, 'nutzer_teile', 'lage.json')))[nm]['bild'] != nr:
                continue
            m = cv2.imread(mp, cv2.IMREAD_UNCHANGED)
            m = (m[..., -1] if m.ndim == 3 else m) > 127
            c, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(r, c, -1, (0, 255, 255), 1)
            cv2.drawContours(orig, c, -1, (0, 255, 255), 1)
        kas = (130, 480, 130, 490)
        aus.append(np.hstack([orig[kas[0]:kas[1], kas[2]:kas[3]], r[kas[0]:kas[1], kas[2]:kas[3]]]))
    cv2.imwrite(pfad, cv2.resize(np.vstack(aus), None, fx=1.5, fy=1.5, interpolation=cv2.INTER_NEAREST))


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=os.path.join(HIER, 'zw', 'ton.png'))
    a.add_argument('--umriss', default=os.path.join(HIER, 'zw', 'ton_umriss.png'))
    a = a.parse_args()
    ks = knochen()
    N = NZ.netze(entwurf.alle())
    P = lade_posen()
    rast = dict(wurzel=(0, -1900, 0), winkel=[[0, 0, 0]] * len(ks))
    if 'ruhe' in P:
        rast = P['ruhe']
    blatt(N, (('Pose 1', P['bild1']), ('Pose 2', P['bild2']), ('Ruhe', rast)), ks, a.aus)
    umriss(N, ks, P, a.umriss)
    print('Flaechen', sum(len(m['F']) for m in N))
