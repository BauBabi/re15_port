"""Runde 38 Spur E: Bild-Waechter fuer RE15_FRAMEDUMP-Serien (Vorlage Runde 30 r30_elza_watch.py).

frame_count springt bei JEDEM Raumwechsel auf 0 -> eine Serie schreibt in jedem Raum-Abschnitt dieselben
Dateinamen. Der Waechter zieht jede FERTIGE Datei sofort auf k<laufnr>_f<bild>.ppm um (Laufnummer =
Ankunftsreihenfolge). Fertig = Groesse >= Kopf + Breite*Hoehe*3 aus dem PPM-Kopf (RE15_WINDOW_SCALE=1 liefert
320x240, die Runde-30-Vorlage pruefte fest 960x720 und zog solche Bilder nie um).

Aufruf:  python e_waechter.py <ordner> <sekunden>
"""
import os, sys, time, glob

def fertig(p):
    try:
        with open(p, "rb") as f:
            kopf = f.read(64)
        t = kopf.split(None, 4)
        if len(t) < 4 or t[0] != b"P6":
            return False
        w, h = int(t[1]), int(t[2])
        return os.path.getsize(p) >= w * h * 3 + 15
    except (OSError, ValueError):
        return False

def main():
    d, secs = sys.argv[1], float(sys.argv[2])
    t_end = time.time() + secs
    n = 0
    while time.time() < t_end:
        for p in sorted(glob.glob(os.path.join(d, "f_*.ppm"))):
            if not fertig(p):
                continue
            q = os.path.join(d, "k%04d_f%s" % (n, os.path.basename(p)[2:]))
            try:
                os.replace(p, q)
                n += 1
            except OSError:
                pass
        time.sleep(0.004)
    print("[waechter] %d Bilder umgezogen" % n)

main()
