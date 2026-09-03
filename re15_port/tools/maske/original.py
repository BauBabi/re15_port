"""Zeigt die ORIGINAL-Masken der Kuenstler: Rechtecke aus dem RDT, Pixel aus dem
SLD-Atlas des BSS-Chunks. Das ist der Massstab, an dem sich nachgezeichnete Masken
messen muessen — WIE eng sitzt eine Kuenstlermaske am Objekt, wie viele Rechtecke,
wie stark streut die Tiefe.

Aufruf:  python re15_port/tools/maske/original.py ROOM1150 [cut]
"""
import argparse, os, struct, sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg, load_rdt
from blatt import aufhellen

CD = "re15_port/shared_assets/PSX"
TBL = {1: 0x1EAE4, 2: 0x1713C, 3: 0x1D0F8, 4: 0x18590, 5: 0x1DF18, 6: 0x01E3C}


def sld_decompress(src, sp, size):
    dst = bytearray(size); dp = 0; fm = 0; fb = 0; n = len(src)
    while dp < size:
        if fm == 0:
            fb = src[sp]; sp += 1; fm = 0x80
        a = src[sp]; sp += 1
        if a < 0x80:
            dst[dp] = a; dp += 1
        elif (fb & fm) == 0:
            dst[dp] = a; dp += 1; fm >>= 1
        else:
            b2 = src[sp]; sp += 1
            raw = (a << 4) | (b2 >> 4)
            if b2 & 0x0F:
                ln = (b2 & 0x0F) + 2
                off = raw - 0x1000 if raw & 0x800 else raw
            else:
                b3 = src[sp]; sp += 1
                ln = (b3 & 0x3F) + 3
                s = raw - 0x1000 if raw & 0x800 else raw
                off = ((s << 2) | (b3 >> 6))
                off = off - 0x10000 if off & 0x8000 else off
            fm >>= 1
            base = dp + off
            for k in range(ln):
                dst[dp + k] = dst[base + k]
            dp += ln
    return bytes(dst)


def atlas(room):
    """-> dict cut -> (idx 256xW uint8, clut) aus dem BSS-Chunk."""
    rid = int(room[4:], 16)
    stage = rid >> 12
    ri = (rid >> 4) & 0xFF
    stg = open(os.path.join(CD, "BIN", "STAGE%d.BIN" % stage), "rb").read()
    bss = os.path.join(CD, "STAGE%d" % stage, "ROOM%03X.BSS" % (rid >> 4))
    if not os.path.exists(bss):
        return {}
    blob = open(bss, "rb").read()
    out = {}
    for cut in range(16):
        off = TBL[stage] + ri * 0x20 + cut * 2
        if off + 2 > len(stg):
            continue
        L = struct.unpack_from("<H", stg, off)[0]
        c0 = cut * 0x10000
        if L < 8 or c0 + L > len(blob):
            continue
        chunk = blob[c0:c0 + 0x10000]
        present = struct.unpack_from("<I", chunk, L - 4)[0]
        if present == 0:
            continue
        so = struct.unpack_from("<I", chunk, L - 8)[0]
        if so + 4 > len(chunk):
            continue
        size = struct.unpack_from("<I", chunk, so)[0]
        try:
            tim = sld_decompress(chunk, so + 4, size)
        except Exception as e:
            print("  Cut %d: Entpacken fehlgeschlagen (%s)" % (cut, e)); continue
        if tim[:4] != b"\x10\x00\x00\x00":
            continue
        flag = struct.unpack_from("<I", tim, 4)[0]
        o = 8; clut = None
        if flag & 8:
            bl, cx, cy, cw, ch = struct.unpack_from("<IHHHH", tim, o)
            clut = np.frombuffer(tim, np.uint16, cw * ch, o + 12); o += bl
        bl, px, py, pw, ph = struct.unpack_from("<IHHHH", tim, o)
        idx = np.frombuffer(tim, np.uint8, pw * 2 * ph, o + 12).reshape(ph, pw * 2)
        out[cut] = (idx, clut)
    return out


def artist_rects(rdt, cam, cut):
    po = struct.unpack_from("<I", rdt, cam + cut * 32 + 0x1C)[0]
    if po + 4 > len(rdt):
        return None
    gc, mc = struct.unpack_from("<HH", rdt, po)
    if gc in (0, 0xFFFF) or mc == 0 or gc > 256:
        return None
    p = po + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", rdt, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = rdt[p], rdt[p+1], rdt[p+2], rdt[p+3]
        dep, size = struct.unpack_from("<HH", rdt, p + 4); p += 8
        if (size & 0xF000) == 0:
            w, h = struct.unpack_from("<HH", rdt, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        out.append((sx, sy,
                    ((dx + ax + 0x8000) & 0xFFFF) - 0x8000,
                    ((dy + ay + 0x8000) & 0xFFFF) - 0x8000, w, h, dep, gi))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room"); ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--out", default="build/original")
    a = ap.parse_args()
    room = a.room.upper(); rid = int(room[4:], 16)
    rdt, _ = load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    at = atlas(room)
    os.makedirs(a.out, exist_ok=True)
    cuts = [a.cut] if a.cut is not None else range(rdt[1])
    for cut in cuts:
        ms = artist_rects(rdt, cam, cut)
        if not ms:
            continue
        bg = load_bg(a.ppm, rid, cut)
        if bg is None:
            continue
        idx = at.get(cut, (None, None))[0]
        deck = np.zeros((240, 320), bool)
        for (sx, sy, X, Y, w, h, dep, gi) in ms:
            x0, x1 = max(0, X), min(320, X + w); y0, y1 = max(0, Y), min(240, Y + h)
            if x1 <= x0 or y1 <= y0: continue
            if idx is not None:
                sub = idx[sy+(y0-Y):sy+(y1-Y), sx+(x0-X):sx+(x1-X)]
                op = sub != 0
                if op.shape != (y1-y0, x1-x0):
                    op = np.ones((y1-y0, x1-x0), bool)
            else:
                op = np.ones((y1-y0, x1-x0), bool)
            deck[y0:y1, x0:x1] |= op
        base = aufhellen(bg); ov = base.copy()
        ov[deck] = ov[deck]*0.35 + np.array([0.1,1.0,0.4])*0.65
        Z = 3
        L = Image.fromarray((base*255).astype(np.uint8)).resize((320*Z,240*Z), Image.LANCZOS)
        R = Image.fromarray((ov*255).astype(np.uint8)).resize((320*Z,240*Z), Image.NEAREST)
        d = ImageDraw.Draw(R)
        for (sx, sy, X, Y, w, h, dep, gi) in ms:
            d.rectangle([X*Z, Y*Z, (X+w)*Z-1, (Y+h)*Z-1], outline=(0,255,255))
            d.text((X*Z+2, Y*Z+1), str(dep), fill=(255,255,0))
        s = Image.new("RGB",(320*Z*2+8,240*Z),(30,30,30)); s.paste(L,(0,0)); s.paste(R,(320*Z+8,0))
        p = os.path.join(a.out, "%s_%02d_orig.png" % (room, cut)); s.save(p)
        deps = sorted(set(m[6] for m in ms))
        print("%s  %d Rechtecke, %d Gruppen, Tiefen %s, Deckung %.1f %%"
              % (p, len(ms), len(set(m[7] for m in ms)),
                 (str(deps) if len(deps) <= 8 else "%d..%d (%d verschiedene)" % (deps[0], deps[-1], len(deps))),
                 100*deck.mean()))


if __name__ == "__main__":
    main()
