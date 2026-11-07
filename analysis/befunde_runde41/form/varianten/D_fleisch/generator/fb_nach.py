#!/usr/bin/env python3
"""Form bemalt, Nachpassung der Glattform an die Bilder: nach dem Glaetten (fb_glatt) und den Formeingriffen (fb_form) liegt der Umriss einige Pixel anders.
Freie Groessen: je Teil und Ausgangsring/Ecke ein log-Faktor delta[T, n] (radial um die Ringmitte), uebertragen auf die Glattecken ueber fb_glatt.delta_karte.
Bindung wie im Rohbau: Mitte spiegelgleich, links = Spiegel von rechts. Posen und Bodenhub aus der gebauten FORM.EMD (form.json).
Ziel: Umriss gegen ziel2_bild*.png (weiss decken, schwarz frei), Zonen der Passung (band_ein, band_aus, o1, pflicht), Boden L1, L2, SCH, BO, FUGE frei, Kerbe E3 frei.
Regel: kleine, glatte Faktoren. python fb_nach.py STAND.json [--schritte 250] [--aus zw/delta.json] [--start zw/delta.json]. Kein RE-Wert."""
import argparse
import json
import math
import os

import cv2
import numpy as np
import torch

from rf_kern import FORM, GERAET, HIER, Kamera, weich
import rf_plan
import rf_bau
import fb_glatt
import fb_form
import form_pruefung as P

BOX = {1: (175, 170, 445, 480), 2: (130, 130, 480, 480)}
# Weiche Projektion (form_kamera, Gleitkomma) gegen den Pruefrender (form_buehne, GTE ganzzahlig, Phase): gemessen mit zw/dbg_nach.py, bester Versatz je Bild,
# Abweichung 1130 -> 606 px (Bild 1), 1520 -> 808 px (Bild 2)
VERSATZ = {1: (-0.75, -0.75), 2: (-1.0, -0.75)}
REG = 1.0


def posen(form_json):
    k = P.Kandidat(form_json)
    aus = {}
    for n in (1, 2):
        lage, gier, clip, bild = k.stand(n)
        kf = k.kf(clip, bild)
        R, T = k.mo.pose(kf)
        a = gier * 2 * math.pi / 4096
        Y = np.array([[math.cos(a), 0, math.sin(a)], [0, 1, 0], [-math.sin(a), 0, math.cos(a)]])
        Rw = np.stack([Y @ (r.astype(float) / 4096.0) for r in R])
        Tw = np.stack([Y @ t.astype(float) + np.asarray(lage, float) for t in T])
        aus[n] = (Rw, Tw)
    return aus


def ziele():
    """-> je Bild dict mit Bool-Masken im Ausschnitt: weiss, schwarz, ein, aus."""
    import rf_fit
    g, teile = rf_plan.plan()
    pa = rf_fit.Passung.__new__(rf_fit.Passung)
    pa.box = BOX
    pa.g = g
    from rf_kern import namen
    pa.ix = namen(g)
    pa.dev = GERAET
    pa._masken_laden()
    out = {}
    for n in (1, 2):
        x0, y0, x1, y1 = BOX[n]
        z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)[y0:y1, x0:x1]
        d = pa.ziele[n]
        logo = d['logo']
        weiss = (z == 255) & ~logo
        erlaubt = np.zeros_like(weiss)
        for k_ in ('kerbe', 'spalt', 'dunkel', 'o1'):
            if k_ in d:
                erlaubt |= d[k_]
        schwarz = (z == 0) & ~logo & ~erlaubt
        ein = d.get('band_ein', np.zeros_like(weiss)).copy()
        aus = d.get('band_aus', np.zeros_like(weiss)).copy()
        if n == 1:
            ein |= d['o1']
            for i in ('L1', 'L2', 'SCH'):
                zz = P.poly_maske(P.zone_b1(i))[y0:y1, x0:x1] & (cv2.imread(os.path.join(FORM, 'ziel2_bild1.png'), 0)[y0:y1, x0:x1] == 0)
                aus |= zz
        else:
            ein |= d['pflicht']
            J = P.formblatt()
            (a0, b0, a1, b1), (c0, e0, c1, e1) = J['pruefpunkte']['bild2']['boden_frei']
            q = np.zeros((480, 640), bool)
            q[b0:b1 + 1, a0:a1 + 1] = True
            q[e0:e1 + 1, c0:c1 + 1] = True
            aus |= q[y0:y1, x0:x1]
        # Pruefpunkte, die nach dem Glaetten knapp lagen (g3/g4): Bild 1 b1.nl_y342_links (226,342), Bild 2 V6 Kante (411,206)
        q = np.zeros((480, 640), bool)
        qe = np.zeros((480, 640), bool)
        qs = np.zeros((480, 640), bool)
        if n == 1:
            q[337:348, 212:224] = True
            qs[337:348, 281:287] = True          # b1.nl_y342_rechts: Kante des Schwanzes (nicht des Umrisses) bei x 287, der Schwanz muss bis dort reichen (g13: Kante 281,5)
        else:
            for x in range(402, 421):
                yl = 196 + (x - 398) * 9 / 13 if x <= 411 else 205 + (x - 411) * 21 / 13
                q[int(round(yl)) - 9:int(round(yl)) - 2, x] = True
                qe[int(round(yl)) + 1:int(round(yl)) + 6, x] = True
        out_scharf = q[y0:y1, x0:x1] & ~ein
        aus |= out_scharf
        ein_s = qe[y0:y1, x0:x1]
        out[n] = dict(weiss=weiss, schwarz=schwarz, ein=ein & ~aus, aus=aus, scharf=out_scharf, ein_scharf=ein_s, schwanz_ein=qs[y0:y1, x0:x1])
    return out


