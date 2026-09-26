# Aufnehmen-"Yes" soll den normalen Bestaetigungssound spielen — die Id ist belegt (CORE-Bank 4, Satz 6, @0x8004a51c), RE1.5 selbst ist an dieser Stelle aber nachweislich STUMM; die Regel dafuer liefert RE2 @0x80030924-54

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Beim yes fuer das aufnehmen von Items/Munitions moechte ich den normalen
Bestaetigungssound."

Ergebnis: Die Id des "normalen Bestaetigungssounds" ist belegt (§2.1/§2.2). Der
Aufnahme-Yes-Pfad des Originals ist NACHWEISLICH STUMM — gemessen ueber einen
vollstaendigen Se_on-Zensus der EXE (41 Aufrufe), aller acht Overlays und des
Rueckruf-Zweigs, den das Skript beim Ja tatsaechlich nimmt (§2.2-§2.6). Der Port
ist damit heute byte-true; der Wunsch ist eine Nutzer-Entscheidung, deren
Mechanismus aber in RE2 byte-belegt existiert (§2.7).

## 2. Das Original — mit Adressen und Bytes

### 2.1 Der SE-Spieler und die Id-Zerlegung — Se_on = FUN_80045024

`info/Re1.5/PSX.EXE`, RAM = 0x80010000 + Dateioffset - 0x800 (Header selbst gelesen:
`PS-X EXE`, pc=0x80054448, t_addr=0x80010000, t_size=0xaf000, Datei 718848 Byte).

    80045024: addiu sp,sp,-88
    80045028: srl   v1,a0,24        ; v1 = BANK  (Byte 3 des Arguments)
    80045058: lui   at,0x800b
    8004505c: addiu at,at,8684      ; 0x800b21ec  = VAB-Handle je Bank
    80045060: addu  at,at,v1
    80045064: lb    a1,0(at)
    80045068: addiu v0,zero,-1
    8004506c: beq   a1,v0,0x8004539c ; Handle -1 -> Bank nicht geladen -> raus
    80045078: srl   v0,a0,16
    8004507c: andi  s4,v0,0xff      ; s4 = SATZ/RECORD (Byte 2)
    80045080: andi  a0,a0,0xff      ; Byte 0 = positional/Emitter-Wahl
    80045088: addiu at,at,9624      ; 0x800b2598 = EDT-Basiszeiger je Bank (x4)
    80045094: sltiu v0,v1,0x6       ; Bank muss < 6 sein

Damit ist `Se_on(0x04NN0000)` = **Bank 4, Satz NN, nicht-positional** (Byte0 == 0).
Bank 4 ist im Port `re15_audio_core_se` (`include/re15_audio.h:139`,
Implementierung `platform/pc/src/audio_pc.c:958`).

### 2.2 Vollzensus aller Se_on-Aufrufe der EXE (selbst gescannt)

Roh-Scan ueber `info/Re1.5/PSX.EXE` nach dem Wort `0x0C011409` (= `jal 0x80045024`):
**41 Aufrufstellen**. Davon benutzen **genau acht** die Bank 4:

| Aufruf @ | `lui a0` @ | Argument | Bedeutung |
|---|---|---|---|
| 0x800185ec | 0x800185e4 | 0x04080001 (`ori a0,a0,0x1`) | Treffer-SE, positional (a1 = &sp[16]) |
| 0x8004a158 | 0x8004a154 | 0x04090000 | Inventar OEFFNEN (SELECT-Edge `andi 0x100` @0x8004a140) |
| 0x8004a47c | 0x8004a478 | 0x04040000 | Cursor-Blip |
| 0x8004a4a4 | 0x8004a4a0 | 0x04040000 | Cursor-Blip |
| 0x8004a4cc | 0x8004a4c8 | 0x04040000 | Cursor-Blip |
| 0x8004a4f4 | 0x8004a4f0 | 0x04040000 | Cursor-Blip |
| **0x8004a520** | **0x8004a51c** | **0x04060000** | **BESTAETIGUNG** |
| 0x8004a664 | 0x8004a660 | 0x04050000 | Abbruch |

