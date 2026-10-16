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
- [x] RE Punkt 3: Ursache = falscher RE2-Ton (Alligator-Schleife statt Pistolenschuss) — §2.3
- [x] RE Punkt 1/2 — §2.1/§2.2
- [x] Umsetzung (§3), Messung nachher (§4), Riegel (§5: 6 Unit + 2 Integration, gruen)
- [ ] gezielte Regression (§5)

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
  **@0x02200 `2b 03 00 00` Message_on 3** = msg 3 @0x022FB `04 00 5f 43 51 4a 4f 44 4b 50 5f 04 01 01 00` = "*gunshot*"
  (Maske 0 = kein Einfrieren), dann `2e 01 00 00` Work_set(1,0), Sleep 25, Plc_dest (Elza dreht sich), Warten Ck(5,0x20),
  Sleep 25, **@0x02224 Message_on 2** "Elza: What was that!?" (synchro room1041/main02.wav).
* Der Text ist ein PLATZHALTER fuer einen Ton: Zensus aller Nachrichten (msg_suche.py: 1229 RE1.5 + 2315 RE2): "gunshot"
  steht NUR in ROOM1041 msg 3; RE2 hat keine einzige Geraeusch-Beschriftung. RE1.5 selbst setzt Geraeusche, auf die eine
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

### 2.3 Punkt 3 — warum der Nutzer keinen Schuss hoert: der Runde-36-Ton ist KEIN Schuss
Runde 36 (L_elza.md §3 P4) nahm "RE2 ROOM40A0 sub04 @0x0287E: Plc_motion(2,14,4) (Schiess-Clip), Se_on(2,8) + Muendungsfeuer"
als "Skript-Pistolenschuss eines NPC". Selbst nachgelesen (info/re2leon/PL0/RDT/ROOM40A0.RDT):
* Der EINZIGE Gegner des Raums ist der ALLIGATOR: Sce_em_set @0x025A0 `44 00 00 23 00 00 01 16 ..` (Slot 0, Typ 0x23 =
  RE2-EM23, im Port der Alligator von ROOM2090); sonst kein Sce_em_set in der Datei. Work_set(2,0) @0x0287E waehlt also
  den Alligator, `3f 02 0e 04` ist SEIN Clip 14.
* Der Ton steht in einer SCHLEIFE: `0f 04 82 00` While + `21 05 0b 00` Ck(5,0x0b)==0, darin `09 0a 08 00` Sleep 8,
  **`36 02 08 01 02 00 00 00 00 00 00 00`** Se_on(2,8) mit Ursprung Modus 2 Index 0 (= am Alligator) und zwei ESP — ein
  wiederholtes Alligator-Geraeusch, kein Schuss. Raumbank laut Nutzer-SAP-Liste "ROOM40A.SAP - Alligator Room sounds".
* Der ECHTE RE2-Skript-Pistolenschuss (Zensus aller 250 RE2-RDTs: Se_on mit nachfolgendem Muendungsfeuer `3a 00 18`):
  Welle **994b137d4007** (5824 B), in den Skripten von ROOM4040 (5 Schuesse @0x014E4/@0x01528/@0x01558/@0x01578/@0x01668
  `36 02 0a 00 03 01 ..`, die ersten zwei mit Muendungsfeuer `3a 00 18 ..` @0x014F0/@0x01534; Skriptszene mit der Spielfigur: Work_set(1,0) @0x014BE, Plc_motion(0,0x11)
  @0x014D6; Raumtexte "He has lost consciousness due to shock from the bullet wound" / "I have to catch that woman!" =
  Adas Episode), ROOM4090 (@0x02A4E, Nutzer-SAP-Liste "ROOM409.SAP - Ada meets Annette") und ROOM6020
  (@0x00B94 `36 02 0a 01 ..`; Nutzer-SAP-Liste: "ROOM602.SAP - ... a gunshot"); dieselbe Welle liegt nur in Raumbaenken
  (4040/4090/6020/6110/D040/F110), in keiner ARMS-/ENEMSE-Bank.
  ROOM4040 Satz 0x0A: EDT @0x0BF60 `00 00 73 00` (Programm 0, Ton 7, Prio 3, Kanal 0), Ton @0x0C8F8
  `00 04 6e 40 54 39 42 42` (vol 110, pan 0x40, center 84, shift 57, min/max 66), VAG @0x18CC8 5824 B.
  (Folge-Satz 0x0B = Welle 2bf10f4ea91f folgt jedem Schuss 1-9 Bilder spaeter, in 4090/6020 am ZIELORT positioniert =
  Abpraller eines FEHLschusses; Johns Schuesse treffen Roy -> nicht uebernommen.)
