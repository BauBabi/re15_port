"""wa_wiederkehr.py - Abnahme W1: ortsfeste Wiederkehr. Oertliche Blitze (ohne Figur, ohne Blendbilder) eines Laufs werden nach
Mittelpunkt (Abstand <= 6 Bildpunkte) gebuendelt; ein Ort mit >= 3 getrennten Blitzen gilt als ORTSFESTES BLINKEN.
Aufruf: wa_wiederkehr.py <glob-muster>"""
import re, glob, os, collections, sys
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
for d in sorted(glob.glob(os.path.join(SP, "runs", sys.argv[1]))):
    p = os.path.join(d, "andere_blitze.txt")
    if not os.path.exists(p): continue
    ev = []
    for t in open(p).read().splitlines():
        m = re.search(r"Bild (\d+)\.\.(\d+) \((\d+) Bilder\) x(\d+)\.\.(\d+) y(\d+)\.\.(\d+) max\+(\d+) px(\d+)", t)
        if m: ev.append(tuple(map(int, m.groups())))
    s = collections.Counter()
    for e in ev:
        for f in range(e[0], e[1] + 1): s[f] += e[8]
    blend = {f for f, v in s.items() if v >= 150}
    rest = [e for e in ev if not any(f in blend for f in range(e[0], e[1] + 1))]
    orte = []
    for e in rest:
        cx, cy = (e[3] + e[4]) / 2, (e[5] + e[6]) / 2
        for o in orte:
            if abs(o["x"] - cx) <= 6 and abs(o["y"] - cy) <= 6: o["n"] += 1; o["dauer"].append(e[2]); break
        else: orte.append({"x": cx, "y": cy, "n": 1, "dauer": [e[2]]})
    fest = [o for o in orte if o["n"] >= 3]
    print("%-36s oertlich %4d | ortsfest (>=3) %d %s" % (os.path.basename(d), len(rest), len(fest),
          " ".join("(%.0f,%.0f)x%d dauer%s" % (o["x"], o["y"], o["n"], sorted(set(o["dauer"]))[:4]) for o in sorted(fest, key=lambda o: -o["n"])[:4])))
