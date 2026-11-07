# Abnahme 3 Spur W: alle "jal 0x8003ee3c" (Thread-Start FUN_8003ee3c) in PSX.EXE (Mod und Version_40%) und in den STAGE*.BIN-Overlays
# (ohne Kopf, Ladeadresse 0x80100000). Kodierung jal: 0x0C000000 | ((ziel & 0x0FFFFFFF) >> 2).
import sys, struct, os
ZIEL = 0x8003ee3c
WORT = 0x0C000000 | ((ZIEL & 0x0FFFFFFF) >> 2)
def scan(pfad, basis, kopf):
    d = open(pfad, 'rb').read()
    treffer = []
    for o in range(kopf, len(d) - 3, 4):
        if struct.unpack_from('<I', d, o)[0] == WORT:
            treffer.append(basis + (o - kopf))
    return d, treffer
for wurzel in sys.argv[1:]:
    exe = os.path.join(wurzel, 'PSX.EXE')
    psx = os.path.join(wurzel, 'PSX')
    d = open(exe, 'rb').read()
    t_addr = struct.unpack_from('<I', d, 0x18)[0]
    _, t = scan(exe, t_addr, 0x800)
    print('%s: t_addr 0x%08X, jal 0x%08X (Wort 0x%08X): %d Treffer: %s' % (exe, t_addr, ZIEL, WORT, len(t), ' '.join('@0x%08X' % a for a in t)))
    for st in range(1, 7):
        b = os.path.join(psx, 'BIN', 'STAGE%d.BIN' % st)
        if os.path.exists(b):
            _, t = scan(b, 0x80100000, 0)
            print('  %s: %d Treffer %s' % (b, len(t), ' '.join('@0x%08X' % a for a in t)))
