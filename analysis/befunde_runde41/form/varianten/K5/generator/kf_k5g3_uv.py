#!/usr/bin/env python3
"""K5g3 Geometrie-Stufe, Runde 2: UV-Stufe 'uvfein' fuer die Spalt-Kinder (Netze 10, 16, 21, 22 der B4-Tabelle).

Befund (Pruefung K5g3 Runde 1, Sicht-Pruefer Punkt 5): "gestreckte Farbstreifen an Kopf und Kiefer aus den UV-entarteten Dreiecken von
Netz 22". Gemessen (Runde 2, GEOMETRIE.md Abschnitt 9): B4 rundet die UV jeder neuen Ecke einzeln (rint der baryzentrischen Mischung der
Mutter-UV, kand_b4/bau.py attribute()). Bei kleinen Kindern faellt das UV-Dreieck dadurch auf eine LINIE (Flaeche 0 bei zwei verschiedenen
Ecken): das Dreieck zeigt dann nur die Texel entlang dieser Linie, quer gestreckt. K5e_f1 hat in den Netzen 10/16/21/22 31 solche
Flaechen, B4 1575 (22: 1462). Ausserdem liegen 2711 Kinder mit mindestens einer Ecke ausserhalb der UV-Flaeche ihrer Mutter.

Regel (je Kind, ganzzahlig im Ergebnis, deterministisch; Ecken, Normalen, Flaechen-Indizes, CLUT und Seite bleiben die der B4-Tabelle):
  X      = exakte UV jeder Kind-Ecke: baryzentrisch aus dem Mutter-Dreieck wie die Port-Zeichnung es teilt (Viereck (0,1,3) + (0,3,2),
           main.c npc_qz1/npc_qz2, form_buehne.py dreiecke()), Projektion der Ecke in die Ebene, Teildreieck mit groesster kleinster
           Gewichtung, Gewichte auf [0,1] geklemmt (wie kand_b4/bau.py attribute()); eine Ecke, die eine Mutterecke ist, behaelt deren UV.
  gebiet = die geschlossene UV-Flaeche der Mutter (Vereinigung ihrer Render-Dreiecke), ganzzahlig geprueft.
  schlecht(U) = |Flaeche(X)| > 0,1 Texel^2 und Flaeche(U) == 0 oder anderes Vorzeichen als X; sonst (winziges Kind): U ist eine Linie.
  Ein Kind bleibt, wie B4 es hat, wenn alle Ecken im gebiet liegen und es nicht schlecht ist. Sonst wird je Ecke unter den ganzzahligen
  Punkten im gebiet (Fenster 2 x 2 um X, falls noetig 4 x 4; leer -> der naechste Punkt im gebiet) die Kombination gewaehlt mit
  Schluessel (schlecht, Fehlrate, Abstand^2 zu X, U): Fehlrate = Anteil der 153 baryzentrischen Proben (Raster 16), deren Texel
  floor(UV) von dem der exakten Abbildung abweicht (der Port tastet floor ab: form_buehne.py rastern, astype(int)).
  Ausnahme: ist im Fenster nur eine schlechte Wahl im gebiet moeglich (duenne Stelle der Mutter), das B4-Kind aber nicht schlecht, bleibt
  das B4-Kind (lieber ausserhalb als eine neue Linie; Bericht 'aussen, im Gebiet nur Linie -> B4 bleibt').
Ergebnis: Kinder lesen innerhalb der UV-Flaeche ihrer Mutter (bis auf die Ausnahme), und es entstehen keine neuen Linien-Flaechen.

  python kf_k5g3_uv.py [<tabelle.json>]   -> Bericht auf stdout (Rechnung im Speicher, schreibt nichts)
"""
import copy
import itertools
import json
import os
import sys

sys.dont_write_bytecode = True
import numpy as np  # noqa: E402

HIER = os.path.dirname(os.path.abspath(__file__))
ART = ('dreiecke', 'vierecke')
GRENZE_FLAECHE = 0.1          # Texel^2: darunter ist schon das exakte Kind kleiner als ein Texel (dann zaehlt nur "keine Linie")


def _raster(n):
    w = []
    for i in range(n + 1):
        for j in range(n + 1 - i):
            w.append((i / n, j / n, 1 - i / n - j / n))
    return np.array(w)


PROBEN = _raster(16)


def render_dreiecke(f, art):
    """UV-Dreiecke einer Flaeche so, wie der Port sie zeichnet (Viereck (0,1,3) + (0,3,2))."""
    u = [tuple(int(x) for x in p) for p in f['uv']]
    return [(u[0], u[1], u[2])] if art == 0 else [(u[0], u[1], u[3]), (u[0], u[3], u[2])]


