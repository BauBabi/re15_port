#!/usr/bin/env python3
"""Kandidat B: Volumen und Pose 2 gemeinsam auf beide Zielmasken ziehen (differenzierbar, Strahlen durch die Kameras der Vorlagen).
    python b_fit.py --aus zw/fit1 [--gier 1081] [--vor zw/fit0.pt]
"""
import argparse, json, math, os, sys, time
import numpy as np, cv2, torch
from b_geom import *
import b_teile as T

GERAET = 'cuda' if torch.cuda.is_available() else 'cpu'
BOX1 = (180, 170, 450, 462)
BOX2 = (170, 130, 470, 440)


def ablage_torch(a):
    return torch.tensor(np.asarray(a), dtype=torch.float32, device=GERAET)


def lade_masken(n):
    z = cv2.imread(FORM + '/ziel_bild%d.png' % n, 0)
    f = cv2.imread(FORM + '/pruefung/figur_bild%d.png' % n, 0) > 0
    t = np.zeros(z.shape, np.float32)
    w = np.zeros(z.shape, np.float32)
    t[z == 255] = 1
    w[z == 255] = 1
    t[z == 0] = 0
    w[z == 0] = 1
    g = z == 128
    t[g] = f[g].astype(np.float32)
    w[g] = 0.5
    if n == 1:
        nah = cv2.dilate((z > 0).astype(np.uint8), np.ones((31, 31), np.uint8)) > 0
        w[(z == 0) & nah] = 3.0
        Y_, X_ = np.mgrid[0:480, 0:640]
        offen = ((Y_ >= 401) & (X_ < 290)) | ((Y_ >= 415) & (X_ >= 290)) | (Y_ >= 430)
        w[offen & g] = 0.04
    else:
        w[g & ~f] = 0.02        # Bild 2: Grau ist fast ueberall frei
        w[g & f] = 0.5
        h = cv2.imread(HIER + '/zw/haken.png', 0)
        if h is not None:
            w[h > 0] = np.maximum(w[h > 0], 1.5)
            t[h > 0] = 1.0
    return t, w, z


class Sicht:
    def __init__(self, n, kam, box, t0, t1, nz, schritt):
        x0, y0, x1, y1 = box
        self.n, self.box, self.schritt = n, box, schritt
        xs, ys = np.meshgrid(np.arange(x0, x1, schritt, dtype=np.float64), np.arange(y0, y1, schritt, dtype=np.float64))
        self.xs, self.ys = xs.ravel(), ys.ravel()
        M = kam['P'][:, :3]
        h = np.stack([self.xs, self.ys, np.ones_like(self.xs)], 1)
        d = (np.linalg.inv(M) @ h.T).T
        d /= np.linalg.norm(d, axis=1, keepdims=True)
        f = kam['R'][2]
        d[(d @ f) < 0] *= -1
        d = d / (d @ f)[:, None]
        t, w, z = lade_masken(n)
        xi, yi = self.xs.astype(int), self.ys.astype(int)
        self.tt, self.ww, self.z = ablage_torch(t[yi, xi]), ablage_torch(w[yi, xi]), z
        self.dT, self.aT, self.tT = ablage_torch(d), ablage_torch(kam['auge']), ablage_torch(np.linspace(t0, t1, nz))


def euler_R(w):
    """w: n x 3 rad. R = Rx * Ry * Rz wie Engine."""
    cx, sx = torch.cos(w[:, 0]), torch.sin(w[:, 0])
    cy, sy = torch.cos(w[:, 1]), torch.sin(w[:, 1])
    cz, sz = torch.cos(w[:, 2]), torch.sin(w[:, 2])
    e, n = torch.ones_like(cx), torch.zeros_like(cx)
    Rx = torch.stack([e, n, n, n, cx, -sx, n, sx, cx], 1).view(-1, 3, 3)
    Ry = torch.stack([cy, n, sy, n, e, n, -sy, n, cy], 1).view(-1, 3, 3)
    Rz = torch.stack([cz, -sz, n, sz, cz, n, n, n, e], 1).view(-1, 3, 3)
    return Rx @ Ry @ Rz


