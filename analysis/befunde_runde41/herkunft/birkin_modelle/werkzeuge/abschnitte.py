#!/usr/bin/env python3
"""Abschnittstabelle je Fassung (md5 je dir-Eintrag), plus RE2-Birkin kinds."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
N = ['d0', 'edd1', 'emr2', 'edd3', 'emr4', 'edd5', 'emr6', 'md1', 'tim']
def zeile(name, text, b, re2_tim=None):
    mo = zerlege(b, re2_tim)
    k = kennzahlen(mo)
    print(f'{name:5s} {text}\n      Laenge {len(b)} md5 {hashlib.md5(b).hexdigest()} n={mo.n}')
    print('      ' + ' '.join(f'{N[i]}:{len(mo.sek[i])}:{md5(mo.sek[i])}' for i in range(mo.n)) +
          (f' tim-extern:{len(re2_tim)}:{md5(re2_tim)}' if re2_tim else ''))
    print(f'      Meshes {k["meshes"]} Tri {k["tris"]} Quads {k["quads"]} Verts {k["verts"]} Knochen {k["knochen"]} Keyframes {k["kf"]} je {k["kfgr"]} Clips {k["clips"]}' +
          (f' TIM {mo.tex.shape[1]}x{mo.tex.shape[0]} CLUTs {mo.pal.shape[0]}' if mo.tex is not None else ''))
    # weitere Baenke
    for pr, (e, r) in {'eigen': (3, 4), 'opfer': (5, 6)}.items():
        try:
            cl, fr = EA.edd_parse(mo.sek[e])
            print(f'      Bank {pr}: {len(cl)} Clips, {len(fr)} Bilder, Clip-Laengen {[c[1] for c in cl]}')
        except Exception as ex:
            print(f'      Bank {pr}: nicht lesbar ({ex})')
    print(f'      Bank loco: {len(mo.clips)} Clips, Clip-Laengen {[c[1] for c in mo.clips]}')
    return mo
for n, (p, s, l, t) in FASSUNGEN.items():
    zeile(n, t, blob(n))
for kind, name in ((0x30, 'G1'), (0x31, 'G2'), (0x33, 'G3'), (0x34, 'G4'), (0x36, 'G5'), (0x37, 'Tentakel')):
    emd, se, le = re2_rec(kind, 3)
    tim, st, lt = re2_rec(kind, 2)
    if emd is None:
        print(f'RE2 kind 0x{kind:02X}: kein EMD'); continue
    zeile(f'R{kind:02X}', f'RE2 kind 0x{kind:02X} {name} (EMD Sektor {se} {le} B, TIM Sektor {st} {lt} B)', emd, tim)
