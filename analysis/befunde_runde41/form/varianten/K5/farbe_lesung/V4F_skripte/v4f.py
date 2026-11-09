"""Messer V4 Feinschliff: gemeinsame Hilfen. Render mit Flaechen-Kennung je Bildpunkt (Mesh, Flaeche, Texel u/v, CLUT-Zeile, Eintrag),
Nachbau von form_buehne.dreiecke/rastern bzw. form_pruefung.rundum_dreiecke Zeile fuer Zeile, nur mit zusaetzlichen Kennungs-Puffern.
Szenenkette wie Tafeln 01/02: Kandidat.rendern (Disc-Licht x k, pruefung/licht.json) und form_video.video_filter (entzerren=False, rauschen=True)."""
import math
import os
import sys

import cv2
import numpy as np

WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
FORM = WT + '/analysis/befunde_runde41/form'
K5 = FORM + '/varianten/K5'
sys.path.insert(0, WT + '/re15_port/tools')
sys.path.insert(0, K5 + '/farbe_lesung/P_skripte')
sys.path.insert(0, K5 + '/farbe_lesung/B_skripte')
import form_pruefung as P  # noqa: E402
import form_buehne as FB  # noqa: E402
import form_video as FV  # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
import p_zonen as PZ  # noqa: E402

SAATEN = (15, 12345, 7)
STAENDE = [  # (Kurzname, Ordner mit form.json)
    ('K4', FORM + '/stufe3/basis_n2'),
    ('K4 kroko', K5 + '/K4_heute'),
    ('K5c roh', K5 + '/K5c'),
    ('V2', K5 + '/farbe/K5c_farbe_v2'),
    ('V3R3', K5 + '/farbe/K5c_farbe_v3r3'),
    ('V3R4', K5 + '/farbe/K5c_farbe_v3r4'),
    ('Asset K5c', FORM + '/stufe3/basis_k5c'),
]
ST = dict(STAENDE)
KLAUEN = {'r (W)': (19, 20, 29, 31), 'l (W)': (14, 15, 28, 30)}


def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(-1, 3) / 255.0
    return cv2.cvtColor(a[None], cv2.COLOR_RGB2LAB)[0].reshape(np.shape(rgb))


def rgb_von_lab(L):
    a = np.asarray(L, np.float32).reshape(1, -1, 3)
    return np.clip(cv2.cvtColor(a, cv2.COLOR_LAB2RGB)[0] * 255, 0, 255).reshape(np.shape(L))


_KC = {}


def kand(ordner):
    if ordner not in _KC:
        _KC[ordner] = P.Kandidat(os.path.join(ordner, 'form.json'))
    return _KC[ordner]


def dreiecke_id(mo, kf, lage, gier, kam, licht=None):
    """= form_buehne.dreiecke, Tupel zusaetzlich mit (mesh, flaeche, viereck)."""
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
                tag = (b, fi, viereck)
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile, (f[inn[a]], f[inn[bb]], f[inn[cc]]), tag))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = (int(vz[ie[0]]) + int(vz[ie[1]]) + int(vz[ie[2]])) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (f[inn[0]], f[inn[1]], f[inn[2]]), tag))
    return aus


