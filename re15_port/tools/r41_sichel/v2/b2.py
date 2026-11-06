"""Spur SF "Schwanz V2": gemeinsame Helfer (Buehne in der Spielkamera mit Knochen-Kennung je Bildpunkt, Masse in H).

Zeichenweg = tools/birkin_buehne.py (Pose re15_skel_compute_pose, GTE-Projektion, RDT-Licht des Schnitts, Homographie
Port 320x240 -> Video 640x480 aus SF_belege/kamera_50A0_12.json). Zusaetzlich schreibt der Rasterer je Bildpunkt den
Knochen, der ihn zuletzt (= vorn, Maler-Reihenfolge wie der Port) bemalt hat; damit sind Schwanz (Knochen 16-22),
Kopf (9), Sichel (15), Klaue (12) im Bild getrennt messbar.

Koordinaten: Videobild 640x480; die Leisten zeigen es x2 (1280x960 wie das Formblatt V2).
H = Umriss-Oberkante bis Sohle im selben Bild (Formblatt V2 Abschnitt 0).

⛔ Werkzeug der Nachbildung, keine RE-Werte.
"""
import os, sys, json, math
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, TOOLS)
import birkin_buehne as BB          # noqa: E402
import emd_ansichtsblatt as EA      # noqa: E402

BAUM = os.path.dirname(os.path.dirname(TOOLS))
BELEGE = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege')
VIDEO = os.path.join(BELEGE, 'roh', 'alle', 'f%04d.png')
V80 = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(BAUM))), 'Version_80%')
if not os.path.isdir(V80):
    V80 = r'C:/workspace/git/reAi_v2/Version_80%'
SCHWANZ = tuple(range(16, 23))

# Zuordnung birkinNN -> (Videobild n, Versatz im 1280x960-Bild); SF_LESER_A/B Abschnitt 1, Formblatt V2 Punkt 1
KI_MAP = {1: (1351, 620, 332), 2: (1354, 617, 326), 3: (1370, 471, 218), 4: (1374, 391, 208), 5: (1411, 538, 273),
          6: (1419, 512, 325), 7: (1421, 567, 338), 8: (1423, 490, 348), 9: (1420, 539, 334), 10: (1422, 582, 341),
          11: (1433, 548, 368), 12: (1435, 547, 366), 13: (1442, 545, 365), 14: (1447, 514, 362), 15: (1451, 596, 351),
          16: (1455, 598, 334), 17: (1459, 595, 320), 18: (1462, 597, 315), 19: (1468, 606, 315), 20: (1475, 584, 293),
          21: (1480, 567, 293), 22: (1476, 581, 292), 23: (1478, 571, 295), 24: (1482, 564, 284)}
KI_VON_H = {n - 1350: k for k, (n, _x, _y) in KI_MAP.items()}

# Video-Schwarzwert (c6.py NB6: Video = 0,954 x Port + 12,65 im Dunklen)
VSCHWARZ, VGAIN = 12.65, 0.954


def video(h):
    return cv2.cvtColor(cv2.imread(VIDEO % (1350 + h)), cv2.COLOR_BGR2RGB)


def video_sw(h):
    v = video(h).astype(np.float32)
    return np.clip((v - VSCHWARZ) / VGAIN, 0, 255).astype(np.uint8)


def ki(h, hinter=(70, 70, 70)):
    """KI-Freisteller (birkinNN) als 640x480-Bild auf grauem Grund + Maske, oder None."""
    k = KI_VON_H.get(h)
    if k is None:
        return None, None
    n, ox, oy = KI_MAP[k]
    a = cv2.cvtColor(cv2.imread(os.path.join(V80, 'birkin%02d.png' % k), cv2.IMREAD_UNCHANGED), cv2.COLOR_BGRA2RGBA)
    I = np.zeros((960, 1280, 4), np.uint8)
    hh, ww = a.shape[:2]
    I[oy:oy + hh, ox:ox + ww] = a
    al = I[..., 3:4].astype(np.float32) / 255.0
    rgb = (I[..., :3] * al + np.array(hinter, np.float32) * (1 - al)).astype(np.uint8)
    return rgb, I[..., 3] >= 128          # 1280x960


