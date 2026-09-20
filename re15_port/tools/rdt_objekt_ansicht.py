#!/usr/bin/env python3
"""Ansichtsblatt der OBJEKT-Modelle eines Raums (RDT-Props): 4 Seiten + Draufsicht.

Warum: Obj_model_set nennt nur Nummer, Position und Drehung — WAS ein Objekt ist,
steht nirgends. Dieses Werkzeug zeichnet jedes Prop-MD1 aus der RDT einzeln, damit
man es ANSEHEN kann, statt es zu raten.

    python re15_port/tools/rdt_objekt_ansicht.py STAGE1/ROOM1150.RDT --ziel blatt.png

Format- und Leser-Herkunft (kein Raten):
    Prop-Zeigertabelle @RDT+0x30, 2*nOmodel u32 LE, abwechselnd TIM,MD1
                                   re15_port/engine/src/rdt_common.c:52-79 (parse_props)
    MD1-Mesh                       re15_port/include/re15/re15_md1.h
    Prop-Zeichner (alle Meshes im
    Prop-Raum, keine Knochen)      re15_port/platform/pc/main.c:9161
    TIM 8bpp + CLUT                re15_port/tools/emd_ansichtsblatt.py (tim_decode)

NICHT byte-true und bewusst so: Orthogonalprojektion, Z-Puffer und die
Lambert-Beleuchtung dienen allein der Lesbarkeit. Geometrie und Textur stammen
unveraendert aus der RDT.
"""
import argparse, math, os, struct, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from emd_ansichtsblatt import md1_parse, tim_decode, font  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PSX  = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')

u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def naechste_grenze(d, start, tbl, entries):
    """rdt_common.c rdt_next_boundary: bis zum naechsten Tabellen-Zeiger dahinter."""
    best = len(d)
    for o in range(0x08, 0x60, 4):
        v = u32(d, o)
        if start < v < best <= len(d):
            best = v
    for i in range(entries):
        v = u32(d, tbl + i*4)
        if start < v < best <= len(d):
            best = v
    return best


def props_lesen(d):
    """parse_props (rdt_common.c:52). Rueckgabe: Liste (tim_bytes, md1_bytes)."""
    n   = d[0x02]                       # nOmodel
    tbl = u32(d, 0x30)
    out = []
    for k in range(n):
        to = u32(d, tbl + (2*k)*4)
        mo = u32(d, tbl + (2*k+1)*4)
        t = d[to:naechste_grenze(d, to, tbl, 2*n)] if to else b''
        m = d[mo:naechste_grenze(d, mo, tbl, 2*n)] if mo else b''
        out.append((t, m))
    return out


class PropModell:
    """Ein RDT-Prop: alle MD1-Meshes im Prop-Raum + eigene TIM."""

    def __init__(self, tim, md1):
        self.meshes = md1_parse(md1)
        self.tex, self.pal, self.clut_y = tim_decode(tim)
        self.texh, self.texw = self.tex.shape
        self.md1_len, self.tim_len = len(md1), len(tim)

    def clut_row(self, clut_id):
        return max(0, min(self.pal.shape[0]-1, (clut_id >> 6) - self.clut_y))

    def page_ubase(self, page):
        return (page & 0xF) * 128

    def geometrie(self):
        V, N, F = [], [], []
        for b, m in enumerate(self.meshes):
            def xf(vs):
                return (np.array([[v[0], v[1], v[2]] for v in vs], float)
                        if vs else np.zeros((0, 3)))

            def xn(ns):
                if not ns:
                    return np.zeros((0, 3))
                w  = np.array([[v[0], v[1], v[2]] for v in ns], float)
                ln = np.linalg.norm(w, axis=1, keepdims=True); ln[ln == 0] = 1
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


def render(mo, V, N, F, right, down, depth, W, H, scale, cx, cy):
    """wie emd_ansichtsblatt.render, nur ohne Knochen."""
    right, down, depth = (np.asarray(v, float) for v in (right, down, depth))
    licht  = right*0.42 - down*0.52 + depth*0.75
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


