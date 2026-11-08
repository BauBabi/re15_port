"""Messblatt Fotovorlage, Schritt 3 (Fotoseite): obere Kontur, Kopfumriss oben, Maulzone, Augenfleck, Haarbogen.
Foto 1 entblendet (G.vorlage), Foto 2 roh. Lab = CIE-Lab D65 (G.lab). Kopfbox = Messblatt V4 (G.KB_ORIG).
Obere Kontur je Spalte: erste Zeile y (von oben), ab der 3 Zeilen in Folge dE76(Foto, BSS 50A0/12 im Videoraum, Gauss 1,0) > 10
liegen UND die innerhalb der Trimap-Figur (+-6 px) liegt; nur Spalten mit Figur sicher (trimap == 1)."""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
S = os.path.join(G.SFV4, 'foto')
bg = G.videoraum(G.hintergrund())


def obere_kontur(b, x0, x1, thr=10.0):
    v = G.vorlage(b); T = np.load(os.path.join(S, 'trimap_%d.npy' % b))
    d = np.sqrt(((G.lab(v) - G.lab(bg)) ** 2).sum(2)).astype(np.float32); d = cv2.GaussianBlur(d, (0, 0), 1.0)
    out = {}
    for x in range(x0, x1 + 1):
        fy = np.nonzero(T[:, x] == 1)[0]
        if not len(fy): continue
        y_t = fy.min()
        cand = [y for y in range(max(y_t - 12, 100), y_t + 13) if (d[y:y + 3, x] > thr).all()]
        out[x] = int(cand[0]) if cand else None
    return out


def lab_mittel(v, m):
    return [round(float(t), 1) for t in G.lab(v[m].reshape(-1, 1, 3)).reshape(-1, 3).mean(0)]


def main():
    R = {}
    v1, v2 = G.vorlage(1), G.vorlage(2)
    L1 = G.lab(v1)[..., 0]; L2 = G.lab(v2)[..., 0]
    R['obere_kontur'] = {1: obere_kontur(1, 296, 511), 2: obere_kontur(2, 300, 466)}
    # Augenfleck (p_blob-Verfahren, Suchfenster um das Linienende in Zeigerichtung, Messblatt V4 Abschnitt 2/3)
    aug = {}
    for b, such, v in ((1, (370, 158, 388, 176), v1), (2, (446, 228, 460, 246), v2)):
        r = G.blob(v, such, 'hell'); m = r['maske']
        aug[b] = dict(schwerpunkt=G.r1(r['sp']), bbox=r['bbox'], w_h=r['w_h'], px=r['px'], lab=lab_mittel(v, m),
                      L_extrem=round(float(G.lab(v[r['extrem'][1], r['extrem'][0]].reshape(1, 1, 3))[0, 0, 0]), 1))
        np.save(os.path.join(S, 'auge_%d.npy' % b), m)
    R['auge'] = aug
    # Maulzone Foto 1: unter der Nase (395,177), Zeilen 178-200, Spalten 376-414: Zeilenmittel L, dunkelstes Band
    zs = {}
    for y in range(176, 202):
        zs[y] = round(float(L1[y, 376:415].mean()), 1)
    ymin = min(zs, key=zs.get)
    band = np.zeros_like(L1, bool); band[ymin - 2:ymin + 3, 376:415] = True
    thr = float(np.percentile(L1[band], 50))
    dunkel = band & (L1 <= thr + 4)
    xs = np.nonzero(dunkel.any(0))[0]
    R['maul_1'] = dict(zeilen_L=zs, dunkelste_zeile=ymin, band_y=[ymin - 2, ymin + 2], band_x=[int(xs.min()), int(xs.max())],
                       breite_px=int(xs.max() - xs.min() + 1), breite_H=round((xs.max() - xs.min() + 1) / G.H_ORIG[1], 3),
                       lab_band=lab_mittel(v1, band), lab_dunkel=lab_mittel(v1, dunkel),
                       hell_im_band_px=int((band & (L1 > 45)).sum()))
    # Maulzone Foto 2: Unterseite der Kopf-Vorderkante (Fenster 436-464 x 248-264): Zeilenmittel
    zs2 = {y: round(float(L2[y, 436:465].mean()), 1) for y in range(244, 268)}
    y2 = min(zs2, key=zs2.get)
    band2 = np.zeros_like(L2, bool); band2[y2 - 2:y2 + 3, 436:465] = True
    R['maul_2'] = dict(zeilen_L=zs2, dunkelste_zeile=y2, lab_band=lab_mittel(v2, band2), hell_im_band_px=int((band2 & (L2 > 45)).sum()))
    # Haarbogen Foto 1: dunkles Feld (B)-rechts = (W)-links in der Kopfbox oberhalb der Maulzone (y <= 181), L < thr
    # (entblendet), 4er-Komponente mit dem Saatpunkt (400,165) = Mitte Haarfeld Messblatt V4 Abschnitt 2
    x0, y0, x1, y1 = G.KB_ORIG[1]
    kb = np.zeros_like(L1, bool); kb[y0:182, x0:x1 + 1] = True
    for thr in (14, 16, 18):
        m = (L1 < thr) & kb
        n, lb, st, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), 4)
        k = lb[165, 400]
        if k == 0: continue
        mk = lb == k
        ys, xs = np.nonzero(mk)
        ober = {int(x): int(ys[xs == x].min()) for x in np.unique(xs)}
        R.setdefault('haar_1', {})[thr] = dict(px=int(mk.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
            lab=lab_mittel(v1, mk), oberrand=ober,
            oberrand_kopfbox=[[round((x - x0) / (x1 - x0), 3), round((y - y0) / (y1 - y0), 3)] for x, y in sorted(ober.items())])
        if thr == 16: np.save(os.path.join(S, 'haar_1.npy'), mk)
    # Haare Foto 2: dunkler Streifen an der Vorderkante (L < 40 in x 455-466, y 222-252)
    m = np.zeros_like(L2, bool); m[222:253, 455:467] = True; m &= L2 < 40
    ys, xs = np.nonzero(m)
    R['haar_2'] = dict(px=int(m.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], lab=lab_mittel(v2, m),
                       je_zeile={int(y): [int(xs[ys == y].min()), int(xs[ys == y].max())] for y in np.unique(ys)})
    np.save(os.path.join(S, 'haar_2.npy'), m)
    json.dump(R, open(os.path.join(S, 'kopf_foto.json'), 'w'), indent=1)
    print(json.dumps({k: R[k] for k in ('auge', 'maul_1', 'maul_2')}, indent=0)[:3000])
    print({t: (d['bbox'], d['lab'], d['px']) for t, d in R['haar_1'].items()}, R['haar_2']['bbox'], R['haar_2']['lab'])


if __name__ == '__main__':
    main()
