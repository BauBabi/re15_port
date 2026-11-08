# SF Messblatt V3 Nachtrag U3: Belegbild "Haare" (P_nachtrag_haare_x4.png). Je Zeile (h118 / h125):
#   KI-Videobild (= Freisteller birkin19/20, bitgleich) x4 | dasselbe mit Straehnen (magenta), Haarzone (blau), Auge (gruen),
#   Kopf links/rechts (gelb) | Original-Video n1468/n1475 (640er, Schwarzwert 17 ab) x8 = gleicher Massstab, Haarzone blau |
#   Port V3 r2 x4 mit Haarzone. Ausschnitt: Augenmitte x -0,05..+0,11 H, y -0,07..+0,05 H.
# Aufruf: python N_haarbild.py AUSDIR
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import N_mess as M
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/'
aus = sys.argv[1]
F = 4
NVID = {118: 1468, 125: 1475}


def schrift(im, s, x, y, sc=0.45):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (255, 255, 255), 1, cv2.LINE_AA)


def box_von(au, H):
    cx, cy = au['mitte']
    return int(cx - 0.05 * H), int(cx + 0.11 * H), int(cy - 0.07 * H), int(cy + 0.05 * H)


def markiert(d, au, ko, ha, xa, xb, ya, yb, staerke=True):
    im = d['im'][ya:yb, xa:xb].astype(float)
    if staerke:
        hx0, hy0, sm = ha['maske']
        z = np.zeros(d['im'].shape[:2], bool); z[hy0:hy0 + sm.shape[0], hx0:hx0 + sm.shape[1]] = sm
        im[z[ya:yb, xa:xb]] = (255, 0, 255)
        x0, y0, m = au['maske']
        mm = np.zeros(d['im'].shape[:2], bool); mm[y0:y0 + m.shape[0], x0:x0 + m.shape[1]] = m
        im[mm[ya:yb, xa:xb]] = im[mm[ya:yb, xa:xb]] * 0.4 + np.array([0, 255, 0]) * 0.6
    im = cv2.resize(np.clip(im, 0, 255).astype(np.uint8), None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    zx0, zy0, zx1, zy1 = ha['zone']
    cv2.rectangle(im, ((zx0 - xa) * F, (zy0 - ya) * F), ((zx1 - xa) * F, (zy1 - ya) * F), (60, 140, 255), 2)
    cy = au['mitte'][1]
    for x in (ko['links'], ko['rechts']):
        X = int((x - xa) * F)
        cv2.line(im, (X, int((cy - 0.02 * d['H'] - ya) * F)), (X, int((cy + 0.02 * d['H'] - ya) * F)), (255, 255, 0), 2)
    return im


zeilen = []
for h in (118, 125):
    d, au, ko, ha, ma = M.alles('KI', h)
    H = d['H']; xa, xb, ya, yb = box_von(au, H)
    roh = cv2.resize(d['im'][ya:yb, xa:xb], None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    schrift(roh, 'KI n%d (= birkin%d) h%d x4' % (NVID[h], 19 if h == 118 else 20, h), 6, 18)
    mk = markiert(d, au, ko, ha, xa, xb, ya, yb)
    schrift(mk, 'Straehnen %d, L %s H' % (ha['straehnen'], '/'.join('%.3f' % v for v in ha['laengen_H'])), 6, 18)
    schrift(mk, 'Zone/Kuppe %.2f; Straehne L* %.1f, dazwischen %.1f' % (ha['L_zone_kuppe'], ha['lab_straehne'][0], ha['lab_zwischen'][0]), 6, 38)
    o = cv2.cvtColor(cv2.imread(SC + 'orig/n%d.png' % NVID[h]), cv2.COLOR_BGR2RGB).astype(float)
    o = np.clip((o - 17) * 255.0 / 238.0, 0, 255).astype(np.uint8)
    oc = o[ya // 2:yb // 2, xa // 2:xb // 2]
    oc = cv2.resize(oc, None, fx=2 * F, fy=2 * F, interpolation=cv2.INTER_NEAREST)
    zx0, zy0, zx1, zy1 = ha['zone']
    cv2.rectangle(oc, ((zx0 // 2 - xa // 2) * 2 * F, (zy0 // 2 - ya // 2) * 2 * F), ((zx1 // 2 - xa // 2) * 2 * F, (zy1 // 2 - ya // 2) * 2 * F), (60, 140, 255), 2)
    schrift(oc, 'Original n%d x8 (640er, Schwarzwert 17 ab)' % NVID[h], 6, 18)
    d2, au2, ko2, ha2, ma2 = M.alles('r2', h)
    H2 = d2['H']; xa2, xb2, ya2, yb2 = box_von(au2, H2)
    pk = markiert(d2, au2, ko2, ha2, xa2, xb2, ya2, yb2, staerke=False)
    schrift(pk, 'Port V3 r2 h%d x4: Zone/Kuppe %.2f, Straehnen %d' % (h, ha2['L_zone_kuppe'], ha2['straehnen']), 6, 18)
    teile = [roh, mk, oc, pk]
    hh = min(t.shape[0] for t in teile); ww = min(t.shape[1] for t in teile)
    zeilen.append(np.hstack([t[:hh, :ww] for t in teile]))
ww = min(z.shape[1] for z in zeilen)
bild = np.vstack([z[:, :ww] for z in zeilen])
os.makedirs(aus, exist_ok=True)
p = os.path.join(aus, 'P_nachtrag_haare_x4.png')
cv2.imwrite(p, cv2.cvtColor(bild, cv2.COLOR_RGB2BGR))
print(p, bild.shape)
