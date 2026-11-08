#!/usr/bin/env python3
"""Ruhige Haut K5e (Nutzer 2026-11-08, Messblatt ruhe/MESSBLATT_RUHE.md): Variante, Spiel-Asset bleibt unangetastet.

    python kf_k5e.py <par.json> <aus-Ordner>        (deterministisch: zweiter Lauf byte-gleich)

Eingaben (fest, sha256 in bau.json):
  feinschliff/K5d/FORM.EMD              Geometrie (Krallen 19/20/28/29 eingezogen), EDD/EMR/3 Posen-Clips
  stufe3/basis_k5c/FORM.EMD/.TIM        UV, Seite, Eckreihenfolge je Flaeche; TIM-Pixel (= Spiel-Asset, G4-Indizes auf Seite 0-2)
  varianten/K5/einbau_k5c/BAHNSTEIG_FORM.EMD   Spiel-Asset K5c (bis E32 shared_assets/RE2): 28 Clips, EDD/EMR/Morph (nur MD1 wird ersetzt)
  RE2 CDEMD0.EMS Typ 0x34 (G4, TIM @0x65e800)    die drei G4-Originalrampen (kf_g4.g4_roh / g4_cluts)
  ruhe/messung/material.json            Orte: Flaechenliste je Material (Messblatt RUHE 3.4)
Ausgaben in <aus>: FORM.EMD/FORM.TIM/form.json (3-Clip-Basis fuer die Szenenkette), BAHNSTEIG_FORM.EMD/.TIM (28 Clips), bau.json.

Verfahren (Messblatt RUHE 3.3/3.4; Memory reai-v2-texturen-nicht-aus-video-eichen):
 - Texel und UV bleiben (TIM-Pixel bytegleich K5c, UV/Seite je Flaeche = K5c): keine Insel, keine Eichung je Fleck/Region/Texel.
 - Materialien nur ueber CLUT-Zeilen (10 Zeilen): jede Flaeche liest die Zeile ihres Materials, Index = G4-Index.
 - Je Material EIN Parametersatz (par.json) auf der G4-Originalrampe der Seite: Haut L* + d, Farbton h, C* (weich an den sRGB-Rand);
   Adern rangtreu auf [Haut-p10, q x Haut-p50], C* = kappa x C, Farbton der Haut; helle Eintraege in die Hautmitte.
   Gelenk = Hautabbildung + dL, eigenes h/C; Fleisch = Hautabbildung + dL, eigenes h/C. Rangfolge je Klasse bleibt (steigende Abbildungen).
 - Zeile 0 (Auge, Maulinneres) = G4-Zeile 0, Zeile 8 (F1) = K5c-Zeile 5, Zeile 9 (Zaehne) = K5c-Zeile 7, bytegleich.

Stufe F1-Rand (Nutzer E32 2026-11-09: "Warum hast du es jetzt beim 4 beiner um den Fleck herum hinten am hintern schwarz gemacht? also um den
roten Fleck herum?"; Schalter par['f1_rand']['an'], ohne den Schalter ist der Bau bytegleich K5e):
 - Kern- und Randmaske des F1-Feldes (Seite 3) kommen aus dem K5c-Bau selbst: kf_g4.bau wird in einen Wegwerf-Ordner nachgebaut und die Maske
   des einzigen binary_fill_holes-Aufrufs (kf_g4.py F1-Stelle, Mehrheitsregel V3R3) mitgeschnitten. Pruefung: sha256 des Nachbau-TIM =
   farbe/K5c_farbe_v3 (512828...), Pixel = basis_k5c, Kern 122 / Rand 70 Texel = farbe/K5c_farbe_v3/bau.json sonder.F1.
 - Kern-Eintraege (Indizes der Kerntexel) bleiben bytegleich K5c-Zeile 5 (E30 gelobt).
 - Alle uebrigen Eintraege, die die 6 F1-Flaechen lesen (G4-F11-Haut des Feldes, dunkler Fleischrand), folgen der Hauttransformation der
   Hautzeilen (par['haut']: L* + dL, Farbton h, C* weich an den sRGB-Rand, naechste 15-Bit-Farbe); Eingang ist ihr K5c-Wort (Haut = G4-Zeile 2).
   Fuer die Haut-Eintraege ist das Ergebnis wortgleich K5e-Zeile 3 (G4-Seite 2); der Bau prueft das. Texel, UV und Flaechen bleiben.
"""
import hashlib
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
FORM = os.path.normpath(os.path.join(K5, '..', '..'))
WT = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
sys.path.insert(0, os.path.join(WT, 're15_port', 'tools'))
sys.path.insert(0, HIER)
import form_emd as FE  # noqa: E402
import kf_g4 as KG  # noqa: E402