class Nach:
    def __init__(self, stand, form_json, start=None):
        g, teile = rf_plan.plan()
        self.g, self.teile = g, teile
        L, C = rf_bau.wirksam(teile, stand['lr'], stand['c'])
        rk = [list(fb_form.klaue_links(p, *fb_form.oberarm(p, *fb_form.schwanz_rund(p, *fb_form.a1_wurzel(p, *rf_bau.teil_ecken(p, L[i], C[i])))))) for i, p in enumerate(teile)]
        rk = fb_form.schwanz_profil(teile, rk, g)
        rk = fb_form.ketten(teile, rk, g)
        dev = GERAET
        self.dev = dev
        self.meister = {}
        self.teil = []
        V, kn, tris = [], [], []
        off = 0
        for i, p in enumerate(teile):
            G = fb_glatt.glatt_geometrie(p, *rk[i])
            base = G['pts']
            versch = fb_form.schulter(p.name, base, G['m'], G['NR']) - base
            A = fb_glatt.delta_karte(G)
            NR, m = G['NR'], G['m']
            ringidx = np.concatenate([np.repeat(np.arange(NR), m), [-1, -1]])
            meister = teile[p.bind].name if isinstance(p.bind, int) else p.name
            if meister == p.name:
                self.meister[p.name] = dict(T=G['T'], n=G['n'], sym=(p.bind == 'sym'))
            self.teil.append(dict(name=p.name, meister=meister, base=torch.tensor(base, dtype=torch.float32, device=dev), versch=torch.tensor(versch, dtype=torch.float32, device=dev),
                                  A=torch.tensor(A, dtype=torch.float32, device=dev), NR=NR, m=m, knochen=p.knochen, off=off))
            V.append(base)
            kn += [p.knochen] * len(base)
            for f in G['quads']:
                tris += [[off + f[0], off + f[1], off + f[2]], [off + f[0], off + f[2], off + f[3]]]
            for f in G['tris']:
                tris.append([off + v for v in f])
            off += len(base)
        self.kn = torch.tensor(kn, device=dev)
        self.tris = torch.tensor(tris, dtype=torch.long, device=dev)
        tk = []
        for t in self.teil:
            pass
        kanal = np.zeros(len(tris), np.int64)
        knt = np.array(kn)
        tri_kn = knt[np.array(tris)[:, 0]]
        kanal[np.isin(tri_kn, [1, 2, 3, 4, 5, 6])] = 1        # Schwanz eigener Kanal (Kante gegen das rechte Bein)
        self.kanal = torch.tensor(kanal, device=dev)
        self.delta = {}
        st = json.load(open(start)) if start and os.path.exists(start) else {}
        for nm, mi in self.meister.items():
            d0 = np.array(st[nm], float) if nm in st and np.array(st[nm]).shape == (mi['T'], mi['n']) else np.zeros((mi['T'], mi['n']))
            self.delta[nm] = torch.tensor(d0, dtype=torch.float32, device=dev, requires_grad=True)
        self.pose = {n: tuple(torch.tensor(x, dtype=torch.float32, device=dev) for x in v) for n, v in posen(form_json).items()}
        self.kam = {n: Kamera(n) for n in (1, 2)}
        self.z = {n: {k_: torch.tensor(v.astype(np.float32), device=dev) for k_, v in d.items()} for n, d in ziele().items()}
        with torch.no_grad():
            v = self.ecken()
            self.y0 = {}
            for n in (1, 2):
                R, T = self.pose[n]
                w = (R[self.kn] @ v[..., None])[..., 0] + T[self.kn]
                self.y0[n] = float(torch.topk(w[:, 1], 8).values.mean())

    def wirksam(self, nm):
        d = self.delta[nm]
        if self.meister[nm]['sym']:
            n = d.shape[1]
            sp = [(-j) % n for j in range(n)]
            d = 0.5 * (d + d[:, sp])
        return d

    def ecken(self):
        aus = []
        for t in self.teil:
            d = self.wirksam(t['meister'])
            f = torch.exp(t['A'] @ d.reshape(-1))
            NR, m = t['NR'], t['m']
            R = t['base'][:NR * m].reshape(NR, m, 3)
            c = R.mean(1, keepdims=True)
            R = c + (R - c) * f.reshape(NR, m, 1)
            v = torch.cat([R.reshape(-1, 3), t['base'][NR * m:]]) + t['versch']
            aus.append(v)
        return torch.cat(aus)

    def verlust(self, sigma=1.0, messen=False):
        v = self.ecken()
        L = 0.0
        ber = {}
        for n in (1, 2):
            R, T = self.pose[n]
            w = (R[self.kn] @ v[..., None])[..., 0] + T[self.kn]
            xy = self.kam[n](w) + torch.tensor(VERSATZ[n], device=w.device, dtype=w.dtype)
            x0, y0, x1, y1 = BOX[n]
            S = weich(xy, self.tris, self.kanal, y1 - y0, x1 - x0, sigma, 1, x0, y0, C=2)
            U = 1 - (1 - S[0]) * (1 - S[1])
            z = self.z[n]
            nw = z['weiss'].sum()
            fehlt = (z['weiss'] * (1 - U)).sum()
            schw = (z['schwarz'] * U).sum()
            ein = (z['ein'] * (1 - U)).sum()
            aus = (z['aus'] * U).sum()
            scharf = (z['scharf'] * U).sum() + (z['ein_scharf'] * (1 - U)).sum() + (z['schwanz_ein'] * (1 - S[1])).sum()
            L = L + (fehlt + 1.2 * schw + 3 * ein + 4 * aus + 20 * scharf) / nw
            yb = torch.topk(w[:, 1], 8).values.mean()
            L = L + ((yb - self.y0[n]) / 30.0) ** 2 * 0.02
            if messen:
                Uh = (U > .5)
                ber[n] = dict(iou=float((Uh & (z['weiss'] > 0)).sum() / ((Uh & ((z['weiss'] + z['schwarz']) > 0)) | (z['weiss'] > 0)).sum()), fehlt=int(((~Uh) & (z['weiss'] > 0)).sum()),
                              schwarz=int((Uh & (z['schwarz'] > 0)).sum()), ein=int(((~Uh) & (z['ein'] > 0)).sum()), aus=int((Uh & (z['aus'] > 0)).sum()), scharf=int((Uh & (z['scharf'] > 0)).sum() + ((~Uh) & (z['ein_scharf'] > 0)).sum() + ((S[1] <= .5) & (z['schwanz_ein'] > 0)).sum()), boden=float(yb - self.y0[n]))
        reg = 0.0
        for nm in self.meister:
            d = self.wirksam(nm)
            reg = reg + 1.5 * (d * d).mean()
            if d.shape[1] >= 3:
                dd = torch.roll(d, 1, 1) + torch.roll(d, -1, 1) - 2 * d
                reg = reg + 3.0 * (dd * dd).mean()
            if d.shape[0] >= 3:
                d2 = d[2:] - 2 * d[1:-1] + d[:-2]
                reg = reg + 3.0 * (d2 * d2).mean()
        reg = reg * REG
        return L + reg, ber, float(reg)

    def lauf(self, schritte, lr=0.004):
        opt = torch.optim.Adam(list(self.delta.values()), lr=lr)
        for i in range(schritte):
            opt.zero_grad()
            L, ber, reg = self.verlust(messen=(i % 50 == 0 or i == schritte - 1))
            L.backward()
            opt.step()
            if ber:
                print('%4d L %.4f reg %.4f %s' % (i, float(L), reg, ' | '.join('B%d IoU %.4f fehlt %d schwarz %d ein %d aus %d scharf %d boden %+.0f' % (n, b['iou'], b['fehlt'], b['schwarz'], b['ein'], b['aus'], b['scharf'], b['boden']) for n, b in ber.items())), flush=True)

    def speichern(self, pfad):
        out = {nm: self.wirksam(nm).detach().cpu().numpy().round(4).tolist() for nm in self.meister}
        json.dump(out, open(pfad, 'w'))
        mx = max(float(np.abs(np.array(v)).max()) for v in out.values())
        print('delta max %.3f (Faktor %.3f) ->' % (mx, math.exp(mx)), pfad)


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('stand')
    a.add_argument('--form', default=os.path.join(HIER, 'form.json'))
    a.add_argument('--schritte', type=int, default=250)
    a.add_argument('--aus', default=os.path.join(HIER, 'zw', 'delta.json'))
    a.add_argument('--start', default=None)
    a.add_argument('--reg', type=float, default=1.0)
    a = a.parse_args()
    REG = a.reg
    nach = Nach(json.load(open(os.path.join(HIER, a.stand))), a.form, a.start)
    nach.lauf(a.schritte)
    nach.speichern(a.aus)
