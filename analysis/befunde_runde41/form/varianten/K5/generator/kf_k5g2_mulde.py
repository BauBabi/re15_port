#!/usr/bin/env python3
"""K5g2 Runde 2, Orchestrator-Entscheid O1: MULDE in Buckel 10 / Hals 21 fuer Kopf 22 / Unterkiefer 23 (Mulden-Bauer).

Das Spiel malt Dreiecke nach mittlerer Tiefe ohne Tiefenpuffer (render_pc.c Einfuegesortierung nach s_textri_depth). Wo der Kopf in
irgendeinem Bild irgendeines Clips durch eine Flaeche von 10/21 geht, wird diese Flaeche feiner geteilt (kf_k5g2_unterteil.unterteilen)
und ihre Ecken auf der Geraden zum Teil-Schwerpunkt zurueckgenommen, bis sie in ALLEN 799 Bildern mindestens `rand` ausserhalb der
Kopf-/Kieferflaechen liegen (kf_k5g2_geo.zuruecknehmen). Clips, EDD/EMR und KI bleiben unberuehrt; kein Tiefenpuffer, kein Engine-Eingriff.

Quelle und Schritte:
  Stufe 1 Netze 10, 21 (Mulde, 3 Runden) und 22 (Hinterkopf-Ecken zurueckgenommen, Runde 1) - Orchestrator-Messung mulde2.py Lauf a,
          uebernommen als k5g2_mulde_stufe1.json (Rechenzeit 77 min, hier NICHT neu gerechnet; `mulde_runden` haelt denselben Rechenweg
          fest: Auswahl -> Unterteilung -> Zuruecknehmen). `pruef huelle` rechnet die Huell-Bedingung der Datei ueber alle 799 Bilder nach.
  tabelle Textur auf der Mulde (O1 vii, `textur_neu`) + fertige Netze 10, 16, 21, 22          -> k5g2_mulde_tabelle.json
  emd     K5e_f1 + nur diese Netze                                                          -> k5g/K5g2/geo/MULDE.EMD
  pruef   Tabelle und EMD ein zweites Mal bauen und byteweise vergleichen; Netze ausserhalb der Tabelle gleich K5e_f1.

Schulterkugel 16 und Unterkiefer 23 bleiben die Netze von K5e_f1 (gemessen und begruendet in k5g/K5g2/GEOMETRIE.md): 16 steht
unveraendert in der Tabelle, 23 steht nicht darin. Unabhaengig von den Krallen: die Netze 19, 20, 28, 29, 31 werden nie angefasst.
Aufruf: python kf_k5g2_mulde.py tabelle | emd | pruef [huelle]
"""
import copy
import hashlib
import json
import os
import sys
import time

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g2_unterteil as U  # noqa: E402

FE = G.FE
K5 = G.K5
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
EMD0 = os.path.join(K5E, 'BAHNSTEIG_FORM.EMD')
TIM0 = os.path.join(K5E, 'BAHNSTEIG_FORM.TIM')
EMD0_SHA = 'f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1'
STUFE1 = os.path.join(HIER, 'k5g2_mulde_stufe1.json')
TABELLE = os.path.join(HIER, 'k5g2_mulde_tabelle.json')
ZIEL = os.path.join(K5, 'k5g', 'K5g2', 'geo')
KOPF = (22, 23)
# Rand wie Runde 1 (k5g2_par.json geometrie.rand = 16 Einheiten = 1/103 der Kopfhoehe 1656; kleinster Rand, mit dem die gerundeten Ecken in
# allen Bildern ausserhalb bleiben).
RAND = 16.0
RUNDEN = 3  # wie mulde2.py Lauf a (3 Runden Auswahl -> Unterteilung -> Zuruecknehmen)
# Probenpunkte je Dreieck fuer die Auswahl (Mitte, Kantenmitten, 3 Innenpunkte) - mulde2.py BARY
BARY = [(1 / 3, 1 / 3, 1 / 3), (.5, .5, 0), (0, .5, .5), (.5, 0, .5), (.6, .2, .2), (.2, .6, .2), (.2, .2, .6)]


def sha(pfad):
    return hashlib.sha256(open(pfad, 'rb').read()).hexdigest()


