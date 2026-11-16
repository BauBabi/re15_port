"""SF V8 Runde 5: Brust von vorn nach der Pruefung V8 r4 -> adern_v8.json (modus "original_r5"; Bau: adern_v8.adern).

    python adern_r5.py [--basis EMD_V7] [--kette JSON] [--ruecken JSON] [--start JSON] [--aus JSON]

Befund (Sicht-Pruefer r4 (3), "Abweichung, die der Nutzer ablehnen wuerde"): "Von vorn liegt rechts neben dem Gesicht, also an der (W)-linken
oberen Brust, eine graue Platte (Brust-Zeile 488, V7 nach O1). Sie grenzt jetzt mit gerader harter Kante an die neue rosa Haut darueber und
links vom Kopf. Darin stehen helle weisse Keile." Gemessen offline F354: Platte L 32 / a* 5,6 gegen Haut L 51 / a* 14,8 (V7 dL 4); echte
Framedumps F344-F372 fast weisse Pixel (L > 75, Chroma < 12) V8 r4 2097 gegen V7 5.
Auftrag (bindend): "die Brust traegt ihre roten Stellen weiter (Nutzer V5: "Die roten Stellen sind korrekt auf der brust und am oberen
linken Arm"), aber auf derselben Original-EM30-Haut wie die Flanke (Original-Texel und -Eintraege, kein Feld C), keine weissen Keile
(Herkunft der weissen Texel bestimmen und durch Original-Haut ersetzen)".
Regel (Memory "Texturen nie aus Videobildern eichen"): Texel und Paletteneintraege nur aus dem Original-EM30; das Video liefert nur Orte.

Messungen (sfv8/r5 b1.py, b3.py, b4.py; Original = birkin_formen.lade_em30, CDEMD0.EMS Typ 0x30):
  - Brust: die 34 Flaechen von Mesh 8 auf Zeile 488 (V7) haben im Original-EM30 Zeile 481 und dieselbe UV (34 von 34). Zeile 488 ist die
    Kopie der Referenz-2-Zeile 481 (Dossier V5). -> Brust auf Zeile 494 (= Original-Zeile 481) mit eigener UV: dieselbe Original-Haut wie
    die Flanke, die roten Stellen der Brust sind dieselben Texel (adern_v8.ORIGINAL_ZEILE[488] = 1).
  - Weisse Keile: KEINE Brust-Texel (V8 r4: 0 weisse Bildpunkte auf Zeile 488), sondern Original-Eintraege der Zeilen 480/481, die fast weiss
    sind (WEISS_L / WEISS_C, Mass des Sicht-Pruefers r4): Zeile 480 Index 0, 6, 7, 9, 130, 138, 141, 154, 156, 159; Zeile 481 Index 0, 7,
    140, 143, 147, 151, 152 (L* 75-98). Sie liegen unter Mesh-8-Flaechen neben dem Kopf (P1: v24 40, d17 40, d66 38, d8 22, v4 21 px) und
    unter 6 Brust-Flaechen (d2, d27, d32, d60, d65, d69: 111 Texel). Das Original-EM30 zeigt dieselben weissen Flecken (P1 115 px).
Umsetzung (PORT-WAHL):
  1. Brust -> Zeile 494, eigene UV (= Original-UV).
  2. Jede Mesh-8-Flaeche auf einer neuen Zeile, die von vorn sichtbar ist (P1 und Szene Takt VORN_TAKTE, Maler-Reihenfolge wie im Spiel) und
     fast weisse Texel zeigt, bekommt eine Abbildung (Skalierung SKALEN_R5 um die kleinste UV-Ecke, dann Schub) auf Texel, die Original-Flaechen
     derselben Zeile benutzen (Seite gleich, kein Feld C) und die nicht fast weiss sind (samt WEISS_RAND). Flaechen der Gruppen von Runde 4
     (rueckenfleisch, blass_rechts) verlassen ihre Gruppe, wenn fuer die ganze Gruppe kein Schub ohne Weiss passt (Basis = Gruppen-Schub).
     Wahl: kleinster groesster Ecken-Versatz gegen die bisherige Lage (versatz: so weit springt die Textur an der Kante zur Nachbarflaeche);
     eine UV-Insel nur dann gemeinsam, wenn das den Versatz nicht vergroessert.
  3. Feinwahl (feinwahl): Gruppen an der Brustgrenze - unter den FEIN_K Abbildungen mit kleinstem Versatz die mit dem kleinsten Kanten-Sprung
     (Riegel S14a, FEIN_POSEN).
  4. blass_r5: Riegel S7c/S7d wieder einhalten (Brust auf Original-Haut macht den Rumpf (W)-rechts roeter) - je Flaeche ein Schub auf blasse
     Original-Haut ohne kraeftiges Rot, in der Flecken-Phase die weiss-Gruppen mit den meisten Flecken neu (kleinster Anteil a* >= 15).
Ablauf des Ergebnisses adern_v8.json (Runde 5): suche + feinwahl mit FEIN_POSEN (P1, F354); dann --nur-fein mit (P1, F344, F354, F364);
dann --nur-blass (Protokolle sfv8/r5 adern_r5f.log, adern_r5g.log, adern_r5l.log).
"""
import os, sys, json, argparse, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import adern_r3 as A3                 # noqa: E402
import adern_r4 as A4                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import f_rast as FR                   # noqa: E402
from scipy import ndimage             # noqa: E402