BASIS = os.path.join(FORM, 'stufe3', 'basis_k5c')
K5D = os.path.join(K5, 'feinschliff', 'K5d')
ASSET = os.path.join(K5, 'einbau_k5c', 'BAHNSTEIG_FORM')   # Spiel-Asset K5c (E30), seit E32 gesichert (einbau_k5c/K5C.md); shared_assets traegt K5e_f1
MATERIAL = os.path.join(K5, 'ruhe', 'messung', 'material.json')
NZ = 10


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


def gew_quantil(x, w, q):
    o = np.argsort(x, kind='stable')
    cs = np.cumsum(w[o])
    return float(np.interp(q, (cs - 0.5 * w[o]) / cs[-1], x[o]))


def klassen(g4zeile):
    """Klassen je G4-Eintrag nach kf_g4.r4_eintraege: haut (w 0, L* < R4_L_HELL), hell (w 0, L* >= R4_L_HELL), ader (w > 0); 0x0000 keine."""
    L, _C0, _h0, w = KG.r4_eintraege(g4zeile)
    z0 = np.array(g4zeile) != 0
    return L, (w == 0) & (L[:, 0] < KG.R4_L_HELL) & z0, (w == 0) & (L[:, 0] >= KG.R4_L_HELL) & z0, (w > 0) & z0


_KOMBI = np.array([[a, b, c] for a in range(-1, 3) for b in range(-1, 3) for c in range(-1, 3)])


def runde15(rgb8, ziel_lab, richtung=0):
    """15-Bit-Rundung EINES Eintrags (keine neue Farbe, nur die Wahl des 5-Bit-Nachbarn): von den 4x4x4 Nachbarn um rgb8 x 31/255 der mit
    kleinstem dE76 zum Lab-Ziel des Materialsatzes. richtung -1 (Ader): zuerst nur Nachbarn mit L* und a* <= Ziel, damit die Rundung eine Ader
    nie heller oder roeter macht (RV6 "nie heller und nie roeter als die Haut", riegel_farbe_v3.rv6 dL/da* <= 0; wie kf_k5d.lab_wort).
    Grund (gemessen, Lauf 1): das Abschneiden r >> 3 (form_emd.clut_wort) streute den Farbton der Haut-Eintraege auf 21,5 Grad
    (Gelenk 55,3, Fleisch 18,0; Riegel H4); die naechste 15-Bit-Farbe ergibt Haut 12,0 / Gelenk 0,5 / Fleisch 6,8 Grad."""
    t = np.asarray(ziel_lab, float)
    k = np.clip(np.floor(np.asarray(rgb8, float) * 31.0 / 255.0)[None, :] + _KOMBI, 0, 31)
    L = KG.lab(k * 255.0 / 31.0)
    d = np.linalg.norm(L - t[None, :], axis=1)
    if richtung < 0:
        ri = (L[:, 0] <= t[0] + 1e-6) & (L[:, 1] <= t[1] + 1e-6)
        if ri.any():
            d = np.where(ri, d, d + 1e6)
    c = k[int(np.argmin(d))].astype(int)
    return int(c[0]) | (int(c[1]) << 5) | (int(c[2]) << 10)


