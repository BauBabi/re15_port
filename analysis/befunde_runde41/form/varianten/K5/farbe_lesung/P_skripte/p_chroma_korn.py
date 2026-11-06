"""Pruefer: Farbkorn = Streuung des a*b*-Rests nach Glaettung (sigma 1 Texel, normierte Faltung im Feld), Lab-Einheiten, Innentexel. Mass fuer E13 'Mischmasch'."""
import numpy as np, cv2
from p_lade import lab
from p_messen import lade_alle, zonen_von

def korn(rgb, zone, sig=1.0):
    L = np.zeros(zone.shape + (3,)); L[zone] = lab(rgb[zone]); m = zone.astype(np.float32)
    inn = cv2.erode(zone.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    r2 = 0
    for c in (1, 2):
        ch = (L[..., c] * m).astype(np.float32)
        g = cv2.GaussianBlur(ch, (0, 0), sig) / np.maximum(cv2.GaussianBlur(m, (0, 0), sig), 1e-6)
        r2 = r2 + ((L[..., c] - g)[inn]) ** 2
    return float(np.sqrt(r2.mean()))

if __name__ == '__main__':
    M = lade_alle(); Z = zonen_von(M)
    for zn in ('EM30_haut', 'EM30_kopf', 'G4_koerper', 'G4_kopf', 'G5_koerper', 'G5_maul', 'Hund_koerper', 'Zombie_koerper', 'C_mitte_koerper', 'D_fleisch_koerper', 'J_koerper', 'K5c_koerper', 'K5c_kopf', 'K5c_kiefer'):
        nm, z, _ = Z[zn]; print('%-20s Farbkorn %.2f' % (zn, korn(M[nm][3], z)))
