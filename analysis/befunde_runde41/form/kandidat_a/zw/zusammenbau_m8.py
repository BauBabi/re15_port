#!/usr/bin/env python3
"""Kandidat A, Stufe 3: Netze + UV-Inseln -> Atlas, EMD, TIM, Eingabe fuer form_pruefung.

    python zusammenbau.py zw/passung.json --lauf zw/l1

Atlas: Inseln aus Blender (Smart UV), je Insel auf gleiche Texeldichte gebracht, gedreht auf das kleinste Rechteck, dann Skyline-Packung in 384x256.
Die Textur selbst kommt aus maler.py (3D-Funktion je Texel, daher keine Naehte).
"""
import argparse, json, math, os, sys
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
TOOLS = os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS); sys.path.insert(0, HIER)
import form_emd as FE
import entwurf as E
import volumen as V

BONES = V.BONES
IDX = V.IDX
PARENT = [k[1] for k in E.KNOCHEN]
TW, TH = 384, 256
RAND = 1    # Texel Abstand zwischen Inseln


def inseln(teil):
    """Flaechen zu Inseln: gleiche Ecke und gleiche UV (auf 1e-5) verbindet."""
    F = teil['f']
    n = len(F)
    eltern = list(range(n))

    def wurzel(a):
        while eltern[a] != a:
            eltern[a] = eltern[eltern[a]]
            a = eltern[a]
        return a
    kanten = {}
    for i, f in enumerate(F):
        k = len(f['v'])
        for j in range(k):
            a, b = f['v'][j], f['v'][(j + 1) % k]
            ua, ub = f['uv'][j], f['uv'][(j + 1) % k]
            sl = (min(a, b), max(a, b))
            uvk = (a, round(ua[0], 5), round(ua[1], 5), b, round(ub[0], 5), round(ub[1], 5)) if a < b else (b, round(ub[0], 5), round(ub[1], 5), a, round(ua[0], 5), round(ua[1], 5))
            q = kanten.setdefault(sl, [])
            for (uv2, i2) in q:
                if uv2 == uvk:
                    eltern[wurzel(i)] = wurzel(i2)
            q.append((uvk, i))
    gr = {}
    for i in range(n):
        gr.setdefault(wurzel(i), []).append(i)
    return list(gr.values())


def tri_fan(f):
    return [(f[0], f[i], f[i + 1]) for i in range(1, len(f) - 1)]


def flaechen(v3, uv):
    """3D-Flaeche, UV-Flaeche einer Polygonliste."""
    a3, au = 0.0, 0.0
    for f, u in zip(v3, uv):
        for (i, j, k) in tri_fan(list(range(len(f)))):
            a3 += 0.5 * np.linalg.norm(np.cross(f[j] - f[i], f[k] - f[i]))
            e1, e2 = u[j] - u[i], u[k] - u[i]
            au += 0.5 * abs(e1[0] * e2[1] - e1[1] * e2[0])
    return a3, au


def bestes_rechteck(pts):
    """Drehwinkel fuer das kleinste achsparallele Rechteck."""
    h = cv2.convexHull(pts.astype(np.float32)).reshape(-1, 2)
    best = (1e30, 0.0)
    for i in range(len(h)):
        d = h[(i + 1) % len(h)] - h[i]
        a = -math.atan2(d[1], d[0])
        c, s = math.cos(a), math.sin(a)
        q = h @ np.array([[c, -s], [s, c]]).T
        w, hh = q.max(0) - q.min(0)
        if w * hh < best[0]:
            best = (w * hh, a)
    return best[1]


