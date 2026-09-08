# RE1.5 Port — v0.7.26 (Early Preview)

**Drei Marker gefixt — und deine letzten 2F-Freistellungen sind drin.**

---

## Marke 1 (ROOM10E0): das Überblenden oberhalb des Blatts

Zwei Quellen, beide beseitigt:
1. Im **Lehn-Fenster** über der Maschine steckten Nicht-Maschinen-Punkte in der
   Pult-Freistellung (graue Kante, grüner Teppich) — die blitteten auf deinen Oberkörper,
   wenn du dich über die Maschine lehnst. 1076 Punkte in einer **Kopie** entfernt; dein
   Original bleibt unangetastet, Maschine/Papier/Tasten decken unverändert.
2. Die **Trennwand rechts** hatte eine Maske ohne jeden möglichen Verdeckungsfall —
   hinter ihrer Zelle kann niemand stehen (Sperrbereich, auch für Gegner). Ihre
   Randkacheln blendeten deinen Arm. Entfernt.

Sichtprobe an deiner Marke: Hand und Arm frei, Beine an der Sperrlinie weiter verschluckt.

## Marke 2 (ROOM10D0): die Pflanze

Ihr Standpunkt kam aus der bildrand-geklippten untersten Zeile → Tiefe 53…62, *vor* dir.
Der Topf steht auf der **Kreiszelle** x7600..8600 z25200..26200 (RDT-Kollision); ihre
Mitte projiziert auf Bild(33,170), Kamera-z 7006. Mit der gemessenen Topfzeile liegt das
Profil bei **100…117 — hinter dir**, wie es sein muss.

## Marke 3 (ROOM1100): Zombies um die Ecke — und deine Frage

**Ja, allgemein lösbar.** Das Original liefert für diesen Raum weder Masken noch ein
Regions-Viereck — deshalb war nichts da, was die Gegner im Querflur verdeckt. Aber die
Eckwände sind **Kollisionszellen**, und der Quader macht daraus Maske samt Tiefe ohne
jede Freistellung. Gemessen: der Zombie hinter der Wand (Tiefe 195) komplett gedeckt,
der in der Ecköffnung bleibt zu Recht sichtbar, dein Spielerkasten 0 Bisse. Dieselbe
Technik trägt für jeden Winkel, dessen Wand eine Zelle ist — F9 genügt.

## Deine letzten 2F-Freistellungen

Alle 15 eingemessen (jede mit **100 % Übereinstimmung** platziert, Standard-Tiefenregel):

```
ROOM10F0: 11 Objekte auf den Winkeln 0-6
ROOM1110:  4 Objekte auf den Winkeln 0, 2, 6
```

Wenn eine davon eine andere Tiefenregel braucht, zeigt es dein nächster F9-Durchlauf.

Tests: **282/282** (im Release-Container).
