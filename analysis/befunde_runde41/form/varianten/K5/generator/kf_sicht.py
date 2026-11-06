#!/usr/bin/env python3
"""K5c Farbe (Runde 1): Sichtkarte Texel -> Bildzone. Rendert K5c in Bild 1 und Bild 2 (form_pruefung.Kandidat, gleiche Kamera/Pose wie
p_riegel) mit drei Kodier-TIMs (Index = Texelkoordinate, CLUT 15 Bit eindeutig) und liest aus dem Albedo-Render je Bildpunkt den Texel.
Ergebnis: je Texel die Zahl der Bildpunkte je Nutzerzone (p_zonen.regionen) in Bild 1 und Bild 2 -> kf_sicht.npz.
Kein RE-Wert: Messwerkzeug fuer die Zuordnung Atlasfeld -> Screenshot-Zone (MESSBLATT_FARBE_V1 Abschnitt 2).

    python kf_sicht.py <Variantenordner> <aus.npz>"""
import json
import os
import shutil
import sys
import tempfile

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'farbe_lesung', 'P_skripte'))
import p_zonen as PZ  # noqa: E402
FE = PZ.P.FE


def kodier_tim(px):
    """eine CLUT-Zeile je Originalzeile (4), Index i -> RGB (i & 31, i >> 5, 0) << 3: eindeutig im 15-Bit-Albedo."""
    cl = [0] + [(i & 31) | ((i >> 5) << 5) for i in range(1, 256)]
    return FE.tim_schreiben(px.astype(np.uint8), [cl] * 4)


def dekodier(alb):
    a = alb.astype(int)
    return (a[..., 0] >> 3) + ((a[..., 1] >> 3) << 5)


def sichtkarte(vdir):
    tim = FE.tim_lesen(open(os.path.join(vdir, 'FORM.TIM'), 'rb').read())
    h, w = tim['pixel'].shape
    assert len(tim['cluts']) == 4, 'K5c-TIM hat 4 CLUT-Zeilen'
    Y, X = np.mgrid[:h, :w]
    kod = dict(A=1 + np.minimum(X // 2, 254), B=1 + np.minimum(Y, 254), C=1 + (X % 2))
    tmp = tempfile.mkdtemp(prefix='kf_sicht_')
    out = {}
    try:
        shutil.copy(os.path.join(vdir, 'FORM.EMD'), tmp)
        shutil.copy(os.path.join(vdir, 'form.json'), tmp)
        idx = {}
        for k, px in kod.items():
            open(os.path.join(tmp, 'FORM.TIM'), 'wb').write(kodier_tim(px))
            kand = PZ.P.Kandidat(os.path.join(tmp, 'form.json'))
            for n in (1, 2):
                sauber, alb, m, kk = kand.rendern(n)
                idx[(k, n)] = (dekodier(alb), m & (alb.max(2) > 0))
        for n in (1, 2):
            A, fig = idx[('A', n)]
            B, _ = idx[('B', n)]
            C, _ = idx[('C', n)]
            tx = 2 * (A - 1) + (C - 1)
            ty = B - 1
            ok = fig & (A > 0) & (B > 0) & (C > 0)
            out['fig%d' % n] = np.zeros((h, w), np.int32)
            np.add.at(out['fig%d' % n], (ty[ok], tx[ok]), 1)
            for name, mz in PZ.regionen(n):
                s = ok & mz
                c = np.zeros((h, w), np.int32)
                np.add.at(c, (ty[s], tx[s]), 1)
                out['b%d|%s' % (n, name)] = c
            # Bildpunkt -> Texel (fuer Rueckprojektion/Tafeln)
            out['px%d_tx' % n] = np.where(ok, tx, -1).astype(np.int16)
            out['px%d_ty' % n] = np.where(ok, ty, -1).astype(np.int16)
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    return out


if __name__ == '__main__':
    o = sichtkarte(sys.argv[1])
    np.savez_compressed(sys.argv[2], **o)
    for k, v in o.items():
        if not k.startswith('px'):
            print('%-28s Texel %6d  Bildpunkte %6d' % (k, int((v > 0).sum()), int(v.sum())))
