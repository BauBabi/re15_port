"""E3: Referenzpaar Raumhintergrund Bild 2 (ROOM5090 Schnitt 11, Port-BSS ueber B in Video-Bildlage) gegen das Dia-Mittel.
Prueft die Lage (ECC-Verschiebung), fittet das Farbmodell em.py robust (soft_l1), Gegenprobe raeumlich (links fitten, rechts testen)
und gegen das bestehende form_video.video_filter. Ausgang daten/e3_hintergrund.json, daten/e3_modell_bild2.json, daten/e3_masken.npz."""
import json, numpy as np, cv2
from scipy.optimize import least_squares
import ek, em
FV = ek.FV
V = ek.video(2)
G = ek.lade_rgb(ek.FORM + '/pruefung/hintergrund_bild2.png').astype(np.float32)
# Port-Bereich (ohne Randwiederholung des Warps)
roh = ek.lade_rgb(ek.WT + '/re15_port/build/form_bg/ROOM50911.ppm')
roh = cv2.resize(roh, (320, 240), interpolation=cv2.INTER_NEAREST) if roh.shape[:2] != (240, 320) else roh
B = np.array(ek.FK.lade_kamera(2)['B'])
innen = cv2.warpPerspective(np.ones((240, 320), np.uint8), B, (640, 480), flags=cv2.INTER_NEAREST, borderValue=0) > 0
G_neu = cv2.warpPerspective(roh, B, (640, 480), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REPLICATE).astype(np.float32)
gleich = float(np.abs(G_neu - G)[innen].mean())
aus = dict(quelle='ROOM50911.ppm (probe_bg_dump, Engine-Dekoder) ueber kamera_bild2.json B; Vergleich mit pruefung/hintergrund_bild2.png MAE %.3f' % gleich)
ausschluss = np.zeros((480, 640), bool)
for y0, y1, x0, x1 in FV._MASKE2:
    ausschluss[y0:y1, x0:x1] = True
fig = cv2.imread(ek.FORM + '/pruefung/figur_bild2.png', 0) > 0
logo = cv2.imread(ek.FORM + '/pruefung/logo_maske.png', 0) > 0
ausschluss |= cv2.dilate(fig.astype(np.uint8), np.ones((31, 31), np.uint8)) > 0
ausschluss |= cv2.dilate(logo.astype(np.uint8), np.ones((11, 11), np.uint8)) > 0
rand = np.zeros_like(ausschluss); rand[6:-6, 6:-6] = True
innen_e = cv2.erode(innen.astype(np.uint8), np.ones((13, 13), np.uint8)) > 0
clip = V.max(2) >= 220
M = ~ausschluss & rand & innen_e & ~clip
aus['maske'] = dict(pixel=int(M.sum()), ausgeschlossen_geclippt=int((~ausschluss & rand & innen_e & clip).sum()),
                    regel='ohne form_video._MASKE2 (Figur, Frau, Feuer, Logo), figur_bild2 +15 px, Logo +5 px, Rand 6 px, Port-Rand 6 px, V >= 220 in einem Kanal')
# Lage pruefen: ECC-Verschiebung zwischen gefiltertem Port und Video (Graustufen, nur Maske)
f0 = FV.video_filter(G.astype(np.uint8), bild=2, entzerren=False, rauschen=False).astype(np.float32)
g1 = cv2.cvtColor(f0, cv2.COLOR_RGB2GRAY); g2 = cv2.cvtColor(V, cv2.COLOR_RGB2GRAY)
war = np.eye(2, 3, dtype=np.float32)
try:
    cc, war = cv2.findTransformECC(g2, g1, war, cv2.MOTION_TRANSLATION, (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 200, 1e-5),
                                   M.astype(np.uint8), 5)
    aus['lage_ecc'] = dict(ecc=round(float(cc), 4), dx=round(float(war[0, 2]), 2), dy=round(float(war[1, 2]), 2))
except cv2.error as ex:
    aus['lage_ecc'] = str(ex)
# Fuellung fuer Streulicht: unbekannte Flaechen aus form_video.video_umkehr (nur fuer das Streulicht, nie Messwert)
fuell = FV.video_umkehr(np.clip(V + .5, 0, 255).astype(np.uint8), 2).astype(np.float32)
E = np.where((ausschluss | ~innen)[..., None], fuell, G)
X = em.optik(em.licht(E))
Xb = cv2.GaussianBlur(em.licht(G), (0, 0), FV.SIG_OPTIK)


