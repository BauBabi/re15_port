"""Ursachen-Beweise: (1) Halo = oben4/grow1, (2) Faktor 0.90, (3) Loecher = Sehstrahl ueber
dem Horizont, (5) Atlas/Kapazitaet, (6) unreferenzierte PNGs. Plus Bilder."""
import json, os, sys, glob
import numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage
S = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, S)
sys.path.insert(0, r"C:\workspace\git\reAi_v2\re15_port\tools\maske")
os.chdir(r"C:\workspace\git\reAi_v2")
import dumplib as D
import maske_aus_png as MAP
from geom import load_bg, DEPTH_FACTOR

rooms = D.parse(sys.argv[1])
OUT = sys.argv[2]
BILD = sys.argv[3]
os.makedirs(BILD, exist_ok=True)
AUS = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
out = open(OUT, "w", encoding="utf-8")
def W(s=""):
    out.write(s + "\n"); print(s, flush=True)

def region_png(o, thr=110):
    return MAP.setze(o["png"], o["x"], o["y"], o.get("massstab", 1), alpha_schwelle=thr)

def halo(r, oben, grow):
    g = r.copy()
    for k in range(1, oben + 1):
        g[:-k] |= r[k:]
    if grow > 0:
        g = ndimage.binary_dilation(g, iterations=grow)
    return g

# ------------------------------------------------------------------ (6) PNG-Referenzen
refs = set()
for room in AUS:
    if not room.startswith("ROOM"): continue
    for cut, e in AUS[room].items():
        if cut.startswith("_") or not isinstance(e, dict): continue
        for o in e.get("objekte") or []:
            if "png" in o: refs.add(o["png"].replace("\\", "/"))
alle = sorted(p.replace("\\", "/") for p in glob.glob("pri/STAGE1/*/*.png"))
W("# (6) Nutzer-PNGs STAGE1: %d Dateien, davon in auswahl.json referenziert: %d" % (len(alle), sum(1 for p in alle if p in refs)))
for p in alle:
    if p not in refs: W("   NICHT REFERENZIERT: %s" % p)

