#!/usr/bin/env python3
"""Kandidat A, Stufe 1: Rundkegel je Knochen an die Zielmasken beider Bilder.

    python passung.py ansehen  [--start datei.json]          Startwerte als Bild
    python passung.py lauf --schritte 1500 [--start datei.json] --aus datei.json

Weiche Maske in Bildebene: je Rundkegel 2D-Abstand im Bild, Radius aus der Tiefe. Das reicht, weil die Figur weit weg steht (Tiefe >= 4200, Teile unter 1000).
Pose 1 = alle Winkel 0, Pose 2 = Winkel je Knochen + Lage + Gier, beides frei. Kein Wert hier ist RE-Wert, die Figur ist neu.
"""
import argparse, json, math, os, sys
import numpy as np
import cv2
import torch

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
TOOLS = os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS); sys.path.insert(0, HIER)
import form_kamera as FK
import form_pruefung as PR
import entwurf as E

DEV = 'cuda' if torch.cuda.is_available() else 'cpu'
BONES = [k[0] for k in E.KNOCHEN]
IDX = {n: i for i, n in enumerate(BONES)}
PARENT = [k[1] for k in E.KNOCHEN]


def lade_ziel(n):
    return cv2.imread(os.path.join(FORM, 'ziel_bild%d.png' % n), 0)


def start_params(pfad=None):
    """Tensoren: gelenke (N,3), prim a/b (M,3), ra/rb (M), winkel2 (N,3), lage2 (3), gier2, lage1, gier1. y nach unten."""
    N = len(BONES)
    g = np.array([[k[2][0], -k[2][1], k[2][2]] for k in E.KNOCHEN], float)
    pb = np.array([IDX[p[0]] for p in E.R])
    a = np.array([[p[1][0], -p[1][1], p[1][2]] for p in E.R], float)
    b = np.array([[p[2][0], -p[2][1], p[2][2]] for p in E.R], float)
    ra = np.array([p[1][3] for p in E.R], float)
    rb = np.array([p[2][3] for p in E.R], float)
    w20 = np.zeros((N, 3))
    for n, v in E.W2_START.items():
        w20[IDX[n]] = v
    d = dict(g=g, pb=pb, a=a, b=b, ra=ra, rb=rb, w2=w20, lage1=np.array(E.LAGE[1]), gier1=np.array(E.GIER[1]), lage2=np.array(E.LAGE[2]), gier2=np.array(E.GIER[2]))
    if pfad:
        j = json.load(open(pfad, encoding='utf-8'))
        for k in d:
            if k in j:
                d[k] = np.array(j[k], float if k != 'pb' else int)
    return d


def achs_matrix(w):
    """Rodrigues, w (N,3) -> (N,3,3)."""
    th = torch.sqrt((w * w).sum(-1, keepdim=True) + 1e-12)
    k = w / th
    K = torch.zeros(w.shape[0], 3, 3, device=w.device, dtype=w.dtype)
    K[:, 0, 1], K[:, 0, 2] = -k[:, 2], k[:, 1]
    K[:, 1, 0], K[:, 1, 2] = k[:, 2], -k[:, 0]
    K[:, 2, 0], K[:, 2, 1] = -k[:, 1], k[:, 0]
    s, c = torch.sin(th)[..., None], torch.cos(th)[..., None]
    I = torch.eye(3, device=w.device, dtype=w.dtype).expand_as(K)
    return I + s * K + (1 - c) * (K @ K)


def yrot(g):
    a = g * 2 * math.pi / 4096
    c, s = torch.cos(a), torch.sin(a)
    z, o = torch.zeros_like(c), torch.ones_like(c)
    return torch.stack([torch.stack([c, z, s]), torch.stack([z, o, z]), torch.stack([-s, z, c])])


def fk(g, w):
    """Pose: R_b, t_b je Knochen, Modellraum. Punkt p (Pose-1-Raum) -> R_b (p - g_b) + t_b."""
    L = achs_matrix(w)
    N = g.shape[0]
    Rs, Ts = [None] * N, [None] * N
    for i in range(N):
        p = PARENT[i]
        if p < 0:
            Rs[i], Ts[i] = L[i], g[i]
        else:
            Rs[i] = Rs[p] @ L[i]
            Ts[i] = Rs[p] @ (g[i] - g[p]) + Ts[p]
    return torch.stack(Rs), torch.stack(Ts)


