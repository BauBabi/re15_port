# -*- coding: utf-8 -*-
"""Zahn-Scanner (Auftrag Vorderzaehne 16.11.2026): ueber ALLE Keys einer EMD (Ganzzahl-Pose wie das Spiel: FE.modell -> kf_k5g2_geo.posen,
erstes Bild, das den Key nutzt) - gerechnet im Raum des Kopfes (Knochen 22), der Unterkiefer 23 wird je Key wie im Spiel hineingedreht.

Zaehne: die 34 alten (kf_k5g3_zaehne.alte_oben/alte_unten) und die neuen (neue_reihe), im Netz gefunden ueber ihre Ganzzahl-Ecken.
Gegenflaeche: das geschlossene Rohr des ANDEREN Kiefers (oben -> Unterkieferrohr 23, unten -> Kopfrohr 22), gerundet wie im Netz.
Je Zahn Probepunkte entlang der drei Kanten Basis -> Spitze und der Achse (t = 0 .. 1 in Zehnteln):
  tiefe   groesster Abstand eines Probepunkts INNERHALB der Gegenflaeche zu ihr (Strahl-Paritaet wie kf_k5g3_n52, 3 Richtungen, Mehrheit)
  austritt Zahl der Kanten, die in die Gegenflaeche eintreten UND wieder heraus (= Durchstich: die Spitze kommt auf der anderen Seite heraus)
  zahn    groesste Eindringtiefe in eine Zahn-Pyramide der Gegenreihe (konvex: Abstand zur naechsten Ebene, nur innen)
  Maler-Pruefung (maler): je Ansicht Pixel, an denen der Maler einen neuen/alten Zahn zeigt, der Tiefenpuffer aber etwas anderes (falsch), davon
           'fremd' = der Tiefenpuffer zeigt das ANDERE Kiefernetz (Zahn durch den Gegenkiefer gemalt); der Rest liegt am eigenen Kiefer (Wurzel
           ueber die eigene Lippe gemalt, bei den alten Zaehnen genauso: Wurzeltiefe O1 6,9 / unten 15)
  vor_nase (nur untere) Probepunkte ausserhalb des Kopfrohrs VOR der Schnauze (x > L), die hoeher stehen als deren Mundlinie (R6 d)
Ausgabe je Key und je Gruppe (alt_oben, alt_unten, neu_oben, neu_unten): max tiefe, Summe austritt, max zahn; dazu der Kieferwinkel.

  python kf_k5g3_zaehne_scan.py <emd> <aus.json>
"""
import hashlib
import json
import math
import os
import sys
import time
from multiprocessing import Pool

import numpy as np

GEN = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GEN)
sys.dont_write_bytecode = True
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g3_n52 as N52  # noqa: E402
import kf_k5g3_zaehne as Z  # noqa: E402

FE = G.FE
TS = np.linspace(0.0, 1.0, 11)


