"""Zaehlt aus einem RE15_TRILOG-Protokoll die eingereihten texturierten Dreiecke je Bild.
Das Protokoll hat keine Bildmarken. Die Liste wird je Bild in fester Folge der TIM-Slots gefuellt (render_pc.c:1351 leert sie am Bildende);
ein neues Bild beginnt, sobald ein Slot wiederkehrt, der in diesem Bild schon abgeschlossen war.
  python tri_zaehlen.py <log> <form_slot>   -> JSON-Zeile
"""
import json
import sys
from collections import Counter

log, form_slot = sys.argv[1], int(sys.argv[2])
bilder = []          # je Bild: dict slot -> Zahl
akt, zuletzt = {}, None
with open(log, "r", errors="replace") as f:
    for z in f:
        i = z.find("slot=")
        if i < 0:
            continue
        s = int(z[i + 5:z.index(" ", i)])
        if s != zuletzt and s in akt:
            bilder.append(akt)
            akt = {}
        akt[s] = akt.get(s, 0) + 1
        zuletzt = s
# das letzte Bild kann abgeschnitten sein (exe beendet): nicht mitzaehlen
mit = [b for b in bilder if b.get(form_slot, 0) > 0]
ges = [sum(b.values()) for b in mit]
form = [b[form_slot] for b in mit]
rest = [sum(v for k, v in b.items() if k != form_slot) for b in mit]
muster = Counter(tuple(sorted(b.items())) for b in mit).most_common(3)
print(json.dumps(dict(bilder_gesamt=len(bilder), bilder=len(mit), max_gesamt=max(ges), min_gesamt=min(ges), max_form=max(form), min_form=min(form),
                      max_rest=max(rest), haeufigste=[[list(map(list, m)), n] for m, n in muster])))