def rampe(g4zeile, cnt, h, C, Lfun, q, kappa, hell_neigung):
    """EINE Transformation auf eine G4-Originalzeile (Messblatt RUHE 3.4).
    Haut: L' = Lfun(L*); hell: Haut-p50' + hell_neigung x (L* - R4_L_HELL); Ader: rangtreu linear auf [Haut-p10', q x Haut-p50'],
    Buntheit kappa x C. Farbton h fuer alle, C* weich an den sRGB-Rand (Cmax x tanh(C/Cmax)); 15 Bit ueber runde15 (naechste Farbe).
    0x0000 bleibt (transparent); 0 nach Rundung wird 0x8000 (opakes Schwarz)."""
    L, haut, hell, ader = klassen(g4zeile)
    z0 = np.array(g4zeile) != 0
    cnt = np.asarray(cnt, float)
    hg = haut & (cnt > 0)
    Lh = Lfun(L[:, 0])
    p10 = gew_quantil(Lh[hg], cnt[hg], 0.10)
    p50 = gew_quantil(Lh[hg], cnt[hg], 0.50)
    p90 = gew_quantil(Lh[hg], cnt[hg], 0.90)
    Ln = np.where(haut, Lh, 0.0)
    if hell.any():
        Ln[hell] = p50 + hell_neigung * (L[hell, 0] - KG.R4_L_HELL)
    ag = ader & (cnt > 0)
    if ag.any():
        v0, v1 = float(L[ag, 0].min()), float(L[ag, 0].max())
        lo, hi = p10, q * p50
        Ln[ader] = np.clip(lo + (L[ader, 0] - v0) / max(v1 - v0, 1e-6) * (hi - lo), min(lo, hi), hi)
    Cw = np.where(ader, kappa * C, C)
    cm = KG.r4_cmax(Ln, h)
    Cn = np.where(cm > 1e-6, cm * np.tanh(Cw / np.maximum(cm, 1e-6)), 0.0)
    hr = math.radians(h)
    ziel = np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1)
    rgb = KG.rgb_aus_lab(ziel)
    neu = list(g4zeile)
    for i in range(256):
        if not z0[i]:
            continue
        wn = runde15(rgb[i], ziel[i], -1 if ader[i] else 0) | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000
    return neu, dict(haut_p10_p50_p90=[round(p10, 3), round(p50, 3), round(p90, 3)], ader_genutzt=int(ag.sum()), haut_genutzt=int(hg.sum()),
                     hell_genutzt=int((hell & (cnt > 0)).sum()))


def flaechen_idx(form, tim):
    """{(mesh, viereck, fi): (seite, zeile, Texel-Indizes im UV-Polygon)} (fillPoly + polylines wie ruhe_material.flaechen)."""
    px = tim['pixel']
    out = {}
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                seite = int(f['page']) & 0xF
                po = seite * 128
                uv = f['uv']
                pts = [uv[0], uv[1], uv[3], uv[2]] if len(uv) == 4 else uv
                p = np.array([[a + po, b] for a, b in pts], np.int32)
                mk = np.zeros(px.shape, np.uint8)
                cv2.fillPoly(mk, [p], 1)
                cv2.polylines(mk, [p], True, 1)
                out[(mi, vi, fi)] = (seite, ((int(f['clut']) >> 6) & 0x1FF) - tim['clut_y'], px[mk.astype(bool)])
    return out


def material_je_flaeche():
    mj = json.load(open(MATERIAL, encoding='utf-8'))
    m = {}
    for nm in ('Haut', 'Gelenk', 'Fleisch_Schwanzspitze', 'Fleisch_Buckel', 'Zahn', 'Auge', 'Maulinneres', 'F1'):
        for s in mj['material'][nm]['liste']:
            k = tuple(int(x) for x in s.split('|'))
            if k in m:
                raise SystemExit('Flaeche %s doppelt (%s, %s)' % (k, m[k], nm))
            m[k] = nm
    quell = {int(z): v['g4_quellzeile'] for z, v in mj['zeilen_k5c'].items()}
    return m, quell


def g4_rampen():
    _e, t, q = KG.g4_roh()
    _px, cl = KG.g4_cluts(t)
    return cl, q


def material_seite(F, mat, quell):
    """{flaeche: (material-gruppe, G4-Seite)}: Seite 0-2 direkt, Seite 3 ueber die G4-Quellzeile ihrer K5c-Zeile (material.json)."""
    out = {}
    for k in sorted(F):
        seite, z16, _idx = F[k]
        out[k] = (mat[k], seite if seite < 3 else quell[z16])
    return out


