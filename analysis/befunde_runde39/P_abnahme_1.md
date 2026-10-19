# Runde 39 Spur P "johnruf1041" - Abnahme 1 (unabhaengig)

Baum `.claude/worktrees/r39_johnruf1041`, Zweig `r39/johnruf1041`, HEAD 90e50a2a (Nachbesserung 1), Merge-Basis zu master
a0d7d749 (master steht inzwischen auf 017f5094 mit Spur Q - nicht Gegenstand). Eigener Diff der Nachbesserung =
`git diff 57e5f920..HEAD` (36 Dateien, +1857/-21; Code: tuersperre_r39.{h,c}, gen/tuersperre_r39.inc, Haken johnruf_r39.c +3,
scd_vm.c +4). Massstab: AUFTRAG.md Nachtrag 10 (Punkte 1/2, Abnahme 0) + Nachtrag 12 (Spur-P-Anteil) + Nachtrag 13 (Form des
Tuersatzes), VERTRAG.md. Zeit: 2026-10-19 ab ~10:30. Messordner (nicht committet): Scratchpad `abn1/` dieser Sitzung.

(Bericht wird fortlaufend geschrieben.)

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, "ninja: no work to do".
  exe `re15_port/build/platform/pc/re15_pc.exe` 08:58; der einzige Code-Commit der Nachbesserung 23cb48d9 (08:59:19) wurde
  nach diesem Bau committet, Ninja findet nichts zu tun -> die exe enthaelt den Stand (11 `tuersperre`-Zeichenketten in der exe,
  u. a. "ROOM%04X: Kanaldeckel Slot %u -> Text msg %u (immer zu)"). `git status` sauber (nur dieser Bericht).

## 1. Tests der Spur (gezielt, keine volle Suite)
- `ctest --test-dir re15_port/build -R "r39_johnruf1041" --timeout 1800 --output-on-failure -j 3`
  -> "100% tests passed, 0 tests failed out of 22" (13 Unit inkl. tuertexte/tuer1041/tuer1051/tuer10b1/tuerszene, 9 Integration:
  a 88,23 s, b 298,83 s, tuer_t1041 41,98 s, t1041o 32,19 s, t1051 28,56 s, t1051o 37,73 s, t10b1v 38,68 s, t10b1s 72,03 s,
  t10b1n 35,29 s). Laeufe unter Last (parallel lief eine fremde Abnahme `re15_pc_qa2_n15_tuer_b`).
- Lauf a (meiner, `re15_port/build/r39_johnruf1041_wd/lauf_a_3f5176a2/debug.log`): `F512 Karte zu ... Set(9,97)=1` ->
  `[tuersperre] F512 ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41` -> `F512 Panik-Musikfenster auf` ->
  `entry=FF01 -> MAIN01`; `F969 ... msg 41 "I have to rescue John! He is in danger!"`, `[msgtext] room=1041 id=41 "I have to
  rescue John! He is in danger!"`, KEIN `loaded room1031.rdt`; danach Treppe `DOOR FIRE slot=2` -> 1061 (MAIN01 laeuft durch).

