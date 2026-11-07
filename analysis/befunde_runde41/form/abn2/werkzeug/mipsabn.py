# Eigener MIPS-Disassembler fuer die Abnahme (capstone), Datei + Basis-Offset + Ladeadresse.
# Aufruf: python dis.py <quelle> <adresse_hex> <anzahl> [words]
#   quelle: g4 | re15exe | re15exe40 | stage5 | stage3 | stage5_40 | stage3_40 | re2exe
import sys, struct, capstone
R = 'C:/workspace/git/reAi_v2/'
def exe(path):
    d = open(path, 'rb').read()
    t_addr = struct.unpack_from('<I', d, 0x18)[0]
    return d, 0x800, t_addr
def raw(path, base=0, load=0x80100000, size=None):
    d = open(path, 'rb').read()
    if size: d = d[base:base+size]; base = 0
    return d, base, load
SRC = {
  'g4':        lambda: raw(R+'info/re2leon/PL0/PLD/CDEMD0.EMS', 0x653000, 0x80100000, 0xB2D4),
  're15exe':   lambda: exe(R+'info/Re1.5/PSX.EXE'),
  're15exe40': lambda: exe(R+'Version_40%/PSX.EXE'),
  'stage5':    lambda: raw(R+'info/Re1.5/PSX/BIN/STAGE5.BIN'),
  'stage3':    lambda: raw(R+'info/Re1.5/PSX/BIN/STAGE3.BIN'),
  'stage5_40': lambda: raw(R+'Version_40%/PSX/BIN/STAGE5.BIN'),
  'stage3_40': lambda: raw(R+'Version_40%/PSX/BIN/STAGE3.BIN'),
}
src, a, n = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3])
mode = sys.argv[4] if len(sys.argv) > 4 else 'dis'
if src == 're2exe':
    import glob
    p = sys.argv[5] if len(sys.argv) > 5 else None
    d, hdr, t = exe(p)
    off = hdr + a - t
else:
    d, base, load = SRC[src]()
    if src.startswith('re15exe'):
        hdr, t = base, load
        off = hdr + a - t
    else:
        off = base + a - load
if mode == 'words':
    for i in range(n):
        w = struct.unpack_from('<I', d, off + 4*i)[0]
        print('%08x: %08x' % (a + 4*i, w))
    sys.exit(0)
if mode == 'half':
    for i in range(n):
        w = struct.unpack_from('<h', d, off + 2*i)[0]
        print('%08x: %d' % (a + 2*i, w))
    sys.exit(0)
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
code = d[off: off + 4*n]
for ins in md.disasm(code, a):
    print('%08x: %s  %s %s' % (ins.address, ' '.join('%02x' % b for b in ins.bytes), ins.mnemonic, ins.op_str))
