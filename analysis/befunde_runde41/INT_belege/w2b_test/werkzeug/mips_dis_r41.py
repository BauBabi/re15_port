# dis.py <psx-exe|bin> <basis|exe> <adresse_hex> [anzahl]  - MIPS R3000 LE (capstone), PSX-EXE: t_addr @0x18, Kopf 0x800
import struct, sys
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
d = open(sys.argv[1], "rb").read()
if sys.argv[2] == "exe":
    t_addr = struct.unpack_from("<I", d, 0x18)[0]; off0 = 0x800
else:
    t_addr = int(sys.argv[2], 16); off0 = 0
a = int(sys.argv[3], 16); n = int(sys.argv[4]) if len(sys.argv) > 4 else 4
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
o = a - t_addr + off0
for i in md.disasm(d[o:o + 4 * n], a):
    print("  %08x: %s  %-6s %s" % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))
