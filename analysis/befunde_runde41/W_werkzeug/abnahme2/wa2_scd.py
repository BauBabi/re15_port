"""wa2_scd.py - Abnahme W2: vollstaendiger linearer SCD-Durchlauf (Laengen = engine/src/scd_vm.c s_opcode_sizes,
0x2D = 34 wie scd_vm.c:4437). Gibt JEDEN Opcode mit Bytes aus. Aufruf: wa2_scd.py <RDT> [main|subNN ...]"""
import struct, sys
L = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,0x0C:1,0x0D:6,0x0E:2,0x0F:4,0x10:2,
     0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x20:1,0x21:4,
     0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,0x30:1,0x31:1,0x32:8,
     0x33:8,0x34:4,0x35:3,0x36:12,0x37:4,0x38:12,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:3,0x3E:6,0x3F:4,0x40:8,0x41:10,0x42:1,
     0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:18,0x4D:10,0x4E:5,0x4F:22,0x50:22,0x51:4,0x52:4,
     0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,0x5B:4,0x5C:4,0x5D:4,0x5E:4}
fn = sys.argv[1]; want = sys.argv[2:]
d = open(fn, "rb").read()
offs = struct.unpack_from("<23I", d, 8)
progs = []
for base, name in ((offs[14], "main"), (offs[15], "sub")):
    n = struct.unpack_from("<H", d, base)[0] // 2
    ptrs = [base + struct.unpack_from("<H", d, base + 2 * i)[0] for i in range(n)]
    for k, p in enumerate(ptrs):
        progs.append(("%s%02d" % (name, k), p, ptrs[k + 1] if k + 1 < n else None))
# Ende des letzten main: Anfang der sub-Tabelle; Ende des letzten sub: bis zum ersten Desync/0x01 auf Ebene 0 (unten)
fixed = []
for i, (nm, p, e) in enumerate(progs):
    if e is None:
        e = offs[15] if nm.startswith("main") else len(d)
    fixed.append((nm, p, e))
for nm, p, e in fixed:
    if want and nm not in want: continue
    print("== %s @0x%05X..0x%05X" % (nm, p, e))
    pc = p
    while pc < e:
        op = d[pc]
        if op not in L:
            print("   @0x%05X ?? %02x  (unbekannt, Abbruch)" % (pc, op)); break
        b = d[pc:pc + L[op]]
        print("   @0x%05X %02X  %s" % (pc, op, b.hex(" ")))
        pc += L[op]
        if e == len(d) and op == 0x01 and pc - p > 2:
            # letztes Unterprogramm: nach Evt_end weiter nur wenn noch Code folgt? hier abbrechen
            break
