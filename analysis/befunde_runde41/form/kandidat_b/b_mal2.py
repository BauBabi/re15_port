#!/usr/bin/env python3
"""Kandidat B: gemalte Textur. Grundhaut prozedural in harten Palettenstufen (Risshaut, Falten, Knoten, Adern), Zeichnung aus den Vorlagen.
Quellen der Farben: farbe_palette.json / formblatt.json materialien. Alles in Texelraum, 3D-Lage je Texel. Keine Zahl gehoert in die Engine."""
import json, math, os, sys
import numpy as np, cv2
from b_geom import *
import b_teile as T
import b_mal as M
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
import form_video as FV
import form_buehne as FB

MAT = P.formblatt()['zeichnung']['materialien']
# Hautstufen: alle innerhalb grundhaut_dunkel .. rotbraun_mittelton
S = np.array([[14, 7, 11], [26, 13, 19], [36, 18, 25], [46, 23, 31], [60, 33, 42], [76, 44, 56], [94, 58, 75]], float)
BAND = np.array([[104, 119, 128], [126, 145, 156], [146, 170, 183]], float)     # helle_baender von/Mitte/bis
LINIE = np.array([[92, 100, 114], [114, 123, 143], [136, 146, 170]], float)     # feine_linien
ROSA = np.array([[158, 118, 130], [181, 137, 150], [204, 156, 170]], float)     # rosa_stelle
ROT = np.array([[84, 48, 70], [103, 63, 84], [122, 78, 99]], float)             # rotbraun_mittelton
WEISS = np.array([[150, 166, 176], [176, 190, 198]], float)                     # nur Mindesthelligkeit belegt, Wert frei
WEISS_P = np.array([[206, 216, 222], [240, 248, 248]], float)
STREIFEN = np.array([[176, 150, 170], [205, 185, 200]], float)                  # innerhalb untere_streifen


def worley(p, zelle, saat=0):
    """F1, F2, Zellzufall 0..1, Vektor Punkt -> Zellpunkt (Einheiten)."""
    q = p / zelle
    i = np.floor(q).astype(np.int64)
    n = p.shape[0]
    d1 = np.full(n, 9.0)
    d2 = np.full(n, 9.0)
    id1 = np.zeros(n)
    vk = np.zeros((n, 3))
    for a in (-1, 0, 1):
        for b in (-1, 0, 1):
            for c in (-1, 0, 1):
                g = i + np.array([a, b, c])
                x = (g[:, 0] * 73856093) ^ (g[:, 1] * 19349663) ^ (g[:, 2] * 83492791) ^ (saat * 2654435761)
                x = ((x ^ (x >> 13)) * 1274126177) & 0xFFFFFFFF
                j = np.stack([(x & 1023), ((x >> 10) & 1023), ((x >> 20) & 1023)], -1) / 1023.0
                v = (g + j) - q
                d = np.sqrt((v ** 2).sum(-1))
                nah = d < d1
                d2 = np.where(nah, d1, np.minimum(d2, d))
                d1 = np.where(nah, d, d1)
                id1 = np.where(nah, ((x >> 7) & 255) / 255.0, id1)
                vk = np.where(nah[:, None], v, vk)
    return d1, d2, id1, vk * zelle


def glied_koordinaten(fit, J):
    """Je Knochen Achse (Anfang, Richtung, Laenge) aus den Knoten."""
    ni = {n: i for i, n in enumerate(fit['namen'])}
    achse = {}
    for b, (nm, _) in enumerate(T.KNOCHEN):
        kn = [k for k in fit['kanten'] if k[0] == nm]
        if not kn:
            continue
        p = np.array([fit['pos'][ni[a]] for _, a, c in kn] + [fit['pos'][ni[c]] for _, a, c in kn])
        d = p - J[b]
        i = int(np.argmax((d ** 2).sum(1)))
        a = d[i]
        lg = float(np.linalg.norm(a))
        if lg > 1:
            achse[b] = (J[b].copy(), a / lg, lg)
    return achse


