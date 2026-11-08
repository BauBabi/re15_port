#!/usr/bin/env python3
"""Messblatt RUHE Teil 3: Machbarkeits-Vorschau des Materialplans K5e, NUR IM SPEICHER (kein Asset, keine Datei-Variante).
Geometrie = K5d (Krallen 19/20/28/29 eingezogen), UV/Seiten/TIM-Indizes = Spiel-Asset K5c, CLUT-Zeilen = Materialplan (10 Zeilen).
Zweck: (1) den EINEN Helligkeitsfaktor f der Haut bestimmen (Bild 2 mit Videofilter >= Vorlage), (2) zeigen, dass der Plan die Ruhe-Riegel
und die Wunsch-Riegel erreichen kann, mit denselben Messfunktionen wie K5c/K5d.
    python ruhe_vorschau.py            -> messung/vorschau.json, Kacheln v1-v3
"""
import copy
import json
import math
import os
import sys

import cv2
import numpy as np

import rl
import ruhe_mass as M
import ruhe_material as RM
import ruhe_lauf as RL

sys.path.insert(0, rl.K5 + '/generator')
import kf_g4 as KG  # noqa: E402
import riegel_k5d as RK  # noqa: E402

# ---------------- Parameter (je Material EIN Satz; Quelle je Wert im Messblatt RUHE 3)
PAR = dict(
    haut=dict(h=-26.3, C=11.68, q=0.74, kappa=0.61),        # h, C: E30-Hauptfamilie (material.json haut_anker_e30); q, kappa: Median der E30-Gruppen
    gelenk=dict(h=-26.3, C=4.8, dL=9.1),     # h = Haut (weisslich = hell + unbunt, gleiche Familie; mit -69 Grad (K4-Gelenkflecken) waren die Gelenke
                                             # in Bild 1 farbfremde Flecken: F_fremd 11,3 / 10k, Lauf 1 der Vorschau); C: K4-Albedo an den Gelenkflecken
                                             # ("vorher da", Messblatt V4F 1.1: a* 1,7, b* -4,5 -> C* 4,8);
                                             # dL: K5d Gelenk- gegen Nachbar-Albedo 28,5 - 19,4 (Nutzer zu K5d "ja, besser"; ruhe_wunsch gemessen)
    fleisch=dict(h=-8.0, C=18.0, dL=6.9),    # h: F1-Leuchtstelle (E30, Lab 48,6/21,6/-3,0); C: Mittel K4 Schwanzspitze/Buckel F02 (11,6 / 24,9);
                                             # dL: K4 Schwanzspitze gegen Schwanz 20,5 - 13,6 ("vorher da", ruhe_wunsch/K4 gemessen)
)
ZEILEN = dict(g4_0=0, haut=(1, 2, 3), gelenk=(4, 5, 6), fleisch=7, f1=8, zahn=9)


def gew_quantil(x, w, q):
    o = np.argsort(x)
    cs = np.cumsum(w[o])
    return float(np.interp(q, (cs - 0.5 * w[o]) / cs[-1], x[o]))


