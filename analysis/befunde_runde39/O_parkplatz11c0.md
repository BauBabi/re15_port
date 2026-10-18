# Runde 39 Spur O "parkplatz11c0" — ROOM11C0: Tueren waehrend des Gorilla-Kampfes gesperrt + Leon-Text

Baum: `.claude/worktrees/r39_parkplatz11c0` (Zweig `r39/parkplatz11c0`, Basis fa2c0e7e = master).
Wortlaut des Nutzers (AUFTRAG.md, Nachtrag 9):
> "Ah, eine Sache fehlt mir noch - bei room11C0, während der Kampf gegen die Gorillas noch läuft, soll bei Leon der Text
> kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tür aus dem Raum raus zu gehen."

Zuteilung (VERTRAG.md): Port-Message-Id 11C0 = 40 (1.2); Bank-9-Bit 96 nur, wenn kein Original-Flag reicht (1.1) —
**nicht gebraucht**: Beginn und Ende des Kampfes tragen Original-Flags (RE-Belege 1).

## Stand
- [x] Messung vorher (echter Weg 11B0 -> Tuer -> 11C0, Szene, Kampf, Tuer)
- [x] RE-Belege (Tuer-AOT, Kampf-Beginn/-Ende im RDT/SCD, Aot_reset/Aot_on/sce-Handler, RE2-Form)
- [x] Umsetzung (Programme SZENE/ENDE + Installer + Stimme, Haken je 1 Zeile)
- [x] Messung nachher (nach1-3 + integration K/W/N/E; Szene/Kampf bitgleich vor/nach)
- [x] Tests (unit 7, integration 4; Modulgruppen; alle unit_/prop_/probe_)

## Messung vorher (Basis fa2c0e7e, exe `re15_pc_r39o_mess.exe` = Kopie der gebauten exe)

Aufbau (echter Weg, kein Raumsprung): Speicherkarte per `probe_r36_schluessel_karte` (Muster
`integration_r36_schluessel`): ROOM11B0 vor der Parkplatz-Tuer (-25480,-26000) Blick 1024, Flags (4,0xF3) Strom,
(3,0x82) Sherry-Szene erledigt, (9,93) Parkplatz-Tuer aufgeschlossen; CONTINUE (RE15_CONTINUE_TEST/RE15_CARD_AUTO),
danach nur `RE15_WEGPLAN` (geschlossen geregelte Pad-Steuerung, engine/src/dachkiste_r36.c). Eingaben und Auszuege:
`O_belege/vor{1,2,3}_*`.

| Lauf | Weg | gemessen |
|---|---|---|
| vor2 | 11B0 AKTION an der Tuer -> 11C0; Szene laeuft ohne Eingabe (msg 0..6); nach der Szene Gegner weg (WEGPLAN `GEGNERWEG`, nur damit Leon lebend zur Tuer kommt — die Flags bleiben im Kampfzustand), Weg zurueck ueber die Plc_dest-Punkte der Szene, AKTION an der Tuer | `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)` -> `loaded room11c0`; `[evt] F6 Evt_exec sub=2 cond=0x0a`; msg 0..6; `[scd F1102] letterbox closed -> gameplay` (= sub02 @0x01ACE, Kampf beginnt); F1115 `AOT slot 0 typ 1 (DOOR) flags 0x31 mitte (-25100,17250) halb (2000,1350)`, FLAG (4,64)=1, (3,67)=0, (7,96)=0, (7,97)=0 (Gorillas leben, Slot 2/3 Typ 0x27). F1739 AKTION bei (-24986,16606) Blick 3047: **`[aot] DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)` -> `loaded room11b0`** — kein Text, Leon verlaesst den Kampf. |
| vor1 | wie vor2, aber MIT lebenden Gorillas zur Tuer | Leon wird unterwegs zerlegt: hp 100 (F1200) -> 88 (F1250) ... -> -2 (F2300), Tod, CONTINUE -> 11B0. Zu Fuss kommt man im Kampf nicht lebend zur Tuer — fuer die Messung nachher braucht es deshalb einen Messhaken, der die Gorillas einfriert (Muster Runde 38 F `RE15_R38F_GORILLAS_STARR`). |
| vor3 | Karte zusaetzlich (4,64)=1 (Ada-Szene gelaufen, Kampf nicht beendet) = Wiedereintritt waehrend des Kampfes (heute moeglich, weil die Tuer offen ist; z. B. alte Spielstaende) | keine Szene; Gorillas neu gesetzt (-9013,-15461) / (9434,2189) grid 0x10 hp 360 (sub00 @0x017E8/@0x017FC), Ada versteckt; F33 `AOT slot 0 typ 1 (DOOR)` — die Tuer geht auch hier. |

**Befund vorher:** Waehrend des Kampfes ist ROOM11C0 Slot 0 eine normale Tuer (Typ DOOR) — Leon kann den Parkplatz
mitten im Kampf verlassen und Ada mit den Gorillas zuruecklassen; keine Sperre, kein Text. Der Raum hat genau EINE Tuer
(Slot 0, s. RE-Belege 1).

## RE-Belege

### 1. ROOM11C0.RDT (`shared_assets/PSX/STAGE1/ROOM11C0.RDT`, main_scd @0x1710, sub_scd @0x1758; `tools/scd_dump_room.py`)
- **Die einzige Tuer:** main00 @0x01712 `3b 00 02 31 00 00 24 96 1c 3e a0 0f 8c 0a 30 9c 00 00 0d 9d fb 0b 00 1b 01 ...`
  = Door_aot_set Slot 0, sce 2, Flags 0x31, Rechteck x -27100..-23100 / z 15900..18600, Ziel (-25552,0,-25331) Blick
  3067, Raum 0x1B (ROOM11B0), Cut 1. Kein weiterer Door_aot_set und kein Aot_reset/Aot_set auf eine Tuer in main00 und
  sub00..sub07 (Dump vollstaendig, 8 Subs). Gemessen identisch (Lauf vor2: einziger DOOR-Slot = 0).
