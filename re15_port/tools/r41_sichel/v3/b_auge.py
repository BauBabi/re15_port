# Leser B v3: Augen der Sichel-Form im KI-Video 4 _slp (1280x960), Masse A1-A5 (+A6 Schulterauge).
# Auge = dunkle Flaeche L* < SCHWELLE (= FAKTOR 0,6 x Haut-L* im Ring 2..10 px ausserhalb des Suchfensters)
#   im Suchfenster, alle Teile >= 3 px; "Augenkreis" = gefuellte konvexe Huelle der dunklen Flaeche + eingeschlossene helle Pixel.
# Kern (Pupille) = L* < 15 in der Huelle; Glanz = L* > 60 in der Huelle; Lidrand = Huelle 2 px dilatiert minus Huelle;
# Haut = Ring 4..10 px ausserhalb der Huelle, ohne L* < 25 (Falten) - ehrliche Umgebung.
# Weissanteil = Anteil L* > 80 im Augenkreis; zusaetzlich Anteil L* > Haut-L* + 15 (relativ hell).
# Lab = OpenCV float Lab (sRGB, D65).
# Aufruf: python b_auge.py <bild.png> <name> x0:y0:x1:y1 [weitere Fenster ...] -> JSON-Zeilen; optional env OUT=<png> fuer Lupe
import sys, os, json, cv2, numpy as np
im = cv2.imread(sys.argv[1]); name = sys.argv[2]
FAKTOR = float(os.environ.get("FAKTOR", "0.6"))
f = cv2.cvtColor(im.astype(np.float32) / 255, cv2.COLOR_BGR2Lab); L = f[..., 0]


def st(sel):
    if sel.sum() == 0:
        return None
    return dict(rgb=[round(float(v), 1) for v in im[sel][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[sel].mean(0)], px=int(sel.sum()))


for win in sys.argv[3:]:
    x0, y0, x1, y1 = map(int, win.split(":"))
    cx, cy = (x0 + x1) // 2, (y0 + y1) // 2
    yy, xx = np.mgrid[0:L.shape[0], 0:L.shape[1]]
    r = np.hypot(xx - cx, yy - cy)
    sub = np.zeros(L.shape, bool); sub[y0:y1, x0:x1] = True
    haut0 = L[(r >= max(x1 - x0, y1 - y0) / 2 + 2) & (r <= max(x1 - x0, y1 - y0) / 2 + 10)].mean()
    thr = FAKTOR * haut0
    dark = ((L < thr) & sub).astype(np.uint8)
    k, l, s, _ = cv2.connectedComponentsWithStats(dark, connectivity=8)
    if k < 2:
        print(name, win, "kein dunkler Fleck"); continue
    comp = np.isin(l, [j for j in range(1, k) if s[j, 4] >= 3]).astype(np.uint8)  # alle dunklen Teile >= 3 px (Glanz teilt das Auge)
    pts = cv2.findNonZero(comp)
    hull = cv2.convexHull(pts); eye = np.zeros_like(comp); cv2.fillPoly(eye, [hull], 1)
    eye = eye.astype(bool)
    bx, by, bw, bh = cv2.boundingRect(hull)
    dil2 = cv2.dilate(eye.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
    lid = dil2 & ~eye
    d4 = cv2.dilate(eye.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool)
    d10 = cv2.dilate(eye.astype(np.uint8), np.ones((21, 21), np.uint8)).astype(bool)
    haut = d10 & ~d4 & (L >= 25)
    kern = eye & (L < 15); glanz = eye & (L > 60)
    gy, gx = np.where(glanz)
    hautL = float(L[haut].mean())
    res = dict(bild=name, fenster=[x0, y0, x1, y1], schwelle_L=round(thr, 1), box=[bx, by, bw, bh],
               mitte=[round(bx + bw / 2, 1), round(by + bh / 2, 1)], flaeche_px=int(eye.sum()),
               aequiv_durchmesser_px=round(float(2 * np.sqrt(eye.sum() / np.pi)), 1),
               auge=st(eye), kern_pupille=st(kern), lidrand=st(lid), haut=st(haut),
               glanz=dict(px=int(glanz.sum()), mitte=[round(float(gx.mean()), 1), round(float(gy.mean()), 1)] if len(gx) else None,
                          L_max=round(float(L[eye].max()), 1)),
               weiss_anteil_L80=round(float((L[eye] > 80).mean()), 3),
               hell_anteil_haut_plus15=round(float((L[eye] > hautL + 15).mean()), 3),
               helligkeit_auge_zu_haut=round(float(L[eye].mean() / hautL), 3))
    print(json.dumps(res))
    if os.environ.get("OUT"):
        pad = 14; c = im[by - pad:by + bh + pad, bx - pad:bx + bw + pad].copy()
        c = cv2.resize(c, None, fx=10, fy=10, interpolation=cv2.INTER_NEAREST)
        cc, _ = cv2.findContours(eye.astype(np.uint8)[by - pad:by + bh + pad, bx - pad:bx + bw + pad], cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cc = [ (q * 10 + 5).astype(np.int32) for q in cc]
        cv2.drawContours(c, cc, -1, (0, 255, 0), 1)
        for (px, py) in zip(gx, gy):
            cv2.rectangle(c, ((px - bx + pad) * 10, (py - by + pad) * 10), ((px - bx + pad) * 10 + 9, (py - by + pad) * 10 + 9), (0, 255, 255), 1)
        cv2.putText(c, f"{name} Auge x{bx}-{bx + bw - 1} y{by}-{by + bh - 1}", (3, 12), 0, 0.4, (0, 255, 255), 1)
        o = os.environ["OUT"]; base, ext = os.path.splitext(o)
        cv2.imwrite(f"{base}_{bx}_{by}{ext}", c)
