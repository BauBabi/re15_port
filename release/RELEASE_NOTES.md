# RE1.5 Port — v0.7.14 (Early Preview)

**Für die Krähen brauche ich einen Mitschrieb aus deiner Sitzung — diese Fassung schreibt ihn.**

---

## Was du tun musst

Start den Port mit gesetzter Umgebungsvariable:

```
RE15_RE2_TRACE=1
```

Dann liegt neben der `re15_pc.exe` eine **`re2_ki.log`** — genau wie `befund.log`. Spiel wie
sonst, bis die Krähen die Macke zeigen (hängen bleiben, oder nach dem ersten Hacken nur
noch an dir kleben). Schick mir die Datei; darin steht jeder Zustandswechsel jeder Krähe
mit Position, Abstand, Clip und Flags.

**Warum das nötig ist:** die Diagnose von vorletzter Woche nennt als ersten Messschritt
genau diesen Trace. Er lief die ganze Zeit — nur ins Leere. Die ausgelieferte exe ist eine
GUI-Anwendung, und die hat **kein stderr**. Gemessen: `2>datei` liefert 0 Bytes. Alle acht
Trace-Stellen schreiben jetzt in die Datei, zeilenweise, damit auch ein Absturz nichts
verliert.

---

## Was ich in der Nacht gemessen habe

Ich habe die erste Sonde gebaut, die **ROOM10C0 wirklich lädt** — die beiden vorhandenen
Tests laufen ohne Raum, womit Wand, Sichtlinie und Kontakt gar nicht geprüft waren, also
genau das, worin die Diagnose die Ursachen vermutet.

Sechs Läufe zu je 3600 Ticks, der Spieler jeweils neben einer der drei Krähen, einmal mit
und einmal ohne wirkende Treffer:

```
Weg je Krähe            151 590 .. 163 690 Einheiten     kein Hänger
längste Stille          2 .. 3 Fenster (600..900 Ticks)
Bildwechsel             1 220 .. 1 533                   Animation läuft
Griff ohne Opfer-FSM    0 .. 1 von hunderten Ticks       der Griff rastet ein
Schuss auf eine Krähe im Anflug (Sub 13):  hp 10 -> -5, Zustand 3 = Tod
```

**Keins deiner beiden Symptome reproduziert sich so.** Der Defekt braucht eine Bedingung,
die mein Prüfstand nicht hat — ein *bewegter* Spieler, ein anderer Raum oder eine längere
Sitzung. Deshalb der Mitschrieb: deine Sitzung hat die Bedingung, meine nicht.

## Ein Fehlschluss, den ich fast geliefert hätte

Der erste Sondenlauf zeigte **101 von 101 Grab-Ticks mit ausgeschalteter Opfer-FSM** — exakt
das Bild, das die Diagnose als Ursache C1 beschreibt („der Griff rastet nie ein"). Das sah
aus wie der gesuchte Fehler.

Es war einer meiner Sonde: ohne geladene Gegnerbank meldet der Griff „keine Victim-Bank"
und rastet nie ein. Gegengemessen — Typ 0x21 aus `CDEMD0.EMS` **hat** eine Opfer-Bank. Mit
geladener Bank verschwand der Befund vollständig. Ich sage das dazu, weil ich es dir sonst
als Fund verkauft hätte.

---

Die Masken aus v0.7.13 sind unverändert enthalten. 282/282 Tests.
