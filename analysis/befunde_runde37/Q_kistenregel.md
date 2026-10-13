# Runde 37 — Spur Q kistenregel (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_kistenregel`, Zweig `r37/kistenregel`, Basis 6f360fb3 (= abgenommener Stand der Spur I
1d338f4e + AUFTRAG/VERTRAG vom master-Stand 3491155e).
Massgeblich: `AUFTRAG.md` "Orchestrator-Auftrag Q" (woertlich):

> Grund: Spur I (I_feuerloescher.md, Nachbesserung 1, offen) mass: legt Leon den Feuerloescher VOR dem Loeschen in die
> Item-Box, kann er das Feuer in ROOM1090 trotzdem loeschen ("Will you use the Fire Extinguisher?" -> Yes, Rettungsszene) —
> ROOM1090 sub00 @0x02306 `21 03 85 00` prueft nur das Aufnahme-Bit (3,0x85), nie das Inventar. [...] Beta -> Retail: in
> RE2 benutzt man ein Item nur, wenn man es TRAEGT. -> Spur Q kistenregel: Zensus ALLER Original-Abfragen (alle RDTs,
> Leon und Elza), die den BESITZ eines Items ueber ein Bit statt ueber das Inventar pruefen [...]; je Fall belegen, ob das
> Item in der Box liegen kann, und wie RE2 dieselbe Art Abfrage macht [...]. Wo die Box den Fall oeffnet: wie RE2 das
> Item im Inventar verlangen (eine Port-Regel an EINER Stelle, je Fall per Tabelle), mit RE2-Text, wenn es fehlt.
> Basis = abgenommener Stand der Spur I.

VERTRAG 1.4 (Spur Q): neue Logik in kistenregel_r37.c (Tabelle je Fall), Haken an der Stelle, wo das Original das Bit
prueft (scd_vm.c Ck-Pfad oder je Raum), feuerloescher_r37.c (Spur I) nur fuer den 1090-Fall; Texte nur mit RE2-Beleg
(Ids nach VERTRAG 1.2 begruenden).

## Stand
* 2026-10-12: Dossier, Zensus Schritt 1+2 (12e27805); Code-Stand beim Sitzungslimit vom Orchestrator UNGEPRUEFT gesichert
  (51a24d29: kistenregel_r37.c/.h, scd_vm.c-Haken, Riegel). Danach Wochenlimit.
* 2026-10-13 Fortsetzung: Baum sauber auf 51a24d29, `local_build.sh build` -> `ninja: no work to do` (der Vorgaenger
  hatte 18:09 noch gebaut: `kistenregel_r37.c.obj` vorhanden, exe 18:09, Testverzeichnis 18:11 mit den Laeufen a/b/c
  des Integrationsriegels, c durch das Limit abgerissen). Der gesicherte Code wurde Zeile fuer Zeile gegen 3.1 und 2.2
  geprueft (Abschnitt 4.1), die Riegel gefahren (Abschnitt 6), der Integrationsriegel neu gefahren (Abschnitt 5).

## 1. Messung vorher
Nicht wiederholt — vorhandene Messung am Stand OHNE Regel (Spur I, I_feuerloescher.md Nachbesserung 1, "Beobachtung",
Lauf v10): CONTINUE ROOM1090 (0,250) mit (9,0x86)/(3,0x85)/(3,0x80), Inventar leer, `kiste:0x31:1` -> `id=7`,
`id=8 "Will you use the Fire Extinguisher?"`, Ja -> Neueintritt, `id=9 "You've used the Fire Extinguisher."`,
Rettungsszene `id=3`/`id=4`. Das ist der Befund des Auftrags Q: Loeschen mit dem Loescher in der Item-Box.
Ursache = Original-SCD ROOM1090 sub00 @0x02306 `06 00 24 00` Ifel_ck / @0x0230A `21 03 85 00` Ck(3,0x85)==0: das
Skript fragt nur das abgeleitete Aufnahme-Bit (3,0x85), nie das Inventar (2.1).
Fuer die uebrigen Faelle K2..K11 gilt derselbe Mechanismus (der Ck liest nur das Bit, 3.1); die Gegenprobe "Regel aus"
(Abschnitt 6, Riegel ohne Haken gebaut) zeigt fuer jeden Fall den Benutzungs-Zweig trotz Kiste = das Verhalten vorher.

## 2. RE-Belege (selbst disassembliert / aus den RDT-Bytes)

### 2.1 RE1.5 kennt keinen Besitz-Opcode — Besitz = Bit
* SCD-Tabelle `PTR_LAB_800744a8`, 95 Eintraege 0x00..0x5E (letzter @0x80074620 -> 0x80042b04). 0x5E ist dort KEIN
  Praedikat (Vorarbeit Runde 34 Nacht A_rolltor.md 3.4): @0x80042b20 `ori v0,v0,0x20` / @0x80042b38
  `jal 0x80013278` / @0x80042b44 `ori v0,zero,0x1` — liefert konstant 1. Zensus: 0 Vorkommen in 206 RDTs.
