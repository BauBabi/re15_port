# -*- coding: utf-8 -*-
"""Pruefer Spielmodell, weitere Masse:
(a) Durchblick getrennt: nur Rueckseiten von Rumpf und Gliedern zaehlen, Zaehne, Klauen, Zehen und Hauer nicht (einseitige Kegel verschwinden nur von hinten).
(b) Malerfehler: Bild ohne Tiefenpuffer (Zeichenweg der Buehne) gegen Z-Puffer. Anteil Figurpixel, wo der Maler einen anderen Knochen zeigt.
(c) Bodenlage je Glied in Clip 0, 1, 2 und lose Stuecke je Netzteil.
(d) Verjuengung: Querschnitt entlang der Kette.
Ausgabe mass.json, Text auf stdout."""
import cv2, trimesh
from u_basis import *
from u_posen import blickachsen, BEIN_L, BEIN_R, ARM_L, ARM_R, SCHWANZ, durchblick

RUMPF_GLIEDER = [I[x] for x in ('huefte', 'wirbel', 'brust', 'hals', 'oberarm_l', 'unterarm_l', 'hand_l', 'oberarm_r', 'unterarm_r', 'hand_r',
                                'oberschenkel_l', 'unterschenkel_l', 'oberschenkel_r', 'unterschenkel_r',
                                'schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6')]