def zeilen(par, F, mat, quell, cl_k5c, cl_g4):
    """Zeilenplan + Zeile je Flaeche. F = flaechen_idx des K5c (UV/Seite/Indizes)."""
    Z = par['zeilen']
    hp, ap, gp, fp = par['haut'], par['ader'], par['gelenk'], par['fleisch']
    if hp['modus'] != 'hebung':
        raise SystemExit('nur Hebung (Messblatt RUHE 0.4)')
    d = float(hp['dL'])
    zeile_je, nutz = {}, {}
    for k, (nm, s) in material_seite(F, mat, quell).items():
        if nm == 'Haut':
            z, gr = Z['haut'][s], ('haut', s)
        elif nm == 'Gelenk':
            z, gr = Z['gelenk'][s], ('gelenk', s)
        elif nm.startswith('Fleisch'):
            if s != 1:
                raise SystemExit('Fleisch-Flaeche %s nicht auf G4-Seite 1' % (k,))
            z, gr = Z['fleisch'], ('fleisch', 1)
        elif nm in ('Auge', 'Maulinneres'):
            z, gr = Z['g4_0'], None
        elif nm == 'Zahn':
            z, gr = Z['zahn'], None
        else:
            z, gr = Z['f1'], None
        zeile_je[k] = z
        if gr:
            n = nutz.setdefault(gr, np.zeros(256))
            np.add.at(n, F[k][2], 1)
    worte = [None] * NZ
    info = {}
    worte[Z['g4_0']] = [int(x) for x in cl_g4[0]]
    worte[Z['f1']] = [int(x) for x in cl_k5c[5]]
    f1_par = par.get('f1_rand') or {}
    worte[Z['zahn']] = [int(x) for x in cl_k5c[7]]
    for s in range(3):
        g4z = [int(x) for x in cl_g4[s]]
        worte[Z['haut'][s]], info['haut%d' % s] = rampe(g4z, nutz[('haut', s)], hp['h'], hp['C'], lambda L: L + d, ap['q'], ap['kappa'],
                                                        hp['hell_neigung'])
        g = float(gp['dL'])
        worte[Z['gelenk'][s]], info['gelenk%d' % s] = rampe(g4z, nutz[('gelenk', s)], gp['h'], gp['C'], lambda L, g=g: L + d + g, ap['q'],
                                                            ap['kappa'], hp['hell_neigung'])
    g = float(fp['dL'])
    worte[Z['fleisch']], info['fleisch1'] = rampe([int(x) for x in cl_g4[1]], nutz[('fleisch', 1)], fp['h'], fp['C'], lambda L: L + d + g,
                                                  ap['q'], ap['kappa'], hp['hell_neigung'])
    if f1_par.get('an'):
        worte[Z['f1']], info['f1_rand'] = f1_rand_zeile(cl_k5c[5], F, mat, hp, worte[Z['haut'][2]], [int(x) for x in cl_g4[2]])
    return worte, zeile_je, info, {'%s%d' % gr: int(n.sum()) for gr, n in sorted(nutz.items())}, nutz


_F1_MASKE = {}


