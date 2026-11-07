"""Leser B V3: Feuerstellen (Bild 1 Kopf vorn / Bild 2 Maul) und schwarz-rote Koerperflaechen. Messung an Original-Standbildern, Rueckrechnung form_video."""
from gem import *
from scipy import ndimage as ndi
NT = FORM + '/nutzer_teile'
R = {}
# Fehler (farbe.md Abschnitt 3/4): Balance Bild 1 +-8 % (geschaetzt), Bild 2 +-(0.02,0.03,0.04) auf (0.951,0.665,0.620).
BAL = {1: np.array([0.08, 0.08, 0.08]), 2: np.array([0.02 / 0.951, 0.03 / 0.665, 0.04 / 0.620])}
LEON = dict(gesicht=np.array([0.339, 0.338, 0.349]), hellste=np.array([0.702, 0.633, 0.493]))  # farbe_palette.json licht.bild1
FIG = {n: np.load(S + 'fid%d.npz' % n) for n in (1, 2)}


def stat(n, m, V, e, T):
    nc = m & ~CLIP[n]
    o = dict(px=int(m.sum()), px_geclippt=int((m & CLIP[n]).sum()))
    if nc.sum() == 0:
        return o
    for nm, a in (('V', V), ('e', e), ('T', T)):
        x = a[nc].astype(float)
        L = lab(x)
        o[nm + '_rgb'] = [int(round(v)) for v in x.mean(0)]
        o[nm + '_rgb_std'] = [int(round(v)) for v in x.std(0)]
        o[nm + '_lab_der_mittelfarbe'] = lab1(x.mean(0))
        o[nm + '_lab_pixel_mittel'] = [round(float(v), 1) for v in L.mean(0)]
        o[nm + '_lab_pixel_std'] = [round(float(v), 1) for v in L.std(0)]
        if nm != 'V':
            C = np.hypot(L[:, 1], L[:, 2]); h = hue(L)
            o[nm + '_chroma_mittel'] = round(float(C.mean()), 1)
            hm = C > 8
            o[nm + '_farbton_median_grad'] = round(float(np.median(h[hm])), 1) if hm.sum() > 5 else None
    return o


def lage(m):
    ys, xs = np.nonzero(m)
    P = np.stack([xs, ys], 1).astype(float)
    c = P.mean(0)
    if len(P) < 3:
        return dict(schwerpunkt=[round(float(v), 1) for v in c])
    w, v = np.linalg.eigh(np.cov((P - c).T))
    a = v[:, 1]
    proj = (P - c) @ a; quer = (P - c) @ v[:, 0]
    ang = (np.degrees(np.arctan2(a[1], a[0])) + 180) % 180
    return dict(bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], schwerpunkt=[round(float(c[0]), 1), round(float(c[1]), 1)],
                laenge_px=round(float(np.percentile(proj, 99) - np.percentile(proj, 1)), 1),
                breite_px=round(float(np.percentile(quer, 99) - np.percentile(quer, 1)), 1),
                achse_grad_bild=round(float(ang), 1))


def koerper(n, m):
    """K5c-Knochen, Flaechen und Knochenraum-Punkte unter der Maske m (Pose/Kamera Bild n)."""
    f = FIG[n]; kn, fa, lok, welt = f['kn'], f['fa'], f['lok'], f['welt']
    s = m & (kn >= 0)
    o = dict(px_auf_K5c=int(s.sum()), anteil_auf_K5c=round(float(s.sum() / max(m.sum(), 1)), 3))
    if s.sum() == 0:
        return o
    hb = np.bincount(kn[s])
    o['knochen_px'] = {int(b): int(c) for b, c in enumerate(hb) if c}
    fl_ = {}
    for b in o['knochen_px']:
        q = s & (kn == b)
        u, c = np.unique(fa[q], return_counts=True)
        fl_[int(b)] = {int(a): int(cc) for a, cc in zip(u, c)}
        P = lok[q].astype(float)
        Wp = welt[q].astype(float)
        if len(Wp) > 3:
            cW = Wp.mean(0); w, v = np.linalg.eigh(np.cov((Wp - cW).T)); pr = (Wp - cW) @ v
            ausd = [int(round(np.percentile(pr[:, i], 99) - np.percentile(pr[:, i], 1))) for i in (2, 1, 0)]
        else:
            ausd = None
        o.setdefault('knochenraum', {})[int(b)] = dict(mittel=[int(round(x)) for x in P.mean(0)], min=[int(x) for x in P.min(0)],
                                                     max=[int(x) for x in P.max(0)], ausdehnung_3d_p1_p99=ausd)
    o['flaechen_je_knochen'] = fl_
    return o


