#!/usr/bin/env python3
"""gp_patch.py - eigener RAM-Patcher der Gegenpruefung fuer DuckStation-Saves (DUCCS/DUCCT).

Frame 1 (Offset @0xd4, csize @0xcc) ist die letzte Sektion und enthaelt den RAM; Patch -> neu
komprimieren -> csize @0xcc setzen -> Datei = Kopf+Frame0 + neuer Frame1. Werte UNSIGNED.
Aufruf: gp_patch.py IN.sav OUT.sav ADDR:SIZE:VALUE [...]
Danach Selbstkontrolle mit gp_ss.SS (liest den gepatchten Wert zurueck, prueft CPU-Block unveraendert).
"""
import sys, os, struct
import zstandard
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gp_ss import SS


def main():
    inp, outp = sys.argv[1], sys.argv[2]
    specs = []
    for s in sys.argv[3:]:
        a, n, v = s.split(":")
        specs.append((int(a, 0), int(n), int(v, 0)))
    src = SS(inp)
    data = open(inp, "rb").read()
    off1 = struct.unpack_from("<I", data, 0xd4)[0]
    c1 = struct.unpack_from("<I", data, 0xcc)[0]
    assert off1 + c1 == len(data), "Frame 1 ist nicht die letzte Sektion"
    blob = bytearray(zstandard.ZstdDecompressor().decompressobj().decompress(data[off1:off1 + c1]))
    assert bytes(blob) == src.blob
    for a, n, v in specs:
        o = src.base + (a & 0x1fffffff)
        fmt = {1: "<B", 2: "<H", 4: "<I"}[n]
        old = struct.unpack_from(fmt, blob, o)[0]
        struct.pack_into(fmt, blob, o, v)
        print("  %08x (%dB): %x -> %x" % (a, n, old, v))
    f1 = zstandard.ZstdCompressor(level=12).compress(bytes(blob))
    out = bytearray(data[:off1])
    struct.pack_into("<I", out, 0xcc, len(f1))
    out += f1
    open(outp, "wb").write(out)
    chk = SS(outp)
    for a, n, v in specs:
        got = {1: chk.u8, 2: chk.u16, 4: chk.u32}[n](a)
        assert got == v, "Rueckleseprobe falsch @%08x" % a
    assert chk.r == src.r and chk.c == src.c and chk.scratch == src.scratch, "CPU-Block veraendert"
    diff = sum(1 for x, y in zip(chk.mem(0x80000000, 0x200000), src.mem(0x80000000, 0x200000)) if x != y)
    print("  geschrieben %s; RAM-Abweichungen gegen Quelle: %d Byte; CPU-Block unveraendert" % (outp, diff))


if __name__ == "__main__":
    main()
