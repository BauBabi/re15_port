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
- Zweiter Weg in dieselbe Falle (Lauf `l_e1261h`): Spielstand ROOM1261 mit den Flags der Sherry-Suche `3:0x83 3:0x41 3:0x46`,
  Steg hinauf (mo 230), Leiter ROOM1261 main00 @0x0116A Slot 2 (ohne Bedingung) per Aktionstaste: `DOOR FIRE slot=2 ...
  spawn=(-21159,0,3289)` -> `loaded room10b1.rdt` -> `[tuersperre] F232 ... Kanaldeckel ... (immer zu)`, am Deckel der Text
  (Blatt `l_e1261h/sheet_deckel.png`). Elza steht damit in der Gasse (Band 0) - zurueck nach unten zu, nach oben (Band 5)
  kein Weg (s. o.).
- Folge: Wer der Geschichte folgt (John geht voran, die Szene liegt unten), kommt aus 10B1 nicht mehr heraus; vorher fuehrte
  der Deckel am Szenenende nach 1261. Die Aussage im Dossier 8.8/8.9 "der Weg in die Kanaele ... geht nicht mehr ueber 10B1,
  sondern nur noch ueber den Keller (Garage 11B1 oder 11D1 -> 1261)" stimmt nicht: aus dem unteren Teil von 10B1 erreicht
  Elza den Keller nicht (die Spur hatte den Umweg ausdruecklich NICHT gemessen).
**Urteil: teilweise** - der Deckel ist in jedem Zustand zu (Wortlaut erfuellt), aber Elzas Szenario ist danach nicht mehr
spielbar und dem Nutzer wird ein Weg genannt, den es nicht gibt (Mangel M1, Entscheidung beim Nutzer/Orchestrator noetig).

### 3.5 Nachtrag 10 (Punkte der Abnahme 0) - Regression nach Merge a0d7d749 und Nachbesserung
Abnahme 0 hatte alle Teile 1a-1g und 2 als erfuellt gemessen; die Nachbesserung aendert johnruf_r39.c nur um zwei Haken
(Install + Tick der Tuersperre). Nachgemessen an meinen Laeufen a/b dieses Stands mit dem Pegel-Werkzeug der Spur
(`tests/unit/test_r39_johnruf1041.exe pegel <cap.raw> <wav> <capTick> <xa_vol>`):
| Teil | Beleg (Lauf) | Ergebnis |
|---|---|---|
| 1a Rufe leise in 1041 nach Johns Verschwinden | a: `F91 ROOM1041: John verschwunden ((4,0xF0)=1)`, help03 Takt 649 g 0,4179 / Soll 0,4167, help02 Takt 711 g 0,4201 | wie Abnahme 0 |
| 1b Elzas Satz in 1041 | a: `[msgtext] room=1041 id=40 "Elza: Oh, no, this is John! He is in trouble, I have to help him!"`, main40 Takt 801 g 1,0021 | wie Abnahme 0 |
| 1c Sprachdatei room1041/main40.wav | liegt, `clip loaded: main40.wav` | wie Abnahme 0 |
| 1d Karte mit 1141 | a: `F370 Kartenhinweis angefordert (Eintrag 5: ROOM1041 -> ROOM1141)`, `[hint]` rot/umriss, Abzug `karte.bmp` (POLICE STATION 3F, Umriss 1141; Kopie `p_abn1/karte_lauf_a.png`) | wie Abnahme 0 |
| 1e Leons Panikmusik MAIN01 | a: `F512 Panik-Musikfenster auf` + `entry=FF01 -> MAIN01`, 1061 `[unveraendert, laeuft durch]` | wie Abnahme 0 |
| 1f 1131 lauter | b: `[voice] Start ROOM1131 Id 54 CD_VOL 13652 capTick 649`, g 0,8331 / Soll 0,8333 | wie Abnahme 0 |
| 1g 1141 wie bisher (P-Anteil) | b: MAIN16, Rufe Id 55/53 CD_VOL 16383; Elzas msg 50 spricht in DIESEM Baum noch in 1141 (Spur J entfernt sie, J ist in der Integration - wie Abnahme 0 H3) | P-Anteil wie Abnahme 0 |
| 2 Panikmusik endet bei der Rettung | b: `F4039 ROOM1141 John gerettet ((4,0xF0)=0, (4,0xF2)=0): Bit 97 = 0, Panikmusik aus`, 1171 `entry=5572 -> MAIN32` | wie Abnahme 0 |
Die Tuersperre beruehrt den Ablauf nicht: in 1121/1131/1141 keine `[tuersperre]`-Zeile (Lauf b). **Urteil: erfuellt** (alle Teile).

