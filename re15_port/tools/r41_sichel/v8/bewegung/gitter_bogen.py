"""Messblatt Bogen: Video 4 h84..h96 (jedes 2. Bild), Ausschnitt x 200-420 / y 160-380, 3x, Raster 10 px (Beschriftung je 20 px)."""
import sys, cv2, numpy as np
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
X0, X1, Y0, Y1, F = 200, 420, 160, 380, 3
k = []
for h in range(84, 97, 2):
    im = cv2.imread(AN + '/v4/n%04d.png' % (1349 + h))[Y0:Y1, X0:X1]
    im = cv2.resize(im, None, fx=F, fy=F, interpolation=cv2.INTER_CUBIC)
    for x in range(X0, X1 + 1, 10):
        c = (0, 140, 255) if x % 20 == 0 else (0, 70, 130)
        cv2.line(im, ((x - X0) * F, 0), ((x - X0) * F, (Y1 - Y0) * F), c, 1)
        if x % 20 == 0: cv2.putText(im, str(x), ((x - X0) * F + 2, 12), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 220, 255), 1)
    for y in range(Y0, Y1 + 1, 10):
        c = (0, 140, 255) if y % 20 == 0 else (0, 70, 130)
        cv2.line(im, (0, (y - Y0) * F), ((X1 - X0) * F, (y - Y0) * F), c, 1)
        if y % 20 == 0: cv2.putText(im, str(y), (2, (y - Y0) * F - 2), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 220, 255), 1)
    cv2.putText(im, 'Video h%d' % h, (8, (Y1 - Y0) * F - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
    k.append(im)
while len(k) % 4: k.append(np.zeros_like(k[0]))
im = np.vstack([np.hstack(k[i:i + 4]) for i in range(0, len(k), 4)])
kopf = np.zeros((40, im.shape[1], 3), np.uint8)
cv2.putText(kopf, 'Messbild - kein Ergebnis | Video 4 h84-h96, x 200-420 y 160-380, 3x, Raster 10 px', (10, 28), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 255), 2)
cv2.imwrite(sys.argv[1], np.vstack([kopf, im]))
print(im.shape)
