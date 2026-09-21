# -*- coding: utf-8 -*-
"""Zensus ueber ALLE geschriebenen Cuts der Seitendaten-Container (R15M, *.MSK).

Liest jede Sektion GENAU wie re15_port/engine/src/pri_common.c:60-145
(Build-Schranke = Summe der Gruppenzaehler, Rechteck gdw. (size_b & 0xf0)==0,
dst = Gruppenanker + Low-Byte als truncating s16) und zaehlt exakte Doppel.

Exaktes Doppel = gleiche (dstX, dstY, w, h, depth) UND gleiche (srcX, srcY).
"""
import os
import struct
import sys
import json

MASKS = "re15_port/shared_assets/PSX/MASKS"
CAP = 105          # RE15_PRI_MAX_MASKS_PER_CUT, re15_port/include/re15_pri.h:50


def parse_section(b, off):
    """-> (masks, declared, group_count, build_total)"""
    gc, mc = struct.unpack_from("<HH", b, off)
    gp = off + 4
    gn, gdx, gdy = [], [], []
    for i in range(gc):
        n, base, dx, dy = struct.unpack_from("<HHhh", b, gp)
        gn.append(n); gdx.append(dx); gdy.append(dy)
        gp += 8
    build_total = sum(gn)
    mp = gp
    out = []
    grp, used = 0, 0
    for i in range(build_total):
        if mp + 8 > len(b):
            break
        sx, sy, dxl, dyl = b[mp], b[mp + 1], b[mp + 2], b[mp + 3]
        dep = struct.unpack_from("<H", b, mp + 4)[0]
        size_b = b[mp + 7]
        mp += 8
        if (size_b & 0xF0) == 0:
            if mp + 4 > len(b):
                break
            w, h = struct.unpack_from("<HH", b, mp)
            mp += 4
        else:
            w = h = (size_b >> 4) * 8
        while grp < gc and used >= gn[grp]:
            grp += 1
            used = 0
        ax = gdx[grp] if grp < gc else 0
        ay = gdy[grp] if grp < gc else 0
        used += 1
        dx = ((dxl + ax + 0x8000) & 0xFFFF) - 0x8000
        dy = ((dyl + ay + 0x8000) & 0xFFFF) - 0x8000
        out.append((sx, sy, dx & 0xFFFF, dy & 0xFFFF, dep, w, h))
    return out, mc, gc, build_total


def container(path):
    b = open(path, "rb").read()
    assert b[:4] == b"R15M", path
    ver, cuts = struct.unpack_from("<II", b, 4)
    offs = struct.unpack_from("<%dI" % cuts, b, 12)
    res = {}
    for c, o in enumerate(offs):
        if o == 0 or o + 4 > len(b):
            continue
        res[c] = parse_section(b, o)
    return res, cuts


def main():
    rows = []
    for f in sorted(os.listdir(MASKS)):
        if not f.endswith(".MSK"):
            continue
        room = f[:-4]
        res, ncut = container(os.path.join(MASKS, f))
        for c in sorted(res):
            masks, decl, gc, build = res[c]
            n = len(masks)
            seen = {}
            dup_pairs = 0
            for i, m in enumerate(masks):
                if m in seen:
                    dup_pairs += 1
                else:
                    seen[m] = i
            uniq = len(seen)
            rows.append(dict(room=room, cut=c, gruppen=gc, gebaut=n, decl=decl,
                             uniq=uniq, doppel=dup_pairs,
                             gezeichnet=min(decl & 0xFF, min(n, CAP)),
                             gekappt=max(0, n - CAP)))
    return rows


if __name__ == "__main__":
    rows = main()
    print("Cuts geschrieben: %d  (Raeume: %d)" % (
        len(rows), len(set(r["room"] for r in rows))))
    tot = sum(r["gebaut"] for r in rows)
    totd = sum(r["doppel"] for r in rows)
    print("Masken gesamt: %d   davon exakte Doppel: %d (%.1f %%)" % (tot, totd, 100.0 * totd / tot))
    print()
    hdr = "%-9s %3s %4s %5s %5s %5s %6s %6s" % ("Raum", "Cut", "Grp", "geb.", "uniq", "dopp", "gez.", "kapp")
    print(hdr); print("-" * len(hdr))
    for r in sorted(rows, key=lambda r: (-r["doppel"], -r["gebaut"])):
        print("%-9s %3d %4d %5d %5d %5d %6d %6d" % (
            r["room"], r["cut"], r["gruppen"], r["gebaut"], r["uniq"], r["doppel"],
            r["gezeichnet"], r["gekappt"]))
    json.dump(rows, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "zensus.json"), "w"), indent=1)
