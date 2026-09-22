# Nacharbeit zur Wegwerf-Abfrage — die zwei Sperrgruende

Stand: 2026-09-22. Vorarbeit: `analysis/befunde_2026-09-21/discard-umsetzung.md`
(Umsetzung) und `analysis/befunde_2026-09-21/discard-re2-mechanismus.md` (Befund).
Suite nach der Nacharbeit: **336/336**.

---

## Sperrgrund 1 — fuenf geratene Zahlen. Behoben, und die Quelle ist jetzt EINE.

### Der Beleg

RE1.5 hat GENAU EINEN Ja/Nein-Zeichner: Zustand 4 der Nachrichten-FSM, **LAB_80028564**
(Sprungtabellen-Eintrag @0x8001095c). Dass der Aufnahme-Prompt UND die Wegwerf-Abfrage dort
landen, haengt am Oeffner FUN_80027e68:

```
80027ee8  beq   a1,v0,LAB_80027f0c     ; a1 == 0x100
80027eec  ori   v0,zero,0x22           ; (Delay-Slot)  Box-X = 0x22 = 34
80027f10  sh    v0,DAT_800b8534
80027f14  ori   v0,zero,0xb4           ;               Box-Y = 0xb4 = 180
80027f1c  sh    v0,DAT_800b8536
80027f38  lhu   v0,DAT_800c4fc6[a2*2]  ; die Prompt-Tabelle, aus der Skript [6] kommt
```

Beide oeffnen mit `a1 = 0x100` — der Aufnahme-Prompt @0x8001df6c-94, die Wegwerf-Zeile als
Skript [6] der Tabelle @0x800C4FC6. Gleiche Box, gleicher Zeichner, also zwangslaeufig
gleiche Koordinaten. Es gab nie einen Grund fuer eigene Zahlen.

Die Zahlen, jede mit ihrer Instruktion:

```
80028624  lhu   a0,DAT_800b8536     ; 0xb4 = 180
80028630  addiu a0,a0,0x10          ; Zeile = 196          (Cursor)
8002866c  lhu   a0,DAT_800b8536
80028674  addiu a0,a0,0x10          ; Zeile = 196          (Text)
80028680  ori   a0,a0,0xae          ; Yes-Text  = 174

8002862c  lbu   v1,DAT_800b8520
80028638  andi  v1,v1,0x1           ; Wahl (0 = Yes, 1 = No)
8002863c  sll   v0,v1,0x3           ; v1*8
80028640  addu  v0,v0,v1            ; v1*9
80028644  sll   v0,v0,0x2           ; v1*36
80028648  subu  v0,v0,v1            ; v1*35
8002864c  sll   v0,v0,0x1           ; v1*70  = v1*0x46
80028650  addiu v0,v0,0xa0          ; Cursor = 160 / 230
```

Die Optionen stehen im Original als EIN String `"Yes" 01 2e "No" 00` @0x800C4954
(DEBUG.BIN laedt RAW nach 0x800C0000, Datei == RAM; Datei-Offset 0x4954 =
`59 65 73 01 2e 4e 6f 00`), gezogen durch den Drucker FUN_800279c8. Der Port zeichnet die
beiden Woerter einzeln in der Spielschrift und braucht dafuer die Spalte von "No": sie
folgt aus denselben zwei gemessenen Konstanten — Zellenabstand 0x46 (@0x8002864c), Abstand
Cursor→Text 174-160 = 14 (@0x80028680 gegen @0x80028650), also **244 = 174 + 0x46**. Genau
diese Herleitung tragen die beiden bereits ausgelieferten Stellen seit ihrer Abnahme.

Das Blink-Gatter:

```
800285e8  lbu   v1,DAT_800b8525     ; Blink-Zaehler
800285f0  addiu v1,v1,-0x1          ; jedes Bild -1 (0 -> 0xFF)
800285f4  sb    v1,DAT_800b8525
800285f8  sll   v1,v1,0x18
800285fc  sra   v1,v1,0x18
80028600  ori   v0,zero,0x18        ; >>> MASKE 0x18 <<<
80028604  sllv  v0,v0,s1            ; s1 = 0 im Port
80028608  and   v1,v1,v0
8002860c  beq   v1,zero,LAB_8002865c ; -> Cursor NICHT zeichnen
```

