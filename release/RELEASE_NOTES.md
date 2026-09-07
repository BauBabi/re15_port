# RE1.5 Port — v0.7.13 (Early Preview)

**Drei Befunde von dir, zwei gelöst, einer war meiner.**

---

## 1. Die Löcher in der Maske — mein Fehler aus v0.7.12

Du: *"das mit der schreibmaschine ist jetzt irgendwie kaputt"*. Zu Recht.

Die neue Entfernungsrechnung setzte jeden Punkt, an dem der Sehstrahl die Kollisionszelle
**verfehlt**, auf 0 — und 0 heißt „keine Maske". Gemessen an ROOM10E0:

```
Rechtecke im Blickwinkel 7     101  ->  82      es fehlten 19
getroffene Punkte der Liege    1898 von 5712
```

Ein Möbel ist aber undurchsichtig. Ein Strahl, der neben der groben Kollisionsbox
vorbeigeht, ist eine Ungenauigkeit der **Box**, kein Loch im Möbel. Die nicht getroffenen
Punkte erben jetzt die Entfernung des nächsten getroffenen. Wieder 101 Rechtecke.

## 2. Der Tresen in ROOM1120 — vier Marken

Er trug **eine** Zahl für alle 81 Kacheln und deckte dich damit komplett zu, obwohl du
davor stehst. Eine einzige Zahl kann das nicht: am 05.09. standest du bei Entfernung 153
*dahinter* und musstest verdeckt werden.

```
                              vorher        jetzt
Entfernung des Tresens        fest 82       84..95, je Bildzeile wachsend
F613 / F626 / F634 / F642     alles zu      0 verdeckende Punkte
bei Entfernung 153 dahinter   verdeckt      verdeckt weiter (Maximum 95)
```

## 3. Leons Kopf in der Wand — ROOM10E0

Die Rückwand lag per Silhouette bei 112, dein **Kopf** bei 112,6 — sie schnitt ihn um einen
halben Punkt ab. Eine Rückwand kann das nie, du stehst davor.

```
Zeile für Zeile im Körperkasten, F2103:
   y110..144 (Kopf und Rumpf)    927 verdeckende Punkte  ->  0
   F523 zusätzlich               164                     ->  0
```

---

## Noch offen — gemessen, aber nicht gelöst

Ich sage lieber, was ich nicht kann, als etwas zu raten:

**Die Schreibmaschine in ROOM10E0** steckt mit dem Teppich in *einer* Freistellung, Modell
„flach". Der Teppich ist zu Recht flach — die Maschine ist es nicht. Ihre Silhouette bekommt
Bodenentfernungen 61..102, du stehst bei 68 mittendrin, also ist dieselbe Maschine halb vor
und halb hinter dir. Ein Schnitt an der Teppichoberkante findet Maschine, Stuhl und Schrank
sauber, lässt aber die untere Hälfte der Maschine beim Teppich — der Riss wandert nur.
Das braucht eine Trennung am Maschinenfuß.

**Die grünen Tische in ROOM10D0.** Keine Kollisionszelle passt (dort liegen nur lange
Flurwände). „aufrecht" gibt pauschal 97 (481 verdeckende Punkte), „spalten" 106..292
(210 Punkte, am rechten Rand nachweislich zu fern). Beides falsch — ein Tisch ist eine
waagerechte Platte in bekannter Höhe, und die will als solche gerechnet werden.

282/282 Tests.
