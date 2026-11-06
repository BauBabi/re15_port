"""Spur SF Schwanz V2, Koerper (Formblatt V2 K3-K7): Masse der Vorderansicht in der Spielkamera, in H.

Bund   = oberste Zeile mit Jeans-Bildpunkten (Knochen 0/1/4, blau: b > r + 8) unter der Rumpfmitte
Saum   = unterste Zeile mit Jeans-Bildpunkten an den Beinen
Breite = Umrissbreite (Koerper ohne Schwanz) in 0,2 H unter der Oberkante
Krallen= tiefster Bildpunkt der Klaue (Knochen 12) ueber der Sohle; Sichel = tiefster Bildpunkt von Knochen 15
python koerper_mass.py <emd> <posen.json>
"""
import sys, os, json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2


def koerper_mass(mo, pose):
    im, ids = b2.zeichne(mo, pose, None)
    koerper = (ids >= 0) & (ids < 16)
    ys, xs = np.nonzero(koerper)
    ok, sohle = ys.min(), ys.max()
    H = float(sohle - ok)
    r, g, b = [im[..., k].astype(int) for k in range(3)]
    jeans = np.isin(ids, (0, 1, 4)) & (b > r + 8)
    jy = np.nonzero(jeans.any(1))[0]
    bund = jy.min() if len(jy) else None
    beine = jeans & np.isin(ids, (1, 4))
    by = np.nonzero(beine.any(1))[0]
    saum = by.max() if len(by) else None
    y2 = int(round(ok + 0.2 * H))
    row = np.nonzero(koerper[y2])[0]
    breite = (row.max() - row.min() + 1) / H if len(row) else 0
    out = dict(H=H, ok=int(ok), sohle=int(sohle), bund=(bund - ok) / H if bund is not None else None,
               saum=(saum - ok) / H if saum is not None else None, breite=breite)
    for name, kb in (('krallen', 12), ('sichel', 15)):
        yy = np.nonzero((ids == kb).any(1))[0]
        out[name] = (sohle - yy.max()) / H if len(yy) else None
    return out


def text(m):
    f = lambda v: '-' if v is None else '%.2f' % v
    return ['Bund %s / Saum %s H unter OK, Breite %s H' % (f(m['bund']), f(m['saum']), f(m['breite'])),
            'Krallen %s / Sichel %s H ueber Sohle' % (f(m['krallen']), f(m['sichel']))]


if __name__ == '__main__':
    mo = b2.modell(sys.argv[1])
    for h, pose in json.load(open(sys.argv[2])).items():
        m = koerper_mass(mo, pose)
        print('h%s H %.0f | %s' % (h, m['H'], ' | '.join(text(m))))
