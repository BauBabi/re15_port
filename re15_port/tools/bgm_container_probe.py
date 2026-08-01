#!/usr/bin/env python3
"""Misst das Layout eines SOUND/*.BGM-Containers so, wie das ORIGINAL es liest.

Belege (RE1.5 PSX.EXE):
  FUN_80044564  @0x80044564  "MAIN BGM"  — liest VH-Offset aus u32 @size-8,
                                            VB-Offset aus u32 @size-12
  FUN_80044774  @0x80044774  "SUB BGM"   — liest SEQ#2-Offset aus u32 @size-8,
                                            VH-Offset  aus u32 @size-12,
                                            VB-Offset  aus u32 @size-16
Beide berechnen den VB-Anfang NICHT aus der VH-Groesse — sie lesen ihn aus dem
Trailer. Zwischen VH-Ende und VB liegen in den ausgelieferten Dateien 4 Byte Pad,
die eine Berechnung ueber (fsize - vb_total_bytes) verschluckt.

Aufruf:
  python re15_port/tools/bgm_container_probe.py re15_port/shared_assets/PSX/SOUND/MAIN32.BGM
  python re15_port/tools/bgm_container_probe.py            # alle SOUND/*.BGM
"""
import glob
import os
import struct
import sys

DEFAULT_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "shared_assets", "PSX", "SOUND")


def u32(d, off):
    return struct.unpack_from("<I", d, off)[0]


def probe(path):
    d = open(path, "rb").read()
    n = len(d)
    print("=" * 72)
    print("%s  size=%d (0x%X)" % (os.path.basename(path), n, n))
    if n < 0x40:
        # Es gibt Platzhalter-Container (z.B. MAIN3C.BGM = 4 Byte). Der Port faengt das
        # in re15_bgm_load_track mit "sz < 0x40" ab.
        print("  PLATZHALTER — zu klein fuer einen Container, uebersprungen")
        return True

    tail = [(n - k, u32(d, n - k)) for k in (20, 16, 12, 8, 4)]
    print("  Trailer:")
    for off, v in tail:
        print("    u32 @size-%-2d (file[%d]) = %d (0x%X)" % (n - off, off, v, v))

    is_sub = os.path.basename(path).upper().startswith("SUB")
    if is_sub:
        seq2_off, vh_off, vb_off = u32(d, n - 8), u32(d, n - 12), u32(d, n - 16)
        print("  Lesart FUN_80044774 (SUB): SEQ#2=%d  VH=%d  VB=%d" % (seq2_off, vh_off, vb_off))
    else:
        seq2_off, vh_off, vb_off = None, u32(d, n - 8), u32(d, n - 12)
        print("  Lesart FUN_80044564 (MAIN): VH=%d  VB=%d" % (vh_off, vb_off))

    ok = True
    if d[0:4] != b"pQES":
        print("  !! SEQ-Magic @0 ist %r, erwartet b'pQES'" % d[0:4]); ok = False
    if d[vh_off:vh_off + 4] != b"pBAV":
        print("  !! VH-Magic @%d ist %r, erwartet b'pBAV'" % (vh_off, d[vh_off:vh_off + 4])); ok = False
    if seq2_off is not None and d[seq2_off:seq2_off + 4] != b"pQES":
        print("  !! SEQ#2-Magic @%d ist %r" % (seq2_off, d[seq2_off:seq2_off + 4])); ok = False

    fsize = u32(d, vh_off + 0x0C)
    ps, ts, vs = struct.unpack_from("<HHH", d, vh_off + 0x12)
    vh_calc = 32 + 128 * 16 + 32 * 16 * ps + 512
    tbl_off = vh_off + 32 + 2048 + 32 * 16 * ps
    sizes = [t * 8 for t in struct.unpack_from("<256H", d, tbl_off)[1:vs + 1]]

    print("  VabHdr @%d: fsize=%d ps=%d ts=%d vs=%d" % (vh_off, fsize, ps, ts, vs))
    print("  VH-Formel 32+2048+512*ps+512 = %d" % vh_calc)
    print("  VH-Spanne laut Trailer (VB-VH) = %d  ->  PAD = %d Byte" %
          (vb_off - vh_off, vb_off - vh_off - vh_calc))
    print("  VAG-Groessen (%d) = %s   Summe = %d" % (vs, sizes, sum(sizes)))
    print("  Probe fsize == VH + Summe(VAG): %d == %d  -> %s" %
          (fsize, vh_calc + sum(sizes), fsize == vh_calc + sum(sizes)))
    print("  VB-Ende = %d, Datei = %d  -> %d Byte Trailer" %
          (vb_off + sum(sizes), n, n - (vb_off + sum(sizes))))

    berechnet = vh_off + vh_calc
    print("  >> ORIGINAL VB-Anfang = %d   |   berechnet (vh+vh_size) = %d   |   Differenz = %d" %
          (vb_off, berechnet, vb_off - berechnet))
    return ok


if __name__ == "__main__":
    args = sys.argv[1:] or sorted(glob.glob(os.path.join(DEFAULT_DIR, "*.BGM")))
    if not args:
        sys.exit("keine .BGM gefunden — Pfad angeben")
    for p in args:
        probe(p)
