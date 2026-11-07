# Runde 41 — Spur W "wasserblink": Abnahme 3 (Delta 9fceaa57b, OFFEN 4 Variante B)

Prüfgegenstand: Commit 9fceaa57b "fix(r41-wasserblink): OFFEN 4 Variante B - Deckentropfen ROOM2000/2001 starten nicht (wie
Version_40%)". Er entstand nach `W_abnahme_2.md` und wurde ohne Abnahme gemergt (Merge 35773161a, Gegenprüfung Welle 1 Hinweis H1).
Dazu gehört der Riegel-Fix 9867e3913 (`integration_r34_plattform_esp_eintritt`). Geprüft im Integrationsbaum `r41/integration`
auf Kopf 9867e3913. Während der Abnahme kamen 9e0066c26 und 8495d82ff dazu. Sie ändern nur `test_r41_birkin_pistole.cmake`,
`test_r41_g5_tod.cmake` und `test_r41_g5_westen.cmake` (kein W-Bezug); die exe ist unverändert (md5 `72bf227f…`).
Dem Commit habe ich nichts geglaubt. Die Skript-Bytes beider Discs habe ich mit einem eigenen Werkzeug durchlaufen, beide EXE
selbst verglichen und an den Startstellen disassembliert. Im Spiel habe ich mit einer eigenen Messkopie gemessen, jeweils mit
Gegenprobe, und die Riegel selbst gefahren. Gebaut habe ich nicht, committet auch nicht.

## Urteil

**BESTANDEN.** Es gibt keinen blockierenden Mangel.
* Der Beleg stimmt: Die 40%-Disc startet die Tropfen-Schleife nie.
* Der Haken lässt genau diesen einen Evt_exec aus, sonst nichts.
* Leon (ROOM2000) und Elza (ROOM2001) sind geprüft, jeweils mit Raumwechsel hin und zurück und mit Laden eines Spielstands.
  Es fällt kein Tropfen, und am Bildrand ist kein Fleck mehr zu sehen.
* Kanalwasser, Wasserlauf und Gitterstrahlen sind unverändert wie in W_abnahme_2.
* Kein Lauf hing, und alle 12 Riegel sind grün.

Nicht blockierend sind drei Punkte: ein Schärfe-Hinweis zum Tropfen-Riegel (H1), ein Hinweis zur Messumgebung (H2) und die Frage an
den Nutzer, ob die Tropfen weg bleiben sollen (Regel "offen melden und dem Nutzer vorlegen").

| Frage des Auftrags | Ergebnis |
|---|---|
| Startet die 40%-Disc die Tropfen wirklich nicht? | **Ja, belegt.** Version_40% ROOM2000 = ROOM2001: main00 endet @0x01598 `01 00`. Im ganzen SCD stehen kein Evt_exec (0x04), kein Gosub (0x18), kein Aot_reset/Aot_on und kein AOT sce 3. Die Schleife sub03 @0x015B6 wird also nie gestartet. Die 9 Aufrufe von FUN_8003ee3c (`jal` @0x8003EFB0..@0x8004D7B0) stehen in beiden EXE an denselben Adressen, und die Startwege sind Byte für Byte gleich (§2). |
| Repo-Baum: wo starten sie? | Nur main00 @0x0177C `04 ff 18 07` (2000) bzw. `04 ff 18 08` (2001). Ziel ist sub07 @0x01B06 bzw. sub08 @0x01D32, beide beginnen mit `3a 00 06 00 00 00 00 10`. Keine andere Stelle startet sub07/sub08 (§2). Im ganzen Port-Baum steht Effekt 6 Sub 0 nur dort. |
| Lässt der Haken genau diesen Evt_exec aus? | **Ja.** Code, Unit-Riegel (33 Prüfungen) und Spiel stimmen überein. `[tropfen41]` erscheint je Eintritt in 2000/2001 genau einmal und in keinem anderen Raum. Die anderen Evt_exec von 2000/2001 starten weiter, gemessen: 2000 sub02, 2001 sub02 und sub03 (§3). |
| Spiel Leon/Elza, Raumwechsel, Laden | **Keine Tropfen, kein Fleck.** Gezählt über alle 16 Läufe der Abnahme-exe ohne Gegenprobe: SPAWN Effekt 6 Sub 0 = 0. In den 13 Läufen mit Bildserie hat das Randfenster Cut 2/5/7 jeweils 0 Wechsel. Dieselbe exe mit gestarteter Schleife (Gegenprobe) zeigt 11 Spawns und den Fleck bei (83,1), (194,1) und (169,15) ab F546. Bildgenau verglichen unterscheiden sich die beiden Läufe nur in diesen 1 bis 3 Bildpunkten (§4). |
| Wasser/Blinken der Spur W unverändert? | **Ja.** Im Kanal-Fenster Cut 2 gibt es 0 Wechsel; die Nutzer-exe vom 2026-11-01 hat dort 32/100 mit Sprung 131. Der Wasserlauf in Cut 10 bewegt im Weg-Fenster 1460 von 1802 Bildpunkten (W_abnahme_2: 1458). Die Gitterstrahlen werden 3x mit 6 Strömen gesetzt (§5). |
| Hänger | **Keine.** Von 27 Starts der Messkopien endeten 26 über `RE15_EXIT_AT` mit rc 0. Der erste Versuch des Rundwegs lief in sein Zeitlimit; Ursache war mein Wegplan, nicht das Spiel (§4). |
| Riegel `integration_r41_wasserblink_*`, `integration_r34_plattform_*` | **7/7 grün**, die Unit-Riegel dazu **5/5 grün** (§6). |

