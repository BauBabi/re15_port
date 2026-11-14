# K5: Grenze in P2 - Innenrichtung je Glied in P2 gegen PV (aus der Bogen-Mitte der Pose), Band-Anteile im Bild mit verschobenen Baendern
import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, r3_gemein as R3, unterseite_r5 as U5, f_rast as FR, kopf_orig_bau as KB
blob = open(sys.argv[1], 'rb').read(); mo = V.modell(blob)
pv = U5.innen_glied(mo, V.PV); p2 = U5.innen_glied(mo, R3.P2)
print('innen_phi PV', pv); print('innen_phi P2', p2)
print('Differenz P2-PV je Glied', {k: round((p2[k] - pv[k] + 180) % 360 - 180, 1) for k in pv})
for nm, pose in (('PV', V.PV), ('P2', R3.P2)):
    R, T, P, ik = U5.innen_modell(mo, pose)
    # Ebenen-Rest: Abstand der Kettenpunkte von ihrer Hauptebene relativ zum Radius
    c = P.mean(0); s = np.linalg.svd(P - c, compute_uv=False)
    print(nm, 'Kette: Singulaerwerte', np.round(s, 1), 'Ebenen-Rest s3/s1 %.3f' % (s[2] / s[0]))
    A = FR.zeichne(mo, pose, None, licht=False)
    ys, xs, bb, th = U5.theta_bild(mo, pose, A)
    z = A['zeile'][ys, xs]
    for lo, hi in ((100, 110), (110, 120), (120, 130), (130, 140)):
        m = (th >= lo) & (th < hi)
        print('   Band %d-%d px %5d Anteil 495 %.1f %%' % (lo, hi, m.sum(), 100 * (z[m] == 495).mean() if m.any() else 0))
    # je Knochen im Band 120-130
    m = (th >= 120) & (th < 130)
    for b in V.SCHWANZ:
        mb = m & (bb == b)
        if mb.sum():
            print('     Knochen %d Band 120-130 px %4d Anteil 495 %.1f %%' % (b, mb.sum(), 100 * (z[mb] == 495).mean()))
    # Flaechen der 495-Pixel im Band 120-130 (welche Klasse)
    par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json'))); par['fleisch_r7b'] = 1
    ms = KB.teile(blob)[2]; kl = U5.flaechen_klassen(ms, par)
    klm = {b: {k: (c, th_) for k, c, th_ in kl[b]} for b in V.SCHWANZ}
    cnt = {}
    for y, x, b, t in zip(ys[m], xs[m], bb[m], th[m]):
        if A['zeile'][y, x] == 495:
            k = (int(b), str(A['flaeche'][y, x])); cnt[k] = cnt.get(k, 0) + 1
    print('   495-Bildpunkte im Band 120-130 nach Flaeche:', ' '.join('%d:%s(%s,%s)=%d' % (b, f, klm[b][f][0], [None if q is None else round(q) for q in klm[b][f][1]], n) for (b, f), n in sorted(cnt.items(), key=lambda t: -t[1])[:12]))
