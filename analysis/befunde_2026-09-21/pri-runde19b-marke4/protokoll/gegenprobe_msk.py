"""Baut aus der ausgelieferten ROOM10D0.MSK einen Container, in dem NUR Cut 7 durch die
Spaltenregel-Sektion ersetzt ist — fuer die externe Gegenprobe des Riegels.
Die ausgelieferten Dateien werden hier nur GELESEN."""
import os, sys, json, struct
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
        o["aufrecht"] = "spalten"
        o["zelle"] = False

rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = geom.load_bg("build/bg_ppm", 0x10D0, 7)
floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", 0x10D0)
b = bau_p2.bau_cut("ROOM10D0", 7, e, rdt, cam, bg, floor, 16, raum.P2_STATISTIK["max"],
                   OUT, "build/r19b", schreiben=True)
print("Bau:", {k: v for k, v in b.items() if k in
      ("fehler", "ok", "rects", "stufen", "tmin", "tmax", "fehlt", "zuviel", "soll_px")})
print("Standplatz-Schiene der ALTEN Abnahme dazu:", b["schiene"]["VORn"], "VORn /",
      b["schiene"]["VORverd"], "VORverd /", b["schiene"]["HINTn"], "HINTn /",
      b["schiene"]["HINTfrei"], "HINTfrei")
print("   [!] Die alte Abnahme AKZEPTIERT dieses Modell (VORverd == 0) — sie hat kein")
print("      Kriterium fuer die Naht oder die Zeilen-Invariante.")

alt = raum._container_lesen(os.path.join(CD, "MASKS", "ROOM10D0.MSK"))
alt[7] = b["sektion"]
blob = geom.pack_container(alt, rdt[1])
open(os.path.join(OUT, "ROOM10D0.MSK"), "wb").write(blob)
print("geschrieben:", sorted(os.listdir(OUT)))