Der Bestaetigungs-Zweig im Kontext (selbst disassembliert, Rohbytes daneben):

    8004a504: lui   v0,0x800b
    8004a508: lw    v0,-14484(v0)   ; 0x800ac76c = VIRTUELLE Pad-Edge
    8004a510: andi  v0,v0,0x4000    ; v0x4000 = Menue-Confirm (Type A = Quadrat)
    8004a514: beq   v0,zero,0x8004a648
    8004a51c: lui   a0,0x406        ; Bytes 06 04 04 3c  -> a0 = 0x04060000
    8004a520: jal   0x80045024      ; Bytes 09 14 01 0c
    8004a524: ori   a1,zero,0x0     ; Bytes 00 00 05 34

Gegenprobe Abbruch @0x8004a660 Bytes `05 04 04 3c` (= `lui a0,0x405`),
Cursor @0x8004a4f0 Bytes `04 04 04 3c` (= `lui a0,0x404`).

→ **Der "normale Bestaetigungssound" ist CORE-Bank 4, Satz 6,
`Se_on(0x04060000)`, belegt an @0x8004a51c-24.**
(Das aeltere Dossier `analysis/confirm_cancel_mapping.md` §2 nennt dieselben
Adressen, zitiert sie aber nur — hier sind sie roh nachgelesen.)

### 2.3 Der Ja/Nein-Zweig der MSG-Maschine ist STUMM — FUN_80028134

Select-Confirm der allgemeinen Nachrichten-FSM (das ist der Zweig, der beim "Yes"
laeuft), vollstaendig selbst disassembliert:

    80028564: lui   v1,0x800b
    80028568: lw    v1,-14484(v1)   ; 0x800ac76c virtuelle Edge
    80028570: andi  v0,v1,0x4000    ; Bytes 00 40 62 30
    80028574: beq   v0,zero,0x800285b8
    8002857c: lui   v1,0x800c
    80028580: addiu v1,v1,-31456    ; 0x800b8520 (Auswahl-Byte)
    80028588: sw    v0,-14488(at)   ; 0x800ac768 = 0xffff (Held-Wort verbrennen)
    8002858c: lbu   v0,0(v1)
    80028594: lw    a0,-31428(a0)   ; 0x800b853c = Pauseflag-Schnappschuss
    80028598: andi  v0,v0,0x7f      ; Bit 7 loeschen = Auswahl beendet
    8002859c: sb    v0,0(v1)
    800285a4: sw    a0,-13760(at)   ; g_pauseflags zurueck
    800285a8: jal   0x8002877c      ; Bytes df a1 00 0c
    800285b0: j     0x80028758

**Kein `jal 0x80045024` im ganzen Zweig.** Das Original spielt beim Bestaetigen einer
Ja/Nein-Abfrage keinen Laut.

### 2.4 Auch die Aufnahme-Praesentation ist stumm — FUN_8001db28

Alle `jal`-Ziele im Bereich 0x8001db28..0x8001e1b0 (Zustandsmaschine der
Item-Aufnahme, Zustandsbyte DAT_80072d3b, 9er-Sprungtabelle @0x800106b4, selbst
gelesen @0x8001db54-6c) — roh gescannt:

    0x8001e1c8 x2, 0x80027e68 x2, 0x80029a98, 0x8004dc4c, 0x8004df2c, 0x8004ee78,
    0x8004ef90, 0x80068348, 0x80068948 x2, 0x80068a60, 0x80068bf4, 0x80068c88,
    0x8006b3d8, 0x8006b538 x4, 0x8006b5ec x2, 0x8006b6a0