- **Kampf-Beginn = Ada-Szene sub02:** sub01 (laeuft JEDES Spielbild neu, FUN_8003f038 @0x8003f064-80) @0x01820
  `21 04 40 00` Ck(4,64)==0 -> @0x01824 `04 0a 18 02` Evt_exec sub02. sub02 @0x0184E `22 04 40 01` Set(4,64)=1,
  @0x01852 Balken, @0x01856 `22 02 07 01` Pad-Sperre, msg 0..4, @0x01982 `54 00 01 ..` Kampfmusik, Gorillas springen
  herein (Cut 12, For-Schleifen), @0x01A5E `2b 05 00 00` msg 5 "Leon: Ada, you hide inside that patrol car.",
  @0x01A80 `2b 06 00 00` msg 6, @0x01ABA Evt_exec sub07 (Ada in den Wagen), **@0x01ACE `22 02 07 00` Set(2,7)=0 =
  Steuerung zurueck = Kampf laeuft**, @0x01B00 Evt_end.
- **Kampf-Ende:** sub01 @0x0182E `21 03 43 00` Ck(3,67)==0 UND @0x01836 `21 07 60 01` Ck(7,96)==1 UND @0x0183E
  `21 07 61 01` Ck(7,97)==1 -> @0x01842 `04 ff 18 03` Evt_exec sub03. sub03 @0x01B02 `22 03 43 01` Set(3,67)=1,
  @0x01B0A Pad-Sperre, @0x01B0E Musik aus, @0x01B68 `2b 07 00 00` msg 7 "Ada: Okay, it's over now.", msg 8/9, Leon
  RUN ueber 4 Punkte bis (-25180,17380) (= im Tuer-Rechteck), @0x01BE6 Evt_exec sub04 (Ada folgt), **@0x01C08 `47 00`
  Aot_on(0)** = die Tuer feuert, Raumwechsel nach 11B0 (die Szene geht selbst durch die Tuer).
- **Wiedereintritt im Kampf:** sub00 @0x017C8 Else (4,64)==1, @0x017CC/@0x017D0 `21 03 43 00` Ck(3,67)==0 ->
  Ada versteckt (@0x017D4, y -20000), Gorillas @0x017E8/@0x017FC (grid 0x10, Kill-Flag pc[7] = 0x60/0x61), Musik
  @0x01810. Das ist die Original-Bedingung "Kampf laeuft": **(4,64)==1 && (3,67)==0**.
- **Kill-Flags:** Sce_em_set LAB_800420a0 @0x80042120 `lbu a1,7(a1)` (pc[7]), @0x80042128 `beq a1,0xff`, @0x80042130
  `jal 0x8004efe4` (Flag gesetzt -> kein Spawn), @0x80042170 `sb v0,454(s0)` (Entity+0x1C6 = pc[7]). Gorilla-Tod
  STAGE1.BIN @0x8011b8e8 `lbu a1,454(v0)`, @0x8011b8f0 `addiu a0,a0,4152` (0x800b1038), @0x8011b8f4 `jal 0x8004ef90`
  (Bit setzen: @0x8004ef90-efb4 `srl/sll/addu/andi/srlv/or/sw`). Bank im Port `re15_em_status_zone()` = 7 (Stage 1,
  @0x800420ec). Also (7,96)/(7,97) = Gorilla 1/2 tot. ROOM11C0 sub00 @0x01784/@0x01798 tragen pc[7] 0x60/0x61.
- Elza-Variante **ROOM11C1** (`scd_dump_room.py`): main00 @0x016AE dieselbe Tuer, sub00 = 5 Hunde (Typ 0x20), sub01 =
  `01 00`. Keine Gorillas, kein Begleiter -> bleibt unveraendert (der Nutzer spricht nur von Leon).
- Messbar: Lauf vor2 F1115 (Kampf) (4,64)=1, (3,67)=0, (7,96)=(7,97)=0; Lauf vor3 (Wiedereintritt) dieselben Flags.
  -> Bank-9-Bit 96 wird NICHT gebraucht (VERTRAG 1.1: nur wenn kein Original-Flag reicht).

### 2. Die Opcodes (PSX.EXE, `re15_disasm.py dis`)
- **Aot_reset** LAB_80040738: a1 = DAT_800ac9b0[pc[1]] (@0x8004074c-58), @0x80040764 `sb v0,0(a1)` rec[0] = pc[2]
  (sce), @0x80040768-78 rec[1] = (rec[1]&0x80) + pc[3], @0x80040788-a8 Nutzlast +0/+2/+4 = pc[4..9], @0x800407ac
  `addiu v0,a2,10`. Ein Volltausch von sce + 3 Halbworten; der Rest des Satzes (Ziel-Raum/Cut der Tuer) bleibt.
- **sce-Tabelle** PTR_8007469c: [1] = 0x80043084 MESSAGE: @0x80043098 `lhu a3,2(v0)`, @0x8004309c `lhu a2,0(v0)` (msg),
  @0x800430a0 `jal 0x80027e68`, @0x800430a4 `sll a3,a3,16` (Maske). [2] = 0x800430bc TUER: @0x800430c4 `sw a0,-13912(at)`
  (0x800ac9a8 = Nutzlast), @0x800430d4 `sb v0,21337(at)` (0x800b5359 = 1), @0x800430e4 g_pauseflags |= 0xFF000000.