* Die Inventar-Suche des EXE `FUN_8004dfec` (selbst: `re15_disasm.py dis 0x8004dfec 22`):
  ```
  8004dff0  lbu  v0,4028(v0)       ; Zaehler 0x800b0fbc
  8004e014  addiu at,at,4268       ; Inventar-Id 0x800b10ac + i*4
  8004e024  bne  v0,a0,0x8004e034  ; Id == gesucht?
  ```
  hat nur ZWEI Aufrufer (ghidra1_V2.txt XREF[2]): FUN_8004eb70 @0x8004ebbc und FUN_8004ebdc @0x8004ec34 — beide
  lesen den Munitions-Partner der ausgeruesteten Waffe (`re15_disasm.py dis 0x8004eb70 60`: @0x8004eb74
  `lbu v0,9672(v0)` = 0x800b25c8 Waffenplatz, @0x8004eba8 Tabelle 0x80074dac) = Nachlade-Pruefung. KEIN Skript- und
  KEIN Tuer-Weg fragt das Inventar.
* Tuer-Handler PTR_8007469c[2] = 0x800430bc hat keinen Schluessel-Test (Runde 36 Q_schluessel.md, Kopf von
  include/re15_schluessel_r36.h). Ein aufgenommener Gegenstand verlaesst das RE1.5-Inventar nie (kein Opcode entfernt
  Gegenstaende; Inventar-Zustand 6 @0x8004b250 schreibt keine Plaetze, A_rolltor.md 3.4). Darum genuegt im Original
  ein Bit als Besitz-Ersatz:
  - (T) TAKEN-Bit der Aufnahme (Bank 9): Item_aot_set pc[18] @0x80040680, gesetzt bei der Aufnahme @0x8001e0d0 /
    @0x80021fd8 (I_feuerloescher.md 2.1);
  - (A) ABGELEITETES Bit, das ein je Bild laufendes sub01 setzt, sobald das Taken-Bit steht (Muster ROOM1000 sub01
    @0x00D00 `21 09 86 01` -> @0x00D0C `22 03 85 01`);
  - (S) SKRIPT-Bit fuer Gegenstaende, die nie ins Inventar gehen ("You've taken the Fuse." ROOM2030 sub06
    @0x01FF2 `22 03 6c 01`).

### 2.2 RE2 Retail prueft den Besitz im INVENTAR, die Kiste zaehlt nicht
* `re2_disasm.py table 0x800a7640 2`: Opcode-Tabelle 0x800A74C8 + 0x5E*4 = 0x800A7640 -> **0x800584F0**
  (Keep_Item_ck). `re2_disasm.py dis 0x800584f0 24`:
  ```
  800584f8  lw   v0,28(a0)         ; PC
  80058500  lbu  v1,1(v0)          ; Id = pc[1]
  80058504  addiu v0,v0,2 / 80058508 sw v0,28(a0)    ; Satzbreite 2
  8005850c  jal  0x800696cc        ; a0 = Id
  80058514  nor  v0,zero,v0 / 8005851c srl v0,v0,31   ; Treffer (>=0) -> 1, -1 -> 0
  ```
  `re2_disasm.py dis 0x800696cc 20` (die Suche):
  ```
  800696d4  addiu a2,a2,18092      ; 0x800d46ac Zaehler (Inventargroesse)
  800696e4  lbu  v0,19004(at)      ; 0x800d4a3c + i*4 = INVENTAR-Id
  800696ec  beq  v0,a0,0x8006970c  ; gefunden -> Index
  80069708  addiu v0,zero,-1       ; sonst -1
  ```
  Das Box-Feld 0x800d4a68 (FUN_800703b8 @0x80070710, I_feuerloescher.md Nachbesserung 1) liest sie NICHT. Dieselbe
  Suche benutzt der RE2-Tuer-Handler LAB_80051514 (@0x80051628 `jal FUN_800696cc`, @0x80051634 `bltz`).
