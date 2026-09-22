# Wegwerf-Abfrage an Toren mit Code-Panel — was markiert „der Code war richtig"?

Stand: 2026-09-22. Untersuchung B zur Nutzer-Korrektur. **Dieser Lauf hat KEINEN
Spielcode geändert** — nur ein Dossier, zwei neue Messwerkzeuge und eine Messsonde.
Nicht angefasst: `engine/src/item_discard_common.c`, `tests/unit/r21_discard_wegwerfen.c`
(existieren in diesem Arbeitsbaum ohnehin nicht — sie liegen im parallelen Lauf).

Vorarbeit: `analysis/befunde_2026-09-21/discard-umsetzung.md`,
`analysis/befunde_2026-09-21/discard-re2-mechanismus.md`,
`analysis/befunde_2026-09-22/discard-nacharbeit.md`.

---

## 0. Der Nutzer hat recht, und zwar wörtlich

> „Das Problem das ich Bei dir sah, war das du die discard Abfrage auch bei Toren mit
> Rätsel panels machst, also wo man einen Code eingeben muss. Da ist das natürlich erst
> dann korrekt, wenn man den zugriffscode den man braucht einmalig richtig eingegeben
> hat. erst dann darf die Abfrage kommen"

Die Wegwerf-Abfrage hängt im Port an `Message_on 9` = **„You've used the Blue Keycard."**
Genau diese Zeile erscheint im Original **bevor** das Tastenfeld überhaupt aufgeht. Sie
bedeutet nicht „die Karte ist erledigt", sondern **„die Karte steckt jetzt im Schlitz"**.
Der Text der Abfrage-Zeile sagt es selbst (ROOM10D0 msg 7):

> „A card reader. You have to use the Blue Keycard **and insert four digits** to unlock
> the door. Will you operate the card reader?"

Gemessen, nicht behauptet — Bildnummern aus
`tests/unit/probe_r22_codepanel_reihenfolge.c` Teil A (echter Port-VM auf den
ausgelieferten Bytes von `ROOM10D0.RDT`):

```
f0     Nachricht  7 GEHT AUF   ("... Will you operate the card reader?")
f201   Nachricht  9 GEHT AUF   ("You've used the Blue Keycard.")      flag(3,50)=0
f248   Nachricht  0 GEHT AUF   ("Enter the first number.")            flag(3,50)=0
f293   Nachricht  5 GEHT AUF   ("You've opened the lock.")            flag(3,50)=1
f293   flag(3,50) := 1
```

**47 Bilder** liegen zwischen der Wegwerf-Zeile und der ersten Ziffern-Aufforderung. Wer
bei f201 „you don't need this key any more" fragt und ein Ja bekommt, wirft die Karte weg,
während der Code noch aussteht.

---

## 1. Die Maschine des Kartenlesers, Anweisung für Anweisung

Alle Offsets sind **Datei-Byte-Offsets in der jeweiligen `ROOM*.RDT`**
(`re15_port/shared_assets/PSX/STAGE1/…`). Beispiel ROOM10D0; die anderen fünf Leser sind
strukturgleich, Offset-Tabelle in §2.

### 1.1 `main00` — welcher Auslöser installiert wird

```
0x01022  Ifel_ck        06 00 4e 01
0x01026  Ck             21 03 32 00      ; flag(3,50) == 0  -> Schloss noch ZU
0x0102A  Aot_set slot0  sce=1  Nutzlast 06 00 ff ff   ; Nachricht 6 „It's electronically
                                                      ; locked. There's a card reader …"
0x0103E  Aot_set slot1  sce=3  Nutzlast ff 00 18 14   ; DER KARTENLESER -> sub20
                                                      ;   (ev = pc[17] = 0x14 = 20,
                                                      ;    Flags pc[3]=0x31 Bit 0x10 ->
                                                      ;    Aktionstaste, scd_vm.c:2793/2925)
0x01052  Door_aot_set slot2 sce=0                     ; Tür INERT (zu)
   …
0x01170  Else_ck        07 00 26 00      ; flag(3,50) == 1  -> Schloss OFFEN
0x01174  Door_aot_set slot0 sce=2                     ; normale, benutzbare Tür
```

### 1.2 `sub20` — die Karte in den Schlitz

```
0x019AE  Message_on 7   2b 07 ff ff   ; die Ja/Nein-Frage (.msg-Steuercode 0x03)
0x019B4  Ifel_ck
0x019B8  Ck  21 0c 1f 00              ; flag(12,31)==0 = Antwort JA
                                      ;   Bank 12 = DAT_800b8520 (Nachrichten-FSM),
                                      ;   Bit 31 -> Maske 0x80000000>>31 = 0x1
0x019BC  Ifel_ck
0x019C0  Ck  21 09 34 01              ; flag(9,52)==1 = Blue Keycard wurde GENOMMEN
                                      ;   Bank 9 = DAT_800b1078 (Taken-Bits)
0x019C4  Message_on 9   2b 09 ff ff   ; <<< HIER hängt der Port die Wegwerf-Abfrage
0x019C8  Evt_next       02
0x019C9  Nop            00
0x019CA  Evt_exec       04 ff 18 11   ; sub17 = >>> JETZT geht das Tastenfeld auf
0x019CE  Else_ck        07 00 0c 00
0x019D2  Message_on 8                 ; „You have not the Blue Keycard to operate it."
```