## 1. Messkopie, Messweg, Werkzeuge

* **exe:** Kopie von `re15_port/build/platform/pc/re15_pc.exe` (2026-11-06 20:12:34, md5 `72bf227ff55080e7dd330b172d57a81e`), abgelegt als
  `scratchpad/testint/w_abn3/bin/re15_wa3.exe`. Sie braucht nur System-DLLs (`objdump -p`). Die Assets kommen laut debug.log aus
  `[asset] cd-root: …/r41_integration/re15_port/shared_assets/PSX`. Die exe enthält 1x `tropfen41`.
* **Vergleichs-exe:** die Nutzer-exe aus dem Hauptbaum (2026-11-01 08:21, md5 `b8a6d111…`). Sie war das "vorher" von W_abnahme_2,
  liegt als `re15_wa3_nutzer.exe` vor und enthält 0x `tropfen41`.
* **Läufe** startet `werkzeug/a3_lauf.py`: je Lauf eine eigene PID, ein eigenes Verzeichnis und ein Zeitlimit. Beendet wird nur die
  eigene PID, nie `taskkill /IM`. Die PIDs stehen in `laeufe.json`.
  * Grundumgebung: `SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
    RE15_PSELECT_AUTO=1`, für Elza zusätzlich `RE15_PSELECT_AUTO_SWITCH=1`.
  * Jeder Lauf schreibt drei Protokolle:
    * `RE15_WAFFEN_LOG`: jede ESP-Erzeugung als "SPAWN id=… sub=…" (re15_esp.c:1386). Das gilt unabhängig von Schnitt und
      Regions-Test, und die Datei wird immer angelegt.
    * `RE15_FX_LOG`: die gezeichneten Teilchen, also erst nach dem Regions-Test.
    * `RE15_EVT_TRACE`: jeder gestartete Evt_exec.
* **Bilder:** `RE15_FRAMEDUMP`-Serien, 320x240. Abgegriffen wird das fertig komponierte Bild vor `SDL_RenderPresent`
  (render_pc.c:955ff), mit beschleunigtem Renderer, ohne AUTOSHOT und ohne SOFTWARE_RENDER.
* **Auswertung** `werkzeug/a3_auswert.py`:
  * Je Bildpaar zählt der größte Kanalsprung im Fenster, Schwelle 8, wie das Riegel-Werkzeug `flacker`.
  * Fenster:
    * Rand Cut 2: x 76..92, y 0..5 (Riegel);
    * Cut 5: x 186..200, y 0..5;
    * Cut 7: x 162..176, y 10..20 (Lagen aus W_abnahme_2 H1);
    * Kanal: x 104..230, y 100..200 (Riegel);
    * Wasserlauf: x 72..124, y 146..179 (W_abnahme_2 §6).
  * Dazu kommt eine Wechselzahl-Karte über das ganze Bild (Bildwechsel mit |d| > 16).
* **Gegenprobe mit derselben exe:** `RE15_SUBSTART=7@1#2000` bzw. `8@1#2001` startet die Schleife von Hand (main.c:8283ff,
  `scd_thread_start(3, sub_scd[N])`). Das ist das Verhalten der Mod. Damit ist für jeden Messweg gezeigt, dass er den Tropfen sieht,
  wenn er läuft.
* **Messfalle, selbst gefunden:** SDL wählte den Renderer nicht immer gleich. 11 Läufe der ersten Reihe liefen mit `direct3d`, die
  übrigen mit `direct3d11` (debug.log "[render] SDL-Renderer …"). Zwei Läufe mit verschiedenem Renderer weichen in einzelnen
  Bildpunkten ab: E2_marke (`direct3d`) und E2_marke_b (`direct3d11`) an 246 Orten, davon 238 an der Figur. Bildgenau vergleiche
  ich deshalb nur Läufe mit gleichem Renderer. In der ersten Reihe meldeten zwei Läufe (K_E2_marke, RW_E)
  "SDL_RenderReadPixels failed: GetRenderTargetData(): DEVICELOST" und lieferten 0 Bilder. Beide habe ich wiederholt, die Ergebnisse
  stehen unten.

## 2. Beleg: Skript-Bytes Repo-Baum gegen Version_40% und die Startwege der EXE

