#!/usr/bin/env python3
"""Auswertung RE15_SPINNEN_LOG (Runde 38 Integration, versteckte Schachtspinne): je Bild Spieler-Blick pr, Zielhilfe zh,
je Spinne Zustand/grid/y/hp/+0x93. Zaehlt: Bilder mit Zielhilfe auf einer Spinne im Schacht (grid & 0x60), Treffer
(HP-Abfall) auf eine Spinne im Schacht bzw. danach, Zustandsfolge, Schweben (y < bu - 100 ausserhalb Zustand 4/1).
Aufruf: ana_versteckt.py <spinnen.log> <raum hex> [ab_bild]"""
import re, sys, collections
log, raum = sys.argv[1], sys.argv[2].upper()
ab = int(sys.argv[3]) if len(sys.argv) > 3 else 0
pl_re = re.compile(r"F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+) hp=(-?\d+).*? fx=(-?\d+) pr=(-?\d+) zh=(-?\d+)")
sp_re = re.compile(r"\[(\d+) t=(\w+) st=(\d+) s=(\d+)\.(\d+)\.(\d+) g=(\w+) fl=(\d+) m=(\d+) gy=(-?\d+) bo=(-?\d+) bu=(-?\d+) mo=(-?\d+) af=(\d+) @\((-?\d+),(-?\d+),(-?\d+)\) r=\((-?\d+),(-?\d+),(-?\d+)\) hp=(-?\d+) h93=(\w+)\]")
prev_hp = {}
pr0 = None; pr_last = None
zh_versteckt = collections.Counter(); zh_offen = collections.Counter()
treffer_versteckt = collections.Counter(); treffer_offen = collections.Counter()
zustaende = collections.defaultdict(list)
schwebt = collections.Counter(); bilder = 0
for line in open(log, encoding="latin-1"):
    m = pl_re.search(line)
    if not m or m.group(2).upper() != raum: continue
    f = int(m.group(1))
    if f < ab: continue
    bilder += 1
    pr = int(m.group(8)); zh = int(m.group(9))
    if pr0 is None: pr0 = pr
    pr_last = pr
    for s in sp_re.finditer(line):
        slot = int(s.group(1)); t = s.group(2)
        if t != "25": continue
        st, s1, s3 = int(s.group(3)), int(s.group(4)), int(s.group(6))
        g = int(s.group(7), 16); y = int(s.group(16)); bu = int(s.group(12)); hp = int(s.group(21)); h93 = int(s.group(22), 16)
        versteckt = (g & 0x60) != 0
        if zh == slot:
            (zh_versteckt if versteckt else zh_offen)[slot] += 1
        if slot in prev_hp and hp < prev_hp[slot][0] and prev_hp[slot][0] > 0:
            (treffer_versteckt if prev_hp[slot][1] else treffer_offen)[slot] += 1
        prev_hp[slot] = (hp, versteckt)
        key = (st, s1, g)
        if not zustaende[slot] or zustaende[slot][-1][1] != key:
            zustaende[slot].append((f, key, y, h93))
        if not (st == 4 and s1 == 1) and y < bu - 100 and int(s.group(9)) != 1:
            schwebt[slot] += 1
print("Raum %s ab Bild %d: %d Bilder; Blick erstes Bild 0x%03X, letztes 0x%03X" % (raum, ab, bilder, (pr0 or 0) & 0xffff, (pr_last or 0) & 0xffff))
for slot in sorted(set(list(zustaende.keys()))):
    print("  Spinne Slot %d: Zielhilfe auf sie VERSTECKT (grid&0x60) %d Bilder, offen %d Bilder | Treffer versteckt %d, offen %d | "
          "schwebt (y < bu-100 ausserhalb 4/1, ohne Sprung) %d Bilder" % (slot, zh_versteckt[slot], zh_offen[slot],
          treffer_versteckt[slot], treffer_offen[slot], schwebt[slot]))
    for (f, (st, s1, g), y, h93) in zustaende[slot][:14]:
        print("      F%-4d Zustand %d/%d grid 0x%02X y %d +0x93 %02X" % (f, st, s1, g, y, h93))
