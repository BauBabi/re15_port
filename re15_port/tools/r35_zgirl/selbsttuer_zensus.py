#!/usr/bin/env python3
"""selbsttuer_zensus.py - Runde 35 Spur C: alle Door_aot_set (0x3B, 32 B) deren Ziel der EIGENE Raum ist
(Ziel-Id = ((stage+1)<<12)|(room<<4)|Variante, Formel aus aot_common.c aot_fire_door). Viereck-Saetze
(0x3B mit 40 B) liefert der Walker ueber op_size; die Nutzlast liegt dann bei pc+22+8 - hier nur 32-B-Saetze.
Ausgabe je Selbst-Tuer: Raum, Region, Offset, Slot, Ziel-Cut, Spawn; und ob der Port sie heute neu
einsteigen laesst (Bedingung aot_common.c: dest_room!=0 && 0x1000|room<<4|var == aktueller Raum)."""
import os, sys, glob, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as L
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "shared_assets", "PSX")
n_all = n_port = 0
per_stage = {}
for p in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x48: continue
    rid = int(os.path.basename(p)[4:8], 16)
    for (tag, idx), ops in sorted(L.regionen(d).items()):
        for pc, op, sz in ops:
            if op != 0x3B: continue
            o = pc + (8 if sz == 40 else 0)
            x, y, z = struct.unpack_from("<hhh", d, o + 14)
            st, rm, cut = d[o + 22], d[o + 23], d[o + 24]
            dest = ((st + 1) << 12) | (rm << 4) | (rid & 0xF)
            if dest != rid: continue
            port = (rm != 0 and (0x1000 | (rm << 4) | (rid & 0xF)) == rid)
            n_all += 1; n_port += port
            per_stage.setdefault(rid >> 12, [0, 0]); per_stage[rid >> 12][0] += 1; per_stage[rid >> 12][1] += port
            if len(sys.argv) > 1 and ("%04X" % rid) not in sys.argv[1:]: continue
            print("ROOM%04X %s%02d @0x%05x slot=%d cut=%d spawn=(%d,%d,%d) port_neu=%d" %
                  (rid, tag, idx, pc, d[pc + 1], cut, x, y, z, port))
print("Selbst-Tueren gesamt %d, davon steigt der Port heute neu ein: %d" % (n_all, n_port))
for s in sorted(per_stage): print("  STAGE%d: %d Selbst-Tueren, Port neu: %d" % (s, per_stage[s][0], per_stage[s][1]))
