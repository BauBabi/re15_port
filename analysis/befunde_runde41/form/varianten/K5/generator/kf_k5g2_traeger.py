#!/usr/bin/env python3
"""K5g2: Fleck-Texel JE TRAEGERFLAECHE aus der Grundlage v2 (zonen_nutzer/v2/skripte/g_texel.py, Mehrheitsregel je (Flaeche, Texel)).

zonen_v2.json speichert je Fleck-Zone nur die Vereinigung der Texel. Viele Traegerflaechen benutzen dieselben Texel an verschiedenen Stellen des
Koerpers (Buckel/Hals: jedes Zone-27-Texel liegt im Mittel auf 11 Traegerflaechen). Damit der Fleck nur dort erscheint, wo der Nutzer ihn
gezeichnet hat, braucht der Bau die Zuordnung je Flaeche, die g_texel.texel_zone intern bildet (`traeger`: Flaeche -> getroffene Texel).
Dieses Skript ruft dieselbe Rechnung (gleiche Abtastung, gleiche Mehrheitsregel, gleicher Lueckenschluss) und schreibt
    k5g/K5g2/traeger_texel.json  {zone-nr: {"m|v|i": [[u, v], ...]}}
Gegenprobe (sonst Abbruch): Vereinigung je Zone = zonen_v2.json `texel.liste` (u, v) und Traegermenge = modell_zuordnung.liste.
Lueckenschluss-Texel (insgesamt 1) gehen an jede Traegerflaeche, in deren UV-Bereich sie liegen (wie g_texel).

    python kf_k5g2_traeger.py      (GRUNDLAGE_ARBEIT = Arbeitsordner der Grundlage, Standard wie g_basis.py)
"""
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
SK = os.path.join(K5, 'zonen_nutzer', 'v2', 'skripte')
sys.path.insert(0, SK)
os.environ.setdefault('GRUNDLAGE_ARBEIT', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/grundlage')
import numpy as np  # noqa: E402
import g_texel as GT  # noqa: E402

AUS = os.path.join(K5, 'k5g', 'K5g2', 'traeger_texel.json')


def je_flaeche(zid, teile, ohne):
    """= g_texel.texel_zone bis zur Traeger-Zuordnung (Zeilen 111-149 dort), Rueckgabe {(mesh, flaeche): set(Texelindex v*512+u)}."""
    z = GT.ZR[zid]; n = z['bild']; mg = GT.MS[zid]
    H, W = mg.shape
    ys, xs = np.nonzero(mg)
    RAND = 12
    box = (max(int(xs.min()) - RAND, 0), max(int(ys.min()) - RAND, 0), min(int(xs.max()) + RAND + 1, W), min(int(ys.max()) + RAND + 1, H))
    K = GT.render_box(n, box, ohne)
    x0, y0, x1, y1 = box
    Mb = np.repeat(np.repeat(mg[y0:y1, x0:x1], GT.F, 0), GT.F, 1)
    modell = K['maske'] & (np.isin(K['mesh'], teile) if teile is not None else True)
    ti_all = (K['v'][modell].astype(np.int64) * GT.TW + K['u'][modell]).astype(np.int64)
    ky_all = K['mesh'][modell].astype(np.int64) * 1000000 + K['flaeche'][modell]
    in_all = Mb[modell]
    code = ky_all * (GT.TH * GT.TW) + ti_all
    _u, inv = np.unique(code, return_inverse=True)
    n_alle = np.bincount(inv); n_in = np.bincount(inv, weights=in_all.astype(float))
    gilt = (n_in / n_alle) >= 0.5
    sel = gilt[inv] & in_all
    idx = ti_all[sel]; key = ky_all[sel]
    traeger = {}
    for kk, i in zip(key.tolist(), idx.tolist()):
        traeger.setdefault((kk // 1000000, kk % 1000000), set()).add(i)
    getroffen = np.zeros(GT.TH * GT.TW, bool); getroffen[idx] = True
    G = getroffen.reshape(GT.TH, GT.TW)
    nb = np.zeros_like(G)
    nb[1:-1, 1:-1] = G[:-2, 1:-1] & G[2:, 1:-1] & G[1:-1, :-2] & G[1:-1, 2:]
    kandidat = nb & ~G
    imtr = np.zeros(GT.TH * GT.TW, bool)
    for f in traeger:
        imtr[GT.UVB[f]] = True
    zu = np.nonzero(kandidat.reshape(-1) & imtr)[0]
    for i in zu.tolist():
        for f in traeger:
            if i in GT.UVS[f]:
                traeger[f].add(i)
    return traeger


def main():
    Z = json.load(open(os.path.join(K5, 'zonen_nutzer', 'v2', 'zonen_v2.json'), encoding='utf-8'))
    zid_nr = {z['id']: z for z in Z['zonen']}
    out = {}
    for zid, teile, ohne in GT.TEXEL_ZONEN:
        z = zid_nr[zid]
        tr = je_flaeche(zid, teile, ohne)
        verein = set()
        for s in tr.values():
            verein |= s
        soll = set(int(t[3]) * GT.TW + int(t[2]) for t in z['texel']['liste'])
        if verein != soll:
            raise SystemExit('Zone %s: Vereinigung %d != zonen_v2 %d' % (z['nr'], len(verein), len(soll)))
        soll_tr = set(z['modell_zuordnung']['liste'])
        ist_tr = set('%d|%d|%d' % (f[0], f[1] // 100000, f[1] % 100000) for f in tr)
        if soll_tr != ist_tr:
            raise SystemExit('Zone %s: Traeger %s != %s' % (z['nr'], sorted(ist_tr), sorted(soll_tr)))
        out[z['nr']] = {'%d|%d|%d' % (f[0], f[1] // 100000, f[1] % 100000): [[i % GT.TW, i // GT.TW] for i in sorted(s)] for f, s in sorted(tr.items())}
        print(z['nr'], zid, 'Texel', len(soll), 'Paare', sum(len(s) for s in tr.values()), 'Traeger', len(tr), flush=True)
    json.dump(dict(quelle='kf_k5g2_traeger.py (g_texel.texel_zone, Mehrheitsregel je Flaeche und Texel)', zonen=out),
              open(AUS, 'w', encoding='utf-8'), indent=0)


if __name__ == '__main__':
    main()
