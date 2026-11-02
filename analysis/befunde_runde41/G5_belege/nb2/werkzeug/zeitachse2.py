import subprocess, numpy as np, sys, re, json, os
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
d = sys.argv[1]
W, H = 80, 60
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", d + "/fenster.mp4", "-vf", "scale=%d:%d,format=rgb24" % (W, H), "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, H, W, 3).astype(np.int16)
diff = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2, 3))
rb = (v[..., 0] - v[..., 2]).mean(axis=(1, 2))
cams = {}
for z in open(os.path.join(d, "state.log"), encoding="latin-1"):
    m = re.match(r"F(\d+) .* cam=(\d+) ", z)
    if m: cams[int(m.group(1))] = int(m.group(2))
Fs = sorted(cams)
F_vl = next(F for F in Fs if cams[F] == 16)
F_437 = next(F for F in Fs if F > F_vl and cams[F] == 17)
F_807 = next(F for F in Fs if F > F_437 and cams[F] == 12)
arm = None
for z in open(os.path.join(d, "debug.log"), encoding="latin-1"):
    m = re.search(r"\[g5auftritt\] F(\d+) .*Armierung", z)
    if m: arm = int(m.group(1))
n = len(v)
# Kante 1: orange -> grau (RE1.5-Schnitt -> RE2-Schnitt 16); Kante 3: grau -> orange (17 -> Cut 12)
k1 = [i for i in range(3, n - 5) if diff[i] > 15 and rb[i - 3] > 40 and rb[i + 4] < 35]
best = None
for i1 in k1:
    for i3 in range(i1 + 20, n - 5):
        if diff[i3] > 15 and rb[i3 - 3] < 25 and rb[i3 + 4] > 40:
            b = ((i3 + 1) - (i1 + 1)) / 20.0 / (F_807 - F_vl)
            t2 = (i1 + 1) / 20.0 + b * (F_437 - F_vl)
            j = int(round(t2 * 20)) - 1
            lo, hi = max(0, j - 10), min(n - 1, j + 10)
            s = diff[lo:hi].max()
            if best is None or s > best[0]: best = (s, i1, i3, b)
            break
s, i1, i3, b = best
t1 = (i1 + 1) / 20.0
a = t1 - b * F_vl
print(json.dumps({"a": a, "b": b, "F_arm": arm, "F_vl": F_vl, "F_437": F_437, "F_807": F_807, "kanten": [t1, a + b * F_437, (i3 + 1) / 20.0], "kante2_diff": float(s)}))
