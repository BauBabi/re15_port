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
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

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
  msg 15, Maske 0xffff); "Text -> Tuer": ROOM1141 sub05 @0x00FDC `46 01 02 31 ..` (Nutzlast = Ziel x/y/z des
  Door_aot_set). Runde 38 F hat 1141 genau so gesperrt (`tools/r38_dining/programme_bauen.py:322-325`, Ruecknahme :352).

### 3. RE2 Retail (Beta -> Retail) — wie RE2 eine Tuer waehrend eines Pflichtkampfes / mit wartendem Begleiter sperrt
Zensus ueber alle 250 RDTs `info/re2leon/PL0/RDT` (eigenes Werkzeug `O_belege/re2_tuer_msg_zensus.py`, Ergebnis
`O_belege/re2_tuer_msg_zensus.txt`, RE2-SCD-Walker `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py`): 53 Stellen,
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
(offen)

## Messung nachher
(offen)

## Tests
(offen)

## OFFEN
(offen)

## Fuer den Nutzer
(offen)