- **Aot_on** LAB_800407bc: @0x8004080c `lbu v0,0(v1)` rec[0], @0x8004082c `jalr v0` = PTR_8007469c[rec[0]] JETZT.
  -> sub03 @0x01C08 `47 00` ruft den Handler, den Slot 0 GERADE hat: ist Slot 0 dann noch der Text, zeigt die
  End-Szene den Text statt hinauszugehen. Die Sperre MUSS vor @0x01C08 zurueck auf die Tuer.
- **Message_on** LAB_800404f4: @0x80040504 `lbu a2,1(v0)` (msg), @0x80040508 `lhu a3,2(v0)`, @0x80040518 `jal 0x80027e68`,
  @0x8004051c `sll a3,a3,16` — derselbe Aufruf wie der MESSAGE-Handler (a1 = 0x300 @0x80040500).
- RE1.5-eigene Form "Tuer -> Text": ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00` (Aot_reset Slot 1 -> sce 1
  msg 15, Maske 0xffff); ebenso ROOM11B1 sub01 @0x011C0 `46 01 01 31 0e 00 ff ff 00 00` (Elzas Parkplatz-Tuer ohne Strom
  -> Text msg 14, je Spielbild aus sub01 unter Ck(4,0xF3)==0 @0x011A0); "Text -> Tuer": ROOM1141 sub05 @0x00FDC
  `46 01 02 31 ..` (Nutzlast = Ziel x/y/z des Door_aot_set). Runde 38 F hat 1141 genau so gesperrt
  (`tools/r38_dining/programme_bauen.py:322-325`, Ruecknahme :352).

### 3. RE2 Retail (Beta -> Retail) — wie RE2 eine Tuer waehrend eines Pflichtkampfes / mit wartendem Begleiter sperrt
Zensus ueber alle 250 RDTs `info/re2leon/PL0/RDT` (eigenes Werkzeug `O_belege/re2_tuer_msg_zensus.py`, Ergebnis
`O_belege/re2_tuer_msg_zensus.txt`, RE2-SCD-Walker `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py`): 54 Stellen in 37 Raeumen,
an denen RE2 eine Tuer durch einen Text ersetzt — ALLE als MESSAGE-AOT (sce 4) auf dem Tuer-Slot/-Rechteck, alle Texte in
Erzaehlform (`fa 02 ...`, kein Sprecherkopf).
- **ROOM7040 (Pflichtkampf):** INIT sub00 @0x010FE Door_aot_set Slot 0 (Nutzlast Ziel `f4 98 00 00 c9 a5`). MAIN sub06
  = Kampf-Beginn-Szene: Pad-Sperre @0x01778, ..., **@0x0185A `46 00 04 31 01 00 00 00 ff ff`** Aot_reset Slot 0 ->
  MESSAGE msg 1 = "I can't put my friends / in danger. [Seite] I have no choice but to / stop that creature here!"
  (@0x01D1A `fa 02 25 00 3f 3d 4a 3a ...`), danach Steuerung zurueck. MAIN sub07 = Kampf-Ende-Szene: **@0x01ACC
  `46 00 01 31 f4 98 00 00 c9 a5`** Aot_reset Slot 0 -> DOOR mit dem eigenen Ziel, @0x01ADF Set(2,7)=0.
  = genau der Ablauf, der hier gebraucht wird: Sperre in der Beginn-Szene VOR der Rueckgabe der Steuerung, Ruecknahme in
  der End-Szene, Text in Erzaehlform (Leons Gedanke, kein "Leon:").
- **ROOM6010 (Begleiter wartet):** MAIN sub00 @0x0068A `2c 05 04 31 00 00 7a ae 5e 9d 08 07 3c 0f 06 00 00 00 ff ff`
  MESSAGE msg 6 "I can't leave Sherry behind!" (@0x015C9 `fa 02 fc 25 00 3f 3d 4a ...`) ueber dem Tuer-Rechteck +
  @0x0069E..@0x006B2 Tueren 0/1/2 -> sce 0. ROOM4040/4050 "I have to catch that woman!" (Aot_reset Tuer -> MESSAGE).

### 4. Textform
- RE1.5-Erzaehlform `04 02 <Zeile> 08 <Zeile> 01 00` — z. B. ROOM1090 msg 7 @0x28E6 `04 02 25 00 49 51 4f 50 ... 08 ...
  1a 01 00` "I must hurry up and get something to / put out this fire to save that woman!" (Leons Gedanke ueber eine
  Frau in Gefahr, Erzaehlform, Ausrufezeichen 0x1A). Runde 38 F hat 1141 msg 41 genauso gebaut (`programme_bauen.py:473`).
  Die Dialogform (`04 00 05 01 Leon 05 00 16 ...`, z. B. ROOM11C0 msg 5 @0x1D96) gehoert den Szenen-Dialogen.
- **Entscheidung:** Erzaehlform wie 1141 msg 41, RE2 ROOM7040 msg 1 / ROOM6010 msg 6 und RE1.5 ROOM1090 msg 7: Leons
  eigener Gedanke an der Tuer, kein Sprecherkopf. "bei Leon der Text" = in Leons Szenario (ROOM11C0; Elza hat ROOM11C1).
  Wortlaut = NUTZER-VORGABE "I cannot leave Ada alone here with those monsters!".

## Umsetzung

### Ablauf (alles an Original-Stellen des Raums, die Szenen selbst laufen UNVERAENDERT aus dem RDT)
| Zeitpunkt | Mechanik | Beleg / Form |
|---|---|---|
| Erster Besuch, Szene beginnt | sub01 @0x01824 Evt_exec sub02 -> Port-Programm **SZENE** = `46 00 01 31 28 00 ff ff 00 00` (Aot_reset Slot 0 -> MESSAGE msg 40, Maske 0xffff) + `18 02` Gosub sub02 + `01 00` | Zeile: ROOM11B0 sub01 @0x01216; Stelle: RE2 ROOM7040 sub06 @0x0185A (in der Kampf-Beginn-Szene vor der Rueckgabe der Steuerung). sub02 sperrt den Pad in DERSELBEN Runde (@0x01856, drittes Opcode) und gibt ihn erst @0x01ACE zurueck — die Sperre vor dem Gosub wirkt wie eine direkt vor @0x01ACE. Gosub-Huelle wie Runde 38 F SZENE1021 |
| Kampf laeuft | Slot 0 = MESSAGE msg 40 (Typ 5 im Port) — Druck an der Tuer zeigt den Text (MESSAGE-Handler 0x80043084 = re15_scd_show_message, Maske 0xffff0000 friert die Welt wie jeder Examine-Text), kein Raumwechsel | Aot_reset LAB_80040738; sce-Tabelle PTR_8007469c[1] |
| Wiedereintritt im Kampf ((4,64)=1, (3,67)=0) | Installer nach dem Init-Lauf: derselbe Aot_reset aus C (`re15_aot_retype(0, 1, 0x31, 40, 0xffff, 0)`), Muster dining_r38.c tuer_1131 | Bedingung = sub00 @0x017C8/@0x017D0 (Original-Zweig "Kampf laeuft weiter") |
| Kampf-Ende | sub01 @0x01842 Evt_exec sub03 -> Port-Programm **ENDE** = `46 00 02 31 30 9c 00 00 0d 9d` (Aot_reset Slot 0 -> TUER, Nutzlast = main00 @0x01712+14) + `18 03` Gosub sub03 + `01 00` | Zeile: ROOM1141 sub05 @0x00FDC; Stelle: RE2 ROOM7040 sub07 @0x01ACC; MUSS vor sub03 @0x01C08 Aot_on(0) stehen (@0x8004082c `jalr` auf den aktuellen Handler) |
| nach dem Kampf | (3,67)=1: kein Eingriff, die Tuer ist die Original-Tuer | sub00 @0x017CC |
| Elza ROOM11C1 | kein Eingriff (Raum-Id 0x11C1, 5 Hunde, kein Begleiter) | ROOM11C1 sub00 @0x016F8.. |
| fremdes RDT | FNV-Anker main00-Tuer (@0x01712, 32 B) + sub00..sub03 (@0x01768, 1188 B) — weicht ein Byte ab, bleibt alles Original | Muster dining_r38.c |
| Stimme | MESSAGE-AOT-Weg (re15_scd_show_message) reiht im Port KEINE Stimme ein (nur Message_on). Haken: fuer ROOM11C0 msg 40 dieselbe Einreihung wie scd_queue_voice -> `synchro/STAGE1/room11C0/main40.wav` spielt nach der Ordnerregel, sobald sie liegt | PORT-WAHL (synchro = eigene Produktion, keine Byte-true-Frage); Messung nachher: `[voice] ROOM11C0 msg 40: keine Aufnahme (synchro/STAGE1/room11c0/main40.wav)` |

Kein Bank-9-Bit (VERTRAG 1.1, 96 bleibt frei): Beginn/Ende tragen Original-Flags (4,64)/(3,67) und die Kill-Bits (7,0x60)/(7,0x61).

### Dateien
- NEU `re15_port/include/re15_parkplatz_r39.h` (alle Konstanten mit @0x-Beleg), `re15_port/engine/src/parkplatz_r39.c`
  (Installer, Evt_exec-Haken, Stimme, Anker, Messhaken).
- GENERIERT `re15_port/engine/src/gen/parkplatz_r39_programme.inc` von `re15_port/tools/r39_parkplatz/programme_bauen.py`
  (prueft 17 Byte-Stellen des RDT, dass es genau EINE Tuer gibt und kein Aot_reset/Aot_set auf eine Tuer, baut SZENE/ENDE
  mit Listing, den Text mit Breitenpruefung 202/139 px <= 271 und die FNV-Anker; `--pruefen` vergleicht nur).
- HAKEN (je 1 Zeile, Kommentar "Runde 39 Spur O"): `engine/src/scd_vm.c` op_evt_exec (hinter dem Runde-38-F-Haken) und
  re15_scd_show_message (vor re15_dialog_open_mask); `engine/src/scd_room_setup.c` Installer am Blockende;
  `platform/pc/main.c` Boot-/CONTINUE-Installer (hinter re15_inventar_r39_install) + Messhaken (hinter
  re15_dining_r38_messhaken). Je ein `#include`.
