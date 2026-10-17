# Runde 38 — Spur F "dining1141" — Abnahme 1 (unabhaengig, laufend geschrieben)

Baum `.claude/worktrees/r38_dining1141` (Zweig `r38/dining1141`, Kopf 8c59363d, Merge-Basis mit master 32a646af).
Pruefer: unabhaengige Abnahme 1 (2026-10-17) nach Nachbesserung 1 (Maengel M1-M4 der Abnahme 0, `F_abnahme_0.md`).
Grundlage: Wortlaut `AUFTRAG.md` (Runde 38 + Nachtrag 2) und `analysis/befunde_runde37/AUFTRAG.md` Nachtrag 14,
`VERTRAG.md`, Dossier `F_dining1141.md` (Abschnitt 9). Dem Bau-Agenten wird nichts geglaubt, was hier nicht selbst
gemessen ist. Messordner (scratchpad, nicht committet): `F38abn1/`.

## 0. Stand der Abnahme
- Bau: `local_build.sh configure` + `build` gruen ("ninja: no work to do", LOCAL-BUILD-OK). exe 07:40 = nach dem
  letzten Code-Commit 7a0bb681 (07:39); alle spaeteren Commits aendern nur Doku/Bilder bzw. 06d53363 die Test-CMake
  (configure lief). Arbeitsbaum sauber.
- Code-Diff der Nachbesserung gelesen (`git diff cde46d8f HEAD -- re15_port`): dining_r38.c (Programm SZENE1021 am
  Ereignis 2, Anker A_SZENE1021, Raumaufbau 1131 "Elza ging waehrend des Wartens hinaus"), gen/dining_r38_programme.inc
  (SZENE1021 24 B, BEFREIUNG 1154 -> 1200 B: Doppeltuer frei + While-Kette Elza-in-Bahn + Member_set(6,3)), Header
  (Konstanten mit Beleg), dining_r38_pc.c (Kommentar M4), Tests (3 Unit-Teile, 2 Integrations-Teile).
- Gezielte Unit-Riegel (eigene 10 + Gruppen aller beruehrten Module, 94 Tests): **94/94 gruen (12,1 s)**,
  Log `F38abn1/ctest_unit.log`.
- Gezielte Integration (eigene 7 Teile + Module/Raeume des Diffs, 23 Tests, -j3): **23/23 gruen (861,6 s)**.
- Eigene Messlaeufe an der exe: A1, A2, A4, A5, A7, A8b, A9 (Elza, echte Tueren), L1 (Leon) — Abschnitt 2.
- **Ergebnis: BESTANDEN** — Punkte 1-4 erfuellt, M1-M4 der Abnahme 0 behoben (gemessen), Gates halten (Abschnitt 3-5).

