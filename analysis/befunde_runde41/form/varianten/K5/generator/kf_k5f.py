#!/usr/bin/env python3
"""K5f - nicht blass (Nutzer 2026-11-09, Messblatt k5f/MESSBLATT_K5F.md): Variante, Spiel-Asset bleibt unangetastet.

    python kf_k5f.py <par.json> <aus-Ordner>      Bau (deterministisch: zweiter Lauf byte-gleich)
    python kf_k5f.py --rf <par.json>              fleisch.rf per Bisektion bestimmen und mit Quelle in par.json schreiben
    python kf_k5f.py --f <par.json>               haut.f per Bisektion (Bild 2 Video >= Vorlage, W6) bestimmen und in par.json schreiben

Nutzer 2026-11-09 nach dem Spielen von K5e: "Komplett augeblasst ... Null wie die restlichen Gegner im Spiel"; Nachtrag: "Und auf der Unterseite
am Hals hat er komische rote streifen, die sollten da auch nicht sein."

Gemessen (Messblatt 0.1/0.2): K5e_f1 ist nicht zu hell, sondern zu flach (additive Hebung +21,4 L* staucht die G4-Hautrampe p10..p90 von Faktor
3,1 auf 1,5; ein Lab-Punkt fuer alle Hauttexel). Die roten Streifen sind 6 Aussenflaechen von Mesh 22 (qf 1/8/9/194/201/209), die als
"Maulinneres" auf G4-Zeile 0 (Mundfleisch, Eintraege 196/37/208/244) lagen.

Verfahren (Messblatt 3; Memory reai-v2-texturen-nicht-aus-video-eichen):
 - Eingaenge, Texel, UV, Seite, Geometrie, EDD/EMR/Clips: wie kf_k5e (basis_k5c, feinschliff/K5d, einbau_k5c, CDEMD0.EMS Typ 0x34 TIM @0x65e800,
   ruhe/messung/material.json). Es aendert sich nur das CLUT-Wort je Flaeche und der Inhalt der Zeilen.
 - Je Material EINE Transformation auf der G4-Originalrampe der Seite (kf_k5e.klassen: haut / hell / ader):
     Haut   L' = knie(f x L*_G4) (multiplikativ, Knie ab L* 75: 75 + 25 tanh((L-75)/25)); Farbton h; C* = s x L'
     Ader   L' rangtreu linear auf [p10', q x p50'] (genutzte Haut-Eintraege der Zeile), C* = kappa x s x L', Ton h (RV6)
     Gelenk wie Haut, dann L' = knie(L' x rg), C* x cg
     Fleisch wie Haut, dann L' = knie(L' x rf), C* konstant cf (Adern kappa x cf), Ton hf
     Grau   wie Haut, C* = min(cgr x s x L', cgr_max)
   C* weich an den sRGB-Rand (kf_g4.r4_cmax: Cmax tanh(C/Cmax)), 15 Bit ueber kf_k5e.runde15 (naechste Farbe, Adern von unten). 0x0000 bleibt.
 - Referenz-Umsetzung ist die Vorschau k5f/skripte/k5f_vorschau.py (v7, im echten Spiel gemessen); dieser Bau ist dieselbe Rechnung mit
   12 statt 13 Zeilen (die leere Grau-Zeile fuer G4-Seite 2 entfaellt, Messblatt 3.4) und rf aus der Bisektion (Messblatt 3.2 Fleisch).
 - Zeilen 0 (G4-Zeile 0: Auge, 29 echte Maulinneres-Flaechen) und 9 (Zaehne, K5c-Zeile 7) bytegleich; Zeile 8 F1: Kern-Eintraege bytegleich
   K5c-Zeile 5, die uebrigen von den F1-Flaechen gelesenen Eintraege: G4-F11-Haut (K5c-Wort = G4-Zeile 2) = Hautzeile 3, sonst die
   Hauttransformation (knie(f L), C* = s L', Ton h) auf das K5c-Wort (wie K5e_f1, nur mit der K5f-Haut).
"""
import json
import math
import os
import shutil
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_g4 as KG  # noqa: E402

FE = E.FE
NZ = 12
K5 = E.K5
PAR = os.path.join(K5, 'k5f', 'K5f', 'k5f_par.json')