Zwischen 0x019C4 und 0x019CA liegt **keine Verzweigung**: `2b 09 ff ff | 02 | 00 |
04 ff 18 11`. Die Sonde prüft diese zehn Bytes als Anker, bevor sie messt.

### 1.3 `sub17` — Tastenfeld auf

```
0x018D6  Cut_chg 0x0b                 ; Kamera auf das Panel
0x018D8  Aot_reset slot1 sce=0        ; den Leser-Auslöser abschalten
0x018E2  Message_on 0                 ; „Enter the first number."
0x018E8  Sleep 2
0x018EC  Set 22 05 00 01              ; flag(5,0)=1  Cursor-Tasten frei
0x018F0  Set 22 05 01 01              ; flag(5,1)=1  Aktionstaste frei
0x018F4  Set 22 05 02 01              ; flag(5,2)=1  Panel offen (sub01 friert den Spieler)
```

Es gibt im ganzen Raum **genau ein** `Evt_exec sub17` (0x019CA) und **genau ein**
`Set(5,2,1)` (0x018F4) — das Tastenfeld ist ausschließlich über die Karte erreichbar.
Dasselbe gilt in ROOM1230 (`sub=17` nur @0x01492, `Set(5,2,1)` nur @0x013BC).

### 1.4 `sub01` — jedes Bild: Tasten lesen und die vier Riegel prüfen

```
0x012E4..0x01342   4x   Ck(5,0,1) + Sce_key_ck(0x01/0x04/0x02/0x08) -> sub02..sub05
                        (Cursor bewegen: Speed_set/Add_speed)
0x01344..0x014FA  11x   Ck(5,1,1) + Work_set(3,1) + Member_cmp(0x0F == k)
                        + Sce_key_ck(0x40) -> sub06..sub16      (Taste k drücken)
0x014FC  Ifel_ck
0x01500  Ck  21 05 02 01           ; Panel offen
0x01504  Set 22 02 00 01           ; PAUSE_PLAYER  (Bank 2 = DAT_800aca40)
0x01508  Set 22 02 02 01           ; PAUSE_AI
0x0150E  Ifel_ck
0x01512  Ck  21 03 32 00           ; flag(3,50)==0   Schloss noch zu
0x01516  Ck  21 05 0d 01           ; flag(5,13)==1   Ziffer 1 richtig
0x0151A  Ck  21 05 0e 01           ; flag(5,14)==1   Ziffer 2 richtig
0x0151E  Ck  21 05 0f 01           ; flag(5,15)==1   Ziffer 3 richtig
0x01522  Ck  21 05 10 01           ; flag(5,16)==1   Ziffer 4 richtig
0x01526  Evt_exec 04 ff 18 13      ; sub19 = aufschließen
0x0152A  Set 22 03 32 01           ; <<< DAS BESTÄNDIGE „CODE WAR RICHTIG"-FLAG
```

`Evt_exec` startet nur einen Thread, es gibt die Kontrolle nicht ab — `Set(3,50,1)`
@0x0152A läuft also **im selben Tick und vor dem ersten Opcode von sub19**. Wenn sub19
später `Message_on 5` ausgibt, steht flag(3,50) bereits auf 1. Die Sonde bestätigt das
(Zeile `f293 Nachricht 5 GEHT AUF flag(3,50)=1`).

### 1.5 `sub19` — der eigentliche Aufschließ-Vorgang

```
0x01960  Set 22 05 00 00 / 0x01964 Set(5,1,0) / 0x01968 Set(5,2,0)   ; Panel zu
0x0196C  Set 22 02 00 00 / 0x01970 Set(2,2,0)                        ; Spieler+KI frei
0x01974  Aot_reset slot0 sce=0      ; die „locked"-Nachricht weg
0x0197E  Aot_reset slot1 sce=0      ; der Leser weg
0x01988  Aot_reset slot2 sce=2      ; <<< DIE TÜR WIRD BENUTZBAR (= der Unlock)
0x01992  Set 22 01 1b 01 / 0x01996 Set(2,7,1)                        ; Kinomodus
0x0199A  Cut_old / 0x0199C Cut_auto
0x0199E  Message_on 5               ; „You've opened the lock."
0x019A4  Set 22 01 1b 00 / 0x019A8 Set(2,7,0)
```