BRUST = 488                           # V7-Zeile der Brust (Kopie Referenz-2-Zeile 481; Original-Zeile der 34 Flaechen: 481)
WEISS_L, WEISS_C = 75.0, 12.0         # "fast weisse Pixel (L > 75, Chroma < 12)" - Sicht-Pruefer r4 (3), sein Mass, unveraendert
VORN_TAKTE = tuple(range(336, 373, 2))   # Szene von vorn (Clip 1, Gier 1513-1529) = Framedump F342-F378 (wie r4_gemein.SCHLITZ_TAKTE)
GRUPPEN_R4 = ('rueckenfleisch', 'huegel', 'blass_rechts')


def weiss_eintraege(k):
    """Original-Zeile 480 + k: Eintraege mit L* > WEISS_L und Chroma < WEISS_C (Lab wie Messblatt, adern_v8.lab15)."""
    _MO, TO = A4.original()
    L = AV.lab15(TO['clut'][k])
    return (L[:, 0] > WEISS_L) & (np.hypot(L[:, 1], L[:, 2]) < WEISS_C)


def weiss_karte(k):
    _MO, TO = A4.original()
    return weiss_eintraege(k)[TO['pix']]


def weiss_texel(uv):
    """Fast weisse Texel unter einer Flaeche auf einer neuen Zeile (493 + k)."""
    z = ((uv[2] >> 6) & 0x1FF) - 493
    if z < 0:
        return 0
    mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    return int(weiss_karte(z)[mk].sum())


WEISS_RAND = 1                        # fast weisse Texel samt 1 Texel Rand meiden: der Rasterer liest an den Flaechenkanten bis 1 Texel ausserhalb
                                      # der UV-Flaeche (gemessen sfv8/r5 r5d: ohne Rand 5 weisse Bildpunkte in P1 an v24/d2/d32, Texel darunter 0)


def erlaubt(k, same):
    """Texel, die Original-Flaechen mit Zeile 480 + k benutzen, Original-Texel, ohne Feld C, nicht fast weiss (mit WEISS_RAND)."""
    w = ndimage.binary_dilation(weiss_karte(k), structure=np.ones((3, 3)), iterations=WEISS_RAND)
    return A4.nutzung(None, 480 + k) & same & ~A4.feld_c_maske() & ~w


SKALEN_R5 = (1.0, 0.95, 0.9, 0.85, 0.8, 0.75, 0.7, 0.65, 0.6)   # wie adern_r4.SKALEN (Unterarmband 1,0 .. 0,5), feiner, bis 0,6