## 1. RE-Gate (Stichproben der Nachbesserung selbst disassembliert / Bytes gelesen, `re15_disasm.py`/`re2_disasm.py`)
| Behauptung (Ort im Diff) | selbst gemessen | Urteil |
|---|---|---|
| Koerper-Schub FUN_8002aec4: Bit 0x2 auf einer der beiden Figuren = kein Schub (@0x8002af04-@0x8002af0c, Header RE15_R38F_JOHN_WORT0) | PSX.EXE @0x8002aef8 `lw a0,0(s3)`, @0x8002aefc `lw v1,0(s2)`, @0x8002af04 `or v0,a0,v1`, @0x8002af08 `andi v0,v0,0x2`, @0x8002af0c `bne v0,zero,0x8002af30`; @0x8002af30 `j 0x8002b464` / `addu v0,zero,zero` | stimmt |
| Member_set Id 6 = Wort 0 (`sw a2,0(a0)` @0x800411c8, Tabelle @0x80010c8c) | Tabelle @0x80010c8c [6] -> 0x800411c4 `j 0x80041230`, Verzoegerungsplatz @0x800411c8 `sw a2,0(a0)` | stimmt |
| Member_cmp Form `3e 00 <id> <op> lo hi`, GET Id 0/2 = +0x34/+0x3c, Operator 2 `>=`, 4 `<=` | @0x800412a8 `lhu s0,2(v0)` (Id = Low-Byte `andi a1,s0,0xff` @0x800412c0, Operator `sra s0,s0,8` @0x800412c8), @0x800412ac `lh s1,4(v0)`; GET-Tabelle @0x80010cfc [0] -> @0x80041384 `lw v0,52(a0)`, [2] -> @0x8004139c `lw v0,60(a0)`; Operator-Tabelle @0x80010cdc [2] -> @0x8004130c `slt v0,v1,s1` + `xori 1` (>=), [4] -> @0x80041320 `slt v0,s1,v1` + `xori 1` (<=). Programmwerte `fe de`/`ce e6`/`80 a8`/`55 dd` = -8450/-6450/-22400/-8875 = Header BAHN_* | stimmt |
| While mit Bedingungskette LAB_8003f6f4 (Kettenlaenge pc+1, Verbinder 0 = UND) | @0x8003f74c `lh s2,2(a2)`, @0x8003f790 `lbu v1,-3(a2)` (Kettenende), @0x8003f7cc `lhu s0,0(a2)` Verbinder, @0x8003f7fc `beq s0,zero` -> @0x8003f80c `and s2,s2,v0`, sonst @0x8003f808 `or`; Port scd_vm.c scd_eval_pred_chain bildet das nach | stimmt |
| Gosub @0x8003fc04 (Ruecksprung pc+2 je Ebene, Ziel aus der Sub-Tabelle des geladenen RDT), Evt_end-Rueckkehr @0x8003f218 | @0x8003fbf0 `lbu a2,1(v1)`, @0x8003fbf4 `addiu v1,v1,2`, @0x8003fc04 `sw v1,324(v0)`, @0x8003fc2c `lw v1,0x3f70(0x800b)`, @0x8003fc3c `sb a1,2(a0)` (Ebene+1), @0x8003fc44 `sw v0,28(a0)`; Evt_end @0x8003f1fc `bne v1,zero` (Ebene>0) -> @0x8003f218 `lw v0,324(v0)` / @0x8003f220 `sw v0,28(a1)`, @0x8003f230 Ebene-1; Ebene 0 @0x8003f204 Faden aus | stimmt |
| ROOM1021 main00 @0x01DE2 (Begehzone Slot 5 -> Ereignis 2), @0x01E24/@0x01E28 Ifel Ck(3,0x5F), @0x01E2C Aot_set Slot 6; sub02 @0x01FD8..; Anker-FNV | Bytes gelesen: @0x01DE2 `2c 05 03 41 00 00 d4 cc 74 dc 34 08 a0 0f ff 00 18 02 00 00`, @0x01E24 `06 00 3c 00 21 03 5f 01`, @0x01E2C `2c 06 03 31 00 00 0e ed 8c d8 e8 03 e8 03 ff 00 18 06 00 00` = Programm SZENE1021 +0002 bytegleich; @0x01FD8 `22 04 60 00 22 03 c9 00 22 04 f0 01 22 03 5f 01 46 05 ..`; @0x021B2 `3c 01 22 02 07 00 22 01 1b 00 01 00`; FNV 0x1FD8+532 = 94BC6DB4, 0x1DE2+94 = 40492B6F = Header | stimmt |
| RE2 Tuertext = key_type - 0x4C (@0x800516b4), Herz-Schluessel 0x5B -> Text 0x0F @0x8009F230 | info/re2leon/PSX.EXE @0x800516b0 `addiu a1,zero,256`, @0x800516b4 `addiu a2,s0,-76`, @0x800516b8 `jal 0x8002fe38`; Offsettabelle @0x8009F368 [15] = 612 -> 0x8009EFCC+0x264 = 0x8009F230: `fc 25 50 3a 4f 00 48 4b 3f 47 41 40 01 fd 00 1d 00 44 41 3d 4e 50 00 45 4f 00 41 50 3f 44 41 40 00 51 4a 40 41 4e fc 50 44 41 00 47 41 55 00 44 4b 48 41 01 fe 00`; Port-Text Zeilen 2/3 Glyphen gleich (Punkt 0x01 -> 0x57, Steuercodes in RE1.5-Form `04 02`/`02 00`/`08`/`01 00`) | stimmt |
| Kaesten 450+450 (PSX.EXE @0x80073e94, STAGE1.BIN @0x801217f8 ueber @0x8011ddd8/@0x8011dde0) | @0x80073e94 und @0x801217f8 je `00 00 06 fa 00 00 c2 01 fa 05 c2 01` (+6 = 0x01c2 = 450); STAGE1 @0x8011ddd8 `lw v0,6148(v0)` (0x80121804 -> 0x801217f8), @0x8011dde0 `sw v0,120(v1)`; STAGE1 @0x801163e4 `ori v0,v0,0x2` (Bit-0x2-Setzer im Overlay) | stimmt |
| Formen: While ROOM1141 sub03 @0x00FA8, Member_set ROOM11B0 @0x01706, Member_cmp ROOM1040 @0x018C0, Cut 7 Satz @0x00140, Cut_old ROOM1150 @0x010F2, Tuer-Nutzlast 1141 main00 @0x00A52+14 | Bytes: `0f 04 08 00 21 05 00 00 02 00 10 00`; `34 06 01 02`; `3e 00 0c 06 80 00`; Cut 7 Lage (-7465,-3024,-9613) Ziel (-4265,2209,-27792); `29 06 2b 01 ff ff 02 00 2a 00 3c 01`; `.. ec fa 00 00 82 c9` | stimmt |
| M4: `lw a0,8(v0)` @0x8004513c | @0x80045130 `lui v0,0x800b`, @0x80045134 `lw v0,-14472(v0)` (0x800ac778), @0x80045138 `nop`, @0x8004513c `lw a0,8(v0)` | stimmt (Kommentar korrigiert) |

- Rate-Woerter im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/approx): **0 Treffer**. Neue
  Umgebungsschalter nur Messhaken (RE15_R38F_GORILLAS_TOT/_STARR, RE15_R38F_SICHT/_JOHN_AUS), kein Abschluss per Schalter.