def sha_lf(pfad):
    """sha256 einer Textdatei mit LF-Zeilenenden (core.autocrlf legt sie im Arbeitsbaum mit CRLF ab - der Inhalt ist derselbe)."""
    return hashlib.sha256(open(pfad, 'rb').read().replace(bytes([13, 10]), bytes([10]))).hexdigest()


def grundform():
    """K5e_f1 als form-Woerterbuch (Basis aller Stufen; Krallen bleiben K5e_f1)."""
    emd0 = open(EMD0, 'rb').read()
    assert hashlib.sha256(emd0).hexdigest() == EMD0_SHA, 'K5e_f1-EMD hat einen anderen Stand'
    return emd0, FE.lese_emd(emd0)


def posen_von(emd0, form):
    mo = FE.modell(G.emd_mit_md1(emd0, form), open(TIM0, 'rb').read())
    return G.posen(mo)


# ------------------------------------------------------------------ Stufen-Dateien
def netz_json(mesh):
    def fl(f):
        return dict(e=[int(x) for x in f['e']], n=[int(x) for x in f['n']], uv=[[int(a), int(b)] for a, b in f['uv']], clut=int(f['clut']),
                    page=int(f['page']))
    return dict(ecken=[[int(x) for x in e] for e in mesh['ecken']], normalen=[[int(x) for x in e] for e in mesh['normalen']],
                dreiecke=[fl(f) for f in mesh['dreiecke']], vierecke=[fl(f) for f in mesh['vierecke']])


