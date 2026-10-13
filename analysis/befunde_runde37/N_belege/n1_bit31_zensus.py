#!/usr/bin/env python3
"""Runde 37 Spur N — Zensus: wo schaltet ein Raumskript das Elza-Bit (Flag-Bank 1 Bit 0) um?

Flag-Bank 1 = DAT_800aca3c (Bank-Zeigertabelle @0x80074664[1] = 0x800aca3c), Bit 0 = Maske 0x80000000 >> 0
(Flag-Setzer @0x80043120: Wort (bit>>3)&0x1c, `lui v1,0x8000` / `srlv v1,v1,v0`) = das Elza-Bit, das
FUN_80022300 per `bltz` @0x80022334 liest. Gesucht: Set(1,0,v) (Opcode 0x22) und Ck(1,0,v) (0x21) in allen
240 RDTs, mit den Sce_em_set (0x44), die zwischen einem Set(1,0,0) und dem naechsten Set(1,0,1) im selben
Skriptabschnitt stehen (= mit dem Leon-Archiv geladen) bzw. umgekehrt.
Walker = re15_port/tools/scd_dump_room.py (opcode-exakt, je Abschnitt main/subNN).
"""
import os, sys, re, subprocess
BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
PSX = os.path.join(BAUM, 're15_port/shared_assets/PSX')
DUMP = os.path.join(BAUM, 're15_port/tools/scd_dump_room.py')
rx_sec = re.compile(r'^=== (\S+)')
rx_op = re.compile(r'^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) (\S+)\s+((?:[0-9a-f]{2} ?)+)')
treffer = []
for st in range(1, 7):
    d = os.path.join(PSX, 'STAGE%d' % st)
    for fn in sorted(os.listdir(d)):
        if not (fn.startswith('ROOM') and fn.endswith('.RDT')):
            continue
        txt = subprocess.run([sys.executable, DUMP, os.path.join(d, fn)], capture_output=True, text=True).stdout
        sec = '?'
        zustand = None          # None = Skript hat Bit 0 in diesem Abschnitt nicht angefasst
        gesehen = set()
        for line in txt.splitlines():
            m = rx_sec.match(line)
            if m:
                sec = m.group(1)
                zustand = None
                continue
            m = rx_op.match(line)
            if not m:
                continue
            off = int(m.group(1), 16)
            if off in gesehen:
                continue
            gesehen.add(off)
            name = m.group(2)
            b = bytes(int(x, 16) for x in m.group(3).split())
            if name in ('Set', 'Ck') and len(b) >= 4 and b[1] == 1 and b[2] == 0:
                treffer.append((fn[4:8], sec, off, '%s(1,0,%d)' % (name, b[3]), ''))
                if name == 'Set':
                    zustand = b[3]
            elif name == 'Sce_em_set' and zustand is not None:
                treffer.append((fn[4:8], sec, off, 'Sce_em_set Typ 0x%02X' % b[2],
                                'mit Elza-Bit %d -> %s' % (zustand, 'CDEMD1' if zustand else 'CDEMD0')))
for t in treffer:
    print('%s %-6s @0x%05X %-22s %s' % t)