V, E, Tt, CLIP, ELAB, FIGM = {}, {}, {}, {}, {}, {}
for n in (1, 2):
    V[n], E[n], Tt[n], CLIP[n] = lade(n)
    ELAB[n] = lab(E[n]).reshape(480, 640, 3)
    FIGM[n] = (ziel(n) >= 128) & ~Z.LOGO & ~Z.BODEN
for n in (1, 2):
    ys = np.nonzero(ziel(n) == 255)[0]
    R['H_bild%d_px' % n] = [int(ys.min()), int(ys.max()), int(ys.max() - ys.min() + 1)]

# ============ A: Bild 1, Stelle F1 =========
L1 = ELAB[1]
kand = (L1[..., 0] >= 12) & (L1[..., 1] >= 8) & FIGM[1]
lab_, k = ndi.label(kand)
F1 = lab_ == lab_[232, 325]
kern = F1 & (L1[..., 0] >= 20)
schwellen = {}
for ls, as_ in ((8, 6), (10, 8), (12, 8), (12, 10), (16, 10), (20, 10), (25, 10)):
    kk = (L1[..., 0] >= ls) & (L1[..., 1] >= as_) & FIGM[1]
    l2, _ = ndi.label(kk)
    sid = l2[232, 325]
    schwellen['L>=%d,a>=%d' % (ls, as_)] = int((l2 == sid).sum()) if sid else 0
A = dict(regel='e-Lab (form_video.video_umkehr Bild 1, Gauss 1,0): L* >= 12 und a* >= 8, Zusammenhang mit (325,232), in ziel2_bild1 >= 128',
         flaeche_je_schwelle=schwellen, lage=lage(F1), lage_kern=lage(kern),
         stat=stat(1, F1, V[1], E[1], Tt[1]), stat_kern=stat(1, kern, V[1], E[1], Tt[1]),
         koerper=koerper(1, F1))
eM = E[1][F1 & ~CLIP[1]].mean(0)
A['T_bei_licht'] = {nm: [int(round(x)) for x in eM / l] for nm, l in (('figur_mittel', LICHT[1]), ('leon_gesicht_ambient', LEON['gesicht']), ('leon_hellste', LEON['hellste']))}
A['T_bei_licht_lab'] = {k_: lab1(v) for k_, v in A['T_bei_licht'].items()}
sa = np.load(S + 'sauber1.npy').astype(float); al = np.load(S + 'alb1.npy').astype(float)
q = F1 & (al.min(2) > 20) & (FIG[1]['kn'] >= 0)
A['licht_K5c_lokal_median'] = [round(float(x), 3) for x in np.median(sa[q] / al[q], 0)]
kopf = np.zeros((480, 640), bool)
for b in (21, 22, 23):
    kopf |= np.load(S + 'allein1_b%d.npy' % b)
np.save(S + 'kopf1.npy', kopf)
dt = ndi.distance_transform_edt(~kopf)
sp = lage(F1)['schwerpunkt']
A['kopf_K5c_bild1'] = dict(projektion_ohne_verdeckung=lage(kopf), sichtbar_px={b: int((FIG[1]['kn'] == b).sum()) for b in (21, 22, 23)},
                           F1_ueberlappt_kopfprojektion_px=int((F1 & kopf).sum()),
                           abstand_F1_schwerpunkt_zur_kopfprojektion_px=round(float(dt[int(round(sp[1])), int(round(sp[0]))]), 1),
                           abstand_F1_naechster_px=round(float(dt[F1].min()), 1))
