"""Selbsttest der Editor-Kette OHNE Oberflaeche.

Geprueft wird, was der Editor beim Speichern erzeugt — Rechteckzerlegung, Atlas-Packung,
Sektionsformat, Container — und zwar mit derselben Probe, die schon die automatisch
erzeugten Masken abgenommen hat: die Masken durch den Atlas auf den Hintergrund blitten.
Da der Atlas aus DEMSELBEN Hintergrund gepackt wird, MUSS das Bild praktisch unveraendert
bleiben. Jede sichtbare Aenderung waere ein Fehler in Packung, Anker oder Zerlegung.

Die Testregion ist synthetisch (Rechtecke an verschiedenen Stellen, auch jenseits von
x=255, damit die u8-Grenze und die Gruppenanker wirklich belastet werden) — es geht hier
um das FORMAT, nicht um Kunst.

Aufruf:  python re15_port/tools/maske/selbsttest.py
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import atlas as atlasmod
import geom
from geom import load_bg, load_rdt, original_has_masks

CD = "re15_port/shared_assets/PSX"
PPM = "build/bg_ppm"


def parse_section(b, off):
    gc, mc = struct.unpack_from("<HH", b, off)
    if gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256:
        return None
    p = off + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", b, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = b[p], b[p + 1], b[p + 2], b[p + 3]
        dep, size = struct.unpack_from("<HH", b, p + 4); p += 8
        if (size & 0xf000) == 0:
            w, h = struct.unpack_from("<HH", b, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        out.append((sx, sy, ((dx + ax + 0x8000) & 0xffff) - 0x8000,
                    ((dy + ay + 0x8000) & 0xffff) - 0x8000, w, h, dep))
    return out


def tim_index(tim):
    flag = struct.unpack_from("<I", tim, 4)[0]
    o = 8
    clut = None
    if flag & 8:
        clen = struct.unpack_from("<I", tim, o)[0]
        clut = np.frombuffer(tim[o + 12:o + clen], "<u2").copy()
        o += clen
    plen, px, py, pw, ph = struct.unpack_from("<IHHHH", tim, o)
    idx = np.frombuffer(tim[o + 12:o + plen], np.uint8).reshape(ph, pw * 2)
    return idx, clut


def main():
    room = sys.argv[1] if len(sys.argv) > 1 else "ROOM1140"
    rid = int(room[4:], 16)
    rdt, stage = load_rdt(CD, room)
    if not rdt:
        print("RDT fehlt: %s" % room); return 1
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    cuts = [c for c in range(rdt[1]) if not original_has_masks(rdt, cam, c)]
    if not cuts:
        print("%s hat ueberall Original-Masken" % room); return 0

    bad = 0
    for c in cuts[:4]:
        bg = load_bg(PPM, rid, c)
        if bg is None:
            print("  Cut %d: Hintergrund fehlt (probe_bg_dump laufen lassen)" % c)
            bad += 1
            continue
        # Synthetische Region, bewusst auch rechts von x=255 und mit Loechern.
        m = np.zeros((240, 320), bool)
        m[150:200, 20:100] = True
        m[100:180, 260:315] = True          # jenseits der u8-Grenze
        m[60:90, 130:180] = True
        # ⛔ DER FALL, DEN MEIN ERSTER SELBSTTEST NICHT HATTE: eine Flaeche, die
        # BREITER ist als das 256 Pixel breite Atlasblatt. Genau daran fehlte in
        # ROOM1140 die vordere Tischhaelfte (292x83, 77.5 % der Flaeche) — der Kasten
        # wurde still abgewiesen. Der Test pruefte damals die u8-Grenze, aber nicht
        # die Atlas-Breite.
        m[205:235, 5:315] = True
        m[170:178, 40:60] = False           # Loch -> Transparenz muss greifen

        dep = geom.depth_map(rdt, cam, c, m)
        boxes = geom.rects_from_mask(m, geom.MAX_MASKS_PER_CUT)
        tim, place, boxes = atlasmod.build(bg, m, boxes)
        if tim is None or dep is None:
            print("  Cut %d: Atlas/Tiefe fehlgeschlagen" % c); bad += 1; continue

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
        sec = geom.pack_section(groups, masks)

        # --- Zurueckgelesen und geblittet ---
        got = parse_section(sec, 0)
        if got is None or len(got) != len(masks):
            print("  Cut %d: Sektion nicht lesbar (%s)" % (c, got and len(got))); bad += 1; continue
        idx, clut = tim_index(tim)
        r = ((clut & 0x1f) << 3).astype(np.uint8)
        g = (((clut >> 5) & 0x1f) << 3).astype(np.uint8)
        b_ = (((clut >> 10) & 0x1f) << 3).astype(np.uint8)
        pal = np.stack([r, g, b_], 1)
        canvas = bg.astype(np.int16).copy()
        painted = np.zeros((240, 320), bool)
        for (sx, sy, X, Y, w, h, d) in got:
            if X < 0 or Y < 0 or X + w > 320 or Y + h > 240: bad += 1; continue
            if sy + h > idx.shape[0] or sx + w > idx.shape[1]: bad += 1; continue
            sub = idx[sy:sy + h, sx:sx + w]
            op = sub != 0
            canvas[Y:Y + h, X:X + w][op] = pal[sub[op]]
            painted[Y:Y + h, X:X + w] |= op
        if painted.sum() == 0:
            print("  Cut %d: nichts gemalt" % c); bad += 1; continue
        diff = np.abs(canvas.astype(int) - bg.astype(int)).sum(2)[painted]
        # Nur die Loch-Pixel duerfen NICHT gemalt sein
        hole_painted = int((painted & ~m).sum())
        print("  Cut %2d: %2d Rechtecke, %d Gruppen | gemalte Pixel %6d | "
              "Farbabweichung Mittel %5.2f p95 %5.2f | ausserhalb der Region %d"
              % (c, len(got), len(groups), int(painted.sum()),
                 float(diff.mean()), float(np.percentile(diff, 95)), hole_painted))
        if diff.mean() > 40 or hole_painted > 0:
            bad += 1

    print("FEHLER: %d" % bad if bad else "OK — Packung, Anker, Transparenz und Format tragen")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
