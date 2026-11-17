"""Streifen-Herkunft: Dreiecke mit Sicht-Raum-Ecken (vx, vy, vz) und zwei Rasterungen im selben Raster wie die Tafeln:
(a) Maler-Ordnung = wie Tafel UND Spiel (Dreiecks-Mitteltiefe, fern zuerst; form_buehne.rastern / render_pc.c Einfuegesortierung)
(b) Tiefenpuffer je Bildpunkt (1/vz perspektivisch korrekt im Port-Bildschirmraum interpoliert) = was geometrisch vorn liegt.
Nur Messung; nichts am Modell, an der Textur oder am Asset."""
import os
import sys
os.environ.setdefault('GRUNDLAGE_ARBEIT', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/grundlage')
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/skripte')
import numpy as np  # noqa: E402
from g_basis import *  # noqa: E402,F401,F403  (P, FB, v4f, kand, g_matrix, ASSET, K5C, TEILNAME, grund_rgba, WT)
import emd_ansichtsblatt as EA  # noqa: E402

AUS = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/streifen'
os.makedirs(AUS, exist_ok=True)
ZIEL = WT + '/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/streifen'
KANDIDATEN = {
    'K5e_f1': ASSET,
    'K5c': K5C,
    'Krallen-Kandidat': WT + '/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/krallen_kandidat',
}


def dreiecke_3d(mo, kf, lage, gier, kam, licht=None):
    """= v4f.dreiecke_id (gleiche Reihenfolge, gleiche Auswahl), Tupel: (tief, p[3], uv[3], zeile, tag, V[3x3] Sicht-Raum).
    tag = (Teil, Flaechennr., Viereck 0/1, Haelfte 0/1)."""
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array(lage, dtype=np.int64)
    ctx = FB.licht_kontext(licht, lage) if licht else None
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW = (kam.V @ W) >> 12
        vT = ((kam.V @ t) >> 12) + kam.t
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (VW @ v.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = FB._gte(vx, vy, vz, kam.h)
            if ctx is not None and norms:
                L = (W.T @ ctx[0].T).T >> 12
                f = FB.prim_zu_sdl(FB.ncct(L, ctx[1], ctx[2], np.array([x[:3] for x in norms], dtype=np.int64)))
            else:
                f = np.full((max(len(norms), 1), 3), 255, dtype=np.int64)
            for fi, (g, uv) in enumerate(zip(faces, uvs)):
                ie = g[1::2]
                inn = g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = FB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128
                zeile = (uv[2] >> 6) & 0x1FF
                V = np.array([[vx[i], vy[i], vz[i]] for i in ie], float)
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for h, (a, bb, cc) in enumerate(((0, 1, 3), (0, 3, 2))):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile, (b, fi, viereck, h), V[[a, bb, cc]],
                                    (f[inn[a]], f[inn[bb]], f[inn[cc]])))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = (int(vz[ie[0]]) + int(vz[ie[1]]) + int(vz[ie[2]])) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (b, fi, viereck, 0), V, (f[inn[0]], f[inn[1]], f[inn[2]])))
    return aus


def welt_dreiecke(mo, kf):
    """Alle Dreiecke im Modellraum (ohne Kamera, ohne Rueckseiten-Auswahl): Liste (tag, V[3x3]). Fuer den Schnitt im Raum."""
    R, T = mo.pose(kf)
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = R[b].astype(np.int64)
        t = T[b].astype(np.int64)
        for (verts, faces, viereck) in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = ((W @ v.T) >> 12).T + t
            for fi, g in enumerate(faces):
                ie = g[1::2]
                if viereck:
                    for h, (a, bb, cc) in enumerate(((0, 1, 3), (0, 3, 2))):
                        aus.append(((b, fi, viereck, h), q[[ie[a], ie[bb], ie[cc]]].astype(float)))
                else:
                    aus.append(((b, fi, viereck, 0), q[list(ie)].astype(float)))
    return aus


