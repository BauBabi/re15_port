#!/usr/bin/env python3
"""reentry_zensus.py - Runde 35 Spur C, Nachbesserung 1 (M2a): was laeuft in den Raeumen, die seit
U1 (aot_common.c, @0x8001d988 unbedingter Raumlader) bei einer Selbst-Tuer NEU einsteigen?

Je Raum (main + alle subs, opcode-exakt ueber scd_walk_lib): alle Sce_em_set (0x44) mit Typ, Slot,
Verhalten, Kill-Flag, und die Zahl der Obj_model_set (0x45). Das ist die Eingabe fuer den Zensus der
raumgebundenen Port-Module (Dossier C_zgirl.md, Nachbesserung 1 / M2a).
Aufruf: python reentry_zensus.py
"""
import os, sys, glob, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as L

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "shared_assets", "PSX")
# Raeume mit Selbst-Tueren, die vor U1 NICHT neu einstiegen (selbsttuer_zensus.py port_neu=0).
RAEUME = ["2040", "2041", "20A0", "20A1", "30E0", "4000", "4050", "4051", "40A0", "40A1",
          "5090", "5091", "6030", "6031"]
for room in RAEUME:
    p = glob.glob(os.path.join(ROOT, "STAGE*", "ROOM%s.RDT" % room))[0]
    d = open(p, "rb").read()
    regs = L.regionen(d)
    typen = {}
    n45 = 0
    for (tag, idx), ops in sorted(regs.items()):
        for pc, op, sz in ops:
            if op == 0x45:
                n45 += 1
            if op != 0x44:
                continue
            slot, typ, beh, kf = d[pc+1], d[pc+2], d[pc+3], d[pc+7]
            typen.setdefault(typ, []).append("%s%02d@0x%05x s%d b%02x kf%02x" % (tag, idx, pc, slot, beh, kf))
    print("ROOM%s Obj_model_set=%d Gegner-Typen=%s" % (room, n45, " ".join("0x%02x" % t for t in sorted(typen))))
    for t in sorted(typen):
        print("   0x%02x: %s" % (t, "; ".join(typen[t])))
