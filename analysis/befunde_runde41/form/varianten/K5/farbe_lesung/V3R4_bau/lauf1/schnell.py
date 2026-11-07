"""Bauer V3 R4: schnelle RV1-RV4-Auswertung einer CLUT-Tabelle ueber das ID-Raster (ids.py). Gleiche Masse wie riegel_rot.py."""
import sys, os, json, math, time
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import ids as I
RR_DIR = I.K5 + '/farbe_lesung/V3R4_skripte'
sys.path.insert(0, RR_DIR)
import riegel_rot as RR
PZ = I.PZ
M = np.load(os.path.join(RR_DIR, 'rot_masken.npz'))
Z = json.load(open(os.path.join(RR_DIR, 'ziele.json'), encoding='utf-8'))
E = json.load(open(os.path.join(RR_DIR, 'mess.json'), encoding='utf-8'))
_S = {}
def szenen():
    if not _S:
        for n in (1, 2):
            d = np.load(os.path.join(HIER, 'ids%d.npz' % n))
            _S[n] = {k: d[k] for k in d.files}
    return _S

def bewerten(lut, saaten=RR.SAATEN, bilder=(1, 2), still=True):
    S = szenen()
    res, rot = {}, 0
    for n in bilder:
        sauber, alb, m = I.sauber_aus(S[n], lut)
        basis = np.unpackbits(M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool)
        reg = M['reg%d' % n].astype(int)
        A0 = E['bilder'][str(n)]['A0']['T']
        basis = basis & m & (alb.max(2) > 0)
        Ls = [RR.lab(PZ.rueck(np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0]) for s in saaten]
        for rn in RR.REGIONEN + ['Figur']:
            z = Z.get('bild%d|%s|T' % (n, rn))
            if not z:
                continue
            rm = basis & ((reg == RR.REGIONEN.index(rn)) if rn != 'Figur' else (reg >= 0))
            ms = [RR.masse(L, rm, A0) for L in Ls]
            for mk, name in RR.RIEGEL.items():
                x = z.get(mk)
                if not x or not x.get('ziel'):
                    continue
                v = [q[mk] for q in ms if q[mk] is not None]
                if not v:
                    continue
                w = float(np.mean(v)); lo, hi = x['ziel']
                ok = (w <= hi) if mk == 'gross_eng_p' else (lo <= w <= hi)
                bind = x['bindend']
                rot += int(bind and not ok)
                res['B%d %s %s' % (n, rn, mk)] = dict(w=round(w, 2), lo=lo, hi=hi, ok=bool(ok), b=bool(bind), o=x.get('O'))
    t = sum(1 for v in res.values() if v['b'])
    return res, t - rot, t

def drucke(res):
    for k, v in res.items():
        print('  %-34s %7.2f [%7.2f..%7.2f] %s%s' % (k, v['w'], v['lo'], v['hi'], 'GRUEN' if v['ok'] else 'ROT', '' if v['b'] else ' (Info)'))

if __name__ == '__main__':
    k = I.P.Kandidat(os.path.join(sys.argv[1], 'form.json'))
    t = I.FE.tim_lesen(k.tim)
    lut = I.lut_aus(t['cluts'])
    t0 = time.time()
    r, g, n = bewerten(lut)
    drucke(r)
    print('%d/%d  (%.1f s)' % (g, n, time.time() - t0))
