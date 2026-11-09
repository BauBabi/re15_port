"""Kette T je Stand und Bild: PZ.rueck(video_filter(sauber, Saat)) fuer die 3 Saaten (wie riegel_rot / kralle_rv), dazu Original T. Cache als npz."""
import os, sys, pickle
import numpy as np
import v4f


def lade(cache, nm):
    return pickle.load(open(cache + '/cache_%s.pkl' % nm.replace(' ', '_'), 'rb'))


def kette(cache, nm, n):
    p = cache + '/kette_%s_%d.npz' % (nm.replace(' ', '_'), n)
    if os.path.exists(p):
        return np.load(p)
    d = lade(cache, nm)[n]
    Ts = [v4f.PZ.rueck(d['video'][s], n)[0] for s in v4f.SAATEN]
    e = v4f.FV.video_umkehr(d['video'][15], bild=n)
    np.savez_compressed(p, T=np.stack(Ts).astype(np.float32), e=np.asarray(e, np.float32))
    return np.load(p)


def original(cache, n):
    p = cache + '/kette_Original_%d.npz' % n
    if os.path.exists(p):
        return np.load(p)
    o = v4f.original(n)
    T, clip = v4f.PZ.rueck(o, n)
    e = v4f.FV.video_umkehr(o, bild=n)
    np.savez_compressed(p, T=T[None].astype(np.float32), e=np.asarray(e, np.float32), clip=clip)
    return np.load(p)


if __name__ == '__main__':
    c = sys.argv[1]
    for n in (1, 2):
        original(c, n)
        for nm, _o in v4f.STAENDE:
            kette(c, nm, n)
            print(nm, n, flush=True)
