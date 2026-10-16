# Runde 38 — Spur H "elzatoene" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_elzatoene`, Zweig `r38/elzatoene`, Basis e2cc7673 (= master nach v0.8.25 2077bb8e + Doku).

## Punkte (Wortlaut, AUFTRAG.md massgeblich)
1. "In Elzas Szenario im ROOM 1040 bei *Gunshot* fehlt der Sound für einen Gunshot."
2. "Wenn Roy in der Cutscene in Room 1020 auf John zuläuft soll er schon irgendwie ein Zombie stöhnen machen."
3. "Wenn John dann in der Cutscene in Room 1020 auf Roy schießt, muss auch ein Schusssound kommen"

Lesart (VERTRAG/AUFTRAG): Elza-Raeume = ROOM1041 / ROOM1021 (Elza-RDT = Leon-RDT + 1, PSX.EXE @0x800397e4/@0x800397ec).

## 0. Stand
- [x] Pflichtlektuere (AUFTRAG/VERTRAG/UEBERGABE R38, L_elza + L_abnahme_0 R36, K_roy + N_elzaems R37)
- [x] Bau Basis e2cc7673 im Spurbaum (local_build.sh configure + build, LOCAL-BUILD-OK)
- [x] Messung vorher Punkt 3 (Testweg, echter Tuerweg, CONTINUE, CAP_SYNC und Echtzeit-SDL) — §1
- [x] RE Punkt 3: Ursache = die gespielte AUFNAHME (Runde-36-Satz = Schuss der Partnerin Ada in RE2 ROOM40A0, keine
  Waffenaufnahme des Spiels) — §2.3, berichtigt in Nachbesserung 1 §8.1/§8.2
- [x] RE Punkt 1/2 — §2.1/§2.2
- [x] Umsetzung (§3), Messung nachher (§4), Riegel (§5: 6 Unit + 2 Integration, gruen)
- [x] gezielte Regression (§5)

## 1. Messung vorher (Bau e2cc7673, Messkopie re15_pc_r38h.exe, Arbeitsordner im Scratchpad)

Laeufe (alle Elza, RE15_ELZA_LOG + RE15_SE_DEBUG; Mitschnitt RE15_AUDIO_CAP_SYNC = 1470 Stereo-Frames je Spielbild):

| Lauf | Weg | Ergebnis |
|---|---|---|
| m1 | Testweg der Runde 36: Neuspiel, RE15_SET_FLAG=4:0x60,3:0xC9, GOTO 1021, FORCE_EVENT=2@40 | Anker F724/F754 (+Roy F754); `[se] Stimme: se=1 ... vag=1 note=69 fine=109 center=91 vol=100 pitch=0x4f3 (13641 Hz) -> SE-Stimme -16` zweimal; Mitschnitt Spitze 6356..6742 (Schuss 1) / 6783..7706 (Schuss 2), Hintergrund 500..1250 |
| m3 | ECHTER Weg: Spielstand ROOM1041 (Tuer 3 -> 1021, Flags 4:0x60 3:0xC9 4:0xF1 3:0xAD), CONTINUE, FIRE_AOT 3, Elza laeuft per INPUT_SCRIPT `L0.25,U0.8,R0.25,U6` ins Auto-Feld AOT 5 (Szene startet ~F268) | Anker F939/F969; dieselben `[se]`-Zeilen; Mitschnitt Bild 900..937 Spitze 0 (Stille), **Bild 940 Spitze 6450, Bild 972 7198**, Roy 1004 7694 |
| m5 | wie m3, aber ECHTZEIT-SDL-Mitschnitt RE15_AUDIO_CAP (kein CAP_SYNC) | Knall bei 44,37 s Spitze 6450 nach Stille — auch der Geraete-Pfad spielt den Ton |
| m4 | Vergleich: Elza schiesst selbst (Browning, RE15_GIVE=3:15 RE15_EQUIP=3, `M1,MX0.1`) in 1021 | Elzas Schuss = RE1.5 ARMS03 Satz 0 (2 Lagen VAG 2, 31183 Hz, vol 90) |

A-bewertete Lautheit (lauteste 100 ms, dB rel. Vollaussteuerung; scratchpad py/laut.py): Johns Schuss **-20,9 dB(A)**, Elzas
eigene Pistole **-19,4 dB(A)**, Sprachzeilen der Szene -11,6..-13,8 dB(A), Schritte -39,9 dB(A). Der Schuss erklingt also
mechanisch, im echten Weg, im Geraetepfad, und etwa so laut wie Elzas eigene Waffe.

Weitere Pruefungen: elza_r36.c / elza_se_pc.c / re15_elza_r36.h / ELZASE.VBS seit v0.8.24 unveraendert
(`git log e8b3958f..HEAD` leer); das v0.8.25-Paket enthaelt shared_assets/RE2/ELZASE.VBS (13144 B) und synchro/STAGE1/
room1021/main00..08, room1041/main02; Anker armiert in jedem Weg ("[elza] ROOM1021: 3 Anker armiert"). => KEINE
Regression (Haken, Bytes, Laden, Geraetepfad gemessen) — der Fehler liegt im gespielten TON selbst (§2.3).

Kamera waehrend der Schuesse (m6, RE15_FRAMEDUMP, WINDOW_SCALE=1): Cut 10 vom Gang hinter Roy; John steht klein am
Gangende, kein Muendungsfeuer — der Schuss ist im Bild nur am Ton erkennbar.

## 2. RE-Belege

### 2.1 Punkt 1 — ROOM1041 "*gunshot*"
* ROOM1041 sub11 (@0x021EA..0x0223C), gestartet von AOT 8 `2c 08 03 41 .. ff 00 18 0b` (main00 @0x01178, sce 3, auto) nur
  wenn Ck(4,0xF1)==0 (@0x0116C) und Ck(3,0xAD)==0 (@0x01174):
  `22 03 ad 01` Set(3,0xAD) @0x021EA, Aot_reset 8, Set(2,7)/(1,0x1b) (Szene),
  **@0x02200 `2b 03 00 00` Message_on 3** = msg 3 @0x022FB `04 00 5f 43 51 4a 4f 44 4b 50 5f 04 01 01 31` = "*gunshot*" (Endecode `01`, Folgebyte
  `31` so in der Datei; Nachbesserung 1 berichtigt, vorher falsch `00` zitiert — Abnahme 0 §4.2)
  (Maske 0 = kein Einfrieren), dann `2e 01 00 00` Work_set(1,0), Sleep 25, Plc_dest (Elza dreht sich), Warten Ck(5,0x20),
  Sleep 25, **@0x02224 Message_on 2** "Elza: What was that!?" (synchro room1041/main02.wav).