- Die Bahn-Grenzen BAHN_* sind eine abgeleitete PORT-WAHL (Ecke @0x00F8E, GANG_X, Kaesten 450+450, gemessene Bildgrenze
  von Cut 7 z -9775) und im Header mit Herleitung gekennzeichnet; keine Zahl ohne Herkunft.

## 2. Messungen am Spiel (echte exe 07:40, eigene exe-Kopie je Lauf `re15_pc_abn1F_<lauf>.exe`, Elza per
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, Wegplan RE15_WEGPLAN, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1,
RE15_MSG_LOG/RE15_STATE_LOG/RE15_SPAWN_DIAG/RE15_SE_DEBUG/RE15_EVT_TRACE/RE15_BGM_CTL_DEBUG; Starter `F38abn1/lauf.sh`)

### 2.1 Lauf A1 — die ECHTE 1021-Szene, danach Roys Leiche im SELBEN Besuch (M1, Punkt 1/2)
Stand vor der Szene wie Abnahme 0 E1a: `RE15_SET_FLAG=4:0xF1,3:0x74,4:0xEC,4:0xFC,4:0x60,3:0xC9,3:0x2C,3:0xBA,7:0xAA,
7:0xAB,7:0xAD,7:0xB5,7:0xB8,7:0xB9 RE15_DEBUG_JUMP=1021@gp RE15_PLAYER_POS=-15500,-7100,0`, danach nur Gehen/Druecken
(Wegplan `a1/wegplan.txt`: in die Begehzone Slot 5, nach der Szene an die Leiche (-3730,-9600) Blick 2048, AKTION),
`RE15_FRAMEDUMP=1400-2800/5`, Ende Bild 2800 in 1021. Dauer 193 s.
- debug.log: `[dining] F64 ROOM1021 Ereignis 2 (Szene sub02) -> Programm SZENE1021: Gosub sub02, danach Leichen-Zone
  Slot 6 (main00 @0x01E2C)`; `Sce_em_set type=0x49` (-4274,-6590) und `type=0x47`; Szene msg 0..8 vollstaendig.
- AOT-Liste F1812 (Steuerung frei, derselbe Besuch): `AOT slot 6 typ 0 ev 6 flags 0x31 band 0 mitte (-4350,-9600)
  halb (500,500)` (vor dem Fix fehlte Slot 6, Abnahme 0 E1a); Slot 5 typ 11 (Szene verbraucht).
- Elza bei (-3803,-9601) Blick 2053 (genau die Stelle der Abnahme 0 E1a), Druck F2030 -> Zustands-Log ab F2060
  `msg(a=1 fsm=1 id=40`, debug.log `[msg] room=1021 id=40`; `loaded room1021.rdt` genau EINMAL im Lauf.
- Bildbogen `F38abn1/a1_leiche_selber_besuch.png`: F2060 "H(e's dead.)" -> F2140 "Poor Roy..." -> F2215..F2270
  "There's an empty key tag on his belt. / It reads “Dining Room”." — Wortlaut exakt wie vom Nutzer, im Besuch der Szene.
- **M1 behoben (gemessen).**

### 2.1a Lauf A8b — echte 1021-Szene -> Hinweis -> Tuer nach 1011 -> Schluessel -> zurueck nach 1021 (Punkt 2)
Gleicher Stand wie A1, Wegplan `a8b/wegplan.txt` (Szene, Leiche 5x AKTION, Tuer Slot 1, in 1011 an Roys Liegestelle,
Aufnahme, Tuer Slot 0 zurueck, Leiche erneut), Ende Bild 3000 im zweiten 1021-Besuch. (Erster Versuch a8 mit 4 Drucken
blieb am offenen Hinweis-Fenster stehen: der Text braucht wie in A1 5 Drucke — Wegplan, kein Spielfehler.)
- 1. Besuch: `Programm SZENE1021`, msg 0..8, danach msg 40 im selben Besuch.
- 1011: `[dining] ROOM1011: Dining Room Key (Item 0x43, Bit (9,64)) Prop obj 6 bei (-2000,0,7450), Zone Slot 14`
  (= Roys Liegestelle, Spawn sub00 @0x009F6); AOT Slot 14 typ 2 mitte (-2000,7450); Aufnahme an (-1372,7415) -> (9,64) = 1.
- 2. Besuch 1021: **kein `Sce_em_set type=0x49` (John weg)**, (4,0x60) = 0, AOT Slot 6 Leiche aktiv, Druck -> msg 40.
  Zustands-Log je Besuch: 1. Besuch 2499 Bilder mit John (t=49), msg 40 aktiv 222 Bilder; 1011 537 Bilder; 2. Besuch
  3000 Bilder, 0 mit John, msg 40 aktiv 222 Bilder; sauberes Ende `EXIT_AT: Bild 3000 in Raum 1021`. Dauer 311 s.