def knie(L, k):
    L = np.asarray(L, float)
    return np.where(L <= k, L, k + (100 - k) * np.tanh((L - k) / (100 - k)))


def schluessel(liste):
    return set(tuple(int(x) for x in s.split('|')) if isinstance(s, str) else tuple(int(x) for x in s) for s in liste)


def grau_flaechen(par):
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    g = set()
    for nm in par['grau']['material_listen']:
        g |= schluessel(mj[nm]['liste'])
    return g


def rampe(g4zeile, cnt, art, par):
    """EINE Transformation auf eine G4-Originalzeile (= k5f_vorschau.k5f_rampe, Messblatt 3.1/3.2)."""
    hp, ap = par['haut'], par['ader']
    f, kn = float(hp['f']), float(hp['knie'])
    L, haut, hell, ader = E.klassen(g4zeile)
    z0 = np.array(g4zeile) != 0
    cnt = np.asarray(cnt, float)
    Ln = knie(L[:, 0] * f, kn)
    hg = haut & (cnt > 0)
    if not hg.any():
        hg = haut
    w_ = cnt[hg] if cnt[hg].sum() > 0 else np.ones(int(hg.sum()))
    p10 = E.gew_quantil(Ln[hg], w_, .1)
    p50 = E.gew_quantil(Ln[hg], w_, .5)
    ag = ader & (cnt > 0)
    if not ag.any():
        ag = ader
    if ag.any():
        v0, v1 = float(L[ag, 0].min()), float(L[ag, 0].max())
        lo, hi = p10, float(ap['q']) * p50
        Ln[ader] = np.clip(lo + (L[ader, 0] - v0) / max(v1 - v0, 1e-6) * (hi - lo), min(lo, hi), hi)
    s = np.where(ader, float(ap['kappa']) * float(hp['s']), float(hp['s']))
    h = float(hp['h'])
    if art == 'gelenk':
        Ln = knie(Ln * float(par['gelenk']['rg']), kn)
        s = s * float(par['gelenk']['cg'])
    if art == 'grau':
        s = s * float(par['grau']['cgr'])
    if art == 'fleisch':
        fp = par['fleisch']
        Ln = knie(Ln * float(fp['rf']), kn)
        h = float(fp['hf'])
        s = np.where(ader, float(ap['kappa']), 1.0) * float(fp['cf']) / np.maximum(Ln, 1e-6)
    C = s * Ln
    if art == 'grau':
        C = np.minimum(C, float(par['grau']['cgr_max']))
    cm = KG.r4_cmax(Ln, h)
    C = np.where(cm > 1e-6, np.minimum(C, cm * np.tanh(C / np.maximum(cm, 1e-6))), 0.0)
    hr = math.radians(h)
    Z = np.stack([Ln, C * math.cos(hr), C * math.sin(hr)], 1)
    rgb = KG.rgb_aus_lab(Z)
    worte, qinfo = runde15_zeile(rgb, Z, L[:, 0], (haut, hell, ader), ader, z0, h, par.get('rundung') or {}, art)
    neu = list(g4zeile)
    for i in range(256):
        if not z0[i]:
            continue
        wn = worte[i] | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000
    return neu, dict(rundung=qinfo, p10_p50=[round(p10, 3), round(p50, 3)], haut_genutzt=int(hg.sum()), ader_genutzt=int((ader & (cnt > 0)).sum()),
                     hell_genutzt=int((hell & (cnt > 0)).sum()))


_KOMBI = np.array([[a, b, c] for a in range(-2, 4) for b in range(-2, 4) for c in range(-2, 4)])


def _lab15(c):
    return KG.lab(np.asarray(c, float) * 255.0 / 31.0)


