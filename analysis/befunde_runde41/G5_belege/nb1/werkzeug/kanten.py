import subprocess, numpy as np, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
d = sys.argv[1]
W, H = 80, 60
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", d + "/fenster.mp4", "-vf", "scale=%d:%d,format=rgb24" % (W, H), "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, H, W, 3).astype(np.int16)
diff = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2, 3))
# Farbton: Rot-Blau-Ueberhang (orange RE1.5 vs grau RE2)
rb = (v[..., 0] - v[..., 2]).mean(axis=(1, 2))
print("frames", len(v))
for i in sorted(np.argsort(-diff)[:14]): print("t %.2f diff %.1f  rb vorher %.1f nachher %.1f" % ((i + 1) / 20, diff[i], rb[max(0, i - 3)], rb[min(len(rb) - 1, i + 4)]))
