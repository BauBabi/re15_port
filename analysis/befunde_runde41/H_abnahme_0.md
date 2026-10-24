# Runde 41 — Spur H "kollision": Abnahme 0 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, gepruefter Stand **103563cc** (merge-base master =
7b59e297, d.h. master ist enthalten). Arbeitsordner der Abnahme (Logs, Bilder, Skripte):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/H41abn0/`.

Status: IN ARBEIT (fortlaufend geschrieben). Fortsetzung 2026-10-24 nachmittags (Vorgaenger vom Sitzungslimit ~12:23
beendet); geprueft wird jetzt HEAD **4616305a** (= 103563cc + Abnahme-Anfang 7552fee6 + AUFTRAG/VERTRAG-Sync, nur Doku:
`git diff 103563cc 4616305a --stat` -> nur AUFTRAG.md und diese Datei).

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
- Gegenprobe, dass die exe den Diff traegt (kein altes Artefakt): `objdump -r` der Objekte —
  `enemy_ai_common.c.obj: IMAGE_REL_AMD64_REL32 re15_kollision_r41_npc_schwanz`,
  `aot_common.c.obj: ... re15_kollision_r41_kisten_pass`, `game_step_common.c.obj: ... re15_kollision_r41_objspiegel`;
  Quellen 11:27:59, `re15_pc.exe` 11:28:49, Arbeitsbaum sauber (`git status` leer).

## 1. Diff gelesen (git diff master -- re15_port)
- NEU `engine/src/kollision_r41.c` (221 Z.), `include/re15_kollision_r41.h` (49 Z.).
- Haken: `enemy_ai_common.c` (+1 Include, +1 Zeile in re15_npc_ai_tick vor re15_npc_wall_clamp),
  `aot_common.c` (+1 Include, Objekt-Pass in re15_prop_push_tick, Kopfkommentar), `game_step_common.c` (+1 Include,
  +1 Zeile re15_kollision_r41_objspiegel nach re15_objfall_nach_pass).
- Tests: `tests/unit/probes/r41_kollision.cmake`, `tests/unit/test_r41_kollision.c`, `tests/unit/probe_r41_kollision_abstand.c`,
  `tests/integration/test_r41_kollision.cmake`; FREMDER Riegel geaendert: `tests/integration/test_r39_dining1141_kampf.cmake`
  (WARTE 60 -> 240).

### 0a. Bau erneut (Fortsetzung)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
- `re15_pc.exe` 11:28:49 > alle Quellen des Diffs (kollision_r41.c 10:14:41, Haken-Dateien 11:27:59); Messkopie
  `re15_pc_h41abn0.exe` sha1 `3da1c0f4c3df7fbabd91679e961d93e6f10df207` = `re15_pc.exe` (bytegleich).

## 2. Tests der Spur (selbst gefahren)
`ctest --test-dir re15_port/build -R "r41_kollision" -j 3 --timeout 900 --output-on-failure` ->
`100% tests passed, 0 tests failed out of 9` (`Total Test time (real) = 221.80 sec`; unit npc/raum3070/raum3001/kiste,
integration kiste/raetsel_leon/raetsel_elza/npc3070/npc3001). Log: Arbeitsordner `ctest_r41_kollision.log`.

## 3. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE`
## bzw. `info/Re1.5/PSX/BIN/STAGEn.BIN`)
Stichproben (mehr als die geforderten 3), jeweils die Stelle selbst gelesen, nicht das Dossier:
1. **NPC-Wurzel-Schwanz @0x8011cc3c (STAGE1, Ada 0x42, Wurzel 0x8011cb70):**
   `8011cc1c jalr v0` (Zustand) -> `8011cc2c jal 0x8002b498` -> `8011cc34 lui a1,0x800b / 8011cc38 lw a1,-14460(a1)` (a1 =
   g_entity(cur) = die NPC) -> **`8011cc3c jal 0x8002aec4` / Delay `8011cc40 addiu a0,s0,20`** (s0 = 0x800aca40 aus
   `8011cb78 lui s0,0x800b / 8011cb7c addiu s0,s0,-13760` -> a0 = 0x800aca54 = Spieler) -> **`8011cc44 jal 0x8002b544`** ->
   `8011cc58 lw v0,120(a0) / 8011cc60 lhu a1,6(v0) / 8011cc64 jal 0x8003b0a4` (Wandklemme, Radius Kasten+6). Freeze-Tor
   `8011cb88 lui v1,0x2000 / and / 8011cb90 bne v0,zero,0x8011cc7c` springt hinter den ganzen Schwanz. = wie behauptet.
   STAGE2 (Ada 0x80115d20): `80115dcc jalr` -> `80115ddc jal b498` -> `80115dec jal 0x8002aec4` / `80115df0 addiu a0,s0,20`
   -> `80115df4 jal 0x8002b544` -> `80115e14 jal 0x8003b0a4`.
   **Alle 28 Wurzeln** (eigener Scanner `skripte/scan_tail2.py`: Registrierung `sw v0,0x2cXX(at)` auf 0x80072bac+typ*4,
   z.B. STAGE1 @0x8011e92c typ 0x42 -> 0x8011cb70; je Wurzel Reihenfolge jalr < b498 < aec4 < b544 (unmittelbar) < b0a4,
   Delay-Slot von aec4 = `0x26040014` = `addiu a0,s0,20`, Prolog `lui s0,0x800b / addiu s0,s0,-13760`, a1 davor =
   `lui a1,0x800b / lw a1,-14460(a1)`): **`ALLE OK: 28 / 28`** (STAGE1 6, STAGE2-4 je 4, STAGE5/6 je 5).
