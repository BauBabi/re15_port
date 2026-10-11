# nahz.py <gy.log> [raum]: NAH-Anker-Zaehler wie zombieki_r36.c (Sub 3, 3000 <= d < 6001, Anker 400 Manhattan) offline aus gy.log;
# je Slot: Maximum vor dem ersten Angriff, Maximum gesamt, Spitzen > 200 mit Bild
import re, sys, math
G = re.compile(r"\[(\d+) t=27 st=(\d+)/(\d+)/(\d+)/(\d+) [^\]]*?@\((-?\d+),(-?\d+),(-?\d+)\)")
H = re.compile(r"^F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+)")
fn = sys.argv[1]; raum = sys.argv[2] if len(sys.argv) > 2 else "11C0"
st = {}
for z in open(fn, errors="replace"):
    m = H.match(z)
    if not m or m.group(2) != raum: continue
    f = int(m.group(1)); px, pz = int(m.group(3)), int(m.group(5))
    for g in G.finditer(z):
        s = int(g.group(1)); sub = int(g.group(3)); x, zz = int(g.group(6)), int(g.group(8)); state = int(g.group(2))
        d = math.hypot(x - px, zz - pz)
        q = st.setdefault(s, dict(n=0, ax=x, az=zz, erst=None, maxv=0, maxg=0, fmax=0, spitzen=[]))
        if q["erst"] is None and sub in (5, 6, 8, 15): q["erst"] = f
        if state != 1 or sub != 3 or d >= 6001 or d < 3000 or abs(x - q["ax"]) + abs(zz - q["az"]) >= 400:
            if q["n"] > 200: q["spitzen"].append((f - q["n"], q["n"]))
            q["ax"], q["az"], q["n"] = x, zz, 0
        else:
            q["n"] += 1
        if q["n"] > q["maxg"]: q["maxg"], q["fmax"] = q["n"], f
        if q["erst"] is None and q["n"] > q["maxv"]: q["maxv"] = q["n"]
for s, q in sorted(st.items()):
    if q["n"] > 200: q["spitzen"].append((None, q["n"]))
    print("S%d erster Angriff F%s, NAH-Anker max vor dem Angriff %d, gesamt %d (F%d), Strecken > 200: %s" % (s, q["erst"], q["maxv"], q["maxg"], q["fmax"], q["spitzen"]))