Den linearen SCD-Durchlauf macht `werkzeug/a3_scd.py`, die Längen stammen aus `s_opcode_sizes` (scd_vm.c:242ff, 0x2D = 34).
Ausgaben: `logs/scd_repo_room2000.txt`, `logs/scd_repo_room2001.txt`, `logs/scd_40prozent_room2000_voll.txt`.

* Der Port-Baum ist gleich `info/Re1.5`: ROOM2000 hat sha1 `071532554015b217…`, ROOM2001 hat sha1 `fd1e9badcbf1fd31…`, jeweils in
  beiden Bäumen dieselbe.
* **Repo ROOM2000.** Die Tabelle @0x0161C hat 1 Eintrag, main00 beginnt @0x0161E. Inhalt von main00:
  * 5 Door_aot_set und 2 Aot_set sce 8 (@0x016BE, @0x016D2);
  * 3 Obj_model_set;
  * 3x Sce_espr_on Effekt 0x0B Sub 0, Skala 0x1900, @0x0174C/@0x0175C/@0x0176C;
  * danach **@0x0177C `04 ff 18 07`** (Evt_exec cond 0xFF, sub07) und @0x01780 `01 00`.

  sub07 @0x01B06..@0x01BB4 enthält:
  * 8x `3a 00 06 00 00 00 00 10`, Effekt 6 Sub 0, y 0xD8F0 = -10000;
  * For 180 @0x01B36/@0x01B60/@0x01B8A und For 45 @0x01BA4;
  * Goto @0x01BAE `17 ff ff 00 58 ff`.
* **Repo ROOM2001.** An derselben Stelle steht **@0x0177C `04 ff 18 08`**. sub08 @0x01D32..@0x01DE0 ist gleich gebaut, das Goto steht
  @0x01DDA.
* **Wer startet sub07/sub08 sonst?** Gezählt sind die Opcodes 0x04, 0x18, 0x46, 0x47 und AOT sce 3:
  * 2000:
    * Evt_exec: main00 @0x0177C (sub07), sub00 @0x0179C (sub02, hinter `Ck(4,0xF4,1)`), sub02 @0x01A32 (sub03).
    * Gosub nur nach 04/05/06, Aot_set nur sce 8.
  * 2001:
    * Evt_exec: main00 @0x0177C (sub08), sub00 @0x017C6 (sub02) und @0x0181E (sub05), sub02 @0x018DA (sub03).
    * Gosub nur nach 06/07. sub07 von 2001 ist `11 00 08 00 …`, also kein Tropfen.
    * AOT sce 3 @0x017EA und Aot_reset @0x01948 zielen auf `18 04` (sub04); Aot_on gibt es für 0 und 2.
  * **sub07/sub08 startet allein main00 @0x0177C.** Das stimmt mit Commit, Kopf von `re15_tropfen_r41.h` und Dossier §6.1.
* **Version_40% ROOM2000 = ROOM2001** (`cmp` 0 Unterschiede, sha1 `f6c0b119…`).
  * main00 @0x01426..@0x01598 enthält:
    * 5 Door_aot_set (sce 0/0/2/2/0) und 3 Aot_set sce 8 (@0x014C6/@0x014DA/@0x014EE);
    * 3 Obj_model_set;
    * 3x 0x0B @0x01568/@0x01578/@0x01588;
    * dann **@0x01598 `01 00`**.
  * Die Unterprogramme:
    * sub00 @0x015A4 und sub01 @0x015A6 sind `01 00`;
    * sub02 @0x015A8 ist Sleep, Work_set, 0x3F, `01 00`;
    * **sub03 @0x015B6..@0x01664 ist dieselbe Schleife**, 8x Effekt 6 Sub 0, mit anderen x/z (x -2020..25414).
  * Im ganzen SCD stehen **kein 0x04, kein 0x18, kein 0x46, kein 0x47 und kein AOT sce 3**.
* **Startwege der EXE**, über W hinaus geprüft (`werkzeug/a3_jal.py`, `werkzeug/a3_exevergleich.py`; Ausgaben
  `logs/exe_jal_8003ee3c.txt`, `logs/exe_vergleich_mod_40.txt`):
  * `jal 0x8003ee3c` (Wort 0x0C00FB8F) steht in `info/Re1.5/PSX.EXE` und `Version_40%/PSX.EXE` an denselben 9 Stellen:
    @0x8003EFB0, @0x8003EFD4, @0x8003F080, @0x8003F2C0, @0x80043104, @0x8004C734, @0x8004CD18, @0x8004D74C und @0x8004D7B0.
    In keinem STAGE1..6.BIN beider Discs kommt er vor.
  * In beiden EXE Byte für Byte gleich sind:
    * FUN_8003ee3c, die Raum-Init FUN_8003ef6c und der sub01-Takt FUN_8003f038;
    * der Evt_exec-Handler LAB_8003f2a0 und der AOT-sce-3-Handler @0x800430f0;
    * die Opcode-Tabelle @0x800744a8 (95 Einträge; 0x04 → 0x8003F2A0, 0x18 → 0x8003FBE8) und die AOT-Typ-Tabelle @0x8007469c;
    * die Aufruferbereiche @0x8004c720, @0x8004cd00 und @0x8004d740.
  * Raum-Init, selbst disassembliert: Für main00 lädt @0x8003efa0 `lw v0,64(v0)` den Zeiger RDT+0x40, dann folgt @0x8003efb0
    `jal 0x8003ee3c` mit a1 = 0. Für sub00 lädt @0x8003efc4 `lw v0,68(v0)` den Zeiger RDT+0x44, a0 = 1 @0x8003efc0, dann folgt
    @0x8003efd4 `jal 0x8003ee3c`.
  * AOT-Typen, selbst disassembliert:
    * sce 0 @0x8004305c kopiert nur zwei Halbworte;
    * sce 2 @0x800430bc ist die Tür;
    * nur sce 3 @0x800430f0 startet einen Faden: `lhu a0,0(v0)`, `lbu a1,3(v0)`, `jal 0x8003ee3c` @0x80043104.
  * **Damit läuft sub03 auf der 40%-Disc nie.** Es gibt keinen Evt_exec, keinen Gosub und kein AOT sce 3, und sub03 ist weder main00
    noch sub00 oder sub01. Die Aussage des Commits ist belegt, enger als im Commit selbst.
