# Runde 35 Spur C "zgirl" — Unabhaengige Abnahme 0 (2026-10-04)

Baum `.claude/worktrees/r35_zgirl`, Zweig `r35/zgirl`, HEAD `0ab1e6f5`, Merge-Basis `154a73c1`.
Gemessen am gebauten Stand: `local_build.sh configure` + `build` -> `ninja: no work to do`,
`=== LOCAL-BUILD-OK (build)` (exe = HEAD, Baum sauber bis auf unversioniertes `build/`).
Code-Diff gegen die Merge-Basis (`git diff master...HEAD`): genau EINE Verhaltenszeile in
`re15_port/engine/src/aot_common.c:801` (`g_scd_pending_scenario = (int)d->target_cut;` jetzt
unbedingt im Selbst-Tuer-Zweig), sonst nur neue Tests/Werkzeuge/Dossier.
(`git diff master --stat` zeigt 166 Dateien, weil master inzwischen auf `87cc8575` weiter ist —
das sind Fremdaenderungen anderer Spuren, nicht dieser Zweig. `git merge-tree master HEAD`: konfliktfrei.)

**Ergebnis: NICHT BESTANDEN.** Das Zombie-Maedchen selbst ist portiert und am echten Tuerweg
gemessen. Aber die eine Fix-Zeile (U1) bricht den Endkampf in ROOM5090: der finale Birkin stirbt
beim Kampfstart von selbst, ohne einen einzigen Treffer (Mangel M1, A/B-Messung mit identischer
Eingabe gegen eine Basis-exe ohne U1).

Messwerkzeug: `scratchpad/abn_lauf.sh` (eigene exe-Kopie `re15_pc_abn0c.exe`, nie fremde Prozesse
beendet), Laufordner `scratchpad/c_mess/<marke>/` (debug.log, state.log, Framedumps).
Scratchpad = `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`
(Belegbilder liegen NUR dort, laut Auftrag wird nur dieser Bericht committet).
Bilder: RE15_FRAMEDUMP mit `SDL_RENDER_DRIVER=opengl` (beschleunigt, KEIN SOFTWARE_RENDER).

---

## Punkt 1 — "Was sollen Zombie Maedchen sein? Wenn es das gibt, muss es natuerlich mit portiert werden"

### 1a Was es ist (Antwort an den Nutzer) — nachgeprueft
* Typ 0x13 / Modell EM013, weibliche Zombie-Variante. Framedump zeigt eine Frau mit braunem Haar
  und gemustertem Kleid (`c_mess/walk6_re2/zoom_walk6.png`, `c_mess/walk7_re15b/kontakt_walk7.png`).
* Vorkommen, selbst nachgefahren: `python re15_port/tools/r35_zgirl/em_zensus.py 13` ->
  genau 4 Records: ROOM4050 und ROOM4051 main00 `@0x01eb4` (`44 00 13 00 00 01 00 a0 ...`,
  (-9900,0,1150), Kill-Flag 0xa0) und `@0x01f5c` (`... 7d ...`, (1600,0,4700), Kill-Flag 0x7d).
  Typen-Histogramm: `type 0x13: 2 Raeume: 4050 4051`. Die 36 `FEHLER`-Zeilen sind die
  4-Byte-Platzhalter-RDTs (im Dossier genannt).
* Raumname im Debug-Menue: `JUMP -> 405 PRIVATE ROOMS (ROOM4050)`; Bild Cut 9 = Schlafraum mit
  Etagenbett und Umbrella-Schild. Deckt sich mit "Fuer den Nutzer" im Dossier.

### 1b Portierung — echter Weg (Gang + Aktionstaste an der Tuer), eigene Laeufe

