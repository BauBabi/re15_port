#!/usr/bin/env python3
"""claire_seg_abn5.py - Abnahme 5 Spur AN, eigene Lesung des RE2-Koerpersegments im Claire-RAM (SLES_109.72, DuckStation lauf3).
Unabhaengig von claire_annette_ram.py (Abnahme 4): RAM-Basis per Suche des EXE-Textanfangs (SLES[0x800:0x900]) im entpackten
Zustand; Entity-Liste ab 0x800c79a8 bis [0x800c5f0c] (Listenende); je Entity: Wort 0, Id +0x08, hp +0x156 (s16),
+0x1E8 Segmentzahl, Segment 0 ab +0x84: x/y/z (s32 +0/+4/+8), +0x14 (s16), +0x16 Radius (u16), +0x1A Halbhoehe (u16).
Die Feldlage stammt aus FUN_80034D0C (Leon PSX.EXE) @0x80034d74 (+0x1E8), @0x80034d78 (+0x84), @0x80034dac (+0x16), @0x80034e5c (+0x1A).
Aufruf: python -I claire_seg_abn5.py <lauf3-Ordner> <SLES_109.72>"""
import sys, os, glob, struct
import zstandard
d0, exe = sys.argv[1], sys.argv[2]
ex = open(exe, "rb").read()
t_addr = struct.unpack_from("<I", ex, 0x18)[0]
sig = ex[0x800:0x900]
print("SLES_109.72: %d Byte, t_addr 0x%08x" % (len(ex), t_addr))
for p in sorted(glob.glob(os.path.join(d0, "e*.sav"))):
    raw = open(p, "rb").read()
    if raw[:5] != b"DUCCT":
        print(os.path.basename(p), "kein DUCCT"); continue
    off = struct.unpack_from("<I", raw, 0xd4)[0]; ln = struct.unpack_from("<I", raw, 0xcc)[0]
    blob = zstandard.ZstdDecompressor().stream_reader(raw[off:off+ln]).read(1 << 23)
    pos = blob.find(sig)
    if pos < 0:
        print(os.path.basename(p), "Signatur nicht gefunden"); continue
    base = pos - (t_addr & 0x1fffff)
    def a(x): return base + (x & 0x1fffff)
    u32 = lambda x: struct.unpack_from("<I", blob, a(x))[0]
    s32 = lambda x: struct.unpack_from("<i", blob, a(x))[0]
    s16 = lambda x: struct.unpack_from("<h", blob, a(x))[0]
    u16 = lambda x: struct.unpack_from("<H", blob, a(x))[0]
    ende = u32(0x800c5f0c)
    zeilen = []
    slot = 0x800c79a8
    while slot < ende and slot < 0x800c79a8 + 0x40:
        e = u32(slot)
        if e:
            zeilen.append("[%08x]=%08x id %02x w0 %08x hp %d nseg %d Lage (%d,%d,%d) S0 (%d,%d,%d) +14 %d r %d hh %d" % (
                slot, e, blob[a(e + 8)], u32(e), s16(e + 0x156), s32(e + 0x1e8), s32(e + 0x38), s32(e + 0x3c), s32(e + 0x40),
                s32(e + 0x84), s32(e + 0x88), s32(e + 0x8c), s16(e + 0x98), u16(e + 0x9a), u16(e + 0x9e)))
        slot += 4
    print(os.path.basename(p), "Ende %08x |" % ende, " | ".join(zeilen))
