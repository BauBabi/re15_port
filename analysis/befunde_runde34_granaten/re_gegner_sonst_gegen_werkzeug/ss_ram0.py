#!/usr/bin/env python3
"""Gegenpruefung R34: RAM 0x80000000..0x8000001f aus Savestates lesen und als MIPS dekodieren.
Zweck: Ziel eines `jalr v0` mit v0 = 0 (Birkin-/Writher-NULL-Zeilen). KUSEG 0x00000000 = physisch 0 =
RAM-Spiegel von 0x80000000. Ergebnis im Spiel: sra zero,zero,0 / addiu k0,k0,3200 / jr zero / nop
= Endlosschleife 0x0..0xc (im Titel-Savestate steht an 0x8 `jr k0`).

Aufruf: python ss_ram0.py <savestate.sav> [...]
"""
import sys, struct, importlib.util

sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram  # noqa: E402

_spec = importlib.util.spec_from_file_location(
    "d", r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts\re15_disasm.py")
d = importlib.util.module_from_spec(_spec)
_argv = sys.argv
sys.argv = ["x"]
_spec.loader.exec_module(d)
sys.argv = _argv


def rd(ram, a, n):
    o = ram.base + (a - 0x80000000)
    return ram.blob[o:o + n]


def main():
    for p in sys.argv[1:]:
        ram = Ram(p)
        print(p)
        for i, w in enumerate(struct.unpack("<8I", rd(ram, 0x80000000, 32))):
            txt, _ = d.dis_one(w, i * 4)
            print("   %08x: %08x  %s" % (i * 4, w, txt))


if __name__ == "__main__":
    main()
