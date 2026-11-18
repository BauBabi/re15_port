# Runde 42 "story" (v0.8.31) — Vertrag der Story-Ressourcen (verbindlich)

Stand 2026-11-14. Basis: Zweig `r42/story` (= r41/integration mit den Alligator-Fixes + Gorilla-Filme 114/11C, f049dfe89),
Arbeitsbaum `.claude/worktrees/r41_sichel`. Grundlage: Story-Konzept
`C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md` (Nutzer 08.11.: "Ja, rein story technisch ist
der Rest fantastisch."), Korrekturen K-01..K-08 (Abschnitt 0) BINDEND. Aufnahmeliste fuer den Nutzer:
`C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/AUFNAHMELISTE.md`.

Diese Datei ist der Vertrag fuer ALLE Spuren der Runde 42: Bank-9-Bits und Nachrichten-Ids werden NUR von hier genommen.
Werte im Code: `re15_port/include/re15_story_r42.h`. Eine Quelle der Zuteilung + Pruefung:
`re15_port/tools/r42_story/zensus_r42.py` (Ausgabe `belege/zensus_r42.txt`: ERGEBNIS FREI, 181 Zeilen, 27 Bits, 0 Fehler).
Seit dem Bau der Stufe STAGE2 nutzt der Port die Vertrags-Bits selbst: der Zensus weist Stellen, deren Bit-Argument ein
`RE15_R42_*`-Makro ist, als EIGEN aus (Zeile "EIGEN ..." unter dem Bit, auch die Wrapper-Stellen `bit9_setzen()`); B2 zaehlt
nur fremde Stellen, B6 prueft, dass jede eigene Stelle ein Vertrags-Bit ist (Stand 2026-11-14: 118..122 EIGEN in story2_r42.c; seit Stufe STAGE1 2026-11-15 auch 118/120 in story1_r42.c).
Riegel: `tests/unit/test_r36_kartenbits.c` k_port[] traegt die 23 Bits (ohne Lageplaene), `unit_r36_kartenbits` gruen
(85 Port-Bits gegen 85 Original-Bits: 0 Kollisionen, `belege/unit_r36_kartenbits_r42.txt`).

## 0. Entscheide (Orchestrator, Empfehlungen des Konzepts uebernommen)

- F-03 ja: John begruesst Sherry in 2081 zum ersten Mal (2081 msg 49).
- F-19: Gravur der Taschenuhr bei Leon "For Sherry. Love, Mom." (Beschreibungstext, keine Nachricht).
- F-26: Rueckgabe der Uhr in 11A1 (msg 6..9, nur mit Uhr).
- F-21: Raketenwerfer wirft bei Leon Marvin, bei Elza John (Finale kommt spaeter; noch keine Zeilen).
- F-24: Umfang v0.8.31 = Block A + Block B + Texte aus Block C (Konzept Abschnitt 7 Punkt 11: 6030 L21, 6031 E6, E9).
  Block C uebrig = v0.8.32; Ids/Bits sind hier schon zugeteilt und die Stimmen stehen in der Aufnahmeliste.
- Implizit nach Empfehlung (nicht auf der Offen-Liste): F-04, F-05, F-06, F-08, F-09, F-12..F-16, F-18, F-20, F-25.
- NICHT bauen (offen): F-07 (Glasbruch Elza), F-11 (Streckung), F-17 (Ersatz Head of Akuma),
  F-22 (Schlussszene), F-23 (Fan-Abspann).
- F-10 (PRI-Maske Liege): Nutzer 2026-11-14 "ja" (nur die Tiefe der Rechtecke ueber Marvin, Form bleibt) -> gebaut in Stufe STAGE6
  (4.4 unten).

## 1. Bank-9-Bits

### 1.1 Zensus (2026-11-14)
| Quelle | Werkzeug | belegt | Beleg |
|---|---|---|---|
| Original (Mod-Baum, das, was der Port spielt) | `tools/r36_kartenbits/bank9_zensus.py original` | 85 Bits: 2 6 10 11 12 14 15 16 18 19 20 21 22 52 85 86 90 91 92 98 99 104 105 108 109 134..253 (Auszug) | `belege/bank9_original_2026-11-14.txt` |
| EXE/Overlays | `bank9_zensus.py exe` | Bank 9 = 0x800b1078 nur ueber RDT-Opcodes (Ck @0x8003fd2c, Set @0x8003fe04, Item_aot_set @0x800406d8); Overlays nutzen Bank 5/7/8 | Kopf des Werkzeugs |
| 40-%-Disc (Version_40%/PSX) | `bank9_zensus.teil_original` mit PSX = Disc | 15 Bits: 2 6 10..22 | `belege/bank9_disc40_2026-11-14.txt` |
| Port (dieser Baum) | `bank9_zensus.py port` | 53..84, 87..89, 93..97, 100..103, 106, 107, 110..117, 125, 128, 133, 134, 236, 238, 243 (5/12/15/17 sind Bildbytes in gen/*_plakat.inc bzw. Original-Lesungen) | `belege/bank9_port_2026-11-14.txt` |
| offene r41-Zweige | `git diff HEAD...<zweig> -- tests/unit/test_r36_kartenbits.c` und `#define ..BIT` | nur r41/moosgang 126/127; master, r41/form, sichel_szene, sichel_v8, integration, gruenblut ohne neue Bits | dieser Vertrag |

Frei in allen Quellen (Auszug): 118..124, 129..132, 23..51. Die VERTRAG-R41-Reserven 118/119 (Spur G), 120 (E), 121..124
(J/K) und 129..132 (Orchestrator) sind laut Zensus unbenutzt; der Konzept-Anhang B nennt selbst "Bit 118" fuer 2080/2081.

### 1.2 Zuteilung
| Bit | Makro (`re15_story_r42.h`) | Block | gebaut | Zweck |
|---|---|---|---|---|
| 118 | RE15_R42_BIT_2080_SZENE | B | v0.8.31 | Nachgespraech 2080/2081 gesehen (L1/E1) |
| 119 | RE15_R42_BIT_2080_TOR | B | v0.8.31 | Tor 2080/2081 mit dem Schluessel geoeffnet (K-01) |
| 120 | RE15_R42_BIT_LEITER | B | v0.8.31 | Szene vor der Leiter 11A0/11A1 gesehen (K-07) |
| 121 | RE15_R42_BIT_TUER_11A | B | v0.8.31 | Tuer 11A0/11A1 <-> 2070/2071 von der 11A-Seite geoeffnet (K-02) |
| 122 | RE15_R42_BIT_TUER_2040 | B | v0.8.31 | Tuer 2040/2041 <-> 2070/2071 aus dem Belohnungsraum geoeffnet (K-03) |
| 123 | RE15_R42_BIT_3020_SCHUESSE | B | v0.8.31 | 3020: Leon hoert die Schuesse (L5) |
| 124 | RE15_R42_BIT_3060_UEBERGABE | B | v0.8.31 | 3060: Wiedersehen + Uebergabe, oeffnet Kartentuer 3010 (L5-L7, L-10) |
| 129 | RE15_R42_BIT_FINALE_ENDE | A | v0.8.31 | Finale beendet, Abspann frei (T-07) |
| 137 | RE15_R42_BIT_WERFER_FINALE | A | v0.8.31 | Raketenwerfer der Wurfszene 5090/5091 aufgehoben (K-13, Strang finale Stufe 3; Taken-Bit des Item_aot_set) |
| 130 | RE15_R42_BIT_ZUG_ELZA | C | v0.8.31 | Zugstart 6031 gesehen (E9) |
| 131 | RE15_R42_BIT_ALARM | C | v0.8.32 | Alarm/Durchsage 5060/5061 (L13/E4) |
| 132 | RE15_R42_BIT_IMPFUNG | C | v0.8.32 | Begleiter geimpft 4010/5011 (L13/E4) |
| 23 | RE15_R42_BIT_SCHUTZRAUM | C | v0.8.32 | Schutzraum-Szene 6020/6021 (L18/E5) |
| 24 | RE15_R42_BIT_HANGAR | C | v0.8.32 | Hangar-Tor 5140/5141 offen (L19/E5) |
| 25 | RE15_R42_BIT_IDKARTE | C | v0.8.32 | Research Center ID Card genommen (L14/E-11) |
| 26 | RE15_R42_BIT_ENZYM_DRIN | C | v0.8.32 | Enzym in der Truhe 4090/4091 (L16) |
| 27 | RE15_R42_BIT_KARTE_OK | C | v0.8.32 | ID-Karte am Tastenfeld 4090/4091 (L16) |
| 28 | RE15_R42_BIT_CODE_OK | C | v0.8.32 | Code eingegeben 4090/4091 (L16/L17) |
| 29 | RE15_R42_BIT_PROBE | C | v0.8.32 | Virusprobe genommen 4090/4091 (L16) |
| 30 | RE15_R42_BIT_PROBE_DRIN | C | v0.8.32 | Probe im Geraet 5060/5061 (L16) |
| 31 | RE15_R42_BIT_ZETTEL | C | v0.8.32 | Laborzettel 40B0/40B1 genommen (L17) |
| 32 | RE15_R42_BIT_DOK_P4 | C | v0.8.32 | Dokument P-4 Lab Incident Report 5021 (E3) |
| 33 | RE15_R42_BIT_DOK_VAKZIN | C | v0.8.32 | Dokument Vaccine Synthesis 5011 (E3) |
| 17 | RE15_R42_BIT_ENZYM | C | v0.8.32 | Enzym genommen = Disc-Bit ROOM40A1 @0x00ED0 pc[18] (L16) |
| 34 | RE15_R42_BIT_PLAN_KANAL | C | v0.8.32 | Lageplan Kanalisation 2000/2001 (6b) |
| 35 | RE15_R42_BIT_PLAN_FABRIK | B/C | v0.8.31 | Lageplan Fabrik: Leon Uebergabe 3060 (L6, v0.8.31), Elza 30C1 (v0.8.32) |
| 36 | RE15_R42_BIT_PLAN_FORSCHUNG | C | v0.8.32 | Lageplan Forschungszentrum 4010/4011 (6b) |
| 37 | RE15_R42_BIT_PLAN_UBAHN | C | v0.8.32 | Lageplan U-Bahn 6010/6011 (6b) |

Regeln zu den Bits:
- Leon und Elza teilen nie einen Spielstand. Ein Bit fuer "dieselbe Szene in beiden Fassungen" (xxx0/xxx1, z. B. 118,
  120, 131, 132, 23) ist deshalb Absicht (Muster der Original-Raumpaare: ROOM2090/2091 tragen beide Taken-Bit 90 @0x00A12).
- Enzym 17 ist KEIN Port-Bit, sondern das Bit der 40-%-Disc: Version_40%/PSX/STAGE4/ROOM40A1.RDT Datei @0x00ED0
  `50 0a 09 31 00 00 48 9f 42 96 68 06 7a 03 3c 00 01 00 11 00 ff 00` (Item_aot_set Slot 10, Item 0x3C, x1, Taken-Bit
  u16 pc[18] = 0x0011), Freigabe bei Licht @0x00FE0 `46 0a 09 31 3c 00 01 00 11 00`. Im Mod-Baum und im Port frei.
- Lageplan-Bits 34..37 gehoeren beim Bau in `k_r36k_lage` (kartenitems_r36), NICHT in k_port[] (teil_p meldet sonst
  "doppelt"). Sie stehen deshalb nur im Kopf und hier.
- `test_r36_kartenbits.c`: Puffer `alle[]` von 64 auf 128 erweitert (57 + 23 + 5 Lageplaene = 85 > 64 waere ein
  Ueberlauf gewesen; mit dem Moosgang-Zweig waeren es schon ohne Runde 42 genau 64).
- Neue Bits nur nach erneutem Lauf von `zensus_r42.py` (Tabelle BITS) + Eintrag hier + k_port[].
- Stufe STAGE1 (2026-11-15, story1_r42.c): Bit 120 wird am Ende der Szene vor der Leiter gesetzt, dazu Bit 118 (wer an der Leiter
  war, hat den Begleiter nach dem Alligator getroffen; ein Altstand ohne 118 bekaeme sonst die 2080-Szene danach). Nachrichten
  11A0 3..6 / 11A1 3..10 (Texte tools/r42_story/texte_stage1.py), Bank-5-Signale des Raums 0x14..0x1A (re15_story1_r42.h; die
  11A0/11A1-Skripte nutzen nur 5:0..7). Zensus danach unveraendert FREI (belege/zensus_r42.txt).

## 2. Nachrichten-Ids

### 2.1 Regel
- Neue Ids liegen direkt hinter den RDT-Nachrichten des Raums (Zahl = off[0]/2 des Blocks @RDT+0x3C, wie
  `msg_common.c re15_msg_load_room_block`) und unter 32, wo es geht: dann passen sie auch in die PSX-Tabelle
  (`MSG_TABLE_N 32`, msg_common.c:31) und sind ueber `synchro/.../main<Id>.wav` sprechbar (`VOICE_MAX_MSG 64`,
  platform/pc/src/audio_pc.c:250).
- Ausnahmen (nur PC, >= 32): 2080/2081 43..54 (Konzept-Nummern aus Anhang B, dem Nutzer schon so genannt; 42 = Spur G),
  2090/2091 47 (Spur F 42..46), 4010 44..49 (RDT hat 44 Nachrichten), 5011 46 (RDT hat 46).
- Leon- und Elza-Fassung desselben Textes bekommen dieselbe Id (2080/2081 52..54, 11A0/11A1 11, 2040/2041 6, ...).
- Ablegen IMMER mit Dauer: `re15_msg_install_text` + `re15_msg_install_durations(id, re15_msg_compute_duration(..))`
  (Form `story_r36.c` ablegen, `kanister2080_r41.c:1185-1196`). Dann gilt fuer fehlende Aufnahmen dasselbe wie fuer jede
  RDT-Zeile ohne Ton: Untertitel mit der Textdauer (`scd_vm.c` msg_show: Dauer aus dem Provider, sonst 90 Bilder); liegt
  die Datei da, wartet `op_message_on` auf die echte Restlaenge (`RE15_VOICE_NOTBREMSE` 300). Ohne install_durations
  fiele die Zeile auf feste 90 Bilder zurueck.
- Neutexte vorhandener Ids (NEUAUF/UMTEXT) laufen ueber die Tabelle `gen/story_r36_texte.inc` (re15_story_r36_texte, nach
  dem Laden der Raum-Nachrichten) oder dieselbe Form.

### 2.2 Port-Belegung der betroffenen Raeume (Zensus)
| Raum | RDT | Port belegt | Quelle |
|---|---|---|---|
| 2080/2081 | 0..1 | 42 (40/41 frei seit Spur C NB3) | kanister2080_r41.c:1190; re15_ventilkette_r41.h:193 |
| 2090/2091 | kein Block | 42, 43 (+44..46 Reserve F) | re15_gatorszene_r41.h:139-140; VERTRAG R41 1.2 |
| 2070/2071 | 0 | 40 | re15_ventilkette_r41.h:267 |
| 2000/2001 | 0..11 / 0..14 | 40 | re15_ventilkette_r41.h:236 |
| 30C1 | 0..10 | 21..26 vorsorglich (VERTRAG R41 1.2 ST80; Programm nutzt nur 1..5) | re15_story80_r41.h |
| 6030 | 0..20 | 21..26 ST80, 27..30 AN, 31, 40 | gen/story_r36_texte.inc; re15_annette_r41.h:62-65; re15_story80_r41.h:51-52 |
| 4010 / 5011 | 0..43 / 0..45 | keine (43/45 Speicherpunkt und 42/44 Itembox sind RDT-Ids) | re15_savepoint.c:51/53; re15_itembox.c:68-69 |
| alle uebrigen der Tabelle 2.3 | siehe zensus_r42.txt | keine | grep der Installationsstellen (re15_msg_install_text in 28 Dateien, story_r36, Programm-Bytes 0x2b) |

### 2.3 Zuteilung (alle neuen Zeilen der Bloecke A, B, C)
Art: NEU = neue Zeile mit Stimme; SYS = neue Zeile ohne Stimme; NEUAUF = vorhandene Id, neuer Text, neu aufnehmen;
UMTEXT = vorhandener Systemtext, neuer Wortlaut. Stimme: `synchro/STAGE<n>/room<RAUM>/main<Id>.wav`.
Block A (Finale, T-03..T-09) hat keine neuen Zeilen; Finale-Texte folgen nach F-22.

| Raum | Id | Art | Sprecher | Text | Block | gebaut | Konzept |
|---|---|---|---|---|---|---|---|
| 11A0 | 3 | NEU | Marvin | Leon, wait up! | B | v0.8.31 | L3 |
| 11A0 | 4 | NEU | Marvin | No sign of the girl back there. And nothing's following us. | B | v0.8.31 | L3 |
| 11A0 | 5 | NEU | Leon | Good. Let's get out of here. | B | v0.8.31 | L3 |
| 11A0 | 6 | NEU | Marvin | I'll go up first. Stay close. | B | v0.8.31 | L3 |
| 11A0 | 11 | SYS | System | Leon unlocked the door. | B | v0.8.31 | L1b K-02 |
| 11A1 | 3 | NEU | Sherry | Elza! Wait for us! | B | v0.8.31 | E2 |
| 11A1 | 4 | NEU | John | It's clear back there. Nothing's following us. | B | v0.8.31 | E2 |
| 11A1 | 5 | NEU | Elza | Good. Let's get out of here. | B | v0.8.31 | E2 |
| 11A1 | 6 | NEU | Elza | Sherry, wait. I think this is yours. | B | v0.8.31 | 6b F-26 K-08 |
| 11A1 | 7 | NEU | Sherry | My watch! I must have dropped it in the water... | B | v0.8.31 | 6b F-26 |
| 11A1 | 8 | NEU | Elza | The current carried it all the way down to that alligator. | B | v0.8.31 | 6b F-26 |
| 11A1 | 9 | NEU | Elza | Hold on to it. Your mom was right. It IS good luck. | B | v0.8.31 | 6b F-26 |
| 11A1 | 10 | NEU | John | I'll go up first. Sherry, you're right behind me. | B | v0.8.31 | E2 |
| 11A1 | 11 | SYS | System | Elza unlocked the door. | B | v0.8.31 | L1b K-02 |
| 2000 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b K-04 F-25 |
| 2000 | 31 | SYS | System | You've taken the drainage map. | C | v0.8.32 | 6b K-04 F-25 |
| 2001 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b K-04 F-25 |
| 2001 | 31 | SYS | System | You've taken the drainage map. | C | v0.8.32 | 6b K-04 F-25 |
| 2040 | 6 | SYS | System | Leon unlocked the door. | B | v0.8.31 | L1b K-03 |
| 2041 | 6 | SYS | System | Elza unlocked the door. | B | v0.8.31 | L1b K-03 |
| 2070 | 1 | SYS | System | It's locked from the other side. | B | v0.8.31 | L1b K-02/K-03 |
| 2071 | 1 | SYS | System | It's locked from the other side. | B | v0.8.31 | L1b K-02/K-03 |
| 2080 | 43 | NEU | Leon | Thanks, Marvin. I thought I was gonna end up as dinner. | B | v0.8.31 | L1 |
| 2080 | 44 | NEU | Leon | What brought you here, anyway? | B | v0.8.31 | L1 |
| 2080 | 45 | NEU | Marvin | Hard to miss. The whole place was shaking. | B | v0.8.31 | L1 |
| 2080 | 46 | NEU | Leon | Yeah... right. Thanks again. | B | v0.8.31 | L1 |
| 2080 | 47 | NEU | Marvin | Go on ahead, Leon. I'll wait here and watch your back. | B | v0.8.31 | L1 K-07 |
| 2080 | 48 | NEU | Marvin | And I'll keep an eye out for that little girl. | B | v0.8.31 | L1 K-07 |
| 2080 | 52 | SYS | System | Next to the computer... there's a keyhole. | B | v0.8.31 | L1 K-01/K-04 |
| 2080 | 53 | SYS | System | The gate is locked. It needs a key. | B | v0.8.31 | L1 |
| 2080 | 54 | SYS | System | Leon used the key. | B | v0.8.31 | L1 |
| 2081 | 43 | NEU | Elza | Thanks, John. I thought I was gonna end up as dinner. | B | v0.8.31 | E1 |
| 2081 | 44 | NEU | Elza | What brought you here, anyway? | B | v0.8.31 | E1 |
| 2081 | 45 | NEU | John | Hard to miss. The whole place was shaking. | B | v0.8.31 | E1 |
| 2081 | 46 | NEU | Elza | Well... thanks again. | B | v0.8.31 | E1 |
| 2081 | 47 | NEU | Sherry | Elza, are you okay? I heard some really loud noises! | B | v0.8.31 | E1 |
| 2081 | 48 | NEU | Elza | I'm fine, Sherry. Thanks. | B | v0.8.31 | E1 |
| 2081 | 49 | NEU | John | So you're Sherry. Stick close, kid. | B | v0.8.31 | E1 F-03 |
| 2081 | 50 | NEU | John | Elza, go on ahead. I'll wait here and look after Sherry until you've cleared the way. | B | v0.8.31 | E1 K-07 |
| 2081 | 51 | NEU | Elza | All right. Sherry, you stay with John. | B | v0.8.31 | E1 K-07 |
| 2081 | 52 | SYS | System | Next to the computer... there's a keyhole. | B | v0.8.31 | E1 K-01/K-04 |
| 2081 | 53 | SYS | System | The gate is locked. It needs a key. | B | v0.8.31 | E1 |
| 2081 | 54 | SYS | System | Elza used the key. | B | v0.8.31 | E1 |
| 2090 | 47 | SYS | System | There's something else lying next to it... | B | v0.8.31 | 6b K-08 |
| 2091 | 47 | SYS | System | There's something else lying next to it... | B | v0.8.31 | 6b K-08 |
| 3020 | 0 | NEUAUF | Leon | Damn it! It won't budge. | B | v0.8.31 | L5 Glaettung |
| 3020 | 1 | NEUAUF | Marvin | There's an open window up there. We could get in that way. | B | v0.8.31 | L5 Glaettung |
| 3020 | 2 | NEUAUF | Marvin | Leon, give me a hand! | B | v0.8.31 | L5 Glaettung |
| 3020 | 7 | NEU | Marvin | Wait here. I'll find a way to open this from the inside. | B | v0.8.31 | L5 |
| 3020 | 8 | NEU | Leon | Watch yourself, Marvin. | B | v0.8.31 | L5 |
| 3020 | 9 | NEU | Leon | Gunshots...? Marvin! | B | v0.8.31 | L5 L-08 |
| 3060 | 11 | NEU | Marvin | Damn it...! | B | v0.8.31 | L5 |
| 3060 | 12 | NEU | Marvin | Leon... Over here. | B | v0.8.31 | L5 |
| 3060 | 13 | NEU | Leon | Marvin! What happened? | B | v0.8.31 | L5 |
| 3060 | 14 | NEU | Marvin | Dogs. One of them got my leg. | B | v0.8.31 | L5 |
| 3060 | 15 | NEU | Marvin | I couldn't get that door open for you. But I found this. | B | v0.8.31 | L6 |
| 3060 | 16 | SYS | System | You've received the Green Keycard. | B | v0.8.31 | L6 |
| 3060 | 17 | NEU | Marvin | And this. A floor plan of the plant. | B | v0.8.31 | L6 |
| 3060 | 18 | SYS | System | You've received the factory map. | B | v0.8.31 | L6 |
| 3060 | 19 | NEU | Marvin | Found this, too. You'll get more use out of it than me. | B | v0.8.31 | L6 F-06 |
| 3060 | 20 | SYS | System | You've received the Super Redhawk. | B | v0.8.31 | L6 F-06 |
| 3060 | 21 | NEU | Leon | A keycard... This should open the door in the loading area. | B | v0.8.31 | L6 |
| 3060 | 22 | NEU | Marvin | Go on. With this leg, I'd only slow you down. | B | v0.8.31 | L7 |
| 3060 | 23 | NEU | Leon | I'm not leaving you out here. | B | v0.8.31 | L7 |
| 3060 | 24 | NEU | Marvin | I'll make it back to the lobby. I can lock myself in and keep an eye on the front gate. | B | v0.8.31 | L7 |
| 3060 | 25 | NEU | Marvin | If Ada turns up, I'll send her your way. | B | v0.8.31 | L7 |
| 3060 | 26 | NEU | Leon | Hang in there. I'll come back for you. | B | v0.8.31 | L7 |
| 3060 | 27 | NEU | Marvin | You'd better, rookie. | B | v0.8.31 | L7 |
| 3070 | 6 | NEU | Leon | That came from the storage room... Ada? | B | v0.8.31 | L10 F-08 |
| 30C1 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b |
| 30C1 | 31 | SYS | System | You've taken the factory map. | C | v0.8.32 | 6b |
| 30E0 | 3 | NEUAUF | Leon | He's back in the lobby. Some dogs got his leg... he can barely walk. | B | v0.8.31 | L7 |
| 30E0 | 10 | NEUAUF | Leon | That's a bite... They got you? | B | v0.8.31 | L8 |
| 30E0 | 11 | NEUAUF | Marvin | The lobby's gone...! They smashed through the front glass and got inside! | B | v0.8.31 | L8 |
| 30E0 | 13 | NEUAUF | Marvin | There's more of them than I thought... If they've come this far, the city's finished. | B | v0.8.31 | L8 |
| 30E0 | 16 | NEU | Leon | That came from the lobby... Marvin! | B | v0.8.31 | L8 |
| 30E0 | 17 | NEU | Ada | Someone's coming. | B | v0.8.31 | L10 optional |
| 4010 | 35 | NEUAUF | Ada | Hurry to the shelter! I'll bring Marvin. | C | v0.8.32 | L13 |
| 4010 | 44 | NEU | Leon | Marvin, I've got the vaccine. Hang on... this should help. | C | v0.8.32 | L13 |
| 4010 | 45 | NEU | Leon | That announcement... what's happening? | C | v0.8.32 | L13 |
| 4010 | 46 | NEU | Ada | Something's gone wrong with the reactor. If it blows, the virus goes everywhere. | C | v0.8.32 | L13 |
| 4010 | 47 | SYS | System | The monitors are on. Most of the security cameras show nothing but static. | C | v0.8.32 | L15 |
| 4010 | 48 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b F-18 |
| 4010 | 49 | SYS | System | You've taken the research center map. | C | v0.8.32 | 6b F-18 |
| 4011 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b F-18 K-05 |
| 4011 | 31 | SYS | System | You've taken the research center map. | C | v0.8.32 | 6b F-18 K-05 |
| 4030 | 2 | UMTEXT | System | The fuses are all connected. The elevator should be running now. | C | v0.8.32 | L15 |
| 4030 | 9 | SYS | System | The breaker hums to life. Power has been restored to the lab. | C | v0.8.32 | L15 |
| 4031 | 2 | UMTEXT | System | The fuses are all connected. The elevator should be running now. | C | v0.8.32 | E-09 |
| 4031 | 9 | SYS | System | The breaker hums to life. Power has been restored to the lab. | C | v0.8.32 | E-09 |
| 4080 | 0 | UMTEXT | System | "Cell Cultivation Room" It's electronically locked. Registered staff only. | C | v0.8.32 | L17 |
| 4081 | 0 | UMTEXT | System | "Cell Cultivation Room" It's electronically locked. Registered staff only. | C | v0.8.32 | E-10 |
| 4090 | 8 | SYS | System | An ultra low temperature freezer. It's dead. There's no power. | C | v0.8.32 | L16 |
| 4090 | 9 | SYS | System | Will you use the Enzyme? | C | v0.8.32 | L16 |
| 4090 | 10 | SYS | System | You've set the Enzyme in the freezer. | C | v0.8.32 | L16 |
| 4090 | 11 | SYS | System | Will you use the Research Center ID Card? | C | v0.8.32 | L16 |
| 4090 | 12 | SYS | System | The keypanel accepted the ID card. The display reads: "ENTER ACCESS CODE." | C | v0.8.32 | L16 |
| 4090 | 13 | SYS | System | "SAMPLE TRANSFER READY." | C | v0.8.32 | L16 |
| 4090 | 14 | SYS | System | The robotic arm lifts a sealed case out of the freezer... | C | v0.8.32 | L16 |
| 4090 | 15 | SYS | System | You've taken the T-Virus. | C | v0.8.32 | L16 |
| 4090 | 16 | NEU | Leon | Easy... one slip and it's all for nothing. | C | v0.8.32 | L16 |
| 4091 | 1 | UMTEXT | System | The enzyme is already in place. I need to operate the machine to synthesize a sample of the G-Virus. | C | v0.8.32 | E-12 |
| 4091 | 8 | SYS | System | An ultra low temperature freezer. It's dead. There's no power. | C | v0.8.32 | E-12 |
| 4091 | 9 | SYS | System | Will you use the Enzyme? | C | v0.8.32 | E-12 |
| 4091 | 10 | SYS | System | You've set the Enzyme in the freezer. | C | v0.8.32 | E-12 |
| 4091 | 11 | SYS | System | Will you use the Research Center ID Card? | C | v0.8.32 | E-12 |
| 4091 | 12 | SYS | System | The keypanel accepted the ID card. The display reads: "ENTER ACCESS CODE." | C | v0.8.32 | E-12 |
| 4091 | 13 | SYS | System | "SAMPLE TRANSFER READY." | C | v0.8.32 | E-12 |
| 4091 | 14 | SYS | System | The robotic arm lifts a sealed case out of the freezer... | C | v0.8.32 | E-12 |
| 4091 | 15 | SYS | System | You've taken the G-Virus. | C | v0.8.32 | E-12 |
| 4091 | 16 | NEU | Elza | Easy... one slip and it's all for nothing. | C | v0.8.32 | E-12 |
| 5011 | 30 | NEUAUF | Elza | And the fire's already spreading. I had to run through it to get back here. | C | v0.8.32 | E4 optional |
| 5011 | 46 | NEU | Elza | Here, Sherry... this will help. | C | v0.8.32 | E4 |
| 5021 | 2 | NEU | Elza | ...If Sherry's been exposed to this... | C | v0.8.32 | E3 |
| 5050 | 5 | UMTEXT | System | Registration complete. Access granted: Cell Cultivation Room. | C | v0.8.32 | L17 |
| 5051 | 5 | UMTEXT | System | Registration complete. Access granted: Cell Cultivation Room. | C | v0.8.32 | E-10 |
| 5060 | 15 | NEU | Durchsage | Warning. The reactor safety system has been disabled. | C | v0.8.32 | L13 |
| 5060 | 16 | NEU | Durchsage | Core overload in progress. Biohazard containment can no longer be maintained. | C | v0.8.32 | L13 |
| 5060 | 17 | NEU | Durchsage | All personnel, proceed to the emergency shelters immediately. | C | v0.8.32 | L13 |
| 5060 | 18 | NEU | Leon | The core...? Marvin... I have to get this to him. Now! | C | v0.8.32 | L13 |
| 5060 | 19 | SYS | System | Current Biohazard Level: CRITICAL. The display is flashing red. | C | v0.8.32 | L13 |
| 5060 | 20 | SYS | System | A vaccine synthesis machine. A sample of the virus must go inside. | C | v0.8.32 | L16 |
| 5060 | 21 | SYS | System | Will you set the T-Virus sample in the machine? | C | v0.8.32 | L16 |
| 5061 | 15 | NEU | Durchsage | Warning. The reactor safety system has been disabled. | C | v0.8.32 | E4 |
| 5061 | 16 | NEU | Durchsage | Core overload in progress. Biohazard containment can no longer be maintained. | C | v0.8.32 | E4 |
| 5061 | 17 | NEU | Durchsage | All personnel, proceed to the emergency shelters immediately. | C | v0.8.32 | E4 |
| 5061 | 18 | NEU | Elza | The core...? I have to get this to Sherry. Now! | C | v0.8.32 | E4 |
| 5061 | 19 | SYS | System | Current Biohazard Level: CRITICAL. The display is flashing red. | C | v0.8.32 | E4 |
| 5061 | 20 | SYS | System | A vaccine synthesis machine. A sample of the virus must go inside. | C | v0.8.32 | E-12 |
| 5061 | 21 | SYS | System | Will you set the G-Virus sample in the machine? | C | v0.8.32 | E-12 |
| 50A0 | 4 | SYS | System | The panel accepts the card, but the shutter won't move. It's locked from the hangar side. | C | v0.8.32 | 6b |
| 50A1 | 4 | SYS | System | The panel accepts the card, but the shutter won't move. It's locked from the hangar side. | C | v0.8.32 | 6b |
| 50B0 | 10 | NEUAUF | Leon | Ada... the reactor's safety system. That's what you "had to do," isn't it? | C | v0.8.32 | L20 |
| 50E0 | 0 | NEU | Leon | What the hell is that thing...? | C | v0.8.32 | L14 |
| 50E0 | 1 | SYS | System | The power control console for the lower levels. Someone is slumped over it. | C | v0.8.32 | L14 |
| 50E0 | 2 | SYS | System | There's an ID card clipped to the body. Will you take it? | C | v0.8.32 | L14 |
| 50E0 | 3 | SYS | System | You've taken the Research Center ID Card. | C | v0.8.32 | L14 |
| 50E0 | 4 | SYS | System | The console is wrecked. Sparks are still spitting from the panel. | C | v0.8.32 | L14 |
| 50F1 | 0 | NEU | Elza | What the hell is that thing...? | C | v0.8.32 | E-11 K-06 |
| 50F1 | 1 | SYS | System | The power control console for the lower levels. Someone is slumped over it. | C | v0.8.32 | E-11 |
| 50F1 | 2 | SYS | System | There's an ID card clipped to the body. Will you take it? | C | v0.8.32 | E-11 |
| 50F1 | 3 | SYS | System | You've taken the Research Center ID Card. | C | v0.8.32 | E-11 |
| 50F1 | 4 | SYS | System | The console is wrecked. Sparks are still spitting from the panel. | C | v0.8.32 | E-11 |
| 5140 | 0 | NEUAUF | Annette | It's open. Come through, quickly. | C | v0.8.32 | L19 F-16 |
| 5140 | 1 | NEU | Leon | ...Who's there? | C | v0.8.32 | L19 optional |
| 5140 | 2 | SYS | System | The hangar shutter is sealed. It can only be opened from the hangar side. | C | v0.8.32 | L19 |
| 5141 | 0 | NEUAUF | Sherry | It's open! Back here, Elza! | C | v0.8.32 | E5 |
| 5141 | 2 | SYS | System | The hangar shutter is sealed. It can only be opened from the hangar side. | C | v0.8.32 | E5 |
| 6010 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b |
| 6010 | 31 | SYS | System | You've taken the subway map. | C | v0.8.32 | 6b |
| 6011 | 30 | SYS | System | There's a map on the wall. Will you take it? | C | v0.8.32 | 6b |
| 6011 | 31 | SYS | System | You've taken the subway map. | C | v0.8.32 | 6b |
| 6020 | 1 | NEU | Leon | Ada! Is Marvin...? | C | v0.8.32 | L18 |
| 6020 | 2 | NEU | Ada | He's breathing. The vaccine's doing its job... for now. | C | v0.8.32 | L18 |
| 6020 | 3 | NEU | Leon | This whole place is coming apart. | C | v0.8.32 | L18 |
| 6020 | 4 | NEU | Ada | The reactor. When it goes, this shelter won't protect anyone. | C | v0.8.32 | L18 |
| 6020 | 5 | NEU | Ada | There's an emergency train below the hangar. That's our way out. | C | v0.8.32 | L18 |
| 6020 | 6 | NEU | Ada | I'll take Marvin down. Go through the C-2 corridor. The hangar gate will be opened for you. | C | v0.8.32 | L18 |
| 6020 | 7 | NEU | Leon | And you? | C | v0.8.32 | L18 |
| 6020 | 8 | NEU | Ada | I'll meet you in the hangar. Go! | C | v0.8.32 | L18 |
| 6021 | 1 | NEU | John | Smoke's getting in under the door. This place won't hold for long. | C | v0.8.32 | E5 |
| 6021 | 2 | NEU | Elza | If that core goes, nowhere down here is safe. | C | v0.8.32 | E5 |
| 6021 | 3 | NEU | Sherry | There's a train! Under the hangar. Dad's people used it to leave the city. | C | v0.8.32 | E5 |
| 6021 | 4 | NEU | Elza | Can you get us there? | C | v0.8.32 | E5 |
| 6021 | 5 | NEU | Sherry | The hangar gate only opens from the inside. But I know a way in! | C | v0.8.32 | E5 |
| 6021 | 6 | NEU | John | Sherry, wait! | C | v0.8.32 | E5 |
| 6021 | 7 | NEU | Elza | John, go after her. I'll meet you at the hangar. | C | v0.8.32 | E5 |
| 6030 | 2 | NEUAUF | Leon | Where is she now? | C | v0.8.31 | L21 |
| 6030 | 3 | NEUAUF | John | We got split up in the fire. One second she was right behind us... | C | v0.8.31 | L21 |
| 6030 | 4 | NEUAUF | Leon | If she's as tough as you say, she'll find another way out. But we can't wait. When that reactor goes, nobody gets out. | C | v0.8.31 | L21 |
| 6030 | 5 | NEUAUF | John | Then I'm staying with the kid. And I'll keep an eye on your friend here. | C | v0.8.31 | L21 |
| 6030 | 21 | NEUAUF | John | Name's John. This is Sherry. We came down here with a girl named Elza. | C | v0.8.31 | L21 |
| 6030 | 22 | NEUAUF | Sherry | Elza's still out there...! | C | v0.8.31 | L21 |
| 6031 | 1 | NEUAUF | Sherry | A policeman brought Marvin down here. Then he went back into the fire. | C | v0.8.31 | E6 |
| 6031 | 2 | NEUAUF | Elza | Back? Why? | C | v0.8.31 | E6 |
| 6031 | 12 | NEU | Sherry | He said he had to find someone. He told us not to wait for him. | C | v0.8.31 | E6 |
| 6031 | 13 | NEU | Elza | ...Then we won't. If that core goes, no one gets out. Let's get this train moving. | C | v0.8.31 | E6 |
| 6031 | 14 | NEU | Sherry | It's moving! Elza, we did it! | C | v0.8.31 | E9 |
| 6031 | 15 | NEU | John | What the hell was that? It came from the back car! | C | v0.8.31 | E9 |
| 6031 | 16 | NEU | Sherry | It's that monster... it followed us! | C | v0.8.31 | E9 |
| 6031 | 17 | NEU | Elza | John, stay with Sherry and Marvin. Don't open that door for anyone but me. | C | v0.8.31 | E9 |

Besonderheiten:
- 6030 (L21): die sechs Saetze gehen in Spielreihenfolge des Wagen-Programms (gen/story80_szenen.inc
  `k_s80_6030_sub02_folge = {0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8}`) auf die Ids 21, 2, 3, 22, 4, 5. Dabei wechselt
  der Sprecher: msg 2 Sherry -> Leon, msg 22 John -> Sherry (Schnitte/Work_set des Programms beim Bau anpassen).
- 6031 (E6): msg 0 bleibt ("Elza: Sherry! Is anyone else here?", story_r36), msg 1/2 Neutext, 12/13 neu; E9 14..17.
- 5140/5141 msg 0: der Disc-Satz " / It's open on this end." (nie gezeigt, keine Aufnahme) wird Annettes bzw. Sherrys
  Lautsprecher-/Rufzeile.
- 3060 msg 20 ("You've received the Super Redhawk.") ist eine Ergaenzung analog L6 (das Konzept nennt nur Marvins Satz); gebaut
  als "You've received the <Name>." je Fund (4.2).
- Elza-Saetze aus Leons Fassung abgeleitet (Kette fuer beide): 50F1 msg 0, 4091 msg 16.
- 3020 msg 0..2: die "Glaettung" aus L5 (Vorschlag des Konzepts) - Neutext + Neuaufnahme.

## 3. Texte ohne Nachrichten-Id (Beschreibungen, Dokumente, Gegenstaende)
- Taschenuhr (Item 0x3F, ROOM2090/2091 main00 @0x00A12 Slot 5, Taken-Bit 90): Beschreibung Leon mit Gravur
  "A pocket watch. Something's engraved on the back: "For Sherry. Love, Mom."" (F-19), Bank gen/inv_desc_bank.inc.
- T-09 Blue Master Keycard: "An ID card for the research center. It belongs to someone named "Michael."" (gebaut, 4.5).
- Neue Gegenstaende (v0.8.32): Enzyme (0x3C, Disc), T-Virus, G-Virus, T-Vaccine, G-Vaccine, Research Center ID Card
  (Nummer der Communications Card, Konzept 6b), Rocket Launcher (Name vorhanden) - Beschreibungen woertlich aus
  Konzept 6b; Item-Nummern teilt die Laborketten-Spur nach eigenem Zensus zu.
- Dokumente (re15_files.c, v0.8.32): "P-4 Lab Incident Report" (5021), "Vaccine Synthesis" (5011), Laborzettel 40B0/40B1
  (Code noch offen: Entwurf "New code: ____").

## 4. Messpunkte vor dem Bau (aus dem Konzept, nicht Teil dieses Zensus)
- L1/K-01: welches Tor der Water Key in 2080 oeffnet (heute: Tor 2090<->20A0, RE15_GSZ_BIT_TOR 113) und wo der Schluessel liegt.
- L1b/K-02/K-03: Korridor 2070/2071 Tuer zu 11A0 = Door_aot_set Slot 4 @0x00AD6/@0x00AFA (gen/tuer_zuordnung.inc S159),
  Tuer zu 2040 = Slot 5 @0x00B1C (S160); Gegenseite 2040 Slot 4 @0x0131C (S149); die 11A0-Seite misst der Bau.
- L3/E2: Ausloesezone "schmaler Abschnitt vor der Leiter" 11A0/11A1 und Laufweg der Begleiter.
- E-11: Birkin-Form/Spawn in 50E0/50F0/50E1/50F1.

### 4.1 Gemessen (Stufe STAGE2, 2026-11-14; Belege im Kopf von `re15_port/include/re15_story2_r42.h`)
- L1/K-01: ROOM2080/2081 haben KEINEN Tor-Satz (main00 nur Slot 0 @0x0BA2 -> 2030 und Slot 1 @0x0BC2 Leiter). Das einzige Tor
  ist ROOM2090/2091 Slot 1 @0x009B6 -> 20A0 (Bit 113). Umsetzung: Schluesselloch = Rechteck des Rechners (sub00 @0x0C1A) nach dem
  Ablassen ((3,245) = 1), der Schluessel setzt Bit 113 + 119 und startet die Szene; das Tor in 2090 nimmt den Schluessel nicht
  mehr. Der Schluessel liegt nach dem Tod bei der Leiche im Suedbecken (Ersatzlage (-170,-1620,-23629), gatorszene_r41.c).
- L1b/K-02: 11A0-Seite = 11A0/11A1 main00 Slot 0 (msg 2 @0x00F42 bei (3,151)=0, sonst Door_aot_set @0x00F5A). Riegel im Original
  auf der 2070-Seite (sub02 @0x0CDA). K-03: 2040/2041 Slot 4 @0x0131C (Galerie, Band 5) <-> 2070 Slot 5 @0x0B1C.
- L3/E2 (Leiter 11A0/11A1) und E-11 (50E0/50F1): noch nicht gemessen (spaetere Stufen).

### 4.2 Gemessen und gebaut (Stufe STAGE3a, 2026-11-15; Belege im Kopf von `re15_port/include/re15_story3a_r42.h`)
- T-01/T-02: Figurwechsel nach RE2 FUN_80068f9c (Eintritt @0x80068fc8..@0x800690f4, Rueckweg @0x80069108..@0x800691e8). Die
  Sicherung (Leons 11 Plaetze, Ausruest-Platz, Waffe; Marvins Funde bis zur Uebergabe) liegt im Spielstand v10 (`story_r42[96]`
  vor checksum, v9 wird gehoben). Marvins Seite = Leons Startmengen @0x80074bb8/@0x80074bd0 ohne Messer, Platz 0, Waffe 3.
- 3060 msg 19/20: msg 19 kommt, wenn Marvin ausser Keycard/Pistole/Munition IRGENDETWAS gefunden hat (F-06 "alle Funde"), msg 20
  wird je Fund zur Laufzeit gebaut: "You've received the <Name>." mit dem Namen aus derselben Namensbank wie "Will you take the
  <Name>?" (item_prompt_common.c re15_item_prompt_name). Ohne Keycard bei Marvin (er hat sie liegen lassen) entfallen 15/16/21.
- L-10: die Kartentuer 3010 braucht keinen eigenen Riegel - die Kistenregel K7 (kistenregel_r37.c @0x020AE) verlangt die Karte im
  Inventar, und die liegt bis zur Uebergabe in Marvins Seite (gemessen unit_r42_story3a_karte3010). Die 30E0-Seite der Kartentuer
  (30E0 Slot 0 @0x009F2) liegt im WEST-Ring von 30E0 (SCA @0x51C Eintraege 0..8), Marvin kommt aus 3030 in den OST-Ring (13..18):
  kein Weg (Nachbesserung 1 B2, unit_r42_story3a_weg30e0, Kopf von re15_story3a_r42.h).
- Nachbesserung 1: 3020 msg 0..2 und 30E0 msg 3 werden NACH re15_msg_load_room_block eingesetzt (msg_common.c, vorher vom Loader
  ueberschrieben, B1); 30E0 msg 3 Neutext (L7) gebaut (B5); "Gunshots...?" nur direkt nach dem Rueckwechsel, Wiedersehen im
  Altstand nur vor der Kartentuer (B6); Rueckwechsel gibt Leon Vor-Platz, Leben und Gift zurueck (RE2 FUN_80049e48
  @0x8004a000-@0x8004a07c), H. Gun Bullets ueber Marvins 50 eigenen sind ein Fund (B8).
- Bank 5 Signale 0x14..0x1A und Ankunftsbits 0x20/0x21 in 3020/3060 (3020-Skripte nutzen nur 5:0/5:1, 3060 keine).

### 4.3 Gemessen und gebaut (Stufe STAGE3b, 2026-11-15; Belege im Kopf von `re15_port/include/re15_story3b_r42.h`)
- L8-L10: 30E0 in EINEM Besuch = Programm aus den Original-Bytes sub02 [@0x00CB0,@0x00E54) + Uebergang + sub03 [@0x00E62,@0x00F86)
  + Warten + Ausklang sub03 [@0x00F86,@0x00FC0) (Nacken zurueck, Plc_ret x3, Sleep 0x1E, Cut_auto @0x00FB4, Riegel los, Evt_end);
  Ids nur aus 2.3 (30E0 3/10/11/13/16/17, 3070 6). Kein neues Bank-9-Bit: der Ablauf haengt an den Original-Bits
  (4,194)/(3,75)/(3,180); die 3070-Zeile laeuft einmal je Uebergang (4,194) 0 -> 1 im Besuch.
- Nutzerbefund 15.11. (Ende der Marvin-Szene): der Riegel "Satz ausreden lassen" sass nur am naechsten Message_on; nach msg 13 kommt
  keines mehr. Jetzt wartet die Szene nach der Sleep-Kette von msg 13 und VOR dem Ausklang @0x00F86 auf Zeile UND Stimme
  (gemessen: main13.wav 270 Bilder, Sleep-Kette 152). Ein erster Bau wartete vor Cut_auto @0x00FB4: dann liefen Marvin und Ada
  nach ihrem Plc_ret @0x00F90/@0x00F9A schon waehrend msg 13 los.
- 3070 msg 6 (Nachbesserung 1, B3): davor poltert es im Lager (Port-Bank 0x3E Satz 0x00, vier Schlaege im Takt ROOM3091 sub07
  @0x01A56..@0x01A9E), Leon dreht den Kopf zur Tuer nach 30E0 (ROOM3070 main00 @0x03322, Mitte (-12400,-19100)).
- Alte Aufnahmen: 30E0 msg 3/10/11/13 sind NEUAUF. Bis zur Neuaufnahme spielt die vorhandene Aufnahme unter dem neuen Text weiter,
  wie 3020 msg 0..2 (so in der AUFNAHMELISTE angekuendigt). Eine neue Aufnahme im Ordner spielt sofort. Die Stummschaltung aus
  Nachbesserung 1 (B4) ist in Nachbesserung 2 (Pruefer R2-1) ganz entfernt. Mit der echten main13.wav wartet die letzte
  Marvin-Szene nach der Sleep-Kette auf Untertitel UND Stimme (Durchlauf integration_r42_story3b_leon, R2-3).
- OFFEN (R2-2): Uebergang Auftritt -> Kampf in 3070 steht 12 Bilder still (Lunge a3 0x100 @0x80117644 fehlt der Port-Kampf-KI);
  wird im Klauenhieb-Umbau (case 3) auf Zweig r41/birkin_kralle behoben, nicht auf r42/story.
- T-03/T-04: enemy_ai_common.c re15_birkin_anim_szene (Auftritt 0x80119378 und Szenen-Modus 0x80118d00), NUR STAGE3 (3070, 3080).
  Nach dem Auftritt setzt der Port die Rate zurueck (der Kampf laeuft wie vor T-03); die Todesphasen tragen a3 0x200 @0x8011a764.
  Vorher/Nachher-Laeufe 50A0, 5080, 5090 bytegleich (Pose-Messschiene RE15_POSE_LOG + state.log); Labor 50E0/50F1 Posen,
  Gewichte, Pruefsummen und state.log gleich (nur das Ratenfeld der Todesposen 512 statt 0, zeichengleich). Der ungesperrte Fix
  haette den Labor-Auftritt 50E0/50F1 geaendert (je 33 Posen, derselbe Fehler wie 3070) - gemeldet, nicht gebaut.
- Bank 5 in 30E0: FREI 0x14, Messmarke 0x15, Ankunftsbits 0x20..0x22 (30E0-Skripte nutzen nur 5:0..5:6).

### 4.4 Gebaut (Stufe STAGE6, 2026-11-16; Belege im Kopf von `re15_port/include/re15_story6_r42.h`)
- L21 (6030): Wagen-Programm k_s80_6030_sub02 (tools/r41_story80/szenen_bauen.py) mit den Ids 21, 2, 3, 22, 4, 5 nach 2.3; Folge
  unveraendert. Sprecherwechsel: msg 2 Leon (Leon-Gesten @0x00ECA, Blick bleibt bei John), msg 22 Sherry (Blick @0x00DEC, Sleep 100
  @0x00E84). 6030 msg 4 (Nutzersatz, vier Zeilen) = zweiseitiger Untertitel in der Form des einzigen Original-Vorbilds ROOM3060 msg 7
  @0x1244 `04 01 02 3c 04 00`, Seitenwartezeit 0x63 = Nachlauf der 6030-Zeilen (`01 63` @0x13F5); 139 Byte > PSX MSG_RAW_LEN 128 (PC 192).
- E6/E9 (6031): engine/src/story6_r42.c, Programme tools/r42_story/szenen_stage6.py (Wagen msg 0,1,2,12,13; Zugstart 14..17 einmalig
  ueber Bit 130; Wagen nach dem Tor und vor dem Zugstart: Tuer nach 5091 = msg 4 wie Leons k_tuer). Texte tools/r42_story/texte_stage6.py.
  Die Runde-36/41-Fassungen 6030 3/5/21/22 und 6031 1 sind aus tools/r36_p/texte_bauen.py entfernt (eine Quelle je Text).
- Bank 5 in 6031: 0x10 Zug-an-Ton, 0x11 Krachen (6031-Skripte nutzen nur 0x20/0x21). Kein neues Bit: 130 (1.2), EIGEN im Zensus.
- T-05 (F-10): Tiefe der Port-Masken Cut 9 in shared_assets/PSX/MASKS/ROOM6030.MSK und ROOM6031.MSK (die Rechtecke liegen dort, nicht
  in `pri/` - die pri/*_port.png sind Uebersichtsbilder der Form, die Form bleibt, also unveraendert; pri_hashes.py ohne Aenderung).
  Werkzeug tools/maske/liege_tiefe_r42.py: 51 von 91 Rechtecken (nur die Tiefen-Halbwoerter), Vorderkante und Seitenholme bleiben.

### 4.5 Gebaut (Stufe Kleinkram, 2026-11-16; Belege im Kopf von `re15_port/include/re15_story_klein_r42.h`)
- T-08: ROOM6020 sub00 @0x00BAA (Sce_em_set Slot 1 Typ 0x42 = Ada) wird verworfen, wenn der Merker (3,0x62) gesetzt ist. Einziger
  Schreiber im Spiel: ROOM50B0 sub02 @0x005A6 `22 03 62 01` (Adas Szene; Zensus aller RDTs: sonst nur Ck @0x004E6). Marvin (@0x00B96)
  bleibt; ROOM6021 (Typ 0x49/0x4B) unberuehrt. Kein neues Bit (Bank 3 ist ein Original-Merker), Zensus unveraendert FREI.
- T-09: Item 0x47 (Blue Master Keycard, ROOM4010 Slot 9 @0x00592) Beschreibung "An ID card for the research / center. It belongs to /
  someone named "Michael."" in der Form der Original-Beschreibung 0x47 @0x800c5e53 (3 Zeilen, 1 Seite); Leon und Elza.
- Pruefhinweis H4: Nachlauf `04 01 01 N` der neuen Untertitel aus den Original-Zeilen desselben Raums (tools/r42_story/nachlauf_r42.py):
  ROOM3060 0x3C, ROOM3020 einzeilig 0x59 / zweizeilig 0x63, ROOM30E0/3070/6030/6031 0x63; Raeume ohne Original-Untertitel
  (11A0/11A1, 2080/2081) nach dem Belegraum der Begleiter-Kette (3000 bzw. 3001, 0x63). Geaendert sind nur Bytes von 3020/3060.
- Nachbesserung 1 (Pruefer Kleinkram R1):
  - T-09 im Bild: das oeffnende Anfuehrungszeichen 0x61 (Original 0x47 @0x800c5e53, auch Taschenuhr F-19 Seite 2 und Original 0x07
    "Ruger & Company" mit 0x64) erschien als fremdes Zeichen. Ursache im PC-Inventar-Renderer: die Schriftseite s_font4 hielt nur
    128 Zeilen (`v & 127`), Glyphen ab Code 0x60 liegen bei v = (code & 0xf0) + 0x20 >= 0x80 (FUN_80028868). Gemessen: Savestate
    stage_saves/inv_states/psx_inv_check.sav, Schriftseite VRAM (768,256) == TEX.TIM Spalten 256..511, Zelle 0x61 bei v = 0x80 ist
    das Anfuehrungszeichen. Behoben in platform/pc/src/inv_render_pc.c (256 Zeilen); Riegel integration_r42_klein_glyphe_karte /
    _glyphe_uhr (Software-Framebuffer gegen die nachgezeichnete Seite, 0 abweichende Pixel; der alte Renderer weicht um 70 ab).
  - T-08: Ada und Marvin stehen in 6020 in keinem Kamerabild, das der Spieler erreicht (cam 0/1); der Beleg sind Spawn-Log und
    Aktorliste. Fuer L18 (Schutzraum-Dialog, v0.8.32) offen: Stellplatz bzw. Kamera der Szene.
  - H4: Abwaegung gegen ROOM2000/2001/11B0/11B1 im Kopf von tools/r42_story/nachlauf_r42.py (Punkt 4); Belegraum bleibt 3000/3001.
  - Zensus: Bits 38/39 (Bank 9, RE15_BF_HANGAR_BIT / RE15_BF_BESIEGT_BIT der Zweige r41/sichel_szene, r42/hangar_aussen,
    r41/birkin_kralle) stehen jetzt in ZWEIG_BITS; belege/zensus_r42.txt neu (76 statt 78 freie Bits).

## 5. Gemeinsame Dateien (nur kleine Haken, Kommentar "Runde 42 Story")
Wie Runde 41 (VERTRAG R41 1.3). Tabu: `synchro/`, `pri/`, `Cutscenes/`, `Version_80%/`, `release/`. Neue Bits nur
mit Eintrag in `re15_story_r42.h`, `zensus_r42.py` BITS, k_port[] und hier.
