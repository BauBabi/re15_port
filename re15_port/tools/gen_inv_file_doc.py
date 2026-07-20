#!/usr/bin/env python3
"""Generate engine/src/gen/inv_file_doc.inc — the byte-true FILE-tab data set
(FILE wave; DEBUG.BIN FUN_800c6ca0 screen).

Source: re15_port/shared_assets/PSX/BIN/DEBUG.BIN. The DEBUG.BIN module maps to RAM
@0x800c0000 (file offset = addr - 0x800c0000), byte-verified file==RAM in the wave-2/3
sessions (mzd_inv_open.sav).

Vendored tables (all addresses raw-MIPS cited in the FILE-wave session):
  - the embedded 7-page "Operation Report" document @0x800ccd34: u16 page count*2 +
    page offset table (7 u16, {0xe,0x27,0x12d,0x1ff,0x2fc,0x402,0x51a}) + text
    (reader drawer 0x800c7600: ptr = base + u16[base+page*2]; end = u16[base]>>1 = 7)
  - row visibility masks u16[3] @0x800c6c98 (lhu @0x800c72f0; shipped {1,0xffff,0xffff})
  - row name-bank base u8[3] @0x800c7370 (lbu @0x800c72f8; {0x48,0x52,0x5c})
  - row names ids 0x48..0x65 from the shared name bank (FUN_80028840 @0x80028844:
    ptr = 0x800c4a28 + u16[0x800c495c + id*2]; the FILE rows use ids base+row)
  - the 21-underscore hidden-row string @0x800c7916 (a3=0x30 print @0x800c731c)
  - page title strings, ptr table @0x800c78e4 -> {0x800c78f0 "Files",
    0x800c78f6 "S.T.A.R.S.", 0x800c7907 "Umbrella"} (FUN_800c727c a3=0x10)

Census asserts (this script FAILS if the shipped bytes differ from the RE'd decode):
masks/bases, title decode, underscore run, the 30 row-name decodes (with the byte-true
digraph expansion of FUN_80013160: (b+0xa0)&0xff<=0x58 -> pair @0x800c44b8+(b-0xa0)*2,
SIGNED index @0x800131c0-c4), and the document control census (only controls
0x08 newline / 0x09 center-u16 / 0x0a right-u16 + terminator 0x07; NO 0x05 palette,
NO digraph bytes — the engine emitter relies on this).
"""
import struct, sys, os

ROOT = os.path.join(os.path.dirname(__file__), "..")
SRC = os.path.join(ROOT, "shared_assets", "PSX", "BIN", "DEBUG.BIN")
OUT = os.path.join(ROOT, "engine", "src", "gen", "inv_file_doc.inc")

BASE = 0x800C0000
DOC = 0x800CCD34 - BASE          # document base (reader drawer 0x800c7614)
MASKS = 0x800C6C98 - BASE        # u16[3] row visibility masks
ROWBASE = 0x800C7370 - BASE      # u8[3] per-page name-bank base
NAME_OFF = 0x800C495C - BASE     # shared name bank (FUN_80028840)
NAME_BLOB = 0x800C4A28 - BASE
UNDERSCORE = 0x800C7916 - BASE   # 21 x 0x3b + 0x07
TITLE_TBL = 0x800C78E4 - BASE    # 3 pointers into DEBUG.BIN
DIGRAPH = 0x800C44B8 - BASE      # FUN_80013160 pair table (signed index!)

d = open(SRC, "rb").read()

def u16(off): return struct.unpack_from("<H", d, off)[0]

# ---- document -------------------------------------------------------------------
npages = u16(DOC) // 2
assert npages == 7, "page count %d != 7 (u16 @0x800ccd34 = 0x%x)" % (npages, u16(DOC))
offs = [u16(DOC + 2 * i) for i in range(npages)]
assert offs == [0xE, 0x27, 0x12D, 0x1FF, 0x2FC, 0x402, 0x51A], offs

