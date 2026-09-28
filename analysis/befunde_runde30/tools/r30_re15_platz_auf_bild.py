#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: ist der Text-Platz eine TUER?  Statt es aus dem Wortlaut zu
vermuten, wird das AOT-Rechteck in jede Kamera des Raums projiziert und auf den
vorgerenderten Hintergrund gezeichnet (extracted/PSX/STAGE<n>/ROOM<xyz>/ROOM<xyz>NN.bmp).

Kameramodell 1:1 wie tools/re2_sicherung/re2_aot_on_bg.py (dort gegen
re15_port/engine/src/camera_common.c bzw. FUN_80053ca4 @0x80053ca4 belegt):
  RID-Satz 32 B: +0 flag u16, +2 fov u16, +4 pos xyz s32, +0x10 ziel xyz s32, +0x1C pri u32
  H = fov >> 7;  Bildpunkt sx = 160 + H*x/z, sy = 120 + H*y/z
RDT-Kopf: +0x24 Kamera-Tabelle, Byte 1 = Anzahl Cuts.

Aufruf: r30_re15_platz_auf_bild.py <raum-hex> <datei-offset-des-AOT-Satzes> [...]
Ausgabe: build/r30_tuer-verschlossen/plaetze/ROOM<raum>_platz<slot>.png (nur Cuts, in denen
das Rechteck im Bild liegt; Rechteck am Boden y=0 und als Saeule bis y=-1800 gezeichnet).
"""
import os, sys, struct, math, glob
from PIL import Image, ImageDraw
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))

def isqrt(x): return int(math.isqrt(max(0, int(x))))
def build_view(cut):
    flag, fov = cut[0], cut[1]
    px, py, pz, tx, ty, tz = cut[2:8]
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx*dx + dy*dy + dz*dz)
    if dist == 0: return None
    horiz = isqrt(dx*dx + dz*dz)
    sp = int((-dy) * 4096 / dist); cp = int(horiz * 4096 / dist)
    if horiz:
        sy = int(dx * 4096 / horiz); cy = int(dz * 4096 / horiz)
        r = [cy, 0, -sy, (sp*sy) >> 12, cp, (sp*cy) >> 12, (cp*sy) >> 12, -sp, (cp*cy) >> 12]
    else:
        r = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(r[0]*-px + r[1]*-py + r[2]*-pz) >> 12, (r[3]*-px + r[4]*-py + r[5]*-pz) >> 12,
         (r[6]*-px + r[7]*-py + r[8]*-pz) >> 12]
    return r, t, fov >> 7
def project(view, x, y, z):
    r, t, H = view
    vx = ((r[0]*x + r[1]*y + r[2]*z) >> 12) + t[0]
    vy = ((r[3]*x + r[4]*y + r[5]*z) >> 12) + t[1]
    vz = ((r[6]*x + r[7]*y + r[8]*z) >> 12) + t[2]
    if vz <= 1: return None
    return (160 + H * vx // vz, 120 + H * vy // vz, vz)

def main():
    room = int(sys.argv[1], 16)
    offs = [int(a, 16) for a in sys.argv[2:]]
    stage = room >> 12
    base = room >> 4
    rdt = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE%d' % stage, 'ROOM%04X.RDT' % room)
    d = open(rdt, 'rb').read()
    ncut = d[1]
    cam = struct.unpack_from('<I', d, 0x24)[0]
    bgdir = os.path.join(REPO, 'extracted', 'PSX', 'STAGE%d' % stage, 'ROOM%03X' % base)
    out = os.path.join(REPO, 'build', 'r30_tuer-verschlossen', 'plaetze')
    os.makedirs(out, exist_ok=True)
    for off in offs:
        op = d[off]
        slot = d[off + 1]
        if op in (0x2C, 0x3B):
            if d[off + 3] & 0x80:
                pts = struct.unpack_from('<8h', d, off + 6)
                quad = [(pts[0], pts[1]), (pts[2], pts[3]), (pts[4], pts[5]), (pts[6], pts[7])]
            else:
                x, z, w, dd = struct.unpack_from('<4h', d, off + 6)
                quad = [(x, z), (x + w, z), (x + w, z + dd), (x, z + dd)]
        else:
            print('ROOM%04X @0x%05X: Opcode 0x%02X traegt kein Rechteck (Aot_reset) - uebersprungen' % (room, off, op))
            continue
        tiles = []
        for i in range(ncut):
            cut = struct.unpack_from('<HHiiiiiiI', d, cam + i * 32)
            v = build_view(cut)
            if v is None: continue
            lo = [project(v, x, 0, z) for x, z in quad]
            hi = [project(v, x, -1800, z) for x, z in quad]
            if any(p is None for p in lo + hi): continue
            xs = [p[0] for p in lo + hi]; ys = [p[1] for p in lo + hi]
            if max(xs) < 0 or min(xs) > 320 or max(ys) < 0 or min(ys) > 240: continue
            bmp = os.path.join(bgdir, 'ROOM%03X%02d.bmp' % (base, i))
            if not os.path.isfile(bmp): continue
            im = Image.open(bmp).convert('RGB').resize((640, 480), Image.NEAREST)
            dr = ImageDraw.Draw(im)
            dr.polygon([(p[0]*2, p[1]*2) for p in lo], outline=(255, 220, 60))
            dr.polygon([(p[0]*2, p[1]*2) for p in hi], outline=(255, 120, 60))
            for a, b in zip(lo, hi):
                dr.line([(a[0]*2, a[1]*2), (b[0]*2, b[1]*2)], fill=(255, 170, 60))
            dr.text((6, 6), 'ROOM%04X cut %d  Platz %d @0x%05X' % (room, i, slot, off), fill=(255, 255, 0))
            tiles.append(im)
        if not tiles:
            print('ROOM%04X Platz %d @0x%05X: in keinem Cut im Bild' % (room, slot, off)); continue
        tiles = tiles[:4]
        sheet = Image.new('RGB', (640 * len(tiles), 480))
        for k, t in enumerate(tiles): sheet.paste(t, (640 * k, 0))
        p = os.path.join(out, 'ROOM%04X_platz%d_%05X.png' % (room, slot, off))
        sheet.save(p)
        print('ROOM%04X Platz %d @0x%05X: %d Cuts -> %s' % (room, slot, off, len(tiles), p))

if __name__ == '__main__':
    main()
