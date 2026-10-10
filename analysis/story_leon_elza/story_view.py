#!/usr/bin/env python3
"""story_view.py - kompakte Story-Sicht je Raum aus census.json: je Unterprogramm die
Nachrichten, NPC-Spawns (Typ>=0x40), gesetzten Flags, Tueren, Items, Ereignis-Aufrufe -
jeweils mit Bedingung und Datei-Offset. Aufruf: python story_view.py 1031 1041 ..."""
import json, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
c = json.load(open(os.path.join(HERE, "census.json"), encoding="utf-8"))
items = json.load(open(os.path.join(HERE, "item_namen.json")))
def line(e):
    k = e["k"]
    if k == "msg": return "MSG %d \"%s\"" % (e["id"], e["text"][:110])
    if k == "em":
        if e["typ"] < 0x40: return None
        return "EM slot%d TYP 0x%02X pos=%s" % (e["slot"], e["typ"], tuple(e["pos"]))
    if k == "set": return "SET (%d,0x%02X)=%d" % (e["bank"], e["bit"], e["val"])
    if k == "door": return "DOOR slot%d sce%d -> ROOM%s" % (e["slot"], e["sce"], e["dest"])
    if k == "item": return "ITEM slot%d 0x%02X %s x%d" % (e["slot"], e["item"], items.get("%02X" % e["item"], "?"), e["n"])
    if k == "evt": return "EVT_EXEC sub%02d" % e["sub"]
    if k == "aot" : return "AOT slot%d sce%d ->sub%02d" % (e["slot"], e["sce"], e["sub"])
    if k == "aot_reset" and "sub" in e: return "AOT_RESET slot%d sce%d ->sub%02d" % (e["slot"], e["sce"], e["sub"])
    if k == "aot_reset": return "AOT_RESET slot%d sce%d (Tuer)" % (e["slot"], e["sce"])
    if k == "aot_text": return "AOT_TEXT slot%d msg%d \"%s\"" % (e["slot"], e["msg"], e["text"][:70])
    if k == "aot_reset_text": return "AOT_RESET_TEXT slot%d msg%d" % (e["slot"], e["msg"])
    return None
emcount = {}
for rid in sys.argv[1:]:
    rid = rid.upper()
    print("##### ROOM%s" % rid)
    last = None
    zcount = {}
    for e in c.get(rid, []):
        if e["k"] == "em" and e["typ"] < 0x40:
            zcount[(e["reg"], e["typ"], tuple(e["cond"]))] = zcount.get((e["reg"], e["typ"], tuple(e["cond"])), 0) + 1
        s = line(e)
        if not s: continue
        if e["reg"] != last:
            print("  -- %s" % e["reg"]); last = e["reg"]
        cond = (" [" + " & ".join(e["cond"]) + "]") if e["cond"] else ""
        print("    @0x%05X %s%s" % (e["pc"], s, cond))
    for (reg, t, cond), n in zcount.items():
        print("  (Gegner %s: %d x Typ 0x%02X%s)" % (reg, n, t, (" [" + " & ".join(cond) + "]") if cond else ""))
