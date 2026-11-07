import sys, numpy as np
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\analysis\befunde_runde41\AN_belege\werkzeug\n3')
import szmass as SZ
import liege, lage
kf, ks, ents, hdr = liege.rbj_skel_clip()
M = liege.M(kind=0x49, kf=kf, ksize=ks)
k0 = ents[0] & 0xfff
class Wrap:   # Modell-Schnittstelle fuer SZ.idpuffer (clips/frames)
    pass
M.clips = [(0, 1)]; M.frames = [k0]
def mass(P, g, cuts=(11, 12, 13, 14)):
    out = {}
    for c in cuts:
        buf, al = SZ.idpuffer(c, [(M, 0, 0, (int(P[0]), 900, int(P[1])), g, 2)])
        a = al[2]; tot = a.sum(); y0, y1 = SZ.BAND
        band = a[y0:y1].sum() / float((y1 - y0) * 640)
        ys, xs = np.nonzero(a)
        out[c] = (round(100*band, 1), (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())) if len(xs) else None)
    return out
if __name__ == '__main__':
    for P, g in (((4854, -22047), 1170), ((4854, -22380), 1170)):
        print(P, g, mass(P, g))