def zaehne_im_netz(form, p, clut_y=480):
    """Alle Zahn-Pyramiden in Netz 22/23, aus den Flaechen gefunden (unabhaengig von Laengen/Kappen): Dreiecke der Zahnzeile 2, verbunden
    ueber gleiche Ecken-Lagen, genau 3 Dreiecke und 4 Lagen (die 8 Kehlen-Deckel der Zeile 2 sind keine Pyramide). 'alt' = die Lagen sind die
    eines der 34 alten Zaehne (kf_k5g3_zaehne.alte_*), sonst 'neu'. Rueckgabe Liste dict(gruppe, netz, ecken (G0 G1 G2 Spitze), flaechen,
    seite, i) - i = Rang nach x innerhalb (gruppe, seite)."""
    alt = set()
    for z in Z.alte_oben(p) + Z.alte_unten(p):
        P, _F, _N = Z.zahn_teile(z)
        alt.add((z['netz'], frozenset(tuple(int(round(c)) for c in q) for q in P)))
    out = []
    for t in (Z.OBEN, Z.UNTEN):
        m = form['meshes'][t]
        fl = [(fi, d) for fi, d in enumerate(m['dreiecke']) if ((int(d['clut']) >> 6) & 0x1FF) - clut_y == Z.ZAHN_ZEILE]
        lage = lambda i: tuple(m['ecken'][i][:3])  # noqa: E731
        par = {}

        def find(a):
            while par.setdefault(a, a) != a:
                par[a] = par[par[a]]
                a = par[a]
            return a
        for _fi, d in fl:
            ps = [lage(i) for i in d['e']]
            for q in ps[1:]:
                par[find(ps[0])] = find(q)
        grp = {}
        for fi, d in fl:
            grp.setdefault(find(lage(d['e'][0])), []).append((fi, d))
        for g in grp.values():
            zahl, idx = {}, {}
            for _fi, d in g:
                for i in d['e']:
                    zahl[lage(i)] = zahl.get(lage(i), 0) + 1
                    idx.setdefault(lage(i), i)
            if len(g) != 3 or len(zahl) != 4:
                continue
            sp = max(zahl, key=lambda q: zahl[q])
            basis = sorted(q for q in zahl if q != sp)
            art = 'alt' if (t, frozenset(zahl)) in alt else 'neu'
            bz = sum(q[2] for q in basis) / 3.0
            out.append(dict(gruppe=art + ('_oben' if t == Z.OBEN else '_unten'), netz=t, ecken=[idx[q] for q in basis] + [idx[sp]],
                            flaechen=sorted(fi for fi, _d in g), seite=0 if abs(bz) < 1.0 else (1 if bz > 0 else -1),
                            _x=sum(q[0] for q in basis) / 3.0))
    for key in set((z['gruppe'], z['seite']) for z in out):
        for r, z in enumerate(sorted((z for z in out if (z['gruppe'], z['seite']) == key), key=lambda z: z['_x'])):
            z['i'] = r
    for z in out:
        z.pop('_x')
    return out


_ST = {}


def _init(emd):
    form = FE.lese_emd(emd)
    p = Z.vorgabe()
    zz = zaehne_im_netz(form, p)
    mo = FE.modell(emd, N52.TIM0)
    cb, R, T = G.posen(mo)
    first = {}
    for i, (c, b, k) in enumerate(cb):
        first.setdefault(int(k), (i, c, b))
    V = {t: np.array(form['meshes'][t]['ecken'], np.int64)[:, :3] for t in (Z.OBEN, Z.UNTEN)}
    # Gegenflaechen: die geschlossenen Rohre von Kopf/Kiefer (kf_k5g3_zaehne.huelle_oben/_unten, Ecken gerundet wie im Netz). Das Netz 22
    # selbst taugt nicht fuer die Strahl-Paritaet: Augen, Augenhoecker, Kehlen-Deckel und der Spalt B4 liegen als innere Flaechen darin
    # (erster Lauf: 378 Keys 'Austritt' der ALTEN Unterkieferzaehne). Vorne (Zahnbereich) sind Netz und Rohr gleich (+-0,75, a2.py).
    huelle, hp = {}, {}
    for t, (P, F) in ((Z.OBEN, Z.huelle_oben(p)), (Z.UNTEN, Z.huelle_unten(p))):
        hp[t] = np.rint(P).astype(np.int64)
        huelle[t] = F
    # Mundlinie der Schnauze von vorn (wie kf_k5g3_zaehne.Keypose / R6 d)
    x_vorn = float(p['L'])
    zs = np.linspace(0.0, float(Z.KO.stationen(p)[1][-1]), 48)
    ys = [Z.senkrecht(hp[Z.OBEN].astype(float), huelle[Z.OBEN], x_vorn - 0.5, q) for q in zs]
    lippe = (np.array([q for q, y in zip(zs, ys) if y]), np.array([y[-1] for y in ys if y]))
    _ST.update(form=form, zz=zz, R=R, T=T, first=first, V=V, H=huelle, HP=hp, x_vorn=x_vorn, lippe=lippe)


