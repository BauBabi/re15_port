# -*- coding: utf-8 -*-
"""Rohbau B: schnelle Kontrolle ohne form_pruefung. Vorwaertsrechnung wie der Port (Gleitkomma), Maler wie das Spiel, Teile gegen die Nutzer-Masken.
python schau.py            baut figur.py und schreibt lauf/s_bild1.png, s_bild2.png, s_rund.png, s_zahlen.txt"""
import glob, json, math, os, sys
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
WURZEL = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_kamera as FK  # noqa: E402

E = 2 * math.pi / 4096
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}
FARBE = {'tail': (120, 255, 125), 'left_leg': (40, 150, 255), 'right_leg': (255, 225, 40), 'right_arm': (255, 40, 255), 'part_of_Body': (185, 165, 255),
         'face_and_upper_body': (40, 235, 255), 'left_arm_front': (60, 210, 60), 'right_arm_front': (255, 130, 50)}


def rm(ax, ay, az):
    sx, cx = math.sin(ax), math.cos(ax)
    sy, cy = math.sin(ay), math.cos(ay)
    sz, cz = math.sin(az), math.cos(az)
    return np.array([[cz * cy, -sz * cy, sy],
                     [sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx],
                     [sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]])


def ry(g):
    s, c = math.sin(g), math.cos(g)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def fk(eltern, versatz, wurzel, winkel):
    n = len(eltern)
    R, T = [None] * n, [None] * n
    for i in range(n):
        L = rm(*(np.array(winkel[i], float) * E))
        p = eltern[i]
        if p < 0:
            R[i], T[i] = L, np.array(wurzel, float)
        else:
            R[i] = R[p] @ L
            T[i] = R[p] @ np.array(versatz[i], float) + T[p]
    return R, T


def maske_lesen(p):
    x = cv2.imread(p, cv2.IMREAD_UNCHANGED)
    return (x[..., -1] if x.ndim == 3 else x) > 127


MASKEN = {os.path.basename(p)[6:-4]: maske_lesen(p) for p in glob.glob(os.path.join(FORM, 'nutzer_teile', 'maske_*.png'))}
LOGO = maske_lesen(os.path.join(FORM, 'pruefung', 'logo_maske.png'))


def welt_netz(fig, pose):
    """Alle Flaechen in Welt: Liste (Punkte n x 3, Knochen, Normale Welt)."""
    R, T = fk(fig['eltern'], fig['versatz'], pose['wurzel'], pose['winkel'])
    Y = ry(pose['gier'] * E)
    la = np.array(pose['lage'], float)
    aus = []
    for b, V, F in fig['netze']:
        W = (V @ R[b].T + T[b]) @ Y.T + la
        aus.append((b, W, F))
    return aus


def maler(fig, pose, kam, H=480, B=640, feld='knochen'):
    """Knochenkarte je Pixel + Flaechen-Licht, von hinten nach vorn, Rueckseiten weg, wie das Spiel."""
    Q = []
    for b, W, F in welt_netz(fig, pose):
        p = FK.projiziere(kam, W)
        d = FK.tiefe(kam, W)
        for f in F:
            q = p[list(f)]
            if not np.isfinite(q).all():
                continue
            s = 0.0
            for i in range(len(f)):
                x0, y0 = q[i]
                x1, y1 = q[(i + 1) % len(f)]
                s += x0 * y1 - x1 * y0
            if s >= 0:                     # Flaeche von aussen im Uhrzeigersinn: Vorzeichen siehe Probe in bau.py
                continue
            Q.append((float(d[list(f)].mean()), b, q))
    Q.sort(key=lambda x: -x[0])
    kn = np.full((H, B), -1, np.int16)
    for _d, b, q in Q:
        cv2.fillConvexPoly(kn, np.round(q * 8).astype(np.int32), int(b), lineType=cv2.LINE_8, shift=3)
    return kn


