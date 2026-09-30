# Spur F — Leichen ROOM1110 / ROOM1230: Untersuchen-Text + einmal Handgun-Munition

Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code). Zweig `r34n/leichen`, Baum `.claude/worktrees/r34n_leichen`.
Stand: vollstaendig (Ermittlung + Bauplan). Werkzeuge: `re15_port/tools/r34n_f/`, Sonde `probe_r34n_f_leiche`.

## 0 Kurzfassung

- **Wo:** Beide Leichen sind Original-EREIGNISSE: ROOM1110/1111 Platz 5 -> sub02, Message_on @0x0D04
  `2b 00 ff ff` (Nachricht 0); ROOM1230/1231 Platz 18 -> sub21, Message_on @0x014BC `2b 0a ff ff`
  (Nachricht 10). Beide Nachrichten werden NUR dort geoeffnet (Zensus). Ist im Port gemessen: drei Seiten
  Originaltext mit dem Zettel-Code, beliebig oft, kein Item.
- **Text:** 1110 lang "It's a police officer, he's dead." ▼ "He is holding something." / kurz "It's a police
  officer, he's dead."; 1230 lang "A miserable death..." ▼ "He is holding something." / kurz "A miserable
  death...". Alle Glyphen aus dem Auslieferungsstand (Fundstellen je Wort), wortgleiche Teile als
  Original-Bytes (daher klein "police"/"holding"), "…" = `57 57 57` (RE1.5 schreibt "..." 270x so, nie vier
  Punkte, keine Einzelglyphe), Seitenumbruch `02 00` wie im Original, Punkt am Satzende (97 % des Bestands).
- **Mechanismus:** Original-Ereignis bleibt; der Port tauscht an Message_on nur die Nachrichten-Id
  (Port-IDs 20/21 bzw. 22/23) und oeffnet im Bild, in dem der Text zugeht, das NORMALE Aufnahme-Modal
  (FUN_8001db28-Port) mit H. Gun Bullets. "Nein"/voll -> nichts gemerkt, naechstes Untersuchen bietet
  wieder an (Original-Regel: nur der Ja-Zweig schaltet ab/merkt, @0x8001e054/@0x8001e06c/@0x8001e090/
  @0x8001e0d0). "Ja" -> Bank-9-Bit 61 bzw. 62, danach nur noch der kurze Text. RE2-Vorbild: ROOM4050
  Leiche haelt ein Item ueber den normalen Item-Weg ohne Weltmodell (md1 0xFF) + Satz "He's holding
  something." -> **kein Modell**.
- **Munition:** Item 0x15 "H. Gun Bullets", Menge **15** (haeufigste Packung RE1.5 22/38, RE2 37/44);
  Nutzer-Halbierung + Stapeln greifen automatisch (7 Schuss).
- **Belegt per Simulation** in der Sonde `probe_r34n_f_leiche` (4 Raeume x Ist/Nein/Ja/Voll, 0 Fehler):
  Modal im Schliess-Bild, Ereignis-Faden steht waehrend des Modals auf @0x0D09/@0x014C1, laeuft danach
  normal weiter, Platz wieder scharf, Ja -> 50 -> 57 Munition + Bit.
- **Bau:** zwei neue Dateien (`re15_leiche.h`, `leiche_1110_1230.c`), ZWEI Haken-Zeilen
  (`scd_vm.c` op_message_on hinter der `[msg]`-Logzeile, `game_step_common.c` nach `re15_granate_tick`), kein
  Raumstart-Haken, kein neuer AOT-Platz, kein Asset-Patch, Speicherstand unveraendert (Bank 9 wird
  gesichert).

## 1 Nutzerwortlaut + Lesart

### 1.1 Wortlaut (AUFTRAG.md Z. 49 und Z. 61-64, woertlich)

> Passend dazu möchte ich, das im ROOM 1110 die Leiche nicht mehr den gleichen Text hat, sondern
> stattdessen: "It's a Police officer, he's dead. He is Holding something" und dann soll man einmal
> Handfeuerwaffen Munition erhalten. Der Text soll immer wiederholt werden können, wenn man den Körper
> anklickt, bis man die Munition annimmt. Danach soll nur noch "It's a Police officer, he's dead." kommen
> wenn man ihn anklickt.

> Passend dazu, soll dann natürlich in ROOM 1230 bei der Leiche, ähnlich wie davor im Evidence Room der
> Text geändert werden:
> "A miserable death…. He is Holding something."
> und dann soll Handfeuerwaffen Munition wieder zum mitnehmen erscheinen. Wenn man sie einmal aufgenommen
> hat, kommt nur noch:
> "A miserable death…"

### 1.2 Lesart (jede Mehrdeutigkeit aufgeloest, mit Grund)

| # | Frage | Lesart | Grund |
|---|---|---|---|
| L1 | Welche "Leiche"? | ROOM1110: Ereignis-Platz **Slot 5** (sce 3 -> sub02, Nachricht 0). ROOM1230: Ereignis-Platz **Slot 18** (sce 3 -> sub21, Nachricht 10). | Einzige Leichen-Saetze beider RDTs (Abschnitt 3.1). "Evidence Room" = ROOM1110: DEBUG.BIN-Sprungliste Stage 0 Index 0x11 = `"EVIDENCE ROOM"` (`include/debug_jump_table.h`, Generator `tools/gen_debug_jump_table.py`). |
| L2 | "Passend dazu" | Die Leiche haelt nicht mehr den Zettel mit dem Code (1110: "4312", 1230: "5632") — diese Codes wandern in Spur E in Marvins Notiz (ROOM1020) bzw. die Armory Notice (ROOM1010). Satz 3 der Original-Nachricht ("The numbers ... printed on the slip.") entfaellt deshalb ganz. | AUFTRAG.md Z. 44 ("The new code is: 4312") und Z. 57 ("with the code: 5632") stehen unmittelbar vor den beiden Leichen-Absaetzen. |
| L3 | "einmal Handfeuerwaffen Munition" | EINE Packung Item **0x15 "H. Gun Bullets"**, nur ein einziges Mal je Leiche (einmal = einmalig, nicht "eine Patrone"). Menge: Abschnitt 3.6. | Namens-Blob Abschnitt 3.6. |
| L4 | "bis man die Munition annimmt" / "zum mitnehmen erscheinen" | Das normale Aufnahme-Modal des Ports (Bild zoomt ein, "Will you take the H. Gun Bullets?" Ja/Nein) — dasselbe wie bei jedem Item (FUN_8001db28-Port `item_modal_common.c`). "annehmen" setzt eine Wahl voraus (Ja/Nein); "erscheinen zum mitnehmen" = das Item-Bild erscheint. | Wortlaut; Auftrag Spur F ("Aufnahme-Abfrage wie bei jedem Item, Ja/Nein"). |
| L5 | Nein / Inventar voll | Beides: Munition bleibt bei der Leiche, das naechste Untersuchen zeigt WIEDER den langen Text und WIEDER das Angebot. | "Der Text soll immer wiederholt werden können ... bis man die Munition annimmt." Voll = nicht angenommen (Modal Zustand 8, Item bleibt, `item_modal_common.c:334`). |
| L6 | Gross-/Kleinschreibung "Police", "Holding" | Die WORTGLEICHEN Teile kommen als ORIGINAL-BYTES aus der RDT: "It's a police officer, he's dead." (klein, ROOM1110 msg 0 Seite 1), "He is holding" (klein, beide RDTs Seite 2), "A miserable death..." (ROOM1230 msg 10 Seite 1). Neu ist nur "something". | Auftrag Spur F: "Wortgleiche Teile aus den Original-Bytes uebernehmen; Nutzer-Schreibung sonst zeichengetreu". Die Grossbuchstaben stehen bei WORTGLEICHEN Teilen (Nutzer zitiert den Original-Satz), "something" ist klein geschrieben. |
| L7 | Seitenumbruch zwischen den zwei Saetzen | Wie im Original: `02 00` (Seite fuer Seite, Taste blaettert). | Der Original-Satz hat genau dort `02 00` (ROOM1110 @0x0D8B, ROOM1230 @0x170A). Der Nutzer schreibt alles in eine Zeile, weil er den Text abtippt; die Seitenfolge ist Teil der "wortgleichen" Original-Bytes. |
| L8 | Punkt am Ende von "He is Holding something" (1110 ohne, 1230 mit) | In BEIDEN Raeumen mit Punkt 0x57. | (a) Derselbe Satz steht in 1230 beim Nutzer MIT Punkt; (b) der ersetzte Original-Satz "He is holding a slip." endet in beiden RDTs auf 0x57; (c) Zensus Abschnitt 3.2: Anteil der ausgelieferten Nachrichten, die ohne Satzzeichen enden. |
| L9 | "…." (Auslassung + Punkt) in 1230 | "A miserable death" + **drei** 0x57 wie im Original, KEIN vierter Punkt. "…" = RE1.5-"..." = `57 57 57`. | Der Teil ist wortgleich zum Original (`1d 00 49 45 4f 41 4e 3d 3e 48 41 00 40 41 3d 50 44 57 57 57` @0x16F6); der Nutzer schreibt den Nachtext "A miserable death…" mit genau EINER Auslassung; der Punkt hinter "…" ist sein Satztrenner beim Abtippen zweier Seiten in einer Zeile. Zensus 3.2: vier Punkte in Folge kommen im Auslieferungsstand 0-mal vor (1227 Nachrichten). |
| L10 | Nachtext | 1110: "It's a police officer, he's dead." = Original-Seite 1 von msg 0 + `01 00`. 1230: "A miserable death..." = Original-Seite 1 von msg 10 + `01 00`. | Nutzer-Wortlaut, wortgleich zum Original. |
| L11 | Menge "Stapeln wie Runde 26" | Keine Sonderregel: das Modal halbiert (Nutzer-Entscheidung 2026-09-20) und stapelt (2026-09-26) automatisch wie bei jeder Welt-Munition. | `re15_pickup_menge_nutzer` / `re15_inv_grant_stapeln_nutzer` sitzen im EINZIGEN Welt-Aufnahme-Pfad (`item_modal_common.c:155`, `:334`). |

