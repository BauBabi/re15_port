# Nachbesserung 4 G5A: MIPS-Disasm der RE2-EM-AI-Bloecke (Datei-Offset = Adresse - 0x80100000)
import sys, capstone
D = r"C:\workspace\git\reAi_v2\build\extracted\re2_ems\CDEMD0_%s_ai0.BIN"
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN); md.skipdata = True
em, a0, a1 = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
b = open(D % em, "rb").read()
for i in md.disasm(b[a0 - 0x80100000:a1 - 0x80100000], a0):
    print("  %08x %s  %s %s" % (i.address, i.bytes.hex(" "), i.mnemonic, i.op_str))
