"""Messblatt: Video 4 h96..h134 (jedes 2. Bild), Ausschnitt x 150-560, y 140-480 (Videoraum 640x480), Raster 50 px."""
import sys, cv2, numpy as np
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
hs = list(range(int(sys.argv[2]), int(sys.argv[3]) + 1, 2))
X0, X1, Y0, Y1 = 150, 560, 140, 480
kacheln = []
for h in hs:
    im = cv2.imread(AN + '/v4/n%04d.png' % (1349 + h))
    c = im[Y0:Y1, X0:X1].copy()
    for x in range(200, X1, 50):
        cv2.line(c, (x - X0, 0), (x - X0, Y1 - Y0), (0, 160, 255), 1)
        cv2.putText(c, str(x), (x - X0 + 2, 12), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 200, 255), 1)
    for y in range(150, Y1, 50):
        cv2.line(c, (0, y - Y0), (X1 - X0, y - Y0), (0, 160, 255), 1)
        cv2.putText(c, str(y), (2, y - Y0 - 2), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 200, 255), 1)
    cv2.putText(c, 'Video h%d' % h, (8, Y1 - Y0 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
    kacheln.append(c)
sp = 5
while len(kacheln) % sp:
    kacheln.append(np.zeros_like(kacheln[0]))
reihen = [np.hstack(kacheln[i:i + sp]) for i in range(0, len(kacheln), sp)]
im = np.vstack(reihen)
kopf = np.zeros((40, im.shape[1], 3), np.uint8)
cv2.putText(kopf, 'Messbild - kein Ergebnis | Video 4 h%d-h%d (Raster 50 px, Videoraum 640x480)' % (hs[0], hs[-1]), (10, 28), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 255), 2)
cv2.imwrite(sys.argv[1], np.vstack([kopf, im]))
print(im.shape)