* Der Text ist ein PLATZHALTER fuer einen Ton: Zensus aller Nachrichten (msg_suche.py: 1229 RE1.5 + 2315 RE2): "gunshot"
  steht NUR in ROOM1041 msg 3, in keiner RE2-Nachricht (Suchmuster gunshot|shot|what was). RE1.5 selbst setzt Geraeusche, auf die eine
  Figur reagiert, als Se_on: ROOM3091 sub07 @0x01A62/@0x01A72/@0x01A82/@0x01A92 `36 02 0d 01 ..` (vier Se_on), danach
  Plc_neck/Plc_dest und @0x01AEE msg 22 "Sherry: What was that?". Nutzer-Regel derselben Runde (AUFTRAG, John/"Help"):
  Geraeusche sind Ton, kein Text.
* Ton: derselbe RE2-Skript-Pistolenschuss wie Punkt 3 (§2.3) — Elza hoert einen Schuss aus dem Nachbarbereich; RE2 spielt
  Skript-Schuesse als Raumbank-Se_on ohne Text (ROOM4040/4090/6020, §2.3).

### 2.2 Punkt 2 — Roys Zombie-Stoehnen beim Zugehen (ROOM1021 sub02)
* Roy = Gegner 1 (Sce_em_set Slot 1 @0x01E0A `44 01 47 ..`), sub02 @0x020AE `2e 02 01 00` Work_set(2,1),
  **@0x020B2 `40 00 04 22 4e ef 42 e6` Plc_dest** -> (-4274,-6590) = Johns Platz (Sce_em_set @0x01DF6), @0x020BA Plc_flg(1,0x14),
  @0x020BE Sleep 40, @0x020C6 John zielt (Sleep 110), @0x020CE Schuss 1. Roy geht also 150 Bilder bis Schuss 1.
* RE1.5-Original: kein Se_on in sub02 (Zensus Runde 36: alle 284 Se_on der RDTs auf Bank 2, keiner in 1021).
  NUTZER-MANDAT 2026-08-23 (enemy_ai_re2_zombie.c:766): Zombie-Toene = RE1.5-Raumbank snd1.
* Wann stoehnt ein GEHENDER Zombie im Original (STAGE1.BIN, re15_disasm.py): beim EINTRITT in den Gehzustand, nie waehrend
  des Gehens. FUN_801021f8 (Engage-Gang, Tabelle @0x8011f890[2]):
  `80102210 lbu v0,6(v1)` / `80102218 bne v0,zero,0x801023b0` (nur Phase +0x6 == 0 = Eintritt) ...
  `80102270 jal 0x8001af20` / `80102278 andi v0,v0,0x3` / `8010227c bne v0,zero,0x801022a4` (1/4) /
  `80102284 jal 0x8001af20` / `8010228c andi v0,v0,0x1` / `80102290 beq v0,zero,0x8010229c` / `80102294 ori a0,zero,0x5` /
  `80102298 ori a0,zero,0x4` / **`8010229c jal 0x800453d0`** (Raum-SE 4 oder 5). Dieselbe Eintritts-Form in FUN_801057bc
  (Annaeherung 0x13), FUN_80102dc8 (Drehen), FUN_80103014 (Sturmlauf); kein Stoehn-Takt im laufenden Gang (alle 40
  `jal 0x800453d0` der Zombie-Funktionen 0x80101xxx..0x80109xxx durchgesehen). RE2-Gegenprobe (Gang-Moan
  @0x80101C44-88, enemy_ai_re2_zombie.c re2z_walk_moan): nach einem Moan 150 Bilder Sperre (@0x80101C7C-88) — Roys Gang
  bis Schuss 1 dauert genau 150 Bilder, also auch nach RE2 genau EIN Stoehnen vor dem Schuss.
* Satz: ROOM1021 snd1 (EDT @0x145A8): **SE 4 = `00 00 5b 15`** -> VAG 5 (10304 B, sha1 a5234f0ba034, 11025 Hz, 1,41 s),
  dieselbe Welle als SE 4 in 26 Zombie-Raeumen (1000/1010/1020/1060/1070/1080/10D0/10E0/10F0/1100/1110/11E0/1250 + Elza-
  Varianten) = das Gang-Stoehnen, das man dort von jedem Zombie hoert. SE 5 (`00 00 69 15`, VAG 6, 2,2 s) klingt schon als
  Roys Sterbe-Ton (Runde 36 P5, @0x80106da8) — PORT-WAHL des Muenzwurfs @0x80102290: SE 4 (der Eintritts-Wurf liefert 4
  oder 5; 5 waere derselbe Ton wie 6 s spaeter beim Sterben). NUTZER-VORGABE "soll stoehnen": der 1/4-Wurf @0x8010227c
  entfaellt (sonst bliebe Roy in 3 von 4 Szenen stumm).

