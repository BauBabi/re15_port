#!/usr/bin/env python3
"""K5g3 Textur-Stufe (Auftrag K5g3 Textur-Bauer Runde 1, 2026-11-14): E3 Grundlage v2 Zone fuer Zone, V1-V6, O3-O5 auf der Geometrie K5g3.

Herkunft: Fortschreibung von kf_k5g2.textur (K5g2 Runde 2, angehalten; Befunde PRUEFUNG_K5G2_r1_sicht/_zahlen eingearbeitet). Geaendert
gegen K5g2 (jede Aenderung mit Grund im Kommentar an der Stelle):
 - Krallen 19/20/29/31 tragen in K5g3 ihre EIGENEN K5e_f1-Flaechen (G2: Flaechen/UV/CLUT/Seite wie K5e_f1) - die G4-Seite kommt von der
   eigenen Flaeche, nicht von der linken Partnerkralle (K5g2 hatte gespiegelte Flaechen).
 - Haut: C' = s x L' mit s aus dem Zielband der Original-Gegner im Spiel (O3, Bisektion kf_k5g3.py --s), nicht aus der Video-Eichung C* 2-6.
 - Haut je G4-Seite multiplikativ auf den gemeinsamen Median gebracht (O3 "kein fast schwarzer, unbunter Sattel"): f_s = f x g_s,
   g_s = texelgewichteter Median L* der Hautklasse auf Seite 1 / auf Seite s (V1: multiplikativ, Rang und Kontrastverhaeltnis bleiben).
 - Auge: eigenes f_auge (V1 "je Material EINE Transformation"; O4 "im Spiellicht sichtbar heller als die Haut daneben").
Die Geometrie bleibt unberuehrt: die Stufe malt auf den K5e_f1-Flaechen (Schluessel wie zonen_v2/traeger_texel); danach setzt kf_k5g3.py
die Spalt-Netze B4 ein (kf_k5g2_geo.mulde_auf_form, Kinder erben CLUT/Seite/UV-Verschiebung der Mutter; G4).
"""
import json
import math
import os
import sys

import cv2
import numpy as np

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_g4 as KG  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402

FE = E.FE
K5 = E.K5
KRALLEN_TEILE = (14, 15, 19, 20, 28, 29, 30, 31)
HAENDE = (13, 18)