* Klang (scratchpad py/ton.py): Alligator-Ton e552c5d77eed bei 13641 Hz 400 ms, Schwerpunkt 859 Hz; Skript-Schuss
  994b137d4007 bei ~16,4 kHz 420 ms, Schwerpunkt 597 Hz. Beide "Knall"-Klasse — der Unterschied ist die HERKUNFT:
  der gespielte Ton stammt aus einer Alligator-Schleife, nicht aus einem RE2-Schuss.

### 2.4 Bisect / Regression (Punkt 3)
Keine Regression: Anker, Bytes und Ton sind seit dem Einbau gleich. Die alte Mini-Bank kam mit 36cb73d8 (2026-10-10,
"wip(r36-elza): ... RE2-Mini-Bank ELZASE.VBS (Unlock + NPC-Pistole)") und liegt so in v0.8.24 (e8b3958f) und v0.8.25
(2077bb8e): `git diff e8b3958f e2cc7673 -- elza_r36.c elza_se_pc.c re15_elza_r36.h ELZASE.VBS re2_elza_bank.inc` ist leer.
Gemessen am v0.8.25-Code (m1/m3/m5): der Schuss klingt — es ist nur der falsche RE2-Ton (§2.3). Verursachender Commit
des Befunds = 36cb73d8 (Satzwahl ROOM40A0 0x08), nicht Runde 37 (K/N aendern weder Bytes noch Ton der Szene).

## 3. Umsetzung

| Datei | Inhalt |
|---|---|
| engine/src/elzatoene_r38.c + include/re15_elzatoene_r38.h (NEU) | Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur, Aktion): ROOM1041 @0x02200 `2b 03 00 00 2e 01 00 00` -> Schuss statt Text; ROOM1021 @0x020B2 `40 00 04 22 4e ef 42 e6` -> Raum-SE 4. Scan je Raum, Opcode-Haken, Message_on-Haken, Zaehler/Takt fuer Riegel, Log ins RE15_ELZA_LOG |
| engine/src/scd_vm.c (4 Zeilen + 3 Kommentar) | include; scd_register_current_rdt: re15_elzatoene_r38_raum_scan; Opcode-Schleife: `if (g_re15_elzatoene_anker_n) re15_elzatoene_r38_pc(t->pc);`; op_message_on hinter re15_leiche_message_on: `if (re15_elzatoene_r38_message_on(t->pc)) { t->pc += 4; return 1; }` |
| tools/r36_l/re2_elza_se_cut.py | Satz 1 SCHUSS: ROOM40A0 0x08 -> ROOM4040 0x0A (EDT @0x0BF60 `00 00 73 00`, Ton @0x0C8F8, VAG @0x18CC8 5824 B, sha1 994b137d4007); Selbsttest des Werkzeugs (EDT-/Ton-/VAG-Lage + sha1) laeuft gegen die RE2-Datei |
| shared_assets/RE2/ELZASE.VBS, engine/src/gen/re2_elza_bank.inc | neu geschnitten (`re2_elza_se_cut.py --install`): 14232 B, md5 ba96a66267286e4754c6b5a7481928a4 (Satz 0 AUF unveraendert) |
| platform/pc/src/elza_se_pc.c, include/re15_elza_r36.h | nur Kommentare (Satz 1 = ROOM4040 0x0A) |
| tests/unit/test_r36_elza.c (Teil bank) | Quellvergleich Satz 1 auf ROOM4040 @0x18CC8 / 5824 B / EDT @0x0BF60 nachgezogen (vorher ROOM40A0 @0x17B80 — die widerlegte Quelle) |
| tests/integration/test_r36_elza.cmake (Teil schuss) | Bezug der Mitschnitt-Pruefung: SPITZE der 28 Bilder vor Schuss 1 (John zielt, nur Raumklang) statt Median aller Raum-Bilder — gemessen: Roys Stoehnen hebt den Raum-Median 1640 -> 2086 (gleicher Weg), die Schuesse wurden lauter (6742/7193 -> 7101/7512), der alte Bezug 4x Median = 8344 schlug trotzdem an; Raum vor dem Schuss = 1177 (alt und neu); Untergrenze 3000/1500 fuer einen stillen Raum |

