# Abnahme 8: sucht 32-bit-Worte / Instruktionsmuster im RE2-Overlay EM23_OVL_0000.BIN (Basis 0x80100000)
import sys
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
BIN = r'C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN'
BASE = 0x80100000
d = open(BIN, 'rb').read()
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
pat = sys.argv[1]
for off in range(0, len(d) - 3, 4):
    raw = d[off:off + 4]
    ins = list(md.disasm(raw, BASE + off))
    if not ins: continue
    t = '%s %s' % (ins[0].mnemonic, ins[0].op_str)
    if pat in t: print('%08x  %s  %s' % (BASE + off, raw.hex(), t))