2. **FUN_8002b544** (PSX.EXE): `8002b550 lbu s1,-13746(s1)` (g_active_count), `8002b564 addiu s0,s0,-13268` (= 0x800acc2c),
   `8002b578 andi v0,v0,0x1` / `beq -> 8002b5a4` (inaktiv: Zaehler NICHT erniedrigt), `8002b590 beq a1,s0` (sich selbst;
   Delay `addiu s1,s1,-1`), **`8002b598 jal 0x8002aec4` / Delay `8002b59c addu a0,s0,zero`** (a0 = die ANDERE, a1 = NPC),
   `8002b5ac addiu s0,s0,500`. Kein Typfilter. = wie behauptet; der Port laeuft die Slots 1..RE15_ACTOR_MAX-1 aufsteigend
   ueber alle aktiven (gleiche Menge, solange g_active_count = Zahl der aktiven Eintraege).
3. **FUN_8002aec4-Gates:** `8002af04 or v0,a0,v1 / andi 0x2 / bne -> return 0`, `8002af14 and v0,a0,v1 / andi 0x1000 / bne ->
   return 0` (beide), `8002af24 andi v0,v1,0x4` (Geschobener) / `beq` sonst return 0; Radien `8002af40 lhu t4,6(s6)` /
   `8002af4c lhu t4,6(s5)`. Port: `re15_body_push` (seit Runde 7) prueft 0x2/0x4 ueber `flags`, 0x1000 an den Aufrufstellen;
   der neue Schwanz bildet "tot/Leiche" ab wie `re15_enemy_body_push_tail` (Bewertung s. Mangel-/Hinweisliste).
   Kaesten selbst gelesen: STAGE1 `0x80121664 = 58 16 12 80` -> 0x80121658 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` (r 450,
   h 1530), 0x80121728 = `... f4 01 a0 05 f4 01` (0x45 r 500), 0x801218c8 = `... 2c 01 a0 05 2c 01` (0x4b r 300) =
   `re15_enemy_apply_hitbox` (vorbestehend).
4. **Objekt-Pass FUN_8002bd44 @0x8002c03c-c0a8:** `8002bfb0 jal 0x8003b558` (a1 = 2) / `8002bfb8 bne v0,zero,0x8002c0b0`;
   `8002bfc0 sb v0(=8),140(s2)`; `8002bfd4 ori v0,v0,0x2000`; `8002bfe4 bne v1,v0(=8),0x8002c0b8`; Entity-Durchlauf
   `8002c020 jal 0x8002cabc` (a2 = 1) / `8002c030 sb s7,140(s2)` (nicht portiert, OFFEN 1 des Dossiers);
   **`8002c03c lw v0,-14472(v0)` (0x800ac778) / `8002c048 lbu v0,2(v0)` / `8002c050 beq v0,zero,0x8002c0d8` /
   `8002c058 addu s0,fp,zero` / `8002c05c beq s2,s0,0x8002c088` (Delay `addiu s1,s1,1`) / `8002c06c andi v0,v0,0x1` /
   `8002c078 jal 0x8002cdf4` (Delay `addu a1,s2,zero`) / `8002c080 bne v0,zero,0x8002c0b0` / `8002c0a4 addiu s0,s0,148` /
   `8002c0a8 j 0x8002c0d8`**; `8002c0b0 sb s7,140(s2)` mit `8002bd84 ori s7,zero,0xa` (= 10), `fp` = `8002bda8/ac` 0x800b3f98;
   Ruecksetzung `8002c0bc-c0d4` (x/z aus s4/s5, `jal 0x8003b558`); Spiegel `8002c0ec lhu v1,56(s2)` -> `8002c0f8 sh v1,66(s2)`,
   `+0x40/+0x44` @0x8002c100/@0x8002c10c. = wie behauptet.
5. **FUN_8002cdf4 Zeile fuer Zeile gegen `re15_kollision_r41_8002cdf4`:** Gate `8002ce18 or / 8002ce1c andi 0x40 / bne`;
   dX `8002ce48-50` (b.cx+b.x)-(a.cx+a.x), dZ `8002ce60-68`; Summen `8002ce78 addu a1,a1,v1` (hx+hx), `8002ce7c addu
   t3,a0,v0` (hz+hz); `8002ce80-8c addu v1,t7,a1 / sll v0,a1,1 / sltu v0,v0,v1 / bne -> 0` (vorzeichenlos, inklusive Grenze)
   und Z `8002ce94-a0`; Y `8002cea8-cee0` (cy +2, y +0x38, hy +8, `sltu` / `xori 1` / `beq a2,zero -> 0`); Vor-Y
   `8002cee8 lh v1,66(a3) / 8002ceec lh v0,66(t2)` .. `8002cf08 xor / 8002cf0c and / 8002cf10 beq -> 0x8002cf40`; vertikal
   `8002cf18 bgez t6` / Delay `subu v0,t8,t1` / `+1+sumY` bzw. `-1-sumY` / `sw v0,56(a3)`; waagrecht `8002cf14 mult t7,t3` /
   `8002cf44 mult t5,a1` / `slt` -> x bzw. z aus der Summe +-1 (`8002cf84 sw v0,52(a3)` / `8002cfb4 sw v0,60(a3)`) /
   `8002cfb8 ori v0,zero,0x1`. Die Portierung bildet jede dieser Zeilen ab (u32-Vergleiche, s16-Spiegel, Vorzeichenzweig);
   die waagrechte Ausschiebung laesst sie weg, weil der Aufrufer x/z @0x8002c0bc-d4 aus s4/s5 zuruecksetzt — selbst
   nachgelesen: `8002c0bc sll v0,s4,16 / sra / sw v0,52(s2)`, `8002c0c8 sll v0,s5,16 / sra / 8002c0d4 sw v0,60(s2)`. Korrekt.
6. Konstanten im Diff: keine neue Zahl ohne Adresse — Radien = `hit_radius_min` (vorbestehend, Kasten +6), Zaehler 10 =
   `s7` @0x8002bd84, Gate 0x40 @0x8002ce1c, Schranke nOmodel @0x8002c048. Suche nach `deferred|tunable|interim|for now|
   faithful|plausibel|TODO` im Code-Diff (`git diff 7b59e297 -- re15_port`): kein Treffer. Env-Schalter sind nur Messhaken
   (`RE15_KOLLISION_LOG`, `RE15_KOLLISION_SETZE`), kein Abschluss ueber einen Schalter.
7. Weitere Zitate der Spur nachgelesen: Spielerkasten PSX.EXE Datei 0x64694 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` (r 450);
   Spieler-Tick `80031cb4 jalr v0` -> `80031cbc jal 0x8002b544` (Spieler aus allen Entitaeten, Gegenstueck re15_body_push_player);
   Objekt-Pool nach obj-Nummer indiziert: Obj_model_set `8004093c lbu a3,1(a2)` (obj-Nummer) / `80040944-58` a3*148 + 0x800b3f98
   (= `re15_kollision_r41_kisten_pass` sucht den Pool-Platz ueber `obj_id`); Hautabstand der Wandklemme `8003bce0 addiu t1,v0,18`.
