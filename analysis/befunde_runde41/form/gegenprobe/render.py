#!/usr/bin/env python3
"""Unabhaengiger Maler: Kegelstuempfe aus gerippe.json als Vierecke, von hinten nach vorn, ohne rig.py und ohne Ellipsoid-Kette.
Liefert Knochenkarte je Pixel, Restfehler gegen die Nutzer-Masken und schattierte Ansichten von allen Seiten.
python render.py [gerippe.json] --aus ordner"""
import argparse, glob
import numpy as np, cv2
from fk import *

TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}


def dicke(k, s):
    d = k['dicke']
    w = np.array([2 * (s - .5) * (s - 1), -4 * s * (s - 1), 2 * s * (s - .5)])
    return w @ np.array([q['a'] for q in d], float), w @ np.array([q['b'] for q in d], float)


def netz(k, Ri, Ti, ns=9, nw=20, kappe=4):
    """Ringe[nr,nw,3] entlang des Knochens, Enden als Halbellipsoide."""
    a0 = np.array(k['achse_von'], float)
    a1 = np.array(k['achse_bis'], float)
    L = np.linalg.norm(a1 - a0)
    e = (a1 - a0) / L
    ea = np.array([0, 1, 0.]) if k['querachse_a'] == 'Y' else np.array([1, 0, 0.])
    eb = np.cross(e, ea)
    eb = eb / max(1e-6, np.linalg.norm(eb))
    ph = np.linspace(0, 2 * np.pi, nw, endpoint=False)
    ringe = []

    def ring(t, fa):
        s = np.clip(t / L, 0, 1)
        a, b = dicke(k, s)
        return a0 + e * t + np.outer(np.cos(ph), ea) * a * fa + np.outer(np.sin(ph), eb) * b * fa
    a_, b_ = dicke(k, 0.0)
    c0 = 0.75 * min(a_, b_)
    for q in np.linspace(1, 0, kappe, endpoint=False):
        ringe.append(ring(-c0 * q, max(0.03, math.sqrt(1 - q * q))))
    for t in np.linspace(0, L, ns):
        ringe.append(ring(t, 1.0))
    a_, b_ = dicke(k, 1.0)
    c1 = 0.75 * min(a_, b_)
    for q in np.linspace(0, 1, kappe + 1)[1:]:
        ringe.append(ring(L + c1 * q, max(0.03, math.sqrt(max(0, 1 - q * q)))))
    r = np.array(ringe)
    return r @ Ri.T + Ti


def quads(g, R, T):
    Q, K = [], []
    for k in g['knochen']:
        i = k['index']
        r = netz(k, R[i], T[i])
        n, w = r.shape[:2]
        for a in range(n - 1):
            for b in range(w):
                c = (b + 1) % w
                Q.append([r[a, b], r[a, c], r[a + 1, c], r[a + 1, b]])
                K.append(i)
    return np.array(Q), np.array(K)


def malen(Qb, Tiefe, K, W, H, normal=None):
    """Qb[n,4,2] Bildkoordinaten, Tiefe[n], Maler von hinten nach vorn. kn[H,W], licht[H,W]."""
    kn = np.full((H, W), -1, np.int16)
    lic = np.zeros((H, W), np.float32)
    ord_ = np.argsort(-Tiefe)
    for j in ord_:
        q = np.round(Qb[j] * 8).astype(np.int32)
        if not np.isfinite(Qb[j]).all():
            continue
        cv2.fillConvexPoly(kn, q, int(K[j]), lineType=cv2.LINE_8, shift=3)
        if normal is not None:
            cv2.fillConvexPoly(lic, q, float(normal[j]), lineType=cv2.LINE_8, shift=3)
    return kn, lic


def kamerabild(g, nr):
    kam = FK.lade_kamera(nr)
    R, T = fk(g, 'bild%d' % nr)
    Q, K = quads(g, R, T)
    n, m = Q.shape[0], Q.shape[1]
    b = FK.projiziere(kam, Q.reshape(-1, 3)).reshape(n, m, 2)
    t = FK.tiefe(kam, Q.reshape(-1, 3)).reshape(n, m).mean(1)
    return malen(b, t, K, 640, 480)[0]


def ansicht(g, pose, rechts, unten, W=520, H=360, sk=0.052, boden=True):
    """Parallelblick. rechts, unten = Einheitsvektoren in Figurraum (Gier heraus). Schattierung aus Normale."""
    R, T = fk(g, pose)
    p = g['posen'][pose]
    Y = ry(p['gier'] * E)
    la = np.array(p['lage'], float)
    Rf = Y.T @ R
    Tf = (T - la) @ Y          # Y^T (T - la)
    Q, K = quads(g, Rf, Tf)
    ex, ey = np.array(rechts, float), np.array(unten, float)
    ez = np.cross(ex, ey)
    mitte = np.array([0.5 * (Tf[:, 0].min() + Tf[:, 0].max()), 0, 0.5 * (Tf[:, 2].min() + Tf[:, 2].max())])
    u = (Q - mitte) @ ex * sk + W / 2
    v = (Q - mitte) @ ey * sk + (H - 30 if boden else H / 2)
    d = (Q - mitte) @ ez
    n = np.cross(Q[:, 1] - Q[:, 0], Q[:, 3] - Q[:, 0])
    n /= np.maximum(1e-6, np.linalg.norm(n, axis=1, keepdims=True))
    li = -0.2 * (n @ ez) + 0.5 * np.abs(n @ ez) + 0.25 + 0.25 * np.clip(-(n @ np.array([0, 1, 0.])), 0, 1)
    b = np.stack([u, v], -1)
    return malen(b, d.mean(1), K, W, H, li)