* **Zensus über alle RDT** (`werkzeug/a3_r13_start.py` plus Rohsuche `3a 00 06 00` in den SCD-Abschnitten, Ausgabe
  `logs/zensus_effekt6sub0_start.txt`):
  * Im Port-Baum steht Sce_espr_on Effekt 6 Sub 0 nur in ROOM2000 sub07 und ROOM2001 sub08, je 8x. Gestartet wird beides nur über
    main00 @0x0177C.
  * Version_40% hat die Schleife in ROOM2000 sub03, ROOM2001 sub03 und ROOM20B1 sub03; gestartet wird sie nirgends.

## 3. Der Haken: lässt er genau diesen Evt_exec aus?

* **Original-Handler** LAB_8003f2a0 (in beiden EXE gleich, selbst disassembliert):
  * @0x8003f2b0 `lbu v1,1(v0)` = cond;
  * @0x8003f2b4 `lbu a1,3(v0)` = sub;
  * @0x8003f2b8 `addiu v0,v0,4` und @0x8003f2bc `sw v0,28(a0)` = pc += 4;
  * @0x8003f2c0 `jal 0x8003ee3c`, mit a0 = cond im Verzögerungsplatz;
  * @0x8003f2c8 `ori v0,zero,0x1` = Rückgabe 1.
* **Port** (scd_vm.c:1241-1244): Greift der Haken, macht er `t->pc += 4` und gibt `SCD_R_CONTINUE` = 1 zurück (scd_vm.c:714). Das ist
  der Original-Handler ohne den Fadenstart. Danach beendet @0x01780 `01 00` main00 im selben Takt, so wie @0x01598 auf der 40%-Disc.
  Der Haken sitzt hinter dem Panel-Haken (nur 11F0/11F1) und vor dem Vorrücken.
* **Eingrenzung im Code** (tropfen_r41.c). Der Haken greift nur, wenn alles zutrifft:
  * Raum 0x2000 mit sub07 oder 0x2001 mit sub08;
  * main00 liegt @0x0161E, und pc steht in main00 @0x0177C;
  * die Bytes dort sind `04 ff 18 07` bzw. `04 ff 18 08`;
  * das Ziel liegt @0x01B06 bzw. @0x01D32 und beginnt mit `3a 00 06 00 00 00 00 10`.

  Andere Räume scheitern schon an der Raumprüfung, auch alle Elza-Räume 20x1 außer 2001. Jede Konstante trägt ihren Datei-Offset (RE-Gate erfüllt).
* **unit_r41_tropfen**, selbst gefahren: "ROOM2000 (Leon): Haken 1, Effekt 6 Sub 0 hoechstens 0, Faden in der Schleife nie,
  Gitterstrahlen 3/3", Elza dasselbe, "unit_r41_tropfen: 33 Pruefungen, OK".
  Dazu kommen Gegenstellen, an denen der Haken nicht greifen darf:
  * fremde Räume: 20B0, 20B1, 2010, 1140, 2002;
  * die übrigen Evt_exec @0x0179C/@0x01A32 bzw. @0x017C6/@0x0181E/@0x018DA;
  * eine Kopie mit `04 ff 18 06` und eine Kopie mit Effekt 7 im Ziel.
* **Im Spiel:**
  * `[tropfen41]` steht je Eintritt in 2000/2001 genau einmal. Im Rundweg steht es zweimal, in 2010/2011 nie.
  * Mit der Abnahme-exe zeigt `RE15_EVT_TRACE` in keinem Lauf "Evt_exec sub=7" in 2000 oder "sub=8" in 2001.
  * Die Nutzer-exe zeigt genau diesen Start: "[evt] F250 room=2000 Evt_exec sub=7 cond=0xff -> slot 10" bzw. "room=2001 … sub=8".
    Nur dieser Start fällt weg.
  * Die anderen Evt_exec laufen weiter:
    * N_L mit `RE15_SET_FLAG=4:244` (Bedingung sub00 @0x01798 `21 04 f4 01`): "[evt] F250 room=2000 Evt_exec sub=2 … -> slot 10";
    * N_E mit `RE15_SET_FLAG=3:65` (Bedingung @0x0179A `21 03 41 01`): "room=2001 Evt_exec sub=2" und daraus "sub=3".
    * In beiden Läufen steht auch die `[tropfen41]`-Zeile, und es gibt 0 Spawns Effekt 6 Sub 0.

