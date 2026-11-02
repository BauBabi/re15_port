"""re2ss.py - RE2 (SLUS-007.48) DuckStation-Savestate lesen/patchen (DUCCT, Hauptspeicher in Frame 1 @0xd4)."""
import struct, zstandard

EXE = r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48"


class SS:
    def __init__(self, path):
        self.path = path
        self.data = open(path, "rb").read()
        self.f1_off = struct.unpack_from("<I", self.data, 0xd4)[0]
        self.f1_csz = struct.unpack_from("<I", self.data, 0xcc)[0]
        assert self.f1_off + self.f1_csz == len(self.data)
        self.blob = bytearray(zstandard.ZstdDecompressor().stream_reader(self.data[self.f1_off:]).read())
        sig = open(EXE, "rb").read()[0x800:0x800 + 48]
        pos = self.blob.find(sig)
        assert pos >= 0, "SLUS signature not found"
        self.base = pos - 0x10000

    def o(self, a):
        return self.base + (a & 0x1fffff)

    def u8(self, a): return self.blob[self.o(a)]
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

    def save(self, out):
        f1 = zstandard.ZstdCompressor(level=9).compress(bytes(self.blob))
        o = bytearray(self.data)
        struct.pack_into("<I", o, 0xcc, len(f1))
        o = o[:self.f1_off] + f1
        open(out, "wb").write(o)