def rastern_beide(tris, tex, H, W, Mg):
    """Beide Ordnungen in einem Durchgang. Rueckgabe dict je Bildpunkt:
    maler_i / puffer_i (Index in tris), maler_z / puffer_z (vz am Bildpunkt des jeweiligen Dreiecks), rgb_maler, rgb_puffer (Texel ohne Licht)."""
    Mg = np.asarray(Mg, float)
    Minv = np.linalg.inv(Mg)
    out = {k: np.full((H, W), -1, np.int32) for k in ('maler_i', 'puffer_i')}
    out['maler_z'] = np.full((H, W), np.nan)
    out['puffer_z'] = np.full((H, W), np.inf)
    out['rgb_maler'] = np.zeros((H, W, 3), np.uint8)
    out['rgb_puffer'] = np.zeros((H, W, 3), np.uint8)
    reihenfolge = sorted(range(len(tris)), key=lambda i: -tris[i][0])   # stabil wie sorted(tris, key=-t[0])
    ab = dict(matrix=Mg.tolist())
    for i in reihenfolge:
        _t, p, uv, zeile, tag, V, farbe = tris[i]
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), ab)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float); v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, tex.w - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, tex.h - 1)
        z = zeile - tex.clut_y
        z = z if 0 <= z < tex.lut.shape[0] else 0
        rgba = tex.lut[z][tex.px[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        # Tiefe: Zielpunkt -> Port-Bildschirm (projektive Umkehr), dort baryzentrisch, 1/vz linear
        den = Minv[2, 0] * PX + Minv[2, 1] * PY + Minv[2, 2]
        qx = (Minv[0, 0] * PX + Minv[0, 1] * PY + Minv[0, 2]) / den - FB.PHASE
        qy = (Minv[1, 0] * PX + Minv[1, 1] * PY + Minv[1, 2]) / den - FB.PHASE
        px_ = np.array([a[0] for a in p], float); py_ = np.array([a[1] for a in p], float)
        f2 = (px_[1] - px_[0]) * (py_[2] - py_[0]) - (px_[2] - px_[0]) * (py_[1] - py_[0])
        if abs(f2) < 1e-9:
            zz = np.full(PX.shape, V[:, 2].mean())
        else:
            b1 = ((qx - px_[0]) * (py_[2] - py_[0]) - (px_[2] - px_[0]) * (qy - py_[0])) / f2
            b2 = ((px_[1] - px_[0]) * (qy - py_[0]) - (qx - px_[0]) * (py_[1] - py_[0])) / f2
            b0 = 1 - b1 - b2
            iz = b0 / V[0, 2] + b1 / V[1, 2] + b2 / V[2, 2]
            zz = 1.0 / np.maximum(iz, 1e-9)
            zz = np.clip(zz, V[:, 2].min(), V[:, 2].max())
        c0, c1, c2 = (np.asarray(q, float) for q in farbe)
        fa = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        farb = np.clip(rgba[..., :3].astype(float) * fa / 255.0, 0, 255).astype(np.uint8)
        sl = (slice(y0, y1), slice(x0, x1))
        out['maler_i'][sl][tref] = i
        out['maler_z'][sl][tref] = zz[tref]
        out['rgb_maler'][sl][tref] = farb[tref]
        vorn = tref & (zz < out['puffer_z'][sl])
        out['puffer_i'][sl][vorn] = i
        out['puffer_z'][sl][vorn] = zz[vorn]
        out['rgb_puffer'][sl][vorn] = farb[vorn]
    return out


def szene(ordner, n=2, clip=None, bild=None, licht=None):
    """licht: None = weiss (Texel unbeleuchtet), 'disc' = Disc-Licht des Schnitts wie im Spiel, 'mess' = Tafel-Licht."""
    k = kand(ordner)
    lage, gier, c0, b0 = k.stand(n)
    clip = c0 if clip is None else clip
    bild = b0 if bild is None else bild
    Mg, kam = g_matrix(n)
    li = P.licht_fuer(n, licht)[0] if licht else None
    tris = dreiecke_3d(k.mo, k.kf(clip, bild), lage, gier, kam, li)
    return k, tris, Mg, kam


def tri_mesh(tris):
    return np.array([t[4][0] for t in tris], np.int32)