def ecken_uv(uv):
    return [(uv[3 * i], uv[3 * i + 1]) for i in range(3 if len(uv) == 9 else 4)]


def versatz(uvs, sk, s, b0):
    """Groesster Versatz (Texel) einer UV-Ecke gegen ihre Lage beim bisherigen Schub b0: so weit springt die Textur an der Kante zur
    Nachbarflaeche (die die Ecke unverschoben zeigt)."""
    u0 = min(c[0] for uv in uvs for c in ecken_uv(uv)); v0 = min(c[1] for uv in uvs for c in ecken_uv(uv))
    d = 0.0
    for uv in uvs:
        for (u, v) in ecken_uv(uv):
            nu = int(round(u0 + (u - u0) * sk)) + s[0]; nv = int(round(v0 + (v - v0) * sk)) + s[1]
            d = max(d, float(np.hypot(nu - (u + b0[0]), nv - (v + b0[1]))))
    return d


def kandidaten(uvs, E, b0=(0, 0), K=1):
    """Die K Abbildungen (Skalierung SKALEN_R5, Schub; Ziel ganz in E) mit dem kleinsten groessten Ecken-Versatz gegen b0 (versatz), bei
    gleichem Versatz die groessere Skalierung; gleiche UV-Ergebnisse nur einmal. Liste von (skal, schub, versatz)."""
    alle = []
    for sk in SKALEN_R5:
        g = A4.skaliert(uvs, sk)
        sh, _Gm, _po = A4.schuebe(g, E)
        for s in sh:
            alle.append(((round(versatz(uvs, sk, s, b0), 2), -sk, s), sk, s, g))
    alle.sort(key=lambda t: t[0])
    out, gesehen = [], set()
    for k, sk, s, g in alle:
        uvk = tuple(tuple((c[0] + s[0], c[1] + s[1]) for c in ecken_uv(uv)) for uv in g)
        if uvk in gesehen:
            continue
        gesehen.add(uvk)
        out.append((sk, s, k[0]))
        if len(out) >= K:
            break
    return out


def schub_wahl(uvs, E, b0=(0, 0)):
    """Kleinste Verzerrung ohne fast weisse Ziel-Texel: je Skalierung SKALEN_R5 (um die kleinste UV-Ecke wie adern_v8.adern_original) alle
    Schuebe, deren Ziel ganz in E liegt; Wahl = kleinster groesster Ecken-Versatz gegen b0 (versatz), dann groessere Skalierung, dann Schub.
    Rueckgabe (skal, schub, versatz) oder None."""
    best = None
    for sk in SKALEN_R5:
        g = A4.skaliert(uvs, sk)
        sh, _Gm, _po = A4.schuebe(g, E)
        for s in sh:
            k = (round(versatz(uvs, sk, s, b0), 2), -sk, s)
            if best is None or k < best[0]:
                best = (k, sk, s)
    if best is None:
        return None
    return best[1], best[2], best[0][0]


def vorn_sichtbar(blob):
    """Flaechen-Schluessel von Mesh 8, die in P1 oder einem Takt VORN_TAKTE sichtbar sind (f_rast, RDT-Licht, Maler-Reihenfolge)."""
    mo = V.modell(blob)
    sp = R3.szene_posen()
    out = set()
    for pose in [R3.P1] + [sp[t] for t in VORN_TAKTE]:
        A = FR.mit_bg(mo, pose)
        b, fl = A['bone'][:480], A['flaeche'][:480]
        out |= set(fl[b == 8].tolist())
    return out


def inseln(ms8, keys):
    info = {k: ((AV.flaeche(ms8, k)[2] >> 6) & 0x1FF, AV.flaeche(ms8, k)[5] & 0xF,
                set((AV.flaeche(ms8, k)[3 * i], AV.flaeche(ms8, k)[3 * i + 1]) for i in range(3 if len(AV.flaeche(ms8, k)) == 9 else 4)))
            for k in keys}
    par = {k: k for k in keys}

    def f(k):
        while par[k] != k:
            k = par[k]
        return k
    ks = sorted(keys, key=lambda k: (k[0], int(k[1:])))
    for i, a in enumerate(ks):
        for b in ks[i + 1:]:
            if info[a][:2] == info[b][:2] and info[a][2] & info[b][2]:
                par[f(a)] = f(b)
    comp = {}
    for k in ks:
        comp.setdefault(f(k), []).append(k)
    return sorted(comp.values(), key=lambda c: (c[0][0], int(c[0][1:])))