**0x80045024 ist NICHT darunter.** Die komplette Aufnahme-Praesentation (Zoom, Flip,
Prompt, Insert) loest keinen SE aus. Ebenso enthalten FUN_80027e68
(0x80027e68..0x80028134) und FUN_80028134 (bis 0x80028840) keinen Se_on-Aufruf;
ihre einzigen Ziele sind 0x80028134/0x8002918c/0x80029560/0x800c004c bzw.
0x800279c8 x3 / 0x8002877c / 0x80028840 / 0x80028868 / 0x800c69bc + ein `jalr`
@0x80028828 (der Rueckruf aus §2.5).

### 2.5 Wer beim JA der Aufnahme-Abfrage ueberhaupt gerufen wird — FUN_8002877c

Die MSG-FSM ruft nach dem Bestaetigen `jal 0x8002877c` (@0x800285a8). Diese
Routine loest den Ja/Nein-VERZWEIG im Skript auf:

    8002877c: addiu sp,sp,-24
    80028784: addiu a1,a1,-31444   ; a1 = &DAT_800b852c (Skript-Cursor)
    8002878c: lw    v1,0(a1)       ; P
    80028794: addiu v1,v1,1        ; P+1
    800287a8: lbu   v0,-31456(v0)  ; DAT_800b8520 = Auswahl-Byte
    800287ac: lbu   a0,0(a0)       ; a0 = Skript[P+1] = Sprungweite fuer "No"
    800287b0: andi  v0,v0,0x1      ; 0 = Yes, 1 = No
    800287b4: mult  a0,v0
    800287bc: addu  v0,v0,v1       ; Cursor = P+1 + weite*choice
    800287d4: lbu   v1,0(...)      ; naechstes Steuerbyte
    800287e0: ori   v0,zero,0xf8
    800287e4: beq   v1,v0,0x80028830   ; 0xF8 -> raus
    800287e8: sltiu v0,v1,0xf9
    800287ec: bne   v0,zero,0x80028830 ; < 0xF9 -> raus
    800287f0: ori   v0,zero,0xf9
    800287f4: bne   v1,v0,0x80028830
    80028800: lw    v0,-31444(v0)  ; Cursor
    80028808: lbu   v0,0(v0)       ; Rueckruf-INDEX
    80028810: sll   v0,v0,2
    80028818: addiu at,at,12084    ; Tabelle 0x80072f34
    80028820: lw    v0,0(at)
    80028828: jalr  v0

Das ausgelieferte Aufnahme-Skript (Skript [0] in
`re15_port/engine/src/gen/item_prompt_data.inc`, erzeugt aus
`shared_assets/PSX/BIN/DEBUG.BIN`, Tabelle @0x800c4fc6, Skript-Offset 0x10) endet
mit den Bytes

    1b   03 02   f9 00   01
    '?'  YesNo / Weite=2  Rueckruf 0  END

Yes (choice 0) landet also auf `f9 00` → Rueckruf-Index 0; No (choice 1)
ueberspringt 2 Bytes und landet auf `01` = END, ohne Rueckruf. Die Tabelle
@0x80072f34 selbst, roh gelesen:

    0x80072f34: 0x80021f6c   <- einziger gueltiger Codezeiger
    0x80072f38: 0x00000000
    0x80072f3c: 0x000345e0
    0x80072f40: 0x00000069

**Der Ja-Zweig der Aufnahme ruft FUN_80021f6c.** Diese Funktion reicht von
0x80021f6c bis 0x80022000 (naechster Prolog `addiu sp,sp,-56` @0x80022004). Ihr
einziger Unterruf ist `jal 0x8004ef90` (@0x80021fd8 — der Zonen-Flag-Setzer,
denselben ruft auch das Modal @0x8001e0d0); danach `sh v0,0x800b0fd6`
(@0x80021fec) und `jr ra` (@0x80021ffc). **Kein Se_on.**

### 2.6 Gesamtergebnis fuer das Original

CORE-Satz 6 wird in RE1.5 an GENAU DREI Stellen gespielt — alle drei sind
Menue-Bestaetigungen, keine davon liegt im Aufnahme-Pfad:

| Binaerdatei | Adresse | Kontext |
|---|---|---|
| PSX.EXE | 0x8004a51c (`06 04 04 3c`) | Inventar-Top-Menue, Zweig `andi 0x4000` @0x8004a510 |
| DEBUG.BIN | 0x80106484 | Item-Raster, Zweig `andi v1,0x4000` @0x80106478 |
| TITLE.BIN | 0x80102ac4 | Titel-Untermenue-Auswahl |

Gegenprobe: der Titel-HAUPTeintrag benutzt Satz **0** — `lui a0,0x400`
@0x80102c20 hinter `andi 0x8f0` @0x80102c14. Satz 0 ist also der Titel-Ton, nicht
"die Bestaetigung".

Roh-Zensus ueber alle Overlays (`info/Re1.5/PSX/BIN/*.BIN`, Code @0x80100000,
kein Header-Offset) — Anzahl `jal 0x80045024` / davon Bank 4 / benutzte Saetze:

    DEBUG.BIN  23 / 22   0x404 x4, 0x406 @80106484, 0x40a, 0x405 …
    STAGE1.BIN 11 /  4   0x401 @8010a7a8, 0x403 @8010a854/@80111de4/@8011c4a8
    STAGE2.BIN  4 /  2   0x401 @8010a63c, 0x403 @8010a6e8
    STAGE3.BIN 16 /  7   0x401, 0x403 x3, 0x400, 0x402, 0x407
    STAGE4.BIN 12 /  7   0x401 x2, 0x403 x3, 0x400
    STAGE5.BIN 19 /  9   0x401 x2, 0x403 x3, 0x400, 0x402, 0x407
    STAGE6.BIN  0 /  0
    TITLE.BIN   4 /  4   0x406 @80102ac4, 0x404 x2, 0x400 @80102c20

**Kein einziges Stage-Overlay spielt Satz 6.**

→ **Das Original spielt beim JA der Aufnahme-Abfrage NICHTS.** Das ist gemessen
(41-Aufrufe-Zensus der EXE + 8 Overlays + der Rueckruf-Zweig), nicht angenommen.

### 2.7 RE2 hat die Regel, die der Nutzer will — FUN_80030844

RE2s Nachrichten-/Auswahl-FSM ist FUN_80030844 (`info/re2leon/PSX.EXE`, RAM =
0x80010000 + Dateioffset - 0x800; Prolog `addiu sp,sp,-112` @0x80030844;
Aufrufer @0x80030760 / @0x8003078c). Ihr Bestaetigungs-Zweig ist strukturgleich
zu RE1.5s @0x80028564 — **aber er spielt einen Laut, und zwar auswahl-abhaengig**:

    800308c8: andi  v0,v1,0x1000        ; RE2-VIRTUELLE Bestaetigung (v0x1000)
    800308cc: beq   v0,zero,0x80030964
    800308e4: ori   v0,zero,0xffff      ; Held-Wort verbrennen (= RE1.5 @0x80028578)
    800308ec: sw    v0,-7412(at)        ; 0x800ce30c
    800308f8: andi  v0,v0,0x7f          ; Auswahl-Bit 7 loeschen (= RE1.5 @0x80028598)
    80030904: sw    v1,-1060(at)        ; 0x800cfbdc Pauseflags zurueck (= RE1.5 @0x800285a4)
    80030908: lhu   v0,23720(s3)        ; Bytes 96 a8 5c 62
    80030910: andi  v0,v0,0x2000        ; Bytes 30 42 20 00
    80030914: bne   v0,zero,0x80030924  ; Bit 0x2000 -> immer Laut
    80030918: andi  v0,s1,0x60          ; sonst: Freigabe-Bits der Nachricht (s1 = a2)
    8003091c: beq   v0,zero,0x80030b68  ; keine Freigabe -> STUMM
    80030924: lbu   v0,23692(s3)        ; Auswahl-Byte
    8003092c: andi  v1,v0,0xf           ; gewaehlter Index
    80030930: beq   v1,zero,0x80030948  ; Bytes 10 60 00 05
    80030934: addiu v0,zero,1
    80030938: beq   v1,v0,0x80030954    ; Bytes 10 62 00 06   (Index 1 …)
    8003093c: lui   a0,0x405            ; Bytes 3c 04 04 05  -> ABBRUCH-Ton
    80030940: j     0x80030954          ; (Index >= 2 …)
    80030944: lui   a0,0x406            ; Bytes 3c 04 04 06  -> BESTAETIGUNG
    80030948: andi  v0,s1,0x20          ; Bytes 32 22 00 20   (Index 0)
    8003094c: bne   v0,zero,0x80030b68  ; Bit 0x20 unterdrueckt den Ja-Ton
    80030950: lui   a0,0x406            ; Bytes 3c 04 04 06  -> BESTAETIGUNG
    80030954: jal   0x8005ba28          ; Bytes 0c 01 6e 8a = RE2s Se_on
    80030958: addu  a1,zero,zero