tz = FIG[1]['tz'][F1 & (FIG[1]['kn'] >= 0)]
A['tiefe_kamera_z_median'] = int(np.median(tz))
R['A_bild1_F1'] = A
lab_all, kk_ = ndi.label((L1[..., 0] >= 12) & (L1[..., 1] >= 8) & FIGM[1])
weitere = []
for i in range(1, kk_ + 1):
    m = lab_all == i
    if m.sum() < 25 or (m & F1).any():
        continue
    weitere.append(dict(lage=lage(m), px=int(m.sum()), e_lab=lab1(E[1][m].mean(0)), T_rgb=[int(round(x)) for x in Tt[1][m].mean(0)],
                        T_lab=lab1(Tt[1][m].mean(0)), knochen=koerper(1, m).get('knochen_px')))
R['A_bild1_weitere_rote_stellen'] = weitere
np.save(S + 'F1.npy', F1)

# ============ B: Bild 2, Maul =========
L2 = ELAB[2]
uk = cv2.imread(NT + '/maske_unterkiefer.png', 0) > 0
ok = cv2.imread(NT + '/maske_oberkiefer.png', 0) > 0
raf = cv2.imread(NT + '/maske_right_arm_front.png', 0) > 0
roi = np.zeros((480, 640), bool); roi[290:425, 140:350] = True
roi &= ~Z.LOGO
Ld, ad, bd = L2[..., 0], L2[..., 1], L2[..., 2]
kl = np.full((480, 640), -1, np.int8)
DUNKEL, BAND, ZAHN, SCHEIN, CLIPK, MITTE = 0, 1, 2, 3, 4, 5
kl[roi] = MITTE
kl[roi & (Ld < 8)] = DUNKEL
kl[roi & (Ld >= 8) & (ad >= 12) & ~(bd > ad + 3)] = BAND
kl[roi & (Ld >= 18) & (ad < 8) & (bd < 15)] = ZAHN
kl[roi & (Ld >= 12) & (bd >= 15) & (bd > ad + 3)] = SCHEIN
kl[roi & CLIP[2]] = CLIPK
np.save(S + 'kl2.npy', kl)
KN = ['dunkel', 'band_rot', 'hell_neutral', 'gelb_feuerschein', 'geclippt', 'mittel']
B = dict(regeln=dict(dunkel='L*e < 8', band_rot='L*e >= 8, a*e >= 12, nicht b*e > a*e+3', hell_neutral='L*e >= 18, a*e < 8, b*e < 15',
                     gelb_feuerschein='L*e >= 12, b*e >= 15, b*e > a*e+3', geclippt='Video min(R,G,B) >= 200', roi='x 140-349, y 290-424 ohne Logo'))
for nm, mk_ in (('unterkiefer', uk & roi), ('oberkiefer_im_roi', ok & roi & ~uk), ('right_arm_front_im_roi', raf & roi & ~uk & ~ok)):
    d = dict(px=int(mk_.sum()), klassen={KN[i]: round(float((kl[mk_] == i).mean()), 3) for i in range(6)})
    for i in range(6):
        q = mk_ & (kl == i)
        if q.sum() >= 10:
            d[KN[i]] = stat(2, q, V[2], E[2], Tt[2])
            d[KN[i]]['lage'] = lage(q)
            d[KN[i]]['koerper'] = koerper(2, q)
    B[nm] = d
hell = roi & np.isin(kl, [BAND, ZAHN, SCHEIN, CLIPK])
lb, nb = ndi.label(hell)
np.save(S + 'lb2.npy', lb)
fig_nutzer = (ziel(2) >= 128)
feuer_aussen = CLIP[2] & ~fig_nutzer
dtf = ndi.distance_transform_edt(~feuer_aussen)
fl_list = []
for i in range(1, nb + 1):
    m = lb == i
    if m.sum() < 20:
        continue
    d = dict(nr=i, lage=lage(m), px=int(m.sum()), klassen={KN[j]: round(float((kl[m] == j).mean()), 3) for j in range(6)},
             im_unterkiefer=round(float((m & uk).sum() / m.sum()), 3), im_nutzerumriss=round(float((m & fig_nutzer).sum() / m.sum()), 3),
             abstand_feuer_aussen_px=round(float(dtf[m].min()), 1), stat=stat(2, m, V[2], E[2], Tt[2]), koerper=koerper(2, m))
    fl_list.append(d)
