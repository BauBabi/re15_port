import sys, cv2, numpy as np
o = sys.argv[1]
for n, s in ((1, (1.0, 660)), (2, (1.0, 620))):
    a = cv2.imread('%s/ausschnitt_bild%d.png' % (o, n))
    h, w = a.shape[:2]
    # Tafeln: 3 Spalten
    pad, oben = 12, 34
    bw = (w - pad) // 3 - pad
    bh = (h - oben) // 2 - 38
    t = [a[oben:oben + bh, pad + i * (bw + pad):pad + i * (bw + pad) + bw] for i in (0, 2)]
    m = np.hstack(t)
    f = s[1] * 2 / m.shape[1] * 1.0
    m = cv2.resize(m, None, fx=f, fy=f, interpolation=cv2.INTER_AREA)
    cv2.imwrite('%s/vgl%d.png' % (o, n), m)
    print(m.shape)
