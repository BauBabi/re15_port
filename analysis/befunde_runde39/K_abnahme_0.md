# Runde 39 — Spur K "heliport1171": ABNAHME 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_heliport1171`, Zweig `r39/heliport1171`, HEAD ab223800 (Merge-Basis mit master 22abaed9;
master ist seither 2 Commits weiter: 37011982 doc, 96ab793c test_r38_itemorte). Der `git diff master`-Eintrag
"UEBERGABE.md / args_welle3_forts.json / test_r38_itemorte.c" ist der Vorsprung von master, KEINE Aenderung der Spur;
die Spur-Dateien = `git diff master...HEAD` (15 Dateien). Abnahme-Laeufe im Scratchpad `K39abn0/`, eigene exe-Kopien
`re15_pc_k39abn0_*.exe`, kein taskkill.

**Ergebnis: NICHT bestanden.** Der Kernbefund ist behoben und belegt (John winkt bei "Elza, hurry up!" mit RBJ rec1 Clip 2
statt des Kletter-Clips; am Nutzerweg mit der v0.8.26-exe nachgestellt und mit HEAD gegengemessen). "Auch sonst" ist nur fuer
John abgearbeitet: Elza zeigt in derselben Szene zwei belegte Ein-Bild-Abweichungen derselben Klassen, die K fuer John
behoben hat (M1, M2), und der Rest der Szene (Einzelbilder TURN->WALK, Kraehen) ist nicht gegen das Original geprueft (M3).
RE-Gate, Vertrag und Riegel halten. Punkt 1: **teilweise**.

## 0. Bau und Riegel (selbst gefahren)
* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> 764/764 Schritte,
  `=== LOCAL-BUILD-OK (build)`; exe `re15_port/build/platform/pc/re15_pc.exe` 18:59 (7 617 264 B) = HEAD ab223800
  (letzter Code-Commit 6d027394; ab223800 aendert nur Dossier + Kopfkommentar der Integration).
* Unit `ctest -R "^unit_r39_heliport1171_" -V`: **8/8 gruen** — banken (rec1 Marker 0x2 {20,30,96}, Bank 0 Clip 2 = 50),
  binder (John nach den Kraehen und nach 19 Slots ohne Record an rec1), szene (7 Plc_motion-Schritte je 0 abweichend,
  Kletter-Laenge 0 Bilder, Szene bis ROOM10B1 Bild 1078), leon1170, initpose (221 Bilder Loco 2/50 Bild 0, 0 abweichend),
  eskorte (29/29 eigene Bank), saatbild (7 Fenster: Saattakt 0 bzw. 19/9, 0 abweichend), schub (Bild 933 Clip 18 vorwaerts,
  danach 9..0, +0x6 = 7 im 10. Takt).
* Integration `ctest -R "^integration_r39_heliport1171_" -j 2 -V`: **2/2 gruen** (laden 44,6 s / weg 58,8 s). Messzeilen:
  laden `INIT-Pose Loco 2/50 Bild 0 221, eigene 2/52 vorher 0; Heran-Winken (Clip 2/96) 128 Bilder, Kletter-Clip 0, RBJ-Clip 0
  (20) 150, Bank-0-Clip 0 (34) 0, eigener Clip 6 (10) 183; Saattakte 7 (falsch: ); Schub ... [9,8,7,6,5,4,3,2,1,0]`; weg
  identisch bis auf INIT 226.
* GEGENPROBE der Riegel (selbst, ohne neu zu bauen — die Vorher-Baeume des Bau-Agenten in `re15_port/build_bisect` nur
  ausgefuehrt):
  - Unit-Teile gegen den b35c66bd-Motor (`build_bisect/tests/unit/test_r39_heliport1171.exe`, K1 schon drin, K2-K4 nicht):
    `szene` OK (K1), `initpose` FEHLER "221 Bilder ... (221 abweichend)", `eskorte` FEHLER "INIT-Bild: Loco Clip 2 Bild 0
    (st 1, loco 0, Bild 1)", `saatbild` 7 FEHLER ("Saattakt Bild 1 (soll 0)" / "Bild 18 (soll 19)" / "Bild 8 (soll 9)"),
    `schub` FEHLER "Bit-Takt (Bild 933): ... (Clip 17, rueckw. 1)". -> die K2-K4-Riegel messen.
  - Integration `laden` gegen die exe VOR K1 (`build_bisect/platform/pc/re15_pc_vorher_1038dc15.exe`, cmake -P mit dem
    Spur-Skript): **ROT** "'Elza, hurry up!' rendert nicht RBJ rec1 Clip 2 (96 Bilder: 0)". Johns Pose-Strom dort
    (Scratchpad `K39abn0/gegen_integ/laden_989fe47f/anim.txt`): F0-F220 eigene Clip 2/52 Bild 1; F221-F280 Bank-0-Clip 0/34;
    **F464-F591 mo 2 / fc 50 (Kletter-Clip) Bild 1..49**; F686-F775 Bank-0-Clip 0/34; Schub-Ruecklauf F933 `9,9,8..0`.
    (Schoenheitsfehler der Meldung: "Kletter-Clip 0" — der Kletterzaehler startet erst nach dem ersten RBJ-Clip-0-Bild, das es
    vor K1 nie gibt; rot wird der Riegel trotzdem ueber "Heran-Winken 0".)

