"""CD-Ladepfad byte-genau: PTR_8007438c -> Datei-Index -> Datei-Tabelle @0x8006f440 -> LBA/Groesse.
   Gegenprobe: Tabellen-Groesse == echte Dateigroesse der ROOM*.BSS."""
import struct, os

EXE = open('info/Re1.5/PSX.EXE', 'rb').read()
BASE = 0x80010000
HDR = 0x800


def rd(addr, n):
    off = addr - BASE + HDR
    return EXE[off:off + n]


def u8(a):  return rd(a, 1)[0]
def u16(a): return struct.unpack('<H', rd(a, 2))[0]
def u32(a): return struct.unpack('<I', rd(a, 4))[0]


print("PTR_DAT_8007438c (Stage -> Zeiger auf Raum->Datei-Index-Tabelle):")
ptrs = {}
for st in range(0, 8):
    p = u32(0x8007438c + st * 4)
    ptrs[st] = p
    print("   [stage %d] = 0x%08x" % (st, p))

print()
print("Datei-Tabelle: base=&DAT_8006f440, Eintrag = base + (fileIndex-1)*8")
print("   Layout je Eintrag: u16 lba_lo | u8 lba_hi | u8 ? | u32 size")
print()
hdr = "%-6s %-6s %-9s %-9s %-10s %-10s %s" % ("stage", "raum", "fileIdx", "LBA", "size_tab", "size_datei", "Datei")
print(hdr)
okc = badc = 0
for st in (1, 2, 3, 4, 5, 6):
    p = ptrs[st - 1]
    d = 'info/Re1.5/PSX/STAGE%d' % st
    for room in range(0, {1:40,2:16,3:16,4:16,5:24,6:8}[st]):
        try:
            fi = u16(p + room * 2)
        except Exception:
            break
        if fi == 0:
            continue
        e = 0x8006f440 + (fi - 1) * 8
        lba = u16(e) | (u8(e + 2) << 16)
        size = u32(e + 4)
        name = 'ROOM%X%02X.BSS' % (st, room)
        path = os.path.join(d, name)
        real = os.path.getsize(path) if os.path.exists(path) else -1
        mark = 'OK' if real == size else 'ABWEICHUNG'
        if real == size:
            okc += 1
        else:
            badc += 1
        if room < 4 or real != size:
            print("%-6d 0x%-4X %-9d %-9d %-10d %-10d %s  %s" %
                  (st, room, fi, lba, size, real, name, mark))
print()
print("Treffer: %d   Abweichungen: %d" % (okc, badc))
