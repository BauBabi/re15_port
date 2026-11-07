"""Pruefer: Zonen Bild 1/2 rueckgerechnet (Original und K5c durch dieselbe Kette). Eigene Klassen + Leser-B-Klassen zum Abgleich."""
import json, sys, math, os
import numpy as np, cv2
from p_lade import WT, K5, lab
sys.path.insert(0, WT + '/re15_port/tools')
import form_video as FV
import form_pruefung as P
FORM = WT + '/analysis/befunde_runde41/form'
NT = FORM + '/nutzer_teile'
FP = json.load(open(FORM + '/farbe_palette.json'))
LICHT = {1: np.array(FP['licht']['bild1']['faktor_figur']), 2: np.array(FP['licht']['bild2']['faktor'])}
PD = os.path.dirname(os.path.abspath(__file__))  # Bodenpixel des Pruefers (boden_fuss_*.png = scratchpad/leser/pruefer/boden_fuss_*.npy)
BODEN = (cv2.imread(PD + '/boden_fuss_links.png', 0) > 0) | (cv2.imread(PD + '/boden_fuss_rechts.png', 0) > 0)
LOGO = np.zeros((480, 640), bool); LOGO[428:, :262] = True


def mk(n):
    return cv2.imread(NT + '/maske_%s.png' % n, 0) > 0


def regionen(n):
    if n == 1:
        tail = mk('tail'); pob = mk('part_of_Body') & ~tail
        bl = (mk('left_leg') | mk('bein_links') | mk('fuss_links')) & ~tail
        br = (mk('right_leg') | mk('bein_rechts') | mk('fuss_rechts')) & ~tail & ~pob
        ra = mk('right_arm') & ~pob
        R = [('Schwanz', tail), ('Ruecken/Flanke', pob), ('Hinterbein l (W)', bl), ('Hinterbein r (W)', br), ('Vorderglied r (W)', ra)]
    else:
        ok = mk('oberkiefer'); uk = mk('unterkiefer')
        fu = mk('face_and_upper_body') & ~ok & ~uk
        raf = mk('right_arm_front') & ~uk & ~ok; laf = mk('left_arm_front') & ~fu & ~ok
        R = [('Oberkiefer', ok), ('Kopf/Rumpf', fu), ('Unterkiefer', uk), ('Klaue r (W)', raf), ('Klaue l (W)', laf)]
    return [(a, m & ~BODEN & ~LOGO) for a, m in R]


def klassen_p(L):
    """Pruefer-Klassen: dunkel L*<20 | mittel 20..45 unbunt | rot a*>=12 und C*>=15 und L*>=20 | hell L*>=45 nicht rot."""
    C = np.hypot(L[:, 1], L[:, 2])
    rot = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20)
    d = (L[:, 0] < 20) & ~rot
    h = (L[:, 0] >= 45) & ~rot
    m = ~(rot | d | h)
    return dict(D=round(100 * float(d.mean()), 1), M=round(100 * float(m.mean()), 1), R=round(100 * float(rot.mean()), 1), H=round(100 * float(h.mean()), 1))


def klassen_b(L, clip):
    k = np.full(len(L), 1); k[L[:, 0] < 20] = 0
    hell = L[:, 0] >= 45
    k[hell & (L[:, 1] >= 10)] = 2
    k[(L[:, 0] >= 30) & (np.hypot(L[:, 1], L[:, 2]) >= 30) & (L[:, 1] > 0)] = 2
    k[hell & (L[:, 1] < 10)] = 3; k[clip] = 4
    return {c: round(100 * float((k == i).mean()), 1) for i, c in enumerate('DMRHW')}


def messe(T, clip, regs, fig=None):
    res = {}; alle = np.zeros((480, 640), bool)
    regs = [(a, (m & fig) if fig is not None else m) for a, m in regs]
    for a, m in regs:
        alle |= m
    for a, m in regs + [('Figur', alle)]:
        L = lab(np.clip(T[m], 0, 255)); C = np.hypot(L[:, 1], L[:, 2])
        res[a] = dict(px=int(m.sum()), anteil=round(m.sum() / max(alle.sum(), 1), 3), lab=[round(float(x), 1) for x in L.mean(0)],
                      rgb=[int(round(x)) for x in T[m].mean(0)], chroma=round(float(C.mean()), 1),
                      L_p5_p95=[round(float(np.percentile(L[:, 0], q)), 1) for q in (5, 95)], L85=round(100 * float((L[:, 0] > 85).mean()), 2),
                      ab_streu=round(float(np.sqrt(L[:, 1].var() + L[:, 2].var())), 1),
                      kl_p=klassen_p(L), kl_b=klassen_b(L, clip[m]))
    return res


def rueck(video, n):
    e = FV.video_umkehr(video, bild=n)
    e = cv2.GaussianBlur(e.astype(np.float32), (0, 0), 1.0)
    return np.clip(e / LICHT[n], 0, 255), np.asarray(video).min(2) >= 200


if __name__ == '__main__':
    out = {}
    for n in (1, 2):
        v = cv2.cvtColor(cv2.imread(FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        T, clip = rueck(v, n)
        out['orig%d' % n] = messe(T, clip, regionen(n))
    k = P.Kandidat(K5 + '/K5c/form.json')
    for n in (1, 2):
        sauber, alb, m, kk = k.rendern(n)
        fig = m & (alb.max(2) > 0)
        vid = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True)
        T, clip = rueck(vid, n)
        out['k5c%d_kette' % n] = messe(T, clip, regionen(n), fig)
        out['k5c%d_albedo' % n] = messe(alb.astype(float), np.zeros((480, 640), bool), regionen(n), fig)
        out['k5c%d_licht' % n] = [float(x) for x in kk] if kk is not None else None
    # Kiefer Bild 2: Band (rot) und Zahnflecken (hell, a*<10) getrennt, Original
    v = cv2.cvtColor(cv2.imread(FORM + '/bild2.png'), cv2.COLOR_BGR2RGB); T, clip = rueck(v, 2)
    uk = dict(regionen(2))['Unterkiefer']; L = lab(np.clip(T[uk], 0, 255)); C = np.hypot(L[:, 1], L[:, 2])
    band = (L[:, 1] >= 12) & (C >= 15); zahn = (L[:, 0] >= 60) & (L[:, 1] < 10)
    out['kiefer_orig2'] = dict(band_anteil=round(float(band.mean()), 3), band_lab=[round(float(x), 1) for x in L[band].mean(0)],
                               band_rgb=[int(round(x)) for x in np.clip(T[uk], 0, 255)[band].mean(0)],
                               zahn_anteil=round(float(zahn.mean()), 3), zahn_lab=[round(float(x), 1) for x in L[zahn].mean(0)])
    mantel = np.zeros((480, 640), bool); mantel[200:340, 140:200] = True
    Lm = lab(np.clip(T[mantel], 0, 255)); s = Lm[:, 0] > 55
    out['mantel_orig2'] = dict(px=int(s.sum()), lab=[round(float(x), 1) for x in Lm[s].mean(0)])
    json.dump(out, open('p_zonen.json', 'w'), indent=1)
    for kk_, vv in out.items():
        print('==', kk_)
        if isinstance(vv, dict):
            for a, r in vv.items():
                print('  ', a, r)
        else:
            print('  ', vv)
