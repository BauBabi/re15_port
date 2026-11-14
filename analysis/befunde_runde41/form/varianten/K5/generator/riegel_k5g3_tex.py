#!/usr/bin/env python3
"""Textur-Riegel K5g3 (Auftrag K5g3 Textur-Bauer: E3, V1-V6, O3-O5) - Erweiterung von riegel_k5g3.py (dort: Geometrie-Stufe R1-R9).

    python riegel_k5g3.py textur [<Variante>] [--spiel <spiel.json>]   (Vorgabe Variante K5/k5g/K5g3; Protokoll <Variante>/riegel_k5g3_textur.txt/.json)

Jede Zeile mit 'Riegel' hat eine Gegenprobe, die ROT sein MUSS (K5e_f1 = Spiel-Asset, K5f = k5f/K5f, K5g2-Runde-1-Kopie mit weisser Hand und dunklen
Augen, oder ein im Riegel gebauter Gegenstand). Ist eine Gegenprobe gruen, zaehlt das wie Rot (Exit 1). 'Info' blockiert nicht.
Wiederverwendet: riegel_k5g2.py (Stand, v3, v4, v3_fleisch, v5, v1, v6, h1, weiss - unveraenderte Definitionen aus dem K5g2-Auftrag).

 TG   Geometrie des Assets == Geometrie-Stufe kf_k5g3_geo (Ecken, Normalen, Flaechen-Ecken aller Netze; Sektionen 0-6)    | K5e_f1
 TB   Generator: zweiter Lauf byte-gleich (kf_k5g3.pruef)                                                                    | Bau mit s + 0,01
 V3.1 Fleck-Material genau auf den Traegertexeln, keine andere Flaeche liest eine Fleck-Zeile (Pruef-Stand)              | ohne eigene Texel
 V3.1b Asset: jedes Spalt-Kind liest die Zeile/Seite seiner Mutter und bleibt im Texturstueck der Mutter; kein Kind einer  | K5e_f1-Zeilen
      Nicht-Traeger-Mutter liest eine Fleck-Zeile
 V3.2 Texel-Konflikte zwischen Zonen (Info)
 V3.3 Buckel-Fleisch (Zone 27) L* 33-39, C* 22-26                                                                        | K5e_f1 Buckel
 V4.1/2/3 Zone 45, Hals 21, Lippe, Unterkiefer, Hals-Unterseite: nicht rot, nicht schwarz; Maul-Zeilen nur Zone 30       | K5e_f1 / K5f
 V4.4 keine Flaeche ausser Zone 30 liest 0x8000 oder G4-Mundfleisch (Asset)                                              | K5e_f1
 V2.1 Hautton (Albedo, C-gewichtet) im Band der Originale [14, 27] Grad und waermer als K5e_f1/K5f                         | K5e_f1 / K5f
 V2.2 Haut-Buntheit Median (Info; O3: C* 2-6 ist keine Grenze)
 V5   Krallen: kein 0x7FFF, keine roten Texel                                                                              | K5f
 H1   Haende 13/18 (O5): kein 0x7FFF, Median im Band der Haut                                                             | K5g2 r1
 S1   kein Sattel (O3): Median L* der Teile 0/7 im Band P25-P75 der Haut                                                   | Bau ohne Seitenangleich
 V1   Adern multiplikativ                                                                                                  | K5f
 V6   F1-Kern bytegleich K5c-Zeile 5, kein schwarzer Ring                                                                  | K5e
 A1   Augen (O4): Bild 2 an beiden Nutzerpunkten L*-max >= 2 x Kopfhaut daneben                                          | K5e_f1
 W1-W4, R1-R4 weisses Licht im Band der Originale, Ruhe                                                                     | K5e_f1
 Z1-Z9 Spiel 5080 im Zielband der Original-Gegner (bindend, O3)                                                            | K5e_f1 (eigener Lauf)
 T1   Zeilen <= 16, Port-Texturhoehe <= 4096 | T5 jede Flaeche liest eine gueltige Zeile | G5 Spiel-Asset = K5e_f1
"""
import hashlib
import json
import os
import shutil
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import numpy as np  # noqa: E402
import riegel_k5g2 as R2  # noqa: E402
import kf_k5g3 as K3  # noqa: E402
import kf_k5g3_geo as G3  # noqa: E402
import kf_k5g3_tex as T3  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402
import kf_k5e as E  # noqa: E402

FE = E.FE
K5 = E.K5
K5E, K5F, K5E_ALT = R2.K5E, R2.K5F, R2.K5E_ALT
GEGEN_R1 = R2.GEGEN_R1