def rampe(g4zeile, cnt, h, C, Lfun, q, kappa):
    """EINE Transformation auf eine G4-Originalzeile: Haut-Eintraege L' = Lfun(L), Farbton h, Buntheit C (weich an den sRGB-Rand);
    helle Eintraege (L* >= R4_L_HELL) rangtreu in die Hautmitte (Haut-Median + 0,1 x (L* - 52,4)); Ader-Eintraege rangtreu auf [Haut-p10, q x Haut-Median], Buntheit kappa x C.
    0x0000 bleibt (transparent). Rangfolge je Klasse bleibt (lineare, steigende Abbildungen)."""
    L, C0, h0, w = KG.r4_eintraege(g4zeile)
    z0 = np.array(g4zeile) != 0
    cnt = np.asarray(cnt, float)
    haut = (w == 0) & (L[:, 0] < KG.R4_L_HELL) & z0
    hell = (w == 0) & (L[:, 0] >= KG.R4_L_HELL) & z0
    ader = (w > 0) & z0
    hg = haut & (cnt > 0)
    Lh = Lfun(L[:, 0])
    p10 = gew_quantil(Lh[hg], cnt[hg], 0.10)
    p50 = gew_quantil(Lh[hg], cnt[hg], 0.50)
    p90 = gew_quantil(Lh[hg], cnt[hg], 0.90)
    Ln = np.where(haut, Lh, 0.0)
    if hell.any():
        lh = L[hell, 0]
        Ln[hell] = p50 + 0.1 * (lh - KG.R4_L_HELL)     # helle G4-Eintraege (Horn/Spitzen, auf K5c-UV verstreut) in die Hautmitte, Rang in der Klasse bleibt
    ag = ader & (cnt > 0)
    if ag.any():
        v0, v1 = float(L[ag, 0].min()), float(L[ag, 0].max())
        lo, hi = p10, q * p50
        Ln[ader] = np.clip(lo + (L[ader, 0] - v0) / max(v1 - v0, 1e-6) * (hi - lo), min(lo, hi), hi)
    Cw = np.where(ader, kappa * C, C)
    cm = KG.r4_cmax(Ln, h)
    Cn = np.where(cm > 1e-6, cm * np.tanh(Cw / np.maximum(cm, 1e-6)), 0.0)
    hr = math.radians(h)
    rgb = KG.rgb_aus_lab(np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1))
    neu = list(g4zeile)
    for i in range(256):
        if not z0[i]:
            continue
        wn = KG.wort(rgb[i]) | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000
    return neu, dict(haut_p10_p50_p90=[round(p10, 2), round(p50, 2), round(p90, 2)], ader=int(ag.sum()), haut=int(hg.sum()), hell=int((hell & (cnt > 0)).sum()))


class Tex:
    def __init__(self, px, clut_y, worte):
        self.px, self.clut_y = px, clut_y
        self.h, self.w = px.shape
        c = np.array(worte, np.int64)
        self.lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3, (c != 0) * 255], -1).astype(np.uint8)


def material_je_flaeche():
    mj = json.load(open(os.path.join(rl.OUT, 'material.json'), encoding='utf-8'))
    m = {}
    for nm in ('Haut', 'Gelenk', 'Fleisch_Schwanzspitze', 'Fleisch_Buckel', 'Zahn', 'Auge', 'Maulinneres', 'F1'):
        for s in mj['material'][nm]['liste']:
            m[tuple(int(x) for x in s.split('|'))] = nm
    return m, mj


MODUS = 'faktor'   # 'faktor': Haut L' = f x L (EIN globaler Faktor, Auftrag); 'hebung': L' = L + f (G4-Kontrast 1:1, Alternative)


def hl(f, L):
    return f * L if MODUS == 'faktor' else L + f


