#!/usr/bin/env python3
"""Abnahme 3: Was traegt der Wurzelknochen s1 (1) in Bild 1 bei? Maske mit und ohne s1, gegen ziel2 und Nutzerumriss tail. Kein RE-Wert, aendert keine Datei."""
import os, sys
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P, form_buehne as FB
k = P.Kandidat(os.path.join(F, 'rohbau_final', 'form.json'))
z = cv2.imread(os.path.join(F, 'ziel2_bild1.png'), cv2.IMREAD_GRAYSCALE)
mt = cv2.imread(os.path.join(F, 'nutzer_teile', 'maske_tail.png'), cv2.IMREAD_GRAYSCALE) > 127
out = []
for n in (1,):
    lage, gier, clip, bild = k.stand(n); kam = P.kamera(n); kf = k.kf(clip, bild)
    w = FB.Textur(k.tim); w.lut = w.lut.copy(); w.lut[..., :3] = 255
    def maske(teile):
        return FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile=teile), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
    alle = maske(set(range(32))); ohne = maske(set(range(32)) - {1}); nur = maske({1})
    weg = alle & ~ohne
    out.append('Bild %d: Figur %d px, ohne s1 %d px, nur s1 sichtbar beigetragen %d px, s1 allein %d px' % (n, alle.sum(), ohne.sum(), weg.sum(), nur.sum()))
    out.append('  davon im Ziel Figur (255) %d, frei (0) %d, grau %d; im Nutzerumriss tail %d' % ((weg & (z == 255)).sum(), (weg & (z == 0)).sum(), (weg & (z != 255) & (z != 0)).sum(), (weg & mt).sum()))
    ys, xs = np.nonzero(weg)
    if len(ys):
        out.append('  Lage der fehlenden Pixel: x %d-%d, y %d-%d' % (xs.min(), xs.max(), ys.min(), ys.max()))
    for x in (290, 300, 305, 311, 316, 322, 330):
        ka = np.nonzero(alle[:, x])[0]; ko = np.nonzero(ohne[:, x])[0]; kz = np.nonzero(z[:, x] == 255)[0]
        out.append('  Spalte x %d: Oberkante mit s1 y %s, ohne s1 y %s, Ziel Figur ab y %s' % (x, ka.min() if len(ka) else '-', ko.min() if len(ko) else '-', kz.min() if len(kz) else '-'))
    # Bild ablegen
    b = np.zeros((480, 640, 3), np.uint8); b[ohne] = (90, 90, 90); b[weg] = (0, 0, 255)
    b[(z == 255) & ~alle] = (0, 200, 0)
    cv2.imwrite(os.path.join(H, 'bild', 's1probe_bild1.png'), cv2.resize(b[150:480, 170:450], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))
open(os.path.join(H, 'mess', 's1probe.txt'), 'w').write('\n'.join(out) + '\n'); print('\n'.join(out))
