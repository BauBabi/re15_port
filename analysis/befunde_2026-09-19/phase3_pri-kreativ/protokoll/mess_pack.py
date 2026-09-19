# Messung: Flaechenbedarf und Packbarkeit fuer ROOM10E0 C7 ueber alle Tiefenstufen.
import os, sys, json, struct, itertools
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, atlas, bau_p2, raum as R

ROOM, CUT = "ROOM10E0", 7
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
e = aus[ROOM][str(CUT)]
Rm, t, H = geom.cut_view(rdt, cam, CUT)
objekte = []
soll = np.zeros((240, 320), bool)
for o in e["objekte"]:
    r, q = bau_p2.objekt_region(o, bg)
    y0 = bau_p2.ebene_des_objekts(o, rdt, cam, CUT, rid, [])
    band = int(round(-y0 / float(geom.BAND_HOEHE)))
    r = bau_p2.kaesten_anwenden(r, o, e)
    vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, r, y0, band, o, None, [])
    dep = geometrie.rastern(vzm) * r
    objekte.append((o.get("name"), r, dep))
    soll |= r
print("Soll %d Punkte, %d Objekte" % (int(soll.sum()), len(objekte)))
for s in (1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64):
    opts = []
    for (n_, reg, dep) in objekte:
        gx, gy = geometrie._gradient(reg, dep)
        sp = gx >= gy
        o2 = []
        for k in geometrie.KANTEN:
            rs = geometrie._zerlege_objekt(reg, dep, s, sp, k, np.max)
            if rs:
                o2.append((k, len(rs), sum(x[2] * x[3] for x in rs), rs))
        opts.append(o2)
    best = None
    for komb in itertools.product(*[range(len(o)) for o in opts]):
        n = sum(opts[i][j][1] for i, j in enumerate(komb))
        fl = sum(opts[i][j][2] for i, j in enumerate(komb))
        if n <= 105 and (best is None or fl < best[0]):
            best = (fl, n, komb)
    if best is None:
        print("Stufe %2d: keine Kombination mit n<=105" % s)
        continue
    fl, n, komb = best
    rects = []
    for i, j in enumerate(komb):
        rects += opts[i][j][3]
    boxes = [(r[0], r[1], r[2], r[3]) for r in rects]
    pl, rej = atlas.shelf_pack(boxes)
    print("Stufe %2d: n=%3d Flaeche=%6d (%.0f%% des Blattes)  shelf_pack: %d abgewiesen"
          % (s, n, fl, 100 * fl / 65536.0, len(rej)))
