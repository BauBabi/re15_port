#!/usr/bin/env python3
"""rdt_props.py - die eingebetteten Prop-Modelle (TIM+MD1) eines RE2-RDT schneiden.

Struktur (identisch zu RE1.5, im Port als rdt_common.c:51-79 umgesetzt, dort
gegen RDTExtractor.parseModelPointers belegt):
    RDT+0x02  u8   nOmodel          Anzahl der Prop-Modelle
    RDT+0x30  u32  Tabellen-Offset  -> 2*nOmodel u32-Offsets, abwechselnd TIM, MD1
Jeder Koerper laeuft bis zum naechstgroesseren Offset aus der Gesamtmenge der
Header-Offsets (0x08..0x5C) plus der Modelltabelle selbst.

Aufruf:
    rdt_props.py list  <ROOM....RDT>
    rdt_props.py dump  <ROOM....RDT> <slot> <ziel-praefix>
"""
import struct, sys, os

HDR_OFFS = list(range(0x08, 0x60, 4))          # die u32-Offsets im RDT-Kopf


def parse(path):
    d = open(path, "rb").read()
    n = d[0x02]
    tbl = struct.unpack_from("<I", d, 0x30)[0]
    offs = []
    for o in HDR_OFFS:
        v = struct.unpack_from("<I", d, o)[0]
        if 0 < v <= len(d):
            offs.append(v)
    ptrs = []
    for k in range(2 * n):
        v = struct.unpack_from("<I", d, tbl + k * 4)[0]
        ptrs.append(v)
        if 0 < v <= len(d):
            offs.append(v)
    offs.append(tbl)
    offs.append(len(d))
    offs = sorted(set(offs))

    def end_of(start):
        for v in offs:
            if v > start:
                return v
        return len(d)

    props = []
    for k in range(n):
        t, m = ptrs[2 * k], ptrs[2 * k + 1]
        props.append({
            "slot": k,
            "tim_off": t, "tim_size": (end_of(t) - t) if 0 < t <= len(d) else 0,
            "md1_off": m, "md1_size": (end_of(m) - m) if 0 < m <= len(d) else 0,
        })
    return d, n, tbl, props


def md1_info(d, off, size):
    """MD1-Kopf: u32 size, u32 tri_off, u32 quad_off, u32 nObj (RE15_KNOWLEDGE §1.4)."""
    if size < 16:
        return None
    sz, tri, quad, nobj = struct.unpack_from("<IIII", d, off)
    return dict(size=sz, tri_off=tri, quad_off=quad, nobj=nobj)


def tim_info(d, off, size):
    if size < 8 or struct.unpack_from("<I", d, off)[0] != 0x10:
        return None
    flags = struct.unpack_from("<I", d, off + 4)[0]
    p = off + 8
    out = {"flags": flags, "bpp": {0: 4, 1: 8, 2: 16, 3: 24}.get(flags & 3)}
    if flags & 8:
        csz, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, p)
        out["clut"] = (csz, cx, cy, cw, ch)
        p += csz
    isz, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
    out["img"] = (isz, ix, iy, iw, ih)
    out["px"] = (iw * (16 // out["bpp"]) if out["bpp"] in (4, 8) else iw, ih)
    return out


if __name__ == "__main__":
    mode, path = sys.argv[1], sys.argv[2]
    d, n, tbl, props = parse(path)
    if mode == "list":
        print("%s: %d Byte, nOmodel=%d, Modelltabelle @0x%X" % (
            os.path.basename(path), len(d), n, tbl))
        for p in props:
            ti = tim_info(d, p["tim_off"], p["tim_size"])
            mi = md1_info(d, p["md1_off"], p["md1_size"])
            print("  Slot %2d  TIM @0x%06X %6d B %-28s   MD1 @0x%06X %6d B %s" % (
                p["slot"], p["tim_off"], p["tim_size"],
                ("%dbpp %dx%d VRAM(%d,%d)" % (ti["bpp"], ti["px"][0], ti["px"][1],
                                              ti["img"][1], ti["img"][2])) if ti else "-",
                p["md1_off"], p["md1_size"],
                ("size=%d nObj=%d tri@%d quad@%d" % (mi["size"], mi["nobj"],
                                                     mi["tri_off"], mi["quad_off"])) if mi else "-"))
    elif mode == "dump":
        k = int(sys.argv[3]); pre = sys.argv[4]
        p = props[k]
        os.makedirs(os.path.dirname(pre) or ".", exist_ok=True)
        open(pre + ".tim", "wb").write(d[p["tim_off"]:p["tim_off"] + p["tim_size"]])
        open(pre + ".md1", "wb").write(d[p["md1_off"]:p["md1_off"] + p["md1_size"]])
        print("Slot %d: TIM 0x%06X+%d -> %s.tim ; MD1 0x%06X+%d -> %s.md1" % (
            k, p["tim_off"], p["tim_size"], pre, p["md1_off"], p["md1_size"], pre))
