# Abnahme 8 (eigenes Werkzeug): MIPS-R3000-Disassembly des RE2-Overlays EM23_OVL_0000.BIN (Basis 0x80100000, kein Header) per capstone.
# Aufruf: python -I dis8.py <start_hex> <end_hex> [<start_hex> <end_hex> ...]   (Datei-Offset = Adresse - 0x80100000)
import sys, hashlib
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
BIN = r'C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN'
BASE = 0x80100000
data = open(BIN, 'rb').read()
print('# %s  Groesse %d  md5 %s' % (BIN, len(data), hashlib.md5(data).hexdigest()))
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
a = sys.argv[1:]
for i in range(0, len(a), 2):
    s = int(a[i], 16); e = int(a[i + 1], 16)
    print('==== %08x-%08x (Datei 0x%x-0x%x) ====' % (s, e, s - BASE, e - BASE))
    p = s
    while p < e:
        raw = data[p - BASE:p - BASE + 4]
        ins = list(md.disasm(raw, p))
        if ins: print('%08x  %s  %-8s %s' % (p, raw.hex(), ins[0].mnemonic, ins[0].op_str))
        else:   print('%08x  %s  .word 0x%08x' % (p, raw.hex(), int.from_bytes(raw, 'little')))
        p += 4