fl_list.sort(key=lambda d: -d['px'])
B['flecken'] = fl_list
R['B_bild2_maul'] = B


# ============ C: Gegenprobe eine Texelfarbe fuer beide Bilder =========
def e_mittel(n, m):
    return E[n][m & ~CLIP[n]].mean(0)


band = uk & (kl == BAND)
zahn = uk & (kl == ZAHN)
np.save(S + 'band2.npy', band); np.save(S + 'zahn2.npy', zahn)
e1 = e_mittel(1, F1); e2 = e_mittel(2, band)
# Sigma je Kanal fuer einen Flaechen-Mittelwert: Balance (relativ) und Bias-Breite der Rueckrechnung (farbe.md 4: Bild 1 +3..+5, Bild 2 -1..-6)
sig = {1: np.hypot(BAL[1] * e1, 5.0), 2: np.hypot(BAL[2] * e2, 4.0)}
Lk = {1: LICHT[1], 2: LICHT[2]}
T1 = e1 / Lk[1]; T2 = e2 / Lk[2]
w1 = (Lk[1] / sig[1]) ** 2; w2 = (Lk[2] / sig[2]) ** 2
Tj = (Lk[1] * e1 / sig[1] ** 2 + Lk[2] * e2 / sig[2] ** 2) / (w1 + w2)


def rest(Tc):
    Tc = np.asarray(Tc, float)
    r1 = (Tc * Lk[1] - e1) / sig[1]; r2 = (Tc * Lk[2] - e2) / sig[2]
    return dict(Tc=[int(round(x)) for x in Tc], Tc_lab=lab1(Tc), e1_vorher=[round(float(x), 1) for x in Tc * Lk[1]], e1_gemessen=[round(float(x), 1) for x in e1],
                r1_sigma=[round(float(x), 2) for x in r1], e2_vorher=[round(float(x), 1) for x in Tc * Lk[2]], e2_gemessen=[round(float(x), 1) for x in e2],
                r2_sigma=[round(float(x), 2) for x in r2], chi2_6=round(float((r1 ** 2).sum() + (r2 ** 2).sum()), 2),
                dE_e1=round(float(np.linalg.norm(np.array(lab1(Tc * Lk[1])) - np.array(lab1(e1)))), 1),
                dE_e2=round(float(np.linalg.norm(np.array(lab1(Tc * Lk[2])) - np.array(lab1(e2)))), 1))


C = dict(sigma_e1=[round(float(x), 1) for x in sig[1]], sigma_e2=[round(float(x), 1) for x in sig[2]],
         T_aus_bild1=rest(T1), T_aus_bild2=rest(T2), T_gemeinsam=rest(Tj))
for nm, Tc in (('gelb_255_200_60', [255, 200, 60]), ('orange_255_140_40', [255, 140, 40]), ('rot_220_60_30', [220, 60, 30]), ('lachs_235_120_100', [235, 120, 100])):
    C['probe_' + nm] = rest(Tc)


def video_probe(n, m, Tc):
    eb = E[n].copy()
    ref = FV.video_filter(np.clip(eb, 0, 255).astype(np.uint8), bild=n, entzerren=False, rauschen=False)
    if Tc is not None:
        eb[m] = np.asarray(Tc, float) * Lk[n]
    v = FV.video_filter(np.clip(eb, 0, 255).astype(np.uint8), bild=n, entzerren=False, rauschen=False)
    return [int(round(x)) for x in v[m].mean(0)], [int(round(x)) for x in ref[m].mean(0)], [int(round(x)) for x in V[n][m].astype(float).mean(0)]


def dE(a, b):
    return round(float(np.linalg.norm(np.array(lab1(a)) - np.array(lab1(b)))), 1)