**Lauf `walk6_re2`** (Default-KI RE2): `RE15_DEBUG_JUMP=4050@gp RE15_PLAYER_POS=-9300,-24100,3072,0
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=90 RE15_INPUT_SCRIPT=U1.2,A0.1,W0.5,A0.1
RE15_FRAMEDUMP=60-600/30:f_ RE15_EXIT_AT=600#4050` (Leon steht 1400 vor Tuer 6 und LAEUFT hin):
```
state.log F90..F102  PL z -24100 -> -23168 (laeuft bis an die Wand)
debug.log [aot] DOOR FIRE slot=6 rect=(-9300,-22700,hw=1000,hh=500) target_cut=9 spawn=(-9350,0,-2600)
          [spawn-diag] Sce_em_set type=0x13 behavior=0x00 slot=0 pos=(-9900,0,1150) dir=512
          [enemy] RE2 EM013 loaded: 17 meshes, 15 bones, 31 clips -> slot 11
          [enemy] Hybrid EM13: RE1.5-Geometrie (15 Meshes) unter RE2-Rig (15 Bones, 31 Clips), 0 Kanten ohne Zuordnung
state.log F126 PL(-9350,-2600) [1 t=13 st=1 ss1=0 d=3788 @(-9900,1150,r512)] hp=98
          F294 gr=1 ss1=3 (Griff), Spieler-HP 100 -> 80 (F318) -> 60 (F354)
          F374-F389 weggestossen (ss2=8, ~2950 Einheiten = RE2-Schub 400/-30 je Bild,
                    enemy_ai_re2_zombie.c @0x80103F60-64 / @0x80103FCC), F390 liegt (ss1=5 g=80)
          F558 wieder Anlauf (ss1=1, d 3124)      475 Bilder mit t=13
```
Bilder `kontakt_walk6.png` (12 Bilder), `zoom_walk6.png` (F150 Anlauf, F300/F330 Griff, F420
liegt, F540 steht wieder auf).

**Lauf `kill6_re2`** (Tod + Kill-Flag, Default-KI): wie oben + `RE15_GIVE=8:7,22:30 RE15_EQUIP=8`,
Skript `...,M1.5,MA0.1,M1,...` (Zielen/Feuern), `RE15_FIRE_AOT=6@900#4050` (zweiter Eintritt):
```
F183 st=3 hp=-1 (ein Schrotschuss), F231 st=7 (Leiche), Leiche bis F891 im Bild
debug.log [fire-aot] slot=6 at F900 -> DOOR FIRE slot=6 -> [spawn-diag] Sce_em_set type=0x13 ...
          (die Diag-Zeile steht in scd_vm.c VOR dem Kill-Flag-Gate)
state.log ab F906: KEIN t=13-Aktor mehr  -> Gate @0x80042120-38 hat unterdrueckt
```
Bild `kontakt_kill6.png` (F160 Anlauf, F200 Sturz, F240/F400/F880 Leiche, F920 nach Wiedereintritt leer).

**Lauf `walk7_re15b`** (zweiter Ort, RE1.5-KI): `RE15_PLAYER_POS=-16000,-24750,2048,0
RE15_AI_FLAVOR=re15`, gleiches Skript, `RE15_EXIT_AT=470#4050`:
```
[aot] DOOR FIRE slot=7 rect=(-17450,-24750,hw=500,hh=1000) target_cut=14 spawn=(1550,0,-1150)
[spawn-diag] Sce_em_set type=0x13 behavior=0x00 slot=0 pos=(1600,0,4700) dir=1024
[enemy] EM13 loaded: 15 meshes, 15 bones, 42 clips -> slot 11
F126 INIT hp=80 (in 50..81), d 5837 -> 861 (F346 Griff), Spieler-HP 100 -> 90 -> 85 -> 80 -> 75
```
(Vorlauf `walk7_re15` ohne fruehes EXIT: F466 Fressen ss1=5, F486 Spieler-HP -1, Game Over.)
Bild `kontakt_walk7.png` (F190 Anlauf mit vorgestreckten Armen, F340-F430 Griff, F460 am Boden).

**Lauf `se6_re2`** (`SDL_AUDIODRIVER=dummy RE15_SE_DEBUG=1`): nach dem Spawn 6 SE-Stimmen aus der
Raumbank (se 0 zweilagig, 5, 3, 3, 8). Toene laufen; Klangtreue ohne Audiogeraet nicht hoerbar.

**ctest** `ctest --test-dir re15_port/build -R "r35_zgirl|zgirl" -V`: 10/10 gruen
(unit_zgirl_ai, unit_r35_zgirl_{zensus,killflag,tuer,ki_re15,ki_re2,tod,messer,selbsttueren},
integration_r35_zgirl 253.57 s: `A_tuer6: 361 Bilder, d 3788 -> min 386, Spieler-HP am Ende 60`).

**Urteil Punkt 1: erfuellt (gegen den Wortlaut).** Frage beantwortet, Gegner erscheint an beiden
Original-Orten ueber die echte Tuer, ist sichtbar/texturiert, greift an, macht Schaden, stirbt,
bleibt tot. Die Abnahme scheitert trotzdem — an der Nebenwirkung des Mittels (M1).

---

## Gates

