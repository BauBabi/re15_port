"""Blatt-Hilfen (wie k5_blatt.schrift/reihe/stapel/kopfzeile, eigenstaendig, damit k5_kern nicht geladen wird)."""
import numpy as np, cv2


def schrift(a, t, x, y, g=0.5, f=(255, 255, 255), d=1):
    cv2.putText(a, t, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, g, (0, 0, 0), d + 2, cv2.LINE_AA)
    cv2.putText(a, t, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, g, f, d, cv2.LINE_AA)


def reihe(kacheln, rand=6, grund=24):
    h = max(k.shape[0] for k in kacheln)
    w = sum(k.shape[1] for k in kacheln) + rand * (len(kacheln) - 1)
    out = np.full((h, w, 3), grund, np.uint8)
    x = 0
    for k in kacheln:
        out[:k.shape[0], x:x + k.shape[1]] = k
        x += k.shape[1] + rand
    return out


def stapel(zeilen, rand=6, grund=24):
    w = max(z.shape[1] for z in zeilen)
    h = sum(z.shape[0] for z in zeilen) + rand * (len(zeilen) - 1)
    out = np.full((h, w, 3), grund, np.uint8)
    y = 0
    for z in zeilen:
        out[y:y + z.shape[0], :z.shape[1]] = z
        y += z.shape[0] + rand
    return out


def kopfzeile(text, breite, h=30, g=0.55, f=(230, 230, 120)):
    a = np.full((h, breite, 3), 24, np.uint8)
    schrift(a, text, 8, h - 10, g, f)
    return a