* RE2-Zensus ueber alle 250 RDTs / 2568 SCD-Bloecke (`tools/r37_kistenregel/re2_besitz_zensus.py`, Ausgabe
  `Q_belege/re2_besitz_zensus.txt`): 10x Keep_Item_ck, 34x Sce_item_lost, 5x Sce_item_ck_lost. Benutzungs-Stellen mit
  Frage haben die Form "schon benutzt? -> Besitz? -> Frage, sonst Raumtext":
  * **ROOM2080 sub13** (Datei @0x01724, roh): `21 04 af 00` Ck(4,0xAF)==0 (noch nicht benutzt) / `06 00 54 00` /
    **`5e 53` Keep_Item_ck(0x53 Special Key)** / `2b 00 02 00 00 ff` msg 2 "Will you use the special key?" /
    `06 00 34 00` `21 0b 1f 00` Ja / `62 53` Sce_item_lost / `22 04 af 01` / `2b 00 03 00 00 ff` msg 3 "You've used
    the special key." ... `07 00 0e 00` ELSE -> **`2b 00 01 00 00 ff` msg 1 "It's locked. A special kind of key is
    required."**
  * **ROOM6150 sub02** (@0x02D3C): `21 04 9f 01` Ck(4,0x9F)==1 (schon benutzt — ZUERST) ... `07 00 60 00` ELSE /
    `06 00 4a 00` / **`5e 3f` Keep_Item_ck(0x3F W. Box Key)** / `2b 00 0a 00 ff ff` msg 10 "Will you use the Weapon
    Box Key?" ... `07 00 10 00` ELSE -> **`2b 00 00 00 ff ff` msg 0 "I could open it if I had the key."**
  Ergebnis: in RE2 wird ein Gegenstand nur benutzt, wenn er im INVENTAR ist; die "schon benutzt"-Pruefung steht VOR
  der Besitz-Pruefung (ROOM6150); fehlt er, zeigt der Raum einen eigenen Text (keine Frage).

### 2.3 Der Ck (0x21) ist der EINZIGE Bit-Abfrage-Weg der Raumskripte (Fortsetzung 2026-10-13, neu)
Neben Ck LAB_8003fcf4 hat RE1.5 einen zweiten Bit-Test: 0x58 = LAB_8003fd54 (Bank pc[1], Bit aus einer Arbeitsvariablen,
RE15_SCD_OPCODES_REFERENCE.md Zeile 0x58), dazu 0x59 = LAB_8003fe90 (indirektes Setzen). Zensus mit derselben
Laengentabelle (tools/scd_walk_lib.py `regionen`/`walk_ops`) ueber alle ROOM*.RDT: **0x58: 0 Vorkommen**, **0x59: 2**
(ROOM1030 sub09 @0x02822 `59 05 04 01`, ROOM1031 sub09 @0x0290E `59 05 04 01` — Bank 5, raumlokal, kein
Besitz-Bit). Damit erfasst der Zensus der Ck-Stellen (3) alle Bit-Abfragen der Raumskripte; ein Haken im Ck-Opcode
(scd_vm.c op_ck = LAB_8003fcf4) ist die EINE Stelle, an der das Original den Besitz prueft.

## 3. Zensus (Schritt 1) — alle RDTs, Leon und Elza

Werkzeug `re15_port/tools/r37_kistenregel/kistenregel_zensus.py` (Walk mit der EINEN Laengentabelle aus
tools/scd_dump_room.py, Blockstapel Ifel/Else/Schleifen; Namen DEBUG.BIN @0x800C495C/@0x800C4A28). Ausgabe:
`Q_belege/re15_zensus.txt`. 206 RDTs mit Kopf (+34 Stummel), 164 Item_aot_set, 0 Desync.
* (T) 27 Gegenstaende mit Platzierung; (A) 16 abgeleitete Bit-Saetze (8 Bits); (S) 21 Skript-Bit-Saetze.
* **107 Ck-Stellen auf T/A/S-Bits**, je Stelle eingeordnet (3.1-3.3). Gegenprobe von der Text-Seite (Abschnitt 5 der
  Ausgabe): jede Message_on mit "use/used/insert/operate" liegt in einem Sub, den ein Aot_set (sce 3/5) startet, das
  unter einer der Ck-Stellen aus 3.1/3.2 angelegt wird. "Use the Research Center ID Card?" (ROOM40A0 msg 4) und
  "There's a driver here. Take it?" (ROOM1190 msg 1) gibt kein Message_on/Aot_set aus (tote Texte, kein Gegenstand).

### 3.1 Benutzungs-Stellen mit INVENTAR-Gegenstand (die Box kann ihn halten) — 20 Ck-Stellen, 11 Faelle
"fertig" = das Bit, das die Benutzung setzt und das der Raum selbst als "schon benutzt" liest; "Text" = der
Original-Text des Raums fuer "nicht im Besitz" (der Zweig, den das Skript ohne Bit nimmt).