## 4. Im Spiel: ROOM2000 Leon und ROOM2001 Elza

Wege:
* **Marke:** `RE15_DEBUG_JUMP=2000@250` mit `RE15_PLAYER_POS=-7000,-25659,1976,1`. Die echten RVD-Zonen schalten Cut 0 → 1 → 2, wie im
  Riegel.
* **Cut 5/7/10:** `RE15_FORCE_CUT`.
* **Laden:** Ein Spielstand vom Riegel-Werkzeug `test_r41_nachtrag30 karte`, Raum 2000 bzw. 2001 bei (-7000,-1800,-25659). Danach
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`.
* **Rundweg:**
  * Hin: `RE15_FIRE_AOT=2@300#2000` (bzw. `#2001`), Tür Slot 2 @0x0165E zu Stage 2 Raum 0x01.
  * Zurück zu Fuß mit `RE15_WEGPLAN` zur Tür Slot 0 von 2010/2011 (@0x00746, Rechteck x -5800..-4800, z -5800..-4050) und AKTION.
  * Der zweite Eintritt liegt in Cut 14 (-26450,-1800,-2700).
* Bilder ab F520: Punkt 7 erscheint ab F546.

"Wechsel" heißt: Bildpaare mit einem Kanalsprung > 8 im Fenster.

| Lauf | Figur, Raum, Weg | Bilder | `[tropfen41]` | SPAWN 6/0 | fx 6/0 | Randfenster | Kanal-Fenster | Ende |
|---|---|---|---|---|---|---|---|---|
| L2_marke | Leon 2000, Marke, Cut 2 | F520..F1300 jedes 2. (zwei Umläufe) | 1 | **0** | 0 | Cut 2: **0/390**, Sprung 0 | 0/390 | F1300 |
| E2_marke | Elza 2001, Marke, Cut 2 | F520..F1300 | 1 | **0** | 0 | **0/390** | 0/390 | F1300 |
| E2_marke_b | Elza 2001, Marke (Wiederholung, direct3d11) | F520..F720 | 1 | **0** | 0 | 0/100 | 0/100 | F720 |
| L5_cut / E5_cut | Cut 5 | F520..F720 | 1 / 1 | **0 / 0** | 0 / 0 (dabei 37863 Zeilen 0x0B gezeichnet) | Cut 5: **0/100** | – | F720 |
| L7_cut / E7_cut | Cut 7 | F520..F720 | 1 / 1 | **0 / 0** | 0 / 0 (38205 Zeilen 0x0B) | Cut 7: **0/100** | – | F720 |
| LD_L / LD_E | **Spielstand geladen** ("[save] CONTINUE: resumed in room 2000/2001"), Cut 0 → 1 → 2 | F520..F720 | 1 / 1 | **0 / 0** | 0 / 0 | Cut 2: **0/100** | 0/100 | F720 |
| RW_L / RW_E | **2000 → 2010 → 2000** bzw. **2001 → 2011 → 2001** ("DOOR FIRE slot=2", dann "slot=0") | F520..F900 im zweiten Aufenthalt | **2 / 2** | **0 / 0** | 0 / 0 | 0/190 in allen drei Randfenstern. Cut 14 wechselt nur an der Figur (Kasten x 150..190, y 75..173) | – | F900 |
| N_L / N_E | Marke mit Ereignis-Bit (§3) | – | 1 / 1 | 0 / 0 | – | – | – | F400 |
| **Gegenprobe** K_L2_marke | Leon, `RE15_SUBSTART=7` | F520..F720 | 1 | **11** | **1065** (ab F186) | Cut 2: **47/100, Sprung 58 bei (83,1), ab F546** | 0/100 | F720 |
| **Gegenprobe** K_E2_marke(_b) | Elza, `RE15_SUBSTART=8` | F520..F720 | 1 | **11** | **1065** | **45/100, Sprung 21 bei (83,1), ab F546** | 0/100 | F720 |
| **Gegenprobe** K_L5_cut | Leon Cut 5 | F520..F720 | 1 | **11** | 2755 | Cut 5: **88/100, Sprung 73 bei (194,1), ab F546** | – | F720 |
| **Gegenprobe** K_L7_cut | Leon Cut 7 | F520..F720 | 1 | **11** | 2755 | Cut 7: **47/100, Sprung 60 bei (169,15), ab F546** | – | F720 |
| **Nutzer-exe** V_L2/V_E2 | 2026-11-01, Marke | F520..F720 | 0 | **11** | 1072 | **45/100, Sprung 21 bei (83,1), ab F546** | **32/100, Sprung 131** (entferntes Raumlicht) | F720 |

