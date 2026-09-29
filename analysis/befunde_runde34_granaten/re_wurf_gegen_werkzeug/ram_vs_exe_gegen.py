#!/usr/bin/env python3
"""Gegenpruefung R34: vergleicht granatenrelevante Code-/Tabellenbereiche der PSX.EXE-Datei mit dem RAM
mehrerer sauberer Savestates (Laufzeit-Patch-Check). Aufruf ohne Argumente."""
import sys, os, struct, glob
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', '.claude', 'skills', 're15-savestate-ghidra', 'scripts'))
from re15_ss import Ram
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
exe = open(os.path.join(REPO, 'info/Re1.5/PSX.EXE'), 'rb').read()
def fx(a, n): return exe[0x800 + a - 0x80010000: 0x800 + a - 0x80010000 + n]
RANGES = [('R29/R30/R31', 0x80018320, 0x800186e4), ('R10/R0/Zeilenvorschub', 0x80017248, 0x80017718),
          ('ESP-Routinentabelle', 0x80071d40, 0x80071e00), ('Spawner 19700/199d4', 0x80019700, 0x80019ca4),
          ('ESP-Tick 19e20', 0x80019e20, 0x8001a4b8), ('FIRE-Substate+Spawn', 0x80033460, 0x800337bc),
          ('Entlade-Tabelle 74100', 0x80074100, 0x80074138), ('Entlade 9/10/11', 0x80033b38, 0x80033b98),
          ('Schaden 12d60', 0x80012d60, 0x8001306c), ('Hitbox 2b5d0', 0x8002b5d0, 0x8002b7e8), ('Versatz 2b498', 0x8002b498, 0x8002b540),
          ('SE 45024', 0x80045024, 0x800453b8), ('Lage 45a64', 0x80045a64, 0x80045c40), ('Hauptbild ce00-d1c0', 0x8001ce00, 0x8001d1c0),
          ('Schadens-/Reaktionstab', 0x8006f418, 0x8006f43c), ('RNG', 0x8001af20, 0x8001af5c), ('SE-Banktabelle', 0x80010e70, 0x80010e88),
          ('Waffen-FSM-Tab 740f4', 0x800740f4, 0x80074100), ('Item-Tab 74030', 0x80074030, 0x800740f4)]
saves = [p for p in sorted(glob.glob(os.path.join(REPO, 'stage_saves', '*.sav'))) if not os.path.basename(p).startswith(('PATCHED', 'boot_16'))]
pick = [p for p in saves if any(k in os.path.basename(p) for k in ('room1140_entry', 'equip_test', 'room1090_orig', 'doorA_square', 'orig_lamp'))]
for p in pick:
    r = Ram(p)
    def rr(a, n):
        o = r.base + (a - 0x80000000); return r.blob[o:o + n]
    bad = []
    for name, a, b in RANGES:
        f = fx(a, b - a); m = rr(a, b - a)
        if f != m:
            diffs = [a + i for i in range(0, b - a, 4) if f[i:i+4] != m[i:i+4]]
            bad.append('%s: %d Worte abweichend, erstes @%08x datei=%s ram=%s' % (name, len(diffs), diffs[0], f[diffs[0]-a:diffs[0]-a+4].hex(), m[diffs[0]-a:diffs[0]-a+4].hex()))
    print(os.path.basename(p), 'OK (alle %d Bereiche gleich)' % len(RANGES) if not bad else '')
    for x in bad: print('   ', x)
