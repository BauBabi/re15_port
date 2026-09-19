# Riegel gegen falsch verortete Freistellungen: fuer JEDES STAGE1-PNG-Objekt der Auswahl
# das Nullmodell rechnen (Verteilung der Farb-Uebereinstimmung ueber ALLE Lagen) und
# melden, ob die eingetragene Lage der eindeutige Gipfel ist.
import os, sys, json
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
from geom import load_bg

SCHWELLE = 110
d = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
print("%-9s %-3s %-22s %5s %6s %6s %7s %6s %s"
      % ("Raum", "Cut", "PNG", "s", "best", "Gipfel", "Lagen95", "dLage", "Urteil"))
schlecht = []
for room in sorted(d):
    if not room.startswith("ROOM"):
        continue
    rid = int(room[4:], 16)
    if rid >= 0x2000:
        continue
    for cut, e in sorted(d[room].items(), key=lambda kv: (kv[0].startswith("_"), kv[0])):
        if cut.startswith("_") or not isinstance(e, dict):
            continue
        bg = load_bg("build/bg_ppm", rid, int(cut))
        if bg is None:
            continue
        bgf = bg.astype(np.float64)
        for o in e.get("objekte", []):
            if "png" not in o:
                continue
            s = int(o.get("massstab", 1))
            im0 = Image.open(o["png"]).convert("RGBA")
            im = im0 if s == 1 else im0.resize((max(1, im0.width // s), max(1, im0.height // s)), Image.BOX)
            a = np.asarray(im)
            if a.shape[0] > 240 or a.shape[1] > 320:
                print("%-9s %-3s %-22s %5d  PNG groesser als das Bild" % (room, cut, os.path.basename(o["png"]), s))
                continue
            m = a[:, :, 3] > SCHWELLE
            if m.sum() < 20:
                continue
            t = a[:, :, :3].astype(np.float64)
            h, w = m.shape
            ys, xs = np.nonzero(m)
            st = max(1, len(ys) // 400)
            ys2, xs2 = ys[::st], xs[::st]
            tv = t[ys2, xs2]
            feld = np.zeros((240 - h + 1, 320 - w + 1))
            for dy in range(240 - h + 1):
                for dx in range(320 - w + 1):
                    feld[dy, dx] = (np.abs(bgf[ys2 + dy, xs2 + dx] - tv).max(1) < 26).mean()
            best = feld.max(); sd = feld.std() or 1e-9; med = np.median(feld)
            nah = int((feld >= best * 0.95).sum())
            iy, ix = np.unravel_index(feld.argmax(), feld.shape)
            dl = abs(ix - int(o["x"])) + abs(iy - int(o["y"]))
            urteil = "ok" if (nah <= 8 and dl <= 2) else ("LAGE?" if nah <= 8 else "NICHT BESTIMMT")
            if urteil != "ok":
                schlecht.append((room, cut, o["png"], s, best, nah, dl))
            print("%-9s %-3s %-22s %5d %5.1f%% %5.1fs %7d %6d %s"
                  % (room, cut, os.path.basename(o["png"]), s, 100 * best,
                     (best - med) / sd, nah, dl, urteil))
print("\n%d von den geprueften Objekten sind auffaellig:" % len(schlecht))
for q in schlecht:
    print("   %s C%s %s s=%d best %.1f%% Lagen95 %d dLage %d" % (q[0], q[1], q[2], q[3], 100 * q[4], q[5], q[6]))