def _kopfraum(R, T, b, V):
    """Ecken von Knochen b im Raum von Knochen 22 (Ganzzahl wie zr.Kopfraum)."""
    Rh, Th = R[22], T[22]
    RhT = Rh.T
    Rb = (RhT @ R[b]) >> 12
    Tb = (RhT @ (T[b] - Th)) >> 12
    return (((Rb @ V.T) >> 12).T + Tb).astype(float), Rb


def _proben(E):
    """E: 4 x 3 (G0 G1 G2 Spitze). Kanten Gi -> S und Achse c -> S, je 11 Punkte. Rueckgabe (4, 11, 3)."""
    S = E[3]
    c = E[:3].mean(0)
    return np.array([[a + t * (S - a) for t in TS] for a in (E[0], E[1], E[2], c)])


def _innen(P, Tri):
    return np.sum([N52.paritaet(P, Tri, d) for d in N52.DIRS], 0) >= 2


def _ebenen(E):
    """Konvexe Pyramide: Ebenen (n, d) nach aussen."""
    c = E.mean(0)
    out = []
    for a, b, d_ in ((0, 1, 3), (1, 2, 3), (2, 0, 3), (0, 1, 2)):
        n = np.cross(E[b] - E[a], E[d_] - E[a])
        n /= max(np.linalg.norm(n), 1e-9)
        if n @ (c - E[a]) > 0:
            n = -n
        out.append((n, float(n @ E[a])))
    return out


def messe_key(k):
    S = _ST
    i, c, b = S['first'][k]
    R, T = S['R'][i], S['T'][i]
    W = {}
    W[22], _ = _kopfraum(R, T, 22, S['V'][22])
    W[23], R23 = _kopfraum(R, T, 23, S['V'][23])
    winkel = math.degrees(math.acos(max(-1.0, min(1.0, float(R23[0, 0]) / 4096.0))))
    HW = {22: _kopfraum(R, T, 22, S['HP'][22])[0], 23: _kopfraum(R, T, 23, S['HP'][23])[0]}
    tri = {t: HW[t][S['H'][t]] for t in (22, 23)}
    E = [W[z['netz']][z['ecken']] for z in S['zz']]
    erg = {}
    for zi, z in enumerate(S['zz']):
        gegen = 23 if z['netz'] == 22 else 22
        Pr = _proben(E[zi])
        flach = Pr.reshape(-1, 3)
        Tg = tri[gegen]
        lo, hi = flach.min(0) - 1, flach.max(0) + 1
        sel = ((Tg.reshape(-1, 3, 3) >= lo - 2000) & (Tg.reshape(-1, 3, 3) <= hi + 2000)).all((1, 2))
        Tg = Tg[sel]
        inn = _innen(flach, Tg).reshape(Pr.shape[:2]) if len(Tg) else np.zeros(Pr.shape[:2], bool)
        tiefe = 0.0
        if inn.any():
            d2, _X, _ii = G.naechster_punkt(flach[inn.reshape(-1)], Tg[:, 0], Tg[:, 1], Tg[:, 2])
            tiefe = float(np.sqrt(d2.min(1)).max())
        vn = 0
        if z['netz'] == 23:
            q = flach[~inn.reshape(-1)]
            q = q[(q[:, 0] > S['x_vorn']) & (np.abs(q[:, 2]) <= S['lippe'][0][-1])]
            if len(q):
                vn = int((q[:, 1] < np.interp(np.abs(q[:, 2]), *S['lippe'])).sum())
        aus = 0
        for kante in inn:
            ein = np.nonzero(kante)[0]
            if len(ein) and (~kante[ein[0]:]).any():
                aus += 1
        # Zahn gegen Zahn der Gegenreihe
        zt = 0.0
        for zj, w in enumerate(S['zz']):
            if w['netz'] == z['netz']:
                continue
            Ew = E[zj]
            if (flach.min(0) > Ew.max(0)).any() or (flach.max(0) < Ew.min(0)).any():
                continue
            eb = _ebenen(Ew)
            ab = np.array([flach @ n - d for n, d in eb])        # < 0 innen
            drin = (ab < 0).all(0)
            if drin.any():
                zt = max(zt, float((-ab[:, drin]).min(0).max()))
        g = z['gruppe']
        e = erg.setdefault(g, dict(tiefe=0.0, austritt=0, zahn=0.0, arg=None, vor_nase=0))
        e['vor_nase'] += vn
        if tiefe > e['tiefe']:
            e['tiefe'], e['arg'] = round(tiefe, 2), '%s%d' % ('+' if z['seite'] >= 0 else '-', z['i'])
        e['austritt'] += aus
        e['zahn'] = max(e['zahn'], round(zt, 2))
    return k, dict(clip_bild=[c, b], kiefer=round(winkel, 2), gruppen=erg)