def _bary(p, A):
    v0, v1, v2 = A[1] - A[0], A[2] - A[0], p - A[0]
    d00, d01, d11 = v0 @ v0, v0 @ v1, v1 @ v1
    d20, d21 = v2 @ v0, v2 @ v1
    den = d00 * d11 - d01 * d01
    if abs(den) < 1e-12:
        return np.array([1.0, 0, 0])
    v = (d11 * d20 - d01 * d21) / den
    w = (d00 * d21 - d01 * d20) / den
    return np.array([1 - v - w, v, w])


def flaeche2(U):
    (a, b), (c, d), (e, f) = U
    return (c - a) * (f - b) - (e - a) * (d - b)


def linie(U):
    U = [tuple(p) for p in U]
    return flaeche2(U) == 0 and len(set(U)) > 1


def im_gebiet(p, tris):
    """p (ganzzahlig) liegt in einem der (ganzzahligen) Dreiecke, Rand eingeschlossen; entartete Dreiecke = ihre Strecken."""
    px, py = p
    for (ax, ay), (bx, by), (cx, cy) in tris:
        d = (bx - ax) * (cy - ay) - (cx - ax) * (by - ay)
        c1 = (bx - ax) * (py - ay) - (px - ax) * (by - ay)
        c2 = (cx - bx) * (py - by) - (px - bx) * (cy - by)
        c3 = (ax - cx) * (py - cy) - (px - cx) * (ay - cy)
        if d != 0:
            if (c1 >= 0 and c2 >= 0 and c3 >= 0) or (c1 <= 0 and c2 <= 0 and c3 <= 0):
                return True
        else:
            for (x1, y1), (x2, y2), cr in (((ax, ay), (bx, by), c1), ((bx, by), (cx, cy), c2), ((cx, cy), (ax, ay), c3)):
                if cr == 0 and min(x1, x2) <= px <= max(x1, x2) and min(y1, y2) <= py <= max(y1, y2):
                    return True
    return False


def fehlrate(U, X):
    U = np.asarray(U, float)
    return float((np.floor(PROBEN @ U + 1e-9) != np.floor(PROBEN @ X + 1e-9)).any(1).mean())


class Muetter:
    def __init__(self, basis):
        self.basis, self.c = basis, {}

    def get(self, mt, ma, mi):
        k = (mt, ma, mi)
        if k not in self.c:
            m = self.basis['meshes'][mt]
            f = m[ART[ma]][mi]
            E = np.array(m['ecken'], float)
            hs = ((0, 1, 2),) if ma == 0 else ((0, 1, 3), (0, 3, 2))
            self.c[k] = (f, [(E[[f['e'][q] for q in h]], np.array([f['uv'][q] for q in h], float)) for h in hs])
        return self.c[k]


def exakt(kind, ecken, teile_m):
    """Exakte (Gleitkomma-)UV der drei Kind-Ecken und ob die Ecke woertlich eine Mutterecke ist."""
    X, wort = [], []
    for ei in kind['e']:
        p = np.array(ecken[ei], float)
        best = None
        for A, UA in teile_m:
            w = _bary(p, A)
            g = w.min()
            if best is None or g > best[0]:
                best = (g, w, UA)
        _g, w, UA = best
        w = np.clip(w, 0.0, 1.0)
        w = w / w.sum()
        hit = [q for q in range(3) if w[q] > 1 - 1e-9]
        if hit:
            X.append(UA[hit[0]])
            wort.append(True)
        else:
            X.append(w @ UA)
            wort.append(False)
    return np.array(X), wort


def _naechste(x, tris):
    fx, fy = int(np.floor(x[0])), int(np.floor(x[1]))
    r = 2
    while r < 64:
        c = [(u, v) for u in range(fx - r + 1, fx + r + 1) for v in range(fy - r + 1, fy + r + 1) if im_gebiet((u, v), tris)]
        if c:
            d = min((u - x[0]) ** 2 + (v - x[1]) ** 2 for u, v in c)
            return sorted((u, v) for u, v in c if (u - x[0]) ** 2 + (v - x[1]) ** 2 <= d + 1e-9)
        r += 1
    raise SystemExit('uvfein: kein ganzzahliger Punkt in der Mutter-UV gefunden')