- Messhaken (env-gegatet, kein Spielverhalten): `RE15_R39O_GORILLAS_STARR=1` (Gorillas im Kampf einfrieren, grid|0x20 =
  Member_set 0x0C=0x30 @0x800410b8 wie sub02 @0x01A58), `RE15_R39O_GORILLAS_TOT=<n>` (nach n freien Kampfbildern die
  Gorillas heraus + genau ihre Kill-Bits (7,0x60)/(7,0x61), STAGE1.BIN @0x8011b8e8-f4). Muster Runde 38 F.

### Konstanten (include/re15_parkplatz_r39.h)
| Konstante | Wert | Beleg |
|---|---|---|
| RE15_R39O_MSG_SPERRE | 40 | VERTRAG 1.2 (11C0 Id 40; Original 0..9, Port 10..14 = irons_tod_1150.c) |
| RE15_R39O_TUER_SLOT / _ZIEL_RAUM | 0 / 0x1B | main00 @0x01712 pc[1] / +23 |
| RE15_R39O_ZIEL_X/Y/Z | -25552 / 0 / -25331 | main00 @0x01712+14 `30 9c 00 00 0d 9d` |
| RE15_R39O_SAT / _MASKE | 0x31 / 0xFFFF | main00 @0x01715; ROOM11B0 sub01 @0x01219 / @0x0121C |
| RE15_R39O_SCE_MESSAGE / _TUER | 1 / 2 | PTR_8007469c[1] = 0x80043084, [2] = 0x800430bc |
| RE15_R39O_SUB_SZENE / _ENDE | 2 / 3 | sub01 @0x01824 `04 0a 18 02` / @0x01842 `04 ff 18 03` |
| RE15_R39O_ADA_BANK/BIT | 4 / 64 | sub02 @0x0184E, sub01 @0x01820 |
| RE15_R39O_ENDE_BANK/BIT | 3 / 67 | sub03 @0x01B02, sub01 @0x0182E, sub00 @0x017D0 |
| RE15_R39O_KILL_BANK / _1 / _2 | 7 / 0x60 / 0x61 | sub01 @0x01836/@0x0183E; sub00 @0x01784+7/@0x01798+7 |
| RE15_R39O_GORILLA_TYP | 0x27 | sub00 @0x01784 pc[2] |
| Text msg 40 | `04 02 25 00 3f 3d 4a 4a 4b 50 ... 08 53 45 50 44 ... 1a 01 00` | NUTZER-VORGABE; Form ROOM1090 msg 7 @0x28E6 |

