# Abnahme 2: Zeitachse Port-Fensterlauf (gdigrab 20 B/s) <-> Spielbild, eigene Messung.
# Spielbilder der Schnittwechsel aus state.log (cam 16 = Vorlaufbeginn, 17 = Bild 437, 12 = Bild 807), Videokanten per Bilddifferenz.
import subprocess, numpy as np, sys, re, json, os
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
d = sys.argv[1]
W, H = 80, 60
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", d + "/fenster.mp4", "-vf", "scale=%d:%d,format=rgb24" % (W, H),
                      "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, H, W, 3).astype(np.int16)
diff = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2, 3))
rb = (v[..., 0] - v[..., 2]).mean(axis=(1, 2))           # orange (RE1.5) gegen grau (RE2)
cams = {}; sec = 0; last = -1
for z in open(os.path.join(d, "state.log"), encoding="latin-1"):
    m = re.match(r"F(\d+) .* cam=(\d+) ", z)
    if not m: continue
    F = int(m.group(1))
    if F < last: sec += 1
    last = F
    if sec == 1: cams[F] = int(m.group(2))
Fs = sorted(cams)
F16 = next(F for F in Fs if cams[F] == 16)
F17 = next(F for F in Fs if F > F16 and cams[F] == 17)
F12 = next(F for F in Fs if F > F17 and cams[F] == 12)
arm = None
for z in open(os.path.join(d, "debug.log"), encoding="latin-1"):
    m = re.search(r"\[g5auftritt\] F(\d+) .*Armierung", z)
    if m: arm = int(m.group(1))
n = len(v)
# Kandidaten: grosse Differenz + Farbwechsel orange->grau (Kante 1) bzw. grau->orange (Kante 3)
k1 = [i for i in range(3, n - 5) if diff[i] > 12 and rb[i - 2] - rb[i + 3] > 8]
k3 = [i for i in range(3, n - 5) if diff[i] > 12 and rb[i + 3] - rb[i - 2] > 8]
best = None
for i1 in k1:
    for i3 in k3:
        if i3 <= i1 + 20: continue
        b = (i3 - i1) / 20.0 / (F12 - F16)
        if abs(b * 30 - 1) > 0.25: continue
        t2 = (i1 + 1) / 20.0 + b * (F17 - F16)
        j = int(round(t2 * 20)) - 1
        s = diff[max(0, j - 3):j + 4].max()
        if best is None or s > best[0]: best = (s, i1, i3, b)
s, i1, i3, b = best
a = (i1 + 1) / 20.0 - b * F16
j2 = int(round((a + b * F17) * 20)) - 1
j2b = max(range(max(0, j2 - 3), j2 + 4), key=lambda j: diff[j])
out = {"a": a, "b": b, "fps_spiel": 1.0 / b, "F_arm": arm, "F16": F16, "F17": F17, "F12": F12,
       "kante1_s": (i1 + 1) / 20.0, "kante3_s": (i3 + 1) / 20.0, "kante2_vorhergesagt_s": a + b * F17, "kante2_gemessen_s": (j2b + 1) / 20.0,
       "kante2_diff": float(diff[j2b])}
json.dump(out, open(os.path.join(d, "zeit.json"), "w"), indent=1)
print(json.dumps(out, indent=1))
