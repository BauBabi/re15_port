"""Spaltenweise Standpunkte der Nutzer-Freistellung 07_01.png, ROOM10D0 Cut 7."""
import os, sys, struct, json
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, maske_aus_png

CD = "re15_port/shared_assets/PSX"
ROOM, CUT = "ROOM10D0", 7
rdt, st = geom.load_rdt(CD, ROOM)
rid = int(ROOM[4:], 16)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)
print("stage %d  cam 0x%X  H=%d" % (st, cam, H))

baender = sorted(geom.begehbare_baender(rid))
print("begehbare Baender:", baender, " -> y0 =", -baender[0]*geom.BAND_HOEHE if len(baender)==1 else "?")
y0 = -baender[0]*geom.BAND_HOEHE
band = int(round(-y0/float(geom.BAND_HOEHE)))
hull = geometrie.huelle(rdt)
print("Huelle:", hull, " y0=%d band=%d" % (y0, band))

reg = maske_aus_png.setze("pri/STAGE1/10D0/07_01.png", 0, 123, 1, alpha_schwelle=110)
print("Freistellung: %d Punkte" % reg.sum())

xs = sorted(int(x) for x in np.nonzero(reg.any(0))[0])

print("\n=== (b) SPALTENREGEL: unterster Punkt -> Weltpunkt am Boden y0=%d ===" % y0)
print("  x   ybot    wx      wz     vz    t   |dWelt zur Vorspalte|")
prev = None
rows = []
for x in xs:
    rr = np.nonzero(reg[:, x])[0]
    yb = int(rr.max())
    vz = geometrie.vz_at_floor(R, t, H, x+0.5, min(yb,239)+0.5, y0)
    P = geometrie.welt_am_boden(R, t, H, x+0.5, min(yb,239)+0.5, y0)
    inh = P is not None and hull[0] <= P[0] <= hull[1] and hull[2] <= P[1] <= hull[3]
    tt = int(np.clip(np.floor(1023.0*vz/65536.0),1,1023)) if vz and vz>0 else -1
    d = ""
    if prev is not None and P is not None and prev[1] is not None:
        d = "%8.0f" % np.hypot(P[0]-prev[1][0], P[1]-prev[1][1])
    rows.append((x, yb, P, vz, tt, inh))
    print(" %3d  %4d  %7s %7s %6s %4d %s%s" % (x, yb,
        "%.0f"%P[0] if P is not None else "-", "%.0f"%P[1] if P is not None else "-",
        "%.0f"%vz if vz else "-", tt, d, "" if inh else "   <- AUSSERHALB Huelle"))
    prev = (x, P)