## 2 Ist-Zustand im Port (gemessen)

Echte exe dieses Baums (Stand master cf0e68ba, `local_build.sh configure` + `build`, LOCAL-BUILD-OK),
als Kopie `build/r34n_f/bin/re15_pc_r34nf.exe`, Lauf `re15_port/tools/r34n_f/r34n_f_lauf.sh`
(RE15_NO_INTRO, Titel-Autovorlauf, `RE15_DEBUG_JUMP=<raum>@240`, `RE15_PLAYER_POS` vor der Leiche
mit Blick +X, `RE15_PAD_AT="330:A,470:A,560:A,700:A,820:A"`, Framedump je 10 Bilder, `RE15_FORCE_CUT`
NUR fuer die Sicht: 1110 Cut 3, 1230 Cut 8 — die zwei Cuts, deren Blickrichtung am naechsten an der
Leiche liegt, 8,8 deg / 12,0 deg, aus den RID-Saetzen @RDT+0x60 gerechnet).

| Raum | Lauf | Beobachtung |
|---|---|---|
| ROOM1110 | `ist1110_b` | Sprung-Spawn (10580,2650) -> Kollision schiebt auf (10448,2646). F330 Viereck: `thread start slot=10 first_op=0x46` (= sub02 Aot_reset), `Plc_motion(entity=1, motion=11)`, ~30 Bilder spaeter `[msg] room=1110 id=0`. Drei Seiten: "It's a police officer, he's dead." ▼ "He is holding a slip." ▼ 'The numbers "4312" are / printed on the slip.' (4312 gruen). Danach steht das Ereignis wieder scharf: F820 Viereck -> dasselbe Ereignis ein zweites Mal (`[msg] room=1110 id=0` 2x). Kein Item, kein Modal (modal.log leer). |
| ROOM1230 | `ist1230_b` | Spawn (3280,25900), F330 Viereck -> sub21, `[msg] room=1230 id=10`: "A miserable death..." ▼ "He is holding a slip." ▼ 'The numbers "5632" are / printed on the slip.' Zweites Untersuchen F820 wieder dasselbe. |

Belegbilder: `F_belege/ist_1110_leiche.png`, `F_belege/ist_1230_leiche.png` (je F320 vor dem Druck,
F370 Schreibmaschine, F450 Seite 1, F540 Seite 2, F660 Seite 3, F760 nach dem Text).
Nebenbefund (nicht Spur F): im Port aendert Plc_motion(1,11,0) Leons sichtbare Haltung nicht
(Ausschnitt F320..F760 gleich: stehend, Messer gesenkt). Ob das Original hier eine andere Pose
zeigt, ist NICHT gemessen; das Ereignis bleibt in Spur F unveraendert, die Frage gehoert nicht in
diesen Bau (Abschnitt 8).

Ist-Stand in einem Satz: der Port zeigt beide Original-Texte byte-true und beliebig oft, es gibt
kein Munitionsangebot und kein Merkbit.

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die zwei Leichen im Auslieferungsstand (selbst gelesen, `tools/scd_dump_room.py`)

**ROOM1110.RDT** (Evidence Room), main00:

    0x00AEE  Aot_set  2c 05 03 31 00 00 cc 29 66 08 e8 03 e8 03 ff 00 18 02 00 00
             slot 5, sce 3 (Ereignis), sat 0x31, Rechteck (10700,2150, 1000x1000), Nutzlast -> sub02

sub02 @0x0CEE (Ereignis der Leiche):

    0x0CEE  Aot_reset  46 05 00 00 00 00 00 00 00 00   Slot 5 stilllegen
    0x0CF8  Work_set   2e 01 00                         Spieler
    0x0CFC  Plc_motion 3f 01 0b 00                      Spieler-Ereignisbewegung 11
    0x0D00  Sleep      09 0a 1e 00                      30 Bilder
    0x0D04  Message_on 2b 00 ff ff                      Nachricht 0, Maske 0xffff (Einfrieren)
    0x0D08  Evt_next   02
    0x0D0A  Plc_motion 3f 01 0b 00                      Spieler-Ereignisbewegung 11 (zweiter Teil)
    0x0D0E  Plc_flg    43 00 80 00
    0x0D12  Sleep      09 0a 1e 00                      30 Bilder
    0x0D16  Plc_ret    42
    0x0D18  Aot_reset  46 05 03 31 ff 00 18 02 00 00   Slot 5 wieder scharf (sce 3 -> sub02)
    0x0D22  Evt_end

Nachricht 0 @0x0D68 (roh): `04 02` | `25 50 3a 4f 00 3d 00 4c 4b 48 45 3f 41 00 4b 42 42 45 3f 41 4e 18 00 44 41 3a 4f 00 40 41 3d 40 57`
= "It's a police officer, he's dead." | `02 00` | `24 41 00 45 4f 00 44 4b 48 40 45 4a 43 00 3d 00 4f 48 45 4c 57`
= "He is holding a slip." | `02 00` | `30 44 41 00 4a 51 49 3e 41 4e 4f 00 61 05 01 10 0f 0d 0e 05 00 19 00 3d 4e 41 08 4c 4e 45 4a 50 41 40 00 4b 4a 00 50 44 41 00 4f 48 45 4c 57`
= 'The numbers "4312" are / printed on the slip.' | `01 00`.

