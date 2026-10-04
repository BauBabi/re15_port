"""re2_frames.py — liest frames.bin aus re2_gdb_grab.py (Satz 'R2G1' n halter | PL 0x248 | PL-Parts 16x0xAC |
Halter 0x248 | Halter-Parts 16x0xAC). Part: Welt-Matrix +0x48 (m[3][3] s16, Q12), Translation +0x5C/+0x60/+0x64 s32."""
import struct
PART_N, PART_SZ, ENT_SZ = 16, 0xAC, 0x248
REC = 12 + ENT_SZ + PART_N * PART_SZ + ENT_SZ + PART_N * PART_SZ


def part(blob, k):
    o = k * PART_SZ
    m = struct.unpack_from("<9h", blob, o + 0x48)
    t = struct.unpack_from("<3i", blob, o + 0x5C)
    return m, t


def frames(path):
    d = open(path, "rb").read()
    out = []
    for i in range(0, len(d) - REC + 1, REC):
        assert d[i:i + 4] == b"R2G1"
        n, h = struct.unpack_from("<II", d, i + 4)
        o = i + 12
        pl = d[o:o + ENT_SZ]; o += ENT_SZ
        lp = d[o:o + PART_N * PART_SZ]; o += PART_N * PART_SZ
        he = d[o:o + ENT_SZ]; o += ENT_SZ
        hp = d[o:o + PART_N * PART_SZ]
        out.append(dict(n=n, holder=h, pl=pl, lparts=lp, hent=he, hparts=hp))
    return out


def ent_pos(e): return struct.unpack_from("<3i", e, 0x38)
def ent_yaw(e): return struct.unpack_from("<h", e, 0x76)[0]
def ent_cw(e): return struct.unpack_from("<I", e, 0x14C)[0]
def ent_f10e(e): return struct.unpack_from("<H", e, 0x10E)[0]
