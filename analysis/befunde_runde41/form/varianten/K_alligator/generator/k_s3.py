#!/usr/bin/env python3
"""Variante K mit den 28 Clips der Stufe 3 (nur fuer die Loch-Messung, kein Spiel-Asset): Gerippe, Keyframes und Clips aus stufe3/asset/BAHNSTEIG_FORM.EMD
(gleiche Knochen und Versaetze wie J, gemessen: knochen gleich), Meshes und Textur aus der Variante. Die Kopfwinkel der Stufe-3-Clips sind die des alten
Kopfes von J; wie der lange Kopf darin liegt, ist eine Folge fuer Stufe 3 (K.md), hier nicht geaendert.
    python k_s3.py VARIANTE_DIR AUS.EMD"""
import os
import sys

import k_kern as KK

FE = KK.FE
S3 = os.path.join(KK.FORM, 'stufe3', 'asset', 'BAHNSTEIG_FORM.EMD')


def bauen(var, aus):
    s3 = FE.lese_emd(open(S3, 'rb').read())
    k = FE.lese_emd(open(os.path.join(var, 'FORM.EMD'), 'rb').read())
    if [x['versatz'] for x in s3['knochen']] != [x['versatz'] for x in k['knochen']]:
        raise SystemExit('Gerippe der Variante weicht von Stufe 3 ab')
    s3['meshes'] = k['meshes']
    open(aus, 'wb').write(FE.schreibe_emd(s3))
    return aus


if __name__ == '__main__':
    print(bauen(sys.argv[1], sys.argv[2]))
