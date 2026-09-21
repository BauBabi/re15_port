#!/usr/bin/env python3
"""re2_aot_on_bg.py - AOT-Rechtecke eines RE2-Raums in jede Kamera projizieren und
auf den vorgerenderten Hintergrund zeichnen.

Damit laesst sich die Frage "steckt der Gegenstand im Hintergrundbild?" ANSCHAUEN
statt sie zu vermuten: das Rechteck landet dort, wo der Spieler untersuchen kann.

Kameramodell 1:1 aus re15_port/engine/src/camera_common.c (dort gegen die RE1.5-
Originalfunktion FUN_80053ca4 @0x80053ca4 belegt) und re15_port/include/re15_camera.h:
  RID-Record 32 B: +0 flag u16, +2 fov u16, +4 pos xyz s32, +0x10 target xyz s32,
                   +0x1C pri_offset u32
  H = fov >> 7                       (fov_to_screen_dist, camera_common.c:38)
  dist  = sqrt(dx^2+dy^2+dz^2), horiz = sqrt(dx^2+dz^2)
  sp = -dy*4096/dist, cp = horiz*4096/dist, sy = dx*4096/horiz, cy = dz*4096/horiz
  V = Rx(pitch)*Ry(yaw), t = V*(-pos)
  Bildpunkt sx = 160 + H*x/z, sy = 120 + H*y/z

Aufruf:
    re2_aot_on_bg.py <room2030> <ROOM203> <zielordner> [y] [--only-item=<id>]
"""
import os, sys, struct, math
from PIL import Image, ImageDraw

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
BSS = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import re2_scd_walk, re2_items
import glob


def isqrt(x):
    return int(math.isqrt(max(0, int(x))))


def build_view(cut):
    flag, fov = cut[0], cut[1]
    px, py, pz, tx, ty, tz = cut[2:8]
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx * dx + dy * dy + dz * dz)
    if dist == 0:
        return None
    horiz = isqrt(dx * dx + dz * dz)
    sp = int((-dy) * 4096 / dist) if dist else 0
    cp = int(horiz * 4096 / dist) if dist else 0
    if horiz:
        sy = int(dx * 4096 / horiz)
        cy = int(dz * 4096 / horiz)
        r = [cy, 0, -sy,
             (sp * sy) >> 12, cp, (sp * cy) >> 12,
             (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    else:
        r = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(r[0] * -px + r[1] * -py + r[2] * -pz) >> 12,
         (r[3] * -px + r[4] * -py + r[5] * -pz) >> 12,
         (r[6] * -px + r[7] * -py + r[8] * -pz) >> 12]
    return r, t, fov >> 7


def project(view, x, y, z):
    r, t, H = view
    vx = ((r[0] * x + r[1] * y + r[2] * z) >> 12) + t[0]
    vy = ((r[3] * x + r[4] * y + r[5] * z) >> 12) + t[1]
    vz = ((r[6] * x + r[7] * y + r[8] * z) >> 12) + t[2]
    if vz <= 1:
        return None
    return (160 + H * vx // vz, 120 + H * vy // vz, vz)


def aots(room):
    """alle Aot_set(0x2C)/Item_aot_set(0x4E)/Door_aot_set(0x3B) mit Rechteck."""
    out = []
    for f in sorted(glob.glob(os.path.join(RDT, room, "scd", "*.scd"))):
        ent = os.path.splitext(os.path.basename(f))[0]
        buf = open(f, "rb").read()
        recs, st = re2_scd_walk.walk(buf)
        for off, op, r in recs:
            if op in (0x2C, 0x4E, 0x3B) and len(r) >= 14:
                x, z, w, d = struct.unpack_from("<hhhh", r, 6)
                lab = {0x2C: "Aot", 0x4E: "ITEM", 0x3B: "Door"}[op]
                if op == 0x4E:
                    iid = struct.unpack_from("<H", r, 14)[0]
                    nm = re2_items.name(iid)[1] if iid < re2_items.N_NAMES else "?"
                    lab = "ITEM %d %s md1=%d" % (iid, nm, r[20])
                out.append((ent, off, op, r[1], x, z, w, d, lab))
    return out


def main():
    room, bssname, out = sys.argv[1], sys.argv[2], sys.argv[3]
    rest = [a for a in sys.argv[4:] if not a.startswith("--")]
    y = int(rest[0]) if rest else 0
    only = None
    for a in sys.argv[4:]:
        if a.startswith("--only-item="):
            only = int(a.split("=", 1)[1])
    os.makedirs(out, exist_ok=True)
    rid = open(os.path.join(RDT, room, "camera.rid"), "rb").read()
    n = len(rid) // 32
    A = aots(room)
    bmps = sorted(glob.glob(os.path.join(BSS, bssname, "*.bmp")))
    print("%s: %d Kameras, %d Hintergrundbilder, %d AOT-Rechtecke" % (room, n, len(bmps), len(A)))
    for i in range(n):
        cut = struct.unpack_from("<HHiiiiiiI", rid, i * 32)
        v = build_view(cut)
        if v is None:
            continue
        im = Image.open(bmps[i]).convert("RGB") if i < len(bmps) else Image.new("RGB", (320, 240))
        im = im.resize((640, 480), Image.NEAREST)
        dr = ImageDraw.Draw(im)
        hits = 0
        for (ent, off, op, aot, x, z, w, d, lab) in A:
            if only is not None and not (op == 0x4E and lab.startswith("ITEM %d " % only)):
                continue
            pts = [project(v, x, y, z), project(v, x + w, y, z),
                   project(v, x + w, y, z + d), project(v, x, y, z + d)]
            if any(p is None for p in pts):
                continue
            xs = [p[0] * 2 for p in pts]
            ys = [p[1] * 2 for p in pts]
            if max(xs) < 0 or min(xs) > 640 or max(ys) < 0 or min(ys) > 480:
                continue
            hits += 1
            col = (255, 60, 60) if op == 0x4E else ((90, 160, 255) if op == 0x3B else (255, 220, 60))
            dr.polygon(list(zip(xs, ys)), outline=col)
            dr.text((min(xs) + 2, min(ys) + 2), "%s#%d %s" % (ent, aot, lab), fill=col)
        p = os.path.join(out, "%s_cam%02d.png" % (room, i))
        im.save(p)
        print("  cam%02d fov=%d H=%d pos=(%d,%d,%d) tgt=(%d,%d,%d)  %d Rechtecke -> %s"
              % (i, cut[1], v[2], cut[2], cut[3], cut[4], cut[5], cut[6], cut[7],
                 hits, os.path.basename(p)))


if __name__ == "__main__":
    main()
