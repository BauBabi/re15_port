# RE1.5 Port — v0.7.27 (Early Preview)

**Dein Pult-Fix gilt, und die Bürostühle im Communication Room decken.**

---

## Schreibmaschine: dein `07_01_pult_gefixed.png` ist die Quelle

Was ich anders gemacht hatte: meine `bereinigt`-Datei war dein Original **minus 35
Punkte** (x211..259/y150..182) — exakt das halbrunde Loch oberhalb der Tischplatte, das
du gesehen hast; entfernt wegen der minimalen Lehn-Blende an F3245. Deine Abwägung ist
übernommen: das Loch stört mehr. (Dein gefixed ist übrigens nicht 100 % identisch mit
dem Original: 14291 statt 14303 Deckpunkte — 12 Randpixel.) Die Lehn-Blende kann an der
einen Stelle minimal wiederkehren.

## Communication Room (ROOM10F0): die Bürostühle

Deine fünf Marken zeigten alle dasselbe: die Stühle deckten nicht, weil meine
Kollisions-Umstellung ihnen ferne Zellen zugewiesen hatte. Die Stühle stehen auf
**eigenen Kreiszellen** (11 im RDT) — jeder sichtbare Stuhl ist jetzt ein eigener
Quader (Winkel 4: acht, Winkel 5: vier; Höhe −700 aus dem einzigen sauberen
Binnen-Gipfel des Kantenmaßes). Fünf-Marken-Probe: der Stuhl zwischen Kamera und dir
deckt seinen Überlapp **vollständig** (238/238, 245/245, 573/573, 257/257, 161/161).

## Werkzeug: zwei Bremsen raus

Der 10F0-Bau hing über eine Stunde. Ursachen gemessen und behoben:
1. Der Kollisions-Raycast lief als reine Python-Doppelschleife je Kandidatenzelle —
   jetzt vektorisiert, **bit-identische** Gegenprobe, ~20× schneller.
2. Die Kachel-Kombinationssuche war `10^Objekte` (11 Objekte = 10¹¹) — jetzt
   Gier-Verfeinerung mit denselben Grenzen. Der ganze Raum baut in Sekunden.

## Karte 1F (dein Treppenhaus-Befund): diagnostiziert, kommt als eigene Runde

Die Ursache ist gefunden und dokumentiert: **ROOM1000 — der lange Flur — saß auf der
Treppenhaus-Kachel** (Rect 10, Bildschirm 119,134), und die Vorgabe für ROOM1060 wurde
im Generator von einem späteren Block überfahren. Der Fix selbst ist gemessen richtig,
löste aber eine Kaskade in den historisch verdrahteten 1F-Zuordnungen aus (Eichungen
wanderten, zwei Kartentests wurden rot). Statt das neben dem Release zu verbiegen,
kommt die Karte als fokussierte eigene Runde — die komplette Schreibpfad-Diagnose
liegt bereit.

Tests: **282/282** (im Release-Container).
