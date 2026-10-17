# Runde 38 Spur I — heliport1171: Unabhaengige Abnahme 0

Pruefer: unabhaengige Abnahme 0 (2026-10-16). Baum `.claude/worktrees/r38_heliport1171`, Zweig `r38/heliport1171`,
HEAD 171daec2, Basis e2cc7673. Bericht wird fortlaufend geschrieben (Sitzungslimit).

## 0. Gelesen

- AUFTRAG.md (Wortlaut Punkt: "Elza stuck in ihrer Cutscene im ROOM 1170."; Lesart Elza = ROOM1171), VERTRAG.md
  (Spur I: Bank-9-Bit 69 nur wenn noetig; msg 1171 40-44; gemeinsame Dateien nur kleine Haken), Dossier
  I_heliport1171.md.
- `git log master..HEAD`: 10 Commits (5e2cd1ec .. 171daec2). `git diff master...HEAD --name-only` (Spur-eigene
  Aenderung; das Zwei-Punkt-`git diff master` zeigt zusaetzlich die seit e2cc7673 auf master gekommenen
  Orchestrator-Dateien als "geloescht" — das ist Basis-Abstand, KEINE Aenderung der Spur).
- Code-Diff vollstaendig gelesen: enemy_ai_common.c (+Include, s_crow_gflags = g_game.flags[5][0], Weiche,
  Rueckgriff statt -8 HP, ORIENT-Effekt, Se(0) ASCEND/SPIN, Wurfstoss), game_step_common.c (1 Haken),
  scd_room_setup.c (Include + Installer), platform/pc/main.c (2 Bank-Weichen), neue heliport1171_r38.c/.h, Tests.

## 1. Bau

`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
to do.` / `=== LOCAL-BUILD-OK (build)` (EXIT=0). Die exe (21:53) ist aktuell: die zwei Commits danach (bd060fdd,
171daec2) aendern nur das Dossier; Arbeitsbaum sauber (kein MERGE_HEAD), `heliport1171_r38.c.obj` gebaut, Symbol
`re15_r38i_rueckgriff` in der exe. Zusatz: die ninja-Kommandozeile von heliport1171_r38.c mit `-Wall -Wextra
-fsyntax-only` -> rc 0, einzige Warnung vorbestehend in include/re15_emd.h:37 (`/*` im Kommentar).

## 2. RE-Gate — selbst disassembliert (re15_disasm.py, STAGE1.BIN roh @0x80100000 / PSX.EXE)

