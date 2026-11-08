#!/usr/bin/env python3
"""Riegel ruhige Haut K5e (Nutzer 2026-11-08, Messblatt ruhe/MESSBLATT_RUHE.md Abschnitt 4). Exit 1 bei Rot.

    python riegel_k5e.py <Variante> [--json aus.json]
    python riegel_k5e.py --gegenprobe [--json aus.json]     (K5c = Spiel-Asset, K5d = abgelehnt; Ruhe-Zeilen muessen dort ROT sein)

<Variante> = Ordner mit FORM.EMD/FORM.TIM/form.json (3 Posen-Clips), BAHNSTEIG_FORM.EMD/.TIM (28 Clips) und bau.json (par).

Kein Riegel verlangt am Video Texel- oder Fleckwerte (Memory reai-v2-texturen-nicht-aus-video-eichen): Ruhe-Zeilen messen die Farbfamilien
der Figur unter weissem Licht und in der Szene ohne Filter gegen Schwellen aus den ORIGINALEN (G4, EM30-Haut); Wunsch-Zeilen messen Albedo
der Material-Flaechen (Orte) und weisses Licht; die einzige Video-Zahl ist der EINE globale Helligkeitswert (W6, Bild 2 Figur-Mittel).
Die Kettenzeilen riegel_k5d (b) Schwanzspitze und (e) Buckel vergleichen Fleckwerte am Video und laufen darum nur als Info.

Zeilen (Soll aus dem Messblatt RUHE 4.1-4.4; Masse ruhe/skripte/ruhe_mass.py, weisses Licht = rl.weiss_id wie Familien-Tafel 07, Clip 2):
 R1 Sektoren gepoolt <= 5 | R2 farbfremde Flecken <= 7,4 je 10 000 px und Flaeche <= 0,64 % | R3 Farbtonspanne 90 % <= 76,8 Grad |
 R4 Block-Farbton <= 16,3 Grad (Mittel der 4 Winkel).
 S1-S4 Szene ohne Filter (v4f.szene_id, Figur ohne Zahn/F1/Auge/Maulinneres): B1 S5 <= 5, Spanne <= 58,9, Block <= 12,25; B2 Block <= 10,9.
 H1 TIM-Pixel bytegleich Spiel-Asset K5c | H2 UV/Seite/Eckreihenfolge je Flaeche = K5c | H3 Zeilen 1-7 = kf_k5e.zeilen(par) aus den
 G4-Originalrampen (CDEMD0.EMS), Zeilen 0/8/9 = G4-0 / K5c-5 / K5c-7 | H4 je Material genutzte Eintraege (C* >= 3) Farbtonspanne <= 15 Grad |
 H5 Rangfolge je Zeile und Klasse 0 Vertauschungen gegen G4, Adern texelgewichtet dunkler und nicht roter als die Haut (RV6).
 W1 Gelenk heller als Nachbarglied (weiss, Ring 6 px): dL* >= 5 je Winkel, Mittel >= 6 | W1b Albedo C* <= 8, L* >= Nachbar + 6 |
 W2 Schwanzspitze Mesh 6 Fleischanteil >= 50 %, dL >= +5 | W3 Buckel 12 Flaechen, 1 Komponente, Ton -8 +- 15, Fleischanteil >= 40 %,
 dL >= +3 | W4 Lippe + Unterkiefer aussen Rotklasse 0 %, L*-p95 <= Haut-p95 | W5 Krallen = Hand dE76 <= 6, Rotklasse 0 % |
 W6 Bild 2 Figur Video >= Vorlage 15,37 und ohne Filter >= 7,33 (Pixelmenge V4F 3.1) | W7 F1-Zeile = K5c-Zeile 5, 6 Flaechen, UV gleich.
 T1 Zeilen <= 16, Port-Texturhoehe <= 4096 (riegel_einbau.ez1) | T2 jede Flaeche gueltige Zeile (ez2), kein neues 0x0000 |
 T3 Ecken/Normalen = K5d, Eckindizes = K5c | T4 Asset EDD/EMR/Morph byte-gleich Spiel-Asset, 32/32/28, MD1 = FORM.EMD, TIM = FORM.TIM.
"""
import argparse
import json
import math
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import ruhe_mass as M  # noqa: E402
import ruhe_wunsch as W  # noqa: E402
import ruhe_material as RM  # noqa: E402
import kf_k5e as KE  # noqa: E402
import riegel_k5d as RK  # noqa: E402
import riegel_einbau as RE  # noqa: E402
FE = KE.FE