### 2.2 Lauf A2 — Elza ROOM1131 OHNE Schluessel (M2, Punkt 2)
Flags = nach der 1021-Szene (`4:0xF1,3:0x74,4:0xEC,4:0xFC,3:0x2C,3:0xBA,4:0xF0,3:0x5F`), `RE15_DEBUG_JUMP=1131@gp
RE15_PLAYER_POS=-2200,-2150,0` (Eintrittspunkt aus 1121), zu Fuss an die Doppeltuer (-1300,-13900) Blick 0, AKTION.
- `[dining] F1005 ROOM1131 Slot 0 (Tuer -> ROOM1141) gesperrt: Ereignis 29, Flag (9,66) = 0`; Druck F236: Schloss-Ton
  (`SE-Stimme 6`), `[msgtext] room=1131 id=40 "It's locked.A heart is etched under the key hole."`, kein Raumwechsel,
  (9,66) = 0 (F488).
- Bildbogen `F38abn1/a2_tuer_zu.png`: F248-F280 "It's locked." -> F312-F400 "A heart is etched under / the key hole.";
  ueber der Tuer haengt im Hintergrund das Schild "Briefing Room" — der Text nennt kein Schild mehr, also kein
  Widerspruch zum Bild. Der Text ist RE2s eigener Text der Herz-Schluessel-Tuer (Abschnitt 1). **M2 behoben.**

### 2.3 Lauf A4 — echter Weg 1131 (mit Schluessel) -> 1141, Elza steht beim Tod der Gorillas AUF Johns Bahn (E10-Lage)
und geht dann nach OSTEN aus der Bahn (Abgang mit Elza IM Bild von Cut 7) (M3, Punkte 1/2/3/4)
Flags nach der 1021-Szene `3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78`, `RE15_GIVE=0x43:1 RE15_DEBUG_JUMP=1131@gp
RE15_PLAYER_POS=-1300,-13900,0 RE15_R38F_GORILLAS_STARR=1 RE15_R38F_GORILLAS_TOT=600` (Messhaken: Kampf nicht Teil der
Spur), Wegplan `a4/wegplan.txt` (Tuer benutzen + durchgehen, 600 Bilder Phase H, Gemaelde "ja", nach der Szene an
(-7418,-14952), 1500 Bilder warten, dann zu Fuss nach (-5600,-14952), danach zur Dachtuer), `RE15_FRAMEDUMP=0-9000/3`,
Ton-Mitschnitt `a4/cap.raw`. Dauer 298 s.
- 1131: `ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1`, Tuer -> `loaded room1141.rdt`.
- Phase H: `ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150)`; Klopfen `0x3C id=10/11/11` bei
  F1/35/65, 245/275/305, 485/515/545, 725/755/785; Hilferuf `0x3D id=0 -> Stimme main42.wav` F155/395/635; Musik
  `Sce_bgm_control slot=0 op=1` beim Eintritt. Erste Nachricht in 1141 = msg 13 (Gemaelde) -> in Phase H kein Text
  (Zustands-Log: erstes `msg(a=1` in 1141 bei F855 = id 13). Bildbogen `F38abn1/a4_phase_h.png`: F156/F168/F180,
  F396/F408, F636/F648 (je direkt nach dem Hilferuf) ohne Textfenster; F840/F852 Cut 5 Rolladen zu, John nicht im Bild;
  erster Text F900 "A beautiful painting.".
- Mitschnitt (eigene Auswertung `ton.py`/`knock.py`): Hilferuf an Takt 3809/4049/4289 Korrelation 0,856/0,810/0,764
  (Kontrolle 3 s vorher 0,054-0,056), Verstaerkung 1,28-1,31 = deutlich im Mix; erster Klopfschlag je Dreier 10-ms-Spitze
  x6,0..x21,4 gegen davor, Schlag 2/3 nur x0,84..x2,84 (Beobachtung B3 der Abnahme 0 unveraendert).
- Gemaelde F854 -> `Programm BEFREIUNG`; Rolladen-Ton `0x3B id=12/10/11` F1046/F1061/F1256 (Punkt 3 am echten Weg);
  Musik op=2 (John frei); Szene msg 40, 3, 4, 5, 6, 7, 8, (Gorillas: Musik op=1), 9, 10, 11 — nie 0/1/2.
- Messhaken F3247 (Gorillas tot), Musik op=2. **Warten:** Zustands-Log F3240..F4380 (1140 Bilder) Elza (-7442,-14881)
  in der Bahn, cam 0, pm 0 (Steuerung frei), John reglos in der Ecke (-7531,-21363) — kein Abgang.
- Elza geht los, verlaesst die Bahn bei x -6392 (> -6450): F4395 cam 7, pm 2. John F4440 (-7377,-15975) -> F4470
  (-7358,-9975) -> F4530 (-7351,-975) -> F4547 geparkt, `Se_on bank=14 id=0/1` (DOOR1A), F4547 Cut_old -> cam 0, F4561
  pm 0. **Elza waehrend pm 2 unverrueckt (groesste Abweichung 0, `dist.py`)**; kleinster Abstand John-Elza 985 bei F4445
  in Cut 7 (Elza (-6392,-14881), John (-7373,-14975)) — > 900, kein Koerperkontakt.