| Behauptung | Eigene Messung | Urteil |
|---|---|---|
| HOVER-Kontakt setzt (5,0x1C) | `801152c0 addiu a0,s0,17824` / `801152c4 jal 0x8004ef90` / `801152c8 ori a1,zero,0x1c`; s0 = `lui s0,0x800b; addiu s0,s0,-13688` @0x80115214-18 = 0x800aca88 -> a0 = 0x800b1028 | bestaetigt |
| Flag-Tabelle [5] = 0x800b1028 | `table 0x80074664`: [5] @0x80074678 -> 0x800b1028; SET 0x8004ef90 = `srl v1,a1,5; sll 2; addu a0; andi a1,0x1f; lui v0,0x8000; srlv; or; sw` (MSB-first -> Bit 0x1C = Maske 0x8) | bestaetigt |
| Rueckgriff aca58=5 / aca59=1, kein HP-Write | `801152a0 sb v0(=5),aca58`, `801152ac sb v0(=1),aca59`, Gate `80115268 lbu aca58 / 80115270 beq v1,v0(=5)`, `801152b8 jal 0x80115d74(2)`; im ganzen Kontaktblock 0x80115248-c8 kein Schreiben auf HP | bestaetigt |
| Wurzel Tod/Wurf | `80112034 s0=0x800b1028`, TEST 0x1f @0x80112048, Zustand 4 + `andi 0x40` -> `80112088 sb 3,+4`, CLR @0x80112098; TEST 0x1d @0x801120a8, aca58==5 @0x801120c4, `sltiu aca5a,3` @0x801120d8 -> `801120e8 sb 3,aca5a` | bestaetigt |
| Rueck-FSM | `table 0x8012123c` = {0x801159bc, **0x80115b80**, 0x80115d6c}; `table 0x80100304` = {bb8, c0c, c74, cb4, cdc}; P0: acc0c=1 @bc4, `sb 7,acae3` (+0x8f) @bd0, `sb zero,acae8` (Clip 0) @bec, Bild 0 @bf4, aca5a++ @c00, `jal 0x80045024` (a0=0x03020001) @c04, faellt in P1; P1 Umlauf Se @c3c, Clip 1 @c54; P2 Se @ca4; P3 Clip 2 @cc4, faellt in P4; P4 Umlauf aca58=4 @d04, aca59=6 @d10, `sh zero,aca5a` @d18 | bestaetigt |
| Se_on 0x03020001 = Raum-SE 2 | 0x80045024: `srl v1,a0,24` @028 (=3), `srl v0,a0,16; andi 0xff` @078-07c (=2); `table 0x80010e70`[3] = 0x800450f8: `sltiu s4,0x19`, EDT = [0x800ac778]+0x14 @104-110 (dieselbe EDT wie 0x800453d0) | bestaetigt |
| ORIENT-Effekt | `80115310 jal 0x80115d94(8)`, `80115318 ori a0,zero,0x1000`, a3 = 0x8012110c, `lw a2,392` / `lh a1,106`, `80115334 jal 0x80019700` mit `addiu a2,a2,408`; Umlauf `8011536c jal 0x8001f314` -> `80115394 jal 0x80019700` | bestaetigt |
| Kraehen-Se(0) ASCEND/SPIN | `80115004 jal 0x800453d0` / `80115008 addu a0,zero,zero`; `801153f8 jal 0x800453d0` / `801153fc addu a0,zero,zero` | bestaetigt |
| (eigene Zusatzpruefung) Se(0) ohne +12 | 0x800453d0 addiert 12, wenn Entity+0 & 0x2000 (`8004540c andi 0x2000`, `80045418 addiu s5,a0,12`). Das Bit setzt em_set (0x800420a0, s2 = Satz+2) nur bei Satz[5] != 0 (`8004229c lbu v0,3(s2)` -> `800422ac ori 0x2001`). ROOM1171 @0x016DC `44 01 21 40 04 00 00 ff`, @0x016F0 `44 02 21 40 04 00 00 ff` -> Satz[5] = 0 -> die Kraehen spielen wirklich SE 0 (John @0x016C8 `44 00 49 00 04 01` hat 0x2000, irrelevant) | bestaetigt |
| Elza-Archiv | 0x80022300: `lw v1,aca3c` / `80022334 bltz v1,0x800223ac` -> `800223ac ori v0,zero,0x27` (Leon-Zweig 0x26) | bestaetigt |
| (eigene Zusatzpruefung) "Hook A [0x21]" | Verteiler 0x8011597c: `lbu v0,aca59` / `sll 2` / `addiu at,at,4668` (=0x8012123c) / `jalr`; registriert vom STAGE1-Registrar `8011ea20 addiu v0,v0,22908` (=0x8011597c) -> `8011ea28 sw v0,-14372(at)` = 0x800ac7dc = Griff-Treiber-Platz [0x21] (Basis 0x800ac758 + 0x21*4, vgl. 0x800ac80c = [0x2d]) | bestaetigt |
| SCD-Bytes | ROOM1171 @0x013F4 `0f 04 08 00 21 05 1c 00` (While Ck(5,0x1C)==0), @0x01404 `22 05 1d 01`, @0x016C4 `21 03 c2 01`, Kraehen @0x016DC/@0x016F0 `44 0x 21 40`; weiter @0x01398 `22 03 c2 00`, @0x013A0 `22 04 e9 01`, @0x013A8 `22 02 07 01`, @0x013AC `22 01 1b 01`, @0x013B2 `22 05 1e 01`, @0x01464/@0x01498 `22 05 1f 01`, @0x0158E `47 01 01 00` (Aot_on 1) (xxd) | bestaetigt |