**RE2-Regel, byte-belegt: Auswahl-Index 0 ("Yes") → `Se_on(0x04060000)`,
Index 1 ("No") → `Se_on(0x04050000)`, Index >= 2 → 0x0406.** Dieselbe Bank und
dieselben Satznummern wie RE1.5s Inventar; RE2 spielt Bank 4 an 54 Stellen
(u.a. Cursor @0x8006b580, Bestaetigung @0x8006b5b8, Abbruch @0x8006b5d0,
Bestaetigung @0x8006ac40 / @0x8006fc34 / @0x80072110 …). Zwei weitere
RE2-Belege in derselben Funktion: der Listen-Blip @0x80030968 (`lui a0,0x404`)
und die Aktions-Bestaetigung @0x8002652c (`lui a0,0x406`) im Zweig, der die
Pauseflags mit 0xff000000 einfriert (@0x8002650c-20) — dasselbe Freeze-Muster wie
RE1.5s Item-Modal @0x8001dbb8/@0x8001dbc8.

### 2.8 Der Laut ist in den ausgelieferten Daten vorhanden

`re15_port/shared_assets/PSX/SOUND/CORE*.EDH`: alle 20 Baenke haben
`pbav = 0x40` (u32 @Dateiende-8) → **16 EDT-Saetze**; Satz 6 existiert ueberall.
CORE00.EDH Datei-Offset 0x18: `00 00 73 01` (Programm 0x73); Satz 5 @0x14
`00 00 63 01`; Satz 4 @0x10 `00 00 53 00`. `re15_audio_core_se(6)` ist also zu
jedem Zeitpunkt spielbar (der Port laedt CORE00 als Vorgabe,
`platform/pc/src/audio_pc.c:965`).

## 3. Was der Port tut — mit datei.c:zeile

### 3.1 Aufnahme-Abfrage — STUMM (byte-true zum Original)

`re15_port/engine/src/item_modal_common.c`, Zustand 6 (`case 6`):

    283:  s_blink = re15_msg_select_blink_tick(s_blink, (pad_edge & 0x3000) != 0);
    284:  if (pad_edge & 0x3000) s_choice ^= 1;
    285:  if (!(pad_edge & 0x4000)) return;      <-- BESTAETIGUNG, kein Laut
    286:  s_msg_no = s_choice;
    287:  s_prompt = 0; s_state = 7; again = 1; break;

Die Datei bindet `re15_audio.h` gar nicht ein (Include-Liste Z.16-23). Auch der
Ausloeser ist stumm: `engine/src/aot_common.c:1332-1344` (`RE15_AOT_TYPE_ITEM`)
ruft nur `re15_item_modal_start(...)`; der Kommentar dort sagt ausdruecklich
"The pickup SE is the room's own SCD Se_on". Einziger SE in aot_common.c ist der
Cursor-Klick `re15_audio_core_se(4)` @Z.482.

### 3.2 Wegwerf-Abfrage — ebenfalls stumm (Frage (d): ja, dasselbe gilt dort)

