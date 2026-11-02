#!/usr/bin/env python3
"""Ergebnis ansehen: Tiefenbild aus Ellipsoid-Ketten, Restfehler je Teil, Blaetter.

    python ansicht.py lauf/s1.pt --beste 3             Tabelle, Blaetter der drei besten Starts
"""
import argparse, json, math, os, sys
import numpy as np, cv2, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT             # noqa: E402
import rig as RG             # noqa: E402
import ziele as ZI           # noqa: E402

F = FT.F
GER, DT = FT.GER, FT.DT
FARBE = {'tail': (80, 255, 80), 'left_leg': (0, 160, 255), 'right_leg': (255, 230, 0), 'right_arm': (255, 60, 255),
         'part_of_Body': (160, 160, 255), 'face_and_upper_body': (0, 255, 255), 'left_arm_front': (60, 255, 60),
         'right_arm_front': (255, 120, 40), 'frei': (150, 150, 150)}


def knochen_teil(nr):
    t = ['frei'] * RG.N
    for n, ks in RG.TEIL_KNOCHEN[nr].items():
        for k in ks:
            t[RG.IDX[k]] = n
    return t


def tiefenbild(st, ger, b, H=480, W=640, stride=1):
    """Harte Tiefe: je Pixel der vorderste Knochen. Liefert kn[H,W] (-1 leer), z[H,W]."""
    ys, xs = np.mgrid[0:H:stride, 0:W:stride]
    px = torch.tensor(xs.ravel(), dtype=DT, device=GER)
    py = torch.tensor(ys.ravel(), dtype=DT, device=GER)
    z_best = torch.full((px.shape[0],), 1e9, device=GER)
    kn_best = torch.full((px.shape[0],), -1, device=GER, dtype=torch.long)
    with torch.no_grad():
        u, v = st['u'][b:b + 1], st['v'][b:b + 1]
        q = tuple(x[b:b + 1] for x in st['q'])
        reff = st['reff'][b:b + 1]
        z, rz = st['z'][b:b + 1], st['rz'][b:b + 1]
        for j in range(0, ger.K, 8):
            je = min(j + 8, ger.K)
            sd, f = RG.abstand(px, py, u[:, j:je], v[:, j:je], q[0][:, j:je], q[1][:, j:je], q[2][:, j:je], reff[:, j:je])
            zp = z[:, j:je, None] - rz[:, j:je, None] * torch.sqrt((1.0 - f * f).clamp(min=0.0))
            zp = torch.where(sd < 0, zp, torch.full_like(zp, 1e9))[0]
            zm, ki = zp.min(0)
            neu = zm < z_best
            z_best[neu] = zm[neu]
            kn_best[neu] = ger.st_kn[j:je][ki[neu]]
    return kn_best.reshape(ys.shape).cpu().numpy(), z_best.reshape(ys.shape).cpu().numpy()


def restfehler(kn, nr, z_map, m, dil=4):
    """Sichtbares Teil gegen Nutzer-Maske: IoU, Deckung, Ueberstand, Kantenabstand (Median, px)."""
    t = knochen_teil(nr)
    aus = {}
    lab = np.full(kn.shape, -1)
    namen = ZI.TEILE[nr]
    idx = {n: i for i, n in enumerate(namen)}
    for b in range(RG.N):
        if t[b] in idx:
            lab[kn == b] = idx[t[b]]
    sil = kn >= 0
    ver = np.zeros_like(sil)
    for n in namen:
        ver |= m[n]
    for n in namen:
        s = lab == idx[n]
        u = m[n]
        iou = (s & u).sum() / max(1, (s | u).sum())
        decke = (s & u).sum() / max(1, u.sum())
        decke_alle = (sil & u).sum() / max(1, u.sum())
        ueber = (s & ~ZI.dil(u, dil)).sum() / max(1, u.sum())
        # Kantenabstand beidseitig
        du = cv2.distanceTransform((~ZI.ero(u, 1)).astype(np.uint8) | 0, cv2.DIST_L2, 3)
        rand_s = s & ~ZI.ero(s, 1)
        d = du[rand_s]
        aus[n] = dict(iou=round(float(iou), 3), decke=round(float(decke), 3), decke_alle=round(float(decke_alle), 3), ueber=round(float(ueber), 3),
                      kante_med=round(float(np.median(d)) if d.size else -1, 1), kante_p90=round(float(np.percentile(d, 90)) if d.size else -1, 1))
    z, _ = ZI.lade()
    z = z[nr]
    neg = (z == 0) & ~ZI.dil(ver, 3)
    fig = z == 255
    aus['_neg_px'] = int((sil & neg).sum())
    aus['_fig_deckung'] = round(float((sil & fig).sum() / max(1, fig.sum())), 3)
    return aus, lab