### 2.3 Punkt 3 — Herleitung des Schuss-Tons (BERICHTIGT in Nachbesserung 1, §8.1/§8.2)
Runde 36 (L_elza.md §3 P4) nahm RE2 ROOM40A0 Satz 0x08. Das ist der Pistolenschuss der PARTNERIN Ada auf den Alligator:
sub04 @0x0287E Work_set Art 2 (RE2 Art 2 = Partner, @0x80055970 lw *0x800CFE18), Se_on @0x02896 `36 02 08 01 02 00 ..`
an der Partnerin, direkt danach ESP 0x18 "GUN" @0x028A2 und ESP 0x1A Wasserspritzer @0x028C6 (§8.1). Die fruehere Lesart
dieses Abschnitts ("kein Schuss, einziger Gegner = Alligator, Se_on am Alligator") war FALSCH: sie uebertrug die
RE1.5-Bedeutung von Art 2 (Gegnerfeld, @0x80040d60) auf RE2, und ihr Sce_em_set-Zensus liess die vier Partner-Saetze
(Id 0xFF) weg.
* RE2-Herleitung des neuen Satzes (§8.2): John steht im RE1.5-Skript im GEGNERFELD (ROOM1021 Sce_em_set Id 0 Typ 0x49
  @0x01DF6, Work_set(2,0) @0x020C2; RE1.5 Art 2 = Gegnerfeld). Dessen RE2-Entsprechung ist Art 3 (@0x80055988/
  @0x800564b8). RE2s Skript-Pistolenschuss aus dem Gegnerfeld ist ROOM4040 Satz 0x0A (Annette, Sce_em_set Id 1 Typ 0x47
  @0x00E74, Se_on Art 3 Index 1 @0x014E4/@0x01528, je mit Muendungsfeuer `3a 00 18 ..` @0x014F0/@0x01534; Raumtexte
  "He has lost consciousness due to shock from the bullet wound" / "I have to catch that woman!" = Adas Episode).
  Welle **994b137d4007** (5824 B); dieselbe Welle schiesst in ROOM4090 @0x02A4E (Nutzer-SAP-Liste "ROOM409.SAP - Ada meets
  Annette") und ROOM6020 @0x00B94 (Nutzer-SAP-Liste "ROOM602.SAP - ... a gunshot") und liegt nur in Raumbaenken
  (4040/4090/6020/6110/D040/F110). Bytegleich liegt sie in keiner ARMS-/ENEMSE-Bank, die AUFNAHME ist aber RE2 ARMS05 SE 0
  (Magnum) = RE1.5 ARMS07 SE 0 (Super Redhawk) mit halber Rate (Korrelation 0,98, §8.2).
  ROOM4040 Satz 0x0A: EDT @0x0BF60 `00 00 73 00` (Programm 0, Ton 7, Prio 3, Kanal 0), Ton @0x0C8F8
  `00 04 6e 40 54 39 42 42` (vol 110, pan 0x40, center 84, shift 57, min/max 66), VAG @0x18CC8 5824 B.
  (Folge-Welle 2bf10f4ea91f, hell — 4040/6020 Satz 0x0B, 4090 Satz 0x09: folgt in 4040 den Schuessen 1-2 Bilder spaeter, in 4090 @0x02A9C und
  6020 @0x00BB4 an einem ANDEREN Ort als der Schuss positioniert = am Ziel (Einschlag/Abpraller, gedeutet). Johns
  Treffer ist im Port schon Roys Sterbe-Ton (Runde 36 P5) -> nicht uebernommen, OFFEN 2.)
* Klang: gemessen in §8.2 (alter Satz = eigene Aufnahme ohne Entsprechung unter den RE1.5-Waffen, hell; neuer Satz =
  Super-Redhawk-Aufnahme). Die fruehere Zeile "Unterschied ist die HERKUNFT: Alligator-Schleife" ist gestrichen.

### 2.4 Bisect / Regression (Punkt 3)
Keine Regression: Anker, Bytes und Ton sind seit dem Einbau gleich. Die alte Mini-Bank kam mit 36cb73d8 (2026-10-10,
"wip(r36-elza): ... RE2-Mini-Bank ELZASE.VBS (Unlock + NPC-Pistole)") und liegt so in v0.8.24 (e8b3958f) und v0.8.25
(2077bb8e): `git diff e8b3958f e2cc7673 -- elza_r36.c elza_se_pc.c re15_elza_r36.h ELZASE.VBS re2_elza_bank.inc` ist leer.
Gemessen am v0.8.25-Code (m1/m3/m5): der Schuss klingt rechtzeitig und etwa so laut wie Elzas eigene Waffe; der
gemessene Unterschied ist die AUFNAHME (§8.2). Verursachender Commit des Befunds = 36cb73d8 (Satzwahl ROOM40A0 0x08 =
Schuss der Partnerin, eine Aufnahme ohne Entsprechung unter den Waffen des Spiels), nicht Runde 37 (K/N aendern weder
Bytes noch Ton der Szene).

## 3. Umsetzung

| Datei | Inhalt |
|---|---|
| engine/src/elzatoene_r38.c + include/re15_elzatoene_r38.h (NEU) | Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur, Aktion): ROOM1041 @0x02200 `2b 03 00 00 2e 01 00 00` -> Schuss statt Text; ROOM1021 @0x020B2 `40 00 04 22 4e ef 42 e6` -> Raum-SE 4. Scan je Raum, Opcode-Haken, Message_on-Haken, Zaehler/Takt fuer Riegel, Log ins RE15_ELZA_LOG |
| engine/src/scd_vm.c (4 Zeilen + 3 Kommentar) | include; scd_register_current_rdt: re15_elzatoene_r38_raum_scan; Opcode-Schleife: `if (g_re15_elzatoene_anker_n) re15_elzatoene_r38_pc(t->pc);`; op_message_on hinter re15_leiche_message_on: `if (re15_elzatoene_r38_message_on(t->pc)) { t->pc += 4; return 1; }` |
| tools/r36_l/re2_elza_se_cut.py | Satz 1 SCHUSS: ROOM40A0 0x08 -> ROOM4040 0x0A (EDT @0x0BF60 `00 00 73 00`, Ton @0x0C8F8, VAG @0x18CC8 5824 B, sha1 994b137d4007); Selbsttest des Werkzeugs (EDT-/Ton-/VAG-Lage + sha1) laeuft gegen die RE2-Datei |
| shared_assets/RE2/ELZASE.VBS, engine/src/gen/re2_elza_bank.inc | neu geschnitten (`re2_elza_se_cut.py --install`): 14232 B, md5 ba96a66267286e4754c6b5a7481928a4 (Satz 0 AUF unveraendert) |
| platform/pc/src/elza_se_pc.c, include/re15_elza_r36.h | nur Kommentare (Satz 1 = ROOM4040 0x0A) |
| tests/unit/test_r36_elza.c (Teil bank) | Quellvergleich Satz 1 auf ROOM4040 @0x18CC8 / 5824 B / EDT @0x0BF60 nachgezogen (vorher ROOM40A0 @0x17B80 = Schuss der Partnerin) |
| tests/integration/test_r36_elza.cmake (Teil schuss) | Bezug der Mitschnitt-Pruefung: SPITZE der 28 Bilder vor Schuss 1 (John zielt, nur Raumklang) statt Median aller Raum-Bilder — gemessen: Roys Stoehnen hebt den Raum-Median 1640 -> 2086 (gleicher Weg), die Schuesse wurden lauter (6742/7193 -> 7101/7512), der alte Bezug 4x Median = 8344 schlug trotzdem an; Raum vor dem Schuss = 1177 (alt und neu); Untergrenze 3000/1500 fuer einen stillen Raum |

Konstanten mit Beleg: @0x02200/msg 3 (ROOM1041.RDT), @0x020B2 (ROOM1021.RDT), SE 4 (snd1 EDT @0x145A8, FUN_801021f8
@0x80102298/@0x8010229c), Satz ROOM4040 0x0A (Datei-Offsets oben). NUTZER-VORGABE: Stoehnen immer (kein 1/4-Wurf
@0x8010227c), Ton statt Text in 1041. PORT-WAHL: SE 4 statt 5 im Muenzwurf @0x80102290 (5 = Roys Sterbe-Ton).
Unveraendert: Anker/Zeitform Runde 36 (Schuesse @0x020CE/@0x020D6 +1, Roy SE 5), Leon-Raeume (keine Anker).

## 4. Messung nachher (Bau dieses Zweigs, echte exe, Messkopie, RE15_AUDIO_CAP_SYNC)

| Lauf | Weg | Ergebnis |
|---|---|---|
| gunshot1041 | Spielstand ROOM1031 an Tuer 2, CONTINUE, Tuer -> ROOM1041 (-20083,-3559), Elza geht 2,5 s nach Westen ins Auto-Feld AOT 8 | elza.log `F83 [elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1`; debug.log `[se] Stimme: se=1 layer=0 vag=1 note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16`; Nachricht 3 in KEINEM Bild offen, msg 2 99 Bilder offen; Kreuzkorrelation ELZASE Satz 1 bei Bild 83: **0,900** (Versatz 0,00, Gegenprobe 60 Bilder frueher 0,063); Spitze 13113 gegen Raumklang ~2000; lauteste 100 ms -21,2 dB(A) |
| szene1021 | echter Weg wie m3 (Spielstand 1041, Tuer 3 -> 1021, ins Auto-Feld AOT 5) | `F788 [elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2`, Schuesse F939/F969 (151 Bilder nach dem Gangbeginn = Sleep 40 + 110 + Verzug 1), Roy F969; Kreuzkorrelation snd1 SE 4 (Abschnitt 600..1200 ms; der Anfang liegt unter Johns Zeile main03) bei Bild 788: **1,000** (Versatz 1,00 = Pump-Takt der Stimmenmaschine, Gegenprobe 0,068); Schuss-Satz bei 939/969: **1,000 / 0,982** (Gegenprobe 0,000); Stoehnen -18,0 dB(A), Schuss -21,3 dB(A) (vorher Partnerin-Schuss RE2 40A0 -20,9) |