## 1. Code-Diff (gelesen, `git diff master...HEAD -- re15_port`)
| Datei | +/- | Inhalt |
|---|---|---|
| engine/src/enemy_common.c | +3/-1 | K1: `rbj_resolve_slot` gibt fuer einen Slot OHNE Record -1 zurueck, BEVOR er einen der 2 Cache-Plaetze nimmt (vorher "negativ gecacht"). Kein weiterer Seiteneffekt: `re15_actor_rbj_anim/_skel` liefern fuer solche Slots vorher wie nachher NULL; Mehrkosten = Marker-Scan (<= 8 u32) je Abfrage. |
| engine/src/heliport1171_r39.c (neu) + include/re15_heliport1171_r39.h (neu) | +40 / +49 | K2: Merker je Aktor-Slot "zeigt noch die INIT-Pose" (Typ-Byte), `re15_r39k_init_pose()` = Merker && Typ 0x40..0x4d && state 0/1/4 && !walk && motion 2 && anim_frame 0; Installer loescht alle Merker beim Raumaufbau. |
| engine/src/enemy_ai_common.c | +31/-12 | K2: Include; Bank-Regel `uses_own_bank` -> 0 / `uses_loco_bank` -> 1 solange Merker; NPC-INIT `anim_frame 1 -> 0` + Merker setzen; Merker loesen im Executor (vor Sub-Dispatch) und in der Eskorte NACH dem +0x9==1-Ausgang (DECIDE-only bleibt ohne anim_set = Original). K3: `re15_npc_sub_motion` Phase 0 setzt `motion_init_delay = 1`; Phase 1 ruft je f314 EIN `re15_npc_anim` (posieren -> vorschieben), Doppelschritt/HOLD/LOOP wie Original. |
| engine/src/szenenleon_r36.c | +8/-2 | K4: NPC-Schub Phase 5 setzt im Bit-Takt nur +0x6 = 6 + Port-Merker 0xFE in sub_state_3; Phase 6 setzt Clip 0x11 rueckwaerts in ihrem ersten Takt (npc_clip mit Vorhalte-Takt -> Bild 9). Spieler-Schub (Elza) unveraendert (setzt Clip 0x11 weiter im Bit-Takt). |
| engine/src/scd_room_setup.c | +2 | Include + Installer-Zeile am Blockende. |
| tests: unit test_r39_heliport1171.c (+620), probes/r39_heliport1171.cmake (+48), integration test_r39_heliport1171.cmake (+283), test_r37_roy.c (+8/-3) | | Riegel (s. §0); r37_roy `spur`: Clip-2-Zeilen Elliots mit fc 50 / loco 1 / Slot 0 gelten jetzt als INIT-Pose. |

Nachgerechnet (Port-Semantik): `re15_npc_anim` = Vorhalte-Takt ODER `anim_frame+1 mod fc`, Rueckgabe 1 wenn das GERADE
posierte Bild fc-1 ist. Saattakt: Bild 0 (rueckwaerts fc-1), Takt fc-1 -> HOLD; LOOP -> naechster Takt Bild 0; Doppelschritt
im Saattakt -> Bild 1. Das deckt sich Takt fuer Takt mit Sub 0 @0x80050cb8 + f314/f3bc (§2).

## 2. RE-Gate (Stichproben selbst disassembliert)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (dis/table/bytes) auf `info/Re1.5/PSX.EXE` (md5 b55fdaa5,
= Hauptbaum; Auslieferungs-Stub @0x80026e4c `08 00 e0 03 21 10 00 00` -> NICHT die gepatchte EXE) und `PSX/BIN/STAGE{1,3,5}.BIN`.