### 1.6 `sub18` — die Ziffern-Aufforderungen und „Wrong code"

```
0x018FA  Ifel_ck / 0x018FE Ck(5,17,0) -> Set(5,17,1) Set(5,18,1) Message_on 1  („second")
0x01914  Ifel_ck / 0x01918 Ck(5,18,1) -> Set(5,18,0) Set(5,19,1) Message_on 2  („third")
0x0192E  Ifel_ck / 0x01932 Ck(5,19,1) -> Set(5,19,0)             Message_on 3  („fourth")
0x01944  Ifel_ck / 0x01948 Ck(5,20,0) -> Set(5,20,1)             Message_on 4  („Wrong code,
                                                                  try again.")
```

Die richtige vierte Ziffer setzt flag(5,20) **vorher** selbst (ROOM10D0 sub07 @0x015C6),
deshalb fällt bei korrektem Code der „Wrong code"-Zweig aus. Danach nimmt der
Tastendruck-Sub den `Else`-Ausgang (`Gosub sub16` = Vollreset + Panel schließen,
z.B. @0x015A6), während sub01 im selben Fenster sub19 zündet.

### 1.7 Bonus-Befund: die drei Zugangscodes

Aus der Riegel-Kette rekonstruiert (`analysis/befunde_2026-09-22/discard-codepanel/code_kette.py`). Jede richtige Taste
schaltet die nächste frei und setzt einen der vier Riegel — die Reihenfolge der Riegel
13→14→15→16 ist die Reihenfolge der Ziffern:

| Raum | Tastenfolge (`Member_cmp 0x0F`) | Tasten-Index → Ziffer | Code |
|---|---|---|---|
| ROOM10D0/10D1 | 6, 5, 3, 4 | Ziffernblock = member 3..12, also `Ziffer = member − 3` | **3201** |
| ROOM1230/1231 | 11, 12, 9, 8 | member 7..16, also `Ziffer = member − 7` | **5632** |
| ROOM11E0/11E1 | 17, 12, 8, 10 | member 8..17, also `Ziffer = member − 8` | **9402** |

Die Abbildungsregel ist **an den Daten gegengeprüft**: ROOM1230 ist der einzige Raum, der
seinen Code im Spiel nennt — msg 10: „A miserable death… He is holding a slip. The numbers
**5632**" are printed on the slip." Die Kette liefert für ROOM1230 genau 5632. Die beiden
anderen Codes folgen mit derselben Regel und sind im Spiel nirgends aufgeschrieben (was
zu einem Vorschau-Build passt).

---

## 2. Der Marker je Raum, mit Adresse

Bank-Zuordnung über die Zeigertabelle `PTR_DAT_80074664`
(`re15_disasm.py table 0x80074664 16`; Port-Spiegel `engine/src/game_state.c:100-106`):

```
[0]=800aca38 [1]=800aca3c [2]=800aca40 (Pause-Wort) [3]=800b0ff8 [4]=800b1018
[5]=800b1028 [6]=800b1030 [7]=800b1038 [8]=800b1058 [9]=800b1078 [10]=800b1098
[11]=800aca44 [12]=800b8520 (Nachrichten-FSM)
```

Bank 5 (`DAT_800b1028`) ist der **Arbeitsspeicher** der Panels, Bank 3 (`DAT_800b0ff8`)
und Bank 4 (`DAT_800b1018`) sind der **beständige Spielstand**.

| Raum | Karte | Taken-Bit (Besitz-Probe des Originals) | Wegwerf-Stelle | Tastenfeld auf | Code-Marker gesetzt | „lock opened" |
|---|---|---|---|---|---|---|
| ROOM10D0 | Blue 0x38 | `Ck(9,52,1)` @0x019C0 | msg 9 @0x019C4 | sub17 @0x018E2 | `Set(3,50,1)` @0x0152A | msg 5 @0x0199E |
| ROOM10D1 | Blue 0x38 | `Ck(9,52,1)` @0x019AA | msg 9 @0x019AE | sub17 @0x018CC | `Set(3,50,1)` @0x01514 | msg 5 @0x01988 |
| ROOM1230 | Red 0x37 | `Ck(9,136,1)` @0x01488 | msg 9 @0x0148C | sub17 @0x013AA | `Set(3,137,1)` @0x00FF2 | msg 5 @0x01466 |
| ROOM1231 | Red 0x37 | `Ck(9,136,1)` @0x01488 | msg 9 @0x0148C | sub17 @0x013AA | `Set(3,137,1)` @0x00FF2 | msg 5 @0x01466 |
| ROOM11E0 | Yellow 0x39 | `Ck(9,138,1)` @0x01FA8 | msg 9 @0x01FAC | sub17 @0x01ECA | `Set(3,139,1)` @0x01A8A | msg 5 @0x01F86 |
| ROOM11E1 | Yellow 0x39 | `Ck(9,138,1)` @0x01FA8 | msg 9 @0x01FAC | sub17 @0x01ECA | `Set(3,139,1)` @0x01A8A | msg 5 @0x01F86 |

