"""Pruefer Zusatz: A-Variante Farben je Teil (globale Palette je Teil gezaehlt), Atlas-Klassen, Paletten, Schwanz je Glied, Augenpunkte Bild 2."""
import json
import numpy as np, cv2
from p_lade import lab
from p_messen import lade_alle, zonen_von, km
from p_zonen import rueck, klassen_p, FORM

M = lade_alle(); Z = zonen_von(M)
out = {}
for zn in ('EM30_haut', 'EM30_koerper', 'G4_koerper', 'G5_koerper', 'Zombie_koerper', 'Hund_koerper', 'K5c_koerper', 'K5c_gesamt_ohne_zahn_auge'):
    nm, z, _ = Z[zn]; rgb, mid = M[nm][3], M[nm][4]
    L = lab(rgb[z]); c, a, lb = km(L, 8)
    ids = mid[z]; cnt = []
    for b in np.unique(ids):
        s = ids == b
        if s.sum() < 200:
            continue
        n = np.bincount(lb[s], minlength=8) / s.sum(); cnt.append(int((n > 0.05).sum()))
    o = np.argsort(c[:, 0])
    out[zn] = dict(farben_je_teil_globalpalette=round(float(np.mean(cnt)), 2), klassen_p=klassen_p(L),
                   palette_nach_L=[[round(float(x), 1) for x in c[i]] + [round(float(a[i]), 3)] for i in o])
    print(zn, out[zn])
# Schwanz je Glied K5c (Mesh 1-6, nur Seiten 0-2)
rgb, mid = M['K5c'][3], M['K5c'][4]
xs = np.arange(mid.shape[1])[None, :] < 384
out['schwanz_L'] = [round(float(lab(rgb[(mid == b) & xs])[:, 0].mean()), 1) for b in range(1, 7)]
print('Schwanz L* je Glied', out['schwanz_L'])
# Augenpunkte Bild 2 (E24: rechtes Auge (W) (223/271), linkes Auge (W) (288/269)); Scheibe r=4 px
v = cv2.cvtColor(cv2.imread(FORM + '/bild2.png'), cv2.COLOR_BGR2RGB); T, clip = rueck(v, 2)
yy, xx = np.mgrid[:480, :640]
for nm, (x, y) in (('auge_r_W', (223, 271)), ('auge_l_W', (288, 269))):
    m = (xx - x) ** 2 + (yy - y) ** 2 <= 16
    L = lab(np.clip(T[m], 0, 255))
    out[nm] = dict(lab=[round(float(q), 1) for q in L.mean(0)], L_max=round(float(L[:, 0].max()), 1), video_rgb=[int(q) for q in v[m].mean(0)])
    print(nm, out[nm])
# Umgebung der Augen (Ring 6..12 px) zum Vergleich
for nm, (x, y) in (('ring_r_W', (223, 271)), ('ring_l_W', (288, 269))):
    d2 = (xx - x) ** 2 + (yy - y) ** 2; m = (d2 >= 36) & (d2 <= 144)
    L = lab(np.clip(T[m], 0, 255)); out[nm] = [round(float(q), 1) for q in L.mean(0)]; print(nm, out[nm])
json.dump(out, open('p_extra.json', 'w'), indent=1)