**ROOM1230.RDT** (B1 Corridor Light, Tastenfeld zum Waffenlager), main00:

    0x00D52  Aot_set  2c 12 03 31 00 00 48 0d 38 63 e8 03 e8 03 ff 00 18 15 00 00
             slot 18, sce 3, sat 0x31, Rechteck (3400,25400, 1000x1000), Nutzlast -> sub21

sub21 @0x14A6 — bytegleich aufgebaut wie ROOM1110 sub02, nur Slot 18 und `2b 0a ff ff` (Nachricht 10):

    0x14A6 Aot_reset 46 12 00 ... | 0x14B0 Work_set 2e 01 00 | 0x14B4 Plc_motion 3f 01 0b 00 | 0x14B8 Sleep 1e
    0x14BC Message_on 2b 0a ff ff | 0x14C0 Evt_next | 0x14C2 Plc_motion 3f 01 0b 00 | 0x14C6 Plc_flg 43 00 80 00
    0x14CA Sleep 1e | 0x14CE Plc_ret | 0x14D0 Aot_reset 46 12 03 31 ff 00 18 15 00 00 | 0x14DA Evt_end

Nachricht 10 @0x16F4 (roh): `04 02` | `1d 00 49 45 4f 41 4e 3d 3e 48 41 00 40 41 3d 50 44 57 57 57` = "A miserable death..." |
`02 00` | `24 41 00 45 4f 00 44 4b 48 40 45 4a 43 00 3d 00 4f 48 45 4c 57` = "He is holding a slip." | `02 00` |
`30 44 41 ... 61 05 01 11 12 0f 0e 05 00 19 ... 4f 48 45 4c 57` = 'The numbers "5632" are / printed on the slip.' | `01 00`.

Beide Leichen sind also ein ORIGINAL-Ereignis mit eigener Spieler-Bewegung — Plc_motion(1,11,0), der Text friert die Welt ein
(Maske 0xffff, `LAB_800404f4` @0x80040508 `lhu a3,2(v0)` / @0x8004051c `sll a3,a3,16`, Port `scd_vm.c` op_message_on),
danach zweite Plc_motion(1,11,0) + Plc_flg + 30 Bilder + Plc_ret, und der Platz wird wieder scharf. Kein Item, kein Flag.

Der Opcode, an dem der Bau einhaengt — Message_on-Handler LAB_800404f4 (Tabelle 0x800744a8 + 0x2B*4 =
@0x80074554 -> 0x800404f4, selbst gelesen):

    800404fc: lw   v0,28(a0)       ; Programmzaehler
    80040500: ori  a1,zero,0x300   ; Nachrichtenart
    80040504: lbu  a2,1(v0)        ; pc[1] = Nachrichten-Id
    80040508: lhu  a3,2(v0)        ; pc[2..3] = Maske (0xffff bei beiden Leichen)
    8004050c: addiu v0,v0,4        ; pc += 4
    80040518: jal  0x80027e68      ; Nachricht oeffnen
    8004051c: sll  a3,a3,16        ; Maske << 16 -> g_pauseflags (Freeze)

Der Bau tauscht NUR a2 (die Id) gegen die Port-Nachricht; Maske, pc += 4 und der nicht blockierende
Schreibmaschinen-Weg bleiben wie im Klartext-Zweig des Ports (`scd_vm.c:1816-1818`).

### 3.2 Text: Zeichen, Satzbau, Glyph-Belege (Werkzeug `re15_port/tools/r34n_f/r34n_f_zensus.py texte glyphen breite msgref`)

Zensus ueber den Auslieferungsstand: 240 RDT-Dateien, 206 mit Kopf, **1227 Nachrichten** mit `01`-Ende.

| Befund | Zahl | Folgerung |
|---|---|---|
| letztes Zeichen vor `01`: `.` (0x57) / `!` / `?` / Auswahl-Code 03 / `"` | 814 / 158 / 126 / 81 / 14 | 1193 von 1227 (97 %) enden auf Satzzeichen oder Auswahl. Auf einen BUCHSTABEN enden nur 14 (m 4, s 3, t 2, y 2, c 2, n 1). -> L8: Punkt ans Satzende. |
| Folgen `57 57 57` ("...") | 270, davon 24 direkt vor Seitenumbruch `02` (u.a. ROOM1230 msg 10 selbst) | "..." ist in RE1.5 IMMER drei Einzelpunkte 0x57. |
| Folgen `57 57 57 57` (vier Punkte) | **0** | -> L9: kein vierter Punkt. |
| Glyphe 0xF2 (vom Dump-Werkzeug als "..." gefuehrt) | **0** Vorkommen in Nachrichten | Es gibt KEINE Auslassungs-Einzelglyphe; "…" des Nutzers = `57 57 57`. |
| Nachrichtenbloecke ROOM1110 == ROOM1111, ROOM1230 == ROOM1231 | 10/10, 12/12 bytegleich | Varianten tragen denselben Text an denselben IDs. |

Glyph-Belege (erste Fundstellen im Auslieferungsstand, `glyphen`):

| Baustein | Bytes | Beleg |
|---|---|---|
| "It's a police officer, he's dead." | `25 50 3a 4f 00 3d 00 4c 4b 48 45 3f 41 00 4b 42 42 45 3f 41 4e 18 00 44 41 3a 4f 00 40 41 3d 40 57` | ROOM1110.RDT @0x00D6A (msg 0, Seite 1), ROOM1111 @0x00D6A |
| "He is holding" | `24 41 00 45 4f 00 44 4b 48 40 45 4a 43` | ROOM1110.RDT @0x00D8D (msg 0, Seite 2); ROOM1230.RDT @0x0170C (msg 10, Seite 2); ROOM1011 @0x01259 |
| " something" | `00 4f 4b 49 41 50 44 45 4a 43` | ROOM1110.RDT @0x00EC4 (msg 6 "A rocket launcher, has something"), 19 Fundstellen |
| "." | `57` | Satzende von "He is holding a slip." ROOM1110.RDT @0x00DA1 / ROOM1230.RDT @0x01720 |
| "A miserable death..." | `1d 00 49 45 4f 41 4e 3d 3e 48 41 00 40 41 3d 50 44 57 57 57` | ROOM1230.RDT @0x016F6 (msg 10, Seite 1), ROOM1231 @0x016F6 |
| Kopf / Seitenumbruch / Ende | `04 02` / `02 00` / `01 00` | wie msg 0 bzw. msg 10 selbst (@0x0D68/@0x0D8B/@0x0DD3 bzw. @0x16F4/@0x170A/@0x1752) |

Vorschau in der Spielschrift (`re15_port/tools/r34n_f/r34n_f_textvorschau.py`, derselbe Weg wie der
PC-Zeichner: DATA/TEX.TIM Schriftseite x 256.., CLUT-Zeile je Farbcode, Vorschub DEBUG.BIN[0x4416+code]):
`F_belege/textvorschau.png` — Original- und neue Seiten nebeneinander. Die Original-Seiten stimmen
Glyph fuer Glyph mit den Framedumps der echten exe (`F_belege/ist_1110_leiche.png`) ueberein (auch die
gruenen Ziffern), die neuen Seiten lesen sich "It's a police officer, he's dead." / "He is holding
something." / "A miserable death...".

Zeilenbreiten (Vorschub `include/font_width.h` = DEBUG.BIN[0x4416+code], Leser FUN_80028868):
1110 lang 210 px / 160 px, 1110 kurz 210 px, 1230 lang 129 px / 160 px, 1230 kurz 129 px.
Bestand: 2505 Zeilen, Median 171 px, 99 %-Quantil 271 px -> alle neuen Zeilen im ueblichen Mass, kein Umbruch noetig.

