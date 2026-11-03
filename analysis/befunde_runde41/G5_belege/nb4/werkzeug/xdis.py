# Nachbesserung 4 G5A: MIPS-Disasm RE2 SLUS_007.48 (Datei-Offset = Adresse - 0x80010000 + 0x800)
import sys, capstone, struct
b = open(r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48", "rb").read()
t_addr = struct.unpack_from("<I", b, 0x18)[0]
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN); md.skipdata = True
a0, a1 = int(sys.argv[1], 16), int(sys.argv[2], 16)
for i in md.disasm(b[a0 - t_addr + 0x800:a1 - t_addr + 0x800], a0):
    print("  %08x %s  %s %s" % (i.address, i.bytes.hex(" "), i.mnemonic, i.op_str))
