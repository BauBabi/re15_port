#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_files_png.py — macht aus den 216 geschnittenen RE2-Dokument-TIMs PNGs.

Zweck: die RE2-Dokumente als VORLAGE fuer eigene Dokumente nutzbar machen. Der Schneider
re2_files_cut.py liefert die TIMs byte-true; dieses Werkzeug macht daraus Bilder, die man
in jedem Malprogramm oeffnen und ueberschreiben kann, plus eine Kontaktbogen-Uebersicht.

⛔ GEOMETRIE, aus der RE2-EXE belegt (FUN 0x80075fd0, der FILE-Leser):
  Ein Dokument = 1 Papier-Bild (8bpp, 128x256) + N Textseiten (4bpp, 256xH).
  Der Leser zeichnet ZWEI Sprites in eine 256x256-Flaeche:
    [1] Textseite  4bpp: u=0, v=0, w=256, h=H       CLUT (0,490)
        u0/v0 = 0   `sb zero,-2(s0)` @0x80076068 / `sb zero,-1(s0)` @0x8007606c
        w     = 256 `sh s1,2(s0)`    @0x80076070
        h     = H   `sh s5,4(s0)`    @0x80076078,  s5 = 256 - y_off @0x80076040-44
    [2] Papier    8bpp: u=0, v=H, w=128, h=256-H    CLUT (0,489)
        v = H       `subu v0,zero,s3` @0x800760b8 / `sb v0,-1(s0)` @0x800760d0
  H kommt aus dem Dokument-Record @0x800aa144 (+2 = y_off, H = 256 - y_off) und ist
  eines von 112/128/144/176 — selbst nachgerechnet ueber alle 25 Dokumente.

  Fuer eigene Dokumente heisst das: die Textseite ist 256 breit und H hoch, das
  Papierbild ist 128 breit und liegt UNTEN BUENDIG auf einer 128x256-Leinwand.

Aufruf:
    python re15_port/tools/re2_files_png.py            # aus build/extracted/re2_files
    python re15_port/tools/re2_files_png.py <in> <out>
"""
import csv
import os
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
IN = os.path.join(REPO, "build", "extracted", "re2_files")
OUT = os.path.join(REPO, "build", "extracted", "re2_files_png")


def tim_decode(buf):
    """PSX-TIM -> (breite, hoehe, RGBA-Bytes). 4bpp und 8bpp mit CLUT."""
    magic, flags = struct.unpack_from("<II", buf, 0)
    if magic != 0x10:
        return None
    pmode = flags & 7
    has_clut = (flags >> 3) & 1
    o = 8
    clut = None
    if has_clut:
        clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", buf, o)
        entries = cw * ch
        clut = [struct.unpack_from("<H", buf, o + 12 + 2 * i)[0] for i in range(entries)]
        o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", buf, o)
    pix = buf[o + 12:o + ilen]
    if pmode == 0:      # 4bpp: ein u16 haelt VIER Pixel
        w = iw * 4
    elif pmode == 1:    # 8bpp: ein u16 haelt ZWEI Pixel
        w = iw * 2
    else:
        return None
    h = ih

    def rgba(c):
        # PSX 16-bit: STP + 5/5/5 BGR. Index/Farbe 0 = durchsichtig (so zeichnet es die GPU).
        r = (c & 0x1F) << 3
        g = ((c >> 5) & 0x1F) << 3
        b = ((c >> 10) & 0x1F) << 3
        return (r, g, b)

    out = bytearray(w * h * 4)
    for y in range(h):
        for x in range(w):
            if pmode == 0:
                byte = pix[y * (w // 2) + (x >> 1)]
                idx = (byte & 0x0F) if (x & 1) == 0 else (byte >> 4)
            else:
                idx = pix[y * w + x]
            c = clut[idx] if clut and idx < len(clut) else 0
            r, g, b = rgba(c)
            p = (y * w + x) * 4
            out[p] = r; out[p + 1] = g; out[p + 2] = b
            out[p + 3] = 0 if idx == 0 else 255
    return w, h, bytes(out)


def png_write(path, w, h, rgba):
    raw = b"".join(b"\x00" + rgba[y * w * 4:(y + 1) * w * 4] for y in range(h))

    def chunk(tag, data):
        c = struct.pack(">I", len(data)) + tag + data
        return c + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n")
        f.write(chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)))
        f.write(chunk(b"IDAT", zlib.compress(raw, 9)))
        f.write(chunk(b"IEND", b""))


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else IN
    dst = sys.argv[2] if len(sys.argv) > 2 else OUT
    if not os.path.isdir(dst):
        os.makedirs(dst)
    rows = list(csv.DictReader(open(os.path.join(src, "toc.csv"))))
    n_ok = 0
    n_bad = 0
    for r in rows:
        p = os.path.join(src, r["name"])
        if not os.path.isfile(p):
            print("fehlt:", r["name"]); n_bad += 1; continue
        got = tim_decode(open(p, "rb").read())
        if not got:
            print("kein 4/8bpp-TIM:", r["name"]); n_bad += 1; continue
        w, h, rgba = got
        # Gegenprobe gegen die TOC des Schneiders - beide muessen dasselbe sagen.
        if int(r["width"]) != w or int(r["height"]) != h:
            print("MASS WEICHT AB %s: TOC %sx%s, TIM %dx%d"
                  % (r["name"], r["width"], r["height"], w, h))
            n_bad += 1
        png_write(os.path.join(dst, r["name"].replace(".TIM", ".png")), w, h, rgba)
        n_ok += 1
    print("%d PNG geschrieben, %d Probleme -> %s" % (n_ok, n_bad, dst))
    # Kurzuebersicht je Dokument
    per = {}
    for r in rows:
        per.setdefault(int(r["doc"]), {"pages": 0, "h": r["doc_page_h"]})
        if r["role"] == "page":
            per[int(r["doc"])]["pages"] += 1
    print("\nDokument | Textseiten | Seitenhoehe")
    for d in sorted(per):
        print("   %2d    |     %2d     |    %s" % (d, per[d]["pages"], per[d]["h"]))


if __name__ == "__main__":
    main()
