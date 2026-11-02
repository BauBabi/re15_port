#!/usr/bin/env python3
"""Schreibt profil_urteil.json fuer vorlage_neu.py. Zahlen stehen im Profil-Block des Blatts, hier nur das Urteil."""
import json, os
H = os.path.dirname(os.path.abspath(__file__))
kopf = "Unterschiede unter rund 0,1 werte ich nicht. Einzelne Laeufe bleiben in Nebenminima haengen (Tiefe Bild 2 = 7000), die Zahl ist dort Obergrenze."
tab = """| Groesse | Urteil | Spanne | Kennung |
|---|---|---|---|
| Arm (Ober- + Unterarm) | nach unten festgelegt | ab 1900, 1700 deutlich schlechter, oben bis 2300 flach. Abgabe 1910 | [g] |
| Bein | nach unten festgelegt | ab 1600, 1400 deutlich schlechter. Abgabe 1780 | [g] |
| Schwanzlaenge | nach unten festgelegt | ab 3400, 3000 etwas schlechter, bis 4200 flach. Abgabe 3820 | [g] |
| Rumpflaenge | nach unten festgelegt | ab 1800, oben bis 2800 flach. Abgabe 2240 | [g] unten, sonst frei |
| Huefte Tiefe Bild 1 | nach unten festgelegt | ab 5600, 5200 schlechter. Abgabe 5840 | [g] |
| Gier Bild 1 | festgelegt | -5 bis +10 Grad, Abgabe 7 | [g] |
| Gier Bild 2 | nach oben festgelegt | -40 bis 0 gleich gut, 21: +0,7, 45: +1,1. Die alte Vorgabe 21 faellt damit | [g] oben, Abgabe -16 aus Handlinie |
| Huefte Tiefe Bild 2 | eng, unruhig | 6500-8000, Abgabe 7200 | [g] |
| Kopfmitte Bild 2 | festgelegt, solange der Kopf in Zug Q steht | x 290-325, y 230-245 | [v] Zug Q |
| Schulter-Halbabstand | eng | 810-1020 aus der ersten Fassung, nicht neu profiliert, Abgabe 898 | [g] |
| Dicken Arm, Bein, Schwanz | festgelegt, wo sichtbar | Schwanz 310-340 Wurzel, 170-220 Mitte, bis 230 Ende, Unterarm 260-305, Bein 140-245 | [g] |
| Rechte Hand Bild 2 | gestuetzt | Hand am Boden passt zum Unterarm ohne Laengenaenderung | [v] Boden |
| Oberarm | nicht sichtbar | Summe Arm belegt, Teilung nicht | [v] mindestens 0,72 Unterarm |
| Beine, Schwanz Bild 2, linker Arm Bild 1 | nicht sichtbar | | [f] |
| Hals, Kiefer, Kopfform seitlich, Fusslaenge, Fingerzahl | nicht sichtbar | | [f] |"""
json.dump(dict(kopf=kopf, tabelle=tab), open(os.path.join(H, 'profil_urteil.json'), 'w', encoding='utf-8'), ensure_ascii=False, indent=0)
print('ok')
