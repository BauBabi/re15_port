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

(in Arbeit)

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die zwei Leichen im Auslieferungsstand (selbst gelesen, `tools/scd_dump_room.py`)

**ROOM1110.RDT** (Evidence Room), main00:

    0x00AEE  Aot_set  2c 05 03 31 00 00 cc 29 66 08 e8 03 e8 03 ff 00 18 02 00 00
             slot 5, sce 3 (Ereignis), sat 0x31, Rechteck (10700,2150, 1000x1000), Nutzlast -> sub02

sub02 @0x0CEE (Ereignis der Leiche):

    0x0CEE  Aot_reset  46 05 00 00 00 00 00 00 00 00   Slot 5 stilllegen
    0x0CF8  Work_set   2e 01 00                         Spieler
    0x0CFC  Plc_motion 3f 01 0b 00                      Leon kniet (Bewegung 11)
    0x0D00  Sleep      09 0a 1e 00                      30 Bilder
    0x0D04  Message_on 2b 00 ff ff                      Nachricht 0, Maske 0xffff (Einfrieren)
    0x0D08  Evt_next   02
    0x0D0A  Plc_motion 3f 01 0b 00                      aufstehen
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

Beide Leichen sind also ein ORIGINAL-Ereignis mit Knien — Leon kniet, der Text friert die Welt ein
(Maske 0xffff, `LAB_800404f4` @0x80040508 `lhu a3,2(v0)` / @0x8004051c `sll a3,a3,16`, Port `scd_vm.c` op_message_on),
danach steht er auf und der Platz wird wieder scharf. Kein Item, kein Flag.

(3.2 ff. in Arbeit)

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