* Die Gegenprobe zählt 11 Spawns in 720 Bildern. Das passt zur Schleife: 3 bei t = 0, 2 bei 180, 2 bei 360, 1 bei 540 und 3 bei 585
  (zweiter Umlauf).
* **Bildgenauer Vergleich**, nur bei gleichem Renderer, F520..F720 jedes 2. Bild (101 Paare):
  * L2_marke gegen K_L2_marke: 13 Bilder bitgleich (F520..F544). In den übrigen weichen nur (83,1) [84x], (83,0) [5x] und (84,0)
    [3x] ab.
  * E2_marke_b gegen K_E2_marke_b: 15 bitgleich, sonst nur (83,1) [83x], (84,0) [3x] und (83,0) [1x].
  * L5_cut gegen K_L5_cut: nur (192..195, 0..2).
  * L7_cut gegen K_L7_cut: nur (169,14), (169,15) und (170,14).
  * K_E2_marke gegen K_E2_marke_b: 101/101 bitgleich. Zwei Läufe derselben exe mit demselben Renderer sind also deterministisch.
  * **Der Haken nimmt aus dem Bild genau den Tropfen-Punkt und sonst nichts**, bei Leon und bei Elza.
* Die Wechselzahl-Karten der Abnahme-Läufe zeigen nur die Figur: L2_marke Kasten x 68..99, y 51..126, E2_marke x 73..99, y 58..125,
  Cut 5 und Cut 7 nichts. Die Gegenprobe zeigt zusätzlich (83,1).
* **Hänger:** keine. Von 27 Starts der Messkopien enden 26 mit "EXIT_AT: Bild N in Raum 200x erreicht" und rc 0. In `laeufe.json`
  stehen 23 Läufe; dazu kommen 4 Wiederholungen:
  * RW_L, siehe unten;
  * LD_L, der erste Spielstand lag in Cut 0;
  * K_E2_marke und RW_E, beide mit DEVICELOST.
  Der erste Rundweg-Versuch (PID 47264) lief in sein Zeitlimit von 600 s. Ursache war mein Wegplan: Er lief auf
  (-5300,-4925) zu, die Figur stand aber bei x -4832 an der Wand, und der Befehl GEHE kam nie an. Das Spiel lief in 2010 weiter bis
  F14460, `[walk]`-Zeilen bis zuletzt. Mit dem Ziel (-4880,-4925) lief der Weg in 109 s bzw. 80 s durch.

Bilder (`W_belege/abn3/`):
* `abn3_cut2_rand_leon_elza.png`: Cut 2, oberer Rand, 8-fach, F544..F566, Abnahme und Gegenprobe, Leon und Elza.
* `abn3_cut5_rand.png` und `abn3_cut7_rand.png`.
* `abn3_cut2_voll_leon_f600.png` und `abn3_cut2_voll_elza_f600.png`: ganzes Bild mit Kreis um Punkt 7.
* `abn3_laden_cut2_f600.png` (Spielstand geladen) und `abn3_rueckweg_cut14_f600.png` (zweiter Eintritt).
* `abn3_wechselkarten_cut2.png`.
* `bilder_roh/` mit 20 verlustfreien 320x240-Bildern der Läufe.

## 5. Wasser und Blinken der Spur W unverändert

* **Kanal-Fenster** x 104..230, y 100..200 in Cut 2: 0 Wechsel > 8 in allen Cut-2-Läufen der Abnahme-exe (L2/E2 je 390 Paare,
  E2_b, LD_L, LD_E je 100). Die Nutzer-exe hat dort 32/100 mit Sprung 131; das sind die in W_abnahme_2 entfernten Raumlicht-Blitze.
  Die gelben Bodenflecken stehen still. Damit gilt der Stand von W_abnahme_2 §2 weiter, und die dortigen 5 Restwechsel am Rand
  (Tropfen-Punkt 7) sind jetzt auch 0.
* **Wasserlauf an der Leiter**, Cut 10, F300..F420, jedes Bild (L10_cut/E10_cut):
  * In jedem Lauf steht `[wasserlauf41] ROOM2000/2001: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F`.
  * Im Weg-Fenster x 72..124, y 146..179 haben 1460 von 1802 Bildpunkten 6 oder mehr Wechsel, in 120/120 Bildpaaren
    (W_abnahme_2 §6: 1458).
  * Die Wechselkarten von Leon und Elza sind gleich.
  * Bild: `abn3_cut10_wasserlauf.png`.
* **Effekte** laut `RE15_WAFFEN_LOG`, in jedem Lauf:
  * 3x "SPAWN id=11 sub=0 scale=0x1900 streams=6", die Gitterstrahlen aus main00 @0x0174C/@0x0175C/@0x0176C;
  * 6x "SPAWN id=11 sub=0 scale=0xc00 streams=6", die Strahlen des Wasserlaufs;
  * dazu id=15 sub 3/6 und id=6 **sub=5** (Ringe des Wasserlaufs).

  Effekt 6 mit anderem Sub läuft also weiter, es fehlt nur Sub 0. In Cut 5/7/10 zeichnet der Port 31725..38205 Zeilen 0x0B.