Und die Reihenfolge beim Umschalten — erst nullen, dann dekrementieren:

```
800285bc  beq   v0,zero,LAB_800285e0 ; kein Links/Rechts -> ueberspringen
800285d4  sb    zero,DAT_800b8525    ; Zaehler := 0
800285d8  xori  v0,v0,0x1            ; Wahl umschalten
   ... und erst danach @0x800285f0 `-1` -> 0xFF -> 0xFF & 0x18 != 0
```

Der Cursor steht nach einem Tastendruck also SOFORT wieder da. Wer erst dekrementiert und
dann nullt, verliert genau dieses eine Bild — so stand es im Port
(`msg_common.c` Zustand 3) und ist mit korrigiert.

### Was geaendert wurde

Statt einer dritten Zahlenreihe gibt es jetzt **eine** Quelle:
`re15_port/engine/src/msg_select_common.c` + `re15_port/include/re15_msg_select.h`.
Sie liefert `re15_msg_select_layout()` und `re15_msg_select_blink_tick()`. Alle VIER
Zeichen-Stellen holen ihre Zahlen dort:

| Stelle | vorher | jetzt |
|---|---|---|
| `platform/pc/main.c` Skript-Abfrage (`message_select`) | Zahlen inline, richtig | `re15_msg_select_layout` |
| `platform/psx/main.c` Skript-Abfrage | Zahlen inline, richtig | `re15_msg_select_layout` |
| `platform/pc/main.c` **Aufnahme-Prompt** | 190/234/202, Cursor 180/224/203, **kein Blinken** | `re15_msg_select_layout` |
| `platform/pc/main.c` **Wegwerf-Abfrage** | 190/234/202, Cursor 180/224/203, **kein Blinken** | `re15_msg_select_layout` |

Das Urteil hatte recht: der Aufnahme-Prompt trug dieselben geratenen Zahlen. Er ist mit
korrigiert und blinkt jetzt ebenfalls (neuer Zaehler `s_blink` in `item_modal_common.c`,
getickt im Auswahl-Zustand 6).

### Nachgemessen — am BILD, nicht am Quelltext

Live-Abzug aus dem laufenden Spiel (`abzug_discard.sh`, ROOM10D0-Kartenleser, echter
Renderpfad: `RE15_FRAMEDUMP` liest das komplett komponierte Bild INNERHALB von
`re15_render_end_frame()` unmittelbar vor `SDL_RenderPresent` — kein AUTOSHOT, kein
Softwarerenderer). `spalten.py` misst die Auswahlzeile nach:

```
bild000389.ppm  Bloecke in Zeile 196..203 ab Spalte 150:
                161..163, 175..181, 184..188, 192..196, 245..251, 255..258
```

* `161..163` = der Cursor, gezeichnet bei **160**
* `175..181 / 184..188 / 192..196` = "Y e s", Textanfang **174**
* `245..251 / 255..258` = "N o", Textanfang **244**
* alles im Zeilenband **196..203**

Bild: `analysis/befunde_2026-09-22/discard-nacharbeit/bild/bild000389.png`.

---

## Sperrgrund 2 — die Abfrage schob sich in eine laufende Szene.
## ⛔ Und die vermutete Ursache war NICHT die Ursache.

### Was der Auftrag annahm

Der Kandidat war das Fenster `Set(2,7,1)` in ROOM1090 sub03, @0x2508 bis @0x26DA.
Das Fenster gibt es, und die Zelle ist auch richtig bestimmt. Ein Zensus ueber alle 206
ausgelieferten RDTs (`re15_port/tools/cutscene_latch_zensus.py`, 0 Desync) zeigt, warum
gerade diese zwei Zellen der Szenen-Riegel sind:

```
Zone:Bit   Fenster  davon +Plc_ret  +Cut_auto   Set!=0  Set==0   Ck
   2:7        141         103          125        193     153     0
  1:27        106          84          100        141     106     0
  1:28         24           4            4         44      38     0
   5:0         13           0            0        131     169   125
```

`Ck = 0` heisst: kein Skript liest sie je — sie schreiben in die ENGINE. Zone 2 ist naemlich
gar kein Flag-Speicher, sondern das Pause-Wort **DAT_800aca40** (Zeigertabelle
@0x80074664[2], aufgeloest in `op_set` LAB_8003fdd0 @0x8003fdf8-@0x8003fe04). Bit 7 ist
Maske `0x80000000>>7 = 0x01000000`, und die liest die Eingabe-Aufbereitung:

```
800304f4  lw   v0,DAT_800aca40
800304f8  lui  v1,0x100              ; 0x01000000
800304fc  and  v0,v0,v1
80030500  beq  v0,zero,LAB_80030520  ; Bit klar -> Eingabe unveraendert
80030514  andi v0,v0,0xf000          ; Bit gesetzt -> nur noch die 4 Menuebits
8003051c  sw   v0,DAT_800ac768
```

"flag(2,7) steht" heisst woertlich: der Spieler hat keine Kontrolle.

### Was die MESSUNG sagte

Und trotzdem greift ein Riegel auf diesem Fenster nicht. Gemessen am echten
ROOM1090 sub03 im echten VM, Bild fuer Bild (Riegel TEIL F):

```
Bild   2  Message_on 9 @0x2502   "You've used the Fire Extinguisher."
Bild  72  Nachricht ausgeredet   <- HIER ging die Abfrage auf
Bild  73  Set(2,7,1) @0x2508     <- ERST HIER beginnt die Szene
Bild 677  Evt_end    @0x26E4     Ende des Unterprogramms
```

Die Nachricht steht **eine Anweisung VOR** dem Fenster. Im Bild, in dem sie ausgeredet
hatte, war flag(2,7) noch gar nicht gesetzt — die Abfrage erwischte genau das eine Bild
dazwischen. Und weil ein sichtbarer Prompt den SCD-Takt anhaelt
(`platform/pc/main.c`: `re15_discard_frozen()` ueberspringt `scd_vm_tick`), kam die Szene
danach ueberhaupt nicht mehr in Gang: sub03 stand auf @0x2508 fest.

Das Fenster-Modell haette also gruen gemessen und den Fehler dringelassen. Das ist der
Grund, warum hier gemessen und nicht modelliert wurde.

### Der Riegel, der wirkt

Die Abfrage geht erst auf, wenn **das Unterprogramm, das die Nachricht ausgegeben hat,
zu Ende ist**. Der Faden-Index wandert dafuer von `op_message_on` in die Abfrage
(`re15_discard_notice_message(room, msg, thread_slot)`), und `re15_discard_tick` wartet auf
`!g_scd.threads[slot].active`. Gemessen war in jedem der Bilder 60..90 genau Faden 0 aktiv,
und er bleibt es bis zum Evt_end in Bild 677.

Der Fenster-Riegel (`re15_cine_active()` = `flag(1,27) || flag(2,7)`) bleibt als dritte
Schranke stehen — fuer die Faelle, in denen die Nachricht INNERHALB eines Fensters faellt
und ein ANDERER Faden die Szene haelt.

⛔ **Ehrliche Einordnung:** RE1.5 hat fuer diese Abfrage gar keinen eigenen Ausloeser
(Befund vom 2026-09-21) — der Port haengt sie an `op_message_on`. RE2 braucht die Schranke
nicht, weil dort der TUER-Handler ausloest und die Fortsetzung aus der HAUPTSCHLEIFE
gerufen wird (@0x80026384 `jalr v1` auf DAT_800D4498), also nie aus einem laufenden
Ereignis heraus. Diese Schranke stellt fuer RE1.5 genau diese Eigenschaft her. Sie ist
damit eine **PORT-Schranke mit RE2-Vorbild, keine nachgewiesene RE1.5-Konstante** — und
wird auch so im Code bezeichnet.

