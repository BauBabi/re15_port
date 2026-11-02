"""Lage der Meldungszeilen im logischen Fensterbild: helle Schriftpixel (min(R,G,B) > 150) je Zeilenband, x-Laeufe (Luecke > 6)."""
import sys, numpy as np, logisch
for p in sys.argv[1:]:
    a, s, e = logisch.lade(p)
    m = a.min(axis=2) > 150
    for y0, y1, n in ((178, 195, 'Zeile 1 (y 0xb4=180)'), (195, 212, 'Zeile 2 (y 0xb4+0x10=196)')):
        xs = np.where(m[y0:y1, 20:300].any(axis=0))[0] + 20
        lauf = []
        if len(xs):
            st = pr = xs[0]
            for x in xs[1:]:
                if x - pr > 6: lauf.append((int(st), int(pr))); st = x
                pr = x
            lauf.append((int(st), int(pr)))
        print('%s %s: x-Laeufe %s' % (p.split('/')[-3] + '/' + p.split('/')[-1], n, lauf))
