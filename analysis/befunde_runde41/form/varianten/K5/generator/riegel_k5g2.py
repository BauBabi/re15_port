#!/usr/bin/env python3
"""Riegel K5g2 (Auftrag K5g Runde 1+2: E1 Streifen, E2 Krallen, E3 Grundlage v2, V1-V6). Exit 1 bei Rot.

Runde 2: Geometrie-Riegel (E1) am fertigen Asset (unterteilt), Textur-/Flaechen-Riegel am Pruef-Stand <Variante>/pruef (gleiche Textur, Flaechen
vor der Unterteilung, Schluessel wie zonen_v2); T6 prueft, dass das Asset genau die Unterteilung des Pruef-Stands ist. Neu: H1 Haende, A1 Augen,
E2.1 Spiegel in der Bild-2-Haltung (Drehpunkt), E1.1 ohne Ausnahme-Clips.

    python riegel_k5g2.py <Variante> [--spiel spiel.json] [--keil keil.json] [--json aus.json]

Jede Zeile hat eine Gegenprobe, die ROT sein muss (K5e_f1 = Spiel-Asset, K5f = k5f/K5f, oder ein im Riegel gebauter Gegenstand); der Riegel
rechnet sie selbst und meldet sie in derselben Tabelle. Zeilen mit 'Info' oder 'Konflikt' blockieren nicht, sie werden genannt.
 E1.1 Keil-Pixel (Maler-Ordnung != Tiefe zwischen {22,23} und {10,21}) ueber ALLE Bilder ALLER Clips, Ansichten bild1/bild2/spiel5080/
      spiel5080_vorn/spiel5090 (kf_k5g2_geo.keil_alle) = 0 | E1.2 dasselbe ohne die Ausnahme-Clips der par | E1.3 bewegliche Kopf-/Kiefer-Ecken in
      Brust/Hals (Bedingungs-Bilder, Rand) = 0 | E1.4 Umriss der Figur (Teile 10/21/22/23, Tiefenpuffer) Bild 1/2: XOR-Pixel gegen K5e_f1 (Info)
 E2.1 rechte Krallen = Spiegel der linken im Handraum (Rastpose), Abweichung <= 1 (Rundung) | E2.2 Kralle 28 = K5c | E2.3 hinten (Fuesse 25/27,
      Unterschenkel 24/26) unveraendert | E2.4 Bild 2: sichtbare Pixel rechte Krallen / verdeckte Hand (Info)
 V3.1 Fleck-Material genau auf den Traegertexeln (traeger_texel.json) und auf keiner anderen Flaeche | V3.2 Texel-Konflikte (Info)
 V4.1 Zone 45 + Hals 21 + Lippe 42 + Unterkiefer 31 + Kopf-Unterseite: keine roten Texel (a* > 12, C* > 15, Ton > -10) | V4.2 dort keine schwarzen
      Texel (RGB-Max <= 12 ausser wo der G4-Eintrag selbst so dunkel ist) | V4.3 Maul-Zeilen nur von Zone 30 gelesen
 V2.1 Hautton (C-gewichtet, Hautflaechen, Albedo) im Band [14, 27] | V2.2 Haut-Buntheit (Median Hautklasse) 2-6 | V3.3 Buckel-Fleisch (Zone 27)
      L* 33-39, C* 22-26 | V5 Krallen: kein 0x7FFF, keine roten Texel | V1 Adern multiplikativ: L'/L_G4 je Hautzeile konstant (Achsenabschnitt
      |a| <= 1,5) | V6 F1-Kern bytegleich K5c-Zeile 5, F1-Rand (Nicht-Kern-Texel der F1-Flaechen) Median L* >= Mesh-0-Haut - 6
 W weisses Licht (4 Winkel, Clip 2): L-cv, lokal_rel, C-std, hell %/F_hell im Band der Originale (MESSBLATT_K5F 2) | R1-R4 Ruhe
 Z1-Z9 Spiel 5080 (kf_k5f_spiel.sh/kf_k5f_spielmass.py, nur wenn --spiel mit passender sha256)
 T1 Zeilen <= 16, Hoehe <= 4096 | T2 EDD/EMR/Morph = K5e_f1, Clips 28/3 | T3 Geometrie ausser 19/20/22/23/28/29/31 = K5e_f1 | T4 UV ausser Kopien/
      Spiegelkrallen = K5e_f1 | T5 jede Flaeche liest eine gueltige Zeile
"""
import argparse
import collections
import json
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
import fam  # noqa: E402
import ruhe_mass as M  # noqa: E402
import kf_k5e as E  # noqa: E402
import kf_g4 as KG  # noqa: E402
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402

FE = E.FE
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
K5F = os.path.join(K5, 'k5f', 'K5f')
K5E_ALT = os.path.join(K5, 'ruhe', 'K5e')
W_BAND = (('W1', 'L-cv', 'L_cv', 0.409, 1.128), ('W2', 'lokal_rel', 'lokal5_rel', 0.175, 0.460), ('W3', 'C-std', 'C_std', 4.78, 13.55))
ZB = (('Z1', 'P L*', 'L_mittel', 5.57, 18.80), ('Z2', 'P L-cv', 'L_cv', 0.481, 1.431), ('Z3', 'P lokal_rel', 'lokal5_rel', 0.243, 0.656),
      ('Z4', 'P C*', 'C_mittel', 4.41, 7.59), ('Z5', 'P C-std', 'C_std', 2.39, 7.45), ('Z6', 'Q L-cv', 'Q_L_cv', 0.369, 1.020),
      ('Z7', 'Q lokal_rel', 'Q_lokal5_rel', 0.210, 0.528), ('Z8', 'Q C*', 'Q_C_mittel', 12.09, 15.05), ('Z9', 'Q C-std', 'Q_C_std', 6.53, 8.88))


def lab_w(w):
    return KG.lab(KG.wort_rgb([int(x) for x in w]))


