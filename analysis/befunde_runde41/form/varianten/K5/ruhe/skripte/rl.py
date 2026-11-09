"""Lader fuer das Ruhe-Messblatt (K5e): vier Modelle, weisses Licht (Familien-Tafel 07), Szenenketten Bild 1/2."""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
RUHE = os.path.dirname(HIER)
K5 = os.path.dirname(RUHE)
FORM = os.path.normpath(os.path.join(K5, '..', '..'))
WT = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
for p in (WT + '/re15_port/tools', K5 + '/farbe_lesung/P_skripte', K5 + '/farbe_lesung/B_skripte', K5 + '/farbe_lesung/V4F_skripte', K5 + '/generator'):
    sys.path.insert(0, p)
import fl            # noqa
import render as R   # noqa  (Familien-Tafel: Licht 0,55 + 0,45 max(0, N.L), Umkugel 0,46, 380 px)
import p_lade as PL  # noqa
import v4f           # noqa
import form_buehne as FB  # noqa
P = v4f.P
BASIS = FORM + '/stufe3/basis_k5c'
K5D = K5 + '/feinschliff/K5d'
OUT = RUHE + '/messung'
SCR = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5e'
WINKEL4 = (('3/4 vorn', 45, 15), ('Seite', 0, 8), ('3/4 hinten', -135, 20), ('Gegenseite', 180, 8))


class _Tex:
    def __init__(self, mo):
        self.px = mo.tex
        self.h, self.w = mo.tex.shape
        self.clut_y = mo.clut_y
        a = (mo.pal.sum(2) > 0) * 255
        self.lut = np.concatenate([mo.pal.astype(np.uint8), a[..., None].astype(np.uint8)], -1)


def modelle():
    g4 = PL.re2(0x34); g4.T = _Tex(g4)
    em = fl.lade_em30()
    k5c = fl.lade_var(BASIS)
    k5d = fl.lade_var(K5D)
    return [('G4', 'RE2-G4 Original (Typ 0x34, Clip 0)', g4, 0, None),
            ('EM30', 'EM30_HANGAR RE1.5 Original (Clip 0)', em, 0, None),
            ('K5c', 'K5c Spiel-Asset (Clip 2 Ruhe)', k5c, 2, BASIS),
            ('K5d', 'K5d abgelehnt (Clip 2 Ruhe)', k5d, 2, K5D)]


def kf(mo, clip):
    return R.kf(mo, clip, 0)


def weiss(mo, clip, az, el, gross=380):
    return R.blick(mo, kf(mo, clip), az, el, gross, None)


EM30_KLEIDUNG = (0, 1, 2, 3, 4, 6)   # EM30-Meshes mit Jeansblau/Schuhen: Median-Farbton -66..-109 Grad (ruhe_lauf, weisses Licht, gemessen)


def haut(key, K):
    """Hautmaske: ganze Figur; bei EM30 ohne Kleidung (Hose, Schuhe), weil K5e keine Kleidung hat."""
    m = K['maske'].copy()
    if key == 'EM30':
        m &= ~np.isin(K['mesh'], EM30_KLEIDUNG)
    return m


def szene_k(mo, n, ordner):
    """Wie szene(), mit Kennungspuffer (v4f.dreiecke_id/rastern_id; fuer K5c/K5d = v4f.szene_id)."""
    if ordner:
        return v4f.szene_id(ordner, n)
    k = v4f.kand(BASIS)
    lage, gier, _c, _b = k.stand(n)
    hg = P.lade_rgb(P.daten('hintergrund_bild%d.png' % n))
    kam = P.kamera(n)
    licht, _k = P.licht_fuer(n, 'mess')
    s = hg.copy()
    K = v4f.rastern_id(v4f.dreiecke_id(mo, kf(mo, 0), lage, gier, kam, licht), mo.T, s, kam.abbildung)
    return s, K


def szene(mo, n, ordner):
    """Szenenkette Bild n: Disc-Licht x k (pruefung/licht.json) ueber Hintergrund; K5c/K5d ueber v4f.szene_id (= Kandidat.rendern),
    G4/EM30 mit Clip 0 an Lage/Gier der K5c (basis_k5c/form.json)."""
    if ordner:
        s, K = v4f.szene_id(ordner, n)
        return s, K['maske']
    k = v4f.kand(BASIS)
    lage, gier, _c, _b = k.stand(n)
    hg = P.lade_rgb(P.daten('hintergrund_bild%d.png' % n))
    kam = P.kamera(n)
    licht, _k = P.licht_fuer(n, 'mess')
    s = hg.copy()
    m = FB.rastern(FB.dreiecke(mo, kf(mo, 0), lage, gier, kam, licht), mo.T, s, kam.abbildung)
    return s, m


def weiss_id(mo, clip, az, el, gross=380):
    """Wie weiss() (R.blick: Licht 0,55 + 0,45 max(0, N.L)), aber mit Kennungspuffer (mesh, flaeche, zeile, eintrag) ueber v4f.rastern_id.
    Gegenprobe gegen R.blick: pixelgleich (ruhe_lauf.py prueft)."""
    import math
    R_, T_ = mo.pose(kf(mo, clip))
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    tl, pts = [], []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = R_[b].astype(float) / 4096.0
        t = T_[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ W.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
            tl.append((b, v, nn, faces, uvs, viereck))
            pts.append(v)
    alle = np.vstack(pts)
    mitte = (alle.max(0) + alle.min(0)) / 2
    s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
    aus = []
    for b, v, nn, faces, uvs, viereck in tl:
        q = (v - mitte) @ A.T
        sx = gross / 2 + q[:, 0] * s
        sy = gross / 2 + q[:, 1] * s
        hell = np.clip(R.AMB + R.DIF * np.maximum(nn @ L, 0), 0, 1) * 255.0
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
    img = np.full((gross, gross, 3), 92, np.uint8)
    K = v4f.rastern_id(aus, mo.T, img, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return img, K
