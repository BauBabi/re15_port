#!/usr/bin/env python3
"""Runde 39 Spur D: Kollisionszellen eines Raums als Bodenrechtecke in die Hintergrundbilder zeichnen.

Zweck: WELCHE Art-3-Zelle (RE1.5-Schuss-Sichtlinie FUN_8003dcc4(0xf00,0x300)) ist ein Tisch/Moebel und welche
eine Wand — das steht in keiner Zahl, nur im Bild. Projektion = re15_camera_build_view (tools/maske/geom.py).

    python re15_port/tools/r39_treffer_zellen_bild.py ROOM1140 <bild_dir> <ziel_dir> [cut ...]
    bild_dir: PNGs ROOM1140_C<n>.png (z.B. analysis/befunde_2026-09-19/pri-masken-phase2)
"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'maske'))
from geom import build_view  # noqa: E402
from PIL import Image, ImageDraw  # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def rdt_laden(room):
    for st in range(1, 7):
        p = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE%d' % st, room + '.RDT')
        if os.path.exists(p):
            return open(p, 'rb').read()
    raise SystemExit('RDT fehlt: ' + room)

def zellen(rdt):
    sca = struct.unpack_from('<I', rdt, 0x20)[0]
    cnt = struct.unpack_from('<5I', rdt, sca + 4)
    out = []
    for i in range(sum(cnt)):
        w, d, x, z, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', rdt, sca + 24 + i * 12)
        out.append((i, w, d, x, z, t, u0, u1, fl))
    return out, cnt

def projizieren(R, t, H, wx, wy, wz):
    vx = (R[0] * wx + R[1] * wy + R[2] * wz >> 12) + t[0]
    vy = (R[3] * wx + R[4] * wy + R[5] * wz >> 12) + t[1]
    vz = (R[6] * wx + R[7] * wy + R[8] * wz >> 12) + t[2]
    if vz <= 16:
        return None
    return (160 + H * vx / vz, 120 + H * vy / vz, vz)

def main():
    room, bild_dir, ziel_dir = sys.argv[1], sys.argv[2], sys.argv[3]
    cuts = [int(c) for c in sys.argv[4:]]
    rdt = rdt_laden(room)
    cam_off = struct.unpack_from('<I', rdt, 0x24)[0]
    zl, cnt = zellen(rdt)
    n0 = cnt[0]                      # die Partitionen wiederholen dieselben Zellen; nur die erste zeichnen
    os.makedirs(ziel_dir, exist_ok=True)
    for cut in cuts:
        rec = cam_off + cut * 32
        fov = struct.unpack_from('<H', rdt, rec + 2)[0]
        P = struct.unpack_from('<iii', rdt, rec + 4)
        T = struct.unpack_from('<iii', rdt, rec + 0x10)
        v = build_view(fov, *P, *T)
        if v is None:
            print('cut', cut, 'ohne Sicht'); continue
        R, t, H = v
        src = os.path.join(bild_dir, '%s_C%d.png' % (room, cut))
        if not os.path.exists(src):
            print('kein Bild', src); continue
        im = Image.open(src).convert('RGB')
        sx = im.width / 320.0; sy = im.height / 240.0
        dr = ImageDraw.Draw(im)
        y0 = 0  # Bodenebene band 0
        for (i, w, d, x, z, ty, u0, u1, fl) in zl[:n0]:
            if (fl >> 4) != 0:
                continue
            ecken = [(x, z), (x + w, z), (x + w, z + d), (x, z + d)]
            pts = []
            for (ex, ez) in ecken:
                p = projizieren(R, t, H, ex, y0, ez)
                pts.append(None if p is None else (p[0] * sx, p[1] * sy))
            if any(p is None for p in pts):
                continue
            farbe = (255, 220, 0) if (ty & 0xf) == 1 else (0, 200, 255)
            dr.polygon(pts, outline=farbe)
            cx = sum(p[0] for p in pts) / 4; cy = sum(p[1] for p in pts) / 4
            dr.text((cx - 4, cy - 5), str(i), fill=(255, 0, 0))
        dst = os.path.join(ziel_dir, '%s_C%d_zellen.png' % (room, cut))
        im.save(dst); print('->', dst, 'Kamera', P, 'Ziel', T)

if __name__ == '__main__':
    main()
