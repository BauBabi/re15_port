# Abnahme 4: echte exe (state.log, Szenenbild n = F1613 + n) gegen Ablauf-Messschiene (spur) - G5 x/z, Spieler x/z, Arm-Gier je Bild
import re, sys
def w12(v):
    v &= 0xfff
    return v - 4096 if v >= 2048 else v
def lies_spur(p):
    out = {}
    for z in open(p):
        k = z.split("||"); a = k[0].split("|")[0].split()
        m = {int(i): w12(int(y)) for i, t, x, y, zz in re.findall(r"m(\d) (-?\d+) (-?\d+) (-?\d+) (-?\d+)", k[1])}
        out[int(a[0])] = (int(a[2]), int(a[3]), int(a[4]), int(a[5]), m)
    return out
def lies_log(p, f0):
    out = {}
    for z in open(p, errors="replace"):
        mf = re.match(r"F(\d+) .*?PL\((-?\d+),(-?\d+),", z)
        if not mf: continue
        n = int(mf.group(1)) - f0
        akt = re.findall(r"\[(\d+) t=(\w\w) .*?@\((-?\d+),(-?\d+),r(-?\d+)\)\]", z)
        g5 = [(int(x), int(zz)) for s, t, x, zz, r in akt if t == "36"]
        arme = [w12(int(r)) for s, t, x, zz, r in akt if t == "37"]
        out[n] = (int(mf.group(2)), int(mf.group(3)), g5[0] if g5 else None, arme)
    return out
S = lies_spur(sys.argv[1]); L = lies_log(sys.argv[2], 1613)
n_ok = n = 0; erste = []
for b in range(0, 1163):
    if b not in S or b not in L: continue
    plx, plz, ex, ez, m = S[b]; lplx, lplz, lg5, larme = L[b]
    gleich_g5 = lg5 == (ex, ez)
    gleich_arm = sorted(larme) == sorted(m[i] for i in range(4))
    gleich_pl = (plx, plz) == (lplx, lplz)
    n += 1
    if gleich_g5 and gleich_arm: n_ok += 1
    elif len(erste) < 6: erste.append((b, (ex, ez), lg5, [m[i] for i in range(4)], larme))
    if b in (0, 313, 652, 677, 812, 900, 1161): print("Bild %4d G5 spur %s exe %s | Arm-Gier spur %s exe %s | Spieler spur (%d,%d) exe (%d,%d)" % (b, (ex, ez), lg5, [m[i] for i in range(4)], larme, plx, plz, lplx, lplz))
print("Bilder %d, G5-Lage und Arm-Gier gleich %d" % (n, n_ok))
for e in erste: print("  abweichend", e)
