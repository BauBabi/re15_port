#!/usr/bin/env python3
"""Variante E (E12): Farben aus den Screenshots wie D, aber geordnet und gemalt. Kein RE-Wert.

 1. Farbe nur grossflaechig: die Projektion (vm_bild: Schirmfarbe / Port-Licht je Texel) wird im Ruheraum (Pose Clip 2, Weltlage je Texel) in ein
    3D-Gitter gesplattet und mit Gauss geglaettet (FELD_SIGMA Einheiten). Jedes Texel liest das Feld an seiner Lage. Dadurch laeuft die Farbe ueber
    Teilgrenzen, Gelenke und UV-Naehte durch, unsichtbare Seiten bekommen die Farbe der Nachbarschaft (kein Spiegeln), und es gibt keine Flecken
    unter rund FELD_SIGMA (bei Bild 1 rund 0,1 px je Einheit: unter 13 px).
    Markentexel (Wunden) zaehlen nicht ins Hautfeld (sonst hellt die Wunde die Haut auf).
    Bild 2 (Feuerschein, den das Port-Licht nicht kennt): dort zaehlt nur die Helligkeit, der Farbton kommt aus dem Feld von Bild 1 (gross geglaettet).
 2. Gemaltes Detail mit Richtung (vm_mal.Maler, gleiche Gewichte fuer alle Teile, Schwanz ohne Zellen nach E4): Licht von oben, Muskelstraenge
    laengs (Fasern), Furchen, Woelbung, Umgebungsverdeckung, verzweigte Adern. Multiplikativ auf das Farbfeld.
 3. Wunden: Lage aus den Bildmasken (vm_mal.marken_flaechen), Umriss geglaettet (Gauss 0,8 Texel, Schwelle), Kleinteile unter 3 Texel weg. Aufbau nach
    Abstand zum Rand: Rand (aufgerissene Haut, hell), Fleisch (Bildfarbe, rosa), Tiefe (dunkelrot), Glanz (selten, oben im Licht). Helligkeit je Wunde
    wie D in Schleifen am Screenshot geeicht. Z5 (Pruefpunkte kappe_*, Gegenprobe N2): Rand in der Spanne helle_baender, ohne Glanz.
"""
import json
import os

import cv2
import numpy as np
from scipy.ndimage import gaussian_filter, map_coordinates

from rf_kern import FORM
import form_pruefung as P
import fb_mal as FM
import vm_mal
import vm_bild

YK = np.array([.299, .587, .114], np.float32)
FELD_SIGMA = 110.0          # Einheiten; Glieder haben 300-800 Umfang/Radius, so bleibt je Glied ein Verlauf laengs und rundum. Frei gesetzt.
FELD_SIGMA_GROSS = 450.0    # Farbton von Bild 1 fuer die Vorderseite und Rueckfall fuer Stellen ohne Messung
VOXEL = 40.0
DETAIL = 0.8                # Staerke des gemalten Details (Exponent auf den Wert von vm_mal)


def ruhelage(form, ml):
    """Weltlage je Texel (Reihenfolge ml.m) in der Ruhepose (Clip 2)."""
    Rr, Tr = FM.pose(form, 2)
    off = vm_mal.bind_versatz(form)
    kn = ml.kn[ml.m]
    loc = ml.pb - off[kn]
    R = np.stack([Rr[b] for b in kn]).astype(np.float64)
    T = np.stack([Tr[b] for b in kn]).astype(np.float64)
    return np.einsum('nij,nj->ni', R, loc) + T


