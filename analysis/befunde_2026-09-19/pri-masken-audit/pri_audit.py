"""PRI-Masken-Audit STAGE1 — liest den Engine-Dump der Sonde probe_r16_pri_masken_audit
(Rechtecke/Deckung/Tiefe/Bodenpunkte/Kamera aus den ECHTEN Engine-Lesern) und vergleicht
mit den Nutzer-PNGs (Alpha > 110 an der in auswahl.json eingetragenen Lage).

Ausgabe: Tabelle je Raum/Cut (Loecher, Ueberschuss, Quelle, Tiefen) + Tiefen-Audit gegen
die begehbaren Bodenpunkte der Engine."""
import json, os, sys, struct
import numpy as np
from PIL import Image
sys.path.insert(0, r"C:\workspace\git\reAi_v2\re15_port\tools\maske")
os.chdir(r"C:\workspace\git\reAi_v2")
import maske_aus_png as MAP
from scipy import ndimage
MAP.ndimage = ndimage
import maskenbild as MB
from geom import load_bg

DUMP = sys.argv[1]
OUT = sys.argv[2]
AUS = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
MASKS = "re15_port/shared_assets/PSX/MASKS"
KOPF_HOCH = 1500
HALB_BREIT = 450
BAND_H = 0x708

# ---------------------------------------------------------------- Dump lesen
rooms = {}
cur = None; curcut = None; curband = None
lines = open(DUMP).read().split("\n")
i = 0
while i < len(lines):
    ln = lines[i]
    if ln.startswith("ROOM "):
        p = ln.split()
        rid = int(p[1], 16)
        cur = rooms[rid] = {"cuts": {}, "floor": {}, "msk": int(p[5][4:]), "ncuts": int(p[2][5:])}
    elif ln.startswith("B "):
        curband = int(ln.split()[1]); cur["floor"][curband] = []
    elif ln.startswith("F "):
        p = ln.split(); cur["floor"][curband].append((int(p[1]), int(p[2])))
    elif ln.startswith("C "):
        p = ln.split()
        if p[2] == "KEINE_SICHT":
            i += 1; continue
        c = int(p[1])
        kv = {}
        j = 2
        while j < len(p):
            if p[j].startswith("rot="):
                kv["rot"] = [int(p[j][4:])] + [int(v) for v in p[j+1:j+9]]; j += 9; continue
            if p[j].startswith("trans="):
                kv["trans"] = [int(p[j][6:])] + [int(v) for v in p[j+1:j+3]]; j += 3; continue
            k, v = p[j].split("="); kv[k] = int(v); j += 1
        kv["rects"] = []
        cur["cuts"][c] = kv; curcut = kv
    elif ln.startswith("R "):
        p = [int(v) for v in ln.split()[1:]]
        curcut["rects"].append(p[1:])       # sx sy dx dy w h depth
    elif ln.startswith("T "):
        p = ln.split(); curcut["tim"] = {k: int(v) for k, v in (q.split("=") for q in p[2:])}
    elif ln.startswith("D "):
        rows = lines[i+1:i+241]
        a = np.zeros((240, 320), bool)
        for y, r in enumerate(rows):
            bits = np.array([int(ch, 16) for ch in r], np.uint8)
            a[y] = np.unpackbits(bits[:, None] << 4, axis=1)[:, :4].reshape(-1)[:320]
        curcut["deck"] = a
        i += 240
    elif ln.startswith("P "):
        rows = lines[i+1:i+241]
        a = np.zeros((240, 320), np.int32)
        for y, r in enumerate(rows):
            a[y] = [int(r[k:k+3], 16) for k in range(0, 960, 3)]
        curcut["tief"] = a
        i += 240
    i += 1

