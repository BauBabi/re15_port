#!/usr/bin/env python3
"""Gerippe rechnen: gemeinsame Masse, zwei Posen, Stapel von Starts auf der GPU.

    python fit.py --saat 1 --stapel 8 --iter 2400 --stride 3 --aus lauf/s1.pt
    python fit.py --weiter lauf/s1.pt --beste 2 --iter 1200 --stride 2 --aus lauf/s1_fein.pt

Jeder Start: Laengen und Dicken gemeinsam, Winkel/Standort/Gier je Bild. Projektion ueber form_kamera.lade_kamera (Matrix P).
Schalter fuer Gegenproben: --hand2 boden|frei, --zwang "name=wert" (siehe zwaenge()).
"""
import argparse, json, math, os, sys, time
import numpy as np, cv2, torch

HIER = os.path.dirname(os.path.abspath(__file__))
F = os.path.normpath(os.path.join(HIER, '..'))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_kamera as FK      # noqa: E402
import rig as RG              # noqa: E402
import ziele as ZI            # noqa: E402

GER = 'cuda'
DT = torch.float32
ROI = {1: (170, 170, 450, 480), 2: (130, 130, 480, 480)}

# Anker nur am Anfang: sie bringen die Glieder in die richtige Gegend. (Knochen, 'start'|'ende', x, y)
ANKER = {1: [('huefte', 'start', 300, 235), ('hand_r', 'ende', 398, 366), ('unterarm_r', 'start', 396, 310), ('fuss_l', 'start', 208, 425), ('fuss_r', 'start', 308, 440),
             ('schwanz1', 'start', 300, 210), ('schwanz2', 'start', 278, 238), ('schwanz3', 'start', 270, 285), ('schwanz4', 'start', 268, 340),
             ('schwanz5', 'start', 268, 395), ('schwanz6', 'start', 264, 440), ('schwanz6', 'ende', 262, 475),
             ('unterschenkel_l', 'start', 234, 350), ('unterschenkel_r', 'start', 326, 352)],
         2: [('hand_l', 'start', 425, 382), ('hand_l', 'ende', 445, 402), ('hand_r', 'start', 262, 398), ('hand_r', 'ende', 232, 425),
             ('unterarm_l', 'start', 400, 300), ('unterarm_r', 'start', 335, 318), ('kopf', 'start', 320, 240)]}


class Bild:
    def __init__(self, nr, stride, ger, nutzer_erosion=3, dilat=5):
        self.nr, self.stride = nr, stride
        self.kam = FK.lade_kamera(nr)
        z, m = ZI.lade()
        z = z[nr]
        teile = ZI.TEILE[nr]
        self.teile = teile
        H, W = z.shape
        x0, y0, x1, y1 = ROI[nr]
        wahl = np.zeros((H, W), bool)
        wahl[y0:y1:stride, x0:x1:stride] = True
        wahl[::stride * 4, ::stride * 4] = True
        ys, xs = np.nonzero(wahl)
        self.xs, self.ys = xs, ys
        self.px = torch.tensor(xs, dtype=DT, device=GER)
        self.py = torch.tensor(ys, dtype=DT, device=GER)
        pix = lambda q: torch.tensor(np.nonzero(q[ys, xs])[0], device=GER)
        ver = np.zeros((H, W), bool)
        for n in teile:
            ver |= m[n]
        self.pos, self.aus, self.aus_z, self.flaeche, self.sek = {}, {}, {}, {}, {}
        mass = json.load(open(os.path.join(HIER, 'lauf', 'teile_mass.json'), encoding='utf-8'))
        for n in teile:
            kern = ZI.ero(m[n], nutzer_erosion)
            # Doppelt beschriftete Pixel zaehlen fuer das vordere Teil (Bild 1: Schwanz vor Beinen vor Koerper)
            for v in {'part_of_Body': ['right_leg', 'tail'], 'right_leg': ['tail'], 'left_leg': ['tail']}.get(n, []):
                kern = kern & ~m[v]
            self.pos[n] = pix(kern)
            # Abschnitte entlang der Mittellinie: duenne Stellen zaehlen so viel wie die breite Hand
            ml = np.array(mass[n]['mittellinie'])[:, :2]
            S = {'tail': 7, 'face_and_upper_body': 3}.get(n, 5)
            xk, yk = xs[self.pos[n].cpu().numpy()], ys[self.pos[n].cpu().numpy()]
            d = (xk[:, None] - ml[:, 0]) ** 2 + (yk[:, None] - ml[:, 1]) ** 2
            ab = (np.argmin(d, 1) * S) // len(ml)
            ls = [torch.tensor(np.nonzero(ab == q)[0], device=GER) for q in range(S) if (ab == q).sum() > 3]
            self.sek[n] = ls
            erlaubt = ZI.dil(m[n], dilat)
            andere = np.zeros((H, W), bool)
            for q in teile:
                if q != n and not (n == 'part_of_Body' and q == 'right_arm'):     # der Arm liegt hinter dem Rumpf, er verdeckt ihn nicht
                    andere |= ZI.dil(m[q], 2)
            self.aus[n] = pix(~(erlaubt | andere))       # ohne Tiefe: andere Teile duerfen verdecken
            self.aus_z[n] = pix(~erlaubt)                # mit Tiefe: sichtbar ausserhalb ist falsch
            self.flaeche[n] = float(len(self.pos[n]))
        self.neg = pix((z == 0) & ~ZI.dil(ver, 3))
        self.fig = pix(z == 255)
        self.nicht_ver = pix(~ZI.dil(ver, 3))
        self.P = torch.tensor(self.kam['P'], dtype=DT, device=GER)
        self.P = self.P / self.P[2, :3].norm()
        self.auge = torch.tensor(self.kam['auge'], dtype=DT, device=GER)
        self.vorn = torch.tensor(self.kam['R'][2], dtype=DT, device=GER)
        kn = ger.st_kn.cpu().numpy()
        self.prim, self.prim_ex = {}, {}
        besetzt = np.zeros(ger.K, bool)
        for n in self.teile:
            ids = [RG.IDX[b] for b in RG.TEIL_KNOCHEN[self.nr][n]]
            self.prim[n] = torch.tensor(np.nonzero(np.isin(kn, ids))[0], device=GER)
            ids = [RG.IDX[b] for b in RG.TEIL_KNOCHEN[self.nr][n]]
            sel = np.nonzero(np.isin(kn, ids))[0]
            besetzt[sel] = True
            self.prim_ex[n] = torch.tensor(sel, device=GER)
        self.prim_frei = torch.tensor(np.nonzero(~besetzt)[0], device=GER)
        if self.nr == 2:
            # Oberkoerper-Maske deckt auch, was im Bild nicht getrennt ist: Beine stoeren dort nicht. Der Schwanz muss in Bild 2 verdeckt bleiben.
            arme = torch.cat([self.prim_ex['left_arm_front'], self.prim_ex['right_arm_front']])
            sw = np.nonzero(np.isin(kn, [RG.IDX['schwanz%d' % q] for q in range(1, 7)]))[0]
            ob = np.nonzero(kn == RG.IDX['oberarm_r'])[0]    # Gegenprobe: rechter Oberarm bleibt unter der Rumpfkante
            alle = np.ones(ger.K, bool)
            alle[arme.cpu().numpy()] = False
            alle[sw] = False
            alle[ob] = False
            self.prim['face_and_upper_body'] = torch.tensor(np.nonzero(alle)[0], device=GER)
            self.prim_verdeckt = torch.tensor(np.concatenate([sw, ob]), device=GER)
        else:
            ids = [RG.IDX[b] for b in RG.NAMEN if b in ('hals', 'kopf', 'kiefer') or b.endswith('_l') and ('arm' in b or 'hand' in b) or b.startswith('klaue_l')]
            self.prim_verdeckt = torch.tensor(np.nonzero(np.isin(kn, ids))[0], device=GER)