## Messung nachher (Stand 6bbc46d0 + Riegel, exe `re15_pc_r39o_mess.exe`; Auszuege `O_belege/nach*`)
| Lauf | Aufbau | gemessen |
|---|---|---|
| nach1 | wie vor2, aber Gorillas LEBEND (RE15_R39O_GORILLAS_STARR=1), Ton-Mitschnitt RE15_AUDIO_CAP_SYNC (Stimmen laufen), Kampfende RE15_R39O_GORILLAS_TOT=1100 | F6 `[parkplatz] ... Evt_exec sub02 -> Programm SZENE`; msg 0..6 mit main00..06.wav; F1107 Steuerung zurueck; F1120/F1740/F1930 `AOT slot 0 typ 5 ev 40` (Text statt Tuer), (4,64)=1, (3,67)=0. F1741 AKTION bei (-25016,16556) Blick 3095: `[parkplatz] F1741 ROOM11C0 Tuer im Kampf -> msg 40 ...`, `[voice] ROOM11C0 msg 40: keine Aufnahme (synchro/STAGE1/room11c0/main40.wav)`; Zustands-Log `msg(a=1 fsm=1 id=40` F1742..F1867 (zweiter Druck F1804 schaltet weiter, dritter F1867 schliesst); **kein DOOR FIRE, kein Raumwechsel**. Danach GEHE (-20000,12000) angekommen F2050 = **Leon steuerbar**. F2207 Messhaken Kill-Bits -> `Evt_exec sub03 -> Programm ENDE` im selben Bild, msg 7/8/9 (main07/08 + main09), `DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)` -> `loaded room11b0` = **nach dem Kampf geht die Tuer** (die Ende-Szene geht selbst hindurch). |
| nach2 | GENAU vor2 (Plan + Env RE15_NOAUDIO=1), neuer Stand | Szene identisch: `Evt_exec sub=7` F1088, `letterbox closed` F1102 (vor2: F1088/F1102). **state.txt 11C0 F1..F1738 bitgleich mit vor2 (1738/1738 Bilder)**; F1739 derselbe Druck -> statt `DOOR FIRE slot=0` jetzt `Tuer im Kampf -> msg 40`; der Lauf bleibt danach im Raum (die Gorillas sind dort per GEGNERWEG weg, ihre Kill-Bits aber nicht gesetzt — der Kampf ist laut Original-Flags nicht vorbei, die Tuer bleibt zu; abgebrochen nach 15 min). |
| nach3 | GENAU vor1 (Gorillas leben, Leon geht zu Fuss und stirbt unterwegs) | **state.txt bitgleich mit vor1, 2746/2746 Bilder** (Szene, Gorilla-Verhalten, Treffer, Tod, CONTINUE): Kampf und Gorillas unveraendert. |
| int K/W/E | Riegel integration_r39_parkplatz11c0_k/w/e (s. Tests) | s. Tests |

Bewertung: waehrend des Kampfes zeigt die (einzige) Tuer den Text, Leon bleibt im Raum und steuerbar; nach dem Kampf geht
die Tuer (die Original-Ende-Szene nimmt sie selbst, beim naechsten Besuch ist sie normal, Riegel `nachher`); Szene, Kampf
und Gorillas sind Bild fuer Bild unveraendert (nach2/nach3); Elza unberuehrt (Riegel `elza`, integration E).

## Tests

