"""w_scd.py - Runde 41 Spur W: linearer SCD-Durchlauf (Laengen = Skill scd-disassembly, RE1.5 0x00-0x5E),
listet Sce_espr_on (0x3A), Sce_espr_kill (0x4C), Obj_model_set (0x2D), Mizu_div_set (0x5D) je Unterprogramm.
Aufruf: python w_scd.py <ROOM.RDT>"""
import struct, sys
L = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,0x0C:1,0x0D:6,0x0E:2,0x0F:4,0x10:2,
     0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x1F:1,0x20:1,0x21:4,
     0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,0x30:1,0x31:1,0x32:8,
     0x33:8,0x34:4,0x35:3,0x36:12,0x37:4,0x38:12,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:3,0x3E:6,0x3F:4,0x40:8,0x41:10,0x42:1,
     0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:5,0x4D:10,0x4E:5,0x4F:22,0x50:22,0x51:4,0x52:4,
     0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,0x5B:4,0x5C:4,0x5D:4,0x5E:4}
d = open(sys.argv[1], "rb").read()
def walk(base, name):
    first = struct.unpack_from("<H", d, base)[0]
    n = first // 2
    ptrs = [struct.unpack_from("<H", d, base + 2*i)[0] for i in range(n)]
    for k, pp in enumerate(ptrs):
        pc = base + pp
        end = base + (ptrs[k+1] if k+1 < n else 0x10000)
        while pc < end and pc < len(d):
            op = d[pc]
            if op not in L: print("  %s%02d @0x%05X ?? 0x%02X" % (name, k, pc, op)); break
            b = d[pc:pc+L[op]]
            if op == 0x3A:
                e, sub, sk = b[2], b[3], struct.unpack_from("<H", b, 6)[0]
                x, y, z, prm = struct.unpack_from("<hhhH", b, 8)
                print("  %s%02d @0x%05X Sce_espr_on  %s  eff=0x%02X sub=%d skala=0x%04X pos=(%d,%d,%d) par=0x%04X" % (name, k, pc, b.hex(" "), e, sub, sk, x, y, z, prm))
            elif op == 0x4C:
                print("  %s%02d @0x%05X Sce_espr_kill %s" % (name, k, pc, b.hex(" ")))
            elif op == 0x5D:
                print("  %s%02d @0x%05X Mizu_div_set %s" % (name, k, pc, b.hex(" ")))
            elif op == 0x2D:
                x, y, z = struct.unpack_from("<hhh", b, 10)
                print("  %s%02d @0x%05X Obj_model_set id=%d typ=%d attr=0x%04X pos=(%d,%d,%d) %s" % (name, k, pc, b[1], b[2], struct.unpack_from("<H",b,8)[0], x, y, z, b.hex(" ")))
            if op == 0x01 and k + 1 < n and pc + 2 >= base + ptrs[k+1]: break
            pc += L[op]
offs = struct.unpack_from("<23I", d, 8)
print(sys.argv[1], "main@0x%X sub@0x%X" % (offs[14], offs[15]))
walk(offs[14], "main"); walk(offs[15], "sub")
