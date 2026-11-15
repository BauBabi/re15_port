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
 Runde 2 (Befunde PRUEFUNG_K5G3_r1_zahlen/_sicht; Gegenprobe meist die Abgabe der Runde 1 aus git, Commit da6560372):
 UV1  UV = Geometrie-Stufe uvfein + ein Kopie-Versatz je Flaeche (ausser Augen-Dreiecken)                                  | Runde 1 (B4-UV)
 K1   kein Zugriff auf Index 0 (leerer Kopie-Texel), abgetastet im Inneren aller Render-Dreiecke                           | Runde 1 (10|0|342-344)
 K2   kein Zugriff auf 0x7FFF ausser der Zahn-Zeile                                                                       | Runde 1
 W6   weisse Musterung fast unbunt (C* <= 0,5 x Haut) und hell (L* >= Haut-P75)                                           | Runde 1
 B1   braeunliche Musterung braun statt beige (L* <= 2 x Haut, Ton 35-80)                                                  | Runde 1
 A1   Augen (O4/E3): Bild 2, beide Nutzerpunkte, 0,5-2 x Nutzerflaeche, Mitte <= 2 px, Median >= 3 x Kopfhaut              | f_auge Runde 1
 A2   kein schwarzes Loch am Kopf (weisses Licht von vorn, RGB-Max <= 16 <= 2 x K5e_f1)                                   | Runde 1
 A3   Augen im echten Spielfenster 5080 von vorn (kf_k5g3_augen_spiel.py): 2 Augen, je >= 5 px, Median >= 2,5 x Haut      | f_auge Runde 1
 W1-W4, R1-R4 weisses Licht im Band der Originale, Ruhe                                                                     | K5e_f1
 Z1-Z9 Spiel 5080 im Zielband der Original-Gegner (bindend, O3)                                                            | K5e_f1 (eigener Lauf)
 T1   Zeilen <= 16, Port-Texturhoehe <= 4096 | T5 jede Flaeche liest eine gueltige Zeile
 G5   (R9-Einbau) Spiel-Asset shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM == diese Variante (vorher: bleibt K5e_f1)                | K5e_f1
 H18  (R9-Einbau) Netz 18 des Assets = spiegelhand(Netz 13 des Assets): Ecken/Normalen/Flaechen wie die Geometrie-Stufe, UV/CLUT/Seite
      Flaeche fuer Flaeche die von 13 (R9 N33) -> die rechte Hand liest genau die Texel der linken                                     | K5g3 Runde 3
 Runde 3 (Orchestrator-Entscheide G6-G8; Gegenprobe die Abgabe der Runde 2 aus git, Commit f48d37df1):
 G6.16/G6.19 braeunliche Musterung: cv(L*) >= 0,5 x cv(G4), ab 15 Texeln >= 5 Stufen (G6.i dieselbe Regel, uebrige Zonen, Info) | Runde 2
 G7   Zone 44 (und alle V4-Flaechen) ohne Fleck-Material, nicht roeter als die Haut                                        | Runde 2
 G8   kein Zugriff auf L* > 85 ausser Zaehnen, Maul, Augen, F1-Kern                                                         | Runde 2
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
    def lies(o, n):
        return json.load(open(os.path.join(o, n), encoding='utf-8')) if n.endswith('.json') else open(os.path.join(o, n), 'rb').read()
    ab = [n for n in K3.DATEIEN + ('bau.json',) if lies(var, n) != lies(d, n)]
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
    # Runde 2: Tabelle mit den uvfein-UV (GEOMETRIE.md 9.6); die Augen-Dreiecke (eigene Zeile/UV, Riegel UV1/A1) sind ausgenommen
    tab = json.load(open(G3.GEO3_SPALT_UV, encoding='utf-8'))
    augen = set((22, 0, i) for i in bau['textur'].get('augen_dekal', {}).get('dreiecke', []))
    P = FE.lese_emd(open(os.path.join(var, 'pruef', 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    A = FE.lese_emd(open(os.path.join(var, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    art = ('dreiecke', 'vierecke')
    namen = bau['textur']['zeilen']
    cy = FE.tim_lesen(open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read())['clut_y']
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    traeger = set(TX.schluessel(k) for nr in tj if nr in TX.FLECK_ART and nr != '29' for k in tj[nr])
    # Runde 2: die Loch-Flaechen (Fleck 'L', Haut auf eigenen Texeln) sind Traeger einer Fleck-Zeile
    traeger |= set(TX.schluessel(k) for k in bau['textur']['flecken'].get('loch_flaechen', {}))

    def gx(f):
        return [(int(u) + (int(f['page']) & 15) * 128, int(v)) for u, v in f['uv']]

    def pruefen(P_, A_):
        falsch, ausser, fremd = 0, 0, 0
        for t, a, i, mt, ma, mi in tab['herkunft']:
            if (t, a, i) in augen:
                continue
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
    R.zeile('V3.1b', 'Asset: %d Spalt-Kinder (ohne %d Augen-Dreiecke) lesen Zeile/Seite der Mutter, UV im Texturstueck der Mutter (+-1), keine Fleck-Zeile '
            'ohne Traeger-Mutter' % (len(tab['herkunft']) - len(augen), len(augen)), 'falsche Zeile/Seite %d, ausserhalb %d, fremde Fleck-Leser %d' % (f1, a1, r1), ok,
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
_LOCH = {}


def loch_je(S):
    """Runde 2: Loch-Flaechen des Generators (bau.json textur.flecken.loch_flaechen) als Fleck 'L' mit allen gelesenen Texeln (kf_k5g3_tex.lese_maske
    auf der K5e_f1-UV samt Spalt-Kindern) - genau wie der Generator."""
    bj = os.path.join(S.ordner, 'bau.json')
    if not os.path.exists(bj):
        bj = os.path.join(os.path.dirname(S.ordner), 'bau.json')
    lf = json.load(open(bj, encoding='utf-8'))['textur']['flecken'].get('loch_flaechen', {})
    if not lf:
        return {}
    if 'k' not in _LOCH:
        _LOCH['k'] = K3.kinder_uv(G3.GEO3_SPALT_UV)
        _LOCH['f'] = R2.Stand(K5E).fl
    aus = {}
    for ks in lf:
        k = TX.schluessel(ks)
        ys, xs = np.nonzero(T3.lese_maske(_LOCH['f'][k], S.T['pixel'].shape, _LOCH['k'].get(k)))
        aus[k] = {(int(x), int(y)): 'L' for y, x in zip(ys, xs)}
    return aus


def v4_flaechen(Z):
    """Runde 3 (G7): Flaechen der V4-Zonen (Lippe 42, Unterkiefer aussen 31, Hals-Unterseite 44, Kopf-Unterseite 45) und Lippensaum/Maul_Band -
    sie tragen kein rotes Fleck-Material (wie der Generator, kf_k5g3_tex.V4_ZONEN)."""
    zl = R2.zonen_listen(Z)
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    return set().union(*[zl[nr] for nr in T3.V4_ZONEN]) | set(TX.schluessel(k) for nm in ('Lippensaum', 'Maul_Band') for k in mj[nm]['liste'])


def je_mit_kern(S, plan):
    """Fleck-Texel je Traeger wie der Generator: kf_k5g2_tex.fleck_zuordnung + F1-Kern-Vorrang (V6). Runde 2: ohne Zone 29 (Augen ueber eigene
    Augen-Texel, Riegel A1-A3), dazu die Loch-Flaechen als Fleck 'L'."""
    Z = TX.lade_zonen()
    je, _k = TX.fleck_zuordnung(Z, S.form, S.T['pixel'].shape)
    kern = E.f1_masken()['kern']
    v4 = v4_flaechen(Z)
    rot_z = set(nr for nr, a in T3.FLECK_ART.items() if a in T3.ROT_MATERIAL)
    for k in list(je):
        for xy in [xy for xy, nr in je[k].items() if nr == '29' or (k in v4 and nr in rot_z)]:
            del je[k][xy]
        if plan[k][0] == 'f1':
            for xy in [xy for xy in je[k] if kern[xy[1], xy[0]]]:
                del je[k][xy]
        if not je[k]:
            del je[k]
    je.update(loch_je(S))
    return je


def v3(R, S, bau, plan):
    """= riegel_k5g2.v3, aber mit dem F1-Kern-Vorrang des Generators (Fleck-Texel im F1-Kern bleiben F1, bau.json flecken.f1_kern_vorrang_texel)."""
    namen = S.namen
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    fleck_z = [i for i, n in enumerate(namen) if n.startswith('fleck_')]
    je = je_mit_kern(S, plan)
    traeger = set(TX.schluessel(k) for nr in tj if nr in TX.FLECK_ART and nr != '29' for k in tj[nr]) | set(je)
    fremd = [k for k, z in S.zeile.items() if z in fleck_z and k not in traeger]
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


# ================================================================== Runde 2 (Befunde PRUEFUNG_K5G3_r1_zahlen / _sicht)
R1_COMMIT = 'da6560372'     # Abgabe K5g3 Runde 1 (EMD 37407afd, TIM 2f7b2444) - Gegenprobe fuer die Befunde der Runde 1
R1_PFAD = 'analysis/befunde_runde41/form/varianten/K5/k5g/K5g3'


def r1_stand():
    """Abgabe der Runde 1 aus git (Commit da6560372) in den Scratchpad holen (einmal)."""
    import subprocess
    d = os.path.join(K3.SCR, 'gegen_r1_' + R1_COMMIT)
    if os.path.exists(os.path.join(d, 'bau.json')):
        return d
    os.makedirs(os.path.join(d, 'pruef'), exist_ok=True)
    for n in K3.DATEIEN + ('bau.json',):
        b = subprocess.run(['git', '-C', E.WT, 'show', '%s:%s/%s' % (R1_COMMIT, R1_PFAD, n)], capture_output=True, check=True).stdout
        open(os.path.join(d, n), 'wb').write(b)
    return d


def _zeilen_namen(o):
    return json.load(open(os.path.join(o, 'bau.json'), encoding='utf-8'))['textur']['zeilen']


def uv1(R, var, bau):
    """UV des Assets = UV der Geometrie-Stufe (uvfein, geo/BAHNSTEIG_FORM.EMD) + EIN Kopie-Versatz je Flaeche; ausgenommen die Augen-Dreiecke."""
    g = FE.lese_emd(open(G3.GEO3_ZIEL, 'rb').read())
    aus = set((22, 0, i) for i in bau['textur'].get('augen_dekal', {}).get('dreiecke', []))

    def n_falsch(o):
        a = FE.lese_emd(open(os.path.join(o, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
        return len(K3.uv_gegen_geo(a, g, aus))
    nv, ng = n_falsch(var), n_falsch(r1_stand())
    R.zeile('UV1', 'UV aller Flaechen = Geometrie-Stufe uvfein (sha %s) + ein Kopie-Versatz je Flaeche (ausser %d Augen-Dreiecken mit eigener UV)'
            % (sha_d(G3.GEO3_ZIEL)[:8], len(aus)), '%d Flaechen abweichend' % nv, nv == 0,
            gegen=('Runde 1 (Commit %s, B4-UV)' % R1_COMMIT, '%d Flaechen abweichend' % ng, ng == 0))


def zugriffe(o):
    """Abtastung (kf_k5g3.abtasten: 16 x 16 Proben je Render-Dreieck, Vierecke in beiden Zerlegungen) des Assets: (Zugriffe, Index-0-Zugriffe,
    0x7FFF-Zugriffe ausser Zahn-Zeile, Flaechen mit Index-0-Zugriff)."""
    A = FE.lese_emd(open(os.path.join(o, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    T = FE.tim_lesen(open(os.path.join(o, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
    cl = [[int(x) for x in z] for z in T['cluts']]
    nam = _zeilen_namen(o)
    z = K3.abtasten(A, T['pixel'], int(T['clut_y']))
    i0 = [t for t in z if t[4] == 0]
    w7 = [t for t in z if 0 <= t[1] < len(cl) and not nam[t[1]].startswith('zahn') and (cl[t[1]][t[4]] & 0x7FFF) == 0x7FFF]
    return len(z), len(i0), len(w7), sorted(set('%d|%d|%d' % t[0] for t in i0))


def kopie(R, var):
    n, i0, w7, f0 = zugriffe(var)
    gn, gi0, gw7, gf0 = zugriffe(r1_stand())
    R.zeile('K1', 'Kopie-Maske (Zahlen-Pruefer r1 ROT 2): kein Zugriff auf Index 0 (= ungeschriebener Kopie-Texel, 0x7FFF) in irgendeiner Flaeche, '
            'abgetastet im Inneren aller Render-Dreiecke', '%d von %d Zugriffen (%s)' % (i0, n, f0[:6] or '-'), i0 == 0,
            gegen=('Runde 1 (Commit %s)' % R1_COMMIT, '%d Zugriffe, Flaechen %s' % (gi0, gf0[:6]), gi0 == 0))
    R.zeile('K2', 'kein Zugriff auf reinweiss 0x7FFF ausser der Zahn-Zeile (K5c bytegleich), ganzes Asset (Zahlen-Pruefer r1 Gelb 4)',
            '%d von %d Zugriffen' % (w7, n), w7 == 0, gegen=('Runde 1 (Commit %s)' % R1_COMMIT, '%d Zugriffe' % gw7, gw7 == 0))


def _zonen_texel(S, bau, je, zonen):
    """Lab der Material-Texelzugriffe der Zonen (Pruef-Stand; Kopie-Versatz aus bau.json)."""
    kopien = {c['flaeche']: c for c in bau['textur']['kopien']}
    out = []
    for k, d in je.items():
        zs = set(nr for nr in d.values() if nr in zonen)
        if not zs:
            continue
        c = kopien.get('%d|%d|%d' % k)
        if c is None:
            continue
        dx, dy = c['nach'][0] - c['von'][0], c['nach'][1] - c['von'][1]
        ys, xs, idx = S.texel(k)
        m = np.array([d.get((int(x) - dx, int(y) - dy)) in zonen for y, x in zip(ys, xs)])
        out.append(S.lab[S.zeile[k]][idx[m]])
    return np.vstack(out) if out else np.zeros((0, 3))


def _haut_werte(S):
    hz = [i for i, n in enumerate(S.namen) if n.startswith('haut')]
    Lh, _ = S.farben([k for k in S.fl if S.zeile[k] in hz])
    return Lh


def weiss_braun(R, S, bau, plan):
    def mess(St, b, pl):
        je = je_mit_kern(St, pl)
        Lw = _zonen_texel(St, b, je, ('14', '15', '18', '24'))
        Lb = _zonen_texel(St, b, je, ('16', '19'))
        Lh = _haut_werte(St)
        Ch = float(np.median(np.hypot(Lh[:, 1], Lh[:, 2])))
        return (float(np.median(Lw[:, 0])), float(np.median(np.hypot(Lw[:, 1], Lw[:, 2]))), float(np.median(Lb[:, 0])),
                float(np.median(np.hypot(Lb[:, 1], Lb[:, 2]))), float(np.degrees(np.arctan2(np.median(Lb[:, 2]), np.median(Lb[:, 1])))),
                float(np.median(Lh[:, 0])), float(np.percentile(Lh[:, 0], 75)), Ch, len(Lw), len(Lb))
    v = mess(S, bau, plan)
    g1 = r1_stand()
    Sg = stand_mit_namen(os.path.join(g1, 'pruef'), _zeilen_namen(g1))
    g = mess(Sg, json.load(open(os.path.join(g1, 'bau.json'), encoding='utf-8')), plan)

    def ok_w(t):
        return t[1] <= 0.5 * t[7] and t[0] >= t[6]

    def ok_b(t):
        return t[2] <= 2.0 * t[5] and 35 <= t[4] <= 80
    R.zeile('W6', 'weisse Musterung (Zonen 14/15/18/24) fast unbunt und hell: Median C* <= 0,5 x Median C* der Haut, Median L* >= P75 der Haut '
            '(Zahlen-Pruefer r1 ROT 1)', 'C* %.1f (Haut %.1f), L* %.1f (Haut P75 %.1f), %d Zugriffe' % (v[1], v[7], v[0], v[6], v[8]), ok_w(v),
            gegen=('Runde 1 (Commit %s)' % R1_COMMIT, 'C* %.1f (Haut %.1f), L* %.1f' % (g[1], g[7], g[0]), ok_w(g)))
    R.zeile('B1', 'braeunliche Musterung (Zonen 16/19) braun statt beige: Median L* <= 2 x Median L* der Haut, Farbton 35-80 Grad '
            '(Zahlen-Pruefer r1 Gelb 2)', 'L* %.1f (Haut %.1f), C* %.1f, Ton %.1f Grad, %d Zugriffe' % (v[2], v[5], v[3], v[4], v[9]), ok_b(v),
            gegen=('Runde 1 (Commit %s)' % R1_COMMIT, 'L* %.1f (Haut %.1f), Ton %.1f' % (g[2], g[5], g[4]), ok_b(g)))


def augen(R, var):
    """A1 (O4, E3; Sicht-Pruefer r1 (5)): beide Augen in Bild 2 an den Nutzerpunkten, so gross wie die Nutzerpunkte (0,5-2-fach), Mitte <= 2 px
    vom Punkt, Median >= kf_k5g3.AUGE_FAKTOR x Kopfhaut daneben. Gegenprobe: derselbe Bau mit f_auge der Runde 1 (1,9844)."""
    def ok(a):
        return all(t['px'] and 0.5 <= t['px'] / t['nutzer_px'] <= 2.0 and t['mitte_abstand'] <= 2.0 for t in a) and K3.auge_ok(a)

    def txt(a):
        return '; '.join('%d px (Nutzer %.1f), Abstand %.1f px, Median %.1f / Haut %.1f' % (t['px'], t['nutzer_px'], t['mitte_abstand'] or -1,
                                                                                        t['L_median'], t['ring_median']) for t in a)
    a = K3.auge_mass(var)
    par = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))['par']
    p2 = json.loads(json.dumps(par))
    p2['textur']['auge']['f'] = 1.9844
    d = os.path.join(K3.SCR, 'riegel_a1_gegen')
    shutil.rmtree(d, ignore_errors=True)
    K3.bauen(p2, d, log=lambda *x: None)
    g = K3.auge_mass(d)
    R.zeile('A1', 'Augen (O4): Bild 2 (Licht der Szene), je Nutzerpunkt Augen-Pixel 0,5-2 x Nutzerflaeche, Mitte <= 2 px, Median >= %.0f x Kopfhaut'
            % K3.AUGE_FAKTOR, txt(a), ok(a), gegen=('f_auge der Runde 1 (1,9844)', txt(g), ok(g)))


def loch(R, var):
    """A2 (Sicht-Pruefer r1 (5): schwarzer Fleck an der rechten Augenhoehle): weisses Licht von vorn (Clip 2, Az 90, El 10, 760 px), Pixel von
    Netz 22 mit RGB-Max <= 16 hoechstens doppelt so viele wie bei K5e_f1."""
    sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
    import rl

    def n(o):
        img, K = rl.weiss_id(rl.fl.lade_var(o), 2, 90, 10, 760)
        return int(((K['mesh'] == 22) & (img.max(2) <= 16)).sum())
    nv, ne, ng = n(var), n(K5E), n(r1_stand())
    R.zeile('A2', 'kein schwarzes Loch am Kopf: weisses Licht von vorn, Netz-22-Pixel mit RGB-Max <= 16 <= 2 x K5e_f1',
            '%d (K5e_f1 %d)' % (nv, ne), nv <= 2 * ne, gegen=('Runde 1 (Commit %s)' % R1_COMMIT, '%d' % ng, ng <= 2 * ne))


def augen_spiel(R, var, pfad):
    """A3: Augen im echten Spielfenster (5080 von vorn, Clip 12 Bild 0; eigene Laeufe mit Markierungs-Variante, kf_k5g3_augen_spiel.py)."""
    if not pfad or not os.path.exists(pfad):
        R.zeile('A3', 'Augen im Spielfenster', 'keine Messung', False)
        return
    d = json.load(open(pfad, encoding='utf-8'))
    if d.get('tim_sha256') != sha_d(os.path.join(var, 'BAHNSTEIG_FORM.TIM')):
        R.zeile('A3', 'Augen im Spielfenster', 'Messung gehoert nicht zu dieser Variante', False)
        return

    def ok(a):
        return len(a) == 2 and all(t['px'] >= 5 and t['L_median'] >= 2.5 * t['ring_median'] for t in a)

    def txt(a):
        return '; '.join('%d px bei %s, Median L* %.1f / Haut %.1f' % (t['px'], t['mitte'], t['L_median'], t['ring_median']) for t in a) or 'keins'
    R.zeile('A3', 'Augen im echten Spielfenster 5080 von vorn (gdigrab, eigenes Fenster): 2 Augen, je >= 5 px, Median L* >= 2,5 x Haut daneben',
            txt(d['augen']), ok(d['augen']), gegen=('f_auge der Runde 1 (1,9844), gleicher Lauf', txt(d['gegen']), ok(d['gegen'])))


# ================================================================== Runde 3 (Orchestrator-Entscheide G6-G8 nach PRUEFUNG_K5G3_r2_zahlen/_sicht)
R2_COMMIT = 'f48d37df1'     # Abgabe K5g3 Runde 2 (EMD 482bd63f, TIM ce9a1dc9) - Gegenprobe fuer die Befunde der Runde 2
L_MAX = 85.0                # G8: "Kein Haut-Texel liest einen Eintrag mit L* > 85"
S_HAUT_G7 = 0.5225          # 'roeter als die Haut' wie V4.1 (Haut C* = s L*, s aus k5g3_par.json haut.s)


def r2_stand():
    """Abgabe der Runde 2 aus git (Commit f48d37df1) in den Scratchpad holen (einmal)."""
    import subprocess
    d = os.path.join(K3.SCR, 'gegen_r2_' + R2_COMMIT)
    if os.path.exists(os.path.join(d, 'bau.json')):
        return d
    os.makedirs(os.path.join(d, 'pruef'), exist_ok=True)
    for n in K3.DATEIEN + ('bau.json',):
        b = subprocess.run(['git', '-C', E.WT, 'show', '%s:%s/%s' % (R2_COMMIT, R1_PFAD, n)], capture_output=True, check=True).stdout
        open(os.path.join(d, n), 'wb').write(b)
    return d


def struktur_je_zone(St, b, je, plan, X, zonen):
    """G6: je Fleck-Zone die eindeutigen Zonen-Texel (x, y) - je Texel das Wort, das der erste Traeger (sortiert) in seiner Kopie liest - gegen die
    G4-Originalfarbe am Original-Index (CDEMD0.EMS-CLUT der G4-Seite des Traegers: X['cl_g4']; X['T']['pixel'] = Atlas K5e_f1, Seiten 0-2 =
    G4-Atlas bytegleich). Rueckgabe {zone: (n, cv_K5g3, Stufen_K5g3, cv_G4, Stufen_G4)}; cv = Standardabweichung / Mittel von L*,
    Stufen = verschiedene Worte."""
    kopien = {c['flaeche']: c for c in b['textur']['kopien']}
    px0 = X['T']['pixel']
    g4lab = [TX.lab_zeile([int(x) for x in z]) for z in X['cl_g4']]
    texel = {}
    for k in sorted(je):
        c = kopien.get('%d|%d|%d' % k)
        if c is None:
            continue
        dx, dy = c['nach'][0] - c['von'][0], c['nach'][1] - c['von'][1]
        z = St.zeile[k]
        for (x, y), nr in sorted(je[k].items()):
            if nr not in zonen or (nr, x, y) in texel:
                continue
            i = int(St.T['pixel'][y + dy, x + dx])
            i0 = int(px0[y, x])
            s = plan[k][1]
            texel[(nr, x, y)] = (St.cl[z][i], float(St.lab[z][i, 0]), int(X['cl_g4'][s][i0]), float(g4lab[s][i0, 0]))
    aus = {}
    for nr in zonen:
        t = [v for kk, v in texel.items() if kk[0] == nr]
        if not t:
            continue
        L = np.array([v[1] for v in t])
        Lg = np.array([v[3] for v in t])
        aus[nr] = (len(t), float(L.std() / max(L.mean(), 1e-6)), len(set(v[0] for v in t)), float(Lg.std() / max(Lg.mean(), 1e-6)),
                   len(set(v[2] for v in t)))
    return aus


def g6(R, S, bau, plan, X):
    """G6 (V3 'Helligkeitsstruktur der Original-Texel bleibt'): in jeder Fleck-Zone cv(K5g3) >= 0,5 x cv(G4) und - ab 15 Texeln - mindestens 5
    Stufen (Vorschlag des Zahlen-Pruefers r2). Riegel fuer die Zonen 16 und 19 (braeunliche Musterung), die uebrigen Fleck-Zonen als Info.
    Gegenprobe: Abgabe der Runde 2 (Pruefer: Zone 16 cv 0,06 gegen 0,42, 2 Stufen)."""
    alle = ('2', '3', '4', '9', '11', '12', '14', '15', '16', '18', '19', '24', '27')
    v = struktur_je_zone(S, bau, je_mit_kern(S, plan), plan, X, alle)
    g2 = r2_stand()
    Sg = stand_mit_namen(os.path.join(g2, 'pruef'), _zeilen_namen(g2))
    bg = json.load(open(os.path.join(g2, 'bau.json'), encoding='utf-8'))
    g = struktur_je_zone(Sg, bg, je_mit_kern(Sg, plan), plan, X, ('16', '19'))

    def ok(t):
        return t[1] >= 0.5 * t[3] and (t[0] < 15 or t[2] >= 5)

    def txt(d, zs):
        return '; '.join('Zone %s: %d Texel, cv %.3f (G4 %.3f), %d Stufen (G4 %d)' % (nr, d[nr][0], d[nr][1], d[nr][3], d[nr][2], d[nr][4])
                         for nr in zs if nr in d) or 'keine Texel'
    for nr in ('16', '19'):
        R.zeile('G6.%s' % nr, 'braeunliche Musterung Zone %s mit der Helligkeitsstruktur der Original-Texel: cv(L*) >= 0,5 x cv(G4) und ab 15 Texeln '
                '>= 5 Stufen (Zahlen-Pruefer r2 ROT)' % nr, txt(v, (nr,)), nr in v and ok(v[nr]),
                gegen=('Runde 2 (Commit %s)' % R2_COMMIT, txt(g, (nr,)), nr in g and ok(g[nr])))
    info = [nr for nr in alle if nr not in ('16', '19') and nr in v]
    schwach = [nr for nr in info if not ok(v[nr])]
    R.zeile('G6.i', 'uebrige Fleck-Zonen, dieselbe Regel (Info; Zahlen-Pruefer r2 Gelb 4: Zonen 3/4 gestaucht)', txt(v, info) +
            ' | unter der Regel: %s' % (schwach or '-'), True, art='Info')


def _material_zugriffe(St, keys):
    """Texelzugriffe der Flaechen (Pruef-Stand) auf Fleck-Material: Fleck-Zeile und Wort != Grundzeile am selben Index."""
    n, alle = 0, 0
    for k in keys:
        if k not in St.fl:
            continue
        _y, _x, idx = St.texel(k)
        z = St.zeile[k]
        alle += len(idx)
        if not St.namen[z].startswith('fleck_'):
            continue
        b = St.namen.index(St.namen[z].split('_')[1])
        n += int((np.array(St.cl[z])[idx] != np.array(St.cl[b])[idx]).sum())
    return n, alle


def g7(R, S, plan):
    """G7 (V4 vor Fleck): die Flaechen der Hals-Unterseite (Zone 44) - und aller V4-Zonen 31/42/44/45 samt Lippensaum - lesen kein Fleck-Material
    (Pruef-Stand vor dem Spalt; die Spalt-Kinder folgen der Mutter, Riegel V3.1b), und keiner ihrer Zugriffe ist roeter als die Haut, das
    Fleck-Material eingeschlossen. Gegenprobe: Abgabe der Runde 2 (5 Flaechen mit Zone-27-Fleisch)."""
    Z = TX.lade_zonen()
    zl = R2.zonen_listen(Z)
    v4 = sorted(v4_flaechen(Z))
    z44 = sorted(zl['44'])

    def mess(St):
        n44, a44 = _material_zugriffe(St, z44)
        nv4, av4 = _material_zugriffe(St, v4)
        L, _W = St.farben([k for k in z44 if k in St.fl])
        r = float(roeter(L, S_HAUT_G7).mean() * 100) if len(L) else 0.0
        return n44, a44, nv4, av4, r
    v = mess(S)
    g2 = r2_stand()
    g = mess(stand_mit_namen(os.path.join(g2, 'pruef'), _zeilen_namen(g2)))

    def txt(t):
        return 'Zone 44: %d von %d Zugriffen Fleck-Material, roeter als die Haut %.2f %%; alle V4-Flaechen: %d von %d' % (t[0], t[1], t[4], t[2], t[3])
    R.zeile('G7', 'Hals-Unterseite (Zone 44) ohne Zone-27-Material und nicht roeter als die Haut (C* > s L* + 6, Fleck-Material eingeschlossen); '
            'alle V4-Flaechen ohne Fleck-Material (V4 geht vor)', txt(v), v[0] == 0 and v[2] == 0 and v[4] == 0,
            gegen=('Runde 2 (Commit %s)' % R2_COMMIT, txt(g), g[0] == 0 and g[2] == 0 and g[4] == 0))


def hell_zugriffe(o):
    """G8: Abtastung des Assets (kf_k5g3.abtasten wie K1/K2): Zugriffe mit L* > L_MAX ohne Zahn-Zeile (K5c-Zeile 7, G4-Zaehne), Maul-Zeilen
    (G4-Mundfleisch), Augen-Zeile (O4) und ohne die F1-Kern-Eintraege (bytegleich K5c-Zeile 5, V6). Rueckgabe (Zugriffe, ueber, je Zeile)."""
    A = FE.lese_emd(open(os.path.join(o, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    T = FE.tim_lesen(open(os.path.join(o, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
    cl = [[int(x) for x in z] for z in T['cluts']]
    lab = [TX.lab_zeile(z) for z in cl]
    nam = _zeilen_namen(o)
    kern_e = set(E.f1_kern_eintraege())
    z5 = [int(x) for x in E.eingaenge()['cl_k5c'][5]]
    zz = K3.abtasten(A, T['pixel'], int(T['clut_y']))
    je = {}
    n = 0
    for _f, zi, _x, _y, i in zz:
        nm = nam[zi]
        if nm.startswith(('zahn', 'maul', 'auge')):
            continue
        if 'f1' in nm and i in kern_e and cl[zi][i] == z5[i]:
            continue
        if float(lab[zi][i, 0]) > L_MAX:
            n += 1
            je[nm] = je.get(nm, 0) + 1
    return len(zz), n, je


def g8(R, var):
    nv, hv, jv = hell_zugriffe(var)
    ng, hg, jg = hell_zugriffe(r2_stand())
    R.zeile('G8', 'fast weiss wie reinweiss: kein Zugriff auf einen Eintrag mit L* > %.0f (Haut, Tausch, Fleck-Grund und Fleck-Material; ausser '
            'Zaehnen, Maul, Augen, F1-Kern), ganzes Asset' % L_MAX, '%d von %d Zugriffen %s' % (hv, nv, jv or ''), hv == 0,
            gegen=('Runde 2 (Commit %s)' % R2_COMMIT, '%d Zugriffe %s' % (hg, jg), hg == 0))


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
    ve, vt = sha_d(os.path.join(var, 'BAHNSTEIG_FORM.EMD')), sha_d(os.path.join(var, 'BAHNSTEIG_FORM.TIM'))
    ke, kt = sha_d(os.path.join(K5E, 'BAHNSTEIG_FORM.EMD')), sha_d(os.path.join(K5E, 'BAHNSTEIG_FORM.TIM'))
    R.zeile('G5', 'Spiel-Asset shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM == diese Variante (R9-Einbau; bis dahin K5e_f1 f277916d/31a35e66)',
            'Asset EMD %s TIM %s, Variante EMD %s TIM %s' % (se[:8], st[:8], ve[:8], vt[:8]), se == ve and st == vt,
            gegen=('K5e_f1 (ruhe/K5e_f1)', 'EMD %s TIM %s' % (ke[:8], kt[:8]), se == ke and st == kt))


def hand18(R, var):
    """R9-Einbau: Netz 18 des Assets = spiegelhand(Netz 13 des Assets) - UV/CLUT/Seite Flaeche fuer Flaeche die von 13."""
    def pruef(o):
        f = FE.lese_emd(open(os.path.join(o, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
        soll = G3.netz_norm(G3.spiegelhand(f['meshes'][G3.HAND_LINKS]))
        ist = G3.netz_norm(f['meshes'][G3.HAND])
        geo = G3.netz_norm(FE.lese_emd(open(G3.GEO3_ZIEL, 'rb').read())['meshes'][G3.HAND])
        g_ok = (ist['ecken'], ist['normalen'], [d['e'] for d in ist['dreiecke']], [q['e'] for q in ist['vierecke']]) == (
            geo['ecken'], geo['normalen'], [d['e'] for d in geo['dreiecke']], [q['e'] for q in geo['vierecke']])
        n_uv = sum(1 for a in ('dreiecke', 'vierecke') for x, y in zip(ist[a], soll[a]) if (x['uv'], x['clut'], x['page']) != (y['uv'], y['clut'], y['page']))
        return ist == soll and g_ok, '%d Flaechen, UV/CLUT/Seite anders als 13: %d, Geometrie == Stufe hand18: %s' % (
            len(ist['dreiecke']) + len(ist['vierecke']), n_uv if len(ist['dreiecke']) == len(soll['dreiecke']) else -1, 'ja' if g_ok else 'nein')
    ok, t = pruef(var)
    gok, gt = pruef(r2_stand_r3())
    R.zeile('H18', 'Netz 18 des Assets = spiegelhand(Netz 13 des Assets): Geometrie == Stufe hand18, UV/CLUT/Seite == 13 (R9 N33)', t, ok,
            gegen=('K5g3 Runde 3 (Commit %s, altes Netz 18)' % R3_COMMIT, gt, gok))


R3_COMMIT = '968001097'     # Abgabe K5g3 Runde 3 (EMD 1981f97e, TIM c00823b3) - Gegenprobe H18


def r2_stand_r3():
    """Abgabe der Runde 3 aus git (Commit 968001097) in den Scratchpad holen (einmal)."""
    import subprocess
    d = os.path.join(K3.SCR, 'gegen_r3_' + R3_COMMIT)
    if os.path.exists(os.path.join(d, 'bau.json')):
        return d
    os.makedirs(os.path.join(d, 'pruef'), exist_ok=True)
    for n in K3.DATEIEN + ('bau.json',):
        b = subprocess.run(['git', '-C', E.WT, 'show', '%s:%s/%s' % (R3_COMMIT, R1_PFAD, n)], capture_output=True, check=True).stdout
        open(os.path.join(d, n), 'wb').write(b)
    return d


def main(argv):
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?', default=K3.AUS)
    ap.add_argument('--spiel')
    ap.add_argument('--neu_spiel', action='store_true', help='Spielmessung neu laufen lassen (sonst <Variante>/spiel_z.json)')
    ap.add_argument('--augen_spiel', help='Augen im Spielfenster (Vorgabe <Variante>/spiel_augen.json)')
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
    uv1(R, var, bau)
    tb(R, var)
    kopie(R, var)
    s_haut = float(bau['textur']['haut']['s'])
    je = v3(R, S, bau, plan)
    v31b(R, var, bau)
    R2.v3_fleisch(R, S, je)
    v4(R, S, plan, s_haut)
    v44(R, var, bau)
    v2(R, S, plan)
    weiss_braun(R, S, bau, plan)
    g6(R, S, bau, plan, X)
    g7(R, S, plan)
    g8(R, var)
    v5(R, S, s_haut)
    R2.h1(R, S)
    s1(R, S, var)
    v1(R, S, bau)
    v6(R, S, plan)
    augen(R, var)
    loch(R, var)
    augen_spiel(R, var, a.augen_spiel or os.path.join(var, 'spiel_augen.json'))
    R2.weiss(R, var)
    sp = a.spiel or os.path.join(var, 'spiel_z.json')
    if a.neu_spiel or not os.path.exists(sp):
        spiel_messen(var, sp)
    spiel(R, var, sp)
    technik(R, var, bau)
    hand18(R, var)
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
