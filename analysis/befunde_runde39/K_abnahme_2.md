# Runde 39 — Spur K "heliport1171": ABNAHME 2 (unabhaengig, 2026-10-18/19, nach Nachbesserung 2)

Baum `.claude/worktrees/r39_heliport1171`, Zweig `r39/heliport1171`, HEAD 9ca95a72 (letzter Code-Commit 21bb3788, Riegel
60cc4367/480d8e4a). Merge-Basis mit master 22abaed9; die Spur-Dateien = `git diff master...HEAD` (24 Dateien; der
Zwei-Punkte-Diff `git diff master` zeigt zusaetzlich die seither in master gekommenen Fremd-Aenderungen als "Loeschungen").
Abnahme-Laeufe im Scratchpad `K39abn2/`, eigene exe-Kopien `re15_pc_k39abn2_*.exe`, kein taskkill.

**Ergebnis: BESTANDEN.** Nachbesserung 2 behebt beide Maengel der Abnahme 1 und ist belegt: die HOVER-Phase 0 der
Ereignis-Kraehen beendet den Takt ohne Posieren/Bewegung (`j 0x801152cc` @0x801151d8; gemessen Zyklus 11, Halt-Takte
0/10 veraendert), die SPIN-Phase 2 dreht nicht mehr (Gier +60 @0x80115460-6c nur im Phase-1-Rumpf; gemessen 0/109,
0/123), dazu der eigene Befund N1 (Wand-Pass der Kraehen-Wurzel nach jedem Zustand, @0x80112204; Westwand-Kraehe im
INIT-Takt nach (-8842,-7623)). Johns Heran-Winken und Elzas Ablauf sind unveraendert. Alle Zitate selbst disassembliert,
Riegel am alten Motor selbst rot, gezielte Tests gruen. Punkt 1: **erfuellt**.

## 0. Bau
* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`
  / `=== LOCAL-BUILD-OK (build)` (Log `K39abn2_build.log`, EXIT 0). exe `re15_port/build/platform/pc/re15_pc.exe` 23:10,
  7 619 582 B, sha256 `9729d461...75cc` = die vom Bau-Agenten gemessene exe (Dossier 13.5); `git status --porcelain` leer.
* Spur-Riegel Unit `ctest --test-dir re15_port/build -R "^unit_r39_heliport1171_|^unit_r38_heliport1171_" --timeout 600
  -j 4 -V` -> **`100% tests passed, 0 tests failed out of 19`** (Log `K39abn2/ctest_unit_spur.log`). Messzeilen NB2:
  - flug: `Kraehe Slot 2: HOVER-Halt-Takte 10 (Pose/Lage veraendert 0, danach Clip 4 Bild 0 10), Abstand 11: 9 von 9; Griff
    Bild 105; SPIN Phase 2 109 Takte (Gier veraendert 0), Laeufe 41 Takte 2 (andere 0), Phase 0/1 99 Takte (nicht +60 0)`;
    Slot 3 `... Griff Bild 116; SPIN Phase 2 123 Takte (Gier veraendert 0), Laeufe 41 Takte 3 ...`.
  - wand: `Kraehe Slot 2: Spawn (-9970,-5680) Zustand 0 -> nach dem INIT-Takt (-9970,-5680) Zustand 4, Band 5` /
    `Kraehe Slot 3: Spawn (-9700,-7680) Zustand 0 -> nach dem INIT-Takt (-8842,-7623) Zustand 4, Band 4`.
* Spur-Riegel Integration `ctest ... -R "^integration_r39_heliport1171_|^integration_r38_heliport1171_" --timeout 1500 -j 3 -V`
  -> **`100% tests passed, 0 tests failed out of 5`** (115,5 s; Log `K39abn2/ctest_integ_spur.log`). Messzeile NB2 (weg):
  `Kraehe 2: erste Lage (-9970,-5680), HOVER-Halt-Takte 10 (veraendert 0), Abstaende 9 (nicht 11: ), SPIN-Phase-2-Takte
  109 (gedreht 0); Kraehe 3: erste Lage (-8842,-7623), HOVER-Halt-Takte 10 (veraendert 0), Abstaende 9 (nicht 11: ),
  SPIN-Phase-2-Takte 123 (gedreht 0)`; Johns Abschnitt: `Heran-Winken (Clip 2/96) 127 Bilder, Kletter-Clip (2/50 nach
  Plc_motion) 0, RBJ-Clip 0 (20) 150, Bank-0-Clip 0 (34) 0`.

## 1. Code-Diff (gelesen)
`git diff master...HEAD -- re15_port` vollstaendig gelesen; NB1-Teile unveraendert seit Abnahme 1 (dort geprueft). NB2 =
`git diff 97b42ad2..HEAD -- re15_port` (Motor 2 Dateien + Header, Tests 3 Dateien):

| Stelle | Inhalt NB2 | eigene Pruefung |
|---|---|---|
| enemy_ai_common.c `re15_crow_flight2_sub` Fall 1 HOVER | `break;` am Ende des Phase-0-Blocks (nach `sub_state_2 = 1`) | `break` verlaesst den `switch (e->sub_state_1)`; danach steht in der Funktion nichts mehr -> im Phase-0-Takt kein `re15_crow_anim`, keine Lenkung, kein `y += vvel`, kein `re15_crow_advance`, keine Kontaktabfrage. Der Aufrufer (Zustand 4) fuehrt danach nur den Koerper-Schub aus (Original: Wurzel-Nachlauf @0x801121d4, ebenfalls nach jedem Zustand). |
| ebd. Fall 3 SPIN | `e->rot_y += 60` aus der Zeile vor der Phasen-Unterscheidung in den Zweig `sub_state_2 == 1` verschoben | Phase 0 setzt `sub_state_2 = 1` und faellt im selben Takt in den Zweig (= Original 0x80115450 -> 0x80115454); Phase 2 (`else`) dreht nicht. Zeitgeber unveraendert. |
| ebd. `re15_crow_ai_tick` | vor dem Zustands-`switch` Zustand + x/z gemerkt; nach dem `switch` fuer Zustand != 1 `re15_r39k_kraehe_wandpass(e, alt_x, alt_z)` | Pause-Tor (`return` vor dem Merken) wie Original @0x80112120 -> @0x8011221c; RE2-Hirn-Pfad (`return` am Anfang) unberuehrt; Zustand 1 behaelt seinen eigenen Pass. Band = `e->floor`, gesetzt VOR dem Zustand (`-(y/1800)`). |
| heliport1171_r39.c `re15_r39k_kraehe_wandpass` | `re15_collision_constrain_contact_band(&g_room_rdt, alt_x, alt_z, &x, &z, 200, e->floor, 4u, NULL, NULL)`; Wort0 Bit 3 (`e->flags & 0x08`) oder ohne Raum -> kein Pass, `crow_wall = 0`; sonst `crow_wall` = Rueckgabe | Resolver = vorhandener FUN_8003b0a4-Port (Band-Vergleich `band != floor>>4`, Maske `mask & u0`, Breitphase, Handler je Zelltyp, Rueckgabe "irgendeine Zelle getroffen"). |
| Tests | `test_r39_heliport1171.c` Teile `flug`/`wand`, `probes/r39_heliport1171.cmake` (Teilliste), Integrationsblock am Ende von `test_r39_heliport1171.cmake` | messend (s. §5). |

Guess-Tells in den hinzugefuegten Zeilen (`git diff master...HEAD -- re15_port | grep '^+' | grep -iE
'deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt|vermutlich|getenv'`): s. §2.1.

## 2. RE-Gate (Stichproben SELBST disassembliert)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX/BIN/STAGE1.BIN` (md5 a482d196) und
`info/Re1.5/PSX.EXE` (md5 b55fdaa5 = Auslieferungsstand, nicht die gepatchte EXE). Ghidra-Dekompilate nur als Gegenprobe.