def anwenden(geo, par):
    return A4.anwenden(geo, par)


def suche(geo, par4, log=print):
    _d, _o, ms_geo, _t = KB.teile(geo)
    same = KB.tim_teile(KB.teile(geo)[3])['pix'] == A4.original()[1]['pix']
    par = copy.deepcopy(par4)
    par['modus'] = 'original_r5'
    # 1) Brust auf die Original-Zeile (eigene UV)
    brust = [k for k, uv in A4.keys(ms_geo[8]) if (uv[2] >> 6) & 0x1FF == BRUST]
    par['flaechen']['8'] = sorted(set(par['flaechen']['8']) | set(brust), key=lambda k: (k[0], int(k[1:])))
    log('Brust: %d Flaechen 488 -> Original-Zeile 481 (494), eigene UV' % len(brust))
    E = {k: erlaubt(k, same) for k in (0, 1)}
    # 2) von vorn sichtbare Flaechen von Mesh 8 mit fast weissen Texeln
    bl = anwenden(geo, par)
    _d, _o, ms_bl, _t = KB.teile(bl)
    sicht = vorn_sichtbar(bl)
    kand = [k for k, uv in A4.keys(ms_bl[8]) if k in sicht and weiss_texel(uv) > 0]
    log('Weiss und von vorn sichtbar: %s' % {k: weiss_texel(AV.flaeche(ms_bl[8], k)) for k in kand})
    # 2a) Gruppen von Runde 4 (rueckenfleisch, huegel, blass_rechts): passt fuer die GANZE Gruppe ein Schub ohne weisse Ziel-Texel, wird er neu
    #     gewaehlt (Rangfolge wie Runde 4, adern_r4.haut_wahl); sonst verlassen die betroffenen Flaechen die Gruppe (Basis = Gruppen-Schub)
    a0 = A4.a_karte(0)
    basis = {}
    for g in par['gruppen']:
        if g['name'] not in GRUPPEN_R4:
            continue
        betr = [k for k in g['flaechen'] if k in kand]
        if not betr:
            log('Gruppe %s: keine von vorn sichtbare weisse Flaeche, bleibt (Schub %s)' % (g['name'], g['schub']))
            continue
        uvs = [AV.flaeche(ms_geo[8], k) for k in g['flaechen']]
        sh, Gm, po = A4.schuebe(uvs, E[0])
        if sh:
            s_ = min(sh, key=lambda s: A4.haut_wahl(a0, Gm, s))
            log('Gruppe %s: weiss %s -> Schub %s statt %s (%d Schuebe ohne Weiss)' % (g['name'], betr, list(s_), g['schub'], len(sh)))
            g['schub_r4'] = list(g['schub']); g['schub'] = list(s_)
            g['erlaubt'] = g['erlaubt'] + ', ohne fast weisse Eintraege (Runde 5)'
            kand = [k for k in kand if k not in g['flaechen']]
        else:
            log('Gruppe %s: kein Schub ohne Weiss fuer die ganze Gruppe; %s verlassen sie (Basis %s)' % (g['name'], betr, g['schub']))
            g['flaechen'] = [k for k in g['flaechen'] if k not in betr]
            g['ohne_r5'] = betr
            for k in betr:
                basis[k] = tuple(g['schub'])
    # 2b) die uebrigen: je UV-Insel (gleiche Zeile, Seite und Basis-Schub, gemeinsame UV-Ecke) EIN Schub; passt keiner, je Flaeche
    neu = []
    nach_basis = {}
    for k in kand:
        nach_basis.setdefault(basis.get(k, (0, 0)), []).append(k)
    for b0, ks in sorted(nach_basis.items()):
        for ins in inseln(ms_geo[8], ks):
            z = ((AV.flaeche(ms_bl[8], ins[0])[2] >> 6) & 0x1FF) - 493
            teile = [ins]
            w_ = schub_wahl([AV.flaeche(ms_geo[8], k) for k in ins], E[z], b0)
            if len(ins) > 1:
                # je Flaeche, wenn das den groessten Ecken-Versatz der Insel senkt (oder fuer die ganze Insel kein Schub passt)
                einzeln = [schub_wahl([AV.flaeche(ms_geo[8], k)], E[z], b0) for k in ins]
                if all(e is not None for e in einzeln) and (w_ is None or max(e[2] for e in einzeln) < w_[2]):
                    teile = [[k] for k in ins]
            for t in teile:
                w2 = w_ if t is ins else einzeln[ins.index(t[0])] if len(ins) > 1 else schub_wahl([AV.flaeche(ms_geo[8], k) for k in t], E[z], b0)
                if w2 is None:
                    raise SystemExit('Weiss: kein Schub fuer %s (Zeile %d)' % (t, 493 + z))
                sk, s2, vs = w2
                g = dict(name='weiss_%s' % '_'.join(t), mesh=8, flaechen=list(t), skal=sk, schub=list(s2), basis=list(b0), versatz=vs,
                         weiss_vorher=int(sum(weiss_texel(AV.flaeche(ms_bl[8], k)) for k in t)),
                         erlaubt='Texel der Original-Flaechen mit Zeile %d, ohne Feld C, ohne fast weisse Eintraege' % (480 + z))
                neu.append(g)
                log('Weiss: %s Zeile %d Basis %s Skalierung %.2f Schub %s Ecken-Versatz %.1f Texel' % (t, 493 + z, list(b0), sk, list(s2), vs))
    weg = set(k for g in neu for k in g['flaechen'])
    par['flaechen']['8'] = [k for k in par['flaechen']['8'] if k not in weg]
    par['gruppen'] = par['gruppen'] + neu
    return par


