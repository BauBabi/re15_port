# Die vier UNANGETASTETEN Cuts (ROOM10F0 C4/C5, ROOM1100 C1/C2) mit derselben Abnahme
# durchrechnen wie alle anderen — nur pruefen, nichts schreiben.
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import raum as R
import bau_p2, abnahme, geom

R.P2_UNANGETASTET = {}
FAELLE = [("ROOM10F0", 4), ("ROOM10F0", 5), ("ROOM1100", 1), ("ROOM1100", 2)]
aus = json.load(open(R.AUSWAHL, encoding="utf-8"))
for (room, cut) in FAELLE:
    rid = int(room[4:], 16)
    rdt, _ = R.load_rdt(R.CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    bg = R.load_bg("build/bg_ppm", rid, cut)
    floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", rid)
    e = aus[room][str(cut)]
    print("=== %s C%d (%d Objekte)" % (room, cut, len(e.get("objekte") or [])))
    b = bau_p2.bau_cut(room, cut, e, rdt, cam, bg, floor, 16, np.max, None,
                       "build/p3/bilder_unangetastet", schreiben=False)
    for z in b.get("zeilen", []):
        print("   ", z)
    if "fehler" in b:
        print("   ⛔ %s" % b["fehler"]); continue
    s = b["schiene"]
    print("   Soll %d px, fehlt %d / zuviel %d | %d Rechtecke, %d Stufen (%d..%d) | "
          "VOR n=%d verdeckt=%d teil=%d | HINTER n=%d frei=%d -> %s"
          % (b["soll_px"], b["fehlt"], b["zuviel"], b["rects"], b["stufen"], b["tmin"], b["tmax"],
             s["VORn"], s["VORverd"], s["VORteil"], s["HINTn"], s["HINTfrei"],
             "BESTEHT" if b["ok"] else "FAELLT DURCH"))
    if s["vor_rest"]:
        print("   VOR-Rest: %s" % ", ".join("(%d,%d,%d,%.2f)" % q for q in s["vor_rest"][:8]))
    print("   Bild: %s" % b.get("bild"))