class Feld:
    """3D-Splat mit Gauss: farbe(pos) = Summe(w c) / Summe(w) im Gitter, beides mit demselben Gauss geglaettet."""

    def __init__(self, pos_alle, voxel=VOXEL):
        self.lo = pos_alle.min(0) - 3 * FELD_SIGMA_GROSS
        hi = pos_alle.max(0) + 3 * FELD_SIGMA_GROSS
        self.v = voxel
        self.n = np.ceil((hi - self.lo) / voxel).astype(int) + 1

    def glaetten(self, pos, farbe, gew, sigma):
        idx = np.round((pos - self.lo) / self.v).astype(int)
        S = np.zeros(tuple(self.n) + (3,), np.float32)
        W = np.zeros(tuple(self.n), np.float32)
        np.add.at(W, tuple(idx.T), gew.astype(np.float32))
        for c in range(3):
            np.add.at(S[..., c], tuple(idx.T), (gew * farbe[:, c]).astype(np.float32))
        s = sigma / self.v
        W = gaussian_filter(W, s, mode='constant')
        for c in range(3):
            S[..., c] = gaussian_filter(S[..., c], s, mode='constant')
        return S, W

    def lesen(self, S, W, pos):
        q = ((pos - self.lo) / self.v).T
        w = map_coordinates(W, q, order=1, mode='nearest')
        c = np.stack([map_coordinates(S[..., k], q, order=1, mode='nearest') for k in range(3)], 1)
        return c, w


def proben(ml, licht):
    """Wie vm_bild.aus_bild, aber je Bild getrennt: (Farbe, Gewicht) je Texel in Reihenfolge ml.m."""
    bk, m = ml.bk, ml.m
    logo = P.logo_maske()
    aus = {}
    for n in (1, 2):
        echt = cv2.imread(os.path.join(FORM, 'farbe_bild%d_echt.png' % n))[:, :, ::-1].astype(np.float32)
        L, rm = licht[n]
        innen = cv2.distanceTransform((rm & ~logo).astype(np.uint8), cv2.DIST_L2, 3).astype(np.float32)
        q = bk.q[n]
        sel = bk.vis[n] & m
        S = vm_bild._abtasten(echt, q, sel)
        Lt = vm_bild._abtasten(L, q, sel)
        d = vm_bild._abtasten(innen[..., None], q, sel)[:, 0]
        ly = Lt @ YK
        w = np.clip((ly - vm_bild.LICHT_MIN) / 0.25, 0, 1) * np.clip(d / vm_bild.RAND_PX[n], 0, 1)
        t = np.clip(S / np.maximum(Lt, 0.08), 0, 255)
        T = np.zeros(m.shape + (3,), np.float32)
        Wm = np.zeros(m.shape, np.float32)
        T[sel] = t
        Wm[sel] = w
        aus[n] = (T[m], Wm[m])
    return aus