def grundhaut(pos, nrm, bone, belegt, fit, J):
    """Dunkle Haut in harten Stufen: Muskelstraenge laengs der Glieder, Falten quer an den Gelenken, Knoten in Inseln, Adern. Keine Ringelung (FORMBLATT 2.3)."""
    m = belegt
    p = pos[m].astype(np.float64)
    n = nrm[m].astype(np.float64)
    b = bone[m]
    N_ = len(p)
    ach = glied_koordinaten(fit, J)
    # Anisotrope Lage: q streckt die Muster laengs der Gliedachse, r quer dazu; Kuppel: Achse senkrecht
    q = p.copy()
    rq = p.copy()
    gelenk_nah = np.zeros(N_)
    for bi in np.unique(b):
        s_ = b == bi
        nm = T.KNOCHEN[bi][0]
        if bi in ach and not nm.startswith(('rumpf', 'maul')):
            j0, ax, lg = ach[bi]
            d = p[s_] - j0
            t = d @ ax
            w = d - t[:, None] * ax
            q[s_] = w + t[:, None] * ax * 0.24 + bi * 700.0
            rq[s_] = w * 0.24 + t[:, None] * ax + bi * 700.0
            gelenk_nah[s_] = np.maximum(1 - np.minimum(np.abs(t), np.abs(t - lg)) / 300.0, 0)
        else:
            q[s_] = p[s_] * np.array([1.0, 0.27, 1.0])
            rq[s_] = p[s_] * np.array([0.3, 1.0, 0.3])
            gelenk_nah[s_] = np.maximum(1 - np.abs(n[s_, 1] - 0.75) / 0.25, 0) * 0.6      # Bauchfalten
    warp = np.stack([M.rauschen3(p, 260.0, 11), M.rauschen3(p, 260.0, 12), M.rauschen3(p, 260.0, 13)], 1) * 42.0
    f = M.rauschen3(q + warp, 150.0, 21) + 0.22 * M.rauschen3(q + warp, 80.0, 22)
    # Grundton: grosse Flecken
    g = M.rauschen3(p, 430.0, 14) + 0.5 * M.rauschen3(p, 170.0, 15)
    stufe = np.full(N_, 2.0) + (g > 0.22) * 1.0 - (g < -0.3) * 1.0
    # Furche, darueber Grat (einseitig, das liest sich als Wulst)
    furche = np.abs(f) < 0.062
    grat = (f > 0.062) & (f < 0.150)
    stufe = np.where(grat, stufe + 1, stufe)
    stufe = np.where(furche, np.minimum(stufe, 1), stufe)
    stufe = np.where(np.abs(f) < 0.028, 0, stufe)
    # Querfalten nah am Gelenk, unregelmaessig, abgebrochen
    h = M.rauschen3(rq + warp, 95.0, 23)
    quer = (np.abs(h) < 0.05) & (M.rauschen3(p, 210.0, 24) + gelenk_nah * 1.3 > 0.72)
    stufe = np.where(quer, 0, stufe)
    stufe = np.where((h > 0.05) & (h < 0.11) & (M.rauschen3(p, 210.0, 24) + gelenk_nah * 1.3 > 0.72), np.maximum(stufe, 3), stufe)
    # Knoten
    ins = M.rauschen3(p, 560.0, 16) + 0.3 * M.rauschen3(p, 190.0, 17)
    kn = ins > 0.46
    h1, h2, hid, hv = worley(p + warp * 0.6, 110.0, 5)
    oben = np.array([0.0, -1.0, 0.0])
    kern = h1 < 0.34
    stufe = np.where(kn & (h1 >= 0.34) & (h1 < 0.46), 0, stufe)
    stufe = np.where(kn & kern, 3, stufe)
    stufe = np.where(kn & kern & ((-hv @ oben) / 110.0 > 0.07), 5, stufe)
    # Adern, duenn, einseitig dunkel begleitet
    a_ = np.abs(M.rauschen3(p + warp, 380.0, 18) + 0.3 * M.rauschen3(p, 130.0, 19))
    masse = M.rauschen3(p, 900.0, 25) > -0.15
    ader = (a_ < 0.026) & masse
    stufe = np.where(ader, 4, stufe)
    stufe = np.where((a_ >= 0.026) & (a_ < 0.042) & masse, np.minimum(stufe, 1), stufe)
    # Unterseite nie ueber Stufe 3
    stufe = np.where(n[:, 1] > 0.55, np.minimum(stufe, 3), stufe)
    stufe = np.clip(stufe, 0, 5).astype(int)
    aus = np.zeros(belegt.shape + (3,), np.float32)
    aus[m] = S[stufe]
    return aus