| Behauptung (Diff/Dossier) | Selbst gefundene Instruktionen | Urteil |
|---|---|---|
| FLIGHT-2-Tabelle; HOVER Phase 0 endet mit `j 0x801152cc` @0x801151d8 (kein f314/Lenkung/pos_advance/Kontakt) | `table 0x80121220`: [0] 0x80114fb8, [1] 0x80115130, [2] 0x801152e0, [3] 0x801153ac, [4] 0x8011553c. `80115144 lbu a0,6(v1)` / `8011514c beq a0,zero,0x80115164` / `80115154 beq a0,v0(=1),0x801151e0`; Phase 0: `8011516c jal 0x80115d94` (a0 = 4) + Delay `80115170 sh v0(=0xb4),140(v1)` / `8011518c sh v0,484(v1)` (+0x1d4 & 0x3f) / `801151a4 slti v0,v0,2000` -> `801151b8 subu` / `801151d4 addiu v0,v0,1` / **`801151d8 j 0x801152cc`** + Delay `801151dc sb v0,6(v1)`; Phase 1 erst `801151e8 jal 0x8001f314`, `80115238 jal 0x8001a8f8` (Delay `8011523c sw v0,56(v1)` = y += +0x1e4), `80115240 jal 0x800245d8`, `80115254 lbu v0,464(a0)`; Epilog `801152cc lw ra` ... `801152d8 jr ra` | stimmt |
| SPIN: Gier +60 nur im Phase-1-Rumpf @0x80115460-6c; Phase 2 @0x801154d0-e8 -> Schwanz @0x801154ec | `801153c4 beq v1,v0(=1),0x80115454` / `801153d4 beq v1,zero,0x801153f8` / `801153e8 beq v1,v0(=2),0x801154d0`; Phase 0 `801153f8 jal 0x800453d0` (a0 = 0), `80115410 jal 0x80115d94` (a0 = 4), `80115424 sh v0(=-30),484`, `80115434 sb v0(=0x20),469`, `80115450 sb v0,6(v1)` und faellt in `80115454`; **`80115460 lhu v0,106(v1)` / `80115468 addiu v0,v0,60` / `8011546c sh v0,106(v1)`**; Zeitgeber `80115484 addiu v0,a0,255` / `80115488 bne a0,zero,0x801154ec` / `801154c8 j 0x801154ec` + `801154cc sb v0(=0x28),469`; **Phase 2 `801154d0 lbu v1,469(a0)` / `801154dc bne v1,zero,0x801154ec` / `801154e4 jal 0x80115d74` (a0 = 1)** — keine Gier; Schwanz `80115508 jal 0x800245d8` (Delay `sw v0,56(v1)`), `80115524 jal 0x8001f314` | stimmt |
| N1: Wurzel 0x80112020 fuehrt nach dem Zustand (`jalr` @0x801121b4) ohne Sprung den Wand-Pass @0x80112204 aus (Radius @0x80112200, Maske 4 @0x801121fc, +0x1d1 @0x80112218) | `801121a4 addiu at,at,4380` (0x8012111c) / `801121b4 jalr v0` / `801121c4 jal 0x8002b498` / `801121d4 jal 0x8002aec4` (a0 = 0x800aca54) / `801121e4 jal 0x8002b544` + Delay `sb v0,464(v1)` / `801121f8 lw v0,120(a0)` / **`801121fc ori a2,zero,0x4` / `80112200 lhu a1,6(v0)` / `80112204 jal 0x8003b0a4` + Delay `addiu a0,a0,52`** / `80112218 sb v0,465(v1)`; zwischen 0x801121b4 und 0x80112218 kein Sprung. Einziger Sprung daran vorbei: Pause-Tor `80112120 beq v0,zero,0x8011221c`. Zustandstabelle `table 0x8012111c`: [0] 0x8011224c, [1] 0x80112420, [2]/[5]/[6] 0x80114e4c, [3] 0x801146d0, [4] 0x80114e54, [7] 0x801157e8; INIT endet `8011240c sw v0(=4),4(v1)` / `80112418 jr ra`, FLIGHT-2 `80114f9c jalr v0` / `80114fb0 jr ra` | stimmt |
| Radius 200 = Kasten[3] | INIT `80112324 lw v0,4360(v0)` (0x80121108) / `8011232c sw v0,120(v1)`; `read 0x80121108` = 0x801210fc; `read 0x801210fc 6 --w 2` = [0, 0, 0, **200**, 180, 200] (Kasten[3] = Kasten[5] -> kein Ellipsen-Zweig in 0x8003b0a4) | stimmt |
| Band = +0x82, VOR dem Zustand geschrieben (@0x80112154), Port `e->floor = -(y/1800)` | `80112134 lw v0,56(a0)` (y) / `80112104 lui v1,0x91a2` + `80112138 ori v1,v1,0xb3c5` / `8011213c mult` / `80112140 mfhi` / `80112144 addu v1,v1,v0` / `80112148 sra v1,v1,10` / `8011214c sra v0,v0,31` / `80112150 subu v0,v0,v1` / **`80112154 sb v0,130(a0)`** = Vorzeichen - floor(y/1800) = -(y/1800) mit Abschneiden gegen 0 = C-Division des Ports (y = -8000 -> 4, y = -10000 -> 5) | stimmt |
| FUN_8003b0a4: Wort0 Bit 3 -> kein Pass, Rueckgabe 0 (@0x8003b134-44); Band-Vergleich mit +0x82 | `8003b10c lw v0,0(a0)` / `8003b110 addiu v1,zero,-49` / `8003b118 sw` (Bits 4/5 loeschen) / `8003b12c lw v0,0(v1)` / **`8003b134 andi v0,v0,0x8` / `8003b138 beq v0,zero,0x8003b148` / `8003b140 j 0x8003b524` + `addu v0,zero,zero`**; Zellschleife `8003b234 lbu v1,130(a3)` (+0x82) gegen `8003b230-38` (Zelle+10 << 16) >> 28 / `8003b23c bne`; Maske `8003b248 lbu v1,24(sp)` (a2) `& 8003b250 sra v0,v0,24` (Zelle+8 hi) | stimmt |
| Alte Lage fuer den Rechteck-Schub = +0x40/+0x44 = Lage VOR dem Logik-Takt (Port: Lage vor dem Zustand) | Zeichenschleife nach der Logik: `8001d108 jal 0x8001e8c8` / **`8001d11c lhu v1,52(v0)` / `8001d124 sh v1,64(v0)`** / `8001d134 lhu v1,56(v0)` / `8001d13c sh v1,66(v0)` / **`8001d14c lhu v1,60(v0)` / `8001d154 sh v1,68(v0)`** (je aktivem Eintrag) — +0x40/+0x44 = Lage am Ende des Vorbilds; Sce_em_set `8004217c sw v0,52(s0)` / `800421a0 sh v0,64(s0)` / `800421b8 sh v0,68(s0)` (Spawn-Lage = alte Lage im INIT-Takt). Der Vor-Pass der Wurzel (0x80112020-0x801121b0) schreibt x/z nicht (nur +0x1d4, +0x82, +0x1ba). | stimmt — "Lage vor dem Zustand" = +0x40/+0x44, solange zwischen Zeichenschleife und Kraehen-Takt niemand x/z der Kraehe verschiebt |
| Bit 3 der Ereignis-Kraehen bleibt 0 (Pass laeuft) | Ghidra-Gegenprobe: `|= 8` nur in `STAGE1_full/FUN_8011553c.c` (DIVE-ARM, Zeile 43); die Todes-Lanes `FUN_80114738.c`/`FUN_80114ba4.c`/`FUN_801149c4.c` setzen nur `|= 2` (Koerper-Schub aus), `|= 0x4a` gilt den Bone-Teilen. Der Port-FLIGHT-2 betritt Sub 4 nie (kein `sub_state_1 = 4` im Kraehen-Code) | stimmt fuer ROOM1171 |