def fit(maske, saat=1, n=30000, th0=None, chroma_stufe=True, art='ridge'):
    """Stufe 1 ohne Chroma nur an den Stichproben (schnell), Stufe 2 mit Chroma am ganzen Bild (wenige Schritte)."""
    ys, xs = np.nonzero(maske)
    r = np.random.default_rng(saat).choice(len(ys), min(n, len(ys)), replace=False)
    ys, xs = ys[r], xs[r]
    ziel = V[ys, xs]
    Xs = X[ys, xs]
    cg = .6 if art != 'diag' else 1e-6
    lo = [1, 1, 1, .15, .15, .15, -400, -400, -400] + [-cg] * 6
    hi = [8000, 8000, 8000, 1.5, 1.5, 1.5, 100, 100, 100] + [cg] * 6
    th0 = em.TH0 if th0 is None else th0
    # Ridge: 0,1 Uebersprechen kostet so viel wie 4 V Restfehler an 10 % der Stichproben (soft_l1 linear im Fernbereich)
    lam = 0.0 if art != 'ridge' else 4.0 * np.sqrt(0.1 * len(ys) * 3) / 0.1
    def res1(th, Xs=Xs, ziel=ziel):
        r = (em.kurve(Xs, th, chroma=False) - ziel).ravel()
        return np.r_[r, lam * np.asarray(th[9:15])] if lam else r
    r1 = least_squares(res1, th0, loss='soft_l1', f_scale=6.0, bounds=(lo, hi), x_scale='jac', max_nfev=600)
    if not chroma_stufe:
        return r1.x
    k = np.random.default_rng(saat + 1).choice(len(ys), min(6000, len(ys)), replace=False)
    y2, x2, z2 = ys[k], xs[k], ziel[k]
    lam2 = lam * np.sqrt(len(k) / len(ys))
    def res2(th):
        r = (em.kurve(X, th)[y2, x2] - z2).ravel()
        return np.r_[r, lam2 * np.asarray(th[9:15])] if lam2 else r
    r2 = least_squares(res2, r1.x, loss='soft_l1', f_scale=6.0, bounds=(lo, hi), x_scale='jac', max_nfev=40)
    return r2.x


def mass(P, maske):
    d = ek.de76(P[maske], V[maske])
    a = np.abs(P[maske] - V[maske])
    return dict(pixel=int(maske.sum()), dE76_median=round(float(np.median(d)), 2), dE76_p90=round(float(np.percentile(d, 90)), 2),
                MAE_V=round(float(a.mean()), 2), MAE_je_kanal=[round(float(x), 2) for x in a.mean(0)])


links = M.copy(); links[:, 320:] = False
rechts = M.copy(); rechts[:, :320] = False
eG = 255 * np.power(cv2.GaussianBlur(em.licht(G), (0, 0), FV.SIG_OPTIK), 1 / 2.2)


def rueck(th):
    eR = em.umkehr(V, th)
    d = eR[M] - eG[M]
    lum = eG[M].mean(1)
    baender = {}
    for lo_, hi_ in ((0, 15), (15, 30), (30, 50), (50, 80), (80, 130)):
        s_ = (lum >= lo_) & (lum < hi_)
        if s_.sum() > 50:
            baender['%d-%d' % (lo_, hi_)] = dict(n=int(s_.sum()), bias=[round(float(x), 1) for x in np.median(d[s_], 0)],
                                                 p10=[round(float(x), 1) for x in np.percentile(d[s_], 10, 0)],
                                                 p90=[round(float(x), 1) for x in np.percentile(d[s_], 90, 0)],
                                                 dE76_median=round(float(np.median(ek.de76(eR[M][s_], eG[M][s_]))), 2))
    return baender, eR


aus['varianten'] = {}
TH = {}
for art in ('diag', 'ridge', 'frei'):
    th = fit(M, art=art)
    TH[art] = th
    thl = fit(links, art=art); thr = fit(rechts, art=art)
    b, _ = rueck(th)
    aus['varianten'][art] = dict(th=[round(float(x), 5) for x in th], mass=mass(em.kurve(X, th), M),
                                 links_fit_rechts_test=mass(em.kurve(X, thl), rechts), rechts_fit_links_test=mass(em.kurve(X, thr), links),
                                 cond_C=round(float(np.linalg.cond(em.teile(th)[3])), 3), rueckrechnung_e=b)
    print(art, json.dumps(aus['varianten'][art]['mass']), aus['varianten'][art]['links_fit_rechts_test']['dE76_median'],
          aus['varianten'][art]['rechts_fit_links_test']['dE76_median'], flush=True)
th = TH['ridge']
P = em.kurve(X, th)
_, eR = rueck(th)
# Vergleich: bestehendes video_filter (ohne Rauschen) auf derselben Fuellung
FVb = FV.video_filter(np.clip(E + .5, 0, 255).astype(np.uint8), bild=2, entzerren=False, rauschen=False).astype(np.float32)
aus['bestand_video_filter'] = mass(FVb, M)
aus['roh_ohne_modell'] = mass(G, M)
FVr, _ = None, None
json.dump(aus, open(ek.DATEN + '/e3_hintergrund.json', 'w'), indent=1)
json.dump(dict(bild=2, th=[float(x) for x in th], alle={k: [float(x) for x in v] for k, v in TH.items()}, hinweis='em.py teile(): a,p,b,C-offdiag; k=1; th = Variante ridge'), open(ek.DATEN + '/e3_modell_bild2.json', 'w'), indent=1)
np.savez_compressed(ek.SCR + '/e3_b2.npz', M=M, P=P.astype(np.float32), FVb=FVb, E=E, eR=eR.astype(np.float32))
print(json.dumps(aus['bestand_video_filter']))