def skyline(rects, W, H):
    """rects: Liste (w, h, id). Rueckgabe {id: (x, y, gedreht)} oder None. Hoehe absteigend, tiefste Stelle, Beste Passung."""
    ordnung = sorted(rects, key=lambda r: (-max(r[0], r[1]), -min(r[0], r[1])))
    sky = [(0, 0, W)]   # (x, y, breite)
    aus = {}
    for (w, h, i) in ordnung:
        best = None
        for (rw, rh, dr) in ((w, h, False), (h, w, True)):
            if rw > W or rh > H:
                continue
            for k in range(len(sky)):
                x = sky[k][0]
                if x + rw > W:
                    continue
                y = 0
                rest = rw
                j = k
                while rest > 0 and j < len(sky):
                    y = max(y, sky[j][1])
                    rest -= sky[j][2]
                    j += 1
                if rest > 0 or y + rh > H:
                    continue
                gute = (y + rh, sky[k][1] != y, x)
                if best is None or gute < best[0]:
                    best = (gute, x, y, rw, rh, dr, k)
        if best is None:
            return None
        _, x, y, rw, rh, dr, k = best
        aus[i] = (x, y, dr)
        neu = [(x, y + rh, rw)]
        nachher = []
        for (sx, sy, sw) in sky:
            if sx + sw <= x or sx >= x + rw:
                nachher.append((sx, sy, sw))
                continue
            if sx < x:
                nachher.append((sx, sy, x - sx))
            if sx + sw > x + rw:
                nachher.append((x + rw, sy, sx + sw - (x + rw)))
        sky = sorted(nachher + neu)
        z = [sky[0]]
        for s in sky[1:]:
            if s[1] == z[-1][1] and z[-1][0] + z[-1][2] == s[0]:
                z[-1] = (z[-1][0], z[-1][1], z[-1][2] + s[2])
            else:
                z.append(s)
        sky = z
    return aus


DICHTE = {'kopf': 1.5, 'kiefer': 1.4, 'hals': 1.3, 'rumpf': 1.0, 'rumpf_mitte': 0.9, 'becken': 0.9, 'nr_ober': 1.15, 'nr_unter': 1.15, 'nl_ober': 1.1, 'nl_unter': 1.1, 'nl_hand': 1.1, 'nl_innen': 1.1, 'nr_hand': 1.1, 'huefte_l': 0.7, 'fl_ober': 0.65, 'fl_unter': 0.65, 'fl_fuss': 0.65}


def maske_insel(polys, rot):
    """polys: Liste n x 2 in Texeln. Rueckgabe Maske inkl. Rand, Versatz der Punkte."""
    a = np.vstack(polys)
    w, h = a.max(0) - a.min(0)
    if rot:
        w, h = h, w
    W, H = int(math.ceil(w)) + 2 * RAND + 3, int(math.ceil(h)) + 2 * RAND + 3
    m = np.zeros((H, W), np.uint8)
    off = np.array([RAND + 1.0, RAND + 1.0]) - (a.min(0)[::-1] if rot else a.min(0))
    for p in polys:
        q = (p[:, ::-1] if rot else p) + off
        cv2.fillPoly(m, [np.round(q).astype(np.int32)], 1)
        cv2.polylines(m, [np.round(q).astype(np.int32)], True, 1, 1)
    m = cv2.dilate(m, np.ones((2 * RAND + 1, 2 * RAND + 1), np.uint8))
    return m, off


def packe(I, mid):
    """Raster-Packung: Inseln nach Flaeche, je Insel die unterste freie Stelle ohne Ueberlappung (FFT-Korrelation mit dem Belegt-Bild)."""
    occ = np.zeros((TH, TW), np.float32)
    occ[TH - 4:, TW - 4:] = 1           # Sammeltexel der Rumpfdeckel
    aus = {}
    reihe = sorted(range(len(I)), key=lambda k: -I[k]['flaeche'] * I[k]['d'] ** 2)
    for k in reihe:
        x = I[k]
        polys = [p * x['d'] / mid for p in x['polys']]
        best = None
        fo = np.fft.rfft2(occ, s=(TH, TW))
        for rot in (False, True):
            m, off = maske_insel(polys, rot)
            H, W = m.shape
            if H > TH or W > TW:
                continue
            km = np.zeros((TH, TW), np.float32)
            km[:H, :W] = m
            fk = np.fft.rfft2(km)
            kor = np.fft.irfft2(fo * np.conj(fk), s=(TH, TW))        # kor[y, x] = Ueberlappung bei Versatz (x, y)
            frei = kor[:TH - H + 1, :TW - W + 1] < 0.5
            if not frei.any():
                continue
            ys, xs = np.nonzero(frei)
            sc = ys + H * 0.6 + xs * 0.02
            i = int(np.argmin(sc))
            if best is None or sc[i] < best[0]:
                best = (sc[i], int(xs[i]), int(ys[i]), rot, off, m)
        if best is None:
            return None
        _, px, py, rot, off, m = best
        occ[py:py + m.shape[0], px:px + m.shape[1]] = np.maximum(occ[py:py + m.shape[0], px:px + m.shape[1]], m)
        aus[k] = (px, py, rot, off)
    return aus


