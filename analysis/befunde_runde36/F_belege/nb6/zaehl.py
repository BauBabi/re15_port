# Offline-Zaehler je Regel aus einer kanon-Spur (spur 1): Zeilen "F<f> (+<r>) S<s> st=a/b/c/d @(x,z) d<d> ... | Leon (lx,lz)"
import re, sys
P = re.compile(r"F(\d+) \(\+(-?\d+)\) S(\d+) st=(\d+)/(\d+)/(\d+)/(\d+) @\((-?\d+),(-?\d+)\) d(\d+) k(\d) L(\d) nah(\d+) \| Leon \((-?\d+),(-?\d+)\)")
def lies(fn):
    rows = {}
    for z in open(fn):
        m = P.search(z)
        if not m: continue
        f, r, s, st, s1, s2, s3, x, zz, d, k, L, nah, lx, lz = map(int, m.groups())
        rows.setdefault(s, []).append((f, r, st, s1, x, zz, d, nah, lx, lz))
    return rows
def sim(rows, regel, anker=400):
    out = []; c = 0; ax = az = 0; dref = 0; lxr = lzr = 0
    for (f, r, st, s1, x, z, d, nah, lx, lz) in rows:
        nahjagd = (st == 1 and s1 == 3 and 3000 <= d < 6001)
        if regel == "R0":
            reset = (not nahjagd) or abs(x - ax) + abs(z - az) >= anker
        elif regel == "Rb":
            reset = (not nahjagd) or d <= dref - anker
        elif regel == "Rbc":
            reset = (not nahjagd) or d <= dref - anker or abs(lx - lxr) + abs(lz - lzr) >= anker
        if reset: c = 0; ax, az, dref, lxr, lzr = x, z, d, lx, lz
        else: c += 1
        out.append((f, r, c, nah))
    return out
if __name__ == "__main__":
    fn = sys.argv[1]; slot = int(sys.argv[2]); bis = int(sys.argv[3])
    rows = lies(fn)[slot]
    for regel in ("R0", "Rb", "Rbc"):
        o = sim(rows, regel)
        vor = [q for q in o if q[0] < bis]
        mx = max(vor, key=lambda q: q[2])
        chk = all(q[2] == q[3] for q in o) if regel == "R0" else None
        print("%s: max vor F%d = %d (bei F%d +%d)%s" % (regel, bis, mx[2], mx[0], mx[1], "  [R0 == Port-Zaehler: %s]" % chk if chk is not None else ""))