def lichtfaktor(n, s, pos, nrm, bone, art='mess'):
    """Texel -> Lichtfaktor der Bilddarstellung (Textur * Faktor = Schirm), wie im Pruefblatt: Disc-Licht mal Eichfaktor, NCCT, psx_prim_to_sdl."""
    c, k = P.licht_fuer(n, art)
    ctx = FB.licht_kontext(c, np.array(s.lage, dtype=np.int64))
    H, B = bone.shape
    aus = np.ones((H, B, 3), np.float32)
    Y_ = s.Yg
    for b in np.unique(bone):
        if b < 0:
            continue
        q = bone == b
        nw = ((s.R[b] @ nrm[q].astype(float).T).T) @ Y_.T
        nw /= np.maximum(np.linalg.norm(nw, axis=1, keepdims=True), 1e-6)
        Nw = np.rint(nw * 4096).astype(np.int64)
        f = FB.prim_zu_sdl(FB.ncct(ctx[0], ctx[1], ctx[2], Nw))
        aus[q] = f / 255.0
    return aus


_umkehr = {}


def spielbild(n):
    if n not in _umkehr:
        e = FV.video_umkehr(P.original(n), n).astype(np.float32)
        _umkehr[n] = cv2.GaussianBlur(e, (0, 0), 1.0)
    return _umkehr[n]


def stufe_waehlen(wert, tabelle):
    """wert 0..1 auf Tabellenzeilen."""
    i = np.clip((wert * len(tabelle)).astype(int), 0, len(tabelle) - 1)
    return tabelle[i]


def albedo_bild1(sicht, Fak):
    xy, si = sicht['xy'], sicht['sicht']
    e = spielbild(1)
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    A = e[y, x] / np.maximum(Fak, 0.2)
    return A, ok, x, y


def strich_aus_bild(rgb, sicht, Fak, innen, schwellen):
    """Helle Stellen aus Bild 1 in harte Palettenstufen. Albedo = Spielbild / Licht. Warm = Rotbraun/Rosa, kuehl = Linie/Band."""
    A, ok, x, y = albedo_bild1(sicht, Fak)
    drin = ok & innen[y, x]
    Ya = A @ np.array([.299, .587, .114], np.float32)
    rr = A[..., 0] / np.maximum(A[..., 1], 1)
    warm = rr > 1.18
    s0, s1, s2, s3 = schwellen
    m = drin & (Ya >= s0) & (Ya < s1)
    rgb[m & warm] = S[5]
    rgb[m & ~warm] = S[4]
    m = drin & (Ya >= s1) & (Ya < s2)
    rgb[m & warm] = ROT[0]
    rgb[m & ~warm] = LINIE[0]
    m = drin & (Ya >= s2) & (Ya < s3)
    rgb[m & warm] = ROT[2]
    rgb[m & ~warm] = LINIE[1]
    m = drin & (Ya >= s3)
    rgb[m & warm] = ROSA[0]
    rgb[m & ~warm] = BAND[1]
    return rgb


FAKTOR = {}


