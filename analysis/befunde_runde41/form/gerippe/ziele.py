#!/usr/bin/env python3
"""Zielkarten fuer das Gerippe: Nutzer-Teile, gesicherte Leerflaechen, Konflikte. Aufruf ohne Argumente schreibt lauf/ziele.npz + Bericht."""
import json, os
import numpy as np, cv2

F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
T = os.path.join(F, 'nutzer_teile')
A = os.path.join(F, 'gerippe', 'lauf')
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'],
         2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}


def maske(name):
    m = cv2.imread(os.path.join(T, 'maske_%s.png' % name), cv2.IMREAD_UNCHANGED)
    return (m[..., -1] if m.ndim == 3 else m) > 127


def ell(r):
    return cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1, 2 * r + 1))


def ero(m, r):
    return cv2.erode(m.astype(np.uint8), ell(r)) > 0


def dil(m, r):
    return cv2.dilate(m.astype(np.uint8), ell(r)) > 0


def lade():
    z = {}
    for b in (1, 2):
        z[b] = cv2.imread(os.path.join(F, 'ziel_bild%d.png' % b), cv2.IMREAD_GRAYSCALE)
    m = {n: maske(n) for k in TEILE.values() for n in k}
    return z, m


if __name__ == '__main__':
    z, m = lade()
    for b in (1, 2):
        vereinigt = np.zeros_like(z[b], bool)
        for n in TEILE[b]:
            vereinigt |= m[n]
            print('Bild', b, n, 'Flaeche', int(m[n].sum()), 'auf ziel=0:', int((m[n] & (z[b] == 0)).sum()),
                  'auf 128:', int((m[n] & (z[b] == 128)).sum()), 'auf 255:', int((m[n] & (z[b] == 255)).sum()))
        print('Bild', b, 'ziel 255 ausserhalb aller Nutzer-Teile:', int(((z[b] == 255) & ~vereinigt).sum()))
        # Bild: rot = Nutzer auf ziel 0, gelb = ziel 255 ohne Nutzer-Teil, Rest gedimmt
        v = cv2.imread(os.path.join(F, 'bild%d.png' % b)).astype(np.float32) / 255
        v = (v ** 0.6)
        v[(z[b] == 0)] *= 0.55
        v[vereinigt & (z[b] == 0)] = [0, 0, 1]
        v[(z[b] == 255) & ~vereinigt] = [0, 1, 1]
        vv = vereinigt.astype(np.uint8)
        c, _ = cv2.findContours(vv, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
        bb = (np.clip(v, 0, 1) * 255).astype(np.uint8)
        cv2.drawContours(bb, c, -1, (0, 255, 0), 1)
        bb = cv2.resize(bb, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        cv2.imwrite(os.path.join(A, 'konflikt_bild%d.png' % b), bb)
