#!/usr/bin/env python3
"""szenen.py - Szenen-Tabelle: jedes Unterprogramm mit Dialog (Steuercode 04:00) oder NPC-Spawn,
mit AUSLOESER (wer ruft das Sub: Evt_exec / Aot_set / Aot_reset, samt Bedingung) und WIRKUNG
(gesetzte Flags). Quelle census.json (scd_story.py) + texte_alle.json (dump_texte.py).
Aufruf: python szenen.py [--var 0|1]  -> szenen_leon.txt / szenen_elza.txt"""
import json, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
c = json.load(open(os.path.join(HERE, "census.json"), encoding="utf-8"))
t = json.load(open(os.path.join(HERE, "texte_alle.json"), encoding="utf-8"))


def dialog_ids(rid):
    return {m["id"] for m in t[rid]["msgs"] if m["mode"] == "00"}


def run(var):
    out = []
    for rid in sorted(c):
        if int(rid[-1], 16) != var:
            continue
        ev = c[rid]
        dlg = dialog_ids(rid)
        regs = {}
        for e in ev:
            regs.setdefault(e["reg"], []).append(e)
        callers = {}
        for e in ev:
            if e["k"] in ("evt", "aot", "aot_reset") and "sub" in e:
                callers.setdefault(e["sub"], []).append(e)
        for reg, es in regs.items():
            msgs = [e for e in es if e["k"] == "msg" and e["id"] in dlg]
            ems = [e for e in es if e["k"] == "em" and e["typ"] >= 0x40]
            if not msgs and not ems:
                continue
            out.append("ROOM%s %s" % (rid, reg))
            if reg.startswith("sub"):
                n = int(reg[3:])
                for cl in callers.get(n, []):
                    out.append("   AUSLOESER %s @0x%05X %s%s" % (
                        cl["reg"], cl["pc"], cl["k"], (" [" + " & ".join(cl["cond"]) + "]") if cl["cond"] else ""))
            for e in ems:
                out.append("   NPC  @0x%05X slot%d Typ 0x%02X pos=%s%s" % (
                    e["pc"], e["slot"], e["typ"], tuple(e["pos"]), (" [" + " & ".join(e["cond"]) + "]") if e["cond"] else ""))
            if msgs:
                out.append("   DIALOG msg %d..%d: \"%s\" ... \"%s\"" % (msgs[0]["id"], msgs[-1]["id"], msgs[0]["text"][:70], msgs[-1]["text"][:70]))
            sets = ["(%d,0x%02X)=%d@0x%05X" % (e["bank"], e["bit"], e["val"], e["pc"]) for e in es
                    if e["k"] == "set" and e["bank"] not in (1, 2, 5)]
            if sets:
                out.append("   SETZT " + " ".join(sets))
    return out


if __name__ == "__main__":
    for var, name in ((0, "szenen_leon.txt"), (1, "szenen_elza.txt")):
        o = run(var)
        open(os.path.join(HERE, name), "w", encoding="utf-8").write("\n".join(o) + "\n")
        print(name, len(o))
