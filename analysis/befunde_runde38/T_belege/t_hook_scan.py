# Runde 38 Spur T: findet in einem STAGE*.BIN alle sw-Stores auf eine absolute Adresse
# (lui-Tracking je Register), z.B. die Spieler-Hook-Tabellen 0x800ac758/0x800ac858 [typ].
# Aufruf: python t_hook_scan.py <STAGE.BIN> <addr-hex> [<addr-hex> ...]
import sys, struct
BASE = 0x80100000
data = open(sys.argv[1], "rb").read()
targets = {int(a, 16) for a in sys.argv[2:]}
hi = {}; full = {}
for off in range(0, len(data) - 3, 4):
    w = struct.unpack_from('<I', data, off)[0]
    op = w >> 26; rs = (w >> 21) & 0x1f; rt = (w >> 16) & 0x1f; imm = w & 0xffff
    simm = imm - 0x10000 if imm & 0x8000 else imm
    if op == 0x0f:
        hi[rt] = imm << 16; full[rt] = imm << 16
    elif op == 0x09 and rs in hi:
        full[rt] = (hi[rs] + simm) & 0xffffffff
    elif op == 0x2b and rs in hi:
        a = (hi[rs] + simm) & 0xffffffff
        if a in targets:
            print("0x%08X: sw r%d -> 0x%08X  (wert r%d = 0x%08X)" % (BASE + off, rt, a, rt, full.get(rt, 0)))