Suche im Diff nach "deferred/tunable/interim/for now/faithful/plausibel/TODO": keine Treffer im Code-Diff. Jede neue
Konstante im Header traegt @0x... (Typ/beh @0x016DC, Bank @0x80074664[5], Bits @0x801152c4/@0x801120a8, SE-Satz
@0x80045078-7c, Blend @0x80115bd0, Clips @0x80115bec/c54/cc4, Phase @0x801120e8, Skala @0x80115318, Teil 2 = 408 =
2*172+64 @0x80115338); die Clip-Laengen 14/31/30 sind Datei-Werte (CDEMD1 EM021 dir[5]) und werden vom Riegel `banken`
aus beiden Archiven nachgelesen (eigener Lauf: CDEMD0 14/36/20, CDEMD1 14/31/30). Die PORT-WAHL (Ereignis-Kraehe in jedem
KI-Modus auf RE1.5-Hirn + RE1.5-Bank) ist als solche gekennzeichnet und begruendet (RE2-Kraehenmodul ruft keinen
Flag-Helfer; Weiche greift nur fuer Typ 0x21 mit beh & 0x40 = nur ROOM1171 sub07).

Fix erklaert den Befund: ja. Der Haenger ist die Warteschleife @0x013F4 auf (5,0x1C); einziger Original-Setzer ist
der HOVER-Kontakt @0x801152c4 auf Wort 0x800b1028 = Bank 5. Im Port war das Wort privat (Skript sah es nie) und im
Standard-KI-Modus flogen die Ereignis-Kraehen im RE2-Hirn (kommen nie an). Beides ist im Diff behoben.

## 3. Riegel der Spur (eigener Lauf)

`ctest --test-dir re15_port/build -R "r38_heliport1171" --timeout 900 --output-on-failure` ->
`100% tests passed, 0 tests failed out of 10` (Total 391 s; weg_re2 127 s, weg_re15 170 s, laden 93 s; unter Last
paralleler Spuren). Unit-Teile einzeln gelesen: szene_re2 und szene_re15 identisch (Griff Bild 95, Warten endet 96,
Clip 0 = 14 Bilder, Clip 1 ab 109, Wurf 156 = Griff+1+60, Modus 6 186 = Wurf+30, HP min 100, Kraehen tot 363/410,
Leichen 398/446, msg 0/1/3, Tuer Slot 1 -> ROOM10B1 in Bild 1077, (4,0xE9)=1, (3,0xC2)=0); leon1170: 7 Kraehen, keine
Ereignis-Kraehe, Bank 5 Bits 0x1C..0x1F = 0.

## 4. Nutzer-Punkt 1 — "Elza stuck in ihrer Cutscene im ROOM 1170." (Elza = ROOM1171)

### 4.1 Eigene Messung: ECHTER Weg komplett gelaufen (kein RE15_FIRE_AOT, kein Sprung)

Die Spur kuerzt im Riegel die Treppen + das Tor mit RE15_FIRE_AOT ab. Ich bin den ganzen Weg mit der Aktionstaste und
Laufen gegangen (eigene Messkopie `re15_pc_abn0i_<tag>.exe` neben der Baum-exe, danach geloescht; Arbeitsverzeichnis im
Scratchpad; Karte `probe_r36_elza_karte re15_card.mcr 1131 2700 15300 1024 3:194` = Elza in ROOM1131 vor der Flurtuer,
(3,0xC2)=1 wie nach ROOM1141 sub02 @0x00C20). Eingabe bildgenau ueber `RE15_PAD_AT` (Generator gen_pad.py):
`30:A` (Flurtuer) | ROOM1171 unten: `L32 U60 R32 U21` -> `A` (Z-Treppe 0->2, Slot 6) | `U21 R32 U28` -> `A` (X-Treppe
2->4, Slot 8) | `U119 L32 U23` -> `A` am Tor (Slot 5). Kalibriert per state.log (L/R = -/+96 Gier je Bild, Gehen 75/Bild).
Umgebung: `RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_EVT_TRACE=1 RE15_MSG_LOG=1
RE15_STATE_LOG=state.log RE15_EXIT_AT=330#10b1`, Standard-KI (RE2), mit Ton + `RE15_SE_DEBUG=1`, `RE15_WINDOW_SCALE=1
RE15_FRAMEDUMP=780-1800/15:fd_` (KEIN RE15_SOFTWARE_RENDER).

