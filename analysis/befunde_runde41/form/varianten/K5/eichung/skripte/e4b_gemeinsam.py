"""E4b: Szene 2 gemeinsam fitten (Hintergrund + Frau), weil der Hintergrund allein die Frau nicht vorhersagt (e4_frau: dE76 40-53).
Der Hintergrund von 5090 Schnitt 11 ist fast nur Orange/Braun; der weisse Kittel (e = 64 neutral) liegt ausserhalb seines Farbraums.
Gueltigkeit je zurueckgehaltener Referenz:
  F_kittel / F_dunkel / F_kopf : Hintergrund + Frau ohne diese Klasse fitten, diese Klasse testen
  F_bg_rechts / F_bg_links     : Hintergrund links (rechts) + Frau fitten, andere Bildhaelfte testen
  F_alle                       : alles fitten (Modell fuer die Umkehr)
Varianten: diag (C = I, 9 Parameter) und frei (C frei, 15 Parameter). Vergleich: form_video.video_filter (39 Dias, hier nicht gefittet).
Ausgang daten/e4b_gemeinsam.json, daten/e4b_modell_bild2.json (gewaehlte Variante)."""
import json
import numpy as np
import cv2
from scipy.optimize import least_squares
import ek
import em

FV = ek.FV
V = ek.video(2)
z3 = np.load(ek.SCR + '/e3_b2.npz')
z4 = np.load(ek.SCR + '/e4_frau.npz')
M = z3['M']
comp = z4['comp'].astype(np.float32)
KL = {'Kittel': z4['k0'], 'dunkel': z4['k1'], 'Kopf': z4['k2']}
X = em.optik(em.licht(comp))
links = M.copy(); links[:, 320:] = False
rechts = M.copy(); rechts[:, :320] = False
eG = 255 * np.power(cv2.GaussianBlur(em.licht(comp), (0, 0), FV.SIG_OPTIK), 1 / 2.2)


def stich(maske, n, saat):
    ys, xs = np.nonzero(maske)
    if len(ys) > n:
        r = np.random.default_rng(saat).choice(len(ys), n, replace=False)
        ys, xs = ys[r], xs[r]
    return ys, xs


def fit(bg, frau, art, saat=3):
    yb, xb = stich(bg, 16000, saat)
    yf, xf = stich(frau, 4000, saat + 1)
    wf = 3.0                                   # Frau rund 30-40 % des Gewichts
    ys = np.r_[yb, yf]; xs = np.r_[xb, xf]
    w = np.r_[np.ones(len(yb)), np.full(len(yf), wf)][:, None]
    ziel = V[ys, xs]
    Xs = X[ys, xs]
    cg = .6 if art == 'frei' else 1e-6
    lo = [1, 1, 1, .15, .15, .15, -400, -400, -400] + [-cg] * 6
    hi = [8000, 8000, 8000, 1.5, 1.5, 1.5, 100, 100, 100] + [cg] * 6
    r1 = least_squares(lambda th: ((em.kurve(Xs, th, chroma=False) - ziel) * w).ravel(), em.TH0, loss='soft_l1', f_scale=6.0,
                       bounds=(lo, hi), x_scale='jac', max_nfev=400)
    k = np.random.default_rng(saat + 2).choice(len(ys), min(5000, len(ys)), replace=False)
    r2 = least_squares(lambda th: ((em.kurve(X, th)[ys[k], xs[k]] - ziel[k]) * w[k]).ravel(), r1.x, loss='soft_l1', f_scale=6.0,
                       bounds=(lo, hi), x_scale='jac', max_nfev=25)
    return r2.x


def mass(P, q):
    if q.sum() == 0:
        return {}
    d = ek.de76(P[q], V[q])
    mv, mp = np.median(V[q], 0), np.median(P[q], 0)
    return dict(pixel=int(q.sum()), dE76_median=round(float(np.median(d)), 2), dE76_p90=round(float(np.percentile(d, 90)), 2),
                dE76_der_mediane=round(float(ek.de76(mp, mv)), 2), video_median=[round(float(x), 1) for x in mv],
                modell_median=[round(float(x), 1) for x in mp])