## 4. Nachbarn / Nebenwirkungen (gezielt)
- `ctest --test-dir re15_port/build -R "integration_r38_wasser$|integration_r39_hund1121|integration_r39_heliport1171|
  integration_r38_heliport1171|integration_r39_parkplatz11c0|unit_r36_kartenbits|unit_r38_wasser|unit_r39_hund1121|
  unit_r39_parkplatz11c0|unit_scd_" --timeout 1800 --output-on-failure -j 3` -> "100% tests passed, 0 tests failed out of 45"
  (u. a. integration_r38_wasser 176,23 s mit Leons Tuerweg 10B0 -> 1260 = Leon-Deckel offen; integration_r39_hund1121 144,58 s;
  integration_r39_heliport1171_weg 76,91 s / _laden 59,82 s und integration_r38_heliport1171_weg_re2/_re15/_laden = Abstieg
  1171 -> 10B1 mit aktiver Deckel-Sperre; integration_r39_parkplatz11c0_k/n/i/w/e = Spur-O-Tuertext im selben MESSAGE-Weg).
- Leon unveraendert: Tabelle der Sperren nur 0x1041/0x1051/0x10B1 (tuersperre_r39.c k_sperren); unit tuer1041/tuer1051/tuer10b1
  pruefen ROOM1040/1050/10B0 (Tuer bleibt Tuer, 10B0 -> ROOM1260).
- Spur Q (master 017f5094, nicht in diesem Baum): ihre Montage-Tueren laufen ueber AOT-Slot 20 (`RE15_R39Q_TUER_SLOT 20`,
  master:re15_port/include/re15_elzazombies_r39.h:60), nicht ueber Slot 1 von 1041 -> kein direkter Konflikt mit der Sperre;
  das Zusammenspiel zeigt erst der Integrationslauf.
- 1141/1131/1121: keine `[tuersperre]`-Zeile (Lauf b), Ablauf wie Abnahme 0.

## 5. Vertrag (VERTRAG.md)
- Bank 9: keine neuen Bits; Bit 97 nur gelesen (re15_tuersperre_r39.c panik()). haelt.
- Nachrichten-Ids: ROOM1041 Id 41 und ROOM1051 Id 40 = VERTRAG 1.2 (Nachtrag 12). ROOM10B1 Id 40 steht NICHT in 1.2 (formal,
  Hinweis H1); keine Kollision: ROOM10B1 hat Original-Ids 0..5 (unit tuertexte "keine Kollision"), kein anderes Modul in master
  oder diesem Baum belegt einen 10B1-Text (`git grep 0x10B1` nur Tuer-Tabellen/Raumliste), keine Stimme.
- Pfade (`git diff 57e5f920..HEAD --name-only`): eigene Dateien (tuersperre_r39.{h,c}, gen/tuersperre_r39.inc,
  tools/r39_johnruf/tuersperre_bauen.py, Tests/Riegel/Dossier der Spur P) + gemeinsame Haken scd_vm.c +4 (Include, op_evt_exec,
  re15_scd_show_message; Kommentar "Runde 39 Spur P"), johnruf_r39.c +3 (eigene Datei). AUFTRAG.md nur durch den Orchestrator
  (dc4bc297, Nachtrag 13). Kein release/, platform/android/, shared_assets/PSX/, synchro/, keine CMakeLists. Werkzeug
  `tuersperre_bauen.py --pruefen` -> "gen/tuersperre_r39.inc ok". haelt.
