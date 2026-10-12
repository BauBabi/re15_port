# RDT-Datei-Index je (Stage-Byte 0x800b0fe0, Raum-Byte 0x800b0fe2) wie der Raumlader @0x800397a4-ec:
# idx = (aca3c>>31) + u16[ *(0x8007438c + stage*4) + raum*2 ];  Groesse aus der Dateitabelle @0x8006F43C (8 Byte je Eintrag)
import struct, sys, os
sys.path.insert(0, sys.argv[1])
import re15_disasm as R
d = open(R.EXE_PATH,'rb').read(); taddr = struct.unpack_from('<I', d, 0x18)[0]
u32 = lambda a: struct.unpack_from('<I', d, 0x800 + a - taddr)[0]
u16 = lambda a: struct.unpack_from('<H', d, 0x800 + a - taddr)[0]
stage, raum = int(sys.argv[2]), int(sys.argv[3], 16)
p = u32(0x8007438c + stage*4)
for elza in (0, 1):
    idx = elza + u16(p + raum*2)
    groesse = u32(0x8006f43c + idx*8)
    print(f"Stage-Byte {stage} Raum 0x{raum:02x} Figur-Bit {elza}: Zeiger 0x{p:08x} -> Datei-Index {idx}, Groesse {groesse} (0x{groesse:x})")