Lauf `echtA` (Stand 171daec2, exit 0, 91 s) — debug.log:
```
57:[aot] DOOR FIRE slot=3 rect=(2700,14650,hw=1000,hh=500) target_cut=8 spawn=(-21225,0,-18870)
65:[room] PC loaded room1171.rdt (161300 bytes)
133:[aot] DOOR FIRE slot=5 rect=(-11065,-27850,hw=875,hh=600) target_cut=0 spawn=(2300,-7200,14365)
144:[evt] F706 room=1171 Evt_exec sub=2 cond=0xff -> slot 10 (aktive Event-Slots 1) flag(3,0x80)=0
149:[enemy] EM21 loaded: 13 meshes, 13 bones, 14 clips -> slot 12          (RE1.5-Bank, kein "RE2 EM021")
255:[msg] room=1171 id=0 / 300:[msg] room=1171 id=1
327:[evt] F1558 room=1171 Evt_exec sub=3 ... / 329: sub=4
364:[msg] room=1171 id=3
375:[aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(16490,-9000,-8110)
376:[room] PC loaded room10b1.rdt
397:[scd F15] letterbox closed -> gameplay (player_mode=0)
442:[flow] EXIT_AT: Bild 330 in Raum 10b1 erreicht -> exit
```
state.log (Auszug): Treppe 1 `F270 pad=8000 ... mo=221` (z -19969 -> -24553, cam 8->9->10), Treppe 2 ab F441
(x -24531 -> -20003), Tor `F706` -> `PL(2300,14365)`, Plc-Gang nach (1164,7435) F750; Griff ab F806 (Auszug `F810 ...
mo=0 ... fx=1 ... gr=1`, dann mo=1, fx steigt 1/2/3 = ORIENT-Effekte), Wurf-Clip ab F867 (`F870 mo=2 gr=1`), Modus 6
ab F897 (`F900 mo=1 gr=0`); HP durchgehend 100; beide
Ereignis-Kraehen `st=7` (Leiche) ab ~F1100; `msg ... id=1` F1400, `id=3` F1780. ROOM10B1: F15 pm=0, danach dreht und
geht Elza mit der wiederholten Eingabe (F100-131 rot 544 -> -2528, F137-196 (16490,-8110) -> (14877,-11110), F239-259
-> (15927,-12265)) = steuerbar.
Ton (debug.log, Reihenfolge nach dem Szenenstart): zwei `se=0` (ASCEND beider Kraehen), drei `se=2` (Griff / Clip-0-Ende /
Halte-Umlauf), `se=0`-Paare (SPIN) zwischen Johns Schuessen `se=10`; kein `VERWORFEN`.
Bilder (Kontaktboegen `wd_echtA/sheetA1.png`, `sheetA2.png` im Scratchpad, 320x240): F810-F870 Kraehe sitzt Elza im
Nacken, F885 Elza nach vorn gekruemmt (Wurf), F900/F915 Elza steht (Modus 6), F1020 John schiesst, F1140/F1260 zwei
Kraehenleichen auf dem Helipad, F1200 "John: Elza, hurry up!", F1260-F1320 Elza laeuft zu John, F1440 "John: We need to
get this thing off of here! Help me out!", F1560/F1620 beide am Container, F1740 "John: Alright, let's get down from
here!". Kein Haenger, kein Schwarzbild, keine verschwundene Figur.