# document extent: walk page 6 to its 0x07 terminator (pages end on 0x07; the
# reader prints until code 7/1 — FUN_80028ec4 @0x80028f88-90)
end = DOC + offs[-1]
ctrl_census = set()
p = DOC + offs[0]
# census over ALL pages: controls used + digraph-range bytes (0x60..0xb8)
for i in range(npages):
    p = DOC + offs[i]
    while d[p] != 0x07:
        b = d[p]
        if b == 0x09 or b == 0x0A:
            ctrl_census.add(b); p += 3          # u16 operand @0x80028ff4-9008
        elif b < 0x0C and b != 0x00:
            ctrl_census.add(b); p += 1
        else:
            assert not (0x60 <= b <= 0xB8), \
                "digraph byte 0x%02x in doc page %d (emitter census violated)" % (b, i)
            p += 1
    if i == npages - 1:
        end = p + 1                              # include the terminator
assert ctrl_census <= {0x08, 0x09, 0x0A}, \
    "unexpected doc controls %s (only 08/09/0a supported)" % ctrl_census
doc_blob = d[DOC:end]

# ---- masks / bases --------------------------------------------------------------
masks = [u16(MASKS + 2 * i) for i in range(3)]
assert masks == [0x0001, 0xFFFF, 0xFFFF], masks
bases = list(d[ROWBASE:ROWBASE + 3])
assert bases == [0x48, 0x52, 0x5C], bases

# ---- row names 0x48..0x65 (rebased into an own blob) ---------------------------
def name_bytes(idx):
    off = u16(NAME_OFF + idx * 2)
    p = NAME_BLOB + off
    q = p
    while d[q] != 0x07:
        q += 1
    return d[p:q + 1]

def decode(bs):
    """byte-true FUN_80013160 digraph expansion + glyph decode (chr = code+0x24)."""
    out = []
    pend = None
    i = 0
    while i < len(bs):
        if pend is not None:
            c, pend = pend, None
            i += 1                       # pending consumption advances (@0x80013180)
        else:
            b = bs[i]
            if ((b + 0xA0) & 0xFF) <= 0x58:      # digraph (sltiu 0x59 @0x8001319c)
                idx = (b - 0xA0) * 2             # SIGNED (@0x800131c0-c4)
                c = d[DIGRAPH + idx]
                pend = d[DIGRAPH + idx + 1]
                # ptr NOT advanced (@0x800131dc)
            else:
                c = b
                i += 1
        if c == 0x07:
            break
        out.append(c)
    return out

def as_text(codes):
    s = ""
    for c in codes:
        if c == 0x00: s += " "
        elif 0x0C <= c < 0x60: s += chr(c + 0x24)
        else: s += "<%02x>" % c
    return s

EXPECTED = [
    "Chris^ Diary", "Operation Report", "File 3", "File 4", "File 5", "File 6",
    "File 7", "File 8", "File 9", "File 10",
    "Albert Wesker", "Jill Valentine", "Chris Redfield", "Barry Burton",
    "Rebecca Chambers", "Brad Vickers", "Enrico Marini",
    # '&' = digraph 0x64 -> pair {0x09,0x08} (width-table bytes @0x800c4440 via the
    # SIGNED index -> control codes in the FUN_80028ec4 printer; the raw decode here
    # shows the code pair)
    "E{ Dewey <09><08> K{ Dooley", "J{ Frost <09><08> F{ Speyer", "K{ Sullivan <09><08> R{ Aiken",
    "Cerberus", "Mutated Baboon", "Mutated Alligator", "Gargantuagator",
    "Arachnophobia", "Giant Spiders", "Umbrella File 7", "Umbrella File 8",
    "Umbrella File 9", " ",
]
# NOTE on rows 0x59-0x5b: the digraph consumes the NEXT byte pair as the control-9
# u16 operand in FUN_80028ec4 (raw operand read @0x80028ff4-9008 desyncs the pending),
# so as_text above (plain expansion) intentionally differs from the on-screen result;
# the assert pins the CODE STREAM, the engine emitter reproduces the printer exactly.
names = [name_bytes(i) for i in range(0x48, 0x66)]
got = []
for nb in names:
    t = as_text(decode(nb))
    # '^' = glyph 0x3a (apostrophe), '{' = glyph 0x57 (period) in this font
    got.append(t)
