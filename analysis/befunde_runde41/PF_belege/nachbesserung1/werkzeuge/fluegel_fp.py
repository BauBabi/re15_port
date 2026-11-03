"""Mass 2b fuer den Fluegel (Tuer-Innenseite). 2060: Ebene des offenen Fluegels aus dem Fluchtpunkt seiner Ober-/Unterkante (liegt auf dem
Kamera-Horizont y 68 -> waagrecht), Richtung theta aus dem Fluchtpunkt-x; Port: offener Fluegel z = -2000, x 4100..5400 (rot_y 1024).
Beide als 'Abstand von der Angel' (links = freier Rand, rechts = Angel), 10 Einheiten je Bildpunkt, dann ECC wie inhalt.py."""
import sys, numpy as np, cv2, geo, inhalt
from PIL import Image
E = 10.0
k6 = geo.kamera('2060', 6)
def fp_richtung(vpx):
    # (d.r)/(d.f) = (vpx-160)/H, d = (cos t, 0, sin t)
    c = (vpx - 160) / k6['H']; r, f = k6['r'], k6['f']
    t = np.arctan2(r[0] - c * f[0], -(r[2] - c * f[2]))
    d = np.array([np.cos(t), 0, np.sin(t)])
    if d @ f < 0: d = -d
    return d, np.degrees(t)
def schnitt(a0, a1, b0, b1):
    (x1, y1), (x2, y2), (x3, y3), (x4, y4) = a0, a1, b0, b1
    den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / den
    py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / den
    return px, py
import os; vp = (float(os.environ.get("FP_X", "130.80")), 67.975)      # Fluchtpunkt aus den Schildkanten (schrank_bild.fluchtpunkt)
d, t = fp_richtung(vp[0])
print('Fluchtpunkt Ober/Unterkante (%.1f, %.1f), Horizont der Kamera y %.1f; Fluegelrichtung %.1f Grad' % (vp[0], vp[1], 120 + k6['H'] * (np.array([1, 0, 0]) @ k6['u']) / (np.array([1, 0, 0]) @ k6['f']), t))
def entzerre_2060_fluegel(bg, smax=2300):
    h = np.array([-4850.0, 0, 4500.0])        # Angel (linke Kante der Rueckwand auf der Ebene z 4500; nur Massstab)
    ss = np.arange(smax, 0, -E); ys = np.arange(-3800, 150, E)
    S, Y = np.meshgrid(ss, ys)
    W = np.stack([h[0] - S * d[0], Y, h[2] - S * d[2]], -1)
    u, v = inhalt.zu_bild(k6, W)
    return cv2.remap(bg.astype(np.float32), (u - .5).astype(np.float32), (v - .5).astype(np.float32), cv2.INTER_LINEAR, borderValue=0)
def entzerre_port_fluegel(fenster, s, raum='2010'):
    k = geo.kamera(raum, 3)
    ss = np.arange(1300, 0, -E); ys = np.arange(-3650, -50, E)
    S, Y = np.meshgrid(ss, ys)
    W = np.stack([5400.0 - S, Y, np.full_like(S, -2000.0)], -1)
    u, v = inhalt.zu_bild(k, W)
    return cv2.remap(fenster.astype(np.float32), (u * s - .5).astype(np.float32), (v * s - .5).astype(np.float32), cv2.INTER_LINEAR)
if __name__ == '__main__':
    fenster = np.array(Image.open(sys.argv[1]).convert('L')).astype(np.float32); s = fenster.shape[1] // 320
    bg = np.array(Image.open(sys.argv[2]).convert('L')).astype(np.float32)
    ref = entzerre_2060_fluegel(bg); port = entzerre_port_fluegel(fenster, s)
    if len(sys.argv) > 4: port = cv2.resize(port, None, fx=float(sys.argv[4]), fy=1.0)   # Gegenprobe gestaucht
    cv2.imwrite(sys.argv[3] + '_2060.png', np.clip(ref, 0, 255).astype(np.uint8)); cv2.imwrite(sys.argv[3] + '_port.png', np.clip(port, 0, 255).astype(np.uint8))
    best = None
    for s0 in (0.8, 0.9, 1.0, 1.1):
        for tx in (-120, -60, 0, 60):
            for ty in (-40, 0, 40):
                t0 = (port.shape[1] / 2 - s0 * (ref.shape[1] / 2 + tx), port.shape[0] / 2 - s0 * (ref.shape[0] / 2 + ty))
                try: r = inhalt.ecc(port, ref, s0, t0)
                except cv2.error: continue
                if best is None or r[0] > best[0]: best = r
    cc, w, sx, sy, sch = best
    print('ECC %.3f  sx %.4f  sy %.4f  sx/sy %.3f  Achsenwinkel %.1f Grad  Verschiebung (%.0f,%.0f)' % (cc, sx, sy, sx / sy, sch, w[0, 2], w[1, 2]))
