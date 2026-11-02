"""Fensterbild (Skalierung s) -> logisches 320x240 (Mitte jedes s*s-Blocks) + Pruefung, ob jeder Block einfarbig ist (nearest, Pixel 1:1)."""
import numpy as np
from PIL import Image

def lade(p):
    a = np.array(Image.open(p).convert('RGB')).astype(int)
    s = a.shape[1] // 320
    assert a.shape[0] == 240 * s and a.shape[1] == 320 * s, a.shape
    if s == 1:
        return a, 1, 1.0
    b = a.reshape(240, s, 320, s, 3)
    m = b[:, s // 2, :, s // 2, :]
    einfarbig = (np.abs(b - m[:, None, :, None, :]).max(axis=(1, 3, 4)) == 0).mean()
    return m, s, float(einfarbig)
