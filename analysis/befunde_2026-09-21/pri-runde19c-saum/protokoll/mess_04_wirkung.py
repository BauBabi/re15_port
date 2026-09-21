"""WIRKUNG des Saums ueber die BEGEHBAREN Standplaetze dieses Winkels (Klemmpfad-Dump der
Sonde probe_p2_floor_dump, Herkunftsmarke '# QUELLE klemmpfad').

Gelesen wird beidesmal ueber den ECHTEN Ladeweg (maskenbild.masken + maskenbild.lies_tim_bytes
+ abnahme.deckung_und_tiefe), einmal aus dem AUSLIEFERUNGSSTAND und einmal aus dem Neubau
build/r19c/neu_MASKS. Verglichen werden:
  * verdeckte Koerperkastenpunkte (Summe ueber alle Standplaetze), vorher gegen nachher,
  * Standplaetze mit GEAENDERTEM Urteil (mindestens ein Kastenpunkt wechselt),
  * die Standplatz-Schiene (VORn/VORverd/VORteil/HINTn/HINTfrei) beider Staende.
Verdeckungsregel wie in der Engine: Maske verdeckt gdw. Tiefe < Eimer der Figur
((1023*vz)>>16 je Bildzeile, geometrie.verdeckt).
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import abnahme                                              # noqa: E402
import geom                                                 # noqa: E402
import geometrie                                            # noqa: E402
import maskenbild                                           # noqa: E402

CD = "re15_port/shared_assets/PSX"
ALT_DIR = os.path.join(CD, "MASKS")
NEU_DIR = "build/r19c/neu_MASKS"
ROOM, CUT, RID = "ROOM10D0", 7, 0x10D0

rdt, _ = geom.load_rdt(CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)


def lies(d):
    blob = open(os.path.join(d, "%s.MSK" % ROOM), "rb").read()
    ms = maskenbild.masken(blob, CUT) or []
    tim = open(os.path.join(d, "%s_PRI%02d.TIM" % (ROOM, CUT)), "rb").read()
    idx = maskenbild.lies_tim_bytes(tim)[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    return ms, deck, tief


msA, dA, tA = lies(ALT_DIR)
msB, dB, tB = lies(NEU_DIR)
print("VORHER : %3d Rechtecke, %d gedeckte Punkte, Tiefen %d..%d"
      % (len(msA), int(dA.sum()), int(tA[dA].min()), int(tA[dA].max())))
print("NACHHER: %3d Rechtecke, %d gedeckte Punkte, Tiefen %d..%d"
      % (len(msB), int(dB.sum()), int(tB[dB].min()), int(tB[dB].max())))
print("Deckung: nur nachher %d, nur vorher %d, gemeinsam %d"
      % (int((dB & ~dA).sum()), int((dA & ~dB).sum()), int((dA & dB).sum())))
gl = dA & dB
print("gemeinsame Punkte mit GEAENDERTER Tiefe: %d (Differenz min %d max %d)"
      % (int((tA[gl] != tB[gl]).sum()), int((tB[gl] - tA[gl]).min()), int((tB[gl] - tA[gl]).max())))

floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", RID)
print("Klemmpfad-Dump: %d Baender, %d Punkte" % (len(floor), sum(len(v) for v in floor.values())))


def sweep(kopf):
    z = dict(plaetze=0, beruehrt=0, sumA=0, sumB=0, boxsum=0, wechsel=0, mehr=0, weniger=0)
    orte = []
    for band, pts in floor.items():
        yf = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yf, wz)
            pk = abnahme.proj(R, t, H, wx, yf - kopf, wz)
            if pf is None or pk is None:
                continue
            z["plaetze"] += 1
            fsx, fsy, fvz = pf
            ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            if not dA[y0:y1, x0:x1].any() and not dB[y0:y1, x0:x1].any():
                continue
            z["beruehrt"] += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)
            bk = np.floor(1023.0 * vzs / 65536.0)[:, None]
            va = dA[y0:y1, x0:x1] & (tA[y0:y1, x0:x1] < bk)
            vb = dB[y0:y1, x0:x1] & (tB[y0:y1, x0:x1] < bk)
            z["boxsum"] += (y1 - y0) * (x1 - x0)
            sa = int(va.sum()); sb = int(vb.sum())
            z["sumA"] += sa; z["sumB"] += sb
            if int((va ^ vb).sum()):
                z["wechsel"] += 1
                if sb > sa:
                    z["mehr"] += 1
                elif sb < sa:
                    z["weniger"] += 1
                orte.append((band, int(wx), int(wz), sa, sb))
    return z, orte


for kopf in (1500, 3000):
    z, orte = sweep(kopf)
    print("\n=== Koerperkasten KOPF=%d, HALB=%d ===" % (kopf, abnahme.HALB))
    print("   Standplaetze %d, von einer Maske beruehrt %d, Kastenpunkte gesamt %d"
          % (z["plaetze"], z["beruehrt"], z["boxsum"]))
    print("   verdeckte Kastenpunkte: VORHER %d -> NACHHER %d  (%+d)"
          % (z["sumA"], z["sumB"], z["sumB"] - z["sumA"]))
    print("   Standplaetze mit geaendertem Urteil: %d (davon mehr verdeckt %d, weniger %d)"
          % (z["wechsel"], z["mehr"], z["weniger"]))
    if orte:
        print("   die ersten 15 (Band, x, z, vorher, nachher):")
        for o in sorted(orte, key=lambda q: -(q[4] - q[3]))[:15]:
            print("     Band %d (%6d,%6d)  %5d -> %5d  (%+d)" % (o[0], o[1], o[2], o[3], o[4], o[4] - o[3]))

# Standplatz-Schiene beider Staende, aus den Sidecar-Standlinien der jeweiligen Dateien
def schiene(d, deck, tief):
    p = os.path.join(d, "%s_PRI%02d.STAND" % (ROOM, CUT))
    zeil = [l for l in open(p).read().splitlines() if not l.startswith("#")]
    sv = np.array([float(v) for v in zeil[0].split()])
    sv[sv < 0] = np.nan
    y0 = np.array([float(v) for v in zeil[1].split()])
    return abnahme.standplatz_schiene(R, t, H, deck, tief, sv, floor, stand_y0=y0)


sa = schiene(ALT_DIR, dA, tA)
sb = schiene(NEU_DIR, dB, tB)
print("\n=== Standplatz-Schiene (Spiegelregel: VORverd MUSS 0 sein) ===")
print("   %-8s %6s %7s %7s %6s %8s" % ("", "VORn", "VORverd", "VORteil", "HINTn", "HINTfrei"))
for nm, s in (("VORHER", sa), ("NACHHER", sb)):
    print("   %-8s %6d %7d %7d %6d %8d"
          % (nm, s["VORn"], s["VORverd"], s["VORteil"], s["HINTn"], s["HINTfrei"]))
for nm, s in (("VORHER", sa), ("NACHHER", sb)):
    print("   %s HINTER-Rest: %s" % (nm, ", ".join("(%d,%d,%d,%.2f)" % q for q in s["hint_rest"])))