| # | Raum/Stelle (Datei-Offset des Ck) | Bit | Gegenstand | fertig (gesetzt @) | Text ohne Gegenstand |
|---|---|---|---|---|---|
| K1 | 1090 sub00 @0x0230A `21 03 85 00` | (3,0x85) A (1000 sub01 @0x00D0C; Port-Fundort 1050, Spur I) | 0x31 Fire Extinguisher | (3,0x81) sub06 @0x0271E | msg 7 "I must hurry up and get something to put out this fire to save that woman!" (Slot 2 sce 1 @0x0230E) |
| K2 | 10D0 sub20 @0x019C0 / 10D1 sub20 @0x019AA `21 09 34 01` | (9,0x34) T (1110/1111 main00 @0x00B2E) | 0x38 Blue Keycard | (3,0x32) @0x0152A; Leser nur bei (3,0x32)==0 (main00 @0x01026) | msg 8 "You have not the Blue Keycard / to operate it." (ELSE @0x019D2 / @0x019BC) |
| K3 | 1230/1231 sub20 @0x01488 `21 09 88 01` | (9,0x88) T (1190/1191 sub13 @0x02A30/@0x02AA2) | 0x37 Red Keycard | (3,0x89); Leser nur bei (3,0x89)==0 (main00 @0x00B98) | msg 8 "You have not the Red Keycard / to operate it." (ELSE @0x0149A) |
| K4 | 11E0/11E1 sub20 @0x01FA8 `21 09 8a 01` | (9,0x8A) T (1011 main00 @0x00954) | 0x39 Yellow Keycard | (3,0x8B) @0x01A8A; Leser nur bei (3,0x8B)==0 (main00 @0x01576) | msg 8 "You have not the Yellow Keycard / to operate it." (ELSE @0x01FBA) |
| K5 | 11E0/11E1 sub00 @0x01798 `21 03 7f 00` | (3,0x7F) A (11B0 sub01 @0x011EA / 11B1 @0x01186) | 0x30 Pliers | (4,0x24) sub21 @0x01FC6 | msg 10 "If I had some kind of pliers maybe I / could cut that wire and disable the safety lock." (Slot 6 sce 1 @0x0179C) |
| K6 | 1100/1101 sub00 @0x00AC8 `21 03 67 00` | (3,0x67) A (1200/1201 sub01 @0x0095A) | 0x44 Minidisc Player w/ Disc | (4,0xE8) sub02 @0x00C68 | msg 1 "It's a control panel. A type of voice device is required / to open the lock." (Slot 2 sce 1 @0x00AD6) |
| K7 | 3010/3011 main00 @0x020AE `21 03 3b 00` | (3,0x3B) A (3040/3041 sub01 @0x014DA) | 0x36 Green Keycard | (3,0x3C) sub02 @0x02302 | msg 0 "It's locked. An ID card is required to open it." (Slot 6 sce 1 @0x020B2) |
| K8 | 3050/3051 main00 @0x01F90 `21 03 b2 00` | (3,0xB2) A (30A0/30A1 sub01 @0x0086C) | 0x46 Red Master Keycard | (3,0xB3) sub15 @0x0260E | msg 3 "The computer seems to be working. There is a note: ... use the Red Master Keycard to restore the system." (Slot 10 sce 1 @0x01F94) |
| K9 | 4000 main00 @0x01128 `21 03 65 00` + sub01 @0x01374 `21 03 65 01` | (3,0x65) A (4010 sub01 @0x006EC) | 0x47 Blue Master Keycard | (3,0x20) sub02 @0x01450 | msg 0 "The door is locked. The text on the / panel reads: Door is under emergency lockdown. / An ID card is required to unlock it." (Slot 6 sce 1 @0x0112C, Slot 2 weg @0x01140) |
| K10 | 4030/4031 main00 @0x004FA `21 03 57 00` | (3,0x57) A (4040/4041 sub01 @0x01020) | 0x41 Spark Plug | (4,0x19) sub06 @0x006D2 | msg 0 "Main Fuse: It's a large circuit breaker system. It looks like someone has removed all / the fuses." (Slot 6 sce 1 @0x004FE) |
| K11 | 1211 sub00 @0x01DEA `21 04 ec 00` | (4,0xEC): im Original S (1011 sub06 @0x00E2C, KEIN Gegenstand); im PORT setzt es die Aufnahme des Prison Key 0x42 in 10F1 (Runde 36 Q, schluessel_r36.c re15_schluessel_r36_genommen) | 0x42 Prison Key (Port) | (4,0xFC) sub02 @0x01F76 | msg 10 "It's locked. It seems that there is someone / inside..." (Slot 8 sce 1 @0x01DEE) |

* Elza-Gegenstuecke: K2 10D1, K3 1231, K4 11E1, K5 11E1, K6 1101, K7 3011, K8 3051, K10 4031, K11 nur Elza. K1 und K9
  nur Leon (1091: Platz schon geloescht, I_feuerloescher.md 2.4; 4001 prueft (3,0x65) nicht).
* K4: fuer LEON tot — ROOM1240 sub02 @0x0055A `22 03 8b 01` setzt (3,0x8B) beim neuen Spiel, 11E0 main00 @0x01576 nimmt
  dann den ELSE-Zweig (fertige Tuer). ROOM1241 setzt es NICHT (roher Zensus `22 03 8b 01` ueber alle RDTs: nur
  11E0/11E1 @0x01A8A und 1240 @0x0055A) -> bei ELZA ist der Leser 11E1 erreichbar, K4 gilt.
