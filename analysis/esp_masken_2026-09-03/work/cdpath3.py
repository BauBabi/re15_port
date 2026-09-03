"""CD-Ladepfad: Datei-Eintrag = { u32 size ; u16 lba_lo ; u8 lba_hi ; u8 flag },
   LBA-Feld liegt bei 0x8006f440 + (fileIdx-1)*8, Groesse 4 Byte davor.
   Gegenprobe gegen die echten ROOM*.BSS-Dateigroessen."""
import struct, os

EXE = open('info/Re1.5/PSX.EXE', 'rb').read()
BASE, HDR = 0x80010000, 0x800


def rd(a, n): return EXE[a - BASE + HDR:a - BASE + HDR + n]
def u8(a): return rd(a, 1)[0]
def u16(a): return struct.unpack('<H', rd(a, 2))[0]
def u32(a): return struct.unpack('<I', rd(a, 4))[0]


ptrs = [u32(0x8007438c + s * 4) for s in range(6)]
nroom = {1: 40, 2: 16, 3: 16, 4: 16, 5: 24, 6: 8}
ok = bad = 0
print("%-3s %-5s %-8s %-8s %-10s %-10s %-6s %s" %
      ("st", "raum", "fileIdx", "LBA", "size_tab", "size_datei", "Chunks", "Datei"))
for st in (1, 2, 3, 4, 5, 6):
    for room in range(nroom[st]):
        fi = u16(ptrs[st - 1] + room * 2)
        a = 0x8006f440 + (fi - 1) * 8
        lba = u16(a) | (u8(a + 2) << 16)
        size = u32(a - 4)
        name = 'ROOM%X%02X.BSS' % (st, room)
        p = 'info/Re1.5/PSX/STAGE%d/%s' % (st, name)
        real = os.path.getsize(p) if os.path.exists(p) else -1
        good = (real == size)
        ok += good
        bad += (not good)
        if room < 3 or not good:
            print("%-3d 0x%-3X %-8d %-8d %-10d %-10d %-6d %s %s" %
                  (st, room, fi, lba, size, real, size // 65536, name, "" if good else "<-- ABWEICHUNG"))
print()
print("Treffer %d / Abweichungen %d" % (ok, bad))
