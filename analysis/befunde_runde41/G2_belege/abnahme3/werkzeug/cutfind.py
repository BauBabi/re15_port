import sys, subprocess, numpy as np
vid, t0, dur = sys.argv[1], float(sys.argv[2]), float(sys.argv[3])
W, H = 80, 60
p = subprocess.run(["C:/ProgramData/chocolatey/bin/ffmpeg", "-v", "error", "-ss", str(t0), "-t", str(dur), "-i", vid, "-vf", "scale=%d:%d,format=gray" % (W, H), "-f", "rawvideo", "-"], capture_output=True)
v = np.frombuffer(p.stdout, np.uint8).reshape(-1, H, W).astype(np.int16)
d = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2))
for i in np.argsort(-d)[:6]: print("t=%.3f d=%.1f" % (t0 + (i + 1) / 30.0, d[i]))
