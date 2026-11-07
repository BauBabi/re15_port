# Abnahme 5: ROOM7040.RDT (RE2 Leon) - Kopf, Skript-Bytes an den Dossier-Adressen, RID/RVD-Felder
import struct, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/info/re2leon/PL0/RDT/ROOM7040.RDT"
b = open(P, 'rb').read()
print('Groesse', len(b), 'Kopf', b[:8].hex(' '))
off = struct.unpack_from('<23I', b, 8)
for i, o in enumerate(off): print('  [%2d] 0x%05X' % (i, o))
def hx(a, n): return b[a:a+n].hex(' ')
for a, n, was in [
    (0x01892, 2, 'sub06 Cut_chg(9)'), (0x01894, 4, 'sub06 Evt_exec(7,sub09)'), (0x01898, 2, 'Evt_next?'), (0x0189A, 4, 'Evt_exec(6,sub04)'),
    (0x0193A, 2, 'Cut_chg(10)'), (0x0193C, 4, 'Evt_exec(7,sub10)'),
    (0x01B20, 4, 'sub09 Evt_exec(4,sub11)'), (0x01B24, 4, 'Sleep 10'), (0x01B28, 4, 'Evt_exec(3,sub12)'),
    (0x01B2E, 32, 'sub10'), (0x01B4C, 0x4C, 'sub11'), (0x01B98, 0x4C, 'sub12'),
    (0x0169E, 0x40, 'sub04'), (0x016DE, 0x9A, 'sub05'),
    (0x01BE4, 0x1E, 'sub13'), (0x01C02, 0x18, 'sub14'), (0x01C1A, 0x30, 'sub15'),
    (0x01778, 0x30, 'sub06 Anfang'), (0x0179E, 2, 'sub06 Gosub sub13'), (0x01290, 8, 'MAIN sub00 @0x01290'),
    (0x00184, 8, 'RID Schnitt 9'), (0x001A4, 8, 'RID Schnitt 10'), (0x0045C, 20, 'RVD @0x45C'), (0x00470, 20, 'RVD @0x470'),
    (0x00244, 7*8, 'Objektmodell-Liste'), (0x035D8, 8, 'ESP-Ids'), (0x036F8, 8, 'ESP-Daten Anfang'), (0x18984, 0x14, 'ESP-TIM Kopf')]:
    print('@0x%05X %-26s %s' % (a, was, hx(a, n)))
print('RID9 +2 = %d -> >>7 = %d' % (struct.unpack_from('<H', b, 0x186)[0], struct.unpack_from('<H', b, 0x186)[0] >> 7))
print('RID10 +2 = %d -> >>7 = %d' % (struct.unpack_from('<H', b, 0x1A6)[0], struct.unpack_from('<H', b, 0x1A6)[0] >> 7))
