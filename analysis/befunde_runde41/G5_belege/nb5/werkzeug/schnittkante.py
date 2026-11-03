import subprocess, numpy as np
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
t0 = 60.0; dur = 4.0
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % dur, "-i", SRC,
                      "-vf", "scale=160:120,format=gray", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 120, 160).astype(float)
print("Bilder", len(v))
d = np.abs(np.diff(v, axis=0)).mean(axis=(1,2))
for k in range(len(d)):
    t = t0 + (k+1)/VFPS
    if d[k] > 4: print("t %.3f  diff %.1f  mean %.1f -> Szenenbild %.1f" % (t, d[k], v[k+1].mean(), (t-63.47)*29.9))