def kennzahlen(fig, kn, n):
    z = fig['zuordnung'][n]
    aus = {}
    frei = ~LOGO
    ueberlapp = {}
    for t in TEILE[n]:
        s = np.isin(kn, z[t])
        u = MASKEN[t] & frei
        v = s & frei
        ober = {}
        aus[t] = dict(iou=float((v & u).sum() / max(1, (v | u).sum())), deck=float((v & u).sum() / max(1, u.sum())), alle=float(((kn >= 0) & u).sum() / max(1, u.sum())),
                      innen=float((v & u).sum() / max(1, v.sum())))
    return aus


def blatt(fig, kn, n, pfad):
    v = cv2.imread(os.path.join(FORM, 'bild%d.png' % n)).astype(np.float32) / 255
    a = (np.clip(v, 0, 1) ** 0.6 * 255).astype(np.uint8)
    b = a.copy()
    idf = np.zeros((64, 3), np.uint8)
    z = fig['zuordnung'][n]
    for t, ids in z.items():
        for i in ids:
            idf[i] = FARBE[t]
    an = kn >= 0
    grau = np.isin(kn, [i for ids in z.values() for i in ids])
    idf2 = idf.copy()
    b[an] = np.full(3, 120, np.uint8)
    b[grau] = (0.25 * b[grau] + 0.75 * idf[kn[grau]]).astype(np.uint8)
    # Knochenkanten
    kante = (kn != np.roll(kn, 1, 0)) | (kn != np.roll(kn, 1, 1))
    b[kante & an] = (b[kante & an] * 0.55).astype(np.uint8)
    for t in TEILE[n]:
        c, _ = cv2.findContours(MASKEN[t].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(a, c, -1, FARBE[t], 1)
        cv2.drawContours(b, c, -1, (255, 255, 255), 1)
    c, _ = cv2.findContours(an.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, c, -1, (255, 255, 255), 1)
    box = (120, 130, 490, 480)
    aa = np.hstack([a[box[1]:box[3], box[0]:box[2]], b[box[1]:box[3], box[0]:box[2]]])
    cv2.imwrite(pfad, cv2.resize(aa, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))


# ═════════════════════════════ Rundum, Lehm
def ansicht(fig, pose, az, el, W=420, H=340, sk=0.05, boden=True):
    """Parallelblick auf die Figur im Aktorraum (Gier heraus). az um die Hochachse, el Hoehe. Licht aus Blickrichtung oben links."""
    R, T = fk(fig['eltern'], fig['versatz'], pose['wurzel'], pose['winkel'])
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    Bl = np.stack([rechts, unten, vor])
    L = Bl.T @ _unit(np.array([-.5, -.65, -.6]))
    alle = []
    Q = []
    for b, V, F, Fn in fig['netze_n']:
        Wp = V @ R[b].T + T[b]
        Nn = Fn @ R[b].T
        q = Wp @ Bl.T
        alle.append(Wp)
        for j, f in enumerate(F):
            pts = q[list(f)]
            s = 0.0
            for i in range(len(f)):
                x0, y0 = pts[i, :2]
                x1, y1 = pts[(i + 1) % len(f), :2]
                s += x0 * y1 - x1 * y0
            if s >= 0:
                continue
            Q.append((float(pts[:, 2].mean()), pts[:, :2], float(Nn[j] @ L), b))
    Q.sort(key=lambda x: -x[0])
    alle = np.vstack(alle)
    m = 0.5 * (alle.max(0) + alle.min(0))
    mb = Bl @ m
    im = np.full((H, W, 3), 30, np.uint8)
    for _d, p, li, b in Q:
        c = 0.30 + 0.75 * max(li, 0.0)
        f = fig['lehm'][b] if 'lehm' in fig else (190, 190, 190)
        cc = tuple(int(min(255, x * c)) for x in f)
        pts = (p - mb[:2]) * sk + np.array([W / 2, H / 2])
        cv2.fillConvexPoly(im, np.round(pts * 8).astype(np.int32), cc, lineType=cv2.LINE_8, shift=3)
    return im


def _unit(v):
    return v / np.linalg.norm(v)


def rund(fig, pfad):
    zeilen = []
    for nm, pose in fig['posen'].items():
        z = []
        for t, az, el in (('rechts', 0, 0), ('3/4 vorn', 50, 14), ('vorn', 90, 0), ('hinten', -90, 0)):
            im = ansicht(fig, pose, az, el, W=480, H=340, sk=.062)
            cv2.putText(im, nm + ' ' + t, (6, 16), 0, .45, (255, 255, 255), 1, cv2.LINE_AA)
            z.append(im)
        zeilen.append(np.hstack(z))
    cv2.imwrite(pfad, np.vstack(zeilen))


def kamera_lehm(fig, pose, kam, H=480, B=640):
    """Figur im Kamerablick, flach schattiert (Licht von oben links vorn, nur zum Lesen der Form)."""
    L = kam['R'].T @ _unit(np.array([-.45, -.7, -.55]))
    Y = ry(pose['gier'] * E)
    Rk, Tk = fk(fig['eltern'], fig['versatz'], pose['wurzel'], pose['winkel'])
    Q = []
    la = np.array(pose['lage'], float)
    for b, V, F, Fn in fig['netze_n']:
        W = (V @ Rk[b].T + Tk[b]) @ Y.T + la
        Nn = (Fn @ Rk[b].T) @ Y.T
        p = FK.projiziere(kam, W)
        d = FK.tiefe(kam, W)
        for j, f in enumerate(F):
            q = p[list(f)]
            if not np.isfinite(q).all():
                continue
            s = 0.0
            for i in range(len(f)):
                x0, y0 = q[i]
                x1, y1 = q[(i + 1) % len(f)]
                s += x0 * y1 - x1 * y0
            if s >= 0:
                continue
            Q.append((float(d[list(f)].mean()), q, float(Nn[j] @ L), b))
    Q.sort(key=lambda x: -x[0])
    im = np.full((H, B, 3), 40, np.uint8)
    for _d, q, li, b in Q:
        c = 0.28 + 0.8 * max(li, 0.0)
        f = fig['lehm'][b] if 'lehm' in fig else (200, 200, 200)
        cc = tuple(int(min(255, x * c)) for x in f)
        cv2.fillConvexPoly(im, np.round(q * 8).astype(np.int32), cc, lineType=cv2.LINE_8, shift=3)
    return im


def blatt_lehm(fig, n, pose, kam, pfad):
    im = kamera_lehm(fig, pose, kam)
    v = cv2.imread(os.path.join(FORM, 'bild%d.png' % n)).astype(np.float32) / 255
    a = (np.clip(v, 0, 1) ** 0.6 * 255).astype(np.uint8)
    for t in TEILE[n]:
        c, _ = cv2.findContours(MASKEN[t].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(im, c, -1, FARBE[t], 1)
    box = (130, 130, 480, 480)
    aa = np.hstack([a[box[1]:box[3], box[0]:box[2]], im[box[1]:box[3], box[0]:box[2]]])
    cv2.imwrite(pfad, cv2.resize(aa, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))


def alles(fig, tag=''):
    os.makedirs(os.path.join(HIER, 'lauf'), exist_ok=True)
    zz = []
    for n in (1, 2):
        kam = FK.lade_kamera(n)
        pose = fig['posen']['bild%d' % n]
        kn = maler(fig, pose, kam)
        kz = kennzahlen(fig, kn, n)
        for t, w in kz.items():
            zz.append('%d %-20s iou %.2f deck %.2f alle %.2f innen %.2f' % (n, t, w['iou'], w['deck'], w['alle'], w['innen']))
        blatt(fig, kn, n, os.path.join(HIER, 'lauf', 's_bild%d%s.png' % (n, tag)))
        blatt_lehm(fig, n, pose, kam, os.path.join(HIER, 'lauf', 's_lehm%d%s.png' % (n, tag)))
    rund(fig, os.path.join(HIER, 'lauf', 's_rund%s.png' % tag))
    t = '\n'.join(zz)
    with open(os.path.join(HIER, 'lauf', 's_zahlen%s.txt' % tag), 'w') as f:
        f.write(t)
    print(t)
    return t
