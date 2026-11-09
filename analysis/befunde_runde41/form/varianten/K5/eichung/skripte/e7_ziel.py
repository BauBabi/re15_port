"""E7: Zielfarben je Material aus e5 (Hauptmodell je Szene) und Vergleich mit K5c, K5e_f1, K5f (dieselbe Kette).
Ziel-Lab = pixelgewichtetes Mittel der Lab-Mediane beider Szenen (nur Szenen mit >= 40 gueltigen Pixeln).
Unsicherheit dE = max(halber Szenenabstand, Modellabstand Haupt/Zweitmodell, halbe L*-Spanne p10..p90 / 2).
Vergleich je Stand: Kettenwert (Stand vorwaerts durch das Modell und zurueck, gleiche Pixel) gegen den Videowert, je Szene und gewichtet:
dE76, dL*, dC*, dh (Grad). Ausgang daten/e7_ziel.json."""
import json
import math
import numpy as np
import ek

NAMEN = ['Haut', 'Gelenk', 'Schwanzspitze', 'Buckel', 'Krallen', 'Kopf', 'Lippe/Unterkiefer', 'Hals', 'F1', 'Sonder']
STAENDE = ('K5c', 'K5e_f1', 'K5f')
MIN = 40
J = json.load(open(ek.DATEN + '/e5_wesen.json', encoding='utf-8'))


def lch(lab):
    L, a, b = lab
    return L, math.hypot(a, b), math.degrees(math.atan2(b, a))


def dh(h1, h2):
    return (h1 - h2 + 180) % 360 - 180


aus = dict(regel=__doc__, materialien={})
for nm in NAMEN:
    z = dict(staende={})
    labs, ws = [], []
    unsich = []
    for n in (1, 2):
        e = J['bild%d' % n]
        mods = list(e['modelle'])
        h = e['modelle'][mods[0]]['video'].get(nm, {})
        zw = e['modelle'][mods[1]]['video'].get(nm, {})
        if h.get('pixel', 0) >= MIN:
            q = dict(h)
            q['modell'] = mods[0]
            if zw.get('pixel', 0) >= MIN:
                q['zweitmodell'] = dict(name=mods[1], lab_median=zw['lab_median'],
                                        dE76_zum_hauptmodell=round(float(np.linalg.norm(np.subtract(zw['lab_median'], h['lab_median']))), 2))
                unsich.append(q['zweitmodell']['dE76_zum_hauptmodell'])
            unsich.append((h['L_p10_p90'][1] - h['L_p10_p90'][0]) / 4.0)
            q['schattierung_median'] = e.get('schattierung_median_je_material', {}).get(nm)
            z['bild%d' % n] = q
            labs.append(np.array(h['lab_median']))
            ws.append(h['pixel'])
        else:
            z['bild%d' % n] = dict(pixel=h.get('pixel', 0), hinweis='weniger als %d gueltige Pixel' % MIN)
    if labs:
        w = np.array(ws, float) / sum(ws)
        lab = (np.array(labs) * w[:, None]).sum(0)
        if len(labs) == 2:
            unsich.append(float(np.linalg.norm(labs[0] - labs[1])) / 2)
        L, C, hh = lch(lab)
        z['ziel'] = dict(lab=[round(float(x), 2) for x in lab], L=round(L, 2), C=round(C, 2), h=round(hh, 1),
                         rgb_textur=[round(float(x), 1) for x in ek.rgb_von_lab(lab)],
                         unsicherheit_dE=round(float(max(unsich)), 2), szenen=[n for n in (1, 2) if 'median' in z['bild%d' % n]])
    for st in STAENDE:
        werte = []
        for n in (1, 2):
            e = J['bild%d' % n]
            mod = list(e['modelle'])[0]
            v = e['modelle'][mod]['video'].get(nm, {})
            k = e['modelle'][mod]['staende'][st]['kette'].get(nm, {})
            t = e['modelle'][mod]['staende'][st]['textur_sichtbar'].get(nm, {})
            if v.get('pixel', 0) >= MIN and k.get('pixel', 0) >= MIN:
                Lv, Cv, hv = lch(v['lab_median'])
                Lk, Ck, hk = lch(k['lab_median'])
                werte.append((k['pixel'], k['lab_median'], dict(bild=n, kette_lab=k['lab_median'], textur_lab=t.get('lab_median'),
                                                               dE76=round(float(np.linalg.norm(np.subtract(k['lab_median'], v['lab_median']))), 2),
                                                               dL=round(Lk - Lv, 2), dC=round(Ck - Cv, 2), dh=round(dh(hk, hv), 1))))
        if werte:
            w = np.array([x[0] for x in werte], float)
            lab = (np.array([x[1] for x in werte]) * (w / w.sum())[:, None]).sum(0)
            eintrag = dict(je_szene=[x[2] for x in werte], lab=[round(float(x), 2) for x in lab],
                           median=[round(float(x), 1) for x in ek.rgb_von_lab(lab)])
            if 'ziel' in z:
                Lz, Cz, hz = z['ziel']['L'], z['ziel']['C'], z['ziel']['h']
                Ls, Cs, hs = lch(lab)
                eintrag.update(dE76_zum_ziel=round(float(np.linalg.norm(lab - np.array(z['ziel']['lab']))), 2), dL=round(Ls - Lz, 2),
                               dC=round(Cs - Cz, 2), dh=round(dh(hs, hz), 1),
                               ausserhalb_unsicherheit=bool(np.linalg.norm(lab - np.array(z['ziel']['lab'])) > z['ziel']['unsicherheit_dE']))
            z['staende'][st] = eintrag
    aus['materialien'][nm] = z
    zi = z.get('ziel')
    print(nm, zi and (zi['lab'], zi['unsicherheit_dE'], zi['szenen']),
          {st: (v.get('dE76_zum_ziel'), v.get('dL'), v.get('dC'), v.get('dh')) for st, v in z['staende'].items()})
json.dump(aus, open(ek.DATEN + '/e7_ziel.json', 'w'), indent=1, ensure_ascii=False)
