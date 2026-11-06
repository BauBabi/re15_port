# Abnahme 4: MIPS-Disasm eines RE2-Overlays (Datei-Offset = Adresse - basis) - eigenes Werkzeug
import sys, capstone
datei, basis, von, bis = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), int(sys.argv[4], 16)
b = open(datei, "rb").read()
md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
for i in md.disasm(b[von - basis:bis - basis], von):
    print("%08x  %s  %-7s %s" % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))
