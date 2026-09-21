"""EXTERNE GEGENPROBE: baut ROOM10D0 Cut 7 mit der SPALTENREGEL (das verworfene Modell),
schreibt die Dateien in ein SEPARATES Verzeichnis und meldet, was der Riegel daran sehen
wuerde. Die ausgelieferten Dateien werden NICHT angefasst.
"""
import os, sys, json, struct, shutil
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, bau_p2, abnahme, raum

CD = "re15_port/shared_assets/PSX"
OUT = "build/r19b/gegenprobe_MASKS"
os.makedirs(OUT, exist_ok=True)

aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
e = json.loads(json.dumps(aus["ROOM10D0"]["7"]))
for o in e["objekte"]:
    if o.get("png", "").endswith("07_01.png"):
        o.pop("aufrecht", None)
        o["aufrecht"] = "spalten"      # <- das verworfene Modell
        o["zelle"] = False
        print("Objekt '%s' auf die Spaltenregel gestellt" % o["name"])

rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = geom.load_bg("build/bg_ppm", 0x10D0, 7)
floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", 0x10D0)
stufe = 16                      # wie der Auslieferungsbau, bau_alle.sh: --stufe 16
stat = raum.P2_STATISTIK["max"]  # --statistik max
b = bau_p2.bau_cut("ROOM10D0", 7, e, rdt, cam, bg, floor, stufe, stat,
                   OUT, "build/r19b", schreiben=True)
for z in b.get("zeilen", []):
    print("  ", z)
print("Bericht:", {k: v for k, v in b.items() if k in
      ("fehler", "ok", "rects", "stufen", "tmin", "tmax", "fehlt", "zuviel", "soll_px")})
print("geschrieben nach", OUT, ":", sorted(os.listdir(OUT)))
