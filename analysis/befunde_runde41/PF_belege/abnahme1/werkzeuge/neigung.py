"""Neigung senkrechter Strukturen (Sicherungsroehren, helle Saeulen) in einem entzerrten Bild (Weltmass, 10 Einheiten je Bildpunkt):
je Zeile im Roehrenbereich die Schwerpunkte der hellen Laeufe; Laeufe ueber die Zeilen verketten; Gerade x(y) je Roehre -> Neigung in Grad."""
import sys, numpy as np
from PIL import Image
def neigung(p, y0, y1, schwelle=None):
    a = np.array(Image.open(p).convert('L')).astype(float)
    a = a[y0:y1]
    if schwelle is None: schwelle = np.percentile(a, 80)
    ketten = []
    for yi, z in enumerate(a):
        hell = z > schwelle
        laeufe = []; x = 0
        while x < len(z):
            if hell[x]:
                x2 = x
                while x2 < len(z) and hell[x2]: x2 += 1
                if 3 <= x2 - x <= 20: laeufe.append(((x + x2 - 1) / 2.0))
                x = x2
            else: x += 1
        for c in laeufe:
            for k in ketten:
                if k[-1][0] == yi - 1 and abs(k[-1][1] - c) < 2.5: k.append((yi, c)); break
            else: ketten.append([(yi, c)])
    out = []
    for k in ketten:
        if len(k) < 0.5 * (y1 - y0): continue
        k = np.array(k); m = np.polyfit(k[:, 0], k[:, 1], 1)[0]
        out.append((float(np.mean(k[:, 1])), len(k), float(np.degrees(np.arctan(m)))))
    return out
if __name__ == '__main__':
    for x, n, g in neigung(sys.argv[1], int(sys.argv[2]), int(sys.argv[3])):
        print('Roehre bei x %.1f: %d Zeilen, Neigung %+.2f Grad' % (x, n, g))