def rot(L):
    C = np.hypot(L[..., 1], L[..., 2]); h = np.degrees(np.arctan2(L[..., 2], L[..., 1]))
    return (L[..., 1] > 12) & (C > 15) & (h > -10)


class Stand:
    """Variante lesen: Form (BAHNSTEIG_FORM.EMD), TIM, Zeilen je Flaeche, Texel je Flaeche."""

    def __init__(self, ordner):
        self.ordner = ordner
        self.emd = open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
        self.tim = open(os.path.join(ordner, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
        self.form = FE.lese_emd(self.emd)
        self.T = FE.tim_lesen(self.tim)
        self.cl = [[int(x) for x in z] for z in self.T['cluts']]
        self.lab = [lab_w(z) for z in self.cl]
        self.fl = dict(TX.alle_flaechen(self.form))
        self.zeile = {k: ((int(f['clut']) >> 6) & 0x1FF) - self.T['clut_y'] for k, f in self.fl.items()}
        self._idx = {}

    def texel(self, k):
        """(ys, xs, Indizes) der Flaeche (UV-Maske wie kf_k5e.flaechen_idx)."""
        if k not in self._idx:
            ys, xs = np.nonzero(TX.flaeche_maske(self.fl[k], self.T['pixel'].shape))
            self._idx[k] = (ys, xs, self.T['pixel'][ys, xs])
        return self._idx[k]

    def farben(self, keys):
        """Lab aller Texel der Flaechen (je Texelzugriff), dazu die Worte."""
        L, W = [], []
        for k in keys:
            _y, _x, i = self.texel(k)
            z = self.zeile[k]
            L.append(self.lab[z][i]); W.append(np.array(self.cl[z])[i])
        if not L:
            return np.zeros((0, 3)), np.zeros(0, int)
        return np.vstack(L), np.concatenate(W)


class Riegel:
    def __init__(self):
        self.z = []

    def zeile(self, nr, text, ist, ok, art='Riegel', gegen=None):
        self.z.append(dict(nr=nr, text=text, ist=ist, ok=bool(ok), art=art, gegenprobe=gegen))
        g = '' if gegen is None else ' | Gegenprobe %s: %s (%s)' % (gegen[0], gegen[1], 'ROT ok' if not gegen[2] else 'GRUEN - Gegenprobe greift nicht!')
        print('%-5s %-6s %-4s %s: %s%s' % (nr, art, 'OK' if ok else 'ROT', text, ist, g), flush=True)


def zonen_listen(Z):
    zon = {z['nr']: z for z in Z['zonen']}
    return {nr: set(TX.schluessel(s) for s in zon[nr]['modell_zuordnung']['liste']) for nr in zon if zon[nr].get('modell_zuordnung')}


# ================================================================== E1
def keil_summe(r, ausn=()):
    a = sum(e['kopf_ueber_rumpf'] + e['rumpf_ueber_kopf'] for e in r if e['clip'] not in ausn)
    b = sum(1 for e in r if e['clip'] not in ausn and e['kopf_ueber_rumpf'] + e['rumpf_ueber_kopf'] > 0)
    je = collections.defaultdict(lambda: [0, 0])
    for e in r:
        if e['clip'] in ausn:
            continue
        je[e['ansicht']][0] += e['kopf_ueber_rumpf']; je[e['ansicht']][1] += e['rumpf_ueber_kopf']
    return a, b, dict(je)


def keil_laden(ordner, pfad):
    import hashlib
    sh = hashlib.sha256(open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'rb').read()).hexdigest()
    if pfad and os.path.exists(pfad):
        d = json.load(open(pfad, encoding='utf-8'))
        if d.get('emd_sha') == sh:
            return d['bilder']
    r = G.keil_alle(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), os.path.join(ordner, 'BAHNSTEIG_FORM.TIM'))
    if pfad:
        json.dump(dict(emd_sha=hashlib.sha256(open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'rb').read()).hexdigest(), bilder=r),
                  open(pfad, 'w', encoding='utf-8'))
    return r


