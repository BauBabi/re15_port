"""K5c und K4 in Kamera Bild 1/2 mit Szenenlicht (form_pruefung.Kandidat.rendern, Licht 'mess'), dann dieselbe Kette wie das Original:
video_filter (mit Rauschen, Saat fest) -> video_umkehr -> / Licht. Zusaetzlich die Albedo (reine Texturfarbe) an denselben Pixeln."""
import json, sys
import numpy as np, cv2
import fl, zonen as Z
sys.path.insert(0, fl.TOOLS)
import form_pruefung as P
import form_video as FV

aus = {}
for nm in sys.argv[1:]:
    k = P.Kandidat(fl.K5 + '/%s/form.json' % nm)
    for n in (1, 2):
        sauber, alb, m, kk = k.rendern(n)
        fig = m & (alb.max(2) > 0)
        vid = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True)
        e, T, clip = Z.rueckrechnen(vid, n)
        res, alle = Z.zonen_messen(e, T, clip, Z.regionen(n), fig=fig)
        # Albedo direkt (Texturfarbe am Pixel), clip = nie
        resA, _ = Z.zonen_messen(alb.astype(float), alb.astype(float), np.zeros((480, 640), bool), Z.regionen(n), fig=fig)
        # Schirm ohne Video (sauber / Licht = Texturschaetzung ohne Filter)
        aus['%s_bild%d' % (nm, n)] = dict(kette=res, albedo=resA, licht_k=list(kk))
        np.save(fl.S + '/%s_sauber%d.npy' % (nm, n), sauber)
        np.save(fl.S + '/%s_alb%d.npy' % (nm, n), alb)
        np.save(fl.S + '/%s_fig%d.npy' % (nm, n), fig)
        np.save(fl.S + '/%s_vid%d.npy' % (nm, n), vid)
        np.save(fl.S + '/%s_T%d.npy' % (nm, n), T)
        np.save(fl.S + '/%s_clip%d.npy' % (nm, n), clip)
        print('==', nm, n, 'licht', kk, 'fig px', fig.sum())
        for r, a in zip(res, resA):
            print('  ', r['zone'], r.get('px'), r.get('anteil_figur'), 'KETTE', r.get('klassen'), r.get('T_mittel'), r.get('T_lab'), r.get('mehrton'))
            print('      ALB', a.get('klassen'), a.get('T_mittel'), a.get('T_lab'), a.get('mehrton'), a.get('glanz_L85', ''))
json.dump(aus, open(fl.S + '/zonen_var_%s.json' % '_'.join(sys.argv[1:]), 'w'), indent=1)