* Riegel `integration_r41_wasserblink_leon` und `_elza`, Bilder mit dem Riegel-Werkzeug nachgezählt:
  "flacker: 120 Bilder, 0 von 119 Wechseln > 8 (groesster Sprung 0), erlaubt 0".

## 6. Riegel (selbst gefahren)

Gefahren aus Git-Bash mit `PATH=/c/msys64/mingw64/bin:$PATH`, `SDL_ASSERT=always_ignore`,
`--test-dir "C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/build"`, `-j 2`.
Gleichzeitig liefen zwei fremde ctest im selben build/ (masken36, r37_elzaems) und einer im Baum r41_gatorfix. Mein Protokoll steht
deshalb per `-O` in `W_belege/abn3/logs/`.

* `-R "^(integration_r41_wasserblink_.*|integration_r34_plattform_.*)$"`, `--timeout 1200`:
  ```
  100% tests passed, 0 tests failed out of 7

  Total Test time (real) = 222.19 sec
  ```
  Einzeln: wasserblink_tropfen_elza 80.46 s, wasserblink_tropfen_leon 80.57 s, wasserblink_leon 79.59 s, wasserblink_elza 79.59 s,
  wasserblink_gegen 62.03 s, r34_plattform_takt 23.18 s, r34_plattform_esp_eintritt 9.73 s.
* `-R "^(unit_r41_tropfen|unit_raumlicht|unit_r41_wasserblink_werkzeug|unit_r34_plattform|unit_r34_plattform_ton)$"`, `--timeout 600`:
  ```
  100% tests passed, 0 tests failed out of 5

  Total Test time (real) =   0.27 sec
  ```
* Kein Riegel war rot; nachzufahren war nichts.
* **`integration_r34_plattform_esp_eintritt` nach 9867e3913.** Mein Lauf `lauf_68527a68` zeigt `[tropfen41]` 1x und im wf.log
  3x "SPAWN id=11 sub=0 scale=0x1900 streams=6", 0x "SPAWN id=6" und 0x "streams=-1".
  * Die eigentliche Aussage bleibt geprüft: Die Raumbank steht vor dem Eintrittstakt, 3 von 3 Spawns haben 6 Ströme. Auch die
    Negativ-Kontrolle des Auswerters ist unverändert.
  * Der Riegel ist nicht bloß grün gemacht. Er verlangt jetzt 0 Spawns von Effekt 6 Sub 0 in den ersten 10 Bildern; vorher waren es
    3, weil sub07 bei t = 0 drei Punkte setzt. Dazu verlangt er genau eine `[tropfen41]`-Zeile.
  * Fällt der Haken weg, etwa weil sich RDT-Bytes verschieben, wird er rot.
  * Die Gegenprüfung Welle 1 (Abschnitt 4) kommt zum selben Ergebnis.
* Die Tropfen-Riegel, mit dem Riegel-Werkzeug nachgezählt (`lauf_731bbfe3_tropfen`, `lauf_47bbfd2c_tropfen`):
  * Rand: "flacker: 60 Bilder, 0 von 59 Wechseln > 8 (groesster Sprung 0), erlaubt 0";
  * Kontrolle Cut 10: 10125 Zeilen 0x0B und 0 Zeilen Effekt 6 Sub 0.

## 7. Restmängel und Hinweise (alle nicht blockierend)

**H1 — Riegel-Schärfe `integration_r41_wasserblink_tropfen_leon/_elza` (nicht blockierend, Vorschlag).**
* Die Prüfung "fx.log ohne id=6 sub=0" ist in Cut 2 leer. Dort zeichnet der Port kein einziges Teilchen, deshalb legt
  `RE15_FX_LOG` die Datei gar nicht an. In beiden Tropfen-Läufen meiner ctest-Runde fehlt fx.log; der Riegel liest dann "" und zählt 0.
* Scharf bleibt der Riegel durch zwei andere Prüfungen: das Randfenster (83,1) in F560..F619 und die `[tropfen41]`-Zeile.
  Mit der exe vor dem Haken war er laut Dossier §6.4 in allen vier Prüfungen rot, weil gezeichnete Tropfen die Datei anlegen.
* Schnittunabhängig und immer vorhanden wäre `RE15_WAFFEN_LOG` "SPAWN id=6 sub=0" = 0.
  * Für Leon prüft das esp_eintritt bereits, allerdings nur 10 Bilder lang.
  * Für Elza (2001) prüft es nur `unit_r41_tropfen`.

**H2 — Messumgebung (Hinweis, kein W-Bezug).** SDL wählte zwischen zwei Läufen derselben exe verschiedene Renderer, `direct3d` und
`direct3d11`. In zwei Läufen kam DEVICELOST dazu, und der Bildabgriff lieferte nichts. Bildgenaue Vergleiche über Läufe gelten nur
bei gleichem Renderer (§1).