def wahl(kind, X, wort, tris):
    """(neue UV, geaendert?, Grund) fuer ein Kind."""
    U0 = [tuple(int(x) for x in p) for p in kind['uv']]
    ax = flaeche2(X)

    def schlecht(U):
        au = flaeche2(U)
        if abs(ax) > GRENZE_FLAECHE:
            return au == 0 or au * ax < 0
        return linie(U)

    innen = all(im_gebiet(p, tris) for p in U0)
    if innen and not schlecht(U0):
        return U0, False, ''
    grund = ('aussen' if not innen else '') + ('+' if not innen and schlecht(U0) else '') + ('linie/kipp' if schlecht(U0) else '')

    def kand(j, r):
        if wort[j]:
            return [U0[j]]
        fx, fy = int(np.floor(X[j][0])), int(np.floor(X[j][1]))
        c = [(u, v) for u in range(fx - r + 1, fx + r + 1) for v in range(fy - r + 1, fy + r + 1) if im_gebiet((u, v), tris)]
        return c or _naechste(X[j], tris)
    best = None
    for r in (1, 2):
        for U in itertools.product(*[kand(j, r) for j in range(3)]):
            Ua = np.array(U, float)
            key = (int(schlecht(U)), round(fehlrate(Ua, X), 9), round(float(((Ua - X) ** 2).sum()), 9), U)
            if best is None or key < best[0]:
                best = (key, [tuple(p) for p in U])
        if best[0][0] == 0:
            break
    if best[0][0] == 1 and not schlecht(U0):
        return U0, False, 'aussen, im Gebiet nur Linie -> B4 bleibt'   # duenne Mutterstelle: lieber 1 Texel ausserhalb als eine neue Linie
    return best[1], best[1] != U0, grund


def uv_fein(tab, basis):
    """B4-Tabelle (dict) + K5e_f1-Form -> (neue Tabelle, Bericht). Nur die UV der Kinder aendern sich."""
    neu = copy.deepcopy(tab)
    M = Muetter(basis)
    ber = dict(kinder=0, geaendert=0, je_netz={}, gruende={}, viereck_kinder_unveraendert=0,
               vorher=dict(aussen=0, linie=0, kipp=0, fehlrate_gew=0.0), nachher=dict(aussen=0, linie=0, kipp=0, fehlrate_gew=0.0))
    wsum = 0.0
    for t, a, i, mt, ma, mi in tab['herkunft']:
        kind = tab['teile'][str(t)][ART[a]][i]
        mf, teile_m = M.get(mt, ma, mi)
        if kind['uv'] == mf['uv'] and a == ma:
            continue                                   # ungeteilte Flaeche = Mutter, bleibt
        if a != 0:
            ber['viereck_kinder_unveraendert'] += 1     # in B4 gibt es keine geteilten Vierecke; falls doch: nicht anfassen
            continue
        ber['kinder'] += 1
        ecken = tab['teile'][str(t)]['ecken']
        X, wort = exakt(kind, ecken, teile_m)
        tris = render_dreiecke(mf, ma)
        U, ch, grund = wahl(kind, X, wort, tris)
        E = np.array(ecken, float)[kind['e']]
        gew = 0.5 * float(np.linalg.norm(np.cross(E[1] - E[0], E[2] - E[0])))
        wsum += gew
        ax = flaeche2(X)
        for wann, UU in (('vorher', [tuple(p) for p in kind['uv']]), ('nachher', U)):
            st = ber[wann]
            st['aussen'] += int(not all(im_gebiet(p, tris) for p in UU))
            st['linie'] += int(linie(UU))
            st['kipp'] += int(abs(ax) > GRENZE_FLAECHE and flaeche2(UU) * ax < 0)
            st['fehlrate_gew'] += gew * fehlrate(np.array(UU, float), X)
        if not ch and grund:
            ber['gruende'][grund] = ber['gruende'].get(grund, 0) + 1
        if ch:
            neu['teile'][str(t)][ART[a]][i]['uv'] = [[int(u), int(v)] for u, v in U]
            ber['geaendert'] += 1
            ber['je_netz'][str(t)] = ber['je_netz'].get(str(t), 0) + 1
            ber['gruende'][grund] = ber['gruende'].get(grund, 0) + 1
    for wann in ('vorher', 'nachher'):
        ber[wann]['fehlrate_gew'] = round(ber[wann]['fehlrate_gew'] / max(wsum, 1e-9), 6)
    neu['uvfein'] = dict(regel='kf_k5g3_uv.py (Runde 2, Befund gestreckte Farbstreifen)', bericht=ber)
    return neu, ber


def linien_flaechen(meshes, teile=(10, 16, 21, 22)):
    """Zahl der Render-Dreiecke, deren UV eine Linie ist (Flaeche 0, mindestens zwei verschiedene Ecken), je Netz."""
    aus = {}
    for t in teile:
        m, n = meshes[t], 0
        for a in (0, 1):
            for f in m[ART[a]]:
                n += sum(1 for tr in render_dreiecke(f, a) if linie(tr))
        aus[t] = n
    return aus


if __name__ == '__main__':
    sys.path.insert(0, HIER)
    import kf_k5g3_geo as K
    tp = sys.argv[1] if len(sys.argv) > 1 else K.GEO3_SPALT
    tab = json.load(open(tp, encoding='utf-8'))
    basis = K.FE.lese_emd(open(K.GEO_BASIS, 'rb').read())
    _n, b = uv_fein(tab, basis)
    print(json.dumps(b, indent=1, sort_keys=True))
