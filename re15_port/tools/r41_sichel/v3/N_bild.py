# SF Messblatt V3 Nachtrag: Belegbilder x4 (P_nachtrag_*.png) aus N_mess.py - KI h118/h125 gegen Port-Varianten.
# Aufruf: python N_bild.py AUSDIR [VARIANTE ...]   (Default r2)
# Spalten je Zeile: Bild x4 (Ausschnitt Augenmitte -0,07..+0,11 H, x -0,07..+0,10 H) und dasselbe mit Masken:
#   gruen = Augenmaske (Huelle), weiss = heller Kern (L* > Kuppe), gelbe senkrechte Linien = Kopf links/rechts auf Augenhoehe,
#   cyan Kreuz = Augenmitte, orange Linie = Kopfmitte; blau = Haarzone, magenta Punkte = Straehnen (Minima), rot = schwarze
#   Maulflaeche (L* < 0,25 Kuppe), gelb Umriss = geschlossener Maulumriss + Ellipse gleicher Momente (hellblau), gruene Punkte = Zaehne.
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import N_mess as M

aus = sys.argv[1]
var = sys.argv[2:] or ['r2']
F = 4


def schrift(im, s, x, y, sc=0.5, col=(255, 255, 255)):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, col, 1, cv2.LINE_AA)


def kachel(wer, h):
    d, au, ko, ha, ma = M.alles(wer, h)
    H = d['H']; cx, cy = au['mitte']
    xa, xb = int(cx - 0.07 * H), int(cx + 0.10 * H); ya, yb = int(cy - 0.07 * H), int(cy + 0.11 * H)
    im = d['im'][ya:yb, xa:xb].copy()
    roh = cv2.resize(im, None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    ov = im.astype(float)
    x0, y0, m = au['maske']
    mm = np.zeros(d['im'].shape[:2], bool); mm[y0:y0 + m.shape[0], x0:x0 + m.shape[1]] = m
    ov[mm[ya:yb, xa:xb]] = ov[mm[ya:yb, xa:xb]] * 0.4 + np.array([0, 255, 0]) * 0.6
    L = M.lab(d['im'].astype(float))[..., 0]
    kern = mm & (L > au['Lh'])
    ov[kern[ya:yb, xa:xb]] = (255, 255, 255)
    hx0, hy0, sm = ha['maske']
    z = np.zeros(d['im'].shape[:2], bool); z[hy0:hy0 + sm.shape[0], hx0:hx0 + sm.shape[1]] = sm
    ov[z[ya:yb, xa:xb]] = (255, 0, 255)
    gx0, gy0, gm, sw, um = ma['_gm']
    s2 = np.zeros(d['im'].shape[:2], bool); s2[gy0:gy0 + sw.shape[0], gx0:gx0 + sw.shape[1]] = sw
    ov[s2[ya:yb, xa:xb]] = ov[s2[ya:yb, xa:xb]] * 0.3 + np.array([255, 0, 0]) * 0.7
    ov = cv2.resize(np.clip(ov, 0, 255).astype(np.uint8), None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    # Haarzone
    zx0, zy0, zx1, zy1 = ha['zone']
    cv2.rectangle(ov, ((zx0 - xa) * F, (zy0 - ya) * F), ((zx1 - xa) * F, (zy1 - ya) * F), (60, 140, 255), 1)
    # Kopf links/rechts, Kopfmitte, Augenmitte
    for x, c in ((ko['links'], (255, 255, 0)), (ko['rechts'], (255, 255, 0)), ((ko['links'] + ko['rechts']) / 2, (255, 140, 0))):
        X = int((x - xa) * F)
        cv2.line(ov, (X, int((cy - 0.02 * H - ya) * F)), (X, int((cy + 0.02 * H - ya) * F)), c, 2)
    X, Y = int((cx - xa) * F), int((cy - ya) * F)
    cv2.line(ov, (X - 10, Y), (X + 10, Y), (0, 255, 255), 1); cv2.line(ov, (X, Y - 10), (X, Y + 10), (0, 255, 255), 1)
    # Maulumriss + Ellipse
    cs, _ = cv2.findContours(um.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    for c in cs:
        c = ((c + np.array([gx0 - xa, gy0 - ya])) * F + F // 2).astype(np.int32)
        cv2.polylines(ov, [c], True, (255, 255, 0), 1)
    ys, xs = np.nonzero(um)
    if len(ys) > 5:
        cvm = np.cov(np.vstack([xs, ys]).astype(float)) + np.eye(2) / 12.0
        ev, evec = np.linalg.eigh(cvm)
        ang = np.degrees(np.arctan2(evec[1, 0], evec[0, 0]))
        cen = (int((gx0 + xs.mean() - xa) * F + F // 2), int((gy0 + ys.mean() - ya) * F + F // 2))
        cv2.ellipse(ov, cen, (int(2 * np.sqrt(ev[0]) * F), int(2 * np.sqrt(ev[1]) * F)), ang, 0, 360, (120, 200, 255), 1)
    for (zx, zy) in ma['zahn_px']:
        cv2.circle(ov, (int((zx - xa) * F + F // 2), int((zy - ya) * F + F // 2)), 3, (0, 255, 0), 1)
    t = '%s h%d' % ('KI birkin19' if (wer == 'KI' and h == 118) else 'KI birkin20' if wer == 'KI' else 'Port ' + wer, h)
    schrift(roh, t + ' x4', 6, 18)
    schrift(ov, 'lage_kopf %.2f  Auge-Kopfmitte %+.4f H (B)' % (ko['lage_kopf'], ko['auge_zu_kopfmitte_H']), 6, 18, 0.45)
    schrift(ov, 'Kern %d px Lmax %.0f  Straehnen %d  Zone/Kuppe %.2f' % (au['kern_px'], au['kern_Lmax'], ha['straehnen'], ha['L_zone_kuppe']), 6, 36, 0.45)
    schrift(ov, 'Maul Umriss-IoU %.2f  Spalt %.3f H  Oberlinie %.4f H' % (ma['umriss_iou'], ma['spalt_median_H'],
                                                                       (ma['ecken'] or {}).get('oberlinie_rest_std_H', 0)), 6, ov.shape[0] - 10, 0.45)
    return roh, ov


zeilen = []
for h in (118, 125):
    teile = []
    for w in ['KI'] + var:
        r, o = kachel(w, h)
        teile += [r, o]
    hh = min(t.shape[0] for t in teile)
    zeilen.append(np.hstack([t[:hh] for t in teile]))
ww = min(z.shape[1] for z in zeilen)
bild = np.vstack([z[:, :ww] for z in zeilen])
os.makedirs(aus, exist_ok=True)
p = os.path.join(aus, 'P_nachtrag_kopf_masken_x4.png')
cv2.imwrite(p, cv2.cvtColor(bild, cv2.COLOR_RGB2BGR))
print(p, bild.shape)
