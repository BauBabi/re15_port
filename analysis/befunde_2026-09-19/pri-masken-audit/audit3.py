"""(2b) Faktor/Modell-Beweis mit Toleranz + korrigierter VOR-Regel (Standlinie auf der EBENE des
Objekts = auswahl 'ebene' oder 0, nicht auf dem Band des Spielers)."""
import json, os, sys
import numpy as np
from scipy import ndimage
S = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, S)
sys.path.insert(0, r"C:\workspace\git\reAi_v2\re15_port\tools\maske")
os.chdir(r"C:\workspace\git\reAi_v2")
import dumplib as D
import maske_aus_png as MAP
rooms = D.parse(sys.argv[1]); OUT = sys.argv[2]
AUS = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
out = open(OUT, "w", encoding="utf-8")
def W(s=""): out.write(s + "\n"); print(s, flush=True)
KOPF = 1500; HALB = 450
def region_png(o): return MAP.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
def halo(r, oben, grow):
    g = r.copy()
    for k in range(1, oben + 1): g[:-k] |= r[k:]
    return ndimage.binary_dilation(g, iterations=grow) if grow > 0 else g
def tiefen_spaltenregel(reg, rot, trans, H, y0, faktor):
    dep = np.zeros((240, 320), np.int32)
    for x in np.nonzero(reg.any(0))[0]:
        rows = np.nonzero(reg[:, x])[0]; yb = int(rows.max())
        v = D.vz_boden(rot, trans, H, x + 0.5, float(min(yb, 239)), y0)
        if v: dep[rows, x] = max(1, min(1023, int(v * faktor / 64.0)))
    return dep
def tile_median(dep, rects):
    t = np.zeros((240, 320), np.int32)
    for (sx, sy, dx, dy, w, h, d) in rects:
        x0, x1 = max(0, dx), min(320, dx + w); y0, y1 = max(0, dy), min(240, dy + h)
        if x1 <= x0 or y1 <= y0: continue
        win = dep[y0:y1, x0:x1]; win = win[win > 0]
        if len(win) == 0: continue
        m = int(np.median(win)); sub = t[y0:y1, x0:x1]
        t[y0:y1, x0:x1] = np.where((dep[y0:y1, x0:x1] > 0) & ((sub == 0) | (m < sub)), m, sub)
    return t
def vor_audit(R, kv, tief, deck, reg_png, ebene):
    rot, trans, H = kv["rot"], kv["trans"], kv["H"]
    stand = np.full(320, np.nan)
    for x in range(320):
        rows = np.nonzero(reg_png[:, x])[0]
        if len(rows):
            v = D.vz_boden(rot, trans, H, x + 0.5, float(min(int(rows.max()), 239)) + 0.5, ebene)
            if v: stand[x] = v
    vor_n = vor_verd = vor_teil = 0; hint_n = hint_frei = 0; fenster = []
    for band, pts in R["floor"].items():
        yfoot = -band * 0x708
        for (wx, wz) in pts:
            pf = D.proj(rot, trans, H, wx, yfoot, wz); pk = D.proj(rot, trans, H, wx, yfoot - KOPF, wz)
            if pf is None or pk is None: continue
            fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
            hw = HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw)); y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0: continue
            box = deck[y0:y1, x0:x1]
            if not box.any(): continue
            vzs = np.array([D.vz_senkrechte(rot, trans, H, wx, wz, y + 0.5) or fvz for y in range(y0, y1)])
            verd = box & (tief[y0:y1, x0:x1].astype(np.float64) * 64.0 < vzs[:, None])
            sv = stand[x0:x1]; colmask = box.any(0) & np.isfinite(sv)
            vor_cols = colmask & (fvz < sv - 1); hin_cols = colmask & (fvz > sv + 1)
            if vor_cols.any():
                vor_n += 1
                q = verd[:, vor_cols].sum() / max(1, box[:, vor_cols].sum())
                if q >= 0.95: vor_verd += 1; fenster.append(float(np.nanmin(sv[vor_cols]) / fvz))
                elif q > 0.05: vor_teil += 1
            if hin_cols.any():
                hint_n += 1
                if verd[:, hin_cols].sum() / max(1, box[:, hin_cols].sum()) < 0.95: hint_frei += 1
    return vor_n, vor_verd, vor_teil, hint_n, hint_frei, fenster
W("# Spaltenregel-Cuts: Modell-Treue (|Kacheltiefe-Modell|<=2) und VOR/HINTER mit Standlinie auf der Objekt-Ebene")
W("%-9s %-3s %6s %8s | %5s %6s %6s %6s %6s | %6s %6s %6s | %s" % ("Raum", "Cut", "ebene", "Treue90", "VORn", "verd90", "teil90", "verd100", "teil100", "HINTn", "frei90", "frei100", "Verhaeltnis Standlinie/Fuss bei verd90 (Median)"))
for room in sorted(k for k in AUS if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    if rid not in rooms: continue
    for cut in sorted((c for c in AUS[room] if not c.startswith("_")), key=int):
        c = int(cut); e = AUS[room][cut]; kv = rooms[rid]["cuts"].get(c)
        if not isinstance(e, dict) or kv is None or "deck" not in kv: continue
        objs = e.get("objekte") or []
        if not objs or not all("png" in o and not any(k in o for k in ("fuss", "aufrecht", "flach", "tiefe", "spalten", "zelle", "bodenkante", "quader")) for o in objs):
            continue
        rot, trans, H = kv["rot"], kv["trans"], kv["H"]
        ebene = objs[0].get("ebene", 0)
        reg_png = np.zeros((240, 320), bool); dep90 = np.zeros((240, 320), np.int32); dep100 = np.zeros((240, 320), np.int32)
        for o in objs:
            r = region_png(o)
            if r is None: continue
            rh = halo(r, o.get("oben", e.get("oben", 4)), o.get("grow", e.get("grow", 1)))
            d90 = tiefen_spaltenregel(rh, rot, trans, H, o.get("ebene", 0), 0.90); d100 = tiefen_spaltenregel(rh, rot, trans, H, o.get("ebene", 0), 1.00)
            dep90 = np.where((d90 > 0) & ((dep90 == 0) | (d90 < dep90)), d90, dep90)
            dep100 = np.where((d100 > 0) & ((dep100 == 0) | (d100 < dep100)), d100, dep100)
            reg_png |= r
        deck = kv["deck"]; tief = kv["tief"]
        t90 = tile_median(dep90, kv["rects"]); t100 = tile_median(dep100, kv["rects"])
        m = deck & (t90 > 0)
        treue = float((np.abs(t90[m] - tief[m]) <= 2).mean()) if m.any() else 0.0
        a = vor_audit(rooms[rid], kv, tief, deck, reg_png, ebene)
        b = vor_audit(rooms[rid], kv, t100, deck & (t100 > 0), reg_png, ebene)
        W("%-9s %-3d %6d %7.1f%% | %5d %6d %6d %6d %6d | %6d %6d %6d | %s" % (room, c, ebene, 100 * treue, a[0], a[1], a[2], b[1], b[2], a[3], a[4], b[4],
            ("%.3f" % float(np.median(a[5]))) if a[5] else "-"))
out.close()