### G1 Suite — erfuellt
Dossier traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 487/487` (>= 478). Beleg im Baum:
`re15_port/build/local_build_ctest.log` (05:57, nach der letzten Code-Aenderung; HEAD-Commit
0ab1e6f5 aendert nur das Dossier): `100% tests passed, 0 tests failed out of 487`, 1365 s.
`ninja: no work to do` bei meinem Bau -> Testbinaries = HEAD. Die volle Suite habe ich laut
Auftrag nicht erneut gefahren; die 10 zgirl-Tests selbst: gruen (oben).

### G2 @0x-Gate — erfuellt (Stichproben selbst disassembliert)
Diff-Konstanten: aot_common.c zitiert `@0x8001d968`/`@0x8001d988`; Tests zitieren je Erwartungswert
eine Adresse. Keine Treffer fuer deferred/tunable/interim/for now/faithful/plausibel/TODO in den
`+`-Zeilen. Selbst nachgelesen (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):
1. PSX.EXE `8001d930 lbu v1,10(a0)` / `8001d948 sh v1,4068(at)` (0x800b0fe4) / `8001d960 lbu v0,8(a0)`
   / `8001d968 beq v1,v0,0x8001d988` / `8001d980 jal 0x80039a30` / `8001d988 jal 0x800396fc`;
   im Lader `80039a00 jal 0x8003ef6c`. -> nur die Stage wird verglichen, Neuladen unbedingt. Stimmt.
2. PSX.EXE `80039710 lui v1,0xffff` / `80039728 and v0,v0,v1` / `80039730 sw v0,-13764(at)`
   (0x800aca3c). Stimmt (Ansprung-Bit wird je Raumladen geloescht). Set(1,31) opcode-exakt
   zensiert: nur ROOM5120/5121 `@0x1402` und ROOM5140/5141 `@0x1a5a` (`22 01 1f 01`). Stimmt.
3. STAGE4.BIN `8010abb4 ori v0,zero,0x14` / `8010abb8 jal 0x8001af20` / `8010abc0 andi v0,v0,0x1f` /
   `8010abcc addiu v0,v0,50` / `8010abd0 sh v0,154(v1)`; `8010aca4 andi v0,v0,0x1` / `8010aca8 beq`;
   `8010a934 addiu a0,zero,-10` / `8010a96c jal 0x8001bd60` / `8010a970 ori a1,zero,0x14`. Stimmt.
4. Test-Erwartungen: STAGE1 `8010277c addiu v0,v0,-10` + `801027dc addiu v0,v0,-5` auf player.hp
   (0x800acaee), STAGE4 dieselben bei `80102730`/`80102790`; PSX.EXE `80042120 lbu a1,7(a1)` /
   `80042128 beq a1,0xff` / `80042130 jal 0x8004efe4` / `80042138 beq v0,zero`; `800124bc sb fp,5(s1)`.
5. Tabellen STAGE4: Zustand `@0x80119730` = {0x8010aae0, 0x8010b228, 0x8010bf34, 0x8010bfc8,
   0x80109150, 0, 0, 0x80109508}; Todes-Master `@0x80119b64` Zeile Waffe 8: [64]=0x80108a70,
   [65]=0x80107e94 (= STAGE1 0x80107ee0 - 0x4c). Maedchen- gegen Standard-Master (Treffer und Tod)
   wortweise: GENAU 2 Unterschiede je Tabelle, Waffe 1 Richtung 0/1 = 0 beim Maedchen. Stimmt (R3).
6. `ovl_reloc_diff.py` nachgefahren: 1609 Woerter = 1551 gleich + 45 imm + 13 j/jal (alle -0x4c);
   eigene Pruefung der imm-Deltas: 44x addiu + 1x lw, alle -27352 (= -0x6ad8). Geteilt: 10537 =
   10247 + 127 imm (126x -0x6ad8, 1x -0x6a94) + 163 j/jal (-0x4c). Stimmt (M3).
7. R5 (OFFEN O1 ohne Wirkung): ROOM4050-SCA (RDT+0x20 -> 0x78c) selbst geparst: 5 x 82 = 410
   Zellen, Typwort `u1|floor<<8` bei allen 410 = 0x0300, Bit 0x2 nirgends. `FUN_8001bd60` testet
   genau dieses Bit (`8001bdb0 andi v0,a1,0x2` / `8001bdb4 beq`). Stimmt.
Kleinigkeit im Dossier (kein Mangel): R2 nennt `@0x8011978c` als DECIDE-Tabelle; derselbe Wert ist
Eintrag [13] der 16er-Modustabelle `@0x80119758` (`andi 0xf`) — die Tabellen ueberlappen, das
Dossier sagt das sinngemaess ("[13..15] zeigen in geteilten Code").

### G3 Fix erklaert den Befund — erfuellt
Alte Bedingung (Merge-Basis aot_common.c:797-800) `dest_room != 0 && (0x1000|room<<4|var) ==
g_current_room_id` ist fuer 0x4050 nie wahr (0x1050 != 0x4050) -> kein `g_scd_pending_scenario`
-> kein `scd_room_reenter` (einziger Verbraucher game_step_common.c:2139) -> main00-Switch auf
work_vars[0x0A] laeuft nie mit 9/14. Mit U1 laeuft er (Laeufe oben). Ursache und Wirkung passen.

### G4 Pfad-/Vertrags-Gate — erfuellt
`git diff master...HEAD --name-only`: kein release/, platform/android/, shared_assets/PSX/, keine
Edits an tests/unit/CMakeLists.txt oder tests/integration/CMakeLists.txt. Gemeinsamer Code: eine
Zeile in aot_common.c mit Kommentar "Runde 35 Spur C". Keine Bank-9-Bits, Nachrichten-IDs,
AOT-Slots oder Ereignisse belegt. Neue Tests ueber `tests/unit/probes/r35_zgirl.cmake`.
VERTRAG.md +9 Zeilen = Orchestrator-Hinweis (Commit e7e7131c).

### G5 Tests — vorhanden und fuer das Maedchen messend; fuer U1 ausserhalb ROOM4050 blind (M2)

### G6 Regressionsfreiheit der Fix-Zeile — NICHT erfuellt (M1)

---

## Maengel

### M1 (blockierend) — U1 bricht den Endkampf ROOM5090: der finale Birkin stirbt beim Kampfstart von selbst
**Was passiert:** U1 laesst jede Selbst-Tuer in Stage 2..6 neu einsteigen. ROOM5090 ("509 TRAIN CAR
ABC") hat vier Selbst-Tueren (main00 `@0x0104E` Slot 0 -> Cut 9, `@0x0108E` Slot 2 -> Cut 14,
`@0x010AE` Slot 3, `@0x010CE` Slot 4). Beim Wiedereintritt laeuft sub00 neu und spawnt den Boss
erneut (sub00 `@0x0124A` `44 01 30 33 ...`, Typ 0x30 -> 0x36, Kill-Flag 0xff, gegated nur durch
Ck(3,42)==0 `@0x01242`). `op_sce_em_set` setzt `a->hp = 0` (scd_vm.c:3858). Der G5-Konstruktor,
der `e->hp = 600` setzt (enemy_ai_boss_g5.c:1447), laeuft aber nur bei
`if (s_g5_slot != slot || !g->aktiv)` (enemy_ai_boss_g5.c:1424) — der Boss landet wieder in
Slot 2 und das Modul ist noch aktiv, also bleibt HP 0. Beim Kampfstart (sub04
Member_set(0x0c,0x13) `@0x130A`) greift der Todes-Trigger `if (e->hp <= 0 && g->routine != 3)`
(enemy_ai_boss_g5.c:1538): Routine 3 = TOD.

**Warum das jeden Durchlauf trifft:** Der einzige Weg nach ROOM5090 ist ROOM6030/6031 main00
`@0x00c92` Slot 3 -> ROOM5090 Cut 4 an (27200,0,4900), also im noerdlichen Wagen. Der Boss steht
im suedlichen Wagen. Dorthin kommt man nur ueber die Selbst-Tueren 0 (Nord -> Mitte) und 2
(Mitte -> Sued). Jeder Spieler steigt vor dem Kampf also zweimal neu ein.

**A/B-Messung, identische Eingabe** (B = Basis-exe: derselbe Baum, nur aot_common.c auf
`154a73c1` zurueck, gebaut in `scratchpad/base_build`, Lauf mit `RE15_CD_ROOT`/`RE15_RE2_ASSET_ROOT`
auf die Assets dieses Baums):
```
Echter Tuerweg: RE15_DEBUG_JUMP=5090@gp RE15_PLAYER_POS=500,5125,2048,0, Skript A0.1,W1,U12
 U1   c_mess/o3_5090_echt        DOOR FIRE slot=0 ... target_cut=9 -> 2. Sce_em_set type=0x36
                                 F61 hp=600 -> F121 hp=0
 Basis c_mess/basis_o3_5090_echt DOOR FIRE slot=0 ... target_cut=9, KEIN 2. Sce_em_set
                                 F61 hp=600 -> F121 hp=600