class Kam:
    def __init__(self, n):
        k = FK.lade_kamera(n)
        self.P = torch.tensor(k['P'], dtype=torch.float32, device=DEV)
        self.auge = torch.tensor(k['auge'], dtype=torch.float32, device=DEV)
        self.vorn = torch.tensor(k['R'][2], dtype=torch.float32, device=DEV)
        self.rechts = torch.tensor(k['R'][0], dtype=torch.float32, device=DEV)
        self.unten = torch.tensor(k['R'][1], dtype=torch.float32, device=DEV)

    def proj(self, x):
        h = x @ self.P[:, :3].T + self.P[:, 3]
        return h[..., :2] / h[..., 2:3]

    def tiefe(self, x):
        return (x - self.auge) @ self.vorn


def d_rundkegel(px, a, b, ra, rb):
    """px (H*W,2), a,b (M,2), ra,rb (M) -> Abstand (M, H*W) im Bild. Rand einer Kugelkette, Radius linear entlang der Achse."""
    ab = b - a
    l2 = (ab * ab).sum(-1).clamp(min=1e-6)
    ap = px[None, :, :] - a[:, None, :]
    t = ((ap * ab[:, None, :]).sum(-1) / l2[:, None]).clamp(0, 1)
    q = ap - t[..., None] * ab[:, None, :]
    return torch.sqrt((q * q).sum(-1) + 1e-6) - (ra[:, None] + t * (rb - ra)[:, None])


def prim_bild(P, kam, lage, gier, Rb, Tb):
    """Rundkegel in Bildpunkte + Bildradius."""
    pb = P['pb']
    Y = yrot(gier)
    wa = ((P['a'] - P['g'][pb])[:, None, :] @ Rb[pb].transpose(1, 2)).squeeze(1) + Tb[pb]
    wb = ((P['b'] - P['g'][pb])[:, None, :] @ Rb[pb].transpose(1, 2)).squeeze(1) + Tb[pb]
    wa = wa @ Y.T + lage
    wb = wb @ Y.T + lage
    pa, pz = kam.proj(wa), kam.proj(wb)
    sa = torch.norm(kam.proj(wa + 100 * kam.rechts) - pa, dim=-1) * 0.01
    sb = torch.norm(kam.proj(wb + 100 * kam.rechts) - pz, dim=-1) * 0.01
    return pa, pz, sa * P['ra'].abs(), sb * P['rb'].abs(), wa, wb


def pixelraster(schritt, box):
    x0, y0, x1, y1 = box
    xs, ys = np.meshgrid(np.arange(x0, x1, schritt), np.arange(y0, y1, schritt))
    return xs, ys


def ladung(params):
    P = {}
    for k, v in params.items():
        P[k] = torch.tensor(v, dtype=torch.long if k == 'pb' else torch.float32, device=DEV)
    return P


def d_feld(P, kam, lage, gier, Rb, Tb, px):
    pa, pb, ra, rb, wa, wb = prim_bild(P, kam, lage, gier, Rb, Tb)
    d = d_rundkegel(px, pa, pb, ra, rb)
    return d, wa, wb


BOX = {1: (170, 160, 460, 480), 2: (170, 130, 470, 480)}


