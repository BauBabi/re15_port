"""re2c_ss.py - RE2 Claire PAL (SLES-10972, EXE SLES_109.72) DuckStation-Savestate (DUCCT) lesen/patchen + Vorschaubild.
Kopf: @0xb8 Bildbreite, @0xbc Bildhoehe, @0xc0 Bild-zstd-Groesse, @0xc4 Bild-Offset (RGBA),
      @0xcc Zustand-zstd-Groesse, @0xd4 Zustand-Offset (Hauptspeicher darin; Basis per EXE-Signatur [0x800:0x830]).
  python re2c_ss.py SAV [PNG]"""
import struct, zstandard, sys

EXE = r"C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\an\cdisc\SLES_109.72"


class SS:
    def __init__(self, path):
        self.path = path
        self.data = open(path, "rb").read()
        assert self.data[:5] == b"DUCCT", self.data[:8]
        self.f1_off = struct.unpack_from("<I", self.data, 0xd4)[0]
        self.f1_csz = struct.unpack_from("<I", self.data, 0xcc)[0]
        assert self.f1_off + self.f1_csz == len(self.data)
        self.blob = bytearray(zstandard.ZstdDecompressor().stream_reader(self.data[self.f1_off:]).read())
        sig = open(EXE, "rb").read()[0x800:0x800 + 48]
        pos = self.blob.find(sig)
        assert pos >= 0, "SLES-Signatur nicht gefunden (andere EXE?)"
        self.base = pos - 0x10000

    def o(self, a): return self.base + (a & 0x1fffff)
    def u8(self, a): return self.blob[self.o(a)]
    def s8(self, a): return struct.unpack_from("<b", self.blob, self.o(a))[0]
    def u16(self, a): return struct.unpack_from("<H", self.blob, self.o(a))[0]
    def s16(self, a): return struct.unpack_from("<h", self.blob, self.o(a))[0]
    def u32(self, a): return struct.unpack_from("<I", self.blob, self.o(a))[0]
    def s32(self, a): return struct.unpack_from("<i", self.blob, self.o(a))[0]
    def bytes(self, a, n): return bytes(self.blob[self.o(a):self.o(a) + n])
    def ram(self): return bytes(self.blob[self.base:self.base + 0x200000])
    def w8(self, a, v): self.blob[self.o(a)] = v & 0xff
    def w16(self, a, v): struct.pack_into("<H", self.blob, self.o(a), v & 0xffff)
    def w32(self, a, v): struct.pack_into("<I", self.blob, self.o(a), v & 0xffffffff)
    def wbytes(self, a, b): self.blob[self.o(a):self.o(a) + len(b)] = b

    def shot(self, png):
        from PIL import Image
        w, h, csz, off = struct.unpack_from("<IIII", self.data, 0xb8)
        px = zstandard.ZstdDecompressor().stream_reader(self.data[off:off + csz]).read()
        Image.frombytes("RGBA", (w, h), px[:w * h * 4]).convert("RGB").save(png)

    def save(self, out):
        f1 = zstandard.ZstdCompressor(level=9).compress(bytes(self.blob))
        o = bytearray(self.data)
        struct.pack_into("<I", o, 0xcc, len(f1))
        o = o[:self.f1_off] + f1
        open(out, "wb").write(o)


if __name__ == "__main__":
    s = SS(sys.argv[1])
    print("RAM-Basis im Zustand: 0x%x" % s.base)
    if len(sys.argv) > 2:
        s.shot(sys.argv[2]); print("Bild ->", sys.argv[2])