Ton-Ausschnitte zum Anhoeren (Mono 44,1 kHz aus den Mitschnitten): `H_belege/h_1021_vorher_schuesse.wav` (Stand
v0.8.25, Partnerin-Schuss RE2 40A0), `H_belege/h_1021_nachher_schuesse.wav`, `H_belege/h_1021_nachher_stoehnen.wav`,
`H_belege/h_1041_nachher_schuss.wav`.

## 5. Tests

Registriert NUR in re15_port/tests/unit/probes/r38_elzatoene.cmake (GLOB). Quelle tests/unit/test_r38_elzatoene.c,
tests/integration/test_r38_elzatoene.cmake (cmake_minimum_required 3.16, Messkopie re15_pc_r38h_<id>.exe neben der exe),
Auswertewerkzeug tests/unit/probe_r38_elzatoene_kreuz.c (kein add_test).

| Test | misst (Mechanik) | Punkt |
|---|---|---|
| unit_r38_elzatoene_gunshot1041 | ROOM1041 am VM, Elza betritt AOT 8 von Osten: sub11 laeuft ((3,0xAD)), genau ein Schuss-Ton im Takt von @0x02200, Nachricht 3 NIE offen, msg 2 + Stimme 2 danach; GEGENPROBE ohne Anker: Text 3 im selben Takt, kein Ton; Leon 1040 ohne Anker | 1 |
| unit_r38_elzatoene_stoehnen1021 | ROOM1021 am VM: genau ein Raum-SE 4 im Takt von @0x020B2, Schuss 1 genau 150 Bilder spaeter, Roys SE 5 weiter einmal; GEGENPROBE ohne Anker: kein SE 4 | 2 |
| unit_r38_elzatoene_schuss1021 | zwei Schuss-Toene je +1 nach @0x020CE/@0x020D6, keine anderen, Satz = RE2_ELZA_SE_SCHUSS | 3 |
| unit_r38_elzatoene_bank | ELZASE.VBS Satz 1: VAG bytegleich RE2 ROOM4040 @0x18CC8 (5824 B), Ton wie @0x0C8F8, EDT Prio 3 / Direkt-Zweig; nicht mehr RE2 40A0 0x08 | 3 |
| unit_r38_elzatoene_re2beleg | (Nachbesserung 1, §8.3) die MECHANIK der Tonwahl: Handlerwoerter RE2 Work_Set/Se_on Art 2 = Partner, Art 3 = Gegnerfeld, Sce_em_set Id 0xFF = Partner-Platz, Lage nur bei Byte 3 != 0; RE1.5 Art 2 = Gegnerfeld; ROOM40A0 alle 5 Sce_em_set (4 Partner + Alligator), sub04 Se_on Art 2 + ESP 0x18 = Schuss der Partnerin, Alligator = Art 3; ROOM4040 Annette Art 3; John = Gegnerfeld[0] | 3 |
| unit_r38_elzatoene_aufnahme | (Nachbesserung 1, §8.2) ELZASE Satz 1 = Aufnahme des RE1.5-Super-Redhawk ARMS07 SE 0 (Korrelation >= 0,95, gemessen 0,982); GEGENPROBE alter Satz 40A0 0x08 vs RE1.5 ARMS02/03/07/08 SE 0 <= 0,35 (gemessen <= 0,209) | 3 |
| unit_r38_elzatoene_signatur | Anker nur bei passender Signatur, unarmiert bleibt Message_on Text, Gang-Anker ersetzt keine Nachricht, Leon 1020/1040 ohne Anker | 1-3 |
| integration_r38_elzatoene_gunshot1041 | echte exe, Tuerweg 1031 -> 1041 + Gehen: Aktion 1 einmal, Text 3 nie, msg 2, Kreuzkorrelation Schuss >= 0,7 / Gegenprobe <= 0,2 | 1 |
| integration_r38_elzatoene_szene1021 | echte exe, Tuerweg 1041 -> 1021 + Gehen: Stoehnen einmal, Schuesse 151 Bilder spaeter, Kreuzkorrelation Stoehnen/Schuesse >= 0,7 / Gegenprobe <= 0,2 | 2, 3 |

* Spur allein: `ctest --test-dir re15_port/build -R "unit_r38_elzatoene|unit_r36_elza" --timeout 600` -> 14/14 (nach dem
  Nachziehen von gunshot1041: Elza betritt das Feld statt darin zu starten — das Feld feuert beim Betreten).
* `ctest --test-dir re15_port/build -R "integration_r38_elzatoene" --timeout 1200 -V` ->
  `100% tests passed, 0 tests failed out of 2` (Werte §4).
* GEZIELTE REGRESSION (KEINE volle Suite) — eigene Tests + jede Testgruppe der beruehrten Module: scd_vm.c (SCD-Opcodes,
  Message_on-Kette: Dialog/Text-Freeze/Stimmen/Leichen-Ersatz/Savepoint/Itembox, Nachbar-Haken Code-Panel/Lift),
  elza_r36 + ELZASE.VBS (alle r36_elza), Szene 1021 (r37_roy), Raum 1041 (gegnerzahl_raum1041), Elza-Durchlaeufe:
  `ctest --test-dir re15_port/build -R "scd|msg_dialog|text_freeze|synchro_stimme|voice_ausreden|se_voice_machine|leiche|r36_elza|r38_elzatoene|r37_roy|gegnerzahl_raum1041|story_elza|elza_vollstart|elza_zweig|codepanel|lift2030|savepoint|itembox" --timeout 1200 --output-on-failure -j2`
  1. Lauf: 68/69 — integration_r36_elza_schuss rot (Bezug Raum-Median, s. §3 letzte Zeile); Bezug umgestellt, Test
     2x einzeln gruen (`Raum vor dem Schuss 1177`, Spitzen 7101/7512, Roy 8407).
  2. Lauf (Endstand, Bau unveraendert seit dem letzten C-Stand, "ninja: no work to do"):
     **`100% tests passed, 0 tests failed out of 69`** (Total Test time 498.75 sec).

## 6. OFFEN (benannt, nicht beauftragt)

1. **Muendungsfeuer**: RE2 setzt zu Skript-Schuessen ein ESP `3a 00 18 ..` (ROOM4040 @0x014F0, ROOM4090 @0x02A68,
   ROOM6020 @0x00BA4, Kendo ROOM1010 @0x0269A). Der Port zeigt Johns Schuesse ohne Blitz; die Kamera steht dabei auf Cut 10 hinter Roy, John ist
   klein am Gangende (m6, Framedump). Der Ton ist jetzt der RE2-Skript-Schuss; das Bild bleibt wie bisher (Runde 36
   OFFEN 2). Eigener Punkt, falls gewuenscht.
