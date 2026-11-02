#!/usr/bin/env python3
"""Varianten, Geometrie: fb_bau.baue_form mit zwei Eingriffen. Kein RE-Wert.

E9 (Schwanz optisch ein Stueck, Knochen bleiben):
 - Profil: aus dem Endstand von form_bemalt (Ringe nach Glaetter und Nachpassung, Rastlage) je Rundum-Winkel Radius und Mitte ueber die ganze Kette,
   mit einem Ausgleichsspline (Knoten alle SPLINE_KNOTEN Einheiten) geglaettet. So verschwinden die Baeuche je Glied (264-337-339-275 | 275-342-...),
   der Verlauf ueber die Kette bleibt.
 - Gelenk: Elternglied endet, Kindglied beginnt mit DEMSELBEN runden Ring am Drehpunkt (Ursprung des Kindknochens). Dahinter je zwei Kugelringe
   (30 und 62 Grad) und die Spitze im Abstand r: eine Kugel um den Drehpunkt dreht sich in sich, darum fuellt sie die Aussenseite der Beuge ohne Stufe.
   Bisher: Ueberlappringe mit 0,8 r und Kegelspitze 30-40 Einheiten, gebeugt eine Kerbe von bis zu 0,3 r.
 - Normalen: Gelenkring radial, Kugelringe von der Kugelmitte aus, Spitze axial. Gelenkring und Nachbarring vorgedreht um die halbe mittlere Gelenkdrehung
   aus Ruhepose und Pose 1 (Elternteil zum Kind hin, Kind zurueck): in beiden Posen treffen sich die Normalen am Gelenk bis auf die halbe Differenz.
 - Keine Nachpassung je Ring am Schwanz (sie brachte die Glieder-Struktur zurueck); Breite kommt aus dem geglaetteten Endstand.
Weniger Flaechen (flaechen='wenig'): Ecken rundum je Art kleiner, Zwischenringe seltener, Glaettung allein ueber die Normalen (fb_glatt)."""
import math

import numpy as np
from scipy.interpolate import LSQUnivariateSpline
from scipy.spatial.transform import Rotation, Slerp

import os

import fb_bau
import fb_form
import fb_glatt
import form_emd as FE

SCHWANZ = ['schwanz%d' % i for i in range(1, 7)]
HIER_V = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
SPLINE_KNOTEN = 750.0       # Einheiten; die Glieder sind 592-678 lang, ein Knoten je Glied laesst keinen Bauch je Glied zu. Frei gesetzt.
GELENK_RUND = 220.0         # Zone um das Gelenk, in der der Querschnitt rund wird (Kugel muss rund sein, sonst dreht sie nicht in sich)
RINGABSTAND = 300.0         # Ringe im Glied hoechstens so weit
KUGEL = (30.0, 62.0)        # Grad der Kugelringe hinter dem Gelenkring
M = 10                      # Ecken rundum (fb_glatt.ECKEN_ART schwanz)


def bindversatz(g):
    off = []
    for k in g['knochen']:
        v = np.array(k['versatz'], float)
        off.append(v if k['eltern'] < 0 else off[k['eltern']] + v)
    return np.array(off)


PROFIL_WINKEL = 40          # Winkelraster des Profils; die Ringe tasten es mit M Ecken ab


def _ring_rad(P, c, ea, eb, k=PROFIL_WINKEL):
    q = P - c
    a = np.arctan2(q @ eb, q @ ea)
    r = np.hypot(q @ ea, q @ eb)
    o = np.argsort(a)
    a, r = a[o], r[o]
    phi = 2 * np.pi * np.arange(k) / k
    phi = np.where(phi > np.pi, phi - 2 * np.pi, phi)
    return np.interp(phi, np.concatenate([a - 2 * np.pi, a, a + 2 * np.pi]), np.concatenate([r, r, r]))


def _abtasten(r40, m):
    """Profil (PROFIL_WINKEL Winkel) -> m Ecken, periodisch linear."""
    k = len(r40)
    x = np.arange(m) * k / m
    i0 = np.floor(x).astype(int) % k
    f = x - np.floor(x)
    return r40[i0] * (1 - f) + r40[(i0 + 1) % k] * f