### Die drei geforderten Faelle, vorher/nachher

Die Annahme des Auftrags, alle drei lägen in ROOM1090 sub03, stimmt nicht — es sind drei
verschiedene Unterprogramme in drei Raeumen. Gemessen mit dem ausgelieferten Unterprogramm:

| Gegenstand | Raum / sub | Nachricht | **vorher auf in Bild** | **jetzt auf in Bild** | Differenz |
|---|---|---|---|---|---|
| Fire Extinguisher 0x31 | ROOM1090 sub03 | msg 9 | **72** (mitten in der Szene) | **677** (nach Evt_end) | +605 |
| Pliers 0x30 | ROOM11E0 sub21 | msg 12 | **80** | **262** | +182 |
| Minidisc Player w/ Disc 0x44 | ROOM1100 sub02 | msg 4 | **84** | **245** | +161 |

In allen drei Faellen laeuft das Unterprogramm jetzt zu Ende, bevor die Abfrage aufgeht.

---

## Uebrige Punkte des Urteils

### Mengen-Dekrement erst beim Fragen — erledigt, und byte-true belegt

Die Reihenfolge in RE2 LAB_80051718 ist eindeutig: die Warte-Schranke kommt VOR dem
Dekrement, und sie verlaesst die Routine, ohne die Anzahl anzufassen.

```
800517dc  lbu   v0,DAT_800e873c
800517f0  andi  v0,v0,0x80
800517f4  bne   v0,zero,LAB_80051870   ; belegt -> RAUS, Anzahl UNBERUEHRT
   ...
80051808  lbu   v0,DAT_800d4a3d        ; Anzahl im Platz
80051810  addiu v0,v0,-0x1             ; ZAEHLER -= 1
8005181c  sb    v0,DAT_800d4a3d
80051824  bne   v0,zero,LAB_8005185c   ; != 0 -> keine Abfrage
8005182c  li    a1,0x100
80051830  li    a2,0x9
80051834  jal   FUN_8002fe38           ; FRAGEN
```

Der Port dekrementierte bisher schon beim Einhaengen (`notice_message`) und zeigte dem
Spieler waehrend der ganzen Wartezeit einen Schluessel mit Anzahl 0. Das Dekrement liegt
jetzt hinter allen drei Schranken, im selben Bild wie die Frage.

### Generator-Regel (C) auf Namens-Vollsuche — erledigt, Tabelle unveraendert

Regel (C) zaehlte die Benutzungsstellen bisher nur unter den Nachrichten, die Regel (A) dem
Gegenstand ZUGESCHLAGEN hatte. (A) ordnet aber nach laengstem Treffer zu: eine Nachricht mit
"Red Master Keycard" geht an den Master, und "Red Keycard" sieht sie nie — obwohl ihr Name
woertlich darin steht. Haette ein Gegenstand zwei Stellen, von denen eine so verschluckt
wird, zaehlte (C) eine und erlaubte faelschlich eine Wegwerf-Abfrage.

(C) sucht den Namen jetzt in ALLEN "used the"-Nachrichten. Die Regel kann dadurch nur
strenger werden. **Die erzeugte Tabelle ist byte-identisch geblieben** (17 Eintraege,
9 Gegenstaende) — die alte Formulierung hat hier nichts verdeckt, aber sie konnte es.

### Was liegen bleibt — benannt, nicht kaschiert

* **Abbrechen mit CROSS = "No"** ist weiterhin Port-Konvention, nicht byte-true. RE2s
  Abfrage kennt nur den Bestaetigen-Knopf (FUN_80030844, `DAT_800ce310 & 0x1000`); welche
  physische Taste dahinter liegt, ist nach wie vor nicht aufgeloest. Der sichere Ausgang
  (Gegenstand behalten) ist gewaehlt.
