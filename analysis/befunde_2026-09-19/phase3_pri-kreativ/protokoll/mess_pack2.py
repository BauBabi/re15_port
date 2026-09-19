# Gegenprobe: MaxRects-BSSF statt Regalverfahren fuer dieselben Rechtecklisten.
import os, sys, json, struct, itertools
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, atlas, bau_p2, raum as R

W = H_ = 256


def maxrects(boxes, w=W, h=H_):
    """MaxRects, Heuristik Best-Short-Side-Fit, Eingabe nach Flaeche absteigend.
    -> (place {i:(x,y)}, rejected [i])"""
    frei = [(0, 0, w, h)]
    place, rej = {}, []
    order = sorted(range(len(boxes)), key=lambda i: -(boxes[i][2] * boxes[i][3]))
    for i in order:
        bw, bh = boxes[i][2], boxes[i][3]
        best = None
        for (fx, fy, fw, fh) in frei:
            if bw <= fw and bh <= fh:
                ss = min(fw - bw, fh - bh)
                ls = max(fw - bw, fh - bh)
                if best is None or (ss, ls) < (best[0], best[1]):
                    best = (ss, ls, fx, fy)
        if best is None:
            rej.append(i)
            continue
        _, _, px, py = best
        place[i] = (px, py)
        neu = []
        for (fx, fy, fw, fh) in frei:
            if px >= fx + fw or px + bw <= fx or py >= fy + fh or py + bh <= fy:
                neu.append((fx, fy, fw, fh)); continue
            if px > fx:
                neu.append((fx, fy, px - fx, fh))
            if px + bw < fx + fw:
                neu.append((px + bw, fy, fx + fw - (px + bw), fh))
            if py > fy:
                neu.append((fx, fy, fw, py - fy))
            if py + bh < fy + fh:
                neu.append((fx, py + bh, fw, fy + fh - (py + bh)))
        # enthaltene Rechtecke entfernen
        neu.sort(key=lambda r: -(r[2] * r[3]))
        sauber = []
        for r in neu:
            if not any(r[0] >= s[0] and r[1] >= s[1] and r[0] + r[2] <= s[0] + s[2]
                       and r[1] + r[3] <= s[1] + s[3] for s in sauber):
                sauber.append(r)
        frei = sauber
    return place, rej


ROOM, CUT = "ROOM10E0", 7
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
e = aus[ROOM][str(CUT)]
Rm, t, H = geom.cut_view(rdt, cam, CUT)
objekte = []
for o in e["objekte"]:
    r, q = bau_p2.objekt_region(o, bg)
    y0 = bau_p2.ebene_des_objekts(o, rdt, cam, CUT, rid, [])
    band = int(round(-y0 / float(geom.BAND_HOEHE)))
    r = bau_p2.kaesten_anwenden(r, o, e)
    vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, r, y0, band, o, None, [])
    objekte.append((o.get("name"), r, geometrie.rastern(vzm) * r))
for s in (1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48):
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
    treffer = None
    for komb in sorted(itertools.product(*[range(len(o)) for o in opts]),
                       key=lambda kb: sum(opts[i][j][2] for i, j in enumerate(kb))):
        n = sum(opts[i][j][1] for i, j in enumerate(komb))
        fl = sum(opts[i][j][2] for i, j in enumerate(komb))
        if n > 105 or fl > 65536:
            continue
        rects = []
        for i, j in enumerate(komb):
            rects += opts[i][j][3]
        boxes = [(r[0], r[1], r[2], r[3]) for r in rects]
        pl, rej = maxrects(boxes)
        if not rej:
            treffer = (s, n, fl)
            break
    print("Stufe %2d: %s" % (s, "PASST mit n=%d, %d Atlaspunkte (%.0f%%)" % (treffer[1], treffer[2], 100 * treffer[2] / 65536.0) if treffer else "keine Kombination packbar"))
