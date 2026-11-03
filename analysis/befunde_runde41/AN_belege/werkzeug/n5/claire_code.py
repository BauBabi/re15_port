"""Sucht Leon FUN_80034D0C (Tore + Segmentzaehler) und FUN_800355C4 (Schleife) im Claire-EXE SLES_109.72 (Muster ohne absolute
Adressen; lui/addiu-Paare als Joker)."""
import struct, re
L = open(r"C:/workspace/git/reAi_v2/info/re2leon/PSX.EXE", "rb").read()
C = open(r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an/cdisc/SLES_109.72", "rb").read()
tL = struct.unpack_from("<I", L, 0x18)[0]; tC = struct.unpack_from("<I", C, 0x18)[0]
def words(buf, t, a, n): o = a - t + 0x800; return [struct.unpack_from("<I", buf, o + 4*i)[0] for i in range(n)]
def find(a, n, mask_ops=(0x0f,)):   # lui (op 0x0f) und lw/sw mit at/v0 als Joker -> nur Opcode+rs+rt vergleichen
    w = words(L, tL, a, n)
    pat = b""
    for x in w:
        op = x >> 26
        if op in (0x0f, 0x23, 0x2b, 0x09, 0x03, 0x02):   # lui, lw, sw, addiu, jal, j: Immediate/Ziel als Joker
            hi = struct.pack("<I", x)[2:]
            pat += b"[\x00-\xff]{2}" + re.escape(hi) if op not in (0x03, 0x02) else b"[\x00-\xff]{3}" + re.escape(struct.pack("<I", x)[3:])
        else:
            pat += re.escape(struct.pack("<I", x))
    hits = [m.start() for m in re.finditer(pat, C, re.S) if (m.start() - 0x800) % 4 == 0]
    return [hex(h - 0x800 + tC) for h in hits]
print("FUN_80034D0C Kopf+Tore (0x80034d0c, 27 Worte):", find(0x80034d0c, 27))
print("FUN_80034D0C Y-Tor (0x80034e50, 11 Worte):   ", find(0x80034e50, 11))
print("FUN_800355C4 Schleife (0x80035604, 21 Worte):", find(0x80035604, 21))
