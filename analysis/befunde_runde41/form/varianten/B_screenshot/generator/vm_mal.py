#!/usr/bin/env python3
"""Varianten, Maler im RE2-Stil (E8). Ersetzt fb_mal.backen fuer die Variantenmatrix. Kein RE-Wert: neues Modell, nur Handwerk wie RE2.

Gemessen an RE2-Gegnern (varianten/VARIANTEN.md Abschnitt 1, re2_handwerk.py): Texturen sind gemalte Bilder mit starkem Hell-Dunkel je Texel
(RE2 Nachbar-Texel gleich 21-35 %, harter Sprung ab 20 Y 10-26 %; form_bemalt 61 % / 3 %), Licht und Form sind eingemalt, je Mesh eine CLUT-Zeile.
Darum hier:
 - Wert je Texel aus gemalter Form: Muskelmassen (Zellen, 3D im Bindraum), harte dunkle Furchen dazwischen, Fasern laengs der Knochenachse, Korn.
 - Licht durchgehend von oben (eine Richtung fuer alle Teile), aus der Normale der Ruhepose, gestoert durch die Woelbung der Muskelmassen.
 - Keine Foto-Rueckprojektion, keine Bildstruktur, kein Faktor je Knochen (der machte Helligkeitsspruenge je Schwanzglied, E9).
 - Zeichnung Z1-Z9, Q, G, P, X: Lage aus der Bildmaske (bindend), gemalt als feste Flaeche mit Licht von oben und dunklem Rand, kein weicher Stempel.
 - Schwanz (E9): Muster im Bindraum (Gelenkringe liegen dort deckungsgleich), Licht mit weich ueber die Gelenke gemischter Drehung, keine Faltenschatten
   zwischen den Gliedern, keine Muskelzellen (E4 glatt), nur Fasern laengs.
Farben je Variante: A Palette der Rueckrechnung (farbe_palette.json oberflaeche_rgb), B Farbton des Videos Bild 1 bei der Helligkeit der Palette,
C Mitte. Was nicht in der Palette steht (Haende, Horn, Zaehne, Maul, Auge, Adern), geht in B durch die lineare Abbildung A -> B, in C durch die Mitte.
"""
import json
import math
import os

import cv2
import numpy as np
from scipy.spatial import cKDTree
from scipy.spatial.transform import Rotation, Slerp

from rf_kern import FORM, HIER
import form_pruefung as P
import fb_mal as FM
import rf_textur as RT

