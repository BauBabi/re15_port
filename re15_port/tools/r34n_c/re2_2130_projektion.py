#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): RE2 ROOM2130 — Lage und Groesse der Schalterlampen-Sprites (ESP 0x16) im
Panel-Cut berechnen und ueber den Original-Hintergrund ROOM21306.bmp legen.

Kamera:   camera.rid-Satz (32 B: flag,fov,pos xyz,target xyz,pri), LookAt wie camera_common.c
          (FUN_80053ca4-Nachbau, hier in Gleitkomma — reicht fuer +-1 px), H = fov>>7.
Groesse:  RE2-Sprite-Bauer FUN_80077ed0: step = S*scale16*camf/(SZ<<4) (@0x80077f14..0x80077fd8),
          w16 = step*defW (@0x80078004/0x8007800c, defW = Zeile+4 = 0x1000) -> w_px = S*scale16*camf/(SZ*256).
Lampen:   sce_espr_on2 @0x01294.. (Lage, scale16 0x02BA), Unterindex 2 -> Anim-Satz 4 (Zeile+2,
          Routine 1 @0x8001dc40/0x8001dc4c) -> Zelle 3 (u 96, dx/dy -16) bzw. Zelle 4 (u 128), S = 32.
Farbe:    Palette 2 (gruen) = CLUT-Basis (0x120,480) (FUN_8001bd38 @0x8001be68-70) + (0x10>>3)*64.
Mischung: ABE (Flags 0xBA03 & 0x1000, @0x8001dc3c-44 / @0x80077a44-50), ABR 1 = B+F (TPAGE |= 0x20,
          @0x8001dc48-60).
Ausgabe:  Tabelle + <out>_rot.png (Palette 0 wie in RE2) + <out>_gruen.png (Palette 2), 3-fach.
"""
import os, sys, struct, math
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
# Hauptbaum als Quelle fuer info/ (im Arbeitsbaum identisch versioniert)
RID = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room2130", "camera.rid")
TIM = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room2130", "esp16.tim")
BG  = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS", "ROOM213", "ROOM21306.bmp")
out = sys.argv[1] if len(sys.argv) > 1 else "re2_2130_lampen"
cut = int(sys.argv[2]) if len(sys.argv) > 2 else 6

r = open(RID, "rb").read()
flag, fov, px_, py_, pz_, tx, ty, tz, pri = struct.unpack_from("<HHiiiiiiI", r, cut * 32)
H = fov >> 7
print("cut %d: fov=%d H=%d pos=(%d,%d,%d) target=(%d,%d,%d)" % (cut, fov, H, px_, py_, pz_, tx, ty, tz))
dx, dy, dz = tx - px_, ty - py_, tz - pz_
dist = math.sqrt(dx*dx + dy*dy + dz*dz); horiz = math.sqrt(dx*dx + dz*dz)
sp, cp = -dy / dist, horiz / dist
sy, cy = dx / horiz, dz / horiz
R = [cy, 0, -sy, sp*sy, cp, sp*cy, cp*sy, -sp, cp*cy]
def proj(x, y, z):
    x -= px_; y -= py_; z -= pz_
    vx = R[0]*x + R[1]*y + R[2]*z; vy = R[3]*x + R[4]*y + R[5]*z; vz = R[6]*x + R[7]*y + R[8]*z
    return 160 + vx * H / vz, 120 + vy * H / vz, vz

lampen = [(-24382, -2610, -11070), (-24300, -2610, -11070), (-24210, -2610, -11070),
          (-24120, -2610, -11070), (-24030, -2610, -11070)]
S, SCALE16 = 32, 0x02BA
res = []
for i, (x, y, z) in enumerate(lampen):
    sx, sy_, sz = proj(x, y, z)
    w = S * SCALE16 * H / (sz * 256.0)
    res.append((sx, sy_, sz, w))
    print("Lampe %d welt=(%d,%d,%d) -> schirm (%.1f, %.1f) SZ=%.0f  Sprite %.1f px (S*scale16/256 = %.2f Welteinheiten)"
          % (i + 1, x, y, z, sx, sy_, sz, w, S * SCALE16 / 256.0))
# Zeiger-Grundstellung zur Kontrolle der Kamera (var7 = 0x9FAC @0x01140, pos_set y/z @0x01130)
for wert in (0, 80, 100):
    sx, sy_, sz = proj(-24660 + 9 * wert, -2754, -11070)
    print("Zeiger wert %3d -> schirm (%.1f, %.1f)" % (wert, sx, sy_))

# Sprite rendern (Zelle 3), additiv (ABR1) auf den Hintergrund
sys.path.insert(0, HERE)
d = open(TIM, "rb").read()
bl, cx_, cy_, cw, ch = struct.unpack_from("<IHHHH", d, 8)
cluts = [[struct.unpack_from("<H", d, 8 + 12 + (p * cw + k) * 2)[0] for k in range(cw)] for p in range(ch)]
o = 8 + bl
bl2, pxx, pyy, pw, ph = struct.unpack_from("<IHHHH", d, o)
pix = d[o + 12:o + bl2]
def texel(u, v):
    return (pix[v * pw * 2 + u // 2] >> (4 * (u & 1))) & 15
def c5(vv): return (vv & 31, (vv >> 5) & 31, (vv >> 10) & 31)
for pal, name in ((0, "rot"), (2, "gruen")):
    im = Image.open(BG).convert("RGB"); pxl = im.load()
    for (sx, sy_, sz, w) in res:
        x0 = sx - w / 2; y0 = sy_ - w / 2   # dx=dy=-16 -> zentriert
        for yy in range(int(math.floor(y0)), int(math.ceil(y0 + w))):
            for xx in range(int(math.floor(x0)), int(math.ceil(x0 + w))):
                u = int((xx + 0.5 - x0) * S / w); v = int((yy + 0.5 - y0) * S / w)
                if not (0 <= u < S and 0 <= v < S): continue
                t = cluts[pal][texel(96 + u, v)]
                if t == 0: continue
                r5, g5, b5 = c5(t)
                R0, G0, B0 = pxl[xx, yy]
                pxl[xx, yy] = (min(248, (R0 >> 3 << 3) + (r5 << 3)), min(248, (G0 >> 3 << 3) + (g5 << 3)),
                               min(248, (B0 >> 3 << 3) + (b5 << 3)))
    im.crop((80, 95, 240, 145)).resize((480, 150), Image.NEAREST).save(out + "_" + name + ".png")
print("geschrieben:", out + "_rot.png", out + "_gruen.png")