| Behauptung (Diff/Dossier) | Selbst gefundene Bytes/Instruktionen | Urteil |
|---|---|---|
| K1 Binder FUN_8001b3f8: RBJ = Raumkopf+0x5C @0x8001b404, Recordzahl @0x8001b420, Marker @0x8001b438, Gegner-Bits ohne Grenze, Stride 500, +0x180 @0x8001b4a0, +0x184 @0x8001b4bc, Zaehler @0x8001b4c4 | `8001b404 lw a2,92(v0)` / `8001b40c beq a2,zero` / `8001b410 addu a3,zero,zero` (EINMAL vor der Record-Schleife) / `8001b420 lbu t2,4(a2)` / `8001b438 lw a0,0(v1)` / Bit 0 -> `8001b450 sw v0,0(t3)` (0x800acbd4) / Bit-Schleife `8001b470 andi v0,a0,0x1`, Stride `sll 5; subu; sll 2; addu; sll 2` = 500 / `8001b4a0 sw t1,0(at)` (0x800acdac+500*a3) / `8001b4bc sw v0,0(at)` (0x800acdb0+...) / `8001b4c0 bne a0,zero,0x8001b470` + Delay `8001b4c4 addiu a3,a3,1`; kein Vergleich gegen eine Kapazitaet, kein Typ-Test | stimmt (Nebenbefund: a3 laeuft ueber ALLE Records weiter — fuer ROOM1171 rec0 = 0x1, rec1 = 0x2 -> Gegner 0 = John, wie behauptet) |
| Binder EINMAL beim Raumaufbau nach den Spawns: `jal 0x8003ef6c` @0x80039a00, `jal 0x8001b3f8` @0x80039a08 | `80039a00 jal 0x8003ef6c` / `80039a08 jal 0x8001b3f8` | stimmt |
| Sub 0 0x80050cb8 (Tabelle @0x80076ca0[0]): Phase 0 @0x80050cec-d0c faellt in den Rumpf @0x80050d34; f314(+0x180,+0x184, a2=+0x1C4>>7) @0x80050d4c; Doppelschritt @0x80050d70/88; HOLD @0x80050da4; LOOP @0x80050dbc-c8 | `table 0x80076ca0`: [0] 0x80050cb8 / `80050cec sb v0(=1),6(a0)` / `80050cfc sb v0(=7),143(v1)` / `80050d0c sb zero,149(v0)` / `80050d30 sb zero,143(v1)` (No-Blend) / danach OHNE Sprung `80050d40 lw a0,384(v0)` / `80050d44 lbu a2,452(v0)` / `80050d48 lw a1,388(v0)` / `80050d4c jal 0x8001f314` + Delay `srl a2,a2,7` / `80050d70 andi v0,a2,0x8` -> `80050d88 jal 0x8001f314` / `80050da4 sb v0(=2),6(v1)` / `80050dbc andi v0,v0,0x4` -> `80050dc8 sb v0(=1),6(v1)` | stimmt |
| f314 posiert ZUERST (+0x95 @0x8001f35c, rueckwaerts @0x8001f344-54, +0x168 @0x8001f36c), Vorschub erst am Ende von FUN_8001f3bc @0x8001f618-1c, Ende @0x8001f63c | `8001f324 lbu v0,148(t0)` / `8001f344 lbu v0,149(t0); ... subu v0,t1,v0; addiu v0,v0,-1` (rueckwaerts) / `8001f35c lbu v0,149(t0)` / `8001f36c sw a2,360(t0)` / `8001f38c jal 0x8001f3bc` ... `8001f610 lbu v0,149(v1)` / `8001f618 addiu v0,v0,1` / `8001f61c sb v0,149(v1)` / `8001f624 sltu v0,v0,s4` / `8001f638 ori v0,zero,0x1` / `8001f63c sb zero,149(v1)` | stimmt |
| K2 John-INIT 0x8011dd98 (Tabelle @0x80121808[0]; Typ-Eintrag @0x80072cd0 gesetzt @0x8011e954-5c): +0x94 = 2 @0x8011de7c, +0x95 = 0 @0x8011de8c, +0x8f = 0 @0x8011de9c, f314(+0x84,+0x16c,a2=0,a3=0x200) @0x8011debc-c8 | `table 0x80121808`: [0] 0x8011dd98, [1] 0x8011df40, [4] 0x80050be8 / `8011e954 addiu v0,v0,-9112` (=0x8011dc68) + `8011e95c sw v0,11472(at)` (0x80072cd0) / `8011de78 ori v0,zero,0x2` / `8011de7c sb v0,148(v1)` / `8011de8c sb zero,149(v0)` / `8011de9c sb zero,143(v0)` / `8011deb8 addu a2,zero,zero` / `8011debc lw a0,132(v0)` / `8011dec0 lw a1,364(v0)` / `8011dec4 jal 0x8001f314` / `8011dec8 ori a3,zero,0x200` | stimmt |
| K2 "gleiches Muster in ALLEN NPC-INITs" (Stichprobe 3 der 13) | STAGE1 @0x8011c7bc-c808 (Marvin), STAGE1 @0x8011d92c-d978 (Elliot), STAGE3 @0x8011c4d4-c520, STAGE5 @0x8011c154-c1a0: je `ori v0,zero,0x2; sb v0,148` / `sb zero,149` / `sb zero,143` / `addu a2,zero,zero` / `lw a0,132` / `lw a1,364` / `jal 0x8001f314` / `ori a3,zero,0x200` | stimmt |
| K2 Eskorte 0x8011df40: Gegner < 15000 (@0x8011df48/54) -> +0x9 = 1 @0x8011df94; Dispatch @0x8012181c[+0x9&0xF]; [1] = 0x8004f100 ohne anim_set bis `jr ra` @0x8004f1fc | `8011df48 ori a0,zero,0x3a98` / `8011df54 jal 0x800509e4` / `8011df94 sb v0(=1),9(v1)` (Delay von `j`) / sonst `8011dfc0 sb zero,9(v0)` / `8011dfe4 addiu at,at,6172` (0x8012181c) + `8011dff4 jalr v0`; `table 0x8012181c`: [0] 0x8011e00c, [1] 0x8004f100; `scan 0x8004f100`: nur `jal 0x8001ab9c` (arc_test) und +0x5/+0x6-Stores, `jr ra` @0x8004f1fc — kein `jal 0x8001f314` | stimmt |
| K4 Schub 0x80052864 (Tabelle @0x80076ca0[19]): Phase 5 f314(Loco, a2=0) @0x80052af4 VOR der Bit-Abfrage `jal 0x8004efe4` @0x80052b44; dann +0x6 = 6 @0x80052b60, +0x94 = 0x11 @0x80052b6c-70, +0x95 = 0 @0x80052b80; Phase 6 a2 = 1 @0x80052b8c, f314 @0x80052b98 | `table`: [19] 0x80052864 / `80052ae8 addu a2,zero,zero` / `80052aec lw a0,132` / `80052af0 lw a1,364` / `80052af4 jal 0x8001f314` / `80052b38 lbu a1,451(v0)` / `80052b40 addiu a0,a0,4136` (0x800b1028) / `80052b44 jal 0x8004efe4` / `80052b60 sb v0(=6),6(v1)` / `80052b6c ori v0,zero,0x11` / `80052b70 sb v0,148(v1)` / `80052b7c j 0x80052bf4` + Delay `80052b80 sb zero,149(v0)` / `80052b8c ori a2,zero,0x1` / `80052b98 jal 0x8001f314` / `80052bb4 addu v1,v1,v0` (+0x6 += Rueckgabe) | stimmt |
| Kletter-Clip = Loco Clip 2: +0x94 = 2 @0x800526a0/a4, f314(+0x84,+0x16c) @0x800527f0-f8 | `800526a0 ori v0,zero,0x2` / `800526a4 sb v0,148(v1)` / `800527f0 lw a0,132(v0)` / `800527f4 lw a1,364(v0)` / `800527f8 jal 0x8001f314` | stimmt |
| Kanal-Lader FUN_80022300: +0x16c = dir[1] @0x80022498, +0x84 = dir[2] @0x800224a8, +0x174 = dir[3] @0x800224b8, +0x170 = dir[4] @0x800224c8 | `8002248c lw v0,4(s3)` -> `80022498 sw v0,364(s2)` / `8002249c lw v0,8(s3)` -> `800224a8 sw v0,132(s2)` / `800224ac lw v0,12(s3)` -> `800224b8 sw v0,372(s2)` / `800224bc lw v0,16(s3)` -> `800224c8 sw v0,368(s2)` | stimmt |
| ROOM1171.RDT (ausgeliefert = info/, md5 b1626467 beide): RBJ @0x188C, rec1 Marker 0x2, EDD @RBJ+0x3E34 {20,30,96}; sub02 Plc_motion(0,0,0) @0x01416/@0x0141E, Plc_flg 0x80 @0x01422, Plc_motion(2,6,0) @0x0142A/@0x014C2, msg 0 @0x014BA, Plc_motion(0,2,0) @0x014CE + Sleep 60, Plc_motion(0,0,0) @0x01526/@0x0152E | Python-Parse: `RBJ @0x188C, nrec=2; rec0 marker=0x1 lens=[20,30]; rec1 marker=0x2 EDD @RBJ+0x3E34 lens=[20,30,96]`; `xxd -s 0x1400`: @0x1416 `3f00 0000`, @0x141E `3f00 0000`, @0x1422 `4300 8000`, @0x142A `3f02 0600`, @0x14BA `2b00 0000`, @0x14C2 `3f02 0600`, @0x14C6 `4300 8000`, @0x14CE `3f00 0200`, @0x14D2 `090a 3c00`, @0x1526 `3f00 0000`, @0x152E `3f00 0000`, @0x1532 `4300 8000` | stimmt |

