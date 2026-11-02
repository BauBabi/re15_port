"""schnitte.py VIDEO [SCHWELLE] - Schnittkanten per Bilddifferenz (160x120 grau, mittlere Absolutdifferenz je Bildpaar).
Gibt Bildnummer, Videozeit (pts), Differenz und mittlere Helligkeit aus; Zeit aus der tatsaechlichen Bildrate (ffprobe)."""
import sys, subprocess, numpy as np
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
FP = r"C:\ProgramData\chocolatey\bin\ffprobe.exe"
v = sys.argv[1]; thr = float(sys.argv[2]) if len(sys.argv) > 2 else 25.0
r = subprocess.run([FP, "-v", "error", "-select_streams", "v:0", "-show_entries", "frame=pts_time", "-of", "csv=p=0", v],
                   capture_output=True, text=True).stdout.split()
pts = [float(x.strip(",")) for x in r if x.strip(",").strip()]
p = subprocess.run([FF, "-v", "error", "-i", v, "-vf", "scale=160:120,format=gray", "-f", "rawvideo", "-"], capture_output=True)
a = np.frombuffer(p.stdout, np.uint8).reshape(-1, 120, 160).astype(np.int16)
n = len(a)
d = np.abs(a[1:] - a[:-1]).mean(axis=(1, 2))
br = a.mean(axis=(1, 2))
np.save(v + ".diff.npy", np.stack([np.r_[0, d], br]))
print("Bilder %d, pts %d, letzte pts %.3f" % (n, len(pts), pts[-1] if pts else -1))
for i in range(1, n):
    if d[i - 1] >= thr:
        print("Bild %5d  t=%8.3f s  diff %5.1f  hell %5.1f -> %5.1f" % (i, pts[i] if i < len(pts) else i / 49.74682, d[i - 1], br[i - 1], br[i]))
