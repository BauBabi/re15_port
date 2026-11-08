# NB4 Runde 3 (Bauer): MIPS-R3000-Disassembly RE2 EM23_OVL_0000.BIN, Basis 0x80100000 (Datei-Offset = Adresse - Basis), capstone.
import sys, hashlib
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
BIN = r'C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN'
BASE = 0x80100000
d = open(BIN, 'rb').read()
print('# %s  %d B  md5 %s' % (BIN, len(d), hashlib.md5(d).hexdigest()))
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
a = sys.argv[1:]
for i in range(0, len(a), 2):
    s = int(a[i], 16); e = int(a[i+1], 16)
    print('==== %08x-%08x ====' % (s, e))
    for p in range(s, e, 4):
        raw = d[p-BASE:p-BASE+4]
        ins = list(md.disasm(raw, p))
        print('%08x  %s  %s' % (p, raw.hex(), (ins[0].mnemonic + ' ' + ins[0].op_str) if ins else '.word 0x%08x' % int.from_bytes(raw, 'little')))