def abst_kanten(p, a, b, ra, rb):
    pa = p[:, None, :] - a[None]
    ba = (b - a)[None]
    n = (ba * ba).sum(-1) + 1e-6
    t = ((pa * ba).sum(-1) / n).clamp(0, 1)
    d = pa - t[..., None] * ba
    return torch.sqrt((d * d).sum(-1) + 1e-6) - (ra[None] + t * (rb - ra)[None])


def glatt_min(d, k):
    return -k * torch.logsumexp(-d / k, dim=-1)


class Modell(torch.nn.Module):
    def __init__(self):
        super().__init__()
        N, E, G = T.start()
        self.namen = list(N)
        self.ni = {k: i for i, k in enumerate(self.namen)}
        p = np.array([N[k][0] for k in self.namen], float)
        r = np.array([N[k][1] for k in self.namen], float)
        self.pos0, self.rad0 = ablage_torch(p), ablage_torch(r)
        self.pos = torch.nn.Parameter(self.pos0.clone())
        self.logr = torch.nn.Parameter(torch.log(self.rad0.clone()))
        self.ka = torch.tensor([self.ni[a] for _, a, _ in E], device=GERAET)
        self.kb = torch.tensor([self.ni[b] for _, _, b in E], device=GERAET)
        self.kk = torch.tensor([T.IDX[b] for b, _, _ in E], device=GERAET)
        self.kanten = E
        self.gelenk = G
        n = len(T.KNOCHEN)
        self.w2 = torch.nn.Parameter(torch.zeros(n, 3, device=GERAET))     # Pose 2 Eulerwinkel im Bindraum
        self.dy2 = torch.nn.Parameter(torch.zeros(1, device=GERAET))
        self.gier2 = torch.nn.Parameter(torch.tensor(0.0, device=GERAET))
        self.lage2 = torch.nn.Parameter(torch.zeros(2, device=GERAET))

    def J(self):
        out = []
        for nm, _ in T.KNOCHEN:
            if nm in self.gelenk:
                out.append(self.pos[self.ni[self.gelenk[nm]]])
            else:
                out.append(ablage_torch(T.FEST[nm]))
        return torch.stack(out)

    def fk(self):
        L = euler_R(self.w2)
        J = self.J()
        R, Tt = [None] * len(T.ELTERN), [None] * len(T.ELTERN)
        for i, p in enumerate(T.ELTERN):
            if p < 0:
                R[i] = L[i]
                Tt[i] = J[i] + torch.cat([torch.zeros(1, device=GERAET), self.dy2, torch.zeros(1, device=GERAET)])
            else:
                R[i] = R[p] @ L[i]
                Tt[i] = (R[p] @ (J[i] - J[p])) + Tt[p]
        return dict(R=torch.stack(R), T=torch.stack(Tt), J=J)

    def radien(self):
        return torch.exp(self.logr)

    def abstand(self, p, po=None):
        A, B = self.ka, self.kb
        r = self.radien()
        if po is None:
            return abst_kanten(p, self.pos[A], self.pos[B], r[A], r[B])
        out = []
        for b in torch.unique(self.kk):
            s = self.kk == b
            q = (p - po['T'][b]) @ po['R'][b] + po['J'][b]
            out.append(abst_kanten(q, self.pos[A[s]], self.pos[B[s]], r[A[s]], r[B[s]]))
        return torch.cat(out, 1)

    def knoten_gepost(self, po):
        """Lage aller Knoten in Pose 2: je Knoten ueber den Knochen der ersten Kante."""
        bn = torch.zeros(self.pos.shape[0], dtype=torch.long, device=GERAET)
        bn[self.ka] = self.kk
        bn[self.kb] = self.kk
        return ((po['R'][bn] @ (self.pos - po['J'][bn]).unsqueeze(-1)).squeeze(-1) + po['T'][bn]), bn


def Y_t(g):
    c, s = torch.cos(g), torch.sin(g)
    n, e = torch.zeros_like(c), torch.ones_like(c)
    return torch.stack([c, n, s, n, e, n, -s, n, c]).view(3, 3)


