#!/usr/bin/env python3
"""Passung: Radien je Ecke (glattes Korrekturfeld q ueber dem Entwurf) und Posenwinkel gegen die Nutzer-Masken. Weicher Rasterer in torch, Tiefenfolge fest je Bild.
Das Feld q ist glatt gehalten: Muskeln und Grate bleiben aus dem Entwurf, nur Lage und Masse wandern zum Bild.
python fit.py [--iter N] [--neu] [--ohne-pose] [--aus DIR]"""
import argparse, json, math, os, time
import numpy as np, cv2, torch
from kn import knochen, lade_posen, posen_roh, HIER, FORM, TEILE, BILD_TEILE, INDEX
import entwurf
import netz as NZ
import form_kamera as FK

DEV = 'cuda' if torch.cuda.is_available() else 'cpu'
H, W = 480, 640
ST = 2                      # Pixelschritt
SIGMA = 1.2
DELTA = 0.5
E = 2 * math.pi / 4096


def lade_maske(pfad):
    m = cv2.imread(pfad, cv2.IMREAD_UNCHANGED)
    return (m[..., -1] if m.ndim == 3 else m)


def masken():
    aus = {}
    for t in TEILE:
        aus[t] = lade_maske(os.path.join(FORM, 'nutzer_teile', 'maske_%s.png' % t)) > 127
    # Keil zwischen linkem Bein und Schwanz: im Bild dunkel, in keinem Nutzer-Teil (pruefung.md O3). Nur fuer die Passung dem linken Bein zugeschlagen.
    k = np.zeros((H, W), np.uint8)
    cv2.fillPoly(k, [np.array([(222, 352), (250, 345), (250, 440), (232, 445), (222, 420)], np.int32)], 1)
    aus['left_leg'] = aus['left_leg'] | (k > 0)
    # Rechts neben dem Arm: gemessene Kante (429,272), Schraegstrich Z9 (411,240)-(420,262), pruefung.md O1. Nur fuer die Passung dem Arm zugeschlagen.
    k = np.zeros((H, W), np.uint8)
    cv2.fillPoly(k, [np.array([(404, 250), (413, 244), (431, 272), (420, 296), (406, 296)], np.int32)], 1)
    aus['right_arm'] = aus['right_arm'] | (k > 0)
    return aus


def t3(x, **k):
    return torch.tensor(x, dtype=torch.float32, device=DEV, **k)


def rot(a):
    """a[n,3] Winkel in Bogenmass, R = Rx Ry Rz."""
    sx, cx = torch.sin(a[:, 0]), torch.cos(a[:, 0])
    sy, cy = torch.sin(a[:, 1]), torch.cos(a[:, 1])
    sz, cz = torch.sin(a[:, 2]), torch.cos(a[:, 2])
    r = torch.zeros(a.shape[0], 3, 3, device=DEV)
    r[:, 0, 0] = cy * cz
    r[:, 0, 1] = -cy * sz
    r[:, 0, 2] = sy
    r[:, 1, 0] = sx * sy * cz + cx * sz
    r[:, 1, 1] = cx * cz - sx * sy * sz
    r[:, 1, 2] = -sx * cy
    r[:, 2, 0] = sx * sz - cx * sy * cz
    r[:, 2, 1] = sx * cz + cx * sy * sz
    r[:, 2, 2] = cx * cy
    return r


