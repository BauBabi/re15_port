#!/usr/bin/env python3
"""Ansichtsblatt eines RE1.5-Gegnermodells: 4 Seiten + Draufsicht, Skelett, Textur.

Liest das Modell direkt aus dem Original-Container (shared_assets/PSX/EMD/CDEMD0.EMS)
und stellt es in Orthogonalprojektion nach dritter Winkelprojektion dar.

    python re15_port/tools/emd_ansichtsblatt.py                        # Gorilla 0x27
    python re15_port/tools/emd_ansichtsblatt.py --typ 0x20 --clip 0    # Hund
    python re15_port/tools/emd_ansichtsblatt.py --einzel               # + Einzelansichten

Alles Format- und Posenrelevante ist ein Port der Engine-Leser, nicht geraten:
    EMS-Blobkette      re15_port/engine/src/re15_ems.c        (re15_ems_get_entry)
    Blob je Typ        s_ems_order[]                          (re15_ems.c)
    Containerdirectory re15_port/engine/src/emd_common.c:455  dir[1]=EDD dir[2]=EMR
                                                              dir[7]=MD1 dir[8]=TIM
    MD1-Mesh           re15_port/include/re15/re15_md1.h
    EMR-Skelett        emd_common.c re15_emd_parse_skeleton
    12-Bit-Euler       emd_common.c re15_emd_get_keyframe_angles
    RotMatrix          re15_port/engine/src/skeleton_common.c:133  (@0x80068130)
    Posenkette         skeleton_common.c:659-710  (re15_skel_compute_pose)

NICHT byte-true und bewusst so: Orthogonalprojektion, Z-Buffer und die
Lambert-Beleuchtung dienen allein der Lesbarkeit. Geometrie, Skelett, Pose und
Textur stammen unveraendert aus dem Original.
"""
import argparse, math, os, struct, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont

REPO       = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EMS_PFAD   = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'EMD', 'CDEMD0.EMS')
EMS_ORDER  = [0x10, 0x11, 0x12, 0x13, 0x16, 0x18, 0x1A, 0x20, 0x21, 0x24,
              0x25, 0x26, 0x27, 0x29, 0x2B, 0x2D, 0x30, 0x36, 0x40, 0x42,
              0x45, 0x47, 0x49, 0x4B, 0x4D]          # re15_ems.c s_ems_order
EMS_ALIGN, EMS_FOOTER = 2048, 36

u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]


# ═══════════════════════════════════════════════════ Container: EMS -> EMD
def ems_entry(ems, index):
    """re15_ems_get_entry (re15_ems.c): sektorausgerichtete Blobkette."""
    cursor = i = 0
    n = len(ems)
    while cursor + 4 <= n:
        dir_off = u32(ems, cursor)
        if dir_off < EMS_FOOTER or (dir_off & 3):
            break
        length = dir_off + EMS_FOOTER
        if not length or cursor + length > n:
            break
        if i == index:
            return cursor, length
        i += 1
        nxt = cursor + length
        if nxt >= n:
            break
        aligned = (nxt + EMS_ALIGN - 1) & ~(EMS_ALIGN - 1)
        if aligned <= cursor:
            break
        cursor = aligned
        skipped = 0
        while cursor + 4 <= n and u32(ems, cursor) == 0:
            cursor += 4
            skipped += 4
        if skipped:
            cursor = (cursor + EMS_ALIGN - 1) & ~(EMS_ALIGN - 1)
    raise SystemExit('EMS: Blob %d nicht gefunden' % index)


def emd_dir(emd):
    return [u32(emd, u32(emd, 0) + i*4) for i in range(u32(emd, 4))]


# ═══════════════════════════════════════════════════ MD1-Mesh (re15_md1.h)
class Mesh:
    pass


