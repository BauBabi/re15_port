# Schnittkanten + Bewegung im echten RE2-Mitschnitt (Lauf 1, 640x480, 59,8 B/s), eigene Messung
import subprocess, numpy as np, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
src = sys.argv[1]; t0 = float(sys.argv[2]); dur = float(sys.argv[3])
W, H = 80, 60
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(t0), "-t", str(dur), "-i", src, "-vf",
                      "scale=%d:%d,format=rgb24" % (W, H), "-vsync", "0", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, H, W, 3).astype(np.int16)
# Zeitstempel je Bild
pts = subprocess.run([r"C:/ProgramData/chocolatey/bin/ffprobe", "-v", "error", "-select_streams", "v:0", "-read_intervals",
                      "%f%%+%f" % (t0, dur), "-show_entries", "frame=pts_time", "-of", "csv=p=0", src], capture_output=True, text=True).stdout.split()
pts = [float(p) for p in pts if p.strip()]
pts = [p for p in pts if p >= t0 - 1e-3]
n = min(len(pts), len(v))
d = np.abs(np.diff(v[:n], axis=0)).mean(axis=(1, 2, 3))
np.save("re2_diff.npy", np.array([pts[1:n], d]))
med = np.median(d)
print("Bilder", n, "Median-Diff", round(float(med), 2))
for i in range(len(d)):
    if d[i] > 15: print("Kante %.3f s diff %.1f" % (pts[i + 1], d[i]))
