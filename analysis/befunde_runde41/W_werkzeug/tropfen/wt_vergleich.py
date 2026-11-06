"""wt_vergleich.py - Spur W OFFEN 4: vorher/nachher je Lauf-Paar aus wt_batch.py (320x240-Grauwerte, 30 Bilder/s).
Je Paar <name>_vorher / <name>_nachher:
  * Tropfen-Fleck: Wechselzahl (Bildwechsel |d| > 16, blitz_wz.npy) und Spanne max-min (spur_oben.npy, Zeilen 0..19) am
    Fleckpunkt des Cuts (W_abnahme_2.md H1: Cut 2 (83,1), Cut 5 (193,1..2), Cut 7 (169,15)); dazu der hoechste Wert der oberen
    20 Zeilen ausserhalb der Figur.
  * Rest des Bildes: Medianbild nachher gegen vorher, groesster |d| ausserhalb eines 9x9-Kastens um den Fleck und ausserhalb der
    Figur-Kaesten (befund_lauf.log 'kasten=', erweitert wie wa2_blitz.py); Bildpunkte mit Wechselzahl >= 6 ausserhalb von Fleck
    und Figur: Zahl vorher / nachher und Jaccard.
  * fx.log: Zeilen Effekt 6 Sub 0 und Effekt 0x0B (id=11) vorher / nachher; debug.log [tropfen41].
Aufruf: wt_vergleich.py <runs_dir> <name>[:x,y] ..."""
import os, re, sys
import numpy as np
runs = sys.argv[1]
FLECK = {"c02": (83, 1), "c05": (193, 1), "c07": (169, 15), "marke": (83, 1)}

def lade(d):
    r = {}
    for k in ("blitz_wz", "median", "spur_oben"):
        p = os.path.join(d, k + ".npy"); r[k] = np.load(p) if os.path.exists(p) else None
    bef = open(os.path.join(d, "befund_lauf.log"), encoding="utf-8", errors="replace").read() if os.path.exists(os.path.join(d, "befund_lauf.log")) else ""
    m = np.zeros((240, 320), bool)
    for x0, x1, y0, y1 in [tuple(map(int, g.groups())) for g in re.finditer(r"kasten=x(-?\d+)\.\.(-?\d+),y(-?\d+)\.\.(-?\d+)", bef)]:
        m[max(0, y0 - 45):max(0, min(240, y1 + 9)), max(0, x0 - 12):max(0, min(320, x1 + 13))] = True
    r["figur"] = m
    fx = open(os.path.join(d, "fx.log"), errors="replace").read() if os.path.exists(os.path.join(d, "fx.log")) else ""
    r["id6"] = len(re.findall(r"^id=6 sub=0 ", fx, re.M)); r["id11"] = len(re.findall(r"^id=11 ", fx, re.M))
    r["id6x"] = len(re.findall(r"^id=6 sub=[1-9]", fx, re.M))
    dbg = open(os.path.join(d, "debug.log"), errors="replace").read() if os.path.exists(os.path.join(d, "debug.log")) else ""
    r["tr"] = len(re.findall(r"\[tropfen41\]", dbg))
    return r

for arg in sys.argv[2:]:
    name, _, xy = arg.partition(":")
    a = lade(os.path.join(runs, name + "_vorher")); b = lade(os.path.join(runs, name + "_nachher"))
    if a["median"] is None or b["median"] is None:
        print(name, "Lauf fehlt"); continue
    key = next((k for k in FLECK if k in name), None)
    fx_, fy_ = tuple(map(int, xy.split(","))) if xy else (FLECK[key] if key else (-1, -1))
    fig = a["figur"] | b["figur"]
    aus = fig.copy()
    if fx_ >= 0: aus[max(0, fy_ - 4):fy_ + 5, max(0, fx_ - 4):fx_ + 5] = True
    dmed = np.abs(a["median"].astype(int) - b["median"].astype(int))
    ma = (a["blitz_wz"] >= 6) & ~aus; mb = (b["blitz_wz"] >= 6) & ~aus
    jac = (ma & mb).sum() / max(1, (ma | mb).sum()) if (ma | mb).any() else 1.0
    s = "%s:" % name
    if fx_ >= 0:
        def fl(r):
            so = r["spur_oben"]
            if fy_ >= so.shape[1]: return "?"
            win = so[:, max(0, fy_ - 1):fy_ + 2, max(0, fx_ - 1):fx_ + 2].astype(int)
            return "Wechselzahl %d, Spanne %d (min %d max %d)" % (r["blitz_wz"][max(0, fy_ - 1):fy_ + 2, max(0, fx_ - 1):fx_ + 2].max(),
                                                                  win.max() - win.min(), win.min(), win.max())
        s += " Fleck (%d,%d) vorher %s | nachher %s;" % (fx_, fy_, fl(a), fl(b))
    oben_a = int((a["blitz_wz"][0:20] * ~fig[0:20]).max()); oben_b = int((b["blitz_wz"][0:20] * ~fig[0:20]).max())
    s += " Rand y0..19 Wechselzahl max %d -> %d;" % (oben_a, oben_b)
    s += " Median |d| ausserhalb Fleck+Figur max %d (Bildpunkte > 8: %d), ueberall max %d;" % (dmed[~aus].max(), int((dmed[~aus] > 8).sum()), dmed.max())
    s += " unruhig (>=6) ausserhalb Fleck+Figur %d -> %d, Jaccard %.2f;" % (ma.sum(), mb.sum(), jac)
    s += " fx.log id=6 sub=0 %d -> %d, id=6 andere Sub %d -> %d, id=11 %d -> %d; [tropfen41] %d -> %d" % (a["id6"], b["id6"], a["id6x"], b["id6x"], a["id11"], b["id11"], a["tr"], b["tr"])
    print(s)