def sha_d(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


def stand_mit_namen(ordner, namen=None):
    S = R2.Stand(ordner)
    if namen is None:
        bj = os.path.join(ordner, 'bau.json')
        if not os.path.exists(bj):
            bj = os.path.join(os.path.dirname(ordner), 'bau.json')
        namen = json.load(open(bj, encoding='utf-8'))['textur']['zeilen']
    S.namen = namen
    return S


# ------------------------------------------------------------------ Geometrie / Generator
def tg(R, var):
    a = FE.lese_emd(open(os.path.join(var, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    g = FE.lese_emd(open(G3.GEO3_ZIEL, 'rb').read())
    s_a, s_g = FE.sektionen(open(os.path.join(var, 'BAHNSTEIG_FORM.EMD'), 'rb').read()), FE.sektionen(open(G3.GEO3_ZIEL, 'rb').read())
    sek = all(s_a[k] == s_g[k] for k in ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3'))
    ok = K3.geo_schluessel(a) == K3.geo_schluessel(g) and sek
    e = FE.lese_emd(open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    gg = K3.geo_schluessel(e) == K3.geo_schluessel(g)
    R.zeile('TG', 'Geometrie des Assets == Geometrie-Stufe kf_k5g3_geo (geo/BAHNSTEIG_FORM.EMD sha %s; Ecken, Normalen, Flaechen-Ecken; '
            'Sektionen 0-6)' % sha_d(G3.GEO3_ZIEL)[:12], 'gleich' if ok else 'VERSCHIEDEN', ok, gegen=('K5e_f1', 'gleich' if gg else 'verschieden', gg))


def tb(R, var):
    par = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))['par']
    d = os.path.join(K3.SCR, 'riegel_tb')
    shutil.rmtree(d, ignore_errors=True)
    K3.bauen(par, d, log=lambda *a: None)
    ab = [n for n in K3.DATEIEN + ('bau.json',) if open(os.path.join(var, n), 'rb').read() != open(os.path.join(d, n), 'rb').read()]
    p2 = json.loads(json.dumps(par))
    p2['textur']['haut']['s'] = float(p2['textur']['haut']['s']) + 0.01
    d2 = d + '_gegen'
    shutil.rmtree(d2, ignore_errors=True)
    K3.bauen(p2, d2, log=lambda *a: None)
    g = open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read() == open(os.path.join(d2, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    R.zeile('TB', 'Generator kf_k5g3.py: neuer Lauf mit par aus bau.json byte-gleich (alle Dateien + bau.json)', 'abweichend: %s' % ab, not ab,
            gegen=('Bau mit s + 0,01', 'TIM gleich' if g else 'TIM verschieden', g))


# ------------------------------------------------------------------ V3.1b Spalt-Kinder im Asset
def v31b(R, var, bau):
    tab = json.load(open(G3.GEO3_SPALT, encoding='utf-8'))
    P = FE.lese_emd(open(os.path.join(var, 'pruef', 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    A = FE.lese_emd(open(os.path.join(var, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    art = ('dreiecke', 'vierecke')
    namen = bau['textur']['zeilen']
    cy = FE.tim_lesen(open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read())['clut_y']
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    traeger = set(TX.schluessel(k) for nr in tj if nr in TX.FLECK_ART for k in tj[nr])

    def gx(f):
        return [(int(u) + (int(f['page']) & 15) * 128, int(v)) for u, v in f['uv']]

    def pruefen(P_, A_):
        falsch, ausser, fremd = 0, 0, 0
        for t, a, i, mt, ma, mi in tab['herkunft']:
            k, m = A_['meshes'][t][art[a]][i], P_['meshes'][mt][art[ma]][mi]
            if int(k['clut']) != int(m['clut']) or (int(k['page']) & 15) != (int(m['page']) & 15):
                falsch += 1
            mxy = gx(m)
            x0, x1 = min(x for x, _ in mxy) - 1, max(x for x, _ in mxy) + 1
            y0, y1 = min(y for _, y in mxy) - 1, max(y for _, y in mxy) + 1
            if any(not (x0 <= x <= x1 and y0 <= y <= y1) for x, y in gx(k)):
                ausser += 1
            z = ((int(k['clut']) >> 6) & 0x1FF) - cy
            if 0 <= z < len(namen) and namen[z].startswith('fleck_') and (mt, ma, mi) not in traeger:
                fremd += 1
        return falsch, ausser, fremd
    f1, a1, r1 = pruefen(P, A)
    Ae = FE.lese_emd(open(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    # Gegenprobe: Spalt-Netze aus der Tabelle mit den K5e_f1-Zeilen (= ohne Erbe der Textur) gegen den Pruef-Stand
    Ag = json.loads(json.dumps(A))
    for t in (10, 16, 21, 22):
        Ag['meshes'][t] = tab['teile'][str(t)]
    del Ae
    f2, a2, r2 = pruefen(P, Ag)
    ok = f1 == 0 and a1 == 0 and r1 == 0
    R.zeile('V3.1b', 'Asset: %d Spalt-Kinder lesen Zeile/Seite der Mutter, UV im Texturstueck der Mutter, keine Fleck-Zeile ohne Traeger-Mutter'
            % len(tab['herkunft']), 'falsche Zeile/Seite %d, ausserhalb %d, fremde Fleck-Leser %d' % (f1, a1, r1), ok,
            gegen=('Tabellen-Netze ohne Erbe (K5e_f1-Zeilen)', 'falsch %d, ausserhalb %d, fremd %d' % (f2, a2, r2), f2 == 0 and a2 == 0 and r2 == 0))


# ------------------------------------------------------------------ V4.4 0x8000 / Mundfleisch
def v44(R, var, bau):
    zl = R2.zonen_listen(TX.lade_zonen())
    z30 = zl['30']
    tab = json.load(open(G3.GEO3_SPALT, encoding='utf-8'))
    mutter = {(t, a, i): (mt, ma, mi) for t, a, i, mt, ma, mi in tab['herkunft']}
    def zaehlen(ordner, ist_asset):
        S = R2.Stand(ordner)
        if ist_asset:
            nam = json.load(open(os.path.join(ordner, 'bau.json'), encoding='utf-8'))['textur']['zeilen']
            maul_z = set(i for i, n in enumerate(nam) if n.startswith('maul'))
        else:
            maul_z = {0}                    # K5e_f1: Zeile 0 = G4-Zeile 0 (Mundfleisch, Auge)
        n8, nm, nf = 0, 0, 0
        for k, f in S.fl.items():
            mk = mutter.get(k, k) if ist_asset and k[0] in (10, 16, 21, 22) else k
            if mk in z30:
                continue
            _y, _x, idx = S.texel(k)
            z = S.zeile[k]
            if not 0 <= z < len(S.cl):
                continue
            w = np.array(S.cl[z])[idx]
            a8 = int((w == 0x8000).sum())
            am = len(idx) if z in maul_z else 0
            n8 += a8
            nm += am
            nf += 1 if (a8 or am) else 0
        return n8, nm, nf
    a = zaehlen(var, True)
    g = zaehlen(K5E, False)
    R.zeile('V4.4', 'keine Flaeche ausser Zone 30 liest 0x8000 (deckendes Schwarz) oder eine Mundfleisch-Zeile (Asset, Spalt-Kinder nach Mutter)',
            '0x8000 %d, Mundfleisch-Zeile %d Texelzugriffe, %d Flaechen' % a, a == (0, 0, 0),
            gegen=('K5e_f1', '0x8000 %d, Zeile 0 %d, %d Flaechen' % g, g == (0, 0, 0)))


# ------------------------------------------------------------------ eigene Fassungen (K5g3) der K5g2-Riegel, wo deren Definition nicht passt
def je_mit_kern(S, plan):
    """Fleck-Texel je Traeger wie der Generator: kf_k5g2_tex.fleck_zuordnung + F1-Kern-Vorrang (V6)."""
    je, _k = TX.fleck_zuordnung(TX.lade_zonen(), S.form, S.T['pixel'].shape)
    kern = E.f1_masken()['kern']
    for k in list(je):
        if plan[k][0] == 'f1':
            for xy in [xy for xy in je[k] if kern[xy[1], xy[0]]]:
                del je[k][xy]
        if not je[k]:
            del je[k]
    return je


def v3(R, S, bau, plan):
    """= riegel_k5g2.v3, aber mit dem F1-Kern-Vorrang des Generators (Fleck-Texel im F1-Kern bleiben F1, bau.json flecken.f1_kern_vorrang_texel)."""
    namen = S.namen
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    fleck_z = [i for i, n in enumerate(namen) if n.startswith('fleck_')]
    traeger = set(TX.schluessel(k) for nr in tj if nr in TX.FLECK_ART for k in tj[nr])
    fremd = [k for k, z in S.zeile.items() if z in fleck_z and k not in traeger]
    je = je_mit_kern(S, plan)
    kopien = {c['flaeche']: c for c in bau['textur']['kopien']}
    zu_viel, fehlt, n_ok, gleich_wort = 0, 0, 0, 0
    for k, d in je.items():
        z = S.zeile[k]
        if z not in fleck_z:
            fehlt += len(d)
            continue
        c = kopien['%d|%d|%d' % k]
        dx, dy = c['nach'][0] - c['von'][0], c['nach'][1] - c['von'][1]
        ys, xs, idx = S.texel(k)
        basis = namen.index(namen[z].split('_')[1])
        mat = np.array(S.cl[z])[idx] != np.array(S.cl[basis])[idx]
        neu_idx = idx != S.T['pixel'][ys - dy, xs - dx]      # Index in der Kopie != Original-Index an der Herkunftsstelle
        soll = np.array([((int(x) - dx), (int(y) - dy)) in d for y, x in zip(ys, xs)])
        # Material-Texel = eigener (umgelegter) Index; zaehlt auch, wenn das Materialwort zufaellig dem Grundwort gleicht
        mat = mat | (neu_idx & soll)
        gleich_wort += int((soll & ~(np.array(S.cl[z])[idx] != np.array(S.cl[basis])[idx]) & neu_idx).sum())
        zu_viel += int((mat & ~soll).sum()); fehlt += int((~mat & soll).sum()); n_ok += int((mat & soll).sum())
    ok = not fremd and zu_viel == 0 and fehlt == 0
    Sg = R2.Stand(K5E)
    pxg = Sg.T['pixel']
    fremd_g = 0
    for k, d in je.items():
        idx_m = set(int(pxg[y, x]) for (x, y) in d)
        zg = Sg.zeile[k]
        for k2 in Sg.fl:
            if k2 in traeger or Sg.zeile[k2] != zg:
                continue
            _y, _x, i2 = Sg.texel(k2)
            if np.isin(i2, list(idx_m)).any():
                fremd_g += 1
        break
    R.zeile('V3.1', 'Fleck-Material genau auf den Traegertexeln (zonen_v2/traeger_texel, F1-Kern-Vorrang), keine andere Flaeche liest eine Fleck-Zeile',
            'fremde Leser %d, Material ausserhalb der Texel %d, fehlende Fleck-Texel %d, richtige %d (davon %d mit zufaellig gleichem Wort)'
            % (len(fremd), zu_viel, fehlt, n_ok, gleich_wort), ok,
            gegen=('ohne eigene Texel (Material an den Original-Indizes, erste Traegerflaeche)', '%d andere Flaechen lesen es' % fremd_g, fremd_g == 0))
    R.zeile('V3.2', 'Texel-Konflikte zwischen Zonen auf derselben Flaeche (nach Lage aufgeloest: VORRANG 11 vor 12, sonst kleinere Zone)',
            '%s' % bau['textur']['flecken']['konflikte'], True, art='Info')
    return je


def nicht_material(S, keys):
    """Lab und Worte der Texelzugriffe der Flaechen OHNE Fleck-Material-Texel (Fleck-Zeile, Wort != Grundzeile)."""
    L, W = [], []
    for k in keys:
        _y, _x, idx = S.texel(k)
        z = S.zeile[k]
        w = np.array(S.cl[z])[idx]
        m = np.ones(len(idx), bool)
        if S.namen[z].startswith('fleck_'):
            b = S.namen.index(S.namen[z].split('_')[1])
            m = w == np.array(S.cl[b])[idx]
        L.append(S.lab[z][idx[m]]); W.append(w[m])
    return (np.vstack(L), np.concatenate(W)) if L else (np.zeros((0, 3)), np.zeros(0, int))


def roeter(L, s):
    """'roeter als die Haut': C* > s x L* + 6 (die Haut hat C* = s x L*; 6 = Rundung der 15-Bit-Farben, gemessen 2026-11-14: C* - s L* der Haut- und
    Tausch-Zeilen hoechstens +2,4 bei s 0,5225 und bei s 0,3; riegel-Werkzeug scratchpad k5g2/k5g3tex/cs.py)."""
    return np.hypot(L[:, 1], L[:, 2]) > s * L[:, 0] + 6.0


def v4(R, S, plan, s_haut):
    zl = R2.zonen_listen(TX.lade_zonen())
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    lippen = set(TX.schluessel(k) for nm in ('Lippensaum', 'Maul_Band') for k in mj[nm]['liste'])
    unten = (zl['45'] | zl['42'] | zl['31'] | zl['44'] | lippen | set(k for k in S.fl if k[0] == 21)) - zl['30'] - zl['41']

    def werte(St):
        keys = sorted(k for k in unten if k in St.fl)
        L, W = nicht_material(St, keys) if hasattr(St, 'namen') else St.farben(keys)
        r = float(roeter(L, s_haut).mean() * 100) if len(L) else 0.0
        ra = float(R2.rot(L).mean() * 100) if len(L) else 0.0
        rgb = E.KG.wort_rgb([int(w) & 0x7FFF for w in W]) if len(W) else np.zeros((0, 3))
        sw = float(((rgb.max(1) <= 12) & (W != 0)).mean() * 100) if len(W) else 0.0
        return r, ra, sw, float(np.median(L[:, 0])) if len(L) else 0.0, len(W)
    rv, rav, sv, lv, nv = werte(S)
    Sg = R2.Stand(K5E)
    rg, rag, sg, lg, _ = werte(Sg)
    Sf = R2.Stand(K5F)
    rf, _raf, sf, lf, _ = werte(Sf)
    hz = [i for i, n in enumerate(S.namen) if n.startswith('haut')]
    hk = [k for k in S.fl if S.zeile[k] in hz and k not in unten]
    Lh, Wh = S.farben(hk)
    rgbh = E.KG.wort_rgb([int(w) & 0x7FFF for w in Wh])
    sh = float(((rgbh.max(1) <= 12) & (Wh != 0)).mean() * 100)
    p25 = float(np.percentile(Lh[:, 0], 25))
    R.zeile('V4.1', 'Zone 45, Hals 21, Lippe, Unterkiefer, Hals-/Kopf-Unterseite (ohne Fleck-Material, ohne Maul/Zaehne): Texelzugriffe roeter als die Haut '
            '(C* > s L* + 6) = 0 %', '%.2f %% von %d (Info absolute Rotklasse a*>12/C*>15: %.2f %%)' % (rv, nv, rav), rv == 0,
            gegen=('K5e_f1', '%.2f %%' % rg, rg == 0))
    R.zeile('V4.2', 'dieselben Flaechen nicht schwarz: Anteil RGB-Max <= 12 nicht ueber der uebrigen Haut, Median L* >= P25 der Haut',
            '%.2f %% (Haut %.2f %%), Median %.1f (Haut-P25 %.1f)' % (sv, sh, lv, p25), sv <= sh and lv >= p25,
            gegen=('K5f', '%.2f %%, Median %.1f' % (sf, lf), sf <= sh and lf >= p25))
    zl30 = zl['30']
    maul_z = [i for i, n in enumerate(S.namen) if n.startswith('maul')]
    leser = [k for k, z in S.zeile.items() if z in maul_z and k not in zl30]
    g4_0 = [k for k, z in Sg.zeile.items() if z == 0 and k not in zl30]
    R.zeile('V4.3', 'Maul-Zeilen (G4-Mundfleisch) nur von Zone 30 gelesen', '%d andere Flaechen' % len(leser), not leser,
            gegen=('K5e_f1 (Zeile 0)', '%d andere Flaechen' % len(g4_0), not g4_0))


def v5(R, S, s_haut):
    kral = [k for k in S.fl if k[0] in T3.KRALLEN_TEILE]
    hand = [k for k in S.fl if k[0] in T3.HAENDE]

    def w(St):
        L, W = St.farben([k for k in kral if k in St.fl])
        Lh, _ = St.farben([k for k in hand if k in St.fl])
        d = float(np.linalg.norm(np.median(L, 0) - np.median(Lh, 0)))
        return int(((W & 0x7FFF) == 0x7FFF).sum()), float(roeter(L, s_haut).mean() * 100), d, len(W)
    a, b, d, n = w(S)
    ag, bg, dg, _ = w(R2.Stand(os.path.join(K5, 'einbau_k5c')))
    R.zeile('V5', 'Krallen wie die Hand (V5): reinweiss (0x7FFF) = 0, roeter als die Haut = 0 %, dE76 der Mediane Kralle/Hand <= 6',
            '%d / %.2f %% / dE %.1f (von %d)' % (a, b, d, n), a == 0 and b == 0 and d <= 6,
            gegen=('K5c (rote Krallen)', '%d / %.2f %% / dE %.1f' % (ag, bg, dg), ag == 0 and bg == 0 and dg <= 6))


def v1(R, S, bau):
    """Adern multiplikativ (V1): je Hautzeile Fit L' = a + b L_G4 ueber die Aderklasse unterhalb des Knies (L_G4 x f_s < 70), |a| <= 1,5."""
    g4 = E.g4_rampen()[0]
    fs = bau['textur']['haut']['f_je_seite']

    def fit(St, zk, fsd):
        worst = 0.0
        for s in range(3):
            Lg = R2.lab_w(g4[s])[:, 0]
            Ln = St.lab[zk[s]][:, 0]
            _L, _h, _he, ader = E.klassen([int(x) for x in g4[s]])
            sel = ader & (Lg * float(fsd[str(s)]) < 70) & (Lg > 1)
            if sel.sum() > 3:
                _b, a = np.polyfit(Lg[sel], Ln[sel], 1)
                worst = max(worst, abs(float(a)))
        return worst
    wv = fit(S, [S.namen.index('haut%d' % s) for s in range(3)], fs)
    Sf = R2.Stand(K5F)
    bf = json.load(open(os.path.join(K5F, 'bau.json'), encoding='utf-8'))
    zk = bf.get('par', {}).get('zeilen', {}).get('haut', [1, 2, 3])
    fk = float(bf.get('par', {}).get('haut', {}).get('f', 2.46))
    wf = fit(Sf, zk, {'0': fk, '1': fk, '2': fk})
    R.zeile('V1', 'Adern multiplikativ: je Hautzeile Fit L\' = a + b L_G4 der Aderklasse unter dem Knie, |a| <= 1,5', '|a| max %.2f' % wv, wv <= 1.5,
            gegen=('K5f (Adern affin)', '|a| max %.2f' % wf, wf <= 1.5))


def v6(R, S, plan):
    k5c = [int(x) for x in E.eingaenge()['cl_k5c'][5]]
    kern = E.f1_kern_eintraege()
    f1 = [k for k, v in plan.items() if v[0] == 'f1']
    M_ = E.f1_masken()
    basis = [i for i, n in enumerate(S.namen) if n.startswith('f1')]
    ok_b = all(S.cl[z][i] == k5c[i] for z in basis for i in kern)
    falsch = 0
    for k in f1:
        _y, _x, idx = S.texel(k)
        z = S.zeile[k]
        for i in set(int(x) for x in idx) & set(kern):
            if S.cl[z][i] != k5c[i]:
                falsch += 1

    def rand(St):
        Ls = []
        for k in f1:
            ys, xs, idx = St.texel(k)
            nk = ~M_['kern'][ys, xs]
            Ls.append(St.lab[St.zeile[k]][idx[nk], 0])
        haut = [k for k in St.fl if k[0] == 0 and k not in f1]
        Lh, _ = St.farben(haut)
        return float(np.median(np.concatenate(Ls))), float(np.median(Lh[:, 0]))
    rv, hv = rand(S)
    rg, hg = rand(R2.Stand(K5E_ALT))
    ok = ok_b and falsch == 0 and rv >= hv - 6
    R.zeile('V6', 'F1: Kern-Eintraege der F1-Zeilen bytegleich K5c-Zeile 5 und jede F1-Flaeche liest im Kern K5c; Rand-Median >= Mesh-0-Haut - 6',
            'Grundzeilen %s, falsch gelesene Kern-Eintraege %d; Rand %.1f gegen Haut %.1f' % ('gleich' if ok_b else 'VERSCHIEDEN', falsch, rv, hv), ok,
            gegen=('K5e (schwarzer Ring, E32)', 'Rand %.1f gegen Haut %.1f' % (rg, hg), rg >= hg - 6))


# ------------------------------------------------------------------ V2 Hautton
def v2(R, S, plan):
    hz = [i for i, n in enumerate(S.namen) if n.startswith('haut')]
    haut = [k for k, v in plan.items() if v[0] == 'haut' and S.zeile[k] in hz]
    hv, cv, lv = R2.haut_werte(S, haut)
    out = {}
    for nm, o in (('K5e_f1', K5E), ('K5f', K5F)):
        St = R2.Stand(o)
        out[nm] = R2.haut_werte(St, [k for k in haut if k in St.fl])
    ok = 14 <= hv <= 27 and hv > out['K5e_f1'][0] and hv > out['K5f'][0]
    R.zeile('V2.1', 'Hautton (Albedo der Hautflaechen, C-gewichtet) im Band der Originale [14, 27] Grad und waermer als K5e_f1 und K5f',
            '%.1f Grad (K5e_f1 %.1f, K5f %.1f)' % (hv, out['K5e_f1'][0], out['K5f'][0]), ok,
            gegen=('K5e_f1', '%.1f Grad' % out['K5e_f1'][0], 14 <= out['K5e_f1'][0] <= 27))
    R.zeile('V2.2', 'Haut-Buntheit Median C* (Info; O3: Eichung C* 2-6 ist keine Grenze, s kommt aus Z4/Z8)', '%.2f (L* Median %.1f)' % (cv, lv), True,
            art='Info')


# ------------------------------------------------------------------ S1 Sattel
def sattel_w(St):
    hz = [i for i, n in enumerate(St.namen) if n.startswith('haut')]
    haut = [k for k in St.fl if St.zeile[k] in hz and k[0] not in (0, 7)]
    Ls, _ = St.farben(haut)
    sat = [k for k in St.fl if k[0] in (0, 7) and St.zeile[k] in hz]
    Lt, _ = St.farben(sat)
    C = np.hypot(Lt[:, 1], Lt[:, 2])
    return float(np.median(Lt[:, 0])), float(np.percentile(Ls[:, 0], 25)), float(np.percentile(Ls[:, 0], 75)), float(np.median(C))


def s1(R, S, var):
    m, p25, p75, c = sattel_w(S)
    par = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))['par']
    p2 = json.loads(json.dumps(par))
    p2['textur']['haut']['seiten_angleichen'] = False
    d = os.path.join(K3.SCR, 'riegel_s1_gegen')
    shutil.rmtree(d, ignore_errors=True)
    K3.bauen(p2, d, log=lambda *a: None)
    Sg = stand_mit_namen(os.path.join(d, 'pruef'))
    mg, g25, g75, cg = sattel_w(Sg)
    R.zeile('S1', 'kein Sattel (O3): Median L* der Haut auf Huefte/Ruecken (Teile 0/7) im Band P25-P75 der uebrigen Haut',
            'Teile 0/7 %.1f (C* %.1f), Haut P25-P75 %.1f-%.1f' % (m, c, p25, p75), p25 <= m <= p75,
            gegen=('Bau ohne Seitenangleich (K5g2-Weg)', 'Teile 0/7 %.1f, Haut %.1f-%.1f' % (mg, g25, g75), g25 <= mg <= g75))


# ------------------------------------------------------------------ A1 Augen
def a1(R, var):
    w = K3.auge_mass(var)
    g = K3.auge_mass(K5E)
    ok = all(m >= 2.0 * r for m, r in w)
    R.zeile('A1', 'Augen (O4): Bild 2 (Licht der Szene), an beiden Nutzerpunkten L*-max 9x9 >= 2 x Median Kopfhaut daneben',
            '; '.join('max %.1f / Kopfhaut %.1f' % t for t in w), ok,
            gegen=('K5e_f1', '; '.join('max %.1f / Kopfhaut %.1f' % t for t in g), all(m >= 2.0 * r for m, r in g)))


# ------------------------------------------------------------------ Z Spiel
def spiel(R, var, pfad):
    if not pfad or not os.path.exists(pfad):
        R.zeile('Z', 'Spiel 5080 Z1-Z9', 'keine Spielmessung uebergeben', False)
        return None
    d = json.load(open(pfad, encoding='utf-8'))
    if d.get('tim_sha256') != sha_d(os.path.join(var, 'BAHNSTEIG_FORM.TIM')) or d.get('emd_sha256') != sha_d(os.path.join(var, 'BAHNSTEIG_FORM.EMD')):
        R.zeile('Z', 'Spiel 5080 Z1-Z9', 'Spielmessung gehoert nicht zu dieser Variante (sha256)', False)
        return None
    m, g, gc = d['f80'], d['gegen_f80'], d.get('gegen_k5c_f80') or {}
    for nr, txt, key, lo, hi in R2.ZB:
        v = m.get(key)
        # Gegenprobe: K5e_f1 (Messblatt: rot in Z2-Z9); wo K5e_f1 im Band liegt (Z1), K5c (Messblatt: Z1 rot, P L* 2,77)
        gn, gv = ('K5e_f1 (eigener Lauf)', g.get(key))
        if gv is not None and lo <= gv <= hi and gc:
            gn, gv = ('K5c (eigener Lauf)', gc.get(key))
        R.zeile(nr, 'Spiel 5080 %s im Band %.3f..%.3f der Original-Gegner (zweiseitig)' % (txt, lo, hi), '%s' % v, v is not None and lo <= v <= hi,
                gegen=(gn, '%s' % gv, gv is not None and lo <= gv <= hi))
    return d


def spiel_messen(var, aus):
    """Eigenes Normal-/Schluessel-Paar fuer die Variante und fuer K5e_f1 (Gegenprobe), 5080 Bild-1-Lage, Bilder 360-440."""
    n = 'riegel_' + sha_d(os.path.join(var, 'BAHNSTEIG_FORM.TIM'))[:8]
    m = K3._spiel_paar(var, n)
    g = K3._spiel_paar(K5E, 'riegel_k5e_f1')
    gc = K3._spiel_paar(os.path.join(K5, 'einbau_k5c'), 'riegel_k5c')
    r = dict(variante=var.replace(chr(92), '/'), tim_sha256=sha_d(os.path.join(var, 'BAHNSTEIG_FORM.TIM')),
             emd_sha256=sha_d(os.path.join(var, 'BAHNSTEIG_FORM.EMD')), f80=m, gegen_f80=g, gegen_k5c_f80=gc,
             verfahren='kf_k5g3_zlauf.sh (RE15_FORCE_CUT=1) + kf_k5f_spielmass.paar (Messblatt K5F 1.1): eigenes Normal-/Schluessel-Paar, echter '
                       'Renderer, Framedump 640x480, 5080 Bild-1-Lage (BB_POSE -20711,-15984,756,0,11,1), Bilder 360-440/20')
    json.dump(r, open(aus, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    return aus


# ------------------------------------------------------------------ Technik
def technik(R, var, bau):
    t = FE.tim_lesen(open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
    nz = len(t['cluts'])
    R.zeile('T1', 'Zeilen <= 16, Port-Texturhoehe <= 4096', '%d / %d' % (nz, nz * 256), nz <= 16 and nz * 256 <= 4096,
            gegen=('17 Zeilen (gebaut)', '17 / 4352', False))
    S = R2.Stand(var)
    bad = [k for k, z in S.zeile.items() if not 0 <= z < len(S.cl)]
    R.zeile('T5', 'jede Flaeche des Assets liest eine gueltige Zeile', '%d ungueltig von %d' % (len(bad), len(S.zeile)), not bad)
    a = os.path.join(E.WT, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM')
    se, st = sha_d(a + '.EMD'), sha_d(a + '.TIM')
    ok = se == 'f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1' and st.startswith('31a35e66')
    R.zeile('G5', 'Spiel-Asset shared_assets/RE2/BAHNSTEIG_FORM.* bleibt K5e_f1 (EMD f277916d, TIM 31a35e66)', 'EMD %s, TIM %s' % (se[:8], st[:8]), ok,
            gegen=('K5g3-Asset', 'EMD %s' % sha_d(os.path.join(var, 'BAHNSTEIG_FORM.EMD'))[:8],
                   sha_d(os.path.join(var, 'BAHNSTEIG_FORM.EMD')) == 'f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1'))


def main(argv):
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?', default=K3.AUS)
    ap.add_argument('--spiel')
    ap.add_argument('--neu_spiel', action='store_true', help='Spielmessung neu laufen lassen (sonst <Variante>/spiel_z.json)')
    a = ap.parse_args(argv)
    var = os.path.abspath(a.variante)
    bau = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))
    S = stand_mit_namen(os.path.join(var, 'pruef'), bau['textur']['zeilen'])
    X = E.eingaenge()
    Z = TX.lade_zonen()
    plan = T3.materialplan(S.form, X, Z)
    R = R2.Riegel()
    zeilen_alt = R.zeile

    def zeile(nr, text, ist, ok, art='Riegel', gegen=None):
        zeilen_alt(nr, text, ist, ok, art, gegen)
    R.zeile = zeile
    tg(R, var)
    tb(R, var)
    s_haut = float(bau['textur']['haut']['s'])
    je = v3(R, S, bau, plan)
    v31b(R, var, bau)
    R2.v3_fleisch(R, S, je)
    v4(R, S, plan, s_haut)
    v44(R, var, bau)
    v2(R, S, plan)
    v5(R, S, s_haut)
    R2.h1(R, S)
    s1(R, S, var)
    v1(R, S, bau)
    v6(R, S, plan)
    a1(R, var)
    R2.weiss(R, var)
    sp = a.spiel or os.path.join(var, 'spiel_z.json')
    if a.neu_spiel or not os.path.exists(sp):
        spiel_messen(var, sp)
    spiel(R, var, sp)
    technik(R, var, bau)
    rot_ = [z['nr'] for z in R.z if z['art'] == 'Riegel' and not z['ok']]
    gg = [z['nr'] for z in R.z if z['gegenprobe'] and z['gegenprobe'][2]]
    nr_ = [z for z in R.z if z['art'] == 'Riegel']
    ng = [z for z in nr_ if z['gegenprobe']]
    summe = ('RIEGEL K5g3 TEXTUR: %d von %d gruen, %d von %d Gegenproben rot wie verlangt; ROT: %s; Gegenproben gruen: %s'
             % (len(nr_) - len(rot_), len(nr_), len(ng) - len(gg), len(ng), rot_ or '-', gg or '-'))
    print(summe)
    txt = ['%-6s %-6s %-4s %s: %s%s' % (z['nr'], z['art'], 'OK' if z['ok'] else 'ROT', z['text'], z['ist'],
                                         '' if not z['gegenprobe'] else ' | Gegenprobe %s: %s (%s)' % (z['gegenprobe'][0], z['gegenprobe'][1],
                                                                                                    'ROT ok' if not z['gegenprobe'][2] else 'GRUEN - greift nicht!'))
           for z in R.z] + [summe]
    open(os.path.join(var, 'riegel_k5g3_textur.txt'), 'w', encoding='utf-8', newline='\n').write('\n'.join(txt) + '\n')
    json.dump(dict(zeilen=R.z, rot=rot_, gegenprobe_gruen=gg, summe=summe), open(os.path.join(var, 'riegel_k5g3_textur.json'), 'w', encoding='utf-8',
                                                                                    newline='\n'), indent=1, ensure_ascii=False, default=str)
    return not rot_ and not gg


if __name__ == '__main__':
    sys.exit(0 if main(sys.argv[1:]) else 1)
