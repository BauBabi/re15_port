"""Render-Gegenprobe: EM30_HANGAR und K5c im selben Renderer (form_buehne.rastern, Gouraud je Ecke wie k5_blatt.blick_bild),
identisches neutrales Licht (Umgebung 0,55 + 0,45 * max(0, N*L), weiss, Richtung oben links vorn im Blickraum), gleiche Groesse
(Umkugel auf 0,46 der Kachel), 3 Winkel; einmal roh, einmal Gamma 0,5 wie die alten Tafeln (k5_blatt: a = (a/255)^0,5 * 255)."""
import math, sys, json
import numpy as np, cv2
import fl
FB = fl.FB

AMB, DIF = 0.55, 0.45
WINKEL = (('3/4 vorn', 45, 15), ('Seite', 0, 8), ('3/4 hinten', -135, 20))


def blick(mo, kf, az, el, gross=400, gamma=None):
    R, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    teile = []
    pts = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        Wm = R[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ Wm.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ Wm.T
            teile.append((v, nn, faces, uvs, viereck))
            pts.append(v)
    alle = np.vstack(pts)
    mitte = (alle.max(0) + alle.min(0)) / 2
    s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
    aus = []
    for v, nn, faces, uvs, viereck in teile:
        q = (v - mitte) @ A.T
        sx = gross / 2 + q[:, 0] * s
        sy = gross / 2 + q[:, 1] * s
        hell = np.clip(AMB + DIF * np.maximum(nn @ L, 0), 0, 1) * 255.0
        for g, uv in zip(faces, uvs):
            ie, inn = g[1::2], g[0::2]
            p = [(float(sx[i]), float(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if viereck else (m0 <= 0):
                continue
            po = (uv[5] & 0xF) * 128
            zeile = (uv[2] >> 6) & 0x1FF
            c = [np.full(3, hell[i]) for i in inn]
            if viereck:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                    aus.append((float(q[ie[x], 2] + q[ie[y], 2] + q[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), zeile, (c[x], c[y], c[zz])))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((float(q[ie[0], 2] + q[ie[1], 2] + q[ie[2], 2]) / 3, tuple(p), tuple(tx), zeile, tuple(c)))
    img = np.full((gross, gross, 3), 92, np.uint8)
    msk = FB.rastern(aus, mo.T, img, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    if gamma:
        img[msk] = np.clip(np.power(img[msk] / 255.0, gamma) * 255, 0, 255).astype(np.uint8)
    return img, msk


def kf(mo, clip, bild=0):
    f0, k = mo.clips[clip]
    return mo.frames[f0 + min(bild, k - 1)] & 0xFFF


def masse(img, msk):
    L = fl.lab(img[msk])
    C = np.hypot(L[:, 1], L[:, 2])
    return dict(L=round(float(L[:, 0].mean()), 1), C=round(float(C.mean()), 1), a=round(float(L[:, 1].mean()), 1), b=round(float(L[:, 2].mean()), 1),
                hell70=round(100 * float((L[:, 0] > 70).mean()), 2), hell85=round(100 * float((L[:, 0] > 85).mean()), 2), px=int(msk.sum()))
