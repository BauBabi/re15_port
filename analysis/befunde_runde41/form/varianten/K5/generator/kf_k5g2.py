#!/usr/bin/env python3
"""K5g2 - Grundlage v2, Streifen, Krallen (Runde 41, Riesenform; Auftrag K5g Runde 1 von 2, 2026-11-09). Variante, Spiel-Asset bleibt.

    python kf_k5g2.py <par.json> <aus-Ordner>        Bau (deterministisch: zweiter Lauf byte-gleich)
    python kf_k5g2.py --f <par.json>                 haut.f per Bisektion (Bild 2 Video >= Vorlage, W6 wie K5f) in par.json schreiben

Eingang: ruhe/K5e_f1 (= Spiel-Asset re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*, sha256 geprueft), stufe3/basis_k5c (Kralle 28, UV/Seite/Material
der K5c), RE2 CDEMD0.EMS Typ 0x34 (G4-Rampen), zonen_nutzer/v2/zonen_v2.json (Grundlage v2, abgenommen), ruhe/messung/material.json.

Stufe 1 Geometrie (kf_k5g2_geo):
 E1 Kopf 22 / Unterkiefer 23: Ecken, die in Brust 10 / Hals 21 liegen, entlang der Geraden zum Anker zurueckgenommen (Weg a), Rand par.
    Gesicht/Augen/Maul (Flaechen der Zonen 29, 30, 41, 42) fest. Bildmenge: alle Clips ausser par['geometrie']['ausnahme_clips']
    (Begruendung in par und FARBE.md: dort zieht die Animation den Kopfkern in die Brust).
 E2 Kralle 28 = K5c-Form; rechte Krallen 19/20/29/31 = Spiegel der linken 14/15/28/30 im Raum der Hand. Hinten unveraendert.
Stufe 2 Textur (kf_k5g2_tex): Grundlage v2 Zone fuer Zone, V1-V6 (siehe kf_k5g2_tex.__doc__).
Ausgabe: BAHNSTEIG_FORM.EMD/.TIM (28 Clips), FORM.EMD/.TIM/form.json (3-Clip-Basis fuer Tafeln/Riegel), bau.json.
"""
import hashlib
import json
import math
import os
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_g4 as KG  # noqa: E402
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402
import kf_k5g2_unterteil as U  # noqa: E402

FE = E.FE
K5 = E.K5
WT = E.WT
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
ASSET = os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM')
BASIS = E.BASIS
PAR = os.path.join(K5, 'k5g', 'K5g2', 'k5g2_par.json')
UT_PLAN = os.path.join(K5, 'k5g', 'K5g2', 'k5g2_unterteilung.json')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def log(*a):
    print(*a, flush=True)


# ================================================================== Stufe 1: Geometrie
def fest_ecken(form, Z, teil):
    zon = {z['nr']: z for z in Z['zonen']}
    s = set()
    for nr in ('29', '30', '41', '42'):
        for k in zon[nr]['modell_zuordnung']['liste']:
            m, v, i = TX.schluessel(k)
            if m == teil:
                s |= set(form['meshes'][m]['vierecke' if v else 'dreiecke'][i]['e'])
    return s


def geometrie(par, Z, emd_asset, tim_asset):
    """Runde 2 (Geometrie-Bauer, 2026-11-10): Stufe 1 = kf_k5g2_geo. -> (form, bericht).
    form = K5e_f1 mit den Krallen (O2), Flaechen der Teile 10/16/21/22 noch wie K5e_f1 (Schluessel wie zonen_v2) - darauf laeuft die Textur;
    die Mulde (O1) kommt danach in bauen() ueber G.mulde_auf_form(form) (Netze der Mulden-Tabelle, Kinder erben clut/Seite/Verschiebung der Mutter).
    Die fertige Geometrie mit der Textur von K5e_f1 baut `python kf_k5g2_geo.py bauen` (k5g/K5g2/geo/BAHNSTEIG_FORM.EMD)."""
    _emd0, form = G.geometrie_form(emd_asset, stufen=('krallen',))
    rezept = json.load(open(G.GEO_KRALLEN, encoding='utf-8'))
    ber = dict(stufe='kf_k5g2_geo.geometrie_form', krallen=dict(rezept='generator/k5g2_krallen.json', paare=rezept['paare'],
                                                             rechts={k: dict(grad=v['grad'], achse=v['achse']) for k, v in rezept['rechts'].items()}),
               mulde=dict(tabelle='generator/k5g2_mulde_tabelle.json', teile=list(G.MULDEN_TEILE)))
    return form, ber