def profil(teile, Gs, g):
    """Messpunkte (s, Mitte y/z in Rastlage, Radius je Winkel) aus den sichtbaren Ringen der Glieder (ohne Ueberlapp)."""
    idx = {p.name: i for i, p in enumerate(teile)}
    off = bindversatz(g)
    p1 = teile[idx['schwanz1']]
    x0 = off[p1.knochen][0]
    S, CY, CZ, R = [], [], [], []
    gelenke = []
    for k_, nm in enumerate(SCHWANZ):
        i = idx[nm]
        p, G = teile[i], Gs[i]
        tj = None
        if k_ + 1 < len(SCHWANZ):
            pk = teile[idx[SCHWANZ[k_ + 1]]]
            tj = float((np.array(g['knochen'][pk.knochen]['versatz'], float) - p.o) @ p.d)
            gelenke.append(-(off[pk.knochen][0] - x0))
        for r in range(G['NR']):
            if G['rolle'][r] != 'k':
                continue
            P = G['ring_pts'][r]
            c = P.mean(0)
            t = float((c - p.o) @ p.d)
            if (k_ > 0 and t < 0) or (tj is not None and t > tj) or (k_ == 0 and t < -60):
                continue
            cb = c + off[p.knochen]
            S.append(-(cb[0] - x0))
            CY.append(cb[1])
            CZ.append(cb[2])
            R.append(_ring_rad(P, c, p.ea, p.eb))
    o = np.argsort(S)
    return np.array(S)[o], np.array(CY)[o], np.array(CZ)[o], np.array(R)[o], gelenke, x0


def glaetten(S, Y):
    s0, s1 = S[0], S[-1]
    n = max(1, int(round((s1 - s0) / SPLINE_KNOTEN)))
    kn = np.linspace(s0, s1, n + 1)[1:-1]
    S2 = S.copy()
    for i in range(1, len(S2)):
        if S2[i] <= S2[i - 1]:
            S2[i] = S2[i - 1] + 1e-3
    return LSQUnivariateSpline(S2, Y, kn, k=3)


