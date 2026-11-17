"""Alle Clips/Bilder: (a) Schnitt im Raum Kopf 22 / Unterkiefer 23 gegen Brust 10 / Hals 21 (Dreieckspaare), Kopf-/Kiefer-Ecken innerhalb
Brust/Hals (Lot in die Flaeche, Rueckseite der naechsten Flaeche) mit Tiefe; (b) Keil-Bildpunkte in der Bild-2-Kamera (5090 Schnitt 11,
Lage 941/-23776, Gier 3874, Fenster 640 x 480): Kopf/Kiefer gemalt, Brust/Hals liegt geometrisch vorn. Fuer (b) werden nur die Teile
10, 21, 22, 23 gerastert (Arme usw. koennten einzelne Keile zusaetzlich verdecken).
Aufruf: python s_alle.py <K5e_f1|K5c|Krallen-Kandidat>"""
import json
import time
from multiprocessing import Pool
from s_basis import *  # noqa: F401,F403
import s_geo

TEILE = (10, 21, 22, 23)
_K = {}


def _kam():
    R5090 = FB.Raum('5090'); s = R5090.schnitt(11)
    return FB.Kamera.aus_rid(s['pos'], s['ziel'], fov=s['fov'])


def eins(arg):
    ordner, clip, bild = arg
    if ordner not in _K:
        _K[ordner] = (kand(ordner), _kam())
    k, kam = _K[ordner]
    kf = k.kf(clip, bild)
    wt = welt_dreiecke(k.mo, kf)
    T = {b: np.array([V for tag, V in wt if tag[0] == b]) for b in TEILE}
    r = dict(clip=clip, bild=bild)
    for a, b in ((22, 10), (22, 21), (23, 21), (23, 10)):
        r['schnitt_%d_%d' % (a, b)] = len(s_geo.tri_schnitte(T[a], T[b]))
    for a in (22, 23):
        P = np.unique(T[a].reshape(-1, 3), axis=0)
        best = np.zeros(len(P)); drin_any = np.zeros(len(P), bool)
        for b in (10, 21):
            A_, B_, C_ = T[b][:, 0], T[b][:, 1], T[b][:, 2]
            d2, X, innen = s_geo.naechster_punkt(P, A_, B_, C_)
            j = d2.argmin(1); d = np.sqrt(d2[np.arange(len(P)), j]); Xj = X[np.arange(len(P)), j]
            n = -np.cross(B_ - A_, C_ - A_)[j]   # Aussen = -Kreuzprodukt (s_schnitt.py: aussen_vorzeichen_modellraum -1)
            n /= np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
            drin = (((P - Xj) * n).sum(1) < 0) & innen[np.arange(len(P)), j]
            best = np.where(drin & (d > best), d, best); drin_any |= drin
        r['ecken_%d_innen' % a] = int(drin_any.sum()); r['ecken_%d_tiefe_max' % a] = float(best.max())
    tris = [t for t in dreiecke_3d(k.mo, kf, (941, 0, -23776), 3874, kam) if t[4][0] in TEILE]
    R = rastern_beide(tris, k.tex, 480, 640, np.diag([2.0, 2.0, 1.0]))
    tm = tri_mesh(tris)
    ma = np.where(R['maler_i'] >= 0, tm[np.maximum(R['maler_i'], 0)], -1)
    mb = np.where(R['puffer_i'] >= 0, tm[np.maximum(R['puffer_i'], 0)], -1)
    keil = np.isin(ma, (22, 23)) & np.isin(mb, (10, 21))
    r['keil_px'] = int(keil.sum())
    r['keil_tiefe_max'] = float((R['maler_z'] - R['puffer_z'])[keil].max()) if keil.any() else 0.0
    r['brust_ueber_hals_px'] = int(((ma == 10) & (mb == 21)).sum())
    r['figur_px'] = int((R['maler_i'] >= 0).sum())
    return r


if __name__ == '__main__':
    name = sys.argv[1]
    ordner = KANDIDATEN[name]
    k = kand(ordner)
    jobs = [(ordner, c, b) for c, (f0, n) in enumerate(k.mo.clips) for b in range(n)]
    t0 = time.time()
    with Pool(6) as p:
        res = p.map(eins, jobs, chunksize=4)
    json.dump(res, open(AUS + '/alle_%s.json' % name, 'w', encoding='utf-8'), indent=0)
    print(name, len(res), 'Bilder, %.0f s' % (time.time() - t0))
