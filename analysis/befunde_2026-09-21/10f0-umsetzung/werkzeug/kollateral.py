"""Hat der Neubau NUR C4/C5 veraendert? — Sektion und TIM jedes Cuts bitweise vergleichen.

⛔ WARUM: raum.py baut beim Aufruf ohne Cut-Nummer den GANZEN Raum neu. Wenn dabei auch
   C0..C3/C6 andere Bytes bekaemen, waere das ein unbemerkter Kollateralschaden - genau
   die Klasse Fehler, die in diesem Projekt schon zweimal aufgeschlagen ist.
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import raum

ALT = "re15_port/shared_assets/PSX/MASKS"
NEU = sys.argv[1] if len(sys.argv) > 1 else "build/r22/neu"


def main():
    a = raum._container_lesen(os.path.join(ALT, "ROOM10F0.MSK"))
    b = raum._container_lesen(os.path.join(NEU, "ROOM10F0.MSK"))
    print("Cut | Sektion alt/neu (Bytes)  gleich | TIM alt/neu (Bytes)  gleich")
    for cut in sorted(set(a) | set(b)):
        sa, sb = a.get(cut), b.get(cut)
        ta = os.path.join(ALT, "ROOM10F0_PRI%02d.TIM" % cut)
        tb = os.path.join(NEU, "ROOM10F0_PRI%02d.TIM" % cut)
        da = open(ta, "rb").read() if os.path.exists(ta) else None
        db = open(tb, "rb").read() if os.path.exists(tb) else None
        print(" %2d | %6s %6s  %5s | %6s %6s  %5s"
              % (cut, len(sa) if sa else "-", len(sb) if sb else "-", sa == sb,
                 len(da) if da else "-", len(db) if db else "-", da == db))


if __name__ == "__main__":
    main()
