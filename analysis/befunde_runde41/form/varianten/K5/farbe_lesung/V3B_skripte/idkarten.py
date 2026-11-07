"""K5c in Kamera Bild 1/2: Knochen-ID je Pixel (mit Verdeckung) und Kopf/Hals/Kiefer allein (ohne Verdeckung)."""
import sys, json
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/B_skripte')
import form_pruefung as P, form_buehne as FB
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/leserb/'
k = P.Kandidat(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/K5c/form.json')
aus = {}
for n in (1, 2):
    zu, w = P.teil_zuordnung(k, n)
    tb = P.teilbild(k, n, zu)
    kn = tb['kn']
    np.save(S + 'kn%d.npy' % n, kn)
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n); kf = k.kf(clip, bild)
    weiss = FB.Textur(k.tim); weiss.lut = weiss.lut.copy(); weiss.lut[..., :3] = 255
    allein = {}
    for b in (21, 22, 23):
        m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile={b}), weiss, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
        allein[b] = m
        np.save(S + 'allein%d_b%d.npy' % (n, b), m)
    # Albedo und Szenenlicht-Render
    sauber, alb, mm, kk = k.rendern(n)
    np.save(S + 'sauber%d.npy' % n, sauber); np.save(S + 'alb%d.npy' % n, alb)
    d = {}
    for b in range(int(kn.max()) + 1):
        s = kn == b
        if s.sum():
            ys, xs = np.nonzero(s)
            d[b] = dict(px=int(s.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])
    for b, m in allein.items():
        ys, xs = np.nonzero(m)
        d['allein_%d' % b] = dict(px=int(m.sum()), sichtbar=int((m & (kn == b)).sum()),
                                  bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if m.any() else None)
    aus[n] = d
    print(n, json.dumps(d))
json.dump(aus, open(S + 'idkarten.json', 'w'), indent=1)