## 2. RE-Gate (Stichproben SELBST disassembliert / Bytes gelesen)
Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (info/Re1.5/PSX.EXE), `xxd` auf
`re15_port/shared_assets/PSX/STAGE1/*.RDT` und `info/re2leon/PL0/RDT/ROOM21A0.RDT`, `tools/scd_dump_room.py`.
| Zitat im Diff | selbst gemessen | Ergebnis |
|---|---|---|
| Aot_reset LAB_80040738: rec[0]=pc[2] @0x80040764, rec[1]=(rec[1]&0x80)+pc[3] @0x8004076c-78, Nutzlast @0x80040788-a8 | `lb v0,1(a2)` Slot -> Tabelle 0x800ac9b0; `lbu v0,2(a2)`/`sb v0,0(a1)` @0x80040764; `lbu v0,3(a2)`, `andi v1,v1,0x80` @0x8004076c, `addu`, `sb v0,1(a1)` @0x80040778; drei `lhu`/`sh` @0x80040788..@0x800407a8 nach rec+12/+20; `addiu v0,a2,10` @0x800407ac (10 B) | stimmt |
| Aot_on LAB_800407bc: `lbu v0,0(v1)` @0x8004080c, `jalr v0` @0x8004082c ueber PTR_8007469c; [1] = 0x80043084 | `lbu v0,0(v1)` @0x8004080c, `addiu at,at,18076` = 0x8007469c @0x8004081c, `jalr v0` @0x8004082c; Tabelle: [1] 0x800746a0 -> 0x80043084, [2] 0x800746a4 -> 0x800430bc | stimmt (Aot_on auf einen MESSAGE-Satz ruft den Text-Handler) |
| MESSAGE-Handler 0x80043084: `jal 0x80027e68` @0x800430a0, Maske @0x800430a4 | `lhu a2,0(v0)` (Text-Id), `lhu a3,2(v0)`, `jal 0x80027e68` @0x800430a0, Verzoegerung `sll a3,a3,16` @0x800430a4 (Maske 0xffff -> 0xffff0000) | stimmt |
| FUN_8003f038 @0x8003f040-4c (VM parkt unter der Pausenmaske) | `lw v0,g_pauseflags` @0x8003f040, `lui v1,0x200`, `and`, `bne v0,zero,0x8003f090` @0x8003f04c | stimmt |
| ROOM11A1 main00 @0x00F9C / @0x00FBC / msg 1 @0x01430 | `3b 02 02 31 ..` / `46 02 01 31 01 00 ff ff 00 00` / `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00` "It's locked." | stimmt |
| ROOM11B1 sub01 @0x0119C / @0x011C0 | `06 00 4a 00 21 04 f3 00` (Ck(4,0xF3)==0) / `46 01 01 31 0e 00 ff ff 00 00` | stimmt |
| Tuersaetze ROOM1041 @0x010B8, ROOM1051 @0x00B5A, ROOM10B1 @0x016F2 | `3b 01 02 31 .. 00 03 08` (Ziel Raum 0x03 Cut 8), `3b 04 02 31 .. 00 0a 00` (Raum 0x0A Cut 0), `3b 01 02 31 .. 00 26 0f` (Raum 0x26 Cut 15) | stimmt |
| ROOM10B1 sub04 @0x01A8E `47 01` + Evt_end; Szenen-Flags @0x0195A..@0x0196A | `47 01` @0x01A8E, `01 00` @0x01A90; `22 03 82 00`, `22 03 83 01`, `22 03 41 01`, `22 02 07 01`, `22 01 1b 01` | stimmt |
| Szenen-Ende ROOM1051 sub02 @0x00CCC, @0x00D90..@0x00D9F, @0x00DA2 | `02 00` @0x00CCC; `22 02 07 00 22 01 1b 00 3c 01 2e 01 00 00 42 00` @0x00D90; `01 00` @0x00DA2 (dazwischen `08 00` Endif @0x00DA0, in der Huelle zu Recht weggelassen) | stimmt |
| RE2 ROOM21A0 msg 4 @0x02963, Aot_reset @0x01EA4 (msg 1) / Else @0x01EC6 (msg 4) | @0x02963 `fa 02 fc 30 44 41 00 48 45 40 .. 3f 48 4b 4f 41 40 01 fd 00 25 50 3a 4f 00 45 49 4c 4b 4f 4f 45 3e 48 41 00 42 4b 4e 00 49 41 fc 50 4b 00 4b 4c 41 4a 00 45 50 01 fe 00` = " / The lid is firmly closed.<fd>It's impossible for me / to open it."; @0x01E9C `06 00 26 00` Ifel, `21 01 00 00` Ck(1,0,0), @0x01EA4 `46 02 04 31 01 00 00 00 ff ff` (sce 4 = RE2 MESSAGE, msg 1), @0x01EC2 `07 00 10 00` Else, @0x01EC6 `46 02 04 31 04 00 00 00 ff ff` (msg 4), `08 00` Endif | stimmt; Umkodierung im Unit-Riegel tuertexte gegen die RE2-Datei geprueft (fa02->0402, fc->08, fd00->0200, 01->57, fe00->0100) |
Rate-Signale: `git diff 57e5f920..HEAD -- re15_port | grep -inE "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx"`
-> 0 Treffer; kein neues `getenv` im Diff. Konstanten ohne Adresse sind gekennzeichnet: Satz "I have to rescue John! He is in
danger!" = NUTZER-VORGABE (Nachtrag 12), Ids 41/40 = Orchestrator/VERTRAG, 10B1-Id 40 = "erste Port-Id hinter den Original-Ids
0..5" (siehe Vertrag 5.). Alle Sperr-Konstanten (Slot, Ziel, sce, sat, Maske) tragen @0x. **Gate haelt.**