def rueck(th):
    eR = em.umkehr(V, th)
    out = {}
    for nm, q in (('Hintergrund', M),) + tuple(KL.items()):
        d = eR[q] - eG[q]
        lum = eG[q].mean(1)
        for lo_, hi_ in ((0, 15), (15, 30), (30, 50), (50, 80)):
            s = (lum >= lo_) & (lum < hi_)
            if s.sum() > 30:
                out['%s %d-%d' % (nm, lo_, hi_)] = dict(n=int(s.sum()), bias=[round(float(x), 1) for x in np.median(d[s], 0)],
                                                       p10=[round(float(x), 1) for x in np.percentile(d[s], 10, 0)],
                                                       p90=[round(float(x), 1) for x in np.percentile(d[s], 90, 0)])
    return out


frau_alle = np.any(list(KL.values()), 0)
aus = dict(bestand={}, varianten={})
FVb = FV.video_filter(np.clip(comp + .5, 0, 255).astype(np.uint8), bild=2, entzerren=False, rauschen=False).astype(np.float32)
for nm, q in (('Hintergrund', M), ('BG rechts', rechts), ('BG links', links)) + tuple(KL.items()):
    aus['bestand'][nm] = mass(FVb, q)
# Rueckrechnung des Bestands (form_video.video_umkehr) an denselben Stellen
eB = FV.video_umkehr(np.clip(V + .5, 0, 255).astype(np.uint8), 2)
aus['bestand_rueckrechnung'] = {}
for nm, q in (('Hintergrund', M),) + tuple(KL.items()):
    d = eB[q] - eG[q]; lum = eG[q].mean(1)
    for lo_, hi_ in ((0, 15), (15, 30), (30, 50), (50, 80)):
        s = (lum >= lo_) & (lum < hi_)
        if s.sum() > 30:
            aus['bestand_rueckrechnung']['%s %d-%d' % (nm, lo_, hi_)] = dict(n=int(s.sum()), bias=[round(float(x), 1) for x in np.median(d[s], 0)])
TH = {}
for art in ('diag', 'frei'):
    v = {}
    th = fit(M, frau_alle, art)
    TH[art] = th
    P = em.kurve(X, th)
    v['F_alle'] = dict(th=[round(float(x), 5) for x in th], Hintergrund=mass(P, M), **{k: mass(P, q) for k, q in KL.items()})
    for k, q in KL.items():
        thk = fit(M, frau_alle & ~q, art)
        v['F_ohne_' + k] = dict(test=mass(em.kurve(X, thk), q))
    thl = fit(links, frau_alle, art)
    v['F_bg_links_fit'] = dict(test_rechts=mass(em.kurve(X, thl), rechts))
    thr = fit(rechts, frau_alle, art)
    v['F_bg_rechts_fit'] = dict(test_links=mass(em.kurve(X, thr), links))
    v['rueckrechnung_e'] = rueck(th)
    v['cond_C'] = round(float(np.linalg.cond(em.teile(th)[3])), 3)
    aus['varianten'][art] = v
    print(art, 'alle', v['F_alle']['Hintergrund']['dE76_median'], [v['F_alle'][k]['dE76_der_mediane'] for k in KL],
          'holdout', [v['F_ohne_' + k]['test']['dE76_der_mediane'] for k in KL], v['F_bg_links_fit']['test_rechts']['dE76_median'],
          v['F_bg_rechts_fit']['test_links']['dE76_median'], flush=True)
json.dump(aus, open(ek.DATEN + '/e4b_gemeinsam.json', 'w'), indent=1, ensure_ascii=False)
json.dump(dict(bild=2, alle={k: [float(x) for x in v] for k, v in TH.items()}, hinweis='gemeinsam Hintergrund + Frau; Wahl in e4c/Messblatt'),
          open(ek.DATEN + '/e4b_modell_bild2.json', 'w'), indent=1)
print('bestand', {k: (v.get('dE76_median'), v.get('dE76_der_mediane')) for k, v in aus['bestand'].items()})
