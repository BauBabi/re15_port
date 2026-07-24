#!/usr/bin/env python3
"""re15_ss.py - DuckStation savestate decoder for RE1.5 (MZD disc).

Decompresses a DuckStation .sav (DUCCS + zstd), locates the PSX main-RAM base
inside the decompressed blob via a PSX.EXE code signature, and exposes RAM
read helpers + a VRAM->PNG dump. Stdlib + `zstandard` only (no Pillow).
"""
import sys, struct, zlib, os

PSX_EXE = r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE"
ZSTD_MAGIC = b"\x28\xb5\x2f\xfd"

def decompress_sav(path):
    """Return the decompressed savestate blob (~4 MB)."""
    import zstandard
    data = open(path, "rb").read()
    off = data.find(ZSTD_MAGIC, 8)
    if off < 0:
        raise RuntimeError("no zstd frame magic in %s" % path)
    dctx = zstandard.ZstdDecompressor()
    with dctx.stream_reader(data[off:]) as r:
        return r.read()

def find_ram_base(blob):
    """Find offset in blob that corresponds to RAM 0x80000000.

    The PSX.EXE has a 0x800-byte header; the first code bytes load at
    0x80010000. We locate that 48-byte signature in the blob; base is the
    match position minus 0x10000.
    """
    sig = open(PSX_EXE, "rb").read()[0x800:0x800 + 48]
    pos = blob.find(sig)
    if pos < 0:
        raise RuntimeError("PSX.EXE signature not found in savestate RAM")
    return pos - 0x10000  # offset of RAM 0x80000000

def find_vram_base(blob):
    """Find the blob offset of PSX VRAM pixel (0,0). BYTE-TRUE.

    A DuckStation savestate is NOT [RAM][VRAM] contiguous: after the 2 MB main
    RAM comes a GPU component whose register state precedes the framebuffer, so
    VRAM does NOT start at ram_base+0x200000. It starts right after the length-
    prefixed ASCII tag "GPU-VRAM" (u32 len=8 + the 8 chars). Using ram_base+
    0x200000 reads 886 bytes (= 443 px) too early -> every VRAM pixel is X-shifted
    by 443 and lands in GPU register state (this silently corrupted CLUT/texture
    reads for months: it made shadow CLUTs read empty and 4bpp textures read as a
    uniform-0xB pattern; verified 2026-07-24, savestate-source wf_08509e8a). Cross-
    checked: at this base the shadow CLUT (272,503) matches TEX.TIM byte-for-byte.
    Falls back to ram_base+0x200000 if the tag is absent (older/other formats)."""
    pos = blob.find(b"GPU-VRAM")
    if pos < 0:
        return None
    return pos + 8

class Ram:
    def __init__(self, path):
        self.blob = decompress_sav(path)
        self.base = find_ram_base(self.blob)
        # BYTE-TRUE VRAM base (GPU-VRAM tag). None -> use the legacy shifted offset.
        self.vram_base = find_vram_base(self.blob)
    def _o(self, addr):
        return self.base + (addr - 0x80000000)
    def u8(self, a):  return self.blob[self._o(a)]
    def s8(self, a):  return struct.unpack_from("<b", self.blob, self._o(a))[0]
    def u16(self, a): return struct.unpack_from("<H", self.blob, self._o(a))[0]
    def s16(self, a): return struct.unpack_from("<h", self.blob, self._o(a))[0]
    def u32(self, a): return struct.unpack_from("<I", self.blob, self._o(a))[0]
    def s32(self, a): return struct.unpack_from("<i", self.blob, self._o(a))[0]
    def bytes(self, a, n): o = self._o(a); return self.blob[o:o+n]
    def vpix(self, x, y):
        """BYTE-TRUE 16-bit VRAM halfword at pixel (x,y) in the 1024x512 VRAM.
        Use THIS (not ram_base+0x200000) for any CLUT/texture/framebuffer read."""
        vb = self.vram_base if self.vram_base is not None else self.base + 0x200000
        return struct.unpack_from("<H", self.blob, vb + (y * 1024 + x) * 2)[0]

def write_png(path, w, h, rgb):
    def chunk(typ, data):
        return (struct.pack(">I", len(data)) + typ + data +
                struct.pack(">I", zlib.crc32(typ + data) & 0xffffffff))
    raw = bytearray()
    for y in range(h):
        raw.append(0)
        raw += rgb[y*w*3:(y+1)*w*3]
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 6))
    png += chunk(b"IEND", b"")
    open(path, "wb").write(png)

def vram_png(ram, path, vram_off=0x200000, w=1024, h=512, true_base=False):
    """Decode the full PSX VRAM (16bpp 1555) to an RGB PNG.

    ⚠️ DEFAULT is the LEGACY offset ram.base+0x200000, which is X-shifted by +443 px
    vs true VRAM (see find_vram_base). It is kept as the default ONLY so the display
    crops already tuned to it (this file's x0=448 detection; parity_net's x=440 crop)
    keep working. For a BYTE-TRUE VRAM image (correct texture/CLUT/framebuffer
    coordinates) pass true_base=True, or read pixels via ram.vpix(x,y)."""
    if true_base and ram.vram_base is not None:
        o = ram.vram_base
    else:
        o = ram.base + vram_off
    blob = ram.blob
    rgb = bytearray(w*h*3)
    for i in range(w*h):
        px = blob[o + i*2] | (blob[o + i*2 + 1] << 8)
        rgb[i*3+0] = (px & 0x1f) << 3
        rgb[i*3+1] = ((px >> 5) & 0x1f) << 3
        rgb[i*3+2] = ((px >> 10) & 0x1f) << 3
    write_png(path, w, h, bytes(rgb))

def display_nonblack_fraction(ram, x0=448, y0=16, x1=760, y1=224,
                              vram_off=0x200000, w=1024):
    """Fraction of non-black pixels in the typical game display rect. A loaded
    room is mostly non-black (>0.5); a savestate captured mid-load is black
    (<0.05). Use to auto-detect a too-early capture."""
    o = ram.base + vram_off
    blob = ram.blob
    nonblack = total = 0
    for y in range(y0, y1):
        row = o + y*w*2
        for x in range(x0, x1):
            px = blob[row + x*2] | (blob[row + x*2 + 1] << 8)
            total += 1
            if px & 0x7fff:
                nonblack += 1
    return nonblack / total if total else 0.0

if __name__ == "__main__":
    sav = sys.argv[1]
    r = Ram(sav)
    print("file        :", os.path.basename(sav))
    print("blob bytes  :", len(r.blob))
    print("RAM base off : 0x%x" % r.base)
    print("display      : %.0f%% non-black (room loaded if >50%%)"
          % (100 * display_nonblack_fraction(r)))
    # Known RE1.5 globals (from reai-v2 memory / CLAUDE.md) for live-state sanity:
    print("DAT_800aca3c (display/freeze flags): 0x%x" % r.u32(0x800aca3c))
    print("DAT_800b5568 (fade level 0..0xf0)  : 0x%x" % r.u8(0x800b5568))
    print("DAT_800acaee (player HP)           : %d"   % r.s16(0x800acaee))
    print("DAT_800aca58 (player state)        : %d"   % r.u8(0x800aca58))
    print("DAT_800b0fe4 (cut/scenario word)   : 0x%x" % r.u32(0x800b0fe4))
    if len(sys.argv) > 2:
        vram_png(r, sys.argv[2])
        print("VRAM PNG    :", sys.argv[2])
