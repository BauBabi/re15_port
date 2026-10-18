# Zeile {o, s} je Achse aus Wand-Paaren: ALLE ganzzahligen (s,o) mit Rest 0 an allen Paaren, gewaehlt wird das s,
# das dem exakten Massstab (2^20 * px-Spanne / Welt-Spanne) am naechsten liegt. Formel FUN_800473f8 wie zeile_fit.py.
import sys, json
from proj import t
def f(w, s, o, achse, flip):
    if achse == 'x': return (-t(w, s) if flip else t(w, s)) + o
    return (t(w, s) if flip else -t(w, s)) + o
d = json.loads(sys.argv[1])
for achse in ('x', 'z'):
    paare = d[achse]; flip = d['flip_' + achse]
    (w0, p0), (w1, p1) = paare[0], paare[-1]
    exakt = (2**20) * abs(p1 - p0) / abs(w1 - w0)
    gut = []
    for s in range(int(exakt) - 40, int(exakt) + 41):
        for o in range(-400, 600):
            if all(f(w, s, o, achse, flip) == p for w, p in paare): gut.append((s, o))
    gut.sort(key=lambda so: abs(so[0] - exakt))
    print(f"{achse}: exakt s {exakt:.1f}; gueltig s {min(g[0] for g in gut)}..{max(g[0] for g in gut)} -> gewaehlt s {gut[0][0]} o {gut[0][1]}" if gut else f"{achse}: keine Loesung")