def blatt(nr, kn, lab, pfad, skala=2, ausschnitt=None, gamma=0.62, m=None):
    v = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
    h, w = v.shape[:2]
    gross = cv2.resize(np.clip(v, 0, 1) ** gamma, None, fx=skala, fy=skala, interpolation=cv2.INTER_CUBIC)
    a = (np.clip(gross, 0, 1) * 255).astype(np.uint8)
    b = a.copy()
    t = knochen_teil(nr)
    kn_g = cv2.resize(kn.astype(np.int16), None, fx=skala, fy=skala, interpolation=cv2.INTER_NEAREST).astype(int)
    farbkarte = np.zeros((RG.N, 3), np.uint8)
    for i in range(RG.N):
        farbkarte[i] = FARBE[t[i]]
    ov = farbkarte[np.clip(kn_g, 0, None)]
    an = kn_g >= 0
    b[an] = (0.45 * b[an] + 0.55 * ov[an]).astype(np.uint8)
    # Knochengrenzen
    rand = np.zeros(kn_g.shape, bool)
    rand[:-1] |= kn_g[:-1] != kn_g[1:]
    rand[:, :-1] |= kn_g[:, :-1] != kn_g[:, 1:]
    b[rand] = (255, 255, 255)
    # Nutzer-Umrisse auf beiden Tafeln
    for n in ZI.TEILE[nr]:
        q = m[n].astype(np.uint8)
        c, _ = cv2.findContours(q, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for k in c:
            k = (k.reshape(-1, 2) * skala + skala // 2).astype(np.int32)
            cv2.polylines(a, [k], True, FARBE[n], 1, cv2.LINE_AA)
    # Modellkontur in Tafel a
    sil = (kn_g >= 0).astype(np.uint8)
    c, _ = cv2.findContours(sil, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, c, -1, (255, 255, 255), 1)
    aus = np.hstack([a, b])
    if ausschnitt:
        x0, y0, x1, y1 = [int(q * skala) for q in ausschnitt]
        aus = np.hstack([a[y0:y1, x0:x1], b[y0:y1, x0:x1]])
    cv2.imwrite(pfad, aus)


def ortho(st, ger, b, wurzel, gier, achse_x, achse_y, s, W, H, cx, cy):
    """Rechtwinklige Ansicht im Figurraum. achse_x/achse_y = Weltachsen als Einheitsvektoren im Figurraum. Tiefe entlang achse_x x achse_y."""
    Rg = np.array([[math.cos(gier), 0, math.sin(gier)], [0, 1, 0], [-math.sin(gier), 0, math.cos(gier)]])
    ax = Rg @ np.array(achse_x, float)
    ay = Rg @ np.array(achse_y, float)
    az = np.cross(ax, ay)
    A = torch.tensor(np.stack([ax, ay, az]), dtype=DT, device=GER)
    o = torch.tensor(wurzel * [1, 0, 1], dtype=DT, device=GER)
    c = (st['c'][b] - o) @ A.T
    def lage(e):
        return e[b] @ A.T
    e0, e1, e2 = lage(st['e0']), lage(st['e1']), lage(st['e2'])
    E11 = (e0[:, 0] ** 2 + e1[:, 0] ** 2 + e2[:, 0] ** 2) * s * s + 1e-3
    E12 = (e0[:, 0] * e0[:, 1] + e1[:, 0] * e1[:, 1] + e2[:, 0] * e2[:, 1]) * s * s
    E22 = (e0[:, 1] ** 2 + e1[:, 1] ** 2 + e2[:, 1] ** 2) * s * s + 1e-3
    det = (E11 * E22 - E12 ** 2).clamp(min=1e-3)
    u = c[:, 0] * s + cx
    v = c[:, 1] * s + cy
    q = (E22 / det, -E12 / det, E11 / det)
    rz = torch.sqrt(e0[:, 2] ** 2 + e1[:, 2] ** 2 + e2[:, 2] ** 2)
    stn = dict(u=u[None], v=v[None], q=tuple(x[None] for x in q), reff=det.pow(.25)[None], z=c[:, 2][None] * 1.0, rz=rz[None])
    return tiefenbild(stn, ger, 0, H, W)


def rundum_bild(par, ger, bilder, b, pfad):
    """Pose 1 und Pose 2: Seite (Figur rechts), Vorn, Hinten, Oben; Knochen farbig nach Klasse, Boden und Mensch 3005 als Mass."""
    with torch.no_grad():
        lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
    Tafeln = []
    for i in range(2):
        s = st[i]
        w = par.wurzel[i][b].detach().cpu().numpy() * 1000
        g = float(par.gier[i][b].detach().cpu())
        reihen = []
        ans = [('Seite von links', (1, 0, 0), (0, 1, 0)), ('Von vorn', (0, 0, 1), (0, 1, 0)), ('Oben', (1, 0, 0), (0, 0, 1))]
        for titel, ax, ay in ans:
            W, H = 460, 340
            sk = 0.052
            cx, cy = W // 2, H - 40
            if titel == 'Oben':
                cy = H // 2
            kn, z = ortho(s, ger, b, w, g, ax, ay, sk, W, H, cx, cy)
            bild = np.full((H, W, 3), 24, np.uint8)
            # Hoehenbezug
            for kx in range(RG.N):
                n = RG.NAMEN[kx]
                k = RG.klasse(n)
                f = (120, 120, 120)
                if k.startswith('schwanz'):
                    f = (80, 255, 80)
                elif 'arm' in k or k in ('hand', 'klaue'):
                    f = (255, 120, 40) if n.endswith('_r') or n[-2:-1] == 'r' else (60, 200, 255)
                elif 'schenkel' in k or k == 'fuss':
                    f = (0, 160, 255) if n.endswith('_l') else (255, 230, 0)
                elif k in ('kopf', 'kiefer', 'hals'):
                    f = (0, 255, 255)
                elif k in ('huefte', 'wirbel', 'brust'):
                    f = (160, 160, 255)
                bild[kn == kx] = f
            # Schattierung nach Tiefe
            an = kn >= 0
            if an.any():
                za = z[an]
                sh = 1.0 - 0.6 * (z - za.min()) / max(1.0, za.max() - za.min())
                bild[an] = (bild[an] * np.clip(sh[an], 0.3, 1)[:, None]).astype(np.uint8)
            rand = np.zeros(kn.shape, bool)
            rand[:-1] |= kn[:-1] != kn[1:]
            rand[:, :-1] |= kn[:, :-1] != kn[:, 1:]
            bild[rand] = (230, 230, 230)
            if titel != 'Oben':
                cv2.line(bild, (0, cy), (W, cy), (120, 120, 120), 1)
                x = W - 30
                cv2.line(bild, (x, cy), (x, int(cy - 3005 * sk)), (255, 255, 255), 1)
                cv2.putText(bild, 'Leon 3005', (x - 70, int(cy - 3005 * sk) - 4), 0, .38, (255, 255, 255), 1, cv2.LINE_AA)
            cv2.putText(bild, 'Pose %d: %s' % (i + 1, titel), (8, 18), 0, .5, (255, 255, 255), 1, cv2.LINE_AA)
            reihen.append(bild)
        Tafeln.append(np.hstack(reihen))
    cv2.imwrite(pfad, np.vstack(Tafeln))




def tabelle(par, ger, bilder, b, m):
    with torch.no_grad():
        lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
    erg = {}
    karten = {}
    for i, bi in enumerate(bilder):
        kn, z = tiefenbild(st[i], ger, b)
        r, lab = restfehler(kn, bi.nr, z, m)
        erg[bi.nr] = r
        karten[bi.nr] = (kn, lab, z)
    return erg, karten, (lae, ver, a3, b3, st)


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--beste', type=int, default=1)
    a.add_argument('--stride', type=int, default=4)
    a.add_argument('--wahl', default='')
    a = a.parse_args()
    ger = RG.Gerippe(GER, DT)
    bilder = [FT.Bild(1, a.stride, ger), FT.Bild(2, a.stride, ger)]
    pfad = a.pt if os.path.isabs(a.pt) else os.path.join(HIER, a.pt)
    par, ids, d = FT.lade(pfad, a.beste, [int(x) for x in a.wahl.split(',')] if a.wahl else None)
    _, m = ZI.lade()
    stem = pfad[:-3]
    print('Kosten', [round(float(d['kosten'][i]), 1) for i in ids], 'von', d['B'])
    for b, i in enumerate(ids):
        erg, karten, (lae, ver, a3, b3, st) = tabelle(par, ger, bilder, b, m)
        L, V, D = FT.als_dict(lae, ver, a3, b3, b)
        print('--- Start', i, 'Kosten %.1f' % float(d['kosten'][i]))
        for nr in (1, 2):
            for n, r in erg[nr].items():
                print('  Bild', nr, n, r)
            kn, lab, z = karten[nr]
            blatt(nr, kn, lab, '%s_%d_b%d.png' % (stem, i, nr), skala=2, m=m, ausschnitt=(130, 125, 490, 480))
        print('  Laengen', {k: int(v) for k, v in L.items()})
        print('  Versatz', {k: round(v, 2) for k, v in V.items()})
        print('  Dicke', {k: ([int(x) for x in v[0]], [int(x) for x in v[1]]) for k, v in D.items()})
        for q in (0, 1):
            print('  Pose', q + 1, 'Wurzel', np.round(par.wurzel[q][b].detach().cpu().numpy() * 1000).tolist(), 'Gier Grad %.1f' % math.degrees(float(par.gier[q][b])))
        rundum_bild(par, ger, bilder, b, '%s_%d_rundum.png' % (stem, i))


if __name__ == '__main__':
    main()
