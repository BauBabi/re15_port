import sys, struct, capstone
f, a, n = sys.argv[1], int(sys.argv[2],16), int(sys.argv[3])
d = open(f,'rb').read()
if f.upper().endswith('.EXE'):
    base = struct.unpack_from('<I', d, 0x18)[0]; off = 0x800
else:
    base = 0x80100000; off = 0
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
o = a - base + off
for ins in md.disasm(d[o:o+4*n], a):
    print('%08x  %s  %s %s' % (ins.address, d[ins.address-base+off:ins.address-base+off+4].hex(), ins.mnemonic, ins.op_str))