class Parameter(torch.nn.Module):
    """Roh-Parameter als Vektoren. Masse log-skaliert ueber den Startwert, Winkel im Bogenmass, Standort in 1000 Einheiten."""

    def __init__(self, B):
        super().__init__()
        self.B = B
        nL, nV, nD = len(RG.L_KEYS), len(RG.V_KEYS), len(RG.D_KEYS)
        self.l = torch.nn.Parameter(torch.zeros(B, nL, dtype=DT, device=GER))
        self.v = torch.nn.Parameter(torch.zeros(B, nV, dtype=DT, device=GER))
        self.da = torch.nn.Parameter(torch.zeros(B, nD, 3, dtype=DT, device=GER))
        self.db = torch.nn.Parameter(torch.zeros(B, nD, 3, dtype=DT, device=GER))
        self.wurzel = torch.nn.ParameterList([torch.nn.Parameter(torch.zeros(B, 3, dtype=DT, device=GER)) for _ in range(2)])
        self.gier = torch.nn.ParameterList([torch.nn.Parameter(torch.zeros(B, dtype=DT, device=GER)) for _ in range(2)])
        self.ang = torch.nn.ParameterList([torch.nn.Parameter(torch.zeros(B, RG.N, 3, dtype=DT, device=GER)) for _ in range(2)])
        t = lambda x: torch.tensor(x, dtype=DT, device=GER)
        self.L0 = t([RG.LAENGE0[k] for k in RG.L_KEYS])
        self.Lmin = t([RG.LAENGE_GRENZE[k][0] for k in RG.L_KEYS])
        self.Lmax = t([RG.LAENGE_GRENZE[k][1] for k in RG.L_KEYS])
        self.V0 = t([RG.VERSATZ0[k] for k in RG.V_KEYS])
        self.Vmin = t([RG.VERSATZ_GRENZE[k][0] for k in RG.V_KEYS])
        self.Vmax = t([RG.VERSATZ_GRENZE[k][1] for k in RG.V_KEYS])
        self.Vska = t([1.0 if k == 'klaue_z' else 1000.0 for k in RG.V_KEYS])
        self.A0 = t([RG.DICKE0[k][0] for k in RG.D_KEYS])
        self.B0 = t([RG.DICKE0[k][1] if RG.DICKE0[k][1] is not None else RG.DICKE0[k][0] for k in RG.D_KEYS])
        self.ell = t([1.0 if RG.DICKE0[k][1] is not None else 0.0 for k in RG.D_KEYS])[None, :, None]
        self.amin = t([RG.DICKE_KLASSE[k][0] for k in RG.D_KEYS])[None, :, None]
        self.amax = t([RG.DICKE_KLASSE[k][1] for k in RG.D_KEYS])[None, :, None]
        self.bmin = t([RG.DICKE_KLASSE[k][2] for k in RG.D_KEYS])[None, :, None]
        self.bmax = t([RG.DICKE_KLASSE[k][3] for k in RG.D_KEYS])[None, :, None]

    def masse(self):
        lae = torch.minimum(torch.maximum(self.L0 * torch.exp(self.l), self.Lmin), self.Lmax)
        ver = torch.minimum(torch.maximum(self.V0 + self.Vska * self.v, self.Vmin), self.Vmax)
        a3 = torch.minimum(torch.maximum(self.A0 * torch.exp(self.da), self.amin), self.amax)
        b3 = torch.minimum(torch.maximum(self.B0 * torch.exp(self.db), self.bmin), self.bmax)
        b3 = self.ell * b3 + (1.0 - self.ell) * a3
        return lae, ver, a3, b3


def als_dict(lae, ver, a3, b3, b=0):
    L = {k: float(lae[b, i]) for i, k in enumerate(RG.L_KEYS)}
    V = {k: float(ver[b, i]) for i, k in enumerate(RG.V_KEYS)}
    D = {k: ([float(x) for x in a3[b, i]], [float(x) for x in b3[b, i]]) for i, k in enumerate(RG.D_KEYS)}
    return L, V, D


def zustand(par, ger, bilder):
    lae, ver, a3, b3 = par.masse()
    off, start, ende = ger.achsen(lae, ver, b3)
    qa, qb = ger.querschnitte(a3, b3)
    aus = []
    for i, bi in enumerate(bilder):
        w = par.wurzel[i] * 1000.0
        R, T = ger.fk(off, w, par.gier[i], par.ang[i])
        c, e0, e1, e2 = ger.primitive(R, T, start, ende, qa, qb)
        u, v, q11, q12, q22, reff, z, rz = RG.projektion(bi.P, bi.auge, bi.vorn, c, e0, e1, e2)
        ry = torch.sqrt(e0[..., 1] ** 2 + e1[..., 1] ** 2 + e2[..., 1] ** 2 + 1e-6)
        aus.append(dict(R=R, T=T, c=c, e0=e0, e1=e1, e2=e2, u=u, v=v, q=(q11, q12, q22), reff=reff, z=z, rz=rz, ry=ry))
    return lae, ver, a3, b3, off, start, ende, aus


# Index-Tabellen einmal bauen
_DRILL = None
_KETTE = [('oberarm', 'unterarm'), ('unterarm', 'hand'), ('oberschenkel', 'unterschenkel'), ('unterschenkel', 'fuss'),
          ('schwanz1', 'schwanz2'), ('schwanz2', 'schwanz3'), ('schwanz3', 'schwanz4'), ('schwanz4', 'schwanz5'),
          ('schwanz5', 'schwanz6'), ('huefte', 'wirbel'), ('wirbel', 'brust'), ('hals', 'kopf')]


