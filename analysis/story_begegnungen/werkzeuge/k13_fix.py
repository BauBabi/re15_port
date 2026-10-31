# Fixpunkt ueber die Disc: Welche subs und Textaufrufe bleiben lebendig, wenn zusaetzlich Bedingungen Ck(b,n)=1 auf Bank 3/4 und
# auf Kill-Bits der falschen Bank nie wahr werden koennen? Bank 5 und alles uebrige gilt als erfuellbar (vorsichtig, also Obergrenze).
import sys, json, re, collections
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung')
import w
from w import u16
R = w.load()
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/'
rows = json.load(open(S + 'texte40.json', encoding='utf-8'))
T = {(r, i): t for r, i, t in rows if i is not None}
ckre = re.compile(r'Ck\((\d+),(\d+)\)=1')
BLOCK5 = {}
def blocked(cond, stage, sat, room=None):
    for c in cond:
        if c.startswith('NICHT'):
            continue
        for b, n in ckre.findall(c):
            b, n = int(b), int(n)
            if b == 5 and room is not None and (room, n) in BLOCK5:
                return (5, n)
            if b in (3, 4) and (b, n) not in sat:
                return (b, n)
            if (b == 8 and stage <= 3) or (b == 7 and stage >= 4):
                return (b, n)
    return None
sat = set()
# Startmenge: alle Schreiber mit Wert 1 aus allen Dateien
for n, r in R.items():
    for k, s in r['subs'].items():
        for pc, op, b, cond in s['ev']:
            if op == 0x22 and b[3] == 1 and b[1] in (3, 4):
                sat.add((b[1], b[2]))
for runde in range(12):
    live_sets = set()
    reach_all = {}
    for n, r in R.items():
        stage = int(n[0])
        subs = r['subs']
        reach = set()
        todo = [k for k in subs if k[0] == 'main'] + [('sub', 0), ('sub', 1)]
        while todo:
            k = todo.pop()
            if k in reach or k not in subs: continue
            reach.add(k)
            for pc, op, b, cond in subs[k]['ev']:
                if blocked(cond, stage, sat, n): continue
                if op == 0x03 or op == 0x04: todo.append(('sub', b[3]))
                elif op == 0x18: todo.append(('sub', b[1]))
                elif op == 0x2C and b[2] == 3:
                    pay = b[22:] if b[3] & 0x80 else b[14:]
                    todo.append(('sub', pay[3]))
                elif op == 0x46 and b[2] == 3: todo.append(('sub', b[7]))
                elif op == 0x22 and b[3] == 1 and b[1] in (3, 4):
                    live_sets.add((b[1], b[2]))
        reach_all[n] = reach
    # Bank 5: fuer jede Datei pruefen
    nb5 = {}
    for n, r in R.items():
        stage = int(n[0]); subs = r['subs']
        wr = collections.defaultdict(lambda: [0, 0])
        for k, s_ in subs.items():
            for pc, op, b, cond in s_['ev']:
                if op == 0x22 and b[1] == 5 and b[3] == 1:
                    wr[b[2]][0] += 1
                    if k in reach_all[n] and not blocked(cond, stage, sat, n):
                        wr[b[2]][1] += 1
                if op in (0x2C,) and b[2] == 4:
                    pay = b[22:] if b[3] & 0x80 else b[14:]
                    wr[pay[2]][1] += 1
        for bit, (a, c) in wr.items():
            if a and not c:
                nb5[(n, bit)] = 1
    if live_sets == sat and nb5 == BLOCK5:
        print('Fixpunkt nach Runde', runde); break
    BLOCK5.clear(); BLOCK5.update(nb5)
    print('Runde', runde, 'entfallen', sorted(sat - live_sets))
    sat = live_sets
dead = collections.defaultdict(list); live = collections.defaultdict(list)
for n, r in R.items():
    stage = int(n[0]); subs = r['subs']
    for k, s in subs.items():
        for pc, op, b, cond in s['ev']:
            ids = []
            if op == 0x2B: ids.append(b[1])
            elif op == 0x2C and b[2] == 1:
                pay = b[22:] if b[3] & 0x80 else b[14:]
                ids.append(u16(pay, 0))
            elif op == 0x46 and b[2] == 1: ids.append(u16(b, 4))
            for i in ids:
                bl = blocked(cond, stage, sat, n)
                if k in reach_all[n] and not bl:
                    live[(n, i)].append(f'{k[0]}{k[1]:02d}@{pc:#x}')
                else:
                    dead[(n, i)].append(f'{k[0]}{k[1]:02d}@{pc:#x}' + (f' wegen {bl}' if bl else ' sub nie gestartet'))
out = []
for k, v in sorted(dead.items()):
    if k not in live:
        out.append((k, v))
print('Tote Aufrufe je Datei', len(out))
paar = collections.defaultdict(lambda: [0, 0])
for (n, i) in set(list(dead) + list(live)):
    paar[(n[:3], i)][0] += (n, i) in live
    paar[(n[:3], i)][1] += (n, i) in dead
tot = sorted(k for k, (l, d) in paar.items() if l == 0)
print('Zeilen je Raumpaar nur tot:', len(tot))
for k in tot:
    ex = [x for x in out if x[0][0][:3] == k[0] and x[0][1] == k[1]]
    print(' ', k[0], '#%d' % k[1], ex[0][0][0] if ex else '', ex[0][1][:2] if ex else '', re.sub(r'⟦[^⟧]*⟧', '', (T.get(ex[0][0]) or '') if ex else '')[:38])
json.dump({'tot': tot}, open('k13_tot.json', 'w'))
print('--- Bank-3/4-Bedingungen =1 ohne Schreiber am Ende:')
seen = set()
for n, r in R.items():
    for k, s in r['subs'].items():
        for pc, op, b, cond in s['ev']:
            if op == 0x21 and b[3] == 1 and b[1] in (3, 4) and (b[1], b[2]) not in sat:
                seen.add((b[1], b[2], n, hex(pc)))
print(sorted(seen))
