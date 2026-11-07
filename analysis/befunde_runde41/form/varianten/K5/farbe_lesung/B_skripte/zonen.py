"""Zonen der Riesenform: Original Bild 1/2 (Rueckrechnung form_video.video_umkehr) und K5c (Render mit Szenenlicht, gleiche Kette)."""
import json, os, sys, math
import numpy as np, cv2
import fl
sys.path.insert(0, fl.TOOLS)
import form_video as FV
NT = fl.FORM + '/nutzer_teile'
# farbe_palette.json licht.bild1.faktor_figur / licht.bild2.faktor
LICHT = {1: np.array([0.487, 0.432, 0.374]), 2: np.array([0.385, 0.342, 0.307])}
PR = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/pruefer/'


def mk(n):
    return cv2.imread(NT + '/maske_%s.png' % n, 0) > 0


BODEN = np.load(PR + 'boden_fuss_links.npy') | np.load(PR + 'boden_fuss_rechts.npy')
# Schriftzug BIOHAZARD 1.5 / www.bioflames.com unten links in beiden Bildern: x 0-261, y 428-479
LOGO = np.zeros((480, 640), bool)
LOGO[428:, :262] = True


def regionen(n):
    if n == 1:
        tail = mk('tail')
        pob = mk('part_of_Body') & ~tail
        bl = (mk('left_leg') | mk('bein_links') | mk('fuss_links')) & ~tail
        br = (mk('right_leg') | mk('bein_rechts') | mk('fuss_rechts')) & ~tail & ~pob
        ra = mk('right_arm') & ~pob
        R = [('Schwanz (tail)', tail), ('Ruecken/Flanke (part_of_Body)', pob),
             ('Hinterbein links (W) (left_leg+bein/fuss_links)', bl),
             ('Hinterbein rechts (W) (right_leg+bein/fuss_rechts)', br), ('Vorderglied rechts (W) (right_arm)', ra)]
    else:
        ok = mk('oberkiefer')
        uk = mk('unterkiefer')
        fu = mk('face_and_upper_body') & ~ok & ~uk
        raf = mk('right_arm_front') & ~uk & ~ok
        laf = mk('left_arm_front') & ~fu & ~ok
        R = [('Kopf/Oberkiefer (oberkiefer)', ok), ('Kopfoberseite/Rumpf (face_and_upper_body ohne Kiefer)', fu),
             ('Unterkiefer (unterkiefer)', uk), ('Klaue rechts (W) (right_arm_front ohne UK)', raf),
             ('Klaue links (W) (left_arm_front)', laf)]
    return [(nm, m & ~BODEN & ~LOGO) for nm, m in R]


def lab(rgb):
    return fl.lab(np.clip(np.round(rgb), 0, 255).astype(np.uint8))


KL = ['D dunkle Grundhaut', 'M rotbraun/violett Mittelton', 'R rosa/rot Fleisch', 'H helles Band (neutral/kuehl)',
      'W weisser Kern (geclippt)']
KL_FARBE = [(40, 40, 40), (140, 60, 90), (240, 90, 140), (120, 200, 230), (255, 255, 255)]


def klassen(L, clip):
    """L: Lab (N,3) der Texturschaetzung; clip: bool (N). Schwellen PORT-WAHL dieses Lesers, an farbe_palette.json gelegt:
    Grundhaut (44,18,27) L* 9; Rotbraun (103,63,84) L* 32; Rosa (181,137,150) L* 61 a* 17; Baender (126,145,156) L* 58 a* -3."""
    k = np.full(len(L), 1)
    k[L[:, 0] < 20] = 0
    hell = L[:, 0] >= 45
    k[hell & (L[:, 1] >= 10)] = 2
    k[(L[:, 0] >= 30) & (np.hypot(L[:, 1], L[:, 2]) >= 30) & (L[:, 1] > 0)] = 2
    k[hell & (L[:, 1] < 10)] = 3
    k[clip] = 4
    return k