Guess-Tells in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|getenv|approx|geschaetzt|vermutlich`):
**keine Treffer**. Konstanten: RE15_R39K_INIT_CLIP 2 (@0x8011de78-7c), RE15_R39K_INIT_BILD 0 (@0x8011de8c) belegt;
RE15_R39K_SCHUB_SAAT 0xFE ist ein Port-Merker (als solcher gekennzeichnet, kein Spielwert). Der Fix ERKLAERT den
gemessenen Befund: Kraehen (state 4/Sub 0, ohne Record) belegten beide Cache-Plaetze -> John fiel auf Bank 0 Clip 2 (=
Kletter-Clip @0x800526a0) zurueck; mit K1 bekommt John rec1 (Original-Binder ohne Grenze).

## 3. Vertrag
* Pfade (`git diff master...HEAD --name-only`): nur `analysis/befunde_runde39/K_heliport1171.md` + `K_belege/*.png`,
  `engine/src/{enemy_ai_common,enemy_common,heliport1171_r39,scd_room_setup,szenenleon_r36}.c`,
  `include/re15_heliport1171_r39.h`, `tests/integration/test_r39_heliport1171.cmake`, `tests/unit/probes/r39_heliport1171.cmake`,
  `tests/unit/test_r3{7_roy,9_heliport1171}.c`. Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists
  (Registrierung ueber GLOB `tests/unit/CMakeLists.txt:4060` -> probes/*.cmake). OK.
* Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse / Sprachdateien: keine (grep der hinzugefuegten Zeilen auf
  game_flag_set/MSG_ID/msg/synchro/audio: 0 Treffer). OK.
* Gemeinsame Dateien: scd_room_setup.c (Include + Installer-Zeile am Blockende, "Runde 39 Spur K") OK. enemy_ai_common.c
  (VERTRAG 1.3 nur D/E zugeteilt) traegt 6 Haken mit je <= 5 Codezeilen und Kommentar "Runde 39 Spur K" (zusammen 23
  geaenderte Codezeilen, darunter der Rumpf von re15_npc_sub_motion); enemy_common.c (1 Codezeile) und szenenleon_r36.c
  (Include + ~5 Zeilen) stehen in 1.3 gar nicht. Form = kleine Haken; der Eingriff in den GEMEINSAMEN NPC-Executor ist aber
  inhaltlich global (alle NPCs, beide Szenarien) — Hinweis fuer die Integration (s. §6 H1). Ueberschneidung mit den anderen
  Welle-3-Zweigen (Stand jetzt): nur scd_room_setup.c mit r39/funk10f1 (Installer-Zeile); r39/dining1141 und r39/karten
  beruehren keine der K-Dateien.
* Fremder Riegel angepasst: `test_r37_roy.c` `spur` — laesst jetzt JEDE Elliot-Zeile mo 2 / fc 50 / loco 1 / Slot 0 als
  "INIT-Pose" zu, nicht nur die Zeilen vor dem ersten Executor-Takt (Dossier 7 sagt "genau diese Form"); schwaecher als
  beschrieben (s. §6 M-Liste).
* Neue Tests parallel-/Linux-fest: eigene Laufordner (`r39_heliport1171_wd/<teil>_<zufall>`), exe-Kopie
  `re15_pc_r39k_<teil>_<zufall>` (wird geloescht), RE15_*-Umgebung des Aufrufers wird geloescht, `cmake_minimum_required`
  im -P-Skript, `m` unter Nicht-Windows gelinkt, keine .exe-/Backslash-Pfade. OK.
* Gezielte Tests selbst gefahren: Unit/Probe NPC-Familie (Regex des Bau-Agenten, `-E "^integration_"`): **206/206 gruen**
  (17 s); Spur-Riegel 8/8 + 2/2 (§0); Integration (echte exe) der beruehrten Module/Raeume (NPC-INIT/Executor/Schub/RBJ in
  beiden Szenarien) `ctest -R "^integration_(r37_roy_leon1170|r37_roy_szene1011|r37_roy_szene1021|r38_werkstatt11b0|
  r36_szenenleon|r35_cut10f0|r38_heliport1171_weg_re2|r38_heliport1171_laden|r39_stimmen_1171|r38_dining1141_d|r36_szene1150|
  r35_cut1150|r39_stimmen_1050|r37_elzaems_elza1141|r39_stimmen_11b0|r38_montage)$" -j 2 --timeout 1200`:
  **`100% tests passed, 0 tests failed out of 16`** (dining1141_d 287 s, cut10f0 270 s, cut1150 562 s, werkstatt11b0 152 s,
  szenenleon 162 s, ...). Keine volle Suite (VERTRAG).

## 4. Messung Nutzer-Punkt (selbst, an der exe)

Wortlaut: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
VERTRAG 0 (Spur K): "John 1171 "Elza hurry up": Heran-Winken statt Klettern; **uebrige Szenen-Animationen 1171**".

### 4.1 Pose-Strom Lade-Weg: drei Motoren nebeneinander (RE15_ANIM_TRACE, Spalten aus anim_select_common.c)
Laeufe: HEAD = `re15_port/build/r39_heliport1171_wd/laden_35869cd7/anim.txt` (mein ctest-Lauf), b35c66bd-Motor (K1 ohne
K2-K4) = `K39abn0/gegen_integ_b35/laden_6a918219/anim.txt`, vor K1 (1038dc15) = `K39abn0/gegen_integ/laden_989fe47f/anim.txt`;
ausgewertet mit `K39abn0/runs.py` (Laeufe gleicher mo/clip/fc/loco/rev). John = Aktor 1, Typ 0x49:

| Bild | Skript | vor K1 (1038dc15) | b35c66bd (nur K1) | HEAD (K1-K4) | Original (Herleitung §2) |
|---|---|---|---|---|---|
| F0-F220 | INIT/Eskorte (DECIDE) | eigene 2/52 Bild 1 | eigene 2/52 Bild 1 | **Loco 2/50 Bild 0**, loco=1 | Loco 2 Bild 0 (@0x8011debc-c4) |
| F221-F260 | Plc_motion(0,0,0) | **Bank-0 0/34** 1..21 | RBJ 0/20 1..19 | RBJ 0/20 **0..19** | RBJ rec1 0 (20) 0..19 |
| F261-F280 | + Plc_flg 0x80 | Bank-0 0/34 rueckw. 32..13 | 18..0,0 | **19..0** | 19..0 |
| F281-F463 | Plc_motion(2,6,0) (+0x80) | eigene 6/10 1..9 / 8..0 | 1..9 / 8..0 | **0..9 / 9..0** | 0..9 / 9..0 |
| F464-F591 | **msg 0 "Elza, hurry up!"** Plc_motion(0,2,0) | **mo 2 fc 50 = Kletter-Clip 1..49** | RBJ 2/96 1..95 | **RBJ 2/96 0..95** (haelt 95) | RBJ rec1 Clip 2 (96) 0..95 |
| F686-F775 | msg 1, Plc_motion(0,0,0) x2 | Bank-0 0/34 | RBJ 0/20 1..19 / 18..0 | **0..19 / 19..0** | dito |
| F933 | Schub Ph 5, Bit-Takt | Clip 17 rueckw. 9 | Clip 17 rueckw. 9 | **Clip 18 Bild 0** | Clip 0x12 (@0x80052af4 vor @0x80052b44) |
| F934-F943 | Ph 6 | 9,8..0 (9 doppelt) | 9,8..0 (9 doppelt) | **9..0** | 9..0 (@0x80052b98) |

* Alle uebrigen Zeilen Johns in allen drei Laeufen gleich (Walk Clip 5/30, Sub 6 Clip 1/16 0..15, Clip 2/52, Schub-Ansatz
  Clip 17/18 Loco).
* Alle NICHT-John-Zeilen (Elza, beide Kraehen) HEAD vs. b35c66bd: **bytegleich**, md5 `65b9f910b1e43bd3b5f6fef2582ce04a`
  beide (= der vom Bau-Agenten gemeldete Wert), 0 Differenzzeilen; John 411 Zeilen anders.
* Ergebnis Kernpunkt: die Kletter-Animation (Loco/Bank-0 Clip 2 = Kante-hoch-Clip, `+0x94 = 2` @0x800526a0) ist weg; John
  spielt bei "Elza, hurry up!" RBJ rec1 Clip 2 (96 Bilder), den das Original ueber den Binder (+0x180/+0x184) waehlt.

### 4.2 ECHTER WEG (Nutzerweg 1141 -> Dachtuer -> 1171 -> Tor -> Szene), HEAD gegen v0.8.26, beschleunigter Renderer
Befehl (Scratchpad `K39abn0/lauf.sh`, Wegplan = `integration_r38_dining1141` Teil D bis zur Dachtuer, `K39abn0/wegplan_1141_1171.txt`):
`RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_MSG_LOG=1
RE15_EVT_TRACE=1 RE15_STATE_LOG=state.txt RE15_ANIM_TRACE=anim.txt RE15_WEGPLAN=wegplan.txt "RE15_EXIT_AT=60#10b1"
RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 RE15_GIVE=0x43:1 RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0
RE15_R38F_GORILLAS_TOT=600 RE15_R38F_GORILLAS_STARR=1 RE15_FRAMEDUMP=200-960/4:fd_ RE15_FIRE_AOT=5@60#1171`, exe-Kopien
`re15_pc_k39abn0_A.exe` (HEAD, sha256 0c1ca3cc...) und `re15_pc_k39abn0_B.exe` (= release/pkg-win/re15_port_v0.8.26/re15_pc.exe,
sha256 a5a1c016...), gleiche Assets, kein SOFTWARE_RENDER.
* Ablauf beider Laeufe (debug.log): `DOOR FIRE slot=0` 1131 -> 1141; `DOOR FIRE slot=1 ... target_cut=9` 1141 -> 1171 (Cut 9,
  KEIN Ereignis — erster Lauf ohne Tor blieb dort stehen, abgebrochen); `DOOR FIRE slot=5 rect=(-11065,-27850,hw=875,hh=600)
  target_cut=0 spawn=(2300,-7200,14365)` (das Tor; RE15_FIRE_AOT ruft `aot_fire_door` = derselbe Pfad wie die Aktionstaste,
  aot_common.c:870-879; der Treppenweg Boden -> Ebene 4 wurde nicht gelaufen) -> `F61 room=1171 Evt_exec sub=2` -> msg 0 / 1 / 3
  -> `loaded room10b1.rdt` -> `EXIT_AT: Bild 60 in Raum 10b1` (EXIT=0 beide).
* John (Aktor 1) im Szenen-Abschnitt (Bildnummern dieses Wegs = Lade-Weg + 66):

| Bild | Skript | v0.8.26 (Nutzerstand) | HEAD |
|---|---|---|---|
| F61-F286 | INIT/Eskorte | eigene 2/52 Bild 1 | Loco 2/50 Bild 0 |
| F287-F346 | Plc_motion(0,0,0) vor/zurueck | Bank-0 0/34 1..21 / 32..13 | RBJ 0/20 0..19 / 19..0 |
| F347-F529 | Plc_motion(2,6,0) vor/zurueck | 1..9 / 8..0 | 0..9 / 9..0 |
| **F530-F657** | **"Elza, hurry up!" Plc_motion(0,2,0)** | **mo 2 / fc 50 Bild 1..49 = Kletter-Clip** | **RBJ 2/96 Bild 0..95** |
| F752-F841 | msg 1 Plc_motion(0,0,0) x2 | Bank-0 0/34 | RBJ 0/20 0..19 / 19..0 |
| F999 / F1000-F1009 | Schub Bit-Takt / Ph 6 | Clip 17 rueckw. 9,9,8..0 | Clip 18 Bild 0 / 9..0 |

* BILD (Framedumps 320x240, je 8 Bilder): `K39abn0/hurry_up_neu_vs_v0826.png` — v0.8.26 F548-F572: John knickt ein, geht in
  die Hocke, Oberkoerper nach vorn (= "komische Kletter Animation" des Nutzers, nachgestellt); HEAD F540-F588: John dreht sich
  mit einem Schritt, hebt den Arm und winkt zu sich heran (F564/F572/F580), F596 Kameraschnitt (Totale). `K39abn0/
  gesten_neu_vs_v0826.png`: erste Geste F284-F316 (HEAD: Arm mit Waffe vor den Koerper = RBJ Clip 0; v0.8.26: fast still =
  Bank-0 Clip 0) und msg 1 "We need to get this thing off of here! Help me out!" F756-F788.
* INIT-Merker: Scan aller NPC-Zeilen (`K39abn0/merker_pop.py`) in beiden HEAD-Laeufen (1031/1131/1141/1171/10B1, 5402 + 4293
  NPC-Zeilen) und in den Baum-Laeufen (laden/weg/werkstatt11b0): INIT-Form nur als zusammenhaengender Lauf ab der INIT
  (1141: 1 Bild bei F6, John geht sofort in den Executor; 1171: 226 Bilder), **kein Wiederaufleben** nach dem Loesen.
* ROOM1141 (Nebenwirkung, gleicher Lauf): Johns Gesten dort beginnen jetzt ebenfalls mit Bild 0 bzw. fc-1 (K3 global; vorher
  1 bzw. fc-2), INIT-Bild F6 Loco 2/50 Bild 0 statt eigene 2/52 Bild 1; sonst gleiche Clipfolge wie v0.8.26.

### 4.3 "Auch sonst ... die Animationen hier": die UEBRIGEN Szenen-Animationen in 1171 (Elza, Kraehen)
Elza (Aktor 0) im selben Szenen-Abschnitt, HEAD, echter Weg (`runs.py runA_neu/anim.txt 0 4`); im Lade-Weg dieselben
Befunde an den um 66 bzw. 67 Bilder frueheren Stellen (F1, F37, F186, F591, F669, F840, F846, F933, F943). K hat an Elza nichts
geaendert (Nicht-John-Zeilen HEAD = b35c66bd bytegleich, 4.1):

| Bild (echter Weg) | Elza posiert (HEAD) | Original (selbst disassembliert) | Befund |
|---|---|---|---|
| F104-F118, F252-F266, F657-F671, F735-F749, F1009-F1023 | Sub 6 (Event-Reach) Clip 1 Bild **1..15** (15 Bilder), danach Clip 2 ab 0 | Sub 6 0x800517f0 (Spieler-Tabelle `table 0x80073e30`: [6] = 0x800517f0, dieselbe wie NPC [6]): Phase 0 `80051844 sb v1(=1),6(a0)` / `80051854 sb v1,148(v0)` (+0x94 = 1) / `80051864 sb zero,149(v0)` / `80051874 sb v0(=7),143(v1)` und OHNE Sprung `8005188c jal 0x8001f314` (+0x170/+0x174) -> der Saattakt posiert **Bild 0**, Clip 1 = 0..15 (16 Bilder) | **Abweichung, K3-Klasse** — Port: `game_step_common.c re15_player_event_reach_tick` schiebt im Saattakt vor (`pl->anim_frame++` vor dem Posieren). Seit Runde 38 bekannt: `analysis/befunde_runde38/I_heliport1171.md` Abschnitt 3 "Uebernommene Konvention: Modus 6 ... Original ... zeigt dort Bild 0; Port: Bild 1 im Umlaufbild. Ein Bild Versatz" (= F252 nach dem Kraehen-Wurf) |
| F999-F1008 (Elza) / F1000-F1009 (John) | Schub: Elza Clip 17 rueckwaerts **ab dem Bit-Takt F999** (9..0 bis F1008); John (HEAD) erst ab F1000 | dieselbe Sub 0x80052864 (`table 0x80073e30`: [19] = 0x80052864): Bit-Takt posiert noch Clip 0x12 (`80052af4 jal 0x8001f314` vor `80052b44 jal 0x8004efe4`), Clip 0x11 rueckwaerts erst in Phase 6 (`80052b98`) | **Abweichung, K4-Klasse** — Port: `szenenleon_r36.c re15_szl_schub_spieler_tick` Fall 5 setzt `sp_clip(p, RE15_SZL_CLIP_ANSATZ, 1, 0)` noch im Bit-Takt. Seit K4 laufen Elza und John beim Loslassen um 1 Bild versetzt |
| F68, F906, F912 | je EIN Bild Clip 1 Bild 1 zwischen TURN (mo 105) und WALK/TURN | ungeklaert (Reihenfolge SCD-Poll vs. Dispatcher, Spur H "eigener RE-Punkt") | offen, nicht untersucht |
| Kraehen (Aktoren 2/3) | unveraendert seit Runde 38 | vom Bau-Agenten NICHT gegen das Original verglichen (nur "vorher = nachher bytegleich") | ungeprueft |

Der Bau-Agent hat die ersten drei Zeilen selbst gemessen und mit Adressen im Dossier (6.3, 8) als "OFFEN — nicht Johns
Animation" abgelegt. Der VERTRAG (0) teilt Spur K aber "uebrige Szenen-Animationen 1171" zu, und der Nutzer schreibt "Auch
sonst scheinen die Animationen **hier** noch nicht ganz zu passen" — nicht "Johns Animationen". Sichtbarkeit: jede dieser
Abweichungen ist eine Ein-Bild-Verschiebung (bei 30 Bildern/s und Blende +0x8f = 7 kaum wahrnehmbar), aber nach dem
Massstab des Projekts (byte-true, jede Abweichung mit Adresse) sind es offene, belegte Fehler genau in dem Bereich, den der
Nutzer bemaengelt.

### 4.4 Nebenwirkung Leon-Szenario (K2/K3 sind global fuer alle NPCs)
`cmake -P tests/integration/test_r37_roy.cmake -DTEIL=leon1170` (echter Weg Neuspiel -> Montage ROOM1240 -> Tuer -> ROOM1170,
RE15_FPS=240, EXIT_AT 1200#1170) einmal mit der HEAD-exe, einmal mit `build_bisect/platform/pc/re15_pc.exe` (b35c66bd-Motor),
Spurwerkzeug = HEAD-`test_r37_roy.exe`: beide `OK`. Pose-Strom je 4360 Zeilen: **Leon (Aktor 0) bytegleich** (md5 c23ba5ea
beide), Elliot (Aktor 1) **24 Zeilen** anders — F884-F889 INIT-Pose Loco 2/50 Bild 0 statt eigene 2/52 (`[enemy-diag] ...
type=0x47 model=elliot ... motion=2 anim_frame=0 clips=24` statt `anim_frame=1 clips=6`), ab F920 "Hey!"-Geste Clip 15 ab
Bild 0 (bei 240 Bildern/s `0,0,1,1,..` statt `0,1,1,2,..`). Deckt sich mit Dossier 6.6.

### 4.5 Erklaert der Fix den Befund?
Ja. v0.8.26-Zustandslog des echten Wegs (`K39abn0/runB_v0826/state.txt`) F70 in 1171: Kraehe 3 `st=4 ss1=0` (Sub 0) — der
Renderer fragt jeden Aktor in state 4/Sub 0 nach dem RBJ-Kanal, der Slot ohne Record belegte einen der 2 Cache-Plaetze
dauerhaft (bis zum Raumwechsel; F530 sind beide Kraehen schon `st=7`). John `st=4 ss1=0 mo=2` bei "Elza, hurry up!" bekam
keinen Platz -> Rueckfall auf die Container-Bank, deren Clip 2 der Kletter-Clip ist (`+0x94 = 2` @0x800526a0, 50 Bilder).
Mit K1 belegen Slots ohne Record keinen Platz mehr; John bindet rec1 wie der Original-Binder (§2).

## 5. Urteile

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1a "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink Animation, die er im Original nicht macht." | **erfuellt** | Nutzerweg (1141 -> 1171 -> Tor) mit der v0.8.26-exe nachgestellt: F530-F657 mo 2 / fc 50 = Kletter-Clip, Bild zeigt das Einknicken (`K39abn0/hurry_up_neu_vs_v0826.png`). HEAD: RBJ rec1 Clip 2 (96 Bilder) 0..95, Bild zeigt Drehen + Heranwinken. Ursache (Cache von Kraehen belegt) und Original-Mechanismus (Binder FUN_8001b3f8 ohne Grenze, Sub 0 auf +0x180/+0x184) selbst disassembliert (§2), Fix erklaert den Befund (§4.5), Riegel unit/integration gruen und am alten Motor rot (§0). |
| 1b "Auch sonst scheinen die Animationen hier noch nicht ganz zu passen." (VERTRAG: "uebrige Szenen-Animationen 1171") | **teilweise** | John: alle Abweichungen behoben und belegt — K1 (auch die anderen Gesten kamen aus Bank 0 Clip 0/34 statt RBJ Clip 0/20), K2 INIT-Pose, K3 Saattakt, K4 Schub (§4.1/4.2, Disasm §2). Elza in derselben Szene: zwei belegte, gemessene Abweichungen offen (Event-Reach-Saatbild, Schub-Ruecklauf im Bit-Takt; §4.3), Einzelbilder TURN->WALK ungeklaert, Kraehen nie gegen das Original geprueft. Der Bau-Agent hat Elzas Abweichungen selbst gefunden und als "nicht Johns Animation" offen gelassen. |
| **Punkt 1 gesamt** | **teilweise** | Kernbefund erfuellt; "Auch sonst" nur fuer John abgearbeitet. |

Gates: RE-Gate **haelt** (alle Stichproben stimmen, keine Guess-Tells, Konstanten mit Adresse bzw. als Port-Merker
gekennzeichnet). Vertrag **haelt** (Pfade, keine Bits/Ids, kleine kommentierte Haken; Hinweis H1/H2). Riegel **vorhanden
und messend** (8 Unit + 2 Integration gruen, am alten Motor rot). Nebenwirkungen: Leon 1170 bytegleich bis auf Elliots
INIT-/Gestenbilder (§4.4), John 1141 Gesten ab Bild 0 (§4.2), NPC-Familie Unit/Probe 206/206.

## 6. Maengel (nummeriert, nachpruefbar)

**M1 — Elzas Event-Reach-Saatbild in der 1171-Szene (K3-Klasse, Spieler-Pfad).** Gemessen (echter Weg, HEAD,
`K39abn0/runA_neu/anim.txt`, Aktor 0): Sub-6-Clip 1 laeuft F104-F118, F252-F266, F657-F671, F735-F749, F1009-F1023 jeweils
**Bild 1..15** (15 Bilder); Lade-Weg ebenso (F37, F186, F591, F669, F943). Original: Spieler-Tabelle @0x80073e30[6] =
0x800517f0, Phase 0 `80051854 sb v1(=1),148` / `80051864 sb zero,149` / `80051874 sb v0(=7),143` faellt ohne Sprung in
`8005188c jal 0x8001f314` -> der Saattakt posiert **Bild 0**, Clip 1 = 0..15. Port: `engine/src/game_step_common.c`
`re15_player_event_reach_tick` erhoeht `anim_frame` vor dem Posieren. Das Dossier fuehrt genau das als OFFEN (6.3/8), Runde 38
Spur I nannte es "Uebernommene Konvention ... Ein Bild Versatz". Pruefung nach Behebung: Clip-1-Laeufe Elzas in 1171 beginnen
mit Bild 0 und haben 16 Bilder; Riegel am alten Stand rot.

**M2 — Elzas Schub-Ruecklauf beginnt im Bit-Takt (K4-Klasse, Spieler-Pfad); seit K4 laeuft sie 1 Bild vor John.**
Gemessen: Elza Clip 17 rueckwaerts F999-F1008 (Bit-Takt F999), John F1000-F1009. Original: dieselbe Sub 0x80052864
(Spieler-Tabelle @0x80073e30[19]) posiert im Bit-Takt noch Clip 0x12 (`80052af4 jal 0x8001f314` vor `80052b44 jal
0x8004efe4`), Clip 0x11 rueckwaerts erst in Phase 6 (`80052b8c ori a2,zero,0x1` / `80052b98 jal 0x8001f314`). Port:
`engine/src/szenenleon_r36.c` `re15_szl_schub_spieler_tick` Fall 5 ruft `sp_clip(p, RE15_SZL_CLIP_ANSATZ, 1, 0)` im
Bit-Takt. Pruefung: Elza zeigt im Bit-Takt Clip 18, ihr Clip-17-Ruecklauf 9..0 beginnt im selben Bild wie Johns.

**M3 — "Auch sonst" fuer den Rest der Szene nicht untersucht.** (a) Elza zeigt je EIN Bild Clip 1 Bild 1 zwischen
TURN-Ankunft und naechstem Gang (echter Weg F68, F906, F912; Lade-Weg F1, F840, F846) — ob das Original dort ein Sub-6-Bild
zeigt (Reihenfolge SCD-Takt vs. Spieler-Dispatcher @0x80031c44, Spur H "eigener RE-Punkt"), ist nicht belegt; falls ja,
waere es Bild 0, nicht 1 (M1). (b) Die Animationen der beiden Ereignis-Kraehen (Griff, Wurf, Tod) wurden vom Bau-Agenten nur
"vorher = nachher" verglichen, nie gegen das Original. Beides gehoert zu "uebrige Szenen-Animationen 1171" (VERTRAG 0) bzw.
"die Animationen hier" (Nutzer); entweder belegen/beheben oder dem Nutzer ausdruecklich als Rest vorlegen.

Kein Mangel am Kernbefund (1a): Heran-Winken statt Klettern ist umgesetzt, belegt und gemessen.

### Hinweise (keine Maengel, fuer Integration/Nachlauf)
* **H1 Globale Wirkung.** K2/K3/K4 gelten fuer ALLE NPCs beider Szenarien (gemeinsamer NPC-INIT/Executor/Schub): jede
  Plc_motion-Geste beginnt ein Bild frueher (Bild 0 bzw. fc-1), jede NPC-INIT zeigt bis zum ersten anim_set Loco Clip 2 Bild 0.
  Gemessen: Leon 1170 bytegleich, Elliot 24 Zeilen (§4.4); John 1141 Gesten ab Bild 0 (§4.2). Spur J (1141, gleiche Welle)
  sieht das automatisch; wer dort Gesten-Bildfolgen pinnt, muss mit Bild 0 rechnen.
* **H2 Gemeinsame Dateien.** enemy_ai_common.c ist im VERTRAG 1.3 D/E zugeteilt, enemy_common.c/szenenleon_r36.c gar nicht;
  K traegt dort nur kleine, kommentierte Haken (der groesste: Rumpf von re15_npc_sub_motion). Textkonflikt-Risiko jetzt nur
  scd_room_setup.c (Installer-Zeile) mit r39/funk10f1.
* **H3 Fremder Riegel.** `test_r37_roy.c spur` zaehlt JEDE Elliot-Zeile mo 2 / fc 50 / loco 1 / Slot 0 als INIT-Pose, nicht
  nur die Zeilen vor dem ersten Executor-Takt — ein spaeteres Wiederaufleben des Merkers fiele dort nicht auf (heute keines:
  `merker_pop.py` ueber alle Laeufe 0). Besser: nur einen zusammenhaengenden Lauf ab der ersten Elliot-Zeile zulassen.
* **H4 Riegel-Meldung.** `integration_r39_heliport1171`: der Kletterzaehler startet erst nach dem ersten RBJ-Clip-0-Bild;
  am Motor VOR K1 meldet er "Kletter-Clip 0", obwohl 128 Kletter-Bilder gerendert werden (rot wird er ueber "Heran-Winken 0").
  Robuster: mo 2 / fc 50 im Fenster von Plc_motion(0,2,0) zaehlen.
* **H5 Restgrenze des Caches.** `RE15_RBJ_CACHE_MAX 2` bleibt fuer Slots MIT Record; ROOM1040/1041 (Marker 0x7E = Gegner 0..5
  an rec0) bindet im Original 6 Aktoren (Binder ohne Grenze). Nicht 1171, vorbestehend (Dossier 3).
* **H6 Aufraeumen.** `re15_port/build_bisect/` (Vorher-Baeume des Bau-Agenten) und die liegengebliebene Messkopie
  `re15_port/build/platform/pc/re15_pc_r35e_haken_173bda57.exe` (18:42, fremder Testlauf) sind ignorierte Dateien —
  bei der Integration entfernen. Meine Kopien `re15_pc_k39abn0_{A,B}.exe` sind entfernt.

## 7. Ergebnis
* Punkt 1 (Wortlaut): **teilweise** — 1a Kletter -> Heran-Winken **erfuellt**; 1b "Auch sonst ... die Animationen hier"
  **teilweise** (John vollstaendig, Elza M1/M2 offen, Rest M3 ungeprueft).
* Gates: RE-Gate haelt, Vertrag haelt, Riegel vorhanden/messend/am alten Motor rot, gezielte Tests gruen (206 Unit/Probe,
  10 Spur-Riegel, 16 Integration).
* **bestanden = false** (Maengel M1-M3). Naechster Schritt fuer die Spur: Spieler-Pendants zu K3/K4 (Event-Reach-Saattakt
  @0x80051844-8c, Schub Phase 5 -> 6 @0x80052af4/@0x80052b44/@0x80052b98) fuer Elza umsetzen und mit Riegeln sichern, die
  Einzelbilder TURN->WALK und die Kraehen-Animationen der Szene gegen das Original belegen — oder vom Nutzer entscheiden
  lassen, dass "Auch sonst" nur John meinte.
