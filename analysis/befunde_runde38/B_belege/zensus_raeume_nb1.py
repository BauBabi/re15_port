#!/usr/bin/env python3
"""Runde 38 Spur B, Nachbesserung 1: Raum-Zensus "Halte-Fenster x Gegner" (fuer die Integrations-Auswahl).

Raum zaehlt, wenn sein SCD (main + alle subs, scd_dump_room.py) ein Halte-Fenster haben KANN
(Set(2,7)=1 `22 02 07 01`, Set(1,27)=1 `22 01 1b 01`, Plc_motion 0x3F, Plc_dest 0x40) UND einen Gegner
setzt (Sce_em_set 0x44, Typ-Byte pc[2] < 0x40; Grenze @0x80050a78 sltiu 0x40).
Danach: welche Integrations-Skripte (tests/integration/*.cmake) beruehren einen dieser Raeume
(ROOMxxxx-Kennung, RE15_DEBUG_JUMP=xxxx, roomxxxx.rdt, #xxxx).
Aufruf: python zensus_raeume.py <baum>/re15_port <ausgabe>
"""
import os
import re
import subprocess
import sys

port = os.path.abspath(sys.argv[1])
out = sys.argv[2]
dump = os.path.join(port, 'tools', 'scd_dump_room.py')
psx = os.path.join(port, 'shared_assets', 'PSX')

rx_set = re.compile(r'\bSet\s+22 (02 07|01 1b) 01\b')
rx_plc = re.compile(r'\b(Plc_motion|Plc_dest)\b')
rx_em = re.compile(r'\bSce_em_set\s+44 ([0-9a-f]{2}) ([0-9a-f]{2})\b')

raeume = {}
for st in sorted(os.listdir(psx)):
    d = os.path.join(psx, st)
    if not (st.upper().startswith('STAGE') and os.path.isdir(d)):
        continue
    for fn in sorted(os.listdir(d)):
        m = re.match(r'ROOM([0-9A-F]{4})\.RDT$', fn, re.I)
        if not m:
            continue
        r = subprocess.run([sys.executable, '-I', dump, os.path.join(d, fn)], capture_output=True, text=True)
        txt = r.stdout
        halt_set = bool(rx_set.search(txt))
        halt_plc = bool(rx_plc.search(txt))
        typen = sorted({int(t, 16) for _, t in rx_em.findall(txt) if 0 < int(t, 16) < 0x40})
        if (halt_set or halt_plc) and typen:
            raeume[m.group(1).upper()] = (halt_set, halt_plc, typen)

integ = os.path.join(port, 'tests', 'integration')
treffer = {}
for fn in sorted(os.listdir(integ)):
    if not fn.endswith('.cmake'):
        continue
    txt = open(os.path.join(integ, fn), encoding='utf-8', errors='replace').read()
    ids = set()
    for m in re.finditer(r'(?:ROOM|room|JUMP=|GOTO_ROOM=|#)([0-9A-Fa-f]{4})\b', txt):
        ids.add(m.group(1).upper())
    hit = sorted(i for i in ids if i in raeume)
    if hit:
        treffer[fn] = hit

with open(out, 'w', encoding='utf-8') as f:
    f.write(f'# Raeume mit Halte-Fenster UND Gegner (Typ < 0x40): {len(raeume)}\n')
    for k in sorted(raeume):
        hs, hp, ty = raeume[k]
        f.write(f'ROOM{k}\tset27={int(hs)}\tplc={int(hp)}\ttypen={" ".join("%02X" % t for t in ty)}\n')
    f.write(f'# Integrations-Skripte, die einen dieser Raeume nennen: {len(treffer)}\n')
    for k in sorted(treffer):
        f.write(f'{k}\t{" ".join(treffer[k])}\n')
print(len(raeume), 'Raeume;', len(treffer), 'Skripte')
