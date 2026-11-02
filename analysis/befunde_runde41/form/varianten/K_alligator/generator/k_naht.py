#!/usr/bin/env python3
"""Variante K, Nachtrag Nutzer: Naehte schliessen. Das Format kennt keine Gewichte (Mesh i haengt starr an Knochen i, werkzeug.md Abschnitt 3), also
schliesst Geometrie, nicht Gewichtung: je Gelenk ein Gelenkkoerper um den Drehpunkt, im Raum des Elternknochens. Eine Kugel um den Drehpunkt dreht
sich in sich; sie fuellt den Raum um das Gelenk in JEDER Pose gleich, egal wie das Kind dreht (dieselbe Idee wie die Kugelkappen am Schwanz, E9).
Damit sie in den Posen der Screenshots nicht aus dem Umriss ragt, ist sie ein Ellipsoid: quer (senkrecht zur Aussenrichtung n = Elternmitte ->
Drehpunkt) Faktor fq x Ansatzradius des Kindes, in Richtung n Faktor fn x Ansatzradius. Quer oeffnen sich die Spalte beim Vor- und Zurueckschwingen,
nach aussen wuerde die Kugel den Umriss vergroessern. fq, fn je Gelenk mit abstimmen() gegen ziel2_bild1/2 (groesstes Volumen, das den Umriss nicht
verletzt). Ansatzradius = 90. Perzentil des Abstands zur Kindachse fuer Kind-Ecken mit Achslage -100..ANSATZ.
Textur: jede Flaeche nimmt UV, CLUT und Seite der naechstgelegenen Flaeche des Elternmeshes (gleiche Haut wie die Umgebung, Textur unveraendert).
Kein RE-Wert."""
import copy
import json
import math

import numpy as np

ANSATZ = 250.0
# Name -> (Kind, fq, fn, Achse des Kindes oder None = aus den Kindknochen). Namen wie k_loecher.NAEHTE.
GELENKE = {'Bein l - Huefte': (8, 1.0, 1.0, None), 'Bein r - Huefte': (9, 1.0, 1.0, None), 'Arm l - Brust': (11, 1.0, 1.0, None),
           'Arm r - Brust': (16, 1.0, 1.0, None), 'Hals - Brust': (21, 1.0, 1.0, None), 'Schwanz - Becken': (1, 1.0, 1.0, None),
           'Kopf - Hals': (22, 1.0, 1.0, (1, 0, 0)), 'Wirbel - Huefte': (7, 1.0, 1.0, None), 'Brust - Wirbel': (10, 1.0, 1.0, (1, 0, 0)),
           'Knie l': (24, 1.0, 1.0, None), 'Knie r': (26, 1.0, 1.0, None), 'Ellbogen l': (12, 1.0, 1.0, None), 'Ellbogen r': (17, 1.0, 1.0, None),
           'Fuss l': (25, 1.0, 1.0, (1, 0, 0)), 'Fuss r': (27, 1.0, 1.0, (1, 0, 0)), 'Hand l': (13, 1.0, 1.0, (1, 0, 0)), 'Hand r': (18, 1.0, 1.0, (1, 0, 0)),
           'Schwanz 1-2': (2, 1.0, 1.0, None), 'Schwanz 2-3': (3, 1.0, 1.0, None), 'Schwanz 3-4': (4, 1.0, 1.0, None),
           'Schwanz 4-5': (5, 1.0, 1.0, None), 'Schwanz 5-6': (6, 1.0, 1.0, None), 'Kiefer - Kopf': (23, 1.0, 1.0, (1, 0, 0))}
# Muffen (zweite Art, nach der ersten Loch-Messung): Ellipsoid am KIND, Mitte im Drehpunkt, lang entlang der Kindachse (fa x Ansatzradius), quer fq x
# Ansatzradius. Dreht mit dem Kind: das Stueck hinter dem Drehpunkt steckt im Eltern und schwenkt beim Beugen in den Spalt auf der Gegenseite
# (Bein nach hinten -> Spalt vorn). Name 'Muffe <Gelenk>', zweiter Faktor = fa statt fn.
MUFFEN = {'Muffe Bein l - Huefte': (8, 1.0, 1.6, None), 'Muffe Bein r - Huefte': (9, 1.0, 1.6, None), 'Muffe Arm l - Brust': (11, 1.0, 1.6, None),
          'Muffe Arm r - Brust': (16, 1.0, 1.6, None), 'Muffe Hals - Brust': (21, 1.0, 1.6, None), 'Muffe Kopf - Hals': (22, 1.0, 1.6, (1, 0, 0)),
          'Muffe Schwanz - Becken': (1, 1.0, 1.6, None), 'Muffe Hand l': (13, 1.0, 1.6, (1, 0, 0)), 'Muffe Hand r': (18, 1.0, 1.6, (1, 0, 0))}
