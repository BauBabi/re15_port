"""F1-Flaeche wie Sicht-Pruefer V3 r2: gefiltertes Bild 1 (Saat 15), Video-Lab L* >= 30 und a* >= 20, 8-zusammenhaengend mit dem a*-Maximum
der Box x 300-345, y 225-265 (Messblatt V3 3.1). python f1_flaeche.py <Variante|orig>"""
import sys, os, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV
import p_zonen as PZ
def flaeche(vid):
    L = cv2.cvtColor(np.asarray(vid, np.float32) / 255.0, cv2.COLOR_RGB2LAB)
    m = (L[..., 0] >= 30) & (L[..., 1] >= 20)
    box = np.zeros(m.shape, bool); box[225:266, 300:346] = True
    a = np.where(box & m, L[..., 1], -1e9)
    y, x = np.unravel_index(np.argmax(a), a.shape)
    n, lab_ = cv2.connectedComponents(m.astype(np.uint8), connectivity=8)
    c = lab_ == lab_[y, x]
    ys, xs = np.nonzero(c)
    # Form: Hauptachsen (4 sigma)
    P = np.stack([xs, ys], 1).astype(float); C = np.cov((P - P.mean(0)).T); w, E = np.linalg.eigh(C)
    ang = np.degrees(np.arctan2(E[1, 1], E[0, 1])) % 180
    return c, dict(px=int(c.sum()), bbox=(int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())), schwerpunkt=(round(xs.mean(), 1), round(ys.mean(), 1)),
                   laenge_breite=(round(4 * np.sqrt(w[1]), 1), round(4 * np.sqrt(w[0]), 1)), achse=round(float(ang), 0))
if __name__ == '__main__':
    v = sys.argv[1]
    if v == 'orig':
        vid = PZ.P.original(1)
    else:
        k = PZ.P.Kandidat(os.path.join(v, 'form.json'))
        s, alb, m, kk = k.rendern(1)
        vid = PZ.FV.video_filter(s, bild=1, entzerren=False, rauschen=True, saat=15)
    c, d = flaeche(vid)
    o, do = flaeche(PZ.P.original(1))
    d['iou_original'] = round(float((c & o).sum() / (c | o).sum()), 2)
    print(d)
