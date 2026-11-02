"""exe_map.py - Leon-DS-EXE (SLUS_007.48) gegen Claire-PAL-EXE (SLES_109.72): Code-Muster suchen, disassemblieren.
  python exe_map.py dis L|C ADDR [N]          disassemblieren (L=Leon DS, C=Claire PAL)
  python exe_map.py find ADDR [N] [--any]     Leon-Fenster ADDR..ADDR+4N maskiert (Immediates/Sprungziele) in Claire suchen
  python exe_map.py refs C ADDR               alle lui/addiu|lw|sw|sh|lh|lbu-Paare in Claire, die ADDR bilden
"""
import sys, struct
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
L = r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48"
C = r"C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\an\cdisc\SLES_109.72"
EX = {"L": open(L, "rb").read(), "C": open(C, "rb").read()}
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
def off(a): return 0x800 + (a - 0x80010000)
def word(k, a): return struct.unpack_from("<I", EX[k], off(a))[0]
def dis(k, a, n):
    d = EX[k]
    for i in range(n):
        x = a + 4 * i; w = word(k, x)
        ins = list(md.disasm(struct.pack("<I", w), x))
        print("  %08x: %08x  %s" % (x, w, (ins[0].mnemonic + " " + ins[0].op_str) if ins else "?"))
def mask(w):
    op = w >> 26
    if op in (2, 3): return w & 0xfc000000            # j/jal
    if op == 0: return w                              # R-Typ
    if op in (4, 5, 6, 7, 1): return w & 0xffff0000  # Verzweigungen: Distanz maskieren
    return w & 0xffff0000                             # I-Typ: Immediate maskieren
def find(a, n, anyhit=False):
    pat = [mask(word("L", a + 4 * i)) for i in range(n)]
    d = EX["C"]; hits = []
    for o in range(0x800, len(d) - 4 * n, 4):
        ok = True
        for i in range(n):
            if mask(struct.unpack_from("<I", d, o + 4 * i)[0]) != pat[i]:
                ok = False; break
        if ok: hits.append(0x80010000 + o - 0x800)
    return hits
def refs(k, target):
    d = EX[k]; out = []
    hi = (target + 0x8000) >> 16; lo = target & 0xffff
    for o in range(0x800, len(d) - 4, 4):
        w = struct.unpack_from("<I", d, o)[0]
        if w >> 26 == 0xf and (w & 0xffff) == hi:
            rt = (w >> 16) & 31
            for j in range(1, 12):
                w2 = struct.unpack_from("<I", d, o + 4 * j)[0]
                if ((w2 >> 21) & 31) == rt and (w2 & 0xffff) == lo and (w2 >> 26) in (0x9, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2b):
                    out.append(0x80010000 + o - 0x800 + 4 * j)
    return out
if __name__ == "__main__":
    c = sys.argv[1]
    if c == "dis": dis(sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4]) if len(sys.argv) > 4 else 32)
    elif c == "find":
        h = find(int(sys.argv[2], 16), int(sys.argv[3]) if len(sys.argv) > 3 else 16)
        print(" ".join("%08x" % x for x in h[:40]), "(%d Treffer)" % len(h))
    elif c == "refs":
        print(" ".join("%08x" % x for x in refs(sys.argv[2], int(sys.argv[3], 16))))