FA = (1.6, 1.3, 1.0)
KUGEL_U, KUGEL_V = 8, 5


def achse_kind(form, c):
    kids = [i for i, k in enumerate(form['knochen']) if k['eltern'] == c]
    if kids:
        d = np.mean([np.array(form['knochen'][i]['versatz'], float) for i in kids], 0)
        if np.linalg.norm(d) > 1:
            return d / np.linalg.norm(d)
    V = np.array(form['meshes'][c]['ecken'], float)
    q = V - V.mean(0)
    w, v = np.linalg.eigh(q.T @ q)
    d = v[:, -1]
    return d if d @ V.mean(0) >= 0 else -d


def radius_ansatz(form, c, ax):
    V = np.array(form['meshes'][c]['ecken'], float)
    t = V @ ax
    r = np.linalg.norm(V - np.outer(t, ax), axis=1)
    m = (t > -100) & (t < ANSATZ)
    return float(np.percentile(r[m], 90)) if m.sum() >= 4 else float(np.percentile(r, 50))


def aussen(form, c, ax):
    """Aussenrichtung n im Elternraum: Mitte des Elternmeshes -> Drehpunkt. Liegt der Drehpunkt nahe der Mitte, die Kindachse."""
    e = form['knochen'][c]['eltern']
    V = np.array(form['meshes'][e]['ecken'], float)
    p = np.array(form['knochen'][c]['versatz'], float)
    d = p - V.mean(0)
    if np.linalg.norm(d) < 100:
        return np.asarray(ax, float) / np.linalg.norm(ax)
    return d / np.linalg.norm(d)


def _basis(n):
    n = np.asarray(n, float) / np.linalg.norm(n)
    h = np.array([0, 1.0, 0]) if abs(n[1]) < 0.8 else np.array([1.0, 0, 0])
    u = h - (h @ n) * n
    u /= np.linalg.norm(u)
    return u, np.cross(n, u), n


def ellipsoid_netz(mitte, n, rq, rn):
    """Ecken, Normalen, Vierecke (Z-Reihenfolge), Dreiecke. Pole auf der Achse n; Vieleck umschreibt das Ellipsoid (Faktor 1/cos(pi/U))."""
    u, w, n = _basis(n)
    k = 1.0 / math.cos(math.pi / KUGEL_U)
    mitte = np.asarray(mitte, float)
    P, N, idx = [], [], {}

    def dazu(su, sw, sn):
        P.append(mitte + k * (su * rq * u + sw * rq * w + sn * rn * n))
        g = su / rq * u + sw / rq * w + sn / rn * n
        N.append(g / np.linalg.norm(g))
        return len(P) - 1
    for i in range(1, KUGEL_V):
        th = math.pi * i / KUGEL_V
        for j in range(KUGEL_U):
            ph = 2 * math.pi * j / KUGEL_U
            idx[i, j] = dazu(math.sin(th) * math.cos(ph), math.sin(th) * math.sin(ph), math.cos(th))
    idx[0] = dazu(0, 0, 1.0)
    idx[KUGEL_V] = dazu(0, 0, -1.0)
    Q, T = [], []
    for i in range(1, KUGEL_V - 1):
        for j in range(KUGEL_U):
            Q.append((idx[i, j], idx[i, (j + 1) % KUGEL_U], idx[i + 1, j], idx[i + 1, (j + 1) % KUGEL_U]))
    for pol, i in ((0, 1), (KUGEL_V, KUGEL_V - 1)):
        for j in range(KUGEL_U):
            T.append((idx[pol], idx[i, j], idx[i, (j + 1) % KUGEL_U]))
    return np.array(P), np.array(N), Q, T


def ellipsoid_dreiecke(mitte, n, rq, rn):
    P, _N, Q, T = ellipsoid_netz(mitte, n, rq, rn)
    tri = [P[[q[0], q[1], q[3]]] for q in Q] + [P[[q[0], q[3], q[2]]] for q in Q] + [P[list(t)] for t in T]
    return np.array(tri)


def _flaechen_mitten(mesh):
    V = np.array(mesh['ecken'], float)
    out = []
    for art in ('dreiecke', 'vierecke'):
        for f in mesh[art]:
            out.append((V[f['e']].mean(0), art, f))
    return out


