#!/usr/bin/env python3
"""Rohbau C, Passung: Radien und Mitten der Ringe (und ausgewaehlte Gelenke) an die Nutzer-Masken, beide Bilder zugleich, Verlust aus weicher Deckung.
Kein Metaball, kein Voxel: Parameter sind nur Ringmitten, Radien je Ecke und Winkel."""
import argparse
import json
import os
import time

import cv2
import numpy as np
import torch

from rc_kern import GERAET, TEILE, Kamera, Kette, Netz, masken, namen, weich, ziel, HIER
import rc_plan

FINGER = ['klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
GRUPPEN = {1: dict(tail=['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6'], left_leg=['oberschenkel_l', 'unterschenkel_l', 'fuss_l'],
                   right_leg=['oberschenkel_r', 'unterschenkel_r', 'fuss_r'], right_arm=['oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'],
                   part_of_Body=['huefte', 'wirbel', 'brust'], versteckt=['oberarm_l', 'unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4', 'hals', 'kopf', 'kiefer']),
           2: dict(face_and_upper_body=['huefte', 'wirbel', 'brust', 'hals', 'oberarm_l'], kopf=['kopf', 'kiefer'], left_arm_front=['unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'],
                   right_arm_front=['unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'], beine=['oberschenkel_l', 'unterschenkel_l', 'fuss_l', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r'],
                   versteckt=['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6', 'oberarm_r'])}
# Wer steht vor wem: erlaubt ist die eigene Maske plus die Masken der Teile davor.
DAVOR = {1: dict(tail=[], left_leg=['tail'], right_leg=['tail'], right_arm=[], part_of_Body=['tail', 'right_leg', 'right_arm']),
         2: dict(face_and_upper_body=[], left_arm_front=['face_and_upper_body'], right_arm_front=['face_and_upper_body'])}
UEBERLAPP = {1: [('part_of_Body', 'right_leg'), ('tail', 'right_leg'), ('tail', 'left_leg')], 2: []}
# In Bild 2 fuellen Beine und rechter Oberarm die dunkle Masse unter dem Rumpf: Sie zaehlen als Deckung der Maske, nicht als Ueberstand.
MIT = {1: {}, 2: dict(face_and_upper_body=['kopf', 'beine', 'versteckt'])}
# Kopf und Kiefer in Bild 2: Rand auf Zug Q (Kreis um (325,245), Radius 62, FORMBLATT2 6), unten bis zur Kante R, damit der Kiefer vor dem Ellbogen steht. Vorgabe [v] aus Bildmerkmalen.
# V3: Bodenlinie ohne Kontrast (340-364,338), dort muss etwas Dunkles stehen (pruefung.md O2), im Gerippe der linke Fuss.
V3 = [(338, 330), (366, 330), (366, 345), (338, 345)]
# Dunkle Keile zwischen den Teilen, in keinem Teil, im Bild dunkel (pruefung.md O3; Spalt Schwanz/rechtes Bein unter der Raute). Beine duerfen hinein, schwache Deckung.
KEILE = {'left_leg': [(250, 362), (254, 372), (254, 388), (243, 398), (234, 392), (236, 378)], 'right_leg': [(283, 368), (300, 360), (303, 380), (296, 410), (291, 432), (283, 432)]}
KEIL_GEW = dict(left_leg=0.02, right_leg=0.08)
# Klauenspitzen: Spitzen des Nutzer-Umrisses (pruefung/teile/selbstprobe.txt), Finger 1 innen, 2 Mitte, 3 aussen, 4 Daumen. Rechts unten unter dem Logo gelesen.
SPITZEN = {1: dict(klaue_r3=(425, 363)), 2: dict(klaue_r1=(222, 475), klaue_r2=(197, 476), klaue_r3=(145, 353), klaue_r4=(274, 443), klaue_l1=(461, 392), klaue_l2=(468, 417), klaue_l4=(439, 418))}
# Stichloecher in der dunklen Masse von Bild 2, gemessen im Lauf d12: hier scheint der Raum durch die Figur.
LOECHER = [(254, 295), (275, 302), (356, 297)]
GESICHT = [(263, 245), (272, 215), (295, 190), (325, 183), (355, 190), (378, 215), (387, 245), (380, 272), (366, 292), (340, 306), (312, 328), (292, 322), (272, 288)]


class Passung:
    def __init__(self, g, teile, gelenke=()):
        self.g, self.teile = g, teile
        self.netz = Netz(teile)
        self.ix = namen(g)
        self.kette = Kette(g)
        self.dev = GERAET
        lr, c = self.netz.startwerte()
        self.lr0 = [x.clone() for x in lr]
        self.lr = [x.clone().requires_grad_(True) for x in lr]
        self.c = [x.clone().requires_grad_(True) for x in c]
        self.c0 = [x.clone() for x in c]
        self.versatz = torch.tensor([k['versatz'] for k in g['knochen']], dtype=torch.float32, device=self.dev)
        self.pose = {}
        for n in (1, 2):
            p = g['posen']['bild%d' % n]
            w = torch.tensor(p['winkel'], dtype=torch.float32, device=self.dev)
            self.pose[n] = dict(w0=w, dw=torch.zeros_like(w, requires_grad=True), wurzel=torch.tensor(p['wurzel'], dtype=torch.float32, device=self.dev),
                                gier=float(p['gier']), lage=torch.tensor(p['lage'], dtype=torch.float32, device=self.dev))
        self.gelenk_maske = {n: torch.zeros((len(g['knochen']), 3), device=self.dev) for n in (1, 2)}
        self.pose_gew = {n: torch.ones((len(g['knochen']), 1), device=self.dev) for n in (1, 2)}
        for (n, b, stark) in gelenke:
            self.gelenk_maske[n][self.ix[b]] = stark
        self.kam = {n: Kamera(n) for n in (1, 2)}
        m, logo = masken()
        self.box = {1: (175, 170, 445, 480), 2: (130, 130, 480, 480)}
        self.ziele = {}
        for n in (1, 2):
            x0, y0, x1, y1 = self.box[n]
            z = ziel(n)
            d = {}
            al = np.zeros_like(logo)
            for t in TEILE[n]:
                al |= m[t]
            d['logo'] = logo[y0:y1, x0:x1]
            d['alle'] = al[y0:y1, x0:x1]
            d['schwarz'] = (z == 0)[y0:y1, x0:x1]
            if n == 1:
                for nm, poly in KEILE.items():
                    q = np.zeros(logo.shape, np.uint8)
                    cv2.fillPoly(q, [np.array(poly, np.int32)], 1)
                    d['keil_' + nm] = q[y0:y1, x0:x1].astype(bool)
            if n == 2:
                q = np.zeros(logo.shape, np.uint8)
                cv2.fillPoly(q, [np.array(GESICHT, np.int32)], 1)
                d['gesicht'] = q[y0:y1, x0:x1].astype(bool)
                q = np.zeros(logo.shape, np.uint8)
                cv2.fillPoly(q, [np.array(V3, np.int32)], 1)
                d['v3'] = q[y0:y1, x0:x1].astype(bool)
                q = np.zeros(logo.shape, np.uint8)
                for p in LOECHER:
                    cv2.circle(q, p, 7, 1, -1)
                cv2.rectangle(q, (404, 226), (415, 262), 1, -1)     # Rand rechts bis x=413 bei y 225-262: Rohr verdeckt (FORMBLATT 1.4)
                d['loch'] = q[y0:y1, x0:x1].astype(bool)
            for t in TEILE[n]:
                d[t] = m[t][y0:y1, x0:x1]
            self.ziele[n] = d
        self.kanal = {}
        for n in (1, 2):
            kn = -np.ones(len(g['knochen']), int)
            nam = list(GRUPPEN[n].keys())
            for i, t in enumerate(nam):
                for b in GRUPPEN[n][t]:
                    kn[self.ix[b]] = i
            self.kanal[n] = (nam, torch.tensor(kn, device=self.dev))
        self.gewicht = dict(rund=3.0, laengs=3.0, gestalt=0.05, mitte=0.1, naht=1.0, sym=1.0, spiegel=0.2, boden=1.0, spill=1.0, cover=1.0, grau=0.25, pose=1.0, gesicht=1.0, keil=0.12)
        idx = {}
        for i, p in enumerate(teile):
            idx.setdefault(p.name, i)
        self.idx = idx
        self.naehte = rc_plan.naehte(idx)
        self.symm = [idx[n] for n in rc_plan.SYMM]
        self.spiegel = rc_plan.spiegelpaare(idx)
        self.entwurf = [rc_plan.ENTWURF.get(p.name, 0.0) + 1.0 for p in teile]
        self.rund_f = [4.0 if p.name == 'hand_r' else 0.2 if p.name == 'unterarm_r' else 1.0 for p in teile]
        self.spitzen = [self.ix[b] for b in FINGER]
        self.spitzen0 = {}
        self.kappe = {}
        o = 0
        for i, p in enumerate(teile):
            self.kappe[p.name] = o + p.T * p.n + 1
            o += p.T * p.n + 2
        # Ringmitten an den Gelenken festhalten: Lage der Glieder kommt aus den Winkeln, nicht aus wandernden Ringen.
        self.pin = []
        for p in teile:
            L = p.t[-1] - p.t[0]
            nah = np.minimum(np.abs(p.t), np.abs(p.t - g['knochen'][p.knochen]['laenge']))
            w = 1.0 + 40.0 * np.exp(-(nah / 120.0) ** 2)
            self.pin.append(torch.tensor(w[:, None], dtype=torch.float32, device=self.dev))

    def ecken(self):
        return self.netz.ecken(self.lr, self.c)

    def welt(self, n, v):
        p = self.pose[n]
        R, T = self.kette(p['w0'] + p['dw'] * self.gelenk_maske[n], self.versatz, p['wurzel'], p['gier'], p['lage'])
        return self.netz.welt(v, R, T), R, T

    def bildverlust(self, n, v, schritt, sigma):
        w, R, T = self.welt(n, v)
        xy = self.kam[n](w)
        x0, y0, x1, y1 = self.box[n]
        W, H = (x1 - x0) // schritt, (y1 - y0) // schritt
        nam, kn = self.kanal[n]
        ka = kn[self.netz.tkn]
        sel = ka >= 0
        S = weich(xy, self.netz.tris[sel], ka[sel], H, W, sigma, schritt, x0, y0)
        d = self._ziele_raster(n, schritt)
        verl, aus = 0.0, {}
        idx = {t: i for i, t in enumerate(nam)}
        for t in TEILE[n]:
            M = d[t]
            Si = S[idx[t]]
            cov = Si
            for a, b in UEBERLAPP[n]:
                if t in (a, b):
                    o = a if t == b else b
                    cov = cov + d[a] * d[b] * S[idx[o]] * (1 - cov)
            for o in MIT[n].get(t, ()):
                cov = cov + S[idx[o]] * (1 - cov)
            fehlt = (M * (1 - cov) * d['frei']).sum() / (M * d['frei']).sum().clamp(min=1)
            erlaubt = M
            for o in DAVOR[n][t]:
                erlaubt = torch.maximum(erlaubt, d[o])
            if n == 1 and t in KEILE:
                Z = d['keil_' + t]
                erlaubt = torch.maximum(erlaubt, Z)
                verl = verl + KEIL_GEW[t] * (Z * (1 - Si)).sum() / Z.sum()
            ueber = Si * (1 - erlaubt) * d['frei']
            grau = dict(part_of_Body=1.0, left_leg=0.7).get(t, self.gewicht['grau'])
            schw = 1.5 if n == 1 else 1.0
            if t == 'right_arm_front':
                grau = 0.7          # Streifen des Nutzers ist schmal: Der Unterarm wird im Querschnitt flach (Bild 1 sieht ihn breit, Bild 2 schmal), statt seitlich aus dem Umriss zu laufen.
            spill = schw * (ueber * d['schwarz']).sum() + grau * (ueber * (1 - d['schwarz'])).sum()
            if n == 2 and t in ('right_arm_front', 'left_arm_front'):
                # Der Arm kommt unter der Rumpfkante hervor: Nur sein oberes Ende steht hinter dem Kiefer, sonst zaehlt er sichtbar. Halbes Gewicht.
                spill = spill + 0.8 * (Si * d['face_and_upper_body'] * (1 - M) * d['frei']).sum()
            spill = spill / (M * d['frei']).sum().clamp(min=1)
            verl = verl + self.gewicht['cover'] * fehlt + self.gewicht['spill'] * spill
            if t == 'face_and_upper_body':
                verl = verl + 0.2 * (Si * d['left_arm_front'] * (1 - M)).sum() / d['left_arm_front'].sum()
            aus[t] = (float(fehlt), float(spill))
        for t in nam:
            if t in TEILE[n]:
                continue
            ueber = S[idx[t]] * (1 - d['alle']) * d['frei']
            verl = verl + 0.5 * ((ueber * d['schwarz']).sum() + 0.3 * (ueber * (1 - d['schwarz'])).sum()) / 2500.0 * (schritt * schritt / 4.0)
        if n == 2:
            F = d['gesicht']
            Sk = S[idx['kopf']]
            ff = (F * (1 - Sk)).sum() / F.sum()
            uu = (Sk * (1 - F)).sum() / F.sum()
            verl = verl + self.gewicht['gesicht'] * (ff + uu)
            aus['kopf'] = (float(ff), float(uu))
            V = d['v3']
            vv = (V * (1 - S[idx['beine']])).sum() / V.sum()
            verl = verl + 0.3 * vv
            Lo = d['loch']
            U = 1 - (1 - S[idx['face_and_upper_body']]) * (1 - Sk) * (1 - S[idx['beine']]) * (1 - S[idx['versteckt']])
            verl = verl + 0.5 * (Lo * (1 - U)).sum() / Lo.sum()
        b = torch.relu(w[:, 1] + 5.0)
        verl = verl + self.gewicht['boden'] * (b / 100.0).pow(2).mean() * 50
        h = []
        for bn in self.spitzen:
            h.append(T[bn] + R[bn][:, 0] * 700.0)
        h = torch.stack(h)[:, 1]
        if n not in self.spitzen0:
            self.spitzen0[n] = h.detach().clone()
        verl = verl + 0.3 * ((h - self.spitzen0[n]) / 60.0).pow(2).mean()
        for nm, z in SPITZEN[n].items():
            q = xy[self.kappe[nm]]
            verl = verl + 0.02 * ((q[0] - z[0]) ** 2 + (q[1] - z[1]) ** 2) / 25.0
        return verl, aus

    def _ziele_raster(self, n, schritt):
        k = (n, schritt)
        if not hasattr(self, '_zr'):
            self._zr = {}
        if k in self._zr:
            return self._zr[k]
        out = {}
        for nm, a in self.ziele[n].items():
            x = a.astype(np.float32)
            h, w = x.shape[0] // schritt, x.shape[1] // schritt
            q = cv2.resize(x[:h * schritt, :w * schritt], (w, h), interpolation=cv2.INTER_AREA)
            out[nm] = torch.tensor(q, device=self.dev)
        out['frei'] = 1 - out['logo']
        self._zr[k] = out
        return out

    def regel(self):
        rund, laengs, gest, mit = 0.0, 0.0, 0.0, 0.0
        for i, (p, l, l0, c, c0) in enumerate(zip(self.teile, self.lr, self.lr0, self.c, self.c0)):
            d = l - l0
            if p.n >= 3:
                dd = torch.roll(d, 1, 1) + torch.roll(d, -1, 1) - 2 * d
                rund = rund + self.rund_f[i] * (dd * dd).mean()
            if p.T >= 3:
                dt = np.diff(p.t)
                h0 = torch.tensor(dt[:-1], dtype=torch.float32, device=self.dev)[:, None]
                h1 = torch.tensor(dt[1:], dtype=torch.float32, device=self.dev)[:, None]
                d2 = ((d[2:] - d[1:-1]) / h1 - (d[1:-1] - d[:-2]) / h0) * (0.5 * (h0 + h1))
                laengs = laengs + (d2 * d2).mean()
            gest = gest + self.entwurf[i] * (d * d).mean()
            dc = (c - c0) / 400.0
            mit = mit + (dc.pow(2) * self.pin[i]).mean() * 0.2
            if p.T >= 3:
                d2c = dc[2:] - 2 * dc[1:-1] + dc[:-2]
                mit = mit + 8 * d2c.pow(2).mean()
        naht = 0.0
        for a, ra, b, rb in self.naehte:
            if self.teile[a].n == self.teile[b].n:
                naht = naht + (self.lr[a][ra] - self.lr[b][rb]).pow(2).mean()
        sym = 0.0
        for i in self.symm:
            d = self.lr[i]
            n = self.teile[i].n
            sym = sym + (d - d[:, [(-j) % n for j in range(n)]]).pow(2).mean()
        sp = 0.0
        for a, b in self.spiegel:
            sp = sp + (self.lr[a] - self.lr[b]).pow(2).mean()
        return rund, laengs, gest, mit, naht, sym, sp

    def posestrafe(self):
        s = 0.0
        for n in (1, 2):
            s = s + ((self.pose[n]['dw'] * self.gelenk_maske[n] * self.pose_gew[n]) / 100.0).pow(2).sum()
        return s

    def verlust(self, schritt, sigma):
        v = self.ecken()
        L = 0.0
        ber = {}
        for n in (1, 2):
            l, a = self.bildverlust(n, v, schritt, sigma)
            L = L + l
            ber[n] = a
        rund, laengs, gest, mit, naht, sym, sp = self.regel()
        G = self.gewicht
        L = L + G['rund'] * rund + G['laengs'] * laengs + G['gestalt'] * gest + G['mitte'] * mit + G['naht'] * naht + G['sym'] * sym + G['spiegel'] * sp + G['pose'] * 0.01 * self.posestrafe()
        return L, ber, (float(rund), float(laengs), float(gest), float(mit), float(naht), float(sym), float(sp))

    def lauf(self, schritte, schritt, sigma, lr=0.01, lr_c=4.0, lr_w=3.0, zeige=50):
        opt = torch.optim.Adam([dict(params=self.lr, lr=lr), dict(params=self.c, lr=lr_c),
                                dict(params=[self.pose[n]['dw'] for n in (1, 2)], lr=lr_w)])
        t0 = time.time()
        for i in range(schritte):
            opt.zero_grad()
            L, ber, reg = self.verlust(schritt, sigma)
            L.backward()
            opt.step()
            if zeige and (i % zeige == 0 or i == schritte - 1):
                s = ' '.join('%s %.3f/%.3f' % (t[:6], a, b) for n in ber for t, (a, b) in ber[n].items())
                print('%4d L %.4f %s reg %s  %.1fs' % (i, float(L), s, ' '.join('%.3f' % x for x in reg), time.time() - t0), flush=True)

    def stand(self):
        return dict(lr=[x.detach().cpu().numpy().tolist() for x in self.lr], c=[x.detach().cpu().numpy().tolist() for x in self.c],
                    dw={str(n): (self.pose[n]['dw'] * self.gelenk_maske[n]).detach().cpu().numpy().tolist() for n in (1, 2)})

    def laden(self, s, ohne=()):
        for i, p in enumerate(self.teile):
            l = np.array(s['lr'][i])
            c = np.array(s['c'][i])
            if l.shape == (p.T, p.n) and p.name not in ohne:
                self.lr[i].data[:] = torch.tensor(l, dtype=torch.float32, device=self.dev)
                self.c[i].data[:] = torch.tensor(c, dtype=torch.float32, device=self.dev)
        for n in (1, 2):
            self.pose[n]['dw'].data[:] = torch.tensor(np.array(s['dw'][str(n)]), dtype=torch.float32, device=self.dev)


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default='zw/s1.json')
    a.add_argument('--von')
    a.add_argument('--schritte', type=int, default=400)
    a.add_argument('--fein', type=int, default=0)
    a.add_argument('--ohne', default='')
    a = a.parse_args()
    g, teile = rc_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in teile), 'Teile', len(teile))
    gl = [(n, k['name'], 1.0) for n in (1, 2) for k in g['knochen']]
    pa = Passung(g, teile, gelenke=gl)
    for n in (1, 2):
        for b in ['hand_r', 'hand_l'] + FINGER:
            pa.pose_gew[n][pa.ix[b]] = 0.3
    for b in ('oberschenkel_r', 'unterschenkel_r', 'fuss_r', 'oberschenkel_l', 'unterschenkel_l', 'fuss_l'):
        pa.pose_gew[2][pa.ix[b]] = 0.25
    if a.von:
        pa.laden(json.load(open(os.path.join(HIER, a.von))), ohne=a.ohne.split(','))
    if a.schritte:
        pa.lauf(a.schritte, 2, 1.5)
        pa.lauf(a.schritte // 2, 1, 1.0, lr=0.005, lr_c=2.0, lr_w=1.0)
    if a.fein:
        pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, spiegel=0.03, mitte=0.06, naht=0.8, sym=0.5)
        pa.lauf(a.fein, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8)
    json.dump(pa.stand(), open(os.path.join(HIER, a.aus), 'w'))