- Bildbogen `F38abn1/a4_warten_abgang.png`: F3300/F3900/F4380 Cut 0, John wartet links neben der Doppeltuer, Elza rechts;
  F4398..F4545 Cut 7: Elza steht links im Bild, John laeuft aus der Ecke an ihr VORBEI (F4446 Schulter an Schulter, keine
  Durchdringung, Elza bleibt stehen) und verlaesst das Bild unten (F4455); F4548 Cut 0. Danach Dachtuer -> `loaded
  room1171.rdt`.
- **M3 fuer die E10-Lage behoben (gemessen):** vorher (Abnahme 0 E10) Durchdringung + 2,3 m Rutschen im Bild.

### 2.4 Lauf A7 — Elza geht waehrend des Wartens durch die Doppeltuer hinaus und kommt zurueck (neuer Weg der Nachbesserung)
Wie A4, nach der Szene an die Doppeltuer (-7900,-17750) (in der Bahn), 1200 Bilder warten, AKTION (Tuer), in 1131
zurueck durch die Tuer, in 1141 zur Dachtuer. `RE15_FRAMEDUMP=0-9000/6`. Dauer 295 s.
- Messhaken F3242; F4119 Wegplan-FLAG (9,67) = 0 (John wartet noch, ~880 Bilder); Druck auf die Doppeltuer -> KEINE
  Sperre mehr (Gorillas tot), `loaded room1131.rdt`, `[dining] F4120 ROOM1131: Elza hat 1141 vor Johns Abgang verlassen ->
  John ist aufs Dach gegangen: (9,67) = 1, Riegel (4,0xC3) = 1`; Wegplan-FLAG in 1131: (9,67) = 1, (4,0xC3) = 1.
- Zurueck in 1141: Zustands-Log des Besuchs (636 Bilder) **0 Zeilen mit John (t=49)**, kein `Sce_em_set type=0x49`; AOT
  Slot 0 Tuer, Slot 1 Tuer (Dachtuer offen), keine Riegel-Zone; Elza (-7600,-17600) -> (-7255,-1029) -> `loaded
  room1171.rdt`. Kein John taucht auf oder verschwindet im Bild.

### 2.5 Lauf A5 — Elza steht beim Tod der Gorillas genau auf Johns Zielpunkt vor der Dachtuer (E7-Lage) (M3)
Wie A4, nach der Szene zu Fuss nach (-7350,-450) (Ankunft (-7332,-718)), 1500 Bilder warten, dann Dachtuer.
`RE15_FRAMEDUMP=2400-9000/3`. Dauer 277 s.
- Messhaken F3247 -> Cut 7 ab F3277 (Elza ausserhalb der sichtbaren Bahn: sofort, nach Sleep 30), John (-7531,-21363)
  -> (-7399,-20375) F3300 -> ... -> (-7351,-975) F3405, geparkt F3420, DOOR1A auf/zu, F3429 Cut_old -> cam 6, F3443 pm 0.
- **Elza unverrueckt (Abweichung 0 waehrend pm 2)**; kleinster Abstand 258 bei F3398 — John steht hinter der Kamera von
  Cut 7 (z -975 gegen Kamera z -9613) in Elzas Kasten, OHNE Schub (Bit 0x2), in keinem Bild sichtbar.
- Bildbogen `F38abn1/a5_dachtuer_abgang.png`: F3246/F3270 Cut 6 Elza vor der Dachtuer; F3279..F3345 Cut 7, John laeuft aus
  der Ecke auf die Kamera zu, F3354 aus dem Bild; F3366..F3420 leerer Raum (Tuer-Toene im Off); F3429..F3501 Cut 6, Elza
  an DERSELBEN Stelle vor der Dachtuer. Vorher (Abnahme 0 E7) stand sie danach rechts neben der Tuer (Lagesprung).
- **M3 fuer die E7-Lage behoben (gemessen).** Beobachtung B5 (Abschnitt 5).

### 2.5a Lauf A9 — Tuersperre im Kampf an beiden Tueren, danach Abgang mit Elza ausserhalb der Bahn (Punkt 2/4)
Wie A4, nach der Szene an die Doppeltuer (AKTION), dann an die Dachtuer (AKTION), dann nach (-5500,-19000) und warten;
`RE15_R38F_GORILLAS_TOT=1800`, `RE15_FRAMEDUMP=2400-9000/3`. Dauer 293 s.
- Szene: `[msgtext]` 40 ("Are you okay? what's happened?"), 3 "John: Somehow... Elza, listen to me!", 4, 5, 6, 7, 8,
  9, 10, 11 "John: Elza, I'll try to distract them, while you try to get out of here, hurry!" — kein "John, wait"/"sorry".
- Doppeltuer F2913 (Elza (-7900,-17718), cam 0) und Dachtuer F3315 (Elza (-7269,-1052), cam 6): je `[msgtext] room=1141
  id=41 "I cannot leave him alone with those monsters."`, kein Raumwechsel.