def kugel(form, eltern, mitte, n, rq, rn, quelle_mesh):
    """Gelenkkoerper ins Elternmesh. UV, CLUT, Seite je Flaeche von der naechsten Flaeche gleicher Art des Elternmeshes (Haut der Umgebung)."""
    m = form['meshes'][eltern]
    P, N, Q, T = ellipsoid_netz(mitte, n, rq, rn)
    quellen = _flaechen_mitten(quelle_mesh)
    qm = np.array([q[0] for q in quellen])
    v0 = len(m['ecken'])
    n0 = len(m['normalen'])
    for p, nn in zip(P, N):
        m['ecken'].append([int(round(c)) for c in p])
        m['normalen'].append([int(round(c)) for c in nn * 4096])

    def quelle(p, art):
        for i in np.argsort(np.linalg.norm(qm - p, axis=1)):
            if quellen[i][1] == art:
                return quellen[i][2]
        return quellen[0][2]

    FE = __import__('form_emd')
    tim = form.get('tim') or dict(clut_x=0, clut_y=480, seite=0x80)

    def uvs(src, k):
        uv, clut, page = FE._uv_aufloesen(src, tim)      # roher Weg (uv/clut/page) oder tex/zeile (neue Teile, Seite 3)
        u = [list(x) for x in uv]
        while len(u) < k:
            u.append(u[-1])
        return u[:k], clut, page
    for f in Q:
        u, cl, pg = uvs(quelle(P[list(f)].mean(0), 'vierecke'), 4)
        m['vierecke'].append(dict(e=[v0 + i for i in f], n=[n0 + i for i in f], uv=u, clut=cl, page=pg))
    for f in T:
        u, cl, pg = uvs(quelle(P[list(f)].mean(0), 'dreiecke'), 3)
        m['dreiecke'].append(dict(e=[v0 + i for i in f], n=[n0 + i for i in f], uv=u, clut=cl, page=pg))
    return len(Q) + len(T)


def gelenk_mass(form, c, ax):
    a = np.array(ax, float) if ax is not None else achse_kind(form, c)
    a = a / np.linalg.norm(a)
    return a, radius_ansatz(form, c, a), aussen(form, c, a)


def schliessen(form, gelenke=None, quelle=None):
    """form -> neue form mit Gelenkkoerpern. quelle: form vor dem Eingriff (Masse und UV). Rueckgabe (form, bericht)."""
    f = copy.deepcopy(form)
    q = copy.deepcopy(quelle or form)
    ber = {}
    for nm, (c, fq, fn, ax) in (gelenke or GELENKE).items():
        if fq <= 0 or fn <= 0:
            continue
        e = f['knochen'][c]['eltern']
        a, r, n = gelenk_mass(q, c, ax)
        if nm.startswith('Muffe '):
            k = kugel(f, c, (0.0, 0.0, 0.0), a, r * fq, r * fn, q['meshes'][c])
            ber[nm] = dict(kind=c, am='Kind', achse=[round(float(x), 3) for x in a], radius_kind=round(r), quer=round(r * fq), laengs=round(r * fn), flaechen=k)
            continue
        k = kugel(f, e, f['knochen'][c]['versatz'], n, r * fq, r * fn, q['meshes'][e])
        ber[nm] = dict(kind=c, eltern=e, aussen=[round(float(x), 3) for x in n], radius_kind=round(r), quer=round(r * fq), aussen_r=round(r * fn), flaechen=k)
    FE = __import__('form_emd')
    FE.orientiere(f)
    return f, ber


# ─────────────────────────────── Abstimmung gegen die Screenshots
FQ = (1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4)
FN = (1.0, 0.8, 0.6, 0.4, 0.25)
NEU_SCHWARZ_MAX = 10      # neue Figurpixel auf Ziel-Schwarz je Bild (Festlegung des Werkzeugs: darunter Rasterrauschen der Kugelkante)
NEU_AUSSEN_MAX = 150      # neue Figurpixel ausserhalb Ziel-Weiss (Schwarz + Grau) je Bild; Grau ist unbestimmt (Feuer, Logo-Saum), zaehlt in der IoU nicht