Riegel (registriert NUR in `re15_port/tests/unit/probes/r39_parkplatz11c0.cmake`):
| Test | misst | Ergebnis |
|---|---|---|
| unit_r39_parkplatz11c0_texte | msg 40 dekodiert = "I cannot leave Ada alone here with those monsters!"; `04 02 .. 1a 01 00`, genau ein 0x08, kein 0x05/0x16 (kein Sprecherkopf); breiteste Zeile 202 px <= 271; keine Ja/Nein-Frage | gruen |
| unit_r39_parkplatz11c0_programme | SZENE-/ENDE-Bytes; Zeilenform = ROOM11B0 sub01 @0x01216; ENDE-Nutzlast = main00 @0x01712+14; alle Kopf-Konstanten gegen die RDT-Bytes (sub00/01/02/03, main00); Anker; Evt_exec-Haken nur 11C0 sub02/sub03 | gruen |
| unit_r39_parkplatz11c0_kampf | ECHTE Szene ab dem Tuer-Spawn (-25279,17268) Blick 0x11FB: Bild 1 = SZENE, Slot 0 Text, (4,64)=1 + Pad-Sperre in derselben Runde; durchgehend Text bis zur Rueckgabe (Bild 1043); 2 Gorillas; Druck an der Tuer -> msg 40, Stimme eingereiht, kein Raumwechsel; danach 30 Bilder UP: 2022 Einheiten = steuerbar; zweiter Druck -> wieder msg 40; Kill-Bits -> sub03 = ENDE im selben Bild, Slot 0 = Tuer -> 0x1B (-25552,0,-25331), (3,67)=1, msg 7, Raumwechsel ROOM11B0 (Aot_on(0) @0x01C08) | gruen |
| unit_r39_parkplatz11c0_wiedereintritt | (4,64)=1/(3,67)=0: Slot 0 VOR dem ersten Bild Text (Installer), 2 Gorillas neu, keine Szene; Druck -> msg 40; Ende wie oben | gruen |
| unit_r39_parkplatz11c0_nachher | (3,67)=1: Tuer normal, keine Gorillas, Druck -> ROOM11B0, kein Text | gruen |
| unit_r39_parkplatz11c0_elza | ROOM11C1 mit (4,64)=0 und =1: Tuer, kein Eingriff, keine Gorillas/Ada, Druck -> ROOM11B1 | gruen |
| unit_r39_parkplatz11c0_fremd | 1 Byte in sub02 geaendert: Anker greift nicht, keine Sperre, Original-Subs | gruen |
| integration_r39_parkplatz11c0_k | echte exe, Karte 11B0 -> Tuer -> 11C0, Szene, Kampf (Gorillas eingefroren), Tuer -> msg 40 (`Tuer im Kampf -> msg 40`, Zustands-Log `msg(a=1 fsm=1 id=40` > 20 Bilder, Stimme angefragt `synchro/STAGE1/room11c0/main40.wav`), kein DOOR FIRE vor dem Kampfende, weg von der Tuer (steuerbar), Kampfende -> ENDE, msg 7, `DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)`, `loaded room11b0`; 5 Framedumps | gruen (131 s) |
| integration_r39_parkplatz11c0_w | echte exe, Wiedereintritt im Kampfzustand: Installer-Zeile, `AOT slot 0 typ 5 ev 40`, Druck -> msg 40, kein DOOR FIRE, EXIT_AT in 11C0 | gruen (41 s) |
| integration_r39_parkplatz11c0_n | echte exe, nach dem Kampf ((4,64)=1, (3,67)=1): `AOT slot 0 typ 1`, kein `[parkplatz]`, Druck -> `DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)`, ROOM11B0 | gruen (128 s unter Last) |
| integration_r39_parkplatz11c0_e | echte exe, ELZA 11B1 -> 11C1: `AOT slot 0 typ 1`, kein `[parkplatz]`, Tuer -> zurueck nach 11B1 | gruen (34 s) |

Bild: `O_belege/nach_k_msg40_F1865.png` (Framedump RE15_WINDOW_SCALE=1, Lauf integration K: Leon an der Parkplatz-Tuer,
Textfenster "I cannot leave Ada alone here / with those monsters!"), `nach_k_msg40_F1775.png` (Schreibmaschine laeuft).

Gezielte Laeufe (Spurbaum, KEINE volle Suite):
- `ctest --test-dir re15_port/build -R "integration_r39_parkplatz11c0" --timeout 1800` -> `100% tests passed, 0 tests failed out of 3`
  (K/W/E; Teil N danach einzeln: `-R "^integration_r39_parkplatz11c0_n$"` -> `100% tests passed, 0 tests failed out of 1`)
- Raum 11C0 an der exe (Montage 1150 -> 11C0, Szene 11C0, Parkplatz-Tuer 11B0 -> 11C0, Gorilla-Schatten):
  `-R "^integration_r37_szene11c0$|^integration_r36_schluessel$|^integration_r39_gegner_schatten$|^integration_r35_cut1150$|^integration_r36_szene1150$|^integration_r38_montage$"`
  -> `100% tests passed, 0 tests failed out of 6`
- Modulgruppen (unit), `-R "r39_parkplatz11c0|^unit_r35_affen_|^unit_r36_karte_tuer11c0$|^unit_r35_cut1150_|^unit_r36_szene1150_|^unit_r37_szene11c0_|^unit_r38_montage_|^unit_r39_gegner_|^unit_r36_schluessel_|^unit_r38_dining1141_|^unit_r39_funk10f1_|^unit_r39_stimmen_|^unit_scd_|^unit_aot_|^unit_msg_dialog$|^unit_door_seq$|^unit_synchro_stimme$|^unit_voice_ausreden$|^unit_se_voice_machine$|^prop_scd_pc_advance$|^unit_maggot_|^unit_r38_affen3_|^unit_r38_griff_gorilla$|^unit_r36_zombieki_affen" -E "^integration_" -j 6`
  -> `100% tests passed, 0 tests failed out of 126` (darin unit_r35_affen_szene = Szenen-Gang bitgleich mit der GDB-Spur des
  Originals: die Gosub-Huelle aendert die Szene nicht).
- Querbezug vor dem Abschluss (Memory reai-v2-gezielte-integration-querbezug), ALLE unit_/prop_/probe_:
  `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j 6 --timeout 900` -> `100% tests passed, 0 tests failed out of 844`
  (875 s).

## OFFEN
- Keine offenen Punkte dieser Spur.
- Hinweis an Spur J / Orchestrator (nicht Spur O): ROOM1141 msg 41 ("I cannot leave him alone with those monsters.",
  Runde 38 F) laeuft ebenfalls ueber den MESSAGE-AOT-Weg, der im Port KEINE Stimme einreiht (scd_vm.c
  re15_scd_show_message, nur Message_on ruft scd_queue_voice). Eine kuenftige `synchro/STAGE1/room1141/main41.wav` bliebe
  dort stumm; fuer 11C0 msg 40 ist das mit `re15_parkplatz_r39_stimme` geloest (gemessen: die Datei wird angefragt).