### 2.1 Guess-Tells / Konstanten
`git diff master...HEAD -- re15_port | grep '^+' | grep -iE 'deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt|vermutlich|getenv'`:
**0 Treffer** (auch 0 fuer `best-effort|annaeher|naeherung` im NB2-Diff; kein neuer Env-Schalter). Neue verhaltensrelevante
Konstanten NB2: `break` (@0x801151d8), Gier +60 (@0x80115468) jetzt nur Phase 1, Radius 200 (@0x80112200 + Kasten
@0x801210fc), Maske 4 (@0x801121fc), Band +0x82 (@0x80112154), Bit 3 (@0x8003b134) — alle mit Adresse, alle selbst
nachgeprueft (Tabelle oben).

## 3. Messung Nutzer-Punkt (selbst, an der exe)

Wortlaut: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
VERTRAG 0 (Spur K): "John 1171 "Elza hurry up": Heran-Winken statt Klettern; uebrige Szenen-Animationen 1171". Abnahme 1
liess offen: M1 HOVER-Phase 0, M2 SPIN-Phase 2 der Ereignis-Kraehen.

### 3.1 ECHTER WEG am Endstand (Nutzerweg 1131 -> 1141 -> Dachtuer -> 1171 -> Tor -> Szene -> 10B1)
Befehl = Abnahme 1 (`K39abn2/lauf.sh` = Kopie von `K39abn1/lauf.sh`, Wegplan `K39abn0/wegplan_1141_1171.txt` =
`integration_r38_dining1141` Teil D bis zur Dachtuer, md5 557b137c): `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_MSG_LOG=1 RE15_EVT_TRACE=1
RE15_STATE_LOG=state.txt RE15_ANIM_TRACE=anim.txt RE15_WEGPLAN=wegplan.txt "RE15_EXIT_AT=60#10b1"
RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 RE15_GIVE=0x43:1 RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0
RE15_R38F_GORILLAS_TOT=600 RE15_R38F_GORILLAS_STARR=1 RE15_SCD_TRACE=1 "RE15_FIRE_AOT=5@60#1171"
"RE15_FRAMEDUMP=60-1150/1:fd_"`, exe-Kopie `re15_pc_k39abn2_A.exe` (sha256 9729d461... = build/platform/pc/re15_pc.exe
23:10), beschleunigter Renderer (kein SOFTWARE_RENDER). Lauf `K39abn2/echtA`, 5:09 min, EXIT=0. debug.log: `DOOR FIRE
slot=0 rect=(-300,-13900,...)` (1131 -> 1141), `DOOR FIRE slot=1 rect=(-7250,-250,...) target_cut=9` (1141 -> 1171),
`DOOR FIRE slot=5 rect=(-11065,-27850,hw=875,hh=600)` (Tor) -> `F61 room=1171 Evt_exec sub=2` -> `[msg] room=1171 id=0`
(F519, "John: Elza, hurry up!" laut `rdt_msgdump.py` @0x17AE) / `id=1` (F760) / `id=3` (F1070) -> `PC loaded room10b1.rdt`;
1091 Framedumps F60-F1150. Vergleich: `K39abn1/echtA` (Abnahme 1, gleicher Befehl, Motor 86956b9c = vor NB2).
Auswertung mit eigenem Skript `K39abn2_tools/ana.py` (Abschnitte an Bildnummer-Ruecksprung, gerendert = letzte
Pose-Zeile je Bild/Aktor; state.txt = Zustand NACH dem Takt; ROOM1171 = Abschnitt 4 beider Dateien).

