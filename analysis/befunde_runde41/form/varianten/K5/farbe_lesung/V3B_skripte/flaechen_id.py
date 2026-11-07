"""K5c in Kamera Bild 1/2: je Pixel Knochen, Flaechen-Nr. (Reihenfolge tf dann qf wie im MD1) und Punkt im Knochenraum (baryzentrisch)."""
import sys, json
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools')
import form_pruefung as P, form_buehne as FB, emd_ansichtsblatt as EA
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/leserb/'
k = P.Kandidat(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/K5c/form.json')
mo = k.mo
for n in (1, 2):
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n); kf = k.kf(clip, bild)
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lg = np.array(lage, dtype=np.int64)
    tris = []   # (tiefe, p_screen[3], bone, face, lokale Ecken[3], welt-Ecken[3])
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        Wm = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lg
        VW = (kam.V @ Wm) >> 12
        vT = ((kam.V @ t) >> 12) + kam.t
        fi = 0
        for (verts, faces, viereck) in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (VW @ v.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = FB._gte(vx, vy, vz, kam.h)
            wv = ((Wm @ v.T) >> 12).T + t
            for g in faces:
                ie = g[1::2]
                nr = fi; fi += 1
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = FB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                sets = ((0, 1, 3), (0, 3, 2)) if viereck else ((0, 1, 2),)
                for a, bb, cc in sets:
                    tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                    tris.append((tief, (p[a], p[bb], p[cc]), b, nr, viereck,
                                 np.array([v[ie[a]], v[ie[bb]], v[ie[cc]]], float),
                                 np.array([vz[ie[a]], vz[ie[bb]], vz[ie[cc]]], float),
                                 np.array([wv[ie[a]], wv[ie[bb]], wv[ie[cc]]], float)))
    H, Wd = 480, 640
    kn = np.full((H, Wd), -1, np.int32); fa = np.full((H, Wd), -1, np.int32); qd = np.zeros((H, Wd), np.int8)
    lok = np.zeros((H, Wd, 3), np.float32); tz = np.zeros((H, Wd), np.float32); welt = np.zeros((H, Wd, 3), np.float32)
    # Textur-Alpha wie rastern: transparente Texel lassen durch. Fuer K5c gilt: Kopf/Koerper ohne Transparenz ausser Zaehne? -> hier ohne Alpha, Abgleich mit idkarten kn
    for tief, p, b, nr, vq, L3, z3, w3 in sorted(tris, key=lambda t: -t[0]):
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), kam.abbildung)
        f = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(f) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, Wd)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / f
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / f
        w0 = 1 - w1 - w2
        inn = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not inn.any():
            continue
        sl = (slice(y0, y1), slice(x0, x1))
        kn[sl][inn] = b; fa[sl][inn] = nr; qd[sl][inn] = vq
        for d in range(3):
            lok[sl][..., d][inn] = (w0 * L3[0, d] + w1 * L3[1, d] + w2 * L3[2, d])[inn]
            welt[sl][..., d][inn] = (w0 * w3[0, d] + w1 * w3[1, d] + w2 * w3[2, d])[inn]
        tz[sl][inn] = (w0 * z3[0] + w1 * z3[1] + w2 * z3[2])[inn]
    np.savez_compressed(S + 'fid%d.npz' % n, kn=kn, fa=fa, qd=qd, lok=lok, tz=tz, welt=welt)
    ref = np.load(S + 'kn%d.npy' % n)
    both = (ref >= 0) | (kn >= 0)
    print(n, 'Pixel', both.sum(), 'gleiche Knochen-ID wie teilbild', round(float((ref[both] == kn[both]).mean()), 4))
