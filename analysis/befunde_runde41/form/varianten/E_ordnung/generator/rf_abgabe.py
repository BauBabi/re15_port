#!/usr/bin/env python3
"""Rohbau final, Abgabe: python rf_abgabe.py TAG. Blaetter des Laufs lauf/TAG in den Ordner, gerippe_final.json, Atlas-Blick, Kennzahlen je Paar (Volumen links/rechts) nach mess/paare.txt."""
import json
import os
import shutil
import sys

import cv2
import numpy as np

from rf_kern import HIER
import form_emd as FE


def volumen(v, tris):
    a, b, c = v[tris[:, 0]], v[tris[:, 1]], v[tris[:, 2]]
    return abs(float((a * np.cross(b, c)).sum()) / 6.0)


def bone_mesh(f, i):
    m = f['meshes'][i]
    e = np.array(m['ecken'], float)
    t = [d['e'] for d in m['dreiecke']]
    for q in m['vierecke']:
        a, b, c, d = q['e']
        t += [[a, b, d], [a, d, c]]
    return e, np.array(t, int)


def main():
    tag = sys.argv[1]
    d = os.path.join(HIER, 'lauf', tag)
    for n in ('vergleich_bild1.png', 'vergleich_bild2.png', 'teile_bild1.png', 'teile_bild2.png', 'rundum.png', 'rundum_hell.png', 'bericht.txt'):
        q = os.path.join(d, n)
        if os.path.exists(q):
            shutil.copy(q, os.path.join(HIER, n))
    emd = open(os.path.join(HIER, 'FORM.EMD'), 'rb').read()
    f = FE.lese_emd(emd)
    aus = dict(knochen=[dict(index=i, name=k.get('name'), eltern=k['eltern'], versatz=k['versatz']) for i, k in enumerate(f['knochen'])],
               clips=[dict(name=n, kf=c['bilder'][0]['kf']) for n, c in zip(('pose1', 'pose2', 'ruhe'), f['clips'])],
               keyframes=[dict(wurzel=k['wurzel'], winkel=k['winkel']) for k in f['pool']])
    json.dump(aus, open(os.path.join(HIER, 'gerippe_final.json'), 'w'), indent=0)
    # Volumen je Paar
    PAARE = [('Oberschenkel', 8, 9), ('Unterschenkel', 24, 26), ('Fuss', 25, 27), ('Oberarm', 11, 16), ('Unterarm', 12, 17), ('Hand', 13, 18), ('Finger innen', 14, 19), ('Finger Mitte', 15, 20), ('Finger aussen', 28, 29), ('Daumen', 30, 31)]
    os.makedirs(os.path.join(HIER, 'mess'), exist_ok=True)
    with open(os.path.join(HIER, 'mess', 'paare.txt'), 'w', encoding='utf-8') as fh:
        for n, l, r in PAARE:
            vl = volumen(*bone_mesh(f, l))
            vr = volumen(*bone_mesh(f, r))
            z = '%-14s Volumen links/rechts %.2f' % (n, vl / vr)
            print(z)
            fh.write(z + '\n')
    # Atlas-Blick
    t = cv2.imread(os.path.join(HIER, 'zw', 'textur.png'))
    a = json.load(open(os.path.join(HIER, 'atlas.json')))
    b = cv2.resize(t, None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    gesehen = set()
    for x in a:
        r = tuple(x['rechteck'])
        if r in gesehen:
            continue
        gesehen.add(r)
        cv2.rectangle(b, (r[0] * 3, r[1] * 3), ((r[0] + r[2]) * 3, (r[1] + r[3]) * 3), (90, 200, 90), 1)
        cv2.putText(b, x['name'] if x['feld'] is None else x['feld'], (r[0] * 3 + 2, r[1] * 3 + 11), cv2.FONT_HERSHEY_SIMPLEX, .34, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.imwrite(os.path.join(HIER, 'atlas_blick.png'), b)
    shutil.copy(os.path.join(HIER, 'zw', 'textur.png'), os.path.join(HIER, 'textur.png'))
    print('Abgabe aus', d)


if __name__ == '__main__':
    main()
