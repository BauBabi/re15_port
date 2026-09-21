"""Traegt die Tiefen-Deutung? Drei Tiefenmodelle fuer die Nutzer-Freistellung 07_01,
gemessen gegen (a) die Figur an Marke F3843 und (b) das ausgelieferte Datenblatt."""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme, maske_aus_png

CD = "re15_port/shared_assets/PSX"; ROOM, CUT = "ROOM10D0", 7
rdt, _ = geom.load_rdt(CD, ROOM); cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT); y0 = 0; band = 0
reg = maske_aus_png.setze("pri/STAGE1/10D0/07_01.png", 0, 123, 1, alpha_schwelle=110)


def modell(o, name):
    ber = []
    vzm, info = geometrie.tiefe_geometrie(rdt, R, t, H, reg, y0, band, o, None, ber)
    dep = geometrie.rastern(vzm) * reg
    tv = dep[reg]
    print("%-36s Tiefen %3d..%3d (%2d Stufen)  %s" % (name, tv.min(), tv.max(),
          len(np.unique(tv)), "; ".join(ber) if ber else ""))
    return dep


print("=== Tiefenmodelle der Freistellung 07_01 (2677 Punkte) ===")
A = modell({"aufrecht": True}, "A AUSGELIEFERT aufrecht:true")
B = modell({"aufrecht": "spalten", "zelle": False}, "B Spaltenregel")
C = modell({"aufrecht": "spalten", "zelle": False, "bodenkante": [1, 68]},
           "C Spaltenregel + bodenkante 1..68")

tief = np.load("build/r19b/tief.npy")
gl = (A[reg] == tief[reg])
print("\nModell A == ausgeliefertes ROOM10D0_PRI07: %d von %d Punkten gleich (%.1f %%)"
      % (gl.sum(), reg.sum(), 100.0 * gl.mean()))

print("\n=== Spalten 64..72: Tiefe je Modell ===")
print("  x  ybot   A(ausgeliefert)   B(Spaltenregel)   C(+bodenkante)")
for x in range(64, 73):
    rr = np.nonzero(reg[:, x])[0]
    print("  %2d  %3d   %3d..%-3d         %3d..%-3d         %3d..%-3d"
          % (x, int(rr.max()), A[rr, x].min(), A[rr, x].max(),
             B[rr, x].min(), B[rr, x].max(), C[rr, x].min(), C[rr, x].max()))

PX, PZ = 418, 26497
bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
rn = np.load("build/r19b/render_F3843.npy").astype(np.int32)
figur = (np.abs(rn - bg).sum(2) > 30)
ys = np.arange(240)
vzs, ok = geometrie.profil_spalte(R, t, H, PX, PZ, ys)
vz_row = np.where(ok, vzs, np.nan)
bk = np.floor(1023.0 * vz_row / 65536.0)
print("\nFigur-Kamera-z je Zeile (Senkrechte durch (418,26497)):")
for y in (120, 130, 150, 179, 190, 200):
    print("   y%3d  vz %7.0f  Bucket %d" % (y, vz_row[y], bk[y]))
print("Figur-Punkte im Bild (|dRGB|>30): %d" % int(figur.sum()))
for nm, D in (("A", A), ("B", B), ("C", C)):
    verd = (D > 0) & (D < bk[:, None]) & figur
    print("  Modell %s verdeckt %d Figurpunkte" % (nm, int(verd.sum())))
vA = (A > 0) & (A < bk[:, None]) & figur
vB = (B > 0) & (B < bk[:, None]) & figur
vC = (C > 0) & (C < bk[:, None]) & figur
print("  Urteil wechselt A->B an %d Figurpunkten, A->C an %d"
      % (int((vA ^ vB).sum()), int((vA ^ vC).sum())))
np.save("build/r19b/figur_F3843.npy", figur)
np.save("build/r19b/bk_row.npy", bk)
np.save("build/r19b/depA.npy", A)
np.save("build/r19b/depB.npy", B)
np.save("build/r19b/depC.npy", C)
