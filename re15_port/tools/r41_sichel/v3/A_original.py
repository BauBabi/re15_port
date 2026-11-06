# Leser A V3: Original-Video 4 (640x480, ohne _slp) Bilder n1433-n1442 (h83-h92): Lab Schwanz gegen Koerperhaut.
# Masken = KI-Masken (A_schwanz.py: Schwanz; Haut = obere 0,45 H ohne Schwanz/Jeans) aus dem naechsten KI-Freisteller
# (birkin11 = n1433, birkin12 = n1435, birkin13 = n1442), auf 640x480 halbiert (Versatz/2), dann 3 px erodiert
# (= 6 px im 1280er Bild) gegen Lageversatz und Kantenmischung. Schwarzwert 17 abgezogen (Auftrag), Werte auf 0 begrenzt.
# Zum Vergleich dieselben Masken im KI-Bild (_slp, 1280x960) ohne Schwarzwertabzug.
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from scipy import ndimage as ndi

NAECHST = {1433: 11, 1434: 11, 1435: 12, 1436: 12, 1437: 12, 1438: 12, 1439: 13, 1440: 13, 1441: 13, 1442: 13}
SCHWARZ = 17.0


def voll(maske, nn, shape, faktor):
    n, h, ox, oy = MAP[nn]
    M = np.zeros(shape, bool)
    ys, xs = np.nonzero(maske)
    X = ((xs + ox) * faktor).astype(int)
    Y = ((ys + oy) * faktor).astype(int)
    ok = (X >= 0) & (X < shape[1]) & (Y >= 0) & (Y < shape[0])
    M[Y[ok], X[ok]] = True
    return M


if __name__ == '__main__':
    out = {}
    for n, nn in NAECHST.items():
        sm = np.load(SCR + 'A_schwanz_m%02d.npy' % nn)
        hm = np.load(SCR + 'A_haut_m%02d.npy' % nn)
        o = np.array(Image.open(SCR + 'A_orig/n%d.png' % n).convert('RGB')).astype(np.float32)
        o = np.clip(o - SCHWARZ, 0, 255)
        S = erode(voll(sm, nn, o.shape[:2], 0.5), 3)
        Hm = erode(voll(hm, nn, o.shape[:2], 0.5), 3)
        L3 = lab(o)
        rg = o[..., 0] - o[..., 1]
        r = dict(maske_aus='birkin%02d' % nn, exakt=(MAP[nn][0] == n), schwanz=stats(o, L3, S), haut=stats(o, L3, Hm),
                 schwanz_rot_rg20=round(float((S & (rg > 20)).sum() / S.sum()), 3), haut_rot_rg20=round(float((Hm & (rg > 20)).sum() / Hm.sum()), 3),
                 schwanz_rot_a15=round(float((S & (L3[..., 1] > 15)).sum() / S.sum()), 3), haut_rot_a15=round(float((Hm & (L3[..., 1] > 15)).sum() / Hm.sum()), 3))
        r['dE00'] = round(de2000(r['schwanz']['lab'], r['haut']['lab']), 1)
        r['dE76'] = round(de76(r['schwanz']['lab'], r['haut']['lab']), 1)
        r['L_verh'] = round(r['schwanz']['lab'][0] / r['haut']['lab'][0], 2)
        # Hintergrund direkt neben dem Schwanz (Ring 4-10 px ausserhalb, nicht Figur) - Trennbarkeit
        fig = voll(np.ones_like(sm), nn, o.shape[:2], 0.5)
        ring = ndi.binary_dilation(voll(sm, nn, o.shape[:2], 0.5), iterations=10) & ~ndi.binary_dilation(voll(sm, nn, o.shape[:2], 0.5), iterations=4)
        r['hintergrund_neben'] = stats(o, L3, ring)
        # KI-Bild, dieselbe Maske (1280, ohne Halbierung)
        k = np.array(Image.open(SCR + 'A_slp/n%d.png' % n).convert('RGB')).astype(np.float32)
        Sk = erode(voll(sm, nn, k.shape[:2], 1.0), 6)
        Hk = erode(voll(hm, nn, k.shape[:2], 1.0), 6)
        K3 = lab(k)
        r['ki_schwanz'] = stats(k, K3, Sk)
        r['ki_haut'] = stats(k, K3, Hk)
        r['ki_dE00'] = round(de2000(r['ki_schwanz']['lab'], r['ki_haut']['lab']), 1)
        r['ki_L_verh'] = round(r['ki_schwanz']['lab'][0] / r['ki_haut']['lab'][0], 2)
        out[n] = r
        print('n%d (h%d, Maske %s%s): O Schwanz %s Haut %s dE00 %.1f L-Verh %.2f rot(rg20) S %.2f H %.2f | Hintergrund %s | KI Schwanz %s Haut %s dE00 %.1f L-Verh %.2f' % (
            n, n - 1350, r['maske_aus'], '' if r['exakt'] else '~', r['schwanz']['lab'], r['haut']['lab'], r['dE00'], r['L_verh'],
            r['schwanz_rot_rg20'], r['haut_rot_rg20'], r['hintergrund_neben']['lab'], r['ki_schwanz']['lab'], r['ki_haut']['lab'], r['ki_dE00'], r['ki_L_verh']))
    json.dump(out, open(SCR + 'A_original.json', 'w'), indent=1)
