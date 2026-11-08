import sys, struct, capstone
exe = open(r'C:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE','rb').read()
t_addr = struct.unpack_from('<I', exe, 0x18)[0]
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
a = int(sys.argv[1],16); n = int(sys.argv[2])
off = 0x800 + a - t_addr
for i in md.disasm(exe[off:off+4*n], a):
    print('0x%08x  %s  %s %s' % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))
