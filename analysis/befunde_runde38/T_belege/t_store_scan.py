# Runde 38 Spur T: alle sh/sw/sb-Stores auf eine ABSOLUTE Adresse (lui-Tracking) in PSX.EXE oder STAGE*.BIN.
# Aufruf: python t_store_scan.py <datei> <addr-hex> [...]
import sys, struct
path = sys.argv[1]; targets = {int(a, 16) for a in sys.argv[2:]}
data = open(path, "rb").read()
if path.upper().endswith(".EXE"):
    base = struct.unpack_from('<I', data, 0x18)[0]; data = data[0x800:]
else:
    base = 0x80100000
hi = {}
names = {0x28: "sb", 0x29: "sh", 0x2b: "sw"}
for off in range(0, len(data) - 3, 4):
    w = struct.unpack_from('<I', data, off)[0]
    op = w >> 26; rs = (w >> 21) & 0x1f; rt = (w >> 16) & 0x1f; imm = w & 0xffff
    simm = imm - 0x10000 if imm & 0x8000 else imm
    if op == 0x0f: hi[rt] = imm << 16
    elif op in names and rs in hi:
        a = (hi[rs] + simm) & 0xffffffff
        if a in targets:
            print("0x%08X: %s r%d -> 0x%08X" % (base + off, names[op], rt, a))
