#!/usr/bin/env python3
"""Runde 37 Spur N — am ORIGINAL messen, aus welchem Archiv die Gegner-/NPC-Banken stammen.

Je DuckStation-Spielstand (MZD-Disc, nur gelesen):
  * EXE sauber? @0x80026e4c == 08 00 e0 03 (Auslieferungs-Stub; gepatchte Builds: 24 c2 01 08)
  * Elza-Bit DAT_800aca3c Bit 31 (bltz @0x80022334)
  * Raum: RDT im RAM (*0x800ac778) gegen alle ROOM*.RDT (erste 0x400 B)
  * je Entity i (0x800acc2c + i*0x1F4, Flagwort +0 != 0): Typ +8, Banken
    +0x16c dir1 EDD, +0x84 dir2 EMR, +0x174 dir3 EDD, +0x170 dir4 EMR, +0x17c dir5 EDD, +0x178 dir6 EMR
    (FUN_80022300 @0x80022498/a8/b8/c8/d8/e8), Mesh +0x10 (@0x800224f8). Fuer jede Bank: Bytes im RAM
    == Abschnitt des CDEMD0-Blobs? == CDEMD1-Blob? (TOC @0x80072f38 / @0x80073178)
Aufruf: python n2_orig_ram.py <sav> [<sav> ...]
"""
import sys, os, struct
BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.insert(0, os.path.join(BAUM, '.claude/skills/re15-savestate-ghidra/scripts'))
import re15_ss
EXE = open(os.path.join(BAUM, 'info/Re1.5/PSX.EXE'), 'rb').read()
T = struct.unpack_from('<I', EXE, 0x18)[0]
def rd(a, n): o = 0x800 + a - T; return EXE[o:o + n]
PSX = os.path.join(BAUM, 're15_port/shared_assets/PSX/')
E = {0: open(PSX + 'EMD/CDEMD0.EMS', 'rb').read(), 1: open(PSX + 'EMD/CDEMD1.EMS', 'rb').read()}
TOC = {0: 0x80072f38, 1: 0x80073178}
def blob(a, t):
    if not 0x10 <= t < 0x50: return None
    s, z = struct.unpack_from('<II', rd(TOC[a] + (t - 0x10) * 8, 8))
    return E[a][s * 2048: s * 2048 + z] if z else None
def dirs(b):
    do = struct.unpack_from('<I', b, 0)[0]; return list(struct.unpack_from('<9I', b, do)), do
RDTS = {}
for st in range(1, 7):
    d = PSX + 'STAGE%d' % st
    for fn in os.listdir(d):
        if fn.startswith('ROOM') and fn.endswith('.RDT'):
            RDTS[fn[4:8]] = open(os.path.join(d, fn), 'rb').read()
BANK = [(0x16c, 1, 'EDD1'), (0x84, 2, 'EMR2'), (0x174, 3, 'EDD3'), (0x170, 4, 'EMR4'),
        (0x17c, 5, 'EDD5'), (0x178, 6, 'EMR6')]
def sect(b, d, do, k):
    st = d[k]; nx = [x for x in d if x > st] + [do]; return b[st:min(nx)]
def scan(path):
    r = re15_ss.Ram(path)
    stub = r.bytes(0x80026e4c, 4).hex()
    elza = (r.u32(0x800aca3c) >> 31) & 1
    rdtp = r.u32(0x800ac778)
    room = []
    if 0x80000000 <= rdtp < 0x80200000:
        # Kopf (0x00..0x60) traegt relozierte Zeiger -> Vergleich ab 0x60 (SCD/Daten unveraendert)
        best = max(((sum(1 for a, b in zip(r.bytes(rdtp + 0x60, 0x800), v[0x60:0x860]) if a == b), k)
                    for k, v in RDTS.items()), default=(0, ''))
        room = ['%s (%d/2048 B gleich)' % (best[1], best[0])]
    print('== %s  EXE-Stub %s (%s)  Elza-Bit %d  Raum %s' % (os.path.basename(path), stub,
          'sauber' if stub == '0800e003' else 'GEPATCHT', elza, room))
    for i in range(20):
        e = 0x800acc2c + i * 0x1f4
        if r.u32(e) == 0: continue
        t = r.u8(e + 8)
        b0 = blob(0, t); b1 = blob(1, t)
        if not b0 or not b1:
            print('   Entity %2d Typ 0x%02X (kein TOC-Eintrag)' % (i, t)); continue
        d0, do0 = dirs(b0); d1, do1 = dirs(b1)
        res = []
        for off, k, nm in BANK:
            p = r.u32(e + off)
            if not (0x80000000 <= p < 0x80200000):
                res.append('%s=--' % nm); continue
            s0 = sect(b0, d0, do0, k); s1 = sect(b1, d1, do1, k)
            n = max(len(s0), len(s1))
            ram = r.bytes(p, n)
            m0 = ram[:len(s0)] == s0; m1 = ram[:len(s1)] == s1
            tag = 'beide' if (m0 and m1) else ('CDEMD1' if m1 else ('CDEMD0' if m0 else 'keins'))
            res.append('%s=%s' % (nm, tag))
        mp = r.u32(e + 0x10)
        md = 'MD1=?'
        if 0x80000000 <= mp < 0x80200000:
            s0 = sect(b0, d0, do0, 7); s1 = sect(b1, d1, do1, 7)
            # +0x10 zeigt nach FUN_80022150 auf MD1+0xC (@0x8002264c addiu a0,v1,12)
            for adj in (0, 12):
                ram = r.bytes(mp - adj, max(len(s0), len(s1)))
                m0 = ram[4:len(s0)] == s0[4:]; m1 = ram[4:len(s1)] == s1[4:]
                if m0 or m1:
                    md = 'MD1(-%d)=%s' % (adj, 'beide' if (m0 and m1) else ('CDEMD1' if m1 else 'CDEMD0')); break
        print('   Entity %2d Typ 0x%02X +0x97=%d  %s  %s' % (i, t, r.u8(e + 0x97), ' '.join(res), md))
for p in sys.argv[1:]:
    try: scan(p)
    except Exception as ex: print('== %s FEHLER %s' % (os.path.basename(p), ex))