Konstanten mit Beleg: @0x02200/msg 3 (ROOM1041.RDT), @0x020B2 (ROOM1021.RDT), SE 4 (snd1 EDT @0x145A8, FUN_801021f8
@0x80102298/@0x8010229c), Satz ROOM4040 0x0A (Datei-Offsets oben). NUTZER-VORGABE: Stoehnen immer (kein 1/4-Wurf
@0x8010227c), Ton statt Text in 1041. PORT-WAHL: SE 4 statt 5 im Muenzwurf @0x80102290 (5 = Roys Sterbe-Ton).
Unveraendert: Anker/Zeitform Runde 36 (Schuesse @0x020CE/@0x020D6 +1, Roy SE 5), Leon-Raeume (keine Anker).

## 4. Messung nachher (Bau dieses Zweigs, echte exe, Messkopie, RE15_AUDIO_CAP_SYNC)

| Lauf | Weg | Ergebnis |
|---|---|---|
| gunshot1041 | Spielstand ROOM1031 an Tuer 2, CONTINUE, Tuer -> ROOM1041 (-20083,-3559), Elza geht 2,5 s nach Westen ins Auto-Feld AOT 8 | elza.log `F83 [elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1`; debug.log `[se] Stimme: se=1 layer=0 vag=1 note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16`; Nachricht 3 in KEINEM Bild offen, msg 2 99 Bilder offen; Kreuzkorrelation ELZASE Satz 1 bei Bild 83: **0,900** (Versatz 0,00, Gegenprobe 60 Bilder frueher 0,063); Spitze 13113 gegen Raumklang ~2000; lauteste 100 ms -21,2 dB(A) |
| szene1021 | echter Weg wie m3 (Spielstand 1041, Tuer 3 -> 1021, ins Auto-Feld AOT 5) | `F788 [elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2`, Schuesse F939/F969 (151 Bilder nach dem Gangbeginn = Sleep 40 + 110 + Verzug 1), Roy F969; Kreuzkorrelation snd1 SE 4 (Abschnitt 600..1200 ms; der Anfang liegt unter Johns Zeile main03) bei Bild 788: **1,000** (Versatz 1,00 = Pump-Takt der Stimmenmaschine, Gegenprobe 0,068); Schuss-Satz bei 939/969: **1,000 / 0,982** (Gegenprobe 0,000); Stoehnen -18,0 dB(A), Schuss -21,3 dB(A) (vorher Alligator-Ton -20,9) |

Ton-Ausschnitte zum Anhoeren (Mono 44,1 kHz aus den Mitschnitten): `H_belege/h_1021_vorher_schuesse.wav` (Stand
v0.8.25, Alligator-Ton), `H_belege/h_1021_nachher_schuesse.wav`, `H_belege/h_1021_nachher_stoehnen.wav`,
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
| unit_r38_elzatoene_bank | ELZASE.VBS Satz 1: VAG bytegleich RE2 ROOM4040 @0x18CC8 (5824 B), Ton wie @0x0C8F8, EDT Prio 3 / Direkt-Zweig; nicht mehr die Alligator-Welle | 3 |
| unit_r38_elzatoene_re2beleg | die zitierten RE2-Bytes: ROOM40A0 einziger Sce_em_set Typ 0x23 @0x025A0, Schleife @0x0287E..@0x02896; ROOM4040 @0x014E4 Se_on + `3a 00 18`, EDT @0x0BF60; dieselbe Welle ROOM4090 @0x3E724 / Se_on @0x02A4E | 3 |
| unit_r38_elzatoene_signatur | Anker nur bei passender Signatur, unarmiert bleibt Message_on Text, Gang-Anker ersetzt keine Nachricht, Leon 1020/1040 ohne Anker | 1-3 |
| integration_r38_elzatoene_gunshot1041 | echte exe, Tuerweg 1031 -> 1041 + Gehen: Aktion 1 einmal, Text 3 nie, msg 2, Kreuzkorrelation Schuss >= 0,7 / Gegenprobe <= 0,2 | 1 |
| integration_r38_elzatoene_szene1021 | echte exe, Tuerweg 1041 -> 1021 + Gehen: Stoehnen einmal, Schuesse 151 Bilder spaeter, Kreuzkorrelation Stoehnen/Schuesse >= 0,7 / Gegenprobe <= 0,2 | 2, 3 |

* Spur allein: `ctest --test-dir re15_port/build -R "unit_r38_elzatoene|unit_r36_elza" --timeout 600` -> 14/14 (nach dem
  Nachziehen von gunshot1041: Elza betritt das Feld statt darin zu starten — das Feld feuert beim Betreten).
* `ctest --test-dir re15_port/build -R "integration_r38_elzatoene" --timeout 1200 -V` ->
  `100% tests passed, 0 tests failed out of 2` (Werte §4).

## 6. OFFEN (benannt, nicht beauftragt)

