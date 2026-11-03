# NB5: echte exe (Messzeile [g5lampe] F.. Zustand) gegen RE2-RAM an den Savestate-Bildern (Szenenbild f = F - 1613), Leon und Elza.
import re, sys
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
RAM = [(3, 31, -9000, -19000), (4, 80, -18000, -8000), (5, 129, -7000, -17000), (6, 165, -3000, -13000), (7, 215, -13000, -3000),
       (8, 264, -2000, -12000), (9, 313, -11000, -1000), (10, 363, -1000, -11000), (11, 412, -10000, -20000), (12, 441, -32000, -32000)]
rx = re.compile(r"\[g5lampe\] F(\d+) Zustand Vorlauf (-?\d+) Bild (-?\d+): Obj 3 (\w+) x (-?\d+), Obj 4 (\w+) x (-?\d+), Cut (\d+), Beben (-?\d+)")
for lauf in ("fd_leon", "fd_elza"):
    z = {}
    for line in open(L + "/" + lauf + "/debug.log", errors="replace"):
        m = rx.search(line)
        if m: z[int(m.group(1))] = m.groups()
    ok = 0
    print("== %s: %d Zustandszeilen" % (lauf, len(z)))
    for s, f, x3, x4 in RAM:
        g = z.get(1613 + f)
        if not g: print("  s%02d Bild %d: keine Zeile" % (s, f)); continue
        p3, p4 = int(g[4]), int(g[6])
        gl = (p3 == x3 and p4 == x4); ok += gl
        print("  s%02d Bild %4d F%d: Port Obj 3 %s %6d Obj 4 %s %6d | RE2 %6d %6d %s" % (s, f, 1613 + f, g[3], p3, g[5], p4, x3, x4, "gleich" if gl else "ABWEICHUNG"))
    print("  %d von %d Savestates bitgleich" % (ok, len(RAM)))
    be = [(int(g[1]), int(g[8])) for F, g in sorted(z.items()) if int(g[1]) >= 0 and int(g[8]) != 0]
    print("  Beben (Vorlaufbild, Wert):", be)
    erst = min((F for F, g in z.items() if g[3] == "an"), default=None)
    print("  erstes Bild mit Obj 3 an: F%s (Vorlauf %s)" % (erst, z[erst][1] if erst else "-"))