* Kann der Gegenstand in der Box liegen? JA fuer alle elf: die RE2-Box des Ports nimmt jeden Gegenstand (re15_itembox.h:
  einzige Abweisung = 2-Zellen-Waffe @0x800707dc-ec), alle elf sind Inventar-Gegenstaende.
* Box-Raeume (re15_itembox.c s_boxpoints: 1150/1151, 2010/2011, 30A0/30A1, 30B0/30B1, 4010/4011, 5010/5011,
  6020/6021, 6030/6031) und Benutzungs-Raeume ueberschneiden sich nicht -> im Benutzungs-Raum aendert sich der Besitz
  nur durch die Benutzung selbst; die Pruefung beim Raumstart (main00/sub00) ist gleichwertig zur Pruefung beim Druck.

### 3.2 Benutzungs-Stellen OHNE Inventar-Gegenstand (die Box kann ihn NICHT halten) — keine Regel
| Raum/Stelle | Bit | Gegenstand | Warum keine Wirkung |
|---|---|---|---|
| 2020/2021 sub00 @0x00954 `21 03 72 00` | (3,0x72) S (2010/2011 sub02 @0x008CA/@0x008E8) | Crank | nur Bit ("You've taken the Crank."), kein Item_aot_set, keine Port-Aufnahme (Port-Aufnahmen = grep re15_aot_set_item: dokumente_r34, feuerloescher_r37, inhalt_r35, irons_tisch_1150, schluessel_r36, scd_vm op_item_aot_set) |
| 2040/2041 sub00 @0x0141A `21 03 92 00` | (3,0x92) S (1110/1111 sub03 @0x00D32) | Time Bomb | dito |
| 2060/2061 sub00 @0x010A2 `21 03 6c 00` | (3,0x6C) S (2030/2031 sub06 @0x01FF2) | Fuse (STAGE2) | dito; die Port-Sicherung 0x40 (sicherung_1150.c) gilt nur ROOM1050 und prueft dort schon das Inventar (rolltor_1050.c `re15_inv_find_item`) |
| 3091 sub02 @0x01596 `21 09 15 00` | (9,0x15) S (3091 sub06 @0x016C2) | Latch Key | dito; nicht in der 102er-Namenstabelle |

### 3.3 Keine Benutzung (Aufnahme-Folgen, Story) — keine Regel
Ableitungs-Setzer (1000/11B0/11B1/1200/1201/3040/3041/30A0/30A1/4010/4040/4041 sub01: die (T)- und (A)-Ck darin),
Aufnahme-Hygiene (1051 @0x00CB6, 11F0/11F1 sub19, 2040/2041 sub01 @0x015F2, 3050/3051 sub01 @0x0223C, 40A0/40A1
main00/sub02, die Aufnahme-Plaetze 2010/2011/2030/2031/1110/1111/5030/5031/3091 sub06), Story nach einer Aufnahme
(11D0/11D1 main00 @0x012BA + sub03 Beretta-Szene, 1200/1201 main00 @0x00800/@0x00852 + sub03, 5060/5061 Impfstoff,
1011 Roy (3,0x78)/(4,0xFA), 4010 sub00 (4,0xFA), 5011 sub00 (4,0xF8), 3070 sub00 @0x03408 (4,0xEC) = Leons STAGE3-Szene
mit Sce_em_set Typ 0x30/0x10, keine Benutzung).

### 3.4 Was schon ueber das INVENTAR prueft (bleibt)
Original: nur die Nachlade-Pruefung (FUN_8004dfec, 2.1). Port: Rolltor ROOM1050 (rolltor_1050.c
`re15_inv_find_item(RE15_SICHERUNG_ITEM)` = RE2 Keep_Item_ck), Parkplatz-Tuer 11B0 (schluessel_r36.c
`re15_inv_find_item(RE15_R36Q_PARK_ITEM)` = RE2 @0x80051628). Port-Code, der Bank-9-Bits liest (grep
`re15_game_flag_get(9`): nur Aufnahme-Hygiene (dokumente_r34, feuerloescher_r37, granate_1150, inhalt_r35,
irons_tisch_1150, leiche_1110_1230, schluessel_r36, sicherung_1150, tuer1060_1040 Protokoll) — keine Benutzung.

## 4. Umsetzung