2. **RE2-Folge-Welle** 2bf10f4ea91f (4040/6020 Satz 0x0B, 4090 Satz 0x09; hell, Schwerpunkt 2090 Hz) folgt in RE2 den Skript-Schuessen 1-2 Bilder
   spaeter (4040), in ROOM4090/6020 an einem anderen Ort als der Schuss positioniert (Einschlag/Abpraller, gedeutet). Johns Schuesse treffen Roy -> nicht
   uebernommen; fuer einen Fehlschuss waere er die RE2-Form.
3. **Positions-Ton**: RE2/RE1.5 setzen Skript-Geraeusche teils positional (Se_on Flags != 0, z.B. ROOM3091 @0x01A62);
   der Port spielt alle Se_on ohne Lage (scd_vm.c op_se_on "Positional-Pfad (OPEN)"). Der Schuss in 1041 klingt deshalb
   so laut wie ein naher Schuss (-21,2 dB(A), lauteste 100 ms) — er ist ja auch das Ereignis, auf das Elza reagiert.
4. **Wahrnehmung** (berichtigt in Nachbesserung 1, §8.2): Pegel alt/neu gleich (-20,9 / -21,3 dB(A), etwa Elzas
   Browning -19,4). Der gemessene Unterschied ist die AUFNAHME: alt = Schuss der Partnerin (RE2 40A0), keine Aufnahme
   einer Waffe des Spiels (Korrelation <= 0,21), Hoehenanteil 3,8 dB ueber dem hellsten RE1.5-Waffenschuss; neu = die
   Aufnahme des Super Redhawk (0,98). Ton-Ausschnitte zum Gegenhoeren: H_belege/.

## 7. Fuer den Nutzer / Orchestrator

* **Neue/geaenderte Assets (Paket-Gate)**: `re15_port/shared_assets/RE2/ELZASE.VBS` GEAENDERT (14232 B, md5
  ba96a66267286e4754c6b5a7481928a4; vorher 13144 B). Das bestehende Gate (release/make_package.sh: vorhanden + cmp gegen
  den Quellbaum) deckt die Datei ab; die Android-APK muss neu gebaut werden (Asset-Gate prueft gegen den Baum).
  Erzeugt von `re15_port/tools/r36_l/re2_elza_se_cut.py --install` (Selbsttest gegen info/re2leon).
* **Neue Quelldateien** (Android-Configure frisch, Memory reai-v2-android-glob-cache): engine/src/elzatoene_r38.c;
  Tests tests/unit/test_r38_elzatoene.c, tests/unit/probe_r38_elzatoene_kreuz.c (kein add_test),
  tests/integration/test_r38_elzatoene.cmake. **RE15_MIN_TESTS +9** (7 Unit + 2 Integration; +1 = Nachbesserung 1
  unit_r38_elzatoene_aufnahme).
* **Sprachdateien**: keine neuen; ROOM1041 msg 2 nutzt die vorhandene synchro/STAGE1/room1041/main02.wav, msg 3 (der
  Platzhalter) hat keine Datei und keinen Text mehr.
* **Gemeinsame Dateien**: scd_vm.c (Include + 3 Haken-Zeilen, Kommentar "Runde 38 Spur H": Raum-Scan, Opcode-Haken,
  op_message_on hinter re15_leiche_message_on — Spur F setzt ihr "Help"-Ton-statt-Text evtl. an dieselbe Stelle:
  beide Zeilen behalten), tests/unit/test_r36_elza.c (Teil bank: Quelle Satz 1).
* **Bedienhinweis**: ROOM1041 (Elza, erster Gang durch den Flur, Zugang z.B. ueber ROOM1031): statt des Textfensters
  "*gunshot*" faellt ein Schuss, danach dreht sich Elza um ("What was that!?"). ROOM1021 (Szene nach Johns Befreiung):
  Roy stoehnt, sobald er auf John zugeht; Johns zwei Schuesse sind jetzt RE2s Skript-Pistolenschuss aus dem
  Gegnerfeld (Annette, ROOM4040) = die Aufnahme des Super Redhawk, also ein Schuss, wie ihn der Spieler selbst abfeuert.
