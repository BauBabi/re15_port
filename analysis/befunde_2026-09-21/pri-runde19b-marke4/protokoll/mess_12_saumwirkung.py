"""Was bewirken die 27 Saumpunkte? Tiefenmodell vorher/nachher und Wirkung ueber ALLE
begehbaren Standplaetze (Klemmpfad-Dump)."""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme

rdt, _ = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
reg = np.load("build/r19b/reg0701.npy"); saum = np.load("build/r19b/saum.npy")
neu = reg | saum
print("Freistellung 2677 -> mit Saum %d Punkte (+%d)" % (int(neu.sum()), int(saum.sum())))
xs_alt = np.nonzero(reg.any(0))[0]; xs_neu = np.nonzero(neu.any(0))[0]
print("Spalten %d..%d -> %d..%d ; Schwerpunktspalte %.2f -> %.2f ; tiefste Zeile %d -> %d"
      % (xs_alt.min(), xs_alt.max(), xs_neu.min(), xs_neu.max(),
         xs_alt.mean() + 0.5, xs_neu.mean() + 0.5,
         np.nonzero(reg.any(1))[0].max(), np.nonzero(neu.any(1))[0].max()))


def mod(r):
    vzm, info = geometrie.tiefe_geometrie(rdt, R, t, H, r, 0, 0, {"aufrecht": True}, None, [])
    return geometrie.rastern(vzm) * r


A = mod(reg); N = mod(neu)
print("Tiefen alt %d..%d, neu %d..%d ; an den 2677 alten Punkten gleich: %d von 2677"
      % (A[reg].min(), A[reg].max(), N[neu].min(), N[neu].max(), int((A[reg] == N[reg]).sum())))
print("Tiefen der 27 Saumpunkte: %d..%d" % (N[saum].min(), N[saum].max()))

deck = np.load("build/r19b/deck.npy"); tief = np.load("build/r19b/tief.npy")
rest_d = deck & ~reg; rest_t = np.where(rest_d, tief, 0)


def ges(D):
    d = rest_d | (D > 0)
    tt = np.where(rest_d & (D > 0), np.minimum(rest_t, D), np.where(D > 0, D, rest_t))
    return d, tt


GA, TA = ges(A); GN, TN = ges(N)
floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", 0x10D0)
for kopf in (1500, 3000):
    pl = ber = mehr = wen = 0; smehr = swen = 0; best = []
    for band, pts in floor.items():
        yf = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yf, wz); pk = abnahme.proj(R, t, H, wx, yf - kopf, wz)
            if pf is None or pk is None:
                continue
            pl += 1
            fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            if not GN[y0:y1, x0:x1].any():
                continue
            ber += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)
            bk = np.floor(1023.0 * vzs / 65536.0)[:, None]
            va = int((GA[y0:y1, x0:x1] & (TA[y0:y1, x0:x1] < bk)).sum())
            vn = int((GN[y0:y1, x0:x1] & (TN[y0:y1, x0:x1] < bk)).sum())
            if vn > va:
                mehr += 1; smehr += vn - va; best.append((vn - va, band, wx, wz, va, vn))
            elif vn < va:
                wen += 1; swen += va - vn
    best.sort(reverse=True)
    print("\nKOPF=%d: %d Standplaetze, %d beruehrt" % (kopf, pl, ber))
    print("   MEHR verdeckt: %d Plaetze, +%d Kastenpunkte (Median +%s)"
          % (mehr, smehr, int(np.median([b[0] for b in best])) if best else 0))
    print("   WENIGER verdeckt: %d Plaetze, -%d Kastenpunkte" % (wen, swen))
    for b in best[:8]:
        print("     +%3d  Band %d (%6d,%6d)  %d -> %d" % (b[0], b[1], b[2], b[3], b[4], b[5]))
np.save("build/r19b/depN.npy", N)