def sil(mod, st, wl, g, idx, k, tau, po):
    D = st.dT[idx]
    N, M = D.shape[0], st.tT.shape[0]
    p = (st.aT + D[:, None, :] * st.tT[None, :, None]).view(-1, 3)
    q = (p - wl) @ g
    s = glatt_min(mod.abstand(q, po), k).view(N, M)
    m = s.min(1).values
    los = torch.nn.functional.binary_cross_entropy_with_logits(-m / tau, st.tt[idx], weight=st.ww[idx], reduction='sum') / (st.ww[idx].sum() + 1e-6)
    return los, m


def welt_param(mod, g0, lage0):
    g = (mod.gier2 + g0) * 2 * math.pi / 4096
    l = torch.stack([mod.lage2[0] + lage0[0], torch.zeros_like(mod.lage2[0]), mod.lage2[1] + lage0[1]])
    return g, l


def maske(mod, st, g, wl, po, k=30.0, block=1200):
    aus = []
    with torch.no_grad():
        for i in range(0, st.dT.shape[0], block):
            idx = torch.arange(i, min(i + block, st.dT.shape[0]), device=GERAET)
            _, m = sil(mod, st, wl, g, idx, k, 14.0, po)
            aus.append(m.cpu().numpy())
    return np.concatenate(aus)