class Netz:
    def __init__(s, m, q0=None):
        t = m['teil']
        s.name, s.knochen = t.name, t.knochen
        sp = torch.tensor([1, 1, -1.0 if t.spiegel else 1.0], device=DEV, dtype=torch.float32)
        s.sp = sp
        s.M = t3(t.M) * sp
        s.D = t3(t.D) * sp
        s.R = t3(t.R)
        s.nr, s.n = t.R.shape
        s.q = torch.zeros((s.nr, s.n), device=DEV, dtype=torch.float32, requires_grad=True)
        if q0 is not None and np.shape(q0) == (s.nr, s.n):
            s.q.data[:] = t3(q0)
        tp = []
        ta, te = t.tip()
        if t.anf == 'spitz':
            tp.append(ta)
        if t.ende == 'spitz':
            tp.append(te)
        s.tp = (t3(np.array(tp)) * sp) if tp else None
        tri = []
        for f, a in zip(m['F'], m['art']):
            if a[0] == 'k':
                continue
            for i in range(1, len(f) - 1):
                tri.append((f[0], f[i], f[i + 1]))
        s.tri = torch.tensor(tri, device=DEV, dtype=torch.long)

    def V(s):
        p = s.M[:, None, :] + s.D * (s.R * torch.exp(s.q))[..., None]
        p = p.reshape(-1, 3)
        if s.tp is not None:
            p = torch.cat([p, s.tp], 0)
        return p


class Bild:
    def __init__(s, nr, mk):
        s.nr = nr
        k = FK.lade_kamera(nr)
        s.P = t3(k['P'])
        s.Rz = t3(k['R'][2])
        s.auge = t3(np.array(k['auge'], float))
        ys, xs = np.mgrid[0:H:ST, 0:W:ST]
        s.gx, s.gy = t3(xs.astype(np.float32)), t3(ys.astype(np.float32))
        s.h, s.w = s.gx.shape
        logo = lade_maske(os.path.join(FORM, 'pruefung', 'logo_maske.png')) > 127
        ziel = lade_maske(os.path.join(FORM, 'ziel2_bild%d.png' % nr))
        s.teile = BILD_TEILE[nr]
        alle = np.zeros((H, W), np.int32)
        for t in s.teile:
            alle += mk[t].astype(np.int32)
        s.M = {}
        s.Wt = {}
        for t in s.teile:
            s.M[t] = t3(mk[t][::ST, ::ST].astype(np.float32))
            w = ((~logo) & ~((alle >= 2) & mk[t])).astype(np.float32)
            # Randband wiegt mehr: gegen die Flaeche grosser Teile faellt eine Kante sonst nicht ins Gewicht
            m8 = mk[t].astype(np.uint8)
            kern = np.ones((3, 3), np.uint8)
            band = cv2.dilate(m8, kern, iterations=6) - cv2.erode(m8, kern, iterations=6)
            w = w * (1.0 + 3.0 * band)
            if t == 'right_arm_front':
                w = w * (~mk['face_and_upper_body']).astype(np.float32)
            s.Wt[t] = t3(w[::ST, ::ST])
        s.frei = t3((ziel == 0)[::ST, ::ST].astype(np.float32))
        s.weiss = t3(((ziel == 255) & ~logo)[::ST, ::ST].astype(np.float32))

    def proj(s, X):
        h = X @ s.P[:, :3].T + s.P[:, 3]
        return h[:, :2] / h[:, 2:3].clamp(min=1.0), h[:, 2]


