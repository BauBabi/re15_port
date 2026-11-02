#!/usr/bin/env python3
"""RGB-Blaetter der TIMs (je 128er-Seite mit eigener CLUT, wie die Flaechen sie lesen) + Template-Matching:
jede 16x16-Kachel von M1 (nicht schwarz) wird in D0 und RE2 G1..G5 gesucht (alle drei CLUT-Seiten), SAD je Pixel."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from PIL import Image
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
Q = {'M1': zerlege(blob('M1')), 'D0': zerlege(blob('D0')), 'D36': zerlege(blob('D36'))}
for kind in (0x30, 0x31, 0x33, 0x34):
    e, *_ = re2_rec(kind, 3); t, *_ = re2_rec(kind, 2)
    Q[f'R{kind:02X}'] = zerlege(e, t)

def seiten_clut(q):
    """Welche CLUT-Zeile nutzen die Flaechen je 128er-Seite? (clut_id>>6)-clut_y"""
    from collections import Counter
    c = {}
    for m in q.meshes:
        for u in m.tuv:
            c.setdefault(u[5] & 0xF, Counter())[(u[2] >> 6) - q.clut_y] += 1
        for u in m.quv:
            c.setdefault(u[5] & 0xF, Counter())[(u[2] >> 6) - q.clut_y] += 1
    return {k: v.most_common() for k, v in sorted(c.items())}

def rgb(q):
    w = q.tex.shape[1] // q.pal.shape[0]
    # Seite i: CLUT i (wird unten gegen seiten_clut geprueft)
    return np.concatenate([q.pal[i][q.tex[:, i*w:(i+1)*w]] for i in range(q.pal.shape[0])], axis=1)

for k, q in Q.items():
    print(k, 'TIM CLUT y', q.clut_y, 'Seiten->CLUT', seiten_clut(q))
R = {k: rgb(q) for k, q in Q.items()}
for k, a in R.items():
    Image.fromarray(a).resize((a.shape[1]*3, a.shape[0]*3), Image.NEAREST).save(os.path.join(AUS, f'tex_{k}.png'))

# Template-Matching 16x16 auf Luminanz + Farbe
def suche(ziel, quelle, B=16, S=1):
    h, w = ziel.shape[:2]
    H, W = quelle.shape[:2]
    Z = ziel.astype(np.int32); Qq = quelle.astype(np.int32)
    erg = np.full((h//B, w//B), 1e9)
    lage = {}
    for yy in range(h//B):
        for xx in range(w//B):
            t = Z[yy*B:yy*B+B, xx*B:xx*B+B]
            if t.sum(axis=2).mean() < 12:      # schwarz
                erg[yy, xx] = np.nan
                continue
            best = 1e9; bl = None
            for y in range(0, H-B+1, S):
                for x in range(0, W-B+1, S):
                    d = np.abs(Qq[y:y+B, x:x+B] - t).mean()
                    if d < best:
                        best = d; bl = (x, y)
            erg[yy, xx] = best
            lage[(xx*B, yy*B)] = (bl, best)
    return erg, lage

if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'suche':
    z = R['M1']
    karte = {}
    for k in ('D0', 'R30', 'R31', 'R33', 'R34', 'D36'):
        e, l = suche(z, R[k], 16, 2)
        karte[k] = (e, l)
        print(k, 'gueltige Kacheln', np.isfinite(e).sum(), 'SAD<8:', np.nansum(e < 8), 'SAD<16:', np.nansum(e < 16), 'Median', np.nanmedian(e))
    np.save(os.path.join(AUS, 'sad_karte.npy'), {k: v[0] for k, v in karte.items()}, allow_pickle=True)
    # Beste Quelle je Kachel
    ks = list(karte)
    st = np.stack([np.nan_to_num(karte[k][0], nan=1e9) for k in ks])
    be = st.argmin(0); mi = st.min(0)
    print('Beste Quelle je 16er-Kachel (Zeichen: D=D0 1=R30 2=R31 3=R33 4=R34 6=D36, kleingeschrieben/. = SAD>=12, Leerzeichen schwarz):')
    zeichen = 'D1234' + '6'
    for y in range(be.shape[0]):
        s = ''
        for x in range(be.shape[1]):
            if mi[y, x] >= 1e8: s += ' '
            elif mi[y, x] >= 12: s += '.'
            else: s += zeichen[be[y, x]]
        print('   ' + s)