**John (Aktor 1) — Kernbefund.** Lauf-Folge (mo/Clip/fc, gerendert):
`F61-F295 Loco 2/50 Bild 0 (235)`, `F296-F335 0/20 0..19+Halt`, `F336-F355 0/20 rueckw.`, `F356-F518 6/10 0..9+Halt`,
`F519-F538 6/10 rueckw.`, **`F539-F665 mo=2 clip=2 fc=96 loco=0: 0..95, 95x31 (127 Bilder)`** (SCD-Spur `[scd] F539
slot=10 @0x14CE op=0x3F` = Plc_motion(0,2,0)), `F666-F759 5/30`, ... `F1034-F1149 2/52`. 19 Laeufe, **dieselbe Lauf-Folge**
wie `K39abn1/echtA` (dort Heran-Winken F530-F657, 128 Bilder); Unterschiede nur: INIT-Pose 235 statt 226 Bilder (Griff 9
Bilder spaeter), Heran-Winken-Halt 127 statt 128, `2/52` 38 statt 39 (je 1 Bild, Elzas kuerzerer Gang, s.u.). Kein
Bank-0-Kletter-Clip (`2/50` nach Plc_motion) im ganzen Abschnitt.
BILD `K39abn2/hurry_up_F530-F640.png` (eigene Framedumps 320x240, F530/539/546/553/560/567/574/581/590/605/620/640):
Untertitel "John: Elza, hurry up!", John dreht sich (F546-F553), hebt den Arm und winkt heran (F560-F590) — keine Hocke,
kein Einknicken; ab F605 Totale mit Elza.

**Ereignis-Kraehen (Aktoren 2/3) — M1/M2 der Abnahme 1, N1.** `ana.py kraehen` (HOVER-Phase-0-Takt = Takt F, vor dem
st 4 / ss1 1 / ss2 0 stand und nach dem ss1 1 steht; SPIN-Phase-2-Takt = vorher st 4 / ss1 3 / ss2 2):

| Groesse | Abnahme 1 (`K39abn1/echtA`, vor NB2) | jetzt (`K39abn2/echtA`) | Original (§2) |
|---|---|---|---|
| Kraehe 2 gerendert F64-F92 | `67:4/0 68:4/0 69:4/1 ... 77:4/9 78:4/0 ... 87:4/9 88:4/0` | `67:4/0 68:4/0 69:4/0 70:4/1 ... 78:4/9 79:4/9 80:4/0 ... 89:4/9 90:4/9 91:4/0` | F68 Halt (Phase 0, kein f314), F69 Bild 0, ..., F78 9, F79 Halt 9, F80 0 = Zyklus 11 |
| HOVER-Phase-0-Takte vor dem Griff, Abstaende | Kraehe 2: 10, alle 10; Kraehe 3: 10, alle 10 | Kraehe 2: 10 (F68, 79, ..., 167), **9 x 11**; Kraehe 3: 10 (F77, 88, ..., 176), **9 x 11** | 11 |
| Phase-0-Takte mit Bild- ODER Lage-/Gier-Aenderung | 10/10 und 10/10 (z.B. Kraehe 2 F68 `(-9813,-5651,r3980) -> (-9639,-5605,r3930)`) | **0/10 und 0/10** (F68 bleibt `(-9813,-5651,r3980)`); Bild im Folgetakt 10 x Clip 4 Bild 0 | Phase 0 nur Einrichten (@0x801151d8) |
| Griff (erstes ss1 = 2) | F161 / F172 | F170 / F178 | folgt aus dem Halt |
| SPIN-Phase-2-Takte (Gier geaendert) | Kraehe 2 109 (108), Kraehe 3 123 (123) | Kraehe 2 **109 (0)**, Laeufe 41/41/27 (Tod F438); Kraehe 3 **123 (0)**, Laeufe 41/41/41 | Phase 2 dreht nicht, 41 Takte (Zeitgeber 40..0) |
| SPIN-Phase-0/1-Takte nicht +60 | 0 / 1 (Todestakt) | 0 von 99 / 1 von 132 (F485 = Todestakt: Tod-Promotion im Vor-Pass, kein SPIN) | +60 je Takt, 33 Takte je Eintritt |
| erste Lage Kraehe 3 (state.txt F61, nach dem INIT-Takt) | `(-9700,-7680,r0)` | **`(-8842,-7623,r0)`**; Kraehe 2 unveraendert `(-9970,-5680,r4000)` | Wand-Pass nach INIT (N1) |