- Messhaken F4436 -> Cut 7 ab F4466 (Elza (-5508,-18993) ausserhalb der Bahn: nach Sleep 30 sofort), John geparkt, DOOR1A
  auf/zu, F4618 Cut_old -> cam 0, F4632 pm 0; Elza unverrueckt (Abweichung 0), kleinster Abstand 1884 (F4496, cam 7).
  Danach Dachtuer -> `loaded room1171.rdt`.
- Bildbogen `F38abn1/a9_sperre_abgang.png`: F2925..F3000 Cut 0 Sperrtext an der Doppeltuer (John links in der Ecke),
  F3318..F3405 Cut 6 Sperrtext an der Dachtuer, F4467..F4593 Cut 7 (Elza im Bild, John laeuft an ihr vorbei aus dem Bild),
  F4620 Cut 0.

### 2.6 Lauf L1 — Leon ROOM1140, Rolladen ueber sub03 (Punkt 3)
`RE15_SET_FLAG=2:0 RE15_GOTO_ROOM=1140 RE15_FORCE_EVENT=3@60 RE15_PRESS=square@120,square@180,square@240` (Leon, wie der
Vorher-Lauf des Dossiers v1140 und integration_..._l), `RE15_FRAMEDUMP=0-700/10`, Mitschnitt `l1/cap.raw`.
- `[force-event] scd_event_fire(3) at F60`, msg 1 (Frage), `Port-Bank 0x3B geladen`, `Se_on bank=0x3B id=12` F243,
  `id=10` F258 (Abstand 15 = Sleep @0x00A54), `id=11` F453.
- Mitschnitt (`knock.py`, 10-ms-Huellkurve): Klick bei Stille (Median vorher 0 -> Spitze 2849), Motor x21,9, Anschlag
  x3,5 — im Mix deutlich. Bildbogen `F38abn1/l1_leon_rolladen.png`: F240 "There's a switch below. Will you push it? Yes",
  F260..F410 Cut 5, der Rolladen faehrt hoch, F440 offen.
- Am echten Weg zeigt Leons Gemaelde in v0.8.25 wie jetzt msg 5 "The switch is not responding..." (Abnahme 0 E4a/E4v,
  sub01 @0x00C7A; von der Nachbesserung nicht beruehrt). Elzas 1141 (gleicher Rolladen) am echten Weg: Lauf A4 0x3B
  12/10/11. **Punkt 3 weiter erfuellt.**

### 2.7 Riegel (gezielt, keine volle Suite)
- Unit: eigene 10 Teile + Gruppen aller beruehrten Module (item_*, inv_*, scd_*, aot_*, se_*/audio_*/voice_*, msg_*,
  r30_tuer*/r33_tuer1120_*, door_seq, r36-Schluessel/Inventar/Elza, r37 Roy/Elza-EMS/Itemband, room1140_*,
  r34n_a_rolltor, enemy_body_push, member*): **94/94 gruen** (`F38abn1/ctest_unit.log`).
- Integration (-j3, unter Last mit anderen Spuren): integration_r38_dining1141_{w,z,d,l,s,r,a} + r37_roy_szene1011/1021,
  r37_elzaems_elza1141, r36_schluessel(_befreiung), r36_inventar, item_name_census, r36_story_elza, door_lock, keydoor,
  r36_tueren, r36_elza_{fenster,irons,unlock,schuss,stoehnen}: **23/23 gruen (861,6 s)** (`F38abn1/ctest_integ.log`).
  Teil A neu ausgewertet (`-DAUSWERTEN`): "Messhaken F3247, Cut 7 ab F4176, Elza davor (-7371,-8823), in Cut 7
  (-7371,-8823), danach (-7371,-8823), Bilder in der Bahn 928" — alle Pruefungen gruen. Teil S (Framedump, 535 s):
  sicht.txt meines Laufs: John sichtbar in 2055 Bildern, zuerst F1090 (16 px = Fuesse unter dem Rolladen bei y -240),
  zuletzt F3352 (945 px am unteren Rand von Cut 7), F3353 0 px — bitgleich zum Lauf des Bau-Agenten.
- `programme_bauen.py --pruefen`: gen-Datei = Werkzeug ("ok").