def ueberlagerung(st, m, ziel):
    x0, y0, x1, y1 = st.box
    s = st.schritt
    o = cv2.imread(FORM + '/bild%d.png' % st.n)
    W = (x1 - x0 + s - 1) // s
    H = (y1 - y0 + s - 1) // s
    q = (m.reshape(H, W) < 0).astype(np.uint8)
    q = cv2.resize(q, None, fx=s, fy=s, interpolation=cv2.INTER_LINEAR)
    voll = np.zeros((480, 640), np.uint8)
    h = min(q.shape[0], 480 - y0)
    b = min(q.shape[1], 640 - x0)
    voll[y0:y0 + h, x0:x0 + b] = q[:h, :b]
    k, _ = cv2.findContours(voll, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    a = o.copy()
    for wert, f in ((255, (90, 240, 70)), (1, (255, 200, 70))):
        kk, _ = cv2.findContours((st.z >= wert).astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(a, kk, -1, f, 1)
    cv2.drawContours(a, k, -1, (0, 245, 255), 1)
    a = cv2.resize(a[y0:y1, x0:x1], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(ziel, a)
    return voll


def speichere(mod, pfad, g0, lage0):
    o = dict(namen=mod.namen, kanten=mod.kanten, gelenk=mod.gelenk, pos=mod.pos.detach().cpu().numpy(), rad=mod.radien().detach().cpu().numpy(), J=mod.J().detach().cpu().numpy(),
             w2=mod.w2.detach().cpu().numpy(), dy2=float(mod.dy2.item()), gier2=float(mod.gier2.item() + g0), lage2=[float(mod.lage2[0].item() + lage0[0]), 0.0, float(mod.lage2[1].item() + lage0[1])])
    torch.save(o, pfad)


PAARE = [('nl_s', 'nl_e', 'nr_s', 'nr_e'), ('nl_e', 'nl_w', 'nr_e', 'nr_w'), ('nl_w', 'nl_a1', 'nr_w', 'nr_a1'), ('nl_a1', 'nl_a2', 'nr_a1', 'nr_a2'), ('nl_i1', 'nl_i2', 'nr_i1', 'nr_i2')]
SPIEGEL_R = ['fr_h', 'fr_s', 'fr_k']
R_MIN = dict(fr_h=270, fr_s=340, fr_k=270, fr_a=260, fr_f=180, fr_t1=85, fr_t2=85, fr_t3=90, fr_t4=105, fl_h=270, fl_s=340, fl_k=270, fl_a=260, fl_f=180, fl_t1=85, fl_t2=85, fl_t3=90, fl_t4=105,
             nl_s=340, nl_e=300, nl_w=220, nl_a1=190, nl_a2=180, nl_i1=170, nl_i2=150, nr_s=300, nr_e=195, nr_w=220, nr_a1=200, nr_a2=200, nr_i1=140, nr_i2=120)
STEIF = ['fr_t1', 'fr_t2', 'fr_t3', 'fr_t4', 'fl_t1', 'fl_t2', 'fl_t3', 'fl_t4', 'fl_f', 'fl_a', 'fr_f']


def ik_start(mod, g0, lage0, ziele, iters=300):
    """Glieder grob auf 3D-Ziele in Pose 2 ziehen, bevor die Silhouette greift. ziele: {knoten: Weltpunkt}."""
    opt = torch.optim.Adam([mod.w2], lr=0.03)
    Y_ = ablage_torch(Y(g0))
    wl = ablage_torch(np.array([lage0[0], 0, lage0[1]]))
    for it in range(iters):
        po = mod.fk()
        q, _ = mod.knoten_gepost(po)
        l = (mod.w2 ** 2).sum() * 0.002
        for k, z in ziele.items():
            w = (Y_ @ q[mod.ni[k]]) + wl
            l = l + ((w - ablage_torch(z)) ** 2).sum() / 1e5
        opt.zero_grad()
        l.backward()
        opt.step()
    with torch.no_grad():
        mod.w2[[0, 9, 10]] = 0
    return l.item()


def lauf(aus, g0=1081, lage0=(2500, -23500), iters=1400, ziele=None, vor=None, lr=7.0):
    mod = Modell().to(GERAET)
    s1 = Sicht(1, K1, BOX1, 3700, 8300, 52, 2)
    s2 = Sicht(2, K2, BOX2, 3000, 9000, 56, 2)
    wl1 = ablage_torch(O1)
    g1 = ablage_torch(Y(G1))
    if ziele:
        print('IK', ik_start(mod, g0, lage0, ziele), flush=True)
    t_arm = None
    opt = torch.optim.Adam([{'params': [mod.pos], 'lr': lr}, {'params': [mod.logr], 'lr': lr / 900.0}, {'params': [mod.w2], 'lr': 0.015},
                            {'params': [mod.dy2], 'lr': 4.0}, {'params': [mod.gier2], 'lr': 2.0}, {'params': [mod.lage2], 'lr': 10.0}])
    f1 = ablage_torch(Y(G1).T @ K1['R'][2])
    t0 = time.time()
    N1, N2 = s1.dT.shape[0], s2.dT.shape[0]
    rumpf = [T.IDX[x] for x in ('rumpf_f', 'rumpf_m', 'rumpf', 'maul_l', 'maul_r')]
    feste = [mod.ni[k] for k in ('fr_t1', 'fr_t2', 'fr_t3', 'fr_t4', 'fl_t1', 'fl_t2', 'fl_t3', 'fl_t4')]
    sp = ablage_torch(np.array([1, 1, -1.0]))
    r_i = {k: i for k, i in mod.ni.items()}
    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]
    c0 = ablage_torch(T.KUPPEL_MITTE)
    starr = [T.IDX[x] for x in ('nl_ober', 'nl_unter', 'nl_faust', 'nl_innen', 'nr_ober', 'nr_unter', 'nr_faust', 'nr_innen', 'fr_ober', 'fr_unter', 'fr_fuss', 'fr_zeh')]
    for it in range(iters):
        idx1 = torch.randperm(N1, device=GERAET)[:4500]
        idx2 = torch.randperm(N2, device=GERAET)[:3500]
        l1, _ = sil(mod, s1, wl1, g1, idx1, 30.0, 14.0, None)
        po = mod.fk()
        g, l = welt_param(mod, g0, lage0)
        l2, _ = sil(mod, s2, l, Y_t(g), idx2, 30.0, 14.0, po)
        p, r = mod.pos, mod.radien()
        dp = p - mod.pos0
        l_tief = ((dp @ f1) ** 2).mean() / (350.0 ** 2)
        l_form = (dp ** 2).sum(1).mean() / (900.0 ** 2)
        l_kup = (dp[i_d] ** 2).sum(1).mean() / (170.0 ** 2) + ((mod.logr[i_d] - torch.log(mod.rad0[i_d])) ** 2).mean() / (0.12 ** 2)
        l_min = sum(torch.relu(ablage_torch(v) - r[r_i[k_]]) ** 2 for k_, v in R_MIN.items()) / (25.0 ** 2)
        l_steif = (dp[[r_i[k_] for k_ in STEIF]] ** 2).sum(1).mean() / (90.0 ** 2)
        l_schulter = 0
        for k_, gr_ in (('nr_s', 1130.0), ('nl_s', 1260.0)):
            i_ = r_i[k_]
            l_schulter = l_schulter + torch.relu(torch.norm(p[i_] - c0) + r[i_] - gr_) ** 2 / (60.0 ** 2)
        # Boden in beiden Posen
        l_b1 = torch.relu(p[:, 1] + r).mean() / 40.0
        q, _ = mod.knoten_gepost(po)
        un = q[:, 1] + r
        l_b2 = torch.relu(un).mean() / 40.0 + torch.relu(-un.max()) / 150.0
        # Fuesse FR, FL stehen in Pose 1 auf dem Boden
        l_fuss = ((p[feste, 1] + r[feste]) ** 2).mean() / (60.0 ** 2)
        # Paare gleich lang, gleich stark
        l_paar = 0
        for a, b, c, d in PAARE:
            la = torch.norm(p[r_i[a]] - p[r_i[b]])
            lb = torch.norm(p[r_i[c]] - p[r_i[d]])
            l_paar = l_paar + ((la - lb) / 200.0) ** 2
        # FL = Spiegel von FR an Ebene Z = z0
        z0 = -100.0
        l_sp = 0
        for k in SPIEGEL_R:
            a = p[r_i[k]]
            b = p[r_i[k.replace('fr_', 'fl_')]]
            m = a * sp
            m = m + ablage_torch(np.array([0, 0, 2 * z0]))
            l_sp = l_sp + ((m - b) ** 2).sum() / (250.0 ** 2)
            l_sp = l_sp + ((r[r_i[k]] - r[r_i[k.replace('fr_', 'fl_')]]) / 60.0) ** 2
        # Rumpf starr in Pose 2, Glieder sanft
        l_w = (mod.w2[rumpf] ** 2).sum() * 4.0 + (mod.w2[starr] ** 2).sum() * 1.0 + (mod.w2 ** 2).sum() * 0.01
        los = l1 + l2 + 0.05 * l_tief + 0.015 * l_form + 0.03 * l_kup + 0.05 * l_schulter + 0.01 * l_min + 0.03 * l_steif + 0.1 * (l_b1 + l_b2) + 0.02 * l_fuss + 0.01 * l_paar + 0.006 * l_sp + 0.02 * l_w
        opt.zero_grad()
        los.backward()
        opt.step()
        if it % 200 == 0 or it == iters - 1:
            print('it %4d b1 %.4f b2 %.4f tief %.3f form %.3f paar %.3f sp %.2f w %.2f gier %.0f lage %.0f %.0f  %.0fs' % (it, l1.item(), l2.item(), l_tief.item(), l_form.item(), float(l_paar), float(l_sp), l_w.item(), g.item() * 4096 / 2 / math.pi % 4096, l[0].item(), l[2].item(), time.time() - t0), flush=True)
    speichere(mod, aus + '.pt', g0, lage0)
    with torch.no_grad():
        m1 = maske(mod, s1, g1, wl1, None)
        ueberlagerung(s1, m1, aus + '_b1.png')
        po = mod.fk()
        g, l = welt_param(mod, g0, lage0)
        m2 = maske(mod, s2, Y_t(g), l, po)
        ueberlagerung(s2, m2, aus + '_b2.png')
    return mod


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', default='zw/fit3')
    ap.add_argument('--iter', type=int, default=1200)
    ap.add_argument('--gier', type=float, default=2336)
    ap.add_argument('--x', type=float, default=2626)
    ap.add_argument('--z', type=float, default=-23499)
    a = ap.parse_args()
    os.makedirs(os.path.dirname(a.aus) or '.', exist_ok=True)
    # Sicht Ende: Pose 2 = Pose 1, nur Bein FL schwingt angehoben nach vorn (Boden vor der Figur ist frei, FORMBLATT 6B)
    lg = np.array([a.x, 0.0, a.z])

    def lokal2welt(p):
        return Y(a.gier) @ np.array(p, float) + lg
    ziele = {'fl_k': lokal2welt((-520, -1060, -820)), 'fl_a': lokal2welt((-240, -640, -820)), 'fl_f': lokal2welt((-120, -470, -820))}
    lauf(a.aus, a.gier, lage0=(a.x, a.z), ziele=ziele, iters=a.iter)