FEIN_K = 12                           # Feinwahl: je Gruppe an der Brustgrenze die 12 Abbildungen mit dem kleinsten Ecken-Versatz gerendert
FEIN_RUNDEN = 2                       # zwei Durchgaenge in fester Reihenfolge (die Gruppen grenzen aneinander: d2|d17, d60|d17)
FEIN_MAX = 0.25                       # Gewicht des groessten Grenzstuecks (eine einzelne harte Kante) neben den Mitteln dL + da*
FEIN_POSEN = ('P1', 344, 354, 364)    # wie Riegel S14 (Foto-1-Pose und Szene von vorn, Messstelle des Sicht-Pruefers r4 F354)


def feinwahl(geo, par, log=print):
    """Gruppen "weiss_*", die in FEIN_POSEN an die Brustgrenze stossen (Grenzstuecke des Riegels S14a): je Gruppe (feste Reihenfolge) unter den
    FEIN_K Abbildungen mit kleinstem Ecken-Versatz die mit dem kleinsten Kanten-Sprung (Summe dL + da* ueber FEIN_POSEN, riegel_v8.brust_kante)."""
    import riegel_v8 as RG
    _d, _o, ms_geo, _t = KB.teile(geo)
    same = KB.tim_teile(KB.teile(geo)[3])['pix'] == A4.original()[1]['pix']
    E = {k: erlaubt(k, same) for k in (0, 1)}
    brust = RG.brust_flaechen()

    def kosten(p):
        r = RG.s14_kante(anwenden(geo, p), FEIN_POSEN, brust)
        return round(sum(v['dL'] + v['da'] + FEIN_MAX * v['max_dL'] for v in r.values()), 3), r
    c0, r0 = kosten(par)
    an_grenze = set(k for v in r0.values() for st in v['stuecke'] for k in st.split('|'))
    log('Feinwahl Start: Kanten-Sprung %.3f %s' % (c0, {p: (v['dL'], v['da']) for p, v in r0.items()}))
    for _rd, g in [(rd, g) for rd in range(FEIN_RUNDEN) for g in par['gruppen']]:
        if not g['name'].startswith('weiss_') or not (set(g['flaechen']) & an_grenze):
            continue
        z = ((AV.flaeche(ms_geo[8], g['flaechen'][0])[2] >> 6) & 0x1FF)
        k = AV.ORIGINAL_ZEILE[z]
        uvs = [AV.flaeche(ms_geo[8], q) for q in g['flaechen']]
        best = (c0, g['skal'], list(g['schub']), g['versatz'])
        for sk, s_, vs in kandidaten(uvs, E[k], tuple(g['basis']), FEIN_K):
            if (sk, list(s_)) == (g['skal'], list(g['schub'])):
                continue
            alt = (g['skal'], g['schub'], g['versatz'])
            g['skal'], g['schub'], g['versatz'] = sk, list(s_), vs
            c, _r = kosten(par)
            g['skal'], g['schub'], g['versatz'] = alt
            if c < best[0] - 1e-9:
                best = (c, sk, list(s_), vs)
        if (best[1], best[2]) != (g['skal'], g['schub']):
            log('Feinwahl %s: Skalierung %.2f Schub %s (Versatz %.1f) -> Kanten-Sprung %.3f' % (g['name'], best[1], best[2], best[3], best[0]))
            g['skal'], g['schub'], g['versatz'] = best[1], best[2], best[3]
            c0 = best[0]
        g['feinwahl'] = dict(kandidaten=FEIN_K, runden=FEIN_RUNDEN, kanten_sprung=c0)
    log('Feinwahl Ende: Kanten-Sprung %.3f' % c0)
    return par