def stellen(rgb, sichten, Fak1):
    """Sollflaechen: harte Flaeche in der Materialfarbe, Stufe nach Bildhelligkeit in der Flaeche."""
    Yb = {n: cv2.GaussianBlur((P.original(n).astype(np.float32) @ P.YK), (0, 0), 1.2) for n in (1, 2)}
    for s in P.stellen():
        v = sichten[s['bild']]
        k = np.zeros((480, 640), np.uint8)
        for q in s['polygone']:
            cv2.fillPoly(k, [np.round(np.array(q, float)).astype(np.int32)], 1)
        xy, si = v['xy'], v['sicht']
        ok = si & np.isfinite(xy[..., 0])
        x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
        y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
        tr = ok & (k[y, x] > 0)
        if not tr.any():
            continue
        if s['id'] == 'Q4':                 # duennes Band am Rand, 2 px breiter malen, sonst verliert es sich im Filter
            k2 = cv2.dilate(k, np.ones((5, 5), np.uint8))
            tr = ok & (k2[y, x] > 0)
        yv = Yb[s['bild']][y, x]
        lo, hi = np.percentile(yv[tr], [15, 85])
        w = np.clip((yv - lo) / max(hi - lo, 1), 0, 0.999)
        w = np.clip(w + FAKTOR.get(s['id'], 0.0), 0, 0.999)
        mat = s['material']
        if mat == 'rosa_stelle':
            rgb[tr] = stufe_waehlen(w[tr], ROSA)
        elif mat == 'helle_baender':
            rgb[tr] = stufe_waehlen(w[tr], BAND)
        elif mat == 'feine_linien':
            rgb[tr] = stufe_waehlen(w[tr], LINIE)
        elif mat == 'rotbraun_mittelton':
            rgb[tr] = stufe_waehlen(w[tr], ROT)
        elif mat == 'weisskerne':
            rgb[tr] = stufe_waehlen(w[tr], WEISS if s['id'] == 'Z3' else WEISS_P)
        elif mat == 'untere_streifen':
            rgb[tr] = stufe_waehlen(w[tr], STREIFEN)
        elif mat == 'z9':
            rgb[tr] = (85, 80, 60)
    return rgb


ZUSATZ = {1: dict(A05='linie', A06='linie', A10='linie', A11='band0', A13='linie', A21='linie', A25='linie0', A33='linie0', A39='linie0', A07='linie', A08='linie', A09='linie'),
          2: dict(B08='band2', B09='band2', B14='linie', B03='rot', B11='linie0', B10='linie0', B12='linie0')}
ZUG_Q = [(300, 192), (314, 189), (345, 189), (353, 194), (372, 201), (384, 218), (388, 236), (383, 260), (374, 278), (363, 290), (365, 302)]


