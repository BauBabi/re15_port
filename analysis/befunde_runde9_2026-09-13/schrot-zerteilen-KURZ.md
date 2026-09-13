# Schrotschuss auf Zombies: Fall-Animation oder Zerteilen? (Runde 9, 2026-09-13)

Nutzer: *"Diese Fall Animation mit der Shotgun so bei den Zombies ist doch im originalen
nicht so .... Da ist die Animation wenn sie zerteilt werden .... schaue da noch mal nach
im originalen Resident Evil 2....."*

Vier parallele Recherchen + zwoelf adversariale Gegenpruefungen (2,44 Mio Token,
690 Werkzeugaufrufe). Rohbericht: `schrot-zerteilen-ROHBERICHT.json`.

## Das Wichtigste in drei Saetzen

1. **Die Fall-Animation ist der RICHTIGE Handler.** RE2 schickt "Schrot + Rumpf" auf
   `FUN_801066FC` — auch im Todesfall (Todes-Tabelle @0x8010CD24, Zeile 7 Spalte 1).
   Diese Funktion enthaelt in ihren ~100 Stores keinen einzigen Abtrenn-Store.
2. **Aber sie bleibt dort meistens nicht.** Ueber `+0x231` verzweigt sie in zwei
   Untermaschinen: `+0x231 == 1` -> `FUN_80109610` = **WEGSCHLEUDERN mit
   Fleischbrocken** (@0x80106738-48), `+0x231 == 2` -> `FUN_801092C4` = **der Kopf
   platzt ab** (@0x80106758-BC). Beim toedlichen Treffer faellt der Wurf
   `(rand & 3) != 0` zu **3/4** in das Wegschleudern (@0x801067F4-828).
3. **⛔ DER EIGENTLICHE FUND — die KLAMMER ist das DISTANZBAND, nicht 0.** Der
   Schrotschuss laeuft gar nicht durch den bisher angenommenen Applier
   `FUN_800470C0` (das ist der Projektil-/AoE-Pfad, 17 Aufrufer, alle Bolzen/
   GL-Runden/Sparkshot), sondern durch den Kontakt-Applier `FUN_800410CC`
   (@0x80047EF4). Dort gilt `+0x1D2 = 3*Klammer + Zone` (@0x80041A6C-9C), und die
   Klammer ist der Index der getroffenen **Angriffs-Teilbox** = das Entfernungsband.

## Die Teilboxen der Schrotflinte (selbst gedumpt @0x800A6724)

    Box 0:  Abstand 100    Breite 500     (nah)
    Box 1:  Abstand 4100   Breite 750     (mittel)
    Box 2:  Abstand 8100   Breite 1000    (fern)

Das erste Feld ist bei JEDER Schusswaffe 100/4100/8100 — nur die Breite ist
waffenspezifisch (Magnum 125/150/175, Handgun 375/500/625, Schrot 500/750/1000 =
die Streuung). Drei voneinander unabhaengige, monoton fallende Datenreihen werden
mit derselben Klammer indiziert und belegen die Bedeutung:

    Schaden            200 / 60 / 40      (Custom-Schrot 300 / 80 / 60)
    Part-HP-Abzug        2 /  1 /  0      (der Gore-Zaehler!)
    Reaktionszelle    66FC / 5BC0 / 5438  (Sturz / STAGGER / MAIN)

## Was das fuer den Port heisst

Der Port klemmt die Klammer hart auf 0 (`re15_damage.c:1886` stempelt nur Zone 0/1,
`enemy_ai_re2_zombie.c:6404` sogar konstant 1) und liefert damit auf JEDE Entfernung
die Nahbereichszelle. Auf mittlere Distanz gehoerte Spalte 4 = `0x80105BC0` (STAGGER),
auf grosse Spalte 7 = `0x80105438` (MAIN). Zugleich bekommt der Zerleger auf Distanz
nie seinen Part-HP-Abzug (2 nah / 1 mittel / 0 fern), was erklaert, warum Gliedmassen
im Port anders fallen als im Original.

Auf NAHER Distanz — und so spielt man die Schrotflinte — ist die gewaehlte Zelle
dagegen richtig. Der Unterschied, den der Nutzer sieht, muss deshalb INNERHALB von
`FUN_801066FC` liegen: an den beiden Untermaschinen und daran, wie vollstaendig
`re2z_death_burst` / `re2z_death_magmum` im Port ausgebaut sind.

## Offen (mit Adresse, nicht geraten)

* Die Weltgeometrie der drei Teilboxen ist nicht bis zur Absolut-Entfernung
  aufgeloest. Belegt ist die Reihenfolge (feld0 = 100/4100/8100) und der Aufbau
  (`FUN_80041CE4` baut eine Matrix mit Translation `(feld0, 0, -(feld1+4*feld3))`
  @0x80041D20-5C, verkettet sie mit Entity+0x24 und testet ein Parallelogramm gegen
  die Opferposition/4 @0x80041DD8-E9C). Die Z-Spanne ist nicht symmetrisch um die
  Zielachse — da steckt eine noch nicht verstandene Konvention in `FUN_8008D6C4`.
  Naechster Weg: `FUN_8008D6C4`/`FUN_8008DBA4` per PSYQ-Signatur identifizieren, oder
  dynamisch per PCSX-Watchpoint auf +0x1D2 gegen die gemessene Distanz.
* Der Ausbaugrad der beiden Untermaschinen im Port ist noch nicht Zeile fuer Zeile
  gegen `0x80109610` / `0x801092C4` geprueft.

## Was die Gegenpruefung KORRIGIERT hat

* "Spalte 2 (KOPF) ist nie erreichbar" ist zwar richtig, aber **keine Divergenz**: der
  Zombie traegt auch im Original nie das Kopf-Bit 0x10000000 (INIT
  word0 |= 0x0C000000 @0x80100984-998), das Kopf-Tor @0x800472e4 springt auch dort
  darueber hinweg. Der Kopf-Wegschuss laeuft ueber den HOCH-Zweig der Rumpfzelle.
* Ein Agent bilanzierte die Stores von `FUN_801066FC` und schloss "kein Zerteilen".
  Die Gegenpruefung hat das kassiert: die Reaktion VERLAESST die Funktion (jal in die
  Untermaschinen) — eine Store-Bilanz der Funktion allein traegt die Aussage nicht.
