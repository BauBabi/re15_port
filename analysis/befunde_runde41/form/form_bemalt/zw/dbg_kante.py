import os, sys, copy, numpy as np
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import rf_kern
import form_pruefung as P, form_buehne as FB
for f in sys.argv[1:]:
    k = P.Kandidat(f)
    n = 1
    lage, gier, clip, bild = k.stand(n); kam = P.kamera(n); kf = k.kf(clip, bild)
    wt = FB.Textur(k.tim); wt.lut = wt.lut.copy(); wt.lut[..., :3] = 255
    orig = k.mo.meshes
    zeile = {}
    for b in range(len(orig)):
        ms = [m if i == b else type(m).__new__(type(m)) for i, m in enumerate(orig)]
        for i, m in enumerate(orig):
            if i != b:
                ms[i] = copy.copy(m); ms[i].tf, ms[i].qf = [], []
        k.mo.meshes = ms
        m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), wt, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
        xs = np.nonzero(m[342, 270:310])[0]
        if len(xs):
            zeile[b] = (int(xs.min()) + 270, int(xs.max()) + 270)
    k.mo.meshes = orig
    print(f, zeile)