* **Kontrolle selbst**: elza.log (RE15_ELZA_LOG=elza.log) zeigt `[elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1` bzw.
  `[elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2`; Ton-Ausschnitte H_belege/*.wav.
* **Entscheidungsfragen**: keine. (Falls zu den Schuessen ein Muendungsfeuer gewuenscht ist: OFFEN 1.)

## 8. Nachbesserung 1 (nach Abnahme 0, `H_abnahme_0.md`)

Stand: Basis a95adbb6 (Spurstand 8328ae1f + Abnahme). Messordner (Scratchpad) `H38n1/`.
Je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher. Fortlaufend ergaenzt.

### 8.1 Mangel 1 — RE2 ROOM40A0 Satz 0x08 ist ein SCHUSS (der Partnerin Ada auf den Alligator)
**Ursache des Fehlers:** die RE1.5-Bedeutung von Work_set-/Se_on-Art 2 (= Gegnerfeld) wurde auf RE2 uebertragen. RE2
hat fuer den Partner eine eigene Art und schiebt das Gegnerfeld auf Art 3. Selbst disassembliert:
* RE1.5 `info/Re1.5/PSX.EXE` (md5 b55fdaa5), Work_set LAB_80040d2c: @0x80040d4c `lbu v1,1(v0)` (Art), @0x80040d50
  `lbu a1,2(v0)` (Index), @0x80040d5c `ori v0,zero,0x2` / @0x80040d60 `beq v1,v0,0x80040db4` -> @0x80040db4..c0 Index*0x1F4,
  @0x80040dc4 `lui v1,0x800b` / @0x80040dc8 `addiu v1,v1,-13268` = Gegnerfeld 0x800ACC2C. Art 1 @0x80040da4/a8 = Spieler
  0x800ACA54, Art 3 @0x80040dd4.. = Objekte 0x800B3F98. **Keine Partner-Art.**
* RE2 `info/re2leon/PSX.EXE`, Opcode-Tabelle @0x800A74C8:
  - Work_Set [0x2E] = 0x80055904: @0x80055920 `lbu v1,1(v0)` (Art), @0x80055924 `lb a1,2(v0)` (Index), Sprungtabelle
    @0x800111f0 (Art-1): Art 1 -> 0x8005595c, @0x80055960 `8c42fe14` lw *0x800CFE14 (Spieler); **Art 2 -> 0x8005596c,
    @0x80055970 `8c42fe18` lw *0x800CFE18 (PARTNER)**; **Art 3 -> 0x8005597c, @0x80055988 `8c22fe1c` lw *(0x800CFE1C +
    4*Index) (GEGNERFELD)**; Art 4 -> 0x80055994 (Objekte 0x800D0324 + 0x1F8*Index); Art 5 -> 0x800559b0.
  - Se_on [0x36] = 0x80056428: @0x80056444 `lh a1,4(s0)` (Byte 4 = Art, Byte 5 = Index), Sprungtabelle @0x80011458:
    Art 0 -> 0x80056478 Weltlage 0 + Versatz; Art 1 -> @0x8005648c *0x800CFE14; **Art 2 -> @0x8005649c `8c63fe18`
    *0x800CFE18**; **Art 3 -> @0x800564b8 `8c23fe1c` *(0x800CFE1C + 4*Index)**; Art 4 -> Objekte. Lage = Figur
    +0x38/+0x3C/+0x40 + Versatz (Bytes 6..11), dann @0x80056530 `jal 0x8005ba28`.
  - Sce_em_set [0x44] = 0x8005714c: @0x80057204 `80420002` lb (Id VORZEICHENBEHAFTET, 0xFF = -1), @0x8005720c `24450002`
    addiu a1,v0,2, @0x80057210 `beq a1,a2` (Id 0xFF: Gegnerzaehler G+0x3A0B wird uebersprungen), @0x80057234 `ac433c2c`
    sw -> G+0x3C2C+(Id+2)*4 (G = 0x800CC1E8; Init @0x80049ec4 `addiu v1,s0,0x3c30` = 0x800CFE18, @0x80049ed4
    `addiu v0,s0,0x3c34` = 0x800CFE1C): **Id 0xFF -> 0x800CFE18 = Partner, Id n -> 0x800CFE1C+4n = Gegner n.**

**Folge fuer ROOM40A0** (RE2-Walk `tools/re2_sicherung/re2_scd_walk.py`, Bytes selbst gelesen):
* 5 Sce_em_set: main00 @0x02564 `44 00 ff 41` (nach Ck(1,0x00)==0: Ada), @0x02586 `44 00 ff 45`, @0x025A0 `44 00 00 23`
  (Alligator, Id 0), sub00 @0x02612 / @0x0262C `44 00 ff 41`. Vier davon besetzen den PARTNER-Platz 0x800CFE18.
  Der Alligator ist Art 3: sub11 @0x02CDC `2e 03 00`, @0x02CFE `22 05 02 01` Set(5,2), @0x02D02 `36 02 0a 01 03 00 ..`.
* sub02 startet @0x02684/@0x02688 `04 ff 18 04` / `04 ff 18 03` sub04 und sub03 parallel; sub03 @0x026CA Work_set(1,0)
  = Leon; **sub04 @0x0287E `2e 02 00` = die PARTNERIN**: @0x02881 Plc_motion(2,14,4), Schleife @0x0288A `0f 04 82 00`
  solange Ck(5,0x0b)==0: Sleep 8, **@0x02896 `36 02 08 01 02 00 ..` Se_on Satz 0x08 an der Partnerin**, @0x028A2
  `3a 00 18 ..` ESP 0x18 (esp18.tim = Blatt "GUN 5.00", bytegleich ROOM4040, md5 fd8fe615 — Abnahme 0), Sleep 2,
  @0x028C6 `3a 00 1a 05 ..` ESP 0x1A ("MIZ", Wasserspritzer an fester Weltlage), Sleep 1+7 -> ein Schuss je 18 Bilder;
  danach @0x0291E derselbe Se_on + @0x0292A ESP 0x18 (letzter Schuss), @0x02990 Plc_motion(2,9), @0x029A0 Warten auf
  Ck(5,0x02) (= Set(5,2) des Alligator-Skripts @0x02CFE).
* **=> Satz 0x08 ist der Pistolenschuss der Partnerin (Leon-Szenario: Ada, Typ 0x41) auf den Alligator.** Runde 36 lag
  bei der ART des Tons richtig ("Skript-Pistolenschuss eines NPC"); die Behauptung "kein Schuss / Alligator-Schleife /
  am Alligator" (Spurstand 8328ae1f) ist falsch und ueberall berichtigt (8.4).
* Zusatz Lage (fuer 8.2): RE2-Pumpe FUN_8005c5e4 @0x8005c658 `86050006` lh a1,6(s0) (= Satz+0x12 = Se_on-Byte 3,
  geschrieben @0x8005bc40 `sh t1,18(v1)`), @0x8005c660 `10a0001a` beq a1,zero,0x8005c6cc, @0x8005c668 `jal 0x8005c970`
  (Lautstaerke nach Lage) -> nur Byte 3 != 0 klingt nach Lage. ROOM40A0 @0x02896 Byte 3 = 1 (in RE2 positional),
  ROOM4040 @0x014E4 Byte 3 = 0 (nicht positional — so spielt der Port jeden ELZASE-Ton).
* Tonhoehe (fuer 8.2): RE2 0x8005ba28 traegt @0x8005bbec `92020006` lbu tone[+6] (Note) und @0x8005bbf8 `92020005` lbu
  tone[+5] (fein) in den Satz ein — dieselbe Note/Fein-Wahl wie RE1.5 SsUtKeyOnV @0x800451fc/@0x80045210, die der Port in
  se_play_layers nutzt (re15_vab_note2pitch2(min, shift, center, shift)). Die Abspielrate der RE2-Wellen im Port ist also
  RE2-treu; die Tonhoehe ist KEINE Ursache.

### 8.2 Mangel 2 — Ursache des Befunds P3 und Wirksamkeit
Messordner `H38n1/` (Scratchpad), Werkzeuge `H38n1/py/`: `schuss_zensus.py` (RE2-Skript-Schuesse), `wellen.py` +
`klangbild.py` + `terz.py` + `baender2.py` (Wellen so getaktet wie der Mischer: Note tone[+6], fein tone[+5],
Schritt pitch << 4, Halten wie audio_callback), `kap_kreuz.py` (Mitschnitte). Mitschnitte = die der Abnahme 0
(`ab0h/runs/alt_s1021` = v0.8.25-Paket-exe, `ab0h/runs/neu_s1021` = Spur-exe; echter Weg 1041 -> Tuer 3 -> 1021,
RE15_AUDIO_CAP_SYNC); seit 8328ae1f ist kein Laufzeitcode geaendert (nur Kommentare, Riegel, Werkzeug-Text).

**Messung vorher — was der Nutzer in v0.8.25 bekam:**
* Zeitpunkt + Pegel stimmten: an F939/F969 (Feuer-Clip +1) klang RE2 40A0/0x08 (Korrelation 1,000/0,977) in stillem
  Raum (F930..F938 Spitze 0), -20,9 dB(A) — etwa Elzas eigene Browning (-19,4 dB(A), §1 m4). Tonhoehe RE2-treu (§8.1).
* Bild: die Szene steht waehrend der Schuesse auf Cut 10 (Original, sub02 @0x0205C `29 0a`) hinter Roy; John ist
  hinter Roy VERDECKT (Framedump F930..F980, `H38n1/s1021_zoom.png`), RE1.5 setzt kein Muendungsfeuer. Der Schuss ist
  also nur am TON erkennbar — alt wie neu.
* Der Ton selbst — Klangvergleich mit den Schuessen, die der Nutzer im Port kennt (eigener Decoder, 0..150 ms, Anteil
  der Terzbaender <= 250 Hz bzw. >= 2,5 kHz an der Gesamtenergie; Korrelation = normierte Kreuzkorrelation 0..0,3 s):

| Welle (wie der Mischer getaktet) | Aufnahme = | <= 250 Hz | >= 2,5 kHz |
|---|---|---|---|
| RE1.5 ARMS02 SE 0 (Leon, Handgun) | b5163f2113a9 | -2,5 dB | -10,4 dB |
| RE1.5 ARMS03 SE 0 (Elza, Browning) | 4769cc0478f3 = RE2 ARMS02/03/04/13 SE 0 | -3,4 | -9,8 |
| RE1.5 ARMS07 SE 0 (Super Redhawk) | 76d95b2b5b52 = RE2 ARMS05 SE 0 (Magnum), Korr. 1,00 | -4,2 | -11,2 |
| RE1.5 ARMS08 SE 0 (Schrotflinte) | 42bbd9b31614 = RE2 ARMS07 VAG 2 = RE2-Kendo-Skriptschuss 1010/0x0C (bytegleich) | -3,3 | -13,0 |
| RE1.5 ARMS13 / ARMS14 SE 0 | 46c4bd385f92 / 6d4d4aedf22c | -4,0 / -3,3 | -14,5 / -13,0 |
| **ALT** RE2 40A0/0x08 (Partnerin, v0.8.25) | e552c5d77eed — KEINE Waffenaufnahme: Korr. mit ARMS02/03/07/08 SE 0 = 0,209/0,182/0,107/0,092 | **-5,3** | **-6,0** |
| **NEU** RE2 4040/0x0A (Annette) | 994b137d4007 = RE1.5 ARMS07 SE 0 (Super Redhawk), halbe Rate, Korr. **0,982** | -4,4 | -10,5 |
| (RE2 ENEMSE30 VAG 6, Adas KI-Schuss) | db906adc10f9 = Browning-Aufnahme (0,99) | -3,5 | -9,2 |
| (RE2 5000/0x0D, Partnerin im Skript) | 4482eb975558, teils Browning (0,70) | -4,6 | -6,9 |

  Der alte Ton ist als einziger keine Aufnahme einer Waffe des Spiels, und er liegt in BEIDEN Baendern ausserhalb aller
  sechs RE1.5-Waffenschuesse: Hoehenanteil 3,8 dB ueber dem hellsten (-6,0 gegen -9,8..-14,5), Bassanteil 1,1 dB unter
  dem schwaechsten (-5,3 gegen -2,5..-4,2). Nicht der Mischer: bandbegrenzt umgetastet bleibt es -5,6 / -5,4 dB
  (`baender2.py`, Halten aendert <= 0,8 dB). Im Mitschnitt v0.8.25 an F939 dasselbe: -5,2 / -6,0 dB; Korrelation mit
  dem Super Redhawk 0,173, mit Elzas Browning 0,184 (`kap_kreuz.py`).

**Ursache (so weit sie messbar ist):** v0.8.25 spielte zu Johns Schuessen rechtzeitig und so laut wie Elzas Waffe einen
Ton — aber eine Aufnahme, die im Spiel sonst nirgends als Waffenschuss vorkommt (heller "Knack" ohne den Bass der
Spielwaffen), waehrend John hinter Roy verdeckt ist und kein Muendungsfeuer zu sehen ist. Der Ton war der einzige
Hinweis auf den Schuss, und er war keiner der Schuesse, die der Nutzer aus dem Spiel kennt. Pegel, Zeitpunkt, Tonhoehe,
Abspielweg und Bild sind gemessen gleich gut bzw. original; die Aufnahme ist der einzige gemessene Unterschied. (Ob der
Nutzer genau daran scheiterte, kann kein Rechner messen; gemessen ist, dass alles andere stimmte.)

**Wie der neue Stand genau diese Ursache beseitigt:** Johns Schuss ist jetzt die Aufnahme des Super Redhawk — derselbe
Schuss, den der Spieler mit dieser Waffe abfeuert: im echten Mitschnitt Korrelation 0,982 (F939) / 0,968 (F969) mit
RE1.5 ARMS07 SE 0; Baender im Mitschnitt -4,2 / -10,5 dB (Hoehen im Bereich der RE1.5-Waffen, Bass am Rand — es ist
dieselbe Aufnahme wie der Redhawk, nur mit halber Abtastrate: Quelle -4,4 gegen Redhawk -4,2).

**RE2-Herleitung der Tonwahl (ersetzt die widerlegte Begruendung):**
1. John steht im RE1.5-Skript im GEGNERFELD: ROOM1021 Sce_em_set @0x01DF6 `44 00 49 ..` (Id 0, Typ 0x49), Work_set(2,0)
   @0x020C2 vor den Feuer-Clips; RE1.5 Art 2 = Gegnerfeld (@0x80040d60 -> 0x800ACC2C, §8.1). RE1.5 kennt keine
   Partner-Art.
2. Die RE2-Entsprechung des Gegnerfelds ist Art 3 (@0x80055988 / @0x800564b8); Art 2 ist der Partner (§8.1).
3. Zensus ALLER RE2-Skript-Schuesse (Se_on, dem binnen 4 Saetzen ein ESP 0x18 "GUN" folgt; opcode-exakter Walk ueber
   alle PL0-RDTs, `schuss_zensus.py`):

| Raum Block @Se_on | Art (Schuetze) | Satz | Welle | Aufnahme |
|---|---|---|---|---|
| 1010 sub31 @0x0268E | 3 (Kendo, Id 0 Typ 0x48, Schrotflinte) | 0x0C | 42bbd9b31614 | RE2 ARMS07 VAG 2 / RE1.5 ARMS08 (bytegleich) |
| 4040 sub21 @0x014E4, @0x01528 | 3 (Annette, Id 1 Typ 0x47, Pistole) | 0x0A | 994b137d4007 | Magnum/Super Redhawk (0,98) |
| 4090 sub06 @0x02A4E | 0 (Weltlage) | 0x08 | 994b137d4007 | wie 4040 |
| 6020 sub04 @0x00B94 | 0 (Weltlage) | 0x0A | 994b137d4007 | wie 4040 |
| 40A0 sub04 @0x02896, @0x0291E | 2 (Partnerin Ada) | 0x08 | e552c5d77eed | keine Waffe (<= 0,21) |
| 5000 sub09/sub10/sub11 @0x01F94/@0x02002/@0x02070 | 2 (Partnerin Ada) | 0x0D | 4482eb975558 | teils Browning (0,70) |
| 6040 sub10 @0x02ABA/@0x02B32/@0x02BAA | 0 (Weltlage) | 0x0F | 4482eb975558 | wie 5000 |
| 1120 sub46/sub47 | 0 (Weltlage) | 0x05 + 0x06 | 57e7f7c6999c + fdd4323b9d43 | zwei Lagen, nicht weiter untersucht |

   => Jeder Skript-Schuss aus dem GEGNERFELD (Art 3) ist in RE2 eine SPIELERWAFFEN-Aufnahme (Kendos Schrotflinte =
   ARMS07 VAG 2 bytegleich; Annettes Pistole = die Magnum-Aufnahme ARMS05 mit halber Rate). RE2 hat genau EINEN
   Skript-Pistolenschuss aus dem Gegnerfeld: ROOM4040 Satz 0x0A. John traegt eine Pistole (Runde 36, Framedump Cut 9)
   und steht im Gegnerfeld -> RE2-Form = ROOM4040 Satz 0x0A. Dieselbe Welle ist auch RE2s Weltlage-Schuss (4090, 6020).
   Der Runde-36-Satz 40A0/0x08 gehoert der Partnerin (Art 2), einer Art, die es in RE1.5 nicht gibt.
4. Lage: ROOM4040 @0x014E4 Se_on-Byte 3 = 0 -> in RE2 ohne Lage (Pumpe @0x8005c660 `beq a1,zero`) — genau so spielt der
   Port den Ton (elza_se_pc.c, nicht positional). Der 40A0-Schuss hat Byte 3 = 1 und waere in RE2 nach Lage geregelt
   worden (eine weitere Abweichung des alten Stands).

**Aenderung:** keine weitere Laufzeit- oder Datenaenderung — der Spurstand spielt bereits ROOM4040 0x0A. Berichtigt sind
Begruendung, Kommentare, Werkzeug-Text und Riegel (8.3/8.4); der neue Riegel `unit_r38_elzatoene_aufnahme` haelt die
gemessene Ursache fest (Super-Redhawk-Aufnahme >= 0,95, Gegenprobe alter Satz <= 0,35 gegen ARMS02/03/07/08).

### 8.3 Mangel 3 — Riegel unit_r38_elzatoene_re2beleg
* Ursache: `teil_re2beleg` zaehlte Sce_em_set mit `a0[i + 2] < 16` — das schliesst genau die vier Id-0xFF-Saetze aus —
  und las "Se_on(2,8) Ursprung 2/0" ohne Pruefung, was Art 2 in RE2 bedeutet.
* Messung vorher (Spurstand 8328ae1f): `ok: ROOM40A0: Sce_em_set @0x025A0 `44 00 00 23` = einziger Gegner (Alligator), 1`.
* Aenderung (`tests/unit/test_r38_elzatoene.c`, Teil re2beleg neu): (1)-(2) Handlerwoerter RE2 Work_Set/Se_on: Art 2 ->
  @0x80055970 `8c42fe18` / @0x8005649c `8c63fe18` (*0x800CFE18 Partner), Art 3 -> @0x80055988 `8c22fe1c` / @0x800564b8
  `8c23fe1c` (Gegnerfeld), Sprungtabellen @0x800111f4/@0x800111f8 und @0x80011460/@0x80011464; (3) Sce_em_set
  @0x80057204 `80420002` / @0x8005720c `24450002` / @0x80057234 `ac433c2c` (Id 0xFF -> Partner-Platz); (4) Pumpe
  @0x8005c658 `86050006` / @0x8005c660 `10a0001a` / @0x8005c668 `0c01725c` (Lage nur bei Byte 3 != 0); (5) RE1.5 Work_set
  @0x80040d5c `34020002` / @0x80040d60 `10620014` / @0x80040dc4 `3c03800b` / @0x80040dc8 `2463cc2c` (Art 2 = Gegnerfeld
  0x800ACC2C); (6) ROOM40A0 ALLE Sce_em_set ohne Id-Filter (Id 0xFF oder < 0x20): genau 5 = 4 Partner + 1 Alligator,
  @0x02564 `44 00 ff 41`, @0x025A0 `44 00 00 23`; (7) sub04 Work_set(2,0) @0x0287E, Se_on Satz 8 Art 2 @0x02896/@0x0291E,
  ESP 0x18 @0x028A2/@0x0292A, ESP 0x1A @0x028C6; (8) sub11 Alligator = Art 3 @0x02CDC/@0x02D02; (9) ROOM4040 Annette
  @0x00E74 `44 00 01 47`, Se_on Art 3 Index 1 Byte 3 = 0 @0x014E4/@0x01528 + ESP 0x18, EDT @0x0BF60, dieselbe Welle
  ROOM4090 @0x3E724/@0x02A4E; (10) RE1.5 ROOM1021 John Id 0 Typ 0x49 @0x01DF6 + Work_set(2,0) @0x020C2.
  Neu `teil_aufnahme` (eigener ctest-Eintrag unit_r38_elzatoene_aufnahme), siehe 8.2.
* Nicht mehr selbstbestaetigend: (1)-(5) pruefen die BEDEUTUNG der Arten an den Maschinenwoertern (eine RE1.5-Lesart
  von Art 2 in RE2 faellt durch); (6) zaehlt ohne Filter; die Aufnahme-Gegenprobe zeigt, dass das Mass trennt (alter
  Satz <= 0,209 gegen jede Waffe, neuer 0,982 gegen den Redhawk) — mit dem alten Satz in ELZASE.VBS faellt der Teil durch.
* Messung nachher: siehe 8.5.

### 8.4 Mangel 4 — die widerlegte Behauptung in Code, Werkzeug und Dossier
Berichtigt (Wortlaut jetzt: 40A0/0x08 = Schuss der PARTNERIN, Art 2 = Partner; Tonwahl = Gegnerfeld-Schuss ROOM4040 0x0A
= Super-Redhawk-Aufnahme, mit Adressen):
* `platform/pc/src/elza_se_pc.c` (Kopfkommentar Satz 1), `include/re15_elzatoene_r38.h` (Kopf P3), `include/re15_elza_r36.h`
  (Kommentar RE15_ELZA_TON_SCHUSS), `tests/unit/test_r36_elza.c` (teil_bank), `tests/unit/test_r38_elzatoene.c` (Kopf,
  teil_bank-Meldung, teil_re2beleg), `tools/r36_l/re2_elza_se_cut.py` (Docstring Satz 1 + SAETZE-Kommentar).
  `engine/src/gen/re2_elza_bank.inc` traegt keine Begruendung (unveraendert, Daten unveraendert).
* Dossier: §0 Stand, §2.3 (neu geschrieben, alte Lesart als FALSCH markiert), §2.4, §3 (test_r36_elza.c), §4 (Mitschnitt-
  Zeile + Ton-Ausschnitte), §5 (Riegel-Tabelle), §6 OFFEN 4, §7 (RE15_MIN_TESTS, Bedienhinweis).
* `grep -i "alligator-schleife|kein schuss|am alligator|alligator-ton|alligator-welle"` ueber alle geaenderten Code-,
  Werkzeug- und Riegeldateien: 0 Treffer; im Dossier nur noch in den als falsch markierten Zitaten.