def deckung(b2, tri, B):
    """Bone: Punkte b2[m,2], Dreiecke tri[K,3]. -> (y0,x0, Kachel) Deckung in Gitterkoordinaten."""
    p = b2[tri]                                         # K,3,2
    ar = (p[:, 1, 0] - p[:, 0, 0]) * (p[:, 2, 1] - p[:, 0, 1]) - (p[:, 1, 1] - p[:, 0, 1]) * (p[:, 2, 0] - p[:, 0, 0])
    sgn = torch.sign(ar.detach()) + (ar.detach() == 0).float()
    lo = p.detach().reshape(-1, 2).min(0).values - 5
    hi = p.detach().reshape(-1, 2).max(0).values + 5
    x0 = max(int(lo[0].item() // ST), 0)
    y0 = max(int(lo[1].item() // ST), 0)
    x1 = min(int(hi[0].item() // ST) + 1, B.w)
    y1 = min(int(hi[1].item() // ST) + 1, B.h)
    if x1 <= x0 or y1 <= y0:
        return None
    gx = B.gx[y0:y1, x0:x1].reshape(-1)
    gy = B.gy[y0:y1, x0:x1].reshape(-1)
    d = None
    for i in range(3):
        a, c = p[:, i], p[:, (i + 1) % 3]
        e = c - a
        L = torch.sqrt((e * e).sum(1) + 1e-6)
        nx, ny = -e[:, 1] / L * sgn, e[:, 0] / L * sgn
        dd = (gx[None] - a[:, 0:1]) * nx[:, None] + (gy[None] - a[:, 1:2]) * ny[:, None]
        d = dd if d is None else torch.minimum(d, dd)
    c = torch.sigmoid((d + DELTA) / SIGMA)
    lg = torch.log1p(-c.clamp(max=1 - 1e-4)).sum(0)
    cov = 1 - torch.exp(lg)
    return y0, x0, cov.reshape(y1 - y0, x1 - x0)


def fk(ks, wurzel, ang, gier, lage):
    n = len(ks)
    Rm = rot(ang)
    R, T = [None] * n, [None] * n
    for i, k in enumerate(ks):
        e = k['eltern']
        if e < 0:
            R[i], T[i] = Rm[i], wurzel
        else:
            R[i] = R[e] @ Rm[i]
            T[i] = R[e] @ k['_v'] + T[e]
    Y = rot(torch.stack([torch.zeros(1, device=DEV)[0], gier, torch.zeros(1, device=DEV)[0]])[None])[0]
    Rw = torch.stack([Y @ r for r in R])
    Tw = torch.stack([Y @ t + lage for t in T])
    return Rw, Tw


# Anker: Punkt im Knochenraum -> Bildpunkt. Finger: Spitzen des Nutzer-Umrisses. Kopf: Blick zur Kamera, Mitte und Rand auf Zug Q (FORMBLATT 2 Abschnitt 6).
SP = (720.0, 170.0, 0.0)            # Krallenspitze lokal (entwurf.arm), Faktoren je Finger wie dort
SP2 = (720.0 * 1.05, 170.0, 0.0)
SP3 = (720.0 * 1.25, 170.0, 0.0)
SD = (720.0 * 0.86, 170.0, 0.0)
ANKER = {1: [('klaue_r3', SP3, (424, 364), 0.3), ('klaue_r2', SP2, (412, 372), 0.3), ('klaue_r1', SP, (398, 373), 0.3), ('klaue_r4', SD, (388, 370), 0.5),
             ('klaue_l1', SP, (330, 364), 0.5), ('klaue_l2', SP2, (322, 362), 0.5), ('klaue_l3', SP3, (314, 364), 0.5), ('klaue_l4', SD, (327, 372), 0.5),
             ('unterarm_l', (0, 0, 0), (318, 300), 0.5), ('hand_l', (0, 0, 0), (322, 386), 0.8), ('hand_l', (689, 0, 0), (322, 374), 0.8), ('kiefer', (800, 0, 0), (350, 282), 0.5),
             ('unterarm_r', (0, 0, 0), (398, 294), 0.8), ('hand_r', (0, 0, 0), (398, 357), 0.8)],
         2: [('klaue_r3', SP3, (146, 352), 1.0), ('klaue_r2', SP2, (197, 472), 1.0), ('klaue_r1', SP, (236, 473), 1.0), ('klaue_r4', SD, (272, 441), 1.0),
             ('klaue_l1', SP, (460, 392), 1.0), ('klaue_l2', SP2, (468, 417), 1.0), ('klaue_l3', SP3, (438, 417), 1.0), ('klaue_l4', SD, (420, 402), 0.3),
             ('hand_r', (689, 0, 0), (200, 416), 1.0),
             ('unterschenkel_r', (0, 0, 0), (230, 318), 0.5), ('fuss_r', (0, 0, 0), (250, 358), 0.5),
             ('kopf', (760, 0, 0), (325, 245), 2.0), ('kopf', (1400, 0, 0), (322, 254), 2.0), ('kopf', (760, -610, 0), (325, 183), 2.0)]}


# Tiefe: (Knochen, Punkt lokal, Tiefe im Bild n, Gewicht). Hand hinter dem Rumpf, nicht davor.
TIEFE = {1: [], 2: []}


def toleranz(name, n=0):
    """Spielraum der Pose gegen das Gerippe, Grad. Armkette und Rumpf bleiben, wie die Gegenprobe sie festgelegt hat."""
    if n == 1 and name in ('oberarm_l', 'unterarm_l', 'hand_l'):
        return 45.0                 # in Bild 1 verdeckt, frei
    if n == 2 and name.startswith('hand'):
        return 75.0                 # Hand am Boden, Finger gespreizt: Spitzen des Umrisses geben die Lage vor
    if n == 2 and name in ('oberschenkel_r', 'unterschenkel_r', 'fuss_r'):
        return 25.0                 # Lage der Beine in Bild 2 ist frei (FORMBLATT2 Abschnitt 4), das rechte Bein fuellt die untere Spitze
    if name.startswith('klaue'):
        if n == 2:
            return 90.0
        return 40.0 if name.endswith('4') else 22.0
    if name.startswith('hand'):
        return 30.0
    if name.startswith('fuss'):
        return 14.0
    if name in ('kopf', 'kiefer', 'hals'):
        return 25.0
    if name.startswith(('unterschenkel', 'oberschenkel')):
        return 4.0
    if name.startswith(('unterarm', 'oberarm')):
        return 5.0
    if name.startswith('schwanz'):
        return 3.0
    return 2.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--iter', type=int, default=500)
    ap.add_argument('--neu', action='store_true', help='q und Posen auf Anfang')
    ap.add_argument('--ohne-pose', action='store_true')
    ap.add_argument('--lq', type=float, default=0.3)
    ap.add_argument('--ls', type=float, default=1.0)
    ap.add_argument('--lp', type=float, default=0.02)
    ap.add_argument('--aus', default=HIER)
    a = ap.parse_args()
    ks = knochen()
    for k in ks:
        k['_v'] = t3(np.array(k['versatz'], float))
    teile = entwurf.alle()
    N = NZ.netze(teile)
    korr = {}
    p_korr = os.path.join(HIER, 'korr.json')
    if os.path.exists(p_korr) and not a.neu:
        korr = json.load(open(p_korr, encoding='utf-8'))
    posen = posen_roh() if a.neu else lade_posen()
    netze = [Netz(m, korr.get(m['name'])) for m in N]
    je_knochen = {}
    for nz in netze:
        je_knochen.setdefault(nz.knochen, []).append(nz)
    mk = masken()
    bilder = {n: Bild(n, mk) for n in (1, 2)}
    ix = {k['name']: i for i, k in enumerate(ks)}
    para = [nz.q for nz in netze]
    pp = {}
    for n in (1, 2):
        p = posen['bild%d' % n]
        roh = posen_roh()['bild%d' % n]
        w0 = roh['wurzel']
        d = dict(ang0=t3(np.array(p['winkel'], float)) * E, ang_roh=t3(np.array(roh['winkel'], float)) * E, d=torch.zeros(len(ks), 3, device=DEV, requires_grad=True), w0=t3(np.array(w0, float)),
                 wurzel=torch.tensor(np.array(p['wurzel'], float), device=DEV, dtype=torch.float32, requires_grad=True),
                 gier=t3(p['gier'] * E), lage=t3(np.array(p['lage'], float)))
        pp[n] = d
    tol = {n: t3(np.array([toleranz(k['name'], n) for k in ks])) * math.pi / 180 for n in (1, 2)}
    gw = {x.name: (12.0 if x.name.startswith(('kopf', 'kiefer', 'hals')) else (4.0 if x.name.startswith(('zeh', 'hauer', 'zahn')) else 1.0)) for x in netze}
    opt_a = [dict(params=para, lr=0.012)]
    if not a.ohne_pose:
        opt_a.append(dict(params=[pp[n]['d'] for n in (1, 2)], lr=0.008))
        opt_a.append(dict(params=[pp[n]['wurzel'] for n in (1, 2)], lr=3.0))
    opt = torch.optim.Adam(opt_a)
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(opt, a.iter, eta_min=0.0)
    ordnung = {}
    t0 = time.time()
    for it in range(a.iter + 1):
        opt.zero_grad()
        L_bild = 0.0
        L_rand = 0.0
        L_boden = 0.0
        L_anker = 0.0
        protokoll = {}
        Vl = {b: torch.cat([x.V() for x in nz]) for b, nz in je_knochen.items()}
        ofs = {}
        for b, nz in je_knochen.items():
            o = 0
            li = []
            for x in nz:
                li.append(x.tri + o)
                o += x.nr * x.n + (0 if x.tp is None else x.tp.shape[0])
            ofs[b] = torch.cat(li)
        for n in (1, 2):
            B = bilder[n]
            d = pp[n]
            Rw, Tw = fk(ks, d['wurzel'], d['ang0'] + d['d'], d['gier'], d['lage'])
            pro = {}
            for b in Vl:
                X = Vl[b] @ Rw[b].T + Tw[b]
                L_boden = L_boden + (torch.relu(X[:, 1] + 4) ** 2).sum() / 400.0
                q, z = B.proj(X)
                pro[b] = (q, z, X)
            # Tiefster Punkt auf dem Boden: Der Bau setzt ihn dorthin, sonst wandert die ganze Figur nachtraeglich
            ymax = torch.stack([pro[b][2][:, 1].max() for b in pro]).max()
            L_boden = L_boden + (ymax / 12.0) ** 2
            for (nm, lok, tf, gew) in TIEFE[n]:
                b = ix[nm]
                X = (t3(np.array(lok, float)) @ Rw[b].T + Tw[b])[None]
                _q, z = B.proj(X)
                L_anker = L_anker + gew * ((z[0] - tf) / 200.0) ** 2
            for (nm, lok, ziel, gew) in ANKER[n]:
                b = ix[nm]
                X = (t3(np.array(lok, float)) @ Rw[b].T + Tw[b])[None]
                q, _z = B.proj(X)
                dq = q[0] - t3(np.array(ziel, float))
                L_anker = L_anker + gew * (dq * dq).sum() / 400.0
            if n not in ordnung or it % 40 == 0:
                # Tiefenfolge: mittlere Tiefe je Knochen, vorn zuerst. Rumpf in Bild 2 vor den Armen (Bildlesung: Arme kommen unter der Rumpfkante hervor)
                tief = {b: float(pro[b][1].mean().detach()) for b in pro}
                ordnung[n] = sorted(tief, key=lambda b: tief[b])
            rest = torch.ones(B.h, B.w, device=DEV)
            sicht = {}
            for b in ordnung[n]:
                c = deckung(pro[b][0], ofs[b], B)
                if c is None:
                    continue
                y0, x0, cov = c
                v = torch.zeros(B.h, B.w, device=DEV)
                f = torch.zeros(B.h, B.w, device=DEV)
                f[y0:y0 + cov.shape[0], x0:x0 + cov.shape[1]] = cov
                v = f * rest
                rest = rest * (1 - f)
                sicht[b] = v
            F = 1 - rest
            for t in B.teile:
                M, Wg = B.M[t], B.Wt[t]
                Vs = sum(sicht[ix[b]] for b in TEILE[t] if ix[b] in sicht)
                if t == 'face_and_upper_body' and ix['unterarm_r'] in sicht:
                    Vs = Vs + sicht[ix['unterarm_r']] * B.M[t]
                ue = (Vs * M * Wg).sum()
                dice = 1 - 2 * ue / ((Vs * Wg).sum() + (M * Wg).sum() + 1e-3)
                L_bild = L_bild + dice * {'right_arm_front': 1.8, 'left_arm_front': 1.3}.get(t, 1.0)
                protokoll[t] = float(ue.detach() / ((Vs + M - Vs * M) * Wg).sum().detach())
            L_rand = L_rand + (F * B.frei).sum() / 2000.0 + ((1 - F) * B.weiss).sum() / 8000.0
        L_q = sum(gw[x.name] * (3.0 if x.name.startswith(('unterarm', 'oberschenkel', 'unterschenkel')) else 1.0) * (x.q ** 2).mean() for x in netze) / len(netze)
        L_s = 0.0
        for x in netze:
            di = x.q[1:] - x.q[:-1]
            dj = x.q - torch.roll(x.q, 1, 1)
            L_s = L_s + gw[x.name] * (((di ** 2).mean() if x.nr > 1 else 0.0) + (dj ** 2).mean())
            if x.nr > 2:
                L_s = L_s + (3.0 if x.name in ('huefte', 'wirbel', 'brust') else 1.0) * gw[x.name] * ((di[1:] - di[:-1]) ** 2).mean()
        L_s = L_s / len(netze)
        # Spielraum gilt gegen das Gerippe, nicht gegen den letzten Lauf: sonst wandert die Pose von Lauf zu Lauf
        def wrap(x):
            return torch.atan2(torch.sin(x), torch.cos(x))
        L_p = sum(((wrap(pp[n]['ang0'] + pp[n]['d'] - pp[n]['ang_roh']) / tol[n][:, None]) ** 2).sum() for n in (1, 2)) / 30.0
        # Wurzel bleibt beim Gerippe: Dort ist sie gegen beide Kanten gerechnet. Sinkt sie, gewinnt der Fuss und die Oberkante verliert.
        L_w = sum(((pp[n]['wurzel'][1] - pp[n]['w0'][1]) / 15.0) ** 2 + (((pp[n]['wurzel'] - pp[n]['w0']) / 400.0) ** 2).sum() for n in (1, 2)) * 0.05
        L = L_bild + L_rand + L_boden * 0.2 + a.lq * L_q * 30 + a.ls * L_s * 30 + a.lp * L_p * 30 + L_anker + L_w
        L.backward()
        opt.step()
        sched.step()
        if it % 50 == 0 or it == a.iter:
            print('%4d L %.3f Bild %.3f Rand %.3f Boden %.3f Anker %.3f Lq %.4f Ls %.4f Lp %.3f  %.0fs  %s' % (it, float(L), float(L_bild), float(L_rand), float(L_boden), float(L_anker), float(L_q), float(L_s), float(L_p), time.time() - t0,
                                                                                                ' '.join('%s %.2f' % (k[:6], v) for k, v in protokoll.items())), flush=True)
    aus = {x.name: x.q.detach().cpu().numpy().round(4).tolist() for x in netze}
    json.dump(aus, open(os.path.join(a.aus, 'korr.json'), 'w', encoding='utf-8'))
    ja = {}
    for n in (1, 2):
        d = pp[n]
        w = ((d['ang0'] + d['d']) / E).detach().cpu().numpy()
        ja['bild%d' % n] = dict(lage=posen['bild%d' % n]['lage'], gier=posen['bild%d' % n]['gier'], wurzel=[float(x) for x in d['wurzel'].detach().cpu().numpy()], winkel=[[int(round(c)) for c in r] for r in w])
    json.dump(ja, open(os.path.join(a.aus, 'posen.json'), 'w', encoding='utf-8'))


if __name__ == '__main__':
    main()