def ansehen(params, aus):
    P = ladung(params)
    bilder = []
    with torch.no_grad():
        for n in (1, 2):
            kam = Kam(n)
            lage = P['lage%d' % n]
            gier = P['gier%d' % n]
            w = P['w2'] * (0.0 if n == 1 else 1.0)
            Rb, Tb = fk(P['g'], w)
            pa, pb, ra, rb, wa, wb = prim_bild(P, kam, lage, gier, Rb, Tb)
            xs, ys = np.meshgrid(np.arange(640), np.arange(480))
            px = torch.tensor(np.stack([xs.ravel(), ys.ravel()], 1), dtype=torch.float32, device=DEV)
            ds = []
            for i in range(0, px.shape[0], 40000):
                ds.append(d_rundkegel(px[i:i + 40000], pa, pb, ra, rb).min(0)[0])
            d = torch.cat(ds).reshape(480, 640).cpu().numpy()
            o = cv2.imread(os.path.join(FORM, 'bild%d.png' % n))
            z = lade_ziel(n)
            s = 2
            o2 = cv2.resize(o, None, fx=s, fy=s, interpolation=cv2.INTER_LINEAR)
            m = (d < 0).astype(np.uint8)
            cs, _ = cv2.findContours(cv2.resize(m, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(o2, cs, -1, (0, 255, 255), 1)
            cz, _ = cv2.findContours(cv2.resize((z == 255).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(o2, cz, -1, (0, 255, 0), 1)
            pa_, pb_, ra_, rb_ = [q.cpu().numpy() for q in (pa, pb, ra, rb)]
            farben = {}
            for i in range(len(ra_)):
                bi = int(P['pb'][i])
                f = farben.setdefault(bi, tuple(int(c) for c in np.random.RandomState(bi * 7 + 3).randint(80, 255, 3)))
                for (q, r) in ((pa_[i], ra_[i]), (pb_[i], rb_[i])):
                    cv2.circle(o2, (int(q[0] * s), int(q[1] * s)), int(r * s), f, 1, cv2.LINE_AA)
                cv2.line(o2, (int(pa_[i][0] * s), int(pa_[i][1] * s)), (int(pb_[i][0] * s), int(pb_[i][1] * s)), f, 1, cv2.LINE_AA)
                cv2.putText(o2, BONES[bi][:7], (int(pa_[i][0] * s) + 3, int(pa_[i][1] * s)), cv2.FONT_HERSHEY_SIMPLEX, 0.33, f, 1, cv2.LINE_AA)
            bx = BOX[n]
            bilder.append(o2[bx[1] * s:bx[3] * s, bx[0] * s:bx[2] * s])
            w_ = z == 255
            sw = z == 0
            ist = d < 0
            print('Bild %d: Weiss gedeckt %.3f, Schwarz getroffen %d, IoU %.3f' % (n, (ist & w_).sum() / w_.sum(), (ist & sw).sum(), (ist & w_).sum() / ((ist & (z != 128)) | w_).sum()))
    h = max(b.shape[0] for b in bilder)
    bl = [cv2.copyMakeBorder(b, 0, h - b.shape[0], 0, 6, cv2.BORDER_CONSTANT) for b in bilder]
    cv2.imwrite(aus, np.hstack(bl))


PARAM_GIER_MIN = 910.0      # FORMBLATT 6B: Achse a <= 100 Grad, a = 180 - Gier in Grad


def etiketten(n):
    """Soll je Pixel + Gewicht. Sicher weiss/schwarz aus der Zielmaske. Im Saum gilt der gemessene Soll-Umriss schwach, offene Zonen zaehlen fast nicht."""
    z = lade_ziel(n)
    f = cv2.imread(os.path.join(FORM, 'pruefung', 'figur_bild%d.png' % n), 0) > 0
    lab = (z == 255).astype(np.float32)
    gew = np.where(z == 128, 0.0, 1.0).astype(np.float32)
    w_d = cv2.distanceTransform((z == 255).astype(np.uint8), cv2.DIST_L2, 3)
    s_d = cv2.distanceTransform((z != 255).astype(np.uint8), cv2.DIST_L2, 3)
    gew = gew * (1.0 + 3.0 * np.exp(-np.where(z == 255, w_d, s_d) / 6))
    if n == 1:
        rand = cv2.distanceTransform((~f).astype(np.uint8), cv2.DIST_L2, 3) + cv2.distanceTransform(f.astype(np.uint8), cv2.DIST_L2, 3)
        saum = (z == 128) & (rand < 14)
        lab[saum] = f[saum]
        a14 = cv2.dilate(PR.poly_maske(PR.merkmale()['A14']['polygon']).astype(np.uint8), np.ones((9, 9), np.uint8)) > 0
        lab[a14 & (z != 0)] = 1.0     # Querband an der Taille: Figur (FORMBLATT Z6), der Umriss-Soll laesst dort eine Rinne
        ys = np.arange(480)[:, None] * np.ones((1, 640))
        gew[saum] = np.where(ys[saum] > 400, 0.15, 0.8)
        gew[a14 & (z != 0)] = 1.5
        c5 = PR.poly_maske([(286, 402), (304, 403), (306, 405), (312, 414), (318, 432), (318, 456), (286, 456)])
        lab[c5] = 0.0      # C5: Boden durch die Luecke, Rundling hat dort seine harte linke Kante
        gew[c5] = 1.0
    else:
        kante = np.array(PR.teil_b2('R')['kante_gelesen'] + PR.teil_b2('R')['kante_belegt'][:1], float)
        k = np.zeros((480, 640), np.uint8)
        cv2.polylines(k, [np.round(kante).astype(np.int32)], False, 255, 1)
        dist = cv2.distanceTransform(255 - k, cv2.DIST_L2, 3)
        # aussen: oberhalb oder links der Linie
        aussen = np.zeros((480, 640), np.uint8)
        q = np.vstack([kante, [[345, 120], [240, 120], [240, 215]]])
        cv2.fillPoly(aussen, [np.round(q).astype(np.int32)], 1)
        grau = z == 128
        m = grau & (aussen > 0) & (dist > 3)
        lab[m] = 0.0
        gew[m] = 0.5
        m = grau & (aussen == 0) & (dist > 3) & (dist < 16) & (np.arange(640)[None, :] > 255) & (np.arange(480)[:, None] < 220)
        lab[m] = 1.0
        gew[m] = 0.4
        # Rechts unter der Kuppel ist Raum (Beleg-Karte pruefung.md 2.2, Video rot, kein Schwarz): zwischen Bein FL (x<376) und Bein FR (x>414) keine dunkle Masse
        frei = PR.poly_maske([(380, 294), (412, 268), (414, 384), (380, 384)])
        lab[frei] = 0.0
        gew[frei] = 0.25
    return lab, gew


def lauf(params, schritte, aus, lr=6.0, nur_pose2=False):
    frei = {k: torch.tensor(v, dtype=torch.float32, device=DEV, requires_grad=True) for k, v in params.items() if k != 'pb'}
    pb = torch.tensor(params['pb'], dtype=torch.long, device=DEV)
    start = {k: v.detach().clone() for k, v in frei.items()}
    gruppen = [dict(params=[frei['a'], frei['b']], lr=lr), dict(params=[frei['ra'], frei['rb']], lr=lr * 0.7), dict(params=[frei['g']], lr=lr * 0.25),
               dict(params=[frei['w2']], lr=0.006), dict(params=[frei['lage1'], frei['lage2']], lr=lr * 1.0), dict(params=[frei['gier1'], frei['gier2']], lr=1.0)]
    if nur_pose2:
        gruppen = [dict(params=[frei['w2']], lr=0.006), dict(params=[frei['lage2']], lr=lr), dict(params=[frei['gier2']], lr=1.5)]
    opt = torch.optim.Adam(gruppen)
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(opt, schritte, eta_min=0.05)
    K = {n: Kam(n) for n in (1, 2)}
    Z = {}
    for n in (1, 2):
        lab, gew_v = etiketten(n)
        s = 2
        xs, ys = pixelraster(s, BOX[n])
        px = torch.tensor(np.stack([xs.ravel(), ys.ravel()], 1), dtype=torch.float32, device=DEV)
        Z[n] = dict(px=px, t=torch.tensor(lab[ys, xs].ravel().astype(np.float32), device=DEV), g=torch.tensor(gew_v[ys, xs].ravel().astype(np.float32), device=DEV))
        q = [p for p in PR.punkte_abschnitt7() if p['bild'] == n and p['zaehlt'] and not p['art'].startswith('mat')]
        Z[n]['soll'] = torch.tensor([[p['x'], p['y']] for p in q], dtype=torch.float32, device=DEV)
    gew_bild = {1: 1.0, 2: 0.6}
    WGEW = torch.tensor([{'hals': 0.5, 'kopf': 0.5, 'kiefer': 0.5, 'nr_ober': 1.0, 'nr_unter': 1.0, 'nr_hand': 1.0, 'nr_daumen': 2.0, 'rumpf': 25.0, 'rumpf_mitte': 25.0, 'huefte_r': 40.0, 'fr_ober': 40.0, 'fr_unter': 40.0, 'fr_fuss': 40.0}.get(b, 3.0) for b in BONES], dtype=torch.float32, device=DEV)
    FUSS_KNOCHEN = [IDX[n] for n in ('fr_fuss', 'fl_fuss', 'nl_daumen', 'nr_daumen')]
    FUSS_PRIM = [i for i, p in enumerate(E.R) if p[0] in ('fl_fuss', 'nl_daumen', 'nr_daumen')]     # Bein FR darf in Pose 2 abheben: Fuss steht sonst auf dem freien Boden BO
    for it in range(schritte):
        opt.zero_grad()
        P = dict(frei)
        P['pb'] = pb
        sig = 2.2 - 1.4 * it / schritte
        loss = 0.0
        info = {}
        for n in ((2,) if nur_pose2 else (1, 2)):
            w = P['w2'] * (0.0 if n == 1 else 1.0)
            Rb, Tb = fk(P['g'], w)
            d, wa, wb = d_feld(P, K[n], P['lage%d' % n], P['gier%d' % n], Rb, Tb, Z[n]['px'])
            dm = -sig * torch.logsumexp(-d / (sig * 0.5), 0) * 0.5
            p = torch.sigmoid(-dm / sig).clamp(1e-5, 1 - 1e-5)
            t = Z[n]['t']
            bce = -(t * torch.log(p) + (1 - t) * torch.log(1 - p))
            l = (bce * Z[n]['g']).sum() / Z[n]['g'].sum()
            info[n] = float(l)
            loss = loss + gew_bild[n] * l * 40
            dk = d_rundkegel(Z[n]['soll'], *prim_bild(P, K[n], P['lage%d' % n], P['gier%d' % n], Rb, Tb)[:4]).min(0)[0]
            loss = loss + 0.012 * (dk ** 2).sum()
            # Boden: nichts unter y=0 in Pose (Weltraum), Rundkegel
            for w_, r_ in ((wa, P['ra']), (wb, P['rb'])):
                unter = (w_[:, 1] + r_.abs()).clamp(min=0)
                loss = loss + 1e-4 * (unter ** 2).sum()
        # Bindung an den Start: Entwurf traegt den Bau, die Tiefe sieht nur Bild 2
        loss = loss + 3e-6 * ((P['a'] - start['a']) ** 2).sum() + 3e-6 * ((P['b'] - start['b']) ** 2).sum() + 6e-6 * ((P['ra'] - start['ra']) ** 2).sum() + 6e-6 * ((P['rb'] - start['rb']) ** 2).sum()
        loss = loss + 3e-6 * ((P['g'] - start['g']) ** 2).sum() + (WGEW[:, None] * (P['w2'] - start['w2']) ** 2).sum() + 1e-6 * ((P['lage1'] - start['lage1']) ** 2).sum() + 1e-6 * ((P['lage2'] - start['lage2']) ** 2).sum()
        loss = loss + 2e-4 * ((P['gier2'] - E.GIER2_SOLL) ** 2)
        # Pose 2: Fuesse bleiben auf dem Boden und flach
        Rb, Tb = fk(P['g'], P['w2'])
        pa, pz, ra, rb, wa, wb = prim_bild(P, K[2], P['lage2'], P['gier2'], Rb, Tb)
        for i in FUSS_PRIM:
            loss = loss + 2e-3 * ((torch.max(wa[i, 1] + P['ra'][i], wb[i, 1] + P['rb'][i])) ** 2)
        for b in FUSS_KNOCHEN:
            loss = loss + (0.5 if BONES[b] == 'fr_fuss' else 6.0) * ((Rb[b] - torch.eye(3, device=DEV)) ** 2).sum()
        loss.backward()
        opt.step()
        sched.step()
        with torch.no_grad():
            frei['lage1'][1] = 0.0
            frei['lage2'][1] = 0.0
        if it % 200 == 0 or it == schritte - 1:
            info.setdefault(1, 0.0)
            print(it, 'loss %.3f  b1 %.4f b2 %.4f  lage1 %s gier1 %.1f lage2 %s gier2 %.1f' % (float(loss), info[1], info[2], np.round(frei['lage1'].detach().cpu().numpy()), float(frei['gier1']), np.round(frei['lage2'].detach().cpu().numpy()), float(frei['gier2'])), flush=True)
    d = {k: v.detach().cpu().numpy().tolist() for k, v in frei.items()}
    d['pb'] = params['pb'].tolist()
    json.dump(d, open(aus, 'w', encoding='utf-8'))
    return {k: np.array(v, float if k != 'pb' else int) for k, v in d.items()}


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('was')
    a.add_argument('--start')
    a.add_argument('--aus', default=os.path.join(HIER, 'zw', 'passung.json'))
    a.add_argument('--bild', default=os.path.join(HIER, 'zw', 'passung.png'))
    a.add_argument('--schritte', type=int, default=1500)
    a.add_argument('--lage2')
    a.add_argument('--gier2', type=float)
    a.add_argument('--setze', default='')
    a.add_argument('--gier-soll', type=float)
    a = a.parse_args()
    os.makedirs(os.path.join(HIER, 'zw'), exist_ok=True)
    p = start_params(a.start)
    if a.gier_soll:
        E.GIER2_SOLL = a.gier_soll
    if a.lage2:
        p['lage2'] = np.array([float(x) for x in a.lage2.split(',')])
    if a.gier2:
        p['gier2'] = np.array(a.gier2)
    for t in [x for x in a.setze.split(';') if x]:
        nm, v = t.split('=')
        p['w2'][IDX[nm]] = [float(x) for x in v.split(',')]
    if a.was == 'ansehen':
        ansehen(p, a.bild)
    elif a.was == 'pose2':
        p = lauf(p, a.schritte, a.aus, nur_pose2=True)
        ansehen(p, a.bild)
    else:
        p = lauf(p, a.schritte, a.aus)
        ansehen(p, a.bild)
