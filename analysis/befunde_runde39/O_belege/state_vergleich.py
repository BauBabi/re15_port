"""Vergleicht zwei RE15_STATE_LOG-Dateien Zeile fuer Zeile im 11C0-Abschnitt bis Bild <bis> (ohne das Ende).
Aufruf: state_vergleich.py <vorher> <nachher> <bis>"""
import sys, re


def abschnitt_11c0(pfad, bis):
    zeilen = open(pfad, encoding="utf-8", errors="replace").read().split("\n")
    # Der 11B0-Vorlauf endet mit dem Tuerdruck; 11C0 beginnt wieder bei F0/F1. Erster Bildnummer-Ruecksprung = Raumwechsel.
    out, letzte, in11c0 = [], -1, False
    for z in zeilen:
        m = re.match(r"^F(\d+) ", z)
        if not m:
            continue
        f = int(m.group(1))
        if not in11c0 and letzte >= 0 and f < letzte:
            in11c0 = True
        letzte = f
        if in11c0:
            if f >= bis:
                break
            out.append(z)
    return out


a = abschnitt_11c0(sys.argv[1], int(sys.argv[3]))
b = abschnitt_11c0(sys.argv[2], int(sys.argv[3]))
n = min(len(a), len(b))
gleich = sum(1 for i in range(n) if a[i] == b[i])
erste = next((i for i in range(n) if a[i] != b[i]), None)
print("11C0-Bilder vorher %d, nachher %d, verglichen %d, gleich %d" % (len(a), len(b), n, gleich))
if erste is not None:
    print("erste Abweichung:\n  vorher  %s\n  nachher %s" % (a[erste][:400], b[erste][:400]))
