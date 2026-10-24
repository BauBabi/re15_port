# Runde 41 — Spur G "kanister2080" — UNABHAENGIGE ABNAHME 0 (2026-10-24/25)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, HEAD 489c0756 (Basis r41/integration 9f10bdb0).
Pruefer: Abnahme-Agent (glaubt nichts Ungemessenes). Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).

## 0. Stand der Abnahme
- [x] Lektuere AUFTRAG/VERTRAG/Dossier/Diff (G-Diff gegen die Basis 9f10bdb0: 32 Dateien, +3054/-1)
- [x] Bau (local_build.sh configure + build: LOCAL-BUILD-OK, ninja 'no work to do'; exe 23:17:18 juenger als jede Quelle, Baum sauber = HEAD 489c0756)
- [x] gezielte Tests 34/34 (Regex des Bau-Agenten) + integration_r41_re2gator 1/1 (272 s) - alle gruen
- [x] P1 gemessen (Leon + Elza, echter Leiterweg 2090 -> 2080/2091 -> 2081, Ton im Mitschnitt, Nachtrag 20 mit 66 s unten)
- [x] P2 gemessen (Cut 5 gegen Gas_Can.bmp)
- [x] P3 gemessen (Leon + Elza; Aufnahme/Explosion an G's Landepunkt; natuerlicher Ablauf ohne GB_TEST)
- [x] RE-Gate (22 Stellen selbst gelesen/disassembliert)
- [x] Vertrag
- [x] Urteil: NICHT BESTANDEN (P3 teilweise)


Bildbelege und Laufordner liegen NICHT im Baum (Auftrag: nur diese Datei committen), sondern unter
`<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g0`.

## 1. Bau und gezielte Tests (selbst gefahren)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. Kontrolle: `find re15_port/{engine,include,platform} -newer re15_pc.exe -name '*.c|*.h|*.inc'` leer,
  `git status` sauber -> die exe ist der Stand HEAD 489c0756.
- `ctest --test-dir re15_port/build -R "unit_r41_kanister2080|integration_r41_kanister2080|unit_r36_kartenbits|unit_scd_|unit_r41_re2gator_"
  --timeout 600 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 34`**, `Total Test time (real) = 227.07 sec`
  (integration_r41_kanister2080 184.74 s). Log: scratchpad g0/ctest1.log.

## 2. RE-Gate: Stichproben selbst disassembliert / gelesen
| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| RE1.5 Gegner-Band-Vergleich `lbu v1,130(a3)` + `bne` @0x8003b228-3c | `8003b234: lbu v1,130(a3)` (a3 = g_entity), `8003b238: srl v0,v0,28` (Zellen-Halbwort >> 12), `8003b23c: bne v1,v0,0x8003b50c` | stimmt |
| Tot-Gate Sce_em_set @0x80042128-38 | `80042128: beq a1(pc[7]),0xff` -> sonst `80042130: jal 0x8004efe4` + `80042138: beq v0,zero` | stimmt |
| y = -Etage*1800 @0x800421d4-0c | `800421d4: lbu v1,2(s2)` ... `800421f8..8004220c` = 0 - ((v1*7*32+v1)*8) = -1800*v1 | stimmt |
| RE2 Door_exit-Ton @0x800141f0 (info/re2leon/PSX.EXE) | `800141f0: jal 0x8005ba28`, `800141f4: lui a0,0x1` (in RE1.5 PSX.EXE steht dort anderer Code -> richtige Binaerdatei = RE2) | stimmt |
| RE2-Schwaeche EM23 @0x80101ebc-dc | `80101ebc: lh v0,342(s0)`, `80101ec4: slti v0,v0,25000`, `80101ed0-dc: lhu/ori 0x20/sh 536(s0)` | stimmt |
| Kopfschuss w7/hp=-1 @0x80012380-88, @0x800124fc-51c | `80012380: ori v0,zero,0x8`/`bne`/`ori v0,zero,0x7`; `800124fc: andi 0x40`, `80012510: sltiu v0,v0,0x20`, `8001251c: sh -1,154(s1)` | stimmt |
| Nachfuehren 0xC8 @0x80033f90, RUN d<300 @0x80051764, WALK d<100 @0x800513f8 | `ori a1,zero,0xc8`; `slti v0,v0,300`; `slti v0,v0,100` (je nach SquareRoot0) | stimmt |
| RE2 ROOM1060 Knall @0x0F48, EDT @0x3F80, VAG 5 @0x066B8 | `36 02 0a 01 00 00 dc 88 f8 f8 3d 05`; `00 00 85 20`; sha1 192e09df824ba47854001928662ec82e9fd33522; Kopf +8/+C/+10 = 0x3F58/0x4018/0x4C38 | stimmt; gen/knall_bank.inc ist dieselbe Datei wie irons_tod_1150.c:1357 / elzazombies_r39.c:1123 (Satz 0 = KNALL_SE_TUER) |
| RE2 ROOM7020 sub12/sub13/sub18 (re2_scd_walk.py, SCD-main-relativ) | 0726 `2907`, 0750 `3f0a1240`, 0754 Sleep 1, 0758 Plc_dest `40001400..`, 0764 Message_on 5, 076A Sleep 30, 0782 Plc_motion, 0786 Sleep 20, 078E Super_reset, 079C-07A4 Do/Ck(5,0e), 07A8 Sleep 15, 07D4 `290b`, 0812 Plc_dest; sub13 088E-08A2 Speed_set (2,10)(1,20)(0,25)(8,0)(7,20)(6,5), 08B0 Member_cmp `3e000c03e0fc` (y < -800), 08BA Se_on 0x0e; sub18 0A2C-0A38 | stimmt (Wurf-Clip: RE2 Ada `3f 0a 12 40` = Clip 0x12 ihrer Bank; Port nimmt Marvins Eigen c6 - PORT-WAHL aus dem Clip-Zensus 1.8, als solche nicht im Header benannt, s. Maengel) |
| ROOM2080 Treppe @0x0BE2, Absatzwand SCA @0x05C4, Tueren @0x0BA2/@0x0BC2, Wasser @0x0C2E/@0x0C5E | `2c 02 0c 31 80 00 54 a7 46 ba 9c 18 fc 08 00 00 03 00` (x -22700..-16400, side 0, Sprung 3); Zelle x -17650..-16650 typ 1 floor 0x88; Tuer 2030 Band 5, Tuer 2090 Band 8; y -10800 / -3420 | stimmt |
| RE2 ROOM40A0 Kanister-Modell @0x0452, Aufschlag @0x0686 (SCD-main) | `2d 01 .. b8ac f8f8 ccdb` + Drehung 0 (Modell 1 stehend); `36 02 0b 01 04 01 ..` | stimmt; `kanister2080_se_cut.py` (ohne --install) erzeugt KANISTER2080.VBS md5 848c40f6c077d937fb025df9ae1bc62b und r41_kanister2080_bank.inc bytegleich zum Commit (Ausgabe danach geloescht) |
| ROOM2030 Tuer @0x1AEA (+22/+23), ROOM2090 Tuer @0x0996 | `.. 01 08 ..` -> STAGE2/2080; 2090 Slot 0 -> 2080 Cut 4 Etage 8 | stimmt |
| ROOM1021 sub02 @0x020C2-F2, ROOM2020 @0x008F6/@0x0090A, ROOM2000 @0x017B4, ROOM2001 @0x0179E | Bytes wie zitiert (`3f 02 06 00`, `09 0a 6e 00`, `3f 02 07 00`, `09 0a 1e 00`, `43 00 80 00`, `09 0a 14 00`; `44 01 16 02 01 00 01 bd`; `44 01 40 40 ..`; `44 00 49 40 ..`) | stimmt |
- Zusatz (der Bau-Agent hat ihn NICHT gefahren, G greift aber per Haken in ROOM2090 ein): `ctest -R "^integration_r41_re2gator$"
  --timeout 900` -> `1/1 Test #1155: integration_r41_re2gator ... Passed 272.22 sec`.

## 3. Messungen an der exe (Messkopie re15_pc_g41abn.exe, Laufordner <SP>/runs/<lauf>)
Gemeinsam: RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore, Titel-Autostart (RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 bzw. Elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1).

### 3.1 P1 Leon, echter Tuerweg 2090 -> 2080 (Leiter, DOOR16) - Lauf p1a_leon
Befehl: `RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_KEIN_GATOR=1
RE15_PLAYER_POS=-8900,-25244,2048 RE15_INPUT_SCRIPT=W1.0,A0.2,W12.0 RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
RE15_FRAMEDUMP=0-400/10:f_ RE15_EXIT_AT=400#2080` -> rc 0.
- debug.log: `[tuer] Sequenz Archiv 2 DOOR16 ...` (Z.165) -> `[k2080] F31 Knall: Knall-Bank 19976 B in der Tuerbank, Satz 0 (RE2 ROOM1060
  Se_on 0x0a @0x0F48 ...)` (Z.185) -> `[se] Stimme: se=0 layer=0 vag=0 note=67 fine=0 center=91 vol=127 pitch=0x400 (11025 Hz)`
  + `layer=1 ... fine=1` (Z.186/187) -> `... Bit 114 = 1` -> `ROOM2080 nach Bit 110: 8 lebende Zombies (Tot-Bits 236..243)` ->
  8 x `[spawn-diag] Sce_em_set type=0x16 behavior=0x02 slot=0..7 pos=(..,-14400,..)` (Z.198-205). ERST Knall, DANN Zombies: ja.
- Mitschnitt: `knall_im_mitschnitt.py cap.raw` -> `RE2_1060 (11025 Hz): t=15.57 s Bild 467 r=0.891 Huelle-max 6853`, DOOR04/R1030
  kein Treffer; 867 Bilder - 400 (EXIT_AT) = Bild 467 = erstes 2080-Bild. Der Knall liegt also wirklich im gemischten Ton.
- Ebene-Zeilen F30..F390: `8 Zombies ... y -14400..-14400, auf der Treppe (x < -16650) 0`.
- Bild: <SP>/belege/p1a_leon_2080_cut4_f0-400.png (Cut 4 = Ankunft von der Leiter): die 8 Zombies stehen im oberen
  Hauptbereich am Rechner/Regal, ab ~F330 setzen sie sich in Bewegung (Leon steht an der Leiter).

### 3.2 P1 Elza, echter Tuerweg 2091 -> 2081 - Lauf p1a_elza
Befehl wie 3.1 mit RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, `RE15_EXIT_AT=400#2081` -> rc 0.
`[tuer] Sequenz Archiv 2 DOOR16` -> `[k2080] F31 Knall: ...` + beide [se]-Lagen (center 91, 11025 Hz) -> `ROOM2081 nach Bit 110: 8 lebende
Zombies` -> 8 x Sce_em_set y -14400; 13/13 Ebene-Zeilen ohne Treppe; Mitschnitt `RE2_1060 ... Bild 1243 r=0.891` = 1643 - 400 = erstes
2081-Bild. Bild <SP>/belege/p1a_elza_2081_cut4.png.

### 3.3 Nachtrag 20 (oberhalb der Treppe), eigene Variante mit langem Warten unten - Lauf n20_treppe2
Befehl: `RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2080@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-16000,-16700,2048,8
RE15_INPUT_SCRIPT=W2.0,A0.2,W65.0 RE15_R39_MESS_SPIELER_HP=100 RE15_STAIR_DBG=1 RE15_STATE_LOG=state.log
RE15_FRAMEDUMP=0-2100/150:f_ RE15_EXIT_AT=2100#2080` (der Agent wartete unten 18 s, hier 66 s).
- stair_dbg.log `[stair] START dir=DOWN cur_band=8 target_band=5`; state.log (eigene Auswertung, alle `t=16`-Aktoren): 2351 Bilder,
  16760 Zombie-Saetze, **x-Minimum -16250, 0 Bilder mit einem Zombie westlich x -16650**, Leon unten (x < -19000) in 1994 Bildern ab F107.
- 70/70 Ebene-Zeilen `auf der Treppe (x < -16650) 0, nicht auf y -14400: 0`.
- Bild <SP>/belege/n20_treppe_unten_66s_cut0.png (Cut 0 = Treppe von oben: Leon unten an der 2030-Tuer, Treppe leer, F150..F2100).
- Gegenprobe (Lauf n20_treppe, Warten 5 s statt 2 s): die Zombies erreichen Leon oben vorher, er kommt nicht zur Treppe; auch dort
  0 Zombie-Saetze westlich -16650 (x-Minimum -16242).

### 3.4 P2 Gaskanister an der Reling - Lauf p2_cut5
Befehl: `RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2080@250 RE15_PLAYER_POS=-12000,-24000,0,8 RE15_FORCE_CUT=5 RE15_FRAMEDUMP=40:f40.ppm
RE15_EXIT_AT=45#2080` (ohne Bit 110).
- `[k2080] F1 Kanister pos=(-6745,-14400,-17625) Cut 5 Kasten=(159..177,75..158) Dreiecke 46`.
- Bild <SP>/belege/p2_gascan_vs_port_cut5_x2.png (links Gas_Can.bmp, rechts Port Bild 40): gleicher Hintergrund (Cut 5,
  "SEPTIC POOL"); der Kanister steht aufrecht genau an der roten Marke am Ende des Balkons zwischen den Relingpfosten
  (Zoom <SP>/belege/p2_marke_zoom_x4.png). Er ist RE2-gross (2784 hoch) und ragt deutlich ueber die Reling.
- Die dunkle Flaeche im Port-Bild ist das Original-Wasserobjekt (sub00 @0x0C2E, y -10800, solange (3,245) = 0) - nicht G.

### 3.5 P3 Leon - Szene (Ausloeser mit echtem Treffer) - Laeufe p3a_leon (bis 2080 F660) und p3r_leon (bis 2090 F900)
Befehl (wie der Riegel `szene`): `RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_GB_TEST=5 RE15_PLAYER_POS=6500,-25000,3072
RE15_GIVE=3:15 RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT=W2.0,M0.3,MA0.1,M0.3,MA0.1,W1.0
RE15_STATE_LOG=state.log RE15_FRAMEDUMP=0-660/10:f_ RE15_EXIT_AT=660#2080` (p3a, dazu CAP_SYNC + SE_DEBUG) bzw. `...0-900/30 ...900#2090` (p3r).
- Ausloeser: `F100 AUSLOESER geschwaecht (RE2 +0x218&0x20 @0x80101ebc-dc), Treffer 1` -> `ROOM2080 SZENE (Marvin 0x40, 8 lebende Zombies)`.
  Schwelle selbst disassembliert (Abschnitt 2): `slti v0,v0,25000` @0x80101ec4 -> +0x218 |= 0x20. Ohne Messhaken = 300 Schaden
  (Pistole 43 Treffer) - Entscheidungsfrage 1 des Dossiers, durch "oft genug" gedeckt.
- "erneut die Geraeusche einer schliessenden Tuer": `F8 Tuer-Ton: DOOR06-Tonteil 19608 B ..., Satz 1` + `[se] Stimme: se=1 layer=0 vag=2
  note=61 ... (16397 Hz)` (Door_exit-Satz, @0x800141f0 selbst gelesen).
- Marvin im Raum: Bild <SP>/belege/p3a_bogen1.png (Cut 2, Marvin in der Oeffnung zum Treppenabsatz, 8 Zombies davor).
- Kopfschuesse: 8 x `Schuss n: Treffer Slot s ... hp=-1 state=3 zeile=5`, je direkt danach `Kopf ab: Slot s (Part 8 Flagwort 0x4B ...)`
  (F148..F429, 40 Bilder Takt), Schuss-Ton `[se] ... vag=1 ... (16397 Hz)` + Gore `se=9`. Bilder <SP>/belege/p3a_kopf_zoom.png
  (Blutwolke am Kopf, Bild 150/160/200) und p3a_bogen2.png (Bild 350..470: Kopf platzt, die Zombies liegen danach am Boden).
- Lauf zum Kanister: `NPC Lauf Sub 5 Phase 2 (-14418,-16612) -> (-11450,..) -> (-8480,..)` = ~200/Bild (Rennen), Cut 5.
- Aufheben/beide Haende: `F550 Kanister aufgehoben (Loco c2 Bild 33)`; Bilder p3a_bogen3.png / p3a_haende_zoom.png: Marvin
  bueckt sich, steht dann mit dem Kanister quer vor dem Koerper (RE2-gross, ragt weit zu beiden Seiten), an der Reling quer vor
  der Brust.
- Zeile: state.log `msg(a=1 fsm=1 id=42` in 92 Bildern; Bild p3a_bogen4.png "Marvin: Hey, Leon, try with this!" (Stimme
  synchro/STAGE2/room2080/main42.wav fehlt -> `[voice] ... keine Aufnahme ... Zeile bleibt stumm`, wie im Dossier fuer den Nutzer
  angekuendigt).
- Wurf: `F620 Wurf: Start (-6328,-16896,-17627) v=(25,20,10) a=(5,20,0) -> Landung (-2278,-3576,-17267) nach 36 Bildern`, Aufschlag
  `F656 ... KANISTER2080.VBS Satz 0` + `[se] ... center=95 ... (8193 Hz)`. Schnittfolge state.log 2 (F1) -> 5 (F477) -> 7 (F569).
  Bilder p3a_bogen4.png: Cut 7 Totale, Marvin klein oben auf dem Balkon, der Kanister faellt in die Grube.
- Rueckkehr (p3r): `F671 ROOM2090 RUECKKEHR ... Zone-7-Tot-Bits 236..243 gesetzt: 8 von 8`, `Kanister im Becken (-2278,-1200,-17267)`,
  gator_boss.log `RE2 KANISTER gesetzt (-2278,-1200,-17267)`, Zeichner `[gator-re2] Kanister Stufe 0 ... Kasten=(254..317,98..120)
  cut=0`. Bild p3r_bogen.png: der Kanister liegt (dunkel) im Wasser am Plattformrand.

### 3.6 P3 Elza - Lauf p3a_elza
Wie 3.5 mit Elza-Wahl, `RE15_EXIT_AT=660#2081`: `AUSLOESER ... -> ROOM2081`, `ROOM2081 SZENE (John 0x49 ...)`, Tuer-Ton, 8 x Kopf ab,
8 x `zeile=5`, `F550 Kanister aufgehoben`, `F620 Wurf ... Landung (-2250,-3545,-17276)`, Aufschlag. Bild <SP>/belege/p3a_elza_bogen.png
("John: Hey, Elza, try with this!").

### 3.7 P3 Alligator nimmt den Kanister auf + Kiefer-Tod (Spur-B-Mechanik an G's Landepunkt)
- Lauf auf_C: `RE15_GB_TEST=1 RE15_GATOR_KANISTER=-2278,-17267` (= G's Landepunkt, Hoehe -1200 wie G's API-Aufruf),
  `RE15_PLAYER_POS=-2300,-23000,3072` (Leon suedlich des Kanisters): gator_boss.log `RE2 KANISTER Stufe 1 laengs=5879 quer=1716 ... F1104`,
  `KANISTER angehaengt ... F1116`, `KANISTER getragen (Stufe 7)`.
- Lauf auf_C_schuss (dasselbe + Pistole, Zielen/Feuern ab Bild 1140): `RE2 R1 VSync 4 (Zeitlupe) F1225`, `R1 start pos=(-4306,-22599)`,
  `R1 ende (Leiche)` - der RE2-Explosionstod laeuft (Bild <SP>/belege/auf_C_schuss_bogen.png, Blitz Bild 1230; die Darstellung ist
  Spur B's Abnahme).
- Laeufe auf_A (Leon an der Leiter) und auf_B (Leon bei (400,-24000)): 1500 Bilder, KEINE Aufnahme - der Gator nimmt den Kanister nur,
  wenn er geradewegs ueber ihn auf Leon zuschwimmt (RE2-Fenster laengs 8601..8999 x Skala, quer Koerperradius + 267).
- Im G-Riegel ist die Aufnahme nach der Szene nicht geprueft (vom Bau-Agenten offen benannt).

### 3.8 P3 natuerlicher Ablauf OHNE RE15_GB_TEST - Lauf n3_natur (Befund)
Befehl: `RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_PLAYER_POS=-6000,-17000,0 RE15_GIVE=3:15 RE15_EQUIP=3
RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT=W10.0,(Zielen/Feuern-Bloecke) RE15_FRAMEDUMP=0-1500/30:f_
RE15_EXIT_AT=1500#2090` (kein GB_TEST: der Gator taucht wie im Spiel per Naehe auf).
- Erste Visite: `RE2 AUFTAUCHEN F6`, `INTRO ende`, Kampf; `F1239 AUSLOESER ... Treffer 1` -> Szene -> `F671 ROOM2090 RUECKKEHR`, `Kanister im
  Becken (-2278,-1200,-17267)`.
- Nach der Rueckkehr: gator_boss.log `F1 R2090 GATOR VERSCHWUNDEN`, `F2 R2090 gator da`, danach **1500 Bilder lang nur `TICK ph=1
  pos=(-7200,-9200)`** (Lauerplatz, untergetaucht, B9) - der Raum 2090 wird beim Rueckweg neu aufgebaut, der Alligator ist ein frischer
  Aktor (HP 25300, untergetaucht), der erst bei Annaeherung (< 9000, GB_AGGRO) bzw. Treffer wieder auftaucht und dann den Intro-Durchbruch
  ERNEUT spielt (enemy_ai_boss_gator.c GBP_LURK -> re15_gr2_intro_start; kein Merkbit in gator_re2_r41.c / enemy_ai_boss_gator.c).
  Bild <SP>/belege/n3_rueckkehr_bogen.png (2090 nach der Szene: Kanister im Wasser, kein Alligator, 50 s).
- Der Messlauf des Bau-Agenten (p3h) lief mit RE15_GB_TEST=5 - dieser Testhaken laesst den Gator aufgetaucht starten und VERDECKT genau
  dieses Verhalten.

## 4. Gates

### 4.1 RE-Gate
- Stichproben (Abschnitt 2): 9 RE1.5-EXE-Adressen, 2 RE2-EXE/EM23-Adressen, 4 RE2-RDT-Stellen, 7 RE1.5-RDT-Stellen selbst gelesen -
  alle zitierten Stellen enthalten das Behauptete (richtige Binaerdatei: @0x800141f0 ist RE2-Code, in RE1.5 steht dort anderes).
- Suche im Code-Diff (9f10bdb0..HEAD -- re15_port) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt: 0 Treffer.
  7 x PORT-WAHL/NUTZER-VORGABE gekennzeichnet.
- Luecken (klein, s. Maengel M4): Dauern ohne eigenen Beleg und ohne PORT-WAHL-Kennzeichnung: `OP_SLEEP(10)` zwischen zwei Zielen
  (Kommentar zitiert nur die Drehrate 0xC8/Bild @0x80033f90, nicht die 10), `OP_SLEEP(30)` nach dem Tuer-Ton (Kommentar "RE2 ROOM7020
  sub12 Sleep 30 @0x076A (Form)" = Wert aus anderem Zusammenhang: dort nach Message_on), `OP_SLEEP(30)` im Rueckkehr-Programm (Kommentar
  "Wsleep @0x081A" - RE2 wartet dort per Wsleep, keine 30 Bilder). Der RE2-Wurf-Clip (Ada `3f 0a 12 40` = Clip 0x12 ihrer Bank @0x0750)
  ist durch Marvins Eigen c6 ersetzt; im Header steht "Plc_motion @0x0750" ohne PORT-WAHL-Vermerk (der Clip-Zensus 1.8 begruendet c6).
- Erklaert der Fix den Befund? P1/N17: der gemischte Ton enthaelt den RE2-ROOM1060-Knall im ersten 2080-Bild (r 0,891), vorher
  (Dossier 2.1) DOOR06 - ja. N20: Band-8-Huelle + Gegner-Bandvergleich @0x8003b228-3c erklaeren, warum kein Zombie auf die Treppe kommt;
  gemessen 66 s - ja.
- PSX-Pfad: `gcc -std=gnu11 -fsyntax-only -Wall -Wextra -Wno-comment -I include -I engine/src engine/src/kanister2080_r41.c` rc 0 ohne
  Warnung, mit -DRE15_PLATFORM_PC ebenso.

### 4.2 Vertrag
- Bank-9-Bits 114/115/116 (Zuteilung G 114-119), je eine k_port[]-Zeile in test_r36_kartenbits.c, unit_r36_kartenbits gruen.
- Zone-7-Tot-Bits 236..243 (Bank re15_em_status_zone, nicht Teil der VERTRAG-Zuteilung): eigener Zensus (Sce_em_set pc[7] + Ck/Set
  Bank 7 in allen RDTs STAGE1-3, scd_walk_lib): 236..245 nirgends belegt, 230-235/246-252 belegt -> frei. Port: kein anderes Modul nutzt sie.
- Nachrichten-Id 42 in 2080/2081 (Zuteilung 42-47).
- Pfade (git diff 9f10bdb0 --name-only): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists;
  neues RE2-Asset shared_assets/RE2/KANISTER2080.VBS (erlaubt, reproduzierbar). Gemeinsame Dateien: scd_room_setup.c 1 Zeile, scd_vm.c
  1 Zeile, platform/pc/main.c 2 Zeilen, je "Runde 41 Spur G".
  (Die Zeilen synchro/STAGE2/room2030/main46.wav in `git diff master --stat` kommen von master nach dem Abzweig, nicht von G.)
- Tests je Punkt: unit lage (P2), spawn/ebene (P1/N20), szene/start/bahn/texte (P3), integration ankunft/ebene/kanister/szene_leon/szene_elza;
  parallel-fest (eigener Laufordner + exe-Kopie re15_pc_r41g_<id>), Linux-fest (m gelinkt, cmake_minimum_required, keine .exe auf Linux).
  LUECKE: kein Riegel prueft nach der echten Szene Aufnahme + Explosion, und alle Szenen-Laeufe fahren mit RE15_GB_TEST=5 (s. M1/M3).
- Nebenwirkungen: integration_r41_re2gator auf diesem Zweig gruen; Elza-Varianten (2081/2091, John) gemessen; 2080 ohne Bit 110:
  keine Zombies, Kanister steht (Riegel kanister); nach Bit 115 keine Zombies/kein Kanister (Code).

## 5. Urteile (gegen den Wortlaut, AUFTRAG.md; Aufteilung FG-E1)

| Punkt | Urteil | Beleg |
|---|---|---|
| P1 Knall (N17), dann 8 Zombies oben im Hauptbereich | **erfuellt** | 3.1/3.2: echter Leiterweg, `Knall: Knall-Bank ... Satz 0` vor 8 x Sce_em_set y -14400, Mitschnitt RE2_1060 r 0,891 im ersten 2080/2081-Bild, Bild Cut 4 |
| N20 Zombies oberhalb der Treppe | **erfuellt** | 3.3: 66 s Leon unten, 0 von 16760 Zombie-Saetzen westlich x -16650, 70/70 Ebene-Zeilen; Mechanismus @0x8003b228-3c / SCA @0x05C4 / Treppe @0x0BE2 selbst gelesen |
| P2 Gaskanister an der Reling (Gas_Can.bmp) | **erfuellt** | 3.4: Cut 5 = Gas_Can.bmp, Kanister (RE2 ROOM40A0 Modell 1, @0x0452 stehend) steht an der Marke |
| P3 Ausloeser, Tuer-Ton, Marvin/John, 8 Kopfschuesse, Kanister in beide Haende, Zeile, Wurf, liegt im Wasser | erfuellt (Teilschritte) | 3.5/3.6: Leon + Elza, Logs + Bildboegen |
| P3 "Wenn der Aligator dran vorbei kommt, soll er ihn aufnehmen ... Sterbe-Animation" | **teilweise** | 3.7: Mechanik (Spur B) greift an G's Landepunkt (Aufnahme F1104, R1-Tod); 3.8: im natuerlichen Ablauf ist der Alligator nach der Szene ein frischer, untergetauchter Aktor am Lauerplatz (1500 Bilder `ph=1`), der Kampf beginnt mit erneutem Intro von vorn (M1); kein Riegel fuer Aufnahme/Tod nach der echten Szene (M3) |
| N5 Kamera des Wurfs "GENAUSO" wie Adas Raketenwerfer-Wurf | **teilweise** | Struktur RE2 (Cut 7 Werfer + Zeile, Sleep 30/20, Loslassen, Aufschlag, Sleep 15, Schnitt auf den Fangenden) ja; Blick von unten/nah (RE2 Cut 7: 43,6 Grad, 8908) nein - 2080 Cut 7 Totale (6,4 Grad ueber, 17168). Eigene Projektion: keine 2080/2090-Kamera zeigt den Werfer von unten (2090 Cut 12: Balkonende 19-24 px ueber dem Bildrand) - offene Entscheidung (M2) |
| N21 Zugang ueber die Leiter | **erfuellt** (keine Aenderung noetig) | Leiterweg 2090 Slot 0 @0x0996 -> 2080 Cut 4 in 3.1 gefahren; die Szene merkt sich Lage/Cut beim Ausloesen |
| RE-Gate | haelt (mit kleinen Luecken M4) | Abschnitt 2 / 4.1 |
| Vertrag | haelt | Abschnitt 4.2 |
| Tests (34/34 + integration_r41_re2gator) | gruen, aber Luecke M3 | Abschnitt 1 |

**bestanden = false** (P3 und N5 nur teilweise).

## 6. Maengel (nummeriert, nachpruefbar)

**M1 (P3, schwer) - Nach der Szene beginnt der Alligator-Kampf von vorn.** Die Szene wechselt 2090 -> 2080 -> 2090; beim Rueckweg wird
ROOM2090 neu aufgebaut und der Alligator neu gesetzt: HP 25300, untergetaucht am Lauerplatz (-7200,-9200) (B9), Auftauchen erst bei
Abstand < 9000 / Treffer, dann spielt der Intro-Durchbruch ERNEUT (enemy_ai_boss_gator.c GBP_LURK -> re15_gr2_intro_start; kein Merkbit
in gator_re2_r41.c / enemy_ai_boss_gator.c). Gemessen Lauf n3_natur (ohne RE15_GB_TEST): nach `F671 ROOM2090 RUECKKEHR` 1500 Bilder nur
`TICK ph=1 pos=(-7200,-9200)`; Bild <SP>/belege/n3_rueckkehr_bogen.png (Becken leer bis auf den Kanister). RE2 (das Vorbild ROOM7020)
setzt den Boss im selben Raum neu (Pos_set @0x07C4) und laesst ihn weiterkaempfen. Der Messlauf des Bau-Agenten (p3h) und alle
Szenen-Laeufe des Riegels fahren mit RE15_GB_TEST=5, der genau dieses Verhalten verdeckt (Gator startet aufgetaucht); die Dossier-Aussage
"Gemessen (Lauf p3h mit RE15_GB_TEST=5): Gator wieder aufgetaucht ..., verfolgt Leon" ist damit kein Beleg fuer das Spiel.
Forderung: Kampfzustand ueber die Szene erhalten (kein erneutes Untertauchen/Intro, Gator nach der Rueckkehr in der Verfolgung, wie RE2)
oder Orchestrator-/Nutzer-Entscheid (Dossier-Frage 2); Riegel OHNE RE15_GB_TEST.

**M2 (N5) - Wurf-Kamera nicht "GENAUSO".** RE2 ROOM7020 Cut 7 zeigt Ada von unten (43,6 Grad unter dem Kopf, Abstand 8908); der Port
zeigt Marvin/John in 2080 Cut 7 als kleine Figur in der Totale (6,4 Grad ueber dem Kopf, 17168; Bild <SP>/belege/p3a_bogen4.png). Eine
Kamera von unten mit dem Werfer im Bild gibt es nicht (selbst projiziert: 2090 Cut 12 sieht die Wand unter dem Balkon, Balkonende
(122.8,-24.3) knapp ueber dem Bild; die Fallbahn erreicht 2090-Kameras erst ab Bild 30 von 36). Der Bau-Agent hat das benannt (OFFEN 2),
aber ohne Entscheid "fertig" gemeldet. Forderung: Orchestrator-/Nutzer-Entscheid (analog B8) - bis dahin teilweise.

**M3 (P3, Riegel-Luecke) - Aufnahme und Kiefer-Tod nach der ECHTEN Szene sind nicht gesichert.** integration_r41_kanister2080 endet mit
`Kanister im Becken`; Aufnahme/Explosion werden nur von Spur B's Riegel mit dessen Testhaken RE15_GATOR_KANISTER an einer anderen Stelle
(-1500,-23400) geprueft. An G's Landepunkt nimmt der Gator den Kanister nur, wenn er gerade darauf zu schwimmt (Laeufe auf_A/auf_B je 1500
Bilder ohne Aufnahme, auf_C mit Aufnahme F1104). Forderung: ein Riegel Szene -> Kanister im Becken -> Aufnahme (`KANISTER angehaengt`)
-> Schuss -> `R1 start`/`R1 ende`, im natuerlichen Ablauf (nach M1).

**M4 (RE-Gate, klein) - Dauern ohne eigenen Beleg und ohne PORT-WAHL-Vermerk** in kanister2080_r41.c: `OP_SLEEP(10)` zwischen zwei Zielen
(baue_szene; zitiert nur 0xC8/Bild @0x80033f90), `OP_SLEEP(30)` nach dem Tuer-Ton (Wert von RE2 @0x076A, dort nach Message_on), `OP_SLEEP(30)`
in baue_rueck (RE2 hat dort Wsleep @0x081A, keine 30). Ausserdem ist Adas Wurf-Clip (`3f 0a 12 40` @0x0750) durch Marvins Eigen c6
ersetzt, ohne PORT-WAHL-Vermerk im Header. Forderung: herleiten oder als PORT-WAHL mit Begruendung kennzeichnen.

**M5 (Doku, klein)** - test_r36_kartenbits.c k_port[] Bit 114: "2080: Tuer-Ton beim ersten Betreten nach Bit 110" (ist seit Nachtrag 17
der Knall); main.c-Haken "Tuer-Ton DOOR06, Aufschlag, NPC-Schuss" ohne den Knall (vom Bau-Agenten selbst benannt).

## 7. Hinweise (kein Mangel dieser Spur)
- H1 Integration mit F: der Knall faellt in F's 2090-Einstellung UND beim ersten Betreten von 2080 (FG-E1). Ob zweimal gewollt ist, bei der
  Integration im Spiel pruefen.
- H2 Zone-7-Tot-Bits 236..243 stehen nicht im VERTRAG (der teilt nur Bank 9 zu); F darf sie fuer seine 2080-Szenenfiguren nicht verwenden.
- H3 In diesem Zweig sind die Spinnen (Typ 0x25) noch in 2090 (F entfernt sie): in den natuerlichen Laeufen n1/n2 traf die Zielhilfe die
  Spinnen statt des Alligators.
- H4 Schwelle 300 Schaden = 43 Pistolentreffer (Dossier-Frage 1) - durch "oft genug" gedeckt, Nutzer fragen.
- H5 Der Kanister ist RE2-gross (2784): an der Reling ragt er weit ueber das Gelaender, in den Haenden 1392 zu jeder Seite (kein Massstab
  erfunden, Dossier OFFEN 5). Im 2090-Becken zeichnet Spur B ihn sehr dunkel.
- H6 Die Messkopien re15_pc_g41abn*.exe wurden nach der Abnahme geloescht; ein Lauf des Kanister-Schnittwerkzeugs (ohne --install) legte
  build/r41_kanister2080/ im Baum an - bytegleich geprueft und wieder entfernt.
