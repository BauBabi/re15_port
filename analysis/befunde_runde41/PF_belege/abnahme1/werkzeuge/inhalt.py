"""Mass 2b: Seitenverhaeltnis des INHALTS. Beide Seiten nur ueber die RDT-Kameras entzerrt (keine Handecken):
  2060: BG06 (Cut 6 @0x120) auf die Wandebene z = Z0 (Wand achsparallel; Z0 aendert nur den Gesamtmassstab, nicht B/H) -> Weltmass, 1 Bildpunkt = 10 Einheiten
  Port: Fensterbild unter ROOM2010 Cut 3 (@0xC0) auf die Rueckwand x = 5400 (z -2000..-3300, y -3650..-50) -> Weltmass, 10 Einheiten
Dann ECC (cv2.findTransformECC, MOTION_AFFINE) Port -> 2060; aus der Affinen: sx, sy, Scherung; sx/sy = 1 heisst unverzerrt.
Gegenprobe: dieselbe Rechnung mit dem Port-Bild auf 80 % Breite gestaucht muss ~0,8 ergeben."""
import sys, numpy as np, cv2, geo
from PIL import Image
E = 10.0

def zu_bild(k, W):          # W: (...,3) Weltpunkte -> Bildpunkte (logisch)
    d = W - k['p']
    z = d @ k['f']
    return 160 + k['H'] * (d @ k['r']) / z, 120 + k['H'] * (d @ k['u']) / z

def entzerre_2060(bg, z0=4500.0, gier=0.0, x0=-5100, x1=-2900, y0=-3800, y1=150):
    k = geo.kamera('2060', 6)
    xs = np.arange(x0, x1, E); ys = np.arange(y0, y1, E)
    X, Y = np.meshgrid(xs, ys)
    g = np.radians(gier)    # Wand um gier gedreht (um die Mitte x -4000)
    Wx = -4000 + (X + 4000) * np.cos(g); Wz = z0 + (X + 4000) * np.sin(g)
    W = np.stack([Wx, Y, Wz], -1)
    u, v = zu_bild(k, W)
    return cv2.remap(bg.astype(np.float32), (u - .5).astype(np.float32), (v - .5).astype(np.float32), cv2.INTER_LINEAR)

def entzerre_port(fenster, s, raum='2010'):
    k = geo.kamera(raum, 3)
    zs = np.arange(-2000, -3300, -E); ys = np.arange(-3650, -50, E)
    Z, Y = np.meshgrid(zs, ys)
    W = np.stack([np.full_like(Z, 5400.0), Y, Z], -1)
    u, v = zu_bild(k, W)
    return cv2.remap(fenster.astype(np.float32), (u * s - .5).astype(np.float32), (v * s - .5).astype(np.float32), cv2.INTER_LINEAR)

def ecc(port, ref, s0, t0):
    a = cv2.GaussianBlur(port, (0, 0), 1.0); b = cv2.GaussianBlur(ref, (0, 0), 1.0)
    w = np.array([[s0, 0, t0[0]], [0, s0, t0[1]]], np.float32)       # ref -> port (findTransformECC: port(W x_ref) ~ ref(x_ref))
    crit = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 2000, 1e-7)
    cc, w = cv2.findTransformECC(b, a, w, cv2.MOTION_AFFINE, crit, None, 5)   # findet w: ref(x) ~ port(W^-1 ...) -> wir nutzen WARP_INVERSE
    A = w[:, :2]
    sx = np.linalg.norm(A[:, 0]); sy = np.linalg.norm(A[:, 1])
    sch = float(np.degrees(np.arccos(abs(A[:, 0] @ A[:, 1]) / sx / sy)))
    return cc, w, sx, sy, sch

if __name__ == '__main__':
    fenster = np.array(Image.open(sys.argv[1]).convert('L')).astype(np.float32)
    s = fenster.shape[1] // 320
    bg = np.array(Image.open(sys.argv[2]).convert('L')).astype(np.float32)
    ref = entzerre_2060(bg)
    port = entzerre_port(fenster, s)
    if len(sys.argv) > 4: port = cv2.resize(port, None, fx=float(sys.argv[4]), fy=1.0)   # Gegenprobe gestaucht
    cv2.imwrite(sys.argv[3] + '_2060.png', np.clip(ref, 0, 255).astype(np.uint8))
    cv2.imwrite(sys.argv[3] + '_port.png', np.clip(port, 0, 255).astype(np.uint8))
    # Startwert: Port 1300 breit = Mitte des 2060-Kastens; Massstab grob 1,0 (Weltmass beider Seiten), Mitte auf Mitte
    best = None
    for s0 in (0.8, 0.9, 1.0, 1.1):
        for tx in (-80, -40, 0, 40, 80):
            for ty in (-40, 0, 40):
                t0 = (port.shape[1] / 2 - s0 * (ref.shape[1] / 2 + tx), port.shape[0] / 2 - s0 * (ref.shape[0] / 2 + ty))
                try:
                    r = ecc(port, ref, s0, t0)
                except cv2.error:
                    continue
                if best is None or r[0] > best[0]:
                    best = r
    cc, w, sx, sy, sch = best
    print('ECC %.3f  sx %.4f  sy %.4f  sx/sy %.3f  Achsenwinkel %.1f Grad  Verschiebung (%.0f,%.0f)' % (cc, sx, sy, sx / sy, sch, w[0, 2], w[1, 2]))