### 4.2 Derselbe echte Weg in allen drei KI-Modi, ueber die Spielerwahl, und VORHER mit v0.8.25

Auswertung je Lauf mit `auswert.py` (state.log, Segmente je Raum; Griff = erstes gr=1, Wurf = gr=1 && mo=2, Modus 6 =
danach gr=0 && mo=1):

| Lauf | exe / Modus | Ergebnis |
|---|---|---|
| echtA | 171daec2, Standard (RE2) | Szene F706, Griff F806, Wurf F867 (= Griff+61), Modus 6 F897 (= Wurf+30), HP min 100, Kraehen 4 -> 3 -> 7, msg 0/1/3, Slot 1 -> ROOM10B1, dort 82 Lagen (geht) |
| echtB | 171daec2, `RE15_AI_FLAVOR=re15` | identisch (F806/F867/F897, HP 100, msg 0/1/3, 10B1, 82 Lagen) |
| echtC | 171daec2, `RE15_AI_FLAVOR=mixed` | identisch |
| pselB | 171daec2, NEUES SPIEL ueber die Spielerwahl (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`), Elza-Intro 1241 -> 1031 gespielt, dann `RE15_DEBUG_JUMP=1131@gp` + `RE15_PLAYER_POS=2700,15300,1024,0` + `RE15_SET_FLAG=3:194`, danach derselbe Fussweg | identisch: `F706 Evt_exec sub=2`, `EM21 loaded` (RE1.5), Griff F806, Wurf F867, Modus 6 F897, HP 100, msg 0/1/3, `DOOR FIRE slot=1` -> ROOM10B1, `letterbox closed -> gameplay`, 82 Lagen in 10B1 |
| vorher0825 | `release/pkg-win/re15_port_v0.8.25/re15_pc.exe` (Release 2077bb8e), Standard | **Haenger reproduziert**: `F706 Evt_exec sub=2`, `RE2 EM021 loaded ... Hybrid EM21`, danach bis `EXIT_AT: Bild 2200` keine msg, keine Tuer; beide Ereignis-Kraehen `st=1 g=40` (RE2-Hirn) mit unveraendertem Abstand d=17132 bei (-9970,-5680); Elza pm=2 steht bei (1164,7435); Framedumps F900/1200/1500/1800/2100 bildgleich (Elza allein auf dem Helipad) |

Damit ist der Befund des Nutzers am Auslieferungsstand reproduziert und am Spurstand auf dem echten Weg (Treppen und Tor
per Aktionstaste, kein RE15_FIRE_AOT) in allen KI-Modi behoben.

### 4.3 Weitere Zugaenge zu ROOM1171 (Tuer-Zensus, selbst aus den RDT-Bytes)

Door_aot_set mit Ziel Raum 0x17: ROOM1131 Slot 3 @0x008EE (-> Cut 8, unten), ROOM1141 Slot 1 @0x00AB2 (-> Cut 9, unten),
ROOM1171 Slot 5 @0x012F0 (Tor, Selbst-Tuer -> Cut 0, Helipad), ROOM1171 Slot 0 @0x011DC (Helipad -> Hof, Cut 11),
ROOM10B1 Slot 0 @0x016D2 (-> Cut 3, Helipad). Die Szene startet nur ueber den Helipad-Eintritt; unten (Cut 8) laufen
keine Kraehen (state.log echtA: bis F705 keine t=21-Zeile), d.h. es gibt keinen Raumabschnitt, in dem vor dem Ereignis
die RE2-Kraehenbank fuer Typ 0x21 geladen waere (pc_enemy_load_ex prueft nur `re15_enemy_find(type)`, nicht die
Herkunft — ein Mischfall normale + Ereignis-Kraehen in EINER Raumsitzung kommt im Skript nicht vor: sub07 spawnt entweder
den Ereigniszweig ODER die 7 normalen Kraehen). ROOM10B1 ist vor dem Ereignis nicht erreichbar (seine zweite Tuer Slot 1
@0x016F2 fuehrt nach ROOM1261, das hinter 10B1 liegt).

### 4.4 Nach der Szene: zurueck von ROOM10B1 auf das Helipad (Nebenwirkung der Bank-Weiche)

Lauf `wiederD` (171daec2, Standard-KI): Lade-Weg `1171 2300 14365 1024 3:194` (Szene sofort), in ROOM10B1 ab F1100
`R16 U10 A` -> `[aot] DOOR FIRE slot=0 rect=(14335,-6840,hw=1665,hh=1700) target_cut=3 spawn=(4014,-7200,-7436)` ->
`PC loaded room1171.rdt` -> `[enemy] RE2 EM021 loaded ... Hybrid EM21` (die normalen Kraehen bekommen wieder die
RE2-Bank — keine Herkunfts-Uebertragung aus dem Ereignis), state.log F1150: sieben Kraehen `t=21 st=1` mit g=90/10/00
(keine g=40, kein zweites Ereignis), Elza pm=0, HP 100, geht. Bild `wd_wiederD/wieder_f1150.png`: Helipad mit Elza und
Kraehen am Boden, kein Fehlbild. Szene im selben Lauf: Griff F95, Wurf F156, Modus 6 F186, ROOM10B1 bei F1077 (= die
Bildzahlen der Unit-Riegel).

## 5. Gates

| Gate | Ergebnis | Beleg |
|---|---|---|
| Bau ueber local_build.sh | haelt | Abschnitt 1 |
| RE-Gate: Konstanten mit @0x / Byte-Offset | haelt | Header + Haken; 10 Stellen selbst disassembliert (Abschnitt 2), dazu `8003ed74 sw zero,4136(at)` = 0x800b1028 (Raum-Init loescht Bank 5) und `80031518 sw zero,-13736(at)` = aca58 (Raumlader loescht das Kommandowort) |
| Guess-Tells im Diff | haelt | keine "deferred/tunable/interim/for now/faithful/plausibel/TODO" im Code-Diff |
| Fix erklaert den Befund | haelt | Abschnitt 2 + Gegenprobe v0.8.25 (Abschnitt 4.2) |
| PORT-WAHL gekennzeichnet | haelt | Header-Kopf "PORT-WAHL (belegte Form)", Dossier Abschnitt 3 |
| Riegel je Punkt, messend | haelt | 7 Unit + 3 exe-Riegel, 10/10 gruen; Unit misst Bildzahlen gegen die Adressen, exe-Riegel prueft msg-Folge/Tuer/HP/Griff-Wurf-Modus-6/pm=0 |
| Vertrag: Bank-9-Bits / msg-Ids / Slots | haelt | keine Bank-9-Bits (Bit 69 nicht gebraucht, k_port[] unberuehrt), keine neuen Nachrichten (1171 40-44 frei), keine neuen Gegenstaende |
| Vertrag: Pfade | haelt | `git diff master...HEAD --name-only`: nur analysis/befunde_runde38/I_*, engine/src (3 Haken-Dateien + neue heliport1171_r38.c), include (neuer Header), platform/pc/main.c, tests/unit/probes/r38_heliport1171.cmake (eigene Sonden-Datei), tests/unit/test_r38_heliport1171.c, tests/integration/test_r38_heliport1171.cmake. KEIN release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists |
| Vertrag: gemeinsame Dateien nur kleine Haken | haelt (mit Hinweis H1) | game_step_common.c 1 Haken (2 Z.), scd_room_setup.c Include + Installer am Blockende, main.c Include + 2 einzeilige Weichen-Zusaetze, enemy_ai_common.c 9 Einzeiler-Haken (+7 berichtigte Kommentare), jeder mit "Runde 38 Spur I"; die Logik liegt in heliport1171_r38.c |
| Nachbarn (eigener Lauf) | haelt | Abschnitt 6 |

## 6. Nachbarn / Nebenwirkungen (eigener Lauf)

- Unit/Sonden: `ctest -R "^(unit|probe)_.*(crow|kraehe|fenster|1170|10c0|victim|grab|elzaems|10d0|1090_push|plc_back|
  r37_roy|1120|tuer1120|10b1|szenenleon|1090_fire|1090_flame|1090_flinch|1090_room|room1090|feuerloescher)|..."
  -E r38_heliport1171` -> `100% tests passed, 0 tests failed out of 71`; dazu `-R "^unit_crow_ai|^unit_fx_emitter_ai|
  ^unit_spider_ai|^unit_adult_spider_ai|^unit_crow_|^unit_re2_crow"` -> `100% ... out of 10`. Erweitert gegenueber der
  Spur um ROOM1090 (Feuer-Emitter 0x26 schreibt seit dem Umbau Bits (variant+29) in die ECHTE Bank 5 @0x80116ac8 statt
  ins private Wort; das ROOM1090-Skript liest (5,0x1D..0x1F) nicht — Zensus der Spur, unit_1090_* / probe_1090_* gruen),
  Feuerloescher r37, Tuer 1120, Szenen-Leon (Plc_dest-Ankunftsbits in Bank 5).