def umriss(ordner, n):
    """Figur-Umriss (Teile 10/21/22/23, Tiefenpuffer) und sichtbarer Kopf (22/23) in Bild n, Buehnen-Kamera."""
    mo = FE.modell(open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'rb').read(), open(os.path.join(ordner, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
    st = {1: (0, 11), 2: (12, 0)}[n]
    mt, pt = G.keil_bild(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), os.path.join(ordner, 'BAHNSTEIG_FORM.TIM'), 'bild%d' % n, *st)
    return pt >= 0, np.isin(pt, (22, 23)), np.isin(pt, (10,))


def e1(R, var, par, keil_pfad):
    ausn = tuple(par['geometrie']['ausnahme_clips'])
    rv = keil_laden(var, keil_pfad)
    rg = keil_laden(K5E, os.path.join(os.path.dirname(keil_pfad or var), 'keil_k5e_f1.json') if keil_pfad else None)
    sv, bv, jv = keil_summe(rv); sg, bg, _ = keil_summe(rg)
    R.zeile('E1.1', 'Keil-Pixel alle Clips/Bilder, 5 Ansichten = 0', '%d px in %d Bild-Ansichten (je Ansicht [Kopf ueber Rumpf, Rumpf ueber Kopf] %s)'
            % (sv, bv, jv), sv == 0, gegen=('K5e_f1', '%d px in %d' % (sg, bg), sg == 0))
    sv2, bv2, jv2 = keil_summe(rv, ausn); sg2, bg2, _ = keil_summe(rg, ausn)
    R.zeile('E1.2', 'Keil-Pixel ohne die Clips %s (Info; Runde 2 gilt E1.1 fuer ALLE Clips)' % (list(ausn),), '%d px in %d (je Ansicht %s)' % (sv2, bv2, jv2),
            True, art='Info')
    je_clip = collections.defaultdict(int)
    for e in rv:
        je_clip[e['clip']] += e['kopf_ueber_rumpf'] + e['rumpf_ueber_kopf']
    R.zeile('E1.2c', 'Keil-Pixel je Clip (alle Ansichten)', '%s' % dict(sorted(je_clip.items())), True, art='Info')
    c12 = [(e['ansicht'], e['bild'], e['kopf_ueber_rumpf'] + e['rumpf_ueber_kopf']) for e in rv if e['clip'] == 12 and e['bild'] <= 3]
    R.zeile('E1.2d', 'Clip 12 Bild 0-3 (Bild-2-Haltung) je Ansicht', '%s' % c12, True, art='Info')
    # 12/0 (Bild 2) einzeln, Info
    b2 = [(e['ansicht'], e['kopf_ueber_rumpf'], e['rumpf_ueber_kopf']) for e in rv if e['clip'] == 12 and e['bild'] == 0]
    b2g = [(e['ansicht'], e['kopf_ueber_rumpf'], e['rumpf_ueber_kopf']) for e in rg if e['clip'] == 12 and e['bild'] == 0]
    R.zeile('E1.2b', 'Bild-2-Pose (Clip 12 Bild 0) je Ansicht [A, B]', '%s; K5e_f1 %s' % (b2, b2g), True, art='Info')
    # Ecken
    for nm, ordner in (('K5g2', os.path.join(var, 'pruef')), ('K5e_f1', K5E)):
        emd = open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
        f = FE.lese_emd(emd)
        mo = FE.modell(emd, open(os.path.join(ordner, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
        cb, Rr, Tt = G.posen(mo)
        cb = np.array(cb)
        sel = ~np.isin(cb[:, 0], ausn)
        H = G.Huellen(f, Rr[sel], Tt[sel])
        Z = TX.lade_zonen()
        import kf_k5g2 as B
        werte = {}
        for teil in (22, 23):
            fest = B.fest_ecken(f, Z, teil)
            D = G.eindringen(f, Rr[sel], Tt[sel], teil, huellen=H)
            mx = D.max(0)
            beweg = [j for j in range(len(mx)) if j not in fest]
            werte[teil] = (int((mx[beweg] > -float(par['geometrie']['rand']) + 1e-6).sum()), round(float(mx[beweg].max()), 1),
                           int((mx[sorted(fest)] > 0).sum()), round(float(mx[sorted(fest)].max()), 1))
        if nm == 'K5g2':
            wv = werte
        else:
            wg = werte
    ok = wv[22][0] == 0 and wv[23][0] == 0
    R.zeile('E1.3', 'bewegliche Kopf-/Kiefer-Ecken naeher als Rand an/in Brust 10, Hals 21 (Bedingungs-Bilder) = 0',
            'Kopf %d (max Tiefe %.1f), Kiefer %d; feste Gesicht/Maul-Ecken innen: Kopf %d (max %.1f), Kiefer %d' % (wv[22][0], wv[22][1], wv[23][0],
                                                                                                                wv[22][2], wv[22][3], wv[23][2]),
            ok, gegen=('K5e_f1', 'Kopf %d (max %.1f), Kiefer %d' % (wg[22][0], wg[22][1], wg[23][0]), wg[22][0] == 0 and wg[23][0] == 0))
    for n in (1, 2):
        fv, kv, bv_ = umriss(var, n)
        fg, kg, bg_ = umriss(K5E, n)
        R.zeile('E1.4.%d' % n, 'Umriss Bild %d (Tiefenpuffer): Figur XOR / sichtbarer Kopf / sichtbarer Buckel 10 gegen K5e_f1' % n,
                'Figur %d px (von %d), Kopf %d -> %d px (XOR %d), Buckel %d -> %d px (XOR %d)' % ((fv ^ fg).sum(), fg.sum(), kg.sum(), kv.sum(),
                                                                                                   (kv ^ kg).sum(), bg_.sum(), bv_.sum(), (bv_ ^ bg_).sum()),
                True, art='Info')


# ================================================================== E2
def e2_spiegel(form, par_kr, R_bild):
    """je Paar: max. Abweichung der rechten Kralle vom Spiegelbild der linken im Raum der Hand in der Bild-2-Haltung (Krallen um ihren
    eigenen Knochen gedreht), nach Abzug des par-Verschubs (Kralle-Raum)."""
    S_ = np.diag([1, 1, -1]).astype(float)
    out = {}
    for l, r in G.KRALLEN_PAARE:
        hl_, hr_ = form['knochen'][l]['eltern'], form['knochen'][r]['eltern']
        Rl = (R_bild[hl_].astype(float).T @ R_bild[l].astype(float)) / 4096.0 ** 2
        Rr = (R_bild[hr_].astype(float).T @ R_bild[r].astype(float)) / 4096.0 ** 2
        ol = np.array(form['knochen'][l]['versatz'], float); orr = np.array(form['knochen'][r]['versatz'], float)
        Vl = np.array(form['meshes'][l]['ecken'], float); Vr = np.array(form['meshes'][r]['ecken'], float)
        if len(Vl) != len(Vr):
            out[r] = 10 ** 6
            continue
        d = np.array(par_kr.get(str(r), [0, 0, 0]), float)
        hl = (Rl @ Vl.T).T + ol
        hr = (Rr @ (Vr - d).T).T + orr
        out[r] = round(float(np.abs(hr - (S_ @ hl.T).T).max()), 2)
    return out


def e2(R, S, Sg, par):
    f, fg = S.form, Sg.form
    kp = par['geometrie']['krallen']
    mo = FE.modell(Sg.emd, Sg.tim)
    cb, Rr, _Tt = G.posen(mo)
    i2 = [i for i, (c, b, _k) in enumerate(cb) if c == int(kp['pose'][0]) and b == int(kp['pose'][1])][0]
    av = e2_spiegel(f, kp['verschub'], Rr[i2])
    ag = e2_spiegel(fg, {}, Rr[i2])
    ok = all(v <= 1.0 for v in av.values())
    R.zeile('E2.1', 'rechte Krallen = Spiegel der linken im Raum der Hand (Bild-2-Haltung, nach par-Verschub), max. Abweichung <= 1 (Rundung)',
            '%s; Verschub %s' % (av, kp['verschub']), ok, gegen=('K5e_f1', '%s' % ag, all(v <= 1.0 for v in ag.values())))
    k5c = FE.lese_emd(open(os.path.join(E.BASIS, 'FORM.EMD'), 'rb').read())
    ok = f['meshes'][28]['ecken'] == k5c['meshes'][28]['ecken'] and f['meshes'][28]['normalen'] == k5c['meshes'][28]['normalen']
    okg = fg['meshes'][28]['ecken'] == k5c['meshes'][28]['ecken']
    R.zeile('E2.2', 'Kralle 28 = K5c-Form (Ecken, Normalen)', 'gleich' if ok else 'verschieden', ok, gegen=('K5e_f1', 'gleich' if okg else 'verschieden', okg))
    hinten = all(f['meshes'][m] == fg['meshes'][m] for m in (24, 25, 26, 27))
    R.zeile('E2.3', 'hinten unveraendert (Teile 24-27 = K5e_f1, 3 Zehen)', 'gleich' if hinten else 'verschieden', hinten)


def e2_bild2(R, var):
    """Bild 2 (Buehnen-Kamera, Maler-Ordnung wie das Spiel): sichtbare Pixel der rechten Krallen und der Hand 18 gegen K5e_f1."""
    import v4f
    out = {}
    for nm, o in (('K5g2', var), ('K5e_f1', K5E)):
        _s, K = v4f.szene_id(o, 2)
        out[nm] = dict(krallen=int(np.isin(K['mesh'], (19, 20, 29, 31)).sum()), hand=int((K['mesh'] == 18).sum()),
                       links=int(np.isin(K['mesh'], (14, 15, 28, 30)).sum()))
    R.zeile('E2.4', 'Bild 2 sichtbar (Vorlage-px): rechte Krallen / Hand 18 / linke Krallen', '%s; K5e_f1 %s' % (out['K5g2'], out['K5e_f1']), True, art='Info')
    return out


# ================================================================== V3 Flecken
def v3(R, S, bau):
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    Z = TX.lade_zonen()
    namen = bau['textur']['zeilen']
    fleck_z = [i for i, n in enumerate(namen) if n.startswith('fleck_')]
    traeger = set(TX.schluessel(k) for nr in tj if nr in TX.FLECK_ART for k in tj[nr])
    fremd = [k for k, z in S.zeile.items() if z in fleck_z and k not in traeger]
    # je Traeger: Texel, deren Index in der Fleck-Zeile vom Grund-Eintrag abweicht (= Material) -> muessen genau die beanspruchten Texel sein
    je, _konf = TX.fleck_zuordnung(Z, S.form, S.T['pixel'].shape)
    kopien = {c['flaeche']: c for c in bau['textur']['kopien']}
    falsch, zu_viel, fehlt, n_ok = 0, 0, 0, 0
    for k, d in je.items():
        z = S.zeile[k]
        if z not in fleck_z:
            fehlt += len(d)
            continue
        c = kopien['%d|%d|%d' % k]
        dx, dy = c['nach'][0] - c['von'][0], c['nach'][1] - c['von'][1]
        ys, xs, idx = S.texel(k)
        basis_name = namen[z].split('_')[1]
        basis = namen.index(basis_name)
        mat = np.array(S.cl[z])[idx] != np.array(S.cl[basis])[idx]
        soll = np.array([((int(x) - dx), (int(y) - dy)) in d for y, x in zip(ys, xs)])
        zu_viel += int((mat & ~soll).sum()); fehlt += int((~mat & soll).sum()); n_ok += int((mat & soll).sum())
    ok = not fremd and zu_viel == 0 and fehlt == 0
    # Gegenprobe ohne eigene Texel: Material an den Original-Indizes der Grund-Zeile -> wie viele Nicht-Traeger-Flaechen laesen es?
    Sg = Stand(K5E)
    pxg = Sg.T['pixel']
    fremd_g = 0
    for k, d in je.items():
        idx_m = set(int(pxg[y, x]) for (x, y) in d)
        zg = Sg.zeile[k]
        for k2 in Sg.fl:
            if k2 in traeger or Sg.zeile[k2] != zg:
                continue
            _y, _x, i2 = Sg.texel(k2)
            if np.isin(i2, list(idx_m)).any():
                fremd_g += 1
        break
    R.zeile('V3.1', 'Fleck-Material genau auf den Traegertexeln, keine andere Flaeche liest eine Fleck-Zeile',
            'fremde Leser %d, Material ausserhalb der Texel %d, fehlende Fleck-Texel %d, richtige %d' % (len(fremd), zu_viel, fehlt, n_ok), ok,
            gegen=('ohne eigene Texel (Material an den Original-Indizes, erste Traegerflaeche)', '%d andere Flaechen lesen es' % fremd_g, fremd_g == 0))
    R.zeile('V3.2', 'Texel-Konflikte zwischen Zonen auf derselben Flaeche (nach Lage aufgeloest)', '%s' % bau['textur']['flecken']['konflikte'], True, art='Info')
    return je


# ================================================================== V4 / V2 / V5 / V1 / V6
def material_texel(S, keys, zuweisung_zone=None):
    return S.farben(keys)


def v4(R, S, plan_listen):
    zl = zonen_listen(TX.lade_zonen())
    unten = (zl['45'] | zl['42'] | zl['31'] | zl['44'] | set(k for k in S.fl if k[0] == 21))
    unten = set(k for k in unten if k in S.fl)

    def werte(St, keys):
        L, W = St.farben(sorted(keys))
        r = float(rot(L).mean() * 100) if len(L) else 0.0
        rgb = KG.wort_rgb([int(w) & 0x7FFF for w in W]) if len(W) else np.zeros((0, 3))
        schwarz = (rgb.max(1) <= 12) & (W != 0)
        return r, float(schwarz.mean() * 100) if len(W) else 0.0, int(len(W))
    rv, sv, nv = werte(S, unten)
    Sg = Stand(K5E)
    rg, sg, _ = werte(Sg, set(k for k in unten if k in Sg.fl))
    Sf = Stand(K5F)
    _rf, sf, _ = werte(Sf, set(k for k in unten if k in Sf.fl))
    R.zeile('V4.1', 'Zone 45, Hals 21, Lippe 42, Unterkiefer 31, Hals-Unterseite 44: rote Texelzugriffe = 0 %', '%.2f %% von %d' % (rv, nv), rv == 0,
            gegen=('K5e_f1', '%.2f %%' % rg, rg == 0))
    # schwarz nur, wo der G4-Eintrag selbst so dunkel ist
    R.zeile('V4.2', 'dieselben Flaechen: schwarze Texelzugriffe (RGB-Max <= 12) = 0 %', '%.2f %%' % sv, sv == 0,
            gegen=('K5f', '%.2f %%' % sf, sf == 0))
    zl30 = zl['30']
    maul_z = [i for i, n in enumerate(S.namen) if n.startswith('maul')]
    leser = [k for k, z in S.zeile.items() if z in maul_z and k not in zl30]
    g4_0 = [k for k, z in Sg.zeile.items() if z == 0 and k not in zl30]
    R.zeile('V4.3', 'Maul-Zeilen (G4-Mundfleisch) nur von Zone 30 gelesen', '%d andere Flaechen' % len(leser), not leser,
            gegen=('K5e_f1 (Zeile 0)', '%d andere Flaechen' % len(g4_0), not g4_0))


def haut_werte(St, keys):
    L, W = St.farben(sorted(keys))
    C = np.hypot(L[:, 1], L[:, 2]); h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    r = np.radians(h)
    hm = float(np.degrees(np.arctan2((np.sin(r) * C).sum(), (np.cos(r) * C).sum())))
    return hm, float(np.median(C)), float(np.median(L[:, 0]))


def v2(R, S, plan):
    haut = [k for k, v in plan.items() if v[0] == 'haut' and S.zeile[k] in [i for i, n in enumerate(S.namen) if n.startswith('haut')]]
    hv, cv, lv = haut_werte(S, haut)
    out = {}
    for nm, o in (('K5e_f1', K5E), ('K5f', K5F)):
        St = Stand(o)
        out[nm] = haut_werte(St, [k for k in haut if k in St.fl])
    R.zeile('V2.1', 'Hautton (C-gewichtet, Albedo der Hautflaechen) im Band der Originale [14, 27] Grad', '%.1f Grad' % hv, 14 <= hv <= 27,
            gegen=('K5e_f1 / K5f', '%.1f / %.1f Grad' % (out['K5e_f1'][0], out['K5f'][0]), 14 <= out['K5e_f1'][0] <= 27 or 14 <= out['K5f'][0] <= 27))
    R.zeile('V2.2', 'Haut-Buntheit (Median C* der Hautflaechen) 2-6 (Eichung a)', '%.2f (L* Median %.1f)' % (cv, lv), 2 <= cv <= 6,
            gegen=('K5f', '%.2f' % out['K5f'][1], 2 <= out['K5f'][1] <= 6))


def v3_fleisch(R, S, je):
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    Ls = []
    for fs in tj['27']:
        k = TX.schluessel(fs)
        ys, xs, idx = S.texel(k)
        z = S.zeile[k]
        basis = S.namen.index(S.namen[z].split('_')[1]) if S.namen[z].startswith('fleck_') else z
        m = np.array(S.cl[z])[idx] != np.array(S.cl[basis])[idx]
        Ls.append(S.lab[z][idx[m]])
    L = np.vstack(Ls) if Ls else np.zeros((0, 3))
    lm, cm = (float(np.median(L[:, 0])), float(np.median(np.hypot(L[:, 1], L[:, 2])))) if len(L) else (0, 0)
    Sg = Stand(K5E)
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    bk = [TX.schluessel(s) for s in mj['Fleisch_Buckel']['liste']]
    Lg, _ = Sg.farben(bk)
    lg, cg = float(np.median(Lg[:, 0])), float(np.median(np.hypot(Lg[:, 1], Lg[:, 2])))
    R.zeile('V3.3', 'Buckel-Fleisch (Zone 27, Material-Texel): Median L* 33-39, C* 22-26 (Eichung b)', 'L* %.1f, C* %.1f (%d Texelzugriffe)' % (lm, cm, len(L)),
            33 <= lm <= 39 and 22 <= cm <= 26, gegen=('K5e_f1 Buckel', 'L* %.1f, C* %.1f' % (lg, cg), 33 <= lg <= 39 and 22 <= cg <= 26))


def v5(R, S, plan):
    kral = [k for k, v in plan.items() if v[0] == 'kralle']

    def w(St):
        L, W = St.farben([k for k in kral if k in St.fl])
        return int(((W & 0x7FFF) == 0x7FFF).sum()), float(rot(L).mean() * 100), len(W)
    a, b, n = w(S)
    ag, bg, ng = w(Stand(K5F))
    R.zeile('V5', 'Krallen: reinweisse Texelzugriffe (0x7FFF) = 0 und rote = 0 %', '%d / %.2f %% von %d' % (a, b, n), a == 0 and b == 0,
            gegen=('K5f', '%d / %.2f %%' % (ag, bg), ag == 0 and bg == 0))


def v1(R, S, bau):
    g4 = E.g4_rampen()[0]
    worst = 0.0
    for s in range(3):
        z = S.namen.index('haut%d' % s)
        Lg = lab_w(g4[s])[:, 0]; Ln = S.lab[z][:, 0]
        _L, haut, hell, ader = E.klassen([int(x) for x in g4[s]])
        sel = ader & (Lg * float(bau['par']['textur']['haut']['f']) < 70) & (Lg > 1)
        if sel.sum() > 3:
            b, a = np.polyfit(Lg[sel], Ln[sel], 1)
            worst = max(worst, abs(a))
    # Gegenprobe K5f (Adern affin)
    Sf = Stand(K5F)
    bf = json.load(open(os.path.join(K5F, 'bau.json'), encoding='utf-8'))
    wf = 0.0
    zk = bf.get('par', {}).get('zeilen', {}).get('haut', [1, 2, 3])
    for s in range(3):
        Lg = lab_w(g4[s])[:, 0]; Ln = Sf.lab[zk[s]][:, 0]
        _L, haut, hell, ader = E.klassen([int(x) for x in g4[s]])
        sel = ader & (Lg > 1)
        if sel.sum() > 3:
            b, a = np.polyfit(Lg[sel], Ln[sel], 1)
            wf = max(wf, abs(a))
    R.zeile('V1', 'Adern multiplikativ (je Hautzeile Fit L\' = a + b L_G4 der Aderklasse, |a| <= 1,5)', '|a| max %.2f' % worst, worst <= 1.5,
            gegen=('K5f', '|a| max %.2f' % wf, wf <= 1.5))


def v6(R, S, plan):
    k5c = [int(x) for x in E.eingaenge()['cl_k5c'][5]]
    kern = E.f1_kern_eintraege()
    f1 = [k for k, v in plan.items() if v[0] == 'f1']
    z = S.zeile[f1[0]]
    ok1 = all(S.cl[z][i] == k5c[i] for i in kern) and all(S.zeile[k] == z for k in f1)
    M_ = E.f1_masken()

    def rand(St):
        Ls = []
        for k in f1:
            ys, xs, idx = St.texel(k)
            nk = ~M_['kern'][ys, xs]
            Ls.append(St.lab[St.zeile[k]][idx[nk], 0])
        haut = [k for k in St.fl if k[0] == 0 and k not in f1]
        Lh, _ = St.farben(haut)
        return float(np.median(np.concatenate(Ls))), float(np.median(Lh[:, 0]))
    rv, hv = rand(S)
    rg, hg = rand(Stand(K5E_ALT))
    R.zeile('V6', 'F1: Kern bytegleich K5c-Zeile 5; Nicht-Kern-Texel der F1-Flaechen Median L* >= Mesh-0-Haut - 6 (kein schwarzer Ring)',
            'Kern %s; Rand %.1f gegen Haut %.1f' % ('gleich' if ok1 else 'VERSCHIEDEN', rv, hv), ok1 and rv >= hv - 6,
            gegen=('K5e (schwarzer Ring, E32)', 'Rand %.1f gegen Haut %.1f' % (rg, hg), rg >= hg - 6))


# ================================================================== Runde 2: Haende, Augen
GEGEN_R1 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/T_k5g2_kopie_2205'


def h1(R, S):
    """Haende 13/18 (Pruefer r1 Sicht 6): kein 0x7FFF; Median L* der Handtexel im Band der Haut (P25..P75 der Hautflaechen)."""
    def w(St, namen):
        hand = [k for k in St.fl if k[0] in (13, 18)]
        hz = [i for i, n in enumerate(namen) if n.startswith('haut')]
        haut = [k for k in St.fl if St.zeile[k] in hz and k[0] not in (13, 18)]
        Lh, Wh = St.farben(hand)
        Ls, _ = St.farben(haut)
        return (int(((Wh & 0x7FFF) == 0x7FFF).sum()), float(np.median(Lh[:, 0])), float(np.percentile(Ls[:, 0], 25)),
                float(np.percentile(Ls[:, 0], 75)), float(np.median(Ls[:, 0])))
    a, m, p25, p75, hm = w(S, S.namen)
    ok = a == 0 and p25 <= m <= p75
    txt = 'reinweiss %d, Hand-Median L* %.1f, Haut P25/Median/P75 %.1f/%.1f/%.1f' % (a, m, p25, hm, p75)
    gegen = None
    if os.path.isdir(GEGEN_R1):
        Sg = Stand(GEGEN_R1)
        ng = json.load(open(os.path.join(GEGEN_R1, 'bau.json'), encoding='utf-8'))['textur']['zeilen']
        ag, mg, g25, g75, _ = w(Sg, ng)
        gegen = ('K5g2 Runde 1 (weisse Hand)', 'reinweiss %d, Median %.1f, Haut %.1f-%.1f' % (ag, mg, g25, g75), ag == 0 and g25 <= mg <= g75)
    R.zeile('H1', 'Haende 13/18: kein 0x7FFF, Median L* im Band der Haut (P25-P75)', txt, ok, gegen=gegen)


def a1(R, S, bau, var):
    """Augen (Pruefer r1 Sicht 2): Median L* der Augen-Material-Texel ueber dem Median der Kopfhaut (Kopf 22, Hautzeilen) = heller Punkt;
    die Lage (nur Zone-29-Texel) prueft V3.1. Dazu Bild 2 (Buehnenlicht, ohne Filter) 9x9 um die Nutzerpunkte (Info)."""
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']['29']

    def w(St, namen):
        Ls = []
        for fs in tj:
            k = TX.schluessel(fs)
            ys, xs, idx = St.texel(k)
            z = St.zeile[k]
            if namen[z].startswith('fleck_'):
                basis = namen.index(namen[z].split('_')[1])
                m = np.array(St.cl[z])[idx] != np.array(St.cl[basis])[idx]
                Ls.append(St.lab[z][idx[m], 0])
            else:
                Ls.append(St.lab[z][idx, 0])
        hz = [i for i, n in enumerate(namen) if n.startswith('haut')]
        kopf = [k for k in St.fl if k[0] == 22 and St.zeile[k] in hz]
        Lk, _ = St.farben(kopf)
        La = np.concatenate(Ls) if Ls else np.zeros(0)
        return (float(np.median(La)) if len(La) else 0.0), float(np.median(Lk[:, 0])), len(La)
    am, km, n = w(S, S.namen)
    gegen = None
    if os.path.isdir(GEGEN_R1):
        Sg = Stand(GEGEN_R1)
        ng = json.load(open(os.path.join(GEGEN_R1, 'bau.json'), encoding='utf-8'))['textur']['zeilen']
        ag, kg, _ = w(Sg, ng)
        gegen = ('K5g2 Runde 1 (Augen dunkel)', 'Auge %.1f gegen Kopfhaut %.1f' % (ag, kg), ag > kg)
    R.zeile('A1', 'Augen: Median L* der Augen-Material-Texel > Median L* der Kopfhaut', 'Auge %.1f gegen Kopfhaut %.1f (%d Texelzugriffe)' % (am, km, n),
            am > km, gegen=gegen)
    import v4f
    out = []
    for o in (var, K5E):
        sauber, K = v4f.szene_id(o, 2)
        L = v4f.lab(sauber)[..., 0]
        for (x, y) in ((217, 274), (280, 275)):
            win = L[y - 4:y + 5, x - 4:x + 5]
            umg = L[y - 12:y + 13, x - 12:x + 13]
            out.append('%s (%d,%d) max %.1f / Umgebung Median %.1f' % ('K5g' if o == var else 'K5e_f1', x, y, win.max(), np.median(umg)))
    R.zeile('A1b', 'Bild 2 ohne Filter, 9x9 um die Nutzerpunkte (Vorlage L* 30 gegen 5-7)', '; '.join(out), True, art='Info')


# ================================================================== weisses Licht, Ruhe, Spiel
def weiss(R, var):
    def mess(o):
        mo = rl.fl.lade_var(o)
        ws, rs = [], []
        for _wn, az, el in rl.WINKEL4:
            img, K = rl.weiss_id(mo, 2, az, el, 380)
            ws.append(fam.alles(img, K['maske']))
            img2, K2 = rl.weiss_id(mo, 2, az, el)
            rs.append(M.masse(img2, K2['maske'], 2))
        return ws, rs
    wv, rv = mess(var)
    wg, rg_ = mess(K5E)
    for nr, txt, key, lo, hi in W_BAND:
        v = float(np.mean([d[key] for d in wv])); g = float(np.mean([d[key] for d in wg]))
        R.zeile(nr, 'weiss %s im Band %.3f..%.3f (Originale)' % (txt, lo, hi), '%.3f' % v, lo <= v <= hi, gegen=('K5e_f1', '%.3f' % g, lo <= g <= hi))
    hp = float(np.mean([d['hell_pct'] for d in wv])); fh = float(np.mean([d['F_hell'] for d in wv]))
    R.zeile('W4', 'weiss hell %% <= 17,8 und F_hell <= 71,7 (Decke der Originale)', '%.1f / %.1f' % (hp, fh), hp <= 17.8 and fh <= 71.7)
    cm = float(np.mean([d['C_mittel'] for d in wv]))
    R.zeile('W5', 'weiss C* Mittel (Info; Band Originale 8,63..23,41)', '%.2f' % cm, True, art='Info')
    mi = M.mittel(rv); mg = M.mittel(rg_)
    acc = np.zeros(24)
    for d in rv:
        for s_, p in d['sektoren'].items():
            acc[int(s_) // 15] += p * d['px'] * d['bunt_pct'] / 1e4
    acc /= max(acc.sum(), 1e-9)
    s5 = int((acc >= 0.05).sum())
    R.zeile('R1', 'Ruhe: Sektoren gepoolt <= 5', '%d' % s5, s5 <= 5)
    R.zeile('R2', 'Ruhe: farbfremde Flecken <= 7,4 je 10k und <= 0,64 %', '%.1f / %.2f %%' % (mi['F_fremd'], mi['fremd_pct']),
            mi['F_fremd'] <= 7.4 and mi['fremd_pct'] <= 0.64)
    R.zeile('R3', 'Ruhe: Farbtonspanne 90 % <= 76,8 Grad', '%.1f' % mi['spanne90'], mi['spanne90'] <= 76.8)
    R.zeile('R4', 'Ruhe: Block-Farbton <= 16,3 Grad', '%.1f' % mi['block_ton_streu'], mi['block_ton_streu'] <= 16.3)
    return wv


def spiel(R, var, pfad):
    if not pfad or not os.path.exists(pfad):
        R.zeile('Z', 'Spiel 5080 Z1-Z9', 'keine Spielmessung uebergeben', False)
        return
    d = json.load(open(pfad, encoding='utf-8'))
    import hashlib
    if d.get('tim_sha256') != hashlib.sha256(open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read()).hexdigest():
        R.zeile('Z', 'Spiel 5080 Z1-Z9', 'Spielmessung gehoert nicht zu dieser Variante (sha256)', False)
        return
    m = d['f80']
    g = d.get('gegen_f80') or {}
    for nr, txt, key, lo, hi in ZB:
        v = m.get(key)
        R.zeile(nr, 'Spiel 5080 %s im Band %.3f..%.3f' % (txt, lo, hi), '%s' % v, v is not None and lo <= v <= hi,
                gegen=('K5e_f1', '%s' % g.get(key), g.get(key) is not None and lo <= g.get(key) <= hi) if g else None)


# ================================================================== Technik
def technik(R, S, bau, var):
    Sg = Stand(K5E)
    Sv = Stand(var)
    R.zeile('T1', 'Zeilen <= 16, Port-Texturhoehe <= 4096', '%d / %d' % (len(Sv.cl), len(Sv.cl) * 256), len(Sv.cl) <= 16 and len(Sv.cl) * 256 <= 4096)
    s1, s2 = FE.sektionen(Sv.emd), FE.sektionen(Sg.emd)
    ok = all(s1[k] == s2[k] for k in ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3'))
    f3 = FE.sektionen(open(os.path.join(var, 'FORM.EMD'), 'rb').read())
    g3 = FE.sektionen(open(os.path.join(K5E, 'FORM.EMD'), 'rb').read())
    ok3 = all(f3[k] == g3[k] for k in ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3')) and f3['md1'] == s1['md1']
    R.zeile('T2', 'EDD/EMR/Morph = K5e_f1 (28 Clips; FORM.EMD 3 Clips, MD1 = BAHNSTEIG)', 'gleich' if ok and ok3 else 'VERSCHIEDEN', ok and ok3)
    erlaubt = {19, 20, 22, 23, 28, 29, 31}
    ab = [m for m in range(len(S.form['meshes'])) if m not in erlaubt and
          (S.form['meshes'][m]['ecken'] != Sg.form['meshes'][m]['ecken'] or S.form['meshes'][m]['normalen'] != Sg.form['meshes'][m]['normalen'] or
           [x['e'] for x in S.form['meshes'][m]['vierecke']] != [x['e'] for x in Sg.form['meshes'][m]['vierecke']])]
    R.zeile('T3', 'Geometrie ausser 19/20/22/23/28/29/31 = K5e_f1', 'abweichend: %s' % ab, not ab)
    kop = set(TX.schluessel(c['flaeche']) for c in bau['textur']['kopien'])
    uv_ab = [k for k, f in S.fl.items() if k[0] not in (19, 20, 29, 31) and k not in kop and
             (f['uv'] != Sg.fl[k]['uv'] or (int(f['page']) & 0xF) != (int(Sg.fl[k]['page']) & 0xF))]
    pxa = S.T['pixel'] != Sg.T['pixel']
    belegt_alt = np.zeros(S.T['pixel'].shape, bool)
    for k in Sg.fl:
        belegt_alt |= TX.flaeche_maske(Sg.fl[k], S.T['pixel'].shape)
    R.zeile('T4', 'UV/Seite ausser Kopien und Spiegelkrallen = K5e_f1; Original-Texel unveraendert', '%d Flaechen abweichend, %d Original-Texel veraendert'
            % (len(uv_ab), int((pxa & belegt_alt).sum())), not uv_ab and not (pxa & belegt_alt).any())
    bad = [k for k, z in Sv.zeile.items() if not 0 <= z < len(Sv.cl)]
    R.zeile('T5', 'jede Flaeche liest eine gueltige Zeile (Asset)', '%d ungueltig' % len(bad), not bad)
    # T6: Asset = Unterteilung des Pruef-Stands nach dem Plan (deterministisch), TIM gleich
    import kf_k5g2 as B
    fp = json.loads(json.dumps(S.form))
    B.unterteilung_anwenden(fp, B.UT_PLAN)
    ok6 = G.emd_mit_md1(S.emd, fp) == Sv.emd and Sv.tim == S.tim
    R.zeile('T6', 'Asset = Pruef-Stand + Unterteilung (Plan k5g2_unterteilung.json), TIM gleich', 'gleich' if ok6 else 'VERSCHIEDEN', ok6)
    # T7: Dreiecks-Warteschlange render_pc.c:226 TEXTRI_QUEUE_MAX 8192 je Bild (Ueberlauf verwirft still). Neben der Form zeichnet die Szene
    # mindestens Leon; render_pc.c:226 nennt "~700 quads = ONE actor" -> 1400 Dreiecke Reserve. Gezaehlt: Vorderseiten der Form (wie der Port
    # nach Rueckseiten-Test und vz-Schnitt) ueber jedes 7. Bild aller Clips in den Spielkameras.
    def vorn_max(St):
        mo = FE.modell(St.emd, St.tim)
        cb, Rr, Tt = G.posen(mo)
        an = G.ansichten()
        mx = 0
        for i in range(0, len(cb), 7):
            for nm in ('spiel5080', 'spiel5080_vorn', 'spiel5090'):
                kam, lage, gier, _H, _W, _ab = an[nm]
                mx = max(mx, len(G.dreiecke_teile(mo, Rr[i], Tt[i], lage, gier, kam, teile=tuple(range(len(mo.meshes))))))
        return mx
    nv, ngv = vorn_max(Sv), vorn_max(Sg)
    nt = sum(len(m['dreiecke']) + 2 * len(m['vierecke']) for m in Sv.form['meshes'])
    R.zeile('T7', 'sichtbare Dreiecke der Form je Bild + 1400 (Leon) <= 8192 (render_pc.c:226 TEXTRI_QUEUE_MAX)',
            'max %d sichtbar (gesamt %d; K5e_f1 max %d sichtbar)' % (nv, nt, ngv), nv + 1400 <= 8192)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('variante')
    ap.add_argument('--spiel')
    ap.add_argument('--keil')
    ap.add_argument('--json')
    a = ap.parse_args()
    var = os.path.abspath(a.variante)
    bau = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))
    par = bau['par']
    S = Stand(os.path.join(var, 'pruef'))
    S.namen = bau['textur']['zeilen']
    import kf_k5g2 as B
    X = E.eingaenge()
    Z = TX.lade_zonen()
    plan = TX.materialplan(S.form, X, Z)
    Sg = Stand(K5E)
    R = Riegel()
    e1(R, var, par, a.keil or os.path.join(var, 'keil_k5g2.json'))
    e2(R, S, Sg, par)
    e2_bild2(R, var)
    je = v3(R, S, bau)
    v3_fleisch(R, S, je)
    v4(R, S, plan)
    v2(R, S, plan)
    v5(R, S, plan)
    v1(R, S, bau)
    v6(R, S, plan)
    weiss(R, var)
    spiel(R, var, a.spiel or os.path.join(var, 'spiel_k5g2.json'))
    h1(R, S)
    a1(R, S, bau, var)
    technik(R, S, bau, var)
    rot_ = [z for z in R.z if z['art'] == 'Riegel' and not z['ok']]
    gg = [z for z in R.z if z['gegenprobe'] and z['gegenprobe'][2]]
    print('\nERGEBNIS: %d Zeilen, %d ROT (%s); Gegenproben, die NICHT rot sind: %s' % (len(R.z), len(rot_), [z['nr'] for z in rot_], [z['nr'] for z in gg]))
    if a.json:
        json.dump(dict(zeilen=R.z, rot=[z['nr'] for z in rot_], gegenprobe_gruen=[z['nr'] for z in gg]), open(a.json, 'w', encoding='utf-8'),
                  indent=1, ensure_ascii=False)
    sys.exit(1 if rot_ or gg else 0)


if __name__ == '__main__':
    main()