## Fuer den Nutzer
- **Sprachdatei:** `synchro/STAGE1/room11C0/main40.wav` = Leon: "I cannot leave Ada alone here with those monsters!"
  (Ordnerregel main<MESSAGE-ID>.wav, Message-Id 40). Sie spielt, sobald sie dort liegt — nichts einzutragen (gemessen: der
  Port fragt beim Text genau diese Datei an, heute `[voice] ROOM11C0 msg 40: keine Aufnahme ... - Zeile bleibt stumm`).
- **Neue Assets fuer das Paket-Gate:** keine (Text und Programme stehen im Code, `engine/src/gen/parkplatz_r39_programme.inc`).
- **So sieht es aus:** nach "Leon: Ada, you hide inside that patrol car." / "Ada: O-okay, I'll do that..." ist die
  Parkplatz-Tuer gesperrt; wer sie benutzt, bekommt das Textfenster "I cannot leave Ada alone here / with those monsters!"
  (Leons Gedanke, ohne "Leon:"-Kopf — dieselbe Form wie Elzas "I cannot leave him alone with those monsters." in ROOM1141
  und wie RE2s Tuertext im Pflichtkampf "I can't put my friends in danger..."), Leon bleibt im Raum und frei steuerbar.
  Sind beide Gorillas tot, laeuft die Original-Szene ("Ada: Okay, it's over now." ...) und geht selbst durch die Tuer;
  danach ist die Tuer wieder normal. Elzas Parkplatz (ROOM11C1, Hunde) ist unveraendert.
- **Alte Spielstaende:** wer in einer frueheren Fassung mitten im Kampf hinausgegangen ist, findet beim Wiederbetreten den
  Kampf wie im Original weiterlaufend vor (Gorillas, Ada im Wagen) — jetzt mit gesperrter Tuer bis zum Kampfende.
- Keine Entscheidungsfragen.

## Nachbesserung 1 (nach Abnahme 0, `O_abnahme_0.md`; Basis HEAD 72c76540)

Stand:
- [x] M1 Messung vorher (exe mit der Spur-A-Karte; Riegel-Teil `ende_gesetzt` am alten Stand rot)
- [x] M1 RE-Beleg (Freigabeweg sub01 @0x0182E/@0x01842, Setzer von (3,67)/(4,64) im RDT-Zensus)
- [x] M1 Aenderung (Programm SZENE nur bei (3,67)==0)
- [ ] M1 Messung nachher (unit, integration Teil I, `integration_r39_stimmen_11b0`)
- [x] Hinweise H1/H2/H3 der Abnahme eingeordnet (H2 als Riegel `echt` uebernommen)
- [ ] gezielte Tests + alle unit_/prop_/probe_

### M1 — Programm SZENE sperrte auch bei schon gesetztem (3,67)

**Ursache.** `re15_parkplatz_r39_evt_exec` (engine/src/parkplatz_r39.c) lieferte fuer sub02 das Programm SZENE ohne Blick
auf (3,67). Die Sperre hat genau EINE Freigabe: sub01 @0x0182E `21 03 43 00` Ck(3,67)==0 -> Kill-Bits -> @0x01842
`04 ff 18 03` Evt_exec sub03 = Programm ENDE. Ist (3,67) beim Start der Ada-Szene schon 1, greift sie nie mehr: Slot 0
(die einzige Tuer des Raums) bleibt fuer den ganzen Besuch Text. Die eigene Definition "Kampf laeuft" ((4,64)=1 &&
(3,67)=0, sub00 @0x017C8/@0x017D0; `re15_parkplatz_r39_kampf`, Installer) galt fuer SZENE nicht. Den Zustand (4,64)=0 /
(3,67)=1 erreicht man nur ueber Spielstaende/Debug-Flags (Zensus unten) — genau so startet der master-Riegel
`integration_r39_stimmen_11b0` (Spur A, test_r39_stimmen.cmake:97 Karte `3:0x43 3:0x82 4:0xf3`, :98
`RE15_FIRE_AOT=0@60#11c0`).

**Messung vorher** (HEAD 72c76540, exe 23:19 = Code 27d587e1):
- exe, Karte und Env des Spur-A-Riegels, nur `RE15_EXIT_AT=300#11c0` statt `30#1260` (`O_belege/nb1_vor_m1_exe.txt`,
  16 s): `[parkplatz] F0 ... Programm SZENE`, `[fire-aot] slot=0 at F60`, `F60 ROOM11C0 Tuer im Kampf -> msg 40`,
  state.log F61..F299 `msg(a=1 fsm=1 id=40) pf=FFFF0000` (die Maske des Textes friert auch die Szene ein), kein
  `DOOR FIRE`, `EXIT_AT: Bild 300 in Raum 11c0` — dasselbe Bild wie die beiden 600-s-Abbrueche der Abnahme.
- neuer Riegel-Teil `unit_r39_parkplatz11c0_ende_gesetzt` gegen den alten Engine-Code (`O_belege/nb1_vor_m1_unit.txt`):
  **7 Fehler** — Programm SZENE trotz (3,67)=1, Slot 0 schon in der Szene Text, Aot_on(0) in Bild 60 ohne Raumwechsel,
  nach dem Tod beider Gorillas Slot 0 weiter Text (kein ENDE: Ck(3,67)==0 falsch), Druck -> msg 40 statt ROOM11B0.