def abstimmen(emd_dir, gelenke=None, aus=None, quelle_dir=None):
    """Je Gelenk (fq, fn) mit dem groessten Volumen fq*fq*fn, bei dem der Koerper in Bild 1 und Bild 2 (Posen des Kandidaten) hoechstens NEU_SCHWARZ_MAX
    neue Figurpixel auf Ziel-Schwarz und NEU_AUSSEN_MAX ausserhalb Ziel-Weiss setzt. Tiefenpuffer aus k_kern gegen ziel2_bild*.png."""
    import cv2
    import k_kern as KK
    form, mo, fj = KK.lade(emd_dir)
    gel = gelenke or GELENKE
    B = {}
    for nb in (1, 2):
        s = fj['bild%d' % nb]
        kam = KK.kamera(nb)
        R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
        Rw, Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
        tief, idb = KK.raster(kam, *KK.tris_welt(form, Rw, Tw))
        z = cv2.imread(KK.os.path.join(KK.FORM, 'ziel2_bild%d.png' % nb), 0)
        B[nb] = dict(kam=kam, Rw=Rw, Tw=Tw, tief=tief, idb=idb, z=z)
    kombis = sorted(((fq, fn) for fq in FQ for fn in FN), key=lambda t: -(t[0] * t[0] * t[1]))
    erg = {}
    kombis_m = sorted(((fq, fa) for fq in FQ for fa in FA), key=lambda t: -(t[0] * t[0] * t[1]))
    for nm, (c, _fq, _fn, ax) in gel.items():
        e = form['knochen'][c]['eltern']
        a, r, n = gelenk_mass(form, c, ax)
        mitte = np.array(form['knochen'][c]['versatz'], float)
        muffe = nm.startswith('Muffe ')
        if muffe:
            e, mitte, n = c, np.zeros(3), a
        wahl, proben = (0.0, 0.0), []
        for fq, fn in (kombis_m if muffe else kombis):
            tri = ellipsoid_dreiecke(mitte, n, r * fq, r * fn)
            ok, werte = True, {}
            for nb, b in B.items():
                w = tri.reshape(-1, 3) @ b['Rw'][e].T + b['Tw'][e]
                _t2, i2 = KK.raster(b['kam'], w.reshape(-1, 3, 3), np.full(len(tri), 99), b['tief'].copy(), b['idb'].copy())
                neu = (i2 >= 0) & (b['idb'] < 0)
                ns, na = int((neu & (b['z'] == 0)).sum()), int((neu & (b['z'] != 255)).sum())
                werte[nb] = (ns, na)
                ok = ok and ns <= NEU_SCHWARZ_MAX and na <= NEU_AUSSEN_MAX
            proben.append(dict(fq=fq, fn=fn, bild1=werte[1], bild2=werte[2], ok=ok))
            if ok:
                wahl = (fq, fn)
                break
        erg[nm] = dict(kind=c, fq=wahl[0], fn=wahl[1], radius_kind=round(r), aussen=[round(float(x), 3) for x in n], proben=len(proben), erste_probe=proben[0])
        print('%-18s r %4d n %s -> fq %.1f fn %.2f (%d Proben; 1,0/1,0: Bild1 %s Bild2 %s)' % (nm, r, np.round(n, 2), wahl[0], wahl[1], len(proben),
                                                                                         proben[0]['bild1'], proben[0]['bild2']), flush=True)
    out = dict(gelenke={nm: [v['kind'], v['fq'], v['fn'], (list(gel[nm][3]) if gel[nm][3] is not None else None)] for nm, v in erg.items() if v['fq'] > 0},
               bericht=erg, grenzen=dict(neu_schwarz_max=NEU_SCHWARZ_MAX, neu_aussen_max=NEU_AUSSEN_MAX, fq=FQ, fn=FN))
    if aus:
        json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1)
    return out


class _Kandidat:
    """form_pruefung.Kandidat ohne Dateien (EMD/TIM als Bytes, j wie form.json)."""

    def __init__(self, j, emd, tim):
        import form_pruefung as P
        import form_buehne as FB
        self.__class__ = type('KK_Kandidat', (P.Kandidat,), {})
        self.j, self.name, self.emd, self.tim = j, j.get('name', 'probe'), emd, tim
        self.mo = __import__('form_emd').modell(emd, tim)
        self.tex = FB.Textur(tim)
        self.licht = j.get('licht', 'mess')


_KAN = {}


def echt_messen(form, j, tim):
    """Umriss-IoU, Pruefpunkte (b) und Boden/Verdeckung/Kerbe E3 (c) wie form_pruefung.pruefe_kandidat, je Bild. Rueckgabe {n: (iou, {Kennung: bestanden})}."""
    import form_pruefung as P
    FE = __import__('form_emd')
    k = _Kandidat(j, FE.schreibe_emd(form), tim)
    G = json.load(open(P.daten('grenzen.json'), encoding='utf-8'))
    if not _KAN:
        rest, _ = P.raum_rest2()
        for n in (1, 2):
            _KAN[n] = P.kanaele(P.original(n), rest if n == 2 else None)
    out = {}
    for n in (1, 2):
        _zn, z = P.ziel_laden(n)
        _s, alb, m, _ = k.rendern(n)
        band = P.material_maske(alb, m, 'helle_baender')
        pk = [P.punkt_messen(p, G[p['id']], m, band) for p in P.punkte_abschnitt7() if p['bild'] == n]
        pk += P.boden_und_verdeckung(n, m, z, _KAN[n])
        out[n] = (P.ueberdeckung(m, z)['iou'], {v['id']: bool(v.get('ok')) for v in pk if v.get('zaehlt', True)})
    return out