def schwanz_e9(teile, rk, Gs, g, clips_rel, quelle):
    """rk / Gs der Schwanzglieder ersetzen. clips_rel: je Gelenk die relative Drehung (Kind gegen Eltern) in Ruhe und Pose 1 (Rotation-Objekte).
    quelle = profil(...) aus dem vollen Endstand (form_bemalt-Topologie, zw/delta.json), fuer alle Varianten dieselbe."""
    idx = {p.name: i for i, p in enumerate(teile)}
    off = bindversatz(g)
    S, CY, CZ, R, gelenke, x0 = quelle
    fy, fz = glaetten(S, CY), glaetten(S, CZ)
    fr = [glaetten(S, R[:, k]) for k in range(R.shape[1])]
    phi = 2 * np.pi * np.arange(M) / M
    p1 = teile[idx['schwanz1']]
    ea, eb, d = np.asarray(p1.ea, float), np.asarray(p1.eb, float), np.asarray(p1.d, float)
    yz_achse = off[p1.knochen][1:]          # Kettenachse in Rastlage: y, z der Knochenursprungs-Linie

    # Gleicher Innenradius wie mit 10 Ecken (Kantenmitte eines M-Ecks liegt bei cos(pi/M) r): mit 8 Ecken lag die Schwanzoberkante in Bild 1
    # 2-3 px tiefer (Lauf _probe/G8, scheitel_x322). Fuer M = 10 ist der Faktor 1.
    inkreis = math.cos(math.pi / 10) / math.cos(math.pi / M)

    def radius(s):
        r = _abtasten(np.array([float(f(s)) for f in fr]), M) * inkreis
        w = 0.0
        for sj in gelenke:
            w = max(w, fb_form.glatt(1 - abs(s - sj) / GELENK_RUND))
        return r * (1 - w) + r.mean() * w

    def mitte(s, rund=True):
        """Mitte in Rastlage. Am Gelenk auf die Achse (Drehpunkt), sonst der geglaettete Verlauf."""
        c = np.array([x0 - s, float(fy(s)), float(fz(s))])
        if rund:
            w = 0.0
            for sj in gelenke:
                w = max(w, fb_form.glatt(1 - abs(s - sj) / GELENK_RUND))
            c[1:] = c[1:] * (1 - w) + yz_achse * w
        return c

    def ring(s, kn_off, r=None, c=None):
        r = radius(s) if r is None else r
        c = mitte(s) if c is None else c
        return (c - kn_off)[None] + (r * np.cos(phi))[:, None] * ea + (r * np.sin(phi))[:, None] * eb
    info = {}
    s_end = S[-1]
    for k_, nm in enumerate(SCHWANZ):
        i = idx[nm]
        p = teile[i]
        o_ = off[p.knochen]
        s_a = 0.0 if k_ == 0 else gelenke[k_ - 1]
        s_b = gelenke[k_] if k_ < len(gelenke) else s_end
        rollen, ringe = [], []
        k0, k1 = rk[i][1], rk[i][2]
        if k_ == 0:
            # Wurzel bleibt wie im Endstand (A1, im Becken): Ringe vor t = -60 aus dem geglaetteten Endstand mit Nachpassung
            # Wurzelringe aus dem vollen Endstand (mit Zwischenringen), auf M Ecken abgetastet: ohne sie schnitt G die Bogenoberkante ab (scheitel_x322 +10,5 px)
            for P in _QUELLE.get('wurzel', []):
                c = P.mean(0)
                q = P - c
                ax = (q @ d)[:, None] * d
                r40 = _ring_rad(P, c, ea, eb)
                rr = _abtasten(r40, M)
                ringe.append(c[None] + float(ax.mean()) * 0 + (rr * np.cos(phi))[:, None] * ea + (rr * np.sin(phi))[:, None] * eb)
                rollen.append('wurzel')
            s_a = float(S[0])
        else:
            rj = radius(s_a).mean()
            cj = mitte(s_a)
            for f in reversed(KUGEL):
                fw = math.radians(f)
                ringe.append(ring(s_a, o_, r=np.full(M, rj * math.cos(fw)), c=cj - rj * math.sin(fw) * d))
                rollen.append('kugel0')
            k0 = (cj - rj * d) - o_
        n_in = max(1, int(math.ceil((s_b - s_a) / RINGABSTAND)))
        for q in range(n_in + 1):
            s = s_a + (s_b - s_a) * q / n_in
            ringe.append(ring(s, o_))
            rollen.append('gelenk0' if (q == 0 and k_ > 0) else 'gelenk1' if (q == n_in and k_ < len(gelenke)) else 'k')
        if k_ < len(gelenke):
            rj = radius(s_b).mean()
            cj = mitte(s_b)
            for f in KUGEL:
                fw = math.radians(f)
                ringe.append(ring(s_b, o_, r=np.full(M, rj * math.cos(fw)), c=cj + rj * math.sin(fw) * d))
                rollen.append('kugel1')
            k1 = (cj + rj * d) - o_
        rk[i] = [np.stack(ringe), np.asarray(k0, float), np.asarray(k1, float)]
        G = fb_glatt.glatt_teil(p, *rk[i], verform=None, delta=None)
        if G['rolle'][:len(ringe)] != ['k'] * len(ringe) or G['rolle'].count('k') != len(ringe):
            raise RuntimeError('%s: Glaetter fuegte Ringe hinzu (%d gegen %d, %s)' % (nm, G['NR'], len(ringe), G['rolle']))
        # Normalen: Gelenk radial, Kugel von der Kugelmitte, Spitzen axial
        pts, nrm = G['pts'], G['nrm']
        m = G['m']
        for r, ro in enumerate(rollen):
            P = pts[r * m:(r + 1) * m]
            if ro in ('gelenk0', 'gelenk1'):
                c = P.mean(0)
                q = P - c
                q -= (q @ d)[:, None] * d
                nrm[r * m:(r + 1) * m] = q / np.linalg.norm(q, axis=1, keepdims=True)
            elif ro in ('kugel0', 'kugel1'):
                # Kappe: radial wie der Gelenkring (nicht Kugelnormale). Sie steckt im Nachbarn und zeigt sich nur aussen an der Beuge; dort setzt sie die
                # Roehre fort. Mit Kugelnormalen las sie, wenn der Maler sie vor die Roehre sortierte, als dunkler Zahn (Lauf _probe/C3e9).
                c = P.mean(0)
                q = P - c
                q -= (q @ d)[:, None] * d
                nrm[r * m:(r + 1) * m] = q / np.linalg.norm(q, axis=1, keepdims=True)
        if k_ > 0:
            nrm[G['a0']] = -d
        if k_ < len(gelenke):
            nrm[G['a1']] = d
        Gs[i] = G
        info[nm] = dict(rollen=rollen, s=(float(s_a), float(s_b)), r=(float(radius(s_a).mean()), float(radius(s_b).mean())), x0=float(x0), achse=[float(v) for v in yz_achse], knochen=int(p.knochen))
    # Vordrehen der Gelenknormalen: halbe mittlere Gelenkdrehung (Ruhe, Pose 1)
    for j in range(len(gelenke)):
        a, b = SCHWANZ[j], SCHWANZ[j + 1]
        rel = clips_rel[j]
        mitt = Slerp([0, 1], Rotation.concatenate([rel[0], rel[1]]))(0.5)
        halb = Slerp([0, 1], Rotation.concatenate([Rotation.identity(), mitt]))(0.5)
        for nm, rot, ziel in ((a, halb, 'gelenk1'), (b, halb.inv(), 'gelenk0')):
            i = idx[nm]
            G = Gs[i]
            m = G['m']
            ro = info[nm]['rollen']
            rj = ro.index(ziel)
            nachbar = rj - 1 if ziel == 'gelenk1' else rj + 1
            for r, anteil in ((rj, 1.0), (nachbar, 0.5)):
                if 0 <= r < len(ro) and ro[r] in ('k', 'gelenk0', 'gelenk1'):
                    rr = Slerp([0, 1], Rotation.concatenate([Rotation.identity(), rot]))(anteil)
                    n_ = G['nrm'][r * m:(r + 1) * m]
                    G['nrm'][r * m:(r + 1) * m] = rr.apply(n_)
    return rk, Gs, info