### 4.1 Entscheidung je Fall (Zensus 3 -> Regel)
| Fall | Raeume | Entscheidung | Grund |
|---|---|---|---|
| K1..K11 (3.1) | 1090; 10D0/10D1; 1230/1231; 11E0/11E1 (2 Stellen); 1100/1101; 3010/3011; 3050/3051; 4000 (2 Stellen); 4030/4031; 1211 | **REGEL**: Besitz-Bit gilt nur mit dem Gegenstand im INVENTAR (oder wenn die Benutzung schon geschehen ist) | Inventar-Gegenstand, die Box nimmt ihn (einzige Abweisung 2-Zellen-Waffe @0x800707dc-ec, re15_itembox.h); RE2 prueft dieselbe Art Benutzung mit Keep_Item_ck @0x800584F0 -> FUN_800696CC nur im Inventar (2.2) |
| Crank 2020/2021, Time Bomb 2040/2041, Fuse 2060/2061, Latch Key 3091 (3.2) | — | keine Regel | Gegenstand geht nie ins Inventar (nur Skript-Bit), die Box kann ihn nicht halten |
| Ableitungs-Setzer, Aufnahme-Hygiene, Story-Folgen (3.3) | — | keine Regel | keine Benutzung; eine Sperre dort wuerde z.B. die Ableitung (3,0x85) selbst verhindern |
| Port-Inventar-Abfragen (3.4) | Rolltor 1050, Parkplatz 11B0 | bleiben | pruefen schon das Inventar |

