# Live-Beleg: die Wegwerf-Abfrage kommt erst NACH dem Zugriffscode

Nutzer-Befund: *"das Problem das ich Bei dir sah, war das du die discard Abfrage auch bei
Toren mit Rätsel panels machst, also wo man einen Code eingeben muss. Da ist das natürlich
erst dann korrekt, wenn man den zugriffscode den man braucht einmalig richtig eingegeben
hat. erst dann darf die Abfrage kommen."*

Der statische Beleg (Bytes, Generator, Riegel TEIL R) stand seit dem 22.09. Was fehlte, war
der **Laufzeit-Nachweis** — die Vorgabe nannte ihn unter „Offen" als Punkt 2
(*0 Laufzeit-Nachweise*). Der ist hiermit erbracht, im echten Renderpfad.

## 1. Die Bytes, an ROOM10D0.RDT nachgemessen

`re15_port/shared_assets/PSX/STAGE1/ROOM10D0.RDT`, Datei-Byte-Offsets:

```
sub01 (laeuft jedes Bild) — die Erfolgs-Schranke des Tastenfelds
  @0x01512  21 03 32 00   Ck(3,50,0)    Schloss noch zu
  @0x01516  21 05 0d 01   Ck(5,13,1)    Ziffer 1 richtig
  @0x0151A  21 05 0e 01   Ck(5,14,1)    Ziffer 2 richtig
  @0x0151E  21 05 0f 01   Ck(5,15,1)    Ziffer 3 richtig
  @0x01522  21 05 10 01   Ck(5,16,1)    Ziffer 4 richtig
  @0x01526  04 ff 18 13   Evt_exec sub19
  @0x0152A  22 03 32 01   Set(3,50,1)   ERFOLG  <- das Gate-Bit

sub19 (Erfolgs-Sub) — der NEUE Ausloeser der Wegwerf-Abfrage
  @0x0199E  2b 05 ff ff   Message_on 5  "You've opened the lock."

sub20 (Kartenleser) — die ALTE, zu fruehe Stelle; jetzt KEINE Wegwerf-Stelle mehr
  @0x019C4  2b 09 ff ff   Message_on 9  "You've used the Blue Keycard."
  @0x019CA  04 ff 18 11   Evt_exec sub17   <- HIER erst geht das Tastenfeld auf
```

Die uebrigen Panel-Raeume, ebenfalls an den Bytes nachgemessen:

```
ROOM10D1  @0x01514  22 03 32 01   Set(3,50,1)    @0x01988  2b 05 ff ff
ROOM1230  @0x00FF2  22 03 89 01   Set(3,137,1)   @0x01466  2b 05 ff ff
ROOM1231  @0x00FF2  22 03 89 01   Set(3,137,1)   @0x01466  2b 05 ff ff
ROOM1240  @0x0055A  22 03 8b 01   Set(3,139,1)   <- deshalb sind 11E0/11E1 gestrichen
```

## 2. Warum der erste Abzugsversuch nichts zeigte

Lauf vom 22.09. (`bild/discard.log`, 2079 Zeilen): die vier Ziffern-Bits wurden mit
`RE15_SET_FLAG` beim **Spielstart** gesetzt — also VOR dem Sprung nach 10D0. Bank 5 ist die
**raum-lokale** Bank und wird beim Raum-Aufbau geloescht, byte-true `FUN_8003ecec`
@0x8003ed74 `sw zero,0x800b1028` (Port: `scd_room_setup.c:276`). Ergebnis: 1959 Bilder in
ROOM10D0, `msg_aktiv=0`, kein einziges `frage=8`. Nicht das Gate war schuld, der Messhaken war
es.

Neu deshalb zwei reine Messhaken in `platform/pc/main.c`, beide env-gegated:

* `RE15_SET_FLAG_AT="<bank>:<bit>[,...]@<bild>"` — setzt Flags NACH dem Raum-Aufbau.
* `RE15_PRESS="<taste>@<bild>[,...]"` — legt an genau diesen Bildern eine Druck-Flanke an.
  Gebraucht, weil die Nachricht den Freeze haelt, bis der Spieler bestaetigt, und die
  Klick-Schleife des Autopiloten an einem Fahrziel haengt (das den Spieler bewegen wuerde).

## 3. Der Lauf (echter Renderpfad)

`abzug_nach_code.sh build/r25_live 85 190 520 5 200`
— `RE15_FRAMEDUMP` liest das komplett komponierte Bild in `re15_render_end_frame()`
unmittelbar vor `SDL_RenderPresent`; kein `RE15_AUTOSHOT`, kein Softwarerenderer.

Aus `build/r25_live/discard.log` (je Bild eine Zeile, auch wenn nichts steht):

```
F201  msg_aktiv=1  abfrage=1  frage=0  gegenstand=0x00   <- Gate passiert, Abfrage vorgemerkt
F251  msg_aktiv=0  abfrage=1  frage=8  gegenstand=0x38  text=0/44  gefragt=1
F429  msg_aktiv=0  abfrage=1  frage=8  gegenstand=0x38  text=44/44
F430  msg_aktiv=0  abfrage=0  frage=0  gefragt=1  weg=1              <- JA -> weggeworfen
```

0x38 = Blue Keycard, 44 Glyphen = „You don't need this key any more. Discard it?".
Bild: `build/r25_live/abfrage_nach_code.png` (Abzug F420).

## 4. Gegenprobe im selben Lauf-Aufbau

Alles identisch, nur **drei statt vier** Ziffern-Bits gesetzt (falscher Code):

```
2341 Zeilen, frage=8: 0, abfrage=1: 0, msg_aktiv=1: 0
```

Ohne richtigen Code zuendet sub01 nicht, `Set(3,50,1)` faellt nicht, sub19 laeuft nicht,
`Message_on 5` kommt nicht — und damit auch keine Abfrage. Kein einziges Bild.

## 5. Was der Lauf NICHT zeigt, und wo es stattdessen steht

* **Das Gate isoliert.** Der Lauf misst die ganze Kette. Ein Bit Unterschied bei sonst
  gleichem Unterprogramm misst das Riegel-Paar **R1b/R2** (`r21_discard_wegwerfen.c`):
  gleicher Erfolgs-Sub, gleiche Karte, nur `Set(3,50,1)` fehlt -> 0 gegen 1 Abfrage.
* **Die Dreh-Eingabe selbst** (Member[15] + Quadrat am Tastenfeld) ist nicht nachgestellt.
  Sie ist fuer diese Frage ohne Belang: die Abfrage haengt hinter der Schranke, nicht an
  der Tastatur.
* **Die alte, zu fruehe Stelle** (`msg 9` am Kartenleser) ist keine Gate-Frage mehr,
  sondern steht schlicht nicht mehr in der Stellen-Tabelle
  (`gen/discard_sites.inc`: `{ 0x10D0, 5, 0x38, 3, 50 }`, kein Eintrag mit `msg 9`).
* **Die stderr-Quittung der Messhaken** fehlt in allen Protokollen: die SDL-GUI-exe hat kein
  brauchbares stderr (deshalb schreibt die Messchiene ueberhaupt in eine Datei). Belegt sind
  die Haken durch ihre Wirkung im Protokoll, nicht durch ihre eigene Zeile.
