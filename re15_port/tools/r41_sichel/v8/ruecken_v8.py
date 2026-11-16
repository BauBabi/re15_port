"""SF V8 Stufe r) Ruecken (Messblatt V8 Abschnitt 6b): Ecken von Mesh 8 verschieben (ruecken_v8.json, aus ruecken_suche.py).

Nutzer 2026-11-08 (Skizze Darstellung.png): "Tail directly smooth into the back - no hill or anything...", "Little red hill from the
back", "smooth straight to head without further hills....". Frueher: "Auch der obere Ruecken geht im original screenshot direkt mehr
oder weniger in den Kopf ueber. wir haben davor wieder so einen Hueckelhoven."
Jede Verschiebung ist PORT-WAHL aus der Suche (Ziel = Oberkontur der Vorlage-Figurmaske n1439 x 372-445 in PV, Messblatt 3.2);
Flaechen, Normalen, UV, Texel bleiben. Kein RE-Wert.
"""


def ruecken(ms, par, prot):
    m = ms[8]
    liste = []
    for k, d in sorted(par['ecken'].items(), key=lambda kv: int(kv[0])):
        i = int(k)
        alt = list(m['v'][i][:3])
        m['v'][i][:3] = [int(alt[j] + d[j]) for j in range(3)]
        liste.append(dict(ecke=i, alt=alt, neu=list(m['v'][i][:3])))
    prot['ruecken'] = dict(ecken=liste, quelle=par.get('quelle'))
