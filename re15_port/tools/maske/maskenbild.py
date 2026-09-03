"""Zeigt, was die fertige Maske im Spiel WIRKLICH verdeckt.

Unterschied zu pruefblatt.py: dort wird die AUSWAHL (Superpixel-Nummern) gezeigt,
also die Absicht. Hier wird das ERGEBNIS gezeigt — die Pixel, die der Renderer
aus dem Atlas an ihre Zielstelle blittet, samt Aufweitung, Spaltenfuellung und
Rechteckzerlegung. Genau diese Pixel verdecken die Figur.

Rechts liegt die Verdeckungsflaeche magenta ueber dem Bild; zusaetzlich wird je
Rechteck die Tiefe (depth) eingeblendet, denn ein Pixel verdeckt nur, wenn seine
Tiefe naeher ist als die Figur.

Aufruf:
    python re15_port/tools/maske/maskenbild.py ROOM1140 2
    python re15_port/tools/maske/maskenbild.py ROOM1140        (alle Cuts mit Maske)
"""
import argparse
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg
from blatt import aufhellen

MASKS = "re15_port/shared_assets/PSX/MASKS"


def lies_tim(p):
    """-> (idx 256x256 uint8, clut 256 uint16) oder None"""
    if not os.path.exists(p):
        return None
    b = open(p, "rb").read()
    magic, flag = struct.unpack_from("<II", b, 0)
    if magic != 0x10:
        return None
    o = 8
    clut = None
    if flag & 8:
        blen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
        clut = np.frombuffer(b, np.uint16, cw * ch, o + 12)
        o += blen
    blen, px, py, pw, ph = struct.unpack_from("<IHHHH", b, o)
    idx = np.frombuffer(b, np.uint8, pw * 2 * ph, o + 12).reshape(ph, pw * 2)
    return idx, clut


def masken(blob, cut):
    """-> Liste (atlasX, atlasY, ZielX, ZielY, w, h, depth) oder None"""
    if blob is None or blob[:4] != b"R15M":
        return None
    _, nc = struct.unpack_from("<II", blob, 4)
    if cut >= nc:
        return None
    offs = struct.unpack_from("<%dI" % nc, blob, 12)
    if not offs[cut]:
        return None
    o = offs[cut]
    gc, mc = struct.unpack_from("<HH", blob, o)
    p = o + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", blob, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = blob[p], blob[p + 1], blob[p + 2], blob[p + 3]
        dep, size = struct.unpack_from("<HH", blob, p + 4); p += 8
        if (size & 0xF000) == 0:
            w, h = struct.unpack_from("<HH", blob, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        X = ((dx + ax + 0x8000) & 0xFFFF) - 0x8000
        Y = ((dy + ay + 0x8000) & 0xFFFF) - 0x8000
        out.append((sx, sy, X, Y, w, h, dep))
    return out


def bild(room, cut, ppm, out):
    rid = int(room[4:], 16)
    bg = load_bg(ppm, rid, cut)
    if bg is None:
        return None
    blob = open(os.path.join(MASKS, "%s.MSK" % room), "rb").read()
    ms = masken(blob, cut)
    if not ms:
        return None
    t = lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
    idx = t[0] if t else None

    deck = np.zeros((240, 320), bool)
    tief = np.zeros((240, 320), np.int32)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        if idx is not None:
            sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
            op = sub != 0
        else:
            op = np.ones((y1 - y0, x1 - x0), bool)
        deck[y0:y1, x0:x1] |= op
        z = tief[y0:y1, x0:x1]
        tief[y0:y1, x0:x1] = np.where(op & ((z == 0) | (dep < z)), dep, z)

    base = aufhellen(bg)
    ov = base.copy()
    ov[deck] = ov[deck] * 0.35 + np.array([1.0, 0.15, 0.9]) * 0.65
    pil = Image.fromarray((ov * 255).astype(np.uint8)).resize((960, 720), Image.NEAREST)
    d = ImageDraw.Draw(pil)
    for (sx, sy, X, Y, w, h, dep) in ms:
        d.rectangle([X * 3, Y * 3, (X + w) * 3 - 1, (Y + h) * 3 - 1], outline=(0, 255, 255))
        d.text((X * 3 + 2, Y * 3 + 1), str(dep), fill=(0, 255, 255))
    # Koordinatengitter alle 20 Bildpunkte — damit sich --minus/--plus-Kaesten direkt
    # ablesen lassen, statt sie zu schaetzen.
    links = Image.fromarray((base * 255).astype(np.uint8)).resize((960, 720), Image.LANCZOS)
    dl = ImageDraw.Draw(links)
    for gx in range(0, 320, 20):
        dl.line([gx * 3, 0, gx * 3, 720], fill=(255, 120, 0))
        dl.text((gx * 3 + 2, 2), str(gx), fill=(255, 200, 0))
    for gy in range(0, 240, 20):
        dl.line([0, gy * 3, 960, gy * 3], fill=(255, 120, 0))
        dl.text((2, gy * 3 + 2), str(gy), fill=(255, 200, 0))
    sheet = Image.new("RGB", (960 * 2 + 8, 720), (30, 30, 30))
    sheet.paste(links, (0, 0))
    sheet.paste(pil, (968, 0))
    os.makedirs(out, exist_ok=True)
    p = os.path.join(out, "%s_%02d_maske.png" % (room, cut))
    sheet.save(p)
    return p, ms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--out", default="build/maskenbild")
    a = ap.parse_args()
    room = a.room.upper()
    cuts = [a.cut] if a.cut is not None else range(32)
    for c in cuts:
        r = bild(room, c, a.ppm, a.out)
        if r:
            p, ms = r
            print("%s  %d Rechtecke, Tiefen %d..%d" %
                  (p, len(ms), min(m[6] for m in ms), max(m[6] for m in ms)))


if __name__ == "__main__":
    main()
