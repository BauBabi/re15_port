#!/usr/bin/env python3
"""Spur E (Runde 37): RE2-Schlosslampen (ESP 0x16, sce_espr_on2) im RE2-Hintergrund nachzeichnen.

Muster: tools/r34n_c/re2_2130_projektion.py (Kamera camera.rid, LookAt wie camera_common.c, H = fov>>7,
Sprite-Kante = S*scale16*H/(SZ*256) aus FUN_80077ed0 @0x80077f14..0x8007800c, ABR 1 = B+F).
Hier fuer die zwei RE2-Schloesser mit Lampen-Zustand:
  ROOM2110 (Kartenleser "Weapon Storage"):  sub03 @0x0183A/@0x0184A (rot 0x00), @0x01866/@0x01876 (gruen 0x10),
           @0x0188A/@0x0189A (blau 0x08); sub08 @0x01B6C/@0x01B7C (blau 0x08) nach dem Piep @0x01B5C, Cut 12 @0x01B2C.
  ROOM4070 ("Clearance identification device"): sub03 @0x0088C (blau 0x0c, Platz 3) / @0x008C6 (rot 0x04, Platz 2);
           sub08 @0x00DC2 (blau an) / @0x00DD2 (rot aus) / @0x00DD4 Piep, Cut 6 @0x00D68.
Unterindex: Strom = Unterindex & 7 (@0x8001c8d8), CLUT-Zeile = Unterindex >> 3 (@0x8001c9e0..0x8001c9fc).
Strom 0 -> Anim-Satz 0 (Zellen 0,1,2 je Bild), Strom 4 -> Anim-Satz 7 (Zellen 5,7,6,7) — effect.esp ESP-0x16-Block
(ROOM2110 effect.esp @0x0BFC = ROOM2130 effect.esp @0x08, 492 B gleich).
Aufruf: re2_lampen_projektion.py <ausgabeverzeichnis>
"""
import os, sys, struct, math
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RE2 = os.path.join(REPO, "info", "re2leon")
out = sys.argv[1] if len(sys.argv) > 1 else "."
os.makedirs(out, exist_ok=True)

def kamera(room, cut):
    r = open(os.path.join(RE2, "PL0", "RDT", "room%s" % room, "camera.rid"), "rb").read()
    flag, fov, px_, py_, pz_, tx, ty, tz, pri = struct.unpack_from("<HHiiiiiiI", r, cut * 32)
    H = fov >> 7
    dx, dy, dz = tx - px_, ty - py_, tz - pz_
    dist = math.sqrt(dx*dx + dy*dy + dz*dz); horiz = math.sqrt(dx*dx + dz*dz)
    sp, cp = -dy / dist, horiz / dist
    sy, cy = dx / horiz, dz / horiz
    R = [cy, 0, -sy, sp*sy, cp, sp*cy, cp*sy, -sp, cp*cy]
    def proj(x, y, z):
        x -= px_; y -= py_; z -= pz_
        vx = R[0]*x + R[1]*y + R[2]*z; vy = R[3]*x + R[4]*y + R[5]*z; vz = R[6]*x + R[7]*y + R[8]*z
        return 160 + vx * H / vz, 120 + vy * H / vz, vz
    return proj, H

tim = open(os.path.join(RE2, "PL0", "RDT", "room2110", "esp16.tim"), "rb").read()
bl, cx_, cy_, cw, ch = struct.unpack_from("<IHHHH", tim, 8)
cluts = [[struct.unpack_from("<H", tim, 8 + 12 + (p * cw + k) * 2)[0] for k in range(cw)] for p in range(ch)]
o = 8 + bl
bl2, pxx, pyy, pw, ph = struct.unpack_from("<IHHHH", tim, o)
pix = tim[o + 12:o + bl2]
def texel(u, v): return (pix[v * pw * 2 + u // 2] >> (4 * (u & 1))) & 15
ZELLE = {0: (0, -16, 32), 1: (32, -16, 32), 2: (64, -16, 32), 5: (160, -12, 24), 6: (184, -16, 32), 7: (216, -16, 32)}

def zeichne(im, proj, H, welt, scale16, zelle, zeile):
    u0, off, S = ZELLE[zelle]
    sx, sy, sz = proj(*welt)
    w = S * scale16 * H / (sz * 256.0)
    x0 = sx + off * w / S; y0 = sy + off * w / S
    p = im.load()
    for yy in range(int(math.floor(y0)), int(math.ceil(y0 + w))):
        for xx in range(int(math.floor(x0)), int(math.ceil(x0 + w))):
            if not (0 <= xx < 320 and 0 <= yy < 240): continue
            u = int((xx + 0.5 - x0) * S / w); v = int((yy + 0.5 - y0) * S / w)
            if not (0 <= u < S and 0 <= v < S): continue
            t = cluts[zeile][texel(u0 + u, v)]
            if t == 0: continue
            r5, g5, b5 = t & 31, (t >> 5) & 31, (t >> 10) & 31
            R0, G0, B0 = p[xx, yy]
            p[xx, yy] = (min(255, R0 + (r5 << 3)), min(255, G0 + (g5 << 3)), min(255, B0 + (b5 << 3)))
    return sx, sy, sz, w

def s16(v): return v - 65536 if v >= 32768 else v
faelle = [
  ("2110", 12, "rot",   [((s16(0xa22c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 0), ((s16(0xa31c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 0)]),
  ("2110", 12, "gruen", [((s16(0xa22c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 2), ((s16(0xa31c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 2)]),
  ("2110", 12, "blau",  [((s16(0xa22c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 1), ((s16(0xa31c), s16(0xf588), s16(0xce96)), 0x0bb4, 0, 1)]),
  ("4070", 6, "rot",    [((s16(0xab6a), s16(0xf844), s16(0xb8a2)), 0x0900, 5, 0)]),
  ("4070", 6, "blau",   [((s16(0xac54), s16(0xf844), s16(0xb8a2)), 0x0900, 5, 1)]),
]
for room, cut, name, lampen in faelle:
    proj, H = kamera(room, cut)
    bg = os.path.join(RE2, "COMMON", "BSS", "ROOM%s" % room[:3], "ROOM%s%02d.bmp" % (room[:3], cut))
    im = Image.open(bg).convert("RGB")
    for (welt, sc, zelle, zeile) in lampen:
        sx, sy, sz, w = zeichne(im, proj, H, welt, sc, zelle, zeile)
        print("ROOM%s Cut %d %-5s welt=%s scale16=0x%04X Zelle %d Zeile %d -> schirm (%.1f, %.1f) SZ %.0f Kante %.1f px"
              % (room, cut, name, welt, sc, zelle, zeile, sx, sy, sz, w))
    im.resize((640, 480), Image.NEAREST).save(os.path.join(out, "re2_%s_c%d_%s.png" % (room, cut, name)))
