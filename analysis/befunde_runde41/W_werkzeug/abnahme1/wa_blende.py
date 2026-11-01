"""wa_blende.py - Abnahme W1: trennt in den Laeufen die Blenden (Bilder, in denen die Blitz-Flecken zusammen >= 150 Bildpunkte
bedecken = Ab-/Aufblende, Szenenschnitt) von oertlichen Blitzen. Liest runs/<lauf>/andere_blitze.txt."""
import re, glob, os, collections, sys
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
muster = sys.argv[1] if len(sys.argv) > 1 else "B_fix_*_20B0_c*"
for d in sorted(glob.glob(os.path.join(SP, "runs", muster))):
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
    bl = sorted(blend)
    spans = []
    for f in bl:
        if spans and f - spans[-1][1] <= 3: spans[-1][1] = f
        else: spans.append([f, f])
    print("%-24s sonstige %4d | Blendbilder %s | oertlich %d %s" % (os.path.basename(d), len(ev), ",".join("%d-%d" % tuple(x) for x in spans) or "-", len(rest),
          " ".join("[B%d-%d x%d-%d y%d-%d px%d]" % (e[0], e[1], e[3], e[4], e[5], e[6], e[8]) for e in rest[:5])))