`re15_port/engine/src/item_discard_common.c`:

    309:  s_blink = re15_msg_select_blink_tick(s_blink, (pad_edge & 0x3000) != 0);
    310:  if (pad_edge & 0x3000) s_choice ^= 1;
    316:  if (pad_edge & 0x8000) { s_choice = 1; }
    317:  else if (!(pad_edge & 0x4000)) return;   <-- BESTAETIGUNG, kein Laut
    319:  if (s_choice == 0) { ... wegwerfen ... }

Auch hier kein `re15_audio.h` (Include-Liste Z.9-16).

### 3.3 Allgemeine SCD-Ja/Nein-Abfrage — stumm

`re15_port/engine/src/msg_common.c:555-558` (FSM-Case 3) — kein Laut, byte-true
zu @0x80028564-b0. Kein `re15_audio.h` (Include-Liste Z.11-14).

### 3.4 Wo der Port den Laut HEUTE schon richtig spielt

| Ort | Zeile | Id |
|---|---|---|
| Inventar-Menue Bestaetigung | `menu_common.c:203/304/316/326/365` | `se4(6)` |
| Inventar-Menue Abbruch | `menu_common.c:332/336/377/778/1446` | `se4(5)` |
| Item-Box | `re15_itembox.c:409/432/464` | `bse(6)` |
| Memory-Card-Schirm | `platform/pc/main.c:1616` | `re15_audio_core_se(6)` |
| OPTIONS | `platform/pc/main.c:2565` | `re15_audio_core_se(6)` |

Die Id 6 ist im Port also bereits ueberall die Bestaetigung — es fehlen genau die
drei Abfragen aus 3.1/3.2/3.3.

## 4. Der Unterschied, in einem Satz

Der Port ist an dieser Stelle byte-true (RE1.5 spielt beim Ja der Aufnahme
nachweislich nichts); was der Nutzer will, ist die RE2-Regel aus FUN_80030844
@0x80030924-54 — Auswahl-Index 0 → `Se_on(0x04060000)`, Index 1 →
`Se_on(0x04050000)` — mit genau der Satznummer, die RE1.5 selbst @0x8004a51c als
Bestaetigung benutzt.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

⛔ Kennzeichnung fuer die Code-Kommentare: Die **Satznummern** 6/5 sind
byte-belegt in RE1.5 (@0x8004a51c bzw. @0x8004a660). Der **Mechanismus**
"Abfrage-Bestaetigung macht ueberhaupt einen Laut" ist byte-belegt in **RE2**
(@0x80030924-54), NICHT in RE1.5 — an RE1.5 gemessen ist es eine
Nutzer-Entscheidung. Jeder Kommentar muss das so ausschreiben, wie es
`aot_common.c:452-476` fuer den Cursor-Klick vormacht.

### P1 — Aufnahme-Abfrage (das, was der Nutzer verlangt)

`re15_port/engine/src/item_modal_common.c`
- Include ergaenzen (bei Z.23):
  `#include "re15_audio.h"   /* re15_audio_core_se — CORE-Bank 4 (Se_on 0x04NN0000 @0x80045028/7c) */`
- Zustand 6, zwischen heutiger Z.285 und Z.286:

      if (!(pad_edge & 0x4000)) return;
      /* NUTZER-ENTSCHEIDUNG + RE2-Regel FUN_80030844 @0x80030924-54:
       * Index 0 (Yes) -> Se_on(0x04060000) @0x80030950,
       * Index 1 (No)  -> Se_on(0x04050000) @0x8003093c.
       * Satznummern byte-belegt in RE1.5 selbst: Bestaetigung @0x8004a51c
       * (lui a0,0x406), Abbruch @0x8004a660 (lui a0,0x405).
       * RE1.5 ist an DIESER Stelle stumm (Zensus: analysis/befunde_2026-09-26/
       * pickup-bestaetigungssound.md §2.2-2.6). */
      re15_audio_core_se(s_choice == 0 ? 6 : 5);
      s_msg_no = s_choice;