def lauf(emd, n=6):
    _init(emd)
    keys = sorted(_ST['first'])
    with Pool(n, initializer=_init, initargs=(emd,)) as p:
        return dict(p.map(messe_key, keys, chunksize=8)), len(_ST['zz'])


# ------------------------------------------------------------------ Maler-Pruefung (sichtbares Durchstechen im Port-Zeichenweg)
NAH = {   # Kopfraum-Kameras (pos, ziel, h) wie die Messbilder: von vorn, schraeg links/rechts, Seite links/rechts
    'nah_vorn': ((5903, 250, 0), (1300, 250, 0), 900),
    'nah_schraeg_l': ((4800, -900, 2900), (1250, 280, 0), 640),
    'nah_schraeg_r': ((4800, -900, -2900), (1250, 280, 0), 640),
    'nah_seite_l': ((1300, 250, 6000), (1300, 250, 0), 700),
    'nah_seite_r': ((1300, 250, -6000), (1300, 250, 0), 700),
}
SPIEL = ('spiel5080', 'spiel5080_vorn', 'spiel5090', 'bild2')
_MW = {}


def _maler_init(emd):
    _init(emd)
    import form_buehne as FB
    mo = FE.modell(emd, N52.TIM0)
    kl = {}
    for z in _ST['zz']:
        for fi in z['flaechen']:
            kl[(z['netz'], fi)] = 2 if z['gruppe'].startswith('neu') else 1
    kam = {n: FB.Kamera.aus_rid(pos, ziel, h=h) for n, (pos, ziel, h) in NAH.items()}
    _MW.update(mo=mo, kl=kl, kam=kam, an=G.ansichten())


def _klassen(tris):
    kl = _MW['kl']
    return np.array([kl.get((t[3], t[4][0]), 0) if not t[4][1] else 0 for t in tris] + [-1], np.int8)


def _maler_key(k):
    S = _ST
    i, c, b = S['first'][k]
    R, T = S['R'][i], S['T'][i]
    mo = _MW['mo']
    out = {}
    Rh, Th = R[22], T[22]
    RhT = Rh.T
    RK = np.array([(RhT @ R[q]) >> 12 for q in range(len(R))])
    TK = np.array([(RhT @ (T[q] - Th)) >> 12 for q in range(len(T))])
    jobs = [(n, _MW['kam'][n], RK, TK, (0, 0, 0), 0, 480, 640, dict(ax=2.0, bx=0.0, ay=2.0, by=0.0)) for n in NAH]
    for n in SPIEL:
        kam, lage, gier, H, W, ab = _MW['an'][n]
        jobs.append((n, kam, R, T, lage, gier, H, W, ab))
    for n, kam, RR, TT, lage, gier, H, W, ab in jobs:
        tris = G.dreiecke_teile(mo, RR, TT, lage, gier, kam, teile=(22, 23))
        mt, pt, mi, pi_ = G.keil_raster(tris, H, W, ab)
        kl = _klassen(tris)
        km, kp = kl[mi], kl[pi_]
        fremd = (pt != mt) & (pt >= 0)            # der Tiefenpuffer zeigt das ANDERE Netz (Kiefer) als der gemalte Zahn
        out[n] = dict(neu_falsch=int(((km == 2) & (kp != 2)).sum()), alt_falsch=int(((km == 1) & (kp != 1)).sum()),
                      neu_fremd=int(((km == 2) & (kp != 2) & fremd).sum()), alt_fremd=int(((km == 1) & (kp != 1) & fremd).sum()),
                      neu_sicht=int((kp == 2).sum()), alt_sicht=int((kp == 1).sum()))
    return k, out