# ------------------------------------------------- Python-Leser gegen Engine pruefen
def py_deckung(room, cut):
    p = os.path.join(MASKS, "%s.MSK" % room)
    if not os.path.exists(p):
        return None, None
    ms = MB.masken(open(p, "rb").read(), cut)
    if not ms:
        return None, None
    t = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
    idx = t[0]
    deck = np.zeros((240, 320), bool); tief = np.zeros((240, 320), np.int32)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w); y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0: continue
        sub = idx[sy+(y0-Y):sy+(y1-Y), sx+(x0-X):sx+(x1-X)]
        op = sub != 0
        deck[y0:y1, x0:x1] |= op
        z = tief[y0:y1, x0:x1]
        tief[y0:y1, x0:x1] = np.where(op & ((z == 0) | (dep < z)), dep, z)
    return deck, tief

def region_png(o):
    return MAP.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))

def region_png_roh(o, thr):
    """Alpha-Region mit anderer Schwelle (fuer die Frage, was die 110 wegschneidet)."""
    return MAP.setze(o["png"], o["x"], o["y"], o.get("massstab", 1), alpha_schwelle=thr)

def proj(rot, trans, H, x, y, z):
    vx = (x*rot[0] + y*rot[1] + z*rot[2]) / 4096.0 + trans[0]
    vy = (x*rot[3] + y*rot[4] + z*rot[5]) / 4096.0 + trans[1]
    vz = (x*rot[6] + y*rot[7] + z*rot[8]) / 4096.0 + trans[2]
    if vz <= 64: return None
    return 160 + vx*H/vz, 120 + vy*H/vz, vz

def vz_senkrechte(rot, trans, H, wx, wz, sy):
    a = rot[4] / 4096.0; b = (rot[3]*wx + rot[5]*wz) / 4096.0 + trans[1]
    c = rot[7] / 4096.0; d = (rot[6]*wx + rot[8]*wz) / 4096.0 + trans[2]
    n = (sy - 120.0)*c - H*a
    if abs(n) < 1e-9: return None
    y = (H*b - (sy - 120.0)*d) / n
    return c*y + d

def vz_boden(rot, trans, H, sx, sy, y0):
    a = (sx - 160.0) / H; b = (sy - 120.0) / H
    k = rot[1]*a + rot[4]*b + rot[7]
    c = rot[1]*trans[0] + rot[4]*trans[1] + rot[7]*trans[2]
    if abs(k) < 1e-9: return None
    vz = (y0*4096.0 + c) / k
    return vz if vz > 1 else None

out = open(OUT, "w", encoding="utf-8")
def W(s=""):
    out.write(s + "\n"); print(s, flush=True)

W("# Teil A: Leser-Verifikation (Python maskenbild.masken/lies_tim gegen Engine-Deckung)")
nver = 0; nbad = 0
for rid in sorted(rooms):
    room = "ROOM%04X" % rid
    for c, kv in sorted(rooms[rid]["cuts"].items()):
        if "deck" not in kv: continue
        d, t = py_deckung(room, c)
        if d is None: W("  %s C%d: Python-Leser liefert NICHTS, Engine %d px" % (room, c, kv["deck"].sum())); nbad += 1; continue
        eq = (d == kv["deck"]).all() and (t == kv["tief"]).all()
        nver += 1
        if not eq:
            nbad += 1
            W("  %s C%d: ABWEICHUNG Deckung %d vs %d, Tiefe gleich=%s" % (room, c, int(d.sum()), int(kv["deck"].sum()), (t == kv["tief"]).all()))
W("  verifiziert: %d Cuts, Abweichungen: %d" % (nver, nbad))

