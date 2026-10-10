"""Nachbesserung 3 Spur C: Kriecher-Kopfabriss-Emitter (@0x80102B24 / @0x80102BBC) in den abn2_blut-Laeufen zaehlen —
Welt-Versatz (ohne a3) wie Abnahme 2 klassen.py ODER a3-Vektor (0,-600,0)/(0,100,0); alle uebrigen Spawns als Summe.
Aufruf: python -I klassen3.py <ordner>"""
import re, glob, collections, os, sys
d0 = sys.argv[1]
PAT = re.compile(r"F(\d+)\s+(Z )?\s*(TOD |TREF|    )\s*st (\S+)->(\S+) hp (\S+)->(\S+) rag (\d) bank (\d) \| part\s+(\d+) "
                 r"(fliegt|      ) fx=\((-?\d+),(-?\d+),(-?\d+)\) a3=(ohne )?\((-?\d+),(-?\d+),(-?\d+)\) \| "
                 r"d\(gezeichnet T\)=(-?\d+) \| d\(T-1\)=(-?\d+)")
cat = collections.defaultdict(list); laeufe = collections.defaultdict(set); n_laeufe = 0; tod = 0
for fn in sorted(glob.glob(os.path.join(d0, "*.txt"))):
    n_laeufe += 1
    for l in open(fn, encoding="utf-8", errors="replace").read().splitlines():
        if l.startswith("ERGEBNIS") and "Tod F-1" not in l: tod += 1
        m = PAT.match(l)
        if not m: continue
        d = int(m.group(19)); ohne = bool(m.group(15)); part = int(m.group(10))
        a3 = (int(m.group(16)), int(m.group(17)), int(m.group(18)))
        if part == 0 and ((ohne and d == 600) or (not ohne and a3 == (0, -600, 0))):
            k = "K1 Kopfabriss Part 0 @0x80102B24 " + ("Welt-Versatz" if ohne else "a3 (0,-600,0)")
        elif part == 8 and ((ohne and d == 100) or (not ohne and a3 == (0, 100, 0))):
            k = "K2 Hals-Stumpf Part 8 @0x80102BBC " + ("Welt-Versatz" if ohne else "a3 (0,100,0)")
        else:
            k = "K9 alle uebrigen Spawns"
        cat[k].append((d, os.path.basename(fn))); laeufe[k].add(os.path.basename(fn))
print("Laeufe %d (davon mit Tod %d)" % (n_laeufe, tod))
for k in sorted(cat):
    ds = [x[0] for x in cat[k]]
    print("%s: Spawns %d in %d Laeufen, d(gezeichnet T) min %d max %d" % (k, len(ds), len(laeufe[k]), min(ds), max(ds)))