def zusatz(rgb, sichten, Fak1=None):
    """Weitere gemessene Stellen aus lesung_zeichnung.json, die auf der Figur liegen, ohne Pruefrolle. Zug Q als durchgehende Linie (FORMBLATT 2.3)."""
    Mk = P.merkmale()
    TON = dict(linie=LINIE[1], linie0=LINIE[0], band=BAND[1], band0=BAND[0], band2=BAND[2], rot=ROT[1])
    for n, d in ZUSATZ.items():
        v = sichten[n]
        xy, si = v['xy'], v['sicht']
        ok = si & np.isfinite(xy[..., 0])
        x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
        y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
        for i, t in d.items():
            k = np.zeros((480, 640), np.uint8)
            cv2.fillPoly(k, [np.round(np.array(Mk[i]['polygon'], float)).astype(np.int32)], 1)
            tr = ok & (k[y, x] > 0)
            if n == 1 and Fak1 is not None:
                A = spielbild(1)[y, x] / np.maximum(Fak1, 0.2)
                Ya = A @ np.array([.299, .587, .114], np.float32)
                tab = np.array([S[4], S[5], ROT[0], LINIE[0], LINIE[1], BAND[1]])
                i_ = np.clip(((Ya - 60.0) / 36.0).astype(int), 0, 5)
                rgb[tr] = tab[i_[tr]]
            else:
                rgb[tr] = TON[t]
    v = sichten[2]
    xy, si = v['xy'], v['sicht']
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    k = np.zeros((480, 640), np.uint8)
    cv2.fillPoly(k, [np.round(np.array(P.teil_b2('H')['polygon'], float)).astype(np.int32)], 1)
    k[:, :] = k * (1 - cv2.dilate(cv2.fillPoly(np.zeros((480, 640), np.uint8), [np.round(np.array(Mk['B08']['polygon'], float)).astype(np.int32), np.round(np.array(Mk['B09']['polygon'], float)).astype(np.int32)], 1), np.ones((3, 3), np.uint8)))
    rgb[ok & (k[y, x] > 0)] = ROT[2]        # FORMBLATT 2.1: Hakeninneres rotbraun
    k = np.zeros((480, 640), np.uint8)
    cv2.polylines(k, [np.array(ZUG_Q, np.int32)], False, 1, 4)
    rgb[ok & (k[y, x] > 0)] = LINIE[0]
    return rgb


def spiegel_fl(rgb, belegt, pos, bone, J, sicht2):
    """Bein FL traegt die Zeichnung von Bein FR gespiegelt (frei gesetzt, Seite L ist nicht sichtbar). Was Bild 2 sieht, bleibt Grundhaut."""
    from scipy.spatial import cKDTree
    paare = [('fr_ober', 'fl_ober'), ('fr_unter', 'fl_unter'), ('fr_fuss', 'fl_fuss'), ('fr_zeh', 'fl_zeh')]
    sz = sicht2['sicht']
    for a, b in paare:
        ia, ib = T.IDX[a], T.IDX[b]
        ma, mb = bone == ia, bone == ib
        if not ma.any() or not mb.any():
            continue
        pa = pos[ma] - J[ia]
        pb = (pos[mb] - J[ib]) * np.array([1, 1, -1.0])
        _, n = cKDTree(pa).query(pb)
        q = rgb[ma][n]
        z = rgb[mb].copy()
        hell = (q @ np.array([.299, .587, .114])) > 70
        hell &= ~sz[mb]
        z[hell] = q[hell]
        rgb[mb] = z
    return rgb


def flicken(rgb, belegt):
    """Einzelne dunkle Texel in hellen Flaechen schliessen."""
    Y_ = rgb @ np.array([.299, .587, .114], np.float32)
    h = (Y_ > 85).astype(np.uint8)
    n = cv2.filter2D(h, -1, np.ones((3, 3), np.uint8), borderType=cv2.BORDER_CONSTANT)
    loch = belegt & (h == 0) & (n >= 6)
    ys, xs = np.nonzero(loch)
    for y, x in zip(ys, xs):
        f = rgb[max(y - 1, 0):y + 2, max(x - 1, 0):x + 2].reshape(-1, 3)
        q = f[(f @ np.array([.299, .587, .114], np.float32)) > 85]
        rgb[y, x] = q[len(q) // 2]
    return rgb


def nahschatten(rgb, pos, bone, belegt, netze):
    """Gemalte Verschattung: Haut nah an einem anderen Teil dunkler (Achsel, Schritt, Fugen). Rein Textur, die Engine rechnet nur diffus."""
    from scipy.spatial import cKDTree
    gruppe = np.full(32, -1)
    gr = {'rumpf': 0, 'rumpf_m': 0, 'rumpf_f': 0, 'maul_l': 0, 'maul_r': 0, 'fr_ober': 1, 'fr_unter': 2, 'fr_fuss': 3, 'fr_zeh': 3, 'fl_ober': 4, 'fl_unter': 5, 'fl_fuss': 6, 'fl_zeh': 6,
          'nl_ober': 7, 'nl_unter': 8, 'nl_faust': 9, 'nl_innen': 10, 'nr_ober': 11, 'nr_unter': 12, 'nr_faust': 13, 'nr_innen': 14}
    for k, v in gr.items():
        gruppe[T.IDX[k]] = v
    ecken = {}
    for nz in netze:
        g = gruppe[nz['bone']]
        idx = sorted({a for e in nz['f'] for a, _ in e})
        ecken.setdefault(g, []).append(nz['v'][idx])
    bg = np.where(bone >= 0, gruppe[np.maximum(bone, 0)], -1)
    for g in sorted(ecken):
        m = bg == g
        if not m.any():
            continue
        andere = np.vstack([np.vstack(v) for k, v in ecken.items() if k != g])
        d, _ = cKDTree(andere).query(pos[m])
        f = np.where(d < 95, 0.5, np.where(d < 170, 0.72, 1.0))
        z = rgb[m]
        z *= f[:, None]
        rgb[m] = z
    return rgb


def innen_maske(erode=5):
    z = cv2.imread(FORM + '/pruefung/figur_bild1.png', 0) > 0
    m = cv2.erode(z.astype(np.uint8), cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * erode + 1, 2 * erode + 1))).astype(bool)
    m[395:, :] = False          # unten offen: Boden und Schatten nicht als Zeichnung lesen (A28, A34, C5)
    return m


