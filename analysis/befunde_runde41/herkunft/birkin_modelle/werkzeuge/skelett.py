#!/usr/bin/env python3
"""Skelett + Keyframes: D0 gegen M1 (Bind-Offsets, Hierarchie, Keyframe-Gleichheit, Clip-Tabelle), Morph-Block d0,
RE2-G2-Hierarchie, Wolverine-Kachel: Flaechen, die in das Rechteck zeigen."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from PIL import Image
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
D0, M1 = zerlege(blob('D0')), zerlege(blob('M1'))
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2); G2 = zerlege(e, t)
e, *_ = re2_rec(0x30, 3); t, *_ = re2_rec(0x30, 2); G1 = zerlege(e, t)
print('D0 Knochen:', D0.skel.bone_count, 'M1:', M1.skel.bone_count, 'kfgr', D0.skel.kf_size, M1.skel.kf_size)
print('Eltern D0:', D0.skel.parent)
print('Eltern M1:', M1.skel.parent)
print('rel gleich:', D0.skel.rel == M1.skel.rel)
for b in range(16):
    print(f'  b{b:2d} D0 rel {D0.skel.rel[b]}  M1 rel {M1.skel.rel[b]}')
print('Eltern G2:', G2.skel.parent)
print('rel G2:', G2.skel.rel)
print('Eltern G1:', G1.skel.parent)
print('rel G1:', G1.skel.rel)

# Keyframes
kd = D0.skel.kf_data; km = M1.skel.kf_data; ks = 84
nd, nm = len(kd)//ks, len(km)//ks
print('Keyframes', nd, nm, 'Reste', len(kd) % ks, len(km) % ks)
D = {kd[i*ks:(i+1)*ks]: i for i in range(nd)}
treffer = [D.get(km[i*ks:(i+1)*ks], -1) for i in range(nm)]
print('M1-Keyframes bytegleich in D0:', sum(1 for x in treffer if x >= 0), '/', nm)
# fortlaufend?
versatz = {}
for i, x in enumerate(treffer):
    versatz.setdefault(x - i if x >= 0 else None, []).append(i)
print('Versatz -> Bereich:', {k: (min(v), max(v), len(v)) for k, v in versatz.items()})
# EDD: Frames
print('Clips D0', [c for c in D0.clips])
print('Clips M1', [c for c in M1.clips])
fd, fm = D0.frames, M1.frames
print('EDD-Bilder D0', len(fd), 'M1', len(fm))
dd = set()
# Clip 1 und Clip 2 je vergleichen
for c in range(21):
    a = D0.clips[c]; b = M1.clips[c]
    fa = fd[a[0]:a[0]+a[1]]; fb = fm[b[0]:b[0]+b[1]]
    glei = fa == fb
    if not glei:
        # relativ?
        ra = [(x & 0xfff) for x in fa]; rb = [(x & 0xfff) for x in fb]
        print(f'  Clip {c}: D0 {len(fa)} Bilder kf {ra[:6]}.. Flags {[hex(x>>12) for x in fa[:3]]} | M1 {len(fb)} Bilder kf {rb[:6]}.. Flags {[hex(x>>12) for x in fb[:3]]}')
# Clip 1 M1 = Teil von Clip 2 ?
def kfb(mo, f):
    return mo.skel.kf_data[(f & 0xfff)*84:(f & 0xfff)*84+84]
c1 = fm[M1.clips[1][0]:M1.clips[1][0]+M1.clips[1][1]]
c2 = fm[M1.clips[2][0]:M1.clips[2][0]+M1.clips[2][1]]
d1 = fd[D0.clips[1][0]:D0.clips[1][0]+D0.clips[1][1]]
print('M1 Clip1 Keyframes:', [x & 0xfff for x in c1])
print('M1 Clip2 Keyframes:', [x & 0xfff for x in c2])
print('D0 Clip1 Keyframes:', [x & 0xfff for x in d1])
print('M1 Clip1 == M1 Clip2 (Keyframe-Bytes):', [kfb(M1, a) == kfb(M1, b) for a, b in zip(c1, c2)].count(True), '/', len(c1))
print('M1 Clip1 == D0 Clip2?', sum(kfb(M1, a) == kfb(D0, b) for a, b in zip(c1, fd[D0.clips[2][0]:D0.clips[2][0]+D0.clips[2][1]])))
print('M1 Clip1 in D0 Clip1 Bildern:', [next((i for i, b in enumerate(d1) if kfb(D0, b) == kfb(M1, a)), -1) for a in c1])
# Morph-Block
d = D0.sek[0]
print('d0 Laenge', len(d), 'Kopf', d[:64].hex(' '))
mask = u32(d, 0)
print('Maske', hex(mask), 'Worte', [hex(u32(d, o)) for o in range(0, 40, 4)])
# Wolverine: Rechteck in TIM (1x-Koord.)
x0, y0, x1, y1 = 298, 4, 330, 34
print('Flaechen auf Rechteck TIM x', x0, x1, 'y', y0, y1)
n = 0
for i, m in enumerate(M1.meshes):
    for (uvl, tag) in ((m.tuv, 'T'), (m.quv, 'Q')):
        for f, u in enumerate(uvl):
            page = (u[5] & 0xF) * 128
            pts = [(u[0], u[1]), (u[3], u[4]), (u[6], u[7])] + ([(u[9], u[10])] if tag == 'Q' else [])
            xs = [p[0] + page for p in pts]; ys = [p[1] for p in pts]
            # Ueberlappung Bounding-Box
            if max(xs) >= x0 and min(xs) <= x1 and max(ys) >= y0 and min(ys) <= y1:
                n += 1
                print(f'   mesh {i} {tag}{f}: page+{page} x {min(xs)}..{max(xs)} y {min(ys)}..{max(ys)}')
print('Summe', n)
rgb = np.concatenate([M1.pal[i][M1.tex[:, i*128:(i+1)*128]] for i in range(3)], axis=1)
Image.fromarray(rgb[0:36, 296:332]).resize((36*10, 36*10), Image.NEAREST).save(os.path.join(AUS, 'wolverine_zoom.png'))
b = M1.sek[8]
pix_off = 20 + (u32(b, 8) - 12) + 12   # TIM: 8 Kopf, CLUT-Block, Pixelkopf 12
print('TIM Pixelblock ab TIM+', pix_off, ' Datei-Offset im Blob', M1.dir[8] + pix_off, 'Pixel-Zeilen 384 breit')
print('Wolverine: Blob-Offset Zeile 8 x 300:', M1.dir[8] + pix_off + 8*384 + 300)