- Bank 5 Wort 0 als gemeinsames Wort: im Original DASSELBE Wort (Tabelle @0x80074664[5]); die Kraehen-Bits 0x1C..0x1F
  liest nur der FLIGHT-2-Zweig (nur grid&0x40 = nur ROOM1171-Ereignis). Leon ROOM1170 unberuehrt (unit leon1170).
- exe-Nachbarn: siehe 6.1.

### 6.1 exe-Nachbarn

`ctest -R "^integration_(r35_fenster|r36_elza_fenster|r37_elzaems_elza1181griff|r37_elzaems_elza10c1kraehe|
r37_elzaems_leon10c0kraehe|r37_roy_leon1170|r37_elzaems_elza1141|r37_feuerloescher)$" --timeout 900` ->
`100% tests passed, 0 tests failed out of 8` (r35_fenster 32 s, r36_elza_fenster 74 s, elza1181griff 43 s,
elza10c1kraehe 56 s, leon10c0kraehe 73 s, elza1141 32 s, r37_feuerloescher 689 s (15 Laeufe inkl. ROOM1090-Feuer),
roy_leon1170 107 s; unter Last paralleler Spuren, kein Nachfahren noetig). Gegenueber der Spur zusaetzlich: elza1141
(Uebergang zum Ereignis-Flag (3,0xC2)) und r37_feuerloescher (ROOM1090, Feuer-Emitter schreiben jetzt in die echte Bank 5).

