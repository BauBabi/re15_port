"""Spaltenmass (Verfahren tools/r41_sichel/v6drei/spalten.py: Tiefenpuffer ueber alle Schwanzdreiecke, Luecke = naechste Flaeche ist eine
Rueckseite/Innenwand, nach Oeffnung 3x3) an den GEZEICHNETEN Szenenposen eines Laufs (pose.log, Bild F = h + 243), je h. Ausgabe je h:
Lueckenpunkte gesamt und je naechstem Gelenk 17-22."""
import sys, os, json
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/tools/r41_sichel/v6drei')
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spalten as SP
import messen as M
DG, BB, b2 = SP.DG, SP.BB, SP.b2
KERN = SP.KERN

def dreiecke(mo, p):
    r, kam, Mh, _li = b2.raum()
    g = int(p['gier'])
    sn, cs = DG.b2.EA.sin_q12(g), DG.b2.EA.cos_q12(g)
    Y = np.array([[cs, 0, sn], [0, 4096, 0], [-sn, 0, cs]], dtype=np.int64)
    R, T = p['R'], p['T']
    lage = np.array((p['x'], p['y'], p['z']), dtype=np.int64)
    aus = []
    for b in DG.SCHWANZ:
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW, vT = (kam.V @ W) >> 12, ((kam.V @ t) >> 12) + kam.t
        for verts, faces, viereck in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            q = (VW @ np.array([x[:3] for x in verts], dtype=np.int64).T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = BB._gte(vx, vy, vz, kam.h)
            X, Yv = BB.abbilden(sx.astype(float) + BB.PHASE, sy.astype(float) + BB.PHASE, Mh)
            for f in faces:
                ie = f[1::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                pp = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = BB._mac0(pp[0], pp[1], pp[2])
                rueck = (m0 < 0) if viereck else (m0 <= 0)
                if viereck:
                    for a_, b_, c_ in ((0, 1, 3), (0, 3, 2)):
                        aus.append(([(X[ie[k]], Yv[ie[k]], float(vz[ie[k]])) for k in (a_, b_, c_)], rueck, b))
                else:
                    aus.append(([(X[i], Yv[i], float(vz[i])) for i in ie], rueck, b))
    return aus

lauf, ausp = sys.argv[1], sys.argv[2]
h0, h1 = int(sys.argv[3]), int(sys.argv[4])
mo = b2.modell(M.EMD)
P = M.pose_log(os.path.join(lauf, 'pose.log'))
erg = {}
for h in range(h0, h1 + 1):
    F = h + 243
    if F not in P:
        continue
    p = P[F]
    bel, rue, kn = SP.zpuffer(dreiecke(mo, p))
    roh = bel & rue
    innen = cv2.dilate(cv2.erode(roh.astype(np.uint8), KERN), KERN).astype(bool) & roh
    k = int(innen.sum())
    je = {}
    if k:
        Pm, gl = M.kette(p, M.dorn(mo), n_je=2)
        pose = dict(clip=p['clip'], bild=p['bild'], gier=p['gier'], x=p['x'], y=p['y'], z=p['z'])
        # Gelenke 16..22 projiziert
        R, T = p['R'].astype(float) / 4096.0, p['T'].astype(float)
        G = np.array([T[b] for b in range(16, 23)])
        q = b2.projiziere(pose, G)
        ys, xs = np.nonzero(innen)
        dj = np.hypot(xs[:, None] - q[None, :, 0], ys[:, None] - q[None, :, 1])
        for j in np.argmin(dj, 1):
            je[str(16 + int(j))] = je.get(str(16 + int(j)), 0) + 1
    erg[h] = dict(luecke=k, je_gelenk=je, belegt=int(bel.sum()))
json.dump(erg, open(ausp, 'w'), indent=1)
summe = sum(v['luecke'] for v in erg.values())
print('Summe Lueckenpunkte', summe, 'Bilder mit Luecke', sum(1 for v in erg.values() if v['luecke']), 'von', len(erg))
for h, v in erg.items():
    if v['luecke']:
        print(h, v)
