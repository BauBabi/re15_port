"""Mass 2a: Kanten des Kastens im Fensterbild gegen die Projektion der Port-Geometrie mit der RDT-Kamera ROOM2010 Cut 3.
Maske = |Fenster - BG03| > Schwelle (BG03 = Original-Hintergrund, zeigt den Schrank geschlossen; die Aussenseite der Tuer ist BG03 zurueckprojiziert).
Erwartung = Rueckwand (x 5400, z -3300..-2000, y -3650..-50) + offener Fluegel (z -2000, x 4100..5400) projiziert.
Je Bildzeile im Kastenbereich: linker/rechter Rand der Maske gegen die projizierten Kanten."""
import sys, numpy as np, geo, logisch
from PIL import Image, ImageDraw
bild, bg3 = sys.argv[1], sys.argv[2]
raum = sys.argv[3] if len(sys.argv) > 3 else '2010'
a, s, ein = logisch.lade(bild)
b = np.array(Image.open(bg3).convert('RGB')).astype(int)
k = geo.kamera(raum, 3)
P = lambda w: geo.proj(k, w)[0]
innen = [P((5400, -3650, -2000)), P((5400, -3650, -3300)), P((5400, -50, -3300)), P((5400, -50, -2000))]
tuer = [P((4100, -3650, -2000)), P((5400, -3650, -2000)), P((5400, -50, -2000)), P((4100, -50, -2000))]
def maske(poly):
    m = Image.new('L', (320, 240), 0); ImageDraw.Draw(m).polygon([tuple(p) for p in poly], fill=1)
    return np.array(m).astype(bool)
soll = maske(innen) | maske(tuer)
d = np.abs(a - b).max(axis=2)
ist = d > 24
# Gegenprobe: Abweichung Fenster - BG03 ausserhalb des Kastens (rechte Wand x>=275, links oben x<150,y<100) -> Farbtreue der Darstellung
aussen = np.zeros_like(ist); aussen[:, 275:] = True; aussen[:100, :150] = True; aussen &= ~soll
print('Skalierung %d, Bloecke einfarbig %.4f' % (s, ein))
print('Abweichung Fenster-BG03 ausserhalb (Mittel/95%%): %.2f / %.1f, Anteil >24: %.4f' % (d[aussen].mean(), np.percentile(d[aussen], 95), ist[aussen].mean()))
# Fehlerbilanz in einem Band um den Kasten, ohne Figur (Figur steht links der Tuer: x < 160 ausgeschlossen)
band = np.zeros_like(ist); band[10:235, 160:280] = True
tp = (ist & soll & band).sum(); fp = (ist & ~soll & band).sum(); fn = (~ist & soll & band).sum()
print('Kastenflaeche soll %d Bildpunkte; Maske deckt %d (%.1f %%), ausserhalb der Projektion %d, IoU %.3f' % ((soll & band).sum(), tp, 100.0 * tp / (soll & band).sum(), fp, tp / (tp + fp + fn)))
# Kanten je Zeile: rechte Kante (Rueckwand, nahe Kante z -3300) und linke Kante (Fluegel, x 4100)
def kante_soll(p0, p1, y):
    (x0, y0), (x1, y1) = p0, p1
    return x0 + (x1 - x0) * (y - y0) / (y1 - y0)
res_r, res_l = [], []
for y in range(45, 185):
    zeile = np.where(ist[y, 150:290])[0]
    if len(zeile) < 5: continue
    xr = 150 + zeile.max() + 0.5; xl = 150 + zeile.min() - 0.5
    res_r.append(xr - kante_soll(innen[1], innen[2], y + 0.5))
    res_l.append(xl - kante_soll(tuer[0], tuer[3], y + 0.5))
res_r, res_l = np.array(res_r), np.array(res_l)
print('rechte Kante (Rueckwand z -3300): Abstand Maske-Projektion Median %.2f, |max| %.2f Bildpunkte (%d Zeilen)' % (np.median(res_r), np.abs(res_r).max(), len(res_r)))
print('linke Kante (Fluegel x 4100):     Abstand Maske-Projektion Median %.2f, |max| %.2f Bildpunkte (%d Zeilen)' % (np.median(res_l), np.abs(res_l).max(), len(res_l)))
# Oberkante: je Spalte erste Maskenzeile
res_o = []
for x in range(170, 262):
    sp = np.where(ist[5:120, x])[0]
    if not len(sp): continue
    yo = 5 + sp.min() - 0.5
    if x < innen[0][0]: ys = kante_soll(tuer[0][::-1], tuer[1][::-1], x)   # Oberkante Fluegel (x ist hier die Laufvariable)
    else: ys = kante_soll(innen[0][::-1], innen[1][::-1], x)
    res_o.append(yo - ys)
res_o = np.array(res_o)
print('Oberkante: Abstand Maske-Projektion Median %.2f, |max| %.2f Bildpunkte (%d Spalten)' % (np.median(res_o), np.abs(res_o).max(), len(res_o)))
# Bild: Maske (rot = ist, nicht soll; gruen = beides; blau = soll, nicht ist) + Projektion
v = (a * 0.5).astype(np.uint8)
v[ist & soll] = [0, 200, 0]; v[ist & ~soll] = [230, 40, 40]; v[~ist & soll] = [40, 80, 255]
im = Image.fromarray(v).resize((960, 720), Image.NEAREST)
dr = ImageDraw.Draw(im)
for poly in (innen, tuer):
    dr.polygon([(x * 3, y * 3) for x, y in poly], outline=(255, 255, 0))
im.save(sys.argv[4] if len(sys.argv) > 4 else 'kasten_geo.png')
