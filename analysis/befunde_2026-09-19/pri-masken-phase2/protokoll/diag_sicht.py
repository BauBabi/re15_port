"""Wo steht Leon im Lauf, und wo liegt die Maske? Diagnose fuer sicht.py."""
import os, re, struct, sys, glob
import numpy as np
WURZEL = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
import maskenbild as MB, abnahme
from geom import load_rdt, cut_view
from abnahme import proj
MASKS = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "MASKS")
room = sys.argv[1].upper(); cut = int(sys.argv[2]); log = sys.argv[3]
rdt, _ = load_rdt(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX"), room)
cam = struct.unpack_from("<I", rdt, 0x24)[0]; R, t, H = cut_view(rdt, cam, cut)
ms = MB.masken(open(os.path.join(MASKS, "%s.MSK" % room), "rb").read(), cut) or []
idx = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)
cols = np.nonzero(deck.any(0))[0]
print("Maske: %d Rechtecke, Spalten %d..%d, %d Punkte" % (len(ms), cols.min(), cols.max(), deck.sum()))
stand = np.array([float(v) for v in open(os.path.join(MASKS, "%s_PRI%02d.STAND" % (room, cut))).read().split("\n")[1].split()])
sv = stand[stand > 0]
print("Standlinie: %d Spalten, Kamera-z %d..%d" % (len(sv), sv.min(), sv.max()))
pos = []
for ln in open(log, errors="replace"):
    m = re.match(r"F(\d+)\s+R([0-9A-F]{4}) C(\d+)\s+hp=\S+\s+pos=\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", ln)
    if m and m.group(2) == room[4:] and int(m.group(3)) == cut:
        pos.append((int(m.group(1)), int(m.group(4)), int(m.group(5)), int(m.group(6))))
print("Positionen: %d" % len(pos))
beruehrt = 0
for f, x, y, z in pos[::max(1, len(pos)//25)]:
    p = proj(R, t, H, x, y, z); pk = proj(R, t, H, x, y - 1500, z)
    if p is None or pk is None:
        print("  F%-5d (%6d,%6d) hinter der Kamera" % (f, x, z)); continue
    sx, sy, vz = p
    hw = 450 * H / vz
    x0, x1 = int(max(0, sx - hw)), int(min(320, sx + hw))
    y0, y1 = int(max(0, min(pk[1], sy))), int(min(240, max(pk[1], sy)))
    box = deck[y0:y1, x0:x1] if (x1 > x0 and y1 > y0) else np.zeros((1, 1), bool)
    s2 = stand[x0:x1]; c2 = box.any(0) & (s2 > 0)
    lage = "-"
    if c2.any():
        lage = "VOR %d / HINTER %d" % (int((vz < s2[c2] - 1).sum()), int((vz > s2[c2] + 1).sum()))
    print("  F%-5d Welt(%6d,%6d) y=%5d Bild x%3d..%3d y%3d..%3d vz=%6d Maskenpunkte %5d  %s"
          % (f, x, z, y, x0, x1, y0, y1, vz, int(box.sum()), lage))
    beruehrt += int(box.any())
print("von %d Stichproben beruehren %d die Maske" % (len(pos[::max(1, len(pos)//25)]), beruehrt))