**Beleg** (ROOM11C0.RDT selbst gelesen, `tools/scd_dump_room.py`):
```
sub01 @0x0181C 06 00 0a 00  Ifel_ck
      @0x01820 21 04 40 00  Ck(4,64)==0
      @0x01824 04 0a 18 02  Evt_exec sub02        <- Haken: Programm SZENE (Sperre)
      @0x01828 08 00        Endif
      @0x0182A 06 00 1e 00  Ifel_ck
      @0x0182E 21 03 43 00  Ck(3,67)==0          <- der EINZIGE Weg zur Freigabe
      @0x01832 06 00 14 00  Ifel_ck
      @0x01836 21 07 60 01  Ck(7,96)==1
      @0x0183A 06 00 0a 00  Ifel_ck
      @0x0183E 21 07 61 01  Ck(7,97)==1
      @0x01842 04 ff 18 03  Evt_exec sub03        <- Haken: Programm ENDE (Freigabe)
sub00 @0x017C8 07 00 52 00  Else_ck / @0x017CC 06 00 48 00 / @0x017D0 21 03 43 00 Ck(3,67)==0  = "Kampf laeuft"
sub02 @0x0184E 22 04 40 01  Set(4,64)=1          sub03 @0x01B02 22 03 43 01  Set(3,67)=1
```
Zensus aller 240 RDTs (`O_belege/nb1_zensus_3_67_4_64.txt`, 0 Walk-Abbrueche): (3,67) setzt nur ROOM11C0 sub03
@0x01B02, (4,64) nur ROOM11C0 sub02 @0x0184E; gelesen werden sie nur in 11C0 (@0x0176C, @0x017D0, @0x01820, @0x0182E)
und in ROOM11B0 @0x011CE `21 03 43 01` (Ck(3,67)==1, Schlussszene 11B0); kein Opcode loescht sie, kein Port-Code setzt
sie (grep `flag_set(3, 67|0x43)` / `(4, 64|0x40)` in engine/platform/include: 0 Treffer). Im Original laeuft im
Zustand (4,64)=0/(3,67)=1 sub02 an und sub03 nie — die Tuer bleibt die Original-Tuer (main00 @0x01712; im Raum kein
Aot_reset/Aot_set auf Slot 0).

**Aenderung** (`engine/src/parkplatz_r39.c` `re15_parkplatz_r39_evt_exec`): der Zweig sub02 liefert Programm SZENE nur
bei (3,67)==0; bei (3,67)=1 das Original-sub02 und die Logzeile `ROOM11C0 sub01 @0x01824 Evt_exec sub02 bei (3,67)=1 ->
keine Sperre (Freigabe sub01 @0x0182E Ck(3,67)==0 greift nie), Original-sub02`. Nach sub02 @0x0184E Set(4,64)=1 ist das
genau "Kampf laeuft" (sub00 @0x017C8/@0x017D0) = `re15_parkplatz_r39_kampf()` und Installer — EINE Bedingung fuer alle
drei Stellen. Keine neue Konstante (RE15_R39O_ENDE_BANK/_BIT = sub03 @0x01B02 / sub01 @0x0182E, schon belegt).
Kopfkommentar `include/re15_parkplatz_r39.h` und Generator-Docstring nachgezogen; das .inc bleibt bitgleich
(`programme_bauen.py --pruefen` -> ok).

**Randzustand (gleiche Klasse, ebenfalls nur per Spielstand):** (4,64)=0, (3,67)=0, Kill-Bits (7,0x60)/(7,0x61) = 1.
sub01 startet SZENE (@0x01824) und ENDE (@0x01842) im SELBEN Durchlauf. Beide landen im ersten freien Event-Slot ab 10
(scd_vm.c op_evt_exec, cond >= 10) und laufen noch im selben Bild in Slot-Reihenfolge (scd_vm_tick: "PSX does NOT defer
spawned threads", Evt_exec @0x8003f2a0 / scd_execute_scripts @0x8003f0a0) -> die Freigabe (Slot 11) laeuft nach der
Sperre (Slot 10). Gemessen (Pruefstand, dann Riegel `kills_gesetzt`): Bild 0 SZENE 1 / ENDE 1, Slot 0 am Bildende Tuer,
msg 7, Raumwechsel ROOM11B0 Bild 876, kein msg 40 — keine Dauersperre, keine zusaetzliche Bedingung noetig.

**Messung nachher:** s. unten (Tests Nachbesserung 1).

### Hinweise der Abnahme (kein Mangel)
- **H1** Kill-Bit-Zeitpunkt im Port (Leichen-Zustand 7, enemy_ai_common.c run_all) 71 Bilder nach dem Original-Todesbild
  (STAGE1.BIN @0x8011b7c8/@0x8011b8f4): Gegner-KI, nicht Spur O -> OFFEN (an Orchestrator/Spur E). Wirkung in 11C0, im
  Riegel `echt` gemessen: Tod Bild 150, Kill-Bits Bild 221, ENDE Bild 222 — so lange zeigt die Tuer nach dem Tod des
  zweiten Gorillas noch msg 40, die Original-Ende-Szene beginnt ~2,4 s spaeter.
- **H2** Riegel ohne echten Abschuss: uebernommen als `unit_r39_parkplatz11c0_echt` (Pruefstand der Abnahme 2.3):
  Pistolen-Treffer mit dem Schreibsatz des Resolvers (Waffe 3, Schaden Zeile 0x27 @0x8006ee38 + 3*4 = @0x8006ee44 = 12,
  in PSX.EXE selbst gelesen), Kill-Bits NUR vom Todes-Commit der Engine -> ENDE -> msg 7 -> ROOM11B0.
- **H3** Schreibmaschine wird durch einen Druck nicht beschleunigt: allgemeine Text-FSM (re15_scd_show_message ->
  re15_dialog_open_mask), nicht Spur O -> OFFEN (Hinweis).
