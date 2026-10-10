"""Nachbesserung 3: je Lauf den Kopfabriss-Spawn (Part 0) vorher (abn2C/blut, Welt-Versatz) und nachher (blut_neu, a3) im
SELBEN Bild gegenueberstellen. Aufruf: python -I vergleich_k1.py <vorher-ordner> <nachher-ordner>"""
import re, glob, os, sys
P = re.compile(r"F(\d+)\s+(?:Z )?\s*(?:TOD |TREF|    )\s*st \S+->\S+ hp \S+->\S+ rag \d bank \d \| part\s+(\d+) (?:fliegt|      ) "
               r"fx=\((-?\d+),(-?\d+),(-?\d+)\) a3=(ohne )?\((-?\d+),(-?\d+),(-?\d+)\) \| d\(gezeichnet T\)=(-?\d+) \| d\(T-1\)=(-?\d+)")
def lese(fn):
    r = {}
    for l in open(fn, encoding="utf-8", errors="replace"):
        m = P.match(l)
        if m and int(m.group(2)) == 0:
            r.setdefault(int(m.group(1)), []).append((m.group(6) is not None, (int(m.group(7)), int(m.group(8)), int(m.group(9))),
                                                     (int(m.group(3)), int(m.group(4)), int(m.group(5))), int(m.group(10)), int(m.group(11))))
    return r
for fn in sorted(glob.glob(os.path.join(sys.argv[2], "*.txt"))):
    neu = lese(fn); alt = lese(os.path.join(sys.argv[1], os.path.basename(fn)))
    typ = ""
    for l in open(fn, encoding="utf-8", errors="replace"):
        if l.startswith("abn2_blut:"): typ = re.search(r"typ (0x\w+)", l).group(1); break
    for f, lst in sorted(neu.items()):
        for ohne, a3, fx, d, d1 in lst:
            if not ohne and a3 == (0, -600, 0):
                av = [x for x in alt.get(f, []) if x[0]]
                s = " | vorher " + ", ".join("fx=%s d=%d" % (x[2], x[3]) for x in av) if av else " | vorher: kein Part-0-Spawn ohne a3 in F%d" % f
                print("%-22s typ %s F%-4d nachher fx=%s d=%d d(T-1)=%d%s" % (os.path.basename(fn), typ, f, fx, d, d1, s))