def maler_lauf(emd, n=12):
    _init(emd)
    keys = sorted(_ST['first'])
    with Pool(n, initializer=_maler_init, initargs=(emd,)) as p:
        return dict(p.map(_maler_key, keys, chunksize=4))


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'maler':
    emdp, aus = sys.argv[2:4]
    emd = open(emdp, 'rb').read()
    t0 = time.time()
    m = maler_lauf(emd)
    json.dump(dict(emd=emdp.replace(chr(92), '/'), sha256=hashlib.sha256(emd).hexdigest(), keys={str(k): v for k, v in sorted(m.items())}),
              open(aus, 'w'), indent=0)
    zs = {}
    for k, e in m.items():
        for n, v in e.items():
            s = zs.setdefault(n, dict(neu_falsch=0, alt_falsch=0, neu_max=(0, None), alt_max=(0, None), neu_sicht=0, alt_sicht=0))
            for a in ('neu_falsch', 'alt_falsch', 'neu_sicht', 'alt_sicht'):
                s[a] += v[a]
            if v['neu_falsch'] > s['neu_max'][0]:
                s['neu_max'] = (v['neu_falsch'], k)
            if v['alt_falsch'] > s['alt_max'][0]:
                s['alt_max'] = (v['alt_falsch'], k)
    print('Maler-Scan %s: %d Keys, %.0f s' % (os.path.basename(emdp), len(m), time.time() - t0))
    for n, s in zs.items():
        print('  %-15s neu falsch %6d (max %s) sichtbar %7d | alt falsch %6d (max %s) sichtbar %7d' % (n, s['neu_falsch'], s['neu_max'], s['neu_sicht'],
                                                                                                s['alt_falsch'], s['alt_max'], s['alt_sicht']))
    sys.exit(0)


if __name__ == '__main__':
    emdp, aus = sys.argv[1:3]
    emd = open(emdp, 'rb').read()
    t0 = time.time()
    m, nz = lauf(emd)
    zus = {}
    for k, e in m.items():
        for g, v in e['gruppen'].items():
            s = zus.setdefault(g, dict(tiefe_max=0.0, tiefe_key=None, austritt_keys=0, austritt_summe=0, zahn_max=0.0, zahn_key=None, vor_nase_keys=0))
            if v.get('vor_nase'):
                s['vor_nase_keys'] += 1
            if v['tiefe'] > s['tiefe_max']:
                s['tiefe_max'], s['tiefe_key'] = v['tiefe'], [k] + e['clip_bild'] + [v['arg'], e['kiefer']]
            if v['austritt']:
                s['austritt_keys'] += 1
                s['austritt_summe'] += v['austritt']
            if v['zahn'] > s['zahn_max']:
                s['zahn_max'], s['zahn_key'] = v['zahn'], [k] + e['clip_bild']
    json.dump(dict(emd=emdp.replace(chr(92), '/'), sha256=hashlib.sha256(emd).hexdigest(), zaehne=nz, zusammen=zus,
                   keys={str(k): v for k, v in sorted(m.items())}), open(aus, 'w'), indent=0)
    print('Zahn-Scan %s (sha %s): %d Keys, %d Zaehne, %.0f s' % (os.path.basename(emdp), hashlib.sha256(emd).hexdigest()[:12], len(m), nz,
                                                                time.time() - t0))
    for g, s in sorted(zus.items()):
        print('  %-10s tiefe max %.1f %s | Austritt %d Keys (%d Kanten) | Zahn-in-Zahn max %.1f %s | vor der Nase %d Keys'
              % (g, s['tiefe_max'], s['tiefe_key'], s['austritt_keys'], s['austritt_summe'], s['zahn_max'], s['zahn_key'], s['vor_nase_keys']))