def geometrie_runde1(par, Z, emd_asset, tim_asset):
    """Stand Runde 1 (Zuruecknehmen des Kopfes + Rastpose-Spiegel der Krallen) - nicht mehr benutzt, nur zum Nachlesen."""
    gp = par['geometrie']
    form = FE.lese_emd(emd_asset)
    mo = FE.modell(emd_asset, tim_asset)
    cb, R, T = G.posen(mo)
    cb = np.array(cb)
    ausn = [int(c) for c in gp['ausnahme_clips']]
    sel = ~np.isin(cb[:, 0], ausn)
    H = G.Huellen(form, R[sel], T[sel])
    ber = dict(bilder_gesamt=int(len(cb)), bilder_bedingung=int(sel.sum()), ausnahme_clips=ausn)
    for teil, anker in ((22, gp['anker_kopf']), (23, gp['anker_kiefer'])):
        fest = fest_ecken(form, Z, teil)
        neu, proto, betroffen = G.zuruecknehmen(form, R[sel], T[sel], teil, fest, anker, float(gp['rand']), huellen=H)
        form['meshes'][teil]['ecken'] = [[int(x) for x in e] for e in neu]
        ber['teil_%d' % teil] = dict(fest=len(fest), betroffen=len(betroffen), bewegt=sum(1 for v in proto.values() if 'neu' in v),
                                     fest_innen=sorted(int(j) for j, v in proto.items() if 'status' in v), ecken={str(j): v for j, v in sorted(proto.items())})
        log('E1 Teil %d: %d betroffen, %d bewegt' % (teil, len(betroffen), ber['teil_%d' % teil]['bewegt']))
    # E2: Kralle 28 in K5c-Form, rechte Krallen = Spiegel der linken
    k5c = FE.lese_emd(open(os.path.join(BASIS, 'FORM.EMD'), 'rb').read())
    m28 = k5c['meshes'][28]
    if [x['e'] for x in m28['vierecke']] != [x['e'] for x in form['meshes'][28]['vierecke']]:
        raise SystemExit('Kralle 28: K5c-Topologie != Asset')
    form['meshes'][28]['ecken'] = [list(e) for e in m28['ecken']]
    form['meshes'][28]['normalen'] = [list(n) for n in m28['normalen']]
    kp = gp['krallen']
    i2 = [i for i, (c, b, _k) in enumerate(cb) if c == int(kp['pose'][0]) and b == int(kp['pose'][1])][0]
    kr_ber = {}
    for links, rechts in G.KRALLEN_PAARE:
        G.spiegel_kralle(form, links, rechts)
        d = [int(x) for x in kp['verschub'].get(str(rechts), [0, 0, 0])]
        kr_ber[str(rechts)] = G.kralle_drehpunkt(form, links, rechts, R[i2], d)
    ber['krallen'] = dict(kralle28='K5c-Ecken/Normalen (stufe3/basis_k5c/FORM.EMD Teil 28)', paare=[list(p) for p in G.KRALLEN_PAARE],
                          spiegel='diag(1,1,-1) im Handraum; Drehpunkt der rechten Kralle (EMR-Versatz, bleibt) so ausgeglichen, dass die '
                                  'Kralle in der Bild-2-Haltung (Spiel-Clip %d Bild %d) genau das Spiegelbild der linken ist; Umlauf gedreht' % tuple(kp['pose']),
                          je_kralle=kr_ber)
    return form, ber


# ================================================================== Stufe 2: Textur
def nutzung(keys, F_neu, plan_seite=None):
    n = np.zeros(256)
    for k in keys:
        np.add.at(n, F_neu[k], 1)
    return n


def circ_mittel(h, w):
    r = np.radians(h)
    return float(np.degrees(np.arctan2((np.sin(r) * w).sum(), (np.cos(r) * w).sum())))


