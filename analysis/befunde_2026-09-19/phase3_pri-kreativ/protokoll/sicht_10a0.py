"""ROOM10A0: der Spieler landet nach dem Sprung auf Band 8 (y=-14400), und Cut 2 hat auf
diesem Band ueberhaupt keinen beruehrenden Standplatz. Auf Band 8 gibt es aber in Cut 0
einen HINTER- und in Cut 1 einen VOR-Standplatz — damit ist fuer den Raum beides gezeigt,
nur nicht im selben Winkel. Das wird so berichtet."""
import os, sys
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.argv = ["x"]
import sicht3

FAELLE = [("ROOM10A0", 1, "vor", 24250, 21350), ("ROOM10A0", 0, "hinter", 24450, 22150)]
for (room, cut, tag, wx, wz) in FAELLE:
    ppm, zeile = sicht3.lauf(room, cut, wx, wz, tag)
    import re
    aktiv = int(re.search(r" C(\d+)", zeile).group(1)) if zeile else -1
    print("%s C%d %s: Welt(%d,%d) -> Bild %s, aktiver Winkel %d"
          % (room, cut, tag, wx, wz, "ja" if ppm else "NEIN", aktiv))
    if ppm:
        ziel = os.path.join(sicht3.OUT, "sicht_%s_C%d_%s.png" % (room, cut, tag))
        Image.open(ppm).save(ziel)
        print("   %s" % ziel)
        print("   %s" % (zeile or "(keine Log-Zeile)"))
