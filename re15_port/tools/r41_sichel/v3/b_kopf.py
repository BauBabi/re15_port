# Leser B v3: Kopfbox, helle Kuppe und rote Zone (A8) im KI-Video 4 _slp, Vorderansicht.
# Kopfbox: links/rechts = Helligkeitsminimum der Halsfalten in der Zeile 2 px unter der Augenmitte (3x3-geglaettetes L*),
#   oben = erste Zeile mit Lab a* > 5 in der Spalte der Augenmitte (rote Scheitellinie gegen den Hintergrund a* < 0),
#   unten = Schattenminimum unter dem Kinn in der Spalte der Augenmitte (an der Lupe k<n>.png bestaetigt, geschaetzt +-3 px).
# Bander je 0,02 H in der Kopfbox: Anteil hell (L* > 40) und Anteil rot (a* > 12).
# Aufruf: python b_kopf.py <bild.png> <name> H x0:y0:x1:y1
import sys, json, cv2, numpy as np
im = cv2.imread(sys.argv[1]); name = sys.argv[2]; H = float(sys.argv[3]); x0, y0, x1, y1 = map(int, sys.argv[4].split(":"))
f = cv2.cvtColor(im.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)
b = f[y0:y1 + 1, x0:x1 + 1]
band = max(1, int(round(0.02 * H)))
rows = []
for y in range(0, b.shape[0], band):
    s = b[y:y + band]
    rows.append(dict(y=y0 + y, unter_kopfoben_H=round(y / H, 3), hell=round(float((s[..., 0] > 40).mean()), 2), rot=round(float((s[..., 1] > 12).mean()), 2)))
res = dict(bild=name, box=[x0, y0, x1, y1], breite_px=x1 - x0 + 1, hoehe_px=y1 - y0 + 1,
           breite_H=round((x1 - x0 + 1) / H, 3), hoehe_H=round((y1 - y0 + 1) / H, 3),
           hell_gesamt=round(float((b[..., 0] > 40).mean()), 3), rot_gesamt=round(float((b[..., 1] > 12).mean()), 3),
           lab_mittel=[round(float(v), 2) for v in b.reshape(-1, 3).mean(0)], baender=rows)
print(json.dumps(res))
