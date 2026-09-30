# Spur F — Leichen ROOM1110 / ROOM1230: Untersuchen-Text + einmal Handgun-Munition

Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code). Zweig `r34n/leichen`, Baum `.claude/worktrees/r34n_leichen`.
Stand: Abschnitte 1 und 3 (Teil) gefuellt, Rest in Arbeit.

## 0 Kurzfassung

(in Arbeit)

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
| L3 | "einmal Handfeuerwaffen Munition" | EINE Packung Item **0x15 "H. Gun Bullets"**, nur ein einziges Mal je Leiche (einmal = einmalig, nicht "eine Patrone"). Menge: Abschnitt 3.4. | Namens-Blob Abschnitt 3.4. |
| L4 | "bis man die Munition annimmt" / "zum mitnehmen erscheinen" | Das normale Aufnahme-Modal des Ports (Bild zoomt ein, "Will you take the H. Gun Bullets?" Ja/Nein) — dasselbe wie bei jedem Item (FUN_8001db28-Port `item_modal_common.c`). "annehmen" setzt eine Wahl voraus (Ja/Nein); "erscheinen zum mitnehmen" = das Item-Bild erscheint. | Wortlaut; Auftrag Spur F ("Aufnahme-Abfrage wie bei jedem Item, Ja/Nein"). |
| L5 | Nein / Inventar voll | Beides: Munition bleibt bei der Leiche, das naechste Untersuchen zeigt WIEDER den langen Text und WIEDER das Angebot. | "Der Text soll immer wiederholt werden können ... bis man die Munition annimmt." Voll = nicht angenommen (Modal Zustand 8, Item bleibt, `item_modal_common.c:334`). |
| L6 | Gross-/Kleinschreibung "Police", "Holding" | Die WORTGLEICHEN Teile kommen als ORIGINAL-BYTES aus der RDT: "It's a police officer, he's dead." (klein, ROOM1110 msg 0 Seite 1), "He is holding" (klein, beide RDTs Seite 2), "A miserable death..." (ROOM1230 msg 10 Seite 1). Neu ist nur "something". | Auftrag Spur F: "Wortgleiche Teile aus den Original-Bytes uebernehmen; Nutzer-Schreibung sonst zeichengetreu". Die Grossbuchstaben stehen bei WORTGLEICHEN Teilen (Nutzer zitiert den Original-Satz), "something" ist klein geschrieben. |
| L7 | Seitenumbruch zwischen den zwei Saetzen | Wie im Original: `02 00` (Seite fuer Seite, Taste blaettert). | Der Original-Satz hat genau dort `02 00` (ROOM1110 @0x0D8C, ROOM1230 @0x1708). Der Nutzer schreibt alles in eine Zeile, weil er den Text abtippt; die Seitenfolge ist Teil der "wortgleichen" Original-Bytes. |
| L8 | Punkt am Ende von "He is Holding something" (1110 ohne, 1230 mit) | In BEIDEN Raeumen mit Punkt 0x57. | (a) Derselbe Satz steht in 1230 beim Nutzer MIT Punkt; (b) der ersetzte Original-Satz "He is holding a slip." endet in beiden RDTs auf 0x57; (c) Zensus Abschnitt 3.2: Anteil der ausgelieferten Nachrichten, die ohne Satzzeichen enden. |
| L9 | "…." (Auslassung + Punkt) in 1230 | "A miserable death" + **drei** 0x57 wie im Original, KEIN vierter Punkt. "…" = RE1.5-"..." = `57 57 57`. | Der Teil ist wortgleich zum Original (`1d 00 49 45 4f 41 4e 3d 3e 48 41 00 40 41 3d 50 44 57 57 57` @0x16F6); der Nutzer schreibt den Nachtext "A miserable death…" mit genau EINER Auslassung; der Punkt hinter "…" ist sein Satztrenner beim Abtippen zweier Seiten in einer Zeile. Zensus 3.2: vier Punkte in Folge kommen im Auslieferungsstand nicht/als Ausnahme vor. |
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

(in Arbeit)

## 5 Bauplan

### 5.1 Dateien / Funktionen

(in Arbeit)

### 5.2 Haken-Zeilen in gemeinsamen Dateien

(in Arbeit)

### 5.3 Konstanten-Tabelle

| Konstante | Wert | Beleg @0x… / Datei-Offset bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| (in Arbeit) | | |

## 6 Abnahmeplan

(in Arbeit)

## 7 Risiken, Softlocks, Wechselwirkungen

(in Arbeit)

## 8 Offene Punkte

(in Arbeit)
