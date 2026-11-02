#!/usr/bin/env python3
"""Rohbau final, Passung: Ringradien, Ringmitten, Gelenke, Wurzel an die Nutzer-Teile, beide Bilder zugleich.
Neu gegen Rohbau C: Oberkoerper streng (nur 0, 7, 10, 11, 21, 22, 23), Sichtkennung (wer steht vorn), Keile als Deckung, vier Glieder am Boden, Glieder nicht im Rumpf, Gelenkgrenzen samt Drehung, Spiegelbindung.
Kein Metaball, kein Voxel: Parameter sind nur Ringe und Winkel."""
import argparse
import json
import math
import os
import time

import cv2
import numpy as np
import torch

from rf_kern import E, GERAET, HIER, TEILE, Kamera, Kette, Netz, innen_tiefe, kennbild, masken, namen, twist_y, weich, ziel
import rf_plan

KLAUEN_R = ['klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4']
KLAUEN_L = ['klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
SCHWANZ = ['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6']
BEIN_L = ['oberschenkel_l', 'unterschenkel_l', 'fuss_l']
BEIN_R = ['oberschenkel_r', 'unterschenkel_r', 'fuss_r']
GRUPPEN = {1: dict(tail=SCHWANZ, left_leg=BEIN_L, right_leg=BEIN_R, right_arm=['oberarm_r', 'unterarm_r', 'hand_r'] + KLAUEN_R, part_of_Body=['huefte', 'wirbel', 'brust'],
                   versteckt=['oberarm_l', 'unterarm_l', 'hand_l'] + KLAUEN_L + ['hals', 'kopf', 'kiefer']),
           2: dict(face_and_upper_body=['huefte', 'wirbel', 'brust', 'hals', 'oberarm_l'], kopf=['kopf', 'kiefer'], left_arm_front=['unterarm_l', 'hand_l'] + KLAUEN_L,
                   right_arm_front=['unterarm_r', 'hand_r'] + KLAUEN_R, versteckt=BEIN_L + BEIN_R + SCHWANZ + ['oberarm_r'])}
DAVOR = {1: dict(tail=[], left_leg=['tail'], right_leg=['tail'], right_arm=[], part_of_Body=['tail', 'right_leg', 'right_arm']),
         2: dict(face_and_upper_body=[], left_arm_front=['face_and_upper_body'], right_arm_front=['face_and_upper_body'])}
UEBERLAPP = {1: [('part_of_Body', 'right_leg'), ('tail', 'right_leg'), ('tail', 'left_leg')], 2: []}
GESICHT = [(263, 245), (272, 215), (295, 190), (325, 183), (355, 190), (378, 215), (387, 245), (380, 272), (366, 292), (340, 306), (312, 328), (292, 322), (272, 288)]
# Punktziele: Spitzen des Nutzer-Umrisses. Rechts unter dem Logo nicht erzwungen (Bewertung Messung): nur schwach. Sporn (145,353) laeuft nach hinten aussen, Hoehe rund 600 statt 1357.
SPITZEN = {1: {'klaue_r3': ((425, 363), .1), 'klaue_r4': ((396, 328), .04), 'klaue_l3': ((320, 354), .04), 'klaue_l2': ((326, 354), .02), 'klaue_l1': ((330, 354), .02), 'klaue_l4': ((336, 358), .02), 'kralle_l': ((334, 354), .03)}, 2: {'wange_l': ((366, 304), .06), 'klaue_r3': ((145, 353), .06), 'klaue_r4': ((274, 443), .02), 'klaue_r1': ((222, 475), .01), 'kralle_r': ((197, 476), .01), 'klaue_r2': ((168, 447), .02),
                                                'klaue_l3': ((461, 392), .02), 'klaue_l2': ((468, 417), .02), 'klaue_l1': ((450, 416), .02), 'kralle_l': ((439, 418), .03), 'klaue_l4': ((414, 394), .02)}}
BODEN = dict(hand_r=['hand_r', 'kralle_r'] + KLAUEN_R, hand_l=['hand_l', 'kralle_l'] + KLAUEN_L, fuss_r=['fuss_r', 'zeh0_r', 'zeh1_r', 'zeh2_r'], fuss_l=['fuss_l', 'zeh0_l', 'zeh1_l', 'zeh2_l'])
# (Glied, ab t): Ecken dieser Ringe sollen nicht im Rumpf stecken. Die Gelenkkoepfe duerfen.
INNEN = [('oberarm_r', 120), ('unterarm_r', -999), ('hand_r', -999), ('oberarm_l', 120), ('unterarm_l', -999), ('hand_l', -999), ('oberschenkel_r', 90), ('unterschenkel_r', -999), ('fuss_r', -999),
         ('oberschenkel_l', 90), ('unterschenkel_l', -999), ('fuss_l', -999)]
KOERPER = ['huefte', 'wirbel', 'brust']
KOPFTEILE = ['kopf', 'kiefer']
TWIST = {'unterarm_l': 18, 'unterarm_r': 18, 'unterschenkel_l': 18, 'unterschenkel_r': 18, 'fuss_l': 38, 'fuss_r': 38, 'hand_l': 65, 'hand_r': 65}
GRENZ_KLAUE = [[-3, 3], [-30, 30], [-30, 95]]


class Passung:
    def __init__(self, g, teile, stand=None, ohne=()):
        self.g, self.teile = g, teile
        self.dev = GERAET
        self.netz = Netz(teile)
        self.ix = namen(g)
        self.kette = Kette(g)
        lr, c = self.netz.startwerte()
        self.lr0 = {i: x.clone() for i, x in lr.items()}
        self.c0 = {i: x.clone() for i, x in c.items()}
        self.lr = {i: x.clone().requires_grad_(True) for i, x in lr.items()}
        self.c = {i: x.clone().requires_grad_(True) for i, x in c.items()}
        self.versatz = torch.tensor([k['versatz'] for k in g['knochen']], dtype=torch.float32, device=self.dev)
        self.pose = {}
        nk = len(g['knochen'])
        for n in (1, 2):
            p = g['posen']['bild%d' % n]
            w = torch.tensor(p['winkel'], dtype=torch.float32, device=self.dev)
            self.pose[n] = dict(w0=w.clone(), dw=torch.zeros_like(w, requires_grad=True), wz0=torch.tensor(p['wurzel'], dtype=torch.float32, device=self.dev), dwz=torch.zeros(3, device=self.dev, requires_grad=True),
                                gier0=float(p['gier']), dg=torch.zeros(1, device=self.dev, requires_grad=True), lage0=torch.tensor(p['lage'], dtype=torch.float32, device=self.dev), dl=torch.zeros(3, device=self.dev, requires_grad=True))
        self.pose_gew = {n: torch.ones((nk, 1), device=self.dev) for n in (1, 2)}
        self.pose_frei = {n: torch.ones((nk, 1), device=self.dev) for n in (1, 2)}      # 0 = Knochen hart gesperrt (Nacharbeit)
        self.xz = torch.tensor([1.0, 0.0, 1.0], device=self.dev)        # Aktor steht auf y = 0, Hoehe traegt die Wurzel
        self.kam = {n: Kamera(n) for n in (1, 2)}
        self.box = {1: (175, 170, 445, 480), 2: (130, 130, 480, 480)}
        self.gewicht = dict(rund=3.0, laengs=3.0, gestalt=0.05, mitte=0.1, naht=1.0, cover=1.0, spill=1.0, grau=0.25, pose=1.0, gesicht=1.0, keil=0.3, boden=1.0, innen=1.0, grenze=1.0, sicht=1.0, blick=0.5, spitze=1.0, wirbelsaeule=.2)
        self._masken_laden()
        idx = {}
        for i, p in enumerate(teile):
            idx.setdefault(p.name, i)
        self.idx = idx
        self.tele = [(idx[a], ra, idx[b], rb, d, torch.tensor(j, device=self.dev)) for a, ra, b, rb, d, j in rf_plan.TELESKOP]
        self.ketten = []
        for i in range(1, 6):
            self.ketten.append((idx['schwanz%d' % i], -1, idx['schwanz%d' % (i + 1)], 1))
        for a, b in (('oberarm_r', 'unterarm_r'), ('oberschenkel_r', 'unterschenkel_r')):
            self.ketten.append((idx[a], -2, idx[b], 1))
        self.gewicht['rolle'] = 0.0
        self.mitte_gew = {}
        self.bilder = (1, 2)
        self.gesperrt = set()
        self.entwurf = {i: dict(kopf=20.0, kiefer=20.0, hals=300.0, unterarm_r=8.0, hand_r=2.0, klaue_r1=6.0, klaue_r2=6.0, klaue_r3=6.0, klaue_r4=6.0, fang_o_r=1e3).get(teile[i].name, 1.0) for i in self.netz.meister}
        self.kappe = {}
        for i, p in enumerate(teile):
            self.kappe[p.name] = self.netz.offs[i] + p.T * p.n + 1
        self._bodenmasken()
        self._innenmasken()
        self.limits = self._grenzen()
        self.lim_b = torch.tensor([b for b, _ in self.limits], device=self.dev)
        self.lim_lo = torch.tensor([[x[0] for x in l] for _, l in self.limits], dtype=torch.float32, device=self.dev)
        self.lim_hi = torch.tensor([[x[1] for x in l] for _, l in self.limits], dtype=torch.float32, device=self.dev)
        self.tw_b = torch.tensor([self.ix[k] for k in TWIST], device=self.dev)
        self.tw_g = torch.tensor([v for v in TWIST.values()], dtype=torch.float32, device=self.dev)
        self.anker = {1: [], 2: []}
        self.zusatz = None      # Nacharbeit: eigener Term je Bild (n, R, T, w, xy) -> Verlust
        self.sicht_id = {}
        self.spitze_h0 = None
        if stand:
            self.laden(stand, ohne)

    # ───────────────────────────────────────── Daten
    def _masken_laden(self):
        import form_pruefung as P
        m, logo = masken()
        self.ziele = {}
        Yb = {n: cv2.GaussianBlur(P.original(n).astype(np.float32) @ P.YK, (0, 0), 1) for n in (1, 2)}
        uni = {}
        for n in (1, 2):
            al = np.zeros_like(logo)
            for t in TEILE[n]:
                al |= m[t]
            uni[n] = al
        logo_w = cv2.dilate(logo.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0

        def zone(n, box, sw, frei=False):
            x0, y0, x1, y1 = box
            w = np.zeros(logo.shape, bool)
            w[y0:y1, x0:x1] = True
            w = w & ~logo_w & (Yb[n] < sw)
            return w if frei else w & ~uni[n]
        for n in (1, 2):
            x0, y0, x1, y1 = self.box[n]
            z = ziel(n)
            d = dict(logo=logo[y0:y1, x0:x1], alle=uni[n][y0:y1, x0:x1], schwarz=(z == 0)[y0:y1, x0:x1])
            for t in TEILE[n]:
                d[t] = m[t][y0:y1, x0:x1]
            if n == 1:
                d['keil_left_leg'] = zone(1, (215, 345, 258, 448), 110)[y0:y1, x0:x1]
                d['keil_right_leg'] = zone(1, (283, 360, 305, 435), 110)[y0:y1, x0:x1]
                d['spalt'] = zone(1, (362, 238, 396, 306), 110)[y0:y1, x0:x1]            # zwischen Rumpf und rechtem Arm: im Video dunkel, kein Licht scheint durch
                d['dunkel'] = zone(1, (336, 280, 386, 326), 75, True)[y0:y1, x0:x1]      # M1 Achselfenster: im Video Figur (RGB 25,23,41), hier darf stehen, was sonst versteckt ist
                d['o1'] = zone(1, (403, 252, 433, 302), 75)[y0:y1, x0:x1]                # M2 O1: dunkle Masse rechts neben dem Arm, Nutzerfrage
                d['kerbe'] = zone(1, (318, 188, 344, 216), 100, True)[y0:y1, x0:x1]      # M5 Kerbe Schwanz/Koerper: im Video dunkel bis y 195-199
                q = np.zeros(logo.shape, np.uint8)
                q[190:197, 296:322] = 1                                                  # M5 Dachlinie y 187 durchgehend, x 300-322
                q[198:204, 322:330] = 1                                                  # M5 Kante faellt gleichmaessig zum Koerper
                q[204:210, 330:340] = 1
                q[300:316, 410:419] = 1                                                  # M2 Aussenkante Unterarm, in der Maske, ausser der Engstelle y 318-330
                q[333:345, 410:418] = 1
                cv2.fillPoly(q, [np.array([(379, 360), (381, 373), (404, 373), (423, 362), (418, 357), (400, 356)], np.int32)], 1)      # Abnahme 2: Fuss des Arms, flache Pranke von hinten
                q[306:313, 290:301] = 1                                                  # Abnahme 2: Rautenspitze L2 oeffnete 5-8 px zu frueh (Spitze Original y 313)
                q[316:330, 301:308] = 1                                                  # rechtes Bein, Innenkante (Soll 300 bei y 322)
                q[336:348, 283:288] = 1                                                  # Schwanz, rechte Kante (Soll 287 bei y 342)
                d['band_ein'] = (q > 0)[y0:y1, x0:x1]
                q = np.zeros(logo.shape, np.uint8)
                q[172:185, 292:326] = 1                                                  # M5 ueber der Dachlinie
                q[172:192, 326:338] = 1
                q[222:238, 378:402] = 1                                                  # Schulterkuppe nicht ueber die Maske (Original hell bis y 247, Maske ab 243)
                q[236:241, 383:399] = 1                                                  # Abnahme 2: Armkuppe x 385-395 lag rund 5 px zu hoch
                q[316:322, 289:297] = 1                                                  # Abnahme 2: Schwanzkante bei y 318 bleibt bei x 287
                q[338:347, 216:223] = 1                                                  # Abnahme 2: left_leg Kante links bei y 342
                q[322:346, 368:379] = 1                                                  # links neben dem Unterarm (Soll Kante 381)
                q[318:331, 417:430] = 1                                                  # rechts neben der Engstelle (Soll Kante 414)
                q[342:358, 345:360] = 1                                                  # Boden rechts neben dem Bein, linke Hand bleibt hinter dem Bein
                q[334:358, 421:434] = 1                                                  # Abnahme 2: rechts neben dem Unterarm, flache Hand steht nicht ueber
                d['band_aus'] = (q > 0)[y0:y1, x0:x1]
            else:
                d['dunkel'] = zone(2, (300, 300, 400, 385), 90)[y0:y1, x0:x1]
                q = np.zeros(logo.shape, np.uint8)
                cv2.fillPoly(q, [np.array(GESICHT, np.int32)], 1)
                d['gesicht'] = q[y0:y1, x0:x1].astype(bool)
                q = np.zeros(logo.shape, np.uint8)
                cv2.circle(q, (366, 304), 9, 1, -1)                     # Stelle P: dort steht im Video Figur mit Glanzpunkt
                cv2.rectangle(q, (404, 226), (415, 262), 1, -1)         # Rand rechts bis x=413, Rohr verdeckt (FORMBLATT 1.4)
                cv2.fillPoly(q, [np.array([(398, 196), (411, 205), (424, 226), (404, 226)], np.int32)], 1)     # Kante 7 (411,206) und Band Q4 liegen auf der Figur
                import form_pruefung as P_
                for v_ in P_.formblatt()['verdeckungen_bild2']:
                    if v_['id'] in ('V1', 'V2', 'V3', 'V4', 'V5'):
                        a_, b_ = v_['stelle'][0], v_['stelle'][1]
                        cv2.line(q, (int(a_[0]), int(a_[1])), (int(b_[0]), int(b_[1])), 1, 3)
                d['pflicht'] = q[y0:y1, x0:x1].astype(bool)
                q = np.zeros(logo.shape, np.uint8)
                q[402:417, 279:330] = 1                                                  # Fuge: Boden frei
                cv2.fillPoly(q, [np.array([(398, 393), (433, 418), (398, 418)], np.int32)], 1)      # Abnahme 2: Boden links unter der linken Hand
                q[384:397, 345:408] = 1                                                  # Abnahme 2: Boden BO
                d['band_aus'] = (q > 0)[y0:y1, x0:x1]
                q = np.zeros(logo.shape, np.uint8)
                cv2.fillPoly(q, [np.array([(360, 297), (367, 296), (374, 308), (372, 313), (364, 309), (358, 302)], np.int32)], 1)      # Abnahme 2: Stelle P, dort steht im Video Figur
                d['band_ein'] = (q > 0)[y0:y1, x0:x1]
            self.ziele[n] = d
        self.kanal = {}
        for n in (1, 2):
            kn = -np.ones(len(self.g['knochen']), int)
            nam = list(GRUPPEN[n].keys())
            for i, t in enumerate(nam):
                for b in GRUPPEN[n][t]:
                    kn[self.ix[b]] = i
            assert (kn >= 0).all(), n
            self.kanal[n] = (nam, torch.tensor(kn, device=self.dev))

    def _bodenmasken(self):
        self.boden = {}
        kn = self.netz.vkn
        pn = []
        for i, p in enumerate(self.teile):
            pn += [p.name] * (p.T * p.n + 2)
        pn = np.array(pn)
        for g_, l in BODEN.items():
            self.boden[g_] = torch.tensor(np.nonzero(np.isin(pn, l))[0], device=self.dev)
        self.teilname = pn

    def _innenmasken(self):
        self.innen = []
        for nm, tmin in INNEN:
            i = self.netz.teile.index(next(p for p in self.teile if p.name == nm))
            p = self.teile[i]
            a = self.netz.offs[i]
            sel = []
            for k in range(p.T):
                if p.t[k] >= tmin:
                    sel += list(range(a + k * p.n, a + (k + 1) * p.n))
            self.innen.append((nm, torch.tensor(sel, device=self.dev)))
        self.koerper = [next(i for i, p in enumerate(self.teile) if p.name == n) for n in KOERPER]
        self.kopfteile = [next(i for i, p in enumerate(self.teile) if p.name == n) for n in KOPFTEILE]
        nr = {p.name: i for i, p in enumerate(self.teile)}
        self.gegen = {}
        for nm, _ in INNEN:
            l_ = list(self.koerper)
            if 'arm' in nm:
                l_ += self.kopfteile
            s_ = nm[-1]
            if 'schenkel' in nm or 'fuss' in nm:
                l_ += [nr['unterarm_' + s_], nr['hand_' + s_], nr['oberarm_' + s_]]      # Bein geht nicht durch den Arm derselben Seite
            self.gegen[nm] = l_

    def _grenzen(self):
        gr = self.g['winkelgrenzen_grad']
        aus = []
        for k in self.g['knochen']:
            kl = k['klasse']
            lim = GRENZ_KLAUE if kl == 'klaue' else gr.get(kl)
            if lim is None:
                continue
            lim = [list(x) for x in lim]
            if k['name'].endswith('_r') and kl in ('oberarm', 'oberschenkel', 'hand', 'fuss', 'unterarm', 'unterschenkel'):
                lim[0] = [-lim[0][1], -lim[0][0]]
                lim[1] = [-lim[1][1], -lim[1][0]]
            if kl == 'klaue' and '_r' in k['name']:
                lim[0] = [-lim[0][1], -lim[0][0]]
                lim[1] = [-lim[1][1], -lim[1][0]]
            aus.append((k['index'], lim))
        return aus

    def _ziele_raster(self, n, schritt):
        if not hasattr(self, '_zr'):
            self._zr = {}
        k = (n, schritt)
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

    # ───────────────────────────────────────── Rechnen
    def winkel(self, n):
        return self.pose[n]['w0'] + self.pose[n]['dw'] * self.pose_frei[n]

    def kette_pose(self, n):
        p = self.pose[n]
        return self.kette(self.winkel(n), self.versatz, p['wz0'] + p['dwz'], p['gier0'] + p['dg'][0], p['lage0'] + p['dl'] * self.xz)

    def bildverlust(self, n, v, L_, C_, schritt, sigma, zaehlen=False):
        R, T = self.kette_pose(n)
        w = self.netz.welt(v, R, T)
        xy = self.kam[n](w)
        x0, y0, x1, y1 = self.box[n]
        W, H = (x1 - x0) // schritt, (y1 - y0) // schritt
        nam, kn = self.kanal[n]
        ka = kn[self.netz.tkn]
        S = weich(xy, self.netz.tris, ka, H, W, sigma, schritt, x0, y0, C=len(nam))
        d = self._ziele_raster(n, schritt)
        verl, aus = 0.0, {}
        idx = {t: i for i, t in enumerate(nam)}
        # Sichtkennung: wer steht vorn. Ohne Gradient, alle paar Schritte neu.
        if self.sicht_id.get((n, schritt)) is None or zaehlen:
            with torch.no_grad():
                z = self.kam[n].tiefe(w).cpu().numpy()
                bild = kennbild(xy.detach().cpu().numpy(), z, self.netz.tris.cpu().numpy(), ka.cpu().numpy(), H, W, schritt, x0, y0)
            self.sicht_id[(n, schritt)] = torch.tensor(bild, device=self.dev)
        sid = self.sicht_id[(n, schritt)]
        Sf = dict(S=S)
        for t in TEILE[n]:
            M = d[t]
            Si = S[idx[t]]
            cov = Si
            for a, b in UEBERLAPP[n]:
                if t in (a, b):
                    o = a if t == b else b
                    cov = cov + d[a] * d[b] * S[idx[o]] * (1 - cov)
            if t == 'face_and_upper_body':
                cov = cov + S[idx['kopf']] * (1 - cov)
            if n == 1 and t == 'part_of_Body':
                cov = cov + d['dunkel'] * S[idx['versteckt']] * (1 - cov)      # M1: im Fenster deckt der Kopf den Auslaeufer zum Arm
            fehlt = (M * (1 - cov) * d['frei']).sum() / (M * d['frei']).sum().clamp(min=1)
            erlaubt = M
            for o in DAVOR[n][t]:
                erlaubt = torch.maximum(erlaubt, d[o])
            if n == 1 and t == 'right_arm':
                erlaubt = torch.maximum(erlaubt, d['spalt'])
                erlaubt = torch.maximum(erlaubt, d['dunkel'])
                if self.gewicht.get('o1', 0):
                    erlaubt = torch.maximum(erlaubt, d['o1'])
            if n == 1 and t in ('tail', 'part_of_Body'):
                erlaubt = torch.maximum(erlaubt, d['kerbe'])
            if n == 1 and t == 'part_of_Body':
                erlaubt = torch.maximum(erlaubt, d['dunkel'])
            if n == 1 and ('keil_' + t) in d:
                Z = d['keil_' + t]
                erlaubt = torch.maximum(erlaubt, Z)
                verl = verl + self.gewicht['keil'] * (Z * (1 - Si)).sum() / Z.sum()
            ueber = Si * (1 - erlaubt) * d['frei']
            grau = dict(part_of_Body=1.0, left_leg=.5, right_arm_front=.7, left_arm_front=.6).get(t, self.gewicht['grau'])
            schw = 1.5 if n == 1 else 1.0
            spill = schw * (ueber * d['schwarz']).sum() + grau * (ueber * (1 - d['schwarz'])).sum()
            if n == 2 and t in ('right_arm_front', 'left_arm_front'):
                spill = spill + 0.8 * (Si * d['face_and_upper_body'] * (1 - M) * d['frei'] * (sid == idx[t]).float()).sum()
            spill = spill / (M * d['frei']).sum().clamp(min=1)
            verl = verl + self.gewicht['cover'] * fehlt + self.gewicht['spill'] * spill
            aus[t] = (float(fehlt), float(spill))
        # Teile ohne Maske: sichtbar nur, wo Dunkel erlaubt ist
        V = idx['versteckt']
        vis = (sid == V).float()
        A = d['alle']
        erl = d.get('dunkel', 0 * A)
        ueb = S[V] * vis * d['frei'] * (1 - erl)
        q = (ueb * d['schwarz']).sum() * 1.0 + (ueb * (1 - d['schwarz']) * (1 - A)).sum() * .4 + (ueb * A).sum() * .8
        verl = verl + self.gewicht['sicht'] * q / (6000.0 / (schritt * schritt))
        aus['versteckt'] = (float((vis * A).sum() * schritt * schritt), float((vis * (1 - A)).sum() * schritt * schritt))
        if n == 1:
            Z = d['spalt']
            verl = verl + 0.12 * (Z * (1 - S[idx['right_arm']])).sum() / Z.sum()
            U = 1 - torch.prod(1 - S, 0)
            G_ = self.gewicht
            if G_.get('fenster', 0):
                Z = d['dunkel']
                verl = verl + G_['fenster'] * (Z * (1 - U)).sum() / Z.sum()
            if G_.get('o1', 0):
                Z = d['o1']
                verl = verl + G_['o1'] * (Z * (1 - S[idx['right_arm']])).sum() / Z.sum()
            if G_.get('kerbe', 0):
                Z = d['kerbe']
                verl = verl + G_['kerbe'] * (Z * (1 - U)).sum() / Z.sum()
            if G_.get('band', 0):
                Z = d['band_ein']
                verl = verl + G_['band'] * (Z * (1 - U)).sum() / Z.sum()
                Z = d['band_aus']
                verl = verl + G_['band'] * (Z * U).sum() / Z.sum()
        if n == 2:
            if self.gewicht.get('band', 0):
                Z = d['band_aus']
                verl = verl + self.gewicht['band'] * (Z * (1 - torch.prod(1 - S, 0))).sum() / Z.sum()
                Z = d['band_ein']
                verl = verl + self.gewicht.get('band_p', 0.0) * (Z * torch.prod(1 - S, 0)).sum() / Z.sum()
            F = d['gesicht']
            Sk = S[idx['kopf']]
            ff = (F * (1 - Sk)).sum() / F.sum()
            uu = (Sk * (1 - F)).sum() / F.sum()
            verl = verl + self.gewicht['gesicht'] * (ff + .6 * uu)
            aus['kopf'] = (float(ff), float(uu))
            Z = d['dunkel']
            U = 1 - (1 - S[V]) * (1 - S[idx['face_and_upper_body']])
            verl = verl + 0.1 * (Z * (1 - U)).sum() / Z.sum()
            Z = d['pflicht']
            U = 1 - (1 - U) * (1 - Sk)
            verl = verl + 0.5 * (Z * (1 - U)).sum() / Z.sum()
            # Kopf schaut zur Kamera
            kr = self.ix['kopf']
            zum_auge = torch.tensor(self.kam[2].k['auge'], dtype=torch.float32, device=self.dev) - T[kr]
            zum_auge = zum_auge / zum_auge.norm()
            verl = verl + self.gewicht['blick'] * (1 - R[kr][:, 0] @ zum_auge)
        # Boden: je Glied der tiefste Punkt
        hoehe = {}
        y = w[:, 1]
        for g_, ix_ in self.boden.items():
            top = torch.topk(y[ix_], 6).values.mean()
            hoehe[g_] = -float(top)
            verl = verl + self.gewicht['boden'] * (top / 60.0) ** 2 * .25
        b = torch.relu(y + 4.0)
        verl = verl + self.gewicht['boden'] * (b / 60.0).pow(2).mean() * 60
        # form_emd.boden_nachfuehren hebt die ganze Figur um den tiefsten Punkt: Eindringen eines einzelnen Fingers versetzt in Bild 1 alles um Pixel
        verl = verl + self.gewicht['boden'] * (torch.topk(b, 6).values.mean() / 12.0) ** 2
        aus['_hoehe'] = hoehe
        # Glieder nicht im Rumpf, Arme nicht im Kopf
        s_ = 0.0
        inn = {}
        for nm, sel in self.innen:
            q_ = w[sel]
            dm = None
            for i in self.gegen[nm]:
                de = innen_tiefe(q_, self.teile[i], self.netz.pl[i], L_[i], C_[i], R, T)
                dm = de if dm is None else torch.maximum(dm, de)
            pen = torch.relu(dm + 15.0) / 120.0
            s_ = s_ + (pen * pen).mean()
            inn[nm] = float((dm > 0).float().mean())
        verl = verl + self.gewicht['innen'] * s_ * 4
        aus['_innen'] = inn
        # Punktziele
        for nm, (zp, gw) in SPITZEN[n].items():
            q = xy[self.kappe[nm]]
            verl = verl + self.gewicht['spitze'] * gw * ((q[0] - zp[0]) ** 2 + (q[1] - zp[1]) ** 2) / 25.0
        if n == 2:
            verl = verl + self.gewicht['spitze'] * .02 * ((-w[self.kappe['klaue_r3'], 1] - 620.0) / 150.0) ** 2
        if self.gewicht.get('rolle', 0):
            # Unterarm rechts rollt: Bild 1 sieht die Ecken um 3 (aussen, hinten), Bild 2 die Ecken um 7 (innen, vorn). Dann traegt G eine Seite, die Bild 1 nicht sieht.
            b_ = self.ix['unterarm_r']
            d_ = self.netz.pl[self.idx['unterarm_r']]
            w_ = 2 * math.pi * (3.0 if n == 1 else 7.0) / 8
            nw = R[b_] @ (math.cos(w_) * d_['ea'] + math.sin(w_) * d_['eb'])
            ze = torch.tensor(self.kam[n].k['auge'], dtype=torch.float32, device=self.dev) - T[b_]
            ze = ze / ze.norm()
            verl = verl + self.gewicht['rolle'] * (1 - nw @ ze)
        for bn, lok, zp, h, gw in self.anker[n]:
            i_ = self.ix[bn]
            pt = R[i_] @ torch.tensor(lok, dtype=torch.float32, device=self.dev) + T[i_]
            if zp is not None:
                q = self.kam[n](pt[None])[0]
                verl = verl + gw * ((q[0] - zp[0]) ** 2 + (q[1] - zp[1]) ** 2) / 25.0
            if h is not None:
                verl = verl + gw * ((-pt[1] - h) / 80.0) ** 2
        if self.zusatz is not None:
            verl = verl + self.zusatz(n, R, T, w, xy, L_, C_)
        return verl, aus

    def basis_jetzt(self):
        """Regeln messen ab jetzt gegen den aktuellen Stand: kleine Korrektur statt neuer Passung."""
        self.lr0 = {i: x.detach().clone() for i, x in self.lr.items()}
        self.c0 = {i: x.detach().clone() for i, x in self.c.items()}

    def regel(self):
        rund, laengs, gest, mit = 0.0, 0.0, 0.0, 0.0
        for i in self.netz.meister:
            p = self.teile[i]
            l, l0, c, c0 = self.lr[i], self.lr0[i], self.c[i], self.c0[i]
            d = l - l0
            if p.n >= 3:
                dd = torch.roll(d, 1, 1) + torch.roll(d, -1, 1) - 2 * d
                rund = rund + (dd * dd).mean()
            if p.T >= 3:
                dt = np.diff(p.t)
                h0 = torch.tensor(dt[:-1], dtype=torch.float32, device=self.dev)[:, None]
                h1 = torch.tensor(dt[1:], dtype=torch.float32, device=self.dev)[:, None]
                d2 = ((d[2:] - d[1:-1]) / h1 - (d[1:-1] - d[:-2]) / h0) * (0.5 * (h0 + h1))
                laengs = laengs + (d2 * d2).mean()
            gest = gest + self.entwurf[i] * (d * d).mean()
            dc = (c - c0) / 400.0 * self.mitte_gew.get(p.name, 1.0)
            mit = mit + (dc.pow(2)).mean() * 2
            if p.T >= 3:
                d2c = dc[2:] - 2 * dc[1:-1] + dc[:-2]
                mit = mit + 8 * d2c.pow(2).mean()
        return rund, laengs, gest, mit

    def naht(self, L_):
        s = 0.0
        for a, ra, b, rb, dl, ji in self.tele:
            # Ring des hinteren Stuecks bleibt unter dem Rand des vorderen
            s = s + torch.relu(L_[a][ra][ji] - L_[b][rb][ji] - dl).pow(2).mean()
        for a, ra, b, rb in self.ketten:
            s = s + (L_[a][ra] - L_[b][rb]).pow(2).mean()
        return s

    def posestrafe(self):
        s = 0.0
        for n in (1, 2):
            p = self.pose[n]
            s = s + ((p['dw'] * self.pose_gew[n]) / 100.0).pow(2).sum()
            s = s + 0.3 * (p['dwz'] / 300.0).pow(2).sum() + (p['dl'] / 400.0).pow(2).sum() + (p['dg'] / 60.0).pow(2).sum()
        return s

    def grenzstrafe(self):
        s = 0.0
        for n in (1, 2):
            w = self.winkel(n)
            g = w[self.lim_b] * 360.0 / 4096
            s = s + ((torch.relu(self.lim_lo - g) / 10.0) ** 2 + (torch.relu(g - self.lim_hi) / 10.0) ** 2).sum()
            tw = twist_y(w[self.tw_b] * E) * 180 / math.pi
            s = s + ((torch.relu(tw.abs() - self.tw_g) / 8.0) ** 2).sum()
        return s

    def verlust(self, schritt, sigma, zaehlen=False):
        v, L_, C_ = self.netz.ecken(self.lr, self.c)
        L = 0.0
        ber = {}
        for n in self.bilder:
            l, a = self.bildverlust(n, v, L_, C_, schritt, sigma, zaehlen)
            L = L + l
            ber[n] = a
        rund, laengs, gest, mit = self.regel()
        G = self.gewicht
        nh = self.naht(L_)
        gs = self.grenzstrafe()
        ps = self.posestrafe()
        L = L + G['rund'] * rund + G['laengs'] * laengs + G['gestalt'] * gest + G['mitte'] * mit + G['naht'] * nh + G['pose'] * 0.01 * ps + G['grenze'] * .1 * gs
        return L, ber, (float(rund), float(laengs), float(gest), float(mit), float(nh), float(ps), float(gs))

    def lauf(self, schritte, schritt, sigma, lr=0.008, lr_c=3.0, lr_w=2.5, lr_wz=4.0, zeige=50, form=True, tag=''):
        gr = [dict(params=[self.pose[n]['dw'] for n in (1, 2)], lr=lr_w), dict(params=[self.pose[n]['dwz'] for n in (1, 2)] + [self.pose[n]['dl'] for n in (1, 2)], lr=lr_wz), dict(params=[self.pose[n]['dg'] for n in (1, 2)], lr=.3)]
        if form:
            fr = [i for i in self.lr if self.teile[i].name not in self.gesperrt]
            gr = [dict(params=[self.lr[i] for i in fr], lr=lr), dict(params=[self.c[i] for i in fr], lr=lr_c)] + gr
        opt = torch.optim.Adam(gr)
        t0 = time.time()
        self.sicht_id = {}
        for i in range(schritte):
            opt.zero_grad()
            L, ber, reg = self.verlust(schritt, sigma, zaehlen=(i % 15 == 0))
            L.backward()
            opt.step()
            if zeige and (i % zeige == 0 or i == schritte - 1):
                s = ' '.join('%s %.3f/%.3f' % (t[:5], a, b) for n in ber for t, (a, b) in ((k_, v_) for k_, v_ in ber[n].items() if not k_.startswith('_')))
                h = ' | '.join(' '.join('%s %.0f' % (k_[0] + k_[-1], v_) for k_, v_ in ber[n]['_hoehe'].items()) for n in ber)
                print('%s%4d L %.4f %s\n      boden %s reg %s  %.0fs' % (tag, i, float(L), s, h, ' '.join('%.3f' % x for x in reg), time.time() - t0), flush=True)
                if i == schritte - 1:
                    for n in ber:
                        print('      innen', n, ' '.join('%s %.2f' % (k_.replace('schenkel', 's'), v_) for k_, v_ in ber[n]['_innen'].items() if v_ > 0.01), flush=True)

    def stand(self):
        return dict(lr={str(i): x.detach().cpu().numpy().tolist() for i, x in self.lr.items()}, c={str(i): x.detach().cpu().numpy().tolist() for i, x in self.c.items()},
                    posen={str(n): dict(winkel=self.winkel(n).detach().cpu().numpy().tolist(), wurzel=(self.pose[n]['wz0'] + self.pose[n]['dwz']).detach().cpu().numpy().tolist(),
                                        gier=float(self.pose[n]['gier0'] + self.pose[n]['dg'][0]), lage=(self.pose[n]['lage0'] + self.pose[n]['dl'] * self.xz).detach().cpu().numpy().tolist()) for n in (1, 2)})

    def laden(self, s, ohne=()):
        for i in self.netz.meister:
            k = str(i)
            if k in s['lr']:
                l = np.array(s['lr'][k])
                p = self.teile[i]
                if l.shape == (p.T, p.n) and p.name not in ohne:
                    self.lr[i].data[:] = torch.tensor(l, dtype=torch.float32, device=self.dev)
                    self.c[i].data[:] = torch.tensor(np.array(s['c'][k]), dtype=torch.float32, device=self.dev)
        for n in (1, 2):
            q = s['posen'][str(n)]
            p = self.pose[n]
            p['w0'][:] = torch.tensor(q['winkel'], dtype=torch.float32, device=self.dev)
            p['dw'].data[:] = 0
            wz = list(q['wurzel'])
            wz[1] += q['lage'][1]
            lg = [q['lage'][0], 0.0, q['lage'][2]]
            p['wz0'][:] = torch.tensor(wz, dtype=torch.float32, device=self.dev)
            p['dwz'].data[:] = 0
            p['gier0'] = float(q['gier'])
            p['dg'].data[:] = 0
            p['lage0'][:] = torch.tensor(lg, dtype=torch.float32, device=self.dev)
            p['dl'].data[:] = 0


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default='zw/s1.json')
    a.add_argument('--von')
    a.add_argument('--pose', type=int, default=0, help='Schritte nur Pose')
    a.add_argument('--schritte', type=int, default=300)
    a.add_argument('--fein', type=int, default=0)
    a = a.parse_args()
    g, teile = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in teile), 'Teile', len(teile), 'Meister', sum(1 for p in teile if not isinstance(p.bind, int)))
    stand = json.load(open(os.path.join(HIER, a.von))) if a.von else None
    pa = Passung(g, teile, stand)
    if a.pose:
        pa.lauf(a.pose, 2, 1.5, form=False, tag='P')
    if a.schritte:
        pa.lauf(a.schritte, 2, 1.5, tag='F')
        pa.lauf(a.schritte // 2, 1, 1.0, lr=0.005, lr_c=2.0, lr_w=1.0, tag='f')
    if a.fein:
        pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
        pa.lauf(a.fein, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e')
    json.dump(pa.stand(), open(os.path.join(HIER, a.aus), 'w'))
    print('->', a.aus)