def stufe_schreiben(pfad, form, hk, teile, kopf):
    """Netze der `teile` + Abstammung (Flaeche -> Mutterflaeche in K5e_f1) in fester Reihenfolge, kompaktes JSON mit sortierten Schluesseln."""
    d = dict(kopf)
    d['teile'] = {str(t): netz_json(form['meshes'][t]) for t in sorted(teile)}
    d['herkunft'] = [[int(a), int(b), int(c)] + [int(x) for x in hk[(a, b, c)]] for (a, b, c) in sorted(k for k in hk if k[0] in teile)]
    with open(pfad, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(d, f, ensure_ascii=True, sort_keys=True, separators=(',', ':'))
        f.write('\n')
    return sha(pfad)


def stufe_lesen(pfad, form):
    """Netze aus der Stufen-Datei in `form` einsetzen; Rueckgabe (Kopfdaten, hk)."""
    d = json.load(open(pfad, encoding='utf-8'))
    for t, netz in d['teile'].items():
        form['meshes'][int(t)] = copy.deepcopy(netz)
    hk = {(a, b, c): (x, y, z) for a, b, c, x, y, z in d['herkunft']}
    return d, hk


# ------------------------------------------------------------------ Mulde (Auswahl -> Unterteilung -> Zuruecknehmen)
def flaechen_liste(mesh):
    out = [(0, i) for i in range(len(mesh['dreiecke']))]
    for qi in range(len(mesh['vierecke'])):
        out += [(1, qi), (1, qi)]
    return out


def auswahl(form, R, T, HK, teile, schwelle=0.0, bilder=None):
    """Flaechen der `teile`, bei denen ein Probenpunkt in irgendeinem Bild tiefer als `schwelle` im Kopf/Kiefer liegt (mulde2.py)."""
    sel = {}
    for hb in teile:
        mesh = form['meshes'][hb]
        idx = G.dreiecke_idx(mesh)
        fl = flaechen_liste(mesh)
        E = np.array(mesh['ecken'], np.int64)
        tief = np.full(len(idx), -1e9)
        for i in (range(len(R)) if bilder is None else bilder):
            Tr = G.welt(R[i], T[i], hb, E)[idx]
            P = np.concatenate([a * Tr[:, 0] + b * Tr[:, 1] + c * Tr[:, 2] for a, b, c in BARY])
            tief = np.maximum(tief, HK.tiefe_max(i, P).reshape(len(BARY), -1).max(0))
        sel[hb] = set(fl[int(k)] for k in np.nonzero(tief > schwelle)[0])
    return sel


def kopf_in(form, R, T, teile):
    """Kopf-/Kieferecken in den `teile` ueber alle Bilder: (Ecken mit Tiefe > 0, groesste Tiefe, Bildindex) je Kopfteil."""
    H = G.Huellen(form, R, T, teile=tuple(teile))
    aus = {}
    for t in KOPF:
        D = G.eindringen(form, R, T, t, huellen=H)
        mx = D.max(0)
        aus[t] = (int((mx > 0).sum()), round(float(mx.max()), 1), int(D.max(1).argmax()))
    return aus


def mulde_runden(form, hk, R, T, teile, runden=RUNDEN, rand=RAND, log=print, zwischen=None, bilder=None, weg_max=None, anker_fest=None):
    """Die Rechnung aus mulde2.py fuer beliebige Teile: je Runde Auswahl -> Unterteilung -> Zuruecknehmen zum Schwerpunkt des Teils (Anker
    = Mittel der Ecken VOR der ersten Runde). `bilder`: Bildindizes der Bedingung (None = alle). `weg_max`: Ecken, deren Weg groesser
    waere, bleiben stehen (None = keine Grenze). Veraendert `form`; Rueckgabe (hk, Anker, Protokoll)."""
    t0 = time.time()
    anker = {t: np.array(form['meshes'][t]['ecken'], float).mean(0) for t in teile}
    if anker_fest:
        anker.update({t: np.asarray(v, float) for t, v in anker_fest.items()})   # Anker ausserhalb des Kopf-Schwenks (Teil 16)
    log('Anker %s' % {t: anker[t].round(1).tolist() for t in teile})
    if bilder is not None:
        R, T = R[bilder], T[bilder]
    HK = G.Huellen(form, R, T, teile=KOPF)
    proto = []
    for k in range(1, runden + 1):
        sel = auswahl(form, R, T, HK, teile)
        log('Runde %d Auswahl %s  %.0f s' % (k, {t: len(v) for t, v in sel.items()}, time.time() - t0))
        sel = {t: v for t, v in sel.items() if v}
        if sel:
            hk = U.unterteilen(form, sel, hk)
        for teil in teile:
            alt = np.array(form['meshes'][teil]['ecken'], np.int64)
            neu, pr, betr = G.zuruecknehmen(form, R, T, teil, set(), anker[teil], rand, huellen=HK)
            if weg_max is not None:
                for j, v in pr.items():
                    if 'weg' in v and v['weg'] > weg_max:
                        neu[j] = alt[j]
                        v['status'] = 'Weg ueber der Grenze, bleibt'
                        del v['weg']
            w = [v['weg'] for v in pr.values() if 'weg' in v]
            nicht = [j for j, v in pr.items() if 'status' in v]
            form['meshes'][teil]['ecken'] = [[int(x) for x in e] for e in neu]
            z = dict(runde=k, teil=teil, ecken=len(neu), bewegt=len(w), weg_max=max(w) if w else 0.0, weg_median=float(np.median(w)) if w else 0.0,
                     bleibt=len(nicht), dreiecke=len(form['meshes'][teil]['dreiecke']) + 2 * len(form['meshes'][teil]['vierecke']))
            proto.append(z)
            log('  %s  %.0f s' % (z, time.time() - t0))
        if zwischen:
            zwischen(k, form, hk)
    return hk, anker, proto


def arbeitsform_stufe1():
    """K5e_f1 + Netze 10/21/22 der Stufe 1."""
    emd0, form = grundform()
    kopf, hk = stufe_lesen(STUFE1, form)
    return emd0, form, hk


# ------------------------------------------------------------------ Textur auf der Mulde (O1 vii)
def port_dreiecke(mesh):
    """[(Schluessel (viereck, fi), Ecke im Flaechen-Eintrag [3])] - die Dreiecke, die der Port zeichnet (Viereck = (0,1,3), (0,3,2))."""
    out = [((0, fi), (0, 1, 2)) for fi in range(len(mesh['dreiecke']))]
    for fi in range(len(mesh['vierecke'])):
        out += [((1, fi), (0, 1, 3)), ((1, fi), (0, 3, 2))]
    return out


def jacobi(P, Q):
    """Abbildung Textur -> Raum eines Dreiecks (3x2); None bei entarteter Texturflaeche (< 0.5 Texel^2)."""
    A = np.stack([Q[1] - Q[0], Q[2] - Q[0]], 1)
    if abs(np.linalg.det(A)) < 1.0:
        return None
    return np.stack([P[1] - P[0], P[2] - P[0]], 1) @ np.linalg.inv(A)


def dehnung(mesh):
    """Je Port-Dreieck: (Schluessel, laengste, kuerzeste Raumlaenge je Texel, Verzug) - Singulaerwerte der Jacobi-Matrix; entartet -> None."""
    E = np.array(mesh['ecken'], float)
    out = []
    for (vi, fi), ix in port_dreiecke(mesh):
        f = mesh['vierecke' if vi else 'dreiecke'][fi]
        J = jacobi(E[[f['e'][i] for i in ix]], np.array([f['uv'][i] for i in ix], float))
        if J is None:
            out.append(((vi, fi), None, None, None))
            continue
        s = np.linalg.svd(J, compute_uv=False)
        out.append(((vi, fi), float(s[0]), float(s[1]), float(s[0] / max(s[1], 1e-9))))
    return out


def mutter_stuecke(mesh, hk, teil):
    """Mutterflaeche -> Liste ihrer Kinder (viereck, fi) im aktuellen Netz, nur fuer geteilte Muttern (mehr als ein Kind)."""
    st = {}
    for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
        for fi in range(len(mesh[art])):
            st.setdefault(tuple(hk[(teil, vi, fi)]), []).append((vi, fi))
    return {ur: sorted(v) for ur, v in st.items() if len(v) > 1}


def _mvc_gewichte(P, tris, innen, nachbarn):
    """Mittelwert-Koordinaten (Floater) am verformten Netz: w_ij = (tan(a/2) + tan(b/2)) / |pi - pj|, a/b = Winkel bei i in den Dreiecken
    an der Kante ij. Positive Gewichte -> Einbettung in ein konvexes Randpolygon ohne Faltung (Tutte)."""
    w = {}
    for t in tris:
        for k in range(3):
            i, j, l = t[k], t[(k + 1) % 3], t[(k + 2) % 3]
            if i not in innen:
                continue
            a, b = P[j] - P[i], P[l] - P[i]
            la, lb = np.linalg.norm(a), np.linalg.norm(b)
            if la < 1e-9 or lb < 1e-9:
                continue
            c = np.clip(np.dot(a, b) / (la * lb), -1.0, 1.0)
            th = np.tan(0.5 * np.arccos(c))
            w[(i, j)] = w.get((i, j), 0.0) + th / la
            w[(i, l)] = w.get((i, l), 0.0) + th / lb
    return w


def stueck_uv(mesh0, mesh, ur, kinder):
    """Neue Texturlage fuer die Kinder EINER Mutterflaeche `ur` (K5e_f1-Netz mesh0). Rand: die Kinder-Ecken auf jeder Mutterkante liegen
    zwischen den beiden Mutter-Ecken, verteilt nach der Bogenlaenge der VERFORMTEN Kante; innen: Mittelwert-Koordinaten am verformten Netz.
    Jede neue Lage ist eine konvexe Mischung der Mutter-UV -> kein Texel ausserhalb der konvexen Huelle der Mutter-Texturflaeche.
    Rueckgabe {Ecke: [u, v]} (Gleitkomma) oder None, wenn das Stueck keine Scheibe mit den Mutter-Ecken am Rand ist."""
    vi0, fi0 = ur[1], ur[2]
    m = mesh0['vierecke' if vi0 else 'dreiecke'][fi0]
    ring = (0, 1, 3, 2) if vi0 else (0, 1, 2)
    ecken_m = [m['e'][i] for i in ring]
    uv_m = [np.array(m['uv'][i], float) for i in ring]
    P = np.array(mesh['ecken'], float)
    tris = []
    for vi, fi in kinder:
        f = mesh['vierecke' if vi else 'dreiecke'][fi]
        for ix in (((0, 1, 3), (0, 3, 2)) if vi else ((0, 1, 2),)):
            tris.append(tuple(f['e'][i] for i in ix))
    kanten = {}
    for t in tris:
        for k in range(3):
            a, b = t[k], t[(k + 1) % 3]
            kanten.setdefault((min(a, b), max(a, b)), []).append((a, b))
    rand = {}
    for (a, b), l in kanten.items():
        if len(l) == 1:
            x, y = l[0]
            rand.setdefault(x, []).append(y)
            rand.setdefault(y, []).append(x)
        elif len(l) > 2:
            return None
    if any(len(v) != 2 for v in rand.values()) or any(e not in rand for e in ecken_m) or len(set(ecken_m)) != len(ecken_m):
        return None
    # Randschleife ab Mutter-Ecke 0 in Richtung Mutter-Ecke 1
    uv = {}
    n = len(ecken_m)
    for k in range(n):
        a, b = ecken_m[k], ecken_m[(k + 1) % n]
        andere = set(ecken_m) - {a, b}
        kette = None
        for start in rand[a]:
            pfad, vor, jetzt = [a], a, start
            while jetzt != b and jetzt not in andere and len(pfad) <= len(rand):
                pfad.append(jetzt)
                nx = [x for x in rand[jetzt] if x != vor]
                vor, jetzt = jetzt, nx[0]
            if jetzt == b:
                kette = pfad + [b]
                break
        if kette is None:
            return None
        s = np.concatenate([[0.0], np.cumsum([np.linalg.norm(P[kette[i + 1]] - P[kette[i]]) for i in range(len(kette) - 1)])])
        for i, e in enumerate(kette):
            lam = s[i] / s[-1] if s[-1] > 0 else i / (len(kette) - 1.0)
            uv[e] = uv_m[k] + lam * (uv_m[(k + 1) % n] - uv_m[k])
    if len(uv) != len(rand):
        return None
    alle = sorted(set(e for t in tris for e in t))
    innen = [e for e in alle if e not in uv]
    if innen:
        pos = {e: i for i, e in enumerate(innen)}
        w = _mvc_gewichte(P, tris, set(innen), None)
        A = np.zeros((len(innen), len(innen)))
        B = np.zeros((len(innen), 2))
        for (i, j), g in w.items():
            A[pos[i], pos[i]] += g
            if j in pos:
                A[pos[i], pos[j]] -= g
            else:
                B[pos[i]] += g * uv[j]
        X = np.linalg.solve(A, B)
        for e, i in pos.items():
            uv[e] = X[i]
    return uv


def _konvex(uv_ring):
    """Texturpolygon der Mutter konvex und nicht entartet? (Vorzeichen aller Kreuzprodukte gleich, Flaeche >= 1 Texel^2)"""
    n = len(uv_ring)
    z = []
    for i in range(n):
        a, b, c = uv_ring[i], uv_ring[(i + 1) % n], uv_ring[(i + 2) % n]
        z.append((b[0] - a[0]) * (c[1] - b[1]) - (b[1] - a[1]) * (c[0] - b[0]))
    z = np.array(z)
    return bool((np.all(z >= 0) or np.all(z <= 0)) and abs(z).sum() >= 2.0)


def textur_neu(mesh0, mesh, hk, teil, log=None):
    """O1 (vii): Texturlage aller Kinder der geteilten Mutterflaechen von `teil` neu verteilen (stueck_uv), auf ganze Texel gerundet.
    Muttern mit nicht konvexer / entarteter Texturflaeche und Stuecke, die keine Scheibe sind, bleiben wie geerbt (gezaehlt).
    Veraendert mesh; Rueckgabe: Bericht mit der Liste der Flaechen, deren UV sich gegenueber der geerbten Lage aendert."""
    st = mutter_stuecke(mesh, hk, teil)
    ber = dict(muttern=len(st), neu=0, nicht_konvex=[], keine_scheibe=[], flaechen_uv_neu=[])
    for ur in sorted(st):
        vi0, fi0 = ur[1], ur[2]
        m = mesh0['vierecke' if vi0 else 'dreiecke'][fi0]
        ring = (0, 1, 3, 2) if vi0 else (0, 1, 2)
        if not _konvex([m['uv'][i] for i in ring]):
            ber['nicht_konvex'].append('%d|%d|%d' % ur)
            continue
        uv = stueck_uv(mesh0, mesh, ur, st[ur])
        if uv is None:
            ber['keine_scheibe'].append('%d|%d|%d' % ur)
            continue
        ber['neu'] += 1
        for vi, fi in st[ur]:
            f = mesh['vierecke' if vi else 'dreiecke'][fi]
            neu = [[int(np.floor(uv[e][0] + 0.5)), int(np.floor(uv[e][1] + 0.5))] for e in f['e']]
            if neu != [list(x) for x in f['uv']]:
                f['uv'] = neu
                ber['flaechen_uv_neu'].append([vi, fi])
    ber['flaechen_uv_neu'].sort()
    if log:
        log('Teil %d: geteilte Muttern %d, neu verteilt %d, nicht konvex %d, keine Scheibe %d, Flaechen mit neuer UV %d' % (
            teil, ber['muttern'], ber['neu'], len(ber['nicht_konvex']), len(ber['keine_scheibe']), len(ber['flaechen_uv_neu'])))
    return ber


def dehnung_bericht(mesh0, mesh, hk, teil):
    """Zahlen fuer O1 (vii) Mass 1: Verzug und laengste Raumlaenge je Texel der Port-Dreiecke; Bezug = dieselben Zahlen auf K5e_f1 (alle
    Flaechen des Teils) und auf den UNGETEILTEN Flaechen des fertigen Netzes; dazu die Dehnung gegen die Mutterflaeche (groesste Streckung
    einer Texturrichtung gegenueber der Mutter, kleinster Wert ueber die Port-Dreiecke der Mutter)."""
    def q(v):
        v = np.array([x for x in v if x is not None], float)
        if not len(v):
            return dict(n=0)
        return dict(n=int(len(v)), median=round(float(np.median(v)), 2), p90=round(float(np.percentile(v, 90)), 2),
                    p99=round(float(np.percentile(v, 99)), 2), max=round(float(v.max()), 2))
    d0 = dehnung(mesh0)
    d1 = dehnung(mesh)
    st = mutter_stuecke(mesh, hk, teil)
    geteilt = set(k for v in st.values() for k in v)
    E0 = np.array(mesh0['ecken'], float)
    E1 = np.array(mesh['ecken'], float)
    J0 = {}
    for (vi, fi), ix in port_dreiecke(mesh0):
        f = mesh0['vierecke' if vi else 'dreiecke'][fi]
        J0.setdefault((vi, fi), []).append(jacobi(E0[[f['e'][i] for i in ix]], np.array([f['uv'][i] for i in ix], float)))
    rel = []
    for (vi, fi), ix in port_dreiecke(mesh):
        if (vi, fi) not in geteilt:
            continue
        f = mesh['vierecke' if vi else 'dreiecke'][fi]
        J = jacobi(E1[[f['e'][i] for i in ix]], np.array([f['uv'][i] for i in ix], float))
        if J is None:
            continue
        best = None
        for Jm in J0[tuple(hk[(teil, vi, fi)])[1:]]:
            if Jm is None:
                continue
            L = np.linalg.cholesky(Jm.T @ Jm)
            s = np.linalg.svd(J @ np.linalg.inv(L.T), compute_uv=False)
            if best is None or s[0] < best[0]:
                best = (float(s[0]), float(s[1]))
        if best:
            rel.append(best)
    rel = np.array(rel) if rel else np.zeros((0, 2))
    return dict(
        k5e_f1=dict(port_dreiecke=len(d0), uv_entartet=sum(1 for x in d0 if x[1] is None), verzug=q([x[3] for x in d0]), laengste=q([x[1] for x in d0])),
        ungeteilt=dict(port_dreiecke=sum(1 for x in d1 if x[0] not in geteilt), verzug=q([x[3] for x in d1 if x[0] not in geteilt]),
                       laengste=q([x[1] for x in d1 if x[0] not in geteilt])),
        mulde=dict(port_dreiecke=sum(1 for x in d1 if x[0] in geteilt), uv_entartet=sum(1 for x in d1 if x[0] in geteilt and x[1] is None),
                   verzug=q([x[3] for x in d1 if x[0] in geteilt]), laengste=q([x[1] for x in d1 if x[0] in geteilt]),
                   gegen_mutter_laengste=q(rel[:, 0]), gegen_mutter_ueber=dict((str(g), int((rel[:, 0] > g).sum())) for g in (1.5, 2, 4, 8)),
                   gegen_mutter_kuerzeste_min=round(float(rel[:, 1].min()), 3) if len(rel) else None))


# ------------------------------------------------------------------ Tabelle, EMD, Pruefung
TEILE_TAB = (10, 16, 21, 22)   # 23 (Unterkiefer) bleibt K5e_f1 und steht deshalb nicht in der Tabelle (GEOMETRIE.md Abschnitt Unterkiefer)
TEILE_TEXTUR = (10, 21)        # Teile mit Mulde (16 und 23 bleiben K5e_f1)


def arbeitsform():
    """K5e_f1 + Stufe 1. Rueckgabe (emd0, form0, form, hk, Quellen)."""
    emd0, form0 = grundform()
    form = copy.deepcopy(form0)
    kopf, hk = stufe_lesen(STUFE1, form)
    quellen = [dict(datei=os.path.basename(STUFE1), sha256_lf=sha_lf(STUFE1))]
    # Flaechen ohne Eintrag sind ihre eigene Mutter (Teil 16, ungeteilte Flaechen)
    for t in TEILE_TAB:
        for vi, fi in flaechen_liste(form['meshes'][t]):
            hk.setdefault((t, vi, fi), (t, vi, fi))
    return emd0, form0, form, hk, quellen


def tabelle_bauen(log=print):
    """Die fertigen Netze: Stufen einsetzen, Textur auf der Mulde neu verteilen. Rueckgabe: Tabelle als Woerterbuch (ohne Zeit, ohne Pfade)."""
    emd0, form0, form, hk, quellen = arbeitsform()
    teile_tex = [t for t in TEILE_TAB if t in TEILE_TEXTUR]
    tab = dict(zweck='K5g2 Runde 2 O1: Mulde in 10/21 fuer Kopf 22/Unterkiefer 23 - fertige Netze, unabhaengig von den Krallen',
               basis=dict(emd='K5/ruhe/K5e_f1/BAHNSTEIG_FORM.EMD', sha256=EMD0_SHA), rand=RAND, runden=RUNDEN, quellen=quellen,
               uv_neu={}, textur={}, dehnung={})
    for t in teile_tex:
        vor = dehnung_bericht(form0['meshes'][t], form['meshes'][t], hk, t)
        ber = textur_neu(form0['meshes'][t], form['meshes'][t], hk, t, log=log)
        nach = dehnung_bericht(form0['meshes'][t], form['meshes'][t], hk, t)
        tab['uv_neu'][str(t)] = ber.pop('flaechen_uv_neu')
        tab['textur'][str(t)] = ber
        tab['dehnung'][str(t)] = dict(k5e_f1=nach['k5e_f1'], ungeteilt=nach['ungeteilt'], mulde_geerbt=vor['mulde'], mulde_neu=nach['mulde'])
    tab['teile'] = {str(t): netz_json(form['meshes'][t]) for t in TEILE_TAB}
    tab['herkunft'] = [[int(a), int(b), int(c)] + [int(x) for x in hk[(a, b, c)]] for (a, b, c) in sorted(k for k in hk if k[0] in TEILE_TAB)]
    tab['zahl'] = {str(t): dict(ecken=len(form['meshes'][t]['ecken']), dreiecke=len(form['meshes'][t]['dreiecke']),
                                vierecke=len(form['meshes'][t]['vierecke']),
                                port_dreiecke=len(form['meshes'][t]['dreiecke']) + 2 * len(form['meshes'][t]['vierecke'])) for t in TEILE_TAB}
    return tab


def tabelle_text(tab):
    return json.dumps(tab, ensure_ascii=True, sort_keys=True, separators=(',', ':')) + '\n'


def tabelle_schreiben(log=print):
    txt = tabelle_text(tabelle_bauen(log))
    with open(TABELLE, 'w', encoding='utf-8', newline='\n') as f:
        f.write(txt)
    log('Tabelle %s  %d Byte  sha256 (LF) %s' % (TABELLE.replace(chr(92), '/'), len(txt), sha_lf(TABELLE)))


def emd_aus_tabelle(ziel=None, log=print):
    """K5e_f1 + nur die Netze der Tabelle -> MULDE.EMD (laeuft mit der TIM von K5e_f1; Clips/EDD/EMR byte-gleich K5e_f1)."""
    emd0, form = grundform()
    tab = json.load(open(TABELLE, encoding='utf-8'))
    assert tab['basis']['sha256'] == EMD0_SHA
    for t, netz in tab['teile'].items():
        form['meshes'][int(t)] = netz
    aus = G.emd_mit_md1(emd0, form)
    ziel = ziel or os.path.join(ZIEL, 'MULDE.EMD')
    os.makedirs(os.path.dirname(ziel), exist_ok=True)
    with open(ziel, 'wb') as f:
        f.write(aus)
    log('EMD %s  %d Byte  sha256 %s' % (ziel.replace(chr(92), '/'), len(aus), sha(ziel)))
    return ziel


def huelle_pruefen(log=print, teile=(10, 21)):
    """Bedingung der Mulde nachrechnen: jede Ecke der Teile liegt in allen 799 Bildern mindestens RAND ausserhalb von Kopf/Unterkiefer."""
    emd0, form = grundform()
    tab = json.load(open(TABELLE, encoding='utf-8'))
    for t, netz in tab['teile'].items():
        form['meshes'][int(t)] = netz
    cb, R, T = posen_von(emd0, form)
    HK = G.Huellen(form, R, T, teile=KOPF)
    gut = True
    for t in teile:
        D = G.eindringen(form, R, T, t, huellen=HK)
        mx = float(D.max()); i, j = np.unravel_index(int(D.argmax()), D.shape)
        gut &= mx <= -RAND
        log('Teil %d: %d Ecken, groesste Tiefe im Kopf %.1f (Clip %d Bild %d, Ecke %d) -> %s' % (
            t, D.shape[1], mx, cb[i][0], cb[i][1], j, 'ausserhalb mit Rand' if mx <= -RAND else 'NICHT ausserhalb'))
    return gut


def pruefen(log=print, mit_huelle=False):
    """Zweiter Lauf byte-gleich? Tabelle und EMD neu erzeugen und mit den Dateien vergleichen."""
    txt = tabelle_text(tabelle_bauen(lambda *a: None))
    alt = open(TABELLE, encoding='utf-8', newline='').read().replace(chr(13) + chr(10), chr(10))
    g1 = txt == alt
    log('Tabelle neu erzeugt: %s (sha256 LF %s)' % ('byte-gleich' if g1 else 'WEICHT AB', sha_lf(TABELLE)))
    ziel = os.path.join(ZIEL, 'MULDE.EMD')
    alt_emd = open(ziel, 'rb').read()
    emd0, form = grundform()
    for t, netz in json.loads(txt)['teile'].items():
        form['meshes'][int(t)] = netz
    neu = G.emd_mit_md1(emd0, form)
    g2 = neu == alt_emd
    log('MULDE.EMD neu erzeugt: %s (sha256 Datei %s)' % ('byte-gleich' if g2 else 'WEICHT AB', sha(ziel)))
    # Clips unveraendert: alles ausser dem MD1-Abschnitt ist K5e_f1
    f0 = FE.lese_emd(emd0); f1 = FE.lese_emd(alt_emd)
    g3 = all(f0['meshes'][t] == f1['meshes'][t] for t in range(len(f0['meshes'])) if t not in TEILE_TAB)
    log('Netze ausserhalb %s gleich K5e_f1: %s' % (list(TEILE_TAB), g3))
    g4 = huelle_pruefen(log) if mit_huelle else True
    return g1 and g2 and g3 and g4


if __name__ == '__main__':
    was = sys.argv[1] if len(sys.argv) > 1 else ''
    if was == 'tabelle':
        tabelle_schreiben()
    elif was == 'emd':
        emd_aus_tabelle()
    elif was == 'pruef':
        sys.exit(0 if pruefen(mit_huelle='huelle' in sys.argv[2:]) else 1)
    else:
        print(__doc__)
