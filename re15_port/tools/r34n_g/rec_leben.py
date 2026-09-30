#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht, Stufe BAU) - Lebendnachweis + exakte Pixel-Spannen einer Aufnahme (Auflage 3).

Liest eine VERLUSTFREIE gdigrab-Aufnahme (ds_rec*.py: libx264rgb -qp 0, gbrp) Bild fuer Bild ueber
eine ffmpeg-Pipe und misst OHNE Schwelle (jede Aenderung eines Farbwerts zaehlt):

  * Lebendnachweis: Bilder, die sich vom vorigen unterscheiden, und das umschliessende Rechteck
    aller Aenderungen - getrennt fuer das ganze Bild und fuer ein Pruefrechteck.
  * Pruefrechteck: je Pixel und Kanal min/max ueber alle Bilder -> groesste Spanne (0 = in keinem
    Bild auch nur ein Farbwert anders), dazu mittlere Helligkeit min/max.

Koordinaten des Pruefrechtecks in PSX-Bildpunkten (320x240); die Aufnahme zeigt Bildspeicher 0
(VRAM x0..319 y0..239) auf 600x450 vergroessert (ds_rec_fb0.py: -offset 0,48 -video_size 600x450),
Faktor 600/320 = 1,875. Fuer eine Ausschnitt-Aufnahme (ds_rec_small.py) --voll angeben: dann ist
das ganze Bild das Pruefrechteck.

  C:/Python310/python.exe re15_port/tools/r34n_g/rec_leben.py <aufnahme.mkv> [--box x0,y0,x1,y1] [--voll]
      [--von N] [--bis N]
"""
import argparse, json, subprocess, sys
import numpy as np

FFMPEG = "C:/ProgramData/chocolatey/bin/ffmpeg"
FFPROBE = "C:/ProgramData/chocolatey/bin/ffprobe"


def probe(path):
    out = subprocess.run([FFPROBE, "-v", "error", "-select_streams", "v:0", "-show_entries",
                          "stream=width,height,codec_name,pix_fmt", "-of", "json", path],
                         capture_output=True, text=True, check=True).stdout
    s = json.loads(out)["streams"][0]
    return s["width"], s["height"], s["codec_name"], s["pix_fmt"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rec")
    ap.add_argument("--box", default="")
    ap.add_argument("--voll", action="store_true")
    ap.add_argument("--von", type=int, default=0)
    ap.add_argument("--bis", type=int, default=1 << 30)
    a = ap.parse_args()
    w, h, codec, pix = probe(a.rec)
    print("%s: %dx%d %s %s" % (a.rec.replace("\\", "/").split("/")[-1], w, h, codec, pix))
    if a.voll:
        bx = (0, 0, w - 1, h - 1)
    elif a.box:
        x0, y0, x1, y1 = [int(v) for v in a.box.split(",")]
        k = w / 320.0
        bx = (int(x0 * k), int(y0 * k), min(w - 1, int((x1 + 1) * k) - 1), min(h - 1, int((y1 + 1) * k) - 1))
    else:
        bx = None
    p = subprocess.Popen([FFMPEG, "-v", "error", "-i", a.rec, "-f", "rawvideo", "-pix_fmt", "rgb24", "-"],
                         stdout=subprocess.PIPE)
    fs = w * h * 3
    prev = None
    n = 0
    n_diff = 0
    first_diff = []
    ubb = None
    n_diff_box = 0
    bmin = bmax = None
    lum_min, lum_max = 1e9, -1e9
    while True:
        buf = p.stdout.read(fs)
        if len(buf) < fs:
            break
        idx = n
        n += 1
        if idx < a.von or idx > a.bis:
            continue
        f = np.frombuffer(buf, dtype=np.uint8).reshape(h, w, 3)
        if prev is not None:
            d = np.any(f != prev, axis=2)
            if d.any():
                n_diff += 1
                if len(first_diff) < 8:
                    first_diff.append(idx)
                ys, xs = np.nonzero(d)
                bb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
                ubb = bb if ubb is None else (min(ubb[0], bb[0]), min(ubb[1], bb[1]),
                                              max(ubb[2], bb[2]), max(ubb[3], bb[3]))
                if bx and d[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1].any():
                    n_diff_box += 1
        if bx:
            c = f[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1]
            bmin = c.copy() if bmin is None else np.minimum(bmin, c)
            bmax = c.copy() if bmax is None else np.maximum(bmax, c)
            lum = float(c.mean())
            lum_min, lum_max = min(lum_min, lum), max(lum_max, lum)
        prev = f
    p.wait()
    von = a.von
    bis = min(a.bis, n - 1)
    m = bis - von + 1
    print("Bilder %d (ausgewertet %d..%d = %d)" % (n, von, bis, m))
    print("GANZES BILD: Bilder mit Aenderung gegen das vorige: %d von %d; erste: %s; Rechteck aller Aenderungen (Aufnahme-px): %s"
          % (n_diff, max(0, m - 1), first_diff, ubb))
    if ubb:
        k = w / 320.0
        print("             dasselbe in PSX-px: x%d..%d y%d..%d" % (int(ubb[0] / k), int(ubb[2] / k),
                                                                   int(ubb[1] / k), int(ubb[3] / k)))
    if bx:
        span = int((bmax.astype(np.int16) - bmin.astype(np.int16)).max())
        print("PRUEFRECHTECK Aufnahme-px %s: Bilder mit Aenderung im Rechteck: %d; groesste Spanne je Pixel/Kanal "
              "ueber alle Bilder: %d; mittlere Helligkeit %.2f..%.2f" % (bx, n_diff_box, span, lum_min, lum_max))
    return 0


if __name__ == "__main__":
    sys.exit(main())
