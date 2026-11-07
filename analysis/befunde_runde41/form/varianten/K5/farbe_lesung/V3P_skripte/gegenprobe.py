"""Pruefer V3, Vorwaerts-Gegenprobe: Texel T an der Leuchtstelle -> T x Licht je Pixel (K5c-Szenenlicht, Weiss-Render)
-> in das rueckgerechnete Spielbild e des Originals einsetzen -> video_filter -> Mittel im Kern gegen das Original-Video.
So entfaellt die Annahme eines einheitlichen Figurlichts (Leser A/B), und die Nachbarschaft ist die des Originals."""
from szene import *
import json
from scipy.optimize import minimize

FP = json.load(open(FORM + '/farbe_palette.json', encoding='utf-8'))
V = {n: P.original(n) for n in (1, 2)}
E = {n: np.clip(FV.video_umkehr(V[n], n), 0, 255).astype(np.float32) for n in (1, 2)}
LP = {n: np.load('L%d.npy' % n) for n in (1, 2)}
MF = {n: np.load('m%d.npy' % n) for n in (1, 2)}
REG = {'F1': (1, np.load('F1_pruef.npy')), 'band': (2, np.load('band2_pruef.npy')), 'zaehne': (2, np.load('zahn2_pruef.npy'))}
# gelber Saum, gleiche Regel wie orte.py
Le2 = lab(E[2]); h2 = np.degrees(np.arctan2(Le2[..., 2], Le2[..., 1])); C2 = np.hypot(Le2[..., 1], Le2[..., 2])
uk = (cv2.imread(FORM + '/nutzer_teile/maske_unterkiefer.png', 0) > 0); uk[428:, :262] = False
REG['gelb'] = (2, uk & (C2 >= 10) & (h2 > 60) & (h2 <= 100) & (V[2].min(2) < 200) & ~((Le2[..., 1] >= 10) & (h2 > -10) & (h2 <= 60)))
KERN = {k: (n, cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8), iterations=1 if k in ('zaehne', 'gelb') else 2).astype(bool)) for k, (n, m) in REG.items()}


def licht_in(k, art='k5c'):
    n, m = REG[k]
    L = LP[n].copy()
    med = np.median(L[m & MF[n]], 0)
    L[m & ~MF[n]] = med
    if art != 'k5c':
        L[m] = np.array(FP['licht']['bild1'][art], np.float32)
    return L


def sim(k, T, art='k5c'):
    n, m = REG[k]
    img = E[n].copy()
    L = licht_in(k, art)
    img[m] = np.clip(np.asarray(T, np.float32)[None] * L[m], 0, 255)
    v = FV.video_filter(np.clip(img + 0.5, 0, 255).astype(np.uint8), bild=n, entzerren=False, rauschen=False)
    return v[KERN[k][1]].astype(np.float32).mean(0)


def basis(k):
    n, m = REG[k]
    v = FV.video_filter(np.clip(E[n] + 0.5, 0, 255).astype(np.uint8), bild=n, entzerren=False, rauschen=False)
    return v[KERN[k][1]].astype(np.float32).mean(0)


def ziel(k):
    n, m = REG[k]
    return V[n][KERN[k][1]].astype(np.float32).mean(0)


def dE(a, b):
    return float(np.linalg.norm(lab(a) - lab(b)))


def fit(ks, art='k5c', T0=(150, 90, 90)):
    def f(T):
        T = np.clip(T, 0, 255)
        return sum(dE(sim(k, T, art), ziel(k)) ** 2 for k in ks)
    r = minimize(f, np.array(T0, float), method='Nelder-Mead', options=dict(xatol=1.0, fatol=0.05, maxiter=250))
    T = np.clip(r.x, 0, 255)
    return T, {k: round(dE(sim(k, T, art), ziel(k)), 2) for k in ks}


def r1(v):
    return [round(float(x), 1) for x in v]