## 3. Messung je Nutzer-Punkt (selbst, an der exe dieses Baums)
Eigenes Laufskript `p_abn1/mess.sh` (Scratchpad dieser Sitzung): Elza-Spielstand per `probe_r36_elza_karte`, CONTINUE,
Messkopie `re15_pc_pabn1_<lauf>.exe` neben der exe (nach dem Lauf geloescht), eigener Laufordner, RE15_*-Umgebung nicht
geerbt, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `RE15_WEGPLAN` (echte Tueren/Aktionstaste), `RE15_MSG_LOG`,
`RE15_EVT_TRACE`, `RE15_STATE_LOG`, `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1`, bei Bedarf `RE15_CLIMB_TRACE`. Laufordner
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/p_abn1/l_<lauf>`.
Basis-Flags (Elza nach Roys Tagebuch, wie die Riegel der Spur) `3:0x5F 4:0xF1 4:0xEC 3:0x78`.

### 3.1 Nachtrag 12a "beim Musikwechsel, wenn Elza John retten soll, soll beim anklciken der Tür von ROOM 1041 zu Room 1031 kommen 'I have to rescue John! He is in danger!'"
- ECHTER Weg ab dem Musikwechsel (mein Lauf a, `build/r39_johnruf1041_wd/lauf_a_3f5176a2/debug.log`): Weg 1021 -> 1041, Johns
  Rufe, Elzas Satz, Karte; `F512 Karte zu ... Set(9,97)=1` -> im SELBEN Bild `[tuersperre] F512 ROOM1041: Panikphase (Bit 97)
  -> Tuer Slot 1 -> Text msg 41` und `Panik-Musikfenster auf` -> `entry=FF01 -> MAIN01`; an der Tuer (Wegplan GEHE (-20800,-3800),
  BLICK 0, AKTION) `F969 ... msg 41`, `[msgtext] room=1041 id=41 "I have to rescue John! He is in danger!"`, kein
  `DOOR FIRE slot=1`, kein `loaded room1031.rdt`; danach Treppe -> 1061.
- ECHTER Tuerweg IN 1041 waehrend der Phase (Lauf `l_d1041`: Spielstand ROOM1061 (26000,25300) + `4:0xF0 9:97 4:5 4:4`,
  Aktionstaste an der Tuer 1061 Slot 2): `[aot] DOOR FIRE slot=2 ... spawn=(-21008,0,-13134)` -> `loaded room1041.rdt` ->
  `[tuersperre] F65 ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41` (Raumaufbau ueber scd_room_reenter, nicht
  nur CONTINUE), AOT-Liste `AOT slot 1 typ 5 ev 41 ... mitte (-19650,-3800)`; Gang -> Tuer, AKTION F410:
  `[msgtext] room=1041 id=41 "I have to rescue John! He is in danger!"`, kein Raumwechsel, zweite AKTION schliesst, Elza frei
  (Wegplan bis Befehl 21). MAIN01 `[unveraendert, laeuft durch]`. Bilder `l_d1041/fd_000450.ppm` / `fd_000520.ppm`,
  Blatt `l_d1041/sheet_text.png`: zweizeiliger Text "I have to rescue John! / He is in danger!" OHNE Sprecherkopf vor der Tuer.
- Ohne Phase: integration_r39_johnruf1041_tuer_t1041o (Bit 97 = 0, (4,0xF0) = 0) -> `DOOR FIRE slot=1` -> ROOM1031 (gruen);
  vor dem Ruf (Lauf a bis F511) ist die Tuer normal (Sperre erst ab F512). Leon ROOM1040: unit tuer1041 "Slot 1 bleibt Tuer".
**Urteil: erfuellt.**

### 3.2 Nachtrag 12b "Auch bei der Tür 1051 zu 10A1."
- ECHTER Tuerweg IN 1051 waehrend der Phase (Lauf `l_d1051`: Spielstand ROOM1001 (21850,-13400) + `4:0xF0 9:97 3:0x79`, Tuer 1001
  Slot 0): `DOOR FIRE slot=0 ... spawn=(14700,0,-13500)` -> `loaded room1051.rdt` -> `[tuersperre] F65 ROOM1051: Panikphase
  (Bit 97) -> Tuer Slot 4 -> Text msg 40`, `AOT slot 4 typ 5 ev 40 ... mitte (17200,-13700)`; GEHE (16450,-13700), BLICK 0,
  AKTION F114: `[msgtext] room=1051 id=40 "I have to rescue John! He is in danger!"`, kein `DOOR FIRE slot=4`, kein
  `loaded room10a1.rdt`; Blatt `l_d1051/sheet_text.png` (Text vor der Notausgang-Tuer, ohne Sprecherkopf).
- integration_r39_johnruf1041_tuer_t1051 (CONTINUE in 1051) gruen; t1051o (ohne Phase) -> ROOM10A1 gruen; Leon ROOM1050 (Ada
  gerettet, Bit 97 = 1) Slot 4 bleibt Tuer (unit tuer1051).
- Hinweis: ROOM1051 hat eine zweite Tuer nach 1031 (main00 @0x00ADA Slot 0 -> Raum 0x03 Cut 2); verlangt war nur 1051 -> 10A1,
  sie bleibt offen.
**Urteil: erfuellt.**

### 3.3 Nachtrag 13 (Form des Tuersatzes) "Achso, nein, das ist ein einfacher text, wie bei Leon auch im Panik Abschnitt." (Orchestrator: "KEIN 'Elza:'-Kopf, keine Sprachdatei")
- Text: `04 02 <Zeile> 08 <Zeile> 01 00` (gen/tuersperre_r39.inc k_r39p_ts_msg_rettung) = Kopf/Ende wie Leons Tuertext
  "I have to get the Chief first..." (engine/src/tuer1060_1040.c k_meldung `0x04, 0x02, ... 0x01, 0x00`), kein Sprecherkopf;
  Bilder 3.1/3.2 zeigen den Text ohne "Elza:". -> Text-Form erfuellt.
- ABER Stimme: `re15_tuersperre_r39_stimme` (engine/src/tuersperre_r39.c:151-163, Haken scd_vm.c re15_scd_show_message
  "1041 msg 41 / 1051 msg 40 -> main41/main40.wav") reiht fuer beide Tuersaetze `SCD_AUDIO_VOICE_ON` ein. Gemessen (l_d1041 /
  l_d1051): `[tuersperre] F410 ROOM1041 Tuer in der Panikphase -> msg 41 ... (Stimme main41.wav)`, `[voice] ROOM1041 msg 41: keine
  Aufnahme (synchro/STAGE1/room1041/main41.wav) - Zeile bleibt stumm`, ebenso `[voice] ROOM1051 msg 40: keine Aufnahme
  (synchro/STAGE1/room1051/main40.wav)`. Leons Tuertexte haben KEINEN Stimmen-Haken (`grep -n "VOICE|voice|stimme"
  engine/src/sperre1050_r37.c` -> 0 Treffer). Die Riegel verlangen die Einreihung sogar: unit tuertexte "Stimme eingereiht nur
  fuer 1041/41 und 1051/40 (main41.wav/main40.wav)", integration t1041/t1051 `pruef_wahr(_stimme "Stimme msg 41 eingereiht
  (Ordnerregel main41.wav)")`. Das Dossier 8.9 "Fuer den Nutzer" bittet den Nutzer um "Sprachdateien fuer diesen Satz ...
  synchro/STAGE1/room1041/main41.wav und synchro/STAGE1/room1051/main40.wav" - genau das hat der Nutzer in Nachtrag 13
  verneint. Nachtrag 13 lag im Baum seit dc4bc297 (08:33:46), VOR dem Code-Commit 23cb48d9 (08:59:19); Dossier und Code
  erwaehnen ihn nicht.
- Heute hoerbar: nichts (keine Datei) - aber Code, Riegel und Nutzer-Hinweis widersprechen dem Wortlaut "einfacher Text wie bei
  Leon"; eine spaeter unter dem genannten Namen abgelegte Datei wuerde gespielt.
**Urteil: teilweise** (Mangel M2).

### 3.4 Nachtrag 12c "in Elzas Szenario die Tür im ROOM 10B1 ausnahmslos IMMER geschlossen ... auch wenn sie nach Sherry sucht"
Welche Tuer (selbst gelesen, `tools/scd_dump_room.py`): ROOM10B1 main00 hat genau zwei Uebergaenge - @0x016D2 Slot 0 -> Raum 0x17
(ROOM1171) Cut 3, `floor=5` (Leiter zum Dach, steht auf der Dachterrasse) und @0x016F2 Slot 1 -> Raum 0x26 (ROOM1261) Cut 15,
`floor=0` (Kanaldeckel in der Gasse); @0x01712 Slot 2 ist sce 4 = FLAG_CHG, keine Tuer. In der Sherry-Szene oeffnet sich nur
Slot 1 (sub04 @0x01A8E `47 01`). Die Lesart der Spur (Tuer = Kanaldeckel) passt zum Wortlaut.

ECHTER Weg vom Dach (Laeufe `l_e10b1d`/`l_e10b1f`/`l_e10b1i`): Spielstand ROOM1171 (2300,14365) + `3:194` (= (3,0xC2), wie
integration_r39_heliport1171 laden), Original-Szene 1171 (`Evt_exec sub=2/3/4`), deren `Aot_on 01` -> `DOOR FIRE slot=1
... spawn=(16490,-9000,-8110)` -> `loaded room10b1.rdt` -> `[tuersperre] F1085 ROOM10B1: Kanaldeckel Slot 1 -> Text msg 40 (immer
zu)`; AOT-Liste: `slot 0 typ 1` (Tuer, Leiter) und `slot 1 typ 5 ev 40` (Text). Wegplan ueber die Dachterrasse (Band 5, y -9000),
an der Kante `BLICK 2048` + AKTION -> Abstieg (`RE15_CLIMB_TRACE`: T1636 sub 9 band 5 -> T1687 y -1800 band 1), zweiter Abstieg
von den Kisten (T1826 -> y 0 band 0), Gasse nach Norden: im Schnitt 7 `[tuersperre] F1280 ROOM10B1 sub01 @0x01824 Evt_exec
sub04 -> Programm SZENE10B1`, msg 0..5 ("Elza: John, wait!" ... "John: ... Follow me.") mit Stimmen, dann
`[msgtext] room=10b1 id=40 " The lid is firmly closed.It's impossible for me to open it."` (state.txt F2008 `msg(a=1 fsm=1
id=40)`, `pf=FFFF0007`), KEIN `loaded room1261.rdt`. Mit `RE15_PRESS=square@2030,2070,2110,2150` (l_e10b1f): Text zu, danach
Wegplan `FLAG (2,7) = 0`, `FLAG (1,27) = 0`, `(3,130) = 0`, `(3,131) = 1`, `(3,65) = 1`, Elza laeuft (F2184 (-20465,1165)).
Blatt `l_e10b1f/sheet_szene.png`: "The lid" -> "The lid is firmly closed." (Seite 1 auf Zeile 2 wegen der leeren Kopfzeile aus
RE2 `fc`) -> "It's impossible ..." -> Balken weg, Elza frei, John steht AUF dem Deckel (Plc_dest sub04 @0x01A84 (-21422,4607)).
Integration t10b1v/t10b1s/t10b1n (Einstieg aus 1261) gruen. Leon: unit tuer10b1 (ROOM10B0 -> ROOM1260), Nachbarn Abschnitt 4.
=> Wortlaut "Tuer immer geschlossen, auch bei der Sherry-Suche" ist umgesetzt.

**NEBENWIRKUNG: Elza sitzt nach der Szene im unteren Teil von 10B1 fest (Softlock).** Der Abstieg von der Dachterrasse ist laut
ausgeliefertem RDT EINSEITIG:
- ROOM10B1 SCA @0x00EE4 (RDT+0x20), 3 Kletterzellen (je Quadranten-Region einmal wiederholt): Zelle 31 @0x01070
  `16 08 aa 10 0e e7 d0 d9 01 01 0e 51` (x -6386..-4316, z -9776..-5510, floor 0x51 -> Band 5, u1 0x0e = "hinab"), Zelle 18
  @0x00FD4 `74 07 48 1b 92 dc 34 d3 01 01 02 10` (Band 1, u1 0x02 "hinab"), Zelle 6 @0x00F44 `f2 19 8a 10 b0 e4 72 d8 01 ff 01 01`
  (Band 0, u1 0x01 "hinauf" = Boden -> Kisten). KEINE Band-1-Zelle mit "hinauf" -> von den Kisten fuehrt kein Weg aufs Dach.
- Regel im Original-EXE selbst disassembliert: Sonde @0x8002d524 `jal 0x8003b7f0` (Zellwort), @0x8002d544 `andi v0,s0,0x1`,
  @0x8002d548 `beq v0,zero,0x8002d634` (kein "hinauf"-Bit -> kein Aufstieg), @0x8002d74c `andi v0,s6,0x2` (hinab);
  Bandfilter @0x8003b8a8-b0 `sll v0,v0,16; sra v0,v0,28; bne a2,v0` (nur Zellen des eigenen Bands).
- GEMESSEN (Lauf `l_e10b1h`, `RE15_CLIMB_TRACE=climb.txt`): Boden -> Kisten geht (T114 sub 9, y 0 -> -1800, band 0 -> 1); auf
  den Kisten (band 1, y -1800) an der Dachwand AKTION mit Blick 0 bei z -8303 / -7563 / -6860 / -8792 sowie Blick 1024 / 3072:
  `sub=0` in jedem Bild, kein Aufstieg. Lauf `l_e10b1g` (zurueck zur Leiter): `KLEMME beim Gehen nach (-2811,-8451)` an der
  Wand x -4252, kein Weg aufs Dach.
- Ausgaenge des unteren Teils: nur Slot 1 (`floor=0`, Kanaldeckel) - jetzt immer Text. Slot 0 (Leiter, `floor=5`) liegt oben.
- Die Szene (und damit (3,0x83)/(3,0x41)) gibt es nur unten: sub01 @0x0181E Cmp(Schnitt == 7); Flag-Zensus aller RDTs
  (Bytes `22 03 83 01` / `22 03 41 01`): Set(3,0x83)=1 nur ROOM10B1 @0x0195E (Elza) und ROOM11B0 @0x01478 (Leon-Raum),
  Set(3,0x41)=1 nur ROOM10B1 @0x01962. Ohne (3,0x83) bleibt die Werkstatttuer 11B1 -> 1261 Text (ROOM11B1 sub00 @0x0112A
  Aot_set Slot 8 MESSAGE msg 12, @0x0113E Aot_reset Slot 5 sce 0); ohne (3,0x41) kein John in 1261/2001 (ROOM2001 sub00
  @0x0179A, Dossier 8.8).
- Folge: Wer der Geschichte folgt (John geht voran, die Szene liegt unten), kommt aus 10B1 nicht mehr heraus; vorher fuehrte
  der Deckel am Szenenende nach 1261. Die Aussage im Dossier 8.8/8.9 "der Weg in die Kanaele ... geht nicht mehr ueber 10B1,
  sondern nur noch ueber den Keller (Garage 11B1 oder 11D1 -> 1261)" stimmt nicht: aus dem unteren Teil von 10B1 erreicht
  Elza den Keller nicht (die Spur hatte den Umweg ausdruecklich NICHT gemessen).
**Urteil: teilweise** - der Deckel ist in jedem Zustand zu (Wortlaut erfuellt), aber Elzas Szenario ist danach nicht mehr
spielbar und dem Nutzer wird ein Weg genannt, den es nicht gibt (Mangel M1, Entscheidung beim Nutzer/Orchestrator noetig).
