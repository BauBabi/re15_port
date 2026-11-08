# Leser A V3: dieselben Masse wie an der KI (S1/S2/S4, A1/A4) am Port-Ist V1 (EM30_V1_SPITZ.EMD).
# Quelle: A_port_v1.py (Spielkamera 50A0/12, RDT-Licht, Knochen-Kennung je Bildpunkt; Schwanz = Knochen 16-22).
# H = Oberkante bis Sohle der Figur ohne Schwanz (Knochen 0-15). Haut = wie A_schwanz.koerpermasken.
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from A_schwanz import mittellinie, form, muster, farben, koerpermasken, groesste
from A_augen import auge
from scipy import ndimage as ndi
import cv2

out = {}
for h in (83, 85, 92):
    rgb = cv2.cvtColor(cv2.imread(SCR + 'A_v1_seite_h%d.png' % h), cv2.COLOR_BGR2RGB)
    ids = np.load(SCR + 'A_v1_seite_h%d_ids.npy' % h)
    al = ids >= 0
    m = groesste((ids >= 16) & (ids <= 22))
    body = al & ~m
    ys, xs = np.nonzero(body)
    top, sole = ys.min(), ys.max()
    H = int(sole - top + 1)
    # Wurzelhinweis: projiziertes Gelenk 16 liegt am Buckel; Schwanzpunkt mit kleinstem Abstand zur Oberkante-Mitte
    tx = xs[ys <= top + 2].mean()
    my, mx = np.nonzero(m)
    i = np.argmin(np.hypot(mx - tx, my - top))
    P = mittellinie(m, np.array([mx[i], my[i]], float))
    haut, jeans = koerpermasken(rgb, body, np.zeros_like(m), H)
    haut &= ~ndi.binary_dilation(m, iterations=4)
    r = dict(H=H, form=form(m, P, H), muster=muster(rgb, m, P, H), farbe=farben(rgb, m, dict(haut=haut, jeans=jeans)))
    f = r['farbe']
    r['dE00_schwanz_haut'] = round(de2000(f['schwanz']['lab'], f['haut']['lab']), 1)
    r['dE76_schwanz_haut'] = round(de76(f['schwanz']['lab'], f['haut']['lab']), 1)
    r['wurzel'] = [round(float(v), 1) for v in P[0]]
    r['spitze'] = [round(float(v), 1) for v in P[-1]]
    np.save(SCR + 'A_v1_schwanz_P%d.npy' % h, P)
    out['v1_seite_h%d' % h] = r
    fo, mu = r['form'], r['muster']
    print('V1 h%d H%d Laenge %.2fH Dicke %s Knicke %d max %.0f Grad/0,05H bei %.2f %s' % (h, H, fo['laenge_H'], fo['dicke_H'], fo['knicke_gt20'], fo['max_dwinkel_005H'], fo['max_bei'], fo['knick_pos']))
    print('    Lab S %s Haut %s Jeans %s | rot a15 S %.2f Haut %.2f | L* S/Haut %.2f | dE00 %.1f dE76 %.1f | hue S %s Haut %s' % (
        f['schwanz']['lab'], f['haut']['lab'], f['jeans']['lab'] if f.get('jeans') else None, f['schwanz_rot_a15'], f['haut_rot_a15'],
        f['schwanz']['lab'][0] / f['haut']['lab'][0], r['dE00_schwanz_haut'], r['dE76_schwanz_haut'], f['schwanz_hue'], f['haut_hue']))
    print('    Muster AK a %s lag %s H std %s | AK L %s lag %s | a Haelften %s/%s' % (mu['ak_a_max'], mu['ak_a_lag_H'], mu['a_rest_std'], mu['ak_L_max'], mu['ak_L_lag_px'], mu['a_haelfte_neg'], mu['a_haelfte_pos']))
    print('    a*prof', mu['a_profil_21'])
    print('    L*prof', mu['L_profil_21'])
    print('    Breiten', fo['breiten_profil'])
    print('    Winkel ', fo['winkel_profil'])

# Augen V1: Kopf vorn nah (960x960, neutrales Licht) - Startpunkte per Sicht auf A_v1_kopfnah_crop.png (Kopfauge (432,438), Schulterauge (88,485))
rgb = cv2.cvtColor(cv2.imread(SCR + 'A_v1_kopfnah.png'), cv2.COLOR_BGR2RGB)
L3 = lab(rgb)
for name, c in (('kopf', (432, 438)), ('schulter', (88, 485))):
    x0, y0, x1, y1 = c[0] - 40, c[1] - 40, c[0] + 40, c[1] + 40
    sub = L3[y0:y1, x0:x1, 0]
    yy, xx = np.mgrid[y0:y1, x0:x1]
    r_ = np.hypot(xx - c[0], yy - c[1])
    # Ringauge: Kreisscheibe = helle Ringpixel L* > 80 plus Inneres; Huelle der hellen Pixel nahe dem Start
    hautring = (r_ >= 34) & (r_ <= 40)
    Lh = float(np.percentile(sub[hautring], 75))
    hell = (sub > Lh + 8) & (r_ < 26)
    lbl, n = ndi.label(hell)
    keep = np.zeros_like(hell)
    for i in range(1, n + 1):
        cc = lbl == i
        if r_[cc].min() < 30:
            keep |= cc
    pts = np.argwhere(keep)[:, ::-1].astype(np.int32)
    hull = cv2.convexHull(pts)
    hm = np.zeros_like(sub, np.uint8)
    cv2.fillConvexPoly(hm, hull, 1)
    hm = ndi.binary_dilation(hm.astype(bool), iterations=4)   # dunkler Aussenrand (ein Texel, ~5 px) gehoert zum Auge
    ring_haut = ndi.binary_dilation(hm, iterations=14) & ~ndi.binary_dilation(hm, iterations=6)
    dunkel = hm & (sub < 30)
    ys, xs = np.nonzero(hm)
    subrgb = rgb[y0:y1, x0:x1]
    sl = L3[y0:y1, x0:x1]
    res = dict(bbox=[int(xs.min() + x0), int(ys.min() + y0), int(xs.max() + x0), int(ys.max() + y0)], breite=int(xs.ptp() + 1), hoehe=int(ys.ptp() + 1),
               auge=stats(subrgb, sl, hm), hell_ring=stats(subrgb, sl, keep), dunkel_innen=stats(subrgb, sl, dunkel), haut=stats(subrgb, sl, ring_haut),
               weiss_anteil=round(float((hm & (sub > 80)).sum() / hm.sum()), 3), hell_anteil=round(float((hm & hell).sum() / hm.sum()), 3), Lhaut_p75=round(Lh, 1))
    res['L_auge_durch_haut'] = round(res['auge']['lab'][0] / res['haut']['lab'][0], 3)
    out['v1_auge_' + name] = res
    print('V1 Auge', name, res['bbox'], '%dx%d' % (res['breite'], res['hoehe']), 'weiss', res['weiss_anteil'], 'hell>Haut+8', res['hell_anteil'], 'Lab Auge', res['auge']['lab'], 'Ring', res['hell_ring']['lab'],
          'innen', res['dunkel_innen']['lab'] if res['dunkel_innen'] else None, 'Haut', res['haut']['lab'], 'L Auge/Haut', res['L_auge_durch_haut'])
json.dump(out, open(SCR + 'A_port_v1.json', 'w'), indent=1, default=lambda o: o.item() if hasattr(o, 'item') else str(o))
