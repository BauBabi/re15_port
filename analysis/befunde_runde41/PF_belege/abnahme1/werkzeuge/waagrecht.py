"""Lage der waagrechten Strukturen: Projektionsprofil-Methode. Fuer Winkel t: Bild um t drehen, Varianz der Zeilenmittel (der Ableitung) -> Maximum = Neigung der Waagrechten.
Ebenso fuer die Senkrechten (Spaltenmittel). Positiv = im Uhrzeigersinn gegen die Bildachse."""
import sys, numpy as np, cv2
from PIL import Image
def profil(a, achse, x0, x1, y0, y1):
    best = []
    for t in np.arange(-8, 8.01, 0.1):
        m = cv2.getRotationMatrix2D(((x0 + x1) / 2, (y0 + y1) / 2), t, 1.0)
        r = cv2.warpAffine(a, m, (a.shape[1], a.shape[0]), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REFLECT)[y0:y1, x0:x1]
        pr = r.mean(axis=1 - achse)            # achse 0: Zeilenmittel (Waagrechte), 1: Spaltenmittel (Senkrechte)
        best.append((float(np.var(np.diff(pr))), t))
    return max(best)[1]
p = sys.argv[1]; x0, x1, y0, y1 = map(int, sys.argv[2:6])
a = np.array(Image.open(p).convert('L')).astype(np.float32)
print('%s: Waagrechte %+.1f Grad, Senkrechte %+.1f Grad' % (p.split('/')[-1], profil(a, 0, x0, x1, y0, y1), profil(a, 1, x0, x1, y0, y1)))
