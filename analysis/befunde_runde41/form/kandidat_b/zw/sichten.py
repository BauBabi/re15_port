"""Einzelstellen Sicht Ende gegen Sicht Flanke: Figur STARR in Pose 1 (nur Lage, Gier), je Giergitter. Bein FL wahlweise ausgelassen."""
import sys, json, math, time
sys.path.insert(0, '.')
import numpy as np, cv2, torch
from b_geom import *
import b_teile as T
import b_fit as F
import b_mesh as BM
from b_fit import GERAET, ablage_torch
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P

fit = BM.lade('zw/fit2.pt')
mod = F.Modell().to(GERAET)
mod.pos.data[:] = ablage_torch(fit['pos'])
mod.logr.data[:] = torch.log(ablage_torch(fit['rad']))
mod.pos.requires_grad_(False)
mod.logr.requires_grad_(False)
st = F.Sicht(2, K2, F.BOX2, 3000, 9000, 44, 2)
aus_fl = [T.IDX[k] for k in ('fl_ober', 'fl_unter', 'fl_fuss', 'fl_zeh')]
aus_n = [T.IDX[k] for k in ('nl_unter', 'nl_faust', 'nl_innen', 'nr_unter', 'nr_faust', 'nr_innen')]


def setze_kanten(ohne):
    s = torch.ones_like(volle[2], dtype=torch.bool)
    for b in ohne:
        s &= volle[2] != b
    return s


volle = (mod.ka.clone(), mod.kb.clone(), mod.kk.clone())
J = F.ablage_torch(fit['J'])
ident = dict(R=torch.eye(3, device=GERAET)[None].repeat(len(T.KNOCHEN), 1, 1), T=J, J=J)
N = st.dT.shape[0]
Z = cv2.imread(FORM + '/ziel_bild2.png', 0)
kan = None


def lauf(g0, ohne):
    s = setze_kanten(ohne)
    mod.ka, mod.kb, mod.kk = volle[0][s], volle[1][s], volle[2][s]
    best = None
    for start in ((2400, -23400), (2000, -23000), (2900, -23700)):
        g = torch.nn.Parameter(torch.tensor(g0 * 2 * math.pi / 4096, device=GERAET, dtype=torch.float32))
        l = torch.nn.Parameter(ablage_torch(np.array(start, float)))
        dy = torch.nn.Parameter(torch.zeros(1, device=GERAET))
        opt = torch.optim.Adam([{'params': [l], 'lr': 25.0}, {'params': [g], 'lr': 0.004}])
        for it in range(110):
            idx = torch.randperm(N, device=GERAET)[:2200]
            wl = torch.stack([l[0], torch.zeros_like(l[0]), l[1]])
            los, _ = F.sil(mod, st, wl, F.Y_t(g), idx, 30.0, 14.0, None)
            opt.zero_grad()
            los.backward()
            opt.step()
        if best is None or los.item() < best[0]:
            best = (los.item(), g.detach().clone(), l.detach().clone())
    los, g, l = best
    wl = torch.stack([l[0], torch.zeros_like(l[0]), l[1]])
    m = F.maske(mod, st, F.Y_t(g), wl, None)
    x0, y0, x1, y1 = st.box
    W = (x1 - x0 + 1) // 2
    H = (y1 - y0 + 1) // 2
    q = (m.reshape(H, W) < 0).astype(np.uint8)
    q = cv2.resize(q, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    v = np.zeros((480, 640), bool)
    v[y0:y0 + q.shape[0], x0:x0 + q.shape[1]] = q[:480 - y0, :640 - x0] > 0
    w = Z == 255
    b = Z == 0
    hk = P.enge(P.poly_maske(P.teil_b2('H')['polygon']), 2)
    u = P.ueberdeckung(v, Z)
    linien = {}
    for nm, a_, b_ in (('V1', (222, 292), (298, 292)), ('V2', (232, 347), (285, 347)), ('V3', (340, 338), (364, 338)), ('V4', (408, 225), (408, 262))):
        n = max(abs(b_[0] - a_[0]), abs(b_[1] - a_[1])) + 1
        xs = np.linspace(a_[0], b_[0], n).astype(int)
        ys = np.linspace(a_[1], b_[1], n).astype(int)
        linien[nm] = round(float(v[ys, xs].mean()), 2)
    return dict(g=round(g.item() * 4096 / 2 / math.pi % 4096), lage=[round(l[0].item()), round(l[1].item())], los=round(los, 4), weiss=u['weiss_gedeckt'], schwarz=u['schwarz_getroffen'], haken=round(float((v & hk).sum() / hk.sum()), 2), **linien)


t0 = time.time()
erg = []
for g0 in range(600, 3500, 200):
    for nm, ohne in (('alle', []), ('ohneFL', aus_fl)):
        r = lauf(g0, ohne)
        r['start'] = g0
        r['satz'] = nm
        erg.append(r)
        print(json.dumps(r), '%.0fs' % (time.time() - t0), flush=True)
json.dump(erg, open('zw/sichten.json', 'w'), indent=0)