def materialplan(form, X, Z):
    """{flaeche: (art, g4seite)} - wie kf_k5g2_tex.materialplan, aber ohne Partner-Abbildung der rechten Krallen (K5g3: eigene Flaechen)."""
    F, mat, quell = X['F'], X['mat'], X['quell']
    ms = E.material_seite(F, mat, quell)
    zon = {z['nr']: z for z in Z['zonen']}

    def liste(nr):
        return set(TX.schluessel(k) for k in zon[nr]['modell_zuordnung']['liste'])
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    lippen = set(TX.schluessel(k) for nm in ('Lippensaum', 'Maul_Band') for k in mj[nm]['liste'])
    maul, zahn = liste('30'), liste('41')
    zehen = liste('21') | liste('22')
    tausch = liste('31') | liste('40') | liste('42') | liste('45') | lippen | set(k for k, v in mat.items() if v in ('Auge', 'Maulinneres'))
    plan = {}
    for k, _f in TX.alle_flaechen(form):
        mi = k[0]
        seite = ms[k][1]
        if k in maul:
            art = 'maul'
        elif k in zahn:
            art = 'zahn'
        elif mat.get(k) == 'F1':
            art = 'f1'
        elif mi in KRALLEN_TEILE or k in zehen:
            art = 'kralle'
        elif mi in HAENDE:
            art = 'hand'
        elif k in tausch:
            art = 'tausch'
        else:
            art = 'haut'
        plan[k] = (art, seite)
    return plan


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
    plan = materialplan(form, X, Z)
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
    f0, kn = float(tp['haut']['f']), float(tp['haut']['knie'])
    # K5g3 (O3 "kein fast schwarzer, unbunter Sattel", Pruefer K5g2 r1 Sicht 1: Teile 0/7 lasen haut0 mit L* 11 / C* 0,1 gegen haut1 L* 24-31):
    # die K5c-UV legt Huefte/Ruecken (Teile 0, 7) auf G4-Seite 0, deren Hauttexel dort dunkler sind. Je G4-Seite ein Faktor g_s =
    # texelgewichteter Median L* der Hautklasse (Hautflaechen dieser Seite) auf Seite 1 / auf Seite s; Helligkeit f_s = f x g_s.
    # Multiplikativ (V1): Rangfolge und Kontrastverhaeltnis der Rampe je Seite bleiben; Seite 1 (die meisten Hautflaechen) traegt f selbst.
    med_s = {}
    for s in range(3):
        n = nutz(gruppe('haut', s))
        sel = klassen[s][1] & (n > 0)
        med_s[s] = TX.gew_quantil(klassen[s][0][sel, 0], n[sel], 0.5) if sel.any() else None
    g_s = {s: (round(med_s[1] / med_s[s], 4) if med_s[s] else 1.0) for s in range(3)} if tp['haut'].get('seiten_angleichen', True) else {0: 1.0, 1: 1.0, 2: 1.0}
    fs = {s: f0 * g_s[s] for s in range(3)}
    f = f0
    # s (Buntheit je Helligkeit, modus 'L'): C' = s x L'. K5g3 (O3): s aus par (kleinste Buntheit mit Zielband Z4/Z8 im Spiel, kf_k5g3.py --s);
    # die Video-Eichung C* 2-6 ist keine Grenze mehr.
    Lsk = np.concatenate([TX.knie(lab_g4[s_][klassen[s_][1] & (nutz(gruppe('haut', s_)) > 0), 0] * fs[s_], kn) for s_ in range(3)])
    l_med = TX.gew_quantil(Lsk, hw, 0.5)
    k_haut = float(tp['haut']['s'])
    rp = tp['rundung']
    band = tp['haut']['ton_band']
    h_haut = float(tp['haut']['h'])
    if not band[0] <= h_haut <= band[1]:
        raise SystemExit('Hautton %.1f ausserhalb des Bandes der Originale %s' % (h_haut, band))
    ber['haut'] = dict(h=h_haut, h_g4_hautklasse_info=h_g4, C_median_g4_hautklasse_info=round(c_med, 3), L_median_hautklasse=round(l_med, 3),
                       s=k_haut, f=f0, knie=kn, seiten_median_L_g4={str(s): (round(v, 3) if v else None) for s, v in med_s.items()},
                       seiten_faktor={str(s): v for s, v in g_s.items()}, f_je_seite={str(s): round(v, 4) for s, v in fs.items()})
    zeilen = {}
    for s in range(3):
        zeilen[('haut', s)] = TX.zeile_transformieren(cl_g4[s], fs[s], k_haut, h_haut, kn, rp)

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
    kral = [k for k in flaechen if k[0] in KRALLEN_TEILE]

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
            neu[i] = TX.zeile_transformieren([z5[i]] + [0] * 255, fs[2], k_haut, h_haut, kn, rp)[0]
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
    # K5g3 (Riegel V3.3 am K5g2-Weg: Median C* 18,5 statt 22-26): k = C_ziel / Median kappt die weiche sRGB-Grenze (kf_g4.r4_cmax, tanh) wieder
    # herunter. k darum so, dass der texelgewichtete Median C* NACH der Abbildung (weiche Grenze, 15-Bit-Rundung) = C_ziel ist (Bisektion).
    # Gemessen am ersten K5g3-Bau: auch danach Median C* der Zone-27-Texel nur 20,2 - die 15-Bit-Wahl (TX.runde15, wie fuer die Fleck-Zeilen
    # unten) senkt C* der dunklen Rottoene. Die Bisektion misst darum die GERUNDETEN Worte (dieselbe Rundung wie die Fleck-Zeilen).
    def _c_gerundet(kk):
        lab_ = TX.transform(lab_g4[1][bsel], fp['f'], kk, fp['h'], kn, 'C')
        ws = []
        for j in range(len(lab_)):
            l1 = lab_[j:j + 1]
            hh1 = float(TX.ton(l1)[0]) if TX.buntheit(l1)[0] > 1e-6 else h_haut
            ws.append((TX.runde15(l1, [float(l1[0, 0])], hh1, rp['fenster'], rp['cmin'])[0] & 0x7FFF) or 0x8000)
        return TX.buntheit(TX.lab_zeile(ws))
    lo, hi = 0.1, 4.0
    for _ in range(30):
        m = 0.5 * (lo + hi)
        Cm = TX.gew_quantil(_c_gerundet(m), nb[bsel], 0.5)
        if Cm < float(tp['fleisch']['C_ziel']):
            lo = m
        else:
            hi = m
    fp['k_einfach_info'] = fp['k']
    fp['k'] = round(0.5 * (lo + hi), 4)
    if tp['fleisch'].get('k') is not None:      # Messwert aus kf_k5g3.py --fleisch (Median der Zone-27-Texelzugriffe im Bau, wie Riegel V3.3)
        fp['k_quellmedian_info'] = fp['k']
        fp['k'] = float(tp['fleisch']['k'])
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
            lab = TX.transform(lab_g4[s][sel], fs[s], k_haut, h_haut, kn)
            worte = None
        elif mat == 'weiss':
            n = nutz_k5c(k5c_gelenk, s)
            sel0 = np.nonzero((n > 0) & (np.array(cl_g4[s]) != 0))[0]
            p75 = TX.gew_quantil(L[sel0, 0], n[sel0], float(tp['weiss']['quantil']))
            sel = sel0[L[sel0, 0] >= p75]
            w = n[sel]
            lab = TX.transform(lab_g4[s][sel], fs[s], k_haut, h_haut, kn)
            worte = None
        elif mat == 'braun':
            n = nutz(gruppe('haut', s))
            hb = TX.ton(lab_g4[s]); cb_ = TX.buntheit(lab_g4[s])
            bw = tp['braun']
            sel = np.nonzero((n > 0) & (hb > bw['ton'][0]) & (hb < bw['ton'][1]) & (cb_ > bw['C_min']))[0]
            w = n[sel]
            hbr = circ_mittel(hb[sel], w)
            lab = TX.transform(lab_g4[s][sel], fs[s], 1.0, hbr, kn, 'C')
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
            # Quantil par textur.auge.quantil der Eintraege der K5c-Augenflaechen auf G4-Zeile 0), Buntheit/Farbton der Quelle (k 1, Kreismittel).
            # K5g3 (O4 "im Spiellicht sichtbar heller als die Haut daneben"; V1 "je Material EINE Transformation", multiplikativ): eigenes
            # f_auge = par textur.auge.f (kf_k5g3.py --auge). Mit dem f der Haut lagen die Augen (G4-Eintraege L* 16-38) unter der Haut.
            n = nutz_k5c(k5c_auge, 0)
            sel0 = np.nonzero((n > 0) & (np.array(cl_g4[0]) != 0))[0]
            L = klassen[0][0]
            qa = TX.gew_quantil(L[sel0, 0], n[sel0], float(tp['auge']['quantil']))
            sel = sel0[L[sel0, 0] >= qa]
            w = n[sel]
            ha = circ_mittel(TX.ton(lab_g4[0][sel]), w)
            lab = TX.transform(lab_g4[0][sel], float(tp['auge']['f']), 1.0, ha, kn, 'C')
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
    # K5g3 (V4 "keine Flaeche liest 0x8000 ausser Zone 30"; gemessen: G4-Eintrag 255 ist auf allen Seiten 0x8000 = deckendes Schwarz, die
    # multiplikative Abbildung laesst ihn schwarz; 829 Texelzugriffe, vor allem Huefte Teil 0): in jeder Zeile ausser den Maul-Zeilen und den
    # F1-Kern-Eintraegen (bytegleich K5c) wird ein deckend schwarzer Eintrag auf den dunkelsten nicht schwarzen deckenden Eintrag derselben
    # Zeile gesetzt (Untergrenze; die Rangfolge bleibt, Schwarz und Dunkelstes fallen zusammen).
    namen_tmp = [None] * len(reihen)
    for key, i in zeile_von.items():
        namen_tmp[i] = key[0]
    for j, (g, _w, _fs, _n) in enumerate(fleck_zeilen):
        namen_tmp[len(zeilen) + j] = g[0]
    boden = {}
    for i, z in enumerate(reihen):
        if namen_tmp[i] == 'maul':
            continue
        lab_z = TX.lab_zeile(z)
        kand = [j for j in range(256) if z[j] not in (0, 0x8000) and (z[j] & 0x7FFF) != 0]
        if not kand:
            continue
        d = min(kand, key=lambda j: (float(lab_z[j, 0]), j))
        n = 0
        for j in range(256):
            if (z[j] == 0x8000 or (z[j] != 0 and (z[j] & 0x7FFF) == 0)) and not (namen_tmp[i] == 'f1' and j in kern_e):
                z[j] = z[d]
                n += 1
        if n:
            boden[str(i)] = dict(eintraege=n, ersatz=int(z[d]), L=round(float(lab_z[d, 0]), 2))
    ber['schwarz_boden'] = boden
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