## 7. Integration (Hinweise an den Orchestrator, selbst per `git merge-tree --write-tree` geprueft, Stand 22:45)

| Zweig | Ergebnis |
|---|---|
| r38/szenenschutz (B) | Textkonflikt NUR Include-Zeile enemy_ai_common.c (beide behalten). Fachlich: Bs Kopf nimmt Skript-Zustaende (+0x4 != 1) und Kraehen mit grid&0x40 aus ("AUSNAHME Teil der Szene"); der Griff laeuft in FLIGHT-2 (+0x4 = 4) -> vertraeglich |
| r38/spiderman (T) | Textkonflikte NUR Include-Zeilen enemy_ai_common.c und main.c (beide behalten) — im Dossier der Spur noch nicht genannt |
| r38/dining1141 (F) | Textkonflikte NUR Include-Zeile + Installer-Zeile scd_room_setup.c (beide behalten) |
| r38/integration (6d2e7ce8) | Textkonflikt NUR Include-Zeile main.c gegen Spur E (`re15_elzaintro_r38.h`); die "konfliktfreie" Probe-Merge-Aussage im Dossier (21:40) ist durch den gewachsenen Integrationszweig ueberholt — trivial |
| r38/spinnen (R), r38/werkstatt11b0 (C) | konfliktfrei |