def lese_faktor(pfad='zw/faktor.json'):
    FAKTOR.clear()
    if os.path.exists(pfad):
        FAKTOR.update(json.load(open(pfad)))


FEST_TON = {'Z7': 0.55, 'Z9': 0.0}        # Stellen am Umriss: Y im Polygon misst dort den Boden mit, kein Nachziehen


def faktor_nachziehen(aus, pfad='zw/faktor.json', schritt=0.22):
    """Helligkeit je Stelle am Bericht nachziehen: Stufe hoch, wenn der gefilterte Render dunkler ist als das Video."""
    R = json.load(open(os.path.join(aus, 'bericht.json'), encoding='utf-8'))
    for n in (1, 2):
        for z in R['bild%d' % n]['zeichnung']:
            q = z['y_video'] / max(z['y_render'], 1.0)
            if z['id'] in FEST_TON:
                FAKTOR[z['id']] = FEST_TON[z['id']]
            elif q > 1.12:
                FAKTOR[z['id']] = min(FAKTOR.get(z['id'], 0.0) + schritt, 0.9)
            elif q < 0.88:
                FAKTOR[z['id']] = max(FAKTOR.get(z['id'], 0.0) - schritt, -0.9)
    json.dump(FAKTOR, open(pfad, 'w'))


def malen(pos, nrm, bone, belegt, sichten, netze, ctx):
    fit, J, s1 = ctx['fit'], ctx['J'], ctx['s1']
    lese_faktor()
    rgb = grundhaut(pos, nrm, bone, belegt, fit, J)
    rgb = nahschatten(rgb, pos, bone, belegt, netze)
    F1 = lichtfaktor(1, s1, pos, nrm, bone)
    rgb = strich_aus_bild(rgb, sichten[1], F1, innen_maske(), (78.0, 118.0, 170.0, 245.0))
    rgb = stellen(rgb, {1: sichten[1], 2: dict(xy=sichten[2]['xy'], sicht=np.zeros_like(sichten[2]['sicht']))}, F1)
    rgb = spiegel_fl(rgb, belegt, pos, bone, J, sichten[2])
    rgb = stellen(rgb, {1: dict(xy=sichten[1]['xy'], sicht=np.zeros_like(sichten[1]['sicht'])), 2: sichten[2]}, F1)
    rgb = zusatz(rgb, sichten, F1)
    rgb = stellen(rgb, {1: sichten[1], 2: sichten[2]}, F1)
    rgb = flicken(rgb, belegt)
    return rgb