def rundum_id(mo, kf, az, el, gross, teile=None):
    """= form_pruefung.rundum_dreiecke mit Kennung (gleiche Kamera/Licht)."""
    R, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    pts, tl = [], []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        if teile is not None and b not in teile:
            continue
        m = mo.meshes[b]
        W = R[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ W.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
            pts.append(v)
            tl.append((b, v, nn, faces, uvs, viereck))
    alle = np.vstack(pts)
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max()) or 1.0
    s = gross * 0.46 / rad
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    aus = []
    for b, v, nn, faces, uvs, viereck in tl:
        q = (v - mitte) @ A.T
        sx = gross / 2 + q[:, 0] * s
        sy = gross / 2 + q[:, 1] * s
        hell = np.clip(0.46 + 0.86 * np.maximum(nn @ L, 0), 0, 1) * 255.0
        for fi, (g, uv) in enumerate(zip(faces, uvs)):
            ie, inn = g[1::2], g[0::2]
            p = [(float(sx[i]), float(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if viereck else (m0 <= 0):
                continue
            po = (uv[5] & 0xF) * 128
            zeile = (uv[2] >> 6) & 0x1FF
            c = [np.full(3, hell[i]) for i in inn]
            tag = (b, fi, viereck)
            if viereck:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                    aus.append((float(q[ie[x], 2] + q[ie[y], 2] + q[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), zeile,
                                (c[x], c[y], c[zz]), tag))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((float(q[ie[0], 2] + q[ie[1], 2] + q[ie[2], 2]) / 3, tuple(p), tuple(tx), zeile, tuple(c), tag))
    return aus


def rastern_id(tris, tex, ziel, abbildung):
    """= form_buehne.rastern (fern zuerst, gleiche Abtastung), plus Puffer: mesh, flaeche (viereck*100000+fi), u, v, zeile, eintrag."""
    H, W = ziel.shape[:2]
    maske = np.zeros((H, W), bool)
    K = {k: np.full((H, W), -1, np.int32) for k in ('mesh', 'flaeche', 'u', 'v', 'zeile', 'eintrag')}
    for _t, p, uv, zeile, farbe, tag in sorted(tris, key=lambda t: -t[0]):
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), abbildung)
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
        u = np.array([q[0] for q in uv], float)
        v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, tex.w - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, tex.h - 1)
        z = zeile - tex.clut_y
        z = z if 0 <= z < tex.lut.shape[0] else 0
        idx = tex.px[vv, uu]
        rgba = tex.lut[z][idx]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]
        s[tref] = rgb[tref]
        maske[y0:y1, x0:x1][tref] = True
        for k_, val in (('mesh', tag[0]), ('flaeche', tag[2] * 100000 + tag[1]), ('zeile', z)):
            K[k_][y0:y1, x0:x1][tref] = val
        K['u'][y0:y1, x0:x1][tref] = uu[tref]
        K['v'][y0:y1, x0:x1][tref] = vv[tref]
        K['eintrag'][y0:y1, x0:x1][tref] = idx[tref]
    K['maske'] = maske
    return K


def szene_id(ordner, n):
    """Szene Bild n wie Kandidat.rendern; zusaetzlich Kennungs-Puffer. Rueckgabe sauber (RGB, Disc-Licht x k ueber Hintergrund), K."""
    k = kand(ordner)
    lage, gier, clip, bild = k.stand(n)
    hg = P.lade_rgb(P.daten('hintergrund_bild%d.png' % n))
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    licht, _kk = P.licht_fuer(n, k.licht)
    sauber = hg.copy()
    K = rastern_id(dreiecke_id(k.mo, kf, lage, gier, kam, licht), k.tex, sauber, kam.abbildung)
    return sauber, K


def rundum_bild(ordner, clip, bild, az, el, gross=380, grund=92, teile=None):
    k = kand(ordner)
    a = np.full((gross, gross, 3), grund, np.uint8)
    K = rastern_id(rundum_id(k.mo, k.kf(clip, bild), az, el, gross, teile), k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return a, K


def video(sauber, n, saat=15):
    return np.asarray(FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=saat), np.uint8)


def original(n):
    return cv2.cvtColor(cv2.imread(FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)


def emd_flaechen(ordner):
    """alle Flaechen des Modells: (mesh, viereck, fi, zeile_rel, uv-Wort-Liste)."""
    k = kand(ordner)
    out = []
    for b, m in enumerate(k.mo.meshes):
        for viereck, uvs in ((0, m.tuv), (1, m.quv)):
            for fi, uv in enumerate(uvs):
                out.append((b, viereck, fi, ((uv[2] >> 6) & 0x1FF) - k.tex.clut_y, uv))
    return out