def atlas(netze, sichtbar=None):
    """Rueckgabe: je Teil Liste Flaechen {v, tex}. Globale Dichte per Halbierung, so dass alles passt."""
    I = []
    netze_uv = {}
    for nm in BONES:
        t = netze[nm]
        v = np.array(t['v'], float)
        voll = t
        t = dict(v=t['v'], f=[f for f in voll['f'] if not f.get('deckel')])
        netze_uv[nm] = t
        for ids in inseln(t):
            v3 = [v[t['f'][i]['v']] for i in ids]
            uv = [np.array(t['f'][i]['uv'], float) for i in ids]
            a3, au = flaechen(v3, uv)
            s = math.sqrt(max(a3, 1e-6) / max(au, 1e-12))       # Einheiten je UV-Einheit
            pts = np.vstack(uv) * s
            a = bestes_rechteck(pts)
            c, sn = math.cos(a), math.sin(a)
            M = np.array([[c, -sn], [sn, c]])
            q = pts @ M.T
            lo = q.min(0)
            polys = [(u * s) @ M.T - lo for u in uv]
            I.append(dict(teil=nm, ids=ids, polys=polys, flaeche=a3, d=DICHTE.get(nm, 1.0)))
    best = None
    lo_d, hi_d = 22.0, 44.0
    for _ in range(6):
        mid = math.sqrt(lo_d * hi_d)
        pl = packe(I, mid)
        if pl is None:
            lo_d = mid
        else:
            hi_d = mid
            best = (mid, pl)
    if best is None:
        best = (60.0, packe(I, 60.0))
    mid, pl = best
    aus = {nm: [] for nm in BONES}
    for k, x in enumerate(I):
        px, py, rot, off = pl[k]
        for i, p in zip(x['ids'], x['polys']):
            q = p * x['d'] / mid
            if rot:
                q = q[:, ::-1]
            q = q + off + np.array([px, py], float)
            aus[x['teil']].append(dict(v=list(netze_uv[x['teil']]['f'][i]['v']), tex=np.clip(np.round(q), [0, 0], [TW - 1, TH - 1]).astype(int).tolist()))
    for nm in BONES:
        for f in netze[nm]['f']:
            if f.get('deckel'):
                aus[nm].append(dict(v=list(f['v']), tex=[[TW - 2, TH - 2]] * len(f['v'])))
    print('Atlas: %d Inseln, %.1f Einheiten je Texel (Grunddichte)' % (len(I), mid))
    return aus


KINDER = {}
for _i, _k in enumerate(E.KNOCHEN):
    if _k[1] >= 0:
        KINDER.setdefault(E.KNOCHEN[_k[1]][0], []).append(_k[0])
RUMPF = ('becken', 'rumpf_mitte', 'rumpf')
TIEF = 110.0


def nachbarn(nm):
    p = E.KNOCHEN[IDX[nm]][1]
    n = list(KINDER.get(nm, []))
    if p >= 0:
        n.append(E.KNOCHEN[p][0])
    if nm in RUMPF:
        n += [x for x in RUMPF if x != nm]
    return sorted(set(n))


