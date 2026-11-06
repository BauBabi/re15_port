"""SF V4 (Bauer): Posen fuer Riegel und Leisten (Messblatt V4 Abschnitt 7 "Render fuer alle Riegel").

  "85"  seite = Kopie von sfv2/posen_seite.json "85" (clip 1, bild 26, gier 556, x -8070, z -21202, IoU 0,62).
  "135" vorn  = neu angepasst an die Silhouette von n1485 (Vorlage 1, entblendet), Verfahren wie anpassen_v2.suche (IoU der
        Umrisse Knochen 0-15 von Referenz 2, Sohlenmitte per Rueckprojektion, Gitter Clip/Bild/Gier + Feinsuche).
        Silhouette: dE76 (Lab) zwischen Vorlage 1 und dem Port-Raumbild (BSS Schnitt 12 im Videoraum, v4_gemein) nach Gauss
        sigma 2 > 6, Schliessen 7x7, Oeffnen 5x5, groesste 8er-Komponente (Schwellen 8/10 zerfallen in Teile, bau/sil_t*.png). Ausgeblendet (beide Masken): x >= 462 (Tuerpfosten: BSS und Video
        weichen dort auch ohne Figur ab, diff1.png) und y >= 396 (Bodenschatten). Startkandidat zusaetzlich die Spielpose h135
        des Laufs sfv3/r/V3r2 (debug.log "[hangar-takt] h135 F378 B clip 1 bild 45 gier 1513 (-9215,-22383)").
python posen_v4.py   -> posen_v4.json (neben diesem Skript), sil135.png im Scratchpad sfv4/bau
"""
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G
import b2
import anpassen_v2 as AP

X_AB, Y_BIS = 462, 396


def silhouette():
    bg = G.videoraum(G.hintergrund())
    v = G.vorlage(1)
    d = np.sqrt(((G.lab(v) - G.lab(bg)) ** 2).sum(2))
    d = cv2.GaussianBlur(d.astype(np.float32), (0, 0), 2.0)
    m = d > 6
    m[:, X_AB:] = False; m[Y_BIS:] = False; m[:, :280] = False; m[:120] = False
    m = cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((7, 7), np.uint8))
    m = cv2.morphologyEx(m, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
    n, l, st, _c = cv2.connectedComponentsWithStats(m, 8)
    k = 1 + int(np.argmax(st[1:, 4]))
    return l == k


def schneide(m):
    m = m.copy(); m[:, X_AB:] = False; m[Y_BIS:] = False
    return m


def main():
    mo = b2.modell(G.REF2)
    sil = silhouette()
    out = (G.vorlage(1) * 0.5).astype(np.uint8); out[sil] = (255, 255, 0)
    os.makedirs(os.path.join(G.SFV4, 'bau'), exist_ok=True)
    cv2.imwrite(os.path.join(G.SFV4, 'bau', 'sil135.png'), cv2.cvtColor(out, cv2.COLOR_RGB2BGR))
    iou = lambda p: AP.iou(schneide(AP.umriss(mo, p)), sil)
    # Sohlenlage: Silhouette ist unten abgeschnitten -> Lage aus der Spielpose, Suche ueber Gier/Bild/x/z
    start = [dict(clip=1, bild=45, gier=1513, x=-9215, z=-22383)]
    gm = 1513
    for clip in (0, 1):
        for bild in range(0, mo.clips[clip][1], 3):
            for dg in range(-256, 257, 64):
                start.append(dict(clip=clip, bild=bild, gier=(gm + dg) & 0xFFF, x=-9215, z=-22383))
    best = max(((iou(p), p) for p in start), key=lambda t: t[0])
    s, p = best
    for schritt in (64, 32, 16, 8):
        besser = True
        while besser:
            besser = False
            for k, d in (('gier', schritt), ('gier', -schritt), ('bild', 1), ('bild', -1),
                         ('x', 4 * schritt), ('x', -4 * schritt), ('z', 4 * schritt), ('z', -4 * schritt)):
                q = dict(p); q[k] = q[k] + d
                if k == 'bild':
                    q['bild'] %= mo.clips[q['clip']][1]
                if k == 'gier':
                    q['gier'] %= 4096
                t = iou(q)
                if t > s + 1e-4:
                    s, p, besser = t, q, True
    seite = json.load(open(os.path.join(G.SFV2, 'posen_seite.json')))['85']
    R = {'85': dict(seite, quelle='Kopie sfv2/posen_seite.json "85" (Messblatt V4 Abschnitt 7)'),
         '135': dict(p, art='vorn', iou=round(s, 4), x_ab=X_AB, y_bis=Y_BIS,
                     quelle='posen_v4.py: Silhouette n1485 entblendet gegen BSS 50A0/12, IoU Knochen 0-15 (Referenz 2)')}
    json.dump(R, open(G.POSEN, 'w'), indent=1)
    print(R)


if __name__ == '__main__':
    main()
