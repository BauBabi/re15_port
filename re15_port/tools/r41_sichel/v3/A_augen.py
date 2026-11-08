# Leser A V3: Augen- und Maulmessung an den KI-Freistellern birkinNN.png
# Augenmaske = Zusammenhangsgebiet L* < Lmin + 0,5*(Haut-Median - Lmin) um den Startpunkt, Loecher gefuellt.
# Startpunkte (x,y im Freisteller) per Lupe gesetzt (A_lk_sheet.png, A_lseite.png); die Grenzen misst das Skript.
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from scipy import ndimage as ndi

KOPF = {17: (190, 24), 18: (174, 22), 19: (184, 42), 20: (206, 43), 21: (190, 54), 22: (206, 43), 23: (205, 46),
        24: (195, 62), 16: (220, 46), 15: (255, 75), 11: (373, 121), 12: (374, 123), 13: (350, 114), 14: (355, 97)}
SCHULTER = {17: (131, 68), 18: (116, 67), 19: (119, 70), 20: (132, 76), 22: (127, 81), 23: (134, 89), 24: (119, 112),
            15: (183, 104), 16: (148, 87)}

def auge(nn, c, tag):
    rgb, al = load(nn)
    L3 = lab(rgb); L = L3[..., 0]
    cx, cy = c
    x0, x1, y0, y1 = cx - 16, cx + 17, cy - 12, cy + 13
    pad = np.zeros((L.shape[0] + 40, L.shape[1] + 40), bool); pad[20:-20, 20:-20] = al
    if x1 > L.shape[1] or y0 < 0 or x0 < 0:
        sh = 40
        rgb = np.pad(rgb, ((20, 20), (20, 20), (0, 0))); al = pad; L3 = lab(rgb); L = L3[..., 0]
        x0 += 20; x1 += 20; y0 += 20; y1 += 20; cx += 20; cy += 20
        off = 20
    else:
        off = 0
    win = L[y0:y1, x0:x1]
    wal = erode(al, 1)[y0:y1, x0:x1]
    # dunkelstes Pixel im Kern +-4 px um den Startpunkt
    core = win[8:17, 12:21]
    iy, ix = np.unravel_index(np.argmin(core), core.shape)
    sy, sx = iy + 8, ix + 12
    Lmin = float(win[sy, sx])
    # Haut: Ring 9..14 px um den Startpunkt
    yy, xx = np.mgrid[y0:y1, x0:x1]
    r = np.hypot(xx - cx, (yy - cy) * 1.0)
    ring = (r >= 9) & (r <= 14) & wal
    Lskin = float(np.percentile(win[ring], 75))  # 75-%-Quantil: Rillen im Ring druecken den Median
    thr = Lmin + 0.5 * (Lskin - Lmin)
    dark = (win < thr) & wal
    lbl, n = ndi.label(dark)
    comp = lbl == lbl[sy, sx]
    filled = ndi.binary_fill_holes(comp)
    # helle Kerne, die nur an einer Seite offen sind: konvexe Huelle der Komponente
    hull = cv2.convexHull(np.argwhere(comp)[:, ::-1].astype(np.int32))
    hm = np.zeros_like(win, np.uint8); cv2.fillConvexPoly(hm, hull, 1)
    eye = hm.astype(bool) | filled
    kern = eye & ~comp & (win > Lskin)
    iris = eye & ~pup & ~kern if False else None
    pup = eye & (win <= Lmin + 0.25 * (Lskin - Lmin))
    iris = eye & ~(win <= Lmin + 0.25 * (Lskin - Lmin)) & ~kern
    lid = ndi.binary_dilation(eye, iterations=1) & ~eye & wal
    haut = ndi.binary_dilation(eye, iterations=7) & ~ndi.binary_dilation(eye, iterations=3) & wal
    ys, xs = np.nonzero(eye)
    sub = rgb[y0:y1, x0:x1]; sl = L3[y0:y1, x0:x1]
    x0 -= off; y0 -= off; x1 -= off; y1 -= off
    res = dict(bild='birkin%02d' % nn, h=MAP[nn][1], art=tag, start=list(c),
               bbox=[int(xs.min() + x0), int(ys.min() + y0), int(xs.max() + x0), int(ys.max() + y0)],
               breite=int(xs.ptp() + 1), hoehe=int(ys.ptp() + 1), flaeche=int(eye.sum()),
               mitte=[round(float(xs.mean() + x0), 1), round(float(ys.mean() + y0), 1)],
               Lmin=round(Lmin, 1), Lhaut_p75=round(Lskin, 1), schwelle=round(thr, 1),
               auge=stats(sub, sl, eye), dunkel=stats(sub, sl, comp), pupille=stats(sub, sl, pup), zwischen=stats(sub, sl, iris), kern=stats(sub, sl, kern),
               lidrand=stats(sub, sl, lid), haut=stats(sub, sl, haut),
               weiss_L80=int((eye & (win > 80)).sum()), weiss_anteil=round(float((eye & (win > 80)).sum() / eye.sum()), 3),
               hell_L60=int((eye & (win > 60)).sum()))
    if res['kern']:
        ky, kx = np.nonzero(kern)
        res['kern_bbox'] = [int(kx.min() + x0), int(ky.min() + y0), int(kx.max() + x0), int(ky.max() + y0)]
        res['kern_mitte_rel'] = [round(float((kx.mean() - xs.min()) / max(xs.ptp(), 1)), 2), round(float((ky.mean() - ys.min()) / max(ys.ptp(), 1)), 2)]
    res['L_auge_durch_haut'] = round(res['auge']['lab'][0] / res['haut']['lab'][0], 3)
    masks = dict(eye=eye, comp=comp, kern=kern, pup=pup, win=(x0, y0, x1, y1))
    return res, masks