1. **Muendungsfeuer**: RE2 setzt zu JEDEM Skript-Schuss ein ESP `3a 00 18 ..` (ROOM4040 @0x014F0, ROOM4090 @0x02A68,
   ROOM6020 @0x00BA4, Kendo ROOM1010 @0x0269A). Der Port zeigt Johns Schuesse ohne Blitz; die Kamera steht dabei auf Cut 10 hinter Roy, John ist
   klein am Gangende (m6, Framedump). Der Ton ist jetzt der RE2-Skript-Schuss; das Bild bleibt wie bisher (Runde 36
   OFFEN 2). Eigener Punkt, falls gewuenscht.
2. **RE2-Folge-Satz** 0x0B (Welle 2bf10f4ea91f, hell, Schwerpunkt 2090 Hz) folgt in RE2 jedem Skript-Schuss 1-9 Bilder
   spaeter, in ROOM4090/6020 am ZIELORT positioniert (Abpraller/Einschlag). Johns Schuesse treffen Roy -> nicht
   uebernommen; fuer einen Fehlschuss waere er die RE2-Form.
3. **Positions-Ton**: RE2/RE1.5 setzen Skript-Geraeusche teils positional (Se_on Flags != 0, z.B. ROOM3091 @0x01A62);
   der Port spielt alle Se_on ohne Lage (scd_vm.c op_se_on "Positional-Pfad (OPEN)"). Der Schuss in 1041 klingt deshalb
   so laut wie ein naher Schuss (-21,2 dB(A), lauteste 100 ms) — er ist ja auch das Ereignis, auf das Elza reagiert.
4. **Wahrnehmung**: Pegel und Klangklasse des alten (Alligator) und neuen (Skript-Schuss) Tons liegen nah beieinander
   (-20,9 / -21,3 dB(A), Schwerpunkt 859 / 597 Hz); geaendert ist die HERKUNFT (jetzt der Ton, mit dem RE2 im Skript
   schiesst). Die Ton-Ausschnitte in H_belege/ sind zum Gegenhoeren da.

## 7. Fuer den Nutzer / Orchestrator

* **Neue/geaenderte Assets (Paket-Gate)**: `re15_port/shared_assets/RE2/ELZASE.VBS` GEAENDERT (14232 B, md5
  ba96a66267286e4754c6b5a7481928a4; vorher 13144 B). Das bestehende Gate (release/make_package.sh: vorhanden + cmp gegen
  den Quellbaum) deckt die Datei ab; die Android-APK muss neu gebaut werden (Asset-Gate prueft gegen den Baum).
  Erzeugt von `re15_port/tools/r36_l/re2_elza_se_cut.py --install` (Selbsttest gegen info/re2leon).
* **Neue Quelldateien** (Android-Configure frisch, Memory reai-v2-android-glob-cache): engine/src/elzatoene_r38.c;
  Tests tests/unit/test_r38_elzatoene.c, tests/unit/probe_r38_elzatoene_kreuz.c (kein add_test),
  tests/integration/test_r38_elzatoene.cmake. **RE15_MIN_TESTS +8** (6 Unit + 2 Integration).
* **Sprachdateien**: keine neuen; ROOM1041 msg 2 nutzt die vorhandene synchro/STAGE1/room1041/main02.wav, msg 3 (der
  Platzhalter) hat keine Datei und keinen Text mehr.
* **Gemeinsame Dateien**: scd_vm.c (Include + 3 Haken-Zeilen, Kommentar "Runde 38 Spur H": Raum-Scan, Opcode-Haken,
  op_message_on hinter re15_leiche_message_on — Spur F setzt ihr "Help"-Ton-statt-Text evtl. an dieselbe Stelle:
  beide Zeilen behalten), tests/unit/test_r36_elza.c (Teil bank: Quelle Satz 1).
* **Bedienhinweis**: ROOM1041 (Elza, erster Gang durch den Flur, Zugang z.B. ueber ROOM1031): statt des Textfensters
  "*gunshot*" faellt ein Schuss, danach dreht sich Elza um ("What was that!?"). ROOM1021 (Szene nach Johns Befreiung):
  Roy stoehnt, sobald er auf John zugeht; Johns zwei Schuesse sind jetzt der RE2-Pistolenschuss.
* **Kontrolle selbst**: elza.log (RE15_ELZA_LOG=elza.log) zeigt `[elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1` bzw.
  `[elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2`; Ton-Ausschnitte H_belege/*.wav.
* **Entscheidungsfragen**: keine. (Falls zu den Schuessen ein Muendungsfeuer gewuenscht ist: OFFEN 1.)
