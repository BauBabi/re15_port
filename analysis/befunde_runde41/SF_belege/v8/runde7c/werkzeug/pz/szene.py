"""Pruefer P5 (Zahlen) - Auswertung der EIGENEN Szenenlaeufe (echte Bild-Dumps 960x720, laeufe/<N> und laeufe/<N>_LM).
Klassen aus dem Markerlauf (mark.py): 0 Rest, 1 Mesh8 unten, 2 Mesh8 oben (R), 3 Kopf, 4 Gesicht, 5 Kopf-Anhang; -1 kein Birkin.
Figur = |LM - plain| Summe > FIG (Lauf deterministisch: Hintergrund und Leon/Ada bitgleich).
Aufruf: python szene.py klassen N            -> laeufe/N_kl.npz (F225-F450, 960x720 int8)
        python szene.py luecke N             -> S16s je Bild F235-F378 (640x480, Soll 21/7/3,2)
"""
import sys, os, json
import numpy as np
from PIL import Image
from scipy import ndimage
from skimage.morphology import convex_hull_image
L_ = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/laeufe/'
FIG = 30
F0, F1 = 225, 450


def ppm(n, f):
    p = L_ + '%s/fd/f%06d.ppm' % (n, f)
    P5 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/'
    if not os.path.exists(p):   # r7c: Laeufe der Pruefer R3/R2 (gleiche Exe 8f15f6c5, deterministisch) nur lesen
        for q in ('pruef_zahlen-r3', 'pruef_zahlen-r2'):
            if os.path.exists(P5 + q + '/laeufe/%s/fd/f%06d.ppm' % (n, f)):
                p = P5 + q + '/laeufe/%s/fd/f%06d.ppm' % (n, f); break
    return np.asarray(Image.open(p).convert('RGB')).astype(np.int32) if os.path.exists(p) else None


def klassen(lm, plain):
    mx = lm.max(-1)
    on = lm > np.maximum(0.5 * mx, 40)[..., None]
    r, g, b = on[..., 0], on[..., 1], on[..., 2]
    k = np.full(mx.shape, 0, np.int8)            # dunkle/unklare Figur = Rest
    k[r & ~g & ~b] = 1; k[b & ~r & ~g] = 2; k[r & b & ~g] = 3; k[g & b & ~r] = 4; k[r & g & ~b] = 5; k[g & ~r & ~b] = 0
    if plain is not None:
        k[np.abs(lm - plain).sum(-1) <= FIG] = -1
    return k


def klein(k):
    ys = np.clip(np.round(np.arange(480) * 1.5 + 0.25).astype(int), 0, 719)
    xs = np.clip(np.round(np.arange(640) * 1.5 + 0.25).astype(int), 0, 959)
    return k[ys][:, xs]


NAH = 12; S8 = np.ones((3, 3), bool)


def mass(k):
    """eigene Kerben-Messung wie luecke_eigen.mass, aus Klassen: H = 3,4,5; m8 = 1,2; R = 2; S = alles >= 0"""
    S = k >= 0; H = (k >= 3); m8 = (k == 1) | (k == 2); R = k == 2
    leer = dict(summe=0, kerbe=0, spalt=0, tief=0, maxtiefe=0.0)
    if not H.any():
        return leer
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard'); Hn = dH <= NAH; Hd = dH <= 1
    hull = convex_hull_image(H | (m8 & Hn), offset_coordinates=False)
    G = np.zeros_like(S); kerbe = 0
    lab, n = ndimage.label(hull & ~S, S8)
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & Hd).any() and (cd & R).any():
            G |= c; kerbe += int(c.sum())
    pad = 4; Sp = np.pad(S, pad)
    zu = ndimage.binary_erosion(ndimage.binary_dilation(Sp, np.ones((5, 5), bool)), np.ones((5, 5), bool))[pad:-pad, pad:-pad]
    lab, n = ndimage.label(zu & ~S & ~G, S8); spalt = 0
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & R).any() and (c & Hn).any():
            G |= c; spalt += int(c.sum())
    if not G.any():
        return leer
    d = ndimage.distance_transform_edt(G)  # Abstand zum naechsten Nicht-Kerbe-Punkt (Figur oder freier Hintergrund)
    d = ndimage.distance_transform_edt(~((~S) & ~G))[G]
    ys, xs = np.nonzero(G)
    return dict(summe=kerbe + spalt, kerbe=kerbe, spalt=spalt, tief=int((d >= 1.5).sum()), maxtiefe=round(float(d.max()), 1),
                ort=(int(xs.mean()), int(ys.mean())))


def soll(m):
    return m['summe'] <= 21 and m['tief'] <= 7 and m['maxtiefe'] <= 3.2


if __name__ == '__main__':
    was, n = sys.argv[1], sys.argv[2]
    if was == 'klassen':
        K = np.full((F1 - F0 + 1, 720, 960), -2, np.int8)
        pl_n = sys.argv[3] if len(sys.argv) > 3 else n
        for f in range(F0, F1 + 1):
            lm = ppm(n + '_LM', f); pn = ppm(pl_n, f)
            if lm is None:
                continue
            K[f - F0] = klassen(lm, pn)
        np.savez_compressed(L_ + '%s_kl.npz' % n, K=K)
        print(n, 'Klassen gespeichert', [(c, int((K == c).sum())) for c in range(-2, 6)])
    elif was == 'luecke':
        K = np.load(L_ + '%s_kl.npz' % n)['K']; out = []
        for f in range(235, 379):
            m = mass(klein(K[f - F0])); m['f'] = f; m['ok'] = soll(m); out.append(m)
        json.dump(out, open(L_ + '%s_luecke.json' % n, 'w'))
        ue = [m for m in out if not m['ok']]
        print(n, 'S16s ueber Soll', len(ue), 'von', len(out), '| Summe', sum(m['summe'] for m in out), 'tief', sum(m['tief'] for m in out))
        print('  ', ' '.join('F%d %d/%d/%.1f' % (m['f'], m['summe'], m['tief'], m['maxtiefe']) for m in ue))
