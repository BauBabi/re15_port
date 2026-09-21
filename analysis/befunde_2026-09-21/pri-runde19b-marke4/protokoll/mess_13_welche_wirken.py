"""Welche der 66 Chrom-Kandidaten wirken ueberhaupt — und erfuellen sie das
Fortsetzungs-Kriterium? Punktweise Zurechnung der Wirkung."""
import os, sys, struct
import numpy as np
from scipy import ndimage as nd
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme

rdt, _ = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
reg = np.load("build/r19b/reg0701.npy")
kand = np.load("build/r19b/chrom_fehl.npy")     # 66, nur Farbregel |b-r|<8
saum = np.load("build/r19b/saum.npy")           # 27, Schnitt beider Kriterien
A = np.load("build/r19b/depA.npy")
hit = A > 0
_, (iy, ix) = nd.distance_transform_edt(~hit, return_indices=True)
Dz = np.where(kand, A[iy, ix], 0)

floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", 0x10D0)
treffer = np.zeros((240, 320), np.int32)
for band, pts in floor.items():
    yf = -band * 0x708
    for (wx, wz) in pts:
        pf = abnahme.proj(R, t, H, wx, yf, wz); pk = abnahme.proj(R, t, H, wx, yf - 1500, wz)
        if pf is None or pk is None:
            continue
        fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
        hw = abnahme.HALB * H / fvz
        x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
        y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
        if x1 <= x0 or y1 <= y0 or not kand[y0:y1, x0:x1].any():
            continue
        ys = np.arange(y0, y1)
        vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
        vzs = np.where(ok, vzs, fvz)
        bk = np.floor(1023.0 * vzs / 65536.0)[:, None]
        v = kand[y0:y1, x0:x1] & (Dz[y0:y1, x0:x1] < bk) & (Dz[y0:y1, x0:x1] > 0)
        treffer[y0:y1, x0:x1] += v.astype(np.int32)

ys, xs = np.nonzero(kand)
print("Punkt      wirkt an n Standplaetzen   im Saum (beide Kriterien)?")
tot_s = tot_n = 0
for y, x in sorted(zip(ys, xs), key=lambda p: -treffer[p[0], p[1]]):
    n = int(treffer[y, x])
    if n == 0:
        continue
    ins = bool(saum[y, x])
    print("  (%2d,%3d)   %3d                      %s" % (x, y, n, "JA" if ins else "nein"))
    if ins:
        tot_s += n
    else:
        tot_n += n
print("\nWirksame Punktvorkommen: im Saum %d, von Kriterium (2) verworfen %d" % (tot_s, tot_n))
print("Kandidaten ohne jede Wirkung: %d von %d" % (int(((treffer == 0) & kand).sum()), int(kand.sum())))
