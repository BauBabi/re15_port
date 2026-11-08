import sys, glob, os
from PIL import Image
import numpy as np
S = sys.argv[1]
for name in ('pose1_r5080_s1', 'pose2_r5090_s11'):
    for f in sorted(glob.glob(os.path.join(S, 'bild', 'vor', name, '*.ppm'))) + [os.path.join(S, 'bild', 'vor', name + '.png')]:
        g = f.replace(os.sep + 'vor' + os.sep, os.sep + 'nach' + os.sep).replace('/vor/', '/nach/')
        a = np.array(Image.open(f).convert('RGB')).astype(int); b = np.array(Image.open(g).convert('RGB')).astype(int)
        d = np.abs(a - b).max(axis=2)
        ys, xs = np.nonzero(d)
        print('%s %s %dx%d: Pixel-Diff %d, max %d, Bild nicht leer: %d Pixel > 8%s' % (name, os.path.basename(f), a.shape[1], a.shape[0], len(ys), d.max(),
              int((a.max(axis=2) > 8).sum()), (' Ort x %d..%d y %d..%d' % (xs.min(), xs.max(), ys.min(), ys.max())) if len(ys) else ''))