def nachpruefen(form_ohne, j, tim, gel, quelle=None, iou_spiel=0.001):
    """Jeder Gelenkkoerper einzeln am echten Pruefweg (Buehne + Punkte von form_pruefung): kein Punkt, der ohne Koerper besteht, darf mit ihm fallen,
    die Umriss-IoU darf je Bild um hoechstens iou_spiel sinken. Sonst naechst kleinere Stufe (fq, fn) aus FQ x FN nach Volumen. Rueckgabe neue Gelenke + Protokoll."""
    basis = echt_messen(form_ohne, j, tim)
    kombis_k = sorted(((fq, fn) for fq in FQ for fn in FN), key=lambda t: -(t[0] * t[0] * t[1]))
    kombis_m = sorted(((fq, fa) for fq in FQ for fa in FA), key=lambda t: -(t[0] * t[0] * t[1]))
    neu, prot = {}, {}
    for nm, (c, fq, fn, ax) in gel.items():
        kombis = kombis_m if nm.startswith('Muffe ') else kombis_k
        start = kombis.index((fq, fn)) if (fq, fn) in kombis else 0
        wahl = None
        for fq2, fn2 in kombis[start:]:
            f, _b = schliessen(form_ohne, {nm: (c, fq2, fn2, ax)}, quelle=quelle)
            e = echt_messen(f, j, tim)
            fall = [i for n in (1, 2) for i, ok in basis[n][1].items() if ok and not e[n][1].get(i, False)]
            diou = [round(e[n][0] - basis[n][0], 4) for n in (1, 2)]
            if not fall and min(diou) >= -iou_spiel:
                wahl = (fq2, fn2)
                prot[nm] = dict(fq=fq2, fn=fn2, iou_delta=diou)
                break
            prot.setdefault(nm + ' (verworfen)', []).append(dict(fq=fq2, fn=fn2, faellt=fall, iou_delta=diou))
        if wahl:
            neu[nm] = (c, wahl[0], wahl[1], ax)
        print('  %-18s %s' % (nm, prot.get(nm, 'kein Koerper')), flush=True)
    return neu, prot, basis


def laden(pfad):
    g = json.load(open(pfad, encoding='utf-8'))
    return {n: (v[0], v[1], v[2], v[3]) for n, v in g['gelenke'].items()}


def befehl_nachpruefen(ohne_dir, gel_json, aus_json):
    import k_kern as KK
    form, mo, fj = KK.lade(ohne_dir)
    tim = open(KK.os.path.join(ohne_dir, 'FORM.TIM'), 'rb').read()
    g = json.load(open(gel_json, encoding='utf-8'))
    gel = laden(gel_json)
    neu, prot, basis = nachpruefen(form, fj, tim, gel)
    f, _b = schliessen(form, neu)
    alle = echt_messen(f, fj, tim)
    fall = [i for n in (1, 2) for i, ok in basis[n][1].items() if ok and not alle[n][1].get(i, False)]
    g['gelenke'] = {nm: [v[0], v[1], v[2], (list(v[3]) if v[3] is not None else None)] for nm, v in neu.items()}
    g['nachpruefung'] = dict(protokoll=prot, ohne=dict({str(n): dict(iou=basis[n][0], bestanden=sum(basis[n][1].values()), von=len(basis[n][1])) for n in (1, 2)}),
                             alle=dict({str(n): dict(iou=alle[n][0], bestanden=sum(alle[n][1].values()), von=len(alle[n][1])) for n in (1, 2)}), faellt_zusammen=fall)
    json.dump(g, open(aus_json, 'w', encoding='utf-8'), indent=1)
    print('zusammen: Bild 1 IoU %.4f %d/%d, Bild 2 IoU %.4f %d/%d, faellt %s' % (alle[1][0], sum(alle[1][1].values()), len(alle[1][1]),
                                                                               alle[2][0], sum(alle[2][1].values()), len(alle[2][1]), fall))


if __name__ == '__main__':
    import sys
    if sys.argv[1] == 'nachpruefen':
        befehl_nachpruefen(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        abstimmen(sys.argv[1], gelenke=dict(GELENKE, **MUFFEN), aus=sys.argv[2] if len(sys.argv) > 2 else None)