**Wer oeffnet die Leichen-Nachricht?** (`msgref`, ueber Message_on pc[1], Aot_set/Aot_reset sce 1 Nutzlast):
ROOM1110 msg 0 und ROOM1111 msg 0: genau 1 Stelle, sub02 @0x00D04 `2b 00 ff ff`.
ROOM1230 msg 10 und ROOM1231 msg 10: genau 1 Stelle, sub21 @0x014BC `2b 0a ff ff`.
-> Ein Abfangen von (Raum 1110/1111, msg 0) bzw. (1230/1231, msg 10) trifft NUR die Leiche.
⛔ ROOM1230 msg **0** ist "Enter the first number." (Tastenfeld sub17 @0x013AA) — der Schluessel
muss deshalb (Raum, msg) sein, nie msg allein.

### 3.3 Mechanismus "Angebot wiederholbar bis Annahme" — RE1.5 FUN_8001db28 Zustand 7 (selbst disassembliert)

Arm-Handler des Item-Platzes (AOT-Tabelle Typ 9, LAB_80043328):

    80043328: lui  v0,0x8007
    8004332c: lbu  v0,0x2d3b(v0)        ; Zustand DAT_80072d3b
    80043334: bne  v0,zero,0x80043368   ; laeuft schon -> nichts
    80043338: ori  v1,zero,0x1
    8004333c: lbu  v0,0(a0)             ; Nutzlast[0] = Item-Typ
    80043344: lw   a0,-0x4264(a0)       ; 0x800bbd9c = ausloesender Satz
    8004334c: sb   v1,0x2d3b(at)        ; Zustand = 1
    8004335c: sb   v0,-0x44a(at)        ; 0x800afbb6 = Item-Typ
    80043364: sw   a0,-0x35d0(at)       ; 0x800aca30 = Satz-Zeiger
    80043368: jr   ra

Zustand 7 (Einfuegen / Ablehnen):

    8001e04c: lb   v0,-0x9d4(v0)        ; 0x8008f62c freier Platz (-1 = voll)
    8001e054: bltz v0,0x8001e0ec        ; VOLL  -> Zustand 8 (Item bleibt)
    8001e060: lbu  v0,-0x7ae0(v0)       ; 0x800b8520 Auswahl-Byte
    8001e068: andi v0,v0,0x1            ; "No"
    8001e06c: bne  v0,zero,0x8001e0ec   ; NEIN  -> Zustand 8 (Item bleibt)
    8001e078: lw   v1,-0x35d0(v1)       ; 0x800aca30 Satz
    8001e090: sb   zero,0(v1)           ; NUR bei JA: sce-Byte des Satzes = 0 (Zone aus)
    8001e0c0: lhu  a1,2(s1)             ; Menge
    8001e0c4: jal  0x8004dc4c           ; Einfuegen
    8001e0cc: lhu  a1,4(s1)             ; Zone-9-Bit
    8001e0d0: jal  0x8004ef90           ; a0 = 0x800b0fd6+162 = 0x800b1078 (Bank 9) -> Bit setzen
    8001e0e0: sb   zero,0x2d3b(at)      ; Zustand 0
    8001e0ec..e100: f630 = f634 = 0, Zustand = 8 (Schrumpfen, 17 Bilder, @0x8001e10c ff.)

-> Nur "Ja" schaltet die Zone ab und setzt das Bank-9-Bit; "Nein" und "voll" lassen beides
unberuehrt, das naechste Ausloesen bietet dasselbe Item wieder an. Genau das verlangt der Nutzer
("immer wiederholt ... bis man die Munition annimmt"). Der Port bildet das 1:1 ab
(`item_modal_common.c:334`/`:340`; aot_slot -1 = keine Zone abzuschalten).

### 3.4 RE2-Vorbild "Leiche haelt ein Item" (Werkzeug `r34n_f_zensus.py re2leiche`, RE2 Leon, alle Raeume)

Suche: Item-AOT (0x4E) und Text-AOT (0x2C sce 4) auf DEMSELBEN Rechteck, Text mit corpse/body/dead/holding.
Genau **ein** Treffer — RE2 ROOM4050 (Kanalisation, toter Umbrella-Soldat), sub00:

    @0x00F1A  4e 06 02 31 01 00 74 dc d4 95 7e 09 aa 05 49 00 01 00 be 00 ff 01
              Item_aot_set aot 6, Rechteck (-9100,-27180, 2430x1450), Item 0x49 "Wolf Medal" x1,
              Flag 0xbe, md1 0xff (KEIN Weltmodell), action 0x01
    (Else-Zweig) @0x00F82  2c 06 04 31 01 00 74 dc d4 95 7e 09 aa 05 08 00 00 00 ff ff
              Aot_set aot 6, sce 4 (Text), GLEICHES Rechteck, msg 8 =
              "He's holding something. | I don't need this right now."