Kampfstart: RE15_DEBUG_JUMP=5090@gp RE15_FIRE_AOT=2@200#5090, Skript U12 (Start 260) im Suedwagen
 U1   c_mess/o3_5090_kampf       F201 hp=0; F445 cam=12 g=13 (Kampf beginnt) hp=-1 mo=6,
                                 F520 mo=7, F701 mo=8, ab F801 mo=10; Boss bleibt bei x=-9000.
                                 Leon feuert keinen Schuss.
 Basis c_mess/basis_o3_5090_kampf F201 hp=600; F451 cam=12 g=13 mo=1 hp=600; Boss laeuft an
                                 (x -9000 -> -7373 F551 -> -1302 F951), Clips mo 1 -> 3 -> 4,
                                 Leon wird versetzt (F701 PL z -23590 -> F751 z -25028)
```
Bild U1: `c_mess/o3_5090_kampf/kontakt_5090.png` (Boss steht im Hintergrund und bewegt sich nicht;
dass er die Todesclips spielt, belegt state.log ueber mo/hp, nicht das Bild).

**Was fehlt / naechster Schritt:** Das Original laedt den Raum neu, also laeuft auch die INIT des
Bosses neu (volle HP). Der Port muss beim Wiedereintritt (scd_room_reenter bzw. beim
Neuspawn von Typ 0x36 in 0x5090) den G5-Modulzustand zuruecksetzen (`s_g5`/`s_g5_slot`,
Tentakel-Reset wie im Konstruktor). Danach mit genau den zwei Laeufen oben nachmessen: U1-exe muss
bei F121 hp=600 zeigen und beim Kampfstart anlaufen statt zu sterben. Pin dazu (M2).

### M2 — Tests sichern U1 ausserhalb ROOM4050 nicht ab
`unit_r35_zgirl_selbsttueren` (T8) feuert die 60 Selbst-Tueren, ruft `scd_room_reenter` und tickt
danach NUR `scd_vm_tick()` 60 Mal (test_r35_zgirl.c:437-438). Weder `re15_game_step` noch die
Gegner-KI noch raumeigene Port-Module (G5, Tentakel usw.) laufen — M1 bleibt darin gruen. Dossier
O3 sagt selbst, dass 44 der 46 neuen Tueren nicht am Bild abgenommen sind. Gefordert: (a) Zensus,
welche raumgebundenen Port-Module mit statischem Zustand in den 46 betroffenen Raeumen haengen
(ROOM2040, 20A0, 30E0, 4000, 4050, 40A0, 5090, 6030/6031) und ob sie einen Wiedereintritt ohne
Raumwechsel ueberstehen; (b) ein exe-Pin fuer ROOM5090 (Selbst-Tuer, danach Boss-HP 600 und Kampf
laeuft an) nach dem Muster von integration_r35_zgirl.

### M3 (klein, nicht blockierend) — widerspruechlicher Kommentar
aot_common.c:794-796 sagt weiter "BEWUSST NICHT verallgemeinert: Stage >= 2 ... gehoert in eine
eigene Runde mit eigener Messung", direkt darunter folgt die Verallgemeinerung. Den alten Satz
umschreiben. Die damalige Warnung hat sich mit M1 bestaetigt.

### Hinweis fuer die Zusammenfuehrung (kein Mangel dieser Abnahme)
O1: Spur H fuehrt `re15_schwerkraft_seed` (trage_1200.c auf r35/raeume) nur fuer die Typen
0x10/0x11/0x12/0x16/0x18. Wird der Aufruf fuer 0x13 nachgezogen, gehoert 0x13 auch in den Seed
(`dog_floor_y`/`fall_1c0`, @0x80042210). In ROOM4050 hat beides keine Wirkung (G2 Nr. 7).
O2: Die Kriecher-Todeszeile fehlt zombieweit. Sichtbar wird das nur mit `RE15_RE15_RE2Z_IMPORT=0`.
Im Default (RE2-KI) und mit RE1.5-KI + Import (Default AN, Nutzerauftrag 2026-08-20) zerlegt der
RE2-Zerleger auch 0x13 (`re15_re2z_owns_type`, enemy_ai_re2_zombie.c:186-190).

---

## Zusammenfassung
| Pruefung | Urteil |
|---|---|
| Punkt 1 (Wortlaut) | erfuellt — Typ 0x13 erklaert, Spawn an Tuer 6 und Tuer 7 ueber die Aktionstaste, KI RE2/RE1.5, Tod + Kill-Flag |
| G1 Suite | erfuellt (487/487 laut Log, zgirl 10/10 selbst gefahren) |
| G2 @0x | erfuellt (7 Stichproben stimmen) |
| G3 Fix erklaert Befund | erfuellt |
| G4 Pfad/Vertrag | erfuellt |
| G5 Tests | fuer das Maedchen ja; fuer U1 in Stage 2..6 blind (M2) |
| G6 Regression | NICHT erfuellt — M1: Endkampf ROOM5090 kaputt |

bestanden = false.