def verstecktes_weg(netze, prims):
    """Flaechen entfernen, die in Pose 1 ganz tiefer als TIEF im Nachbarteil liegen: spart Flaechen und Texel, die Malreihenfolge bekommt keine versteckten Dreiecke."""
    gesamt = 0
    for nm in BONES:
        t = netze[nm]
        v = np.array(t['v'], float)
        tief = np.full(len(v), 1e9)
        for o in nachbarn(nm):
            tief = np.minimum(tief, V.teil_sdf(o, prims, v))
        behalte = [f for f in t['f'] if tief[f['v']].max() > -TIEF]
        if nm in RUMPF:                  # Schnittdeckel der Rumpfscheiben: liegen im Nachbarn, nie sichtbar
            x1, x2 = V.SCHNITT
            eb = []
            if nm == 'rumpf':
                eb = [x1 - V.UEBERLAPP]
            elif nm == 'rumpf_mitte':
                eb = [x1 + V.UEBERLAPP, x2 - V.UEBERLAPP]
            else:
                eb = [x2 + V.UEBERLAPP]
            def deckel(f):
                q = v[f['v']]
                nn = np.cross(q[1] - q[0], q[2] - q[0])
                return np.abs(nn[0]) > 0.92 * (np.linalg.norm(nn) + 1e-9) and any(np.abs(q[:, 0] - e).max() < 30 for e in eb)
            for f in behalte:
                if deckel(f):
                    f['deckel'] = True
        gesamt += len(t['f']) - len(behalte)
        t['f'] = behalte
    print('versteckt entfernt: %d Flaechen' % gesamt)


def normalen_sdf(name, prims, v):
    """Normale aus dem Feld ohne Kegelschnitt: glatt, die Schnitte duerfen das Licht nicht zerreissen."""
    e = 18.0
    d = []
    for i in range(3):
        o = np.zeros(3); o[i] = e
        d.append(V.teil_sdf(name, prims, v + o) - V.teil_sdf(name, prims, v - o))
    n = np.stack(d, -1)
    return n / np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-9)


def normalen_licht(n, lo=0.16, hi=0.34):
    """Oben-Anteil der Normalen begrenzen. L0 steht in Bild 1 fast senkrecht ueber der Figur (pruefung.md, Licht): Senkrechte Flaechen bekaemen nur Ambient, Oberseiten liefen weiss zu.
    Die Geometrie bleibt, nur die Lichtnormale wird gestaucht, wie bei von Hand gesetzten Normalen. Unterseiten bleiben unveraendert."""
    auf = -n[:, 1]
    neu = np.where(auf > hi, hi + (auf - hi) * 0.25, np.where(auf < lo, lo - (lo - auf) * 0.25, auf))
    h = np.sqrt(np.maximum(1 - neu ** 2, 1e-6) / np.maximum(1 - auf ** 2, 1e-6))
    m = n.copy()
    m[:, 0] *= h
    m[:, 2] *= h
    m[:, 1] = -neu
    return m / np.linalg.norm(m, axis=1, keepdims=True)


def baue_form(netze_atlas, netze, prims, j):
    g = np.array(j['g'], float)
    f = FE.neu()
    for i, nm in enumerate(BONES):
        p = PARENT[i]
        FE.knochen_dazu(f, nm, p, np.round(g[i] - (g[p] if p >= 0 else 0)) if p >= 0 else (0, 0, 0))
    gi = np.array([np.round(g[i] - g[PARENT[i]]) if PARENT[i] >= 0 else np.round(g[i]) for i in range(len(BONES))])
    # gerundete Gelenke: Kette aufsummieren, damit Mesh und Knochen zusammenpassen
    gr = np.zeros_like(g)
    for i in range(len(BONES)):
        gr[i] = gi[i] + (gr[PARENT[i]] if PARENT[i] >= 0 else 0)
    daten = {}
    for i, nm in enumerate(BONES):
        v = np.array(netze[nm]['v'], float)
        n = normalen_sdf(nm, prims, v)
        fl = netze_atlas[nm]
        m = f['meshes'][i]
        for q in v - gr[i]:
            FE.ecke(m, q)
        for q in normalen_licht(n):
            FE.normale(m, q)
        for x in fl:
            r = x['v']
            t = x['tex']
            if len(r) == 3:
                FE.dreieck(m, r, r, t, 0)
            else:
                z = (0, 1, 3, 2)
                FE.viereck(m, [r[k] for k in z], [r[k] for k in z], [t[k] for k in z], 0)
        daten[nm] = dict(v=v, n=n, f=fl)
    FE.orientiere(f)
    return f, gr, daten