def bauen(f):
    """Vorschau-Modell im Speicher (Geometrie K5d, UV K5c) und Zeilen nach Plan mit Hautfaktor f."""
    F, form, tim = RM.flaechen(rl.BASIS)
    _rgb, cl = RM.zeilen_rgb(tim)
    mat, mj = material_je_flaeche()
    basis16 = {int(k): v['g4_quellzeile'] for k, v in mj['zeilen_k5c'].items()}
    # Nutzung je Material und Quellseite (Texelzugriffe je Index), Quellseite = Seite (0-2) bzw. G4-Quellzeile der Sonderseite 3
    nutz = {}
    zeile_je = {}
    for k, d in F.items():
        nm = mat[k]
        s = d['seite'] if d['seite'] < 3 else basis16[d['zeile']]
        if nm == 'Haut':
            z = ZEILEN['haut'][s]
            gr = ('haut', s)
        elif nm == 'Gelenk':
            z = ZEILEN['gelenk'][s]
            gr = ('gelenk', s)
        elif nm.startswith('Fleisch'):
            assert s == 1, (k, s)
            z = ZEILEN['fleisch']
            gr = ('fleisch', 1)
        elif nm in ('Auge', 'Maulinneres'):
            z, gr = ZEILEN['g4_0'], None
        elif nm == 'Zahn':
            z, gr = ZEILEN['zahn'], None
        else:
            z, gr = ZEILEN['f1'], None
        zeile_je[k] = z
        if gr:
            n = nutz.setdefault(gr, np.zeros(256))
            np.add.at(n, d['idx'], 1)
    worte = [list(cl[0])] + [None] * 9
    info = {}
    for s in range(3):
        g4z = [int(x) for x in cl[s]]
        if ('haut', s) in nutz:
            worte[ZEILEN['haut'][s]], info['haut%d' % s] = rampe(g4z, nutz[('haut', s)], PAR['haut']['h'], PAR['haut']['C'], lambda L: hl(f, L),
                                                                 PAR['haut']['q'], PAR['haut']['kappa'])
        if ('gelenk', s) in nutz:
            Lr, _c, _h, w = KG.r4_eintraege(g4z)
            n = nutz[('gelenk', s)]
            sel = (w == 0) & (Lr[:, 0] < KG.R4_L_HELL) & (n > 0) & (np.array(g4z) != 0)
            g = PAR['gelenk']['dL']        # Hautabbildung f x L* plus dL (G4-Kontrast x f bleibt, nur gehoben)
            worte[ZEILEN['gelenk'][s]], info['gelenk%d' % s] = rampe(g4z, n, PAR['gelenk']['h'], PAR['gelenk']['C'], lambda L, g=g: hl(f, L) + g,
                                                                     PAR['haut']['q'], PAR['haut']['kappa'])
            info['gelenk%d' % s]['dL'] = round(g, 3)
    g4z = [int(x) for x in cl[1]]
    Lr, _c, _h, w = KG.r4_eintraege(g4z)
    n = nutz[('fleisch', 1)]
    sel = (w == 0) & (Lr[:, 0] < KG.R4_L_HELL) & (n > 0) & (np.array(g4z) != 0)
    g = PAR['fleisch']['dL']
    worte[ZEILEN['fleisch']], info['fleisch1'] = rampe(g4z, n, PAR['fleisch']['h'], PAR['fleisch']['C'], lambda L: hl(f, L) + g, PAR['haut']['q'],
                                                       PAR['haut']['kappa'])
    info['fleisch1']['dL'] = round(g, 3)
    worte[ZEILEN['f1']] = list(cl[5])
    worte[ZEILEN['zahn']] = list(cl[7])
    for i in range(3):
        if worte[ZEILEN['haut'][i]] is None:
            worte[ZEILEN['haut'][i]] = list(cl[0])
        if worte[ZEILEN['gelenk'][i]] is None:
            worte[ZEILEN['gelenk'][i]] = list(cl[0])
    mo = rl.fl.lade_var(rl.K5D)
    k5c = rl.fl.lade_var(rl.BASIS)
    for b, (m, mc) in enumerate(zip(mo.meshes, k5c.meshes)):
        for art, liste in ((0, 'tuv'), (1, 'quv')):
            neu = []
            for fi, uv in enumerate(getattr(mc, liste)):
                z = zeile_je[(b, art, fi)]
                u = list(uv)
                u[2] = (((tim['clut_y'] + z) & 0x1FF) << 6) | (u[2] & 0x3F)
                neu.append(tuple(u))
            setattr(m, liste, neu)
    mo.T = Tex(np.array(tim['pixel']), tim['clut_y'], worte)
    return mo, worte, info, zeile_je


def szene_vorschau(mo, n):
    k = rl.v4f.kand(rl.K5D)
    lage, gier, clip, bild = k.stand(n)
    hg = rl.P.lade_rgb(rl.P.daten('hintergrund_bild%d.png' % n))
    kam = rl.P.kamera(n)
    licht, _k = rl.P.licht_fuer(n, k.licht)
    s = hg.copy()
    f0, kk = mo.clips[clip]
    kf = mo.frames[f0 + min(bild, kk - 1)] & 0xFFF
    K = rl.v4f.rastern_id(rl.v4f.dreiecke_id(mo, kf, lage, gier, kam, licht), mo.T, s, kam.abbildung)
    return s, K


_VOR = {}