8. Erklaert der Fix den gemessenen Befund? Ja, beide: (1) der Port-NPC-Tick hatte nach dem Zustand NUR die Klemme
   (`re15_npc_wall_clamp`), das Original hat davor aec4 + b544 — ohne sie schiebt nichts eine NPC aus einer anderen (vorher-Laeufe
   4.1/4.4: Abstand 0); (2) `re15_prop_push_tick` fuehrte den Durchlauf @0x8002c03c-c0a8 nicht (alter Kopfkommentar "Nicht
   portiert ... ROOM1090 ... verschiedene Baender"), also gab es keine Kiste-gegen-Kiste-Sperre (vorher-Laeufe 4.3/4.5/4.6).

## 4. Messungen an der exe (Fortsetzung, eigene Laeufe)
Werkzeuge (Arbeitsordner `skripte/`): `lauf2.sh` (je Lauf eigener Ordner, alle RE15_* des Aufrufers geloescht, exe-Kopie
unter eigenem Namen), Auswerter `npcpaar.py` (eigener NPC-Paar-/Spieler-Abstand je Bild aus RE15_STATE_LOG, Radien 450/500/300
aus den Kaesten oben, unabhaengig von `probe_r41_kollision_abstand`) und `kisten.py` (Kisten-Ueberlappung aus
RE15_OBJFALL_LOG, Kriterium = FUN_8002cdf4 inklusive Grenze). Zwei exe:
- **nach** = `re15_pc_h41abn0.exe` (Stand 4616305a, sha1 3da1c0f4...),
- **vor** = `re15_pc_h41abn0_vorher.exe` = Kopie der Hauptbaum-exe (master fa883caa-Code, gebaut 09:57:15, sha1 13f9cba5...;
  `git diff 7b59e297 master -- re15_port` ist leer, d.h. master-Code = Zweig-Code ohne Spur H; die exe enthaelt
  `wasser20b0` (Spur A) und NICHT `RE15_KOLLISION_LOG`, per `grep -c` geprueft).

### 4.1 Punkt 1, Leon, ROOM3070 Eskorte, ECHTER Tuerweg (`laeufe/p1_3070_{vor,nach}`)
Befehl: `skripte/p1_3070.sh <vor|nach>` = `RE15_SET_FLAG=4:215,4:236,3:180` (Story-Stand nach G-Birkin/Zombies, (3,180)
gesetzt von ROOM30E0 @0x00E66) + `RE15_DEBUG_JUMP=30E0@250` + `RE15_PLAYER_POS=-25660,-22130,3072` (Ankunftsstelle der
3070-Tuer in 30E0), Wegplan `plaene/p1_3070.txt`: Aktionstaste an Tuer 30E0 slot 1 (@0x00A12 -> ROOM3070 (-11840,-18140))
-> `[room] PC loaded room3070.rdt` -> sub00 @0x0343C/@0x03450 setzt Ada (0x42) + Marvin (0x40) als Eskorte -> Leon steht 450,
geht (-9700,-17900), steht, geht (-13300,-17000), steht, zurueck, steht; `RE15_EXIT_AT=4600#3070` (debug.log
`[flow] EXIT_AT: Bild 4600 in Raum 3070 erreicht`), `RE15_FRAMEDUMP=300-4500/150`.

| exe | Paar Ada/Marvin (Summe 900): min | Bilder < 850 | Bilder < 900 | Ende F4600 | Spieler/Ada min |
|---|---|---|---|---|---|
| vor | **0** (F1875) | **4012 / 4600** | 4013 | **0** | 335 |
| nach | **882** (F1408) | **0** | 47 | 1034 | 675 |

`npcpaar.py`-Zeilen woertlich: vor `Seg 2 Paar slot1(0x42)/slot2(0x40) Summe 900: Paarbilder 4600 (F1..F4600), MIN 0 bei
F1875, <Summe-50: 4012, <Summe: 4013, Ende 0`; nach `... MIN 882 bei F1408, <Summe-50: 0, <Summe: 47, Ende 1034`.
`kollision.txt` (nach): 1451 Zeilen `NPC-SCHUB`, davon 643 `slot=2 typ=40 von=1 typ=42` (Marvin aus Ada, b544).
Bildbogen `bild_p1_3070_vor_nach.png` (F1950/F2400/F3000/F4500, RE15_WINDOW_SCALE=1): **vor** stehen Ada und Marvin als EINE
Figur ineinander (Nutzer-Befund am echten Weg reproduziert), **nach** stehen sie getrennt nebeneinander. Die 47 Bilder
zwischen 850 und 900 sind die Wandklemme nach dem Schub (Reihenfolge b544 @0x8011cc44 vor b0a4 @0x8011cc64, s. RE-Gate 1).
Spieler/Ada min 675 (F1755) nach: Ada steht zwischen Leon und Marvin — `kollision.txt` F1755 `von=SPIELER vor=(-10204,-17278)
nach=(-10128,-17078)` und danach `von=2 typ=40 ... nach=(-10224,-17285)`: der spaetere b544-Schub (Marvin) gewinnt gegen den
aec4-Schub (Spieler), genau die Original-Reihenfolge @0x8011cc3c -> @0x8011cc44; kein Befund.

### 4.2 Punkt 1, Leon, STAGE2 ROOM2000 (Ada + Marvin gehen dieselbe Strecke), ECHTER Tuerweg (`laeufe/p1_2000_{vor,nach}`)
Warum dieser Raum: der Nutzer spielt gerade STAGE2; im Zensus der Spur stehen Ada + Marvin in 2000 gemeinsam, gemessen hat
die Spur dort nicht. ROOM2000 sub00 @0x01798 Ck(4,244)=1 -> sub02: Ada (slot 0) und Marvin (slot 1) laufen per Plc_dest
nacheinander dieselbe Strecke (-8350,-26100) -> (-26100,-26100) -> (-26100,-11550) (sub03 @0x01A80-0x01AC4 fuer Ada,
sub02 @0x01A3A-0x01A60 fuer Marvin nach `Sleep 120` @0x01A36). (4,244) setzt ROOM11B0 sub06 @0x01480.
Befehl `skripte/p1_2000.sh <vor|nach>`: `RE15_SET_FLAG=4:244`, `RE15_DEBUG_JUMP=1260@250`, `RE15_PLAYER_POS=-8750,-6500,1024,1`,
Wegplan `plaene/p1_2000.txt` (GEHE vor Tuer 1260 slot 1 rect (-9700,-9350,1900,1900) @0x0114A, Aktionstaste ->
`[room] PC loaded room2000.rdt`, dann 45x Aktionstaste fuer die Szenentexte), `RE15_EXIT_AT=4200#2000`.
Ergebnis (vor = nach, bitgleich in allen Abstandswerten): Ada/Marvin **min 1572** (F931), 0 Bilder < 900; Szenenende identisch
(beide auf den Parkplatz Pos_set (6900,3400)/(2400,3400) ab **F1926**, Spieler steuerbar `pm=0` ab **F2010**, vor und nach);
`kollision.txt` nach: **0** `NPC-SCHUB`-Zeilen. => In 2000 kommen sich die beiden nicht nahe; der Schub aendert die STAGE2-
Szene nicht (kein Haenger an den gemeinsamen Wegpunkten). Kein Befund-Ort, aber Nebenwirkungs-Pruefung bestanden.

### 4.3 Punkt 2, Leon, ROOM2050, EIGENES Szenario (andere Kiste, andere Richtung als der Riegel) (`laeufe/k1_{vor,nach}`)
Befehl `skripte/k.sh k1 <vor|nach>`: `RE15_DEBUG_JUMP=2050@250`, `RE15_PLAYER_POS=4248,2850,1024,0` (Becken, Band 0, suedlich
von obj 3), Wegplan `plaene/k1.txt` (echte Pad-Bits ueber den Wegplan-Treiber): `GEGNERWEG` (die zwei Spinnen 0x25 weg),
obj 3 (4248,0,1400) nach Norden bis z <= 0, dann um die Kiste herum und obj 3 nach WESTEN gegen obj 2 (-1256,0,-64)
(`SCHIEB 2048 X <= -3000`), `RE15_PUSH_LOG`/`RE15_OBJFALL_LOG`/`RE15_KOLLISION_LOG`, Auswerter `kisten.py`.
(Erster Versuch `laeufe/k1a_zelle_*` direkt nach Westen bei z 1400: in BEIDEN exe von der Zelle `r0[9]: x -350..2100 z
2250..4250` bei x 3030 gesperrt — `TREFFER-ZELLE`, keine Kistenberuehrung; deshalb erst nach Norden.)

| exe | Zeilen mit Kisten-Ueberlappung | tiefste Eindringung | SCHUB-GESPERRT / OBJEKT-GESPERRT | Endlage obj 3 / obj 2 |
|---|---|---|---|---|
| vor | **142** | **1751** (F570: obj 3 (-1321,0,-15) IN obj 2 (-1321,0,-64), \|dX\| 0, \|dZ\| 49) | 0 / — | **(-3030,0,-15) / (-3030,0,-64)** (beide zusammen weitergeschoben) |
| nach | **0** | — | **1 / 1**: `F480 OBJEKT-GESPERRT p=4 obj=3 ziel=(536,-15) gegen p=3 obj=2 lage=(-1256,0,-64) dX=1792 dZ=49 sumX=1800` | **(557,0,-15) / (-1256,0,-64)**, \|dX\| 1813 > 1800 |

=> Nutzer-Befund im eigenen Szenario vorher reproduziert (Kiste ganz IN der anderen und beide zusammen weiter), nachher
sperrt der Objekt-Pass an der Grenze |dX| <= 1800 (FUN_8002cdf4 @0x8002ce80-8c, inklusive) und die Kiste bleibt 13 Einheiten
vor der Beruehrung stehen.

### 4.4 Punkt 1, ELZA, ROOM3001 Eskorte John + Sherry, ECHTER Tuerweg (`laeufe/p1_3001_{vor,nach}`)
Befehl `skripte/p1_3001.sh <vor|nach>`: `RE15_PSELECT_AUTO_SWITCH=1` (debug.log `[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)`), `RE15_SET_FLAG=4:198,4:199` ((4,199) setzt ROOM3000/3001 @0x01672/@0x016AA), `RE15_DEBUG_JUMP=3011@250`,
`RE15_PLAYER_POS=-23168,11769,2048` (Ankunftsstelle der 3001-Tuer in 3011), Wegplan `plaene/p1_3001.txt`: Aktionstaste an Tuer
3011 slot 0 (rect (-25000,10620,1600,1820) @0x01FD2) -> `[room] PC loaded room3001.rdt` -> sub00 @0x015A2/@0x015B6 John (0x49) +
Sherry (0x4b) als Eskorte; Elza steht 450, geht in den Gang (-11250,-13378), nach Westen (-18000,-13378), zurueck, steht;
`RE15_EXIT_AT=4000#3001`.

| exe | Paar John/Sherry (Summe 750): min | Bilder < 700 | Bilder < 750 | Ende F4000 | Spieler/Sherry min |
|---|---|---|---|---|---|
| vor | **0** (F1023) | **3917 / 4000** | 3917 | **196** | 59 |
| nach | **737** (F1405) | **0** | 369 | 751 | 629 |

`kollision.txt` nach: 4180 `NPC-SCHUB`-Zeilen. Rest bis zur Summe (min 737 = 13 unter 750) = Wandklemme nach dem Schub; den
Hautabstand 18 der Klemme selbst nachgelesen: `8003bce0 addiu t1,v0,18` / `8003bce8 addiu a0,a0,-18`. => Nutzer-Befund fuer
Elza am echten Weg reproduziert (vor: beide stehen ineinander) und behoben (nach: getrennt).
Nebenbeobachtung (vorbestehend, kein Punkt der Spur): Spieler/John direkt nach dem Eintritt F55-110 53 Bilder < 850 in BEIDEN exe
(John an der Ostwand x -6140 geklemmt) — gleiche Zahl vor/nach, nicht NPC-gegen-NPC.
Bildbeleg zu 4.3 (`laeufe/k1cut13_{vor,nach}`, derselbe Lauf mit `RE15_FORCE_CUT=13` = Beckenkamera, nur Bild; Lagen und
F480-Sperre bitgleich zu k1_*): Bildbogen `bild_k1cut13_vor_nach.png` (F450/F550/F650/F1000, RE15_WINDOW_SCALE=1): **vor**
schiebt sich die vordere Kiste ab F550 in die hintere, ab F650 ist nur noch EINE Kiste zu sehen (beide an derselben Stelle);
**nach** stehen ab F550 zwei Kisten Kante an Kante, die geschobene bleibt an der anderen haengen. (Die Kamerasuche
`laeufe/cut2050_<n>` / `cutsurvey_[ab].png`: Cut 11/12/13 zeigen das Becken.)

### 4.5 Punkt 2, Leon, ROOM2050, zweite Achse (Z) und dritte Kistenpaarung (`laeufe/k2_{vor,nach}`)
Befehl `skripte/k.sh k2 <vor|nach>`: Spawn (600,-5552) Blick Ost, Band 0; Wegplan `plaene/k2.txt`: obj 1 (2052,0,-5552) nach
OSTEN auf x 2868 (dann \|dX\| zu obj 3 = 1380 < 1800), Weg ueber (1350,-3000)/(5700,-3000)/(5700,2850) suedlich vor obj 3, obj 3
nach NORDEN gegen obj 1 (`SCHIEB 1024 Z <= -6000`).

| exe | Zeilen mit Ueberlappung | tiefste Eindringung | SCHUB-GESPERRT / OBJEKT-GESPERRT | Lage obj 3 nach dem Nordschub |
|---|---|---|---|---|
| vor | **130** | **420** (F690: obj 1 (2868,0,-5552) / obj 3 (4248,0,-4386), \|dX\| 1380 \|dZ\| 1166) | 0 / — | bis **z -6041** (Bedingung `obj.z = -6041` erfuellt), obj 1 mitgeschoben |
| nach | **0** | — | 2 / **1**: `F654 OBJEKT-GESPERRT p=4 obj=3 ziel=(4248,-3753) gegen p=2 obj=1 lage=(2868,0,-5552) dX=1380 dZ=1799` | **z -3706** (\|dZ\| 1846 > 1800) |

(Die zweite Sperre nach F1429 ist eine Zellsperre an obj 1, das Leon nach dem Abgleiten an obj 3 selbst nach Norden schob — keine
Kistenberuehrung, 0 Ueberlappungszeilen.) => auch die Z-Pruefung (@0x8002ce94-a0) sperrt, Grenze inklusive (dZ 1799 gesperrt).

### 4.6 Punkt 2, ELZA, ROOM2051 (`laeufe/k1_elza_{vor,nach}`)
Befehl `skripte/k.sh k1_elza nach` (= k1 mit `RE15_PSELECT_AUTO_SWITCH=1`, `RE15_DEBUG_JUMP=2051@250`): debug.log `[pl] Spieler-Familie
PL04 (character=4, Elza-Bit=1)`, `[room] PC loaded room2051.rdt`; `F486 room=2051 OBJEKT-GESPERRT p=4 obj=3 ziel=(521,-45) gegen p=3
obj=2 lage=(-1256,0,-64) dX=1777 dZ=19`, obj 3 bleibt bei x 552 (\|dX\| 1808), **0 Ueberlappungszeilen**.
**vor** (`k.sh k1_elza vor`, ebenfalls PL04 + room2051): **141** Ueberlappungszeilen, tiefste Eindringung **1781** (F585:
obj 3 (-1396,0,-45) IN obj 2 (-1396,0,-64), \|dX\| 0), Ende beide bei x -3015, 0 SCHUB-GESPERRT. => Elza-Szenario wie Leon:
vorher ineinander, nachher gesperrt.

### 4.7 Punkt 2, echter Weg IN den Raum (`laeufe/tuer2000_nach`)
`skripte/tuer2050.sh nach 2000`: Sprung nach 2000 an die Leiter, Aktionstaste -> debug.log `[aot] DOOR FIRE slot=4
rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` -> `[room] PC loaded room2050.rdt`. Die Kisten stehen nach
dem Tuerweg auf denselben Records wie nach dem Sprung (objfall F15 und F300: `p2 (2052,0,-5552) b0 | p3 (-1256,0,-64) b0 | p4
(4248,0,1400) b0`, obj 6 `(-1600,-5400,-9348) b3`) — der Sprung ins Becken (`RE15_PLAYER_POS` Band 0) in 4.3/4.5/4.6 aendert also
nichts am Raumzustand; er erspart nur den Abstieg von der Ankunft (Band 3, y -5400) ins Becken (Band 0).
Abstieg ins Becken zu Fuss (`laeufe/becken_nach`, Plan `plaene/becken.txt`): nach dem Tuerweg (Ankunft (9100,-5400,4100), Band 3)
Aktionstaste in den Randzonen `AOT slot 62 typ 3 ev 0 mitte (9050,1150)` und `slot 63 typ 3 ev 1 mitte (9050,-350)` (Liste `AOTS`
im Wegplan-Log) -> Spieler bleibt auf Band 3 (objfall F240/F465/F690 `spieler=(9056,-5400,...) band=3`). Den Weg hinunter ins
Becken (vermutlich ueber den Objekt-Fall von obj 6 und Klettern) habe ich NICHT nachgefahren; die Kisten-Messungen 4.3-4.6 beginnen
deshalb mit dem Sprung-Spawn im Becken (wie die Riegel der Spur). Begruendung, warum das den Befund nicht verfaelscht: Raumzustand
nach dem Tuerweg = nach dem Sprung (4.7), und die Schubkette haengt nur an Spielerlage, Blick, Pad und den Pool-Objekten.

## 5. Gezielte Testgruppen (selbst gefahren, Spurbaum, keine volle Suite)
### 5.1 unit_/prop_/probe_ (der Diff aendert den NPC-Tick aller NPC-Typen, die Schubkette aller Kisten und den Bildablauf)
- Erster Lauf `-R "^(unit_|prop_|probe_)" -j 5 --timeout 600` (`ctest_unit_alle.log`): die vier zuerst gestarteten Bash-Release-/
  Android-Riegel liefen in ihre eigenen Zeitgrenzen (`unit_r35_android_pruefkette ***Timeout 900.21 sec`,
  `unit_r38_releaseskript_fehler_umfeld/fehler_bau/ablauf ***Timeout 1200.2x sec`; die Spur hatte sie in 246/371 s gruen) — unter
  der Last mehrerer paralleler Agenten haengen deren Git-/Bash-Fixtures. Diese Riegel pruefen `release/` und die Android-Kette,
  die der Diff nicht beruehrt. Lauf abgebrochen (eigene Prozesse per PID, kein /IM), Fixtures aufgeraeumt.
- Zweiter Lauf ohne diese 10 Bash-Riegel: `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -E
  "releaseskript|android_pruefkette|android_bash_mutanten|titelversion_ablauf" -j 4 --timeout 600 --output-on-failure` ->
  **`100% tests passed, 0 tests failed out of 922`** (`Total Test time (real) = 692.26 sec`, `ctest_unit_alle2.log`; enthaelt
  unit_r41_kollision_*, unit_r41_wasser20b0_* (NPC-Wasserschritte der Spur A), unit_r36_* Kisten/Zellschub/Objfall,
  unit_r37/r38/r39 NPC-Szenen).
- Ausgeschlossen und NICHT gefahren: unit_r38_releaseskript_{version,bausteine,trocken,ablauf,fehler_bau,fehler_umfeld,cmd},
  unit_r35_android_pruefkette, unit_r35_android_bash_mutanten, unit_r39_titelversion_ablauf (kein Bezug zum Diff; Hinweis H3).

### 5.2 Fremder Riegel `test_r39_dining1141_kampf.cmake` (Runde 39 Spur J), WARTE 60 -> 240 — Behauptung nachgemessen
Die UNVERAENDERTE master-Fassung (`git show master:.../test_r39_dining1141_kampf.cmake`, WARTE 60) als Kopie im Arbeitsordner
(`dining_orig/kampf_w60_{vor,nach}.cmake`, einzige Aenderung: eigener exe-Praefix `re15_pc_h41dk{v,n}_` statt `re15_pc_r39j_k_`, damit
sich die Laeufe nicht gegenseitig die Kopie loeschen; `diff` nach Praefix-Normierung leer) gegen beide exe:
- **vor** (`w60_vor.log`, rv=0): `12 Schuesse, 11 Treffer` / `ok   NB2 (a): 2 Treffer, waehrend Elza im Griff ist (gr=1); kein
  Fehlschuss wegen des Opferzustands (0)`.
- **nach** (`w60_nach.log`, rv=1): `12 Schuesse, 10 Treffer` / `FAIL NB2 (a): 0 Treffer, waehrend Elza im Griff ist (gr=1); kein
  Fehlschuss wegen des Opferzustands (0)`; (b), (c), N4.3, Querungs-Wache gruen.
=> Die Behauptung der Spur stimmt: rot wird der alte Riegel ERST durch Spur H, und nur Kriterium (a) (Zufallsdeckung Salve/Griff-
Fenster), nicht durch einen Fehlschuss am Opferzustand. Dass John (0x49) jetzt aus den Gorillas geschoben wird, ist Original:
b544 hat keinen Typfilter (RE-Gate 2), Gorilla-Kasten r 1600 vorbestehend (`re15_damage.c`, @0x80121350). Die geaenderte Fassung
(WARTE 240) laeuft in der Integrationsgruppe 5.3 mit.

## 6. Vertrag (VERTRAG.md)
- Pfade (`git diff master...HEAD --name-only`, gegen die merge-base 7b59e297, weil master inzwischen weiter ist — ein plumpes
  `git diff master` listet die pri/-Ersetzung 919cc1ed des Orchestrators, die NICHT von der Spur stammt): ausser `analysis/
  befunde_runde41/H_*` und der Orchestrator-Synchronisation von AUFTRAG.md nur die 10 Dateien unter re15_port aus Abschnitt 1.
  Keine Datei unter release/, platform/android/, shared_assets/PSX/, pri/, synchro/, kein CMakeLists.txt (Registrierung ueber
  `tests/unit/probes/r41_kollision.cmake`). **haelt.**
- Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine belegt (grep im Code-Diff nach Bank 9, `re15_game_flag_set(9`,
  Nachrichten-Ids: nichts). **haelt.**
- Gemeinsame Dateien nur kleine Haken mit "Runde 41 Spur H": aot_common.c (1 Include + 1 Zeile + 1 Zweig + berichtigter Kopf-
  kommentar), game_step_common.c (1 Include + 1 Zeile), enemy_ai_common.c (1 Include + 1 Zeile); 8 markierte Zeilen. **Klein: ja.**
  **ABER enemy_ai_common.c ist in VERTRAG §1.3 nicht der Spur H zugeteilt** (dort A, L; seit I-E1 auch I) — Mangel V1 (formal).
  Begruendung der Spur selbst geprueft: die Original-Reihenfolge Zustand -> aec4 @0x8011cc3c -> b544 @0x8011cc44 -> Wandklemme
  @0x8011cc64 liegt IM NPC-Tick (RE-Gate 1); ein Haken in game_step_common.c nach dem Gegnerdurchlauf liefe hinter der Klemme und
  hinter allen spaeteren Ticks. Technisch zwingend; Konfliktpruefung mit Spur I (`git -C r41_zombiefresser diff master`): I setzt
  ihr Include hinter master-Zeile 60, H hinter Zeile 58 — zwei unveraenderte Zeilen dazwischen, kein Textkonflikt zu erwarten.
- Fremder Riegel `tests/integration/test_r39_dining1141_kampf.cmake` (Runde 39 Spur J) geaendert (WARTE 60 -> 240, Kriterien
  unveraendert, Kommentar "Runde 41 Spur H") — VERTRAG "nur eigene Pfade adden" -> Mangel V2 (formal). Inhaltlich nachgemessen
  und begruendet (5.2): ohne die Aenderung waere der Riegel nach Spur H rot, und zwar nur an der Zufallsdeckung (a).
- Riegel je Punkt vorhanden und messend: Punkt 1 `unit_r41_kollision_{npc,raum3070,raum3001}` + `integration_r41_kollision_
  {npc3070,npc3001}` (Abstand je Bild, Kontakt-Pflicht gegen Vakuitaet), Punkt 2 `unit_r41_kollision_kiste` + `integration_r41_
  kollision_{kiste,raetsel_leon,raetsel_elza}` (Lage statt Ziel, Grenze 1800/1801, Raetsel loesbar Leon UND Elza). Parallel-fest
  (eigener `lauf_<zufall>`-Ordner, exe-Kopie `re15_pc_r41h_<teil>_*`, nur eigene Kopien geloescht, RE15_*-Umgebung geleert),
  Linux-fest (`cmake_minimum_required` im -P-Skript, `m` gelinkt, keine .exe-/Backslash-Pfade, kein popen). Unit-Ausgaben selbst
  gelesen (`test_r41_kollision.exe npc|raum3070|raum3001|kiste`: alle PASS, z.B. `|dX| = 1800 = hx+hx -> UEBERLAPPUNG`,
  `|dX| = 1801 -> frei`, `min. Abstand 899 >= 880`, `min. Abstand 732 >= 730`). **haelt.**

### 5.3 Integrationsgruppe (laeuft; Ergebnis folgt unten in 5.4)
Auswahl = alle Integrationsriegel mit NPC-Szenen (der Schub wirkt in jedem NPC-Tick, auch in Szenen), alle mit Kisten/Objekten
(Schubkette, Spiegel +0x42) und die breiten Raumlauf-Riegel; 81 Tests:
`ctest --test-dir re15_port/build -R "^integration_(r35_cut10f0|r35_cut1150|r36_szene1150|r36_szenenleon|r36_story_leon|
r36_story_elza|r37_szene11c0|r37_roy_szene1011|r37_roy_szene1021|r37_roy_leon1170|r37_sperre1050|r38_heliport1171_.*|
r39_heliport1171_.*|r38_werkstatt11b0|r38_szenenschutz|r38_kleinkram_irons|r36_elza_irons|r39_johnruf1041_a|r39_johnruf1041_b|
r39_funk10f1_.*|r39_parkplatz11c0_.*|r38_dining1141_.*|r39_dining1141_.*|r39_stimmen_.*|r39_toene_szene.*|r38_elzatoene_.*|
r30_irons_tisch_.*|r30_cut_blitz|r37_elzaems_.*|r41_wasser20b0.*|r39_gegner_1141|r36_dachkiste|r36_klettern|r37_kiste.*|
r37_itemband.*|r38_wasser.*|room_transition|cross_stage|door_traversal|room_probe_sweep)$" -j 3 --timeout 2700
--output-on-failure` (`ctest_integ.log`).
Nicht gefahren: die ROOM1260-Parallelszene (Ada + Marvin, Switch case 7 @0x0124E, sub04/sub05) — mein Tuerweg 11B0 slot 5 kam
nicht an die Tuer (Spieler blieb bei (-62,-18308) haengen, Laeufe `p1_1260_*` abgebrochen). Statisch: die Wegpunkte der beiden
liegen paarweise 1498/1543 auseinander, die Endlagen (-7500,-6250)/(-8900,-4600) 2164 — kein Plc_dest-Ziel innerhalb der
Radien-Summe 900 einer anderen Figur; die Spur mass dort vorher min 971.