W("\n# Teil B: Pixelmenge je Cut — Maske (Engine) gegen PNG-Alpha>110 an der eingetragenen Lage")
W("%-9s %-3s %5s %7s %7s %7s %7s %7s %5s %5s %8s %-30s" % ("Raum", "Cut", "Rects", "MaskePx", "PNGPx", "Loch", "Ueber", "eigene", "tMin", "tMax", "clut0", "Quelle/Tiefenschluessel"))
tabelle = {}
for room in sorted(k for k in AUS if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    if rid not in rooms: continue
    for cut in sorted((c for c in AUS[room] if not c.startswith("_")), key=int):
        e = AUS[room][cut]; c = int(cut)
        if not isinstance(e, dict): continue
        objs = e.get("objekte") or []
        kv = rooms[rid]["cuts"].get(c)
        png_u = np.zeros((240, 320), bool); png_objs = []; quellen = []; tiefk = []
        for o in objs:
            src = [k for k in ("png", "polygon", "zauberstab", "kontrast", "quader", "kaesten", "ids") if k in o]
            quellen.append(src[0] if src else "?")
            tk = [k for k in ("fuss", "ebene", "aufrecht", "flach", "tiefe", "spalten", "zelle", "bodenkante") if k in o]
            tiefk.append("+".join("%s" % k for k in tk) or "SPALTENREGEL")
            if "png" in o:
                r = region_png(o)
                if r is not None:
                    png_u |= r; png_objs.append((o, r))
        q = ",".join(sorted(set(quellen)))
        if kv is None or "deck" not in kv:
            n = kv["n"] if kv else -1
            orig = kv["orig"] if kv else -1
            W("%-9s %-3d %5s %7s %7d %7s %7s %7s %5s %5s %8s %s  <-- KEINE MASKE (n=%d orig=%d)" % (room, c, "-", "-", int(png_u.sum()), "-", "-", "-", "-", "-", "-", q, n, orig))
            tabelle[(rid, c)] = {"fehlt": True, "png": int(png_u.sum()), "quelle": q}
            continue
        deck = kv["deck"]; tief = kv["tief"]
        loch = int((png_u & ~deck).sum()); ueber = int((deck & ~png_u).sum())
        # 'eigene' = Maskenpixel, die aus NICHT-PNG-Objekten stammen koennen (Ueberschuss, wenn es solche gibt)
        eigene = ueber if any(s != "png" for s in quellen) else 0
        tv = tief[deck]
        tabelle[(rid, c)] = {"fehlt": False, "png": int(png_u.sum()), "maske": int(deck.sum()), "loch": loch, "ueber": ueber,
                             "quelle": q, "tief": (int(tv.min()), int(tv.max())) if tv.size else (0, 0), "tiefk": tiefk,
                             "rects": len(kv["rects"]), "draw": kv["draw"], "clut0": kv["tim"]["clut0000"]}
        W("%-9s %-3d %5d %7d %7d %7d %7d %7d %5d %5d %8d %s | %s" % (room, c, kv["n"], int(deck.sum()), int(png_u.sum()), loch, ueber, eigene,
            tv.min() if tv.size else 0, tv.max() if tv.size else 0, kv["tim"]["clut0000"], q, ";".join(tiefk)))
        # je PNG-Objekt: Loch/Ueberschuss + was die Alpha-Schwelle/Massstab wegschneidet
        for o, r in png_objs:
            r0 = region_png_roh(o, 0)
            im = Image.open(o["png"]).convert("RGBA"); a = np.asarray(im)[:, :, 3]
            voll = int((a > 110).sum())
            s = o.get("massstab", 1)
            W("      %-26s s=%d Alpha>110 im PNG %6d -> im Bild %6d | Loch %5d Ueber(um das Objekt, 2px) %5d | Alpha>0 statt >110: +%d px" % (
                o["png"][4:], s, voll, int(r.sum()), int((r & ~deck).sum()),
                int((deck & ~r & MAP.ndimage.binary_dilation(r, iterations=2)).sum()) if hasattr(MAP, "ndimage") else -1,
                int((r0 & ~r).sum())))

W("\n# Teil C: Tiefen-Audit gegen die begehbaren Bodenpunkte der Engine (Koerper 1500 hoch, +-450 breit)")
W("#  je Standplatz, dessen Koerperkasten Maskenpixel beruehrt: Anteil der beruehrten Pixel, die verdeckt werden")
W("#  (Maske naeher: depth*64 < Koerper-vz in dieser Bildzeile). VOR = Fuss naeher als die PNG-Bodenlinie der")
W("#  Spalte (Objekt-Standlinie aus der untersten PNG-Zeile je Spalte auf der Bodenebene des Bands).")
W("%-9s %-3s %6s %6s %6s %6s %6s | %6s %6s %6s | %6s %6s" % ("Raum", "Cut", "Plaetze", "beruehr", "voll", "teil", "frei", "VORn", "VORverd", "VORteil", "HINTn", "HINTfrei"))
for room in sorted(k for k in AUS if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    if rid not in rooms: continue
    R = rooms[rid]
    for cut in sorted((c for c in AUS[room] if not c.startswith("_")), key=int):
        c = int(cut); kv = R["cuts"].get(c)
        if kv is None or "deck" not in kv: continue
        e = AUS[room][cut]
        rot, trans, H = kv["rot"], kv["trans"], kv["H"]
        deck, tief = kv["deck"], kv["tief"]
        # Objekt-Standlinie je Spalte aus den PNGs (unterste Zeile je Spalte)
        png_u = np.zeros((240, 320), bool)
        for o in (e.get("objekte") or []):
            if "png" in o:
                r = region_png(o)
                if r is not None: png_u |= r
        n_pl = 0; ber = 0; voll = 0; teil = 0; frei = 0
        vor_n = 0; vor_verd = 0; vor_teil = 0; hint_n = 0; hint_frei = 0
        for band, pts in R["floor"].items():
            yfoot = -band * BAND_H
            # Standlinie des Objekts fuer dieses Band
            stand_vz = np.full(320, np.nan)
            for x in range(320):
                rows = np.nonzero(png_u[:, x])[0]
                if len(rows):
                    v = vz_boden(rot, trans, H, x + 0.5, float(min(int(rows.max()), 239)) + 0.5, yfoot)
                    if v: stand_vz[x] = v
            for (wx, wz) in pts:
                pf = proj(rot, trans, H, wx, yfoot, wz)
                if pf is None: continue
                pk = proj(rot, trans, H, wx, yfoot - KOPF_HOCH, wz)
                if pk is None: continue
                n_pl += 1
                fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
                hw = HALB_BREIT * H / fvz
                x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
                y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
                if x1 <= x0 or y1 <= y0: continue
                box = deck[y0:y1, x0:x1]
                if not box.any(): continue
                ber += 1
                # Koerper-vz je Zeile (senkrechte Linie durch den Fusspunkt)
                vzs = np.array([vz_senkrechte(rot, trans, H, wx, wz, y + 0.5) or fvz for y in range(y0, y1)])
                td = tief[y0:y1, x0:x1].astype(np.float64)
                verd = box & (td * 64.0 < vzs[:, None])
                anteil = verd.sum() / box.sum()
                if anteil >= 0.95: voll += 1
                elif anteil <= 0.05: frei += 1
                else: teil += 1
                # vor/hinter dem Objekt je Spalte
                sv = stand_vz[x0:x1]
                colmask = box.any(0) & np.isfinite(sv)
                if colmask.any():
                    vor_cols = colmask & (fvz < sv - 1)      # Fuss naeher als die Standlinie
                    hin_cols = colmask & (fvz > sv + 1)
                    if vor_cols.any():
                        vor_n += 1
                        bv = box[:, vor_cols]; vv = verd[:, vor_cols]
                        q = vv.sum() / max(1, bv.sum())
                        if q >= 0.95: vor_verd += 1
                        elif q > 0.05: vor_teil += 1
                    if hin_cols.any():
                        hint_n += 1
                        bh = box[:, hin_cols]; vh = verd[:, hin_cols]
                        if vh.sum() / max(1, bh.sum()) < 0.95: hint_frei += 1
        W("%-9s %-3d %6d %6d %6d %6d %6d | %6d %6d %6d | %6d %6d" % (room, c, n_pl, ber, voll, teil, frei, vor_n, vor_verd, vor_teil, hint_n, hint_frei))
        tabelle[(rid, c)].update({"plaetze": n_pl, "beruehrt": ber, "voll": voll, "teil": teil, "frei": frei,
                                   "vor_n": vor_n, "vor_verd": vor_verd, "vor_teil": vor_teil, "hint_n": hint_n, "hint_frei": hint_frei})

json.dump({"%04X_%d" % k: v for k, v in tabelle.items()}, open(OUT + ".json", "w"), indent=1, default=str)
out.close()
