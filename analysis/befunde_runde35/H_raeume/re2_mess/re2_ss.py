"""re2_ss.py — RE2-Retail-Leon-RAM aus einem DuckStation-Savestate (neues Format "DUCCT": Frame 1 =
Vorschaubild 128 KiB, Frame 2 = Systemzustand mit der PSX-RAM). RAM-Basis per Code-Signatur von
info/re2leon/PSX.EXE (FUN_800157d4, 32 Bytes) -> zugleich der Beleg, dass die Disc dieselbe EXE faehrt.
Runde 35 Spur H, Nachbesserung 3."""
import struct, sys, os, zstandard

HERE = os.path.dirname(os.path.abspath(__file__))
EXE = open(os.path.join(HERE, "..", "..", "..", "..", "info", "re2leon", "PSX.EXE"), "rb").read()
SIG_ADDR = 0x800157D4


def exe_bytes(addr, n):
    o = addr - 0x80010000 + 0x800
    return EXE[o:o + n]


def load(path):
    d = open(path, "rb").read()
    sig = exe_bytes(SIG_ADDR, 32)
    i = 0
    while True:
        i = d.find(b"\x28\xb5\x2f\xfd", i)
        if i < 0:
            return None
        try:
            blob = zstandard.ZstdDecompressor().decompressobj().decompress(d[i:])
            pos = blob.find(sig)
            if pos >= 0:
                return Ram(blob, pos - (SIG_ADDR & 0x1FFFFF))
        except Exception:
            pass
        i += 4


class Ram:
    def __init__(s, b, base):
        s.b, s.base = b, base

    def bytes(s, a, n): o = s.base + (a & 0x1FFFFF); return s.b[o:o + n]
    def u8(s, a): return s.b[s.base + (a & 0x1FFFFF)]
    def u16(s, a): return struct.unpack_from("<H", s.b, s.base + (a & 0x1FFFFF))[0]
    def s16(s, a): return struct.unpack_from("<h", s.b, s.base + (a & 0x1FFFFF))[0]
    def u32(s, a): return struct.unpack_from("<I", s.b, s.base + (a & 0x1FFFFF))[0]
    def s32(s, a): return struct.unpack_from("<i", s.b, s.base + (a & 0x1FFFFF))[0]

    def code_match(s, a0=0x80014000, a1=0x80050000):
        same = sum(1 for a in range(a0, a1, 4) if s.bytes(a, 4) == exe_bytes(a, 4))
        return same, (a1 - a0) // 4


PL = 0x800CFBF8
if __name__ == "__main__":
    for p in sys.argv[1:]:
        r = load(p)
        if r is None:
            print(os.path.basename(p), "keine RAM gefunden"); continue
        same, n = r.code_match()
        print(os.path.basename(p), "code %d/%d" % (same, n), "stage", r.s16(0x800D481C), "room", r.s16(0x800D481E),
              "cut", r.u8(0x800CFBF2), "PL", (r.s32(PL + 0x38), r.s32(PL + 0x3C), r.s32(PL + 0x40)),
              "yaw", r.s16(PL + 0x76), "w4 %08x" % r.u32(PL + 4), "mode", r.u8(0x800DF348))


def vram_png(ram, path, x0=0, y0=0, w=1024, h=512, scale=1):
    """PSX-VRAM (16bpp 1555) ab dem Tag "GPU-VRAM" (+8) als PNG; x0/y0/w/h schneiden aus."""
    from PIL import Image
    pos = ram.b.find(b"GPU-VRAM")
    if pos < 0:
        return False
    o = pos + 8
    im = Image.new("RGB", (w, h)); px = im.load()
    for y in range(h):
        row = o + ((y0 + y) * 1024 + x0) * 2
        for x in range(w):
            v = ram.b[row + 2 * x] | (ram.b[row + 2 * x + 1] << 8)
            px[x, y] = ((v & 0x1F) << 3, ((v >> 5) & 0x1F) << 3, ((v >> 10) & 0x1F) << 3)
    if scale != 1:
        im = im.resize((w * scale, h * scale), Image.NEAREST)
    im.save(path)
    return True
