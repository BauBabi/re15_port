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
