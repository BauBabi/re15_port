# Abnahme 5 — Auswerter (Definition der Abnahmen 2/3/4 unveraendert uebernommen, eigene Zusatzspalten):
# fest = Gorilla aktiv (Sub nicht 0/1 seit erstem aktiven Bild), Zustand 1, kein Angriff (Sub 5/6/8/15), Leon > 3000 entfernt,
# >= 90 Bilder innerhalb 400 (euklidisch) eines Ankers; fern = Bild mit d >= 6001 (0x1771); NAH fest = fest - fern.
# Zusaetzlich je Slot: alle fest-Strecken (Beginn, Dauer, Gorilla-Lage, d-Bereich, Leon-Lage am Beginn/Ende, Lage des anderen Gorillas).
import re, sys, os, math
G = re.compile(r"\[(\d+) t=27 st=(\d+)/(\d+)/(\d+)/(\d+) [^\]]*?@\((-?\d+),(-?\d+),(-?\d+)\)")
H = re.compile(r"^F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+)")
def lies(fn, raum="11C0"):
    rows = []
    for z in open(fn, errors="replace"):
        m = H.match(z)
        if not m: continue
        if m.group(2) != raum:
            if rows: break
            continue
        g = {int(a.group(1)): (int(a.group(2)), int(a.group(3)), int(a.group(6)), int(a.group(8))) for a in G.finditer(z)}
        rows.append((int(m.group(1)), int(m.group(3)), int(m.group(5)), g))
    return rows
def ausw(fn):
    rows = lies(fn)
    erst = {}
    for f, px, pz, g in rows:
        for s, (st, sub, x, z) in g.items():
            if sub in (5, 6, 8, 15) and s not in erst: erst[s] = f
    res = {}
    for s in (2, 3):
        akt = False; seg = []; segs = []; anker = None
        for f, px, pz, g in rows:
            if s not in g: continue
            st, sub, x, z = g[s]; d = math.hypot(x - px, z - pz)
            o = 5 - s
            ox, oz, ost, osub = (g[o][2], g[o][3], g[o][0], g[o][1]) if o in g else (0, 0, -1, -1)
            if not akt and sub not in (0, 1): akt = True
            ok = akt and st == 1 and sub not in (5, 6, 8, 15) and d > 3000
            rec = (f, d, x, z, px, pz, ox, oz, ost, osub)
            if ok and anker is not None and math.hypot(x - anker[0], z - anker[1]) < 400:
                seg.append(rec); continue
            if len(seg) >= 90: segs.append(seg)
            seg = [rec] if ok else []
            anker = (x, z) if ok else None
        if len(seg) >= 90: segs.append(seg)
        fest = sum(len(q) for q in segs); fern = sum(1 for q in segs for b in q if b[1] >= 6001)
        res[s] = dict(erst=erst.get(s), fest=fest, fern=fern, nah=fest - fern, segs=segs,
                      nangr=len({f for f, px, pz, g in rows if s in g and g[s][1] in (5, 6, 8, 15)}))
    return res, (rows[0][0] if rows else None), (rows[-1][0] if rows else None)
if __name__ == "__main__":
    for d in sys.argv[1:]:
        fn = os.path.join(d, "gy.log") if os.path.isdir(d) else d
        r, f0, f1 = ausw(fn)
        print("%s | F%s..F%s" % (os.path.basename(d.rstrip("/")), f0, f1))
        for s in (2, 3):
            v = r[s]
            print("   S%d Angriff %s (Angriffsbilder %d) fest %d = NAH %d + FERN %d" % (s, "F%d" % v["erst"] if v["erst"] else "nie", v["nangr"], v["fest"], v["nah"], v["fern"]))
            for q in v["segs"]:
                a, b = q[0], q[-1]
                print("      F%d+%d G(%d,%d) d%d..%d Leon (%d,%d)->(%d,%d) anderer S%d st%d/sub%d (%d,%d)->(%d,%d)" % (
                    a[0], len(q), a[2], a[3], min(x[1] for x in q), max(x[1] for x in q), a[4], a[5], b[4], b[5], 5 - s, b[8], b[9], a[6], a[7], b[6], b[7]))
