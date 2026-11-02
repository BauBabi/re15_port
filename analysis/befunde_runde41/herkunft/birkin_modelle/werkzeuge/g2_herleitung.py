#!/usr/bin/env python3
"""Herleitung M1 aus RE2 G2.
1) Beine: vertexweise (gleiche Reihenfolge?) und Regel 'negative Komponente +1'.
2) Klaue: G2 Mesh 5 + Finger 6,7,8 gebacken (rel je Knochen, keine Drehung) gegen M1 Mesh 15.
3) Rumpf: M1 Mesh 0 gegen G2 Mesh 0, 1, 2 (auch verschoben), Flaechenbilanz.
4) Normalen: Meshes 8-14 bytegleich D0? Beine bytegleich G2?
5) UV-Versatz: Abstimmung (du,dv) M1-Mesh gegen G2-Mesh."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from collections import Counter
M1, D0 = zerlege(blob('M1')), zerlege(blob('D0'))
e, *_ = re2_rec(0x31, 3); t, *_ = re2_rec(0x31, 2); G2 = zerlege(e, t)
V = lambda m: np.array([v[:3] for v in m.tv], int)

print('== 1) Beine, vertexweise')
for i, j in ((1, 12), (2, 13), (3, 14), (4, 15), (5, 16), (6, 17)):
    a, b = V(M1.meshes[i]), V(G2.meshes[j])
    d = a - b
    print(f'M1[{i}] G2[{j}] n={len(a)} gleiche Reihenfolge, Differenz je Achse min {d.min(0)} max {d.max(0)}; Anteil Vertices exakt {(d==0).all(1).mean():.2f}')
    # Differenz in Abhaengigkeit vom Vorzeichen
    for ax, nm in enumerate('xyz'):
        neg = b[:, ax] < 0
        c1 = Counter(d[neg, ax].tolist()); c2 = Counter(d[~neg, ax].tolist())
        print(f'    {nm}: bei G2<0 Differenzen {dict(c1)}  bei G2>=0 {dict(c2)}')

print()
print('== Flaechen-Indizes der Beine: gleich?')
for i, j in ((1, 12), (2, 13), (3, 14), (4, 15), (5, 16), (6, 17)):
    a, b = M1.meshes[i], G2.meshes[j]
    print(f'M1[{i}] G2[{j}]: Tri-Indizes gleich {a.tf == b.tf}, Quad-Indizes gleich {a.qf == b.qf}, Tri-Satz gleich {sorted(a.tf)==sorted(b.tf)}; UV gleich T {sum(x==y for x,y in zip(a.tuv,b.tuv))}/{len(a.tuv)} Q {sum(x==y for x,y in zip(a.quv,b.quv))}/{len(a.quv)}')
    # UV-Versatz je Flaeche in Seite+uv
    c = Counter()
    for x, y in zip(a.quv + a.tuv, b.quv + b.tuv):
        c[((x[5]&0xF)*128 + x[0] - (y[5]&0xF)*128 - y[0], x[1]-y[1])] += 1
    print('    haeufigster TIM-Versatz (dx,dy):', c.most_common(4))

print()
print('== 2) Klaue')
for j in range(4, 9):
    n = V(G2.meshes[j]); print(f'  G2[{j}] n={len(n)} Box {n.min(0)}..{n.max(0)}  T={len(G2.meshes[j].tf)} Q={len(G2.meshes[j].qf)}')
a = V(M1.meshes[15]); print('M1[15] Box', a.min(0), a.max(0))
print('M1[15]==M1[16] Vertices:', (V(M1.meshes[15]) == V(M1.meshes[16])).all(), 'Flaechen', M1.meshes[15].tf == M1.meshes[16].tf, M1.meshes[15].qf == M1.meshes[16].qf,
      'UV', M1.meshes[15].tuv == M1.meshes[16].tuv, M1.meshes[15].quv == M1.meshes[16].quv)
print('D0[15]==D0[16]? Vertexzahlen', len(D0.meshes[15].tv), len(D0.meshes[16].tv))
bake = {5: (0, 0, 0), 6: G2.skel.rel[6], 7: G2.skel.rel[7], 8: G2.skel.rel[8]}
pkt = []
for j, r in bake.items():
    pkt.append(V(G2.meshes[j]) + np.array(r))
P = np.concatenate(pkt)
# Abstimmung Verschiebung
c = Counter()
for x in a:
    for y in P:
        c[tuple(((y - x) // 4).tolist())] += 1
print('beste Verschiebung (Raster 4) G2-gebacken -> M1[15]:', c.most_common(3))
t0 = np.array(c.most_common(1)[0][0]) * 4 + 2
d = np.sqrt(((a[:, None, :] + t0 - P[None, :, :])**2).sum(-1))
print('Anteil M1[15] innerhalb 6 Einheiten nach Verschiebung:', (d.min(1) < 6).mean(), ' Anteil G2-gebacken:', (d.min(0) < 6).mean())
print('je G2-Teil Abdeckung:')
o = 0
for j, r in bake.items():
    n = len(G2.meshes[j].tv)
    print(f'   G2[{j}]: {(d[:, o:o+n].min(0) < 6).sum()}/{n}')
    o += n

print()
print('== 3) Rumpf M1[0]')
a = V(M1.meshes[0]); print('M1[0] n', len(a), 'Box', a.min(0), a.max(0), 'Tri', len(M1.meshes[0].tf), 'Quad', len(M1.meshes[0].qf))
for j in (0, 1, 2):
    b = V(G2.meshes[j])
    print(f' G2[{j}] Box {b.min(0)} {b.max(0)}')
    c = Counter()
    for x in a:
        for y in b:
            c[tuple(((y - x) // 4).tolist())] += 1
    print('   beste Verschiebung', c.most_common(3))
    for s, _ in c.most_common(2):
        t0 = np.array(s) * 4 + 2
        dd = np.sqrt(((a[:, None, :] + t0 - b[None, :, :])**2).sum(-1))
        print(f'   bei {t0}: Anteil M1 nahe {(dd.min(1)<6).mean():.2f}, Anteil G2 nahe {(dd.min(0)<6).mean():.2f}')

print()
print('== 4) Normalen')
N = lambda m: [tuple(v[:3]) for v in m.tn]
for i in range(17):
    a = N(M1.meshes[i]); b = N(D0.meshes[i])
    print(f'  M1[{i}] Normalen {len(a)}; gleich D0[{i}]: {a == b}; Satz-Treffer {len(set(a)&set(b))}')
for i, j in ((1, 12), (2, 13), (3, 14), (4, 15), (5, 16), (6, 17)):
    a = N(M1.meshes[i]); b = N(G2.meshes[j])
    aa, bb = np.array(a), np.array(b)
    print(f'  M1[{i}] gegen G2[{j}]: exakt {sum(x==y for x,y in zip(a,b))}/{len(a)}, max Abw. {np.abs(aa-bb).max()}')

print()
print('== MD1-Layout')
for nm, q in (('D0', D0), ('M1', M1), ('G2', G2)):
    m = q.md1
    print(nm, 'Kopf', struct.unpack_from('<III', m, 0))
    t = [struct.unpack_from('<14I', m, 12 + i*56) for i in range(struct.unpack_from('<I', m, 8)[0] // 2)]
    print('   Mesh 0-2:', t[:3])
    print('   tvo==qvo aller Meshes:', all(x[0] == x[7] for x in t), ' tno==qno:', all(x[2] == x[9] for x in t))