# ═══════════════════════════════════════════════════ weniger Flaechen
WENIG_ECKEN_ART = dict(rumpf=12, schwanz=10, glied=8, hand=8, finger=4, kegel=4)
WENIG_ECKEN_NAME = dict(kopf=8, kiefer=6, hals=6)
WENIG_RINGABSTAND, WENIG_KUGEL = 300.0, (45.0,)
WENIG_SCHRITT = 9999.0
WENIG_SCHRITT_ART = dict(rumpf=600.0)      # Zwischenringe nur am Rumpf und nur ueber Luecken ab 600 (voll: 420 ueberall)


_QUELLE = {}


def schwanz_quelle(g, teile, stand):
    """Profil des Schwanzes aus dem vollen Endstand von form_bemalt (Glaetter mit den vollen Ecken, Nachpassung zw/delta.json), unabhaengig von der Variante."""
    gespeichert = fb_bau._delta
    try:
        fb_bau.delta_laden(None)
        L, C = fb_bau.wirksam(teile, stand['lr'], stand['c'])
        rk = [list(fb_form.klaue_links(p, *fb_form.oberarm(p, *fb_form.schwanz_rund(p, *fb_form.a1_wurzel(p, *fb_bau.teil_ecken(p, L[i], C[i])))))) for i, p in enumerate(teile)]
        rk = fb_form.schwanz_profil(teile, rk, g)
        rk = fb_form.ketten(teile, rk, g)
        Gs = [fb_glatt.glatt_teil(p, *rk[i], verform=fb_form.schulter, delta=fb_bau.delta_fuer(p)) if p.name.startswith('schwanz') else None for i, p in enumerate(teile)]
        i1 = [i for i, p in enumerate(teile) if p.name == 'schwanz1'][0]
        p1, G1 = teile[i1], Gs[i1]
        wurzel = [G1['ring_pts'][r].copy() for r in range(G1['NR'])
                  if G1['rolle'][r] == 'k' and float((G1['ring_pts'][r].mean(0) - p1.o) @ p1.d) < -60]
        _QUELLE['wurzel'] = wurzel
        return profil(teile, Gs, g)
    finally:
        fb_bau._delta = gespeichert


