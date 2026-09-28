"""Runde 30, Thema G: Bild-Waechter fuer RE15_FRAMEDUMP-Serien.

WARUM: main.c setzt g_engine.frame_count bei JEDEM Raumwechsel auf 0 zurueck
(Block "Frame-Cap des Handoffs relativ zum Eintritt"). Eine Serie "0-10/1" schreibt
deshalb in jedem Raum-Abschnitt dieselben Dateinamen f_000000.ppm..f_000010.ppm und
ueberschreibt den vorigen Abschnitt. Der Waechter zieht jede fertige Datei sofort auf
einen eindeutigen Namen  k<laufnr>_f<bild>.ppm  um; die Laufnummer ist die
Ankunftsreihenfolge, also die Reihenfolge im Lauf.

Aufruf:  python r30_elza_watch.py <ordner> <sekunden>
"""
import os, sys, time, glob

def main():
    d, secs = sys.argv[1], float(sys.argv[2])
    t_end = time.time() + secs
    n = 0
    while time.time() < t_end:
        for p in sorted(glob.glob(os.path.join(d, "f_*.ppm"))):
            base = os.path.basename(p)[2:]
            q = os.path.join(d, "k%04d_f%s" % (n, base))
            try:
                # Groesse muss vollstaendig sein (960x720x3 + Kopf); sonst schreibt das Spiel noch
                if os.path.getsize(p) < 960 * 720 * 3:
                    continue
                os.replace(p, q)
                n += 1
            except OSError:
                pass            # noch offen -> naechste Runde
        time.sleep(0.004)
    print("[watch] %d Bilder umgezogen" % n)

main()