def tabellen(ger):
    global _DRILL
    if _DRILL is None:
        d = np.zeros((RG.N, 3))
        for b, n in enumerate(RG.NAMEN):
            k = RG.klasse(n)
            j = 1 if RG.RICHTUNG[k][1] != 0 else 0
            if k not in ('oberarm', 'oberschenkel', 'hand'):
                d[b, j] = 1.0
        _DRILL = dict(
            drill=torch.tensor(d, dtype=DT, device=GER),
            sw=torch.tensor([RG.IDX['schwanz%d' % q] for q in range(1, 7)], device=GER),
            sw_l=torch.tensor([RG.L_KEYS.index('schwanz%d' % q) for q in range(1, 7)], device=GER),
            sw_d=torch.tensor([RG.D_KEYS.index('schwanz%d' % q) for q in range(1, 7)], device=GER),
            ka=torch.tensor([RG.D_KEYS.index(a) for a, _ in _KETTE], device=GER),
            kb=torch.tensor([RG.D_KEYS.index(b) for _, b in _KETTE], device=GER))
    return _DRILL


def verlust(par, ger, bilder, stufe, gew, tau=2.0, gamma=150.0, details=False, opt=None):
    lae, ver, a3, b3, off, start, ende, st = zustand(par, ger, bilder)
    tb = tabellen(ger)
    opt = opt or {}
    B = par.B
    summe = torch.zeros(B, dtype=DT, device=GER)
    tk = {}
    for i, bi in enumerate(bilder):
        s = st[i]
        sd, f = RG.abstand(bi.px, bi.py, s['u'], s['v'], *s['q'], s['reff'])
        sd_all = sd.min(1).values
        for n in bi.teile:
            sdg = sd[:, bi.prim[n]].min(1).values
            h = torch.relu(sdg[:, bi.pos[n]] + 1.0).pow(2)
            cov = torch.stack([h[:, q].mean(1) for q in bi.sek[n]], 0).mean(0)
            sdx = sd[:, bi.prim_ex[n]].min(1).values
            exc = torch.relu(1.0 - sdx[:, bi.aus[n]]).pow(2).sum(1) / bi.flaeche[n]
            summe = summe + gew['cov'] * cov + gew['exc'] * exc
            tk['%d_%s_cov' % (bi.nr, n)] = cov.detach()
            tk['%d_%s_exc' % (bi.nr, n)] = exc.detach()
        ska = bi.stride ** 2
        neg = torch.relu(2.0 - sd_all[:, bi.neg]).pow(2).sum(1) * ska / 2000.0
        fig = torch.relu(sd_all[:, bi.fig] + 1.0).pow(2).mean(1)
        summe = summe + gew['neg'] * neg + gew['fig'] * fig
        tk['%d_neg' % bi.nr] = neg.detach()
        tk['%d_fig' % bi.nr] = fig.detach()
        if stufe >= 1:
            zp = s['z'][..., None] - s['rz'][..., None] * torch.sqrt((1.0 - f * f).clamp(min=0.0) + 1e-6)
            o = torch.sigmoid(-sd / tau)
            # Sichtbarkeit nur unter Primitiven, die das Pixel decken. Sonst schlaegt ein ferner Tiefenvorsprung jede Deckung
            # (Fehler bis Serie G: der Arm in Bild 1 liegt 3000 Einheiten hinter dem Schwanz und galt als unsichtbar).
            deckt = (o > 0.01).to(o.dtype)
            z0 = (zp.detach() + (1.0 - deckt) * 1e6).min(1, keepdim=True).values
            w = deckt * o * torch.exp(-torch.clamp((zp - z0) / gamma, 0.0, 25.0))
            P = w / (w.sum(1, keepdim=True) + 1e-9)
            S = 1.0 - torch.exp(torch.log1p(-o.clamp(max=0.9999)).sum(1))
            for n in bi.teile:
                Lv = S * P[:, bi.prim[n]].sum(1)
                h = (1.0 - Lv[:, bi.pos[n]])
                pos = torch.stack([h[:, q].mean(1) for q in bi.sek[n]], 0).mean(0)
                Lx = S * P[:, bi.prim_ex[n]].sum(1)
                aus = Lx[:, bi.aus_z[n]].sum(1) / bi.flaeche[n]
                summe = summe + gew['vis_pos'] * pos + gew['vis_aus'] * aus
                tk['%d_%s_vpos' % (bi.nr, n)] = pos.detach()
                tk['%d_%s_vaus' % (bi.nr, n)] = aus.detach()
            if len(bi.prim_frei):
                Lf = S * P[:, bi.prim_frei].sum(1)
                fr = Lf[:, bi.nicht_ver].sum(1) * ska / 3000.0
                summe = summe + gew['frei'] * fr
                tk['%d_frei' % bi.nr] = fr.detach()
            summe = summe + gew['neg_s'] * S[:, bi.neg].sum(1) * ska / 2000.0
            vd = (S * P[:, bi.prim_verdeckt].sum(1)).sum(1) * ska / 3000.0
            summe = summe + gew['verdeckt'] * vd
            tk['%d_verdeckt' % bi.nr] = vd.detach()
        # Boden
        tief = s['c'][..., 1] + s['ry']
        boden = torch.relu(tief + 10.0).pow(2).sum(1) / (60.0 ** 2)
        summe = summe + gew['boden'] * boden
        ko = 0.0
        kontakt = list(RG.BODEN_KONTAKT[bi.nr])
        if bi.nr == 2 and opt.get('hand2', 'boden') == 'frei':
            kontakt = [q for q in kontakt if not q.startswith('hand')]
        if bi.nr == 2 and opt.get('hand2', 'boden') == 'links':
            kontakt = [q for q in kontakt if q != 'hand_r']
        for b in kontakt:
            sel = (ger.st_kn == RG.IDX[b]).nonzero()[:, 0]
            t = tief[:, sel].max(1).values
            ko = ko + (t / 80.0).pow(2)
            # Handflaeche und Sohle am Boden liegen flach: Knochen-Y zeigt nach unten, das Ende steht nicht ueber dem Anfang
            ko = ko + 8.0 * (1.0 - s['R'][:, RG.IDX[b], 1, 1]).pow(2)
            ko = ko + (torch.relu(s['c'][:, sel[0], 1] - s['c'][:, sel[-1], 1]) / 100.0).pow(2)
        summe = summe + gew['kontakt'] * ko
        tk['%d_kontakt' % bi.nr] = ko.detach()
        if bi.nr == 1:
            # Sohle K1 ist die einzige sicher gelesene Bodenstelle (FORMBLATT 1.2): Handmitte am Boden -> (398,374)
            sel = (ger.st_kn == RG.IDX['hand_r']).nonzero()[:, 0]
            pm = s['c'][:, sel[1]] * torch.tensor([1.0, 0.0, 1.0], dtype=DT, device=GER)
            h = pm @ bi.P[:, :3].T + bi.P[:, 3]
            k1 = ((h[:, 0] / h[:, 2] - 398.0) ** 2 + (h[:, 1] / h[:, 2] - 374.0) ** 2) / 36.0
            summe = summe + gew['k1'] * k1
            tk['1_k1'] = k1.detach()
        if gew['anker'] > 0:
            ka = 0.0
            for nm, wo, x, y in ANKER[bi.nr]:
                sel = (ger.st_kn == RG.IDX[nm]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                ka = ka + ((s['u'][:, j] - x) ** 2 + (s['v'][:, j] - y) ** 2) / 400.0
            summe = summe + gew['anker'] * ka
        ang = par.ang[i]
        grenze = (torch.relu(ger.lo - ang) ** 2 + torch.relu(ang - ger.hi) ** 2) * ger.frei
        summe = summe + gew['grenze'] * grenze.sum((1, 2))
        summe = summe + gew['ruhe'] * (ang * ger.frei).pow(2).sum((1, 2))
        summe = summe + gew['drill'] * (ang * tb['drill']).pow(2).sum((1, 2))
        d2 = (ang[:, tb['sw'][1:], 1:] - ang[:, tb['sw'][:-1], 1:]).pow(2).sum((1, 2))
        summe = summe + gew['schwanz'] * d2
        # Verdeckte Glieder: linker Arm in Bild 1 wie der rechte, Beine in Bild 2 gegengleich aehnlich, Schwanz in Bild 2 wie in Bild 1
        sp = torch.tensor([-1.0, -1.0, 1.0], dtype=DT, device=GER)
        if bi.nr == 1:
            for x, y in (('oberarm_l', 'oberarm_r'), ('unterarm_l', 'unterarm_r'), ('hand_l', 'hand_r')):
                summe = summe + 10.0 * gew['spiegel'] * (ang[:, RG.IDX[x]] - sp * ang[:, RG.IDX[y]]).pow(2).sum(1)
        else:
            for x, y in (('oberschenkel_l', 'oberschenkel_r'), ('unterschenkel_l', 'unterschenkel_r'), ('fuss_l', 'fuss_r')):
                summe = summe + gew['spiegel'] * (ang[:, RG.IDX[x]] - sp * ang[:, RG.IDX[y]]).pow(2).sum(1)
            summe = summe + gew['spiegel'] * (ang[:, tb['sw']] - par.ang[0][:, tb['sw']]).pow(2).sum((1, 2))
        # Blickrichtung: Bild 1 von hinten, Bild 2 von vorn. Rumpf (Aktor) und in Bild 2 auch der Kopf
        g = par.gier[i]
        vor = torch.stack([torch.cos(g), torch.zeros_like(g), -torch.sin(g)], -1)
        dot = vor @ bi.vorn
        if bi.nr == 1:
            summe = summe + 10.0 * torch.relu(0.5 - dot) ** 2
        else:
            summe = summe + 10.0 * torch.relu(dot + 0.25) ** 2
            kx = s['R'][:, RG.IDX['kopf'], :, 0]
            summe = summe + 10.0 * torch.relu((kx @ bi.vorn) + 0.5) ** 2
        for nr, kn, wo, x, y, sig in opt.get('punkte', []):
            if nr == bi.nr:
                sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                summe = summe + ((s['u'][:, j] - x) ** 2 + (s['v'][:, j] - y) ** 2) / sig ** 2
        for name, wert in opt.get('zwang', {}).items():
            if name == 'tiefe%d' % bi.nr:
                z0 = ((s['T'][:, 0] - bi.auge) @ bi.vorn)
                summe = summe + ((z0 - wert) / 60.0) ** 2
            if name == 'gier%d' % bi.nr:
                w = wert * math.pi / 180.0
                d = torch.atan2(torch.sin(g - w), torch.cos(g - w))
                summe = summe + (d / math.radians(2.0)) ** 2
            if name == 'kopfx%d' % bi.nr or name == 'kopfy%d' % bi.nr:
                sel = (ger.st_kn == RG.IDX['kopf']).nonzero()[:, 0]
                c = (s['u'] if name.startswith('kopfx') else s['v'])[:, sel].mean(1)
                summe = summe + ((c - wert) / 4.0) ** 2
    # Masse, Zwaenge auf Summen
    def ls(*ks):
        return sum(lae[:, RG.L_KEYS.index(k)] for k in ks)
    for name, wert in opt.get('zwang', {}).items():
        if name == 'rumpf':
            summe = summe + ((ls('huefte', 'wirbel', 'brust') + ver[:, RG.V_KEYS.index('huefte_hinten')] - wert) / 40.0) ** 2
        if name == 'arm':
            summe = summe + ((ls('oberarm', 'unterarm') - wert) / 40.0) ** 2
        if name == 'bein':
            summe = summe + ((ls('oberschenkel', 'unterschenkel') - wert) / 40.0) ** 2
        if name == 'schwanz':
            summe = summe + ((ls('schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6') - wert) / 40.0) ** 2
    # Verhaeltnisse: sehen die Bilder nicht (beide blicken laengs), darum schwache Vorgabe. Arm zu Bein 1,1, Rumpf mindestens 1600.
    arm_l = ls('oberarm', 'unterarm')
    bein_l = ls('oberschenkel', 'unterschenkel')
    rumpf_l = ls('huefte', 'wirbel', 'brust') + ver[:, RG.V_KEYS.index('huefte_hinten')]
    summe = summe + gew['prop'] * ((torch.log(arm_l / bein_l) - math.log(1.1)) ** 2 / 0.15 ** 2 + torch.relu(1600.0 - rumpf_l) ** 2 / 150.0 ** 2)
    reg = par.l.pow(2).sum(1) + 0.3 * (par.da.pow(2).sum((1, 2)) + (par.db * par.ell).pow(2).sum((1, 2)))
    summe = summe + gew['mass'] * reg
    st_reg = (torch.log(a3[:, tb['ka'], 2]) - torch.log(a3[:, tb['kb'], 0])).pow(2).sum(1)
    sa = a3[:, tb['sw_d']]
    st_reg = st_reg + (torch.relu(sa[..., 1:] / sa[..., :-1] - 1.0) ** 2).sum((1, 2))
    st_reg = st_reg + 0.3 * (torch.log(a3[..., 0]) + torch.log(a3[..., 2]) - 2 * torch.log(a3[..., 1])).pow(2).sum(1)
    # Rumpf auch in der Breite stetig; Schwanzwurzel und Schultern haengen am Rumpf
    ih, iw, ib = RG.D_KEYS.index('huefte'), RG.D_KEYS.index('wirbel'), RG.D_KEYS.index('brust')
    st_reg = st_reg + (torch.log(b3[:, ih, 2]) - torch.log(b3[:, iw, 0])).pow(2) + (torch.log(b3[:, iw, 2]) - torch.log(b3[:, ib, 0])).pow(2)
    sy = torch.abs(ver[:, RG.V_KEYS.index('schwanz_y')])
    st_reg = st_reg + torch.relu(sy - a3[:, ih, 0] - 0.6 * a3[:, RG.D_KEYS.index('schwanz1'), 0]).pow(2) / 100.0 ** 2
    sz = ver[:, RG.V_KEYS.index('schulter_z')]
    st_reg = st_reg + torch.relu(sz - b3[:, ib, 1] - 0.6 * a3[:, RG.D_KEYS.index('oberarm'), 0]).pow(2) / 100.0 ** 2
    ga = torch.abs(torch.log(a3[..., 1:] / a3[..., :-1]))
    gb = torch.abs(torch.log(b3[..., 1:] / b3[..., :-1]))
    st_reg = st_reg + 3.0 * (torch.relu(ga - 0.45).pow(2).sum((1, 2)) + torch.relu(gb - 0.45).pow(2).sum((1, 2)))
    summe = summe + gew['stetig'] * st_reg
    sw_l = torch.log(lae[:, tb['sw_l']])
    summe = summe + gew['stetig'] * (sw_l[:, 1:] - sw_l[:, :-1]).pow(2).sum(1)
    if opt.get('anatomie'):
        summe = summe + anatomie(par, ger, st, bilder, lae, ver, a3, b3, opt, tk)
    if details:
        return summe, tk
    return summe, None


_ANA = None


def ana_tabellen(ger):
    """Stationen fuer die Anatomie-Kosten (Gegenprobe): Rumpf, Glieder ausserhalb des Rumpfes, Klauen."""
    global _ANA
    if _ANA is None:
        kn = ger.st_kn.cpu().numpy()
        s = ger.st_s.cpu().numpy()
        rumpf = np.nonzero(np.isin(kn, [RG.IDX[n] for n in ('huefte', 'wirbel', 'brust')]))[0]
        gl = []
        for i, n in enumerate(RG.NAMEN):
            k = RG.klasse(n)
            if k in ('oberschenkel', 'oberarm'):
                gl += [q for q in np.nonzero(kn == i)[0] if s[q] > 0.45]
            elif k in ('unterschenkel', 'fuss', 'unterarm', 'hand'):
                gl += list(np.nonzero(kn == i)[0])
        kl = [np.nonzero(kn == RG.IDX[n])[0][-1] for n in RG.NAMEN if RG.klasse(n) == 'klaue']
        _ANA = dict(rumpf=torch.tensor(rumpf, device=GER), glied=torch.tensor(np.array(gl), device=GER), klaue=torch.tensor(np.array(kl), device=GER),
                    kopf=(ger.st_kn == RG.IDX['kopf']).nonzero()[:, 0])
    return _ANA


def anatomie(par, ger, st, bilder, lae, ver, a3, b3, opt, tk):
    """Was kein Bild sieht: Kopf aufrecht, Glieder nicht im Rumpf, Klauen am Boden, Oberarm nicht zur Schulter duenner, Kopfrand auf Zug Q."""
    t = ana_tabellen(ger)
    qa, qb = ger.querschnitte(a3, b3)
    rg = torch.minimum(qa, qb)[:, t['glied']]
    k = torch.zeros(par.B, dtype=DT, device=GER)
    ik = RG.IDX['kopf']
    wo = torch.tensor([0.0, -1.0, 0.0], dtype=DT, device=GER)
    for i, bi in enumerate(bilder):
        s = st[i]
        ex = s['R'][:, ik, :, 0]
        up = -s['R'][:, ik, :, 1]
        w = wo[None] - ex * (ex @ wo)[:, None]
        roll = torch.relu(math.cos(math.radians(opt.get('kopf_roll', 12.0))) * w.norm(dim=1) - (up * w).sum(1))
        k = k + 200.0 * roll ** 2
        tk['%d_kopfroll' % bi.nr] = roll.detach()
        if bi.nr == 2:
            k = k + 40.0 * torch.relu((ex * bi.vorn[None]).sum(1) + math.cos(math.radians(opt.get('kopf_blick', 15.0)))) ** 2
        c = s['c'][:, t['rumpf']]
        e = [s[q][:, t['rumpf']] for q in ('e0', 'e1', 'e2')]
        p = s['c'][:, t['glied']]
        d = p[:, :, None, :] - c[:, None, :, :]
        q2 = sum(((d * x[:, None]).sum(-1) / (x * x).sum(-1)[:, None]) ** 2 for x in e)
        n = torch.sqrt(q2 + 1e-6)
        abst = (n - 1.0) * d.norm(dim=-1) / n
        pen = torch.relu(0.7 * rg[:, :, None] - abst) / 100.0
        vol = (pen ** 2).sum((1, 2))
        k = k + opt.get('gew_rumpf', 0.15) * vol
        tk['%d_glied_im_rumpf' % bi.nr] = vol.detach()
        tief = s['c'][..., 1] + s['ry']
        kl = (tief[:, t['klaue']] / 60.0) ** 2
        k = k + opt.get('gew_klaue', 0.1) * kl.sum(1)
        tk['%d_klaue' % bi.nr] = kl.sum(1).detach()
        if bi.nr == 2 and opt.get('kante'):
            # FORMBLATT Abschnitt 1.4: gemessene Kante R und Verdeckungen V1-V5 gelten weiter
            kx, ky, ks, dx, dy = opt['kante']
            sd, _ = RG.abstand(kx, ky, s['u'], s['v'], *s['q'], s['reff'])
            m = sd.min(1).values
            m = torch.where(ks[None] < 5.0, m, torch.relu(m))     # belegt: beidseitig, gelesen: nur Mindestausdehnung
            ka = ((m / ks[None]) ** 2).mean(1)
            sd, _ = RG.abstand(dx, dy, s['u'], s['v'], *s['q'], s['reff'])
            de = ((torch.relu(sd.min(1).values + 2.0) / 3.0) ** 2).mean(1)
            k = k + opt.get('gew_kante', 3.0) * ka + opt.get('gew_deck', 3.0) * de
            tk['2_kante'] = ka.detach()
            tk['2_deckung'] = de.detach()
        wb = [RG.IDX['wirbel'], RG.IDX['brust']]
        k = k + opt.get('gew_wirbel', 3.0) * ((par.ang[i][:, wb, 1] ** 2).sum(1) + (par.ang[i][:, wb, 0] ** 2).sum(1))
        if bi.nr == 2 and opt.get('kopfrand'):
            ar = opt['kopfrand']
            sel = t['kopf']
            sd, _ = RG.abstand(ar[0], ar[1], s['u'][:, sel], s['v'][:, sel], *[q[:, sel] for q in s['q']], s['reff'][:, sel])
            rd = (sd.min(1).values / 6.0) ** 2
            k = k + rd.mean(1)
            tk['2_kopfrand'] = rd.mean(1).detach()
    io = RG.D_KEYS.index('oberarm')
    k = k + (torch.relu(0.85 * a3[:, io, 2] - a3[:, io, 0]) / 40.0) ** 2
    # Kopf ohne Trichter: Ende nicht dicker als die Mitte
    ik = RG.D_KEYS.index('kopf')
    k = k + (torch.relu(a3[:, ik, 2] - a3[:, ik, 1]) / 30.0) ** 2 + (torch.relu(b3[:, ik, 2] - b3[:, ik, 1]) / 30.0) ** 2
    # Finger: Nutzer-Umriss zeigt je Finger 45 px bei Tiefe 4800, Hand in Bild 1 48 px bei Tiefe 6900. Rumpf ohne Kragen hinten, Brust nicht spitz
    ic = RG.D_KEYS.index('klaue')
    k = k + (torch.relu(190.0 - a3[:, ic, 0]) / 30.0) ** 2 + (torch.relu(130.0 - a3[:, ic, 1]) / 30.0) ** 2 + (torch.relu(60.0 - a3[:, ic, 2]) / 20.0) ** 2
    ih = RG.D_KEYS.index('hand')
    k = k + (torch.relu(380.0 - b3[:, ih, 1]) / 30.0) ** 2 + (torch.relu(380.0 - b3[:, ih, 2]) / 30.0) ** 2
    ih = RG.D_KEYS.index('huefte')
    k = k + (torch.relu(b3[:, ih, 0] - 1.02 * b3[:, ih, 1]) / 30.0) ** 2
    ib = RG.D_KEYS.index('brust')
    k = k + (torch.relu(0.62 * a3[:, ib, 0] - a3[:, ib, 2]) / 30.0) ** 2
    # Oberarm-Anteil und Kiefer: sieht kein Bild
    L = {n: lae[:, RG.L_KEYS.index(n)] for n in ('oberarm', 'unterarm', 'kopf', 'kiefer')}
    k = k + 40.0 * torch.relu(opt.get('oberarm_anteil', 0.72) - L['oberarm'] / L['unterarm']) ** 2
    kx = ver[:, RG.V_KEYS.index('kiefer_x')]
    k = k + ((kx + L['kiefer'] - 0.92 * L['kopf']) / 60.0) ** 2 + (torch.relu(250.0 - kx) / 60.0) ** 2
    return k


def startwerte(par, ger, bilder, rng, streu=0.12):
    """Zufallsstarts: Gier, Lage, Neigung, grobe Glieder. Bild 1 von hinten, Bild 2 von vorn."""
    B = par.B
    with torch.no_grad():
        k1, k2 = bilder[0].kam, bilder[1].kam
        for b in range(B):
            f1 = k1['R'][2] * [1, 0, 1]
            f1 = f1 / np.linalg.norm(f1)
            gier_sehe = math.atan2(-f1[2], f1[0])
            d1 = math.radians(rng.uniform(-35, 35))
            tiefe1 = rng.uniform(4900, 6000)
            auge, d = FK.strahl(k1, np.array([rng.uniform(290, 315), rng.uniform(225, 250)], float))
            w1 = auge + d * (tiefe1 / (d @ k1['R'][2]))
            par.wurzel[0].data[b] = torch.tensor(w1 / 1000.0, dtype=DT, device=GER)
            par.gier[0].data[b] = gier_sehe + d1
            th1 = rng.uniform(-5, 30)
            a = np.zeros((RG.N, 3))

            def zs(n, az):
                a[RG.IDX[n], 2] = math.radians(az)
            zs('huefte', th1)
            zs('oberarm_r', -th1 + rng.uniform(-15, 15)); zs('oberarm_l', -th1 + rng.uniform(-15, 15))
            zs('unterarm_r', rng.uniform(-20, 0)); zs('unterarm_l', rng.uniform(-20, 0))
            zs('hand_r', rng.uniform(0, 20)); zs('hand_l', rng.uniform(0, 20))
            al, be = rng.uniform(-60, -15), rng.uniform(30, 100)
            zs('oberschenkel_l', al - th1); zs('oberschenkel_r', al - th1)
            zs('unterschenkel_l', be); zs('unterschenkel_r', be)
            zs('fuss_l', -(al + be)); zs('fuss_r', -(al + be))
            t = [rng.uniform(0, 45)] + [rng.uniform(-30, -5) for _ in range(5)]
            for q in range(6):
                zs('schwanz%d' % (q + 1), t[q])
            par.ang[0].data[b] = torch.tensor(a, dtype=DT, device=GER)
            f2 = -k2['R'][2] * [1, 0, 1]
            f2 = f2 / np.linalg.norm(f2)
            gier_blick = math.atan2(-f2[2], f2[0])
            d2 = math.radians(rng.uniform(-65, 65))
            tiefe2 = rng.uniform(6000, 8800)
            auge, d = FK.strahl(k2, np.array([rng.uniform(280, 350), rng.uniform(225, 300)], float))
            w2 = auge + d * (tiefe2 / (d @ k2['R'][2]))
            par.wurzel[1].data[b] = torch.tensor(w2 / 1000.0, dtype=DT, device=GER)
            par.gier[1].data[b] = gier_blick + d2
            th2 = rng.uniform(-70, 25)
            a = np.zeros((RG.N, 3))
            zs('huefte', th2)
            zs('oberarm_r', -th2 - rng.uniform(10, 70)); zs('oberarm_l', -th2 - rng.uniform(10, 70))
            zs('unterarm_r', rng.uniform(-40, 0)); zs('unterarm_l', rng.uniform(-40, 0))
            zs('hand_r', rng.uniform(0, 40)); zs('hand_l', rng.uniform(0, 40))
            al, be = rng.uniform(-90, -20), rng.uniform(30, 120)
            zs('oberschenkel_l', al - th2); zs('oberschenkel_r', al - th2)
            zs('unterschenkel_l', be); zs('unterschenkel_r', be)
            zs('fuss_l', -(al + be)); zs('fuss_r', -(al + be))
            t = [rng.uniform(-10, 30)] + [rng.uniform(-25, 10) for _ in range(5)]
            for q in range(6):
                zs('schwanz%d' % (q + 1), t[q])
            par.ang[1].data[b] = torch.tensor(a, dtype=DT, device=GER)
            par.l.data[b] = torch.tensor(rng.normal(0, streu, par.l.shape[1]), dtype=DT, device=GER)
            par.da.data[b] = torch.tensor(rng.normal(0, streu * 0.8, par.da.shape[1:]), dtype=DT, device=GER)
            par.db.data[b] = torch.tensor(rng.normal(0, streu * 0.8, par.db.shape[1:]), dtype=DT, device=GER)


def vorlauf(par, ger, bilder, n=500, lr=0.03, rng=None, zittern=0.0):
    """Nur Anker, Grenzen, Boden: bringt Ketten ohne Bildrechnung in die richtige Gegend."""
    versatz = {}
    for bi in bilder:
        for q, (nm, wo, x, y) in enumerate(ANKER[bi.nr]):
            versatz[(bi.nr, q)] = torch.tensor(rng.normal(0, zittern, (par.B, 2)) if zittern > 0 else np.zeros((par.B, 2)), dtype=DT, device=GER)
    gr = [par.ang[0], par.ang[1], par.wurzel[0], par.wurzel[1], par.gier[0], par.gier[1]]
    op = torch.optim.Adam(gr, lr=lr)
    for it in range(n):
        op.zero_grad(set_to_none=True)
        lae, ver, a3, b3, off, start, ende, st = zustand(par, ger, bilder)
        k = torch.zeros(par.B, dtype=DT, device=GER)
        for i, bi in enumerate(bilder):
            s = st[i]
            for q, (nm, wo, x, y) in enumerate(ANKER[bi.nr]):
                sel = (ger.st_kn == RG.IDX[nm]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                vz = versatz[(bi.nr, q)]
                k = k + ((s['u'][:, j] - x - vz[:, 0]) ** 2 + (s['v'][:, j] - y - vz[:, 1]) ** 2) / 400.0
            ang = par.ang[i]
            k = k + 60.0 * ((torch.relu(ger.lo - ang) ** 2 + torch.relu(ang - ger.hi) ** 2) * ger.frei).sum((1, 2))
            k = k + 0.1 * (ang * ger.frei).pow(2).sum((1, 2))
            tief = s['c'][..., 1] + s['ry']
            k = k + 2.0 * torch.relu(tief + 10.0).pow(2).sum(1) / (60.0 ** 2)
        k.sum().backward()
        op.step()
    return k.detach()


def vorlauf2(par, ger, bilder, opt, n=400, lr=0.02):
    """Nur Pose 2: Bildpunkte aus --punkt und Kopfzwang, Grenzen, Boden. Bringt Glieder um, ohne die Bildkosten zu fragen."""
    op = torch.optim.Adam([par.ang[1], par.wurzel[1], par.gier[1]], lr=lr)
    a0 = par.ang[1].detach().clone()
    bi = bilder[1]
    for it in range(n):
        op.zero_grad(set_to_none=True)
        st = zustand(par, ger, bilder)[-1][1]
        k = torch.zeros(par.B, dtype=DT, device=GER)
        for nr, kn, wo, x, y, sig in opt.get('punkte', []):
            if nr == 2 and not opt.get('anker3d'):
                sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                k = k + ((st['u'][:, j] - x) ** 2 + (st['v'][:, j] - y) ** 2) / (2.0 * sig) ** 2
        for kn, wo, Pw, sig in opt.get('anker3d', []):
            sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
            j = sel[0] if wo == 'start' else sel[-1]
            k = k + ((st['c'][:, j] - Pw) ** 2).sum(1) / sig ** 2
        sel = (ger.st_kn == RG.IDX['kopf']).nonzero()[:, 0]
        z = opt.get('zwang', {})
        if 'kopfx2' in z:
            k = k + ((st['u'][:, sel].mean(1) - z['kopfx2']) / 8.0) ** 2 + ((st['v'][:, sel].mean(1) - z['kopfy2']) / 8.0) ** 2
        ang = par.ang[1]
        k = k + 60.0 * ((torch.relu(ger.lo - ang) ** 2 + torch.relu(ang - ger.hi) ** 2) * ger.frei).sum((1, 2))
        k = k + 0.3 * ((ang - a0) * ger.frei).pow(2).sum((1, 2))
        tief = st['c'][..., 1] + st['ry']
        k = k + 2.0 * torch.relu(tief + 10.0).pow(2).sum(1) / (60.0 ** 2)
        k.sum().backward()
        op.step()
    return k.detach()


def plan(p):
    """Gewichte und Stufe je Fortschritt p (0..1)."""
    g = dict(cov=0.02, exc=0.02, neg=0.5, fig=0.01, boden=1.0, kontakt=0.15, anker=0.0, grenze=40.0, ruhe=0.02, drill=0.5, schwanz=0.6,
             mass=0.15, stetig=1.0, vis_pos=0.0, vis_aus=0.0, frei=0.0, neg_s=0.0, spiegel=0.1, k1=0.3, verdeckt=0.0, prop=0.3)
    if p < 0.25:
        g['anker'] = 0.15 * (1.0 - p / 0.25)
        stufe = 0
    else:
        stufe = 1
        ra = min(1.0, (p - 0.25) / 0.2)
        g.update(vis_pos=3.0 * ra, vis_aus=2.5 * ra, frei=0.5 * ra, neg_s=1.0 * ra, verdeckt=1.0 * ra)
    return stufe, g


def speichere(par, kosten, pfad, meta):
    d = dict(zustand={k: v.detach().cpu() for k, v in par.state_dict().items()}, kosten=kosten.detach().cpu(), meta=meta, B=par.B)
    torch.save(d, pfad)


def lade(pfad, beste=None, wahl=None):
    d = torch.load(pfad, weights_only=False)
    ids = wahl if wahl is not None else (torch.argsort(d['kosten'])[:beste].tolist() if beste else list(range(d['B'])))
    par = Parameter(len(ids))
    par.load_state_dict({k: v[ids].to(GER) for k, v in d['zustand'].items()})
    return par, ids, d


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--saat', type=int, default=1)
    a.add_argument('--stapel', type=int, default=8)
    a.add_argument('--iter', type=int, default=2400)
    a.add_argument('--stride', type=int, default=3)
    a.add_argument('--aus', required=True)
    a.add_argument('--weiter')
    a.add_argument('--beste', type=int, default=0)
    a.add_argument('--wahl', default='')
    a.add_argument('--lr', type=float, default=0.02)
    a.add_argument('--hand2', default='boden')
    a.add_argument('--streu', type=float, default=0.12)
    a.add_argument('--zittern', type=float, default=0.0)
    a.add_argument('--zwang', default='')
    a.add_argument('--profil', default='', help='name=w1,w2,w3: Ausgangsstart je Wert einmal, Zwang je Stapelzeile')
    a.add_argument('--punkt', action='append', default=[], help='bild,knochen,start|ende,x,y,sigma: Bildpunkt als weiche Vorgabe')
    a.add_argument('--vorpunkte', type=int, default=0, help='Schritte Vorlauf nur Pose 2 auf die Punkte')
    a.add_argument('--anker3d', default='', help='JSON: Liste [knochen, start|ende, x, y, tiefe, sigma] fuer den Vorlauf, Bild 2')
    a.add_argument('--setze', default='', help='name=wert,...: Laengen und Versaetze setzen (Startwert)')
    a.add_argument('--setzed', default='', help='klasse=a0/a1/a2[:b0/b1/b2];...: Dicken setzen (Startwert)')
    a.add_argument('--gier2start', default='', help='Grad je Stapelzeile, wiederholt')
    a.add_argument('--anatomie', action='store_true', help='Gegenprobe: Kopf aufrecht, Glieder nicht im Rumpf, Klauen am Boden')
    a.add_argument('--kopfrand', action='store_true', help='Kopfumriss auf Zug Q (Vorgabe)')
    a.add_argument('--kante', action='store_true', help='gemessene Kante R und Verdeckungen V1-V5 aus FORMBLATT')
    a.add_argument('--gew', default='', help='name=wert,...: Gewichte der Anatomie-Kosten (gew_rumpf, gew_klaue, kopf_roll)')
    a = a.parse_args()
    ger = RG.Gerippe(GER, DT)
    grob = [Bild(1, max(4, a.stride), ger), Bild(2, max(4, a.stride), ger)]
    fein = [Bild(1, a.stride, ger), Bild(2, a.stride, ger)]
    opt = dict(hand2=a.hand2, zwang={})
    for q in a.zwang.split(','):
        if q:
            k, w = q.split('=')
            opt['zwang'][k] = float(w)
    opt['punkte'] = [(int(p.split(',')[0]), p.split(',')[1], p.split(',')[2], float(p.split(',')[3]), float(p.split(',')[4]), float(p.split(',')[5])) for p in a.punkt]
    opt['anatomie'] = a.anatomie
    for q in a.gew.split(','):
        if q:
            k, w = q.split('=')
            opt[k] = float(w)
    if a.kante:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
        r = [q for q in fb['teile_bild2'] if q['id'] == 'R'][0]
        ku = np.array([p + [4.0 if p[0] >= 354 else 7.0] for p in r['kante']], float)
        pu = []
        for q in fb['verdeckungen_bild2']:
            if q['id'] in ('V1', 'V2', 'V3', 'V4', 'V5'):
                (x0, y0), (x1, y1) = q['stelle']
                pu += [[x0 + (x1 - x0) * w, y0 + (y1 - y0) * w] for w in np.linspace(0, 1, 14)]
        pu = np.array(pu, float)
        t_ = lambda x: torch.tensor(x, dtype=DT, device=GER)
        opt['kante'] = (t_(ku[:, 0]), t_(ku[:, 1]), t_(ku[:, 2]), t_(pu[:, 0]), t_(pu[:, 1]))
    if a.kopfrand:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
        zug = np.array([[262, 225], [268, 212], [277, 206], [286, 183]] + fb['zeichnung']['zug_q']['linie'][:-1], float)
        opt['kopfrand'] = (torch.tensor(zug[:, 0], dtype=DT, device=GER), torch.tensor(zug[:, 1], dtype=DT, device=GER))
    print('Primitive', ger.K, 'Pixel', [len(b.px) for b in fein], {k: v for k, v in opt.items() if k not in ('kopfrand', 'kante')}, flush=True)
    if a.profil:
        nm, ws = a.profil.split('=')
        ws = [float(x) for x in ws.split(',')]
        par, ids, _ = lade(os.path.join(HIER, a.weiter), wahl=[int(a.wahl)] * len(ws))
        opt['zwang'][nm] = torch.tensor(ws, dtype=DT, device=GER)
        p_start = 0.55
    elif a.weiter:
        par, ids, _ = lade(os.path.join(HIER, a.weiter), a.beste or None, [int(x) for x in a.wahl.split(',')] if a.wahl else None)
        p_start = 0.5
    else:
        par = Parameter(a.stapel)
        rng = np.random.default_rng(a.saat)
        startwerte(par, ger, grob, rng, a.streu)
        k0 = vorlauf(par, ger, grob, rng=rng, zittern=a.zittern)
        print('Vorlauf', np.round(k0.cpu().numpy(), 1).tolist(), flush=True)
        p_start = 0.0
    if a.setze:
        with torch.no_grad():
            for q in a.setze.split(','):
                k, w = q.split('=')
                if k in RG.L_KEYS:
                    par.l.data[:, RG.L_KEYS.index(k)] = math.log(float(w) / RG.LAENGE0[k])
                else:
                    j = RG.V_KEYS.index(k)
                    par.v.data[:, j] = (float(w) - float(par.V0[j])) / float(par.Vska[j])
    if a.setzed:
        with torch.no_grad():
            for q in a.setzed.split(';'):
                k, w = q.split('=')
                j = RG.D_KEYS.index(k)
                t3 = w.split(':')
                va = torch.tensor([float(x) for x in t3[0].split('/')], dtype=DT, device=GER)
                par.da.data[:, j] = torch.log(va / par.A0[j])
                if len(t3) > 1:
                    vb = torch.tensor([float(x) for x in t3[1].split('/')], dtype=DT, device=GER)
                    par.db.data[:, j] = torch.log(vb / par.B0[j])
    if a.anker3d:
        kam = grob[1].kam
        liste = []
        for kn, wo, x, y, d, sig in json.load(open(a.anker3d, encoding='utf-8')):
            auge, r = FK.strahl(kam, np.array([x, y], float))
            Pw = auge + r * (d / float(r @ kam['R'][2]))
            liste.append((kn, wo, torch.tensor(Pw, dtype=DT, device=GER)[None], float(sig)))
        opt['anker3d'] = liste
    if a.gier2start:
        ws = [float(x) for x in a.gier2start.split(',')]
        for b in range(par.B):
            par.gier[1].data[b] = math.radians(ws[b % len(ws)])
    if a.vorpunkte:
        print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
        opt.pop('anker3d', None)
    gruppen = [dict(params=list(par.ang.parameters()), lr=a.lr),
               dict(params=list(par.gier.parameters()) + list(par.wurzel.parameters()), lr=a.lr),
               dict(params=[par.l, par.v], lr=a.lr * 0.5),
               dict(params=[par.da, par.db], lr=a.lr * 0.5)]
    optim = torch.optim.Adam(gruppen)
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(optim, a.iter, eta_min=a.lr * 0.05)
    t0 = time.time()
    for it in range(a.iter):
        p = p_start + (1.0 - p_start) * it / a.iter
        stufe, g = plan(p)
        optim.zero_grad(set_to_none=True)
        tau = max(0.75, 2.5 * (1.0 - p)) * a.stride / 2.0 + 0.5
        kost, _ = verlust(par, ger, grob if stufe == 0 else fein, stufe, g, tau=tau, opt=opt)
        kost.sum().backward()
        optim.step()
        sched.step()
        if it % 300 == 0 or it == a.iter - 1:
            print(it, 'Stufe', stufe, 'Kosten', np.round(kost.detach().cpu().numpy(), 2).tolist(), '%.0f s' % (time.time() - t0),
                  'GPU %.1f GB' % (torch.cuda.max_memory_allocated() / 1e9), flush=True)
    stufe, g = plan(1.0)
    with torch.no_grad():
        kost, det = verlust(par, ger, fein, 1, g, details=True, opt=opt)
    daten = sum(v for k, v in det.items() if k.endswith('_vpos')) + 0.5 * sum(v for k, v in det.items() if k.endswith('_vaus')) + sum(v for k, v in det.items() if k.endswith('_neg'))
    opt2 = dict(hand2=opt['hand2'], punkte=opt['punkte'], zwang={k: (v.cpu().numpy().tolist() if torch.is_tensor(v) else v) for k, v in opt['zwang'].items()},
                anatomie=bool(opt.get('anatomie')), kopfrand=bool(opt.get('kopfrand')), gew=a.gew)
    speichere(par, kost, os.path.join(HIER, a.aus), dict(saat=a.saat, stride=a.stride, iter=a.iter, opt=opt2, daten=daten.cpu().numpy().tolist(), details={k: v.cpu().numpy().tolist() for k, v in det.items()}))
    print('fertig', np.round(kost.cpu().numpy(), 2).tolist())
    print('daten', np.round(daten.cpu().numpy(), 3).tolist())


if __name__ == '__main__':
    main()
