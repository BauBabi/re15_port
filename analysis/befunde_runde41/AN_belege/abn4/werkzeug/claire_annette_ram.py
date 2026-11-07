#!/usr/bin/env python3
"""claire_annette_ram.py - Abnahme 4 Spur AN, eigene Lesung (unabhaengig vom Autor-Werkzeug re2c_ss.py):
RE2 Claire PAL (SLES_109.72) DuckStation-Savestates lauf3 e01..e30 (Szene ROOM6141, Annette).
Liest je Bild: Claire = [0x800c79a8], Annette = [0x800c79b0] (Entity-Liste, Ende [0x800c5f0c]) und je Entity:
 +0x00 w0, +0x08 Id, +0x156 hp, +0x1E8 Segmentzahl, Segment 0 ab +0x84: x/y/z (+0/+4/+8), +0x14 (s16), +0x16 Radius, +0x1A Halbhoehe,
 Lage +0x38/+0x3c/+0x40 (wie im Autor-Werkzeug; hier gegen das Segment gegengeprueft: Segment-x/z == Lage-x/z).
DUCCT: Zustand zstd ab [0xd4], Laenge [0xcc]; RAM-Basis per Signatur SLES_109.72[0x800:0x830] (text @0x80010000).
Aufruf: python -I claire_annette_ram.py <lauf3-dir> <SLES_109.72>"""
import sys, struct, glob, os, zstandard
d0, exe = sys.argv[1], sys.argv[2]
sig = open(exe, "rb").read()[0x800:0x830]
t_addr = struct.unpack_from("<I", open(exe, "rb").read(), 0x18)[0]
print("SLES t_addr = 0x%08x" % t_addr)
for p in sorted(glob.glob(os.path.join(d0, "e*.sav"))):
    raw = open(p, "rb").read()
    assert raw[:5] == b"DUCCT"
    off = struct.unpack_from("<I", raw, 0xd4)[0]; csz = struct.unpack_from("<I", raw, 0xcc)[0]
    blob = zstandard.ZstdDecompressor().stream_reader(raw[off:off + csz]).read(1 << 23)
    pos = blob.find(sig); assert pos >= 0
    base = pos - (t_addr & 0x1fffff)
    def o(a): return base + (a & 0x1fffff)
    u8 = lambda a: blob[o(a)]
    s16 = lambda a: struct.unpack_from("<h", blob, o(a))[0]
    u16 = lambda a: struct.unpack_from("<H", blob, o(a))[0]
    s32 = lambda a: struct.unpack_from("<i", blob, o(a))[0]
    u32 = lambda a: struct.unpack_from("<I", blob, o(a))[0]
    room = None
    def ent(name, e):
        if e == 0: return name + " -"
        seg = (s32(e+0x84), s32(e+0x88), s32(e+0x8c), s16(e+0x84+0x14), u16(e+0x84+0x16), u16(e+0x84+0x1a))
        return ("%s @%08x id %02x w0 %08x hp %d nseg %d Lage (%d,%d,%d) Seg0 Mitte (%d,%d,%d) +14 %d r %d hh %d"
                % (name, e, u8(e+8), u32(e), s16(e+0x156), s32(e+0x1e8), s32(e+0x38), s32(e+0x3c), s32(e+0x40), *seg))
    cl = u32(0x800c79a8); an = u32(0x800c79b0)
    print(os.path.basename(p), "| Listenende [0x800c5f0c] = %08x |" % u32(0x800c5f0c), ent("Claire", cl), "|", ent("Annette", an))
