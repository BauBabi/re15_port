# Zensus: RE2-Retail (info/re2leon/PSX.EXE) - Sperre (w1>>9)&0x7F der GL-Zeilen 9/10/11 JE Typ.
# Zeiger *(0x800A6A88 + Typ*4); Zeile z liegt bei Basis + (z-1)*20 (w0 @+0, w1 @+4).
import struct, sys
p = r"C:\workspace\git\reAi_v2\info\re2leon\PSX.EXE"
d = open(p, "rb").read()
t_addr = struct.unpack_from("<I", d, 0x18)[0]
def rd(a):
    o = a - t_addr + 0x800
    return struct.unpack_from("<I", d, o)[0]
locks = {}
for typ in range(0, 64):
    base = rd(0x800A6A88 + typ * 4)
    row = []
    for z in (9, 10, 11):
        a = base + (z - 1) * 20
        w0, w1 = rd(a), rd(a + 4)
        row.append((z, a, w0, w1, (w1 >> 9) & 0x7F))
    locks[typ] = row
    print("Typ 0x%02X Basis 0x%08X  " % (typ, base) + "  ".join(
        "Z%d @0x%08X w0=%08X w1=%08X sperre=%d" % r for r in row))
alle = sorted({r[4] for rows in locks.values() for r in rows})
print("verschiedene Sperren ueber alle Typen/Zeilen 9..11:", alle)
gegner = sorted({r[4] for typ, rows in locks.items() if 0x10 <= typ <= 0x3F for r in rows})
print("verschiedene Sperren NUR Gegnertypen 0x10..0x3F (48 Typen x 3 Zeilen = 144):", gegner)
