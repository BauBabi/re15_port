"""Lage-Pruefung: passt jedes Nutzer-PNG an der in auswahl.json eingetragenen Stelle
(x, y, massstab) zum Hintergrund des Cuts? Metrik = maske_aus_png.platziere (Anteil der
freigestellten Punkte mit Kanalabstand < 26). Bei schlechter Uebereinstimmung wird die
beste Stelle ueber alle Massstaebe gesucht (dieselbe Suche wie platziere)."""
import json, os, sys, time
import numpy as np
from PIL import Image
sys.path.insert(0, r"C:\workspace\git\reAi_v2\re15_port\tools\maske")
os.chdir(r"C:\workspace\git\reAi_v2")
import maske_aus_png as MAP
from geom import load_bg

aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
out = open(sys.argv[1], "w", encoding="utf-8")

def match_at(png, bg, x, y, s, thr=110):
    im = MAP._skaliere(Image.open(png).convert("RGBA"), s)
    a = np.asarray(im)
    m = a[:, :, 3] > thr
    t = a[:, :, :3].astype(np.float64)
    h, w = m.shape
    ys, xs = np.nonzero(m)
    ok = (ys + y >= 0) & (ys + y < 240) & (xs + x >= 0) & (xs + x < 320)
    ys, xs = ys[ok], xs[ok]
    if len(ys) == 0:
        return 0.0, 0, (w, h)
    d = np.abs(bg.astype(np.float64)[ys + y, xs + x] - t[ys, xs]).max(1)
    return float((d < 26).mean()), int(len(ys)), (w, h)

for room in sorted(k for k in aus if k.startswith("ROOM1")):
    rid = int(room[4:], 16)
    for cut in sorted((c for c in aus[room] if not c.startswith("_")), key=int):
        e = aus[room][cut]
        if not isinstance(e, dict):
            continue
        for o in e.get("objekte") or []:
            if "png" not in o:
                continue
            bg = load_bg("build/bg_ppm", rid, int(cut))
            if bg is None:
                out.write("%s %s %s HINTERGRUND FEHLT\n" % (room, cut, o["png"])); continue
            x, y, s = o.get("x"), o.get("y"), o.get("massstab", 1)
            g, n, (w, h) = match_at(o["png"], bg, x, y, s)
            line = "%s C%s %-28s eingetragen x=%3d y=%3d s=%d -> %dx%d  Treffer %5.1f %% (%d Punkte)" % (
                room, cut, o["png"][4:], x, y, s, w, h, 100 * g, n)
            if g < 0.85:
                t0 = time.time()
                r, info = MAP.platziere(o["png"], bg)
                line += " | SUCHE: best s=%d x=%d y=%d %5.1f %% (%d px, %.0fs)" % (
                    info.get("massstab", -1), info.get("x", -1), info.get("y", -1),
                    100 * info.get("uebereinstimmung", 0), info.get("punkte", 0), time.time() - t0)
            out.write(line + "\n"); out.flush()
            print(line, flush=True)
out.close()
