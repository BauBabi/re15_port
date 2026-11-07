# Abnahme 5: RE2-Aufnahme (640x480, 59,817 B/s) - Videobilder 60,8..64,2 s, Schnittkante Schnitt 9 und Bildverschiebung (Beben) selbst messen
import subprocess, numpy as np, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5/re2/re2_lauf1.mp4"
t0, dauer = float(sys.argv[1]), float(sys.argv[2])
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-i", SRC, "-t", "%.3f" % dauer,
                      "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "gray", "-"], capture_output=True).stdout
n = len(raw) // (320 * 240)
v = np.frombuffer(raw[:n * 320 * 240], np.uint8).reshape(n, 240, 320).astype(np.int16)
np.save(sys.argv[3], v)
fps = 598173 / 10000
print('Bilder', n)
for i in range(1, n):
    d = np.abs(v[i] - v[i - 1]).mean()
    # vertikale Verschiebung gegen das Vorbild: beste Zeilenverschiebung -10..10 in der Bildmitte (Zeilen 40..200, Spalten 40..280)
    best = None
    for s in range(-12, 13):
        a = v[i][40 + s:200 + s, 40:280]; b = v[i - 1][40:200, 40:280]
        e = np.abs(a - b).mean()
        if best is None or e < best[1]: best = (s, e)
    print('%3d t %.3f  diff %6.2f  dy(vs vorher) %+d (%.2f)' % (i, t0 + i / fps, d, best[0], best[1]))