if __name__ == '__main__':
    R = {}
    for k in REG:
        n, m = REG[k]
        R['region_%s' % k] = dict(bild=n, px=int(m.sum()), kern_px=int(KERN[k][1].sum()), V_ziel=r1(ziel(k)), V_ziel_lab=r1(lab(ziel(k))),
                                  rundlauf_dE=round(dE(basis(k), ziel(k)), 2), licht_K5c_median=[round(float(x), 3) for x in np.median(LP[n][m & MF[n]], 0)],
                                  T_einfach_e_durch_licht=r1(np.median(E[n][m & MF[n]] / np.maximum(LP[n][m & MF[n]], 1e-3), 0)))
    # Einzelanpassungen
    for k, T0 in (('F1', (140, 90, 105)), ('band', (175, 90, 85)), ('zaehne', (200, 190, 180)), ('gelb', (210, 160, 120))):
        T, d = fit([k], T0=T0)
        R['fit_%s' % k] = dict(T=r1(T), T_lab=r1(lab(T)), farbton=round(float(np.degrees(np.arctan2(lab(T)[2], lab(T)[1]))), 1), dE=d)
    for art in ('faktor_leon_gesicht', 'faktor_leon_hellste'):
        T, d = fit(['F1'], art=art, T0=(140, 90, 105))
        R['fit_F1_%s' % art] = dict(T=r1(T), T_lab=r1(lab(T)), farbton=round(float(np.degrees(np.arctan2(lab(T)[2], lab(T)[1]))), 1), dE=d)
    T, d = fit(['F1', 'band'], T0=(165, 92, 95))
    R['fit_gemeinsam_F1_band'] = dict(T=r1(T), T_lab=r1(lab(T)), farbton=round(float(np.degrees(np.arctan2(lab(T)[2], lab(T)[1]))), 1), dE=d)
    TF1 = np.array(R['fit_F1']['T']); TB = np.array(R['fit_band']['T'])
    R['kreuz_T_F1_in_band_dE'] = round(dE(sim('band', TF1), ziel('band')), 2)
    R['kreuz_T_band_in_F1_dE'] = round(dE(sim('F1', TB), ziel('F1')), 2)
    # Kandidaten
    G4 = {'G4 F8 Feueranteil': (65.6, 25.4, 30.0), 'G4 F16 Feueranteil': (55.4, 37.6, 28.3), 'G4 F8 Mittel': (56.4, 17.5, 21.7),
          'G4 F7 Mittel': (39.9, 46.5, 26.9), 'G4 F2 Mittel': (35.8, 42.3, 20.4), 'G4 F1 Mittel': (30.3, 9.1, 6.4), 'G4 F20 Mittel': (26.1, 18.4, 7.5)}
    kand = {'A Bild1 (133,81,104)': (133, 81, 104), 'A Bild2 Creme (234,208,189)': (234, 208, 189), 'B gemeinsam frei (174,95,96)': (174, 95, 96),
            'B gemeinsam Figurlicht (163,91,96)': (163, 91, 96), 'B Band (177,89,84)': (177, 89, 84), 'Orange (255,140,40)': (255, 140, 40),
            'Gelb (255,200,60)': (255, 200, 60), 'Feuerrot (220,60,30)': (220, 60, 30)}
    for nm, L in G4.items():
        kand[nm] = tuple(int(round(x)) for x in rgb_von_lab(L))
    K_ = {}
    for nm, T in kand.items():
        K_[nm] = dict(T=list(T), T_lab=r1(lab(T)), dE_F1=round(dE(sim('F1', T), ziel('F1')), 1), dE_band=round(dE(sim('band', T), ziel('band')), 1),
                      V_F1=r1(sim('F1', T)), V_band=r1(sim('band', T)))
    R['kandidaten'] = K_
    json.dump(R, open('gegenprobe.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    for k, v in R.items():
        if k != 'kandidaten':
            print(k, v)
    for k, v in K_.items():
        print('  ', k, v)