## 3. Urteil je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg (eigene Messung) |
|---|---|---|
| 1 Hinweis „There's an empty key tag on his belt. / It reads "Dining Room".“; "Help" und Klopfen nur Ton, kein Text | **erfuellt** | A1: nach der ECHTEN Szene im selben Besuch msg 40, Bilder F2215..F2270 Wortlaut exakt (`a1_leiche_selber_besuch.png`); A8b: auch beim Wiederbetreten; A4: in Phase H keine Nachricht (erste = msg 13), Hilferuf als Sprachzeile main42 (Korrelation 0,76-0,86, Verstaerkung 1,28-1,31), Klopfen als SE 0x3C |
| 2 Ablauf Nachtrag 14 (1141 vorher zu; Roys Schluessel an seiner Liegestelle in 1011; John weg, wenn Elza zurueckkommt; ohne "John, wait"/"sorry", direkt "are you okay? what's happened?"; Dining-Room-Schluessel; Hinweis bei Roy; Klopfen/Help regelmaessig bis zum Schalter mit panischer Musik; "I cannot leave him alone with those monsters" an Tueren, solange die Gorillas leben) | **erfuellt** | A2: Tuer zu, Text "It's locked." / "A heart is etched under / the key hole." (RE2 0x0F, kein Widerspruch mehr zum Schild, M2); A8b: echte Szene -> Hinweis -> Schluessel bei (-2000,7450) in 1011 -> zurueck: kein John; A4/A9: Szene 40, 3..11, nie 0/1/2; A4: Klopfen alle 240 Bilder + Hilferuf + MAIN16 bis zum Gemaelde, danach still; Rolladen-Ton; A9: msg 41 an Doppel- UND Dachtuer im Kampf; A7: nach dem Tod der Gorillas Doppeltuer frei. M1 (Hinweis im Besuch der Szene) behoben (A1) |
| 3 ROOM1140 Rolltor ohne Ton beim Hochfahren | **erfuellt** | L1: Leon sub03 0x3B 12/10/11 F243/F258/F453, Klick/Motor/Anschlag im Mitschnitt (x21,9 Motor), Bilder `l1_leon_rolladen.png`; A4: Elza 1141 am echten Weg 0x3B 12/10/11; integration_r38_dining1141_l gruen. Leons Gemaelde am echten Weg = msg 5 wie in v0.8.25 (Original sub01 @0x00C7A, Abnahme 0) |
| 4 Nachtrag 2 "keine plötzlich rein poppenden Charaktere" (1141, John) | **erfuellt** | integration_r38_dining1141_s in MEINEM Lauf gruen: John zuerst F1090 (Fuesse unter dem hochfahrenden Rolladen), zuletzt F3352 (laeuft am unteren Rand aus Cut 7), nirgends Auftauchen/Verschwinden ohne Bewegung; A4/A5/A9: Abgang ohne Koerper-Schub, Elza unverrueckt (Abweichung 0) an der E10-, E7- und Suedlage, kleinster Abstand im Bild 985/1884 (> 900); A7: beim Wiederbetreten kein John. M3 behoben |

## 4. Gates
- **RE-Gate: haelt** (Abschnitt 1; neun Stichproben der Nachbesserung selbst disassembliert/gelesen: FUN_8002aec4 Bit 0x2,
  Member_set Id 6, Member_cmp + GET + Operatoren, While-Kette, Gosub/Evt_end, ROOM1021-Bytes + Anker-FNV, RE2-Text 0x0F +
  @0x800516b4, Kaesten 450/450 + Overlay-Setzer Bit 0x2, Formen 1141/11B0/1040/1150; M4-Zitat). Keine Rate-Woerter; neue
  Umgebungsschalter nur Messhaken. BAHN_* als hergeleitete PORT-WAHL gekennzeichnet.
- **Fix erklaert den Befund:** M1 — Slot 6 fehlte, weil nur main00 ihn setzt; SZENE1021 setzt ihn nach dem Gosub (A1:
  Slot 6 in der AOT-Liste F1812). M3 — Schub ueber re15_body_push (Gate `(pusher->flags | pushee->flags) & 0x2` =
  FUN_8002aec4 @0x8002af04); Member_set(6,3) setzt John-flags = 3 (actor_common.c case 6); in der Bahn wartet der Abgang
  (A4: 1140 Bilder kein Abgang). Bit 0x2 hat im Port keine weitere Bedeutung (grep: nur der Schub-Gate).
- **Vertrag: haelt.** Bank 9 nur 64/66/67 (k_port[] in test_r36_kartenbits.c); Nachrichten unveraendert 1021/40,
  1131/40-41, 1141/40-41 (+42 nur Stimme) — die Nachbesserung aendert nur den TEXT von 1131/40 und setzt (9,67) zusaetzlich
  in 1131; keine neuen Ids/Bits/Ereignisse (Ereignis 2 in 1021 ist das Original-Ereignis der Szene, es wird ueber den
  Haken umgeleitet; Anker anderer Spuren in sub02 bleiben gueltig: integration_r36_elza_schuss/_stoehnen und
  r37_roy_szene1021 gruen). Pfade (git diff master...HEAD --name-only): keine release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists (Registrierung tests/unit/probes/r38_dining1141.cmake); gemeinsame Dateien nur
  Haken mit "Runde 38 Spur F" (scd_room_setup.c 2, scd_vm.c 4, audio_pc.c 5, main.c 13 Zeilen in mehreren 1-5-Zeilen-
  Haken, Inventar-/Item-Dateien 2-4) — die Nachbesserung beruehrt keine gemeinsame Datei.
