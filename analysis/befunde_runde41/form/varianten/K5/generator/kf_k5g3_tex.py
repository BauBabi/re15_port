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
Runde 2 (Befunde PRUEFUNG_K5G3_r1_zahlen/_sicht, je Stelle im Kommentar): Kopie-Maske lese_maske (ROT 2 weisser Strich); weisse Musterung fast
unbunt (ROT 1); braeunliche Musterung nicht heller als die Haut (Gelb 2); keine reinweissen Worte (nie_weiss, Gelb 4); Loch-Flaechen 22|0|28-35
als Haut (Sicht 5, schwarzer Fleck); Zone 29 ueber eigene Augen-Texel (kf_k5g3_auge, Sicht 5 Augen verschwunden).
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
FLECK_ART = dict(TX.FLECK_ART, L='hautrampe')     # 'L' = Loch-Flaechen (Runde 2), Material Haut
# Runde 3 (G7/V4): diese Zonen der Grundlage v2 tragen nie rotes Fleck-Material (V4 "Lippe, Unterkiefer aussen, Hals-Unterseite und
# Kopf-Unterseite dunkelgrau wie die Haut, NIE schwarz und NIE rot")
V4_ZONEN = ('31', '42', '44', '45')
ROT_MATERIAL = ('fleisch', 'wunde', 'rotfleisch')


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


def lese_maske(f, shape, kinder=None):
    """Runde 2 (Zahlen-Pruefer K5g3 r1 ROT 2, GEOMETRIE.md 9.3): Texel, die eine Flaeche im Spiel liest. kf_k5g2_tex.flaeche_maske nimmt das
    Polygon (0,1,3,2); bei 73 Vierecken schneidet es sich selbst ("Fliege"), bei 48 liegen Render-Texel mehr als 1 Texel ausserhalb - die Kopie
    blieb dort leer (Index 0 = 0x7FFF, weisser Strich auf 10|1|76). Jetzt: konvexe Huelle aller UV-Ecken (deckt jede Zerlegung des Vierecks,
    (0,1,2)+(1,3,2) wie (0,1,3)+(0,3,2)) vereinigt mit den UV-Dreiecken der Spalt-Kinder dieser Mutter (uvfein: 49 Kinder lesen bis 8 Texel
    ausserhalb der Mutter-UV), dann 3x3 gedehnt (floor-Abtastung am Rand)."""
    po = (int(f['page']) & 0xF) * 128
    pts = np.array([[a + po, b] for a, b in f['uv']], np.int32)
    mk = np.zeros(shape, np.uint8)
    cv2.fillConvexPoly(mk, cv2.convexHull(pts), 1)
    cv2.polylines(mk, [cv2.convexHull(pts)], True, 1)
    for poly in (kinder or ()):
        q = np.array(poly, np.int32)
        cv2.fillPoly(mk, [q], 1)
        cv2.polylines(mk, [q], True, 1)
    return cv2.dilate(mk, np.ones((3, 3), np.uint8)).astype(bool)


def deckel(worte, L0, idx, grenze):
    """Runde 3 (Orchestrator G8, Zahlen-Pruefer K5g3 r2 Gelb 2: 0x7FFE/0x7BDF/0x77BF mit L* 95-99 auf der Haut, im Spiel 107 Pixel mit L* > 90 -
    "wie reinweiss zu behandeln"): kein Eintrag der Rampe liest L* > grenze. Die Eintraege `idx` werden nach ihrem Quell-L* (L0, G4-Rang)
    aufsteigend durchlaufen; ein Eintrag ueber der Grenze bekommt das Wort des naechsten Rangs der Rampe darunter, der unter der Grenze liegt
    (Rangfolge bleibt, nicht streng: die obersten Raenge fallen zusammen). Eintraege unter der Grenze bleiben bytegleich. STP-Bit bleibt.
    Aendert `worte` an Ort und Stelle; Rueckgabe Liste (Index, altes Wort, neues Wort)."""
    lab = TX.lab_zeile(worte)
    letzt = None
    aus = []
    for i in sorted(idx, key=lambda j: (float(L0[j]), j)):
        if float(lab[i, 0]) > grenze:
            if letzt is not None:
                neu = (int(worte[letzt]) & 0x7FFF) | (int(worte[i]) & 0x8000)
                aus.append((int(i), int(worte[i]), neu or 0x8000))
                worte[i] = neu or 0x8000
        else:
            letzt = i
    return aus