def farbe(n):
    if n.startswith('schwanz'):
        return (80, 220, 80)
    if 'arm' in n or n.startswith(('hand', 'klaue')):
        return (255, 110, 40) if '_r' in n else (60, 190, 255)
    if 'schenkel' in n or n.startswith('fuss'):
        return (200, 200, 0) if n.endswith('_r') else (0, 130, 240)
    if n in ('kopf', 'kiefer', 'hals'):
        return (0, 235, 235)
    return (165, 160, 225)


def tafel(g, pose, titel, rechts, unten, boden=True):
    kn, li = ansicht(g, pose, rechts, unten, boden=boden)
    f = np.array([farbe(k['name']) for k in g['knochen']], np.float32)
    b = np.full(kn.shape + (3,), 26, np.uint8)
    an = kn >= 0
    b[an] = np.clip(f[kn[an]] * np.clip(li[an], 0.15, 1.1)[:, None], 0, 255).astype(np.uint8)
    if boden:
        cv2.line(b, (0, kn.shape[0] - 30), (kn.shape[1], kn.shape[0] - 30), (120, 120, 120), 1)
        x = kn.shape[1] - 26
        cv2.line(b, (x, kn.shape[0] - 30), (x, int(kn.shape[0] - 30 - 3005 * 0.052)), (255, 255, 255), 1)
    cv2.putText(b, titel, (6, 16), 0, .45, (255, 255, 255), 1, cv2.LINE_AA)
    return b


def rundblick(g, pfad):
    zeilen = []
    for pose, nm in (('bild1', 'Pose 1'), ('bild2', 'Pose 2')):
        z = [tafel(g, pose, nm + ' von rechts', (1, 0, 0), (0, 1, 0)),
             tafel(g, pose, nm + ' von links', (-1, 0, 0), (0, 1, 0)),
             tafel(g, pose, nm + ' von vorn', (0, 0, 1), (0, 1, 0))]
        zeilen.append(np.hstack(z))
        z = [tafel(g, pose, nm + ' von hinten', (0, 0, -1), (0, 1, 0)),
             tafel(g, pose, nm + ' von oben (vorn rechts)', (1, 0, 0), (0, 0, -1), boden=False),
             tafel(g, pose, nm + ' von unten (vorn rechts)', (1, 0, 0), (0, 0, 1), boden=False)]
        zeilen.append(np.hstack(z))
    cv2.imwrite(pfad, np.vstack(zeilen))


def masken():
    m = {}
    for p in glob.glob(os.path.join(F, 'nutzer_teile', 'maske_*.png')):
        x = cv2.imread(p, cv2.IMREAD_UNCHANGED)
        m[os.path.basename(p)[6:-4]] = (x[..., -1] if x.ndim == 3 else x) > 127
    return m


def rest(g, nr, kn, m):
    z = g['zuordnung']['bild%d' % nr]
    ix = {k['name']: k['index'] for k in g['knochen']}
    aus = {}
    for t in TEILE[nr]:
        ids = [ix[b] for b in z[t]]
        s = np.isin(kn, ids)
        u = m[t]
        aus[t] = dict(iou=round(float((s & u).sum() / max(1, (s | u).sum())), 3), decke=round(float((s & u).sum() / max(1, u.sum())), 3),
                      alle=round(float(((kn >= 0) & u).sum() / max(1, u.sum())), 3), ueber=int((s & ~u).sum()), fl=int(u.sum()))
    v = np.zeros_like(kn, bool)
    for t in TEILE[nr]:
        v |= m[t]
    aus['_aussen_px'] = int(((kn >= 0) & ~cv2.dilate(v.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)).sum())
    return aus


def bildblatt(nr, kn, g, m, pfad):
    v = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
    a = (np.clip(v, 0, 1) ** 0.6 * 255).astype(np.uint8)
    f = np.array([farbe(k['name']) for k in g['knochen']], np.uint8)
    b = a.copy()
    an = kn >= 0
    b[an] = (0.35 * b[an] + 0.65 * f[kn[an]]).astype(np.uint8)
    for t in TEILE[nr]:
        c, _ = cv2.findContours(m[t].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(a, c, -1, (0, 255, 255), 1)
        cv2.drawContours(b, c, -1, (255, 255, 255), 1)
    c, _ = cv2.findContours(an.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, c, -1, (255, 255, 255), 1)
    cv2.imwrite(pfad, cv2.resize(np.hstack([a[130:480, 130:490], b[130:480, 130:490]]), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('json', nargs='?')
    a.add_argument('--aus', default='.')
    a = a.parse_args()
    g = lade(a.json)
    m = masken()
    os.makedirs(a.aus, exist_ok=True)
    for nr in (1, 2):
        kn = kamerabild(g, nr)
        r = rest(g, nr, kn, m)
        for k, v in r.items():
            print(nr, k, v)
        bildblatt(nr, kn, g, m, os.path.join(a.aus, 'maler_b%d.png' % nr))
    rundblick(g, os.path.join(a.aus, 'rundblick.png'))