⚠️ Das Taken-Bit in Bank 9 ist **kein Besitznachweis**: es wird beim Aufnehmen gesetzt
(`FUN_8004ef90`, Install-Gegenprobe @0x800406d4) und beim Verbrauchen **nicht** gelöscht.
Das Original benutzt es als Näherung; der Port hat dafür mit
`re15_discard_besitz_vor_nachricht()` die echte Inventarsuche und soll dabei bleiben.

---

## 3. Umkehrbarkeit — geprüft, nicht angenommen

Vollzensus über **alle 206 ausgelieferten RDTs** (`tools/discard_codegate.py`, jedes
`Set`/`Ck` opcode-exakt, 230 verschiedene `(bank,bit)` beschrieben, 260 geprüft):

| Marker | Set | davon `val=0` | Ck | Urteil |
|---|---|---|---|---|
| flag(3,50) | 2 (ROOM10D0 @0x0152A, ROOM10D1 @0x01514) | **0** | 24 | einmalig |
| flag(3,137) | 2 (ROOM1230/1231 @0x00FF2) | **0** | 24 | einmalig |
| flag(3,139) | **3** (ROOM11E0/11E1 @0x01A8A, **ROOM1240 @0x0055A**) | 0 | 24 | einmalig, aber **fremdbeschrieben** → §4 |
| flag(3,32) | 1 (ROOM4000 @0x01450) | 0 | 2 | einmalig |
| flag(3,60) | 2 (ROOM3010 @0x02302, ROOM3011 @0x02336) | 0 | 2 | einmalig |
| flag(3,179) | 2 (ROOM3050/3051 @0x0260E) | 0 | 2 | einmalig |
| flag(4,36) | 2 (ROOM11E0/11E1 @0x01FC6) | 0 | 4 | einmalig |
| flag(4,232) | 2 (ROOM1100/1101 @0x00C68) | 0 | 4 | einmalig |
| flag(3,110) | 2 (ROOM1090 @0x024D6 **=1**, ROOM1050 @0x00D88 **=0**) | **1** | 1 | **wird zurückgestellt** |
| *Gegenprobe* Bank 5 | 1647 | **960** | — | flüchtig, taugt nicht |

Zwei Zusatzprüfungen, damit der Zensus vollständig ist:

* **Kein Overlay schreibt Bank 3 oder Bank 4.** `grep 800b0ff8` / `800b1018` über
  `RE_15_Quellcode_Overlays/` und `.agent_refs/stage*_decomp.c`: **0 Treffer** (die
  einzigen Fundstellen liegen im RE2-EXE-Dump und betreffen anderen Inhalt).
  `ghidra1_V2.txt` kennt zu `DAT_800b0ff8` genau **einen** Xref: die Tabelle @0x80074670.
  Damit sind die Skripte die einzigen Schreiber, und der RDT-Zensus ist abgeschlossen.
* **Kein Massen-Löschen.** Es gibt keinen `memset`/Schleifen-Schreiber über
  0x800b0ff8..0x800b10a8. Der Port hält dieselbe Semantik: `g_game.flags[][]` wird nur in
  `re15_game_state_init()` genullt (neues Spiel) und vollständig in den Spielstand
  geschrieben (`re15_savedata.c:152/198`).

**Antwort auf Frage 2 der Aufgabe:** flag(3,50) und flag(3,137) sind echte
„einmalig richtig eingegeben"-Zustände — unumkehrbar, raumübergreifend beständig,
speicherstandsfest. Bank 5 taugt nicht (960 Rückstellungen). flag(3,139) taugt **nicht**,
aus einem anderen Grund:

---

## 4. ⛔ Der Intro-Raum schreibt flag(3,139) — ROOM11E0/11E1 fallen aus

Der Zensus fand für flag(3,139) einen **dritten** Schreiber:

```
ROOM1240.RDT  sub00 @0x00544  Evt_exec  04 ff 18 02          ; beim Betreten -> sub02
              sub02 @0x0055A  Set       22 03 8b 01          ; ERSTE Anweisung: flag(3,139)=1
```

