# Abnahme 9 (Abnehmer, eigenes Werkzeug): RE2 EM23_OVL_0000.BIN, Basis 0x80100000, capstone MIPS32 LE.
# Aufruf: abn9_dis.py start ende [start ende ...]   |  abn9_dis.py --find <hex-wort-LE> | --scan238 | --jal <ziel>
import sys, hashlib, struct
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
BIN = r'C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN'
BASE = 0x80100000
d = open(BIN, 'rb').read()
print('# %s  %d B  md5 %s' % (BIN, len(d), hashlib.md5(d).hexdigest()))
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
def one(p):
    raw = d[p-BASE:p-BASE+4]
    ins = list(md.disasm(raw, p))
    return raw, ((ins[0].mnemonic + ' ' + ins[0].op_str) if ins else '.word 0x%08x' % int.from_bytes(raw, 'little'))
a = sys.argv[1:]
if a and a[0] == '--scan238':
    # alle Lade-/Speicherbefehle mit Offset 0x238 (568)
    for p in range(BASE, BASE + len(d) - 3, 4):
        w = struct.unpack_from('<I', d, p-BASE)[0]
        op = w >> 26; imm = w & 0xffff
        if op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2b) and imm == 0x238:
            raw, t = one(p); print('%08x  %s  %s' % (p, raw.hex(), t))
    sys.exit()
if a and a[0] == '--jal':
    tgt = int(a[1], 16); w = (3 << 26) | ((tgt >> 2) & 0x3ffffff); j = (2 << 26) | ((tgt >> 2) & 0x3ffffff)
    for p in range(BASE, BASE + len(d) - 3, 4):
        x = struct.unpack_from('<I', d, p-BASE)[0]
        if x in (w, j) or x == tgt:
            raw, t = one(p); print('%08x  %s  %s' % (p, raw.hex(), t))
    sys.exit()
for i in range(0, len(a), 2):
    s = int(a[i], 16); e = int(a[i+1], 16)
    print('==== %08x-%08x ====' % (s, e))
    for p in range(s, e, 4):
        raw, t = one(p); print('%08x  %s  %s' % (p, raw.hex(), t))
