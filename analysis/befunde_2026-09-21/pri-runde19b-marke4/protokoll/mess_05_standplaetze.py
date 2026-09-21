"""Wirkung der drei Tiefenmodelle ueber ALLE BEGEHBAREN Standplaetze (Klemmpfad-Dump)
und am Koerperkasten der Marke F3843. Antwort auf: "was richtet die Tiefe im Bild an?"
"""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme, maske_aus_png

CD = "re15_port/shared_assets/PSX"; ROOM, CUT = "ROOM10D0", 7; rid = 0x10D0
rdt, _ = geom.load_rdt(CD, ROOM); cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)
reg = np.load("build/r19b/reg0701.npy")
A = np.load("build/r19b/depA.npy"); B = np.load("build/r19b/depB.npy"); C = np.load("build/r19b/depC.npy")
# Die uebrigen Objekte des Cuts bleiben unangetastet: ihre Deckung/Tiefe aus dem Datenblatt,
# aber OHNE die Punkte von 07_01 (die ersetzen wir je Modell).
deck = np.load("build/r19b/deck.npy"); tief = np.load("build/r19b/tief.npy")
rest_d = deck & ~reg
rest_t = np.where(rest_d, tief, 0)


def gesamt(D):
    d = rest_d | (D > 0)
    tt = np.where(rest_d & (D > 0), np.minimum(rest_t, D), np.where(D > 0, D, rest_t))
    return d, tt


GA, TA = gesamt(A); GB, TB = gesamt(B); GC, TC = gesamt(C)

KOPF_LISTE = (1500, 3000)
floor = abnahme.floor_aus_dump("build/p2/dump_klemmpfad.txt", rid)


def sweep(kopf):
    res = {k: 0 for k in ("plaetze", "beruehrt", "AneB", "AneC", "sumA", "sumB", "sumC", "boxsum")}
    orte = []
    for band, pts in floor.items():
        yf = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yf, wz)
            pk = abnahme.proj(R, t, H, wx, yf - kopf, wz)
            if pf is None or pk is None:
                continue
            res["plaetze"] += 1
            fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            if not GA[y0:y1, x0:x1].any() and not GB[y0:y1, x0:x1].any():
                continue
            res["beruehrt"] += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)
            bk = np.floor(1023.0 * vzs / 65536.0)[:, None]
            va = GA[y0:y1, x0:x1] & (TA[y0:y1, x0:x1] < bk)
            vb = GB[y0:y1, x0:x1] & (TB[y0:y1, x0:x1] < bk)
            vc = GC[y0:y1, x0:x1] & (TC[y0:y1, x0:x1] < bk)
            res["boxsum"] += (y1 - y0) * (x1 - x0)
            res["sumA"] += int(va.sum()); res["sumB"] += int(vb.sum()); res["sumC"] += int(vc.sum())
            dab = int((va ^ vb).sum()); dac = int((va ^ vc).sum())
            if dab: res["AneB"] += 1
            if dac: res["AneC"] += 1
            if dab or dac:
                orte.append((band, wx, wz, int(va.sum()), int(vb.sum()), int(vc.sum())))
    return res, orte


for kopf in KOPF_LISTE:
    r, orte = sweep(kopf)
    print("=== Koerperkasten KOPF=%d, HALB=%d ===" % (kopf, abnahme.HALB))
    print("   Standplaetze %d, davon von der Maske beruehrt %d" % (r["plaetze"], r["beruehrt"]))
    print("   verdeckte Kastenpunkte gesamt: A %d | B %d | C %d" % (r["sumA"], r["sumB"], r["sumC"]))
    print("   Standplaetze mit ABWEICHENDEM Urteil: A!=B %d | A!=C %d" % (r["AneB"], r["AneC"]))
    if orte:
        print("   die ersten 12:")
        for o in orte[:12]:
            print("     Band %d (%6d,%6d)  A %4d  B %4d  C %4d" % o)
    print()