Die Kuerzung des exe-Riegels (Tor per RE15_FIRE_AOT) ist auch byte-seitig gedeckt: Treppen 1171 @0x01310/@0x01324/
@0x01338/@0x0134C und Tor @0x012F0 sind zu 1170 @0x0137A/@0x0138E/@0x013A2/@0x013B6/@0x0135A bytegleich bis auf das
Slot-Byte (xxd selbst verglichen) — und der Fussweg ist in 4.1/4.2 gemessen.

## 8. Maengel und Hinweise

Maengel (blockierend): **keine**.

Hinweise (nicht blockierend, nachpruefbar):
- H1 (Vertrag 1.4): enemy_ai_common.c ist im Vertrag fuer B und T vorgesehen. Spur I setzt dort 9 Einzeiler-Haken
  (+7 berichtigte Kommentare; `git diff master...HEAD --stat` 32 Zeilen). Noetig, weil das Kraehenhirn (privates Wort,
  RE2-Weiche, erfundene -8 HP) dort liegt; jede Stelle traegt "Runde 38 Spur I" (die ORIENT-Eintrittszeile nur ueber die
  Folgezeile), die Logik liegt in heliport1171_r38.c, `git merge-tree` gegen B und T kollidiert nur an der Include-Zeile.
- H2 (Riegel): `integration_r38_heliport1171_weg_*` feuert das Tor per RE15_FIRE_AOT. Der echte Fussweg funktioniert
  (4.1/4.2). Optional fuer spaeter: den RE15_PAD_AT-Fussweg aus 4.1 (deterministisch, ~700 Bilder ab ROOM1131) als
  vierten exe-Riegel uebernehmen.
- H3 (Integration): die Konflikte mit T (enemy_ai_common.c + main.c Include) und mit E ueber r38/integration (main.c
  Include) fehlen im Dossier; alle trivial (beide Zeilen behalten).
- H4 (offen, vom Bau-Agenten selbst benannt, ausserhalb des Nutzer-Punkts): 0x800acc0c := 1 @0x80115bc4 ohne Port-Modell;
  Se_on 0x03020001 positional (@0x80115c04) im Port ohne Positionsmischung; Modus 6 ein Bild frueher (gemeinsame Maschine
  re15_player_event_reach_*); im P0-/P3-Bild kein f314-Vorschub (Port-Takt wie re15_player_victim_tick; im Original faellt
  P0 in P1 mit `jal 0x8001f314` @0x80115c20 im selben Bild) — je hoechstens ein Bild Versatz, kein Einfluss auf den Ablauf.
- H5 (fuer den Nutzer, aus dem Dossier uebernommen und die Dateien geprueft): synchro/STAGE1/room1171/ hat main00/01/02.wav;
  die Szene zeigt msg 0, 1, 3 (Message_on `2b 00` @0x014BA, `2b 01` @0x0151E, `2b 03` @0x01586); fuer msg 3 fehlt
  main03.wav.

## 9. Urteil

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "Elza stuck in ihrer Cutscene im ROOM 1170." | **erfuellt** | Haenger mit v0.8.25 auf dem echten Weg reproduziert (Kraehen im RE2-Hirn, keine msg, keine Tuer bis F2200); am Spurstand derselbe echte Weg (Flurtuer, zwei Treppen, Tor per Aktionstaste) in RE2/RE15/MIXED und ueber die Spielerwahl: Griff F806, Wurf F867, Modus 6 F897, HP 100, msg 0/1/3, ROOM10B1, Elza danach steuerbar; Bilder ohne Fehlbild; Mechanismus selbst disassembliert |

Gates: alle halten (Abschnitt 5; exe-Nachbarn 8/8 in 6.1). **bestanden.**

Messkopien `re15_pc_abn0i_*.exe` nach jedem Lauf geloescht (eine haengende Kopie `re15_pc_abn0i_pselA.exe` — Titel ohne
RE15_TITLE_CONFIRM_MS — gezielt per Pfad beendet, keine fremde exe beruehrt). Kein Code geaendert; committet wird nur
dieser Bericht.
