#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Export one RDT prop as raw .md1 + .tim + .obj/.mtl + texture .png + render .png.

usage: export_obj.py <rdt path> <md1_off hex> <tim_off hex> <outdir> <basename>

Coordinates: PSX model space is kept 1:1 in the .md1; the .obj flips Y
(PSX Y grows downward, OBJ/glTF Y grows upward) — the project's documented
PSX->glTF rule `Y' = -Y` (CLAUDE.md "Technical Notes").
"""
import os, sys, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1tex, timlib, md1lib
from PIL import Image


def texel_uv(t, page, clut, u, v):
    """-> (col,row) in the TIM's own pixel grid, or None."""
    mode = (page >> 7) & 0x03
    tpx = (page & 0x0F) * 64
    tpy = ((page >> 4) & 0x01) * 256
    if mode == 0:
        hx = tpx + (u >> 2); sub = u & 3
    elif mode == 1:
        hx = tpx + (u >> 1); sub = u & 1
    else:
        hx = tpx + u; sub = 0
    col = hx - t['ix']
    row = tpy + v - t['iy']
    if t['bpp'] == 4:
        px = col * 4 + sub
    elif t['bpp'] == 8:
        px = col * 2 + sub
    else:
        px = col
    return px, row


def main():
    rdt, moff, toff, outdir, base = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4], sys.argv[5]
    os.makedirs(outdir, exist_ok=True)
    d = open(rdt, 'rb').read()
    # blob end = next boundary among the header offsets / this file's end
    offs = list(struct.unpack_from('<22I', d, 8))
    extra = []
    nOmodel = d[2]
    tbl = offs[(0x30 - 8) // 4]
    if nOmodel and tbl and tbl + 8 * nOmodel <= len(d):
        for i in range(nOmodel):
            t_, m_ = struct.unpack_from('<II', d, tbl + 8 * i)
            extra += [t_, m_]
    bs = sorted(set([o for o in offs + extra if 0 < o <= len(d)] + [toff, len(d)]))
    end = len(d)
    for b in bs:
        if b > moff:
            end = b
            break
    blob = d[moff:end]
    open(os.path.join(outdir, base + '.md1'), 'wb').write(blob)
    tim = timlib.decode(d, toff) if toff else None
    if tim:
        tend = tim['end']
        open(os.path.join(outdir, base + '.tim'), 'wb').write(d[toff:tend])
        img = timlib.to_image(tim, 0)
        img.save(os.path.join(outdir, base + '_tex.png'))
    faces = md1tex.parse_uv(blob)
    md1 = md1lib.parse(blob)
    polys = md1lib.faces_world(md1)
    # renders
    import math
    for ang, nm in ((0, 'front'), (35, 'q35'), (90, 'side'), (20, 'top')):
        pit = -18 if nm == 'q35' else (-70 if nm == 'top' else 0)
        md1tex.render_tex(faces, tim, 512, 512, 'front', yaw=math.radians(ang),
                          pitch=math.radians(pit)).save(
            os.path.join(outdir, '%s_%s.png' % (base, nm)))
    # OBJ
    vs = []
    vts = []
    fl = []
    for (verts, uvs, clut, page) in faces:
        idx = []
        for k, vv in enumerate(verts):
            vs.append((vv[0], -vv[1], vv[2]))
            if tim:
                px, row = texel_uv(tim, page, clut, uvs[k][0], uvs[k][1])
                vts.append((px / float(max(1, tim['w'])), 1.0 - row / float(max(1, tim['h']))))
            else:
                vts.append((0.0, 0.0))
            idx.append(len(vs))
        fl.append(idx)
    with open(os.path.join(outdir, base + '.obj'), 'w') as f:
        f.write('# %s — extracted from %s  md1@0x%X tim@0x%X\n'
                % (base, os.path.basename(rdt), moff, toff))
        f.write('# PSX model space with Y flipped (Y\' = -Y), 1 unit = 1 PSX unit.\n')
        f.write('mtllib %s.mtl\nusemtl %s\n' % (base, base))
        for v in vs:
            f.write('v %d %d %d\n' % v)
        for t in vts:
            f.write('vt %.6f %.6f\n' % t)
        for face in fl:
            f.write('f ' + ' '.join('%d/%d' % (i, i) for i in face) + '\n')
    with open(os.path.join(outdir, base + '.mtl'), 'w') as f:
        f.write('newmtl %s\nKd 1 1 1\nmap_Kd %s_tex.png\n' % (base, base))
    print('%s: %d faces, %d verts, md1 %d B, tim %s'
          % (base, len(faces), len(vs), len(blob),
             ('%dx%d %dbpp' % (tim['w'], tim['h'], tim['bpp'])) if tim else 'none'))


main()