**H3 — OFFEN 5 eingegrenzt (Hinweis).** Im Port-Baum startet kein anderes Raumskript Effekt 6 Sub 0 (§2, Zensus). Der Port-Fehler
"hängender Tropfen" kann über den Skriptweg Sce_espr_on also nirgends mehr auftreten. Er stammt aus der Klemme `floor_y` in re15_esp.c
und dem Fehlen von Routine 13 in `esp_fx_dispatch`. Andere Startwege, etwa Engine-Code, habe ich nicht geprüft.

**H4 — PSX nicht gemessen (Hinweis).** Hier ist kein PSX-Lauf möglich. Der Haken ist Engine-Code und gilt für beide Plattformen.
`g_current_room_id` setzt die PSX-Seite beim RDT-Laden (platform/psx/src/re15_room.c:39).

**H5 — Stärke des Video-Gegenbelegs (Hinweis).** Die Rechnung "in 9,5 s hätte ein Tropfen fallen müssen" (Dossier OFFEN 4,
W_abnahme_2 §5.3) stützt sich auf die Punkte der Mod-Schleife. Die Schleife der 40%-Disc hat andere x/z.
* Der Gegenbeleg bleibt schwach.
* Für den Entscheid tragend ist etwas anderes: Ein positiver 80%-Beleg fehlt, und die 40%-Disc startet die Schleife nicht.
* Das Video habe ich nicht neu ausgewertet; die Auswertung steht in W_abnahme_2 §5.3.

**H6 — Prozess (Hinweis).** Merge 35773161a heißt "Abnahme bestanden", deckte 9fceaa57b aber nicht ab (Gegenprüfung H1). Diese
Abnahme schließt die Lücke. Die Commit-Nachricht nennt "Nachbarschaft 43/45", Dossier §6.4 den Endstand 45/45. Das widerspricht
sich nicht.

## Für den Nutzer

* In ROOM2000 (Leon) und ROOM2001 (Elza) fallen keine Deckentropfen mehr.
* Der kleine flackernde Bildpunkt oben an der Decke ist in allen drei Kameras weg, in denen er zu sehen war. Das gilt auch nach
  Laden eines Spielstands und nach dem Weg durch die Tür nach 2010 und zurück.
* Das ist der Stand der echten 40%-Disc: Dort steht die Tropfen-Schleife zwar im Raumskript, wird aber nie gestartet. Gestartet hat
  sie nur die Fan-Mod.
* Kanalwasser, Wasserlauf an der Leiter und Gitterstrahlen sind unverändert.

## Frage an den Nutzer (Release Notes)

* Die Deckentropfen im Kanal (ROOM2000/2001) sind aus. Die Fan-Mod ließ sie alle 19,5 s fallen; im Port hingen sie bisher nur als
  flackernder Bildpunkt unter der Decke. Die echte 40%-Disc hat dieselbe Tropfen-Schleife, startet sie aber nie, und im 80%-Video
  ist in den 9,5 s dieses Kanals kein Tropfen zu sehen. **Sollen die Tropfen so wegbleiben?**
  Falls Sie sie als Raumausstattung zurück möchten, müssten sie richtig fallen und am Boden aufspritzen. Dazu würde die
  Tropfen-Routine des Originals (Routine 13 @0x80017990) nachgebaut.

## Werkzeuge und Belege

* Werkzeuge (`W_belege/abn3/werkzeug/`; gelaufen im Scratchpad `testint/w_abn3/`, die Pfade sind relativ zu dort):
  * `a3_scd.py`: SCD-Durchlauf und Zensus;
  * `a3_jal.py`: Aufrufe von FUN_8003ee3c in EXE und Overlays;
  * `a3_exevergleich.py`: Bytevergleich der Startwege, Mod gegen 40%;
  * `a3_r13_start.py`: welche Räume Effekt 6 Sub 0 starten;
  * `a3_lauf.py`: Läufe mit eigener PID;
  * `a3_auswert.py`: Logs, Fenster, Wechselkarten;
  * `a3_belege.py`: Bildtafeln;
  * Wegpläne `rw_leon.txt`, `rw_elza.txt`.
* Laufliste `liste_a3.json`, PIDs und Laufzeiten `laeufe.json`, Messwerte je Lauf `auswertung.json`.
* `logs/`:
  * je Lauf `<lauf>_debug.log` und `<lauf>_spawn_fx.txt` (SPAWN-Zählung aus wf.log, Zeilenzahl fx.log);
  * SCD-Durchläufe, EXE-Vergleich, Zensus;
  * `ctest_integration.log`, `ctest_unit.log`.
* Bildtafeln `abn3_*.png` und Rohbilder `bilder_roh/`.
* Die Messkopien und vollständigen Bildserien (664 MB) bleiben im Scratchpad `testint/w_abn3/` und werden nicht eingecheckt.