def blass_r5(geo, par, log=print):
    """Riegel S7c/S7d wieder einhalten (gemessen am Bau mit Brust auf Original-Haut, sfv8/r5 V8r5a: Rumpf 8 (W)-rechts 53,9 % gegen Original
    gleiche Pose 49,3; rote Flecken je 1000 px 6,17 / 4,98 gegen Original 6,10 / 4,96): wie adern_r4 Schritt 5 ("blass (W)-rechts") eine Gruppe
    "blass_r5_<Flaeche>" je Flaeche mit einem Schub (adern_r4.haut_wahl, Ziel ohne fast weisse Texel; eine gemeinsame Gruppe wie in Runde 4
    senkte den Rumpf nicht: sfv8/r5 adern_r5h.log 53,9 -> 50,9 -> 55,3 -> 54,5), gierig die (W)-rechte Flaeche von Mesh 8 mit den meisten roten
    Bildpunkten (a* >= 10, PV) bzw. danach mit den meisten roten Flecken, bis Rumpf (W)-rechts <= Original - adern_r4.RUMPF_RAND und Flecken je
    1000 px <= Original - adern_r4.FLECKEN_RAND. Nicht in die Gruppe: Brust (Nutzer V5 "Die roten Stellen sind korrekt auf der brust"), Flaechen
    an der Brustgrenze (Riegel S14a), Flaechen anderer Gruppen."""
    import riegel_v8 as RG
    import birkin_formen as BF
    _d, _o, ms_geo, _t = KB.teile(geo)
    same = KB.tim_teile(KB.teile(geo)[3])['pix'] == A4.original()[1]['pix']
    E0 = erlaubt(0, same)
    E0[:, 128:] = False
    a0 = A4.a_karte(0)
    brust = RG.brust_flaechen()
    an_grenze = set(k for v in RG.s14_kante(anwenden(geo, par)).values() for st in v['stuecke'] for k in st.split('|'))
    in_gruppen = set(k for g in par['gruppen'] if g['mesh'] == 8 for k in g['flaechen'])
    ro, _x, _y = A4.rot_pv(BF.lade_em30())
    fo = {k: RG.rote_flecken(BF.lade_em30(), knochen=k) for k in ((8, 13), (8, 13, 14))}
    dichte = lambda t_: 1000.0 * t_[0] / max(t_[2], 1)

    def mass(p):
        bl = anwenden(geo, p)
        r, _h, _A = A4.rot_pv(bl)
        return bl, r['r8z'], max(dichte(RG.rote_flecken(bl, knochen=k)) - dichte(fo[k]) for k in fo)
    bl, r8z, fu = mass(par)
    log('blass_r5 Start: Rumpf (W)-rechts %.1f (Original %.1f), Flecken-Ueberschuss %.2f' % (r8z, ro['r8z'], fu))
    sf, neu_g = [], []
    p2 = copy.deepcopy(par)
    for _runde in range(16):
        if r8z <= ro['r8z'] - A4.RUMPF_RAND and fu <= -A4.FLECKEN_RAND:
            break
        phase_rot = r8z > ro['r8z'] - A4.RUMPF_RAND
        rotz = A4.rot_flaechen_rechts(bl) if phase_rot else A4.flecken_flaechen(bl)
        weiss_g = {g['flaechen'][0]: g for g in p2['gruppen'] if g['name'].startswith('weiss_') and len(g['flaechen']) == 1
                   and g['flaechen'][0] not in an_grenze and g['flaechen'][0] not in sf}
        if not phase_rot:
            # Flecken-Phase: die Flecken liegen meist auf Flaechen der weiss-Gruppen (gemessen sfv8/r5 V8r5a: v24 5, v4 3, v21 2, d66 2, v25 2);
            # deren Abbildung wird unter den FEIN_K mit kleinstem Ecken-Versatz neu gewaehlt: kleinster Anteil a* >= 15, dann Versatz
            wk = [k for k in sorted(rotz, key=lambda k: (-rotz[k], k)) if k in weiss_g]
            if wk:
                k = wk[0]; g_ = weiss_g[k]
                z = AV.ORIGINAL_ZEILE[(AV.flaeche(ms_geo[8], k)[2] >> 6) & 0x1FF]
                ak = A4.a_karte(z)
                Ez = erlaubt(z, same)
                uvs = [AV.flaeche(ms_geo[8], k)]
                bestk = None
                for sk, s_, vs in kandidaten(uvs, Ez, tuple(g_['basis']), FEIN_K):
                    g2 = A4.skaliert(uvs, sk)
                    mk = np.zeros((256, 384), bool)
                    for uv in g2:
                        q = list(uv)
                        for i in range(3 if len(uv) == 9 else 4):
                            q[3 * i] += s_[0]; q[3 * i + 1] += s_[1]
                        mk |= KB.raster(q, 3 if len(q) == 9 else 4, (q[5] & 0xF) * 128)
                    r15 = round(float((ak[mk] >= 15).mean()), 2)
                    if bestk is None or (r15, vs) < bestk[0]:
                        bestk = ((r15, vs), sk, s_, vs)
                for g in p2['gruppen']:
                    if g['name'] == g_['name']:
                        g['skal'], g['schub'], g['versatz'] = bestk[1], list(bestk[2]), bestk[3]
                        g['flecken_r5'] = dict(rot15=bestk[0][0], kandidaten=FEIN_K)
                sf.append(k)
                bl, r8z, fu = mass(p2)
                log('blass_r5: %s neu Skalierung %.2f Schub %s (a* >= 15 %.2f) -> Rumpf (W)-rechts %.1f, Flecken-Ueberschuss %.2f'
                    % (g_['name'], bestk[1], list(bestk[2]), bestk[0][0], r8z, fu))
                continue
        kand = [k for k in sorted(rotz, key=lambda k: (-rotz[k], k)) if k not in brust and k not in an_grenze and k not in in_gruppen
                and k not in sf and k in par['flaechen']['8'] and (AV.flaeche(ms_geo[8], k)[2] >> 6) & 0x1FF == 480]
        neu = None
        for k in kand:
            uvs = [AV.flaeche(ms_geo[8], k)]
            sh, Gm, _po = A4.schuebe(uvs, E0)
            if sh:
                # Ziel mit kleinem Anteil a* >= 15 (keine neuen roten Flecken, Riegel S7d; Stufen 0,02), dann haut_wahl (Adern wie die Umgebung)
                neu = (k, min(sh, key=lambda s_: (round(float((a0[:, :128][np.roll(np.roll(Gm, s_[1], 0), s_[0], 1)] >= 15).mean()) / 0.02),
                                                  A4.haut_wahl(a0, Gm, s_))))
                break
        if neu is None:
            log('blass_r5: keine weitere Flaeche mit Schub'); break
        sf.append(neu[0])
        neu_g.append(dict(name='blass_r5_%s' % neu[0], mesh=8, flaechen=[neu[0]], skal=1.0, schub=list(neu[1]),
                          erlaubt='Texel der Original-Flaechen mit Zeile 480, Seite 0, ohne Feld C, ohne fast weisse Eintraege'))
        p2['flaechen']['8'] = [k for k in p2['flaechen']['8'] if k not in sf]
        p2['gruppen'] = [g for g in p2['gruppen'] if not g['name'].startswith('blass_r5_')] + neu_g
        bl, r8z, fu = mass(p2)
        log('blass_r5: %s Schub %s -> Rumpf (W)-rechts %.1f, Flecken-Ueberschuss %.2f' % (neu[0], neu[1], r8z, fu))
    if sf:
        par = p2
    return par


