#!/usr/bin/env python3
"""Offline-Buehne fuer Birkin-Formen im RE1.5-EMD-Format (EM30_*.EMD mit eingebetteter TIM), Spur SF Runde 41.

    python re15_port/tools/birkin_buehne.py vergleich --emd EM30_HANGAR.EMD --raum 50A0 --schnitt 12 \
        --clip 0x0B --bild 20 --lage -8475,0,-21675 --gier zur_kamera --video 1418 --aus blatt.png
    python re15_port/tools/birkin_buehne.py rundum --emd EM30_HANGAR.EMD --clip 0 --bild 0 --aus rundum.png

Zeichenweg wie tools/form_buehne.py (Zweig r41/form), hier fuer EMD-Blobs mit TIM in dir[8]:
    Pose      EA.Modell.pose = re15_skel_compute_pose
    Welt      Y(gier) = [c 0 s; 0 1 0; -s 0 c] (platform/pc/main.c nyaw), T = Y*T_knochen + Lage
    Blick     V, t aus RID pos/ziel (camera_common.c re15_camera_build_view, ganzzahlig)
    GTE       sx = 160 + (x * n) >> 16, n = H/z, H = fov >> 7; z < 64 verwirft die Flaeche
    Rueckseite Dreieck MAC0 <= 0, Viereck MAC0 < 0; Viereck = (a,b,d) + (a,d,c); fern zuerst
    Licht     RDT-Licht des Schnitts an der Aktorlage + NCCT je Ecke (re15_light_setup_actor), Gouraud
Abbildung Port-Bild 320x240 -> Videobild 640x480: Homographie aus SF_belege/kamera_50A0_12.json (ECC gegen BSS).

⛔ Werkzeug der Nachbildung. Keine Zahl hier ist ein RE-Wert ausser den zitierten Port-Wegen.
"""
import argparse, json, math, os, struct, sys
import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402

REPO = EA.REPO
PSX = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
BELEGE = os.path.join(REPO, 'analysis', 'befunde_runde41', 'SF_belege')
VIDEO_BILDER = os.path.join(BELEGE, 'roh', 'alle', 'f%04d.png')
OFX, OFY, PHASE = 160, 120, 0.375


# ═══════════════════════════════════════════════════ Modell aus EMD-Blob
def modell(blob):
    d = EA.emd_dir(blob)
    dir_off = EA.u32(blob, 0)
    mo = EA.Modell.__new__(EA.Modell)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = blob, 0x30, 0, len(blob)
    mo.meshes = EA.md1_parse(blob[d[7]:d[8]])
    mo.skel = EA.emr_parse(blob[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(blob[d[1]:d[2]])
    tim = blob[d[8]:dir_off]
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', tim, 8)
    c = np.array([struct.unpack_from('<%dH' % cw, tim, 20 + 2 * cw * z) for z in range(ch)], dtype=np.int64)
    mo.lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3, (c != 0) * 255], -1).astype(np.uint8)
    return mo


def kf_von_clip(mo, clip, bild):
    f0, n = mo.clips[clip]
    return mo.frames[f0 + (bild % n)] & 0xFFF


# ═══════════════════════════════════════════════════ Raum: RID + Licht
class Raum:
    def __init__(self, raum):
        self.b = open(os.path.join(PSX, 'STAGE%s' % raum[0], 'ROOM%s.RDT' % raum), 'rb').read()
        self.rid, self.licht_off = EA.u32(self.b, 0x24), EA.u32(self.b, 0x2C)

    def schnitt(self, i):
        _fl, fov, px, py, pz, tx, ty, tz, _p = struct.unpack_from('<HH6iI', self.b, self.rid + i * 32)
        return dict(fov=fov, pos=(px, py, pz), ziel=(tx, ty, tz))

    def licht(self, i):
        p, b = self.licht_off + i * 40, self.b
        return dict(skala=b[p], typ=tuple(b[p + 1:p + 4]),
                    farbe=[tuple(b[p + 4 + k * 3:p + 7 + k * 3]) for k in range(3)], umgebung=tuple(b[p + 13:p + 16]),
                    lage=[struct.unpack_from('<3h', b, p + 0x10 + k * 6) for k in range(3)],
                    hell=struct.unpack_from('<3H', b, p + 0x22))