BILD `K39abn2/kraehen_tod_F440-F605.png` (eigene Framedumps, 2x): F470 eine, F605 beide Kraehen-Leichen liegen auf dem
Hubschrauberlandeplatz (Boden, nicht in Wand/Gelaender, nicht schwebend); Elza steht weiter an ihrem Platz.

Die zwei bzw. drei HOVER-Folgetakte NACH dem Griff (F305/F379/F453), in denen das Skript-Bit 0x1d die Kraehe im Verteiler
sofort wieder in SPIN schickt (state.txt `ss1 1 -> 3`), sind keine HOVER-Phase-0-Takte (Original: 0x80114f3c ->
0x80115d74(3) @0x80114f68 vor dem Tabellensprung) und drehen dort korrekt +60.

**Elza (Aktor 0).** 25 Laeufe, **dieselbe Lauf-Folge** wie `K39abn1/echtA`, 22 bitgleich (verschoben); anders nur: Lauf 6
(Clip 2/52 Warten auf den Griff) 49 statt 40 Bilder, Lauf 12 Gang (100/0/22) und Lauf 18 (105/5/30) je 1 Bild kuerzer.
Ursache selbst gemessen (state.txt PL): im alten Lauf wird Elza F438-F443 von `(1164,7435)` nach `(1179,7665)` geschoben
(die tote Kraehe 2 faellt neben sie, Kraehe-2-Tod dort bei `(650,6539)`); jetzt keine einzige Elza-Lageaenderung
F330-F520 (Kraehe 2 stirbt bei `(1198,285)` nach Geradeausflug in Phase 2). Elza-Sub-6/Schub-Folgen (NB1) unveraendert:
Clip 1 0..15 nach jeder Ankunft, Schub Clip 18 -> 17 rueckw. 9..0 -> Halt (F1007-F1017).

