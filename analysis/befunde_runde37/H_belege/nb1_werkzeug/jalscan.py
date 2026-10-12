# Alle jal <ziel> in der EXE, mit den 6 Instruktionen davor (Argumente)
import struct, sys
sys.path.insert(0, sys.argv[1])
import re15_disasm as R
ziel = int(sys.argv[2], 16)
d = open(R.EXE_PATH,'rb').read()
taddr = struct.unpack_from('<I', d, 0x18)[0]
jw = 0x0C000000 | ((ziel >> 2) & 0x03FFFFFF)
for off in range(0x800, len(d) - 3, 4):
    w = struct.unpack_from('<I', d, off)[0]
    if w == jw:
        a = off - 0x800 + taddr
        print(f"-- jal @0x{a:08x}")
        for k in range(-6, 2):
            ww = struct.unpack_from('<I', d, off + k*4)[0]
            s, _ = R.dis_one(ww, a + k*4)
            print(f"   {a+k*4:08x}: {s}")