def farbfeld(form, ml, licht, ausgabe=None):
    m = ml.m
    pos = ruhelage(form, ml)
    pr = proben(ml, licht)
    marke = np.zeros(m.shape, bool)
    for i, nm, grund, fak, s_, r in ml.marken:
        marke |= s_
        if r is not None:
            marke |= r
    marke = cv2.dilate(marke.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    mk = marke[m]
    f = Feld(pos)
    # im Wurzelraum mitteln (dunkle Haut traegt ihre Unterschiede, helle Stellen dominieren nicht)
    t1, w1 = pr[1]
    t2, w2 = pr[2]
    w1 = w1 * ~mk
    w2 = w2 * ~mk
    q1, q2 = np.sqrt(t1 / 255.0), np.sqrt(t2 / 255.0)
    # Farbton von Bild 1, gross geglaettet (Vorderseite, Rueckfall)
    Sg, Wg = f.glaetten(pos, q1, w1, FELD_SIGMA_GROSS)
    cg, wg = f.lesen(Sg, Wg, pos)
    ton1 = cg / np.maximum(wg, 1e-6)[:, None]
    # Bild 2: Helligkeit behalten, Farbton von Bild 1 (Feuerschein heraus)
    y2 = q2 @ YK
    yt = ton1 @ YK
    q2t = ton1 * (y2 / np.maximum(yt, 1e-4))[:, None]
    farbe = np.concatenate([q1, q2t])
    gew = np.concatenate([w1, 0.5 * w2])
    pp = np.concatenate([pos, pos])
    S, W = f.glaetten(pp, farbe, gew, FELD_SIGMA)
    c, w = f.lesen(S, W, pos)
    fein = c / np.maximum(w, 1e-6)[:, None]
    # Rueckfall: wo das feine Feld kaum Messung hat, ins grobe Feld uebergehen (Gesamtmittel, wo auch das fehlt)
    alpha = np.clip(w / 0.05, 0, 1)[:, None]
    med = np.average(q1[w1 > 0.3], axis=0) if (w1 > 0.3).any() else np.array([.35, .3, .32])
    grob = np.where((wg > 0.02)[:, None], ton1, med[None])
    q = fein * alpha + grob * (1 - alpha)
    rgb = (np.clip(q, 0, 1) ** 2) * 255.0
    # Kein Gruen: die Figur zeigt im Video Violett, Rot, Blau; Gruen entsteht in der Rueckrechnung dunkler Stellen (D/E1: Schwanzansatz oliv).
    # Ueberschuss von G ueber max(R, B) kappen, Helligkeit (Y) halten.
    y0 = rgb @ YK
    gm = np.maximum(rgb[:, 0], rgb[:, 2])
    rgb[:, 1] = np.minimum(rgb[:, 1], gm)
    rgb *= (y0 / np.maximum(rgb @ YK, 1e-3))[:, None]
    if ausgabe is not None:
        ausgabe.update(dict(gemessen=float((w1 + w2 > 0.15).mean()), rueckfall=float((alpha[:, 0] < 0.5).mean())))
    return rgb


def detail(ml):
    """Gemalter Wert je Texel aus vm_mal (Licht, Fasern, Furchen, Woelbung, Verdeckung, Korn), gleiche Gewichte fuer alle Teile, Schwanz ohne Zellen.
    -> Faktor (Mittel 1) und Adermaske."""
    # eigener Wert fuer E: Muskelstraenge laengs (Fasern) staerker, Furchen der Zellen schwaecher (E2 las wieder als Pflaster), Licht von oben,
    # Woelbung, Verdeckung, wenig Korn. Schwanz ohne Zellen (E4). Gleiche Gewichte fuer alle anderen Teile.
    schw = ml.ist_schwanz[ml.m]
    v = (0.5 + 0.30 * ml.licht_g + 0.20 * ml.faser + 0.03 * ml.korn - np.where(schw, 0.0, 0.26) * ml.furche
         + np.where(schw, 0.0, 0.10) * (ml.woelb - 0.5) - 0.42 * (1 - ml.ao_m) / 0.6)
    v = np.clip(v, 0, 1)
    # Kontrast wie A-C (Rampe 0,16 x .. 2,5 x Grundhaut, rund 1:15); E1 mit 1,25 hatte rund 1:3 und las glatt
    fak = np.exp(DETAIL * 2.2 * (v - np.median(v)))
    fak = fak / np.mean(fak)
    return np.clip(fak, 0.18, 3.2), ml.ader


def wunden_formen(ml):
    """Markenflaechen geglaettet: Gauss 0,8 Texel je Teil, Schwelle 0,5, Kleinteile unter 3 Texel weg. -> Liste (id, Maske), Abstand zum Rand."""
    m, tid = ml.m, ml.tid
    aus = []
    alle = np.zeros(m.shape, bool)
    for i, nm, grund, fak, s_, r in ml.marken:
        if not s_.any():
            aus.append((i, s_))
            continue
        neu = np.zeros(m.shape, bool)
        for t in np.unique(tid[s_]):
            if t < 0:
                continue
            q = (s_ & (tid == t)).astype(np.float32)
            g = cv2.GaussianBlur(q, (0, 0), 0.8)
            q2 = (g > 0.45) & (tid == t)
            n, lab, st, _ = cv2.connectedComponentsWithStats(q2.astype(np.uint8), connectivity=8)
            for k in range(1, n):
                if st[k, cv2.CC_STAT_AREA] >= 3:
                    neu |= lab == k
        aus.append((i, neu))
        alle |= neu
    tief = cv2.distanceTransform(alle.astype(np.uint8), cv2.DIST_L2, 3)
    return aus, alle, tief


WUND_FLEISCH = np.array([196, 92, 112], np.float32)   # rosa Fleisch (Handwerk RE2-Birkin: Kern rot-rosa), nur als Richtung des Farbtons
WUND_TIEF = np.array([84, 14, 24], np.float32)
WUND_GLANZ = np.array([240, 226, 234], np.float32)


def zusammen(form, ml, feld_rgb, faktoren=None, gain=1.0):
    m = ml.m
    H, W = m.shape
    fak, ader = detail(ml)
    haut = feld_rgb * fak[:, None] * gain
    adf = np.array([62, 14, 20], np.float32)
    haut = haut + (adf[None] - haut) * (0.35 * ader)[:, None]
    out = np.zeros((H, W, 3), np.float32)
    out[m] = haut
    # Zonen (Haende, Krallen, Zaehne, Maul, Auge) wie vm_mal, aber auf der Feldfarbe: Horn heller zur Spitze
    out = out * ml.horn[..., None]
    fs = vm_mal.farbsatz('C')
    zon = ml.spez & ~ml.rb & ~ml.tausch
    if zon.any():
        out[zon] = vm_mal.frei(fs, ml.z[zon]) * ml.schatt[zon][:, None]
    fa = np.zeros((H, W), np.float32)
    fa[m] = ml.faser
    # Wunden
    formen, alle, tief = wunden_formen(ml)
    pos = np.zeros((H, W, 3), np.float32)
    pos[m] = ml.pb.astype(np.float32)
    n2 = vm_mal.rauschen(pos / 16.0, 73)
    n1 = vm_mal.rauschen(pos / 30.0, 71)
    # Bildfarbe der Wunde: Projektion (D-Grundtextur) im Mittel ueber die Wunde, Farbton daraus
    if not hasattr(ml, '_d_psx'):
        raise RuntimeError('ml._d_psx fehlt (Projektion je Texel aus vm_bild)')
    dps = ml._d_psx
    z5 = np.zeros((H, W), bool)
    for i, s_ in formen:
        if not s_.any():
            continue
        k = (faktoren or {}).get(i.replace('_z5', ''), 1.0)
        bild = dps[s_].mean(0) * k
        d = tief[s_]
        istz5 = i == 'Z5' or i.endswith('_z5')
        yb = float(bild @ YK)
        fleisch = WUND_FLEISCH * (yb / float(WUND_FLEISCH @ YK))
        rand = np.clip(bild * 1.18 + 10, 0, 255)
        mitte = 0.5 * bild + 0.5 * fleisch
        c = np.zeros((int(s_.sum()), 3), np.float32)
        r_ = d < 1.5
        c[r_] = rand
        c[~r_] = mitte
        tiefst = (~r_) & (d >= 2.5) & (n1[s_] < -0.15)
        c[tiefst] = c[tiefst] * 0.35 + WUND_TIEF * 0.65 * (yb / 120.0)
        if not istz5:
            gl = (~r_) & (n2[s_] > 0.55) & (ml.lichtk[s_] > 0.1)
            c[gl] = c[gl] * 0.3 + WUND_GLANZ * 0.7
        # Fasern im Fleisch, leise (Z5 nur abdunkelnd, Gegenprobe N2)
        c[~r_] *= (1 + (0.12 if not istz5 else -0.12) * np.abs(fa[s_][~r_]) * (1 if not istz5 else 1))[:, None]
        out[s_] = c
        if istz5:
            z5 |= s_
    # entzuendeter Ring aussen (1 Texel)
    ring = (cv2.dilate(alle.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & ~alle & m
    out[ring] = out[ring] * 0.75 + np.array([92, 26, 36], np.float32) * 0.25 * (out[ring] @ YK / 60.0)[:, None]
    # Z5 in die Spanne helle_baender (Pruefpunkte kappe_*), Helligkeit bleibt
    if z5.any():
        rand_z5 = z5 & (tief < 1.5)
        von = np.array(vm_mal._PAL['helle_baender']['oberflaeche_von'], np.float32) + 2
        bis = np.array(vm_mal._PAL['helle_baender']['oberflaeche_bis'], np.float32) - 2
        out[rand_z5] = np.clip(out[rand_z5], von, bis)
    # Haende von hinten in Bild 1: wie vm_mal (Haut statt Zone) entfaellt, die Farbe kommt ohnehin aus dem Feld
    for s_ in ml.n12:
        Y = out @ YK
        zu = s_ & (Y > vm_mal.Y_N)
        out[zu] *= (vm_mal.Y_N / Y[zu])[:, None]
    iy, ix = ml.naechst
    out[~m] = out[iy[~m], ix[~m]]
    return np.clip(np.round(out), 0, 255).astype(np.uint8)


def backen_eichen(form, ml, aus, form_json, schreib, runden=5):
    """Farbfeld einmal, Wunden je Stelle wie D am Screenshot geeicht (Faktor auf die Bildfarbe der Wunde)."""
    import form_video as FV
    licht = vm_bild.licht_bilder(aus, form_json)
    info = {}
    feld = farbfeld(form, ml, licht, info)
    # Bildfarbe je Texel (wie D vor dem Zusammenfassen), nur fuer den Farbton der Wunden
    roh, bekannt, _ = vm_bild.aus_bild(ml, licht)
    voll, _hat = vm_bild.ergaenzen(ml, roh, bekannt)
    ml._d_psx = np.nan_to_num(voll)
    liste = [(i, n, mk) for i, n, mk, _f in FM.liste()]
    Yo = {n: P.original(n).astype(np.float32) @ YK for n in (1, 2)}
    logo = P.logo_maske()
    fak, verlauf, gain = {}, [], 1.0
    alle_mk = {n: np.zeros((P.HOEHE, P.BREITE), bool) for n in (1, 2)}
    for i, n, mk in liste:
        alle_mk[n] |= cv2.dilate((mk > 0).astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    haut_mk = {n: (P.ziel_laden(n)[1] == 255) & ~logo & ~alle_mk[n] for n in (1, 2)}
    for r in range(runden + 1):
        rgb = zusammen(form, ml, feld, fak, gain)
        schreib(rgb)
        if r == runden:
            break
        k = P.Kandidat(form_json)
        Yr = {}
        for n in (1, 2):
            sauber, _a, _m, _ = k.rendern(n)
            Yr[n] = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False).astype(np.float32) @ YK
        zeile = {}
        for i, n, mk in liste:
            q = (mk > 0) & ~logo
            if q.sum() < 4:
                continue
            ys, yr = float(Yo[n][q].mean()), float(Yr[n][q].mean())
            zeile[i] = (round(ys), round(yr))
            fak[i] = float(np.clip(fak.get(i, 1.0) * (ys / max(yr, 1.0)) ** 1.3, 0.4, 4.0))
        q = float(np.sqrt((Yo[1][haut_mk[1]].mean() / max(Yr[1][haut_mk[1]].mean(), 1)) * (Yo[2][haut_mk[2]].mean() / max(Yr[2][haut_mk[2]].mean(), 1))))
        zeile['_haut'] = (round(gain, 3), round(q, 3))
        gain *= q ** 1.2
        verlauf.append(zeile)
    info['haut_gain'] = round(gain, 4)
    info['marken_faktoren'] = {k: round(v, 3) for k, v in fak.items()}
    info['wunden_y_screenshot_render'] = verlauf[-1] if verlauf else {}
    return rgb, info
