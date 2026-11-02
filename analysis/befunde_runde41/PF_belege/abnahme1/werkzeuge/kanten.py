"""Mass 2a (Fensteraufloesung): Kanten der gezeichneten Schrankflaechen gegen die Projektion der Port-Geometrie (ROOM2010 Cut 3 @0xC0, H = 26684>>7).
Maske = |Fenster - BG03 (nearest auf Fenstergroesse)| > 24. Je Kante: in jeder Zeile/Spalte der erste Maskenpunkt von aussen in einem Fenster +-5 logischen
Bildpunkten um die projizierte Kante; Abstand in logischen Bildpunkten. Zeilen mit Figur (Maske ausserhalb der Projektion daneben) entfallen."""
import sys, numpy as np, geo
from PIL import Image
a = np.array(Image.open(sys.argv[1]).convert('RGB')).astype(int); s = a.shape[1] // 320
b = np.array(Image.open(sys.argv[2]).convert('RGB').resize((320 * s, 240 * s), Image.NEAREST)).astype(int)
m = np.abs(a - b).max(axis=2) > 24
k = geo.kamera('2010', 3); P = lambda w: geo.proj(k, w)[0] * s
E = {'Rueckwand rechts (z -3300)': (P((5400, -3650, -3300)), P((5400, -50, -3300)), +1),
     'Fluegel links (x 4100)':     (P((4100, -3650, -2000)), P((4100, -50, -2000)), -1)}
H = {'Oberkante Rueckwand':        (P((5400, -3650, -2000)), P((5400, -3650, -3300)), -1),
     'Oberkante Fluegel':          (P((4100, -3650, -2000)), P((5400, -3650, -2000)), -1),
     'Unterkante Rueckwand':       (P((5400, -50, -2000)), P((5400, -50, -3300)), +1),
     'Unterkante Fluegel':         (P((4100, -50, -2000)), P((5400, -50, -2000)), +1)}
w = 5 * s
for name, (p0, p1, seite) in E.items():
    res = []
    for y in range(int(min(p0[1], p1[1])) + 3 * s, int(max(p0[1], p1[1])) - 3 * s):
        xs = p0[0] + (p1[0] - p0[0]) * (y + .5 - p0[1]) / (p1[1] - p0[1])
        x0, x1 = int(xs) - w, int(xs) + w
        z = m[y, x0:x1]
        if seite < 0 and m[y, x0 - 3 * s:x0].any(): continue      # links davon Maske (Figur): Zeile auslassen
        if not z.any(): continue
        kante = x0 + (np.where(z)[0].max() + 1 if seite > 0 else np.where(z)[0].min())
        res.append((kante - xs) / s)
    res = np.array(res)
    print('%-28s %4d Zeilen: Abstand Median %+.2f, 90%%-Betrag %.2f, max %.2f logische Bildpunkte' % (name, len(res), np.median(res), np.percentile(np.abs(res), 90), np.abs(res).max()))
for name, (p0, p1, seite) in H.items():
    res = []
    for x in range(int(min(p0[0], p1[0])) + 3 * s, int(max(p0[0], p1[0])) - 3 * s):
        ys = p0[1] + (p1[1] - p0[1]) * (x + .5 - p0[0]) / (p1[0] - p0[0])
        y0, y1 = int(ys) - w, int(ys) + w
        z = m[y0:y1, x]
        if not z.any(): continue
        kante = y0 + (np.where(z)[0].max() + 1 if seite > 0 else np.where(z)[0].min())
        res.append((kante - ys) / s)
    res = np.array(res)
    print('%-28s %4d Spalten: Abstand Median %+.2f, 90%%-Betrag %.2f, max %.2f logische Bildpunkte' % (name, len(res), np.median(res), np.percentile(np.abs(res), 90), np.abs(res).max()))
