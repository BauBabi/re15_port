"""Zahlen-Pruefer V4 r3: T2 nach dem EINEN Werkzeug t2_mass_v4.py (Entscheid O2) - Drehung UND Lage der Biegung (Mitte des Fensters
mit der groessten Drehung) am Original (3 Leser-Masken) und an V4 r3 / V4 r2 / V3 r2 (Renders sfv4/pz3, Pose 85).
Gegenprobe: Masken in v4/t2_masken == Leser-Masken sfv4/schwanz_maske*.npy; eigene Mittellinie pz2_t2orig-Variante (Sehnen)."""
import os, sys, json, numpy as np
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import t2_mass_v4 as T2M
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
out = {}
for f, g in zip(T2M.MASKEN, ('schwanz_maske.npy', 'schwanz_maske_hi.npy', 'schwanz_maske_lo.npy')):
    d = np.load(f); t = np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
    ref = np.load(SP + g).astype(bool)
    ys, xs = np.nonzero(t); seed = (int(xs.max()), int(ys[np.argmax(xs)]))
    P, s = T2M.linie(t, seed); w, mitte, prof = T2M.drehung(P, s, T2M.H2, T2M.BEGINN_X_ORIG - 0.10 * T2M.H2)
    out['orig_' + g] = dict(maske_gleich_leser=bool(t.shape == ref.shape and np.array_equal(t, ref)), drehung=round(w, 1),
                            mitte=[round(float(v), 1) for v in mitte],
                            rel_beginn=[round((mitte[0] - 357) / T2M.H2, 3), round((mitte[1] - 243) / T2M.H2, 3)])
o = T2M.original(); out['original'] = o
M = json.load(open(SP + 'pz3/mess_V4_V4R2_V3.json'))
for nm in ('V4', 'V4R2', 'V3'):
    d = np.load(SP + 'pz3/r_%s_85.npz' % nm); ids = d['ids']; H = M[nm]['seite']['H']
    r = T2M.port(ids, H)
    # Beginn nach Messblatt-Wortlaut T1 (unterster Id-16-Pixel an Id 7/8, 4er) wie pz3_schwanz
    import cv2
    kr = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]], np.uint8)
    m = (ids == 16) & (cv2.dilate(np.isin(ids, (7, 8)).astype(np.uint8), kr) > 0)
    ys, xs = np.nonzero(m); i = int(np.argmax(ys + 1e-3 * xs)); b = (int(xs[i]), int(ys[i]))
    out[nm] = dict(H=H, drehung=round(r['drehung'], 1), mitte=[round(float(v), 1) for v in r['mitte']], beginn_t1=b, beginn_t2m=r['beginn'],
                   rel_beginn_t1=[round((r['mitte'][0] - b[0]) / H, 3), round((r['mitte'][1] - b[1]) / H, 3)],
                   ok_drehung=bool(abs(r['drehung'] - o['ziel']) <= o['tol']),
                   profil=[p for p in r['profil'] if 0.15 <= p[0] <= 0.6])
json.dump(out, open(SP + 'pz3/t2.json', 'w'), indent=1)
for k, v in out.items(): print(k, v)
