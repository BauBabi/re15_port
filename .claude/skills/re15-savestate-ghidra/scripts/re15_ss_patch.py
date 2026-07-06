#!/usr/bin/env python3
"""re15_ss_patch.py - patch PSX main-RAM in a DuckStation savestate + repack.

Decompresses container frame 0 (which holds RAM @0x80000000), applies u8/u16/u32
patches at absolute PSX addresses, recompresses frame 0, fixes the section-table
size/offset fields, and re-appends frame 1 unchanged. See SAVESTATE_CONTAINER_FORMAT.md.

Usage:
  python re15_ss_patch.py IN.sav OUT.sav ADDR:SIZE:VALUE [ADDR:SIZE:VALUE ...]
    e.g.  0x800acaee:2:77   (set player HP s16 to 77)
"""
import sys, struct, zstandard

ZSTD = b"\x28\xb5\x2f\xfd"
PSX_EXE = r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE"

def find_ram_base(blob):
    sig = open(PSX_EXE, "rb").read()[0x800:0x800 + 48]
    pos = blob.find(sig)
    if pos < 0:
        raise RuntimeError("PSX.EXE signature not found in frame0")
    return pos - 0x10000

def main():
    inp, outp = sys.argv[1], sys.argv[2]
    patches = []
    for spec in sys.argv[3:]:
        a, sz, val = spec.split(":")
        patches.append((int(a, 0), int(sz), int(val, 0)))

    data = open(inp, "rb").read()
    # Main PSX-RAM lives in FRAME 1 (@0xd4 off, @0xcc csize, @0xd0 dsize); it is the
    # LAST section (ends at EOF) -> patching it only updates the frame1 csize @0xcc.
    f1_off  = struct.unpack_from("<I", data, 0xd4)[0]   # 87299
    f1_csz  = struct.unpack_from("<I", data, 0xcc)[0]
    assert f1_off + f1_csz == len(data), "frame1 is not the final section"

    blob = bytearray(zstandard.ZstdDecompressor().stream_reader(data[f1_off:]).read())
    base = find_ram_base(blob)
    for addr, sz, val in patches:
        o = base + (addr - 0x80000000)
        fmt = {1: "<B", 2: "<h", 4: "<i"}[sz]
        old = struct.unpack_from(fmt, blob, o)[0]
        struct.pack_into(fmt, blob, o, val)
        print(f"  patched 0x{addr:08x} ({sz}B): {old} -> {val}")

    # recompress frame1 (embeds content-size in the zstd frame header)
    f1_new = zstandard.ZstdCompressor(level=12).compress(bytes(blob))
    n = len(f1_new)
    out = bytearray(data)
    struct.pack_into("<I", out, 0xcc, n)            # frame1 compressed size
    out = out[:f1_off] + f1_new                     # header+frame0 unchanged + new frame1
    open(outp, "wb").write(out)
    print(f"  repacked: frame1 csize {f1_csz} -> {n}  (frame1 dsize unchanged)")
    print(f"  wrote {outp} ({len(out)} bytes, was {len(data)})")

if __name__ == "__main__":
    main()
