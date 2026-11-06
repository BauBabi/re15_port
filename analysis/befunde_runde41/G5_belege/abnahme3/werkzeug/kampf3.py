# Abnahme 3: Kampfverlauf aus state.log (erster Abschnitt): Spieler-HP/Lage, G5-HP/Lage ab Szenenende
import re, sys
END = 2775
last = -1; sec = 0; php = None; ghp = None; out = []
for z in open(sys.argv[1], encoding="latin-1"):
    m = re.match(r"F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\)", z)
    if not m: continue
    F = int(m.group(1))
    if F < last: sec += 1
    last = F
    if sec > 1: break
    if sec < 1: continue
    if F < END: continue
    g = re.search(r"\[\d t=36 [^]]*@\((-?\d+),(-?\d+),r\d+\)\] hp=(-?\d+)", z)
    hp = int(m.group(5)); gh = int(g.group(3)) if g else None; gx = int(g.group(1)) if g else None
    if hp != php or (gh is not None and gh != ghp):
        out.append("F%d (+%.1f s) Spieler (%s,%s) HP %d | G5 x %s HP %s" % (F, (F - END) / 30.0, m.group(2), m.group(3), hp, gx, gh))
    php, ghp = hp, gh
print("\n".join(out[:40])); print("letztes Bild", last)
