# RE1.5 Port — v0.7.11 (Early Preview)

**Die Holzbank in ROOM10C0 verdeckt jetzt genau dort, wo sie soll — und nur dort.**
Drei deiner F9-Marken, drei Anforderungen, alle erfüllt.

```
"am Ende der Bank"   520 verdeckende Punkte   (soll verdecken)
"daneben"              0                      (soll nicht)
"geht durch"           0                      (soll nicht)
```

---

## Der Weg dahin

**Zuerst zu fern.** Die Silhouettenregel nimmt je Bildspalte den untersten sichtbaren
Punkt als Bodenkontakt. Am rechten Bankende ist das die **Seitenkante**, nicht der Fuß —
der Sehstrahl landet weit dahinter (Spalte 88 → Tiefe 187 statt real ~150). Deshalb
`"tiefe": "kollision"`: die Bank steht in den Kollisionsdaten und hat dort eine echte
Entfernung.

**Dann zu nah**, aus zwei Gründen in der Aufbereitung:

*Der Sicherheitsabschlag.* Der Faktor 0,90 ist am Fehler der **Silhouetten**-Ableitung
gemessen (Medianfehler +2 von rund 80). Die Kollisionstiefe schneidet den Sehstrahl exakt
mit der Zellwand und braucht ihn nicht.

```
Faktor            0.90   1.00   1.05   1.10
am Ende            295    295    295    236
daneben            120      0      0      0
geht durch         101      0      0      0
```

*Die Kachelstatistik.* Jedes 12×12-Feld bekam den Median. Für die streuende Silhouette
richtig; für die exakte Kollisionstiefe soll ein Feld nur verdecken, wenn es **ganz** vor
dir liegt — sonst zieht ein halb auf dem Objekt liegendes Feld mit.

```
Statistik        am Ende   daneben   geht durch
Median               648        72           48
Maximum              648         0            0
```

Beides hängt jetzt je Objekt an seiner Tiefenquelle.

## Was ich verworfen habe, obwohl du es wolltest

Die Objektgeometrie aus der Freistellung abzuleiten statt aus der Kollisionszelle („mach
3"). Gemessen: die Bodenpunkte der Bank-Freistellung liegen bei z≈−7100, alle drei deiner
Positionen bei z −4782..−5782 — also davor. Damit hätte die Bank an **keiner** der drei
Marken verdeckt, auch nicht am Bankende. Die grobe Kollisionszelle ist trotz ihrer
Ausdehnung die bessere Quelle; was fehlte, war nur die richtige Aufbereitung.

---

**282/282 Tests**, lokal und im Linux-Container. ROOM1130 unverändert (22,3 %),
ROOM10C0 Cut 3 bei 14,7 % — unter der Warnschwelle.
