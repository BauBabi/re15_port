"""aussen5.py <laufordner> [...]: Figur AUSSERHALB der Raum-Aussenwaende von ROOM2090 (SCA x -8900..7200, z -27000..-5400; Dateikopf
enemy_ai_boss_gator.c "SCA-Aussenwaende x[-8900..7200] z[-27000..-5400]"), Ostnische (x > 7200 bei z -18100..-14500) getrennt gezaehlt.
Je Lauf (letzter Besuch): Bilder aussen, erstes/letztes Bild, Lage, Treffer davor (hp-Flanke), ob die Figur zurueckkommt, Treffer waehrend aussen."""
import re, sys, os


def besuch(run):
    sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
    cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")] if os.path.exists(run + "/climb.log") else []
    start = 0
    prev = None
    for i, l in enumerate(sl):
        f = int(re.match(r"F(\d+)", l).group(1))
        if prev is not None and f < prev:
            start = i
        prev = f
    out = []
    for i in range(start, len(sl)):
        m = re.match(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\).*? mo=(\d+)", sl[i])
        if not m:
            continue
        y = None
        if i < len(cl):
            c = re.search(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i])
            if c:
                y = int(c.group(2))
        g = re.search(r"@\((-?\d+),(-?\d+),r(\d+)\)", sl[i])
        out.append(dict(F=int(m.group(1)), x=int(m.group(2)), z=int(m.group(3)), hp=int(m.group(5)), mo=int(m.group(6)), y=y,
                        gx=int(g.group(1)) if g else None, gz=int(g.group(2)) if g else None))
    return out


def lage(s):
    x, z = s["x"], s["z"]
    if x > 7200 and -18100 <= z <= -14500:
        return "nische"
    if x < -8900 or x > 7200 or z < -27000 or z > -5400:
        return "aussen"
    return "innen"


for run in sys.argv[1:]:
    st = besuch(run)
    name = run.replace("\\", "/").rstrip("/").split("/")[-1]
    au = [i for i, s in enumerate(st) if lage(s) == "aussen"]
    ni = [i for i, s in enumerate(st) if lage(s) == "nische"]
    if not au and not ni:
        print("%-34s %5d Bilder: Figur nie ausserhalb der Aussenwaende, nie in der Ostnische" % (name, len(st)))
        continue
    msg = "%-34s %5d Bilder: aussen %d Bilder" % (name, len(st), len(au))
    if au:
        a = au[0]
        # Treffer unmittelbar davor
        tr = [i for i in range(max(0, a - 40), a + 1) if st[i]["hp"] < 100 and (i == 0 or st[i - 1]["hp"] >= 100)]
        zur = next((i for i in range(au[-1] + 1, len(st)) if lage(st[i]) == "innen"), None)
        tr_aus = sum(1 for i in au if st[i]["hp"] < 100 and st[i - 1]["hp"] >= 100)
        s0 = st[a - 1] if a > 0 else st[a]
        msg += " (F%d..F%d), vorher F%d bei (%d,%s,%d), Treffer davor %s, aussen bis (%d,%s,%d), Gator dabei (%s,%s); %s; Treffer waehrend aussen %d" % (
            st[a]["F"], st[au[-1]]["F"], s0["F"], s0["x"], s0["y"], s0["z"], ",".join("F%d" % st[i]["F"] for i in tr) or "-",
            st[au[-1]]["x"], st[au[-1]]["y"], st[au[-1]]["z"], st[au[-1]]["gx"], st[au[-1]]["gz"],
            ("zurueck innen ab F%d" % st[zur]["F"]) if zur is not None else ("bis Laufende F%d aussen" % st[-1]["F"] if au[-1] == len(st) - 1 else "kein Innen-Bild danach"), tr_aus)
    if ni:
        msg += "; Ostnische %d Bilder (F%d..F%d)" % (len(ni), st[ni[0]]["F"], st[ni[-1]]["F"])
    print(msg)
