#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: alle `jal <ziel>` in RE1.5 (PSX.EXE + DEBUG.BIN + STAGE*.BIN
+ TITLE.BIN) bzw. RE2 (--re2: info/re2leon/PSX.EXE + COMMON/BIN/*.BIN) finden und je Fundstelle
den Kontext disassemblieren (Argument-Register a0..a3 sichtbar machen).

Abbildung (NIE selbst rechnen, sondern wie re15_disasm.load()):
  PSX.EXE  : text @ Datei 0x800, t_addr aus Header @0x18
  DEBUG.BIN: RE1.5 resident @0x800C0000 (FUN_80013b60(7,&0x800c0000,0), SPEC §2 Id 14)
  STAGE*/TITLE.BIN: roh @0x80100000 (kein Header)
RE2-Overlays: roh @0x80100000 (re2_disasm.py: OVERLAY_LOAD).

Aufruf:
  jal_xref.py 0x80012d60 [--vor 14] [--nach 2] [--re2] [--nur DATEI]
  jal_xref.py --word 0x80071d40 ...   (sucht zusaetzlich lui/addiu-Paare auf eine Adresse)
"""
import os, sys, struct, argparse, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R


def images(re2):
    out = []
    if re2:
        exe = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
        bindir = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN")
        extra = []
    else:
        exe = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
        bindir = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN")
        extra = [("DEBUG.BIN", 0x800C0000)]
    d = open(exe, "rb").read()
    taddr = struct.unpack_from("<I", d, 0x18)[0]
    tsize = struct.unpack_from("<I", d, 0x1c)[0]
    out.append(("PSX.EXE", taddr, d[0x800:0x800 + tsize]))
    for name, base in extra:
        p = os.path.join(bindir, name)
        if os.path.exists(p):
            out.append((name, base, open(p, "rb").read()))
    for p in sorted(glob.glob(os.path.join(bindir, "*.BIN"))):
        name = os.path.basename(p)
        if any(name == e[0] for e in extra):
            continue
        out.append((name, 0x80100000, open(p, "rb").read()))
    return out


def dis_range(img, base, a0, n):
    lines = []
    for i in range(n):
        a = a0 + 4 * i
        off = a - base
        if off < 0 or off + 4 > len(img):
            continue
        w = struct.unpack_from("<I", img, off)[0]
        s, call = R.dis_one(w, a)
        lines.append("    %08x: %08x  %s" % (a, w, s))
    return lines


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("ziel", type=lambda s: int(s, 0))
    ap.add_argument("--vor", type=int, default=14)
    ap.add_argument("--nach", type=int, default=2)
    ap.add_argument("--re2", action="store_true")
    ap.add_argument("--nur", default=None)
    a = ap.parse_args()
    jw = 0x0C000000 | ((a.ziel >> 2) & 0x03FFFFFF)
    total = 0
    for name, base, img in images(a.re2):
        if a.nur and a.nur != name:
            continue
        hits = []
        for off in range(0, len(img) - 3, 4):
            if struct.unpack_from("<I", img, off)[0] == jw:
                addr = base + off
                # jal-Ziel liegt im selben 256-MB-Segment -> Treffer echt
                hits.append(addr)
        for h in hits:
            total += 1
            print("== %s  jal 0x%08x @0x%08x" % (name, a.ziel, h))
            for l in dis_range(img, base, h - 4 * a.vor, a.vor + 1 + a.nach):
                print(l)
    print("; %d Fundstellen fuer jal 0x%08x (%s)" % (total, a.ziel, "RE2" if a.re2 else "RE1.5"))


if __name__ == "__main__":
    main()