def modell(pfad_oder_blob):
    blob = pfad_oder_blob if isinstance(pfad_oder_blob, (bytes, bytearray)) else open(pfad_oder_blob, 'rb').read()
    return BB.modell(bytes(blob))


_raum = None
def raum():
    global _raum
    if _raum is None:
        r = BB.Raum('50A0')
        s = r.schnitt(12)
        _raum = (r, BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7), BB.abbildung_laden('50A0', 12), r.licht(12))
    return _raum


def rastern_ids(tris, mo, ziel, M):
    """birkin_buehne.rastern + Knochen-Kennung je Bildpunkt (-1 = leer)."""
    H, W = ziel.shape[:2]
    ids = np.full((H, W), -1, np.int16)
    for _t, p, uv, zeile, farbe, b in sorted(tris, key=lambda t: -t[0]):
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
        rgba = mo.lut[z if 0 <= z < mo.lut.shape[0] else 0][mo.tex[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]; s[tref] = rgb[tref]
        si = ids[y0:y1, x0:x1]; si[tref] = b
    return ids


def dreiecke_ids(mo, kf, lage, gier, kam, licht, R_T=None, knochen=None):
    """birkin_buehne.dreiecke mit Knochen je Dreieck; R_T = (R, T) ersetzt mo.pose(kf) (z. B. Kette nach Feder)."""
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = R_T if R_T is not None else mo.pose(kf)
    lage = np.array(lage, dtype=np.int64)
    ctx = BB.licht_kontext(licht, lage) if licht else None
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
            q = (VW @ np.array([x[:3] for x in verts], dtype=np.int64).T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = BB._gte(vx, vy, vz, kam.h)
            if ctx is not None:
                L = (W.T @ ctx[0].T).T >> 12
                f = np.minimum((BB.ncct(L, ctx[1], ctx[2], np.array([x[:3] for x in norms], dtype=np.int64)) * 0xFF + 0x40)
                               // 0x80, 0xFF)
            else:
                f = np.full((len(norms), 3), 255, dtype=np.int64)
            for g, uv in zip(faces, uvs):
                ie, inn = g[1::2], g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = BB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po, zeile = (uv[5] & 0xF) * 128, (uv[2] >> 6) & 0x1FF
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile,
                                    (f[inn[a]], f[inn[bb]], f[inn[cc]]), b))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = sum(int(vz[i]) for i in ie) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (f[inn[0]], f[inn[1]], f[inn[2]]), b))
    return aus


def zeichne(mo, pose, hintergrund=None, R_T=None, knochen=None, licht=True):
    """pose = dict(clip, bild, gier, x, z[, y]) -> (Bild 640x480 RGB, ids). Hintergrund 640x480 RGB oder None (grau)."""
    r, kam, M, li = raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    lage = (pose['x'], pose.get('y', 0), pose['z'])
    ziel = np.array(hintergrund, copy=True) if hintergrund is not None else np.full((480, 640, 3), 60, np.uint8)
    tris = dreiecke_ids(mo, kf, lage, int(pose['gier']), kam, li if licht else None, R_T=R_T, knochen=knochen)
    ids = rastern_ids(tris, mo, ziel, M)
    return ziel, ids


def projiziere(pose, p_modell):
    """Modellpunkt(e) (N x 3, Knochenraum der Pose) -> Videopunkt(e) 640x480 (Gleitkomma, wie GTE ohne Rundung)."""
    r, kam, M, li = raum()
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    P = np.atleast_2d(np.asarray(p_modell, float)) @ Y.T + np.array([pose['x'], pose.get('y', 0), pose['z']], float)
    V = kam.V.astype(float) / 4096.0
    q = P @ V.T + kam.t.astype(float)
    sx = BB.OFX + q[:, 0] * kam.h / q[:, 2]
    sy = BB.OFY + q[:, 1] * kam.h / q[:, 2]
    x, y = BB.abbilden(sx + BB.PHASE, sy + BB.PHASE, M)
    return np.stack([x, y], 1)


