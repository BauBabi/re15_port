import cv2, numpy as np, glob, os, sys
SP = 'k5g3tex/spiel'
for tag in sys.argv[1:]:
    n90 = n85 = tot = 0
    for f in sorted(glob.glob('%s/f80_riegel_%s_normal/dump_*.ppm' % (SP, tag))):
        k = f.replace('_normal', '_key')
        a = cv2.imread(f).astype(np.float32); ky = cv2.imread(k).astype(np.float32)
        m = (a != ky).any(2) & (ky.mean(2) >= 24)
        q = np.clip(a / np.maximum(ky, 1) * 255, 0, 255).astype(np.uint8)
        L = cv2.cvtColor(q.astype(np.float32) / 255., cv2.COLOR_BGR2Lab)[..., 0]
        n90 += int((L[m] > 90).sum()); n85 += int((L[m] > 85).sum()); tot += int(m.sum())
    print(tag, 'Q-Pixel', tot, 'L*>90', n90, 'L*>85', n85)