* **Das Blinken im Live-Bild** ist nicht abgelichtet. Die Aus-Phase ist 8 von je 32 Takten;
  im Abzug stand die Abfrage nach dem Austippen nur 3 Takte, bevor das Eingabe-Skript sie
  bestaetigte, und mehrere Versuche, das Skript so zu kuerzen, dass die Abfrage stehen
  bleibt, haben die Stelle nicht mehr erreicht. Das Gatter ist stattdessen ueber alle 256
  Zaehlerstaende im Riegel gepinnt (TEIL D) — der Live-Beleg fehlt.
* **7 der 17 Benutzungsstellen** kann TEIL F nicht selbst fahren: die Kartenleser
  (ROOM10D0/10D1/1230/1231, ROOM11E0 msg 9) haengen ihre Nachricht hinter
  `Ifel_ck`/`Ck(12,31,0)` — die Ja-Antwort auf die Frage davor (ROOM10D0 sub20 @0x19B8/
  @0x19BC) —, und ROOM4001 msg 2 liegt nicht in einer sub-Region. Ohne echte Spieler-Antwort
  geht der Zweig nach Else. Sie werden MIT GRUND ausgelassen und stehen nicht still auf
  gruen; fuer ROOM10D0 tritt der Live-Abzug an ihre Stelle (Abfrage offen ab Bild F333).
* **Zone 0 (DAT_800aca38) und Zone 1 (DAT_800aca3c)** sind im Port weiterhin nicht an die
  echten Engine-Woerter gebunden (Zone 2 ist es). Kein gemessener Defekt haengt dran; der
  Punkt steht schon im Block bei `re15_game_flag_set`.

---

## Riegel

`re15_port/tests/unit/r21_discard_wegwerfen.c`, gewachsen um:

* **TEIL D** — die fuenf Zahlen + das Blink-Gatter gegen die Original-Werte gepinnt.
  Das Gatter ueber ALLE 256 Zaehlerstaende (192 sichtbar / 64 aus), der Zaehler-Takt
  (`-1` je Bild, beim Umschalten erst 0 dann `-1` = 0xFF).
  **Gegenprobe im Riegel selbst:** die sechs frueheren geratenen Zahlen
  (190/234/202/180/224/203) muessen alle verfehlt werden, und ein Cursor ohne Gatter
  (256/256 sichtbar) faellt durch.
* **TEIL E** — waehrend einer Szene geht die Abfrage nicht auf; je Fall vier Pruefungen
  (Szene zu / danach offen / Gegenprobe ohne Szene / auch ueber flag(1,27)).
* **TEIL F** — das AUSGELIEFERTE Unterprogramm, Bild fuer Bild, mit dem alten
  Oeffnungszeitpunkt als eingebauter Gegenprobe.

**Gegenprobe am alten Stand, getrennt nachgewiesen:** die sechs alten Zahlen plus den
fehlenden Blink durch dieselben Pruefungen geschickt —

```
ROT  Yes-Spalte 190 statt 174 (@0x80028680)
ROT  No-Spalte 234 statt 244 (@0x8002864c)
ROT  Zeile 202 statt 196 (@0x80028674)
ROT  Cursor bei Yes 180 statt 160 (@0x80028650)
ROT  Cursor bei No 224 statt 230
ROT  Cursor-Zeile 203 statt 196 (@0x80028630)
ROT  Blink-Gatter: 256 von 256 sichtbar statt 192 (@0x80028600)
Gegenprobe am ALTEN Stand: 7 Pruefungen ROT
```

Der Riegel hat ausserdem einen eigenen Rechenfehler gefangen: die erste Fassung erwartete
96 von 256 sichtbaren Zaehlerstaenden. Maske 0x18 verdeckt nur, wenn Bit 3 UND Bit 4 klar
sind — 1 von 4 Bitpaaren, also 64. Der Riegel stand rot, bis die Zahl stimmte.

Suite: **336/336**.