def loch_flaechen(plan, flaechen, idx_alt, px0, klassen, anteil):
    """Runde 2 (Sicht-Pruefer K5g3 r1 (5): schwarzer Fleck an der rechten Augenhoehle, Netz 22, Zeile haut1, Eintraege 213/204/255): Flaechen,
    deren gelesene Texel zu mindestens `anteil` unter dem texelgewichteten 1-%-Quantil der Hautklasse ihrer G4-Seite liegen. Gemessen: genau
    22|0|28-35 (der Augenwulst; G4-Seite 1, Eintraege 204 L* 1,7 / 213 L* 2,4 / 255 = 0x8000, ein schwarzes Detail der G4-Textur, das die
    K5c-UV ganz auf diese acht Flaechen legt). Mit der multiplikativen Abbildung bleibt es schwarz (K5e_f1 zeigte dort L* 22,9)."""
    aus = {}
    for s in range(3):
        keys = [k for k, v in plan.items() if v == ('haut', s)]
        n = np.zeros(256)
        for k in keys:
            np.add.at(n, px0[idx_alt[k]], 1)
        L, haut, _h, _a = klassen[s]
        sel = haut & (n > 0)
        if not sel.any():
            continue
        q1 = TX.gew_quantil(L[sel, 0], n[sel], 0.01)
        for k in keys:
            Lk = L[px0[idx_alt[k]], 0]
            if len(Lk) and float((Lk < q1).mean()) >= anteil:
                aus[k] = round(float(q1), 3)
    return aus