- Fremde Riegel geaendert (vom Bau-Agenten offengelegt, Orchestrator soll bestaetigen): integration_r38_wasser Lauf
  elza10b1_tuer entfernt (+ Kommentar in probes/r38_wasser.cmake) und integration_r39_hund1121 Spielstaende A/C/E + `4:0xF0`.
  Beide Aenderungen folgen aus gemessenem Verhalten (Deckel zu; Rettungs-Regel loescht Bit 97 ohne (4,0xF0)) und sind
  kommentiert -> Hinweis H2 (haengt an M1: aendert sich die 10B1-Loesung, ist der entfernte Lauf neu zu bewerten).
- Riegel je Punkt vorhanden und messend (5 Unit + 7 Integration neu, a erweitert); Parallel-/Linux-Festigkeit wie Abnahme 0
  (exe-Kopie `re15_pc_r39pt_<teil>_<zufall>`, eigener Laufordner, RE15_*-Umgebung geloescht, `cmake_minimum_required`). ABER:
  die Riegel unit tuertexte und integration t1041/t1051 verlangen die Stimmen-Einreihung, die Nachtrag 13 ausschliesst (M2), und
  KEIN Riegel prueft, dass Elza 10B1 nach der Szene wieder verlassen kann (M1; t10b1s prueft nur "Elza laeuft").

## 6. Hinweise (kein eigener Mangel)
- **H1** ROOM10B1 Port-Id 40 (Deckel-Text) ist in VERTRAG 1.2 nicht zugeteilt (formal; keine Kollision, s. 5.).
- **H2** Fremde Riegel geaendert (integration_r38_wasser ohne Lauf elza10b1_tuer; integration_r39_hund1121 Spielstaende mit
  `4:0xF0`) - begruendet und gemessen, Bestaetigung durch den Orchestrator offen; der entfernte Elza-Lauf ist nach der Loesung
  von M1 neu zu bewerten.
- **H3** ROOM1051 Slot 0 -> ROOM1031 (@0x00ADA) bleibt in der Panikphase offen (nicht verlangt).
- **H4** Nach der Szene steht John AUF dem Deckel (sub04 @0x01A84 Plc_dest (-21422,4607)); ein erneuter Versuch am Deckel scheitert
  an seinem Koerper (l_e10b1f: `GEHE (-21000,2900)` "Runden erschoepft" bei (-20781,2673), AKTION ohne Treffer). Fuer M1 relevant.
- **H5** In diesem Baum spricht Elza ihren Satz in 1141 noch (msg 50, Lauf b) - Spur J entfernt ihn, J ist in der Integration
  (wie Abnahme 0 H3).
- **H6** Deckel-Text Seite 1 steht auf Zeile 2 (leere Kopfzeile = RE2 `fc` @0x02965, so umkodiert) - RE2-Form, kein Fehler.

