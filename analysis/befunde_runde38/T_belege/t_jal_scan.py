# Runde 38 Spur T: alle jal <ziel> in einem Overlay-BIN (RAW @0x80100000) bzw. der EXE, optional Adressfenster.
# Aufruf: python t_jal_scan.py <BIN|PSX.EXE> <ziel-hex> [lo-hex hi-hex]
import sys, struct
path = sys.argv[1]; ziel = int(sys.argv[2], 16)
lo = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0; hi = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0xffffffff
data = open(path, "rb").read()
if path.upper().endswith(".EXE"):
    t_addr = struct.unpack_from('<I', data, 0x18)[0]; base = t_addr; data = data[0x800:]
else:
    base = 0x80100000
want = 0x0c000000 | ((ziel >> 2) & 0x03ffffff)
for off in range(0, len(data) - 3, 4):
    if struct.unpack_from('<I', data, off)[0] == want:
        a = base + off
        if lo <= a <= hi: print("0x%08X: jal 0x%08X" % (a, ziel))
