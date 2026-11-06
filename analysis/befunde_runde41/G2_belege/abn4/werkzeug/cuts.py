"""cuts.py <laufordner> [schwelle=0.08]: Schnittspitzen im Video (ffmpeg scene-Score) und die cam=-Wechsel des LETZTEN F-Abschnitts im state.log (= Raum 2090/2091).
Der Versatz t_video = F/30 + off wird am Raum-Laden festgemacht: off = letzter Videoschnitt vor dem ersten cam-Wechsel-Kandidaten, Pruefung an den weiteren cam-Wechseln."""
import sys, subprocess, re
run = sys.argv[1]; thr = float(sys.argv[2]) if len(sys.argv) > 2 else 0.08
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"
p = subprocess.run([FF, "-hide_banner", "-i", run + "/video.mp4", "-vf", "select='gt(scene,%g)',showinfo" % thr, "-f", "null", "-"], capture_output=True, text=True)
schnitte = [float(m.group(1)) for m in re.finditer(r"pts_time:([0-9.]+)", p.stderr)]
dauer = re.search(r"Duration: (\d+):(\d+):([0-9.]+)", p.stderr); dauer = int(dauer.group(1)) * 3600 + int(dauer.group(2)) * 60 + float(dauer.group(3)) if dauer else 0
segs = []; cur = None; prev = None
for l in open(run + "/state.log", errors="replace"):
    m = re.match(r"F(\d+) .*cam=(\d+)", l)
    if not m: continue
    F, c = int(m.group(1)), int(m.group(2))
    if F <= 1 and (cur is None or F < cur["last"]): cur = {"cams": [], "last": F, "first_cam": c}; segs.append(cur); prev = None
    if prev is not None and c != prev: cur["cams"].append((F, prev, c))
    prev = c; cur["last"] = F
s = segs[-1]
print("Video %.1f s; Schnitte (s): %s" % (dauer, ", ".join("%.2f" % t for t in schnitte)))
print("letzter Abschnitt: F bis %d, erste cam %d, cam-Wechsel: %s" % (s["last"], s["first_cam"], ", ".join("F%d %d->%d" % c for c in s["cams"])))
# Kandidaten: jeder Schnitt als t(F0); bewerten mit den cam-Wechseln (ausser F<=60) -> Treffer in +-0.15 s
best = None
for t0 in schnitte:
    tr = 0; n = 0
    for (F, a, b) in s["cams"]:
        if F <= 60: continue
        n += 1; tt = t0 + F / 30.0
        if tt < dauer and any(abs(tt - x) < 0.15 for x in schnitte): tr += 1
    if best is None or (tr, -t0) > (best[0], -best[1]): best = (tr, t0, n)
if best: print("Versatz-Kandidat: t_video = F/30 + %.2f s (%d von %d cam-Wechseln > F60 im Video wiedergefunden)" % (best[1], best[0], best[2]))
