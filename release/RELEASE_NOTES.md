# RE1.5 Port — v0.7.15 (Early Preview)

**Die Schreibmaschine verdeckt wieder — und dein Hinweis auf RE2 Retail war goldrichtig.**

---

## 1. Die Regression, die du gemeldet hast

Du: *„das lief ja mal und sah auch mal absolut super aus … das ist also eine Regression."*
Stimmt, und es war meine. Ich habe sie in der Versionsgeschichte gefunden: das Objekt
*„Vordergrund Teppich und Schreibmaschine"* stand auf `spalten` und wurde von mir am
07.09. auf `flach` umgestellt — weil der **Teppich** sonst die Entfernung seiner Vorderkante
für die ganze Fläche trug.

Der Teppich wurde damit richtig. Die Maschine falsch: mit `flach` bekommt ihre Silhouette
Bodenentfernungen 64…121, du stehst bei 71 — **dieselbe Maschine liegt halb vor und halb
hinter dir** und zerreißt.

Der Fehler war nicht das Modell, sondern dass **zwei Gegenstände in einer Freistellung
steckten.** Jetzt sind es zwei: Teppich flach, Maschine aufrecht.

```
an deiner Marke F1134          v0.7.13        jetzt
verdeckende Punkte                 376         3271
zu ferne Punkte                   2947           56
```

Gemessen an der **gebauten** Maske und nur über die *gezeichneten* Punkte — der
Maskenatlas ist RGBA mit transparentem Index 0, über ganze Rechtecke zu messen würde grobe
Kacheln überschätzen. Über alle 215 deiner Standorte in diesem Blickwinkel: Median 9,1 % →
10,9 % verdeckt. Die 16 Standorte mit ≥ 80 % liegen **alle** im Umkreis von 400 Einheiten um
den gemessenen Maschinenstandpunkt — genau dort, wo du Verdeckung verlangt hast.

Deine `07_01.png` habe ich nicht angefasst. Die Trennung ist am Hintergrundbild **gemessen**
(Farbabstand zum Teppichgrün), nicht gezeichnet.

## 2. Was RE2 Retail wirklich macht

Dein Hinweis war der wichtigste Beitrag der Woche. Ich habe 250 RE2-RDTs geparst und
**4253 Maskengruppen** ausgewertet:

```
Kacheln je Gruppe (= ein Gegenstand)     Median 14
VERSCHIEDENE Tiefen je Gruppe            Median  5   — nur 31 % haben genau eine
Tiefenspanne innerhalb einer Gruppe      Median 32
Kachelkanten                             Vielfache von 8 (8 … 112)
Anteil frei geformter Rechtecke          66 %
```

Retail vergibt also **weder eine Tiefe je Gegenstand** (mein `fuss`) **noch eine je
Bildpunkt** (mein `flach`), sondern ein paar gestufte Tiefen **je Objekt** — und jedes Objekt
ist eine eigene Gruppe. Genau die Mitte, die meinen beiden Modellen fehlte, und genau der
Grund, warum ROOM10E0 nicht funktionieren konnte.

**Der nächste Hebel, den das freilegt:** Retail benutzt zu 66 % frei geformte Rechtecke, die
der Objektform folgen. Der Port legt ein **gleichmäßiges Gitter** darüber. Ein Gitter ragt an
jeder schrägen Kante über die Silhouette hinaus und verbraucht Atlasfläche, die es nicht
braucht — ROOM10E0 steht deshalb bei 104 von 105 Rechtecken und 64482 von 65536
Atlaspunkten am Anschlag, und die Maschine bekommt eine 24er-Kachel, obwohl sie nur 80×76
Punkte groß ist. Ich habe versuchsweise auf „Kachel passend zur Objektgröße" umgestellt: die
Maschine bekommt 16, aber 12873 statt 303 Punkte der gewollten Fläche fehlen. Der richtige
Weg ist die Rechteckzerlegung statt des Gitters — das ist der nächste Umbau.

**Nebenbefund:** RE1.5 und RE2 kodieren die Maskensätze *verschieden*. Die Regel des Ports
liest RE1.5 zu 280 von 280 Blickwinkeln richtig und RE2 nur zu 46 von 808; RE2 braucht
„Größenfeld = 0 → Breite/Höhe folgen, sonst Quadrat der Feldgröße" (808 von 808). Kein
Fehler im Port — aber wichtig für jeden, der RE2-Daten liest.

## 3. Für die Krähen: die F9-Marke schreibt jetzt die Gegner mit

Du hast beide Macken mit F9 dokumentiert — aber die Marke enthielt nur dich und die Masken.
Aus deinen Bildern ist das Symptom sichtbar (die Krähe klebt an den Jalousien; die Krähe
hängt reglos an Leons Kopf), der **Zustand** aber nicht. Ab jetzt steht in jeder Marke:

```
Gegner im Raum:
   3  typ=0x21 st=1 ss=13/0 clip=5 bild=3 hp=10 dist=612 pos=(...) spd=0
      fl22a=0006 grid=00 anim=0004
Opfer-FSM des Spielers: state=1 typ=0x21
```

Damit sehe ich beim nächsten Mal sofort, ob die Krähe im Anspruch feststeckt (Ursache B des
Dossiers) oder ob der Griff einseitig zerfällt (Ursache C1) — ohne Umgebungsvariable.

⚠️ **Ehrlich dazu:** in meiner Sitzung startet die exe nicht (Exit 1, kein Fenster — mein
Arbeitsplatz hat gerade keinen Desktop-Zugriff). Der Gegner-Block ist übersetzt und im
richtigen Pfad, aber ich habe ihn **nicht im Log gesehen**. Schau bitte nach dem ersten F9,
ob die Zeile „Gegner im Raum:" auftaucht.

282/282 Tests.
