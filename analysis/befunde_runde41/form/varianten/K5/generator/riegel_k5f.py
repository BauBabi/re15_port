#!/usr/bin/env python3
"""Riegel K5f - nicht blass (Nutzer 2026-11-09, Messblatt k5f/MESSBLATT_K5F.md Abschnitt 4). Exit 1 bei Rot.

    python riegel_k5f.py <Variante> [--spiel spiel.json] [--json aus.json]
    python riegel_k5f.py --gegenprobe [--json aus.json]     (K5e_f1 = Spiel-Asset, K5c = einbau_k5c; die Messblatt-Gegenproben muessen ROT sein)

<Variante> = Ordner mit FORM.EMD/FORM.TIM/form.json, BAHNSTEIG_FORM.EMD/.TIM und bau.json (par). Standard fuer --spiel: <Variante>/spiel_k5f.json
(kf_k5f_spiel.sh + kf_k5f_spielmass.py); der Riegel nimmt die Spielwerte nur an, wenn deren TIM-/EMD-sha256 = Variante.

Zeilen (Soll = Messblatt 4.1-4.6, Baender = min..max der Originale, keine Erweiterung):
 Z1-Z9 Spiel 5080 Bild-1-Lage, Bilder 360-440, Figurmaske aus eigenem Normal-/Schluessel-Paar: P L* 5,57..18,80 | P L-cv 0,481..1,431 |
   P lokal_rel 0,243..0,656 | P C* 4,41..7,59 | P C-std 2,39..7,45 | Q L-cv 0,369..1,020 | Q lokal_rel 0,210..0,528 | Q C* 12,09..15,05 |
   Q C-std 6,53..8,88.
 WK1-WK4 weisses Licht (rl.weiss_id, Tafel-07-Licht, 4 Winkel, Clip 2, 380 px; k5f/skripte/fam.alles): L-cv 0,409..1,128 | lokal_rel 0,175..0,460 |
   C-std 4,78..13,55 | hell % <= 17,8 und F_hell <= 71,7. W-C* nur Info (8,63..23,41).
 R1-R4 Ruhe unveraendert (riegel_k5e.Lauf.ruhe, MESSBLATT_RUHE): S5 gepoolt <= 5, F_fremd <= 7,4 (<= 0,64 %), Spanne <= 76,8, Block <= 16,3.
 H1 TIM-Pixel = Spiel-Asset K5c | H2 UV/Seite/Eckreihenfolge = K5c | H3 Zeilen = kf_k5f.zeilen(par) neu berechnet (0/9 bytegleich G4-0/K5c-7,
   8 Kern = K5c-5) | H4 je Material (Haut, Grau, Gelenk, Fleisch) genutzte Eintraege mit C* >= 3: Farbtonspanne <= 15 Grad |
   H5 Rangfolge je Zeile/Klasse 0 Vertauschungen, Adern dunkler und nicht roeter als die Haut (RV6).
 HU1 Mesh 22 qf 1/8/9/194/201/209 lesen eine Hautzeile auf G4-Seite 0 (nicht Zeile 0) | HU2 weiss, 5 Ansichten von unten (k5f/skripte/hals4.py):
   warm-rot (a* > 12, C* > 15, Ton > -10) auf Mesh 21-23 ausser Zahn und den 29 echten Maulinneres-Flaechen = 0 | HU3 Spiel 5090 Pose Clip 27
   Bild 20 an den Zeile-0-Orten des K5e_f1: warm-rot (a* > 6, Ton > -10) = 0 | HU4 Rundum (12 x 6 Winkel, 400 px; hals3.py): jede Flaeche auf
   Zeile 0 ausser Auge <= 40 px im Ruhe-Clip 2 (die Messblatt-Zahlen 705-2879 / max 38 sind die Clip-2-Spalte von maulinneres_sicht.json);
   Clip 0 als Info (dort 22|1|98 mit 48 px, schon im Messblatt-Lauf).
 W1 Gelenk weiss dL >= 5 je Winkel, Mittel >= 6 | W1b Albedo C* <= 8, L* >= Nachbar + 6 | W2 Schwanzspitze Fleischanteil >= 50 %, dL >= +6,89 |
   W3 Buckel 12 Flaechen, 1 Komponente, Ton -8 +- 15, Anteil >= 40 %, dL >= +4,77 | W4 Lippe/Unterkiefer Rotklasse 0 %, L-p95 <= Haut-p95 |
   W5 Krallen dE76 <= 6, Rotklasse 0 % | W6 Bild 2 Video >= 15,37 (riegel_k5e.Lauf.kette) | W7 F1-Kern bytegleich K5c-Zeile 5, 6 Flaechen, UV gleich |
   W8 F1-Rand: Nicht-Kern-F1 gegen Mesh-0-Nachbarhaut (13x13-Ring), weiss "Seite" 500 px (f1ring.py) >= -lokal5 der Figur (weiss, Mittel 4 Winkel).
 T1 Zeilen <= 16, Hoehe <= 4096 | T2 gueltige Zeile je Flaeche, 0x0000 nur wo G4 0x0000 | T3 Geometrie = K5d, Eckindizes = K5c |
 T4 EDD/EMR/Morph = Spiel-Asset, 32/32/28, MD1 = FORM.EMD, TIM = FORM.TIM.
Kein Riegel eicht am Video Texel-, Fleck- oder Regionswerte (Memory reai-v2-texturen-nicht-aus-video-eichen); die einzige Videozahl ist W6.
"""
import argparse
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
sys.path.insert(0, os.path.join(K5, 'k5f', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import ruhe_wunsch as W  # noqa: E402
import ruhe_material as RM  # noqa: E402
import fam  # noqa: E402
import kf_k5e as KE  # noqa: E402
import kf_k5f as KF  # noqa: E402
import riegel_k5e as R5  # noqa: E402
import riegel_k5d as RK  # noqa: E402
import riegel_einbau as RE  # noqa: E402
FE = KE.FE

ZB = (('Z1', 'P L*', 'L_mittel', 5.57, 18.80), ('Z2', 'P L-cv', 'L_cv', 0.481, 1.431), ('Z3', 'P Lokalkontrast lokal5_rel', 'lokal5_rel', 0.243, 0.656),
      ('Z4', 'P C*', 'C_mittel', 4.41, 7.59), ('Z5', 'P C-std', 'C_std', 2.39, 7.45), ('Z6', 'Q L-cv', 'Q_L_cv', 0.369, 1.020),
      ('Z7', 'Q Lokalkontrast', 'Q_lokal5_rel', 0.210, 0.528), ('Z8', 'Q C*', 'Q_C_mittel', 12.09, 15.05), ('Z9', 'Q C-std', 'Q_C_std', 6.53, 8.88))
AUSSEN = {(22, 1, 1), (22, 1, 8), (22, 1, 9), (22, 1, 194), (22, 1, 201), (22, 1, 209)}
UNTEN5 = ((0, -85), (90, -45), (45, -40), (0, -60), (-90, -45))
GEGEN = {
    # Messblatt 4: diese Zeilen muessen in der Gegenprobe ROT sein
    'K5e_f1': ('Z2', 'Z3', 'Z4', 'Z5', 'Z6', 'Z7', 'Z8', 'Z9', 'WK1', 'WK2', 'WK3', 'HU1', 'HU2', 'HU3', 'HU4'),
    'K5c': ('Z1', 'Z4', 'Z5', 'Z8', 'Z9', 'R1', 'R2', 'R3', 'R4', 'HU1', 'HU2'),
}


def rnd(x, n=3):
    return None if x is None else round(float(x), n)


class Lauf(R5.Lauf):
    def __init__(self, ordner, asset, vor, ref, mt, spiel):
        super().__init__(ordner, asset, vor, ref, mt)
        self.spiel_pfad = spiel
        self.erg = {}
        self.mo = rl.fl.lade_var(ordner)

    # ------------------------------------------------ Z1-Z9 Spiel
    def spiel(self):
        sha_v = KE.sha(self.asset + '.TIM')
        sha_e = KE.sha(self.asset + '.EMD')
        if not self.spiel_pfad or not os.path.exists(self.spiel_pfad):
            for kz, nm, _k, lo, hi in ZB:
                self.zeile(kz, '%s %s im Spiel 5080 in %.3g..%.3g' % (kz, nm, lo, hi), 'keine Spielmessung (%s)' % self.spiel_pfad, False)
            self.zeile('HU3', 'HU3 Spiel 5090 Clip 27 Bild 20: warm-rot an den Zeile-0-Orten K5e_f1 = 0', 'keine Spielmessung', False)
            return
        d = json.load(open(self.spiel_pfad, encoding='utf-8'))
        echt = d['gemessen_tim_sha256'] == sha_v and d['gemessen_emd_sha256'] == sha_e
        self.erg['spiel'] = dict(datei=self.spiel_pfad, sha_gleich=echt, f80=d['f80']['mittel'] if d.get('f80') else None,
                                 f90=d['f90']['mittel'] if d.get('f90') else None, hu3=d.get('hu3'))
        m = d['f80']['mittel'] if d.get('f80') else None
        for kz, nm, k, lo, hi in ZB:
            if m is None or not echt:
                self.zeile(kz, '%s %s im Spiel 5080 in %.3g..%.3g' % (kz, nm, lo, hi), 'keine gueltige Messung (sha gleich %s)' % echt, False)
                continue
            self.zeile(kz, '%s %s im Spiel 5080 (Bilder 360-440) in %.3g..%.3g' % (kz, nm, lo, hi), '%.3f (n %d, Maske %d px)' %
                       (m[k], d['f80']['n'], m['maske_px']), lo <= m[k] <= hi)
        if d.get('f90'):
            f = d['f90']['mittel']
            self.zeile('Info', 'Info 5090 Bild-2-Pose (Feuer in der Maske, nur Info): P L* / L-cv / C* / C-std / Q lokal_rel',
                       '%.2f / %.3f / %.2f / %.2f / %.3f' % (f['L_mittel'], f['L_cv'], f['C_mittel'], f['C_std'], f['Q_lokal5_rel']), True, info=True)
        h = d.get('hu3')
        if h:
            self.zeile('HU3', 'HU3 Spiel 5090 Clip 27 Bild 20: warm-rot (a* > 6, Ton > -10) an den Zeile-0-Orten des K5e_f1 = 0',
                       '%d / %d px (Lab-Mittel %s)' % (h['warm_rot_px'], h['orte_px'], h['lab_mittel']), echt and h['warm_rot_px'] == 0)
        else:
            self.zeile('HU3', 'HU3 Spiel 5090 Clip 27 Bild 20: warm-rot an den Zeile-0-Orten K5e_f1 = 0', 'kein Lauf', False)

    # ------------------------------------------------ WK weisses Licht
    def weiss_kontrast(self):
        ws = []
        for _wn, az, el in rl.WINKEL4:
            img, K = rl.weiss_id(self.mo, 2, az, el, 380)
            ws.append(fam.alles(img, K['maske']))
        m = {k: float(np.mean([w[k] for w in ws])) for k in ws[0] if isinstance(ws[0][k], (int, float))}
        self.erg['weiss'] = {k: rnd(v) for k, v in m.items()}
        self.lokal5 = m['lokal5']
        self.zeile('WK1', 'WK1 weiss L-cv in 0,409..1,128', '%.3f' % m['L_cv'], 0.409 <= m['L_cv'] <= 1.128)
        self.zeile('WK2', 'WK2 weiss Lokalkontrast lokal5_rel in 0,175..0,460', '%.3f' % m['lokal5_rel'], 0.175 <= m['lokal5_rel'] <= 0.460)
        self.zeile('WK3', 'WK3 weiss C-std in 4,78..13,55', '%.2f' % m['C_std'], 4.78 <= m['C_std'] <= 13.55)
        self.zeile('WK4', 'WK4 weiss hell % <= 17,8 und F_hell <= 71,7', '%.1f %% / %.1f' % (m['hell_pct'], m['F_hell']), m['hell_pct'] <= 17.8 and m['F_hell'] <= 71.7)
        self.zeile('Info', 'Info weiss L* / C* (Band 8,63..23,41, nur Info) / lokal5', '%.2f / %.2f / %.2f' % (m['L_mittel'], m['C_mittel'], m['lokal5']), True, info=True)

    # ------------------------------------------------ H Herkunft (K5f-Zeilenplan)
    def herkunft(self):
        tv = FE.tim_lesen(open(os.path.join(self.ordner, 'FORM.TIM'), 'rb').read())
        ta = FE.tim_lesen(open(KE.ASSET + '.TIM', 'rb').read())
        dpx = int((tv['pixel'] != ta['pixel']).sum()) if tv['pixel'].shape == ta['pixel'].shape else -1
        self.zeile('H1', 'H1 TIM-Pixel bytegleich Spiel-Asset K5c (keine Insel)', '%d Texel abweichend' % dpx, dpx == 0)
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
        self.zeile('H2', 'H2 UV/Seite/Eckreihenfolge je Flaeche = K5c (nur CLUT-Wort neu)', '%d/%d gleich' % (n - uv_neu, n), uv_neu == 0)
        cl = [[int(x) for x in z] for z in tv['cluts']]
        self.cl = cl
        X = KE.eingaenge()
        self.E = X
        bau = os.path.join(self.ordner, 'bau.json')
        par = json.load(open(bau, encoding='utf-8'))['par'] if os.path.exists(bau) else None
        self.par = par if par is not None and 'grau' in par and len(cl) == KF.NZ else None
        A, F = W.stand_albedo(self.ordner)
        self.A, self.F = A, F
        if self.par is not None:
            worte, _zj, _i, _n, _nu = KF.zeilen(self.par, X['F'], X['mat'], X['quell'], X['cl_k5c'], X['cl_g4'])
            dz = {z: sum(1 for a, b in zip(worte[z], cl[z]) if a != b) for z in range(KF.NZ)}
            Zp = self.par['zeilen']
            ke = KE.f1_kern_eintraege()
            sonder = (cl[Zp['g4_0']] == [int(x) for x in X['cl_g4'][0]], all(cl[Zp['f1']][i] == X['cl_k5c'][5][i] for i in ke),
                      cl[Zp['zahn']] == X['cl_k5c'][7])
            self.zeile('H3', 'H3 Zeilen = kf_k5f.zeilen(par) aus den G4-Originalrampen (neu berechnet); Z0 = G4-0, Z8-Kern = K5c-5, Z9 = K5c-7',
                       'Abweichung je Zeile %s; Z0 %s, Z8-Kern %s, Z9 %s' % (dz, *sonder), sum(dz.values()) == 0 and all(sonder))
        else:
            self.zeile('H3', 'H3 Zeilen = kf_k5f.zeilen(par) (12-Zeilen-Bau)', '%d Zeilen, kein K5f-Materialsatz' % len(cl), False)
        # H4: ein Farbton je Material (Material nach K5f-Plan, Eintraege der eigenen Zeilen)
        parm = self.par or json.load(open(KF.PAR, encoding='utf-8'))
        mk = KF.material_k5f(X['F'], X['mat'], X['quell'], parm)
        rgb = np.stack([np.array(cl, np.int64) & 31, (np.array(cl, np.int64) >> 5) & 31, (np.array(cl, np.int64) >> 10) & 31], -1) * 255.0 / 31.0
        labz = RM.lab(rgb.reshape(-1, 3)).reshape(rgb.shape)
        self.labz = labz
        res, ok4 = {}, True
        for nm, gruppe in (('Haut', ('Haut',)), ('Grau', ('Grau',)), ('Gelenk', ('Gelenk',)), ('Fleisch', ('Fleisch_Schwanzspitze', 'Fleisch_Buckel'))):
            ein = set()
            for k, (g, _s) in mk.items():
                if g in gruppe:
                    for e in np.unique(F[k]['idx']):
                        ein.add((F[k]['zeile'], int(e)))
            x = np.array([labz[z][e] for z, e in sorted(ein)])
            C = np.hypot(x[:, 1], x[:, 2])
            h = np.degrees(np.arctan2(x[C >= 3, 2], x[C >= 3, 1]))
            sp = R5.kreisspanne_voll(h)
            hm = float(np.degrees(np.angle(np.exp(1j * np.radians(h)).mean()))) if len(h) else float('nan')
            res[nm] = '%.1f Grad (Ton %.1f, %d von %d C* >= 3)' % (sp, hm, int((C >= 3).sum()), len(x))
            ok4 &= sp <= 15.0
        self.zeile('H4', 'H4 ein Farbton je Material: Spanne genutzter Eintraege (C* >= 3) <= 15 Grad', str(res), ok4)
        if self.par is not None:
            Zp = self.par['zeilen']
            quelle = {Zp['haut'][s]: s for s in range(3)}
            quelle.update({Zp['gelenk'][s]: s for s in range(3)})
            quelle.update({Zp['grau'][s]: s for s in range(len(Zp['grau']))})
            quelle[Zp['fleisch']] = 1
            inv, rv6 = {}, {}
            for z, s in sorted(quelle.items()):
                L0, haut, hell, ader = KE.klassen([int(x) for x in X['cl_g4'][s]])
                Ln = labz[z]
                n_inv = 0
                for kl in (haut, hell, ader):
                    ii = np.nonzero(kl)[0]
                    a0, a1 = L0[ii, 0], Ln[ii, 0]
                    n_inv += int(((a0[:, None] < a0[None, :] - 1e-9) & (a1[:, None] > a1[None, :] + 1e-9)).sum())
                inv[z] = n_inv
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
            self.zeile('H5', 'H5 Rangfolge (nur 12-Zeilen-K5f-Bau pruefbar)', 'n/a', False)

    # ------------------------------------------------ HU Hals-Unterseite
    def hals(self):
        z = {k: self.F[k]['zeile'] for k in AUSSEN}
        hz = set(self.par['zeilen']['haut']) if self.par else set()
        ok1 = self.par is not None and all(v == self.par['zeilen']['haut'][0] for v in z.values())
        self.zeile('HU1', 'HU1 Mesh 22 qf 1/8/9/194/201/209 lesen die Hautzeile der G4-Seite 0 (nicht Zeile 0)', 'Zeilen %s (Hautzeilen %s)' % (sorted(set(z.values())), sorted(hz)), ok1)
        echt_mi = set(self.mt['Maulinneres']) - AUSSEN
        ausnahme = echt_mi | set(self.mt['Zahn'])
        codes = np.array(sorted(m * 1000000 + v * 100000 + f for m, v, f in ausnahme), np.int64)
        rot, px, je = 0, 0, {}
        for az, el in UNTEN5:
            img, K = rl.weiss_id(self.mo, 2, az, el, 500)
            lab = RM.lab(img.reshape(-1, 3)).reshape(img.shape)
            c = K['mesh'].astype(np.int64) * 1000000 + K['flaeche'].astype(np.int64)
            m = K['maske'] & np.isin(K['mesh'], (21, 22, 23)) & ~np.isin(c, codes)
            r = m & (lab[..., 1] > 12) & (np.hypot(lab[..., 1], lab[..., 2]) > 15) & (np.degrees(np.arctan2(lab[..., 2], lab[..., 1])) > -10)
            je['%d/%d' % (az, el)] = int(r.sum())
            rot += int(r.sum())
            px += int(m.sum())
        self.erg['hu2'] = dict(rot=rot, px=px, je=je)
        self.zeile('HU2', 'HU2 weiss, 5 Ansichten von unten: warm-rot auf Mesh 21-23 ausser Zahn/29 echtem Maulinneren = 0', '%d px von %d (%s)' % (rot, px, je), rot == 0)
        z0 = [k for k, d in self.F.items() if d['zeile'] == 0 and k not in set(self.mt['Auge'])]
        codes0 = {m * 1000000 + v * 100000 + f: (m, v, f) for m, v, f in z0}
        cnt = {k: [0, 0] for k in z0}
        for ci, clip in enumerate((2, 0)):
            for az in range(0, 360, 30):
                for el in (-80, -45, -15, 15, 45, 80):
                    img, K = rl.weiss_id(self.mo, clip, az, el, 400)
                    c = (K['mesh'].astype(np.int64) * 1000000 + K['flaeche'].astype(np.int64))[K['maske']]
                    u, n = np.unique(c, return_counts=True)
                    for a, b in zip(u, n):
                        if int(a) in codes0:
                            cnt[codes0[int(a)]][ci] += int(b)
        mx = {'%d|%d|%d' % k: v[0] for k, v in cnt.items()}
        m0 = {'%d|%d|%d' % k: v[1] for k, v in cnt.items()}
        gross = {k: v for k, v in mx.items() if v > 40}
        g0 = {k: v for k, v in m0.items() if v > 40}
        self.erg['hu4'] = dict(flaechen=len(z0), max_clip2=max(mx.values()) if mx else 0, ueber_40_clip2=gross, max_clip0=max(m0.values()) if m0 else 0,
                               ueber_40_clip0=g0)
        self.zeile('HU4', 'HU4 Rundum (12 x 6 Winkel, Ruhe-Clip 2, 400 px): jede Zeile-0-Flaeche ausser Auge <= 40 px',
                   '%d Flaechen auf Zeile 0, max %d px; ueber 40: %s' % (len(z0), max(mx.values()) if mx else 0, dict(sorted(gross.items(), key=lambda t: -t[1]))),
                   not gross)
        self.zeile('Info', 'Info HU4 Clip 0 (Messblatt-Zahlen "705-2879" / "max 38" sind Clip 2; maulinneres_sicht.json Spalte 3 = Clip 0)',
                   'max %d px; ueber 40: %s' % (max(m0.values()) if m0 else 0, dict(sorted(g0.items(), key=lambda t: -t[1]))), True, info=True)

    # ------------------------------------------------ W Wuensche
    def wuensche(self):
        e = W.wuensche(self.A, self.mo, 2, self.mt)
        self.wu = e
        gw = e['gelenk_weiss']
        self.zeile('W1', 'W1 Gelenk heller als Nachbarglied (weiss): dL* >= 5 je Winkel, Mittel >= 6', 'min %s / Mittel %s' % (gw['dL_min'], gw['dL_mittel']),
                   gw['dL_min'] is not None and gw['dL_min'] >= 5 and gw['dL_mittel'] >= 6)
        ga = e['gelenk_albedo']
        self.zeile('W1b', 'W1b Gelenk weisslich: Albedo C* <= 8 und L* >= Nachbar + 6', 'C* %.2f, dL %+.2f' % (ga['C_gelenk'], ga['dL_median']),
                   ga['C_gelenk'] <= 8 and ga['dL_median'] >= 6)
        sp = e['schwanzspitze']
        self.zeile('W2', 'W2 Schwanzspitze Mesh 6: Fleischanteil >= 50 %, dL >= +6,89 (K4)', '%.1f %%, %+.2f (Spitze %s)' % (sp['fleisch_anteil'], sp['dL'], sp['albedo_median']),
                   sp['fleisch_anteil'] >= 50 and sp['dL'] >= 6.89)
        bu = e['buckel']
        komp = self.buckel_komponenten()
        self.zeile('W3', 'W3 Buckel: 12 Flaechen, 1 Komponente, Ton -8 +- 15, Anteil >= 40 %, dL >= +4,77 (K4)',
                   '%d Fl., %d Komp., Ton %.1f, %.1f %%, %+.2f' % (bu['flaechen'], komp, bu['ton'], bu['fleisch_anteil'], bu['dL']),
                   bu['flaechen'] == 12 and komp == 1 and abs(bu['ton'] + 8) <= 15 and bu['fleisch_anteil'] >= 40 and bu['dL'] >= 4.77)
        li = e['lippe_unterkiefer']
        self.zeile('W4', 'W4 Lippe + Unterkiefer: Rotklasse 0 %, L*-p95 <= Haut-p95', '%.1f %%, %.1f <= %.1f' % (li['rot_pct'], li['L_p95'], li['haut_L_p95']),
                   li['rot_pct'] == 0 and li['L_p95'] <= li['haut_L_p95'])
        kr = e['krallen']
        self.zeile('W5', 'W5 Krallen = Hand: Albedo dE76 <= 6, Rotklasse 0 %', 'dE %.2f, Rot %.1f %% (Kralle %s, Hand %s)' % (kr['dE'], kr['rotklasse_pct'], kr['kralle'], kr['hand']),
                   kr['dE'] <= 6 and kr['rotklasse_pct'] == 0)

    def f1(self):
        f1 = self.mt['F1']
        z8 = {self.F[k]['zeile'] for k in f1}
        tc = FE.tim_lesen(open(os.path.join(KE.BASIS, 'FORM.TIM'), 'rb').read())
        k5 = [int(x) for x in tc['cluts'][5]]
        A_c = W.stand_albedo(KE.BASIS)[1]
        uv_gl = all(np.array_equal(self.F[k]['idx'], A_c[k]['idx']) for k in f1)
        ke = KE.f1_kern_eintraege()
        gleich = len(z8) == 1 and all(self.cl[list(z8)[0]][i] == k5[i] for i in ke)
        self.zeile('W7', 'W7 F1: Kern-Eintraege (%d) bytegleich K5c-Zeile 5, 6 Flaechen, UV gleich' % len(ke),
                   '%d Flaechen, Zeile %s, Kern gleich %s, UV gleich %s' % (len(f1), sorted(z8), gleich, uv_gl), len(f1) == 6 and gleich and uv_gl)
        kern = set(ke)
        img, K = rl.weiss_id(self.mo, 2, 0, 8, 500)
        L = RM.lab(img.reshape(-1, 3)).reshape(img.shape)[..., 0]
        m = np.zeros(L.shape, bool)
        for me, q, fi in f1:
            m |= (K['mesh'] == me) & (K['flaeche'] == q * 100000 + fi)
        m &= K['maske']
        rk = m & ~np.isin(K['eintrag'], list(kern))
        nb = (cv2.dilate(m.astype(np.uint8), np.ones((13, 13), np.uint8)) > 0) & ~m & K['maske'] & (K['mesh'] == 0)
        dl = float(L[rk].mean() - L[nb].mean()) if rk.any() and nb.any() else float('nan')
        self.erg['w8'] = dict(f1_px=int(m.sum()), nicht_kern=rnd(L[rk].mean() if rk.any() else None, 2), nachbar=rnd(L[nb].mean() if nb.any() else None, 2),
                              dL=rnd(dl, 2), lokal5=rnd(self.lokal5, 2))
        self.zeile('W8', 'W8 F1-Rand (weiss Seite): Nicht-Kern-F1 gegen Mesh-0-Nachbarhaut >= -lokal5 der Figur',
                   'dL %+.2f gegen -%.2f (F1 %d px)' % (dl, self.lokal5, m.sum()), dl >= -self.lokal5)

    # ------------------------------------------------ T Technik
    def technik(self):
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
                zz = [Zp['haut'][s], Zp['gelenk'][s]] + ([Zp['grau'][s]] if s < len(Zp['grau']) else []) + ([Zp['fleisch']] if s == 1 else [])
                for z in zz:
                    null_neu += sum(1 for a, b in zip(g4, self.cl[z]) if a != 0 and b == 0)
        self.zeile('T2', 'T2 jede Flaeche liest gueltige Zeile (E-Z2), 0x0000 nur wo G4 0x0000', '%s; neue 0x0000: %d' % (t2, null_neu),
                   ok2 and null_neu == 0 and self.par is not None)
        fv = FE.lese_emd(emdb)
        fd = FE.lese_emd(open(os.path.join(KE.K5D, 'FORM.EMD'), 'rb').read())
        fc = FE.lese_emd(open(os.path.join(KE.BASIS, 'FORM.EMD'), 'rb').read())
        geo = [mi for mi, (a, b) in enumerate(zip(fv['meshes'], fd['meshes'])) if a['ecken'] != b['ecken'] or a['normalen'] != b['normalen']]
        idx = all(x['e'] == y['e'] and x['n'] == y['n'] for a, b in zip(fv['meshes'], fc['meshes']) for art in ('dreiecke', 'vierecke')
                  for x, y in zip(a[art], b[art]))
        self.zeile('T3', 'T3 Ecken/Normalen = K5d, Eckindizes = K5c', 'andere Geometrie als K5d %s; Eckindizes = K5c %s' % (geo, idx), not geo and idx)
        a_v = open(self.asset + '.EMD', 'rb').read()
        sv, sr = FE.sektionen(a_v), FE.sektionen(open(KE.ASSET + '.EMD', 'rb').read())
        gleich = [k for k in sv if k != 'md1' and sv[k] == sr[k]]
        fa = FE.lese_emd(a_v)
        md1_ok = FE.md1_lesen(sv['md1']) == fv['meshes']
        t_ok = open(self.asset + '.TIM', 'rb').read() == timb
        self.zeile('T4', 'T4 Asset EDD/EMR/Morph byte-gleich Spiel-Asset, 32/32/28, MD1 = FORM.EMD, TIM = FORM.TIM',
                   'gleich %s; Meshes %d Knochen %d Clips %d; MD1 %s; TIM %s' % (gleich, len(fa['meshes']), len(fa['knochen']), len(fa['clips']), md1_ok, t_ok),
                   len(gleich) == 7 and len(fa['meshes']) == 32 and len(fa['knochen']) == 32 and len(fa['clips']) == 28 and md1_ok and t_ok)

    def alles(self):
        self.spiel()
        self.weiss_kontrast()
        self.ruhe()
        self.herkunft()
        self.hals()
        self.wuensche()
        self.kette()
        self.f1()
        self.technik()
        ordnung = ['Z1', 'Z2', 'Z3', 'Z4', 'Z5', 'Z6', 'Z7', 'Z8', 'Z9', 'WK1', 'WK2', 'WK3', 'WK4', 'R1', 'R2', 'R3', 'R4', 'H1', 'H2', 'H3', 'H4', 'H5',
                  'HU1', 'HU2', 'HU3', 'HU4', 'W1', 'W1b', 'W2', 'W3', 'W4', 'W5', 'W6', 'W7', 'W8', 'T1', 'T2', 'T3', 'T4']
        rang = {k: i for i, k in enumerate(ordnung)}
        self.z.sort(key=lambda x: rang.get(x['kz'], 999))
        return self.z


def ausgabe(titel, z, f):
    g = n = 0
    f.write('== Riegel K5f nicht blass: %s\n' % titel)
    for x in z:
        st = 'Info' if x['ok'] is None else ('GRUEN' if x['ok'] else 'ROT')
        if x['ok'] is not None:
            n += 1
            g += x['ok']
        f.write('  %-100s %-6s %s\n' % (x['name'], st, x['wert']))
    f.write('  -> %d/%d bindende Zeilen GRUEN\n' % (g, n))
    f.flush()
    return g, n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?')
    ap.add_argument('--spiel')
    ap.add_argument('--json')
    ap.add_argument('--gegenprobe', action='store_true')
    a = ap.parse_args()
    vor = RK.Vorlage()
    ref = RK.Stand(KE.BASIS)
    mt = W.mat()
    sp_dir = os.path.join(K5, 'k5f', 'K5f')
    if a.gegenprobe:
        staende = {'K5e_f1': (os.path.join(K5, 'ruhe', 'K5e_f1'), os.path.join(K5, 'ruhe', 'K5e_f1', 'BAHNSTEIG_FORM'), os.path.join(sp_dir, 'spiel_k5e_f1.json')),
                   'K5c': (KE.BASIS, KE.ASSET, os.path.join(sp_dir, 'spiel_k5c.json'))}
        aus, fehlt = {}, []
        for nm, (o, asset, sp) in staende.items():
            lauf = Lauf(o, asset, vor, ref, mt, sp)
            z = lauf.alles()
            g, n = ausgabe('Gegenprobe %s (%s)' % (nm, o), z, sys.stdout)
            rot = {x['kz'] for x in z if x['ok'] is False}
            nicht = [k for k in GEGEN[nm] if k not in rot]
            if nicht:
                fehlt.append('%s: %s nicht ROT' % (nm, nicht))
            print('  %s ROT-Zeilen: %s; erwartet ROT (Messblatt 4): %s -> %s' % (nm, sorted(rot), list(GEGEN[nm]), 'erfuellt' if not nicht else 'FEHLT %s' % nicht))
            aus[nm] = dict(zeilen=z, gruen=g, bindend=n, rot=sorted(rot), erwartet_rot=list(GEGEN[nm]), erg=lauf.erg)
        print('  Gegenprobe gesamt: %s' % ('erfuellt' if not fehlt else fehlt))
        if a.json:
            json.dump(aus, open(a.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
        print('EXIT %d' % (1 if fehlt else 0))
        sys.exit(1 if fehlt else 0)
    v = a.variante
    lauf = Lauf(v, os.path.join(v, 'BAHNSTEIG_FORM'), vor, ref, mt, a.spiel or os.path.join(v, 'spiel_k5f.json'))
    z = lauf.alles()
    g, n = ausgabe(v, z, sys.stdout)
    if a.json:
        json.dump(dict(zeilen=z, gruen=g, bindend=n, erg=lauf.erg, weiss_ruhe=lauf.weiss, wuensche=lauf.wu, helligkeit=lauf.hell), open(a.json, 'w', encoding='utf-8'),
                  indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('EXIT %d' % (0 if g == n else 1))
    sys.exit(0 if g == n else 1)


if __name__ == '__main__':
    main()
