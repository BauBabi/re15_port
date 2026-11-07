#!/usr/bin/env python3
"""npc_hp_zensus.py - Abnahme 4 Spur AN: NB5 schuetzt Begleiter nur noch ueber hp < 0 (re15_damage.c, NPC 0x40..0x4D).
Durchsucht state.log-Dateien nach NPC-Aktoren (t=40..4d) mit hp >= 0 und meldet je Datei/Typ die Bildspanne.
  python npc_hp_zensus.py <wurzel> [...]"""
import sys, os, re, collections
pat = re.compile(r"\[(\d+) t=(4[0-9a-d]) st=(\d+) [^\]]*\] hp=(-?\d+)")
tot_npc = collections.Counter(); bad = collections.defaultdict(list)
files = []
for root in sys.argv[1:]:
    for dp, dn, fn in os.walk(root):
        for f in fn:
            if f == "state.log": files.append(os.path.join(dp, f))
for p in sorted(files):
    with open(p, "r", errors="replace") as fh:
        for line in fh:
            if not line.startswith("F"): continue
            fr = line.split(" ", 1)[0]
            for m in pat.finditer(line):
                slot, typ, st, hp = m.group(1), m.group(2), m.group(3), int(m.group(4))
                tot_npc[typ] += 1
                if hp >= 0: bad[(p, typ, slot)].append((fr, st, hp))
print("Dateien:", len(files))
print("NPC-Zeilen je Typ:", dict(tot_npc))
for (p, typ, slot), v in sorted(bad.items()):
    print("hp>=0: %s Typ 0x%s Slot %s: %d Bilder, erstes %s st %s hp %d, letztes %s st %s hp %d"
          % (os.path.relpath(p, sys.argv[1]), typ, slot, len(v), v[0][0], v[0][1], v[0][2], v[-1][0], v[-1][1], v[-1][2]))
