"""Auswahl anwenden: aus Flaechennummern werden Maskendaten.

Gegenstueck zu blatt.py. Die Nummern kommen von dort — angesehen und benannt, nicht
geraten. Alles Weitere ist gemessene Mechanik: Rechteckzerlegung, Tiefe aus dem
Bodenkontakt je Bildspalte, Atlas aus dem Hintergrund gepackt, Ausgabe im
Original-Sektionsformat.

    --vergleich   prueft die Auswahl gegen die KUENSTLER-Masken desselben Cuts.
                  Nur fuer Cuts, die welche haben — das ist die Abnahmepruefung,
                  bevor man der Auswahl auf unbekannten Cuts glaubt.
    sonst         schreibt MASKS/<ROOM>.MSK + MASKS/<ROOM>_PRI<cc>.TIM

Aufruf:
    python re15_port/tools/maske/anwenden.py ROOM1150 1 --ids 37,38,43,46 --vergleich
    python re15_port/tools/maske/anwenden.py ROOM1140 0 --ids 12,13,14
"""
import argparse
import json
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import atlas as atlasmod
import geom
from geom import load_bg, load_rdt


def artist_region(rdt, cam, cut):
    """Vereinigung der Original-Maskenrechtecke, oder None."""
    po = struct.unpack_from("<I", rdt, cam + cut * 32 + 0x1C)[0]
    if po + 4 > len(rdt):
        return None
    gc, mc = struct.unpack_from("<HH", rdt, po)
    if gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256:
        return None
    p = po + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", rdt, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    reg = np.zeros((240, 320), bool)
    gi = used = 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = rdt[p], rdt[p + 1], rdt[p + 2], rdt[p + 3]
        dep, size = struct.unpack_from("<HH", rdt, p + 4); p += 8
        if (size & 0xf000) == 0:
            w, h = struct.unpack_from("<HH", rdt, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        X = ((dx + ax + 0x8000) & 0xffff) - 0x8000
        Y = ((dy + ay + 0x8000) & 0xffff) - 0x8000
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 > x0 and y1 > y0:
            reg[y0:y1, x0:x1] = True
    return reg


def build(rdt, cam, rid, cut, region, bg, out_dir, room):
    dep = geom.depth_map(rdt, cam, cut, region)
    if dep is None:
        return None
    # Das Zerlegen uebergrosser Kaesten (atlas.split_oversize) kann die Zahl erhoehen —
    # ein Bildschirmrechteck ist bis 320 breit, das Atlasblatt nur 256, also bis zu
    # zwei Stuecke je Kasten. Deshalb wird die Anforderung so lange gesenkt, bis das
    # ZERLEGTE Ergebnis in das Budget passt. NICHT abschneiden: ein weggelassener
    # Kasten ist fehlende Verdeckung, und genau so entstand der Befund
    # "die Fuesse stehen auf dem Tisch".
    budget = geom.MAX_MASKS_PER_CUT
    while budget >= 4:
        boxes = geom.rects_from_mask(region, budget)
        if not boxes:
            return None
        if len(atlasmod.split_oversize(boxes)[0]) <= geom.MAX_MASKS_PER_CUT:
            break
        budget -= 4
    tim, place, boxes = atlasmod.build(bg, region, boxes)
    if tim is None:
        return None
    groups, masks = [], []
    for i, (x, y, w, h) in enumerate(boxes):
        if i not in place:
            continue
        ax, ay = place[i]
        win = dep[y:y + h, x:x + w]
        if not (win > 0).any():
            continue
        groups.append((1, x - ax, y - ay))
        masks.append((ax, ay, x, y, w, h, int(np.median(win[win > 0]))))
    if not masks:
        return None
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
        open(os.path.join(out_dir, "%s_PRI%02d.TIM" % (room, cut)), "wb").write(tim)
    return geom.pack_section(groups, masks), len(masks)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int)
    ap.add_argument("--ids", required=True, help="Flaechennummern, z.B. 37,38,43")
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--blatt", default="build/blaetter")
    ap.add_argument("--out", default="re15_port/shared_assets/PSX/MASKS")
    ap.add_argument("--vergleich", action="store_true")
    a = ap.parse_args()

    room = a.room.upper()
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(a.cd, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    seg = np.load(os.path.join(a.blatt, "%s_%02d_seg.npy" % (room, a.cut)))
    bg = load_bg(a.ppm, rid, a.cut)
    ids = [int(x) for x in a.ids.replace(" ", "").split(",") if x]
    region = np.isin(seg, ids)
    print("  Auswahl: %d Flaechen -> %.1f %% des Bildes" % (len(ids), 100 * region.mean()))

    if a.vergleich:
        truth = artist_region(rdt, cam, a.cut)
        if truth is None:
            print("  Cut hat keine Kuenstler-Masken — Vergleich nicht moeglich")
            return 1
        hit = int((region & truth).sum())
        print("  PRAEZISION %.1f %%  (Anteil unserer Flaeche, der auch Kuenstler-Flaeche ist)"
              % (100.0 * hit / max(1, region.sum())))
        print("  AUSBEUTE   %.1f %%  (Anteil ihrer Flaeche, den wir treffen)"
              % (100.0 * hit / max(1, truth.sum())))
        print("  unsere Flaeche %d px, ihre %d px" % (int(region.sum()), int(truth.sum())))
        return 0

    r = build(rdt, cam, rid, a.cut, region, bg, a.out, room)
    if not r:
        print("  nichts erzeugt")
        return 1
    sec, n = r
    path = os.path.join(a.out, "%s.MSK" % room)
    secs = {}
    if os.path.exists(path):
        b = open(path, "rb").read()
        if b[:4] == b"R15M":
            _, nc = struct.unpack_from("<II", b, 4)
            offs = struct.unpack_from("<%dI" % nc, b, 12)
            ends = sorted([o for o in offs if o] + [len(b)])
            for c in range(nc):
                if offs[c]:
                    e = min(x for x in ends if x > offs[c])
                    secs[c] = b[offs[c]:e]
    secs[a.cut] = sec
    open(path, "wb").write(geom.pack_container(secs, rdt[1]))
    print("  %s: Cut %d mit %d Masken geschrieben (%d Cuts im Container)"
          % (os.path.basename(path), a.cut, n, len(secs)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
