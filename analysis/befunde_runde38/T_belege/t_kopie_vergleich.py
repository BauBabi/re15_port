# Runde 38 Spur T: STAGE4- gegen STAGE5-Spiderman-Code (+Versatz) instruktionsweise vergleichen.
# Gleich = identisches Wort; reloziert = gleicher Opcode/Register, nur Immediate/Sprungziel anders.
import struct, sys
s4 = open(sys.argv[1], "rb").read(); s5 = open(sys.argv[2], "rb").read()
lo = int(sys.argv[3], 16); hi = int(sys.argv[4], 16); d = int(sys.argv[5], 16)
gleich = rel = anders = 0; bsp = []
for a in range(lo, hi, 4):
    w4 = struct.unpack_from('<I', s4, a - 0x80100000)[0]
    w5 = struct.unpack_from('<I', s5, a + d - 0x80100000)[0]
    if w4 == w5: gleich += 1; continue
    op4, op5 = w4 >> 26, w5 >> 26
    if op4 == op5 and (op4 in (2, 3) or ((w4 >> 16) == (w5 >> 16) and op4 != 0)):
        rel += 1
    else:
        anders += 1
        if len(bsp) < 12: bsp.append("0x%08X: %08X vs %08X" % (a, w4, w5))
print("gleich %d, reloziert %d, anders %d" % (gleich, rel, anders))
for b in bsp: print("  ", b)