### 3.2 Lade-Weg mit Kollisions-Spur (N1-Wirkung ueber die ganze Szene)
Eigener Lauf `K39abn2/laden_coll`: Spielstand `probe_r36_elza_karte.exe re15_card.mcr 1171 2300 14365 1024 3:194` (wie
`integration_r39_heliport1171_laden`), exe-Kopie `re15_pc_k39abn2_L.exe`, `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_SOFTWARE_RENDER=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_EVT_TRACE=1 RE15_MSG_LOG=1
RE15_ANIM_TRACE=anim.txt RE15_STATE_LOG=state.txt "RE15_EXIT_AT=60#10b1" RE15_COLL_TRACE=1`, EXIT=0, bis ROOM10B1. Alle
`[coll]`-Zeilen (debug.log, 12): Kraehe (r=200) genau 6 — INIT-Takt `cell#37 typ=1 band=4 u0=255
rect=(-12242,-10442,3240x27648) r=200 (-9700,-7680)->(-8784,-7680)` + `cell#43 typ=3 ... (-8784,-7680)->(-8842,-7623)`,
danach 4 Kreis-Schuebe `cell#13` im ASCEND `(-8683,-7618)->(-8746,-7536)` ... `(-8323,-7373)->(-8325,-7356)`; keine
Klemmung in HOVER/SPIN/Tod/Leiche, Kraehe 2 nie. Die uebrigen 6 = Elza (r=450, Zelle #10) und ROOM10B1 (r=400).
Nachgerechnet (Port-push_rect = FUN_8003bca8 mit alter = neuer Lage): penX = (-12242+3240+200)-(-9700)+0x12 = 916,
penZ = -2980, Code 2, |penX| < |penZ| -> x = -9700+916 = -8784 — deckt sich mit der Spur und der Unit-Erwartung.

### 3.3 "Auch sonst" — die uebrigen Kraehen-Ablaeufe der Szene (Vollstaendigkeit nach NB2)
* ASCEND 0x80114fb8 selbst disassembliert (nicht von NB2 geaendert, Bau-Agent meldet "= Port"): Phase 0 `80115004 jal
  0x800453d0` (a0 = 0) / `8011500c jal 0x80115d94` (a0 = 4) / `8011502c` +0x6 = 1, faellt in Phase 1 `8011503c sh
  v0(=-120),484` / `8011504c sh v0(=0xa0),140` / `80115068` +0x6 = 2, faellt in Phase 2: `80115078 lbu v1,149(a0)` (+0x95 vor
  dem f314) / `80115080 bne v1,v0(=8)` -> `8011508c sb v0(=1),6(a0)` / `801150a4 addiu v0,v0,6` / `801150cc jal 0x8001a8f8`
  (a1 = 0x14) + Delay `sw v0,56(v1)` / `801150d4 jal 0x800245d8` / `801150f0 jal 0x8001f314`; danach in JEDEM Takt `80115104 lh
  v0,492(v0)` / `8011510c slti v0,v0,2001` -> `80115118 jal 0x80115d74` (a0 = 1). Port Fall 0 (enemy_ai_common.c
  `re15_crow_flight2_sub`): dieselbe Folge (Se(0)+Clip 4 -> vvel -120/Tempo 160 -> Bild-8-Abfrage ueber +0x95 -> vvel += 6 ->
  Lenkung 0x14 -> y += vvel -> Vorschub -> f314 -> Hoehenabfrage). **= Original.**
* HOVER (NB2), ORIENT (NB1, Abnahme 1 geprueft), SPIN (NB2), Tod/FINISH/Leiche (NB1, Abnahme 1 geprueft); DIVE-ARM
  0x8011553c wird in STAGE1 nicht betreten (Sub-Setzer 0x80115d74 aus FLIGHT-2 nur mit 3/1/2/1; Port setzt Sub 4 im
  FLIGHT-2 nie). Damit sind alle Kraehen-Zustaende, die in der Szene laufen, gegen das Original geprueft.
* Bild `K39abn2/schub_F900-F1060.png`: Elza und John schieben (F925-F1005), Kiste faellt (F1012/F1020, Skript-Threads 11/12 =
  Original-SCD sub03/sub04 im Port-SCD-VM), beide stehen an der Kante (F1060) — unauffaellig, von NB2 nicht beruehrt.

## 4. Vertrag
* Pfade (`git diff master...HEAD --name-only`, 24 Dateien): Dossier/Abnahmen/Belegbilder unter
  `analysis/befunde_runde39/K_*`, Motor `engine/src/{actor_locomotion,elza_r39,enemy_ai_common,enemy_common,
  game_step_common,heliport1171_r39,player_common,scd_room_setup,szenenleon_r36}.c`, `include/re15_heliport1171_r39.h`,
  Tests `tests/integration/test_r39_heliport1171.cmake`, `tests/unit/{probe_crow_death,test_plc_back_yaw_1090,test_r37_roy,
  test_r38_heliport1171,test_r39_heliport1171}.c`, `tests/unit/probes/r39_heliport1171.cmake`. Keine `release/`,
  `platform/android/`, `shared_assets/PSX/`, keine CMakeLists (Registrierung ueber den GLOB probes/*.cmake). OK.
* Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse / Sprachdateien: keine (`re15_game_flag_set|MSG_ID|k_port` in den
  hinzugefuegten Motor-Zeilen: 0 Treffer). OK.
* Gemeinsame Dateien (`--numstat`): NB2 beruehrt nur enemy_ai_common.c (+9/-1, drei Haken je "Runde 39 Spur K NB2":
  HOVER `break` 1 Zeile + Kommentar, SPIN-Zeile verschoben, Wurzel 1 Merkzeile + 1 Aufruf), Logik in der eigenen Datei
  heliport1171_r39.c/h. Gesamtstand wie Abnahme 1 (H1 dort): kleine kommentierte Haken in game_step_common.c,
  actor_locomotion.c, player_common.c, szenenleon_r36.c, elza_r39.c (Spur H), enemy_common.c, enemy_ai_common.c (Spuren D/E)
  — haelt mit Hinweis fuer die Integration (H1 unten).
* Neue Tests parallel- und Linux-fest: Unit-Teile im bestehenden Test-exe mit eigenem Laufordner je Teil
  (`r39_heliport1171_{flug,wand}_wd`), kein Env; Integrationsblock im bestehenden -P-Skript (Laufordner + exe-Kopie
  `re15_pc_r39k_<teil>_<id>` je Lauf, RE15_*-Umgebung geloescht, `cmake_minimum_required`, keine Backslash-Pfade). OK.

## 5. Riegel — Gegenprobe SELBST (alter Motor, neue Testquellen)
`git archive 97b42ad2 re15_port ":(exclude)re15_port/shared_assets"` nach Scratchpad `K39abn2/alt/` (Motor = Abnahme-1-Stand
86956b9c; enemy_ai_common.c = 97b42ad2 geprueft, 0 Zeilen "Spur K NB2"), darin `tests/unit/test_r39_heliport1171.c` und
`probes/r39_heliport1171.cmake` von HEAD (nur `RE15_ASSET_PSX_DIR` auf den Baum umgebogen), eigener Configure (`cmake -G
Ninja -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON`, PATH msys64 zuerst), Bau NUR `test_r39_heliport1171` (186 Schritte, BUILD=0):
* `test_r39_heliport1171.exe flug` -> **EXIT 1, `FEHLER (6 Fehler)`**: `Kraehe Slot 2: HOVER-Halt-Takte 10 (Pose/Lage
  veraendert 10, danach Clip 4 Bild 0 0), Abstand 11: 0 von 9; Griff Bild 95; SPIN Phase 2 109 Takte (Gier veraendert 108)`
  (+ M1 x2, M2 x1 FEHLER), Slot 3 dasselbe (`Gier veraendert 123`); Beispiel `Halt-Takt Bild 2: Pose 4/0 -> 4/0, Lage
  (-9813,-10514,-5651,r3980) -> (-9639,-10476,-5605,r3930)`.
* `test_r39_heliport1171.exe wand` -> **EXIT 1, `FEHLER (1 Fehler)`**: `Kraehe Slot 3: Spawn (-9700,-7680) Zustand 0 -> nach
  dem INIT-Takt (-9700,-7680)`.
* Am Endstand (§0) beide gruen. Der Integrationsblock wertet genau die Groessen aus, die sich zwischen `K39abn1/echtA` (alt)
  und `K39abn2/echtA` (neu) unterscheiden (§3.1: Halt-Takte 10/10 veraendert, Abstaende 10, Phase-2-Gier 108/123, erste
  Lage -9700,-7680) -> am alten Motor rot (Block-Logik gelesen: Halt = vorher st4/ss1 1/ss2 0 und nachher ss1 1, Pose
  mo/af + Lage + Gier gleich, Abstaende 11 vor dem Griff, Phase 2 Gier gleich, erste Lage -8842,-7623 und nie -9700,-7680).
  Riegel **vorhanden und messend**. Hinweis: `wand` rechnet das Soll mit derselben Resolver-Funktion, prueft aber
  zusaetzlich die absoluten Werte (-8842,-7623), Band 4 und Zustand 0 vor dem Takt — nicht selbstbestaetigend.

## 6. Nebenwirkungen (beide Szenarien, andere Raeume)
* **Wirkungsbereich NB2** (Code gelesen): nur `re15_crow_ai_tick`/`re15_crow_flight2_sub` = Typ 0x21 mit RE1.5-Hirn. Im
  Standard-KI-Modus (`s_flavor = RE15_AI_FLAVOR_RE2`, enemy_ai_re2_zombie.c:110) laufen die normalen Kraehen ueber das
  RE2-Hirn (Ruecksprung vor dem Vor-Pass) — unberuehrt; RE1.5 laufen dort nur die ROOM1171-Ereignis-Kraehen
  (`re15_r38i_ereigniskraehe`). Im RE1.5-/MIXED-Modus bekommen auch die normalen Kraehen (10C0/10C1/1120/1121/1170) den
  Wand-Pass nach INIT/Tod/Leiche (= Original-Wurzel). Die Kraehen-Weiche in `re15_enemy_ai_run_all` (Typ 0x21: Navigator +
  `re15_crow_ai_tick`, "NOT ground-clamped") hat keinen zweiten Klemmer -> kein Doppel-Pass.
* **Alt gegen neu, RE1.5-Kraehen-Raeume** (eigene Laeufe, Spielstand + CONTINUE wie `integration_r37_elzaems_*kraehe`,
  `RE15_AI_FLAVOR=re15`, 900 Bilder, `K39abn2_tools/kraehenraum.sh`; neu = Baum-exe-Kopie `re15_pc_k39abn2_K.exe`, alt =
  eigener Bau 97b42ad2 `K39abn2/alt/build/platform/pc/re15_pc_k39abn2_alt.exe` mit `RE15_ASSET_ROOT` auf den Baum),
  `K39abn2_tools/kraehen_diff.py` ueber state.txt: **ROOM10C0 (Leon)**: 901/901 Zeilen, Kraehen-Unterschied **0**,
  Spieler-Unterschied keiner (3 Kraehen, Zustand 1); **ROOM10C1 (Elza)**: dasselbe (0 / keiner). D.h. der INIT-Pass klemmt
  dort niemanden; die `[coll]`-Zeilen (je 200 = Ausgabegrenze, Zelle #2, Band 0, Zustand-1-Pass) und der ganze Pose-Strom
  `anim.txt` sind in alt und neu **bytegleich** (`diff`/`cmp`, beide Raeume).
  (Erster Alt-Lauf ohne Asset-Wurzel blieb auf "BOOT-BG LOAD FAILED" stehen und wurde per PID beendet — kein Befund.)
* **John/Elza ROOM1171**: §3.1 (gleiche Lauf-Folgen, Verschiebung durch den spaeteren Griff, 1-Bild-Unterschiede durch den
  weggefallenen Leichen-Schub). **Leon ROOM1170**: `integration_r37_roy_leon1170` gruen (NB2 aendert keinen Leon-/Elliot-Pfad;
  die Kraehen dort laufen im Standardmodus ueber das RE2-Hirn).

### 6.1 Gezielte Tests (keine volle Suite, VERTRAG)
* Unit/Probe, Regex des Bau-Agenten erweitert um `coll|wand|enemy_ai|gorilla|zombie|dog|hund`, `-E "^integration_"`, -j 4:
  **`100% tests passed, 0 tests failed out of 326`** (73 s; Log `K39abn2/ctest_unit_breit.log`; darin alle Kraehen-Riegel
  `unit_crow_ai`, `unit_crow_fixes`, `probe_crow_kill`, `unit_crow_death_corpse`, `unit_r35_fenster_kraehe`,
  `unit_r36_schuss_kraehe`, `unit_r36_kleidung_kraehe`, `unit_re2_crow_*`, alle `unit_r38/r39_heliport1171_*`).
* Integration (echte exe) `ctest -R "^integration_.*(crow|kraehe|1120|1121|10c0|10c1|1170|1171|fenster|szenenschutz|schuss|
  elzaems|heliport)" --timeout 1500 -j 3 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 24`** (489,7 s;
  Log `K39abn2/ctest_integ_kraehen.log`; u.a. `r37_elzaems_leon10c0kraehe`/`elza10c1kraehe` mit RE15_AI_FLAVOR=re15,
  `r37_roy_leon1170`, `r35_fenster`/`r36_elza_fenster`, `r36_schuss`/`r36_elza_schuss`, `r38_szenenschutz`,
  `r38_heliport1171_weg_re2/_weg_re15/_laden`, `r39_heliport1171_weg/_laden`, `r39_stimmen_1171`). Kein roter Fenster-Haken,
  kein Nachfahren noetig.

## 7. Urteile

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1a "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink Animation, die er im Original nicht macht." | **erfuellt** | Echter Weg 1131 -> 1141 -> Dachtuer -> 1171 -> Tor (`K39abn2/echtA`): `[msg] room=1171 id=0` "John: Elza, hurry up!" F519, Plc_motion(0,2,0) @0x14CE F539, John F539-F665 `mo=2 clip=2 fc=96` Bild 0..95 + Halt = RBJ rec1 Clip 2 (Heran-Winken), kein Bank-0-Kletter-Clip (2/50) nach einem Plc_motion; Bild `K39abn2/hurry_up_F530-F640.png` (dreht sich, hebt den Arm, winkt heran); Riegel `unit_r39_heliport1171_szene` + Integration `Heran-Winken (Clip 2/96) 127 Bilder, Kletter-Clip 0` gruen. |
| 1b "Auch sonst scheinen die Animationen hier noch nicht ganz zu passen." (VERTRAG: "uebrige Szenen-Animationen 1171") | **erfuellt** | John (K2-K4) und Elza (NB1) mit unveraenderter Lauf-Folge am echten Weg (§3.1); die in Abnahme 1 offenen Kraehen-Stellen jetzt = Original: HOVER-Phase-0-Takt ohne Bild-/Lage-/Gier-Aenderung (0/10, 0/10; vorher 10/10), Zyklus 11 (9/9 je Kraehe; vorher 10), gerendert `67:4/0 68:4/0 69:4/0 70:4/1 ... 78:4/9 79:4/9 80:4/0` = die aus den Instruktionen abgeleitete Folge; SPIN-Phase 2 ohne Gier-Drehung (0/109, 0/123; vorher 108/123), Phase 0/1 je +60; Wand-Pass der Wurzel nach jedem Zustand (N1, Kraehe 3 nach INIT `(-8842,-7623)`); ASCEND, ORIENT, Tod/Leiche geprueft (§3.3, Abnahme 1). Alle Stellen selbst disassembliert (§2), Riegel am alten Motor selbst rot (§5). |
| **Punkt 1 gesamt** | **erfuellt** | 1a und 1b erfuellt. |

Gates: **RE-Gate haelt** (11 Stellen selbst disassembliert, §2/§3.3: HOVER @0x801151d8, SPIN @0x80115460-6c/@0x801154d0-ec,
Wurzel @0x801121b4-218 + Tabelle @0x8012111c, Kasten @0x801210fc, Band @0x80112154, FUN_8003b0a4 @0x8003b134/@0x8003b234,
alte Lage @0x8001d124/@0x8001d154 + Sce_em_set @0x800421a0/b8 + Wort0 @0x80042298/@0x800422b0, ASCEND @0x80114fb8; jede
NB2-Konstante mit Adresse in Code und Commit-Message; 0 Guess-Tells; die Fixes erklaeren die gemessenen Befunde 1:1).
**Vertrag haelt** (Pfade, keine Bits/Ids/Slots/Ereignisse, kleine kommentierte Haken). **Riegel vorhanden und messend**
(flug 6 / wand 1 Fehler am alten Motor, selbst gefahren). **Gezielte Tests gruen** (Spur 19 + 5, Unit/Probe 326,
Integration 24).

## 8. Maengel

Keine. (M1 und M2 der Abnahme 1 sind behoben und belegt; der eigene Befund N1 des Bau-Agenten ist richtig belegt und wirkt in
ROOM1171 nur auf die Westwand-Kraehe, in 10C0/10C1 gar nicht.)

### Hinweise (keine Maengel, fuer Integration/Nachlauf)
* **H1 Integration — fremde Dateien.** Der Spurstand hat kleine kommentierte Haken in enemy_ai_common.c (Spuren D/E; NB2
  zusaetzlich: HOVER `break`, SPIN-Zeile, Wurzel-Merkzeile + Aufruf nach dem Zustands-`switch` in `re15_crow_ai_tick`),
  game_step_common.c, actor_locomotion.c, player_common.c, szenenleon_r36.c, enemy_common.c und elza_r39.c (Spur H) — bei der
  Integration gegen D/E/H/J abgleichen. Die Griff-Zeit in ROOM1171 liegt jetzt 9-10 Bilder spaeter; fremde Riegel mit festen
  1171-Bildnummern gibt es nach Aussage und Lesung (`test_r38_heliport1171` relativ zum Griff) nicht.
* **H2 Ton der Opferphasen P1/P2 einen Takt spaet** (heliport1171_r38.c; SE @0x80115c3c/@0x80115ca4 im Takt des letzten f314
  @0x80115c20/@0x80115c88) — vom Bau-Agenten offen gefuehrt, Ton statt Animation, seit Abnahme 1 unveraendert.
* **H3 Zustand-1-Wand-Pass der normalen Kraehen** klemmt mit dem Band aus dem y nach der Bewegung
  (`re15_collision_constrain_enemy(..., e->y, 4u)`), das Original mit +0x82 aus dem Vor-Pass (@0x80112154) — Unterschied nur im
  Takt einer 1800er-Bandgrenze, nur RE1.5-/MIXED-Modus, nicht ROOM1171 (vom Bau-Agenten notiert, vorbestehend).
* **H4 Nachlauf-Teile der Wurzel fuer Nicht-4-Zustaende** (vorbestehend, nicht NB2): Koerper-Schub FUN_8002aec4 @0x801121d4
  (+0x1d0) und der Gegner-Schub FUN_8002b544 @0x801121e4 laufen im Original ebenfalls nach JEDEM Zustand; der Port hat den
  Koerper-Schub nur in Zustand 1/4. In ROOM1171 ohne Wirkung (INIT > 17000 Einheiten von Elza; Tod/Leiche: Wort0 Bit 2 ->
  aec4 kehrt mit 0 zurueck, Ghidra FUN_8002aec4.c / FUN_80114738.c `|= 2`); fuer eine Kraehen-Runde.
* **H5 Aufraeumen.** build_bisect und `re15_pc_r35e_haken_685df09f.exe` sind weg (H6 der Abnahme 1 erledigt). Meine
  Messkopien `re15_pc_k39abn2_{A,L,K}.exe` sind entfernt; der alte Motor liegt nur im Scratchpad (`K39abn2/alt`).

## 9. Ergebnis
* Punkt 1 (Wortlaut): **erfuellt** — 1a Kletter -> Heran-Winken (RBJ rec1 Clip 2, 96 Bilder) am echten Weg gemessen und im
  Bild belegt; 1b "Auch sonst": John und Elza = Original (unveraenderte Lauf-Folge), die Ereignis-Kraehen jetzt auch in
  HOVER-Phase 0, SPIN-Phase 2 und Wand-Pass = Original (selbst disassembliert, am echten Weg gemessen, Riegel am alten Motor
  rot).
* Gates: RE-Gate haelt, Vertrag haelt, Riegel vorhanden und messend, gezielte Tests gruen.
* **bestanden = true.**
