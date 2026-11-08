"""Zusatzmessungen fuer das Messblatt Fotovorlage: (a) Rot-Anteil je fester Bildregion (posenunabhaengig, nur Foto),
(b) Huckel der oberen Kontur (Modell >= 4 px ueber dem Foto) mit Knochen/Flaeche/Knochenraum, (c) Kopfhoehe gegen Figur."""
import os, sys, json, numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import f_rast as FR
from f_analyse import pose_von
S = os.path.join(G.SFV4, 'foto')
REG = {1: {'Kopf (Kopfbox V4)': (368, 139, 416, 191), 'Brust Mitte (B-Mitte)': (360, 192, 450, 245),
           'Schulter/Oberarm Bild-links = (W)-rechts': (296, 175, 355, 250), 'Schulter/Oberarm Bild-rechts = (W)-links': (455, 178, 512, 265),
           'Unterarm/Hand Bild-rechts = (W)-links': (478, 265, 512, 372), 'Bauch/Huefte': (360, 250, 445, 300)},
       2: {'Kopf (Kopfbox V4)': (412, 190, 467, 263), 'Buckel/Ruecken oben': (330, 186, 410, 225),
           'Schulter/Oberarm Mitte = (W)-rechts': (360, 228, 430, 300), 'Schwanz Bogen': (280, 222, 335, 262)}}


def main():
    out = {}
    mo = b2.modell(G.EMD_V4)
    A_ = json.load(open(os.path.join(S, 'analyse.json')))
    for b in (1, 2):
        v = G.vorlage(b); Lf = G.lab(v); T = np.load(os.path.join(S, 'trimap_%d.npy' % b))
        r = {}
        for n, (x0, y0, x1, y1) in REG[b].items():
            m = np.zeros(T.shape, bool); m[y0:y1 + 1, x0:x1 + 1] = True; m &= T == 1
            r[n] = dict(box=[x0, y0, x1, y1], px=int(m.sum()), rot=round(float((Lf[m][:, 1] >= 10).mean()), 3),
                        lab=G.r1(Lf[m].mean(0)), a50=round(float(np.median(Lf[m][:, 1])), 1))
        # Modell gleiche Boxen (gefundene Pose, Videoraum)
        pose = A_[str(b)]['pose']
        A = FR.mit_bg(mo, pose); bo = A['bone'][:480]; Lm = G.lab(G.videoraum(A['img'][:480]))
        for n, (x0, y0, x1, y1) in REG[b].items():
            m = np.zeros(T.shape, bool); m[y0:y1 + 1, x0:x1 + 1] = True; m &= bo >= 0
            r[n]['modell_px'] = int(m.sum()); r[n]['modell_rot'] = round(float((Lm[m][:, 1] >= 10).mean()), 3) if m.any() else None
        # Huckel
        ko = A_[str(b)]['obere_kontur']; huckel = []
        for xs_, k in sorted(ko.items(), key=lambda t: int(t[0])):
            if k['d'] is not None and k['d'] >= 4:
                x = int(xs_); y = k['modell']
                huckel.append(dict(x=x, foto=k['foto'], modell=y, d=k['d'], knochen=k['knochen'], teil=k['teil'], flaeche=A['flaeche'][y, x],
                                   zeile=int(A['zeile'][y, x]), lok=k['lok']))
        # Kopfhoehe: Kopf-Oberkante unter der Figur-Oberkante (in H)
        ys, xs = np.nonzero(bo == 9); fy = np.nonzero((bo >= 0).any(1))[0]
        H = A_[str(b)]['H_port']
        tf = np.nonzero((T == 1).any(1))[0]
        kb = G.KB_ORIG[b]
        out[b] = dict(regionen=r, huckel=huckel,
                      kopf=dict(modell_kopf_ok_unter_figur_ok_H=round((ys.min() - fy.min()) / H, 3), modell_kopf_box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                                modell_kopf_wh_H=[round((xs.max() - xs.min() + 1) / H, 3), round((ys.max() - ys.min() + 1) / H, 3)],
                                foto_kopf_ok_unter_figur_ok_H=round((kb[1] - tf.min()) / G.H_ORIG[b], 3),
                                foto_kopf_wh_H=[round((kb[2] - kb[0]) / G.H_ORIG[b], 3), round((kb[3] - kb[1]) / G.H_ORIG[b], 3)],
                                modell_kopf_mitte=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                                foto_kopf_mitte=[(kb[0] + kb[2]) / 2, (kb[1] + kb[3]) / 2]))
    # Maul in der Vorderansicht posen_v4 "135" (dort ist das Gesicht zur Kamera; in der Vergleichsansicht 1 abgewandt)
    p135 = G.posen()['135']
    A = FR.mit_bg(mo, p135); bo = A['bone'][:480]; zl = A['zeile'][:480]; fl = A['flaeche'][:480]
    Lm = G.lab(G.videoraum(A['img'][:480]))
    mm = (bo == 9) & (zl == 487)
    F = FR.mit_bg(mo, p135, filt=lambda bn, key, zeile: bn == 9 and zeile == 487); fm = F['bone'][:480] == 9
    vd = {}
    for y, x in zip(*np.nonzero(fm & ~mm)):
        k = '%d/%s/z%d' % (bo[y, x], fl[y, x], zl[y, x]); vd[k] = vd.get(k, 0) + 1
    ys, xs = np.nonzero(mm); H = G.h_port(A['bone'])[2]
    ueber = {}
    if len(xs):
        for x in range(xs.min(), xs.max() + 1):            # Kopfpixel (nicht Maul) direkt ueber dem Maul je Spalte (bis 6 px)
            col = np.nonzero(mm[:, x])[0]
            if not len(col): continue
            for y in range(max(col.min() - 6, 0), col.min()):
                if bo[y, x] == 9 and zl[y, x] != 487:
                    k = '9/%s/z%d' % (fl[y, x], zl[y, x]); ueber[k] = ueber.get(k, 0) + 1
    out['maul_vorn_135'] = dict(pose=p135, sichtbar_px=int(mm.sum()), fussabdruck_px=int(fm.sum()), verdecker=vd,
                                box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                                breite_H=round((xs.max() - xs.min() + 1) / H, 3) if len(xs) else None,
                                hoehe_H=round((ys.max() - ys.min() + 1) / H, 3) if len(xs) else None,
                                lab=G.r1(Lm[mm].mean(0)) if mm.any() else None, hell_px=int((mm & (Lm[..., 0] > 45)).sum()),
                                hell_lab=G.r1(Lm[mm & (Lm[..., 0] > 45)].mean(0)) if (mm & (Lm[..., 0] > 45)).any() else None,
                                kopfflaechen_ueber_maul=dict(sorted(ueber.items(), key=lambda t: -t[1])), H_port=int(H))
    print('maul135', out['maul_vorn_135'])
    json.dump(out, open(os.path.join(S, 'zusatz.json'), 'w'), indent=1, default=str)
    for b in (1, 2):
        print('==', b, out[b]['kopf'])
        for n, v in out[b]['regionen'].items(): print('  ', n, v)
        for h in out[b]['huckel'][::3]: print('   H', h)


if __name__ == '__main__':
    main()
