"""Welche CHROMROHR-Punkte (hell UND farbneutral) grenzen an die Nutzer-Freistellung,
ohne darin zu liegen? Und wirken sie: verdecken sie an einem BEGEHBAREN Standplatz etwas?

Farbtrennung nach derselben Regel wie im Vorgaenger-Dossier (§3): das Chromrohr ist
neutral (b-r um 0), der blaugraue Eimer/Kasten dahinter ist blau (b-r >= +10).
"""
import os, sys, struct
import numpy as np
from scipy import ndimage as nd
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); reg2 = np.load("build/r19b/reg0702.npy")
deck = np.load("build/r19b/deck.npy"); tief = np.load("build/r19b/tief.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]

print("=== Eichung der Farbregel an GEDECKTEM Chrom und am blauen Kasten ===")
chrom_ref = reg & (L >= 250)                        # die hellen Punkte der Freistellung selbst
print("  Chrom IN der Freistellung (L>=250): n=%d  b-r Quartile %+d / %+d / %+d"
      % (chrom_ref.sum(), np.percentile(BR[chrom_ref], 25), np.median(BR[chrom_ref]),
         np.percentile(BR[chrom_ref], 75)))
blau = np.zeros_like(reg); blau[126:157, 44:66] = True
blau &= ~reg & (L >= 150)
print("  blauer Gegenstand x44..65 y126..156 (L>=150): n=%d  b-r Quartile %+d / %+d / %+d"
      % (blau.sum(), np.percentile(BR[blau], 25), np.median(BR[blau]), np.percentile(BR[blau], 75)))

NEUTRAL = 8       # b-r < 8: nicht blau (Blau-Referenz hat Median +22, 25 %-Quantil s.o.)
HELL = 150
fen = np.zeros_like(reg); fen[118:240, 0:76] = True
chrom = (L >= HELL) & (np.abs(BR) < NEUTRAL) & fen
print("\nChromkandidaten im Stuhlfenster x0..75 y118..239: %d" % int(chrom.sum()))
print("  davon in der Freistellung 07_01: %d   in 07_02: %d   gedeckt (alle 104): %d"
      % (int((chrom & reg).sum()), int((chrom & reg2).sum()), int((chrom & deck).sum())))
fehl = chrom & ~deck
print("  ungedeckt: %d" % int(fehl.sum()))
nah = nd.binary_dilation(reg, np.ones((3, 3), bool)) & ~reg
print("  ungedeckt UND direkt an der Freistellung (8er-Nachbar): %d" % int((fehl & nah).sum()))
lab, n = nd.label(fehl & nah, np.ones((3, 3), bool))
sz = sorted(((int((lab == i).sum()), i) for i in range(1, n + 1)), reverse=True)
print("  Komponenten:")
for s, i in sz[:12]:
    m = lab == i; ys, xs = np.nonzero(m)
    print("    %3d Punkte  x%2d..%2d y%3d..%3d  b-r %+3d  Hell %4d"
          % (s, xs.min(), xs.max(), ys.min(), ys.max(), int(np.median(BR[m])), int(np.median(L[m]))))
kand = fehl & nah
np.save("build/r19b/chrom_fehl.npy", kand)

# ---- wirken sie? ----
ROOM, CUT, rid = "ROOM10D0", 7, 0x10D0
rdt, _ = geom.load_rdt("re15_port/shared_assets/PSX", ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)
A = np.load("build/r19b/depA.npy")
# Die Zusatzpunkte erben die Tiefe des naechsten Punktes der Freistellung (dieselbe
# Distanztransformation, die geometrie._erben benutzt).
hit = A > 0
_, (iy, ix) = nd.distance_transform_edt(~hit, return_indices=True)
Dz = np.where(kand, A[iy, ix], 0)
print("\nTiefe der Zusatzpunkte (geerbt vom naechsten Freistellungspunkt): %d..%d"
      % (Dz[kand].min(), Dz[kand].max()))

floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", rid)
for kopf in (1500, 3000):
    plaetze = ber = wirk = 0; summe = 0; best = []
    for band, pts in floor.items():
        yf = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yf, wz); pk = abnahme.proj(R, t, H, wx, yf - kopf, wz)
            if pf is None or pk is None:
                continue
            plaetze += 1
            fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            sub = kand[y0:y1, x0:x1]
            if not sub.any():
                continue
            ber += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)
            bk = np.floor(1023.0 * vzs / 65536.0)[:, None]
            v = sub & (Dz[y0:y1, x0:x1] < bk) & (Dz[y0:y1, x0:x1] > 0)
            c = int(v.sum())
            if c:
                wirk += 1; summe += c; best.append((c, band, wx, wz))
    best.sort(reverse=True)
    print("KOPF=%d: %d Standplaetze, %d beruehren die Zusatzpunkte, %d wuerden dadurch "
          "MEHR verdeckt (Summe %d Kastenpunkte)" % (kopf, plaetze, ber, wirk, summe))
    for c, b, wx, wz in best[:8]:
        print("    +%3d Punkte  Band %d (%6d,%6d)" % (c, b, wx, wz))
