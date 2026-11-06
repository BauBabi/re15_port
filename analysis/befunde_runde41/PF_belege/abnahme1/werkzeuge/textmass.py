"""Je Panel-Text: Groesse der Schrift im logischen Bild (Texel) und im Fenster, Strichbreite, Zwischentoene, Skalierung (Bloecke einfarbig).
Tinte = Bildpunkte, deren Abstand zum Schildgrund (Median der Schildflaeche) > 50 % des groessten Abstands ist; 'Zwischentoene' = Anteil der
Bildpunkte mit 20..80 % des Kontrasts unter allen mit > 20 % (Kantenunschaerfe). Hoehe = Zeilen mit Tinte (Versalhoehe inkl. Unterlaengen)."""
import sys, numpy as np, logisch
SCHILDER = {   # logische Rechtecke (beide Staende), x0, y0, x1, y1
    'A 100': (50, 131, 84, 143), 'B 50': (134, 131, 161, 143), 'C 30': (178, 131, 205, 143), '80 links': (90, 70, 107, 82),
    'BRIDGE': (214, 49, 268, 61), 'PRESSURE 80 / SET 80': (214, 66, 268, 83), 'CAUTION': (124, 183, 228, 194), 'HIGH PRESSURE': (124, 193, 228, 205),
    'Ziffern im Zeiger-Instrument': (112, 75, 136, 99)}
def mass(p):
    a, s, ein = logisch.lade(p)
    g = a.astype(float)
    print('%s: Fenster %dx%d, Faktor %d, 3x3-Bloecke einfarbig %.4f' % (p.split('/')[-1], 320 * s, 240 * s, s, ein))
    for n, (x0, y0, x1, y1) in SCHILDER.items():
        z = g[y0:y1, x0:x1]
        grund = np.median(z.reshape(-1, 3), axis=0)
        d = np.abs(z - grund).max(axis=2)
        k = d.max() if d.max() > 0 else 1
        tinte = d > .5 * k
        zw = ((d > .2 * k) & (d < .8 * k)).sum() / max(1, (d > .2 * k).sum())
        ys = np.where(tinte.any(axis=1))[0]; xs = np.where(tinte.any(axis=0))[0]
        h = (ys.max() - ys.min() + 1) if len(ys) else 0; w = (xs.max() - xs.min() + 1) if len(xs) else 0
        # Strichbreite: Median der waagrechten Tintenlaeufe
        laeufe = []
        for row in tinte:
            x = 0
            while x < len(row):
                if row[x]:
                    e = x
                    while e < len(row) and row[e]: e += 1
                    laeufe.append(e - x); x = e
                else: x += 1
        sb = float(np.median(laeufe)) if laeufe else 0
        print('  %-30s Tinte %2dx%2d Texel -> %3dx%3d Fensterpunkte | Strich %.1f Texel = %.0f Fensterpunkte | Kontrast %3d | Zwischentoene %4.1f %%' % (
            n, w, h, w * s, h * s, sb, sb * s, k, 100 * zw))
for p in sys.argv[1:]: mass(p)
