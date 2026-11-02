#!/usr/bin/env python3
"""TIM-Vergleich M1 gegen D0, D36, RE2 G1..G5: CLUT-Zeilen, Pixel gleich je Kachel; Differenzbild als PNG."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from PIL import Image
AUS = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
Q = {'M1': zerlege(blob('M1')), 'D0': zerlege(blob('D0')), 'D36': zerlege(blob('D36'))}
for kind in (0x30, 0x31, 0x33, 0x34):
    e, *_ = re2_rec(kind, 3); t, *_ = re2_rec(kind, 2)
    Q[f'R{kind:02X}'] = zerlege(e, t)
m1 = Q['M1']
print('TIM-Kopf M1:', m1.tim[:32].hex(' '))
print('CLUT-Form:', {k: v.pal.shape for k, v in Q.items()}, 'Pixel-Form:', {k: v.tex.shape for k, v in Q.items()})
for k, q in Q.items():
    if k == 'M1': continue
    print(f'--- gegen {k}: TIM md5 {md5(q.tim)}')
    for r in range(3):
        same = (q.pal[r] == m1.pal[r]).all(axis=1).sum()
        print(f'    CLUT {r}: gleiche Eintraege {same}/{m1.pal.shape[1]}')
    h, w = m1.tex.shape
    eq = (q.tex[:h, :w] == m1.tex)
    print(f'    Pixel-Index gleich {eq.sum()}/{eq.size} = {100*eq.mean():.1f}%')
    # nach RGB
    if q.pal.shape == m1.pal.shape:
        # je Halbseite CLUT?
        pass
    # 32er Kacheln
    T = 32
    z = []
    for y in range(0, h, T):
        z.append(''.join('#' if eq[y:y+T, x:x+T].mean() > .98 else ('+' if eq[y:y+T, x:x+T].mean() > .6 else '.') for x in range(0, w, T)))
    print('    Kacheln 32x32 (# >98% gleich, + >60%, . sonst):')
    for l in z: print('      ' + l)
    Image.fromarray((eq*255).astype(np.uint8)).resize((w*2, h*2), Image.NEAREST).save(os.path.join(AUS, f'tim_gleich_M1_{k}.png'))
