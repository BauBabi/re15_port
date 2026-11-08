"""Rasterer mit vollem Pixel-Protokoll (Werkzeug Messblatt Fotovorlage): wie b2.dreiecke_ids/rastern_ids (Pose, Licht,
GTE, Homographie, Maler-Reihenfolge fern zuerst, Texel-Transparenz), zusaetzlich je Videopixel:
  bone, flaeche (laufende Nr. je Knochen: 'd<i>' Dreieck / 'v<i>' Viereck), zeile (CLUT-Zeile), u, v (Texel), idx
  (Palettenindex), lok (Knochenraum-Koordinate, Modelleinheiten, baryzentrisch), nl (Normale im Knochenraum).
"""
import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G, b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA


def dreiecke(mo, pose, licht=True, knochen=None):
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    gier = int(pose['gier'])
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array((pose['x'], pose.get('y', 0), pose['z']), dtype=np.int64)
    ctx = BB.licht_kontext(li, lage) if licht else None
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        if knochen is not None and b not in knochen:
            continue
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW, vT = (kam.V @ W) >> 12, ((kam.V @ t) >> 12) + kam.t
        for verts, norms, faces, uvs, viereck in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            P = np.array([x[:3] for x in verts], dtype=np.int64)
            NN = np.array([x[:3] for x in norms], dtype=np.int64)
            q = (VW @ P.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = BB._gte(vx, vy, vz, kam.h)
            if ctx is not None:
                L = (W.T @ ctx[0].T).T >> 12
                f = np.minimum((BB.ncct(L, ctx[1], ctx[2], NN) * 0xFF + 0x40) // 0x80, 0xFF)
            else:
                f = np.full((len(norms), 3), 255, dtype=np.int64)
            for fi, (g, uv) in enumerate(zip(faces, uvs)):
                ie, inn = g[1::2], g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = BB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po, zeile = (uv[5] & 0xF) * 128, (uv[2] >> 6) & 0x1FF
                key = ('v%d' if viereck else 'd%d') % fi
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile,
                                    (f[inn[a]], f[inn[bb]], f[inn[cc]]), b, key,
                                    P[[ie[a], ie[bb], ie[cc]]], NN[[inn[a], inn[bb], inn[cc]]]))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = sum(int(vz[i]) for i in ie) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (f[inn[0]], f[inn[1]], f[inn[2]]), b, key,
                                P[list(ie)], NN[list(inn)]))
    return aus


def zeichne(mo, pose, hintergrund=None, licht=True, knochen=None, H=480 + G.UNTEN, filt=None):
    """-> dict(img, bone, flaeche(obj), zeile, u, v, idx, lok(Hx640x3), nl). filt(b, key, zeile) -> bool: nur diese Flaechen."""
    r, kam, M, li = b2.raum()
    W = 640
    ziel = np.array(hintergrund, copy=True) if hintergrund is not None else np.full((H, W, 3), 60, np.uint8)
    A = dict(bone=np.full((H, W), -1, np.int16), zeile=np.full((H, W), -1, np.int16), u=np.full((H, W), -1, np.int16),
             v=np.full((H, W), -1, np.int16), idx=np.full((H, W), -1, np.int16), lok=np.full((H, W, 3), np.nan, np.float32),
             nl=np.full((H, W, 3), np.nan, np.float32), flaeche=np.full((H, W), '', object))
    for _t, p, uv, zeile, farbe, b, key, PL, NL in sorted(dreiecke(mo, pose, licht, knochen), key=lambda t: -t[0]):
        if filt is not None and not filt(b, key, zeile):
            continue
        x, y = BB.abbilden(np.array([q[0] + BB.PHASE for q in p], float), np.array([q[1] + BB.PHASE for q in p], float), M)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, H)
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
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, mo.texw - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, mo.texh - 1)
        z = zeile - mo.clut_y
        zz = z if 0 <= z < mo.lut.shape[0] else 0
        ix = mo.tex[vv, uu]
        rgba = mo.lut[zz][ix]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        sl = (slice(y0, y1), slice(x0, x1))
        ziel[sl][tref] = rgb[tref]
        A['bone'][sl][tref] = b; A['zeile'][sl][tref] = zeile; A['u'][sl][tref] = uu[tref]; A['v'][sl][tref] = vv[tref]
        A['idx'][sl][tref] = ix[tref]; A['flaeche'][sl][tref] = key
        PLf = PL.astype(float); NLf = NL.astype(float)
        lok = w0[..., None] * PLf[0] + w1[..., None] * PLf[1] + w2[..., None] * PLf[2]
        nl = w0[..., None] * NLf[0] + w1[..., None] * NLf[1] + w2[..., None] * NLf[2]
        A['lok'][sl][tref] = lok[tref]; A['nl'][sl][tref] = nl[tref]
    A['img'] = ziel
    return A


def mit_bg(mo, pose, **kw):
    bg = np.vstack([G.hintergrund(), np.full((G.UNTEN, 640, 3), 60, np.uint8)])
    return zeichne(mo, pose, bg, **kw)