- **Riegel: vorhanden, messend, gruen** (Abschnitt 2.7). Neue Teile messen die Mechanik: unit abgang (Abweichung Elzas
  je Bild, Abstand im Bild, Warten in der Bahn), szene1021 (Slot 6 + msg 40 nach der echten Szene), weg1131; integration
  R (1021 nur einmal geladen, msg 40 nach msg 8) und A (928 Bilder Warten, 0 Abweichungen). Die Gegenprobe gegen den
  alten Modulstand (Dossier 9.6: szene1021 3, abgang 7 Fehler) habe ich nicht selbst gefahren; die Pruefgroessen (Slot 6
  nach der Szene, Abweichung Elzas je Bild, Wartebilder) sind genau die, an denen Abnahme 0 den alten Stand (E1a, E5-E10)
  rot gemessen hat.
- **Nebenwirkungen:** beide Szenarien geprueft — Leon 1140 (L1, integration_l), Leon nutzt 1130/1140 ohne Haken;
  Elza 1021/1011/1131/1141/1171 ueber echte Tueren (A1-A9).

## 5. Maengel
**Keine.** Die Maengel der Abnahme 0 sind am gebauten Stand gemessen behoben:
- M1 (Hinweis im Besuch der Szene) — A1/A8b: Slot 6 direkt nach der Szene, msg 40 im selben Besuch (1021 einmal geladen).
- M2 (Tuertext gegen Schild) — A2: "It's locked." / "A heart is etched under / the key hole." (RE2 Text 0x0F), kein Schild.
- M3 (Schub beim Abgang) — A4 (E10-Lage, Elza dann im Bild von Cut 7), A5 (E7-Lage), A9 (Suedlage), integration A
  (Dachgang-Eingang): Elza in keinem Lauf verschoben; in der sichtbaren Bahn wartet der Abgang (A4: 1140 Bilder).
- M4 (Zitat) — @0x8004513c `lw a0,8(v0)` selbst disassembliert, Kommentar stimmt.

Beobachtungen (kein Mangel gegen den Wortlaut, zur Kenntnis fuer Orchestrator/Nutzer):
- B1/B3/B4 der Abnahme 0 unveraendert: Schluessel am Boden nur ein kleiner Lichtpunkt (Modal gross); vom Klopf-Dreier ist
  der erste Schlag laut (10-ms-Spitze x6..x21), Schlag 2/3 liegen nahe am Musikpegel (x0,8..x2,8, A4); der Hilferuf ist
  Johns ruhiges "Help me out." (ersetzbar ueber synchro/STAGE1/room1141/main42.wav).
- B5 (neu): Steht Elza beim Tod der Gorillas IN der Dachtuer (E6/E7-Lage, A5), laeuft der Abgang sofort; John laeuft
  hinter der Kamera von Cut 7 ohne Schub durch ihre Lage (kleinster Abstand 258, kein Bild zeigt es), die Tuer-Toene
  kommen aus dem Off, danach zeigt Cut 6 Elza unveraendert vor der Tuer. Nichts taucht im Bild auf oder springt; nur die
  gedachte Bahn Johns fuehrt durch die Tuer, vor der Elza steht (Plausibilitaet, kein sichtbarer Fehler).
- B6 (Information, vom Bau-Agenten selbst gemeldet): Schluesselname/Anhaenger sagen "Dining Room" (Nutzer-Vorgabe), das
  Hintergrundbild ueber der Tuer 1131 -> 1141 zeigt "Briefing Room"; der Tuertext nennt seit M2 kein Schild mehr.

## 6. Messordner / Werkzeuge (scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/F38abn1/`, nicht committet)
`lauf.sh` (eigene exe-Kopie je Lauf), Wegplaene `a1,a2,a4,a5,a7,a8b,a9,l1/wegplan.txt`, Logs `*/debug.log`, `*/state.txt`,
`*/wegplan_log.txt`, Mitschnitte `a4/cap.raw`, `a5/cap.raw`, `l1/cap.raw`, Auswertung `ton.py` (Kreuzkorrelation),
`knock.py` (10-ms-Huellkurve), `spur.py`/`dist.py` (Lagen, Abstand, Abweichung aus dem Zustands-Log), `bogen.py`;
Bildboegen `a1_leiche_selber_besuch.png`, `a2_tuer_zu.png`, `a4_phase_h.png`, `a4_warten_abgang.png`, `a4_gorillas.png`,
`a5_dachtuer_abgang.png`, `a9_sperre_abgang.png`, `l1_leon_rolladen.png`; ctest-Logs `ctest_unit.log`,
`ctest_integ.log`, Unit-Ausgabe `unit_abgang.txt`. Jeder Lauf ist mit Wegplan + Env-Zeile aus Abschnitt 2 reproduzierbar.
Die Einzelbilder (*.ppm, 5036 Stueck, ~1,2 GB) sind nach dem Erstellen der Bildboegen geloescht (Platte); Logs, Wegplaene, Boegen
und Mitschnitte bleiben.

## 7. Ergebnis
**BESTANDEN.** Alle vier Nutzer-Punkte am gebauten Stand gemessen erfuellt; RE-Gate, Vertrag und Riegel halten; keine
Maengel. Beobachtungen B1/B3/B4/B5/B6 zur Kenntnis.