def textur(form, par, X, Z):
    tp = par['textur']
    T = X['T']
    px0 = T['pixel']
    shape = px0.shape
    cl_g4 = [[int(x) for x in z] for z in X['cl_g4']]
    cl_k5c = X['cl_k5c']
    plan = TX.materialplan(form, X, Z)
    flaechen = dict(TX.alle_flaechen(form))
    idx_alt = {}
    for k, f in flaechen.items():
        idx_alt[k] = np.nonzero(TX.flaeche_maske(f, shape))
    lab_g4 = [TX.lab_zeile(z) for z in cl_g4]
    klassen = [E.klassen(z) for z in cl_g4]    # (L, haut, hell, ader)
    ber = {}

    def gruppe(art, s):
        return [k for k, v in plan.items() if v == (art, s)]

    def nutz(keys):
        n = np.zeros(256)
        for k in keys:
            np.add.at(n, px0[idx_alt[k]], 1)
        return n
    # ---- Haut-Parameter (Quelle je Wert in par): Farbton = G4-Hautton, k aus Eichungsband C* 2-6
    hw, hh, hc = [], [], []
    for s in range(3):
        n = nutz(gruppe('haut', s))
        L, haut, _hell, _ader = klassen[s]
        sel = haut & (n > 0)
        Cs = TX.buntheit(lab_g4[s][sel])
        hw.append(n[sel]); hh.append(TX.ton(lab_g4[s][sel])); hc.append(Cs)
    hw, hh, hc = np.concatenate(hw), np.concatenate(hh), np.concatenate(hc)
    mk = hc >= 1.0
    h_g4 = round(circ_mittel(hh[mk], hw[mk]), 2)
    c_med = TX.gew_quantil(hc, hw, 0.5)
    f, kn = float(tp['haut']['f']), float(tp['haut']['knie'])
    # s (Buntheit je Helligkeit, modus 'L'): texelgewichteter Median C' der Hautklasse = C_ziel
    Lsk = np.concatenate([TX.knie(lab_g4[s_][klassen[s_][1] & (nutz(gruppe('haut', s_)) > 0), 0] * f, kn) for s_ in range(3)])
    l_med = TX.gew_quantil(Lsk, hw, 0.5)
    k_haut = round(float(tp['haut']['C_ziel']) / l_med, 4)
    rp = tp['rundung']
    band = tp['haut']['ton_band']
    h_haut = float(tp['haut']['h'])
    if not band[0] <= h_haut <= band[1]:
        raise SystemExit('Hautton %.1f ausserhalb des Bandes der Originale %s' % (h_haut, band))
    ber['haut'] = dict(h=h_haut, h_g4_hautklasse_info=h_g4, C_median_g4_hautklasse_info=round(c_med, 3), L_median_hautklasse=round(l_med, 3), s=k_haut, f=f, knie=kn)
    zeilen = {}
    for s in range(3):
        zeilen[('haut', s)] = TX.zeile_transformieren(cl_g4[s], f, k_haut, h_haut, kn, rp)

    # ---- Tausch-Zeilen (Lippe, Unterkiefer aussen, Kopf-Unterseite, Augenhoehle, alte Augen-/Maulflaechen): Rang -> Hautrampe
    def hautrampe(s):
        n = nutz(gruppe('haut', s))
        L, haut, _h, _a = klassen[s]
        sel = np.nonzero(haut & (n > 0))[0]
        o = sel[np.argsort(L[sel, 0], kind='stable')]
        return o, n[o]

    def rang_auf_rampe(Lq, wq, rampe_idx, rampe_w):
        """je Quelle (L, Gewicht) der Rampeneintrag am gleichen texelgewichteten Rang (Mittelrang)."""
        o = np.argsort(Lq, kind='stable')
        cs = np.cumsum(wq[o])
        q = np.empty(len(Lq))
        q[o] = (cs - 0.5 * wq[o]) / cs[-1]
        rc = np.cumsum(rampe_w)
        rq = (rc - 0.5 * rampe_w) / rc[-1]
        j = np.clip(np.searchsorted(rq, q), 0, len(rampe_idx) - 1)
        return rampe_idx[j]
    # Runde 2 (Pruefer r1 Sicht 6/5): Haende 13/18 lasen helle G4-Eintraege (Hand 18 Bild 2 L* 31,4 gegen Haut 6,7; 1132 px reinweiss). Haende
    # und Krallen/Zehen bekommen wie Lippe/Unterkiefer den Rang-Tausch auf die Hautrampe ihrer G4-Seite (Hand "im Band der uebrigen Haut",
    # V5 "Krallenfarbe wie die Hand"); je Gruppe eigener texelgewichteter Rang, ein schon belegter Eintrag behaelt die Zuordnung der
    # frueheren Gruppe (Reihenfolge Lippe/Tausch, Hand, Kralle; Konflikte in bau.json textur.tausch).
    tausch_info = {}
    for s in range(3):
        gruppen_s = [(g, gruppe(g, s)) for g in ('tausch', 'hand', 'kralle')]
        if not any(keys for _g, keys in gruppen_s):
            continue
        ri, rw = hautrampe(s)
        z = list(zeilen[('haut', s)])
        belegt_e = {}
        info = {}
        for g, keys in gruppen_s:
            if not keys:
                continue
            n = nutz(keys)
            ein = np.nonzero((n > 0) & (np.array(cl_g4[s]) != 0))[0]
            ziel = rang_auf_rampe(lab_g4[s][ein, 0], n[ein], ri, rw)
            konf = 0
            for i, j in zip(ein, ziel):
                if int(i) in belegt_e:
                    konf += 1
                    continue
                belegt_e[int(i)] = g
                z[i] = zeilen[('haut', s)][j]
            info[g] = dict(flaechen=len(keys), eintraege=int(len(ein)), schon_belegt=konf)
        zeilen[('tausch', s)] = z
        tausch_info[s] = info
    for k, v in list(plan.items()):
        if v[0] in ('hand', 'kralle'):
            plan[k] = ('tausch', v[1])
    ber['tausch'] = tausch_info
    hand = [k for k in flaechen if k[0] in (13, 18)]
    kral = [k for k in flaechen if k[0] in TX.KRALLEN_TEILE]

    def med_L_zeile(keys):
        Ls, ws = [], []
        for s in range(3):
            ks = [k for k in keys if plan[k][1] == s]
            if not ks:
                continue
            n = nutz(ks)
            sel = (n > 0) & (np.array(cl_g4[s]) != 0)
            Ls.append(TX.lab_zeile(zeilen[('tausch', s)])[sel, 0]); ws.append(n[sel])
        return TX.gew_quantil(np.concatenate(Ls), np.concatenate(ws), 0.5)
    ber['kralle'] = dict(regel='Rang-Tausch auf die Hautrampe wie die Haende (Runde 2)', median_hand_L=round(med_L_zeile(hand), 2),
                         median_kralle_L=round(med_L_zeile(kral), 2), flaechen=len(kral))

    # ---- F1 (V6): Kern-Eintraege bytegleich K5c-Zeile 5, uebrige gelesene Eintraege Haut (wie kf_k5f.f1_zeile)
    z5 = [int(x) for x in cl_k5c[5]]
    kern_e = set(E.f1_kern_eintraege())
    f1k = [k for k, v in plan.items() if v[0] == 'f1']
    gel = nutz(f1k)
    neu = list(z5)
    um = [i for i in range(256) if gel[i] > 0 and i not in kern_e and z5[i] != 0]
    n_haut = 0
    for i in um:
        if z5[i] == cl_g4[2][i]:
            neu[i] = zeilen[('haut', 2)][i]
            n_haut += 1
        else:
            neu[i] = TX.zeile_transformieren([z5[i]] + [0] * 255, f, k_haut, h_haut, kn, rp)[0]
    for s in sorted(set(plan[k][1] for k in f1k)):
        zeilen[('f1', s)] = neu
    ber['f1'] = dict(kern_eintraege=len(kern_e), umgefaerbt=len(um), davon_haut_zeile=n_haut)
    # ---- Maul innen (Zone 30): G4-Mundfleisch. Seite 0 liest die G4-Zeile 0 bytegleich; Flaechen anderer Seiten: Rang -> Mundfleisch-Rampe
    # (Eintraege der K5c-Maulinneres-Flaechen auf G4-Zeile 0, texelgewichtet). Zaehne (Zone 41): K5c-Zeile 7 (G4-Zahnmaterial), bytegleich.
    mj_ = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    mund = [TX.schluessel(s_) for s_ in mj_['Maulinneres']['liste']]
    nm0 = np.zeros(256)
    for k in mund:
        np.add.at(nm0, X['F'][k][2], 1)
    msel = np.nonzero((nm0 > 0) & (np.array(cl_g4[0]) != 0))[0]
    mo_ = msel[np.argsort(lab_g4[0][msel, 0], kind='stable')]
    maul_info = {}
    for s in sorted(set(plan[k][1] for k, v in plan.items() if v[0] == 'maul')):
        if s == 0:
            zeilen[('maul', 0)] = list(cl_g4[0])
        else:
            keys = gruppe('maul', s)
            n = nutz(keys)
            ein = np.nonzero((n > 0) & (np.array(cl_g4[s]) != 0))[0]
            ziel = rang_auf_rampe(lab_g4[s][ein, 0], n[ein], mo_, nm0[mo_])
            z = list(zeilen[('haut', s)])
            for i, j in zip(ein, ziel):
                z[i] = cl_g4[0][j]
            zeilen[('maul', s)] = z
        maul_info[s] = len(gruppe('maul', s))
    ber['maul'] = maul_info
    for s in sorted(set(plan[k][1] for k, v in plan.items() if v[0] == 'zahn')):
        if s != 0:
            raise SystemExit('Zahnflaeche nicht auf G4-Seite 0')
        zeilen[('zahn', 0)] = [int(x) for x in cl_k5c[7]]

    # ---- Fleck-Zonen
    je, konflikte = TX.fleck_zuordnung(Z, form, shape)
    M = E.f1_masken()
    kern = M['kern']
    zon = {z['nr']: z for z in Z['zonen']}
    fl_info = dict(konflikte=konflikte)
    # F1-Kern hat Vorrang (V6): Fleck-Texel auf F1-Flaechen im Kern bleiben F1
    weg_kern = 0
    for k in list(je):
        if plan[k][0] == 'f1':
            for xy in [xy for xy in je[k] if kern[xy[1], xy[0]]]:
                del je[k][xy]
                weg_kern += 1
        if not je[k]:
            del je[k]
    fl_info['f1_kern_vorrang_texel'] = weg_kern

    def basis_L(k, x, y):
        art, s = plan[k]
        i = int(px0[y, x])
        if art == 'f1':
            return float(TX.lab_zeile([z5[i]])[0, 0])
        return float(lab_g4[s][i, 0])
    # Materialrampen: (Liste Lab-Ziel je Rampeneintrag, Gewichte) je (Material, Seite)
    mat_f = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    k5c_gelenk = [TX.schluessel(s_) for s_ in mat_f['Gelenk']['liste']]
    k5c_buckel = [TX.schluessel(s_) for s_ in mat_f['Fleisch_Buckel']['liste']]
    k5c_auge = [TX.schluessel(s_) for s_ in mat_f['Auge']['liste']]
    ms = E.material_seite(X['F'], X['mat'], X['quell'])

    def nutz_k5c(keys, s):
        n = np.zeros(256)
        for k in keys:
            if ms[k][1] == s:
                np.add.at(n, X['F'][k][2], 1)
        return n
    # Fleisch (Zone 27, Wunde): Quelle = G4-Eintraege der K5c-Buckelflaechen (Seite 1); EINE Transformation, f und k so, dass der
    # texelgewichtete Median der Quelle auf die Mitte des Eichungsbandes faellt (L* 33-39 -> 36, C* 22-26 -> 24; GEGENPRUEFUNG_EICHUNG (b)).
    nb = nutz_k5c(k5c_buckel, 1)
    # Runde 2 (Pruefer r1 Sicht 2: dunkelgraue Inseln 462/318/147 px im Buckel, Wunde rosa-violett): gemessen sind 57 % (texelgewichtet) der
    # Quelleintraege unbunte/dunkle G4-Hauteintraege (z. B. 0x0c63 L* 8,5 C* 0, 648 Texel), die der Rang-Tausch als graue Flecken in den Buckel
    # traegt. Fleisch = nur die roten G4-Eintraege der Buckelquelle (Rot-Gewicht w > 0, kf_g4.r4_eintraege: Ader-/Fleischklasse).
    _Lr, _Cr, _hr, w_rot = KG.r4_eintraege(cl_g4[1])
    bsel = np.nonzero((nb > 0) & (np.array(cl_g4[1]) != 0) & (w_rot > 0))[0]
    Lb_med = TX.gew_quantil(lab_g4[1][bsel, 0], nb[bsel], 0.5)
    Cb_med = TX.gew_quantil(TX.buntheit(lab_g4[1][bsel]), nb[bsel], 0.5)
    lo, hi = 0.5, 6.0
    for _ in range(40):
        m = 0.5 * (lo + hi)
        if float(TX.knie(Lb_med * m, kn)) < float(tp['fleisch']['L_ziel']):
            lo = m
        else:
            hi = m
    fp = dict(f=round(0.5 * (lo + hi), 4), k=round(float(tp['fleisch']['C_ziel']) / Cb_med, 4),
              h=round(circ_mittel(TX.ton(lab_g4[1][bsel]), nb[bsel]), 2))
    ber['fleisch'] = dict(fp, quelle_L_median=round(Lb_med, 2), quelle_C_median=round(Cb_med, 2), quelle_eintraege=int(len(bsel)))
    rampen = {}

    def rampe(mat, s):
        key = (mat, s)
        if key in rampen:
            return rampen[key]
        L, haut, hell, ader = klassen[s]
        if mat == 'muster':
            n = nutz(gruppe('haut', s))
            sel = np.nonzero(ader & (n > 0))[0]
            w = n[sel]
            lab = TX.transform(lab_g4[s][sel], f, k_haut, h_haut, kn)
            worte = None
        elif mat == 'weiss':
            n = nutz_k5c(k5c_gelenk, s)
            sel0 = np.nonzero((n > 0) & (np.array(cl_g4[s]) != 0))[0]
            p75 = TX.gew_quantil(L[sel0, 0], n[sel0], float(tp['weiss']['quantil']))
            sel = sel0[L[sel0, 0] >= p75]
            w = n[sel]
            lab = TX.transform(lab_g4[s][sel], f, k_haut, h_haut, kn)
            worte = None
        elif mat == 'braun':
            n = nutz(gruppe('haut', s))
            hb = TX.ton(lab_g4[s]); cb_ = TX.buntheit(lab_g4[s])
            bw = tp['braun']
            sel = np.nonzero((n > 0) & (hb > bw['ton'][0]) & (hb < bw['ton'][1]) & (cb_ > bw['C_min']))[0]
            w = n[sel]
            hbr = circ_mittel(hb[sel], w)
            lab = TX.transform(lab_g4[s][sel], f, 1.0, hbr, kn, 'C')
            worte = None
        elif mat in ('fleisch', 'wunde'):
            n = nutz_k5c(k5c_buckel, 1)
            sel = np.asarray(bsel)
            w = n[sel]
            lab = TX.transform(lab_g4[1][sel], float(fp['f']), float(fp['k']), float(fp['h']), kn, 'C')
            worte = None
            L = klassen[1][0]
        elif mat == 'rotfleisch':
            pxk = px0[kern]
            n = np.bincount(pxk.ravel(), minlength=256).astype(float)
            sel = np.nonzero(n > 0)[0]
            w = n[sel]
            lab = TX.lab_zeile([z5[i] for i in sel])
            worte = [z5[i] for i in sel]
            L = TX.lab_zeile(z5)
        elif mat == 'auge':
            # Runde 2 (Pruefer r1 Sicht 2: Augen fehlen, Augenflaechen L* 2-13): nur die hellen G4-Augeneintraege (L* >= texelgewichtetes
            # Quantil par textur.auge.quantil der Eintraege der K5c-Augenflaechen auf G4-Zeile 0) und dieselbe globale Helligkeit f wie die Haut
            # (V1 multiplikativ fuer alle Klassen), Buntheit/Farbton der Quelle (k 1, Kreismittel).
            n = nutz_k5c(k5c_auge, 0)
            sel0 = np.nonzero((n > 0) & (np.array(cl_g4[0]) != 0))[0]
            L = klassen[0][0]
            qa = TX.gew_quantil(L[sel0, 0], n[sel0], float(tp['auge']['quantil']))
            sel = sel0[L[sel0, 0] >= qa]
            w = n[sel]
            ha = circ_mittel(TX.ton(lab_g4[0][sel]), w)
            lab = TX.transform(lab_g4[0][sel], f, 1.0, ha, kn, 'C')
            worte = None
            ber['auge'] = dict(quantil=float(tp['auge']['quantil']), L_grenze=round(qa, 2), eintraege=[int(i) for i in sel],
                               L_quelle=[round(float(x), 1) for x in L[sel, 0]], ton=round(ha, 1))
        else:
            raise SystemExit(mat)
        Lsrc = (L[sel, 0] if L.ndim == 2 else L[sel])
        o = np.argsort(Lsrc, kind='stable')
        rampen[key] = dict(eintraege=sel[o], w=w[o], lab=lab[o], worte=None if worte is None else [worte[i] for i in o],
                           h=None)
        return rampen[key]

    # je Zone: Rang der Texel (texelgewichtet ueber alle Traeger), -> Rampeneintrag
    zuweisung = {}     # (flaeche, (x, y)) -> (mat, s, j)
    je_zone = {}
    for k, d in je.items():
        for xy, nr in d.items():
            je_zone.setdefault(nr, []).append((k, xy))
    zone_info = {}
    for nr, lst in sorted(je_zone.items(), key=lambda t: float(t[0])):
        mat = TX.FLECK_ART[nr]
        Lb = np.array([basis_L(k, xy[0], xy[1]) for k, xy in lst])
        o = np.argsort(Lb, kind='stable')
        q = np.empty(len(Lb))
        q[o] = (np.arange(len(Lb)) + 0.5) / len(Lb)
        for (k, xy), qq in zip(lst, q):
            s = plan[k][1] if mat in ('muster', 'weiss', 'braun') else (1 if mat in ('fleisch', 'wunde') else 0)
            r = rampe(mat, s)
            rc = np.cumsum(r['w']); rq = (rc - 0.5 * r['w']) / rc[-1]
            j = int(np.clip(np.searchsorted(rq, qq), 0, len(rq) - 1))
            zuweisung[(k, xy)] = (mat, s, j)
        zone_info[nr] = dict(material=mat, texel_flaeche_paare=len(lst), traeger=len(set(k for k, _ in lst)),
                             zone_texel=len(zon[nr]['texel']['liste']))
    fl_info['zonen'] = zone_info

    # ---- Fleisch-Band (Eichung b): Median L*/C* ueber Zone-27-Texel muss im Band liegen (Pruefung, Riegel misst nach)
    # ---- Kopien + Indizes
    belegt = np.zeros(shape, bool)
    for k in flaechen:
        belegt[idx_alt[k]] = True
    px = px0.copy()
    gruppen = {}
    for k in sorted(je):
        gruppen.setdefault(plan[k], []).append(k)
    fleck_zeilen = []     # (basis-key, worte, flaechen)
    face_zeile = {}
    kopie_info = []
    for gkey in sorted(gruppen, key=lambda g: (g[0], g[1])):
        faces = gruppen[gkey]
        offen = list(faces)
        while offen:
            # Flaechen in diese Zeile aufnehmen, solange freie Indizes reichen
            genutzt = set()
            bedarf = []
            zeile_faces = []
            for k in list(offen):
                m = cv2.dilate(TX.flaeche_maske(flaechen[k], shape).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
                ys, xs = np.nonzero(m)
                nicht = set(int(px0[y, x]) for y, x in zip(ys, xs) if (int(x), int(y)) not in je[k])
                b = set(zuweisung[(k, xy)] for xy in je[k])
                neu_gen = genutzt | nicht
                neu_bed = set(bedarf) | b
                frei = [i for i in range(1, 256) if i not in neu_gen]
                if len(neu_bed) > len(frei) and zeile_faces:
                    continue
                genutzt, bedarf = neu_gen, sorted(neu_bed)
                zeile_faces.append(k)
                offen.remove(k)
            frei = [i for i in range(1, 256) if i not in genutzt]
            if len(bedarf) > len(frei):
                raise SystemExit('Fleck-Zeile %s: %d Materialeintraege, nur %d freie Indizes' % (gkey, len(bedarf), len(frei)))
            remap = {b: frei[i] for i, b in enumerate(bedarf)}
            basis = zeilen[gkey]
            worte = list(basis)
            for (mat, s, j), r in remap.items():
                ra = rampen[(mat, s)]
                if ra['worte'] is not None:
                    w = int(ra['worte'][j])
                else:
                    lab = ra['lab'][j:j + 1]
                    hh_ = float(TX.ton(lab)[0]) if TX.buntheit(lab)[0] > 1e-6 else h_haut
                    w = TX.runde15(lab, [float(lab[0, 0])], hh_, rp['fenster'], rp['cmin'])[0] & 0x7FFF
                    w = w or 0x8000
                worte[r] = w
            zi = len(fleck_zeilen)
            fleck_zeilen.append((gkey, worte, zeile_faces, len(remap)))
            # Kopien je Flaeche
            for k in zeile_faces:
                f = flaechen[k]
                m = cv2.dilate(TX.flaeche_maske(f, shape).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
                ys, xs = np.nonzero(m)
                y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
                block = m[y0:y1, x0:x1]
                lage = TX.platzieren(belegt, block)
                if lage is None:
                    raise SystemExit('kein freier Texturplatz fuer %s' % (k,))
                nx, ny = lage
                dx, dy = nx - x0, ny - y0
                for y, x in zip(ys, xs):
                    xy = (int(x), int(y))
                    i = int(px0[y, x])
                    if xy in je[k]:
                        i = remap[zuweisung[(k, xy)]]
                    px[y + dy, x + dx] = i
                belegt[ys + dy, xs + dx] = True
                alt_page = int(f['page'])
                po = (alt_page & 0xF) * 128
                np_ = nx // 128
                f['uv'] = [[int(u + po + dx - np_ * 128), int(v + dy)] for u, v in f['uv']]
                f['page'] = (alt_page & ~0xF) | np_
                if max(u for u, _v in f['uv']) > 255 or min(u for u, _v in f['uv']) < 0 or max(v for _u, v in f['uv']) > 255:
                    raise SystemExit('UV ausserhalb nach Kopie: %s' % (k,))
                face_zeile[k] = ('fleck', zi)
                kopie_info.append(dict(flaeche='%d|%d|%d' % k, von=[int(x0), int(y0)], nach=[int(nx), int(ny)], groesse=[int(x1 - x0), int(y1 - y0)],
                                       seite_alt=alt_page & 0xF, seite_neu=int(np_), fleck_texel=len(je[k])))
    fl_info['kopien'] = len(kopie_info)
    fl_info['fleck_zeilen'] = [dict(basis='%s%d' % g, flaechen=len(fs), material_eintraege=n) for g, _w, fs, n in fleck_zeilen]

    # ---- Zeilenplan
    reihen = []
    zeile_von = {}
    for key in sorted(zeilen, key=lambda t: (['maul', 'zahn', 'f1', 'haut', 'tausch', 'kralle'].index(t[0]), t[1])):
        zeile_von[key] = len(reihen)
        reihen.append(zeilen[key])
    for g, w, _fs, _n in fleck_zeilen:
        reihen.append(w)
    nz = len(reihen)
    if nz > 16:
        raise SystemExit('%d Zeilen > 16' % nz)
    for k, f in flaechen.items():
        if k in face_zeile:
            z = len(zeilen) + face_zeile[k][1]
        else:
            z = zeile_von[plan[k]]
        f['clut'] = ((((T['clut_y'] + z) & 0x1FF) << 6) | (int(f['clut']) & 0x3F))
    namen = [None] * nz
    for key, i in zeile_von.items():
        namen[i] = '%s%d' % key
    for j, (g, _w, fs, n) in enumerate(fleck_zeilen):
        namen[len(zeilen) + j] = 'fleck_%s%d_%d' % (g[0], g[1], j)
    ber['zeilen'] = namen
    ber['flecken'] = fl_info
    ber['kopien'] = kopie_info
    ber['material_zaehlung'] = {a: sum(1 for v in plan.values() if v[0] == a) for a in sorted(set(v[0] for v in plan.values()))}
    ber['texel_neu'] = int((px != px0).sum())
    return px, reihen, ber, plan, face_zeile, zuweisung, je


# ================================================================== E1 Weg (b'): Unterteilungs-Plan (Runde 2)
def geo_sha(form):
    return sha(json.dumps([[m['ecken'], [d['e'] for d in m['dreiecke']], [q['e'] for q in m['vierecke']]] for m in form['meshes']]).encode())


def unterteilung_plan(par, aus_json, prozesse=6):
    """Runden: Keil-Pixel ueber alle Bilder aller Clips in allen Ansichten (kf_k5g2_geo.keil_paare_alle, beide Partnerflaechen je Keil-Pixel),
    alle beteiligten Flaechen von 10/21/22/23 unterteilen; bis par geometrie.unterteilung.runden oder bis keine Abnahme. Schreibt den Plan
    (Auswahl je Runde, Summen) mit der Pruefsumme der Eingangsgeometrie; bauen() spielt ihn nach (deterministisch)."""
    Z = TX.lade_zonen()
    emd_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    form, _b = geometrie(par, Z, emd_a, tim_a)
    g0 = geo_sha(form)
    tmp = os.path.join(os.path.dirname(aus_json), '_ut_tmp')
    os.makedirs(tmp, exist_ok=True)
    tim_p = os.path.join(K5E, 'BAHNSTEIG_FORM.TIM')
    runden, summen, hk = [], [], None
    nmax = int(par['geometrie']['unterteilung']['runden'])
    for r in range(nmax + 1):
        p = os.path.join(tmp, 'r%d.EMD' % r)
        open(p, 'wb').write(G.emd_mit_md1(emd_a, form))
        res = G.keil_paare_alle(p, tim_p, prozesse=prozesse)
        su = int(sum(e['kopf_ueber_rumpf'] + e['rumpf_ueber_kopf'] for e in res))
        ntri = {str(b): len(form['meshes'][b]['dreiecke']) + 2 * len(form['meshes'][b]['vierecke']) for b in (10, 21, 22, 23)}
        summen.append(dict(runde=r, keil_px=su, dreiecke=ntri))
        log('Unterteilung Runde %d: Keil-Pixel %d, Dreiecke %s' % (r, su, ntri))
        if r == nmax or su == 0 or (len(summen) > 1 and su >= summen[-2]['keil_px']):
            break
        aus = {}
        for e in res:
            for b, v, fi in e['flaechen']:
                aus.setdefault(b, set()).add((v, fi))
        runden.append({str(b): sorted([list(x) for x in v]) for b, v in sorted(aus.items())})
        hk = U.unterteilen(form, {b: v for b, v in aus.items()}, hk)
    if len(summen) > 1 and summen[-1]['keil_px'] >= summen[-2]['keil_px']:
        runden = runden[:-1]
    json.dump(dict(eingang_geo_sha=g0, runden=runden, summen=summen), open(aus_json, 'w', encoding='utf-8'), indent=0)
    shutil.rmtree(tmp, ignore_errors=True)


def unterteilung_anwenden(form, plan_json):
    pj = json.load(open(plan_json, encoding='utf-8'))
    if pj['eingang_geo_sha'] != geo_sha(form):
        raise SystemExit('Unterteilungs-Plan passt nicht zur Geometrie (kf_k5g2.py --unterteilung neu laufen lassen)')
    hk = None
    for rd in pj['runden']:
        hk = U.unterteilen(form, {int(b): set(tuple(x) for x in v) for b, v in rd.items()}, hk)
    return hk or {}, pj['summen']


# ================================================================== Bau
def bauen(par, aus, geo=None):
    Z = TX.lade_zonen()
    emd_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    if emd_a != open(ASSET + '.EMD', 'rb').read() or tim_a != open(ASSET + '.TIM', 'rb').read():
        raise SystemExit('ruhe/K5e_f1 != Spiel-Asset (shared_assets/RE2)')
    emd_f = open(os.path.join(K5E, 'FORM.EMD'), 'rb').read()
    if FE.sektionen(emd_f)['md1'] != FE.sektionen(emd_a)['md1']:
        raise SystemExit('K5e_f1 FORM.EMD-MD1 != BAHNSTEIG_FORM.EMD-MD1')
    if geo is None:
        geo = geometrie(par, Z, emd_a, tim_a)
    form = json.loads(json.dumps(geo[0]))
    X = E.eingaenge()
    px, reihen, ber_t, plan, _fz, _zw, _je = textur(form, par, X, Z)
    T = X['T']
    tim = FE.tim_schreiben(px, reihen, T['clut_x'], T['clut_y'], T['bild_x'], T['bild_y'])
    os.makedirs(aus, exist_ok=True)
    # Pruef-Stand vor der Unterteilung (gleiche Textur, Flaechen-Schluessel wie zonen_v2/traeger_texel; fuer die Textur-Riegel)
    os.makedirs(os.path.join(aus, 'pruef'), exist_ok=True)
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.EMD'), 'wb').write(G.emd_mit_md1(emd_a, form))
    open(os.path.join(aus, 'pruef', 'BAHNSTEIG_FORM.TIM'), 'wb').write(tim)
    # Runde 2: Mulde (O1) statt des Unterteilungs-Plans der Runde 1 - Netze der Mulden-Tabelle, Textur-Zuordnung von der Mutterflaeche geerbt
    ut_summen = G.mulde_auf_form(form)
    hk = {(t, a_, i): (mt, ma, mi) for t, a_, i, mt, ma, mi in json.load(open(G.GEO_TABELLE, encoding='utf-8'))['herkunft']}
    emd_bahn = G.emd_mit_md1(emd_a, form)
    emd_form = G.emd_mit_md1(emd_f, form)
    for n, b in (('BAHNSTEIG_FORM.EMD', emd_bahn), ('BAHNSTEIG_FORM.TIM', tim), ('FORM.EMD', emd_form), ('FORM.TIM', tim)):
        open(os.path.join(aus, n), 'wb').write(b)
    fj = json.load(open(os.path.join(K5E, 'form.json'), encoding='utf-8'))
    fj['name'] = par.get('name', 'K5g2')
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    bericht = dict(variante=par.get('name', 'K5g2'), generator='generator/kf_k5g2.py', par=par, geometrie=geo[1], textur=ber_t,
                   unterteilung=dict(plan='generator/k5g2_mulde_tabelle.json', summen=ut_summen,
                                     herkunft={'%d|%d|%d' % k: '%d|%d|%d' % v for k, v in sorted(hk.items()) if k != v}),
                   zeilen=len(reihen), port_texturhoehe=len(reihen) * 256,
                   sha256={n: sha(open(os.path.join(aus, n), 'rb').read()) for n in ('FORM.EMD', 'FORM.TIM', 'BAHNSTEIG_FORM.EMD', 'BAHNSTEIG_FORM.TIM')},
                   eingang_sha256=dict(k5e_f1_bahnsteig_emd=sha(emd_a), k5e_f1_tim=sha(tim_a), k5e_f1_form_emd=sha(emd_f),
                                       basis_k5c_emd=sha(open(os.path.join(BASIS, 'FORM.EMD'), 'rb').read()),
                                       zonen_v2=sha(open(TX.ZONEN, 'rb').read()), material_json=sha(open(E.MATERIAL, 'rb').read())))
    json.dump(bericht, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    return bericht, geo


def f_bisektion(pp):
    """haut.f: kleinster Faktor mit Bild 2 Video (3 Saaten, Pixelmenge V4F 3.1 = Nutzer-Teilmasken & K5c-Modell & ohne Weisskappe) >= Vorlage
    (W6 wie kf_k5f.f_bisektion; EIN globaler Helligkeitswert). Intervall [0,6; 3,0], 10 Schritte, aufgerundet auf 0,001."""
    sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
    import rl
    import riegel_k5d as RK
    par = json.load(open(pp, encoding='utf-8'))
    tmp = os.path.join(rl.SCR, '..', 'k5g2', 'f_bau')
    vor = RK.Vorlage()
    _s, Kc = rl.v4f.szene_id(E.BASIS, 2)
    fig = RK.nutzer_union(2) & Kc['maske'] & ~vor.d[2]['clip']
    ziel = float(vor.d[2]['LV'][fig][:, 0].mean())
    Z = TX.lade_zonen()
    emd_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim_a = open(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    geo = geometrie(par, Z, emd_a, tim_a)

    def hell(fv):
        p = json.loads(json.dumps(par))
        p['textur']['haut']['f'] = fv
        if os.path.isdir(tmp):
            shutil.rmtree(tmp)
        bauen(p, tmp, geo)
        rl.v4f._KC.pop(tmp, None)
        s_, _K = rl.v4f.szene_id(tmp, 2)
        return float(np.mean([RK.labm(rl.v4f.video(s_, 2, sa))[fig][:, 0].mean() for sa in rl.v4f.SAATEN]))
    lo, hi = 0.6, 3.0
    proto = []
    for _ in range(10):
        fv = round(0.5 * (lo + hi), 5)
        v = hell(fv)
        proto.append([fv, round(v, 3)])
        log('  f %.5f -> Bild 2 Video %.3f (Vorlage %.3f)' % (fv, v, ziel))
        if v >= ziel:
            hi = fv
        else:
            lo = fv
    fv = math.ceil(hi * 1000 - 1e-9) / 1000.0
    v = hell(fv)
    shutil.rmtree(tmp, ignore_errors=True)
    par['textur']['haut']['f'] = fv
    par['textur']['haut']['f_quelle'] = ('kf_k5g2.py --f: kleinster Faktor (Bisektion [0,6; 3,0], 10 Schritte, aufgerundet auf 0,001) mit Bild 2 Video '
                                         '(3 Saaten, Pixelmenge V4F 3.1) >= Vorlage %.2f (W6 wie K5f, EIN globaler Helligkeitswert); Ergebnis %.3f. '
                                         'Protokoll [f, Bild 2]: %s' % (ziel, v, proto))
    json.dump(par, open(pp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    log('fertig f', fv, 'Bild 2 Video %.3f' % v)


if __name__ == '__main__':
    if sys.argv[1] == '--f':
        f_bisektion(sys.argv[2])
        sys.exit(0)
    if sys.argv[1] == '--unterteilung':
        unterteilung_plan(json.load(open(sys.argv[2], encoding='utf-8')), UT_PLAN)
        sys.exit(0)
    par = json.load(open(sys.argv[1], encoding='utf-8'))
    b, _g = bauen(par, sys.argv[2])
    print(json.dumps({k: b[k] for k in ('zeilen', 'port_texturhoehe', 'sha256')}, indent=1))
    print(json.dumps({k: b['textur'][k] for k in ('zeilen', 'haut', 'kralle', 'f1', 'tausch', 'material_zaehlung', 'texel_neu')}, indent=1, ensure_ascii=False))
