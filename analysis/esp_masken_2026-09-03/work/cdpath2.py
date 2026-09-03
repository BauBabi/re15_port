"""Datei-Tabelle @0x8006f440 abklopfen: welcher Eintrag traegt die Groesse einer ROOM*.BSS?"""
import struct, os, glob

EXE = open('info/Re1.5/PSX.EXE', 'rb').read()
BASE, HDR = 0x80010000, 0x800


def rd(a, n): return EXE[a - BASE + HDR:a - BASE + HDR + n]
def u8(a): return rd(a, 1)[0]
def u16(a): return struct.unpack('<H', rd(a, 2))[0]
def u32(a): return struct.unpack('<I', rd(a, 4))[0]


TBL = 0x8006f440
ent = []
for i in range(0, 1400):
    a = TBL + i * 8
    ent.append((u16(a) | (u8(a + 2) << 16), u8(a + 3), u32(a + 4)))

ptrs = [u32(0x8007438c + s * 4) for s in range(6)]
nroom = {1: 40, 2: 16, 3: 16, 4: 16, 5: 24, 6: 8}

print("Stage 1, Raum 0..5:  ptr[0]=0x%08x" % ptrs[0])
for room in range(6):
    fi = u16(ptrs[0] + room * 2)
    print("   room %2d -> fileIdx %d ; Eintraege [fi-1 .. fi+1]:" % (room, fi))
    for k in (fi - 2, fi - 1, fi, fi + 1):
        lba, fl, sz = ent[k]
        print("       idx %4d : LBA %-7d flag 0x%02x size %-9d (%.2f Chunks)" % (k, lba, fl, sz, sz / 65536.0))
    p = 'info/Re1.5/PSX/STAGE1/ROOM1%02X.BSS' % room
    print("       echte Datei %s = %d Bytes (%d Chunks)" % (os.path.basename(p), os.path.getsize(p),
                                                            os.path.getsize(p) // 65536))
print()
# Wo steht ueberhaupt die Groesse 589824?
hits = [i for i, e in enumerate(ent) if e[2] == 589824]
print("Eintraege mit size==589824:", hits[:20])
