"""ROOM2060 BG06 ueber Cut 6 (@0x120) entzerrt: Rueckwand Ebene z 4500 (achsparallel), Fluegel senkrechte Ebene durch (-4830, *, 4500)
mit der Richtung des eigenen Fluchtpunkts (fluchtpunkt2.py: 131,3 = Median der vier Schildkanten)."""
import numpy as np, cv2
from PIL import Image
import kam
S = 4.0
bg = np.asarray(Image.open('bg/ROOM20606.ppm').convert('RGB'))
k = kam.kamera('2060', 6)
def remap(P):
    p = k.proj(P)
    return cv2.remap(bg, (p[..., 0] - .5).astype(np.float32), (p[..., 1] - .5).astype(np.float32), cv2.INTER_CUBIC, borderMode=cv2.BORDER_CONSTANT)
def rueckwand(x0=-5100, x1=-2900, y0=-3900, y1=300):
    X, Y = np.meshgrid(np.arange(x0, x1, S) + S / 2, np.arange(y0, y1, S) + S / 2)
    return remap(np.stack([X, Y, np.full_like(X, 4500.)], -1))
def richtung(vp):
    d = np.array([vp - 160, 67.975 - 120, k.H], float) @ k.R
    d[1] = 0
    return d / np.linalg.norm(d)
def fluegel(vp=131.3, s0=0, s1=2200, y0=-3600, y1=400):
    d = richtung(vp)
    # s waechst vom freien Rand (links im Bild) zur Angel (rechts): Punkt = Angel - (s1 - s) * d ... Orientierung pruefen wir am Bild
    sv, Y = np.meshgrid(np.arange(s0, s1, S) + S / 2, np.arange(y0, y1, S) + S / 2)
    P = np.stack([-4830 - (s1 - sv) * d[0], Y, 4500 - (s1 - sv) * d[2]], -1)
    return remap(P), d
if __name__ == '__main__':
    r = rueckwand(); f, d = fluegel()
    print('Fluegelrichtung', d, 'Gier %.2f Grad' % np.degrees(np.arctan2(d[0], d[2])))
    h = max(r.shape[0], f.shape[0])
    im = np.concatenate([np.pad(f, ((0, h - f.shape[0]), (0, 10), (0, 0))), np.pad(r, ((0, h - r.shape[0]), (0, 0), (0, 0)))], 1)
    Image.fromarray(im).save('../ref_2060.png'); print(im.shape)