VP = {}
for nm, Tc in (('T_aus_bild1', T1), ('T_aus_bild2', T2), ('T_gemeinsam', Tj), ('orange_255_140_40', [255, 140, 40]), ('gelb_255_200_60', [255, 200, 60]),
               ('rot_220_60_30', [220, 60, 30])):
    p1, r1, o1 = video_probe(1, F1 & ~CLIP[1], Tc)
    p2, r2, o2 = video_probe(2, band & ~CLIP[2], Tc)
    VP[nm] = dict(bild1_video_vorher=p1, bild1_video_original=o1, bild1_dE=dE(p1, o1), bild2_video_vorher=p2, bild2_video_original=o2, bild2_dE=dE(p2, o2))
p1, r1, o1 = video_probe(1, F1 & ~CLIP[1], None); p2, r2, o2 = video_probe(2, band & ~CLIP[2], None)
VP['bezug_hin_und_zurueck'] = dict(bild1=r1, bild1_original=o1, bild1_dE=dE(r1, o1), bild2=r2, bild2_original=o2, bild2_dE=dE(r2, o2))
C['video_raum'] = VP
C['zahn_T'] = [int(round(x)) for x in e_mittel(2, zahn) / Lk[2]]
C['zahn_T_lab'] = lab1(e_mittel(2, zahn) / Lk[2])
R['C_gegenprobe'] = C

# ============ D: schwarz-rote Koerperflaechen =========
D = {}
for n in (1, 2):
    T = Tt[n]; LT = lab(T).reshape(480, 640, 3)
    out = []
    alle = np.zeros((480, 640), bool)
    for nm, m in Z.regionen(n):
        m = m & FIGM[n]
        alle |= m
        out.append((nm, m))
    out.append(('Figur (Nutzer-Teile vereinigt)', alle))
    rr = []
    for nm, m in out:
        q = m & ~CLIP[n]
        L = LT[q]
        C_ = np.hypot(L[:, 1], L[:, 2]); h = hue(L)
        dunkel = L[:, 0] < 20
        rot = (L[:, 1] >= 15) & (L[:, 0] >= 20) & (h >= -30) & (h <= 70)
        dunkelrot = (L[:, 0] < 20) & (L[:, 1] >= 8)
        hell = (L[:, 0] >= 45) & (L[:, 1] < 10)
        d = dict(zone=nm, px=int(m.sum()), geclippt=round(float((m & CLIP[n]).sum() / max(m.sum(), 1)), 3),
                 anteil_dunkel_Llt20=round(float(dunkel.mean()), 3), anteil_dunkelrot_Llt20_a_ge8=round(float(dunkelrot.mean()), 3),
                 anteil_rot=round(float(rot.mean()), 3), anteil_hell_neutral=round(float(hell.mean()), 3),
                 T_lab_mittel=[round(float(x), 1) for x in L.mean(0)],
                 T_dunkel_rgb=[int(round(x)) for x in T[q][dunkel].mean(0)] if dunkel.sum() > 10 else None,
                 T_dunkel_lab=[round(float(x), 1) for x in L[dunkel].mean(0)] if dunkel.sum() > 10 else None,
                 T_rot_rgb=[int(round(x)) for x in T[q][rot].mean(0)] if rot.sum() > 10 else None,
                 T_rot_lab=[round(float(x), 1) for x in L[rot].mean(0)] if rot.sum() > 10 else None,
                 T_rot_ton_median=round(float(np.median(h[rot])), 1) if rot.sum() > 10 else None,
                 L_p5_p50_p95=[round(float(np.percentile(L[:, 0], p)), 1) for p in (5, 50, 95)],
                 a_p50_p95=[round(float(np.percentile(L[:, 1], p)), 1) for p in (50, 95)],
                 chroma_mittel=round(float(C_.mean()), 1),
                 ton_median_C_gt8=round(float(np.median(h[C_ > 8])), 1) if (C_ > 8).sum() > 10 else None,
                 koerper_K5c=koerper(n, m).get('knochen_px'))
        rr.append(d)
    D['bild%d' % n] = rr
R['D_schwarzrot'] = D
json.dump(R, open(S + 'messung.json', 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else int(o))
print('ok')
