# Abgleich gdigrab (echtes Fenster, 2x) gegen FB_SHOT (320x240) desselben Laufs.
import sys
from PIL import Image
import numpy as np
f = np.array(Image.open(sys.argv[1]).convert('RGB')).astype(int)
k = np.array(Image.open(sys.argv[2]).convert('RGB').resize((640, 480), Image.NEAREST)).astype(int)
print('Fenster max', f.max(), 'mittel', round(f.mean(), 1), '| Abzug mittel', round(k.mean(), 1))
best = None
for dy in range(-4, 5):
    for dx in range(-4, 5):
        a = f[max(0, dy):480 + min(0, dy), max(0, dx):640 + min(0, dx)]
        b = k[max(0, -dy):480 + min(0, -dy), max(0, -dx):640 + min(0, -dx)]
        d = np.abs(a - b).max(axis=2)
        g = (d <= 8).mean()
        if best is None or g > best[0]: best = (g, dx, dy, (d > 8).sum())
print('gleich (<=8 je Kanal): %.4f bei Versatz (%d,%d), abweichend %d px' % best)
