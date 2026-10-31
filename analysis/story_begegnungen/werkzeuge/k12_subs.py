# Welche subs jeder Datei werden ueberhaupt gestartet (ohne Blick auf Bedingungen)?
# Start: main00, sub00, sub01 (Beleg 0x8003efb0/0x8003efd4/0x8003f080). Kanten: Evt_chain 0x03 pc[3], Evt_exec 0x04 pc[3], Gosub 0x18 pc[1],
# AOT sce=3 Nutzlast+3 (0x80043100). Danach: Textaufrufe, die nur in nie gestarteten subs stehen.
import sys, json, collections
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung')
import w
from w import u16
R = w.load()
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/'
rows = json.load(open(S + 'texte40.json', encoding='utf-8'))
T = {(r, i): t for r, i, t in rows if i is not None}
dead_lines = collections.defaultdict(set); live_lines = collections.defaultdict(set)
dead_subs = {}
for n, r in sorted(R.items()):
    subs = r['subs']
    reach = set()
    todo = [k for k in subs if k[0] == 'main'] + [('sub', 0), ('sub', 1)]
    while todo:
        k = todo.pop()
        if k in reach or k not in subs: continue
        reach.add(k)
        for pc, op, b, cond in subs[k]['ev']:
            if op == 0x03 or op == 0x04:
                todo.append(('sub', b[3]))
            elif op == 0x18:
                todo.append(('sub', b[1]))
            elif op == 0x2C and b[2] == 3:
                pay = b[22:] if b[3] & 0x80 else b[14:]
                todo.append(('sub', pay[3]))
            elif op == 0x46 and b[2] == 3:
                todo.append(('sub', b[4 + 3]))
    dead = sorted(k[1] for k in subs if k not in reach and k[0] == 'sub')
    nontrivial = []
    for i in dead:
        ev = subs[('sub', i)]['ev']
        if len(ev) > 1: nontrivial.append(i)
    dead_subs[n] = nontrivial
    for k, s in subs.items():
        for pc, op, b, cond in s['ev']:
            ids = []
            if op == 0x2B: ids.append(b[1])
            elif op == 0x2C and b[2] == 1:
                pay = b[22:] if b[3] & 0x80 else b[14:]
                ids.append(u16(pay, 0))
            elif op == 0x46 and b[2] == 1:
                ids.append(u16(b, 4))
            for i in ids:
                (live_lines if k in reach else dead_lines)[(n, i)].add(f'{k[0]}{k[1]:02d}@{pc:#x}')
print('Dateien mit nie gestarteten subs (mit Inhalt):')
for n, d in dead_subs.items():
    if d: print(' ', n, d)
print('--- Textaufrufe nur in nie gestarteten subs, je Datei:')
for k, v in sorted(dead_lines.items()):
    if k not in live_lines:
        print(' ', k[0], '#%d' % k[1], sorted(v)[:3], (T.get(k) or '')[:40])
# Je Raumpaar: Zeilen, die in KEINER Datei lebend aufgerufen werden, aber einen toten Aufruf haben
pairs = collections.defaultdict(lambda: [False, False])
for (n, i) in set(list(dead_lines) + list(live_lines)):
    pairs[(n[:3], i)][0] = pairs[(n[:3], i)][0] or (n, i) in live_lines
    pairs[(n[:3], i)][1] = pairs[(n[:3], i)][1] or (n, i) in dead_lines
print('--- Nur tote Aufrufe je Raumpaar:')
for k, (lv, dd) in sorted(pairs.items()):
    if dd and not lv: print(' ', k)