def licht_kontext(c, aktor):
    L = np.zeros((3, 3), dtype=np.int64); C = np.zeros((3, 3), dtype=np.int64)
    for i in range(3):
        if c['typ'][i] == 0:
            d = [(int(aktor[k]) - int(c['lage'][i][k])) >> 4 for k in range(3)]
            rng = c['hell'][i] >> 4
            if rng <= 0:
                continue
            ab = rng - math.isqrt(d[0] * d[0] + d[2] * d[2])
            if ab <= 0:
                continue
            f = [c['farbe'][i][k] * ab // rng for k in range(3)]
        else:
            d, f = list(c['lage'][i]), list(c['farbe'][i])
        if not any(f) or not any(d):
            continue
        n = np.array(d, float)
        L[i] = -c['skala'] * np.rint(n / np.linalg.norm(n) * 4096.0).astype(np.int64)
        C[i] = np.array(f, dtype=np.int64) << 4
    return L, C, np.array(c['umgebung'], dtype=np.int64)


def ncct(L_lokal, C, umgebung, N):
    d = np.clip((L_lokal @ N.T) >> 12, 0, 0x7FFF)
    ir = np.clip(((umgebung[:, None] << 16) + (C.T @ d)) >> 12, 0, 0x7FFF)
    return np.clip((((128 * ir) << 4) >> 12) >> 4, 0, 255).T


# ═══════════════════════════════════════════════════ Kamera
class Kamera:
    def __init__(self, pos, ziel, h):
        px, py, pz = (int(c) for c in pos)
        dx, dy, dz = int(ziel[0]) - px, int(ziel[1]) - py, int(ziel[2]) - pz
        dist, hz = math.isqrt(dx*dx + dy*dy + dz*dz), math.isqrt(dx*dx + dz*dz)

        def tdiv(a, b):
            q = abs(a) // abs(b)
            return q if (a < 0) == (b < 0) else -q
        sp, cp = tdiv(-dy * 4096, dist), tdiv(hz * 4096, dist)
        sy, cy = tdiv(dx * 4096, hz), tdiv(dz * 4096, hz)
        self.V = np.array([cy, 0, -sy, (sp*sy) >> 12, cp, (sp*cy) >> 12, (cp*sy) >> 12, -sp, (cp*cy) >> 12],
                          dtype=np.int64).reshape(3, 3)
        self.t = (self.V @ np.array([-px, -py, -pz], dtype=np.int64)) >> 12
        self.h, self.pos = int(h), (px, py, pz)


def gier_zur_kamera(lage, kam):
    """Gier, bei der die Vorderseite (+X) zur Kamera zeigt: Y(g)*(1,0,0) = (c, 0, -s)."""
    dx, dz = kam.pos[0] - lage[0], kam.pos[2] - lage[2]
    return int(round(math.atan2(-dz, dx) * 4096 / (2 * math.pi))) & 0xFFF


def _gte(vx, vy, vz, h):
    z = np.clip(vz, 1, 0xFFFF)
    n = np.minimum(((h * 0x20000) // z + 1) // 2, 0x1FFFF)
    return OFX + ((np.clip(vx, -0x8000, 0x7FFF) * n) >> 16), OFY + ((np.clip(vy, -0x8000, 0x7FFF) * n) >> 16)


def _mac0(p0, p1, p2):
    return p0[0]*p1[1] + p1[0]*p2[1] + p2[0]*p0[1] - p0[0]*p2[1] - p1[0]*p0[1] - p2[0]*p1[1]


def dreiecke(mo, kf, lage, gier, kam, licht=None):
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array(lage, dtype=np.int64)
    ctx = licht_kontext(licht, lage) if licht else None
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW, vT = (kam.V @ W) >> 12, ((kam.V @ t) >> 12) + kam.t
        for verts, norms, faces, uvs, viereck in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            q = (VW @ np.array([x[:3] for x in verts], dtype=np.int64).T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = _gte(vx, vy, vz, kam.h)
            if ctx is not None:
                L = (W.T @ ctx[0].T).T >> 12
                f = np.minimum((ncct(L, ctx[1], ctx[2], np.array([x[:3] for x in norms], dtype=np.int64)) * 0xFF + 0x40)
                               // 0x80, 0xFF)
            else:
                f = np.full((len(norms), 3), 255, dtype=np.int64)
            for g, uv in zip(faces, uvs):
                ie, inn = g[1::2], g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = _mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po, zeile = (uv[5] & 0xF) * 128, (uv[2] >> 6) & 0x1FF
                if viereck:
                    tx = [(uv[0]+po, uv[1]), (uv[3]+po, uv[4]), (uv[6]+po, uv[7]), (uv[9]+po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile,
                                    (f[inn[a]], f[inn[bb]], f[inn[cc]])))
                else:
                    tx = [(uv[0]+po, uv[1]), (uv[3]+po, uv[4]), (uv[6]+po, uv[7])]
                    tief = sum(int(vz[i]) for i in ie) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (f[inn[0]], f[inn[1]], f[inn[2]])))
    return aus


def abbilden(x, y, M):
    w = M[2, 0]*x + M[2, 1]*y + M[2, 2]
    return (M[0, 0]*x + M[0, 1]*y + M[0, 2]) / w, (M[1, 0]*x + M[1, 1]*y + M[1, 2]) / w


def rastern(tris, mo, ziel, M):
    H, W = ziel.shape[:2]
    maske = np.zeros((H, W), bool)
    for _t, p, uv, zeile, farbe in sorted(tris, key=lambda t: -t[0]):
        x, y = abbilden(np.array([q[0] + PHASE for q in p], float), np.array([q[1] + PHASE for q in p], float), M)
        fl = (x[1]-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(y[1]-y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(PY-y[0])) / fl
        w2 = ((x[1]-x[0])*(PY-y[0]) - (PX-x[0])*(y[1]-y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float); v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0*u[0] + w1*u[1] + w2*u[2]).astype(int), 0, mo.texw - 1)
        vv = np.clip((w0*v[0] + w1*v[1] + w2*v[2]).astype(int), 0, mo.texh - 1)
        z = zeile - mo.clut_y
        rgba = mo.lut[z if 0 <= z < mo.lut.shape[0] else 0][mo.tex[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None]*c0 + w1[..., None]*c1 + w2[..., None]*c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]; s[tref] = rgb[tref]
        ms = maske[y0:y1, x0:x1]; ms[tref] = True
    return maske


# ═══════════════════════════════════════════════════ Abbildung Port -> Video
def abbildung_laden(raum, schnitt):
    p = os.path.join(BELEGE, 'kamera_%s_%d.json' % (raum, schnitt))
    if os.path.exists(p):
        return np.array(json.load(open(p, encoding='utf-8'))['M'], float)
    return np.diag([2.0, 2.0, 1.0])


def render(blob, raum, schnitt, clip, bild, lage, gier, hintergrund=None, M=None, licht=True):
    mo = modell(blob)
    r = Raum(raum)
    s = r.schnitt(schnitt)
    kam = Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
    if gier == 'zur_kamera':
        gier = gier_zur_kamera(lage, kam)
    M = abbildung_laden(raum, schnitt) if M is None else M
    ziel = np.array(hintergrund, copy=True) if hintergrund is not None else np.full((480, 640, 3), 40, np.uint8)
    tris = dreiecke(mo, kf_von_clip(mo, clip, bild), lage, int(gier), kam, r.licht(schnitt) if licht else None)
    return ziel, rastern(tris, mo, ziel, M), gier


def boden_aus_video(raum, schnitt, vx, vy, boden_y=0):
    """Videopixel -> Weltpunkt auf der Ebene y = boden_y (Umkehrung von Kamera + Abbildung, Gleitkomma)."""
    M = abbildung_laden(raum, schnitt)
    u, w, s = np.linalg.inv(M) @ np.array([vx, vy, 1.0])
    u, w = u / s, w / s
    r = Raum(raum).schnitt(schnitt)
    kam = Kamera(r['pos'], r['ziel'], r['fov'] >> 7)
    V = kam.V.astype(float) / 4096.0
    d = V.T @ np.array([(u - OFX) / kam.h, (w - OFY) / kam.h, 1.0])      # Blickstrahl in Weltachsen
    o = np.array(kam.pos, float)
    t = (boden_y - o[1]) / d[1]
    return [int(round(c)) for c in o + t * d]


def umriss(m):
    return m & ~(np.roll(m, 1, 0) & np.roll(m, -1, 0) & np.roll(m, 1, 1) & np.roll(m, -1, 1))


def tafelblatt(tafeln, ziel, kopf, spalten=None):
    h, w = tafeln[0][1].shape[:2]
    sp = spalten or len(tafeln)
    zeilen = (len(tafeln) + sp - 1) // sp
    pad = 10
    b = Image.new('RGB', (pad + sp * (w + pad), 30 + zeilen * (h + 26)), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 6), kopf, font=EA.font(15), fill=(170, 173, 180))
    for i, (titel, a) in enumerate(tafeln):
        x, y = pad + (i % sp) * (w + pad), 30 + (i // sp) * (h + 26)
        b.paste(Image.fromarray(np.ascontiguousarray(a)), (x, y))
        d.text((x, y + h + 3), titel, font=EA.font(14, True), fill=(214, 138, 74))
    os.makedirs(os.path.dirname(os.path.abspath(ziel)), exist_ok=True)
    b.save(ziel)


def video_bild(n):
    return np.array(Image.open(VIDEO_BILDER % n).convert('RGB'))


# ═══════════════════════════════════════════════════ Rundum (hell, ohne Raum)
def rundum(blob, clip, bild, ziel, kopf, schritte=8, groesse=360):
    mo = modell(blob)
    kf = kf_von_clip(mo, clip, bild)
    V, N, F = mo.geometrie(kf)
    tafeln = []
    mitte = (V.min(0) + V.max(0)) / 2
    ausd = (V.max(0) - V.min(0)).max()
    for k in range(schritte):
        a = 2 * math.pi * k / schritte
        depth = np.array([math.cos(a), 0.0, math.sin(a)])   # Blick auf +X-Seite bei a = pi
        right = np.array([-math.sin(a), 0.0, math.cos(a)])
        down = np.array([0.0, 1.0, 0.0])
        col, alp = EA.render(mo, V - mitte, N, F, right, down, depth, groesse, groesse, groesse * 0.9 / ausd,
                             groesse / 2, groesse / 2)
        bild_ = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
        bild_[alp == 0] = (92, 96, 104)
        namen = {0: 'vorn', 2: 'linke Flanke', 4: 'hinten', 6: 'rechte Flanke'}
        tafeln.append(('%d Grad %s' % (k * 360 // schritte, namen.get(k, '')), bild_))
    tafelblatt(tafeln, ziel, kopf, spalten=4)


def streifen(blob, clip, ziel, kopf, schritt=4, winkel=135, groesse=300, spalten=8):
    """Bildfolge eines Clips aus fester Blickrichtung (Grad, 0 = vorn), gleicher Massstab fuer alle Bilder."""
    mo = modell(blob)
    f0, n = mo.clips[clip]
    a = math.radians(winkel)
    depth = np.array([math.cos(a), 0.0, math.sin(a)])
    right = np.array([-math.sin(a), 0.0, math.cos(a)])
    down = np.array([0.0, 1.0, 0.0])
    geo = [(i, mo.geometrie(mo.frames[f0 + i] & 0xFFF)) for i in range(0, n, schritt)]
    alle = np.vstack([g[1][0] for g in geo])
    mitte = (alle.min(0) + alle.max(0)) / 2
    ausd = (alle.max(0) - alle.min(0)).max()
    tafeln = []
    for i, (V, N, F) in geo:
        col, alp = EA.render(mo, V - mitte, N, F, right, down, depth, groesse, groesse, groesse * 0.92 / ausd,
                             groesse / 2, groesse / 2)
        b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
        b[alp == 0] = (92, 96, 104)
        tafeln.append(('Bild %d' % i, b))
    tafelblatt(tafeln, ziel, kopf, spalten=spalten)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('modus', choices=('vergleich', 'rundum', 'render', 'streifen'))
    ap.add_argument('--emd', required=True)
    ap.add_argument('--raum', default='50A0')
    ap.add_argument('--schnitt', type=int, default=12)
    ap.add_argument('--clip', type=lambda s: int(s, 0), default=0)
    ap.add_argument('--bild', type=int, default=0)
    ap.add_argument('--lage', default='-8475,0,-21675')
    ap.add_argument('--gier', default='zur_kamera')
    ap.add_argument('--video', type=int, help='Bildnummer im 480p-Video (SF_belege/roh/alle)')
    ap.add_argument('--titel', default='')
    ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    if a.modus == 'rundum':
        rundum(blob, a.clip, a.bild, a.aus, a.titel or os.path.basename(a.emd))
        return
    if a.modus == 'streifen':
        streifen(blob, a.clip, a.aus, a.titel or '%s Clip 0x%02X' % (os.path.basename(a.emd), a.clip),
                 winkel=int(a.gier) if a.gier != 'zur_kamera' else 135)
        return
    lage = [int(x) for x in a.lage.split(',')]
    gier = a.gier if a.gier == 'zur_kamera' else int(a.gier, 0)
    vid = video_bild(a.video) if a.video else None
    leer, maske, g = render(blob, a.raum, a.schnitt, a.clip, a.bild, lage, gier, None)
    if a.modus == 'render':
        Image.fromarray(leer).save(a.aus); return
    ueber, _m, _g = render(blob, a.raum, a.schnitt, a.clip, a.bild, lage, gier, vid)
    rand = vid.copy(); rand[umriss(maske)] = (0, 255, 255)
    tafelblatt([('Video Bild %d' % a.video, vid), ('Render ueber Video', ueber), ('Umriss Render auf Video', rand)],
               a.aus, '%s  ROOM%s Cut %d  Clip 0x%02X Bild %d  Lage %s  Gier %d' %
               (a.titel or os.path.basename(a.emd), a.raum, a.schnitt, a.clip, a.bild, a.lage, g))


if __name__ == '__main__':
    main()
