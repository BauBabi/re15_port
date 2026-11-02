import json, os, sys, numpy as np, torch, cv2
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import fb_nach
import form_buehne as FB
from rf_kern import weich
import form_pruefung as P
nach = fb_nach.Nach(json.load(open('stand_final.json')), 'form.json', 'zw/delta.json')
k = P.Kandidat('form.json')
with torch.no_grad():
    v = nach.ecken()
    for n in (1, 2):
        R, T = nach.pose[n]
        w = (R[nach.kn] @ v[..., None])[..., 0] + T[nach.kn]
        xy = nach.kam[n](w)
        x0, y0, x1, y1 = fb_nach.BOX[n]
        S = weich(xy, nach.tris, nach.kanal, y1 - y0, x1 - x0, 0.3, 1, x0, y0, C=2)
        U = (1 - (1 - S[0]) * (1 - S[1])).cpu().numpy() > .5
        lage, gier, clip, bild = k.stand(n); kam = P.kamera(n); kf = k.kf(clip, bild)
        wt = FB.Textur(k.tim); wt.lut = wt.lut.copy(); wt.lut[..., :3] = 255
        m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), wt, np.zeros((480, 640, 3), np.uint8), kam.abbildung)[y0:y1, x0:x1]
        print('Bild', n, 'nur weich', int((U & ~m).sum()), 'nur Pruefung', int((m & ~U).sum()), 'beide', int((U & m).sum()))
        # Schwerpunktversatz der Differenz
        ys, xs = np.nonzero(m); yu, xu = np.nonzero(U)
        print('   Schwerpunkt Pruefung', xs.mean() + x0, ys.mean() + y0, ' weich', xu.mean() + x0, yu.mean() + y0)
        if n == 2:
            col = 411 - x0
            print('   Spalte 411 oberste Zeile: Pruefung', np.nonzero(m[:, col])[0].min() + y0, ' weich', np.nonzero(U[:, col])[0].min() + y0)
        b = np.zeros(U.shape + (3,), np.uint8); b[U & m] = 128; b[U & ~m] = (0, 0, 255); b[m & ~U] = (255, 0, 0)
        cv2.imwrite('zw/dbg_nach_%d.png' % n, cv2.resize(b, None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST))
print('Versatzsuche')
with torch.no_grad():
    v = nach.ecken()
    for n in (1, 2):
        R, T = nach.pose[n]
        w = (R[nach.kn] @ v[..., None])[..., 0] + T[nach.kn]
        xy0 = nach.kam[n](w)
        x0, y0, x1, y1 = fb_nach.BOX[n]
        lage, gier, clip, bild = k.stand(n); kam = P.kamera(n); kf = k.kf(clip, bild)
        wt = FB.Textur(k.tim); wt.lut = wt.lut.copy(); wt.lut[..., :3] = 255
        m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), wt, np.zeros((480, 640, 3), np.uint8), kam.abbildung)[y0:y1, x0:x1]
        best = []
        for dx in np.arange(-2, 1.01, .25):
            for dy in np.arange(-2, 1.01, .25):
                xy = xy0 + torch.tensor([dx, dy], device=xy0.device, dtype=xy0.dtype)
                S = weich(xy, nach.tris, nach.kanal, y1 - y0, x1 - x0, 0.3, 1, x0, y0, C=2)
                U = (1 - (1 - S[0]) * (1 - S[1])).cpu().numpy() > .5
                best.append(((U ^ m).sum(), dx, dy))
        best.sort()
        print('Bild', n, best[:3])