def runde15_zeile(rgb, Z, L0, klassen, ader, z0, h, rp, art='haut'):
    """15-Bit-Rundung einer Zeile (keine neue Farbe, nur die Wahl des 5-Bit-Nachbarn; 6x6x6 Nachbarn um rgb x 31/255, kleinstes dE76 zum Ziel).
    Grund (gemessen, Riegel Lauf 1 mit kf_k5e.runde15 = Vorschau v7): bei den dunklen K5f-Toenen (L* 5-20, C* 3-8) liegen die 15-Bit-Farben im
    Farbton grob (nahe Grau nur ca. -68 / -34 / +21 Grad); die naechste Farbe streute den Haut-Farbton auf 88,4 Grad (H4 Soll <= 15), Grau 55,9,
    Gelenk 88,9, Fleisch 16,2, und vertauschte die Helligkeitsfolge (H5: 17 Paare). Darum, in dieser Reihenfolge der Bedingungen (rp['modus']
    'fenster'; 'naechste' = kf_k5e.runde15):
      F  Farbtonfenster: ein Kandidat mit C* >= cmin (3, = H4-Schwelle) muss im Fenster h +- fenster (7,5 = halbe H4-Spanne) liegen; Kandidaten
         mit C* < cmin sind unbunt und immer erlaubt.
      M  Rangfolge: je Klasse (haut / hell / ader) aufsteigend nach G4-L*; L* des Kandidaten >= groesstes L* der Eintraege mit kleinerem G4-L*.
      A  Ader von unten (RV6, wie runde15 richtung -1): L* und a* <= Ziel.
      B  nie bunter, nur fuer die Materialien in rp['nie_bunter'] (Gelenk 'weisslich', Grau 'grau/blau', 'ohne Rot'): C* des Kandidaten
         <= C* des Ziels oder unbunt (< cmin). Lauf 2 ohne B: Gelenk-Albedo-C* 8,41 statt Ziel-Median 6,5 (W1b Soll <= 8); Lauf 3 mit B fuer
         alle Materialien: Fleischanteil Schwanzspitze 49,9 % / Buckel 29,4 % (W2/W3 ROT), Haut weiss C* 5,18 - darum nur Gelenk und Grau.
    Stufen: F+B+M+A, F+M+A, F+M, F+A, F, alle (Zaehlung in der Rueckgabe)."""
    if rp.get('modus', 'fenster') == 'naechste':
        return [E.runde15(rgb[i], Z[i], -1 if ader[i] else 0) if z0[i] else 0 for i in range(256)], dict(modus='naechste')
    fen = float(rp.get('fenster', 7.5))
    hm = float((rp.get('fenster_mitte') or {}).get(art, h))      # Mitte des Farbtonfensters (Standard: Materialton h)
    cmin = float(rp.get('cmin', 3.0))
    out = [0] * 256
    stufen = [0] * 6
    for kl in klassen:
        ii = [i for i in np.nonzero(kl & z0)[0]]
        ii.sort(key=lambda i: (L0[i], i))
        vorher = -1e9
        gr_L = None
        gr_max = -1e9
        for i in ii:
            if gr_L is None or L0[i] > gr_L + 1e-9:
                vorher = max(vorher, gr_max)
                gr_L = L0[i]
            k = np.unique(np.clip(np.floor(np.asarray(rgb[i], float) * 31.0 / 255.0)[None, :] + _KOMBI, 0, 31), axis=0)
            lk = _lab15(k)
            d = np.linalg.norm(lk - Z[i][None, :], axis=1)
            C = np.hypot(lk[:, 1], lk[:, 2])
            dh = np.abs((np.degrees(np.arctan2(lk[:, 2], lk[:, 1])) - hm + 180.0) % 360.0 - 180.0)
            okf = (C < cmin) | (dh <= fen)
            okm = lk[:, 0] >= vorher - 1e-6
            oka = ((lk[:, 0] <= Z[i][0] + 1e-6) & (lk[:, 1] <= Z[i][1] + 1e-6)) if ader[i] else np.ones(len(k), bool)
            okb = (C < cmin) | (C <= float(np.hypot(Z[i][1], Z[i][2])) + 1e-6) if art in rp.get('nie_bunter', ()) else np.ones(len(k), bool)
            for st, ok in enumerate((okf & okb & okm & oka, okf & okm & oka, okf & okm, okf & oka, okf, np.ones(len(k), bool))):
                if ok.any():
                    break
            stufen[st] += 1
            j = int(np.argmin(np.where(ok, d, d + 1e9)))
            c = k[j].astype(int)
            out[i] = int(c[0]) | (int(c[1]) << 5) | (int(c[2]) << 10)
            gr_max = max(gr_max, float(lk[j, 0]))
    return out, dict(modus='fenster', fenster=fen, cmin=cmin, stufen_FBMA_FMA_FM_FA_F_alle=stufen)


