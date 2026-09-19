# ROOM10D0 C1: die EINE Nutzer-Freistellung 01.png in zwei Tiefen-Objekte teilen.
import json, collections, io, os

P = "analysis/esp_masken_2026-09-03/auswahl.json"
d = json.load(open(P, encoding="utf-8"), object_pairs_hook=collections.OrderedDict)
e = d["ROOM10D0"]["1"]
alt = e["objekte"][0]
GRUND = (
    "Phase 3 (2026-09-19, Nutzer-Auftrag 'bei den offenen pri findings sei kreativ'): "
    "die EINE Nutzer-Freistellung 01.png (54x61, 1021 Punkte, Lage x=124 y=87, 100,0 %) "
    "traegt jetzt ZWEI Tiefen-Objekte. Die Silhouette bleibt bitgenau die des Nutzers: "
    "Platte 476 + Gestell 545 = 1021 Punkte. "
    "GEMESSEN (build/p3/mess_10d0c1c.py, mess_10d0c1f.py): als EIN Objekt liest die "
    "Spaltenregel je Spalte die unterste Silhouettenzeile als Bodenkontakt; der "
    "Weltpunkt springt zwischen den Spalten 146->147 um 6958 und 175->176 um 9855 "
    "Einheiten, waehrend der groesste Schritt INNERHALB von 147..175 bei 1044 liegt. "
    "Die Spalten 126..146 lesen die eigene Kante der Platte (Tiefe 225..234) und die "
    "Spalte 176 (2 Punkte, oberste Ecke) laeuft ueber den Horizont hinaus auf Welt "
    "z=-13512 und Tiefe 319 — das ist der Grund, warum die F9-Marke F423 des Nutzers "
    "fiel (sie verlangt wirksam < 180 in x156..176)."
)
e["_phase3"] = GRUND
e["objekte"] = [
    collections.OrderedDict([
        ("name", "Klapptisch hinten — Platte (Nutzer-Original 01.png)"),
        ("png", "pri/STAGE1/10D0/01.png"),
        ("x", 124), ("y", 87), ("massstab", 1), ("oben", 0), ("grow", 0),
        ("teil", collections.OrderedDict([("kanal", 1), ("gegen", [0, 2]),
                                          ("abstand", 10), ("weiten", 1), ("nimm", "ja")])),
        ("aufrecht", True),
        ("grund", "Die gruene Tischplatte, am Hintergrundbild getrennt (g > r+10 und "
                  "g > b+10, um 1 Punkt geweitet fuer den Antialias-Saum): 476 Punkte, "
                  "x129..174. Sie ist WAAGERECHT — ihre unterste Silhouettenzeile ist "
                  "nie der Boden. Gemessen gibt die Spaltenregel ihr deshalb eine "
                  "INVERTIERTE Rampe: fernes linkes Ende 234, nahes rechtes Ende 319. "
                  "'aufrecht' nimmt EINEN Standpunkt am tiefsten Silhouettenpunkt und "
                  "das Zeilenprofil darueber — gemessen 219..224 statt einer nachweislich "
                  "verkehrten Rampe. Entscheidung, nicht Messwert: die Platte traegt EINE "
                  "Tiefenstufe; nur ihre Richtung ist gemessen."),
    ]),
    collections.OrderedDict([
        ("name", "Klapptisch hinten — Gestell (Nutzer-Original 01.png)"),
        ("png", "pri/STAGE1/10D0/01.png"),
        ("x", 124), ("y", 87), ("massstab", 1), ("oben", 0), ("grow", 0),
        ("teil", collections.OrderedDict([("kanal", 1), ("gegen", [0, 2]),
                                          ("abstand", 10), ("weiten", 1), ("nimm", "nein")])),
        ("bodenkante", [147, 175]),
        ("grund", "Das Metallgestell, 545 Punkte, x126..176. Die Fusslinie ist GEMESSEN: "
                  "in den Spalten 147..175 liegt der groesste Schritt des Bodenpunkts "
                  "zwischen Nachbarspalten bei 1044 Welteinheiten, bei 146->147 sind es "
                  "6958 und bei 175->176 sind es 9855 (6,7-fach) — dort ist die "
                  "unterste Silhouettenzeile nicht der Boden, sondern die eigene Kante "
                  "bzw. die oberste Ecke. Diese Spalten erben den naechsten eigenen "
                  "Standpunkt (176 -> 175 mit Tiefe 172, wirksam unter der von der "
                  "Marke F423 verlangten Grenze 180)."),
    ]),
]
json.dump(d, open(P, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
print("ROOM10D0 C1 auf zwei Objekte geteilt")