def koerper_durchblick(form, W_, tris, gross=900, azs=range(0, 360, 30), els=(-35, 0, 20, 40, 65)):
    alle = np.vstack([w for w in W_ if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.46 / rad
    ke = np.ones((3, 3), np.uint8)
    summe = 0
    for az in azs:
        for el in els:
            A = blickachsen(az, el)
            vorne = np.zeros((gross, gross), np.uint8)
            hinten = np.zeros((gross, gross), np.uint8)
            for b, (w, F) in enumerate(zip(W_, tris)):
                if len(F) == 0:
                    continue
                q = (w - mitte) @ A.T
                xy = np.round(q[:, :2] * s * 16 + gross * 8).astype(np.int32)
                t = xy[F]
                d = (t[:, 1, 0] - t[:, 0, 0]) * (t[:, 2, 1] - t[:, 0, 1]) - (t[:, 2, 0] - t[:, 0, 0]) * (t[:, 1, 1] - t[:, 0, 1])
                for tt, dd in zip(t, d):
                    if dd > 0:
                        cv2.fillConvexPoly(vorne, tt, 1, lineType=cv2.LINE_8, shift=4)
                    elif b in RUMPF_GLIEDER:
                        cv2.fillConvexPoly(hinten, tt, 1, lineType=cv2.LINE_8, shift=4)
            loch = cv2.morphologyEx(((hinten > 0) & (vorne == 0)).astype(np.uint8), cv2.MORPH_OPEN, ke)
            summe += int(loch.sum())
    return summe


def maler_fehler(form, W_, gross=500, azs=(0, 45, 90, 135, 180, 225, 270, 315), els=(10, 35)):
    """Je Blick: Knochen je Pixel im Maler (Mitteltiefe, fern zuerst) gegen Z-Puffer. Rueckgabe: Anteil Pixel mit anderem Knochen, Summe ueber Blicke."""
    alle = np.vstack([w for w in W_ if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.46 / rad
    fehl = fig = 0
    for az in azs:
        for el in els:
            A = blickachsen(az, el)
            L = []
            for b, (w, m) in enumerate(zip(W_, form['meshes'])):
                if len(w) == 0:
                    continue
                q = (w - mitte) @ A.T
                xy = q[:, :2] * s + gross / 2
                z = q[:, 2]
                for f in m['dreiecke']:
                    e = list(f['e'])
                    p = xy[e]
                    if (p[0, 0] * p[1, 1] + p[1, 0] * p[2, 1] + p[2, 0] * p[0, 1] - p[0, 0] * p[2, 1] - p[1, 0] * p[0, 1] - p[2, 0] * p[1, 1]) > 0:
                        L.append((z[e].mean(), p, z[e], b))
                for f in m['vierecke']:
                    a_, b_, c_, d_ = f['e']
                    p = xy[[a_, b_, c_]]
                    if (p[0, 0] * p[1, 1] + p[1, 0] * p[2, 1] + p[2, 0] * p[0, 1] - p[0, 0] * p[2, 1] - p[1, 0] * p[0, 1] - p[2, 0] * p[1, 1]) >= 0:
                        for e in ([a_, b_, d_], [a_, d_, c_]):
                            L.append((z[e].mean(), xy[e], z[e], b))
            mal = np.full((gross, gross), -1, np.int16)
            zb = np.full((gross, gross), 1e18)
            zid = np.full((gross, gross), -1, np.int16)
            for zm, p, zz, b in sorted(L, key=lambda t: -t[0]):
                cv2.fillConvexPoly(mal, np.round(p * 16).astype(np.int32), int(b), lineType=cv2.LINE_8, shift=4)
            for zm, p, zz, b in L:
                x0, x1 = int(max(np.floor(p[:, 0].min()), 0)), int(min(np.ceil(p[:, 0].max()) + 1, gross))
                y0, y1 = int(max(np.floor(p[:, 1].min()), 0)), int(min(np.ceil(p[:, 1].max()) + 1, gross))
                if x0 >= x1 or y0 >= y1:
                    continue
                PX, PY = np.meshgrid(np.arange(x0, x1) + 0.0, np.arange(y0, y1) + 0.0)
                d = (p[1, 0] - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (p[1, 1] - p[0, 1])
                if abs(d) < 1e-9:
                    continue
                w1 = ((PX - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (PY - p[0, 1])) / d
                w2 = ((p[1, 0] - p[0, 0]) * (PY - p[0, 1]) - (PX - p[0, 0]) * (p[1, 1] - p[0, 1])) / d
                w0 = 1 - w1 - w2
                inn = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
                if not inn.any():
                    continue
                zp = w0 * zz[0] + w1 * zz[1] + w2 * zz[2]
                sub = zb[y0:y1, x0:x1]
                neu = inn & (zp < sub)
                sub[neu] = zp[neu]
                zid[y0:y1, x0:x1][neu] = b
            f_ = (mal >= 0) & (zid >= 0)
            fig += int(f_.sum())
            fehl += int((f_ & (mal != zid)).sum())
    return fehl / max(fig, 1)


def querschnitt(V, F, achse, n=9):
    """Flaecherkreis-Radius des Querschnitts an n Stationen entlang achse (0 x, 1 y), Mitte des Netzes. Konvexe Huelle der Schnittpunkte."""
    a = V[:, achse]
    lo, hi = a.min(), a.max()
    st = lo + (hi - lo) * (np.arange(n) + 0.5) / n
    aus = []
    q = [i for i in range(3) if i != achse]
    for s in st:
        pts = []
        for t in F:
            for i, j in ((0, 1), (1, 2), (2, 0)):
                pa, pb = V[t[i]], V[t[j]]
                da, db = pa[achse] - s, pb[achse] - s
                if da * db < 0:
                    pts.append(pa + (pb - pa) * da / (da - db))
        if len(pts) < 3:
            aus.append(0.0)
            continue
        P = np.array(pts)[:, q].astype(np.float32)
        h = cv2.convexHull(P)
        aus.append(math.sqrt(max(cv2.contourArea(h), 0) / math.pi))
    return st, np.array(aus)


def profil(form, b, achse, zeichen=1):
    m = form['meshes'][b]
    V = np.array(m['ecken'], float).reshape(-1, 3)
    if len(V) == 0:
        return None
    return querschnitt(V, dreiecke(m), achse)


def lose_stuecke(form, R, T):
    """Netz-Inseln je Knochen, deren Abstand zum Rest des Modells groesser als 12 Einheiten ist."""
    from scipy.spatial import cKDTree
    n = len(form['knochen'])
    W = welt(form, R, T)
    aus = []
    for b in range(n):
        m = form['meshes'][b]
        if not m['ecken']:
            continue
        F = dreiecke(m)
        V = W[b]
        rund = np.round(np.array(m['ecken'], float), 0)
        u, inv = np.unique(rund, axis=0, return_inverse=True)
        inv = inv.reshape(-1)
        E = np.vstack([inv[F[:, [0, 1]]], inv[F[:, [1, 2]]], inv[F[:, [2, 0]]]])
        comps = trimesh.graph.connected_components(E, nodes=np.arange(len(u)), min_len=1)
        comps = [c for c in comps if len(c) >= 3]
        if len(comps) <= 1:
            continue
        # Proben je Insel
        Pw = u @ R[b].T + T[b]
        for ci, c in enumerate(comps):
            P = Pw[c]
            rest = []
            for b2 in range(n):
                if not len(W[b2]):
                    continue
                if b2 == b:
                    o = [x for x in range(len(comps)) if x != ci]
                    idx = np.concatenate([comps[x] for x in o])
                    rest.append(Pw[idx])
                else:
                    rest.append(W[b2])
            baum = cKDTree(np.vstack(rest))
            d, _ = baum.query(P, k=1)
            if d.min() > 60:
                aus.append((b, NAMEN[b] if b < len(NAMEN) else 'b%d' % b, len(c), round(float(d.min()), 1)))
    return aus


if __name__ == "__main__":
    erg = {}
    for k in ('A', 'B', 'C', 'K34', 'K33', 'K28', 'K23'):
        form = lade(k)
        n = len(form['knochen'])
        tris = [dreiecke(m) for m in form['meshes'][:n]]
        e = {}
        if k in ROHBAU:
            for c, nm in ((2, 'ruhe'), (0, 'pose1'), (1, 'pose2')):
                R, T = pose(form, c)
                W_ = welt(form, R, T)
                j, bst, s = durchblick(W_, tris)
                ges = sum(j.values())
                kp = koerper_durchblick(form, W_, tris)
                low = {}
                for g, ch in (('bein_r', BEIN_R), ('bein_l', BEIN_L), ('arm_r', ARM_R), ('arm_l', ARM_L), ('schwanz', SCHWANZ)):
                    low[g] = round(-max(float(W_[i][:, 1].max()) for i in teilbaum(form, ch[0]) if len(W_[i])))
                wrz = round(float(T[0][1]))
                mal = maler_fehler(form, W_)
                print(k, nm, 'Durchblick gesamt', ges, 'Koerper', kp, 'Hoehe je Glied ueber Boden', low, 'Wurzel y', wrz, 'Malerfehler %.3f' % mal)
                print('    lose Stuecke', lose_stuecke(form, R, T))
                e[nm] = dict(durchblick=ges, koerper=kp, hoehe=low, maler=round(mal, 4))
        else:
            cl = {'K34': (5, 40), 'K33': (0, 0), 'K28': (0, 0), 'K23': (0, 0)}[k]
            R, T = pose(form, *cl)
            W_ = welt(form, R, T)
            j, bst, s = durchblick(W_, tris)
            ges = sum(j.values())
            mal = maler_fehler(form, W_)
            print(k, 'Durchblick gesamt', ges, 'Malerfehler %.3f' % mal)
            e['clip'] = dict(durchblick=ges, maler=round(mal, 4))
        erg[k] = e
    # Verjuengung
    print('--- Querschnitt, Flaecherkreis-Radius an 9 Stationen, Knochenraum')
    KETTEN = {
        'A': (('Arm l', [(I['oberarm_l'], 1), (I['unterarm_l'], 1), (I['hand_l'], 1)]), ('Bein l', [(I['oberschenkel_l'], 1), (I['unterschenkel_l'], 1), (I['fuss_l'], 1)]),
              ('Schwanz', [(I['schwanz%d' % i], 0) for i in range(1, 7)])),
    }
    KETTEN['B'] = KETTEN['C'] = KETTEN['A']
    KETTEN['K28'] = (('Arm', [(9, 1), (10, 1), (11, 1)]), ('Bein', [(13, 1), (14, 1)]), ('Schwanz', [(18, 1), (19, 1), (20, 1), (21, 1), (22, 1)]))
    KETTEN['K34'] = (('Arm', [(12, 1), (13, 1), (14, 1)]), ('Bein', [(1, 1), (2, 1), (3, 1)]))
    KETTEN['K33'] = (('Arm', [(11, 1), (12, 1), (13, 1)]), ('Bein', [(23, 1), (24, 1)]))
    KETTEN['K23'] = (('Schwanz', [(11, 0), (12, 0), (13, 0), (14, 0), (15, 0)]),)
    pr = {}
    for k, ks in KETTEN.items():
        form = lade(k)
        pr[k] = {}
        for nm, ch in ks:
            z = []
            for b, ac in ch:
                p = profil(form, b, ac)
                if p is None:
                    continue
                st, r = p
                if ac == 0 and st[0] > st[-1]:
                    pass
                z.append((b, st, r))
            txt = []
            for b, st, r in z:
                rr = r[::-1] if nm == 'Schwanz' and k != 'K28' else r    # Schwanz der Rohbauten laeuft nach -X: Wurzel zuerst
                v = rr[1:-1]
                txt.append('b%d L%d r %s  Bauch %.2f  vorn/hinten %.2f  schlank %.1f' % (b, st[-1] - st[0], np.round(rr).astype(int).tolist(), v.max() / max((v[0] + v[-1]) / 2, 1), v[:3].mean() / max(v[-3:].mean(), 1), (st[-1] - st[0]) / max(2 * v.mean(), 1)))
            print(k, nm)
            for t in txt:
                print('   ', t)
            pr[k][nm] = txt
    erg['profil'] = pr
    json.dump(erg, open(AUS + '/mass.json', 'w'), indent=1, default=str)