def baue_form(g, teile, stand, schwanz='e9', flaechen='voll', rel_clips=(2, 0), beine=False):
    """Wie fb_bau.baue_form; schwanz='e9' ersetzt die Schwanzglieder (schwanz_e9), flaechen='wenig' setzt die Ecken und Ringe des Glaetters herab."""
    if schwanz == 'e9':
        _QUELLE['profil'] = schwanz_quelle(g, teile, stand)
    alt = (dict(fb_glatt.ECKEN_ART), dict(fb_glatt.ECKEN_NAME), dict(fb_glatt.SCHRITT))
    global M, RINGABSTAND, KUGEL
    m_alt, ra_alt, ku_alt = M, RINGABSTAND, KUGEL
    try:
        if flaechen == 'wenig':
            fb_glatt.ECKEN_ART.update(WENIG_ECKEN_ART)
            fb_glatt.ECKEN_NAME.update(WENIG_ECKEN_NAME)
            for k in fb_glatt.SCHRITT:
                fb_glatt.SCHRITT[k] = WENIG_SCHRITT_ART.get(k, WENIG_SCHRITT) if k != 'finger' and k != 'kegel' else fb_glatt.SCHRITT[k]
            M = WENIG_ECKEN_ART['schwanz']
            RINGABSTAND, KUGEL = WENIG_RINGABSTAND, WENIG_KUGEL
        return _baue(g, teile, stand, schwanz, flaechen, rel_clips, voll=alt, beine=beine)
    finally:
        fb_glatt.ECKEN_ART.clear(); fb_glatt.ECKEN_ART.update(alt[0])
        fb_glatt.ECKEN_NAME.clear(); fb_glatt.ECKEN_NAME.update(alt[1])
        fb_glatt.SCHRITT.clear(); fb_glatt.SCHRITT.update(alt[2])
        M, RINGABSTAND, KUGEL = m_alt, ra_alt, ku_alt


def inkreis_verform(voll):
    """G: jeder Ring bekommt den Innenkreis, den er mit den vollen Ecken hatte (Kantenmitte eines m-Ecks liegt bei cos(pi/m) r). So liegt der Umriss
    von G nie innerhalb dessen des vollen Modells (das die Pruefung besteht); die Ecken ragen dafuer etwas weiter hinaus. Kein neuer Freiheitsgrad."""
    ea_, en_ = voll[0], voll[1]

    def f(name, pts, m, NR):
        pts = fb_form.schulter(name, pts, m, NR)
        a = fb_glatt.art(name)
        m_voll = en_.get(name, ea_.get(a, m))
        if m_voll == m:
            return pts
        k = math.cos(math.pi / m_voll) / math.cos(math.pi / m)
        pts = pts.copy()
        R = pts[:NR * m].reshape(NR, m, 3)
        c = R.mean(1, keepdims=True)
        pts[:NR * m] = (c + (R - c) * k).reshape(-1, 3)
        return pts
    return f