def f1_masken():
    """Kern-/Randmaske des F1-Feldes (Seite 3, 512 x 256) aus dem K5c-Bau (kf_g4.bau = Spiel-Asset K5c, Pixel bytegleich basis_k5c).
    Weg: kf_g4.bau in einen Wegwerf-Ordner, scipy.ndimage.binary_fill_holes mitschneiden (einziger Aufruf: kern = fill_holes(kern) & feld);
    Rand = 1 Texelring (3x3-Dilatation) im Feld ohne Kern (kf_g4.py F1-Stelle, Messblatt V3 3.1).
    Pruefungen (sonst Abbruch): Nachbau-TIM sha256 = farbe/K5c_farbe_v3/bau.json sha256.tim, Pixel = basis_k5c/FORM.TIM,
    Kern/Rand-Zahl = bau.json sonder.F1 kern_texel/rand_texel."""
    if _F1_MASKE:
        return _F1_MASKE
    import shutil
    import tempfile
    import scipy.ndimage as ndi
    v3 = os.path.join(K5, 'farbe', 'K5c_farbe_v3')
    soll = json.load(open(os.path.join(v3, 'bau.json'), encoding='utf-8'))
    orig, mit = ndi.binary_fill_holes, []

    def haken(x, *a, **k):
        r = orig(x, *a, **k)
        mit.append(np.array(r, bool))
        return r
    tmp = tempfile.mkdtemp(prefix='kf_k5e_f1_')
    ndi.binary_fill_holes = haken
    try:
        b = KG.bau(tmp)
        tim_n = open(os.path.join(tmp, 'FORM.TIM'), 'rb').read()
    finally:
        ndi.binary_fill_holes = orig
        shutil.rmtree(tmp, ignore_errors=True)
    if len(mit) != 1:
        raise SystemExit('kf_g4.bau: %d binary_fill_holes-Aufrufe statt 1' % len(mit))
    if hashlib.sha256(tim_n).hexdigest() != soll['sha256']['tim'] or b['sha256']['tim'] != soll['sha256']['tim']:
        raise SystemExit('Nachbau kf_g4 != farbe/K5c_farbe_v3 (TIM)')
    px_n = FE.tim_lesen(tim_n)['pixel']
    px_b = FE.tim_lesen(open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read())['pixel']
    if not np.array_equal(px_n, px_b):
        raise SystemExit('Nachbau-Pixel != basis_k5c')
    fx, fy, fw, fh = soll['sonder']['F1']['feld']
    feld = np.zeros(px_b.shape, bool)
    feld[fy:fy + fh, fx:fx + fw] = True
    kern = mit[0] & feld
    rand = cv2.dilate(kern.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & feld & ~kern
    if int(kern.sum()) != soll['sonder']['F1']['kern_texel'] or int(rand.sum()) != soll['sonder']['F1']['rand_texel']:
        raise SystemExit('F1 Kern/Rand %d/%d != bau.json %d/%d' % (kern.sum(), rand.sum(), soll['sonder']['F1']['kern_texel'],
                                                                   soll['sonder']['F1']['rand_texel']))
    _F1_MASKE.update(kern=kern, rand=rand, feld=feld, feld_xywh=[int(fx), int(fy), int(fw), int(fh)], tim_sha=soll['sha256']['tim'])
    return _F1_MASKE


def f1_kern_eintraege():
    """Indizes der F1-Kerntexel (f1_masken) im K5c-TIM: diese Eintraege bleiben bytegleich K5c-Zeile 5 (E32)."""
    px = FE.tim_lesen(open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read())['pixel']
    return sorted(int(x) for x in np.unique(px[f1_masken()['kern']]))


def f1_rand_zeile(z5, F, mat, hp, zeile3, g4z2):
    """F1-Zeile (K5c-Zeile 5) mit Hautrand (E32): Kern-Eintraege bytegleich; jeder andere Eintrag, den die 6 F1-Flaechen lesen, bekommt die
    Hauttransformation der Hautzeilen (L* + dL, Farbton h, C* = Cmax tanh(C / Cmax), runde15 naechste Farbe) auf sein K5c-Wort. 0x0000 bleibt.
    Pruefung: Eintraege, deren K5c-Wort = G4-Zeile 2 ist (F11-Haut), ergeben dasselbe Wort wie K5e-Zeile 3 (rampe, G4-Seite 2)."""
    M = f1_masken()
    px = FE.tim_lesen(open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read())['pixel']
    f1 = sorted(k for k, v in mat.items() if v == 'F1')
    gelesen = np.zeros(256, int)
    for k in f1:
        np.add.at(gelesen, F[k][2], 1)
    kern_e = set(int(x) for x in np.unique(px[M['kern']]))
    z5 = [int(x) for x in z5]
    L = KG.r4_eintraege(z5)[0]
    um = [i for i in range(256) if gelesen[i] > 0 and i not in kern_e and z5[i] != 0]
    if any(L[i, 0] >= KG.R4_L_HELL for i in um):
        raise SystemExit('F1-Rand: Eintrag mit L* >= R4_L_HELL (helle Klasse braucht Haut-p50)')
    d = float(hp['dL'])
    Ln = np.array([L[i, 0] + d for i in um])
    cm = KG.r4_cmax(Ln, hp['h'])
    Cn = np.where(cm > 1e-6, cm * np.tanh(float(hp['C']) / np.maximum(cm, 1e-6)), 0.0)
    hr = math.radians(hp['h'])
    ziel = np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1)
    rgb = KG.rgb_aus_lab(ziel)
    neu = list(z5)
    for j, i in enumerate(um):
        w = runde15(rgb[j], ziel[j], 0) | (z5[i] & 0x8000)
        neu[i] = w if w != 0 else 0x8000
    haut_g4 = [i for i in um if z5[i] == g4z2[i]]
    abw3 = [i for i in haut_g4 if neu[i] != zeile3[i]]
    if abw3:
        raise SystemExit('F1-Rand: Haut-Eintraege %s != K5e-Zeile 3 (nicht derselbe Parametersatz)' % abw3)
    Lo = KG.r4_eintraege(neu)[0]
    inv = sum(1 for a in um for b in um if L[a, 0] < L[b, 0] - 1e-9 and Lo[a, 0] > Lo[b, 0] + 1e-9)
    rand_fl = M['rand'] & ~np.isin(px, haut_g4)
    rand_e = set(int(x) for x in np.unique(px[rand_fl]))
    feld_haut = M['feld'] & ~M['kern'] & ~M['rand']
    info = dict(kern_texel=int(M['kern'].sum()), rand_texel=int(M['rand'].sum()), feld_xywh=M['feld_xywh'],
                kern_eintraege=len(kern_e), umgefaerbt=len(um), umgefaerbt_liste=um, haut_eintraege_g4=len(haut_g4),
                haut_eintraege_gleich_zeile3=len(haut_g4) - len(abw3),
                fleischrand_eintraege=len(rand_e), fleischrand_eintraege_auch_kern=len(rand_e & kern_e),
                fleischrand_eintraege_nur_rand=len(rand_e - kern_e),
                rand_texel_haut=int((M['rand'] & np.isin(px, haut_g4)).sum()), rand_texel_fleisch=int(rand_fl.sum()),
                rand_texel_fleisch_folgen=int((rand_fl & np.isin(px, um)).sum()),
                rand_texel_fleisch_kern_eintrag=int((rand_fl & np.isin(px, sorted(kern_e))).sum()),
                feld_haut_texel=int(feld_haut.sum()), feld_haut_texel_folgen=int((feld_haut & np.isin(px, um)).sum()),
                rang_vertauschungen=int(inv), nachbau_tim_sha256=M['tim_sha'])
    return neu, info


