"""Nachbesserung 3: je Lauf vorher/nachher — Tod-Bild, Spawn-Zahl, Part-8-Zeilen (Lauf-Folge gleich?)."""
import glob, os, re, sys
A, N = sys.argv[1], sys.argv[2]
def info(fn):
    t = open(fn, encoding="utf-8", errors="replace").read()
    m = re.search(r"Tod F(-?\d+), Spawns (\d+)", t)
    return (m.group(1), m.group(2)) if m else None, t.count("part  8 fliegt"), len(re.findall(r"^F\d+ ", t, re.M))
n_diff = 0
for fn in sorted(glob.glob(os.path.join(N, "*.txt"))):
    a = info(os.path.join(A, os.path.basename(fn))); n = info(fn)
    if a != n:
        n_diff += 1
        print(os.path.basename(fn), "vorher", a, "nachher", n)
print("Laeufe mit Unterschied:", n_diff)