SS, TILE, PAD, LBL_H = 3, 430, 24, 30
BG, CELL_BG, GRID    = (18, 19, 22), (28, 30, 35), (48, 52, 60)
FG, DIM, ACC         = (232, 233, 236), (150, 153, 160), (214, 138, 74)

# PSX-Welt: +X rechts, +Y UNTEN, +Z nach hinten.
ANSICHTEN = [
    ('Draufsicht (-Y)',  (1, 0, 0), (0, 0,  1), (0, -1, 0)),
    ('Vorn (+Z Blick)',  (1, 0, 0), (0, 1,  0), (0,  0, 1)),
    ('Links (-X Blick)', (0, 0, 1), (0, 1,  0), (-1, 0, 0)),
    ('Hinten (-Z)',      (-1, 0, 0), (0, 1, 0), (0,  0, -1)),
]


def blatt(rdt_name, modelle, ziel):
    F_T, F_S, F_V, F_M = font(38, True), font(18), font(20, True), font(15)
    cols = len(ANSICHTEN) + 1                       # + Texturkachel
    rows = len(modelle)
    W = PAD + cols*(TILE+PAD)
    H = 118 + rows*(TILE+LBL_H+PAD)
    img = Image.new('RGB', (W, H), BG)
    dr  = ImageDraw.Draw(img)
    dr.text((PAD, 26), 'Objektmodelle  %s' % rdt_name, FG, F_T)
    dr.text((PAD, 78), 'Orthogonalprojektion, gemeinsamer Massstab je Zeile. '
                       'Geometrie + Textur unveraendert aus der RDT.', DIM, F_S)

    for r, (nr, mo, hinweis) in enumerate(modelle):
        y0 = 118 + r*(TILE+LBL_H+PAD)
        V, N, F = mo.geometrie()
        if len(V) == 0:
            dr.text((PAD, y0+10), 'Objekt %d: leer' % nr, ACC, F_V)
            continue
        lo, hi = V.min(0), V.max(0)
        ctr, ext = (lo+hi)/2.0, hi-lo
        scale = (TILE*SS*0.84) / max(ext.max(), 1.0)
        for c, (name, right, down, depth) in enumerate(ANSICHTEN):
            x0 = PAD + c*(TILE+PAD)
            cx = TILE*SS/2 - float(np.dot(ctr, np.asarray(right, float)))*scale
            cy = TILE*SS/2 - float(np.dot(ctr, np.asarray(down,  float)))*scale
            col, alp = render(mo, V, N, F, right, down, depth,
                              TILE*SS, TILE*SS, scale, cx, cy)
            cell = np.zeros((TILE*SS, TILE*SS, 3), np.float32)
            cell[:] = np.array(CELL_BG, np.float32)/255.0
            cell = cell*(1-alp[..., None]) + col*alp[..., None]
            tile = Image.fromarray((np.clip(cell, 0, 1)*255).astype(np.uint8))
            tile = tile.resize((TILE, TILE), Image.LANCZOS)
            img.paste(tile, (x0, y0+LBL_H))
            dr.rectangle([x0, y0+LBL_H, x0+TILE-1, y0+LBL_H+TILE-1], outline=GRID)
            dr.text((x0+4, y0+6), name, FG, F_V)
        # Texturkachel
        x0 = PAD + len(ANSICHTEN)*(TILE+PAD)
        half = mo.texw // 2
        if mo.pal.shape[0] >= 2:
            tb = np.concatenate([mo.pal[0][mo.tex[:, :half]],
                                 mo.pal[1][mo.tex[:, half:]]], axis=1)
        else:
            tb = mo.pal[0][mo.tex]
        t = Image.fromarray(tb.astype(np.uint8)).resize((TILE, TILE), Image.NEAREST)
        img.paste(t, (x0, y0+LBL_H))
        dr.rectangle([x0, y0+LBL_H, x0+TILE-1, y0+LBL_H+TILE-1], outline=GRID)
        dr.text((x0+4, y0+6), 'Textur %dx%d' % (mo.texw, mo.texh), FG, F_V)
        info = ('Objekt %d  |  MD1 %d B, %d Mesh(es), %d Dreiecke  |  Ausdehnung '
                'X %d  Y %d  Z %d  |  %s'
                % (nr, mo.md1_len, len(mo.meshes), len(F),
                   int(ext[0]), int(ext[1]), int(ext[2]), hinweis))
        dr.text((PAD, y0+LBL_H+TILE+4), info, ACC, F_M)
    img.save(ziel)
    print('geschrieben: %s  (%dx%d)' % (ziel, W, H))