SONDER = ('Zahn', 'F1', 'Auge', 'Maulinneres')
STAENDE = {'K5c': (KE.BASIS, KE.ASSET), 'K5d': (KE.K5D, os.path.join(KE.K5D, 'BAHNSTEIG_FORM'))}
RUHE_ZEILEN = ('R1', 'R2', 'R3', 'R4')
SZENE_ZEILEN = ('S1', 'S2', 'S3', 'S4')


def r1(x):
    return [round(float(v), 1) for v in np.atleast_1d(x)]


def kreisspanne_voll(h):
    s = np.sort(np.mod(h, 360.0))
    if len(s) < 2:
        return 0.0
    gaps = np.diff(np.concatenate([s, [s[0] + 360.0]]))
    return float(360.0 - gaps.max())


class Lauf:
    def __init__(self, ordner, asset, vor, ref, mt):
        self.ordner, self.asset, self.vor, self.ref, self.mt = ordner, asset, vor, ref, mt
        self.z = []

    def zeile(self, kz, nm, wert, ok, info=False):
        self.z.append(dict(kz=kz, name=nm, wert=wert, ok=None if info else bool(ok)))

    # ------------------------------------------------ Ruhe (weisses Licht)
    def ruhe(self):
        mo = rl.fl.lade_var(self.ordner)
        ws = []
        for _wn, az, el in rl.WINKEL4:
            img, K = rl.weiss_id(mo, 2, az, el)
            ws.append(M.masse(img, K['maske'], 2))
        mi = M.mittel(ws)
        acc = np.zeros(24)
        for d in ws:
            for s_, p in d['sektoren'].items():
                acc[int(s_) // 15] += p * d['px'] * d['bunt_pct'] / 1e4
        acc /= acc.sum()
        s5 = int((acc >= 0.05).sum())
        self.weiss = dict(mittel=mi, je_winkel=ws, S5_gepoolt=s5)
        self.zeile('R1', 'R1 Sektoren gepoolt (weiss) <= 5', '%d (je Winkel %s)' % (s5, [d['S5'] for d in ws]), s5 <= 5)
        self.zeile('R2', 'R2 farbfremde Flecken (weiss) <= 7,4 je 10k und <= 0,64 %', '%.1f / %.2f %%' % (mi['F_fremd'], mi['fremd_pct']),
                   mi['F_fremd'] <= 7.4 and mi['fremd_pct'] <= 0.64)
        self.zeile('R3', 'R3 Farbtonspanne 90 % (weiss) <= 76,8 Grad', '%.1f' % mi['spanne90'], mi['spanne90'] <= 76.8)
        self.zeile('R4', 'R4 Block-Farbton (weiss) <= 16,3 Grad', '%.1f' % mi['block_ton_streu'], mi['block_ton_streu'] <= 16.3)
        self.zeile('Info', 'Info Flecken alle (<= 148,6) / C*-Std (<= 16,9) / L* Mittel (weiss)', '%.1f / %.2f / %.1f' % (mi['F'], mi['C_std'], mi['L_mittel']),
                   True, info=True)

    # ------------------------------------------------ Szene ohne Filter
    def szene(self):
        codes = set()
        for nm in SONDER:
            for (me, vi, fi) in self.mt[nm]:
                codes.add(me * 1000000 + vi * 100000 + fi)
        codes = np.array(sorted(codes), np.int64)
        self.sz = {}
        for n in (1, 2):
            s, K = rl.v4f.szene_id(self.ordner, n)
            c = K['mesh'].astype(np.int64) * 1000000 + K['flaeche'].astype(np.int64)
            m = K['maske'] & ~np.isin(c, codes)
            self.sz[n] = M.masse(s, m, 2)
        b1, b2 = self.sz[1], self.sz[2]
        self.zeile('S1', 'S1 Bild 1 Sektoren (ohne Filter, ohne Sonder) <= 5', '%d' % b1['S5'], b1['S5'] <= 5)
        self.zeile('S2', 'S2 Bild 1 Farbtonspanne <= 58,9', '%.1f' % b1['spanne90'], b1['spanne90'] <= 58.9)
        self.zeile('S3', 'S3 Bild 1 Block-Farbton <= 12,25', '%.1f' % b1['block_ton_streu'], b1['block_ton_streu'] <= 12.25)
        self.zeile('S4', 'S4 Bild 2 Block-Farbton <= 10,9', '%.1f' % b2['block_ton_streu'], b2['block_ton_streu'] <= 10.9)
        self.zeile('Info', 'Info Szene F_fremd Bild 1 / Bild 2 (Originale 0-1,7; Messblatt 2.2 S7)', '%.1f / %.1f' % (b1['F_fremd'], b2['F_fremd']),
                   True, info=True)

    # ------------------------------------------------ Herkunft
    def herkunft(self):
        tv = FE.tim_lesen(open(os.path.join(self.ordner, 'FORM.TIM'), 'rb').read())
        ta = FE.tim_lesen(open(KE.ASSET + '.TIM', 'rb').read())
        dpx = int((tv['pixel'] != ta['pixel']).sum()) if tv['pixel'].shape == ta['pixel'].shape else -1
        self.zeile('H1', 'H1 TIM-Pixel bytegleich Spiel-Asset K5c (keine Insel)', '%d Texel abweichend (Form %s)' % (dpx, list(tv['pixel'].shape)),
                   dpx == 0)
        fv = FE.lese_emd(open(os.path.join(self.ordner, 'FORM.EMD'), 'rb').read())
        fc = FE.lese_emd(open(os.path.join(KE.BASIS, 'FORM.EMD'), 'rb').read())
        uv_neu = n = 0
        for a, b in zip(fv['meshes'], fc['meshes']):
            for art in ('dreiecke', 'vierecke'):
                for x, y in zip(a[art], b[art]):
                    n += 1
                    if x['uv'] != y['uv'] or (int(x['page']) & 0xF) != (int(y['page']) & 0xF) or x['e'] != y['e'] or \
                            (int(x['clut']) & 0x3F) != (int(y['clut']) & 0x3F):
                        uv_neu += 1
        self.zeile('H2', 'H2 UV/Seite/Eckreihenfolge je Flaeche = K5c (nur CLUT-Wort neu)', '%d/%d gleich, %d neu' % (n - uv_neu, n, uv_neu), uv_neu == 0)
        cl = [[int(x) for x in z] for z in tv['cluts']]
        self.cl = cl
        bau = os.path.join(self.ordner, 'bau.json')
        if len(cl) == KE.NZ and os.path.exists(bau):
            par = json.load(open(bau, encoding='utf-8'))['par']
            E = KE.eingaenge()
            worte, _zj, _i, _n, self.nutz = KE.zeilen(par, E['F'], E['mat'], E['quell'], E['cl_k5c'], E['cl_g4'])
            dz = {z: sum(1 for a, b in zip(worte[z], cl[z]) if a != b) for z in range(KE.NZ)}
            Zp = par['zeilen']
            sonder = (cl[Zp['g4_0']] == [int(x) for x in E['cl_g4'][0]], cl[Zp['f1']] == E['cl_k5c'][5], cl[Zp['zahn']] == E['cl_k5c'][7])
            self.par, self.E = par, E
            self.zeile('H3', 'H3 Zeilen 1-7 = G4-Rampe + EIN Materialsatz (neu berechnet), 0/8/9 bytegleich G4-0/K5c-5/K5c-7',
                       'Abweichung je Zeile %s; Z0=G4-0 %s, Z8=K5c-5 %s, Z9=K5c-7 %s' % (dz, *sonder), sum(dz.values()) == 0 and all(sonder))
        else:
            self.par = None
            self.zeile('H3', 'H3 Zeilen 1-7 = G4-Rampe + EIN Materialsatz (neu berechnet), 0/8/9 bytegleich G4-0/K5c-5/K5c-7',
                       '%d Zeilen, kein Materialsatz-Bau (Gruppen-/Inselzeilen)' % len(cl), False)
        # H4: je Material die genutzten Eintraege
        A, F = W.stand_albedo(self.ordner)
        self.A, self.F = A, F
        grp = {'Haut (inkl. Kralle/Lippe/Band)': self.mt['Haut'], 'Gelenk': self.mt['Gelenk'],
               'Fleisch': self.mt['Fleisch_Schwanzspitze'] + self.mt['Fleisch_Buckel']}
        rgb = np.stack([np.array(cl, np.int64) & 31, (np.array(cl, np.int64) >> 5) & 31, (np.array(cl, np.int64) >> 10) & 31], -1) * 255.0 / 31.0
        labz = RM.lab(rgb.reshape(-1, 3)).reshape(rgb.shape)
        res, ok4 = {}, True
        for nm, keys in grp.items():
            ein = set()
            for k in keys:
                z = F[k]['zeile']
                for e in np.unique(F[k]['idx']):
                    ein.add((z, int(e)))
            x = np.array([labz[z][e] for z, e in sorted(ein)])
            C = np.hypot(x[:, 1], x[:, 2])
            h = np.degrees(np.arctan2(x[C >= 3, 2], x[C >= 3, 1]))
            sp = kreisspanne_voll(h)
            res[nm] = '%.1f Grad (%d von %d Eintraegen C* >= 3)' % (sp, int((C >= 3).sum()), len(x))
            ok4 &= sp <= 15.0
        self.zeile('H4', 'H4 ein Farbton je Material: Spanne genutzter Eintraege (C* >= 3) <= 15 Grad', str(res), ok4)
        # H5: Rangfolge
        if self.par is not None:
            Zp = self.par['zeilen']
            quelle = {Zp['haut'][s]: s for s in range(3)}
            quelle.update({Zp['gelenk'][s]: s for s in range(3)})
            quelle[Zp['fleisch']] = 1
            inv, rv6 = {}, {}
            for z, s in sorted(quelle.items()):
                L0, haut, hell, ader = KE.klassen([int(x) for x in self.E['cl_g4'][s]])
                Ln = labz[z]
                n_inv = 0
                for kl in (haut, hell, ader):
                    ii = np.nonzero(kl)[0]
                    for a_ in ii:
                        for b_ in ii:
                            if L0[a_, 0] < L0[b_, 0] - 1e-9 and Ln[a_, 0] > Ln[b_, 0] + 1e-9:
                                n_inv += 1
                inv[z] = n_inv
                # RV6 texelgewichtet ueber die Nutzung dieser Zeile
                cnt = np.zeros(256)
                for k, d in F.items():
                    if d['zeile'] == z:
                        np.add.at(cnt, d['idx'], 1)
                wh, wa = cnt * haut, cnt * ader
                if wa.sum() > 0 and wh.sum() > 0:
                    mh = (Ln * wh[:, None]).sum(0) / wh.sum()
                    ma = (Ln * wa[:, None]).sum(0) / wa.sum()
                    rv6[z] = (round(float(ma[0] - mh[0]), 2), round(float(ma[1] - mh[1]), 2))
            ok5 = sum(inv.values()) == 0 and all(dl < 0 and da <= 0 for dl, da in rv6.values())
            self.zeile('H5', 'H5 Rangfolge je Zeile/Klasse 0 Vertauschungen; Adern dunkler, nicht roeter (dL < 0, da* <= 0, RV6)',
                       'Vertauschungen %s; Ader-Haut (dL, da*) %s' % (inv, rv6), ok5)
        else:
            self.zeile('H5', 'H5 Rangfolge (nur 10-Zeilen-Bau pruefbar)', 'n/a (Gruppenzeilen)', True, info=True)

    # ------------------------------------------------ Wuensche
    def wuensche(self):
        mo = rl.fl.lade_var(self.ordner)
        e = W.wuensche(self.A, mo, 2, self.mt)
        self.wu = e
        gw = e['gelenk_weiss']
        self.zeile('W1', 'W1 Gelenk heller als Nachbarglied (weiss): dL* >= 5 je Winkel, Mittel >= 6',
                   'min %s / Mittel %s (%s)' % (gw['dL_min'], gw['dL_mittel'], [x['dL'] for x in gw['je_winkel']]),
                   gw['dL_min'] is not None and gw['dL_min'] >= 5 and gw['dL_mittel'] >= 6)
        ga = e['gelenk_albedo']
        self.zeile('W1b', 'W1b Gelenk weisslich: Albedo C* <= 8 und L* >= Nachbar + 6', 'C* %.1f, dL %+.1f (Gelenk %s, Nachbar %s)'
                   % (ga['C_gelenk'], ga['dL_median'], ga['gelenk'], ga['nachbar']), ga['C_gelenk'] <= 8 and ga['dL_median'] >= 6)
        sp = e['schwanzspitze']
        self.zeile('W2', 'W2 Schwanzspitze Mesh 6: Fleischanteil >= 50 %, dL >= +5 gegen Schwanz', '%.1f %%, %+.1f (Spitze %s)'
                   % (sp['fleisch_anteil'], sp['dL'], sp['albedo_median']), sp['fleisch_anteil'] >= 50 and sp['dL'] >= 5)
        bu = e['buckel']
        komp = self.buckel_komponenten()
        self.zeile('W3', 'W3 Buckel: 12 Flaechen, 1 Komponente, Ton -8 +- 15, Fleischanteil >= 40 %, dL >= +3',
                   '%d Flaechen, %d Komponente(n), Ton %.1f, %.1f %%, %+.1f (Buckel %s)' % (bu['flaechen'], komp, bu['ton'], bu['fleisch_anteil'], bu['dL'],
                                                                                       bu['albedo_median']),
                   bu['flaechen'] == 12 and komp == 1 and abs(bu['ton'] + 8) <= 15 and bu['fleisch_anteil'] >= 40 and bu['dL'] >= 3)
        li = e['lippe_unterkiefer']
        self.zeile('W4', 'W4 Lippe + Unterkiefer aussen (165 Fl.): Rotklasse 0 %, L*-p95 <= Haut-p95', '%.1f %%, %.1f <= %.1f (Median %s)'
                   % (li['rot_pct'], li['L_p95'], li['haut_L_p95'], li['median']), li['rot_pct'] == 0 and li['L_p95'] <= li['haut_L_p95'])
        kr = e['krallen']
        self.zeile('W5', 'W5 Krallen = Hand: Albedo dE76 <= 6, Rotklasse 0 %', 'dE %.2f, Rot %.1f %% (Kralle %s, Hand %s)'
                   % (kr['dE'], kr['rotklasse_pct'], kr['kralle'], kr['hand']), kr['dE'] <= 6 and kr['rotklasse_pct'] == 0)

    def buckel_komponenten(self):
        fv = FE.lese_emd(open(os.path.join(self.ordner, 'FORM.EMD'), 'rb').read())
        fl = [set(fv['meshes'][me]['vierecke' if vi else 'dreiecke'][fi]['e']) for me, vi, fi in self.mt['Fleisch_Buckel']]
        eltern = list(range(len(fl)))

        def wurzel(i):
            while eltern[i] != i:
                i = eltern[i]
            return i
        for i in range(len(fl)):
            for j in range(i + 1, len(fl)):
                if fl[i] & fl[j]:
                    eltern[wurzel(i)] = wurzel(j)
        return len({wurzel(i) for i in range(len(fl))})

    # ------------------------------------------------ Helligkeit + Kette (Info)
    def kette(self):
        st = RK.Stand(self.ordner)
        vor, ka = self.vor, self.ref
        fig = {n: RK.nutzer_union(n) & ka.d[n]['K']['maske'] & ~vor.d[n]['clip'] for n in (1, 2)}
        h = {}
        for n in (1, 2):
            for k in ('LV', 'LS', 'LT'):
                h[(n, k)] = (float(st.d[n][k][fig[n]][:, 0].mean()), float(vor.d[n][k][fig[n]][:, 0].mean()))
        self.hell = {'bild%d_%s' % k: [round(v[0], 2), round(v[1], 2)] for k, v in h.items()}
        v2, s2 = h[(2, 'LV')], h[(2, 'LS')]
        self.zeile('W6', 'W6 Helligkeit Bild 2 Figur: Video >= Vorlage und ohne Filter >= Vorlage (Pixelmenge V4F 3.1)',
                   'Video %.2f (>= %.2f), ohne %.2f (>= %.2f)' % (v2[0], v2[1], s2[0], s2[1]), v2[0] >= v2[1] - 0.005 and s2[0] >= s2[1])
        self.zeile('Info', 'Info Helligkeit Bild 1 Video / ohne Filter (Vorlage; Messblatt Offen 1)', '%.2f (%.2f) / %.2f (%.2f)'
                   % (h[(1, 'LV')][0], h[(1, 'LV')][1], h[(1, 'LS')][0], h[(1, 'LS')][1]), True, info=True)
        # riegel_k5d (b)/(e) in der Kette: Fleck-/Videovergleich -> nur Info (Auftrag: kein Riegel verlangt am Video Fleckwerte)
        K1 = ka.d[1]['K']
        import cv2
        schwanz = cv2.imread(RK.NT % 'tail', 0) > 0
        m6 = (K1['mesh'] == 6) & schwanz & ~vor.d[1]['clip']
        ra, ro = RK.rot_anteil(st.d[1]['LT'], m6), RK.rot_anteil(vor.d[1]['LT'], m6)
        self.zeile('Info', 'Info riegel_k5d (b) Schwanzspitze Rot-Anteil Kette T (K5d-Fenster 25..50 %)', '%.1f %% (Vorlage %.1f) -> %s'
                   % (ra, ro, 'im Fenster' if 25 <= ra <= 50 else 'ausserhalb'), True, info=True)
        for fid in ('B2-F01', 'B2-F02'):
            m = RK.FLECKEN[fid] & ~vor.d[2]['clip']
            lv, lo = RK.m_lab(st.d[2]['LT'], m), RK.m_lab(vor.d[2]['LT'], m)
            hv, ho = np.degrees(np.arctan2(lv[2], lv[1])), np.degrees(np.arctan2(lo[2], lo[1]))
            ok = lv[0] >= 0.8 * lo[0] and lv[1] >= 0.7 * lo[1] and abs(hv - ho) <= 20
            self.zeile('Info', 'Info riegel_k5d (e) Buckel %s Kette T (L >= 80 %%, a* >= 70 %%, Ton +-20)' % fid, '%s h %.0f (Vorlage %s h %.0f) -> %s'
                       % (r1(lv), hv, r1(lo), ho, 'erfuellt' if ok else 'nicht erfuellt'), True, info=True)

    # ------------------------------------------------ F1 + Technik
    def technik(self):
        A_c = W.stand_albedo(KE.BASIS)[1]
        f1 = self.mt['F1']
        z8 = {self.F[k]['zeile'] for k in f1}
        tc = FE.tim_lesen(open(os.path.join(KE.BASIS, 'FORM.TIM'), 'rb').read())
        k5 = [int(x) for x in tc['cluts'][5]]
        uv_gl = all(np.array_equal(self.F[k]['idx'], A_c[k]['idx']) for k in f1)
        ok7 = len(f1) == 6 and len(z8) == 1 and self.cl[list(z8)[0]] == k5 and uv_gl
        self.zeile('W7', 'W7 F1: Zeile = K5c-Zeile 5 bytegleich, 6 Flaechen, UV gleich', '%d Flaechen, Zeile %s, bytegleich %s, UV gleich %s'
                   % (len(f1), sorted(z8), self.cl[list(z8)[0]] == k5, uv_gl), ok7)
        timb = open(os.path.join(self.ordner, 'FORM.TIM'), 'rb').read()
        emdb = open(os.path.join(self.ordner, 'FORM.EMD'), 'rb').read()
        ok1, t1 = RE.ez1(timb)
        self.zeile('T1', 'T1 Zeilen <= 16, Port-Texturhoehe <= 4096 (riegel_einbau E-Z1)', t1, ok1)
        ok2, t2 = RE.ez2(emdb, timb)
        null_neu = 0
        if self.par is not None:
            Zp = self.par['zeilen']
            for s in range(3):
                g4 = [int(x) for x in self.E['cl_g4'][s]]
                for z in (Zp['haut'][s], Zp['gelenk'][s]) + ((Zp['fleisch'],) if s == 1 else ()):
                    null_neu += sum(1 for a, b in zip(g4, self.cl[z]) if a != 0 and b == 0)
        self.zeile('T2', 'T2 jede Flaeche liest gueltige Zeile (E-Z2), kein neues 0x0000', '%s; neue 0x0000: %d' % (t2, null_neu), ok2 and null_neu == 0)
        fv = FE.lese_emd(emdb)
        fd = FE.lese_emd(open(os.path.join(KE.K5D, 'FORM.EMD'), 'rb').read())
        fc = FE.lese_emd(open(os.path.join(KE.BASIS, 'FORM.EMD'), 'rb').read())
        geo = [mi for mi, (a, b) in enumerate(zip(fv['meshes'], fd['meshes'])) if a['ecken'] != b['ecken'] or a['normalen'] != b['normalen']]
        idx = all(x['e'] == y['e'] and x['n'] == y['n'] for a, b in zip(fv['meshes'], fc['meshes']) for art in ('dreiecke', 'vierecke')
                  for x, y in zip(a[art], b[art]))
        self.zeile('T3', 'T3 Ecken/Normalen = K5d (Krallen 19/20/28/29 eingezogen), Eckindizes = K5c', 'Meshes mit anderer Geometrie als K5d %s; '
                   'Eckindizes = K5c %s' % (geo, idx), not geo and idx)
        a_v = open(self.asset + '.EMD', 'rb').read()
        sv, sr = FE.sektionen(a_v), FE.sektionen(open(KE.ASSET + '.EMD', 'rb').read())
        gleich = [k for k in sv if k != 'md1' and sv[k] == sr[k]]
        fa = FE.lese_emd(a_v)
        md1_ok = FE.md1_lesen(sv['md1']) == fv['meshes']
        t_ok = open(self.asset + '.TIM', 'rb').read() == timb
        self.zeile('T4', 'T4 Asset EDD/EMR/Morph byte-gleich Spiel-Asset, 32/32/28, MD1 = FORM.EMD, TIM = FORM.TIM',
                   'gleich %s; Meshes %d Knochen %d Clips %d; MD1 %s; TIM %s' % (gleich, len(fa['meshes']), len(fa['knochen']), len(fa['clips']), md1_ok,
                                                                              t_ok),
                   len(gleich) == 7 and len(fa['meshes']) == 32 and len(fa['knochen']) == 32 and len(fa['clips']) == 28 and md1_ok and t_ok)

    def alles(self, kette=True):
        self.ruhe()
        self.szene()
        self.herkunft()
        self.wuensche()
        if kette:
            self.kette()
        self.technik()
        return self.z


def ausgabe(titel, z, f):
    g = n = 0
    f.write('== Riegel ruhige Haut K5e: %s\n' % titel)
    for x in z:
        st = 'Info' if x['ok'] is None else ('GRUEN' if x['ok'] else 'ROT')
        if x['ok'] is not None:
            n += 1
            g += x['ok']
        f.write('  %-92s %-6s %s\n' % (x['name'], st, x['wert']))
    f.write('  -> %d/%d bindende Zeilen GRUEN\n' % (g, n))
    return g, n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?')
    ap.add_argument('--json')
    ap.add_argument('--gegenprobe', action='store_true')
    a = ap.parse_args()
    vor = RK.Vorlage()
    ref = RK.Stand(KE.BASIS)
    mt = W.mat()
    if a.gegenprobe:
        aus, fehlt = {}, []
        for nm, (o, asset) in STAENDE.items():
            z = Lauf(o, asset, vor, ref, mt).alles()
            g, n = ausgabe('Gegenprobe %s (%s)' % (nm, o), z, sys.stdout)
            rot = {x['kz'] for x in z if x['ok'] is False}
            nicht_rot = [k for k in RUHE_ZEILEN if k not in rot]
            if nicht_rot:
                fehlt.append('%s: %s nicht ROT' % (nm, nicht_rot))
            if not rot & set(SZENE_ZEILEN):
                fehlt.append('%s: keine Szenenzeile S1-S4 ROT' % nm)
            print('  %s ROT-Zeilen: %s' % (nm, sorted(rot)))
            aus[nm] = dict(zeilen=z, gruen=g, bindend=n, rot=sorted(rot))
        print('  Gegenprobe (R1-R4 ROT und mind. eine S-Zeile ROT je Stand): %s' % ('erfuellt' if not fehlt else fehlt))
        if a.json:
            json.dump(aus, open(a.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        print('EXIT %d' % (1 if fehlt else 0))
        sys.exit(1 if fehlt else 0)
    lauf = Lauf(a.variante, os.path.join(a.variante, 'BAHNSTEIG_FORM'), vor, ref, mt)
    z = lauf.alles()
    g, n = ausgabe(a.variante, z, sys.stdout)
    if a.json:
        json.dump(dict(zeilen=z, gruen=g, bindend=n, weiss=lauf.weiss, szene=lauf.sz, wuensche=lauf.wu, helligkeit=lauf.hell), open(a.json, 'w', encoding='utf-8'),
                  indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('EXIT %d' % (0 if g == n else 1))
    sys.exit(0 if g == n else 1)


if __name__ == '__main__':
    main()