def material_k5f(F, mat, quell, par):
    """{flaeche: (gruppe, G4-Seite)} mit den K5f-Aenderungen: 6 Hals-Aussenflaechen Maulinneres -> Haut; Lippe/Band/Krallen/Mesh-23-Haut -> Grau."""
    aussen = schluessel(par['hals']['aussen_maulinneres_zu_haut'])
    grau = grau_flaechen(par)
    gm = set(int(m) for m in par['grau']['meshes_haut'])
    out = {}
    for k, (nm, s) in E.material_seite(F, mat, quell).items():
        if nm == 'Maulinneres' and k in aussen:
            nm = 'Haut'
        if nm == 'Haut' and (k in grau or k[0] in gm):
            nm = 'Grau'
        out[k] = (nm, s)
    if any(out[k][0] != 'Haut' or out[k][1] != 0 for k in aussen):
        raise SystemExit('Hals-Aussenflaechen nicht Haut auf G4-Seite 0: %s' % {k: out[k] for k in aussen})
    return out


def zeilen(par, F, mat, quell, cl_k5c, cl_g4):
    Z = par['zeilen']
    zeile_je, nutz = {}, {}
    for k, (nm, s) in material_k5f(F, mat, quell, par).items():
        if nm == 'Haut':
            z, gr = Z['haut'][s], ('haut', s)
        elif nm == 'Grau':
            if s >= len(Z['grau']):
                raise SystemExit('Grau-Flaeche %s auf G4-Seite %d ohne Zeile' % (k, s))
            z, gr = Z['grau'][s], ('grau', s)
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
        elif nm == 'F1':
            z, gr = Z['f1'], None
        else:
            raise SystemExit('unbekanntes Material %s' % nm)
        zeile_je[k] = z
        if gr:
            n = nutz.setdefault(gr, np.zeros(256))
            np.add.at(n, F[k][2], 1)
    worte = [None] * NZ
    info = {}
    worte[Z['g4_0']] = [int(x) for x in cl_g4[0]]
    worte[Z['zahn']] = [int(x) for x in cl_k5c[7]]
    for s in range(3):
        g4z = [int(x) for x in cl_g4[s]]
        worte[Z['haut'][s]], info['haut%d' % s] = rampe(g4z, nutz[('haut', s)], 'haut', par)
        worte[Z['gelenk'][s]], info['gelenk%d' % s] = rampe(g4z, nutz[('gelenk', s)], 'gelenk', par)
    for s in range(len(Z['grau'])):
        worte[Z['grau'][s]], info['grau%d' % s] = rampe([int(x) for x in cl_g4[s]], nutz[('grau', s)], 'grau', par)
    worte[Z['fleisch']], info['fleisch1'] = rampe([int(x) for x in cl_g4[1]], nutz[('fleisch', 1)], 'fleisch', par)
    worte[Z['f1']], info['f1'] = f1_zeile(cl_k5c[5], F, mat, par, worte[Z['haut'][2]], [int(x) for x in cl_g4[2]])
    if any(w is None for w in worte):
        raise SystemExit('Zeile ohne Inhalt: %s' % [i for i, w in enumerate(worte) if w is None])
    return worte, zeile_je, info, {'%s%d' % gr: int(n.sum()) for gr, n in sorted(nutz.items())}, nutz


def f1_zeile(z5, F, mat, par, zeile3, g4z2):
    """F1 (Zeile 8): Kern-Eintraege bytegleich K5c-Zeile 5 (E30/E32); uebrige gelesene Eintraege: G4-F11-Haut = Hautzeile 3 (G4-Seite 2),
    sonst Hauttransformation (knie(f L), C* = s L', Ton h; runde15) auf das K5c-Wort (= k5f_vorschau)."""
    hp = par['haut']
    z5 = [int(x) for x in z5]
    kern = set(E.f1_kern_eintraege())
    gel = np.zeros(256, int)
    for k, v in mat.items():
        if v == 'F1':
            np.add.at(gel, F[k][2], 1)
    um = [i for i in range(256) if gel[i] > 0 and i not in kern and z5[i] != 0]
    Lz = KG.r4_eintraege(z5)[0]
    neu = list(z5)
    n_haut = 0
    hr = math.radians(float(hp['h']))
    for i in um:
        if z5[i] == g4z2[i]:
            neu[i] = zeile3[i]
            n_haut += 1
        else:
            l2 = knie(Lz[i:i + 1, 0] * float(hp['f']), float(hp['knie']))
            t = np.stack([l2, float(hp['s']) * l2 * math.cos(hr), float(hp['s']) * l2 * math.sin(hr)], 1)
            neu[i] = (E.runde15(KG.rgb_aus_lab(t)[0], t[0], 0) | (z5[i] & 0x8000)) or 0x8000
    return neu, dict(kern_eintraege=len(kern), umgefaerbt=len(um), davon_haut_zeile3=n_haut, fleischrand=len(um) - n_haut)