def eingaenge():
    """Gemeinsame Eingaenge fuer Generator und Riegel (H3 rechnet damit nach)."""
    emd_c = open(os.path.join(BASIS, 'FORM.EMD'), 'rb').read()
    tim_c = open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read()
    fc = FE.lese_emd(emd_c)
    T = FE.tim_lesen(tim_c)
    cl_k5c = [[int(x) for x in z] for z in T['cluts']]
    cl_g4, g4q = g4_rampen()
    F = flaechen_idx(fc, T)
    mat, quell = material_je_flaeche()
    return dict(emd_c=emd_c, fc=fc, T=T, cl_k5c=cl_k5c, cl_g4=cl_g4, g4q=g4q, F=F, mat=mat, quell=quell)


def bauen(par, aus):
    E = eingaenge()
    emd_c, fc, T, cl_k5c, cl_g4, F, mat, quell = (E[k] for k in ('emd_c', 'fc', 'T', 'cl_k5c', 'cl_g4', 'F', 'mat', 'quell'))
    fd = FE.lese_emd(open(os.path.join(K5D, 'FORM.EMD'), 'rb').read())
    # Gegenprobe der Quelle: K5c-Zeilen 0/1 = G4-Zeilen 0/1 bytegleich. G4-Zeile 2 steht in der 16-Zeilen-K5c NICHT mehr (K5c-Zeile 2 ist
    # die Schwanz-Gruppenzeile, stufe3/basis_k5c/zeilen.json); sie kommt darum direkt aus CDEMD0.EMS (kf_g4.g4_roh), wie alle drei Rampen.
    for s in range(2):
        if cl_k5c[s] != [int(x) for x in cl_g4[s]]:
            raise SystemExit('K5c-Zeile %d != G4-Zeile %d (FARBE.md V3.1 verletzt)' % (s, s))
    if set(mat) != set(F):
        raise SystemExit('material.json deckt nicht alle Flaechen: %d / %d' % (len(set(mat) & set(F)), len(F)))
    worte, zeile_je, info, nutz, _n = zeilen(par, F, mat, quell, cl_k5c, cl_g4)
    # Geometrie K5d, UV/Seite/Eckreihenfolge K5c, nur das CLUT-Wort neu
    n_fl = 0
    for mi, (md, mc) in enumerate(zip(fd['meshes'], fc['meshes'])):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            if len(md[art]) != len(mc[art]):
                raise SystemExit('Mesh %d %s: Flaechenzahl K5d != K5c' % (mi, art))
            for fi, (a, b) in enumerate(zip(md[art], mc[art])):
                if list(a['e']) != list(b['e']):
                    raise SystemExit('Mesh %d %s %d: Eckindizes K5d != K5c' % (mi, art, fi))
                n_fl += 1
                a['uv'] = [list(p) for p in b['uv']]
                a['page'] = int(b['page'])
                a['clut'] = ((((T['clut_y'] + zeile_je[(mi, vi, fi)]) & 0x1FF) << 6) | (int(b['clut']) & 0x3F))
                for kx in ('tex', 'zeile'):
                    a.pop(kx, None)
    os.makedirs(aus, exist_ok=True)
    emd = FE.schreibe_emd(fd)
    tim = FE.tim_schreiben(T['pixel'], worte, T['clut_x'], T['clut_y'], T['bild_x'], T['bild_y'])
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(emd)
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(tim)
    fj = json.load(open(os.path.join(K5D, 'form.json'), encoding='utf-8'))
    fj['name'] = par.get('name', 'K5e')
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    a = open(ASSET + '.EMD', 'rb').read()
    dir_off, dd = FE.verzeichnis(a)
    md1_alt = a[dd[7]:dir_off]
    md1_neu = FE.md1_schreiben(fd)
    doff_c, dc = FE.verzeichnis(emd_c)
    if len(md1_neu) != len(md1_alt) or emd_c[dc[7]:doff_c] != md1_alt:
        raise SystemExit('MD1 des Assets passt nicht zur Basis K5c')
    open(os.path.join(aus, 'BAHNSTEIG_FORM.EMD'), 'wb').write(a[:dd[7]] + md1_neu + a[dir_off:])
    open(os.path.join(aus, 'BAHNSTEIG_FORM.TIM'), 'wb').write(tim)
    zaehl = {}
    for z in zeile_je.values():
        zaehl[z] = zaehl.get(z, 0) + 1
    bericht = dict(
        variante=par.get('name', 'K5e'), par=par, g4_quelle=E['g4q'], zeilen=NZ, port_texturhoehe=NZ * 256,
        flaechen=n_fl, flaechen_je_zeile={str(z): zaehl[z] for z in sorted(zaehl)},
        material_je_flaeche_zaehlung={nm: sum(1 for v in mat.values() if v == nm) for nm in sorted(set(mat.values()))},
        texelzugriffe_je_material_seite=nutz, rampen=info,
        texel_neu=int((FE.tim_lesen(tim)['pixel'] != T['pixel']).sum()),
        sha256={n: sha(os.path.join(aus, n)) for n in ('FORM.EMD', 'FORM.TIM', 'BAHNSTEIG_FORM.EMD', 'BAHNSTEIG_FORM.TIM')},
        eingang_sha256=dict(basis_k5c_emd=sha(os.path.join(BASIS, 'FORM.EMD')), basis_k5c_tim=sha(os.path.join(BASIS, 'FORM.TIM')),
                            k5d_emd=sha(os.path.join(K5D, 'FORM.EMD')), asset_emd=sha(ASSET + '.EMD'), asset_tim=sha(ASSET + '.TIM'),
                            material_json=sha(MATERIAL)))
    json.dump(bericht, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    return bericht, worte, zeile_je


if __name__ == '__main__':
    par = json.load(open(sys.argv[1], encoding='utf-8'))
    b, _w, _z = bauen(par, sys.argv[2])
    print(json.dumps({k: b[k] for k in ('zeilen', 'flaechen', 'flaechen_je_zeile', 'texel_neu', 'rampen', 'sha256')}, indent=1))
