# Abnahme 5 G5A: eigener RE2-RAM-Leser fuer DuckStation-Savestates (zstd ab der ersten Magie 28 B5 2F FD;
# RAM-Basis ueber SLUS_007.48[0x800:0x840] = Code bei 0x80010000). ram(i) liefert 2 MB RAM als bytes.
import struct, zstandard, functools
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
EXE = r"C:/workspace/git/reAi_v2/info/re2leon/SLUS_007.48"
@functools.lru_cache(None)
def ram(i):
    raw = open(SN % i, 'rb').read()
    k = raw.find(b'\x28\xb5\x2f\xfd')
    d = zstandard.ZstdDecompressor().stream_reader(raw[k:]).read()
    sig = open(EXE, 'rb').read()[0x800:0x840]
    p = d.find(sig)
    assert p >= 0x10000, 'RAM-Signatur nicht gefunden'
    return d[p - 0x10000: p - 0x10000 + 0x200000]
def u8(r, a): return r[a & 0x1FFFFF]
def u16(r, a): return struct.unpack_from('<H', r, a & 0x1FFFFF)[0]
def s16(r, a): return struct.unpack_from('<h', r, a & 0x1FFFFF)[0]
def u32(r, a): return struct.unpack_from('<I', r, a & 0x1FFFFF)[0]
def s32(r, a): return struct.unpack_from('<i', r, a & 0x1FFFFF)[0]
