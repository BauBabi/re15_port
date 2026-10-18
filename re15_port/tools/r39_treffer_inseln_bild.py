#!/usr/bin/env python3
"""Runde 39 Spur D: Uebersichtsblatt der INSEL-Zellen eines Raums (Art 3, nicht an der Huelle des Bands) in den
Hintergruenden aller Cuts, in denen sie sichtbar sind. Grundlage fuer die Moebel-Liste gen/treffer_r39_moebel.inc:
welche Art-3-Zelle ist ein Tisch/Pult/Stuhl (Schuss geht drueber, RE2-Flag-Praxis) und welche eine Wand/Saeule.

    python re15_port/tools/r39_treffer_inseln_bild.py ROOM1140 <ppm_dir> <ziel_dir>
    ppm_dir: Abzug von probe_bg_dump (ROOM<rrr><cc>.ppm)

Projektion: re15_camera_build_view (tools/maske/geom.py build_view), Bodenebene des Bands (band * -1800).
"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'maske'))
from geom import build_view  # noqa: E402
from PIL import Image, ImageDraw  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
TOL = 150

def rdt_laden(room):
    for st in range(1, 7):
        p = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE%d' % st, room + '.RDT')
        if os.path.exists(p):
            return open(p, 'rb').read()
    raise SystemExit('RDT fehlt: ' + room)

def zellen(rdt):
    sca = struct.unpack_from('<I', rdt, 0x20)[0]
    cnt = struct.unpack_from('<5I', rdt, sca + 4)
    return [struct.unpack_from('<HHhhBBBB', rdt, sca + 24 + i * 12) for i in range(cnt[0])]

def inseln(zl):
    out = set()
    for b in {c[7] >> 4 for c in zl}:
        bc = [(i, c) for i, c in enumerate(zl) if (c[7] >> 4) == b]
        x0 = min(c[2] for _, c in bc); z0 = min(c[3] for _, c in bc)
        x1 = max(c[2] + c[0] for _, c in bc); z1 = max(c[3] + c[1] for _, c in bc)
        for i, (w, d, x, z, t, u0, u1, fl) in bc:
            huelle = x <= x0 + TOL or z <= z0 + TOL or x + w >= x1 - TOL or z + d >= z1 - TOL
            if not huelle and (fl & 0xf) == 3:
                out.add(i)
    return out

def proj(R, t, H, wx, wy, wz):
    vx = (R[0] * wx + R[1] * wy + R[2] * wz >> 12) + t[0]
    vy = (R[3] * wx + R[4] * wy + R[5] * wz >> 12) + t[1]
    vz = (R[6] * wx + R[7] * wy + R[8] * wz >> 12) + t[2]
    if vz <= 16:
        return None
    return (160 + H * vx / vz, 120 + H * vy / vz)

def main():
    room, ppm_dir, ziel_dir = sys.argv[1], sys.argv[2], sys.argv[3]
    rdt = rdt_laden(room)
    cam_off = struct.unpack_from('<I', rdt, 0x24)[0]
    ncut = rdt[1]
    zl = zellen(rdt)
    ins = inseln(zl)
    rid = int(room[4:], 16) >> 4
    blaetter = []
    for cut in range(ncut):
        rec = cam_off + cut * 32
        fov = struct.unpack_from('<H', rdt, rec + 2)[0]
        P = struct.unpack_from('<iii', rdt, rec + 4)
        T = struct.unpack_from('<iii', rdt, rec + 0x10)
        v = build_view(fov, *P, *T)
        if v is None:
            continue
        R, t, H = v
        src = os.path.join(ppm_dir, 'ROOM%03X%02d.ppm' % (rid, cut))
        if not os.path.exists(src):
            continue
        im = Image.open(src).convert('RGB')
        dr = ImageDraw.Draw(im)
        sichtbar = []
        for i, (w, d, x, z, ty, u0, u1, fl) in enumerate(zl):
            y0 = -1800 * (fl >> 4)
            pts = [proj(R, t, H, ex, y0, ez) for (ex, ez) in ((x, z), (x + w, z), (x + w, z + d), (x, z + d))]
            if any(p is None for p in pts):
                continue
            innen = any(0 <= p[0] < 320 and 0 <= p[1] < 240 for p in pts)
            if not innen:
                continue
            ist_insel = i in ins
            farbe = (255, 230, 0) if ist_insel else (90, 90, 90)
            if (ty & 0xf) != 1 and ist_insel:
                farbe = (0, 220, 255)
            dr.polygon(pts, outline=farbe)
            if ist_insel:
                cx = sum(p[0] for p in pts) / 4; cy = sum(p[1] for p in pts) / 4
                dr.rectangle((cx - 7, cy - 5, cx + 8, cy + 5), fill=(0, 0, 0))
                dr.text((cx - 6, cy - 5), str(i), fill=(255, 80, 80))
                sichtbar.append(i)
        if sichtbar:
            dr.text((2, 2), 'C%d' % cut, fill=(255, 255, 255))
            blaetter.append((cut, im, sichtbar))
    if not blaetter:
        print(room, 'keine sichtbaren Inseln'); return
    os.makedirs(ziel_dir, exist_ok=True)
    cols = 2
    for k in range(0, len(blaetter), 6):
        teil = blaetter[k:k + 6]
        rows = (len(teil) + cols - 1) // cols
        blatt = Image.new('RGB', (320 * cols, 240 * rows), (0, 0, 0))
        for j, (cut, im, sv) in enumerate(teil):
            blatt.paste(im, ((j % cols) * 320, (j // cols) * 240))
        dst = os.path.join(ziel_dir, '%s_inseln_%d.png' % (room, k // 6))
        blatt.save(dst)
        print('->', dst, ' '.join('C%d:%s' % (cut, ','.join(map(str, sv))) for cut, _, sv in teil))

if __name__ == '__main__':
    main()