def bauen(par, aus):
    """kf_k5e.bauen mit dem K5f-Zeilenplan (NZ 12, zeilen()): Geometrie K5d, UV/Seite K5c, nur das CLUT-Wort neu; Asset-Sektionen wie kf_k5e."""
    alt = (E.NZ, E.zeilen)
    E.NZ, E.zeilen = NZ, zeilen
    try:
        b, w, z = E.bauen(par, aus)
    finally:
        E.NZ, E.zeilen = alt
    X = E.eingaenge()
    mk = material_k5f(X['F'], X['mat'], X['quell'], par)
    b['material_k5f_zaehlung'] = {nm: sum(1 for v in mk.values() if v[0] == nm) for nm in sorted(set(v[0] for v in mk.values()))}
    je = {}
    for k in sorted(mk):
        zz = str(z[k])
        je.setdefault(zz, {})
        je[zz][mk[k][0]] = je[zz].get(mk[k][0], 0) + 1
    b['material_k5f_je_zeile'] = {k: je[k] for k in sorted(je, key=int)}
    b['generator'] = 'generator/kf_k5f.py'
    json.dump(b, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    return b, w, z


def rf_bisektion(pp):
    """fleisch.rf: kleinster Faktor, mit dem Schwanzspitze und Buckel im Albedo ihre K4-Abstaende erreichen (kf_k5e_global.abstaende /
    k4_abstaende; Messblatt 3.2 Fleisch, K4 'vorher da'). Kein Videowert. Intervall [1,0; 2,0], 12 Schritte, aufgerundet auf 0,001."""
    sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
    import rl
    import kf_k5e_global as KGL
    par = json.load(open(pp, encoding='utf-8'))
    tmp = os.path.join(rl.SCR, 'k5f_rf_bau')
    k4 = KGL.k4_abstaende()
    print('K4 Abstaende Spitze/Buckel', [round(x, 3) for x in k4], flush=True)

    def mess(rf):
        p = json.loads(json.dumps(par))
        p['fleisch']['rf'] = rf
        if os.path.isdir(tmp):
            shutil.rmtree(tmp)
        bauen(p, tmp)
        return KGL.abstaende(tmp)
    lo, hi = 1.0, 2.0
    proto = []
    for _ in range(12):
        r = round(0.5 * (lo + hi), 5)
        a = mess(r)
        ok = a[0] >= k4[0] and a[1] >= k4[1]
        proto.append([r, round(a[0], 2), round(a[1], 2), ok])
        print('  rf %.5f -> Spitze %+.2f Buckel %+.2f %s' % (r, a[0], a[1], 'ok' if ok else ''), flush=True)
        if ok:
            hi = r
        else:
            lo = r
    rf = math.ceil(hi * 1000 - 1e-9) / 1000.0
    a = mess(rf)
    if not (a[0] >= k4[0] and a[1] >= k4[1]):
        raise SystemExit('rf %.3f erreicht K4 nicht' % rf)
    shutil.rmtree(tmp, ignore_errors=True)
    par['fleisch']['rf'] = rf
    par['fleisch']['rf_quelle'] = ('kf_k5f.py --rf: kleinster Faktor (Bisektion [1,0; 2,0], 12 Schritte, aufgerundet auf 0,001), mit dem Spitze und Buckel im '
                                   'Albedo (ruhe_wunsch.stand_albedo, Median) mindestens ihren K4-Abstand erreichen (K4_heute: Spitze gegen Schwanz %+.2f, '
                                   'Buckel gegen Mesh-10-Haut %+.2f; "vorher da", Messblatt K5F 3.2 Fleisch / W2 / W3); Ergebnis Spitze %+.2f, Buckel %+.2f. '
                                   'Messblatt: rf 1,51 ergab Buckel +3,57 (ROT), rf 1,60 ergab +18,65 / +11,11. Protokoll [rf, Spitze, Buckel, ok]: %s'
                                   % (k4[0], k4[1], a[0], a[1], proto))
    json.dump(par, open(pp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print('fertig rf', rf, 'Spitze %+.2f Buckel %+.2f' % a)


def f_bisektion(pp):
    """haut.f: kleinster Faktor mit Bild 2 Video (3 Saaten, Pixelmenge V4F 3.1 = Nutzer-Teilmasken & K5c-Modell & ohne Weisskappe) >= Vorlage
    (Messblatt K5F 3.3 / W6; wie kf_k5e_global, EIN globaler Helligkeitswert, kein Texel-/Fleckwert). Intervall [2,3; 3,0], 10 Schritte,
    aufgerundet auf 0,001."""
    sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
    import rl
    import riegel_k5d as RK
    par = json.load(open(pp, encoding='utf-8'))
    tmp = os.path.join(rl.SCR, 'k5f_f_bau')
    vor = RK.Vorlage()
    _s, Kc = rl.v4f.szene_id(E.BASIS, 2)
    fig = RK.nutzer_union(2) & Kc['maske'] & ~vor.d[2]['clip']
    ziel = float(vor.d[2]['LV'][fig][:, 0].mean())

    def hell(f):
        p = json.loads(json.dumps(par))
        p['haut']['f'] = f
        if os.path.isdir(tmp):
            shutil.rmtree(tmp)
        bauen(p, tmp)
        rl.v4f._KC.pop(tmp, None)
        s_, _K = rl.v4f.szene_id(tmp, 2)
        return float(np.mean([RK.labm(rl.v4f.video(s_, 2, sa))[fig][:, 0].mean() for sa in rl.v4f.SAATEN]))
    lo, hi = 2.3, 3.0
    proto = []
    for _ in range(10):
        f = round(0.5 * (lo + hi), 5)
        v = hell(f)
        proto.append([f, round(v, 3)])
        print('  f %.5f -> Bild 2 Video %.3f (Vorlage %.3f)' % (f, v, ziel), flush=True)
        if v >= ziel:
            hi = f
        else:
            lo = f
    f = math.ceil(hi * 1000 - 1e-9) / 1000.0
    v = hell(f)
    if v < ziel - 0.005:
        raise SystemExit('f %.3f erreicht die Vorlage nicht (%.3f)' % (f, v))
    shutil.rmtree(tmp, ignore_errors=True)
    par['haut']['f'] = f
    par['haut']['f_quelle'] = ('kf_k5f.py --f: kleinster Faktor (Bisektion [2,3; 3,0], 10 Schritte, aufgerundet auf 0,001) mit Bild 2 Video (3 Saaten, Pixelmenge V4F 3.1) '
                               '>= Vorlage %.2f (Messblatt K5F 3.3 / W6, EIN globaler Helligkeitswert); Ergebnis %.3f. Messblatt-Vorschau v7: f 2,46 mit naechster '
                               '15-Bit-Farbe und rf 1,60 ergab 15,37; mit Farbtonfenster-Rundung und rf aus der Bisektion faellt Bild 2 darunter. '
                               'Protokoll [f, Bild 2]: %s' % (ziel, v, proto))
    json.dump(par, open(pp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print('fertig f', f, 'Bild 2 Video %.3f' % v)


if __name__ == '__main__':
    if sys.argv[1] == '--rf':
        rf_bisektion(sys.argv[2])
        sys.exit(0)
    if sys.argv[1] == '--f':
        f_bisektion(sys.argv[2])
        sys.exit(0)
    par = json.load(open(sys.argv[1], encoding='utf-8'))
    b, _w, _z = bauen(par, sys.argv[2])
    print(json.dumps({k: b[k] for k in ('zeilen', 'flaechen', 'flaechen_je_zeile', 'texel_neu', 'material_k5f_zaehlung', 'sha256')}, indent=1))