## 7. Maengel
- **M1 (kritisch, Nebenwirkung von Nachtrag 12c): ROOM10B1 wird fuer Elza zur Falle - Elzas Szenario ist nicht mehr spielbar.**
  Der untere Teil von 10B1 (Kisten Band 1, Gasse Band 0) ist nur ueber den einseitigen Abstieg vom Dach (SCA Zelle 31 @0x01070,
  Band 5, u1 0x0e "hinab") oder ueber die Leiter aus ROOM1261 (main00 @0x0116A Slot 2, ohne Bedingung) erreichbar und hat als
  einzigen Ausgang den Kanaldeckel (Slot 1 @0x016F2, floor 0), der jetzt immer Text ist; einen Aufstieg Band 1 -> Band 5 gibt es
  nicht (keine Band-1-Zelle mit "hinauf"; EXE @0x8002d544/@0x8002d548; gemessen l_e10b1h: AKTION an der Dachwand bei vier
  Lagen und drei Blickrichtungen -> `sub=0`). Die Szene der Sherry-Suche - und damit (3,0x83)/(3,0x41), die der Rest der Kette
  braucht (11B1-Werkstatttuer @0x0112A/@0x0113E, John in 1261/2001) - liegt NUR dort (sub01 @0x0181E Schnitt 7; Flag-Zensus:
  Set(3,0x83)=1 fuer Elza nur @0x0195E, Set(3,0x41)=1 nur @0x01962). Am echten Weg (l_e10b1f) endet die Szene mit dem
  Deckel-Text, Elza ist frei, kommt aber aus dem unteren Teil nicht heraus. Das Dossier (8.8/8.9 "Fuer den Nutzer") nennt dem
  Nutzer einen Umweg "ueber den Keller (Garage 11B1 oder 11D1 -> 1261)", den Elza von dort nicht erreichen kann; der Bau-Agent
  hat ihn nach eigener Angabe nicht gemessen. Kein Riegel prueft, dass Elza 10B1 nach der Szene verlassen kann.
  Noetig: Entscheidung des Nutzers, wie es nach der Szene weitergeht (z. B. Deckel in der Sherry-Szene doch oeffnen = Abweichung
  vom Wortlaut "auch wenn sie nach Sherry sucht"; oder ein belegter Weg aus dem unteren Teil zurueck auf das Dach bzw. in den
  Keller), danach am ECHTEN Weg 1171 -> 10B1 -> ... -> 1261/2001 messen und als Riegel sichern; Dossier 8.8/8.9 berichtigen.
- **M2 (Nachtrag 13): Der Tuersatz 1041/1051 ist im Code, in den Riegeln und im Nutzer-Hinweis KEIN "einfacher Text wie bei Leon".**
  `re15_tuersperre_r39_stimme` (engine/src/tuersperre_r39.c:151-163) + Haken scd_vm.c re15_scd_show_message reihen fuer
  1041 msg 41 / 1051 msg 40 eine Stimme ein (Log `[voice] ROOM1041 msg 41: keine Aufnahme (synchro/STAGE1/room1041/main41.wav)`);
  Leons Tuertexte (sperre1050_r37.c, tuer1060_1040.c) haben keinen solchen Haken. unit_r39_johnruf1041_tuertexte und
  integration_r39_johnruf1041_tuer_t1041/_t1051 VERLANGEN die Einreihung; Dossier 8.9 bittet den Nutzer um
  `room1041/main41.wav` und `room1051/main40.wav` - der Nutzer hat in Nachtrag 13 genau das verneint, das Dossier erwaehnt
  Nachtrag 13 nicht. Noetig: Stimmen-Haken fuer diese beiden Zeilen entfernen, Riegel auf "keine Stimme" umstellen
  (z. B. `pruef_nicht(_d "[voice] ROOM1041 msg 41")`), Dossier 8.9 berichtigen.

## 8. Ergebnis
| Punkt | Urteil |
|---|---|
| Nachtrag 12a: Tuer 1041 -> 1031 ab dem Musikwechsel = "I have to rescue John! He is in danger!" | erfuellt |
| Nachtrag 12b: dasselbe an der Tuer 1051 -> 10A1 | erfuellt |
| Nachtrag 13: einfacher Text wie bei Leon (ohne Sprecherkopf, ohne Stimme) | teilweise (M2) |
| Nachtrag 12c: Tuer ROOM10B1 fuer Elza immer zu, auch bei der Sherry-Suche | teilweise (M1: Wortlaut umgesetzt, Szenario danach nicht spielbar, falscher Umweg-Hinweis) |
| Nachtrag 10 (Abnahme 0: Rufe/Satz/Datei/Karte/Panikmusik/1131/1141; Musik endet bei der Rettung) | erfuellt (Regression gemessen) |
Gates: Bau gruen; Riegel der Spur 22/22, Nachbarn 45/45; RE-Gate haelt (10 Zitate selbst gelesen/disassembliert); Vertrag haelt
(H1/H2 formal). **Abnahme 1: NICHT bestanden** (M1 kritisch, M2).