"RE2-gleiche Reaktion": RE2 zeigt ohne Gegenstand einen RAUMTEXT statt der Frage (ROOM2080 sub13 msg 1, ROOM6150 sub02
msg 0 — die beiden Benutzungs-Stellen MIT Frage im RE2-Zensus; die uebrigen 8 Keep_Item_ck schalten raumlokale
Zustaende: 4010 sub06 Small Key gegen Dietrich, 7020 sub01 Stecker, 6010 sub00 Platz-Aufbau, 10B0 sub05, A020 main00;
keiner ruft einen allgemeinen "Gegenstand fehlt"-Text auf, Q_belege/re2_besitz_zensus.txt). Das
RE1.5-Gegenstueck ist der Zweig, den jedes der elf Raumskripte selbst fuer "nicht im Besitz" hat (Spalte "Text ohne
Gegenstand" in 3.1) — die Regel schickt das Skript genau dorthin. Folge: KEIN neuer Text, KEINE neue Nachrichten-Id
(VERTRAG 1.2 nicht beruehrt), kein neues Bit (VERTRAG 1.1), kein RDT-Patch. Die Reihenfolge "schon benutzt" vor
"Besitz" ist die von RE2 ROOM6150 sub02 (@0x02D3C `21 04 9f 01` vor @0x02D64 `5e 3f`).

### 4.2 Code (gesicherter Stand 51a24d29, am 2026-10-13 Zeile fuer Zeile geprueft, unveraendert uebernommen)
* `re15_port/engine/src/kistenregel_r37.c` (neu): Tabelle `k_faelle[20]` = die 20 Ck-Stellen aus 3.1 {Raum,
  Datei-Offset des Ck-Satzes, Bank, Bit, pc[3], Item, fertig-Bank/-Bit, K-Nummer}. Jede Zeile gegen 3.1 abgeglichen
  (Raum, Offset, `21 bank bit wert`, Item-Id der 102er-Tabelle DEBUG.BIN @0x800C495C, fertig-Bit); Elza-Zeilen tragen
  die Offsets ihrer eigenen RDT (10D1 @0x019AA, sonst gleich) — der Riegel `unit_r37_kistenregel_tabelle` liest die
  Bytes an jedem Offset aus der ausgelieferten RDT. `re15_kistenregel_r37_ck(raum, off, bank, bit, got)`:
  1. nicht in der Tabelle -> `got` (alle anderen Ck aller Raeume unveraendert);
  2. `got == 0` -> 0 (nichts zu sperren); fertig-Bit gesetzt -> `got` (RE2-Reihenfolge ROOM6150 @0x02D3C);
  3. `re15_inv_find_item(item) >= 0` (sucht NUR g_inv = RE1.5 FUN_8004dfec @0x8004e014 / RE2 FUN_800696CC @0x800696e4)
     -> `got`; sonst 0 + Zaehler `g_re15_kr37_gesperrt_zaehler`.
  Messzeile `[kistenregel] ROOMxxxx @0x..... Ck(b,0x..) Kn: Item 0x.. NICHT im Inventar (Item-Box Platz p) -> Besitz 0,
  Zweig ohne Gegenstand` bzw. `... im Inventar -> wie Original` (nur PC, nur bei Wechsel der Entscheidung — K9 sub01
  @0x01374 laeuft je Bild).
* `re15_port/include/re15_kistenregel_r37.h` (neu): Typ, Prototypen, Herleitung mit allen Adressen.
* `re15_port/engine/src/scd_vm.c` (gemeinsame Datei, Haken 2 Zeilen + Kommentar "Runde 37 Spur Q", VERTRAG 1.4 "Ck-Pfad"):
  in `op_ck` (= LAB_8003fcf4) direkt nach `re15_game_flag_get`, VOR der Bedingung `cond = (got!=0) ^ (value==0)`
  (@0x8003fd48 `sltu` / @0x8003fd50 `xor`) — so gilt die Regel fuer beide Formen `Ck(..)==0` (Zweig "nicht im Besitz"
  wird wahr) und `Ck(..)==1` (Else-Zweig). Schluessel = `t->pc - s_current_rdt->raw` (Datei-Offset; nur fuer Skripte
  im RDT-Puffer, Port-Programme ausserhalb des Puffers bleiben unberuehrt).
* feuerloescher_r37.c (Spur I) NICHT geaendert: Der 1090-Fall braucht keinen Eingriff dort — das Tor des leeren
  Loeschers bleibt an (3,0x81) = Ja gebunden (sub06 @0x0271E), die Regel greift VOR der Frage.
* Keine Assets, keine Texte, keine Bits, keine Ereignisnummern, keine Installer-Zeile in scd_room_setup.c.
* Grenze (gemessen am Raumbestand, nicht per Code abgefangen): Box-Raeume (re15_itembox.c s_boxpoints 1150/1151,
  2010/2011, 30A0/30A1, 30B0/30B1, 4010/4011, 5010/5011, 6020/6021, 6030/6031) und die elf Benutzungs-Raeume sind
  disjunkt -> im Benutzungs-Raum aendert sich der Besitz nur durch die Benutzung selbst; die Auswertung beim Raumstart
  (main00/sub00) ist darum gleichwertig zur Auswertung beim Druck.

### 4.3 Gegenprobe "Regel aus" = Verhalten VORHER fuer alle elf Faelle (2026-10-13)
Haken-Zeile in scd_vm.c op_ck voruebergehend durch `(void)0;` ersetzt, `local_build.sh build`, dieselben Riegel:
* `test_r37_kistenregel raumstart`: **58 Pruefungen, 16 Fehler** — fuer JEDEN Fall legt der Raumstart mit dem Gegenstand
  in der Kiste den BENUTZUNGS-Platz an (Auszug woertlich):
  `FAIL: K1 1090 KISTE: Platz 2 = EVENT ev 6 (soll Raumtext msg 7), Regel griff 0 x`,
  `FAIL: K5 11E0 KISTE: Platz 6 = EVENT ev 26 (soll Raumtext msg 10)`, `FAIL: K6 1100 KISTE: Platz 2 = EVENT ev 7
  (soll Raumtext msg 1)`, `FAIL: K7 3010 KISTE: Platz 6 = EVENT ev 2 (soll Raumtext msg 0)`, `FAIL: K8 3050 KISTE:
  Platz 10 = EVENT ev 10 (soll Raumtext msg 3)`, `FAIL: K9 4000 KISTE: Marker Slot 2 weg (Aot_reset @0x01140), ist
  MARKER(sce5)`, `FAIL: K10 4030 KISTE: kein Marker Slot 5, ist MARKER(sce5)`, `FAIL: K11 1211 KISTE: kein
  Befreiungs-Platz Slot 6, ist AUTO` (dazu die Elza-Raeume 11E1/1101/3011/3051/4031 und die Raumtext-Zeilen 4000/4030/1211).
* `test_r37_kistenregel ablauf`: **23 Pruefungen, 12 Fehler** — Druck auf den Platz mit dem Gegenstand in der Kiste:
  `FAIL: K1 1090 KISTE: Druck auf Platz 2 -> Nachrichten [7,8,9,3,4]` (Frage, Loeschen, Rettungsszene),
  `FAIL: K2 10D0 KISTE: ... [7,9,0]` ("You've used the Blue Keycard."), dito 10D1, 1230, 1231, 11E1 (K4),
  `FAIL: K5 11E1 KISTE: ... [10,11]`, `FAIL: K6 1100 KISTE: ... [1,3]`, `FAIL: K7 3010 KISTE: ... [1]`,
  `FAIL: K8 3050 KISTE: ... [3,4]`, `FAIL: K11 1211 KISTE: ... [-]` (Befreiung statt Raumtext),
  `FAIL: K1 1090 KISTE: Feuer brennt weiter ((3,0x81)=1, (3,0x84)=0)`.
* `test_r37_kistenregel spielstand`: **4 Pruefungen, 1 Fehler** (`nach dem Laden: Platz 2 = msg 7 ..., ist EVENT ev 6`).
* `tabelle`: 54/0 (prueft nur Tabelle gegen RDT-Bytes, haengt nicht am Haken).
* `integration_r37_kistenregel` an der exe desselben Gegenprobe-Baus (`ctest -R "^integration_r37_kistenregel$" -V`,
  Laeufe `lauf_509d4551_*`): `0% tests passed, 1 tests failed out of 1`; woertlich u.a.
  `FAIL A: unerwartet 'id=8 "Will you use the Fire Extinguisher?"'`, `FAIL A: unerwartet 'id=9 "You've used the Fire
  Extinguisher."'`, `FAIL I: unerwartet 'id=8 ...'` / `'id=9 ...'` (ECHTE Box ROOM1150: debug.log
  `[msg] room=1150 id=3 ITEMBOX (box screen, message suppressed)`, `[debug-menu] AUTO-JUMP -> ROOM1090 (Frame 330)`,
  `[msgtext] room=1090 id=7 ...`, `id=8 "Will you use the Fire Extinguisher?"`, `id=9 "You've used the Fire
  Extinguisher."`), `FAIL C: unerwartet 'id=9 "You've used the Blue Keycard."'`, `FAIL E: unerwartet 'id=9 "You've used
  the Yellow Keycard."'`, `FAIL F: fehlt 'id=10 "It's locked.It seems that there is someone inside..."'` (Platz 8 leer,
  statt dessen der Befreiungs-Platz), `FAIL G: unerwartet 'id=1 "You've used the Green Keycard."'`, `FAIL H: fehlt
  'id=0 "Main Fuse"'`; die Gegenproben B/D (Gegenstand im Inventar) sind dort wie mit Regel gruen.
Mit dem Haken (Endstand): alle vier Teile 0 Fehler, Integrationsriegel gruen (Abschnitte 5/6). Die Riegel messen also
die Regel selbst. Danach scd_vm.c per `git checkout` zurueck, neu gebaut (`LOCAL-BUILD-OK (build)`).

### 4.4 Riegel anderer Spuren, die "Bit = Besitz" annahmen (gemessen, nachgezogen)
Erster gezielter Lauf am Endstand (82 Tests, Regex Abschnitt 6) + Zensus aller Test-Quellen, die eines der elf
Besitz-Bits setzen (grep `flag_set(3,0x85|0x7F|0x67|0x3B|0xB2|0x65|0x57)`, `(9,0x34|0x88|0x8A)`, `(4,0xEC)`, dezimal
und `flag:b:n`-Kartenargumente; zweiter Lauf ueber die Tests dieser Quellen): vier Riegel rot, alle mit demselben Grund
— sie setzen das Aufnahme-Bit OHNE den Gegenstand ins Inventar zu legen (der Zustand vor der Item-Box, in dem das Bit
allein den Besitz bedeutete). Im Spiel kommt "Bit ohne Gegenstand" nicht vor: der Gegenstand verlaesst das Inventar nur
ueber die Kiste (Q-Fall) oder ueber Wegwerf-Ja, das erst NACH der Benutzung angeboten wird (gen/discard_sites.inc; dann
steht das fertig-Bit und die Regel laesst das Original-Bit gelten).
| Riegel | alt (rot am Endstand) | Aenderung | neu |
|---|---|---|---|
| `unit_1090_flame_out_pin` (test_1090_flame_out_pin.c) | `FAIL: die brennenden Truemmer sind weg (7 Emitter)` u.a., 4 Fehler: ohne Loescher im Inventar legt sub00 den Loesch-Platz + Selbst-Tuer @0x02352 nicht an | Vorbedingung "aufgenommen" = `(3,0x85)` + `re15_inv_grant(0x31,1)` | gruen |
| `unit_r36_schluessel_zelle` (test_r36_schluessel.c) | `FAIL: Z ROOM1211 mit (4,0xEC) (= Prison Key genommen): John ..., Slot 6 -> sub02` | "genommen" = Bit + Item 0x42 im Inventar (`s_hf_gabe` in hochfahren) | gruen |
| `unit_r36_schluessel_befreiung` Pruefung O | alt erwartet "Schluessel nicht im Inventar (z.B. Item-Box): Szene bis zum Ende, keine Abfrage" — genau der Q-Fall | Erwartung auf die Q-Regel umgestellt: keine Befreiung (Set(4,0xFC) nie), Platz 8 = Raumtext msg 10, keine Abfrage | gruen (`ok: O Schluessel nicht im Inventar: keine Befreiung (Set(4,0xFC) Bild -1, msg 9 Bild -1), Platz 8 = Raumtext msg 10, keine Abfrage ...`) |
| `unit_r36_elza_unlock` (test_r36_elza.c) | Szene erreicht die Aufschliess-Geste @0x020F8 nicht | "Prison key" = Bit + Item 0x42 im Inventar (`s_boot_gabe` in room_boot) | gruen |
Die Pruefung O war eine Erwartung der Runde 36 (Nachbesserung 1, Wegwerf-Abfrage) fuer einen Zustand, den der
Orchestrator-Auftrag Q jetzt ausdruecklich verbietet (Benutzung ohne Gegenstand im Inventar); ihr eigentlicher Kern
(keine Wegwerf-Abfrage ohne Besitz, RE2 @0x80051634) bleibt geprueft.

## 5. Messung nachher
(laufend)

## 6. Tests
(laufend)

## 7. OFFEN
(laufend)

## 8. Fuer den Nutzer
(laufend)