Also: RE2 laesst die Leiche das Item ueber den NORMALEN Aufnahme-Weg anbieten (Item-AOT, Frage
"Will you take the ...?"), ohne Weltmodell (md1 = 0xFF), und benutzt fuer "haelt etwas" den Satz
"He's holding something." — der Nutzersatz "He is holding something." ist dessen RE1.5-Form.
⛔ Im RE2-Zweig MIT Item liegt auf dem Rechteck NUR der Item-AOT — kein Text davor; der Satz erscheint dort
nur im Zweig OHNE Item. Die Reihenfolge "erst Text, dann Angebot" hat also kein RE2-Vorbild, sie ist
NUTZER-VORGABE ("…He is Holding something" und dann soll man … Munition erhalten"). Aus RE2 belegt sind
der Aufnahme-Weg (normales Item-Modal), das fehlende Weltmodell und der Wortlaut "holding something".
Der RE2-Item-Handler (AOT-Tabelle @0x800A73C4, Typ 2 -> 0x80051884) zweigt auf action Bit 0:
`lbu v0,7(a0)` @0x800518cc / `andi v0,v0,0x1` @0x800518d4 / `bne` @0x800518d8 -> @0x80051924
`sb 6,-0x403(at)` (0x800cfbfd) statt sofortigem Aufnahme-Start (@0x800518f8 `sb 2 -> 0x800d5c00`).
Die Leichen-Medaille traegt action 1 (eigener Spieler-Ablauf vor der Aufnahme). RE1.5 hat fuer
diese zwei Leichen bereits einen eigenen Spieler-Ablauf: Plc_motion(1,11,0) im Ereignis selbst
(3.1). ⛔ Was Spieler-Routine 6 in RE2 genau zeigt, ist NICHT weiter disassembliert — fuer den
Bau nicht noetig, weil der RE1.5-Ereignisablauf unveraendert bleibt.

RE2 "Sce_Item_get" (Opcode 0x76, Handler 0x800587b8 aus Tabelle @0x800A74C8): liest pc[1] Item,
pc[2] Menge, ruft direkt das Einfuegen `jal 0x80069adc` @0x80058864, pc += 3 — **ohne** Frage.
Fuer "Ja/Nein, bis man annimmt" ist es deshalb NICHT das Vorbild; das ist der Item-AOT-Weg.

**Folgerung Modell:** kein Weltmodell (RE2-Vorbild md1 = 0xFF; RE1.5 hat an beiden Leichen kein Prop).

### 3.5 RE1.5-Vorbild "Text, Ja/Nein, nehmen, danach anderer Text" — ROOM1110 selbst

ROOM1110 sub03 @0x0D24 (Zeitbombe, Platz 20 aus sub00 @0x0CD4, nur solange (3,146) = 0):

    0x0D24 Message_on 2b 08 ff ff      "Will you take the Time Bomb?"  (03 = Ja/Nein)
    0x0D2A Ifel_ck / 0x0D2E Ck 21 0c 1f 00   (12,31) == 0  = "Ja"
    0x0D32 Set 22 03 92 01             (3,146) = 1  (genommen)
    0x0D36 Aot_reset 46 14 00 ...      Platz 20 aus
    0x0D40 Message_on 2b 09 ff ff      "You've taken the Time Bomb."

Das ist RE1.5s eigener Skript-Weg fuer "anbieten, bei Ja merken, danach nie wieder" — er ist
aber ein reiner Text-Dialog ohne Inventar-Eintrag (die Zeitbombe ist kein Item; kein
Item_aot_set, kein Einfuegen). Fuer Munition, die "wie jedes Item" ins Inventar soll, ist das
Aufnahme-Modal (3.3) der richtige Weg; die Zeitbombe belegt nur die REIHENFOLGE Text -> Frage ->
Merkbit in RE1.5 selbst.

### 3.6 Munition: Item-Id und Menge (Werkzeug `r34n_f_zensus.py munition`)

Item-Id **0x15** = "H. Gun Bullets": Namens-Blob des Ports (`inventory_common.c` s_item_names[0x15]
"H. GUN BULLETS", aus DAT_800c4a28 ueber die Offsettabelle DAT_800c495c), gleiche Id wie die
Handgun-Munition im Auslieferungsstand (38 Item_aot_set mit Typ 0x15, z.B. ROOM1110.RDT @0x00B18
`50 07 09 31 00 00 d2 dd 32 00 20 03 20 03 15 00 1e 00 e7 00 01 00`). Munitions-Klasse byte-true
(`sltiu id,0x15` @0x80047d54 / `sltiu id,0x22` @0x80049124).

| Spiel | Saetze H. Gun Bullets | Menge 15 | Menge 30 | andere |
|---|---|---|---|---|
| RE1.5 (Typ 0x15, alle RDTs inkl. Varianten) | 38 | **22** | 16 | – |
| RE2 Leon (Id 0x14, 2553 SCD-Bloecke, 45 desynchron) | 44 | **37** | 6 | 6 (1x) |

Menge **15** = die haeufigste Packung in BEIDEN Spielen (RE1.5 58 %, RE2 84 %). Es gibt kein
RE2-Vorbild "Leiche haelt Munition" (3.4: einziger Leichen-Item-Treffer ist die Wolf Medal).
Beim Aufnehmen halbiert der Port auf Nutzerwunsch jede Welt-Munition (`re15_pickup_menge_nutzer`,
15 -> 7) und stapelt sie (`re15_inv_grant_stapeln_nutzer`) — das gilt hier genauso ("wie bei jedem Item").

### 3.7 Bank 9 (Zone-9-Bits) und Speicherstand (`r34n_f_zensus.py bank9`)

Auslieferungsstand: 85 Bits belegt (Item_aot_set +18, Ck/Set Bank 9; Opcode 0x59 mit Bank 9: 0),
freier Block 53..84 — Bit 61 und 62 frei. Port-Code: nur 53 (Sicherung), 54 (Diary), 55 (Memory
Card), 56 (Granate) (`grep` ueber engine/include). VERTRAG 1.1: 61 = Leiche 1110, 62 = Leiche 1230.
Speicherstand: `re15_savedata.h:131` sichert `flags[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE]`
(16 Baenke x 256 Bit) -> Bank 9 ist im Speicherstand, kein neues Format.

### 3.8 Varianten und Plaetze (`r34n_f_zensus.py slots`)

ROOM1111 / ROOM1231 (Elza-Variante, unterste Hex-Ziffer = RDT-Variante, `re15_room_full_text`):
Leichen-Aot_set und Ereignis bytegleich (1110/1111 sub02 54 B, 1230/1231 sub21 54 B).
Belegte AOT-Plaetze: 1110/1111 0..20, 1230/1231 0..19 (48..63 Kamerazonen). **Kein neuer Platz
noetig** — das Original-Ereignis bleibt der Ausloeser.
Nachrichten: 1110/1111 IDs 0..9, 1230/1231 IDs 0..11 -> Port-IDs 20..23 frei (VERTRAG 1.3).

## 4 Soll-Verhalten (Zeitlinie)

Gemessen als SIMULATION in der Sonde `probe_r34n_f_leiche` (Protokoll `F_belege/probe_r34n_f_leiche.log`):
dort oeffnet die Sonde genau an den geplanten Haken-Stellen den neuen Text und das Modal, ohne Port-Code.
Bildnummern = Spielbilder ab dem Druck (F1). Der Spielerablauf des Originals bleibt unveraendert.

### 4.1 ROOM1110 (und ROOM1111), Bit (9,61) = 0 — Angebot

| Bild | Was passiert | Beleg |
|---|---|---|
| F1 | Quadrat vor der Leiche -> Platz 5 (sce 3, Ereignis 2) -> sub02 startet: Aot_reset(5 aus) @0x0CEE, Work_set, Plc_motion(1,11,0) @0x0CFC, Sleep 30 @0x0D00 | Ist-Messung, unveraendert |
| F31/F32 | Message_on @0x0D04 `2b 00 ff ff` -> **HAKEN**: statt Nachricht 0 oeffnet Port-Nachricht **20** (lang), Maske 0xffff -> Freeze 0xFFFF0000 (wie Original @0x80040508/@0x8004051c); Angebot scharf. Evt_next @0x0D08 -> Faden parkt @0x0D09 | Sonde: `Nachricht AUF`, `Faden @0x00D09` |
| F32..F98 | Seite 1 "It's a police officer, he's dead." (Schreibmaschine 1 Glyphe / 2 Bilder), Pfeil ▼ | Sonde: `Seite 1 fertig` F98 |
| Quadrat | Seite 2 "He is holding something." | |
| Quadrat (F151) | Text zu, Freeze aufgehoben -> **im selben Bild** (re15_game_step, vor dem Freeze-Gate) oeffnet das Aufnahme-Modal: `re15_item_modal_start(0x15, 15, 61, -1, 0xFF)` | Sonde: `Modal auf F151 == zu F151` |
| F151..F178 | Modal: INIT, 17 Bilder Zoom, Gate, 9 Bilder Muenzwurf (FUN_8001db28, `item_modal_common.c`), Bild = ITPS H. Gun Bullets | Port unveraendert |
| ab ~F178 | "Will you take the H. Gun Bullets?" Ja/Nein (Schreibmaschine, Blinkcursor, Ja-Ton RE2 Bank 4/6) | Port unveraendert |
| waehrend des Modals | Faden steht @0x0D09 — der SCD-Takt ist durch das Modal angehalten (Original: g_pauseflags \|= 0xFF000000 @0x8001dbb8/@0x8001dbc8, SCD-Laeufer FUN_8003f038 prueft 0x02000000 @0x8003f044-4c; Port: main.c:5343) | Sonde: `Faden waehrend Modal @0x00D09..@0x00D09` |
| **Nein** / voll | Modal schrumpft 17 Bilder (Zustand 8), Bit bleibt 0, Inventar unveraendert. Danach laeuft der Faden weiter: Plc_motion(1,11,0) @0x0D0A, Plc_flg @0x0D0E, Sleep 30, Plc_ret @0x0D16, Aot_reset(5 wieder scharf) @0x0D18 | Sonde soll-nein: Modal zu F272, Ende F303, scharf 1, Runde 2 identisch |
| **Ja** | Einfuegen: 15 -> 7 (Nutzer-Halbierung), gestapelt auf vorhandene H. Gun Bullets (Sonde 50 -> 57); Bit (9,61) = 1. Danach derselbe Rest des Ereignisses | Sonde soll-ja: Modal zu F245, Bit 1, 50 -> 57, Ende F276 |

### 4.2 ROOM1110, Bit (9,61) = 1 — nach der Annahme (auch nach Laden)

F1 Quadrat -> sub02 wie oben -> Message_on @0x0D04 -> **HAKEN**: Port-Nachricht **21** (kurz) "It's a police
officer, he's dead." (eine Seite, kein Pfeil), KEIN Angebot -> Text zu -> Faden laeuft sofort weiter
(Plc_motion, Re-Arm). Sonde soll-ja Runde 2: 0 Seitenstopps, kein Modal, Ende F132, scharf 1.

### 4.3 ROOM1230 (und ROOM1231)

Identisch mit Platz 18 / sub21 / Bit (9,62): Message_on @0x14BC `2b 0a ff ff` -> Port-Nachricht **22**
(lang: "A miserable death..." ▼ "He is holding something.") bzw. **23** (kurz: "A miserable death...").
Faden parkt @0x014C1, laeuft nach dem Modal mit Plc_motion @0x014C2 weiter, Re-Arm @0x014D0.
Sonde: soll-nein Modal F131..F252, soll-ja F131..F225 Bit 1 50 -> 57, Runde 2 kurz ohne Modal.

### 4.4 Was sich NICHT aendert

Alle anderen Nachrichten beider Raeume (ROOM1110 msg 1..9, ROOM1230 msg 0..9/11 inkl. Tastenfeld und
Kartenleser), die Zeitbombe (ROOM1110 sub03), die Item_aot_set des Evidence Room (u.a. H. Gun Bullets x30
Platz 7 @0x0B18), Kamera, Ereignis-Ablauf, RDT-Bytes (kein Asset-Patch).

## 5 Bauplan

### 5.1 Dateien / Funktionen (alles NEU, eigene Dateien der Spur F)

**`re15_port/include/re15_leiche.h`** — Konstanten (Tabelle 5.3) + API:

    /* Message_on-Haken: uebernimmt (Raum 1110/1111, msg 0) bzw. (1230/1231, msg 10).
     * Rueckgabe 1 = uebernommen (Aufrufer: t->pc += 4; return 1), 0 = nicht zustaendig. */
    int  re15_leiche_message_on(const uint8_t *pc, uint32_t pause_mask);
    /* Je Spielbild in re15_game_step VOR dem Item-Modal-Freeze-Gate: Text zu -> Modal. */
    int  re15_leiche_tick(void);
    /* Fuer Tests: die vier Texte (raum 0x1110/0x1230, kurz 0/1). */
    const uint8_t *re15_leiche_text(uint16_t raum, int kurz, int *out_len);

**`re15_port/engine/src/leiche_1110_1230.c`**:

- Vier `static const uint8_t`-Texte (Bytes = Sonde `k_1110_lang/kurz`, `k_1230_lang/kurz`), JEDES Wort mit
  seiner Fundstelle kommentiert (Muster `tuer1120_1130.c`, Tabelle 3.2).
- `re15_leiche_message_on(pc, mask)`:
  1. `RE15_ROOM_BASE(g_current_room_id)` (`re15_gameflow.h:82`, `id & 0xFFF0`) == 0x1110 und `pc[1] == 0`,
     bzw. == 0x1230 und `pc[1] == 10` — sonst `return 0` (ROOM1230 msg **0** = Tastenfeld bleibt unberuehrt).
  2. `genommen = re15_game_flag_get(9, bit)`; `id = genommen ? kurz : lang`.
  3. `re15_msg_install_text(id, text, n)` + `re15_msg_install_durations(id, re15_msg_compute_duration(...))`
     (wie `tuer1120_1130.c:67-71`) — die Texte werden erst hier installiert, deshalb braucht es KEINEN
     Raumstart-Haken (Tuer- UND Lade-Weg sind automatisch abgedeckt).
  4. Stimme wie der Klartext-Zweig (`scd_queue_voice`, `scd_vm.c:1582`): `scd_audio_event_t` mit
     `kind = SCD_AUDIO_VOICE_ON`, `sample_id = id`, `scd_audio_queue_push` (oeffentlich, `re15_scd.h:746`).
  5. `re15_dialog_open_mask(id, 0, mask)`; `g_scd.message_arg2 = pc[2]; g_scd.message_arg3 = pc[3];`
     (= Klartext-Zweig `scd_vm.c:1816-1818`, nicht blockierend, Freeze aus der Maske).
  6. `s_angebot = !genommen; s_raum = RE15_ROOM_BASE(...); s_bit = bit;` — Logzeile (nur PC):
     `[leiche] ROOM%04X msg %d -> Port-Nachricht %d (Bit (9,%d)=%d)`.
  7. `return 1`.
- `re15_leiche_tick()`:
  `if (!s_angebot) return 0; if (RE15_ROOM_BASE(g_current_room_id) != s_raum) { s_angebot = 0; return 0; }`
  `if (g_scd.message_active || re15_item_modal_active()) return 0;`
  `s_angebot = 0; if (re15_game_flag_get(9, s_bit)) return 0;`
  `re15_item_modal_start(RE15_LEICHE_ITEM, RE15_LEICHE_MENGE, s_bit, -1, 0xFF); return 1;`
  plus Ergebnis-Logzeile nach dem Modal wie `sicherung_1150.c:163-175` (`[leiche] Yes: Bit ... Munition ...` /
  `[leiche] No/voll: ... bleibt bei der Leiche`).

### 5.2 Haken-Zeilen in gemeinsamen Dateien (minimal, je eine Anweisung)

| Datei | Stelle | Zeilen |
|---|---|---|
| `re15_port/engine/src/scd_vm.c` | `op_message_on`, direkt NACH der Logzeile `[msg] room=%04x id=%d savepoint=0` (heute Z. 1680-1681), VOR dem Ja/Nein-Zweig — so bleibt das `RE15_MSG_LOG` mit der ORIGINAL-Id erhalten; Savepoint/Item-Box davor treffen 1110/1230 nie | `if (re15_leiche_message_on(t->pc, pause_mask)) { t->pc += 4; return 1; }` + Kommentar, `#include "re15_leiche.h"` |
| `re15_port/engine/src/game_step_common.c` | direkt NACH `if (c->rdt_ok) re15_granate_tick();` (heute Z. 1050), VOR `if (re15_item_modal_active()) return;` | `if (c->rdt_ok) re15_leiche_tick();` + Kommentar, `#include "re15_leiche.h"` |

Warum genau dort (gemessen, Sonde): Bild-Reihenfolge main.c SCD-Takt (:5378, beim Modal uebersprungen :5343)
-> `re15_msg_tick` (:5503, hier geht der Text zu) -> `re15_game_step` (:7358, hier der Tick) -> Modal-Tick
(:7380). Der Tick sieht den geschlossenen Text also im SELBEN Bild und oeffnet das Modal, bevor der
SCD-Takt des naechsten Bildes den Faden (Plc_motion @0x0D0A) weiterlaufen laesst. Der Faden steht dadurch
das ganze Modal lang auf @0x0D09 / @0x014C1 (Sonde: 0 Abweichungen in 4 Raeumen x 3 Faellen x 2 Runden).
KEIN Haken in `main.c`, `msg_common.c`, `aot_common.c`, `scd_room_setup.c`, `menu_common.c`.

### 5.3 Konstanten-Tabelle

| Konstante | Wert | Beleg @0x… / Datei-Offset bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| Leichen-Platz 1110 / 1230 | Slot 5 / Slot 18 (nur Doku, der Code haengt an der Nachricht) | ROOM1110 main00 @0x00AEE `2c 05 03 31 ... ff 00 18 02 00 00`; ROOM1230 main00 @0x00D52 `2c 12 03 31 ... ff 00 18 15 00 00` |
| `RE15_LEICHE_1110_MSG_ORIG` | 0 | ROOM1110/1111 sub02 @0x00D04 `2b 00 ff ff` — einzige Stelle (msgref-Zensus) |
| `RE15_LEICHE_1230_MSG_ORIG` | 10 | ROOM1230/1231 sub21 @0x014BC `2b 0a ff ff` — einzige Stelle (msgref-Zensus) |
| `RE15_LEICHE_1110_MSG_LANG` / `_KURZ` | 20 / 21 | VERTRAG 1.3 (F: 20..23); ROOM1110 hat IDs 0..9; PSX-Tabelle 32 (`msg_common.c` MSG_TABLE_N), Stimme 0..63 |
| `RE15_LEICHE_1230_MSG_LANG` / `_KURZ` | 22 / 23 | VERTRAG 1.3; ROOM1230 hat IDs 0..11 |
| `RE15_LEICHE_1110_BIT` | 61 | VERTRAG 1.1; Auslieferungsstand frei (85 Bits belegt, Block 53..84 frei, Zensus `bank9`), Port belegt 53..56 |
| `RE15_LEICHE_1230_BIT` | 62 | wie oben |
| `RE15_LEICHE_ITEM` | 0x15 "H. Gun Bullets" | Namens-Blob DAT_800c4a28 (`inventory_common.c` s_item_names[0x15]); 38 Item_aot_set Typ 0x15, z.B. ROOM1110 @0x00B18 `... 15 00 1e 00 e7 00 01 00` |
| `RE15_LEICHE_MENGE` | 15 | PORT-WAHL (keine Original-Leiche mit Munition): haeufigste H.-Gun-Bullets-Packung in RE1.5 (22 von 38) UND RE2 (Id 0x14, 37 von 44), Zensus `munition`. Nutzer-Vorgabe "einmal ... Munition" = eine Packung. Ins Inventar kommen 7 (Nutzer-Halbierung `re15_pickup_menge_nutzer`, 2026-09-20) |
| Modal-Argument aot_slot | -1 | Der Ja-Zweig des Originals nullt die ausloesende Zone (`sb zero,0(v1)` @0x8001e090). Hier ist die Zone das Leichen-EREIGNIS, das nach "Ja" fuer den kurzen Text scharf bleiben MUSS (Nutzer: "Danach soll nur noch ... kommen wenn man ihn anklickt"); das Ereignis legt sich selbst still und wieder scharf (@0x0CEE / @0x0D18, @0x014A6 / @0x014D0). Muster `sicherung_1150.c:201` |
| Modal-Argument taken_prop | 0xFF (kein Weltmodell) | RE2-Vorbild ROOM4050 @0x00F1A md1 = 0xFF (Leiche haelt Wolf Medal ohne Modell); beide RE1.5-Leichen haben kein Prop |
| Modal-Argument taken_bit | 61 / 62 | Original setzt das Zone-9-Bit nur bei Ja (`jal 0x8004ef90` @0x8001e0d0, Bank 0x800b1078) |
| Zeitpunkt des Modals | im Bild, in dem der Text zugeht | NUTZER-VORGABE (Reihenfolge "Text ... und dann ... Munition"). PORT-WAHL der Stelle: der Faden steht dort nach Evt_next @0x0D08 / @0x014C0 auf @0x0D09 / @0x014C1 (gemessen), das Modal friert den SCD-Laeufer ein (@0x8001dbb8 `or v0,v0,t0` t0 = 0xFF000000, @0x8001dbc8 `sw v0,0(t1)` t1 = g_pauseflags; Laeufer-Gate @0x8003f044 `lui v1,0x200` / @0x8003f04c `bne`) -> Ereignis setzt erst nach dem Modal fort |
| Kopf / Seitenumbruch / Ende | `04 02` / `02 00` / `01 00` | msg 0 @0x0D68 / @0x0D8B / @0x0DD3; msg 10 @0x16F4 / @0x170A / @0x1752 |
| "It's a police officer, he's dead." | 33 B (Tabelle 3.2) | ROOM1110.RDT @0x00D6A (wortgleich zum Nutzersatz; Kleinschreibung aus dem Original, L6) |
| "He is holding" | 13 B | ROOM1110.RDT @0x00D8D / ROOM1230.RDT @0x0170C |
| " something" | 10 B | ROOM1110.RDT @0x00EC4 (msg 6) — NUTZER-VORGABE ("something"), Glyphen aus dem Bestand |
| Satzende "." | 0x57 | @0x00DA1 / @0x01720; L8 (97 % aller 1227 Nachrichten enden auf Satzzeichen/Auswahl) |
| "A miserable death..." | 20 B | ROOM1230.RDT @0x016F6; "…" = `57 57 57` (270x im Bestand, 0x "57 57 57 57", 0x Glyphe 0xF2) |
| Texte | 1110 lang 63 B, kurz 37 B; 1230 lang 50 B, kurz 24 B | Zeilen 210/160 px, 129/160 px <= 99 %-Quantil 271 px (Vorschub DEBUG.BIN[0x4416+code]) |

Die Byte-Folgen selbst stehen fertig in `re15_port/tests/unit/probe_r34n_f_leiche.c` (k_1110_lang ...),
von dort 1:1 zu uebernehmen.

## 6 Abnahmeplan

### 6.1 Riegel (ctest, engine-seitig) — `re15_port/tests/unit/test_r34n_f_leiche.c`, registriert in `probes/r34n_f_leiche.cmake`

Aus der Sonde `probe_r34n_f_leiche` wird der Riegel: statt der SIMULIERTEN Haken laufen die ECHTEN
(`re15_leiche_message_on` im VM, `re15_leiche_tick` im Spielschritt). Teile (je ein add_test):

| Teil | Prueft | Nutzerpunkt |
|---|---|---|
| `texte` | die vier Texte sind Byte fuer Byte die Belegstellen der RDTs (ROOM1110 @0x0D68..0x0D8C, @0x0D8D.., @0x0EC4.., ROOM1230 @0x16F4..0x170B) | Wortlaut |
| `nein_1110`, `nein_1230` | Druck -> Nachricht **20/22** (nicht 0/10), 2 Seiten, Modal im Schliess-Bild, Faden steht waehrend des Modals @0x0D09/@0x014C1, "No" -> Bit 0, Inventar gleich, Platz wieder scharf, zweites Untersuchen: wieder Text + Modal | "immer wiederholt ... bis man annimmt" |
| `ja_1110`, `ja_1230` | "Yes" -> Bit (9,61)/(9,62) = 1, H. Gun Bullets +7 (gestapelt), zweites Untersuchen -> Nachricht **21/23**, KEIN Modal | "danach nur noch ..." |
| `voll` | Inventar voll ohne Munition -> "can't carry", Bit 0, wiederholbar | L5 |
| `varianten` | ROOM1111/ROOM1231 identisch | Varianten |
| `laden` | Bit vor dem Raumaufbau gesetzt (Modell eines geladenen Standes) -> kurzer Text, kein Modal; `re15_savedata` Schreiben/Lesen traegt Bit 61/62 (re15_savedata.c:210/:274) | Speichern/Laden |
| `andere` | ROOM1110 msg 1..9 und ROOM1230 msg 0..9/11 oeffnen unveraendert (u.a. sub17 Tastenfeld msg 0, Kartenleser msg 7) | keine Kollision |

GEGENPROBE (Pflicht, im Dossier des Baus festhalten): Haken-Zeile in `scd_vm.c` auskommentieren ->
`nein_*`/`ja_*` ROT (Nachricht 0/10 statt 20/22); Tick-Zeile auskommentieren -> ROT (kein Modal).
Bestehende Riegel muessen gruen bleiben: `integration_keypad` (ROOM1230 Tastenfeld), `unit_item_modal`,
`unit_r33_tuer1120_*` (anderer Text-Platz-Haken), volle Suite (Basis 428 + neue Teile).

### 6.2 Echte exe (`re15_port/tools/r34n_f/r34n_f_lauf.sh`, Bilder ansehen)

| Lauf | Einstellung | Abnahme am Artefakt |
|---|---|---|
| `soll1110_nein` | `1110`, CUT=3, PAD_AT: Druck, 2x Seite, danach nach Prompt-Ende `R` (Nein) + `A` | Framedumps: Seite 1 "It's a police officer, he's dead." ▼, Seite 2 "He is holding something.", Bild der Munition zoomt ein, "Will you take the H. Gun Bullets?" mit Cursor auf No; debug.log `[leiche] ROOM1110 msg 0 -> Port-Nachricht 20`, modal.log Zustaende 1..6 -> 7 -> 8 -> 0; zweiter Druck: wieder dieselbe Folge |
| `soll1110_ja` | wie oben, `A` auf Yes | modal.log 6 -> 7 -> 0, `[leiche] Yes: Bit (9,61)`, danach Druck -> `Port-Nachricht 21`, Bild "It's a police officer, he's dead." ohne Pfeil, KEINE Modal-Zeile; Inventar (Start-Taste) zeigt H. Gun Bullets 57 |
| `soll1230_nein` / `soll1230_ja` | `1230`, CUT=8 | dasselbe mit "A miserable death..." / 22 / 23 / (9,62) |
| `laden_1110` | Karten-Werkzeug `probe_r34n_f_karte` (Muster `probe_r30_granate_karte.c`: Spielstand in ROOM1110 vor der Leiche, mit/ohne Bit 61), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1` | ohne Bit: langer Text + Modal; mit Bit: kurzer Text, kein Modal |
| `durchlauf_1110` | Eintritt ueber die TUER: ROOM1100 main00 @0x009BA Slot 1 (Rechteck (-27330,-11736) 900x1600) -> ROOM1110 Spawn (-2400,0,-5000) Blick 0, Cut 0; von dort nach Osten zur Leiche (Standplatz der Sonde (10330,2650) Blick 0), Weg per RE15_INPUT_SCRIPT im Bau zu vermessen | gleiche Folge wie `soll1110_nein`; beweist den Tuer-Weg (Nachrichtentabelle neu geladen, `re15_msg_clear_room_block`) |
| `durchlauf_1230` | Eintritt aus der GARAGE: ROOM11B0 main00 @0x00F88 Slot 0 -> ROOM1230 Spawn (4725,0,28650) Blick 2048, Cut 8 — 2750 Einheiten noerdlich der Leiche (Mitte (3900,25900)) | gleiche Folge wie `soll1230_nein` |

Gegenproben an der exe: Untersuchen des Kartenlesers/Tastenfelds in ROOM1230 und eines Regals in
ROOM1110 (msg 1) -> Originaltexte; `RE15_FORCE_CUT` NUR fuer die Sicht, ein Lauf ohne CUT pro Raum
(Text/Modal muessen auch in Cut 0 erscheinen, Ist-Lauf `ist1110_a` zeigt den Text in Cut 0).

## 7 Risiken, Softlocks, Wechselwirkungen

| # | Risiko | Einschaetzung / Gegenmittel |
|---|---|---|
| R1 | Zusammenfuehrung: die zwei Haken-Zeilen liegen dort, wo auch andere Spuren einhaengen (op_message_on hinter der `[msg]`-Logzeile; game_step nach `re15_granate_tick`) | Reiner Textkonflikt, fachlich unabhaengig (disjunkte Raeume/Nachrichten). Bei der Integration alle Zeilen behalten, Reihenfolge egal. |
| R2 | Softlock | Keiner: der Faden haengt nur am Text-Freeze und danach am Modal; das Modal endet immer (Zustand 0 bzw. 8 -> 0, `item_modal_common.c`). Kann das Modal nicht starten (nur theoretisch, anderes Modal aktiv), wartet der Tick; das Ereignis laeuft nach dem Modal normal weiter. |
| R3 | ROOM1230 Tastenfeld (Nachricht **0**) | Schluessel ist (Raum, Nachricht); ROOM1230 msg 0 wird nie abgefangen. Riegel `andere` + `integration_keypad`. |
| R4 | Wegwerf-/Tuerton-/Savepoint-Abfangungen im selben op_message_on | Keine hat einen Eintrag fuer (1110, 0) oder (1230, 10) (`gen/discard_sites.inc` nur msg 5/9 in 1230, `gen/lock_se_sites.inc` nur msg 6; kein Savepoint/Item-Box in beiden Raeumen) — der fruehe Ausstieg verliert nichts. |
| R5 | Stimme | Neue IDs -> Dateien `synchro/STAGE1/room1110/main20.wav`, `main21.wav`, `synchro/STAGE1/room1230/main22.wav`, `main23.wav` (Nutzer nimmt auf). Ohne Datei stumm mit Untertitel. Elza-Varianten suchen unter room1111/room1231. |
| R6 | PSX-Ziel | IDs < 32 (MSG_TABLE_N 32), Texte <= 63 B < MSG_RAW_LEN 128; Logzeilen nur unter RE15_PLATFORM_PC. |
| R7 | Menge | Der Spieler bekommt 7 Schuss (15 halbiert) — Folge der Nutzer-Halbierung fuer JEDE Welt-Munition, bewusst nicht umgangen. |
| R8 | Spur E | Marvins Notiz (4312) und Armory Notice (5632) nehmen die Codes auf, die hier aus den Leichen-Texten verschwinden — inhaltlich passend, technisch unabhaengig (andere Raeume, eigene Bits 57..60, eigene IDs in 1000/1010/1020). Die Codes der Tastenfelder selbst aendern sich nicht. |
| R9 | Zwei Untersuchungen schnell hintereinander | Das Ereignis legt Platz 5/18 beim Start still (@0x0CEE / @0x014A6) und erst am Ende wieder scharf — kein Doppel-Ausloesen. |
| R10 | Integrationstests mit echter exe flattern unter Last | Hoechstens EIN neuer exe-Haken; Befund immer einzeln 2x nachfahren (memory reai-v2-gui-tests-flattern-bei-parallelen-agenten). |

## 8 Offene Punkte

1. **Pose bei Plc_motion(1,11,0)** (nicht Spur F): der Port zeigt waehrend der Leichen-Ereignisse keine
   sichtbare Haltungsaenderung (Ist-Bilder F320..F760 gleich). Ob das Original hier eine Pose zeigt, ist nicht
   gemessen. Der Bau aendert das Ereignis nicht; eine eigene Pruefung gehoert in eine andere Runde.
2. **Stimmdateien** main20..23.wav (Texte in 4.1-4.3) nimmt der Nutzer auf; bis dahin stumm mit Untertitel.
3. **Laufwege fuer `durchlauf_1110` / `durchlauf_1230`** (Tuer-Spawns belegt: ROOM1100 @0x009BA -> (-2400,0,-5000),
   ROOM11B0 @0x00F88 -> (4725,0,28650)) sind im Bau per Framedump zu vermessen; die Sprung-Laeufe (6.2) decken die
   Funktion schon ab.
4. **RE2-Spieler-Routine 6** (Item-AOT action Bit 0, @0x80051924) ist nicht weiter disassembliert — fuer
   den Bau nicht noetig (RE1.5-Ereignisablauf bleibt).
5. Keine offene RE-Frage fuer den Bau selbst: Text-Bytes, Nachrichten-Stelle, Faden-Stand, Modal-Weg,
   Item, Menge, Bits, Speicherstand und Varianten sind belegt bzw. gemessen.

## 9 Umsetzung

(Stufe BAU, Runde 34 Nacht, Spur F — Geruest, wird fortlaufend gefuellt)

### 9.1 Dateien
- (offen)

### 9.2 Auflagen der Gegenpruefung (abgehakt / begruendet abgelehnt)
- (offen)

### 9.3 Konstanten und Belege
- (offen)

### 9.4 Sonden/Pins + Mutationsprobe
- (offen)

### 9.5 Suite
- (offen)

### 9.6 Eigene Abnahme an der echten exe (Bilder unter F_belege/)
- (offen)

### 9.7 Abweichungen vom Plan (mit Grund)
- (offen)

### 9.8 Commits
- (offen)

### 9.9 Offene Punkte
- (offen)