- Der "can't carry"-Zweig (Z.277-279) hat keine Auswahl; RE2 spielt dort mit
  Index 0 den Satz 6 (@0x80030950). Gehoert zu P1:
  `if (!(pad_edge & 0xc000)) return; re15_audio_core_se(6);`

### P2 — Wegwerf-Abfrage

`re15_port/engine/src/item_discard_common.c`
- Include ergaenzen (bei Z.14).
- Nach heutiger Z.317, vor Z.319:

      re15_audio_core_se(s_choice == 0 ? 6 : 5);   /* wie P1, RE2 @0x80030924-54 */

  Das greift auch fuer den CROSS-Abbruch in Z.316: der setzt `s_choice = 1` und
  faellt in dieselbe Zeile, spielt also 0x0405 — genau RE2s Index-1-Zweig
  @0x8003093c.

### P3 — allgemeine SCD-Ja/Nein-Abfrage (gleiche Klasse, gleicher Ton)

`re15_port/engine/src/msg_common.c:555-558` →

      if (act_edge) {
          re15_audio_core_se(g_scd.message_choice == 0 ? 6 : 5);  /* RE2 @0x80030924-54 */
          re15_game_flag_set(12, 31, g_scd.message_choice);
          g_scd.message_fsm = 6;
      }

  Dafuer `#include "re15_audio.h"` in msg_common.c ergaenzen.

### P4 — Testhaken (kein Bau in diesem Lauf)

`re15_port/tests/test_support.c:65` fuehrt bereits `g_test_core_se_last` und
`g_test_core_se_count` (`re15_audio_core_se` ist dort der Test-Stub). Ein
Sonden-Test kann damit ohne Audio pruefen: Aufnahme-Modal bis Zustand 6 ticken,
`pad_edge = 0x4000` → `g_test_core_se_last == 6`; mit vorherigem
`0x1000`-Toggle → `== 5`. Muster wie `tests/unit/probe_1090_flame_touch.c:44`.

## 6. Offen / nicht belegt

1. **RE2s Freigabe-Bits `s1 & 0x60` (@0x80030918) und `s1 & 0x20` (@0x80030948)
   sind nicht aufgeloest.** `s1 = a2` kommt vom Aufrufer (`lbu a2,1(s0)`
   @0x80030734), also aus einem Nachrichten-Deskriptor. Ich habe NICHT
   nachgewiesen, dass RE2s Aufnahme-/Wegwerf-Nachricht diese Bits gesetzt hat —
   es kann also sein, dass RE2 ausgerechnet dort ebenfalls stumm ist. Der
   Umsetzungsplan spielt bewusst unbedingt; das ist die Nutzer-Entscheidung, nicht
   eine RE2-Messung. Naechster Weg: die Deskriptor-Tabellen @0x8009dee4 /
   @0x8009df10 (6 Byte je Eintrag, Index `(a2 & 0xf)*6`, gebildet @0x80030744-5c
   bzw. @0x80030770-88) dumpen und Byte 1 je Eintrag lesen.
2. **Der Kontext-Gate `lhu 23720(s3) & 0x2000` (@0x80030908-14)** ist nicht
   aufgeloest (s3 = a0 der FSM = eine Laufzeitstruktur, kein statisches DAT_).
3. **RE1.5s Rueckruf-Indizes 1 und 2** (Skripte "place"/"use" enden auf `f9 01`,
   "discard" auf `f9 02`) zeigen in der Tabelle @0x80072f34 auf `0x00000000`
   bzw. auf Nicht-Code (`0x000345e0`). Diese Prompts sind im Auslieferungsstand
   also tot — passt zum bekannten Befund, dass die Wegwerf-Abfrage in RE1.5 nie
   verdrahtet wurde. Fuer diesen Report ohne Belang, deshalb nicht weiterverfolgt.
4. **Nicht gehoert:** In diesem Durchgang war kein Bau/Lauf erlaubt, der Ton
   wurde also nicht abgespielt. Dass Satz 6 spielbar IST, ist ueber die
   EDH-Satzzahl belegt (§2.8), nicht ueber Hoeren.
