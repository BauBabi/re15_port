import re, sys
from zaehl import lies
def sim(rows, regel, anker=400, X=200):
    out = []; c = 0; ax = az = 0; da = 0; lxr = lzr = 0
    for (f, r, st, s1, x, z, d, nah, lx, lz) in rows:
        nahjagd = (st == 1 and s1 == 3 and 3000 <= d < 6001)
        moved = abs(x - ax) + abs(z - az) >= anker
        lmoved = abs(lx - lxr) + abs(lz - lzr) >= anker
        if regel == "R0": reset = (not nahjagd) or moved; reanker = reset
        elif regel == "Rp":   # Lage-Anker; Reset nur, wenn die Ankerstrecke Leon um >= X naeher brachte
            reset = (not nahjagd) or (moved and da - d >= X); reanker = reset or moved
        elif regel == "RpL":  # Rp + Leon bewegt >= anker
            reset = (not nahjagd) or (moved and da - d >= X) or lmoved; reanker = reset or moved
        if reset: c = 0
        else: c += 1
        if reanker: ax, az, da = x, z, d
        if reset: lxr, lzr = lx, lz
        out.append((f, r, c, nah))
    return out
if __name__ == "__main__":
    fn = sys.argv[1]; slot = int(sys.argv[2]); bis = int(sys.argv[3])
    rows = lies(fn)[slot]
    for regel in ("R0", "Rp", "RpL"):
        for X in ((0,) if regel == "R0" else (100, 200, 300)):
            o = sim(rows, regel, X=X)
            vor = [q for q in o if q[0] < bis]
            mx = max(vor, key=lambda q: q[2])
            print("%s X=%d: max vor F%d = %d (bei F%d +%d)" % (regel, X, bis, mx[2], mx[0], mx[1]))