def rest_winkel(j):
    return None


def winkel_von_w2(w2):
    """Achsenvektor (Rodrigues) -> Engine-Euler je Knochen."""
    aus = []
    for w in np.asarray(w2, float):
        th = np.linalg.norm(w)
        if th < 1e-9:
            aus.append([0, 0, 0])
            continue
        k = w / th
        K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
        M = np.eye(3) + math.sin(th) * K + (1 - math.cos(th)) * K @ K
        aus.append(list(FE.winkel_aus_matrix(M)))
    return aus


def clips_setzen(f, gr, j, ruhe=None):
    w = (gr[0][0], gr[0][1], gr[0][2])
    n = len(BONES)
    FE.clip_dazu(f, 'pose1', [FE.bild(f, wurzel=w, winkel=[[0, 0, 0]] * n)])
    FE.clip_dazu(f, 'pose2', [FE.bild(f, wurzel=w, winkel=winkel_von_w2(j['w2']))])
    FE.clip_dazu(f, 'ruhe', [FE.bild(f, wurzel=w, winkel=winkel_von_w2(ruhe) if ruhe is not None else [[0, 0, 0]] * n)])
    FE.boden_nachfuehren(f, 0)


def main():
    a = argparse.ArgumentParser()
    a.add_argument('passung')
    a.add_argument('--roh', default=os.path.join(HIER, 'zw', 'roh'))
    a.add_argument('--lauf', default=os.path.join(HIER, 'zw', 'l'))
    a.add_argument('--feinton', type=int, default=int(os.environ.get('FEINTON', '0')))
    a = a.parse_args()
    import maler
    os.makedirs(a.lauf, exist_ok=True)
    prims, j = V.lade_prims(a.passung)
    netze = json.load(open(os.path.join(a.roh, 'netze.json')))
    verstecktes_weg(netze, prims)
    na = atlas(netze)
    f, gr, daten = baue_form(na, netze, prims, j)
    clips_setzen(f, gr, j, maler.ruhe_w(j))
    c, info = maler.male(daten, gr, j, a.lauf, prims)
    open(os.path.join(a.lauf, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(f))
    px, clut = maler.in_clut(c, np.ones(c.shape[:2], bool))
    open(os.path.join(a.lauf, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, [clut]))
    x = dict(name='A', emd='FORM.EMD', tim='FORM.TIM', ruhe=dict(clip=2, bild=0),
             bild1=dict(lage=[int(round(q)) for q in j['lage1']], gier=int(round(float(j['gier1'])))),
             bild2=dict(lage=[int(round(q)) for q in j['lage2']], gier=int(round(float(j['gier2'])))))
    json.dump(x, open(os.path.join(a.lauf, 'A.json'), 'w'), indent=1)
    if a.feinton:
        c = maler.feinton(c, info, os.path.join(a.lauf, 'A.json'), os.path.join(a.lauf, 'FORM.TIM'), a.feinton)
        px, clut = maler.in_clut(c, np.ones(c.shape[:2], bool))
        open(os.path.join(a.lauf, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, [clut]))
    c = maler.nachmalen(c, os.path.join(a.lauf, 'A.json'), os.path.join(a.lauf, 'FORM.TIM'))
    px, clut = maler.in_clut(c, np.ones(c.shape[:2], bool))
    open(os.path.join(a.lauf, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, [clut]))
    cv2.imwrite(os.path.join(a.lauf, 'textur.png'), np.clip(c, 0, 255)[:, :, ::-1])
    d = sum(len(m['dreiecke']) for m in f['meshes'])
    q = sum(len(m['vierecke']) for m in f['meshes'])
    print('Flaechen %d (%d Dreiecke, %d Vierecke), Knochen %d' % (d + q, d, q, len(BONES)))


if __name__ == '__main__':
    main()