# ------------------------------------------------------------------ (1)+(3)+(5) je Cut
W("\n# (1) Halo-Beweis: Region(PNG>110) mit oben/grow aus auswahl (Default 4/1) gegen die gelieferte Deckung")
W("%-9s %-3s %6s %6s %8s %8s | %5s %5s %7s %6s" % ("Raum", "Cut", "oben", "grow", "Halo!=M", "PNG!=M", "rects", "kante", "atlasPx", "draw"))
for room in sorted(k for k in AUS if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    if rid not in rooms: continue
    for cut in sorted((c for c in AUS[room] if not c.startswith("_")), key=int):
        c = int(cut); e = AUS[room][cut]; kv = rooms[rid]["cuts"].get(c)
        if not isinstance(e, dict) or kv is None or "deck" not in kv: continue
        objs = e.get("objekte") or []
        if not all("png" in o for o in objs):
            tag = "gemischt/eigene Quellen"
        else:
            tag = ""
        std_oben = e.get("oben", 4); std_grow = e.get("grow", 1)
        reg_halo = np.zeros((240, 320), bool); reg_png = np.zeros((240, 320), bool)
        obs = set(); grs = set()
        for o in objs:
            if "png" not in o: continue
            r = region_png(o)
            if r is None: continue
            ob = o.get("oben", std_oben); gr = o.get("grow", std_grow); obs.add(ob); grs.add(gr)
            reg_png |= r
            reg_halo |= halo(r, ob, gr)
            uk = o.get("unterkante")
        deck = kv["deck"]
        rects = kv["rects"]
        kante = max((max(r[4], r[5]) for r in rects), default=0)
        atlas = sum(r[4] * r[5] for r in rects)
        W("%-9s %-3d %6s %6s %8d %8d | %5d %5d %7d %6d %s" % (room, c, "/".join(str(v) for v in sorted(obs)), "/".join(str(v) for v in sorted(grs)),
            int((reg_halo != deck).sum()) if not tag else -1, int((reg_png != deck).sum()), len(rects), kante, atlas, kv["draw"], tag))

# ------------------------------------------------------------------ (3) ROOM1000 C3 Loecher
W("\n# (3) ROOM1000 Cut 3: trifft der Sehstrahl der untersten PNG-Zeile den Boden (y=0)?")
kv = rooms[0x1000]["cuts"][3]
for o in AUS["ROOM1000"]["3"]["objekte"]:
    r = region_png(o)
    xs = np.nonzero(r.any(0))[0]
    hits = 0; nohit = 0; vals = []
    for x in xs:
        yb = int(np.nonzero(r[:, x])[0].max())
        v = D.vz_boden(kv["rot"], kv["trans"], kv["H"], x + 0.5, float(yb), 0)
        if v: hits += 1; vals.append(v)
        else: nohit += 1
    W("   %-26s Spalten %3d: Bodentreffer %3d, KEIN Treffer %3d%s | Maske deckt %d von %d px" % (
        o["png"][4:], len(xs), hits, nohit, "" if not vals else "  vz %.0f..%.0f -> Tiefe %d..%d" % (min(vals), max(vals), int(min(vals)*DEPTH_FACTOR/64), int(max(vals)*DEPTH_FACTOR/64)),
        int((r & kv["deck"]).sum()), int(r.sum())))
# Horizontzeile des Cuts
rot, trans, H = kv["rot"], kv["trans"], kv["H"]
hz = [y for y in range(240) if D.vz_boden(rot, trans, H, 160.5, y + 0.5, 0)]
W("   Bodenebene y=0 wird nur in den Bildzeilen %s getroffen (Horizont darueber)" % ("%d..%d" % (min(hz), max(hz)) if hz else "keine"))
W("   Hinweis: 1000 C1 tMax=531 / C3 tMax=496 = Spalten knapp UNTER dem Horizont (Sehstrahl fast parallel zum Boden)")

# ------------------------------------------------------------------ (2) Faktor 0.90: Beweis am Tiefenprofil
W("\n# (2) Faktor-Beweis: Spaltenregel-Tiefen aus PNG-Unterkante (Ebene y=-band*0x708 bzw. 'ebene') mit 0.90 und 1.00")
W("#    'Tile-Median==Maske' = Anteil der Maskenpixel, deren gelieferte Tiefe dem 0.90-Modell (Median je Kachel) entspricht")
W("#    VORverd = Standplaetze VOR der PNG-Standlinie, an denen der Koerper >=95%% verdeckt wird — mit 0.90 (geliefert) / mit 1.00 (Modell)")
W("%-9s %-3s %10s %8s %8s %8s | %8s %8s" % ("Raum", "Cut", "Modell0.90", "VORn", "VORverd", "VORteil", "VORv1.00", "VORt1.00"))
KOPF = 1500; HALB = 450
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
        m = int(np.median(win))
        sub = t[y0:y1, x0:x1]
        t[y0:y1, x0:x1] = np.where((dep[y0:y1, x0:x1] > 0) & ((sub == 0) | (m < sub)), m, sub)
    return t
def vor_audit(rooms_rid, kv, tief, deck, reg_png):
    rot, trans, H = kv["rot"], kv["trans"], kv["H"]
    vor_n = vor_verd = vor_teil = 0
    for band, pts in rooms_rid["floor"].items():
        yfoot = -band * 0x708
        stand = np.full(320, np.nan)
        for x in range(320):
            rows = np.nonzero(reg_png[:, x])[0]
            if len(rows):
                v = D.vz_boden(rot, trans, H, x + 0.5, float(min(int(rows.max()), 239)) + 0.5, yfoot)
                if v: stand[x] = v
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
            vor_cols = colmask & (fvz < sv - 1)
            if vor_cols.any():
                vor_n += 1
                q = verd[:, vor_cols].sum() / max(1, box[:, vor_cols].sum())
                if q >= 0.95: vor_verd += 1
                elif q > 0.05: vor_teil += 1
    return vor_n, vor_verd, vor_teil
for room in sorted(k for k in AUS if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    if rid not in rooms: continue
    for cut in sorted((c for c in AUS[room] if not c.startswith("_")), key=int):
        c = int(cut); e = AUS[room][cut]; kv = rooms[rid]["cuts"].get(c)
        if not isinstance(e, dict) or kv is None or "deck" not in kv: continue
        objs = e.get("objekte") or []
        # nur reine Spaltenregel-Cuts (kein Tiefenschluessel ausser 'ebene')
        if not objs or not all("png" in o and not any(k in o for k in ("fuss", "aufrecht", "flach", "tiefe", "spalten", "zelle", "bodenkante", "quader")) for o in objs):
            continue
        rot, trans, H = kv["rot"], kv["trans"], kv["H"]
        reg_png = np.zeros((240, 320), bool); dep90 = np.zeros((240, 320), np.int32); dep100 = np.zeros((240, 320), np.int32)
        for o in objs:
            r = region_png(o)
            if r is None: continue
            ob = o.get("oben", e.get("oben", 4)); gr = o.get("grow", e.get("grow", 1))
            rh = halo(r, ob, gr)
            y0 = o.get("ebene", 0)
            d90 = tiefen_spaltenregel(rh, rot, trans, H, y0, 0.90); d100 = tiefen_spaltenregel(rh, rot, trans, H, y0, 1.00)
            dep90 = np.where((d90 > 0) & ((dep90 == 0) | (d90 < dep90)), d90, dep90)
            dep100 = np.where((d100 > 0) & ((dep100 == 0) | (d100 < dep100)), d100, dep100)
            reg_png |= r
        deck = kv["deck"]; tief = kv["tief"]
        t90 = tile_median(dep90, kv["rects"]); t100 = tile_median(dep100, kv["rects"])
        m = deck & (t90 > 0)
        gleich = float((t90[m] == tief[m]).mean()) if m.any() else 0.0
        vn, vv, vt = vor_audit(rooms[rid], kv, tief, deck, reg_png)
        vn2, vv2, vt2 = vor_audit(rooms[rid], kv, t100, deck & (t100 > 0), reg_png)
        W("%-9s %-3d %9.1f%% %8d %8d %8d | %8d %8d" % (room, c, 100 * gleich, vn, vv, vt, vv2, vt2))

# ------------------------------------------------------------------ Bilder
def bild(room, c, name):
    rid = int(room[4:], 16); kv = rooms[rid]["cuts"][c]
    bg = load_bg("build/bg_ppm", rid, c)
    if bg is None: return
    e = AUS[room][str(c)]
    png_u = np.zeros((240, 320), bool)
    for o in e.get("objekte") or []:
        if "png" in o:
            r = region_png(o)
            if r is not None: png_u |= r
    deck = kv["deck"]; tief = kv["tief"]
    ov = bg.astype(float) * 0.55
    both = deck & png_u; loch = png_u & ~deck; ueber = deck & ~png_u
    ov[both] = ov[both] * 0.4 + np.array([255, 0, 200]) * 0.6
    ov[loch] = ov[loch] * 0.3 + np.array([255, 30, 30]) * 0.7
    ov[ueber] = ov[ueber] * 0.3 + np.array([255, 230, 0]) * 0.7
    Z = 3
    im = Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for (sx, sy, dx, dy, w, h, dep) in kv["rects"]:
        d.rectangle([dx * Z, dy * Z, (dx + w) * Z - 1, (dy + h) * Z - 1], outline=(0, 255, 255))
        d.text((dx * Z + 2, dy * Z + 1), str(dep), fill=(0, 255, 255))
    d.text((4, 4), "%s C%d  magenta=PNG&Maske  rot=PNG ohne Maske (Loch)  gelb=Maske ohne PNG (eigene/Halo)  cyan=Kachel+Tiefe" % (room, c), fill=(255, 255, 255))
    p = os.path.join(BILD, "%s_C%d_%s.png" % (room, c, name)); im.save(p); W("   Bild: %s" % p)

W("\n# Bilder")
for room, c, name in [("ROOM1000", 3, "loecher"), ("ROOM10D0", 6, "lage"), ("ROOM10D0", 7, "quader_eigene"), ("ROOM10F0", 4, "eigene_dunkel"),
                      ("ROOM1010", 6, "halo"), ("ROOM1050", 6, "tiefenstaffel"), ("ROOM1000", 5, "faktor"), ("ROOM10A0", 0, "faktor"), ("ROOM1000", 1, "massstab4")]:
    bild(room, c, name)
out.close()
