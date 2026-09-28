#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: Zensus der RE1.5-'verschlossen'-Stellen ueber alle 240 RDTs
(re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT), opcode-exakt mit re15_port/tools/scd_walk_lib.py.

Erfasst wird:
 (1) jede Nachricht, deren Text auf ein Schloss deutet (Muster unten),
 (2) jeder MESSAGE-AOT (Aot_set/Aot_reset mit sce=1) auf so eine Nachricht  -> dort laeuft KEIN Skript
     (Handler LAB_80043084 @0x80043084: nur jal FUN_80027e68),
 (3) jeder Skriptblock mit Message_on auf so eine Nachricht, samt ALLER Se_on desselben Blocks,
 (4) Tuer-Zwillinge: derselbe AOT-Platz bekommt im selben Block einmal sce=1 (Text) und einmal
     Door_aot_set (Tuer) - das RE1.5-Schloss (analysis/door_lock_1170.md §3),
 (5) Keep_Item_ck / Sce_key_ck im selben Block (welcher Gegenstand oeffnet).
"""
import sys, os, glob, re, struct, collections
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as L
PAT = re.compile(r"lock|card|code|password|won't open|can't open|cannot|sealed|shut|key |key\.|keyhole|electronic|\bID\b|open", re.I)
KIND = [
 ('kartenleser', re.compile(r"card|\bID\b", re.I)),
 ('code',        re.compile(r"code|password|number", re.I)),
 ('elektronisch', re.compile(r"electronic", re.I)),
 ('andere_seite', re.compile(r"other side|from inside|from the inside", re.I)),
 ('schluessel',  re.compile(r"key", re.I)),
 ('verschlossen', re.compile(r"lock", re.I)),
]
def kind(t):
    for k, p in KIND:
        if p.search(t): return k
    return 'sonst'
files = sorted(glob.glob(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE*', 'ROOM*.RDT')))
msgaot = []; scripts = []; twins = []; allmsg = []; klein = []
se_all = collections.Counter(); n_se = 0; rooms_with_se = set()
for f in files:
    room = os.path.basename(f)[4:8]
    d = open(f, 'rb').read()
    if len(d) < 0x100:
        klein.append((room, len(d))); continue
    ms = L.messages(d)
    lockmsg = {i: t for i, (t, c) in ms.items() if PAT.search(t)}
    for i, t in lockmsg.items(): allmsg.append((room, i, t))
    reg = L.regionen(d)
    for (tag, idx), ops in sorted(reg.items()):
        blk = '%s%02d' % (tag, idx)
        ses = []; mons = []; items = []
        aot_msg = {}; aot_door = {}
        for pc, op, sz in ops:
            r = d[pc:pc+sz]
            if op == 0x36:
                ses.append((pc, r)); se_all[(r[1], r[2])] += 1; n_se += 1; rooms_with_se.add(room)
            elif op == 0x2B:
                mons.append((pc, r[1], r))
            elif op in (0x51, 0x5E):
                items.append((pc, op, r))
            elif op == 0x2C:
                sce = r[2]
                pay = r[14:] if not (r[3] & 0x80) else r[22:]
                if sce == 1:
                    mi = pay[0] | (pay[1] << 8)
                    aot_msg.setdefault(r[1], []).append((pc, mi, r))
                    if mi in lockmsg: msgaot.append((room, blk, pc, r[1], mi, lockmsg[mi], 'Aot_set'))
            elif op == 0x46:
                if r[2] == 1:
                    mi = r[4] | (r[5] << 8)
                    if mi in lockmsg: msgaot.append((room, blk, pc, r[1], mi, lockmsg[mi], 'Aot_reset'))
            elif op == 0x3B:
                pay = 14 if not (r[3] & 0x80) else 22
                aot_door.setdefault(r[1], []).append((pc, r[2], r[pay+8], r[pay+9], r[pay+10], r))
        for slot in aot_msg:
            if slot in aot_door:
                for pc, mi, r in aot_msg[slot]:
                    for dpc, dsce, st, rm, cut, dr in aot_door[slot]:
                        twins.append((room, blk, slot, pc, mi, ms.get(mi, ('?', []))[0], dpc, dsce, st, rm, cut))
        lk = [(pc, mi) for pc, mi, r in mons if mi in lockmsg]
        if lk: scripts.append((room, blk, lk, ses, items, lockmsg))
print('# %d RDTs, davon %d Platzhalter < 0x100 B: %s' % (len(files), len(klein), klein))
print('# Se_on gesamt: %d Aufrufe in %d Raeumen; (bank,id)-Verteilung der 12 haeufigsten: %s' % (
    n_se, len(rooms_with_se), ', '.join('(%d,0x%02x)x%d' % (k[0], k[1], v) for k, v in se_all.most_common(12))))
print()
print('### (1) Schloss-Nachrichten: %d' % len(allmsg))
kc = collections.Counter(kind(t) for _, _, t in allmsg)
print('#    nach Art: %s' % dict(kc))
for room, i, t in allmsg:
    print('ROOM%s msg[%2d] [%-12s] %s' % (room, i, kind(t), t[:150]))
print()
print('### (2) MESSAGE-AOTs (sce=1) auf Schloss-Nachrichten: %d' % len(msgaot))
for room, blk, pc, slot, mi, t, how in msgaot:
    print('ROOM%s %-6s @0x%05X slot %2d msg %2d %-9s [%-12s] %s' % (room, blk, pc, slot, mi, how, kind(t), t[:110]))
print()
print('### (3) Skriptbloecke mit Message_on auf Schloss-Nachrichten: %d' % len(scripts))
nse = 0
for room, blk, lk, ses, items, lockmsg in scripts:
    print('ROOM%s %-6s' % (room, blk))
    for pc, mi in lk:
        print('      Message_on @0x%05X msg %d [%s]: %s' % (pc, mi, kind(lockmsg[mi]), lockmsg[mi][:110]))
    for pc, op, r in items:
        print('      %s @0x%05X %s' % ('Sce_key_ck' if op == 0x51 else 'Keep_Item_ck', pc, r.hex(' ')))
    if not ses: print('      (kein Se_on im Block)')
    for pc, r in ses:
        nse += 1
        print('      Se_on @0x%05X %s  bank %d id 0x%02x' % (pc, r.hex(' '), r[1], r[2]))
print('#    Bloecke mit Se_on: %d von %d' % (sum(1 for s in scripts if s[3]), len(scripts)))
print()
print('### (4) Tuer-Zwillinge (sce=1-Text und Door_aot_set auf demselben Platz im selben Block): %d' % len(twins))
for room, blk, slot, pc, mi, t, dpc, dsce, st, rm, cut in twins:
    print('ROOM%s %-6s slot %2d  Text @0x%05X msg %2d  /  Tuer @0x%05X sce %d -> %d%02X0 cut %d  [%-12s] %s' % (
        room, blk, slot, pc, mi, dpc, dsce, st + 1, rm, cut, kind(t), t[:90]))
