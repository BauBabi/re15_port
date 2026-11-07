"""Pruefer V3, eigener Code: K5c in Kamera Bild 1/2 mit Szenenlicht; Licht je Pixel aus einem Weiss-Texel-Render;
Knochen-ID je Pixel durch Weglassen je Knochen (unabhaengig vom Rasterer von Leser B)."""
import sys, os, json, copy
import numpy as np, cv2
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import form_pruefung as P, form_buehne as FB, form_video as FV
FORM = WT + '/analysis/befunde_runde41/form'
K5 = FORM + '/varianten/K5'
HIER = os.path.dirname(os.path.abspath(__file__))
K = P.Kandidat(K5 + '/K5c/form.json')
WEISS = copy.copy(K.tex); WEISS.lut = K.tex.lut.copy(); WEISS.lut[..., :3] = 255
NB = K.mo.skel.bone_count


def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(-1, 3) / 255.0
    return cv2.cvtColor(a[None], cv2.COLOR_RGB2LAB)[0].reshape(np.shape(rgb))


def rgb_von_lab(L):
    a = np.asarray(L, np.float32).reshape(1, -1, 3)
    return np.clip(cv2.cvtColor(a, cv2.COLOR_LAB2RGB)[0] * 255, 0, 255).reshape(np.shape(L))


def render(n, teile=None):
    lage, gier, clip, bild = K.stand(n)
    kam = P.kamera(n); kf = K.kf(clip, bild); licht, kk = P.licht_fuer(n, K.licht)
    tl = FB.dreiecke(K.mo, kf, lage, gier, kam, licht, teile)
    ta = FB.dreiecke(K.mo, kf, lage, gier, kam, None, teile)
    alb = np.zeros((480, 640, 3), np.uint8); FB.rastern(ta, K.tex, alb, kam.abbildung)
    wb = np.zeros((480, 640, 3), np.uint8); m = FB.rastern(tl, WEISS, wb, kam.abbildung)
    sau = np.zeros((480, 640, 3), np.uint8); FB.rastern(tl, K.tex, sau, kam.abbildung)
    return alb, wb.astype(np.float32) / 255.0, np.asarray(m, bool), sau


def knochen_karte(n):
    p = os.path.join(HIER, 'kn%d.npy' % n)
    if os.path.exists(p):
        return np.load(p)
    alb, Lp, m, _ = render(n)
    kn = np.full((480, 640), -1, np.int32)
    alle = list(range(NB))
    for b in alle:
        a2, L2, m2, _ = render(n, [x for x in alle if x != b])
        d = (np.abs(a2.astype(int) - alb.astype(int)).sum(2) > 0) | (np.abs(L2 - Lp).sum(2) > 1e-6) | (m2 != m)
        kn[d & m] = b
    np.save(p, kn)
    return kn


if __name__ == '__main__':
    for n in (1, 2):
        alb, Lp, m, sau = render(n)
        kn = knochen_karte(n)
        print(n, 'Figur px', int(m.sum()), 'ohne Knochen-ID', int(((kn < 0) & m).sum()), 'Licht Median', np.round(np.median(Lp[m], 0), 3))
        np.save(os.path.join(HIER, 'L%d.npy' % n), Lp); np.save(os.path.join(HIER, 'alb%d.npy' % n), alb); np.save(os.path.join(HIER, 'm%d.npy' % n), m)