TEX_W, TEX_H = 384, 256
YK = np.array([.299, .587, .114], np.float32)
_PAL = {x['name']: x for x in json.load(open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8'))['materialien']}


# ═══════════════════════════════════════════════════ Farben je Variante
def _b_farbe(name):
    """B: Farbton und Saettigung des Videos Bild 1, Helligkeit (Y) der Palette. So ist nur die Helligkeit des Videos herausgerechnet;
    die Unschaerfe steckt nicht in den Werten, sie sind Flaechenmittel aus farbe_palette.json."""
    a = np.array(_PAL[name]['oberflaeche_rgb'], float)
    v = np.array(_PAL[name]['video_bild1_rgb'], float)
    return v * float(a @ YK) / float(v @ YK)


MATERIAL = ('grundhaut_dunkel', 'rotbraun_mittelton', 'rosa_stelle', 'helle_baender', 'feine_linien')
_A = np.array([_PAL[n]['oberflaeche_rgb'] for n in MATERIAL], float)
_B = np.array([_b_farbe(n) for n in MATERIAL])
M_B, *_ = np.linalg.lstsq(_A, _B, rcond=None)      # lineare Abbildung A -> B fuer Farben ausserhalb der Palette (Rest <= 10 je Kanal)


def farbsatz(variante):
    """-> dict Materialname -> RGB (float) und 'abb' (3x3) fuer freie Farben."""
    if variante == 'A':
        mat = {n: np.array(_PAL[n]['oberflaeche_rgb'], float) for n in MATERIAL}
        abb = np.eye(3)
    elif variante == 'B':
        mat = {n: _b_farbe(n) for n in MATERIAL}
        abb = M_B
    elif variante == 'C':
        mat = {n: 0.5 * (np.array(_PAL[n]['oberflaeche_rgb'], float) + _b_farbe(n)) for n in MATERIAL}
        abb = 0.5 * (np.eye(3) + M_B)
    else:
        raise ValueError(variante)
    us = np.array(_PAL['untere_streifen']['oberflaeche_rgb'], float)
    wk = np.array(_PAL['weisskerne']['oberflaeche_rgb'], float)
    mat['untere_streifen'] = np.clip(us @ abb, 0, 255)
    mat['weisskerne'] = np.clip(wk @ abb, 0, 255)
    return dict(mat=mat, abb=abb, name=variante)


def frei(fs, rgb):
    return np.clip(np.asarray(rgb, float) @ fs['abb'], 0, 255)


# Farben der Zeichnung (rf_marken.FARBE) auf Materialien: die Marken tragen die Variantenfarbe ihres Materials, mit dem Faktor der Vorlage gegen das Material.
MARKE_MATERIAL = dict(rosa='rosa_stelle', band='helle_baender', linie='feine_linien', rot='rotbraun_mittelton', streifen='untere_streifen', kern=None, z9=None, warm=None,
                      rosa_end='rosa_stelle', weiss_end=None, rot_end=None)


def markenfarbe(fs, rf_farbe, name):
    """rf_marken.FARBE[name] in der Variante: Material -> Variantenfarbe mal (Vorlage / Palettenfarbe), sonst lineare Abbildung."""
    import rf_marken as RM
    f = np.array(RM.FARBE[name], float)
    mt = MARKE_MATERIAL.get(name)
    if mt is None:
        return frei(fs, f)
    a = np.array(_PAL[mt]['oberflaeche_rgb'], float)
    return np.clip(fs['mat'][mt] * (f / np.maximum(a, 1)), 0, 255)


# ═══════════════════════════════════════════════════ Rauschen, Zellen
def _hash3(i, saat):
    h1 = FM._hash(i[..., 0], i[..., 1], i[..., 2], saat)
    h2 = FM._hash(i[..., 0], i[..., 1], i[..., 2], saat + 101)
    h3 = FM._hash(i[..., 0], i[..., 1], i[..., 2], saat + 211)
    return np.stack([h1, h2, h3], -1)


def zellen(q, saat=3):
    """Worley 3D: q in Zelleinheiten [N,3] -> F1, F2 (Abstand zum naechsten und zweitnaechsten Merkpunkt)."""
    i0 = np.floor(q).astype(np.int64)
    best1 = np.full(q.shape[0], 1e9, np.float32)
    best2 = np.full(q.shape[0], 1e9, np.float32)
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            for dz in (-1, 0, 1):
                c = i0 + np.array([dx, dy, dz])
                fp = c + 0.15 + 0.7 * _hash3(c, saat)
                d = np.linalg.norm(q - fp, axis=1).astype(np.float32)
                n2 = np.minimum(best2, np.maximum(best1, d))
                best1 = np.minimum(best1, d)
                best2 = n2
    return best1, best2


def rauschen(p, saat):
    return FM.rauschen(p, 1.0, saat)


def glatt_stufe(x, a, b):
    t = np.clip((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)


# ═══════════════════════════════════════════════════ Raeume
def bind_versatz(form):
    """Kumulierter Bindversatz je Knochen (Bindpose ohne Drehung = Rastlage)."""
    off = []
    for k in form['knochen']:
        v = np.array(k['versatz'], float)
        off.append(v if k['eltern'] < 0 else off[k['eltern']] + v)
    return np.array(off)


SCHWANZ = ['schwanz%d' % i for i in range(1, 7)]
SCHWANZ_ZONE = 180.0       # Gelenkzone fuer die Mischdrehung (Einheiten laengs); frei gesetzt, kleiner als das kuerzeste Glied (rund 600)


def drehung_je_texel(form, kn, Pb, Rr, teile):
    """Ruhepose-Drehung je Texel [N,3,3]. Am Schwanz weich ueber die Gelenke gemischt (Slerp), damit Licht und Normalen dort nicht springen (E9)."""
    H, W = kn.shape
    Rt = np.zeros((H, W, 3, 3), np.float32)
    for b in np.unique(kn[kn >= 0]):
        Rt[kn == b] = Rr[b]
    idx = {p.name: p.knochen for p in teile}
    if not all(n in idx for n in SCHWANZ):
        return Rt
    kette = [idx[n] for n in SCHWANZ]
    off = bind_versatz(form)
    # Kettenlaenge s = -x im Bindraum (alle Glieder laufen nach -x, rf_plan d = (-1,0,0)); Gelenk j = Ursprung von Glied j+1
    sj = [-float(off[kette[j + 1]][0]) for j in range(len(kette) - 1)]
    rots = Rotation.from_matrix(np.stack([Rr[b] for b in kette]).astype(float))
    ist = np.isin(kn, kette)
    s = -Pb[ist][:, 0]
    bi = kn[ist]
    R_neu = np.stack([Rr[b] for b in bi]).astype(float)
    for j, s0 in enumerate(sj):
        a, b = kette[j], kette[j + 1]
        z = (np.abs(s - s0) < SCHWANZ_ZONE) & np.isin(bi, [a, b])
        if not z.any():
            continue
        t = glatt_stufe(s[z], s0 - SCHWANZ_ZONE, s0 + SCHWANZ_ZONE)
        sl = Slerp([0.0, 1.0], Rotation.from_matrix(np.stack([Rr[a], Rr[b]]).astype(float)))
        R_neu[z] = sl(t).as_matrix()
    Rt[ist] = R_neu.astype(np.float32)
    return Rt


def falten(form, kn, pos, Rr, Tr, teile, ohne_paare):
    """Gemalte Umgebungsverdeckung wie fb_mal.falten, aber ohne die Paare in ohne_paare (Schwanzglieder untereinander: E9, kein Schatten am Gelenk)."""
    ecken, eig = [], []
    for b, m in enumerate(form['meshes']):
        if m['ecken']:
            e = np.array(m['ecken'], float) @ Rr[b].T + Tr[b]
            ecken.append(e)
            eig += [b] * len(e)
    E = np.concatenate(ecken)
    eig = np.array(eig)
    baum = cKDTree(E)
    m = kn >= 0
    w = np.zeros(kn.shape + (3,), np.float32)
    for b in np.unique(kn[m]):
        s = kn == b
        w[s] = pos[s] @ Rr[b].T.astype(np.float32) + Tr[b].astype(np.float32)
    d, j = baum.query(w[m], k=32)
    eigen = kn[m][:, None]
    fremd = eig[j] != eigen
    for a, b in ohne_paare:
        fremd &= ~(((eigen == a) & (eig[j] == b)) | ((eigen == b) & (eig[j] == a)))
    d = np.where(fremd, d, np.inf).min(1)
    ao = np.ones(kn.shape, np.float32)
    ao[m] = 1 - 0.6 * np.exp(-(d / 160.0) ** 2)
    return ao, w


# ═══════════════════════════════════════════════════ Maler
# Muskelzelle (Einheiten): Bahnsteig-Form rund 2900 hoch, RE2-Bosse haben je Glied 3-5 grosse Formen (tex_re2_34/36). Frei gesetzt.
ZELLE = 430.0
ZELLE_LANG = 1.8           # Zellen laengs der Knochenachse gestreckt
FASER_QUER, FASER_LAENGS = 34.0, 260.0
LICHT = np.array([0.22, -1.0, 0.05])
LICHT = LICHT / np.linalg.norm(LICHT)
Y_N = 57.0                 # Gegenproben N1/N2 (fb_mal.Y_N)


def art(name):
    if name.startswith('schwanz'):
        return 'schwanz'
    if name.startswith(('klaue', 'kralle', 'zeh', 'fang', 'wange')):
        return 'spitze'
    if name in ('kopf', 'kiefer', 'hals'):
        return 'kopf'
    if name.startswith(('hand', 'fuss')):
        return 'hand'
    if name in ('huefte', 'wirbel', 'brust'):
        return 'rumpf'
    return 'glied'


# Gewichte je Art: Furche, Woelbung, Fasern, Adern. Schwanz glatt (E4) und ein Stueck (E9): keine Zellen, keine Furchen, nur Fasern.
GEWICHT = dict(rumpf=(0.50, 0.30, 0.10, 0.30), glied=(0.50, 0.32, 0.12, 0.30), kopf=(0.35, 0.22, 0.06, 0.15), hand=(0.30, 0.20, 0.06, 0.10),
               spitze=(0.0, 0.0, 0.05, 0.0), schwanz=(0.0, 0.0, 0.12, 0.0))


def rampe(fs, v):
    """Wert 0..1 -> Hautfarbe: Furche fast schwarz, Schatten, Grundhaut, Rotbraun, Rotbraun mit Rosa (Glanz)."""
    h, r, ro = fs['mat']['grundhaut_dunkel'], fs['mat']['rotbraun_mittelton'], fs['mat']['rosa_stelle']
    stufen = [(0.00, h * 0.16), (0.26, h * 0.62), (0.50, h * 1.0), (0.72, h * 0.55 + r * 0.45), (0.90, r * 0.92), (1.00, r * 0.78 + ro * 0.22)]
    xs = np.array([s[0] for s in stufen])
    out = np.zeros(v.shape + (3,), np.float32)
    for c in range(3):
        out[..., c] = np.interp(v, xs, np.array([s[1][c] for s in stufen]))
    return out


class Maler:
    """Alles ausser der Farbe wird einmal gerechnet (Wertfeld, Zonen, Marken, Gegenproben); farbe() setzt je Variante und Helligkeit zusammen."""

    def __init__(self, form, g, teile, lage, stand, L=None, C=None):
        self.teile, self.lage = teile, lage
        kn, pos, nrm = FM.texeldaten(form)
        m = kn >= 0
        tid, U, V = FM.teilkarte(teile, lage, kn)
        Rr, Tr = FM.pose(form, 2)
        off = bind_versatz(form)
        Pb = np.zeros(kn.shape + (3,), np.float32)
        for b in np.unique(kn[m]):
            s = kn == b
            Pb[s] = pos[s] + off[b].astype(np.float32)
        D = np.zeros(kn.shape + (3,), np.float32)
        D[..., 0] = 1
        gew = np.zeros(kn.shape + (4,), np.float32)
        gew[m] = GEWICHT['rumpf']
        ist_schwanz = np.zeros(kn.shape, bool)
        for i, p in enumerate(teile):
            s = tid == i
            if not s.any():
                continue
            D[s] = np.asarray(p.d, np.float32) / max(np.linalg.norm(p.d), 1e-6)
            gew[s] = GEWICHT[art(p.name)]
        idx = {p.name: p.knochen for p in teile}
        kette = [idx[n] for n in SCHWANZ if n in idx]
        ist_schwanz |= np.isin(kn, kette)
        gew[ist_schwanz] = GEWICHT['schwanz']
        paare = [(a, b) for k_, a in enumerate(kette) for b in kette[k_ + 1:]]
        ao, _w = falten(form, kn, pos, Rr, Tr, teile, paare)
        Rt = drehung_je_texel(form, kn, Pb, Rr, teile)
        # E9: Kappen der Schwanzglieder tragen die Bemalung ihres Gelenkrings (Malort auf den Gelenkring geklemmt, Radius des Gelenks).
        e9 = form.get('_e9') or {}
        for nm_, inf in e9.items():
            s_ = kn == inf['knochen']
            if not s_.any():
                continue
            q_ = Pb[s_].astype(np.float64)
            sk = -(q_[:, 0] - inf['x0'])
            lo_, hi_ = inf['s']
            if nm_ == 'schwanz1':
                lo_ = -1e9
            ra, rb = inf['r']
            aussen = (sk < lo_) | (sk > hi_)
            if not aussen.any():
                continue
            sc = np.clip(sk, lo_, hi_)
            yz = q_[:, 1:] - np.array(inf['achse'])
            rr = np.linalg.norm(yz, axis=1, keepdims=True)
            rj = np.where(sk < lo_, ra, rb)[:, None]
            q2 = q_.copy()
            q2[:, 0] = inf['x0'] - sc
            q2[:, 1:] = np.array(inf['achse']) + yz / np.maximum(rr, 1e-6) * rj
            q_[aussen] = q2[aussen]
            Pb[s_] = q_.astype(np.float32)
        N = int(m.sum())
        pb = Pb[m].astype(np.float64)
        dd = D[m].astype(np.float64)
        laengs = (pb * dd).sum(1, keepdims=True)

        def woelbung(p_):
            la = (p_ * dd).sum(1, keepdims=True)
            q = (p_ - la * dd + la * dd / ZELLE_LANG) / ZELLE
            f1, f2 = zellen(q, 3)
            return f1, f2, np.clip(1 - f1 / 0.8, 0, 1) ** 0.8
        f1, f2, woelb = woelbung(pb)
        rand = f2 - f1
        # Furchen nur an einem Teil der Grenzen, mit wechselnder Breite (gemalt, kein Pflaster): Maske und Breite aus grobem Rauschen
        an = glatt_stufe(rauschen(pb / 380.0, 53), -0.3, 0.3)
        breite = 0.06 + 0.10 * (0.5 + 0.5 * rauschen(pb / 210.0, 59))
        furche = (1 - glatt_stufe(rand, 0.0, breite)) * (0.3 + 0.7 * an)
        h = 12.0
        grad = np.zeros((N, 3))
        for k in range(3):
            e = np.zeros(3)
            e[k] = h
            grad[:, k] = (woelbung(pb + e)[2] - woelbung(pb - e)[2]) / (2 * h)
        grad *= ZELLE * 0.5
        nb = nrm[m].astype(np.float64)
        gt = grad - (grad * nb).sum(1, keepdims=True) * nb
        G4 = gew[m].astype(np.float64)
        n_g = nb - 1.0 * G4[:, 1:2] / 0.32 * gt
        n_g /= np.maximum(np.linalg.norm(n_g, axis=1, keepdims=True), 1e-9)
        Rm = Rt[m].astype(np.float64)
        licht = np.clip(np.einsum('nij,nj->ni', Rm, n_g) @ LICHT, -0.7, 1.0)
        licht_glatt = np.clip(np.einsum('nij,nj->ni', Rm, nb) @ LICHT, -0.7, 1.0)
        quer = pb - laengs * dd
        pf = quer / FASER_QUER + dd * (laengs / FASER_LAENGS)
        faser = 0.65 * rauschen(pf, 31) + 0.35 * rauschen(pf * 2.1, 37)
        korn = rauschen(pb / 20.0, 41)
        gross = FM.fraktal(pb, 650.0, 2, 61)
        ad = np.abs(FM.fraktal(pb * np.array([1, 1.5, 1]), 230.0, 2, 9))
        ader = np.clip(1 - ad / 0.045, 0, 1)
        aom = ao[m].astype(np.float64)
        v = (0.50 + 0.30 * licht + 0.10 * G4[:, 2] / 0.12 * faser + 0.045 * korn - G4[:, 0] * furche + 0.14 * G4[:, 1] / 0.32 * (woelb - 0.5)
             + 0.10 * gross - 0.42 * (1 - aom) / 0.6)
        self.v = np.clip(v, 0, 1)
        # fuer vm_fleisch (D): Rohschichten je Texel (Reihenfolge wie m)
        self.pb, self.licht_g, self.woelb, self.furche, self.faser, self.ao_m, self.korn = pb, licht, woelb, furche, faser, aom, korn
        self.dd = dd
        self.ader = ader * (G4[:, 3] / 0.3)
        self.fl = glatt_stufe(faser, 0.42, 0.62) * glatt_stufe(licht, 0.35, 0.75) * (1 - furche) * np.where(ist_schwanz[m], 0.5, 1.0)
        self.m, self.kn, self.tid, self.ist_schwanz = m, kn, tid, ist_schwanz
        lichtk = np.zeros(kn.shape, np.float32)
        lichtk[m] = licht_glatt
        furk = np.zeros(kn.shape, np.float32)
        furk[m] = furche * G4[:, 0]
        self.lichtk = lichtk
        vk = np.full(kn.shape, 0.5, np.float32)
        vk[m] = self.v
        self.vk = vk
        # Zonen (rf_textur.zonen wie fb_mal)
        merk = np.full((TEX_H, TEX_W, 3), 7, np.uint8)
        z = RT.zonen(merk.copy(), teile, lage, L, C)
        spez = (z != 7).any(-1) & m
        au = (z == np.array(RT.AUGE, np.uint8)).all(-1) & spez
        kern = cv2.erode(au.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
        z[au & ~kern] = 7
        spez = spez & ~(au & ~kern)
        z[kern] = (92, 22, 20)
        self.rb = (z == np.array(RT.ROTBRAUN, np.uint8)).all(-1) & spez
        self.z, self.spez = z.astype(np.float32), spez
        self.schatt = np.clip(0.55 + 0.45 * (lichtk + 0.3), 0.35, 1.15) * (1 - 0.5 * furk) * (0.55 + 0.45 * ao)
        self.horn = np.ones(kn.shape, np.float32)
        for i, p in enumerate(teile):
            if p.name.startswith(('klaue', 'kralle', 'zeh')):
                s = tid == i
                self.horn[s] = 0.7 + 0.55 * np.clip((V[s] - 0.45) / 0.5, 0, 1)
        import rf_textur
        geschont = np.zeros(kn.shape, bool)
        for p, r in zip(teile, lage):
            if p.feld is not None:
                x, y, ww, hh = r
                geschont[y:y + hh, x:x + ww] = True
        bk = rf_textur.Bake(form, stand, kn, pos, geschont)
        self.bk = bk
        # Bild 1 sieht den rechten Arm von hinten: dort Haut statt Rotbraun (wie fb_mal, Pruefstelle N2)
        tausch = np.zeros(kn.shape, bool)
        for i, p in enumerate(teile):
            if p.name in ('hand_r', 'unterarm_r', 'kralle_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'):
                x, y, ww, hh = lage[i]
                sub = bk.vis[1][y:y + hh, x:x + ww] & (tid[y:y + hh, x:x + ww] == i)
                if p.name != 'unterarm_r':
                    sub = sub & ~bk.vis[2][y:y + hh, x:x + ww]
                else:
                    sub = cv2.dilate(sub.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
                tausch[y:y + hh, x:x + ww] |= sub
        self.tausch = tausch & m
        self.marken = marken_flaechen(bk, tid, m)
        self.n12 = []
        for n, (x0, y0, x1, y1) in ((2, (288, 223, 342, 282)), (1, (382, 300, 413, 356))):
            zone = np.zeros((P.HOEHE, P.BREITE), np.uint8)
            zone[y0:y1, x0:x1] = 1
            self.n12.append(bk.stelle(n, zone))
        _, self.naechst = _naechste(m)

    def farbe(self, variante, gain=1.0, gain_schwanz=1.0):
        """gain: Helligkeit der Haut und Zonen (nicht der Zeichnung), am Standbild geeicht (vm_bau.eichen). Schwanz ein eigener Wert, gleich fuer alle Glieder."""
        fs = farbsatz(variante)
        m = self.m
        gk = np.where(self.ist_schwanz[m], gain_schwanz, gain)[:, None]
        haut = rampe(fs, self.v)
        ader_f = frei(fs, (62, 14, 20))
        haut = haut + (ader_f[None] - haut) * (0.45 * self.ader)[:, None] * np.clip(1.2 - self.v, 0, 1)[:, None]
        haut = haut + (fs['mat']['feine_linien'][None] * 0.8 - haut) * (0.7 * self.fl)[:, None]
        haut = haut * gk
        farbe = np.zeros(m.shape + (3,), np.float32)
        farbe[m] = haut
        zf = self.z.copy()
        zf[self.rb] = fs['mat']['rotbraun_mittelton'] * 0.8
        q = self.spez & ~self.rb
        zf[q] = frei(fs, zf[q])
        gz = np.full(m.shape, gain, np.float32)
        gz[self.ist_schwanz] = gain_schwanz
        farbe[self.spez] = zf[self.spez] * (self.schatt * gz)[self.spez][:, None]
        farbe = farbe * self.horn[..., None]
        hb = np.zeros_like(farbe)
        hb[m] = haut
        farbe[self.tausch] = hb[self.tausch]
        farbe = marken_malen(farbe, self.marken, fs, self.lichtk, self.vk)
        for s in self.n12:
            Y = farbe @ YK
            zu = s & (Y > Y_N)
            farbe[zu] *= (Y_N / Y[zu])[:, None]
        iy, ix = self.naechst
        farbe[~m] = farbe[iy[~m], ix[~m]]
        return np.clip(np.round(farbe), 0, 255).astype(np.uint8)


def backen(form, g, teile, lage, stand, farbe='A', L=None, C=None, ausgabe=None, gain=1.0, gain_schwanz=1.0):
    ml = Maler(form, g, teile, lage, stand, L, C)
    if ausgabe is not None:
        ausgabe.update(dict(kn=ml.kn, tid=ml.tid, m=ml.m, maler=ml))
    return ml.farbe(farbe, gain, gain_schwanz), ml.bk


def _naechste(m):
    from scipy.ndimage import distance_transform_edt
    d, ind = distance_transform_edt(~m, return_indices=True)
    return d, (ind[0], ind[1])


def marken_flaechen(bk, tid, m):
    """Marken: Lage aus fb_mal.zeichnung (dieselben Masken, dieselben Ausschluesse fuer G gegen Bild 1), aber gemalt: Deckkraft > 0,5 wird feste Flaeche,
    je Teil geschlossen; dazu der Randring (1 Texel). -> Liste (id, Farbname oder None, Grundfarbe, Faktor, Flaeche, Rand)."""
    import rf_marken as RM
    pf = os.path.join(HIER, 'zw', 'marken.json')
    k = json.load(open(pf)) if os.path.exists(pf) else {}
    arm1 = np.zeros((P.HOEHE, P.BREITE), bool)
    arm1[296:360, 378:418] = True
    arm1t = bk.stelle(1, arm1.astype(np.uint8))
    g_ok = (np.isin(bk.kn, [17, 18]) | ~bk.vis[1]) & ~arm1t
    z5z = np.zeros((P.HOEHE, P.BREITE), np.uint8)
    z5z[266:304, 366:424] = 1
    z5t = bk.stelle(1, z5z)
    aus = []
    for i, n, mk, f in FM.liste():
        a = FM.deckkraft(bk, n, mk)
        nm = [x for x, v in RM.FARBE.items() if tuple(v) == tuple(f)]
        nm = nm[0] if nm else None
        if i in ('Gg', 'Gr'):
            a = a * g_ok
            ab = a * z5t
            aus.append((i + '_z5', 'band', None, float(k.get('Z5', 1.0))) + _flaeche_maske(ab, tid, m, rand=False))
            a = a * ~z5t
        elif i == 'P':
            a = a * (~bk.vis[1])
        aus.append((i, nm, np.array(f, float), float(k.get(i, 1.0))) + _flaeche_maske(a, tid, m, rand=i not in ('Gr', 'Xa', 'Xb', 'Xc')))
    return aus


SPANNE = dict(rosa='rosa_stelle', band='helle_baender', linie='feine_linien', rot='rotbraun_mittelton', rosa_end='rosa_stelle')


def _f_grenzen(ziel, nm):
    """Spanne des Lichtfaktors, in der die Marke in ihrer Materialspanne bleibt (form_pruefung.material_maske: von - 8 .. bis + 8, hier 6)."""
    mt = SPANNE.get(nm)
    if nm == 'kern':
        # weisskerne: mindestens so hell wie das Band (form_pruefung.material_maske, Formblatt 2.1)
        yb = float(np.array(_PAL['helle_baender']['oberflaeche_rgb'], float) @ YK) + 2
        return max(0.95, yb / max(float(ziel @ YK), 1)), 1.15
    if mt is None:
        return 0.95, 1.12
    von = np.array(_PAL[mt]['oberflaeche_von'], float) - 6
    bis = np.array(_PAL[mt]['oberflaeche_bis'], float) + 6
    z = np.maximum(ziel, 1)
    lo, hi = float((von / z).max()), float((bis / z).min())
    if lo > hi:
        return 1.0, 1.0
    return max(lo, 0.86), min(hi, 1.15)


def marken_malen(rgb, marken, fs, lichtk, vk):
    """Farbe der Variante; die gemalte Form der Haut (Wert, Licht von oben) scheint durch, je Marke begrenzt auf ihre Materialspanne
    (Lauf _probe/C3e9: ohne Grenze fiel Z5 auf Material 0,09, C4e9: Q1/Q4 mit Faktor 1,4-1,5 ueber die Obergrenze). Rand 1 Texel nur auf Haut,
    nie auf einer anderen Marke, Schattenseite dunkler."""
    f0 = 1.02 + 0.25 * (vk - 0.5) + 0.08 * lichtk
    rd = np.where(lichtk < 0.15, 0.68, 0.82)
    alle = np.zeros(vk.shape, bool)
    for _i, _nm, _g, _f, s, _r in marken:
        alle |= s
    for i, nm, grund, fak, s, r in marken:
        if not s.any():
            continue
        ziel = (markenfarbe(fs, None, nm) if nm else frei(fs, grund)) * fak
        lo, hi = _f_grenzen(ziel, nm)
        f = np.clip(f0[s], lo, hi)
        rgb[s] = np.clip(ziel[None] * f[:, None], 0, 255)
    for i, nm, grund, fak, s, r in marken:
        if r is not None:
            q = r & ~alle
            rgb[q] = rgb[q] * rd[q][:, None]
    return rgb


def _flaeche_maske(a, tid, m, rand=True):
    s = (a > 0.5) & m
    neu = np.zeros_like(s)
    for t in np.unique(tid[s]):
        q = (s & (tid == t)).astype(np.uint8)
        q = cv2.morphologyEx(q, cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8))
        neu |= (q > 0) & (tid == t)
    r = ((cv2.dilate(neu.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & ~neu & m) if rand and neu.any() else None
    return neu, r