def _baue(g, teile, stand, schwanz, flaechen, rel_clips, voll=None, beine=False):
    form = FE.neu()
    for k in g['knochen']:
        FE.knochen_dazu(form, k['name'], k['eltern'], k['versatz'])
    L, C = fb_bau.wirksam(teile, stand['lr'], stand['c'])
    lage, dichte = fb_bau.atlas(teile, L, C)
    for p in teile:
        if isinstance(p.bind, int):
            fb_bau._namen_meister[p.name] = teile[p.bind].name
    gi = {}
    rk = [list(fb_form.klaue_links(p, *fb_form.oberarm(p, *fb_form.schwanz_rund(p, *fb_form.a1_wurzel(p, *fb_bau.teil_ecken(p, L[i], C[i])))))) for i, p in enumerate(teile)]
    rk = fb_form.schwanz_profil(teile, rk, g)
    rk = fb_form.ketten(teile, rk, g)
    vf = fb_form.schulter
    ersetzt = set()
    ruhe_neu = None
    if beine:
        # E16: Unterschenkel, Fuss, Zehen neu (vm_beine), Winkel Pose 1 aus der Anpassung, Pose 2 und Ruhe Fuss flach
        import copy
        import vm_beine
        ersetzt = vm_beine.ringe_ersetzen(teile, rk)
        stand = copy.deepcopy(stand)
        ruhe_neu = copy.deepcopy(fb_bau.ruhe_winkel(g, stand))

        def probe(w):
            pr = FE.neu()
            for k in g['knochen']:
                FE.knochen_dazu(pr, k['name'], k['eltern'], k['versatz'])
            FE.clip_dazu(pr, 'p', [FE.bild(pr, wurzel=[0, 0, 0], winkel=np.round(np.array(w, float)).astype(int).tolist())])
            return FE.pose_aus_bild(pr, pr['clips'][0]['bilder'][0])
        alt_c = FE.lese_emd(open(os.path.join(HIER_V, 'C_mitte', 'FORM.EMD'), 'rb').read())
        wz_c = alt_c['pool'][alt_c['clips'][1]['bilder'][0]['kf']]['wurzel']
        stand, ruhe_neu = vm_beine.winkel_setzen(g, stand, ruhe_neu, probe, wurzel_y=wz_c)
    Gs = [fb_glatt.glatt_teil(p, *rk[i], verform=vf, delta=None if i in ersetzt else fb_bau.delta_fuer(p)) for i, p in enumerate(teile)]
    if schwanz == 'e9':
        # Gelenkdrehungen der Clips aus dem bisherigen Stand (Posen kommen aus stand, nicht aus der Geometrie)
        probe = FE.neu()
        for k in g['knochen']:
            FE.knochen_dazu(probe, k['name'], k['eltern'], k['versatz'])
        for n in (1, 2):
            p = stand['posen'][str(n)]
            FE.clip_dazu(probe, 'p%d' % n, [FE.bild(probe, wurzel=[0, 0, 0], winkel=np.round(np.array(p['winkel'], float)).astype(int).tolist())])
        r = fb_bau.ruhe_winkel(g, stand)
        FE.clip_dazu(probe, 'ruhe', [FE.bild(probe, wurzel=[0, 0, 0], winkel=np.round(np.array(r['winkel'], float)).astype(int).tolist())])
        idx = {p.name: p.knochen for p in teile}
        rel = []
        for j in range(len(SCHWANZ) - 1):
            a, b = idx[SCHWANZ[j]], idx[SCHWANZ[j + 1]]
            rr = []
            for c in rel_clips:
                R, _T = FE.pose_aus_bild(probe, probe['clips'][c]['bilder'][0])
                Ra, Rb = R[a].astype(float) / 4096, R[b].astype(float) / 4096
                rr.append(Rotation.from_matrix(_orth(Ra.T @ Rb)))
            rel.append(rr)
        rk, Gs, info = schwanz_e9(teile, rk, Gs, g, rel, _QUELLE['profil'])
        form['_e9'] = info
    else:
        fb_form.normalen_ketten(teile, Gs, g)
    for i, p in enumerate(teile):
        fb_bau.teil_mesh(form, p, L[i], C[i], lage[i], gi, rk[i], G=Gs[i])
    form['_glatt'] = gi
    print('orientiere: gedreht', FE.orientiere(form))
    for n, nm in ((1, 'pose1'), (2, 'pose2')):
        p = stand['posen'][str(n)]
        w = np.round(np.array(p['winkel'], float)).astype(int)
        wz = list(p['wurzel'])
        wz[1] += p['lage'][1]
        FE.clip_dazu(form, nm, [FE.bild(form, wurzel=[int(round(x)) for x in wz], winkel=w.tolist())])
    r = ruhe_neu if ruhe_neu is not None else fb_bau.ruhe_winkel(g, stand)
    FE.clip_dazu(form, 'ruhe', [FE.bild(form, wurzel=[int(round(x)) for x in r['wurzel']], winkel=np.round(np.array(r['winkel'], float)).astype(int).tolist())])
    FE.boden_nachfuehren(form)
    if beine:
        # Pose 1 (Bild 1): Sohlen angehoben zur Kamera, der tiefste Punkt ist nicht mehr der Fuss. Wurzelhoehe wie im bisherigen Modell (C, gleiche
        # Geometrie ausser den Beinen), sonst verrutscht die ganze Figur (Lauf I_fuesse/_g v1: IoU 0,743, Oberkante Schwanz -24 px).
        alt_ = FE.lese_emd(open(os.path.join(HIER_V, 'C_mitte', 'FORM.EMD'), 'rb').read())
        for c in (0, 1):
            # Pose 2 ebenso: die flach gestellten Fuesse aenderten den tiefsten Punkt, die Figur rutschte in Bild 2 um 6-7 px (Lauf I_fuesse/_g v3)
            kf = alt_['clips'][c]['bilder'][0]['kf']
            form['clips'][c]['bilder'][0]['wurzel'][1] = int(alt_['pool'][kf]['wurzel'][1])
    form['_stand'] = stand
    return form, lage, dichte, L, C


def _orth(m):
    u, _s, vt = np.linalg.svd(m)
    r = u @ vt
    if np.linalg.det(r) < 0:
        u[:, -1] *= -1
        r = u @ vt
    return r