def textur(form, par, X, Z, kinder=None):
    """kinder: {Mutterflaeche: [UV-Dreiecke der Spalt-Kinder (global, inkl. 128 x Seite)]} aus geo/spalt_uv_tabelle.json (fuer lese_maske)."""
    kinder = kinder or {}
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
    deckel_info = {}
    ber['deckel_haut'] = deckel_info
    for s in range(3):
        # Runde 2 (Zahlen-Pruefer K5g3 r1 Gelb 4: 76 Zugriffe auf 0x7FFF auf Huefte/Ruecken, Zeile haut0 Index 221/231 nach Faktor 4,54):
        # reinweiss ist in keiner Hautzeile erlaubt (nie_weiss; der 15-Bit-Nachbar daneben wird genommen, die Rangfolge bleibt).
        zeilen[('haut', s)] = TX.zeile_transformieren(cl_g4[s], fs[s], k_haut, h_haut, kn, rp, nie_weiss=True)
        # Runde 3 (G8): fast weiss wie reinweiss - kein Hauteintrag mit L* > par haut.L_max; naechster Rang darunter (deckel). Die Tausch-Zeilen,
        # die Loch-Haut und die Grundworte der Fleck-Zeilen nehmen ihre Worte aus dieser Zeile und erben den Deckel.
        gd = deckel(zeilen[('haut', s)], lab_g4[s][:, 0], [i for i in range(256) if cl_g4[s][i] != 0], float(tp['haut']['L_max']))
        deckel_info[str(s)] = dict(eintraege=len(gd), worte=['%d:%04x->%04x' % t for t in gd])

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
            neu[i] = TX.zeile_transformieren([z5[i]] + [0] * 255, fs[2], k_haut, h_haut, kn, rp, nie_weiss=True)[0]
    # Runde 3 (G8): die umgefaerbten (Haut-)Eintraege der F1-Zeile ebenfalls ohne L* > L_max; die Kern-Eintraege bleiben bytegleich K5c (V6)
    gd = deckel(neu, TX.lab_zeile(z5)[:, 0], um, float(tp['haut']['L_max']))
    deckel_info['f1'] = dict(eintraege=len(gd), worte=['%d:%04x->%04x' % t for t in gd])
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
    # Runde 2: Zone 29 (Augen) laeuft nicht mehr ueber die Traegertexel, sondern ueber eigene Augen-Texel an den Nutzerpunkten (kf_k5g3_auge.dekal,
    # nach dem Spalt; Grund dort im Kopf: die Traeger 22|0|82/83 sind UV-entartet, in Bild 2 trafen nur 5 Pixel).
    for k in list(je):
        for xy in [xy for xy, nr in je[k].items() if nr == '29']:
            del je[k][xy]
        if not je[k]:
            del je[k]
    # Runde 3 (Orchestrator G7, Zahlen-Pruefer K5g3 r2 Gelb: Zone 44 Hals-Unterseite traegt auf 5 Flaechen Zone-27-Fleisch, 2722 Zugriffe): V4 geht
    # vor - Lippe (42), Unterkiefer aussen (31), Hals-Unterseite (44), Kopf-Unterseite (45) und der Lippensaum tragen kein rotes Fleck-Material
    # (fleisch, wunde, rotfleisch). Die Flaechen behalten ihre Haut; das Fleck-Material der Zone liegt weiter auf den Kopien der uebrigen
    # Traeger (kein Original-Texel wird umgefaerbt). Gemessen 2026-11-14: betroffen nur 21|0|0, 21|1|0, 21|1|8, 21|1|56, 21|1|64 (Zone 27).
    zon_ = {z['nr']: z for z in Z['zonen']}
    mj_v4 = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    v4_flaechen = set(TX.schluessel(s_) for nr in V4_ZONEN for s_ in zon_[nr]['modell_zuordnung']['liste']) | \
        set(TX.schluessel(s_) for nm in ('Lippensaum', 'Maul_Band') for s_ in mj_v4[nm]['liste'])
    rot_zonen = set(nr for nr, a in FLECK_ART.items() if a in ROT_MATERIAL)
    v4_weg = {}
    for k in sorted(je):
        if k not in v4_flaechen:
            continue
        weg = [xy for xy, nr in je[k].items() if nr in rot_zonen]
        for xy in weg:
            v4_weg.setdefault('%d|%d|%d' % k, {}).setdefault(je[k][xy], 0)
            v4_weg['%d|%d|%d' % k][je[k][xy]] += 1
            del je[k][xy]
        if not je[k]:
            del je[k]
    # Runde 2: Loch-Flaechen (loch_flaechen) als eigener Fleck "L" mit Material Haut (Rang-Tausch auf die Hautrampe der Seite, eigene Texel):
    # E3 "der Rest des heutigen Augenfelds wird Haut" - die Flaechen zeigen danach die Haut mit ihrer Helligkeitsstruktur statt eines Lochs.
    loch = loch_flaechen(plan, flaechen, idx_alt, px0, klassen, float(tp['loch']['anteil']))
    for k in sorted(loch):
        if k in je:
            raise SystemExit('Loch-Flaeche %s traegt schon einen Fleck' % (k,))
        ys_, xs_ = np.nonzero(lese_maske(flaechen[k], shape, kinder.get(k)))
        je[k] = {(int(x), int(y)): 'L' for y, x in zip(ys_, xs_)}
    fl_info_loch = {'%d|%d|%d' % k: v for k, v in sorted(loch.items())}
    M = E.f1_masken()
    kern = M['kern']
    zon = {z['nr']: z for z in Z['zonen']}
    fl_info = dict(konflikte=konflikte, loch_flaechen=fl_info_loch, loch_anteil=float(tp['loch']['anteil']),
                   v4_vorrang=dict(regel='V4 vor Fleck: Zonen %s + Lippensaum/Maul_Band ohne %s' % ('/'.join(V4_ZONEN), '/'.join(ROT_MATERIAL)),
                                   entfernt=v4_weg))
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
    braun_pos = {}      # Runde 3 (G6): (G4-Seite, Index) -> Rampeneintrag der braeunlichen Musterung

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
            # Runde 2 (Zahlen-Pruefer K5g3 r1 ROT 1: weisse Musterung C* 18-25 = so bunt wie das Fleisch; Ursache C' = s_haut x L'): "helle, fast
            # unbunte Gelenk-Eintraege" = die hellen G4-Gelenk-Eintraege (oberes Viertel, wie bisher), davon nur die fast unbunten (Quell-C* <=
            # par weiss.C_max), Buntheit der Quelle (modus 'C', k 1), Helligkeit multiplikativ wie die Haut derselben Seite (f_s).
            n = nutz_k5c(k5c_gelenk, s)
            sel0 = np.nonzero((n > 0) & (np.array(cl_g4[s]) != 0))[0]
            p75 = TX.gew_quantil(L[sel0, 0], n[sel0], float(tp['weiss']['quantil']))
            Cq = TX.buntheit(lab_g4[s])
            sel = sel0[(L[sel0, 0] >= p75) & (Cq[sel0] <= float(tp['weiss']['C_max']))]
            if not len(sel):
                raise SystemExit('weiss Seite %d: kein heller, fast unbunter Gelenk-Eintrag' % s)
            w = n[sel]
            hw_ = circ_mittel(TX.ton(lab_g4[s][sel]), w * np.maximum(Cq[sel], 1e-3))
            lab = TX.transform(lab_g4[s][sel], fs[s], 1.0, hw_, kn, 'C')
            worte = None
        elif mat == 'braun':
            # Runde 3 (Orchestrator G6, Zahlen-Pruefer K5g3 r2 ROT: Zonen 16/19 als flache Farbinsel mit 2 Werten, cv 0,06 gegen G4 0,42 mit 23
            # Stufen; Ursache: Rang-Tausch auf eine Rampe aus nur 3 braeunlichen Eintraegen). Jetzt EINE Transformation auf die G4-Helligkeit
            # der Zonen-Texel selbst (V3 "Material-Tausch je Texel ueber die Rampe, keine flache Farbinsel"): je Zonen-Texel der G4-Eintrag an
            # seiner Stelle (Seite s, Index i), L' = knie(b x f_s x L_G4), C' = r x L', Farbton h. So bleibt die Helligkeitsstruktur der
            # Original-Texel (Rang und Verhaeltnis, nur das Knie bei 75 staucht). Die drei Kennwerte kommen aus den braeunlichen G4-Eintraegen
            # wie in Runde 2 (Ton 35-80 Grad, C* > 6, Quell-L* <= P90 der Haut ihrer Seite; texelgewichtet wie die Hautflaechen sie lesen):
            #   h = Kreismittel ihrer Farbtoene, r = Median ihres C*/L* (Buntheit je Helligkeit der Quelle),
            #   b so, dass der texelgewichtete Median von b x f_s x L_G4 der Zonen-Texel = Median von f_s x L der braeunlichen Eintraege ist
            #   (dasselbe Helligkeitsniveau wie Runde 2: Zahlen-Pruefer "die uebrigen B1-Werte (L*, Ton) muessen bleiben").
            bw = tp['braun']
            ws_, Ls_, hs_, rs_, ein_ = [], [], [], [], []
            for s_ in range(3):
                n = nutz(gruppe('haut', s_))
                hb = TX.ton(lab_g4[s_]); cb_ = TX.buntheit(lab_g4[s_])
                Lh, hk, _hh, _ah = klassen[s_]
                hs = hk & (n > 0)
                lq = TX.gew_quantil(Lh[hs, 0], n[hs], float(bw['L_quantil']))
                sel_ = np.nonzero((n > 0) & (hb > bw['ton'][0]) & (hb < bw['ton'][1]) & (cb_ > bw['C_min']) & (Lh[:, 0] <= lq))[0]
                for i in sel_:
                    ein_.append('%d:%d' % (s_, int(i))); ws_.append(float(n[i])); Ls_.append(float(Lh[i, 0]) * fs[s_]); hs_.append(float(hb[i]))
                    rs_.append(float(cb_[i]) / max(float(Lh[i, 0]), 1e-6))
            if not ein_:
                raise SystemExit('braun: kein braeunlicher G4-Eintrag')
            wb = np.array(ws_)
            hbr = circ_mittel(np.array(hs_), wb)
            L_bezug = TX.gew_quantil(np.array(Ls_), wb, 0.5)
            r_cl = TX.gew_quantil(np.array(rs_), wb, 0.5)
            quellen = {}
            for nr_, lst_ in je_zone.items():
                if FLECK_ART[nr_] != 'braun':
                    continue
                for k_, xy_ in lst_:
                    if plan[k_][0] == 'f1':
                        raise SystemExit('braun auf F1-Flaeche %s' % (k_,))
                    q_ = (plan[k_][1], int(px0[xy_[1], xy_[0]]))
                    if cl_g4[q_[0]][q_[1]] == 0:
                        raise SystemExit('braun: transparenter Quelltexel %s' % (q_,))
                    quellen[q_] = quellen.get(q_, 0) + 1
            qs = sorted(quellen, key=lambda q_: (float(lab_g4[q_[0]][q_[1], 0]) * fs[q_[0]], q_))
            w = np.array([quellen[q_] for q_ in qs], float)
            L = np.array([float(lab_g4[s_][i, 0]) * fs[s_] for s_, i in qs])
            b_br = L_bezug / TX.gew_quantil(L, w, 0.5)
            lab = np.vstack([TX.transform(lab_g4[s_][i:i + 1], b_br * fs[s_], r_cl, hbr, kn, 'L') for s_, i in qs])
            sel = np.arange(len(qs))
            worte = None
            braun_pos.update({q_: j for j, q_ in enumerate(qs)})
            Lq0 = np.array([float(lab_g4[s_][i, 0]) for s_, i in qs])
            cv_g4 = float(np.sqrt(np.average((Lq0 - np.average(Lq0, weights=w)) ** 2, weights=w)) / np.average(Lq0, weights=w))
            ber['braun'] = dict(regel='L = knie(b x f_s x L_G4) je Zonen-Texel, C = r x L, Ton h (Runde 3, G6)', bezug_eintraege=ein_,
                                bezug_L_median=round(L_bezug, 3), r=round(r_cl, 4), ton=round(hbr, 2), b=round(b_br, 4),
                                quellen=len(qs), quell_texel=int(w.sum()), cv_L_g4_quelle=round(cv_g4, 3))
        elif mat == 'hautrampe':
            # Runde 2: Loch-Flaechen = Haut (Rang-Tausch auf die Worte der Hautzeile ihrer Seite, nur Hautklasse ohne die Eintraege unter dem
            # 1-%-Quantil, gewichtet wie die Hautflaechen dieser Seite sie lesen).
            n = nutz(gruppe('haut', s))
            Lh, hk, _hh, _ah = klassen[s]
            hs = hk & (n > 0)
            q1 = TX.gew_quantil(Lh[hs, 0], n[hs], 0.01)
            sel = np.nonzero(hs & (Lh[:, 0] >= q1))[0]
            w = n[sel]
            worte = [int(zeilen[('haut', s)][i]) for i in sel]
            lab = TX.lab_zeile(worte)
            L = Lh
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
    for nr, lst in sorted(je_zone.items(), key=lambda t: (t[0] == 'L', float(t[0]) if t[0] != 'L' else 0.0)):
        mat = FLECK_ART[nr]
        Lb = np.array([basis_L(k, xy[0], xy[1]) for k, xy in lst])
        o = np.argsort(Lb, kind='stable')
        q = np.empty(len(Lb))
        q[o] = (np.arange(len(Lb)) + 0.5) / len(Lb)
        for (k, xy), qq in zip(lst, q):
            s = plan[k][1] if mat in ('muster', 'weiss', 'hautrampe') else (1 if mat in ('fleisch', 'wunde') else 0)
            r = rampe(mat, s)
            if mat == 'braun':
                # Runde 3 (G6): kein Rang-Tausch, sondern der eigene G4-Eintrag des Texels (EINE Transformation, Struktur bleibt)
                zuweisung[(k, xy)] = (mat, s, braun_pos[(plan[k][1], int(px0[xy[1], xy[0]]))])
                continue
            rc = np.cumsum(r['w']); rq = (rc - 0.5 * r['w']) / rc[-1]
            j = int(np.clip(np.searchsorted(rq, qq), 0, len(rq) - 1))
            zuweisung[(k, xy)] = (mat, s, j)
        zone_info[nr] = dict(material=mat, texel_flaeche_paare=len(lst), traeger=len(set(k for k, _ in lst)),
                             zone_texel=(len(zon[nr]['texel']['liste']) if nr in zon else None))
    fl_info['zonen'] = zone_info

    rw_cache = {}

    def rampen_worte(mat, s):
        """15-Bit-Worte aller Eintraege der Rampe (mat, s), je Eintrag wie bisher (runde15, Farbton des Eintrags). Runde 3 (G8): danach entlang
        der Rampe (aufsteigend nach Quell-L*) gedeckelt - kein Eintrag mit L* > haut.L_max (naechster Rang darunter); ausgenommen das Auge
        (eigenes Material, O4) und das rote Fleisch (F1-Kern-Worte K5c, V6)."""
        key = (mat, s)
        if key in rw_cache:
            return rw_cache[key]
        ra = rampen[key]
        if ra['worte'] is not None:
            ws = [int(x) for x in ra['worte']]
        else:
            ws = []
            for j in range(len(ra['lab'])):
                lab = ra['lab'][j:j + 1]
                hh_ = float(TX.ton(lab)[0]) if TX.buntheit(lab)[0] > 1e-6 else h_haut
                w = TX.runde15(lab, [float(lab[0, 0])], hh_, rp['fenster'], rp['cmin'], nie_weiss=True)[0] & 0x7FFF
                ws.append(w or 0x8000)
        if mat not in ('auge', 'rotfleisch'):
            gd = deckel(ws, np.arange(len(ws), dtype=float), list(range(len(ws))), float(tp['haut']['L_max']))
            if gd:
                deckel_info['%s%d' % key] = dict(eintraege=len(gd), worte=['%d:%04x->%04x' % t for t in gd])
        rw_cache[key] = ws
        return ws

    # ---- Fleisch-Band (Eichung b): Median L*/C* ueber Zone-27-Texel muss im Band liegen (Pruefung, Riegel misst nach)
    # ---- Kopien + Indizes
    belegt = np.zeros(shape, bool)
    lm = {k: lese_maske(f, shape, kinder.get(k)) for k, f in flaechen.items()}
    for k in flaechen:
        belegt |= lm[k]
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
                m = lm[k]
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
                worte[r] = rampen_worte(mat, s)[j]
            zi = len(fleck_zeilen)
            fleck_zeilen.append((gkey, worte, zeile_faces, len(remap)))
            # Kopien je Flaeche
            for k in zeile_faces:
                f = flaechen[k]
                m = lm[k]
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
    # Runde 2: Augen-Rampe fuer kf_k5g3_auge.dekal (eigene Augen-Texel nach dem Spalt): Worte aufsteigend nach Quell-L*, Gewichte der K5c-Augenflaechen
    ra = rampe('auge', 0)
    aw = []
    for j in range(len(ra['lab'])):
        lab = ra['lab'][j:j + 1]
        hh_ = float(TX.ton(lab)[0]) if TX.buntheit(lab)[0] > 1e-6 else h_haut
        aw.append((TX.runde15(lab, [float(lab[0, 0])], hh_, rp['fenster'], rp['cmin'], nie_weiss=True)[0] & 0x7FFF) or 0x8000)
    ber['auge_rampe'] = dict(worte=aw, w=[float(x) for x in ra['w']], L=[round(float(x), 2) for x in TX.lab_zeile(aw)[:, 0]])
    return px, reihen, ber, plan, face_zeile, zuweisung, je, belegt