def hell_bild(mo, n, fig):
    s, K = szene_vorschau(mo, n)
    vids = [rl.v4f.video(s, n, sa) for sa in rl.v4f.SAATEN]
    lv = float(np.mean([RK.labm(v)[fig][:, 0].mean() for v in vids]))
    lt = float(np.mean([RK.labm(rl.v4f.PZ.rueck(v, n)[0])[fig][:, 0].mean() for v in vids]))
    ls = float(RK.labm(s)[fig][:, 0].mean())
    return dict(video=round(lv, 2), T=round(lt, 2), ohne=round(ls, 2)), s, K, vids


def main():
    vor = RK.Vorlage()
    fig = {}
    for n in (1, 2):
        _s, Kc = rl.v4f.szene_id(rl.BASIS, n)
        fig[n] = RK.nutzer_union(n) & Kc['maske'] & ~vor.d[n]['clip']
    ziel = {n: dict(video=round(float(vor.d[n]['LV'][fig[n]][:, 0].mean()), 2), T=round(float(vor.d[n]['LT'][fig[n]][:, 0].mean()), 2),
                    ohne=round(float(vor.d[n]['LS'][fig[n]][:, 0].mean()), 2), px=int(fig[n].sum())) for n in (1, 2)}
    print('Vorlage', ziel, flush=True)
    # Gegenprobe der Pixelmenge: Asset K5c muss die Messblatt-V4F-Werte geben (Bild 2 Video 10,9, Vorlage 15,4)
    st = {}
    for nm, o in (('K5c', rl.BASIS), ('K5d', rl.K5D)):
        st[nm] = {}
        for n in (1, 2):
            s, _K = rl.v4f.szene_id(o, n)
            vids = [rl.v4f.video(s, n, sa) for sa in rl.v4f.SAATEN]
            st[nm][n] = dict(video=round(float(np.mean([RK.labm(v)[fig[n]][:, 0].mean() for v in vids])), 2), ohne=round(float(RK.labm(s)[fig[n]][:, 0].mean()), 2))
    print('Staende', st, flush=True)
    tabelle = []
    for f in (1.0, 1.25, 1.5, 2.0, 2.5):
        mo, _w, _i, _z = bauen(f)
        tabelle.append(dict(f=f, bild1=hell_bild(mo, 1, fig[1])[0], bild2=hell_bild(mo, 2, fig[2])[0]))
        print('tabelle', tabelle[-1], flush=True)
    # Bisektion f (Haut): Bild 2 Video >= Vorlage
    lauf = []
    lo, hi = 1.0, 3.0
    for _ in range(9):
        f = 0.5 * (lo + hi)
        mo, _w, _i, _z = bauen(f)
        h2, *_r = hell_bild(mo, 2, fig[2])
        lauf.append(dict(f=round(f, 4), bild2=h2))
        print('f', f, h2, flush=True)
        if h2['video'] >= ziel[2]['video']:
            hi = f
        else:
            lo = f
    f = round(hi, 3)
    mo, worte, info, zeile_je = bauen(f)
    erg = dict(par=PAR, zeilen=ZEILEN, f=f, tabelle=tabelle, bisektion=lauf, ziel_vorlage=ziel, staende=st, rampen=info)
    import ruhe_wunsch as W
    mt = W.mat()
    F, _form, _tim = RM.flaechen(rl.BASIS)
    erg['wuensche'] = {'K5e-Plan': W.wuensche(W.albedo(F, zeile_je, worte), mo, 2, mt)}
    for nm, o in (('K5c', rl.BASIS), ('K5d', rl.K5D)):
        A, _F = W.stand_albedo(o)
        erg['wuensche'][nm] = W.wuensche(A, rl.fl.lade_var(o), 2, mt)
    print('wuensche', json.dumps(erg['wuensche'], ensure_ascii=False)[:3000], flush=True)
    erg['zeilen_worte'] = [[int(x) for x in z] for z in worte]
    for n in (1, 2):
        h, s, K, vids = hell_bild(mo, n, fig[n])
        erg.setdefault('helligkeit', {})['bild%d' % n] = h
        m = rl.haut('K5e', K)
        erg.setdefault('szene', {})['bild%d' % n] = dict(ohne_filter=M.masse(s, m, 2), video=M.mittel([M.masse(v, m, 3) for v in vids]))
        zs = [RL.kachel(rl.v4f.original(n), 'Vorlage Bild %d (Video-Standbild)' % n)]
        zv = [RL.kachel(rl.v4f.original(n), 'Vorlage Bild %d' % n)]
        for nm, o in (('K5c', rl.BASIS), ('K5d', rl.K5D)):
            s_, _K = rl.v4f.szene_id(o, n)
            zs.append(RL.kachel(s_, '%s ohne Filter' % nm))
            zv.append(RL.kachel(rl.v4f.video(s_, n, 15), '%s Videofilter Saat 15' % nm))
        zs.append(RL.kachel(s, 'K5e-Plan (Vorschau, f %.3f) ohne Filter' % f, RL.zeilen_text(erg['szene']['bild%d' % n]['ohne_filter'])))
        zv.append(RL.kachel(vids[0], 'K5e-Plan Videofilter Saat 15', ['Figur L* Video %.1f (Vorlage %.1f)' % (h['video'], ziel[n]['video'])]))
        RL.speichern('v%d_plan_szene_bild%d.png' % (1 + n, n), RL.stapel([RL.reihe(zs), RL.reihe(zv)],
                     'V%d Materialplan-Vorschau (KEIN Asset) Bild %d: Vorlage | K5c | K5d | K5e-Plan, gleiche Kette, roh' % (1 + n, n)))
    # weisses Licht: Ruhe-Masse + Wunsch-Masse
    ws = []
    kach = []
    for wn, az, el in rl.WINKEL4:
        img, K = rl.weiss_id(mo, 2, az, el)
        m, karte, v = M.masse(img, K['maske'], 2, True)
        ws.append(m)
        erg.setdefault('weiss', {})[wn] = m
        kach.append((img, karte, v, K, wn, az, el))
    erg['weiss']['MITTEL'] = M.mittel(ws)
    acc = np.zeros(24)
    for d in ws:
        for s_, p in d['sektoren'].items():
            acc[int(s_) // 15] += p * d['px'] * d['bunt_pct'] / 1e4
    acc /= acc.sum()
    erg['weiss']['S5_gepoolt'] = int((acc >= 0.05).sum())
    erg['weiss']['hist_gepoolt'] = {str(i * 15): round(100 * float(acc[i]), 1) for i in range(24) if acc[i] >= 0.005}
    json.dump(erg, open(os.path.join(rl.OUT, 'vorschau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    np.save(os.path.join(rl.SCR, 'vorschau_worte.npy'), np.array(worte, np.int64))
    # Kacheln: v1 weiss rundum K5e-Vorschau neben G4 / K5c / K5d (gleiches Licht)
    mod = {k: (mo_, c) for k, _t, mo_, c, _o in rl.modelle()}
    zeilen = []
    for img, karte, v, K, wn, az, el in kach:
        ks = []
        for key in ('G4', 'EM30', 'K5c', 'K5d'):
            a, KK = rl.weiss_id(mod[key][0], mod[key][1], az, el)
            mm = M.masse(a, rl.haut(key, KK), 2)
            ks.append(RL.kachel(a, '%s, %s' % (key, wn), RL.zeilen_text(mm)))
        ks.append(RL.kachel(img, 'K5e-PLAN (Vorschau im Speicher, f %.3f), %s' % (f, wn), RL.zeilen_text(erg['weiss'][wn])))
        zeilen.append(RL.reihe(ks))
    RL.speichern('v1_plan_weiss_rundum.png', RL.stapel(zeilen, 'V1 Materialplan K5e als Vorschau im Speicher (KEIN Asset): G4 | EM30 | K5c | K5d | K5e-Plan, '
                                                         'weisses Licht wie Tafel 07, Ruhe-Clip, roh.'))
    print(json.dumps({k: erg[k] for k in ('f', 'helligkeit', 'ziel_vorlage')}, ensure_ascii=False))
    print('fertig')


if __name__ == '__main__':
    main()