assert got == EXPECTED, "name census mismatch:\n%s\nvs\n%s" % (got, EXPECTED)

name_off = []
name_blob = bytearray()
for nb in names:
    name_off.append(len(name_blob))
    name_blob += nb

# ---- underscores ----------------------------------------------------------------
us = d[UNDERSCORE:UNDERSCORE + 22]
assert us == b"\x3b" * 21 + b"\x07", us.hex()

# ---- titles ---------------------------------------------------------------------
tptr = [struct.unpack_from("<I", d, TITLE_TBL + 4 * i)[0] for i in range(3)]
assert tptr == [0x800C78F0, 0x800C78F6, 0x800C7907], ["%08x" % v for v in tptr]
titles = []
for tp in tptr:
    p = tp - BASE
    q = p
    while d[q] != 0x07:
        q += 1
    titles.append(d[p:q + 1])
assert [as_text(decode(t)) for t in titles] == \
    ["Files", "S{T{A{R{S{ Files", "Umbrella Files"], \
    [as_text(decode(t)) for t in titles]
title_off = []
title_blob = bytearray()
for t in titles:
    title_off.append(len(title_blob))
    title_blob += t

def carr(b):
    return ",".join("0x%02x" % x for x in b)

with open(OUT, "w") as f:
    f.write(
        "/* GENERATED by tools/gen_inv_file_doc.py from shared_assets/PSX/BIN/DEBUG.BIN\n"
        " * (module maps @0x800c0000; file==RAM byte-verified). DO NOT EDIT.\n"
        " * FILE-tab data (FUN_800c6ca0): document @0x800ccd34 (7 pages, offsets in-blob),\n"
        " * masks u16[3] @0x800c6c98, row bases u8[3] @0x800c7370, row names ids 0x48-0x65\n"
        " * (bank @0x800c495c/0x800c4a28), underscores @0x800c7916, titles @0x800c78e4.\n"
        " * All census-asserted against the RE'd decode at generation time. */\n")
    f.write("#define RE15_INV_FILEDOC_PAGES %d\n" % npages)
    f.write("static const unsigned short re15_inv_filedoc_off[%d] = {%s};\n"
            % (npages, ",".join("0x%x" % o for o in offs)))
    f.write("static const unsigned char re15_inv_filedoc_blob[%d] = {%s};\n"
            % (len(doc_blob), carr(doc_blob)))
    f.write("static const unsigned short re15_inv_file_mask[3] = {%s};\n"
            % ",".join("0x%04x" % m for m in masks))
    f.write("static const unsigned char re15_inv_file_rowbase[3] = {%s};\n"
            % ",".join("0x%02x" % b for b in bases))
    f.write("static const unsigned short re15_inv_file_name_off[%d] = {%s};\n"
            % (len(name_off), ",".join("0x%x" % o for o in name_off)))
    f.write("static const unsigned char re15_inv_file_name_blob[%d] = {%s};\n"
            % (len(name_blob), carr(name_blob)))
    f.write("static const unsigned char re15_inv_file_underscores[22] = {%s};\n" % carr(us))
    f.write("static const unsigned short re15_inv_file_title_off[3] = {%s};\n"
            % ",".join("0x%x" % o for o in title_off))
    f.write("static const unsigned char re15_inv_file_title_blob[%d] = {%s};\n"
            % (len(title_blob), carr(title_blob)))

print("wrote", OUT, "doc", len(doc_blob), "names", len(name_blob), "titles", len(title_blob))
