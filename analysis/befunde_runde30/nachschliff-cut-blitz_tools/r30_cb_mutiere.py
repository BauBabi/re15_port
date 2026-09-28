"""Mutationsprobe fuer integration_r30_cut_blitz (Runde 30 cut-blitz, Dossier 8.3).

r30_cb_mutiere.py <M1|M2|M3>   mutiert re15_port/platform/pc/main.c im Baum dieses Skripts:
  M1 Montage-Schalter hinter den Apply, M2 gecullte NPCs vor der Pose verwerfen,
  M3 Apply hinter den Hintergrund-Blit. Danach bauen, Riegel laufen lassen,
  git checkout -- re15_port/platform/pc/main.c.
"""
import os, sys
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "re15_port", "platform", "pc", "main.c")
s = open(p, "rb").read().decode("utf-8")
CR = "\r\n"
APPLY = "        if (md1_ok) pc_cam_present_apply(&rdt, rdt_ok);" + CR
m = sys.argv[1]

def once(old):
    c = s.count(old)
    if c != 1:
        sys.exit("anker %d: %r" % (c, old[:60]))

if m == "M1":   # Montage-Schalter hinter den Apply (der gemeldete Fehler)
    once(APPLY)
    anchor = "        int mfx_frisch = 0;" + CR
    once(anchor)
    s = s.replace(APPLY, "")
    s = s.replace(anchor, APPLY + anchor)
elif m == "M2":  # gecullte NPCs wieder VOR der Pose verwerfen
    old = "if (!g5_front_in) npc_region_culled = 1;"
    once(old)
    s = s.replace(old, "if (!g5_front_in) continue;")
elif m == "M3":  # Apply wieder hinter den Hintergrund-Blit
    once(APPLY)
    anchor = "        /* PRE-INTRO " + chr(0x2192) + " HELIPAD handoff: once main00's narrator"
    once(anchor)
    s = s.replace(APPLY, "")
    s = s.replace(anchor, APPLY + anchor)
else:
    sys.exit("unbekannt")
open(p, "wb").write(s.encode("utf-8"))
print("mutiert", m)