def mehrton(L, k=6, merge=10.0, rng=0):
    """Anzahl Lab-Cluster >= 5 % (k-means k=6, Zentren mit dE < 10 verschmolzen), L*-Spanne p5..p95, Farbtonstreuung (Grad, C*>8)."""
    if len(L) < 30:
        return dict(n=0)
    Z = L.astype(np.float32)
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 50, 0.2)
    cv2.setRNGSeed(rng)
    _, lab_, cen = cv2.kmeans(Z, min(k, len(Z)), None, crit, 4, cv2.KMEANS_PP_CENTERS)
    lab_ = lab_.ravel()
    cen = cen.astype(float)
    grp = list(range(len(cen)))

    def f(i):
        while grp[i] != i:
            i = grp[i]
        return i
    for i in range(len(cen)):
        for j in range(i + 1, len(cen)):
            if np.linalg.norm(cen[i] - cen[j]) < merge:
                grp[f(j)] = f(i)
    g = np.array([f(i) for i in lab_])
    ant = {gi: (g == gi).mean() for gi in set(g.tolist())}
    n = sum(1 for a in ant.values() if a >= 0.05)
    C = np.hypot(L[:, 1], L[:, 2])
    hm = C > 8
    hs = None
    if hm.sum() > 20:
        h = np.arctan2(L[hm, 2], L[hm, 1])
        Rr = np.hypot(np.cos(h).mean(), np.sin(h).mean())
        hs = round(math.degrees(math.sqrt(max(-2 * math.log(max(Rr, 1e-9)), 0))), 1)
    return dict(n=int(n), l_p5=round(float(np.percentile(L[:, 0], 5)), 1), l_p95=round(float(np.percentile(L[:, 0], 95)), 1),
                ton_streu_grad=hs, chroma_mittel=round(float(C.mean()), 1))


def rueckrechnen(video, n, glatt=1.0):
    e = FV.video_umkehr(video, bild=n)
    if glatt:
        e = cv2.GaussianBlur(e.astype(np.float32), (0, 0), glatt)
    T = np.clip(e / LICHT[n], 0, 255)
    clip = np.asarray(video).min(2) >= 200
    return e, T, clip


def zonen_messen(e, T, clip, regs, fig=None):
    res = []
    alle = np.zeros((480, 640), bool)
    regs = [(nm, (m & fig) if fig is not None else m) for nm, m in regs]
    for nm, m in regs:
        alle |= m
    gesamt = alle.sum()
    for nm, m in regs:
        if m.sum() == 0:
            res.append(dict(zone=nm, px=0))
            continue
        L = lab(T[m])
        kk = klassen(L, clip[m])
        nc = ~clip[m]
        ys, xs = np.where(m)
        r = dict(zone=nm, px=int(m.sum()), anteil_figur=round(m.sum() / max(gesamt, 1), 3),
                 klassen={KL[i][0]: round(float((kk == i).mean()), 3) for i in range(5)},
                 e_mittel=[int(round(x)) for x in e[m][nc].mean(0)] if nc.any() else None,
                 T_mittel=[int(round(x)) for x in T[m][nc].mean(0)] if nc.any() else None,
                 bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])
        r['T_lab'] = fl.lab1(r['T_mittel']) if r['T_mittel'] else None
        r['mehrton'] = mehrton(L[nc]) if nc.sum() > 30 else None
        r['klassen_farbe'] = {}
        for i in range(5):
            s = kk == i
            if s.sum() >= 15:
                c = T[m][s].mean(0)
                r['klassen_farbe'][KL[i][0]] = dict(T=[int(round(x)) for x in c], lab=fl.lab1(c), n=int(s.sum()))
        res.append(r)
    L = lab(T[alle])
    kk = klassen(L, clip[alle])
    ges = dict(zone='Figur gesamt', px=int(gesamt), klassen={KL[i][0]: round(float((kk == i).mean()), 3) for i in range(5)},
               mehrton=mehrton(L[~clip[alle]]), glanz_L85=round(float((L[:, 0] > 85).mean()), 4),
               T_mittel=[int(round(x)) for x in T[alle][~clip[alle]].mean(0)])
    ges['T_lab'] = fl.lab1(ges['T_mittel'])
    res.append(ges)
    return res, alle


def klassenbild(T, clip, maske):
    a = np.zeros((480, 640, 3), np.uint8)
    L = lab(T[maske])
    kk = klassen(L, clip[maske])
    a[maske] = np.array(KL_FARBE, np.uint8)[kk]
    return a
