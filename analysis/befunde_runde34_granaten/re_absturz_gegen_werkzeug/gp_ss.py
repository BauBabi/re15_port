#!/usr/bin/env python3
"""gp_ss.py - EIGENER DuckStation-Savestate-Leser der Gegenpruefung (unabhaengig von
re15_ss.py / ss_cpu.py des Ermittlers).

Container: Magic DUCCS/DUCCT, @0xc0 csize0, @0xc4 off0, @0xcc csize1, @0xd0 dsize1, @0xd4 off1.
Frame 1 = System-Zustand (Marker als u32-Laenge + ASCII: "System", "CPU", "Bus", ...).
RAM-Basis: Fundstelle der PSX.EXE-Textbytes [0x800:0x840] minus 0x10000 (=RAM 0x80000000);
Gegenprobe mit einer zweiten, weit entfernten Signatur (EXE @0x80012d60, 64 Byte).
CPU-Block: Marker "CPU" (u32 3 + 'CPU'), danach 4 u32 Takt, r[0..31], hi, lo, pc, npc,
BPC, BDA, TAR, BadVaddr, BDAM, BPCM, EPC, PRID, SR, CAUSE, DCIC, next_instr, cur_instr,
cur_instr_pc, 6 x bool, u8 ld_reg, u32 ld_val, u8 next_ld_reg, u32 next_ld_val,
u32 cache_control, 1024 B Scratchpad.  Plausibilitaet wird geprueft (r0 == 0,
ld_reg <= 34, cache_control), sonst Abbruch.
"""
import struct, sys, os
import zstandard

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
EXE = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
EXE_TADDR = struct.unpack_from("<I", EXE, 0x18)[0]
EXE_TSIZE = struct.unpack_from("<I", EXE, 0x1c)[0]
REGN = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra","hi","lo"]


class SS:
    def __init__(self, path):
        d = open(path, "rb").read()
        self.path = path
        self.magic = d[:5]
        self.title = d[8:0x88].split(b"\0")[0].decode("latin1")
        self.hash = d[0x88:0xb8].split(b"\0")[0].decode("latin1")
        self.media = d[0xd8:0x12c].split(b"\0")[0].decode("latin1", "replace")
        off1 = struct.unpack_from("<I", d, 0xd4)[0]
        c1 = struct.unpack_from("<I", d, 0xcc)[0]
        self.blob = zstandard.ZstdDecompressor().decompressobj().decompress(d[off1:off1 + c1])
        sig = EXE[0x800:0x840]
        p = self.blob.find(sig)
        assert p >= 0, "EXE-Signatur 1 nicht gefunden"
        self.base = p - (EXE_TADDR - 0x80000000)
        # Gegenprobe: zweite Signatur (Resolver 0x80012d60)
        fo = 0x800 + (0x80012d60 - EXE_TADDR)
        sig2 = EXE[fo:fo + 64]
        assert self.blob[self._o(0x80012d60):self._o(0x80012d60) + 64] == sig2, "Signatur 2 passt nicht"
        self._cpu()

    def _o(self, a):
        return self.base + ((a & 0x1fffffff) - 0x0)

    def u8(self, a): return self.blob[self._o(a)]
    def s8(self, a): return struct.unpack_from("<b", self.blob, self._o(a))[0]
    def u16(self, a): return struct.unpack_from("<H", self.blob, self._o(a))[0]
    def s16(self, a): return struct.unpack_from("<h", self.blob, self._o(a))[0]
    def u32(self, a): return struct.unpack_from("<I", self.blob, self._o(a))[0]
    def mem(self, a, n): return self.blob[self._o(a):self._o(a) + n]

    def _cpu(self):
        b = self.blob
        i = b.find(b"\x03\x00\x00\x00CPU")
        assert 0 <= i < 0x400, "CPU-Marker nicht am Anfang"
        o = i + 7
        w = lambda k: struct.unpack_from("<I", b, o + 4 * k)[0]
        self.clock = [w(k) for k in range(4)]
        self.r = [w(4 + k) for k in range(34)]
        names = ["pc", "npc", "BPC", "BDA", "TAR", "BadVaddr", "BDAM", "BPCM", "EPC", "PRID",
                 "SR", "CAUSE", "DCIC", "next_instr", "cur_instr", "cur_instr_pc"]
        self.c = {n: w(38 + k) for k, n in enumerate(names)}
        q = o + 4 * (38 + len(names))
        self.bools = list(b[q:q + 6]); q += 6
        self.ld_reg = b[q]; self.ld_val = struct.unpack_from("<I", b, q + 1)[0]; q += 5
        self.nld_reg = b[q]; self.nld_val = struct.unpack_from("<I", b, q + 1)[0]; q += 5
        self.cache_control = struct.unpack_from("<I", b, q)[0]; q += 4
        self.scratch = b[q:q + 1024]
        assert self.r[0] == 0, "r0 != 0 -> Layout falsch"
        assert self.ld_reg <= 34 and self.nld_reg <= 34, "Ladeverzoegerung unplausibel -> Layout falsch"

    def sp_u32(self, a):
        assert 0x1f800000 <= a < 0x1f800400
        return struct.unpack_from("<I", self.scratch, a - 0x1f800000)[0]

    def reg(self, n): return self.r[REGN.index(n)]


def dump_cpu(s):
    print("datei   :", s.path)
    print("magic   :", s.magic, "titel:", s.title, "|", s.hash)
    print("medium  :", s.media)
    print("RAM-Basis im Blob: 0x%x" % s.base)
    for k in range(0, 34, 4):
        print("  " + "  ".join("%-4s=%08x" % (REGN[j], s.r[j]) for j in range(k, min(k + 4, 34))))
    print("  " + "  ".join("%s=%08x" % (k, v) for k, v in s.c.items()))
    print("  bools=%s ld_reg=%d ld_val=%08x nld_reg=%d cache_control=%08x" % (s.bools, s.ld_reg, s.ld_val, s.nld_reg, s.cache_control))


if __name__ == "__main__":
    s = SS(sys.argv[1])
    dump_cpu(s)
    sp = s.reg("sp")
    if 0x1f800000 <= sp < 0x1f800400:
        print("Scratchpad-Stapel ab sp:")
        a = sp
        while a < 0x1f800400:
            print("  %08x: %s" % (a, " ".join("%08x" % s.sp_u32(a + 4 * k) for k in range(4) if a + 4 * k < 0x1f800400)))
            a += 16
