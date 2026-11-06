"""Sitzt der Kasten im Raumbild? Naht (Hintergrund scheint durch), Lage der Kanten, Helligkeit. Fensterbild 960x720, Cut 3, Tuer offen."""
import numpy as np, cv2
from PIL import Image
import kam
k = kam.kamera('2010', 3)
bg = np.asarray(Image.open('bg/ROOM20103.ppm').convert('RGB'))
bg3 = np.repeat(np.repeat(bg, 3, 0), 3, 1).astype(int)
def poly(P):
    p = k.proj(np.array(P, float))
    return (p[:, :2] * 3).astype(np.float32)
INNEN = poly([(5400, -3650, -2000), (5400, -3650, -3300), (5400, -50, -3300), (5400, -50, -2000)])
FLUEG = poly([(4100, -3650, -2000), (5400, -3650, -2000), (5400, -50, -2000), (4100, -50, -2000)])
def maske(pg, erode=0):
    m = np.zeros((720, 960), np.uint8)
    cv2.fillPoly(m, [np.round(pg * 16).astype(np.int32)], 1, shift=4)
    if erode:
        m = cv2.erode(m, np.ones((2 * erode + 1, 2 * erode + 1), np.uint8))
    return m.astype(bool)
for fig in ('leon', 'elza'):
    for st in ('vor', 'nach'):
        w = np.load('k_k_%s_%s_auf.npy' % (st, fig)).astype(int)
        z = np.load('k_k_%s_%s_zu.npy' % (st, fig)).astype(int)
        d = np.abs(w - bg3).max(-1)
        out = []
        for n, pg in (('innen', INNEN), ('fluegel', FLUEG)):
            inn = maske(pg, 3)
            gleich_bg = int((d[inn] <= 2).sum())
            lum = w[inn].mean()
            out.append('%s: Innenpunkte %d, davon = BG03 (Naht/Durchschein) %d; Helligkeit %.1f' % (n, int(inn.sum()), gleich_bg, lum))
        # Gesamtflaeche, die vom Hintergrund abweicht, gegen die Projektion (Kantenlage)
        proj = maske(INNEN) | maske(FLUEG)
        ab = d > 24
        rand = maske(INNEN, 0) ^ maske(INNEN, 3) | (maske(FLUEG, 0) ^ maske(FLUEG, 3))
        aussen_ab = ab & ~proj
        ys, xs = np.nonzero(aussen_ab[0:700, 470:830])
        print('%s %s | %s | Abweichung vom BG03 ausserhalb der projizierten Flaechen (Fenster x 470..830, ohne Figur): %d Punkte' % (fig, st, ' | '.join(out), int(aussen_ab[:, 500:830].sum())))
        # Geschlossen: Abweichung vom BG03 in der Schrankflaeche (Aussentuer)
        dz = np.abs(z - bg3).max(-1)
        print('      zu: Tuerflaeche mittlere |Bild-BG03| %.1f' % dz[maske(INNEN, 3)].mean())

# Lage der BG03-gleichen Innenpunkte und der Aussenabweichungen (nachher Leon)
import scipy.ndimage as nd
w = np.load('k_k_nach_leon_auf.npy').astype(int)
d = np.abs(w - bg3).max(-1)
proj = maske(INNEN) | maske(FLUEG)
inn = maske(INNEN, 3) | maske(FLUEG, 3)
m_in = (d <= 2) & inn
lab, n = nd.label(m_in, np.ones((3, 3)))
groessen = np.bincount(lab.ravel())[1:]
print('BG03-gleiche Innenpunkte: %d Gruppen, groesste %d Punkte, Gruppen >= 6 Punkte: %d' % (n, groessen.max(), int((groessen >= 6).sum())))
# laengste gerade Kette (Naht waere eine Linie): Ausdehnung der Gruppen
sl = nd.find_objects(lab)
ausd = sorted([max(s[0].stop - s[0].start, s[1].stop - s[1].start) for s in sl], reverse=True)[:5]
print('groesste Ausdehnungen (Fensterpunkte):', ausd)
aussen = (d > 24) & ~proj
aussen[:, :500] = False
dist = nd.distance_transform_edt(~proj)
print('Aussenabweichungen: %d Punkte, Abstand zur projizierten Kante max %.1f Fensterpunkte, Median %.1f' % (int(aussen.sum()), dist[aussen].max(), np.median(dist[aussen])))
# Bild: Maske zur Ansicht
v = (w * 0.6).astype(np.uint8)
v[m_in] = (255, 0, 255); v[aussen] = (0, 255, 0)
Image.fromarray(v[30:690, 470:830]).save('../sitz_nach.png')