def md1_parse(d):
    _, _, obj = struct.unpack_from('<III', d, 0)
    base, meshes = 12, []
    for i in range(obj // 2):
        (tvo, tvc, tno, tnc, tfo, tfc, tuo,
         qvo, qvc, qno, qnc, qfo, qfc, quo) = struct.unpack_from('<14I', d, 12 + i*56)
        m = Mesh()
        m.tv  = [struct.unpack_from('<4h', d, base+tvo+k*8)  for k in range(tvc)]
        m.tn  = [struct.unpack_from('<4h', d, base+tno+k*8)  for k in range(tnc)]
        m.tf  = [struct.unpack_from('<6H', d, base+tfo+k*12) for k in range(tfc)]
        m.tuv = [struct.unpack_from('<BBHBBHBBH', d, base+tuo+k*12) for k in range(tfc)]
        m.qv  = [struct.unpack_from('<4h', d, base+qvo+k*8)  for k in range(qvc)]
        m.qn  = [struct.unpack_from('<4h', d, base+qno+k*8)  for k in range(qnc)]
        m.qf  = [struct.unpack_from('<8H', d, base+qfo+k*16) for k in range(qfc)]
        m.quv = [struct.unpack_from('<BBHBBHBBHBBH', d, base+quo+k*16) for k in range(qfc)]
        meshes.append(m)
    return meshes


# ═══════════════════════════════════════════════════ EMR-Skelett + EDD
class Skel:
    pass


def emr_parse(e):
    bones_tab, kf_off, nb, kf_size = (u16(e, 0), u16(e, 2), u16(e, 4), u16(e, 6))
    s = Skel()
    s.bone_count, s.kf_size = nb, kf_size
    s.rel = [(s16(e, 8+b*6), s16(e, 8+b*6+2), s16(e, 8+b*6+4)) for b in range(nb)]
    cc = [u16(e, bones_tab+b*4)     for b in range(nb)]
    co = [u16(e, bones_tab+b*4 + 2) for b in range(nb)]
    parents   = [-1] * nb
    child_max = len(e) - bones_tab
    for b in range(nb):                                  # emr_assign_parents
        if cc[b] <= 0 or co[b] >= child_max:
            continue
        for i in range(min(child_max - co[b], cc[b])):
            ch = e[bones_tab + co[b] + i]
            if ch < nb and ch != b and parents[ch] == -1:
                parents[ch] = b
    s.parent, s.kf_data = parents, e[kf_off:]
    s.kf_count = len(s.kf_data) // kf_size if kf_size else 0
    return s


def bit_read(data, bitoff, count):
    v = 0
    for i in range(count):
        bi = (bitoff + i) >> 3
        if bi >= len(data):
            break
        v |= ((data[bi] >> ((bitoff + i) & 7)) & 1) << i
    return v


def kf_angles(s, kf, b):
    """re15_emd_get_keyframe_angles: 36 Bit je Knochen ab Byte +12, vorzeichenerweitert."""
    size = s.kf_size - 12
    ap   = s.kf_data[kf*s.kf_size + 12: kf*s.kf_size + 12 + size]
    if b*36 + 36 > size*8:
        return (0, 0, 0)
    out = []
    for k in range(3):
        v = bit_read(ap, b*36 + k*12, 12)
        out.append(v - 4096 if v & 0x800 else v)
    return tuple(out)


def kf_pos(s, kf):
    o = kf * s.kf_size
    return (s16(s.kf_data, o), s16(s.kf_data, o+2), s16(s.kf_data, o+4))


def edd_parse(d):
    count0, off0 = u16(d, 0), u16(d, 2)
    clips, first, cur = [(0, count0)], count0, 4
    for _ in range(1, off0 // 4):
        c = u16(d, cur)
        cur += 4
        clips.append((first, c))
        first += c
    return clips, [u32(d, off0 + i*4) for i in range(first)]


# ═══════════════════════════════════════════════════ TIM-Textur
def tim_decode(t):
    o  = 8
    cs = u32(t, o)
    cy = u16(t, o+6)
    cw, chh = u16(t, o+8), u16(t, o+10)
    clut = np.frombuffer(t[o+12:o+cs], dtype='<u2').reshape(chh, cw)
    o += cs
    iw, ih = u16(t, o+8), u16(t, o+10)
    px  = np.frombuffer(t[o+12:o+12+iw*2*ih], dtype=np.uint8).reshape(ih, iw*2)   # 8 bpp
    pal = np.stack([((clut       & 0x1f) * 255 // 31).astype(np.uint8),
                    ((clut >>  5 & 0x1f) * 255 // 31).astype(np.uint8),
                    ((clut >> 10 & 0x1f) * 255 // 31).astype(np.uint8)], -1)
    return px, pal, cy


# ═══════════════════════════════════════════════════ Posenmathematik
def sin_q12(a): return int(round(math.sin(a * 2*math.pi / 4096) * 4096))
def cos_q12(a): return int(round(math.cos(a * 2*math.pi / 4096) * 4096))


def _s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def mat_euler(ax, ay, az):
    """mat3_from_euler == RE1.5 RotMatrix (skeleton_common.c:133, Disasm @0x80068130).
    Wie dort: Abschneiden PRO Produkt, und bei m[1]/m[5] negieren VOR dem Schieben."""
    sx, cx = sin_q12(ax), cos_q12(ax)
    sy, cy = sin_q12(ay), cos_q12(ay)
    sz, cz = sin_q12(az), cos_q12(az)
    nsy = -sy
    m = [0]*9
    m[2] = _s16(sy)
    m[5] = _s16((-(cy*sx)) >> 12)
    m[8] = _s16((cy*cx) >> 12)
    m[0] = _s16((cz*cy) >> 12)
    m[1] = _s16((-(sz*cy)) >> 12)
    t1 = (cz*nsy) >> 12
    m[3] = _s16(_s16((sz*cx) >> 12) - _s16((t1*sx) >> 12))
    m[6] = _s16(_s16((sz*sx) >> 12) + _s16((t1*cx) >> 12))
    t2 = (sz*nsy) >> 12
    m[4] = _s16(_s16((cz*cx) >> 12) + _s16((t2*sx) >> 12))
    m[7] = _s16(_s16((cz*sx) >> 12) - _s16((t2*cx) >> 12))
    return np.array(m, dtype=np.int64).reshape(3, 3)


class Modell:
    """Ein geladenes EMD: Meshes, Skelett, Clips, Textur — plus Pose und Geometrie."""

    def __init__(self, typ):
        ems = open(EMS_PFAD, 'rb').read()
        if typ not in EMS_ORDER:
            raise SystemExit('Typ 0x%02x liegt nicht in CDEMD0.EMS (s_ems_order)' % typ)
        off, ln    = ems_entry(ems, EMS_ORDER.index(typ))
        self.blob  = ems[off:off+ln]
        self.typ, self.blob_index, self.blob_len = typ, EMS_ORDER.index(typ), ln
        d = emd_dir(self.blob)
        self.meshes         = md1_parse(self.blob[d[7]:d[8]])
        self.skel           = emr_parse(self.blob[d[2]:d[3]])
        self.clips, self.frames = edd_parse(self.blob[d[1]:d[2]])
        self.tex, self.pal, self.clut_y = tim_decode(self.blob[d[8]:])
        self.texh, self.texw = self.tex.shape

    # -- Pose ---------------------------------------------------------------
    def pose(self, keyframe):
        """re15_skel_compute_pose: rot = Elternrot * lokal, t = Elternrot * rel + Elternt."""
        n = self.skel.bone_count
        R, T = [None]*n, [None]*n
        wurzel = kf_pos(self.skel, keyframe)
        for b in range(n):
            loc = mat_euler(*kf_angles(self.skel, keyframe, b))
            p   = self.skel.parent[b]
            if p < 0 or p >= b:
                R[b], T[b] = loc, np.array(wurzel, dtype=np.int64)
            else:
                R[b] = (R[p] @ loc) >> 12
                T[b] = ((R[p] @ np.array(self.skel.rel[b], dtype=np.int64)) >> 12) + T[p]
        return R, T

    # -- Geometrie ----------------------------------------------------------
    def geometrie(self, keyframe):
        """Alle Mesh-Dreiecke in Weltkoordinaten. Mesh-Index == Knochen-Index
        (so machen es beide Renderer des Ports, siehe emd_common.c Kommentar)."""
        R, T = self.pose(keyframe)
        V, N, F = [], [], []
        for b in range(min(self.skel.bone_count, len(self.meshes))):
            m   = self.meshes[b]
            rot = R[b].astype(np.float64) / 4096.0
            tr  = T[b].astype(np.float64)

            def xf(vs):
                if not vs:
                    return np.zeros((0, 3))
                return np.array([[v[0], v[1], v[2]] for v in vs], float) @ rot.T + tr

            def xn(ns):
                if not ns:
                    return np.zeros((0, 3))
                w  = np.array([[v[0], v[1], v[2]] for v in ns], float) @ rot.T
                ln = np.linalg.norm(w, axis=1, keepdims=True)
                ln[ln == 0] = 1
                return w / ln

            tvb = len(V); V.extend(xf(m.tv)); N.extend(xn(m.tn))
            qvb = len(V); V.extend(xf(m.qv)); N.extend(xn(m.qn))
            for i, (n0, v0, n1, v1, n2, v2) in enumerate(m.tf):
                uv = m.tuv[i]
                F.append(dict(v=(tvb+v0, tvb+v1, tvb+v2), n=(tvb+n0, tvb+n1, tvb+n2),
                              uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])),
                              clut=uv[2], page=uv[5], bone=b))
            for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m.qf):
                uv = m.quv[i]
                a, bb = (uv[0], uv[1]), (uv[3], uv[4])
                c, dd = (uv[6], uv[7]), (uv[9], uv[10])
                F.append(dict(v=(qvb+v0, qvb+v1, qvb+v2), n=(qvb+n0, qvb+n1, qvb+n2),
                              uv=(a, bb, c), clut=uv[2], page=uv[5], bone=b))
                F.append(dict(v=(qvb+v1, qvb+v3, qvb+v2), n=(qvb+n1, qvb+n3, qvb+n2),
                              uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=b))
        return np.array(V), np.array(N), F

    # -- Textur -------------------------------------------------------------
    def clut_row(self, clut_id):
        return max(0, min(self.pal.shape[0]-1, (clut_id >> 6) - self.clut_y))

    def page_ubase(self, page):
        return (page & 0xF) * 128          # 8 bpp: eine TPage deckt 128 Pixel

    def texturblatt(self):
        """Jede TPage-Haelfte mit IHRER CLUT-Zeile — so liest das Spiel sie
        (gemessen: u < 128 -> CLUT 0 = Rumpf/Kopf, u >= 128 -> CLUT 1 = Gliedmassen)."""
        half = self.texw // 2
        if self.pal.shape[0] < 2:
            return self.pal[0][self.tex]
        return np.concatenate([self.pal[0][self.tex[:, :half]],
                               self.pal[1][self.tex[:, half:]]], axis=1)


# ═══════════════════════════════════════════════════ Rasterizer
def render(mo, V, N, F, right, down, depth, W, H, scale, cx, cy):
    right, down, depth = (np.asarray(v, float) for v in (right, down, depth))
    licht  = right*0.42 - down*0.52 + depth*0.75      # kamerarelativ: jede Ansicht gleich hell
    licht /= np.linalg.norm(licht)
    col = np.zeros((H, W, 3), np.float32)
    alp = np.zeros((H, W), np.float32)
    zbf = np.full((H, W), -1e18)
    sx, sy, sz = V @ right * scale + cx, V @ down * scale + cy, V @ depth
    shade = 0.40 + 0.60 * np.clip(N @ licht, 0, 1)
    for f in F:
        i0, i1, i2 = f['v']
        x = np.array([sx[i0], sx[i1], sx[i2]])
        y = np.array([sy[i0], sy[i1], sy[i2]])
        z = np.array([sz[i0], sz[i1], sz[i2]])
        area = (x[1]-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(y[1]-y[0])
        if abs(area) < 1e-9:
            continue
        xmin = max(int(np.floor(x.min())), 0); xmax = min(int(np.ceil(x.max()))+1, W)
        ymin = max(int(np.floor(y.min())), 0); ymax = min(int(np.ceil(y.max()))+1, H)
        if xmin >= xmax or ymin >= ymax:
            continue
        PX, PY = np.meshgrid(np.arange(xmin, xmax)+0.5, np.arange(ymin, ymax)+0.5)
        w0 = ((x[1]-x[0])*(PY-y[0]) - (PX-x[0])*(y[1]-y[0])) / area
        w1 = ((PX-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(PY-y[0])) / area
        w2 = 1.0 - w0 - w1
        inside = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not inside.any():
            continue
        zz  = w2*z[0] + w1*z[1] + w0*z[2]
        sub = zbf[ymin:ymax, xmin:xmax]
        hit = inside & (zz > sub)
        if not hit.any():
            continue
        n0, n1, n2 = f['n']
        sh = w2*shade[n0] + w1*shade[n1] + w0*shade[n2]
        (u0, v0), (u1, v1), (u2, v2) = f['uv']
        ub = mo.page_ubase(f['page'])
        uu = np.clip((w2*(u0+ub) + w1*(u1+ub) + w0*(u2+ub)).astype(int), 0, mo.texw-1)
        vv = np.clip((w2*v0 + w1*v1 + w0*v2).astype(int), 0, mo.texh-1)
        rgb = mo.pal[mo.clut_row(f['clut'])][mo.tex[vv, uu]].astype(np.float32) / 255.0
        c   = rgb * sh[..., None]
        cs  = col[ymin:ymax, xmin:xmax]; cs[hit] = c[hit]
        col[ymin:ymax, xmin:xmax] = cs
        sub[hit] = zz[hit]; zbf[ymin:ymax, xmin:xmax] = sub
        asub = alp[ymin:ymax, xmin:xmax]; asub[hit] = 1.0
        alp[ymin:ymax, xmin:xmax] = asub
    return col, alp


# ═══════════════════════════════════════════════════ Blatt
SS, TILE, PAD, LBL_H = 3, 470, 26, 32
BG, CELL_BG, GRID    = (18, 19, 22), (28, 30, 35), (48, 52, 60)
FG, DIM, ACC         = (232, 233, 236), (150, 153, 160), (214, 138, 74)

# Charakter-Vorderseite = +X, Y = unten (+), Z = seitlich.
# oben x vorn = (0,-1,0) x (1,0,0) = (0,0,1)  =>  +Z ist die LINKE Koerperseite.
# Alle Bildachsen erfuellen right x down = Blickrichtung, also ist keine Ansicht gespiegelt.
V_TOP   = ((0, 0,  1), (1, 0, 0), (0, -1,  0))   # Draufsicht; vorn zeigt nach unten (3. Winkel)
V_LEFT  = ((-1, 0, 0), (0, 1, 0), (0,  0,  1))   # Kamera bei +Z = linke Flanke
V_FRONT = ((0, 0,  1), (0, 1, 0), (1,  0,  0))
V_RIGHT = ((1, 0,  0), (0, 1, 0), (0,  0, -1))   # Kamera bei -Z = rechte Flanke
V_BACK  = ((0, 0, -1), (0, 1, 0), (-1, 0,  0))


def font(sz, bold=False):
    cands = [r'C:\Windows\Fonts\segoeuib.ttf'] if bold else [r'C:\Windows\Fonts\segoeui.ttf']
    cands += [r'C:\Windows\Fonts\consola.ttf', r'C:\Windows\Fonts\arial.ttf',
              '/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf']
    for p in cands:
        if os.path.exists(p):
            try:
                return ImageFont.truetype(p, sz)
            except Exception:
                pass
    return ImageFont.load_default()


def blatt(mo, clip, ziel, einzel=False):
    F_TITLE, F_SUB, F_VIEW = font(40, True), font(19), font(22, True)
    F_LBL, F_MONO, F_SMALL, F_TINY = font(16), font(15), font(14), font(12)

    kf0     = mo.frames[mo.clips[clip][0]] & 0xfff
    V, N, F = mo.geometrie(kf0)
    _, BT   = mo.pose(kf0)
    lo, hi  = V.min(0), V.max(0)
    ctr, ext = (lo+hi)/2.0, hi-lo
    SCALE   = (TILE * SS * 0.82) / ext.max()          # gemeinsamer Massstab aller Ansichten

    def project(right, down):
        r, d = np.asarray(right, float), np.asarray(down, float)
        cx = TILE/2 - float(np.dot(ctr, r)) * SCALE/SS
        cy = TILE/2 - float(np.dot(ctr, d)) * SCALE/SS
        return lambda P: (np.asarray(P, float) @ r * SCALE/SS + cx,
                          np.asarray(P, float) @ d * SCALE/SS + cy)

    def kachel(view):
        right, down, depth = view
        w = h = TILE * SS
        cx = w/2 - float(np.dot(ctr, right)) * SCALE
        cy = h/2 - float(np.dot(ctr, down))  * SCALE
        col, alp = render(mo, V, N, F, right, down, depth, w, h, SCALE, cx, cy)
        rgb = np.clip(col * 1.62, 0, 1) ** (1/1.18)
        img = Image.fromarray((rgb*255).astype(np.uint8)).convert('RGBA')
        img.putalpha(Image.fromarray((np.clip(alp, 0, 1)*255).astype(np.uint8)))
        return img.resize((TILE, TILE), Image.LANCZOS)

    COLS, ROWS = 4, 2
    HEAD, FOOT = 118, 160
    SHEET_W = PAD*2 + COLS*TILE + (COLS-1)*PAD
    SHEET_H = HEAD + PAD + ROWS*(TILE+LBL_H) + PAD + FOOT
    sheet = Image.new('RGB', (SHEET_W, SHEET_H), BG)
    dr    = ImageDraw.Draw(sheet)

    name_typ = {0x27: 'GORILLA-BOSS'}.get(mo.typ, 'EM%02X' % mo.typ)
    dr.text((PAD, 26), 'EM%03X - %s (Typ 0x%02x)' % (mo.typ, name_typ, mo.typ),
            font=F_TITLE, fill=FG)
    dr.text((PAD, 76), 'Resident Evil 1.5 - CDEMD0.EMS Blob %d - Bank 0, Clip 0x%02x, Keyframe %d'
            % (mo.blob_index, clip, kf0), font=F_SUB, fill=DIM)
    dr.line([(PAD, HEAD-8), (SHEET_W-PAD, HEAD-8)], fill=(60, 63, 72), width=1)

    cell_xy = lambda c, r: (PAD + c*(TILE+PAD), HEAD + PAD + r*(TILE+LBL_H))

    def cell_frame(c, r, grid=True):
        x0, y0 = cell_xy(c, r)
        dr.rectangle([x0, y0, x0+TILE-1, y0+TILE-1], fill=CELL_BG, outline=(58, 61, 70))
        if grid:
            for g in range(1, 4):
                dr.line([(x0+g*TILE//4, y0+1), (x0+g*TILE//4, y0+TILE-2)], fill=GRID)
                dr.line([(x0+1, y0+g*TILE//4), (x0+TILE-2, y0+g*TILE//4)], fill=GRID)
        return x0, y0

    def cell_label(c, r, nm, sub):
        x0, y0 = cell_xy(c, r)
        dr.text((x0, y0+TILE+5), nm, font=F_VIEW, fill=ACC)
        if sub:
            dr.text((x0+TILE-dr.textlength(sub, font=F_LBL), y0+TILE+9), sub, font=F_LBL, fill=DIM)

    def put_view(c, r, nm, sub, view, ground=True):
        x0, y0 = cell_frame(c, r)
        img = kachel(view)
        sheet.paste(img, (x0, y0), img)
        if einzel:
            img.save(os.path.join(os.path.dirname(ziel),
                                  'em%03x_%s.png' % (mo.typ, nm.lower())))
        if ground:
            gy = int(round(float(project(view[0], view[1])((0.0, 0.0, 0.0))[1])))
            if 0 < gy < TILE:
                dr.line([(x0+6, y0+gy), (x0+TILE-7, y0+gy)], fill=(120, 92, 60), width=1)
                dr.text((x0+8, y0+gy+3), 'y = 0  (Boden)', font=F_SMALL, fill=(150, 116, 76))
        cell_label(c, r, nm, sub)

    put_view(0, 1, 'LINKS',  'linke Flanke  (Kamera +Z)',  V_LEFT)
    put_view(1, 1, 'VORNE',  'von vorn  (Kamera +X)',      V_FRONT)
    put_view(2, 1, 'RECHTS', 'rechte Flanke  (Kamera -Z)', V_RIGHT)
    put_view(3, 1, 'HINTEN', 'von hinten  (Kamera -X)',    V_BACK)
    put_view(1, 0, 'DRAUFSICHT', 'von oben  (Kamera -Y)',  V_TOP, ground=False)

    # -- Skelett, in derselben Projektion wie LINKS
    x0, y0 = cell_frame(0, 0)
    pr = project(V_LEFT[0], V_LEFT[1])
    BP = [tuple(float(v) for v in pr(BT[b])) for b in range(mo.skel.bone_count)]
    for b in range(mo.skel.bone_count):
        p = mo.skel.parent[b]
        if 0 <= p < b:
            dr.line([(x0+BP[p][0], y0+BP[p][1]), (x0+BP[b][0], y0+BP[b][1])],
                    fill=(96, 104, 118), width=2)
    for b in range(mo.skel.bone_count):
        px, py = x0+BP[b][0], y0+BP[b][1]
        dr.ellipse([px-5, py-5, px+5, py+5],
                   fill=ACC if mo.skel.parent[b] < 0 else (206, 212, 224), outline=(20, 21, 25))
        dr.text((px+8, py-7), str(b), font=F_TINY, fill=(160, 168, 182))
    gy = int(round(float(pr((0.0, 0.0, 0.0))[1])))
    if 0 < gy < TILE:
        dr.line([(x0+6, y0+gy), (x0+TILE-7, y0+gy)], fill=(120, 92, 60), width=1)
    cell_label(0, 0, 'SKELETT', '%d Knochen, Ansicht wie LINKS' % mo.skel.bone_count)

    # -- Textur
    x0, y0 = cell_frame(2, 0, grid=False)
    tex = Image.fromarray(mo.texturblatt()).resize((TILE-24, TILE-24), Image.NEAREST)
    sheet.paste(tex, (x0+12, y0+12))
    dr.rectangle([x0+12, y0+12, x0+TILE-13, y0+TILE-13], outline=(70, 74, 84))
    cell_label(2, 0, 'TEXTUR', 'TIM %dx%d, 8 bpp - links CLUT 0, rechts CLUT 1'
               % (mo.texw, mo.texh))

    # -- Kennzahlen + Massstabsbalken
    x0, y0 = cell_frame(3, 0, grid=False)
    tris  = sum(len(m.tf) for m in mo.meshes[:mo.skel.bone_count])
    quads = sum(len(m.qf) for m in mo.meshes[:mo.skel.bone_count])
    zeilen = [
        ('Abmessungen der Pose', None),
        ('Hoehe   (Y)', '%d Einheiten' % round(ext[1])),
        ('Breite  (Z)', '%d Einheiten' % round(ext[2])),
        ('Tiefe   (X)', '%d Einheiten' % round(ext[0])),
        ('', ''),
        ('Modell', None),
        ('Knochen',     '%d' % mo.skel.bone_count),
        ('Meshes',      '%d  (0-%d am Knochen, %d ungenutzt)'
                        % (len(mo.meshes), mo.skel.bone_count-1,
                           len(mo.meshes)-mo.skel.bone_count)),
        ('Dreiecke',    '%d  (%d Tri + %d Quads)' % (tris + quads*2, tris, quads)),
        ('Vertices',    '%d' % len(V)),
        ('Textur',      '%dx%d, 8 bpp, %d CLUTs' % (mo.texw, mo.texh, mo.pal.shape[0])),
        ('Keyframes',   '%d, je %d Byte' % (mo.skel.kf_count, mo.skel.kf_size)),
        ('Clips',       '%d (Bank 0)' % len(mo.clips)),
        ('Blobgroesse', format(mo.blob_len, ',d').replace(',', '.') + ' Byte'),
    ]
    ty = y0 + 24
    for k, v in zeilen:
        if v is None:
            dr.text((x0+20, ty), k, font=F_VIEW, fill=ACC); ty += 31
        elif k == '':
            ty += 13
        else:
            dr.text((x0+20, ty), k, font=F_LBL, fill=DIM)
            dr.text((x0+146, ty), v, font=F_MONO, fill=FG); ty += 24
    bar = int(round(1000 * SCALE / SS))
    by  = y0 + TILE - 56
    dr.line([(x0+20, by), (x0+20+bar, by)], fill=FG, width=2)
    for t in (x0+20, x0+20+bar):
        dr.line([(t, by-7), (t, by+7)], fill=FG, width=2)
    dr.text((x0+20, by+11), '1000 PSX-Einheiten', font=F_LBL, fill=FG)
    dr.text((x0+20, by+32), 'alle fuenf Ansichten im selben Massstab', font=F_SMALL, fill=DIM)
    cell_label(3, 0, 'KENNZAHLEN', 'aus dem Original gelesen')

    # -- Fusszeile
    fy = HEAD + PAD + ROWS*(TILE+LBL_H) + 10
    dr.line([(PAD, fy), (SHEET_W-PAD, fy)], fill=(60, 63, 72), width=1)
    fuss = [
      'Achsen   X = vorn (+) / hinten, Y = oben (-) / unten (+), Z = seitlich - aus der EMR-Symmetrie belegt: Knochen 4/8 (Schultern) bei Z = -526 / +526, Knochen 12/15 (Hueften) bei Z = -160 / +160.',
      'Seitigkeit   oben x vorn = (0,-1,0) x (1,0,0) = (0,0,1), also ist +Z die LINKE Koerperseite: Knochen 8 = linke Schulter, Knochen 4 = rechte. Dritte Winkelprojektion, keine Ansicht ist gespiegelt.',
      'Massstab im Spiel   Das Original rendert diesen Gegner uniform 1,7x vergroessert: entity+0x166 = 0x1b33 (Q12) mit Flag-Bit 0x800, gesetzt im INIT FUN_80116f50, angewandt per ScaleMatrix @0x80065ff0',
      '                    im Render-Root FUN_8001e8c8 @0x8001e904-40. Die Masse oben sind daher die MODELL-Masse - auf der Buehne mal 1,7 (Hoehe %d -> %d).' % (round(ext[1]), round(ext[1]*1.7)),
      'Pipeline   EMS-Index re15_ems.c - Containerverzeichnis dir[2]=EMR, dir[7]=MD1, dir[8]=TIM - Posenkette re15_skel_compute_pose (skeleton_common.c:659-710) - RotMatrix @0x80068130.',
      'Nicht byte-true ist allein die Darstellung: Orthogonalprojektion, Z-Buffer und Lambert-Beleuchtung dienen der Lesbarkeit. Geometrie, Skelett, Pose und Textur stammen unveraendert aus dem Original.',
    ]
    ty = fy + 12
    for i, ln in enumerate(fuss):
        dr.text((PAD, ty), ln, font=F_SMALL, fill=(176, 179, 186) if i == 0 else DIM)
        ty += 21

    os.makedirs(os.path.dirname(os.path.abspath(ziel)), exist_ok=True)
    sheet.save(ziel)
    print('geschrieben: %s  (%dx%d)' % (ziel, *sheet.size))
    print('Typ 0x%02x, Blob %d, Clip 0x%02x, Keyframe %d' % (mo.typ, mo.blob_index, clip, kf0))
    print('Modellmasse  Hoehe %d  Breite %d  Tiefe %d  (PSX-Einheiten)'
          % (ext[1], ext[2], ext[0]))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--typ',  default='0x27', help='Gegnertyp aus s_ems_order, z.B. 0x27')
    ap.add_argument('--clip', default='0x16', help='Clip der Bank 0; dessen erster Keyframe posiert')
    ap.add_argument('--out',  default=None,   help='Zieldatei (.png)')
    ap.add_argument('--einzel', action='store_true', help='zusaetzlich jede Ansicht einzeln')
    a = ap.parse_args()
    typ, clip = int(a.typ, 0), int(a.clip, 0)
    mo = Modell(typ)
    if not 0 <= clip < len(mo.clips):
        raise SystemExit('Clip 0x%02x liegt ausserhalb der %d Clips der Bank 0'
                         % (clip, len(mo.clips)))
    ziel = a.out or os.path.join(REPO, 'analysis', 'gorilla_11c0', 'em%03x_ansichten.png' % typ)
    blatt(mo, clip, ziel, a.einzel)


if __name__ == '__main__':
    main()
