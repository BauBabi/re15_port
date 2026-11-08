# Abnahme G2-2: eigene Eingabe "Wand- und Ecken-Tour" (anders als walk.py der Abnahme 1 / des Baus):
# lange Laeufe bis an die Wand (Wand als Trichter), dort lange stehen, an der Wand entlang in die Ecke, dort lange stehen,
# Zappeln/Rueckwaertsschritte, kurze Ausfluege ins offene Wasser. Drehrate 96/Bild im Stand (gemessen state.log 0x0020 -> +96).
import random, sys
seed = int(sys.argv[1]); total = int(sys.argv[2]) if len(sys.argv) > 2 else 8150
pre = sys.argv[3] if len(sys.argv) > 3 else "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0"
r = random.Random(seed)
toks = pre.split(",")
def n_of(t):
    i = 0
    while i < len(t) and t[i].isalpha(): i += 1
    return round(float(t[i:]) * 30)
ticks = sum(n_of(t) for t in toks)
def dreh(grad):  # Stand-Drehung, 96/Bild
    return ("R" if grad > 0 else "L") + "%.3f" % (round(abs(grad) / 360 * 4096 / 96) / 30)
while ticks < total:
    p = r.random(); seq = []
    if p < 0.30:   # an die Wand und stehen
        seq = ["XU%.3f" % r.uniform(6.0, 9.0), "W%.3f" % r.uniform(6.0, 14.0)]
    elif p < 0.55: # Wand entlang in die Ecke und stehen
        seq = [dreh(r.choice((90, -90))), "XU%.3f" % r.uniform(6.0, 9.0), "W%.3f" % r.uniform(8.0, 18.0)]
    elif p < 0.70: # zurueck ins Wasser
        seq = [dreh(r.choice((180, 135, -135))), "XU%.3f" % r.uniform(1.5, 3.5), "W%.3f" % r.uniform(2.0, 5.0)]
    elif p < 0.85: # Zappeln an Ort und Stelle
        for _ in range(r.randint(2, 5)):
            seq += [r.choice(("U0.400", "D0.500", "L0.200", "R0.200")), "W%.3f" % r.uniform(0.8, 2.5)]
    else:          # gehen (nicht rennen) im Bogen
        seq = ["U%s%.3f" % (r.choice("LR"), r.uniform(1.0, 3.0)), "W%.3f" % r.uniform(1.0, 3.0)]
    for t in seq:
        n = n_of(t)
        if ticks + n > total: n = total - ticks
        if n <= 0: break
        k = 0
        while t[k].isalpha(): k += 1
        toks.append("%s%.3f" % (t[:k], n / 30)); ticks += n
print(",".join(toks))