ROOM1240 ist der **Neuspiel-Startraum** (`engine/src/re15_gameflow.c:16`
`#define RE15_NEWGAME_ROOM 0x1240`) — die Erzähler-Montage des Pre-Intros
(`analysis/preintro_re2/re15-montage-inventar.md` §3.3 führt dieselbe Zeile als
„Story-Flag ‚Pre-Intro läuft/gesehen'", ohne die Doppelbelegung zu bemerken).

Gemessen (`probe_r22_codepanel_reihenfolge.c` Teil C):

```
[anker] ROOM1240 sub00 @0x00544 Evt_exec sub02  ->  sub02 @0x0055A Set(3,139,1)  OK
[mess ] Nach dem Betreten von ROOM1240: flag(3,139)=1 (ab Bild 0)
[mess ] ROOM11E0 mit flag(3,139)=0: Ausloeser fuer sub20 (Kartenleser) VORHANDEN
[mess ] ROOM11E0 mit flag(3,139)=1: Ausloeser fuer sub20 (Kartenleser) FEHLT
```

Folge, direkt aus `ROOM11E0` main00 @0x01572: bei gesetztem Flag greift der `Else`-Zweig
@0x016C0 und installiert `Door_aot_set slot3 sce=2` — eine **funktionierende Tür** statt
Nachricht + Kartenleser. Im ausgelieferten Stand ist die Gefängnistür also **ab dem ersten
Bild eines neuen Spiels offen**, den Kartenleser gibt es nicht, und damit sind in
ROOM11E0/11E1 **beide** Zeilen tot: msg 7 („Will you operate…"), msg 9 („You've used the
Yellow Keycard.") und auch msg 5 („You've opened the lock.").

**Zwei Konsequenzen:**

1. Für ROOM11E0/11E1 wäre `flag(3,139)==1` als Gate **das Schlimmste von allem**: es steht
   ab Bild 0, die Abfrage käme also sofort — genau der Fehler, den der Nutzer meldet, nur
   noch früher. Diese Bedingung darf dort **nicht** eingebaut werden.
2. Der Yellow Keycard (0x39, genau 1 `Item_aot_set`) hat im ausgelieferten Stand **keine
   erreichbare Benutzungsstelle**. Das ist eine Eigenschaft der Daten, nicht der
   Wegwerf-Abfrage. Ob das gerichtet werden soll (dann müsste ROOM1240 ein anderes Bit
   benutzen — eine Änderung am ausgelieferten Spielverlauf, **nicht** byte-true), ist eine
   Entscheidung für den Nutzer und gehört nicht in diesen Umbau.

Die Pliers-Stelle desselben Raums ist **nicht** betroffen: sie hängt an flag(3,127)
(gesetzt in ROOM11B0 @0x011EA / ROOM11B1 @0x01186) und flag(4,36) (ROOM11E0 @0x01FC6) und
bleibt erreichbar.

---

## 5. Wie macht es RE2? (Frage 4)

**RE2 kennt die Frage gar nicht — dort ist eine Kartentür ein EINZIGER Schritt.**

Gemessen über die 250 Leon-A-RDTs (`analysis/befunde_2026-09-22/discard-codepanel/re2_panels.py`): `Sce_key_ck`
(der einzige Opcode, mit dem ein Skript den Pad selbst liest) kommt in **drei** Räumen
**je einmal** vor (ROOM10A0, ROOM60A0, ROOM6130) — gegen **15/20/15** Vorkommen allein in
jedem der drei RE1.5-Kartenleser-Räume. Eine Ziffernreihe „Enter the first number." …
„Wrong code, try again." existiert in RE2 **nicht**. Der einzige Kartenleser-Text ist
ROOM2110 msg 6 („Weapon Storage … the power to the card reader is off"), also wieder
einstufig. (Vorbehalt: der Textdekoder ist der RE1.5-Glyphentabelle nachgebaut, RE2s
Kodierung weicht ab — die Zählung der `Sce_key_ck`-Opcodes ist davon unberührt.)

RE2 bindet die Abfrage deshalb nicht an einen „Code fertig"-Zustand, sondern **an die
Aufschließ-Routine selbst** (`info/re2leon/PSX.EXE`):

```
80051718  lbu  v1, 0x800d4249        ; Phase (0 = erster Durchgang, !=0 = Bestätigung)
80051730  bne  v1,zero,0x800517dc
8005173c  lbu  v1, 0x800e873c        ; Flags-Byte DES SCHLUESSELS
80051744  andi v0,v1,0x80            ; Bit 0x80 = „nie wegwerfen"
80051748  bne  v0,zero,0x80051870    ;   -> raus, KEINE Abfrage
   …
800517dc:                            ; Bestätigungs-Durchgang
800517e0  lbu  v0, 0x800e873c
800517f0  andi v0,v0,0x80
800517f4  bne  v0,zero,0x80051870
80051808  lbu  v0, <Tuer-Record>     ; DER ZAEHLER (Item_aot_set-Feld)
80051810  addiu v0,v0,-1             ; eine Tuer weniger
8005181c  sb   v0, <Tuer-Record>
80051820  andi v0,v0,0xff
80051824  bne  v0,zero,0x8005185c    ; noch nicht null -> KEINE Abfrage
80051830  addiu a2,zero,9            ; Skript 9 = die Wegwerf-Abfrage
80051834  jal  0x8002fe38
```

Die Abfrage fällt also **nach** dem Aufschließen, und „erledigt" heißt bei RE2
„der Zähler ist auf null". Die davorliegende Besitzprüfung sitzt bei
@0x80051628 `jal 0x800696cc` / @0x80051634 `bltz s1` — die Kettenposition, die der Port
für `re15_discard_besitz_vor_nachricht()` schon übernommen hat.

**Die Vorlage, die RE2 liefert, ist damit: die Abfrage gehört an den Moment, in dem die
Tür tatsächlich aufgeschlossen wird.** In RE1.5 ist dieser Moment nicht msg 9, sondern
`sub19` — die Stelle, an der `Aot_reset <Türslot> sce=2` die Tür benutzbar macht
(ROOM10D0 @0x01988) und unmittelbar danach `Message_on 5` („You've opened the lock.")
läuft (@0x0199E).

---

## 6. Die Bedingung je Stelle — umsetzungsreif (Frage 5)

### 6.1 Die zehn Stellen, die sich NICHT ändern

Bei allen zehn liegt die Wegwerf-Zeile **schon hinter** der Bedienung; ihr beständiger
Marker steht im selben geradlinigen Block direkt davor oder direkt danach.

| # | Raum | msg | Gegenstand | Warum unverändert |
|---|---|---|---|---|
| 1 | ROOM1090 | 9 | Fire Extinguisher 0x31 | Kein Panel (0× `Sce_key_ck`). msg 8 stellt die Ja/Nein-Frage, msg 9 liegt in der Rettungs-Cutscene sub03 @0x02502, davor `Set(3,187,1)` @0x024D2 + `Set(3,110,1)` @0x024D6. |
| 2 | ROOM1100 | 4 | Minidisc Player 0x44 | Cursor-Panel (6× `Sce_key_ck`), aber **zweistufig VOR** der Zeile: sub07 @0x00D66-7A macht es auf, sub08 (member 4) dann sub02 (member 5). sub02 setzt @0x00C68 `Set(4,232,1)` und gibt **danach** @0x00C90 msg 4 aus. Sonde Teil B: `flag(4,232)=1 ab f0, msg4=f0`. |
| 3 | ROOM1101 | 4 | Minidisc Player 0x44 | wie ROOM1100, gleiche Offsets |
| 4 | ROOM11E0 | 12 | Pliers 0x30 | Eigenes, **einstufiges** Cursor-Panel (sub26 @0x020CC-D4 armiert 5.21/5.22, sub01 @0x01B00 `member 0x0F == 7`). sub21 setzt @0x01FC6 `Set(4,36,1)` und gibt **danach** @0x01FEE msg 12 aus. |
| 5 | ROOM11E1 | 12 | Pliers 0x30 | wie ROOM11E0 |
| 6 | ROOM3010 | 1 | Green Keycard 0x36 | Kein Panel. sub02 @0x02302 `Set(3,60,1)` **vor** @0x02306 msg 1. |
| 7 | ROOM3011 | 1 | Green Keycard 0x36 | wie ROOM3010, `Set` @0x02336 / msg @0x0233A |
| 8 | ROOM3050 | 5 | Red Master Keycard 0x46 | Cursor-Panel (5× `Sce_key_ck`), **einstufig und VOR** der Zeile: sub10 @0x02598-AC armiert 5.0..5.5 nach der Ja/Nein-Frage msg 4, sub01 @0x0220E `member 0x0F == 11` → sub15. sub15 gibt @0x02608 msg 5 aus und setzt @0x0260E `Set(3,179,1)`, @0x02612 `Set(3,176,1)`. Die Zeile liegt also hinter der Bedienung; der Marker folgt in der nächsten Anweisung. |
| 9 | ROOM3051 | 5 | Red Master Keycard 0x46 | wie ROOM3050 |
| 10 | ROOM4000 | 2 | Blue Master Keycard 0x47 | Kein Panel. sub02: msg 0 (Panel-Text) → msg 1 („Will you use the Blue Master Keycard?") → @0x01446 `Ck(12,31,0)` → @0x0144A msg 2 → @0x01450 `Set(3,32,1)`. |

### 6.2 Die vier Stellen, die sich ändern

Für ROOM10D0/10D1/1230/1231 gilt: **die Wegwerf-Stelle wandert von msg 9 nach msg 5
desselben Raums.**

| Raum | jetzt | neu | zusätzliche Bedingung |
|---|---|---|---|
| ROOM10D0 | `{ 0x10D0, 9, 0x38 }` | `{ 0x10D0, 5, 0x38 }` | `re15_game_flag_get(3, 50) == 1` |
| ROOM10D1 | `{ 0x10D1, 9, 0x38 }` | `{ 0x10D1, 5, 0x38 }` | `re15_game_flag_get(3, 50) == 1` |
| ROOM1230 | `{ 0x1230, 9, 0x37 }` | `{ 0x1230, 5, 0x37 }` | `re15_game_flag_get(3, 137) == 1` |
| ROOM1231 | `{ 0x1231, 9, 0x37 }` | `{ 0x1231, 5, 0x37 }` | `re15_game_flag_get(3, 137) == 1` |

Warum das trägt — jeder Punkt belegt:

* **msg 5 kommt nur nach richtigem Code.** msg 5 wird ausschließlich in sub19 ausgegeben
  (ROOM10D0 @0x0199E). sub19 wird ausschließlich von sub01 @0x01526 gezündet, und zwar erst
  wenn flag(5,13..16) alle vier stehen. Die Riegel werden ausschließlich in sub06..sub16
  gesetzt, die nur bei offenem Tastenfeld (`Ck(5,1,1)`) laufen. Das Tastenfeld geht
  ausschließlich in sub17 auf (`Set(5,2,1)` nur @0x018F4), und sub17 wird ausschließlich in
  sub20 @0x019CA gestartet — hinter der Ja-Antwort und der Karten-Prüfung.
  **Kein anderer Weg führt zu msg 5.**
* **Das Flag steht garantiert.** `Set(3,lockbit,1)` läuft im selben Tick unmittelbar nach
  dem `Evt_exec`, bevor sub19 seinen ersten Opcode ausführt (§1.4). Die Gate-Prüfung ist
  also ein Riegel gegen künftige Umbauten, keine Wettlaufbedingung.
* **„Einmalig" ist automatisch erfüllt.** Sobald flag(3,lockbit) steht, installiert main00
  (@0x01026) die normale Tür statt Leser + Nachricht — msg 5 kann nie ein zweites Mal
  kommen. Der vom Nutzer verlangte „einmalig"-Charakter braucht keinen Zähler.
* **Die Besitzprüfung bleibt wie sie ist.** `re15_discard_besitz_vor_nachricht()` an der
  RE2-Kettenposition (@0x80051628/@0x80051634/@0x8005164C/@0x80051670) — sie ist die
  Antwort auf den Fehler „Abfrage obwohl man den Schlüssel nicht hat" und wird hier nicht
  berührt. Zwischen msg 9 und msg 5 kann der Spieler weder das Inventar öffnen noch den
  Raum wechseln (flag(2,0)/flag(2,2) frieren ihn ab @0x01504/@0x01508), die Karte ist bei
  msg 5 also noch da.

### 6.3 Die zwei Stellen, die weder bleiben noch wandern können

| Raum | msg | Gegenstand | Befund |
|---|---|---|---|
| ROOM11E0 | 9 | Yellow Keycard 0x39 | **unerreichbar** — §4. Weder msg 9 noch msg 5 laufen je, weil ROOM1240 @0x0055A flag(3,139) vorab setzt und main00 @0x01572 dann den Else-Zweig nimmt. Ein Gate auf flag(3,139) wäre ab Bild 0 wahr und damit schädlich. |
| ROOM11E1 | 9 | Yellow Keycard 0x39 | wie ROOM11E0 |

Empfehlung: diese zwei Einträge aus der Tabelle **entfernen** (mit dieser Begründung im
Generator-Kopf), statt sie auf ein fremdbeschriebenes Flag zu gaten. Die Stückzahl der
Wegwerf-Stellen sinkt damit von 16 auf 14. Die Alternative — ROOM1240 ein anderes Bit
geben, damit die Gefängnistür wieder verschlossen startet — ist eine Änderung am
ausgelieferten Spielverlauf und braucht die Zustimmung des Nutzers.

### 6.4 Wo der Riegel im Port sitzen soll

Die Bedingung ist **datengetrieben, nicht als `if (room == 0x10D0)`**: der Generator
`tools/gen_discard_sites.py` kann sie selbst ableiten, und zwar mit denselben drei
Messungen, die `tools/discard_codegate.py` schon fährt:

1. Führt der Raum die vier Ziffern-Texte **und** gibt er sie per `Message_on` aus?
2. Startet der Block der Wegwerf-Zeile **nach** ihr ein `Evt_exec`/`Gosub`, von dem aus
   (transitiv) eine Ziffern-Nachricht ausgegeben wird? → dann ist die Zeile zu früh.
3. Dann: als Stelle die msg-Id nehmen, die in derselben sub-Kette **hinter** der
   Riegel-Kette `Ck(3,x,0) + Ck(5,13..16,1) → Evt_exec subN → Set(3,x,1)` ausgegeben wird,
   und `(bank,bit) = (3,x)` als Gate mitschreiben — plus der Vollzensus-Prüfung, dass
   dieses `(bank,bit)` **nur** von den Räumen dieser Stelle geschrieben wird (sonst
   verwerfen, §4).

So bleibt die Tabelle abgeleitet statt gewählt, und der ROOM1240-Fall fällt von selbst
heraus statt als handgepflegte Ausnahme.

---

## 7. Die Messwerkzeuge dieses Laufs

| Datei | Was sie messt |
|---|---|
| `re15_port/tools/discard_codegate.py` | Vollzensus: je Wegwerf-Stelle Panel-Art, Ausführungs-Reihenfolge (nicht Datei-Offset!), Marker vor/nach der Zeile, Riegel-Kette, und für jeden gefundenen `(bank,bit)` alle `Set`/`Ck` über alle 206 RDTs + Umkehrbarkeits-Urteil. Endet mit der 16-Zeilen-Ergebnistabelle. |
| `re15_port/tests/unit/probe_r22_codepanel_reihenfolge.c` (+ `tests/unit/probes/r22_codepanel.cmake`) | Fährt die ausgelieferten SCD-Bytes im Port-VM. Teil A: Reihenfolge msg 9 / msg 0 / msg 5 mit Bildnummern (ROOM10D0). Teil B: Gegenprobe ROOM1100 (Marker steht schon). Teil C: ROOM1240-Kollision und ihre Wirkung auf ROOM11E0s Auslöser. Jeder Teil prüft vorher seine Byte-Anker. |
| `re15_port/tools/scd_walk_lib.py` | Der SCD-Walker als importierbare Bibliothek — **ohne eigene Längentabelle**: sie liest die eine vorhandene aus `scd_dump_room.py` (Quelle `engine/src/scd_vm.c s_opcode_sizes`) statt eine zweite Wahrheit anzulegen, und bricht mit `AssertionError` ab, wenn dieser Zugriff nicht mehr passt. Dazu der Nachrichten-Dekoder (Glyphentabelle = `engine/src/msg_common.c:175-199`). |
| `analysis/befunde_2026-09-22/discard-codepanel/code_kette.py` | Löst die Ziffern-Kette der sechs Leser auf → die drei Zugangscodes (§1.7). |
| `analysis/befunde_2026-09-22/discard-codepanel/re2_panels.py` | Zählt `Sce_key_ck` und sucht Ziffern-Texte in den 250 RE2-Leon-RDTs (§5). |
| `analysis/befunde_2026-09-22/discard-codepanel/bitfrage.py` | Vollzensus für beliebige `(bank,bit)` — der Schnellweg für die nächste Flag-Frage. |
| `analysis/befunde_2026-09-22/discard-codepanel/codegate-lauf-2026-09-22.txt` | Die vollständige Ausgabe des Zensus-Laufs, auf dem dieses Dossier beruht. |

Alle fünf hängen nur an `scd_walk_lib.py` + `scd_dump_room.py` und sind damit
**unabhängig** von `discard_zensus.py` / `discard_nutzstellen.py` des parallelen
Wegwerf-Laufs — sie laufen auch, wenn der noch nicht zusammengeführt ist.

Fahren:

```bash
python re15_port/tools/discard_codegate.py

export PATH="/c/msys64/mingw64/bin:$PATH"
cmake -S re15_port -B re15_port/build_r22codepanel -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON \
      -DFETCHCONTENT_SOURCE_DIR_SDL2=<repo>/re15_port/build/_deps/sdl2-src
cmake --build re15_port/build_r22codepanel --target probe_r22_codepanel_reihenfolge
./re15_port/build_r22codepanel/tests/unit/probe_r22_codepanel_reihenfolge.exe      # a|b|c
```

Ergebnis des Laufs vom 2026-09-22: `=== SONDE: OK ===` für alle drei Teile.

---

## 8. Offene Punkte

1. **Yellow Keycard (ROOM11E0/11E1).** Entscheidung des Nutzers nötig: Stelle streichen
   (byte-true zum ausgelieferten Stand) oder ROOM1240 @0x0055A auf ein freies Bit legen,
   damit die Gefängnistür verschlossen startet und der Kartenleser wieder existiert
   (Änderung am Spielverlauf, **nicht** byte-true). §4.
2. **flag(3,110) wird in ROOM1050 @0x00D88 zurückgestellt.** Betrifft die Wegwerf-Abfrage
   nicht (ROOM1090 braucht kein Gate), ist aber eine Falle für jeden, der dieses Bit
   künftig als „erledigt"-Marker benutzen will.
3. **Die Sonde messt die Reihenfolge nur an ROOM10D0.** Die anderen fünf Leser sind
   strukturgleich und statisch vollständig belegt (§2), aber nicht einzeln gefahren.
4. **Die beiden Codes 3201 (ROOM10D0) und 9402 (ROOM11E0)** stehen im Spiel nirgends
   geschrieben — nur ROOM1230s 5632 liegt auf dem Zettel. Ob das ein Vorschau-Loch ist
   oder ein fehlendes Dokument, ist nicht untersucht.