def weiss_rest(blob, nur=None):
    """Riegel: fast weisse Texel unter Flaechen von Mesh 8 auf neuen Zeilen (nur = Schluesselmenge, z. B. Brust)."""
    _d, _o, ms, _t = KB.teile(blob)
    return {k: n for k, uv in A4.keys(ms[8]) for n in [weiss_texel(uv)] if n and (nur is None or k in nur)}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--kette', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--start', default=os.path.join(HIER, 'adern_r4_ergebnis.json'), help='Ergebnis Runde 4 (modus original_r4, = adern_v8.json in 282e4d594)')
    ap.add_argument('--aus', default=os.path.join(HIER, 'adern_v8.json'))
    ap.add_argument('--stufen', default='srw')
    ap.add_argument('--nur-fein', help='Feinwahl auf einem frueheren Ergebnis (modus original_r5) wiederholen')
    ap.add_argument('--nur-blass', help='nur Schritt blass_r5 auf einem frueheren Ergebnis (modus original_r5)')
    a = ap.parse_args()
    import v8_bau as VB
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    geo, _p = VB.bauen(a.basis, a.stufen, kette=json.load(open(a.kette)), ruecken=json.load(open(a.ruecken)))
    if a.nur_blass:
        par = json.load(open(a.nur_blass))
        assert par['modus'] == 'original_r5', par['modus']
        par = blass_r5(geo, par, log=lambda *x: print(*x, flush=True))
        json.dump(par, open(a.aus, 'w'), indent=1)
        return
    if a.nur_fein:
        par = json.load(open(a.nur_fein))
        assert par['modus'] == 'original_r5', par['modus']
        par = feinwahl(geo, par, log=lambda *x: print(*x, flush=True))
        json.dump(par, open(a.aus, 'w'), indent=1)
        return
    par4 = json.load(open(a.start))
    assert par4['modus'] == 'original_r4', par4['modus']
    par = suche(geo, par4, log=lambda *x: print(*x, flush=True))
    par = feinwahl(geo, par, log=lambda *x: print(*x, flush=True))
    par = blass_r5(geo, par, log=lambda *x: print(*x, flush=True))
    par['quelle'] = par4['quelle'] + ('; Runde 5 (adern_r5.py): Brust 488 -> Original-Zeile 481 mit eigener UV, von vorn sichtbare fast weisse '
                                      'Texel durch Schuebe auf Original-Haut ohne Weiss ersetzt')
    json.dump(par, open(a.aus, 'w'), indent=1)
    print({k: len(v) for k, v in par['flaechen'].items()}, [(g['name'], len(g['flaechen']), g['schub']) for g in par['gruppen']])


if __name__ == '__main__':
    main()
