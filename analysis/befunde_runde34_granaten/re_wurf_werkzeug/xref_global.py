#!/usr/bin/env python3
"""Runde 34 / Granate: ALLE Lade-/Speicherzugriffe auf eine absolute RAM-Adresse finden
(EXE + Overlays), inkl. Adressbildung per lui/addiu. Reines Mess-/RE-Werkzeug.

Muster (MIPS, R3000): `lui rX,hi` gefolgt (bis zu WIN Instruktionen spaeter, ohne dass rX
dazwischen ueberschrieben wird) von `lb/lbu/lh/lhu/lw/sb/sh/sw/addiu rY,lo(rX)`, wobei
(hi<<16)+sign_extend(lo) == Zieladresse. Binaerdateien und Ladeadressen:
  PSX.EXE   text @Datei 0x800, t_addr aus Kopf @0x18
  STAGE1..6.BIN / TITLE.BIN  roh @0x80100000 (kein Kopf)
  DEBUG.BIN roh @0x800c0000 (resident, FUN_80013b60(7,&0x800c0000,0))

Aufruf: python xref_global.py 0x800b5358 [--re2]
Ausgabe: je Treffer  <bin> <addr>: <instr>   (+ lui-Stelle)
"""
import os, sys, struct, importlib.util

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
spec = importlib.util.spec_from_file_location(
    "d", os.path.join(REPO, ".claude/skills/re15-psx-disasm/scripts/re15_disasm.py"))
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)

WIN = 24
MEMOPS = {0x20: "lb", 0x24: "lbu", 0x21: "lh", 0x25: "lhu", 0x23: "lw",
          0x28: "sb", 0x29: "sh", 0x2b: "sw", 0x09: "addiu", 0x0d: "ori"}


def images(re2=False):
    if re2:
        base = os.path.join(REPO, "info", "re2leon")
        exe = os.path.join(base, "PSX.EXE")
        ovd = os.path.join(base, "COMMON", "BIN")
        ovs = [f for f in sorted(os.listdir(ovd)) if f.upper().endswith(".BIN")]
        res = []
        raw = open(exe, "rb").read()
        t = struct.unpack_from("<I", raw, 0x18)[0]
        res.append(("PSX.EXE(RE2)", raw[0x800:], t))
        for f in ovs:
            res.append((f, open(os.path.join(ovd, f), "rb").read(), 0x80100000))
        return res
    raw = open(d.EXE_PATH, "rb").read()
    t = struct.unpack_from("<I", raw, 0x18)[0]
    res = [("PSX.EXE", raw[0x800:], t)]
    for f in ["STAGE1.BIN", "STAGE2.BIN", "STAGE3.BIN", "STAGE4.BIN", "STAGE5.BIN",
              "STAGE6.BIN", "TITLE.BIN"]:
        res.append((f, open(os.path.join(d.OVERLAY_DIR, f), "rb").read(), 0x80100000))
    res.append(("DEBUG.BIN", open(os.path.join(d.OVERLAY_DIR, "DEBUG.BIN"), "rb").read(),
                0x800c0000))
    return res


def scan(target, re2=False):
    hi = (target >> 16) & 0xffff
    lo = target & 0xffff
    if lo & 0x8000:
        hi = (hi + 1) & 0xffff
    hits = []
    for name, data, base in images(re2):
        n = len(data) // 4
        words = struct.unpack_from("<%dI" % n, data, 0)
        for i, w in enumerate(words):
            op = w >> 26
            if op not in MEMOPS or (w & 0xffff) != lo:
                continue
            rs = (w >> 21) & 31
            # rueckwaerts das lui rs,hi suchen (rs darf dazwischen nicht neu geschrieben werden)
            ok = None
            for k in range(1, WIN + 1):
                j = i - k
                if j < 0:
                    break
                wj = words[j]
                opj = wj >> 26
                if opj == 0x0f and ((wj >> 16) & 31) == rs:
                    if (wj & 0xffff) == hi:
                        ok = base + 4 * j
                    break
            if ok is None:
                continue
            a = base + 4 * i
            txt, _ = d.dis_one(w, a)
            hits.append((name, a, txt, ok))
    return hits


if __name__ == "__main__":
    tgt = int(sys.argv[1], 0)
    re2 = "--re2" in sys.argv
    for name, a, txt, luiat in scan(tgt, re2):
        print("%-14s %08x: %-28s (lui @%08x)" % (name, a, txt, luiat))
