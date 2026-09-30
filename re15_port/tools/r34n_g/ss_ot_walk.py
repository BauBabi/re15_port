#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht) - GPU-Ordering-Tables eines DuckStation-Savestates ablaufen.

Die Present-Routine FUN_8002137c schickt je Bild drei OTs an die GPU (RE_15_Quellcode_V2/
FUN_8002137c.c):
    DrawOTag(0x800ac714 + buf*0x40)     (klein)
    DrawOTag(0x800ab6d4 + buf*0x1000)   (Haupt-OT)
    DrawOTag(0x800aa6b4 + buf*0x20)     (klein)
buf = DAT_800aca34 (Doppelpuffer). Jedes OT-Wort = (Laenge<<24) | naechste Adresse (24 Bit),
Ende = 0xFFFFFF. Das Werkzeug listet jedes Primitiv mit Bildschirm-Rechteck und markiert die, die
ein Pruefrechteck (Default: Schild MAGAZINE CLUB in Cut 2) schneiden. Zweck: belegen, ob im
Original ueberhaupt etwas ueber die Schrift gezeichnet wird.

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_ot_walk.py <sav> [--box x0,y0,x1,y1] [--all]
"""
import argparse, os, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

OTS = [(0x800ac714, 0x40), (0x800ab6d4, 0x1000), (0x800aa6b4, 0x20)]


def s11(v):
    v &= 0x7ff
    return v - 0x800 if v & 0x400 else v


def xy(w):
    return s11(w & 0xffff), s11(w >> 16)


def decode(words):
    """-> (name, bbox or None, extra)"""
    if not words:
        return ("leer", None, "")
    c = words[0] >> 24
    if 0x20 <= c <= 0x3f:  # polygon
        quad = bool(c & 0x08); tex = bool(c & 0x04); gour = bool(c & 0x10)
        n = 4 if quad else 3
        pts = []; i = 1
        for k in range(n):
            if gour and k > 0:
                i += 1  # colour word
            pts.append(xy(words[i])); i += 1
            if tex:
                i += 1
        xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
        return ("POLY%s%s%s" % ("G" if gour else "F", "T" if tex else "", "4" if quad else "3"),
                (min(xs), min(ys), max(xs), max(ys)), "rgb=%06x abe=%d" % (words[0] & 0xffffff, (c >> 1) & 1))
    if 0x60 <= c <= 0x7f:  # rect
        tex = bool(c & 0x04); size = (c >> 3) & 3
        x, y = xy(words[1])
        if size == 0:
            wh = words[3] if tex else words[2]
            w, h = wh & 0xffff, wh >> 16
        else:
            w = h = {1: 1, 2: 8, 3: 16}[size]
        extra = "rgb=%06x abe=%d" % (words[0] & 0xffffff, (c >> 1) & 1)
        if tex:
            extra += " uv=%04x clut=%04x" % (words[2] & 0xffff, words[2] >> 16)
        return ("%s%s" % ("SPRT" if tex else "TILE", "" if size == 0 else str({1: 1, 2: 8, 3: 16}[size])),
                (x, y, x + w - 1, y + h - 1), extra)
    if 0x40 <= c <= 0x5f:
        return ("LINE", None, "")
    if c == 0x80:
        sx, sy = words[1] & 0xffff, words[1] >> 16
        dx, dy = words[2] & 0xffff, words[2] >> 16
        w, h = words[3] & 0xffff, words[3] >> 16
        return ("MOVE", (dx, dy, dx + w - 1, dy + h - 1), "src=(%d,%d)" % (sx, sy))
    if c == 0x02:
        x, y = words[1] & 0xffff, words[1] >> 16
        w, h = words[2] & 0xffff, words[2] >> 16
        return ("FILL", (x, y, x + w - 1, y + h - 1), "rgb=%06x" % (words[0] & 0xffffff))
    if 0xe1 <= c <= 0xe6:
        return ("ENV%02x" % c, None, "%06x" % (words[0] & 0xffffff))
    return ("cmd%02x" % c, None, "")


def walk(r, head, limit=20000):
    out = []
    addr = head; seen = set()
    while len(out) < limit:
        if addr in seen:
            out.append((addr, "SCHLEIFE", None, "", 0)); break
        seen.add(addr)
        tag = r.u32(0x80000000 | addr)
        n = tag >> 24; nxt = tag & 0xffffff
        if n:
            words = [r.u32(0x80000000 | (addr + 4 + 4 * i)) for i in range(n)]
            name, bb, ex = decode(words)
            out.append((addr, name, bb, ex, n))
        if nxt == 0xffffff:
            break
        addr = nxt
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sav")
    ap.add_argument("--box", default="283,48,315,66")
    ap.add_argument("--all", action="store_true")
    a = ap.parse_args()
    bx = [int(v) for v in a.box.split(",")]
    r = re15_ss.Ram(a.sav)
    buf = r.u8(0x800aca34)
    print("%s  cut=%d  aca34(buf)=%d" % (os.path.basename(a.sav), r.u16(0x800b0fe4), buf))
    for base, stride in OTS:
        for b in (0, 1):
            head = (base + b * stride) & 0xffffff
            prims = walk(r, head)
            hits = [p for p in prims if p[2] and not (p[2][2] < bx[0] or p[2][0] > bx[2] or
                                                    p[2][3] < bx[1] or p[2][1] > bx[3])]
            kinds = {}
            for p in prims:
                kinds[p[1]] = kinds.get(p[1], 0) + 1
            print("OT 0x%08x buf%d: %d Pakete %s" % (0x80000000 | head, b, len(prims),
                  " ".join("%s:%d" % kv for kv in sorted(kinds.items()))))
            for p in (prims if a.all else hits):
                print("   @0x%08x %-8s bbox=%s %s%s" % (0x80000000 | p[0], p[1], p[2], p[3],
                      "   <== SCHILD" if p in hits else ""))


if __name__ == "__main__":
    main()