def maul(nn, c):
    # Maul = Huelle der Zahnflecken (L*>50, Chroma<25, 1-8 px) im Fenster x cx-28..cx+32, y cy+12..cy+42;
    # Innenraum = Pixel L*<18 innerhalb dieser Huelle.
    rgb, al = load(nn)
    L3 = lab(rgb); L = L3[..., 0]; A = L3[..., 1]
    cx, cy = c
    x0, x1, y0, y1 = max(cx - 28, 0), min(cx + 36, L.shape[1]), cy + 12, cy + 50
    win = L[y0:y1, x0:x1]
    wal = erode(al, 1)[y0:y1, x0:x1]
    C = np.hypot(L3[y0:y1, x0:x1, 1], L3[y0:y1, x0:x1, 2])
    cand = (win > 50) & (C < 25) & wal
    tl, tn = ndi.label(cand, structure=np.ones((3, 3)))
    keep = np.zeros_like(cand)
    for i in range(1, tn + 1):
        a = (tl == i).sum()
        if 1 <= a <= 8:
            keep |= tl == i
    tl, tn = ndi.label(keep, structure=np.ones((3, 3)))
    if tn < 2:
        return None, None
    pts = np.argwhere(keep)[:, ::-1].astype(np.int32)
    hull = cv2.convexHull(pts)
    hm = np.zeros_like(win, np.uint8); cv2.fillConvexPoly(hm, hull, 1); hm = hm.astype(bool)
    ys, xs = np.nonzero(hm)
    bx0, bx1, by0, by1 = xs.min(), xs.max(), ys.min(), ys.max()
    mid = (by0 + by1) / 2
    oben = unten = 0; groessen = []
    for i in range(1, tn + 1):
        ty, tx = np.nonzero(tl == i)
        groessen.append([int(tx.ptp() + 1), int(ty.ptp() + 1)])
        if ty.mean() < mid: oben += 1
        else: unten += 1
    innen = hm & (win < 18) & wal
    rot = hm & (A[y0:y1, x0:x1] > 15) & (win >= 18) & ~keep & wal
    sub = rgb[y0:y1, x0:x1]; sl = L3[y0:y1, x0:x1]
    res = dict(fenster=[int(x0), int(y0), int(x1 - 1), int(y1 - 1)], bbox=[int(bx0 + x0), int(by0 + y0), int(bx1 + x0), int(by1 + y0)],
               breite=int(bx1 - bx0 + 1), hoehe=int(by1 - by0 + 1),
               mitte=[round(float((bx0 + bx1) / 2 + x0), 1), round(float((by0 + by1) / 2 + y0), 1)],
               innen_dunkel=stats(sub, sl, innen), innen_anteil_dunkel=round(float(innen.sum() / hm.sum()), 2),
               rot_im_maul=stats(sub, sl, rot), zaehne=stats(sub, sl, keep),
               zaehne_oben=oben, zaehne_unten=unten, zahn_groessen=groessen)
    return res, dict(hull=hm, teeth=keep, win=(x0, y0, x1, y1))

if __name__ == '__main__':
    out = {}
    for nn, c in KOPF.items():
        r, mk = auge(nn, c, 'kopf')
        rm, mm = maul(nn, c) if nn not in (11, 12, 13, 14) else (None, None)
        r['maul'] = rm
        out['k%02d' % nn] = r
        np.save(SCR + 'A_mask_k%02d.npy' % nn, np.stack([mk['eye'], mk['comp'], mk['kern'], mk['pup']]))
        r['_win'] = [int(v) for v in mk['win']] if 'win' in mk else None
        print('birkin%02d h%d Kopf-Auge bbox %s %dx%d A%d Lmin %.0f Lhaut %.0f weiss %d kern %s' % (
            nn, r['h'], r['bbox'], r['breite'], r['hoehe'], r['flaeche'], r['Lmin'], r['Lhaut_p75'], r['weiss_L80'], r.get('kern_bbox')))
        if rm:
            print('     Maul bbox %s %dx%d Zaehne o/u %d/%d' % (rm['bbox'], rm['breite'], rm['hoehe'], rm['zaehne_oben'], rm['zaehne_unten']))
    for nn, c in SCHULTER.items():
        r, mk = auge(nn, c, 'schulter')
        out['s%02d' % nn] = r
        print('birkin%02d h%d Schulter-Auge bbox %s %dx%d A%d Lmin %.0f Lhaut %.0f weiss %d kern %s' % (
            nn, r['h'], r['bbox'], r['breite'], r['hoehe'], r['flaeche'], r['Lmin'], r['Lhaut_p75'], r['weiss_L80'], r.get('kern_bbox')))
    json.dump(out, open(SCR + 'A_augen.json', 'w'), indent=1)
