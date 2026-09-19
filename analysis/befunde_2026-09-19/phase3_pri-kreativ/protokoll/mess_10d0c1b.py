# ROOM10D0 C1: welche SCA-Sperrzelle ist der Klapptisch? IoU der Quader-Silhouette
# gegen die Freistellung des Nutzers, ueber ALLE Zellen des Bands.
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, bau_p2, raum as R

ROOM, CUT = "ROOM10D0", 1
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
e = aus[ROOM][str(CUT)]
Rm, t, H = geom.cut_view(rdt, cam, CUT)
o = e["objekte"][0]
r, _ = bau_p2.objekt_region(o, bg)
zellen = geom.sca_sperrzellen(rdt, 0) or []
print("%d Sperrzellen im Band 0" % len(zellen))
erg = []
for z in zellen:
    hm = geometrie.hoehe_messen(r, Rm, t, H, z, 0)
    if hm is None:
        continue
    hoehe, iou, kante, saeule = hm
    if iou < 0.05:
        continue
    vz, tr = geometrie.tiefe_zelle(r, Rm, t, H, z, hoehe, saeule, 0)
    d = geometrie.rastern(vz) * r if vz is not None else None
    sp = [x for x in range(156, 177) if r[:, x].any()]
    mm = [int(d[:, x][r[:, x]].max()) for x in sp] if d is not None else []
    erg.append((iou, z, hoehe, kante, saeule, (min(mm), max(mm)) if mm else None,
                (int(d[r].min()), int(d[r].max())) if d is not None else None))
erg.sort(reverse=True)
for (iou, z, hoehe, kante, saeule, sp, ges) in erg[:8]:
    print("  IoU %.2f  Zelle x%d..%d z%d..%d (Typ %d)  Hoehe %d  Oberkante %s px  %s | "
          "Tiefe gesamt %s | Spalten 156..176 max %s"
          % (iou, z[0], z[0] + z[2], z[1], z[1] + z[3], z[4], hoehe,
             "-" if kante is None else "%.1f" % kante, "Saeule" if saeule else "Quader",
             ges, sp))
