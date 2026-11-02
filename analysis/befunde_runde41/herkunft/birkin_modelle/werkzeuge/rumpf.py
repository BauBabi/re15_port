#!/usr/bin/env python3
"""M1 Mesh 0 gegen RE2 G2 Mesh 0+1: Einzelbilder texturiert (gleicher Massstab), UV-Polygon-Abstimmung gegen alle G2-Meshes,
RANSAC mit Skala, Vertex-Lage nach Kastenanpassung. Die Klaue M1[15] gegen G2[5]: welche Vertices fehlen."""
import sys, os, random
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from collections import Counter
from PIL import Image, ImageDraw, ImageFont
import blaetter_lib as BL
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
M1b = blob('M1')
mo1 = BL.modell(M1b, 0x30, 'M1', 'M1')
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2)
mo2 = BL.modell(e, 0x31, 'G2', 'G2', t)
G2 = zerlege(e, t)
M1 = zerlege(M1b)
V = lambda m: np.array([v[:3] for v in m.tv], float)


def bild(mo, mesh_ids, name):
    """Meshes ohne Pose in ihrem lokalen Raum (Y unten)."""
    Vs, Ns, Fs = [], [], []
    for b in mesh_ids:
        m = mo.meshes[b]
        tvb, tnb = len(Vs), len(Ns)
        Vs.extend([np.array(v[:3], float) for v in m.tv])
        Ns.extend([np.array(n[:3], float) / max(1, np.linalg.norm(n[:3])) for n in m.tn])
        for i, (n0, v0, n1, v1, n2, v2) in enumerate(m.tf):
            uv = m.tuv[i]
            Fs.append(dict(v=(tvb+v0, tvb+v1, tvb+v2), n=(tnb+n0, tnb+n1, tnb+n2), uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])), clut=uv[2], page=uv[5], bone=b))
        for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m.qf):
            uv = m.quv[i]
            a, bb, c, dd = (uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])
            Fs.append(dict(v=(tvb+v0, tvb+v1, tvb+v2), n=(tnb+n0, tnb+n1, tnb+n2), uv=(a, bb, c), clut=uv[2], page=uv[5], bone=b))
            Fs.append(dict(v=(tvb+v1, tvb+v3, tvb+v2), n=(tnb+n1, tnb+n3, tnb+n2), uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=b))
    Vv, Nn = np.array(Vs), np.array(Ns)
    W, H = 460, 460
    sc = 0.28
    teile = []
    for right, down, depth in (((0, 0, 1), (0, 1, 0), (1, 0, 0)), ((-1, 0, 0), (0, 1, 0), (0, 0, 1)), ((0, 0, -1), (0, 1, 0), (-1, 0, 0))):
        cx = W/2
        cy = H/2 + 60
        col, alp = EA.render(mo, Vv, Nn, Fs, right, down, depth, W, H, sc, cx, cy)
        rgb = np.clip(col*1.6, 0, 1)
        teile.append((rgb*255).astype(np.uint8))
    return np.concatenate(teile, axis=1)


b1 = bild(mo1, [0], 'M1')
b2 = bild(mo2, [0, 1], 'G2')
ges = np.concatenate([b1, b2], axis=0)
im = Image.fromarray(ges)
d = ImageDraw.Draw(im)
f = ImageFont.truetype(r'C:\Windows\Fonts\segoeui.ttf', 20)
d.text((10, 8), 'M1 Mesh 0 (vorne, links, hinten; lokaler Raum, gleicher Massstab)', fill=(235, 235, 235), font=f)
d.text((10, 468), 'RE2 G2 Mesh 0 + 1', fill=(235, 235, 235), font=f)
im.save(os.path.join(AUS, 'rumpf_M1_gegen_G2.png'))

print('== UV-Abstimmung M1[0] gegen G2-Meshes: gleiche Polygonform, gleicher Versatz')


def polys(m):
    out = []
    for u in m.tuv:
        p = (u[5] & 0xF) * 128
        out.append(((u[0]+p, u[1]), (u[3]+p, u[4]), (u[6]+p, u[7])))
    for u in m.quv:
        p = (u[5] & 0xF) * 128
        out.append(((u[0]+p, u[1]), (u[3]+p, u[4]), (u[6]+p, u[7]), (u[9]+p, u[10])))
    return out


pa = polys(M1.meshes[0])
print('M1[0] UV-Bereich x', min(p[0] for f in pa for p in f), max(p[0] for f in pa for p in f), 'y', min(p[1] for f in pa for p in f), max(p[1] for f in pa for p in f))
for j, n in enumerate(G2.meshes):
    pb = polys(n)
    c = Counter()
    for a in pa:
        for b in pb:
            if len(a) != len(b):
                continue
            d0 = (a[0][0]-b[0][0], a[0][1]-b[0][1])
            if all((x[0]-y[0], x[1]-y[1]) == d0 for x, y in zip(a, b)):
                c[d0] += 1
    if c:
        print(f'  G2[{j}] ({len(pb)} Flaechen): haeufigster Versatz {c.most_common(3)}')
print('Anzahl Vertices M1[0]:', len(V(M1.meshes[0])))

print('== Klaue M1[15]: Herkunft je Vertex')
A = V(M1.meshes[15])
Teile = {'G2[5]': (V(G2.meshes[5]), np.array([23., -37., 2.])), 'G2[6]': (V(G2.meshes[6]), np.array([21., 711., 3.])),
         'G2[7]': (V(G2.meshes[7]), np.array([-110., 711., 2.])), 'G2[8]': (V(G2.meshes[8]), np.array([189., 613., 0.]))}
frei = np.ones(len(A), bool)
for k, (P, tt) in Teile.items():
    d = np.sqrt(((A[:, None] - (P + tt)[None])**2).sum(-1))
    print(f'  {k}: Abdeckung M1 {(d.min(1) < 6).sum()} Vertices, {k}-Vertices getroffen {(d.min(0) < 6).sum()}/{len(P)}')
    frei &= d.min(1) >= 6
print('  M1[15]-Vertices ohne Partner:', frei.sum(), A[frei].astype(int).tolist()[:30])
print('== Klaue M1[15] Flaechen-UV gegen G2')
pa = polys(M1.meshes[15])
for j in (4, 5, 6, 7, 8):
    pb = polys(G2.meshes[j])
    c = Counter()
    for a in pa:
        for b in pb:
            if len(a) != len(b):
                continue
            d0 = (a[0][0]-b[0][0], a[0][1]-b[0][1])
            if all((x[0]-y[0], x[1]-y[1]) == d0 for x, y in zip(a, b)):
                c[d0] += 1
    print(f'  G2[{j}] ({len(pb)} Flaechen): {c.most_common(3)}')
