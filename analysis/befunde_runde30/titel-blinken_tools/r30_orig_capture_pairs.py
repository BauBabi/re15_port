#!/usr/bin/env python3
"""r30_orig_capture_pairs.py - Runde 30 / Thema D.

Wertet die DuckStation-Aufnahme des ORIGINAL-Titelmenues aus
(shots/title_confirm_capture.avi, 640x480, 59.8173 Bilder/s = 1 Bild je VBlank):
Helligkeit der aktiven Zeile NEW GAME je Bild -> wie viele Bilder haelt eine
Helligkeitsstufe?  Erwartung aus der Disassembly: 2 (VSync(2) je Pulsschritt).

Eingabe: die von ffmpeg geschriebene Metadaten-Datei
  ffmpeg -i shots/title_confirm_capture.avi -frames:v 69 \
     -vf "crop=512:34:64:266,signalstats,metadata=print:key=lavfi.signalstats.YAVG:file=<datei>" -f null -
Aufruf: r30_orig_capture_pairs.py <datei> [erstes_bild] [letztes_bild]
"""
import sys, re

def main():
    txt = open(sys.argv[1]).read()
    ys = [float(x) for x in re.findall(r"YAVG=([0-9.]+)", txt)]
    ts = [float(x) for x in re.findall(r"pts_time:([0-9.]+)", txt)]
    a = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    b = int(sys.argv[3]) if len(sys.argv) > 3 else 68
    EPS = 0.02          # Rauschgrenze der VP9-Kompression (groesste Schwankung INNERHALB einer Stufe: 0.0071)
    runs = []; start = a
    for i in range(a + 1, b + 1):
        if abs(ys[i] - ys[i - 1]) > EPS:
            runs.append((start, i - 1)); start = i
    runs.append((start, b))
    lens = [e - s + 1 for s, e in runs]
    print("Bilder %d..%d (%.4f s .. %.4f s)" % (a, b, ts[a], ts[b]))
    print("Helligkeitsstufen: %d" % len(runs))
    hist = {}
    for l in lens: hist[l] = hist.get(l, 0) + 1
    print("Haltedauer je Stufe (Bilder -> Anzahl): %s" % dict(sorted(hist.items())))
    dt = (ts[b] - ts[a]) / (b - a)
    print("Bildabstand der Aufnahme: %.4f ms  (%.4f Bilder/s)" % (dt * 1000, 1 / dt))
    full = [l for l in lens[1:-1]]
    if full:
        m = sum(full) / len(full)
        print("mittlere Haltedauer (ohne Randstufen): %.3f Bilder = %.3f ms" % (m, m * dt * 1000))
        print("=> 60 Pulsschritte = %.1f ms" % (60 * m * dt * 1000))
    imax = max(range(a, b + 1), key=lambda i: ys[i])
    print("hellstes Bild: %d (YAVG %.4f); danach %d Stufen abwaerts bis Bild %d" % (
        imax, ys[imax], sum(1 for s, e in runs if s > imax), b))

if __name__ == "__main__":
    main()
