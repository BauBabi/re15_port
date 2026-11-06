import sys, capstone
b = open(r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48", "rb").read()
von, bis = int(sys.argv[1], 16), int(sys.argv[2], 16)
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
for i in md.disasm(b[von - 0x80010000 + 0x800:bis - 0x80010000 + 0x800], von):
    print("%08x  %s  %-7s %s" % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))
