#!/usr/bin/env python3
"""r30_diary_item_id_zensus.py - welche Item-Ids platziert RE1.5 ueberhaupt?
Laeuft scd_dump_room.py (opcode-exakter Walker) ueber alle ROOM*.RDT und liest aus jedem
Item_aot_set (0x50) die Item-Id: pc[14] (Kurzform, 22 B) bzw. pc[22] (Langform, 30 B,
pc[3] & 0x80) - dieselben Felder wie re15_port/engine/src/scd_vm.c.
FRAGE: gibt es im Auslieferungsstand eine Platzierung mit Id >= 0x48 (FILE-Bereich)?
"""
import glob, os, re, subprocess, sys
from collections import Counter
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
TOOL = os.path.join(REPO, "re15_port", "tools", "scd_dump_room.py")
ids = Counter(); wo = {}
raeume = sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT")))
abbruch = 0
for r in raeume:
    out = subprocess.run([sys.executable, TOOL, r], capture_output=True, text=True, errors="replace").stdout
    if "ABGEBROCHEN" in out or "abgebrochen" in out:
        abbruch += 1
    for ln in out.split("\n"):
        if "Item_aot_set" not in ln:
            continue
        m = re.search(r"Item_aot_set\s+((?:[0-9a-f]{2} )+)", ln)
        if not m:
            continue
        b = [int(x, 16) for x in m.group(1).split()]
        lang = (b[3] & 0x80) != 0
        iid = b[22] if lang else b[14]
        ids[iid] += 1
        wo.setdefault(iid, os.path.basename(r))
print("Raeume: %d   Item_aot_set gesamt: %d   (Walks mit Abbruch-Meldung: %d)" % (len(raeume), sum(ids.values()), abbruch))
print("hoechste platzierte Item-Id: 0x%02x" % max(ids))
print("Platzierungen mit Id >= 0x48: %d" % sum(n for i, n in ids.items() if i >= 0x48))
for i in sorted(ids):
    print("  0x%02x  x%-3d  z.B. %s" % (i, ids[i], wo[i]))