def ok_sohle(ids, ausser=()):
    """Umriss-Oberkante und Sohle (Videozeilen) der Figur ohne die Knochen `ausser`."""
    m = (ids >= 0)
    for b in ausser:
        m &= ids != b
    ys = np.nonzero(m.any(1))[0]
    return (int(ys.min()), int(ys.max())) if len(ys) else (None, None)


def kette_punkte(mo, kf, R_T=None):
    """Gelenke 16..22 und Spitze im Modellraum (Pose kf)."""
    R, T = R_T if R_T is not None else mo.pose(kf)
    P = [T[b].astype(float) for b in SCHWANZ]
    L = float(np.linalg.norm(np.array(mo.skel.rel[22], float)))
    P.append(T[22].astype(float) + R[22].astype(float)[:, 1] / 4096.0 * L)
    return np.array(P)


def tafel(bild, titel, unten=None, f=2, box=None):
    """Kachel: Ausschnitt box (x0,y0,x1,y1 in 640x480) x f, Titel oben, Zahlenzeilen unten (Liste)."""
    x0, y0, x1, y1 = box or (0, 0, bild.shape[1], bild.shape[0])
    t = cv2.resize(np.ascontiguousarray(bild[y0:y1, x0:x1]), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    t = np.ascontiguousarray(t)
    schrift(t, titel, 6, 20, (255, 255, 0), 0.55)
    if unten:
        zeilen = unten if isinstance(unten, (list, tuple)) else [unten]
        band = np.full((20 * len(zeilen) + 6, t.shape[1], 3), 16, np.uint8)
        for i, z in enumerate(zeilen):
            schrift(band, z, 5, 17 + 20 * i, (255, 255, 255), 0.47)
        t = np.vstack([t, band])
    return t


def schrift(im, s, x, y, col=(255, 255, 255), sc=0.5):
    cv2.putText(im, s, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, sc, col, 1, cv2.LINE_AA)


def leiste(zeilen, kopf, pfad, q=88):
    """zeilen = Liste von Kachel-Listen; gleiche Hoehe je Zeile wird aufgefuellt."""
    reihen = []
    for r in zeilen:
        h = max(t.shape[0] for t in r)
        reihen.append(np.hstack([np.vstack([t, np.full((h - t.shape[0], t.shape[1], 3), 16, np.uint8)]) for t in r]))
    w = max(r.shape[1] for r in reihen)
    reihen = [np.hstack([r, np.full((r.shape[0], w - r.shape[1], 3), 16, np.uint8)]) for r in reihen]
    kz = kopf if isinstance(kopf, (list, tuple)) else [kopf]
    k = np.full((14 + 22 * len(kz), w, 3), 16, np.uint8)
    for i, z in enumerate(kz):
        schrift(k, z, 8, 22 + 22 * i, (255, 255, 255) if i else (0, 255, 255), 0.6 if i == 0 else 0.5)
    im = np.vstack([k] + reihen)
    os.makedirs(os.path.dirname(os.path.abspath(pfad)), exist_ok=True)
    cv2.imwrite(pfad, cv2.cvtColor(im, cv2.COLOR_RGB2BGR), [cv2.IMWRITE_JPEG_QUALITY, q])
    return im.shape


def umriss_schnell(mo, pose, knochen=None, R_T=None):
    """Nur der Umriss (ohne Textur/Transparenz): Dreiecke per fillConvexPoly in 640x480 (schnell, fuer Suchen)."""
    r, kam, M, li = raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    tris = dreiecke_ids(mo, kf, (pose['x'], pose.get('y', 0), pose['z']), int(pose['gier']), kam, None,
                        R_T=R_T, knochen=knochen)
    m = np.zeros((480, 640), np.uint8)
    for _t, p, _uv, _z, _f, _b in tris:
        x, y = BB.abbilden(np.array([q[0] + BB.PHASE for q in p], float), np.array([q[1] + BB.PHASE for q in p], float), M)
        cv2.fillConvexPoly(m, np.round(np.stack([x, y], 1) * 4).astype(np.int32), 1, lineType=cv2.LINE_8, shift=2)
    return m > 0