class Verbund:
    """Mehrere Objekte in EINEM Bezugsrahmen — fuer die Anhaenge-Form pc[5]&0xC0==0xC0
    (LAB_80040914 @0x80040a84-9c: Elternmatrix = Objektpool[pc[5]-0xC0]+0x48). Das Kind
    traegt dann local pos (0,0,0), seine Vertizes liegen bereits im Elternrahmen, also
    genuegt das Zusammenlegen der Geometrie."""

    def __init__(self, teile):
        self.teile = teile                    # [(nr, PropModell)]
        self.texw, self.texh = teile[0][1].texw, teile[0][1].texh
        self.tex, self.pal = teile[0][1].tex, teile[0][1].pal
        self.clut_y = teile[0][1].clut_y
        self.md1_len = sum(m.md1_len for _, m in teile)
        self.meshes  = [x for _, m in teile for x in m.meshes]

    clut_row   = PropModell.clut_row
    page_ubase = PropModell.page_ubase

    def geometrie(self):
        V = np.zeros((0, 3)); N = np.zeros((0, 3)); F = []
        for _, m in self.teile:
            v, n, f = m.geometrie()
            b = len(V)
            V = np.concatenate([V, v]); N = np.concatenate([N, n])
            for e in f:
                F.append(dict(e, v=tuple(i+b for i in e['v']),
                              n=tuple(i+b for i in e['n'])))
        return V, N, F


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('rdt', help='z.B. STAGE1/ROOM1150.RDT (relativ zu shared_assets/PSX)')
    ap.add_argument('--ziel', default='objekte.png')
    ap.add_argument('--hinweis', nargs='*', default=[],
                    help='je Objekt eine Beschriftung')
    ap.add_argument('--verbund', default='',
                    help='zusaetzliche Zeile: Objekte in EINEM Rahmen, z.B. 0,1,2')
    a = ap.parse_args()
    pfad = a.rdt if os.path.isabs(a.rdt) else os.path.join(PSX, a.rdt)
    d = open(pfad, 'rb').read()
    modelle = []
    for k, (tim, md1) in enumerate(props_lesen(d)):
        if not md1 or not tim:
            print('Objekt %d: TIM %d B, MD1 %d B — uebersprungen' % (k, len(tim), len(md1)))
            continue
        try:
            mo = PropModell(tim, md1)
        except Exception as e:
            print('Objekt %d: nicht lesbar (%s)' % (k, e)); continue
        hin = a.hinweis[k] if k < len(a.hinweis) else ''
        modelle.append((k, mo, hin))
    if a.verbund:
        nrs = [int(x) for x in a.verbund.split(',')]
        teile = [(nr, mo) for (nr, mo, _) in modelle if nr in nrs]
        modelle.append((-1, Verbund(teile),
                        'VERBUND ' + '+'.join(str(n) for n, _ in teile) +
                        ' (Anhaenge-Form pc[5]=0xC0, LAB_80040914 @0x80040a84)'))
    blatt(os.path.basename(pfad), modelle, a.ziel)


if __name__ == '__main__':
    main()
