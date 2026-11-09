"""Messer V4 Feinschliff: Tafeln m1_details.png und m2_regionen.png (roh, kein Gamma, gleiche Szenenkette je Zeile).

    python tafeln_v4f.py <cache> <messung_v4f.json> <aus-Ordner>
"""
import json
import sys

import cv2
import numpy as np

import v4f
import ketten as KT
import gelenke as G

GELB, CYAN, MAG, GRUEN, ROT, WEISS = (255, 255, 0), (0, 255, 255), (255, 0, 255), (0, 255, 0), (255, 60, 60), (255, 255, 255)


def schrift(img, t, x, y, g=0.45, c=WEISS):
    cv2.putText(img, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(img, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, c, 1, cv2.LINE_AA)


def kachel(img, box, f, titel, unter=None):
    x0, y0, x1, y1 = box
    a = cv2.resize(img[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST).copy()
    schrift(a, titel, 6, 18)
    if unter:
        schrift(a, unter, 6, a.shape[0] - 8, 0.4, (230, 230, 120))
    return a


def kopf(t, breite, h=30):
    k = np.full((h, breite, 3), 24, np.uint8)
    schrift(k, t, 6, 20, 0.5, (200, 230, 255))
    return k


def reihe(ks, gap=4):
    h = max(k.shape[0] for k in ks)
    out = []
    for k in ks:
        p = np.full((h, k.shape[1], 3), 24, np.uint8)
        p[:k.shape[0]] = k
        out += [p, np.full((h, gap, 3), 24, np.uint8)]
    return np.hstack(out[:-1])


def stapel(rs, gap=4):
    w = max(r.shape[1] for r in rs)
    out = []
    for r in rs:
        p = np.full((r.shape[0], w, 3), 24, np.uint8)
        p[:, :r.shape[1]] = r
        out += [p, np.full((gap, w, 3), 24, np.uint8)]
    return np.vstack(out[:-1])


def marke(a, x, y, box, f, txt, c):
    px, py = int((x - box[0]) * f), int((y - box[1]) * f)
    cv2.circle(a, (px, py), 7 * f, c, 2)
    schrift(a, txt, px + 7 * f, py - 4, 0.42, c)


def umriss(a, m, box, f, c):
    x0, y0, x1, y1 = box
    mm = cv2.resize(m[y0:y1, x0:x1].astype(np.uint8), ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)
    cs, _ = cv2.findContours(mm, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, cs, -1, c, 1)


DETAIL_NAMEN = {}   # id -> Kurzname (aus der Messung gesetzt)


def kurzname(e):
    t = e['ort_asset']['teile'][0][0] if e['ort_asset']['teile'] else 'Hintergrund'
    return t


def m1(cache, M, aus):
    staende = ['K4', 'V3R3', 'V3R4', 'Asset K5c']
    zeilen = []
    for n, box, f in ((1, (180, 170, 450, 480), 2), (2, (190, 140, 470, 430), 2)):
        o = v4f.original(n)
        fl = [e for e in M['details']['bild%d' % n]['flecken'] if e['px'] >= 20]
        for art in ('video', 'sauber'):
            ks = [kachel(o, box, f, 'Vorlage Bild %d (Video roh)' % n, 'Kreise = helle Flecken der Vorlage (T-L* > %d)' % (40 if n == 1 else 28))]
            for nm in staende:
                d = KT.lade(cache, nm)[n]
                img = d['video'][15] if art == 'video' else d['sauber']
                ks.append(kachel(img, box, f, '%s, %s' % (nm, 'Disc-Licht x k + Videofilter Saat 15' if art == 'video' else 'Disc-Licht x k, ohne Filter')))
            for e in fl:
                lt = e['lab_T'][0]
                for i, a in enumerate(ks):
                    if i == 0:
                        txt = '%s L%.0f' % (e['id'][3:], lt)
                    else:
                        kl = e['kette'][staende[i - 1]][0]
                        txt = '%.0f' % kl
                    c = GRUEN if (i == 0 or e['kette'][staende[i - 1]][0] >= 0.8 * lt) else ROT
                    marke(a, e['mitte'][0], e['mitte'][1], box, f, txt, c)
            zeilen.append(reihe(ks))
    t = stapel(zeilen)
    k1 = kopf('m1 Details: Vorlage | K4 (Spiel bis E30) | V3 Runde 3 | V3 Runde 4 | Asset K5c (= Runde 5, 16 Zeilen). Z.1/2 Bild 1, Z.3/4 Bild 2; '
              'gleiche Kette je Zeile, roh. Zahl am Kreis = Ketten-L* (3 Saaten) auf den Fleckpixeln; gruen >= 80 % der Vorlage (T-L*), rot darunter.', t.shape[1])
    # Rundum K4 | Asset mit Gelenk-/Detailmarken
    rz = []
    for wt, az, el in (('3/4 vorn', 45, 15), ('Seite', 0, 8), ('3/4 hinten', -135, 20)):
        ks = []
        for nm in ('K4', 'Asset K5c'):
            o_ = v4f.ST[nm]
            a, K = v4f.rundum_bild(o_, 2, 0, az, el, 900)
            g, _ = G.gelenk_flaechen(o_)
            lab = np.zeros(a.shape[:2], object)
            nmap = {}
            for y, x in zip(*np.nonzero(K['maske'])):
                key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
                nmn = g.get(key)
                if nmn is None and key[0] == 6:
                    nmn = 'Schwanzspitze'
                if nmn:
                    nmap.setdefault(nmn, []).append((x, y))
            b = a.copy()
            schrift(b, '%s, Clip 2 Ruhe, %s, weisses Licht 0,46+0,86 N.L, roh' % (nm, wt), 6, 18)
            for nmn, pts in nmap.items():
                if len(pts) < 30 or not any(s in nmn for s in ('Knie', 'Ellbogen', 'Huefte', 'Fuss', 'Hand', 'Schulter', 'Schwanzspitze')):
                    continue
                p = np.mean(pts, 0).astype(int)
                cv2.circle(b, tuple(int(v) for v in p), 12, CYAN, 2)
                schrift(b, nmn.replace('Muffe ', 'M.'), int(p[0]) + 12, int(p[1]), 0.45, CYAN)
            ys, xs = np.nonzero(K['maske'])
            b = b[max(ys.min() - 40, 0):ys.max() + 40, max(xs.min() - 60, 0):xs.max() + 160].copy()
            b[:26] = 24
            schrift(b, '%s, Clip 2 Ruhe, %s, weisses Licht 0,46+0,86 N.L, roh' % (nm, wt), 6, 18)
            ks.append(b)
        rz.append(reihe(ks))
    t2 = stapel(rz)
    k2 = kopf('Rundum K4 | Asset K5c (je Ansicht nebeneinander), Gelenkkoerper (k_naht.kugel) und Schwanzspitze (Mesh 6) beschriftet, gleiches Licht, roh.', t2.shape[1])
    cv2.imwrite(aus + '/m1_details.png', cv2.cvtColor(stapel([k1, t, k2, t2]), cv2.COLOR_RGB2BGR))


def m2(cache, M, aus):
    oa = v4f.ST['Asset K5c']
    NT = v4f.FORM + '/nutzer_teile/maske_%s.png'
    fm = np.load(aus + '/_flecken.npz') if False else None
    zeilen = []
    # Bild 1: Schwanzspitze + Gelenkflecken
    n, box, f = 1, (180, 170, 450, 480), 2
    o = v4f.original(n)
    O = KT.original(cache, n)
    T = np.clip(O['T'][0], 0, 255).astype(np.uint8)
    d = KT.lade(cache, 'Asset K5c')[n]
    K = d['K']
    g, _ = G.gelenk_flaechen(oa)
    gm = np.zeros(K['maske'].shape, bool)
    for y, x in zip(*np.nonzero(K['maske'])):
        key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
        if key in g:
            gm[y, x] = True
    fl = M['details']['bild1']['flecken']
    ks = [kachel(o, box, f, 'Vorlage Bild 1 (Video roh)'), kachel(T, box, f, 'Vorlage T (p_zonen.rueck, Szenenkette rueckwaerts)')]
    ov = d['sauber'].copy()
    ov[gm] = (0.4 * ov[gm] + 0.6 * np.array(CYAN)).astype(np.uint8)
    ov[K['mesh'] == 6] = (0.4 * ov[K['mesh'] == 6] + 0.6 * np.array(MAG)).astype(np.uint8)
    ks.append(kachel(ov, box, f, 'Asset K5c: Gelenkkoerper cyan, Mesh 6 (Schwanzspitze) magenta'))
    ks.append(kachel(d['video'][15], box, f, 'Asset K5c, Kette (Disc-Licht x k + Videofilter)'))
    for e in fl:
        if e['px'] < 20:
            continue
        nm = kurzname(e)
        c = MAG if nm == 'Mesh 6' else (GELB if e['ort_asset']['zeilen'].get('5', e['ort_asset']['zeilen'].get(5, 0)) else CYAN)
        for a in ks:
            marke(a, e['mitte'][0], e['mitte'][1], box, f, '%s %s' % (e['id'][3:], nm.replace('Gelenkkoerper ', '')), c)
    zeilen.append(reihe(ks))
    # Bild 2: Buckel, Krallen, Kiefer
    n, box, f = 2, (130, 140, 490, 480), 2
    o = v4f.original(n)
    O = KT.original(cache, n)
    T = np.clip(O['T'][0], 0, 255).astype(np.uint8)
    d = KT.lade(cache, 'Asset K5c')[n]
    K = d['K']
    ks = [kachel(o, box, f, 'Vorlage Bild 2 (Video roh)'), kachel(T, box, f, 'Vorlage T (p_zonen.rueck)')]
    ov = d['sauber'].copy()
    kl = np.isin(K['mesh'], (14, 15, 28, 30, 19, 20, 29, 31))
    ov[kl] = (0.3 * ov[kl] + 0.7 * np.array(GELB)).astype(np.uint8)
    lip = np.isin(K['zeile'], (0, 6)) & K['maske']
    ov[lip] = (0.3 * ov[lip] + 0.7 * np.array(ROT)).astype(np.uint8)
    huck = (K['mesh'] == 10) & K['maske']
    ov[huck] = (0.6 * ov[huck] + 0.4 * np.array(MAG)).astype(np.uint8)
    ks.append(kachel(ov, box, f, 'Asset: Krallen gelb, Zeilen 0/6 (Maul) rot, Mesh 10 magenta'))
    ks.append(kachel(d['video'][15], box, f, 'Asset K5c, Kette (Disc-Licht x k + Videofilter)'))
    for mk, c in (('right_arm_front', (0, 128, 255)), ('left_arm_front', GRUEN), ('unterkiefer', ROT), ('oberkiefer', (255, 128, 0))):
        m = cv2.imread(NT % mk, 0) > 0
        for a in ks:
            umriss(a, m, box, f, c)
    kr = M['krallen']
    for hand, h in kr.items():
        for kb, v in h['klauen'].items():
            for a in ks[1:]:
                marke(a, v['spitze'][0], v['spitze'][1], box, f, 'K%s' % kb, GELB)
        for _w, p in h['umriss_spitzen']:
            for a in ks:
                px, py = int((p[0] - box[0]) * f), int((p[1] - box[1]) * f)
                cv2.drawMarker(a, (px, py), MAG, cv2.MARKER_CROSS, 16, 2)
    for e in M['details']['bild2']['flecken']:
        if e['px'] < 100 or e['mitte'][1] > 300:
            continue
        for a in ks:
            marke(a, e['mitte'][0], e['mitte'][1], box, f, '%s %s' % (e['id'][3:], kurzname(e).replace('Gelenkkoerper ', '')), CYAN)
    zeilen.append(reihe(ks))
    t = stapel(zeilen)
    k1 = kopf('m2 Regionen der Nutzerpunkte. Z.1 Bild 1: Schwanzspitze (magenta), weissliche Gelenkflecken (cyan). Z.2 Bild 2: Buckel-Flecken, '
              'Krallen (gelb K<Mesh> = Modellspitze, magenta Kreuz = Spitze im Nutzerumriss), Kiefer. Nutzerumrisse: blau r-Arm, gruen l-Arm, rot UK, orange OK. Gelber Kreis Bild 1 = F1-Zeile 5.', t.shape[1])
    # Kopf nah: Lippe
    k = v4f.kand(oa)
    rz = []
    for clip, cn in ((1, 'Clip 1 Maul offen (Pose Bild 2)'), (2, 'Clip 2 Ruhe')):
        ks = []
        for az, el in ((60, 10), (90, 0), (80, -30)):
            a, K = v4f.rundum_bild(oa, clip, 0, az, el, 420, teile=(22, 23))
            rgb = k.tex.lut[K['zeile'].clip(0), K['eintrag'].clip(0)][..., :3]
            L = v4f.lab(rgb)
            zahn = (K['zeile'] == 7) & K['maske']
            nah = cv2.dilate(zahn.astype(np.uint8), np.ones((9, 9), np.uint8)) > 0
            lipm = nah & K['maske'] & ~zahn
            rot = lipm & (L[..., 1] > 15) & (np.hypot(L[..., 1], L[..., 2]) > 15)
            b = a.copy()
            for z, c in ((0, ROT), (6, (255, 160, 0)), (14, CYAN)):
                mm = rot & (K['zeile'] == z)
                b[mm] = c
            b[rot & ~np.isin(K['zeile'], (0, 6, 14))] = MAG
            schrift(a, '%s, %d/%d, roh' % (cn, az, el), 6, 18, 0.4)
            schrift(b, 'Lippe rot: Z0 rot, Z6 orange, Z14 cyan', 6, 18, 0.4)
            ks += [a, b]
        rz.append(reihe(ks))
    t3 = stapel(rz)
    k3 = kopf('Lippe (Asset, Kopf+Kiefer Meshes 22/23, weisses Licht): Kopf | Lippensaum (9 px um die Zaehne) mit roter Albedo (a* > 15, C* > 15) nach Zeile.', t3.shape[1])
    cv2.imwrite(aus + '/m2_regionen.png', cv2.cvtColor(stapel([k1, t, k3, t3]), cv2.COLOR_RGB2BGR))


if __name__ == '__main__':
    cache, mj, aus = sys.argv[1], sys.argv[2], sys.argv[3]
    M = json.load(open(mj, encoding='utf-8'))
    m1(cache, M, aus)
    m2(cache, M, aus)
    print('Tafeln:', aus)
