# Runde 37 Spur L "re2zombie" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r37_re2zombie`, Zweig `r37/re2zombie`, gepruefter Stand `ab2bf2b9` (Basis `e399ee3f`).
Abnahme 2026-10-13/14. Alles unten ist SELBST gemessen bzw. selbst disassembliert; Behauptungen des Bau-Agenten sind nur
dort uebernommen, wo es ausdruecklich steht.

## Ergebnis

| # | Punkt (Wortlaut, AUFTRAG.md) | Urteil | Kern-Beleg |
|---|---|---|---|
| 1 | "die Zombies sind immer noch zu passiv ... nach Irons tod ... bleiben einfach nur Stumpf alle im Gang stehen ... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang" | **erfuellt** | echte Tuer 1150 -> 1130 nach der Montage (Laeufe I, a1, a2, v1): alle fuenf gehen ab Bild 6 auf Leon zu; mit Spur L heben sie bei +0x1F0 1936..1995 die Arme (1/2), greifen aus 0x201 (1108..1193) und beissen (Leon-HP 100 -> -20, Tod F852 beim Gehen); Bild 3.5 zeigt die erhobenen Arme |
| 2 | "das soll nicht nur da gelten, das darf generell für die Zombies gelten" | **erfuellt** | Leon 1180 (a4), **Elza** 1181 (a3, PL04), Harness 1010 (Normalspiel) und 1040 (Montage): 0x201-Wechsel vor Leon, Griffe aus 0x201; Mechanismus sitzt im gemeinsamen RE2-Executor EXEC[1] (jeder RE2-Zombie). Vorbestehende Navigator-Tasche 1030 = Hinweis H1 |
| 3 | "schaue einfach nach wie Resident Evil 2 das macht" | **erfuellt** | EMZ0.BIN selbst disassembliert (Abschnitt 1); RE2-Original-Rohdaten selbst ausgezaehlt (Abschnitt 2): Arme hoch 1999/1987 (2) bzw. 3565 (1), Griff 1180/1152 — Port 1936..1995 / 4283..4692 / 1108..1193 |
| 4 | Orchestrator-Auftrag L: (a) drei 0x201-Produzenten nach Aufklaerung des Nahkampf-Zusammenbruchs, (b) DECISION[0] Block 3 bei (rand&1)==1, (c) EXEC[0] P1 Wecker im Wurf-Takt / P2 im naechsten; Zensus; exe: Arme hoch, auf Leon zu, greifen, beissen; Nahkampf nicht schlechter | **erfuellt** | (a)(b)(c) + DECISION[12] Zeile fuer Zeile gegen EMZ0.BIN; Maske-0-Gegenprobe = Basis; an der exe alle drei Produzenten (2) 1991, (1) 4283/4692, (3) 2346 bei rennendem Leon und der Griff aus dem Ausfall (D12) beobachtet; Positiv-Kontrolle Messer 368 -> 369/373 (kein Zusammenbruch, Modal 0x). An der exe weniger Messertreffer je Anmarsch (6 gegen 10 in vier Paaren) — Ursache = schnellerer RE2-Arme-hoch-Gang, byte-true; die Nutzer-Aussage dazu stimmt nicht -> **M1** |

Gates: RE-Gate haelt (alle Stichproben stimmen, keine Guess-Tells, keine Env-Schalter in der Engine); Vertrag haelt (Pfade,
keine Ressourcen); Tests: 9/9 + Integration 1/1 der Spur, Unit-Auswahl 231/231, Integrations-Auswahl 28/28.

**bestanden: NEIN** — alle Punkte erfuellt, aber **M1**: eine Aussage im Abschnitt "Fuer den Nutzer" des Dossiers widerspricht
der Messung an der exe (reine Textkorrektur, kein Verhalten; gleicher Massstab wie Abnahme H#0 M2).

## Maengel

* **M1 — Dossier `L_re2zombie.md`, Abschnitt "Fuer den Nutzer", 4. Punkt: "Nahkampf/Messer: unveraendert wirksam (gemessen 368 ->
  373 Treffer im Messer-Sweep)".** Gemessen an der exe (Abschnitt 3.6, vier Paare Spur L / Basis mit identischer Eingabe, Leon
  steht mit Messer an der Tuer 1130): der erste anrueckende Zombie nimmt mit Spur L **6** statt **10** Messertreffer, bevor er
  greift (1/2, 2/2, 1/2, 2/4). Mechanismus (state.log k_L_20/k_B_20): nach jedem Treffer geht der Zombie per Produzent (2)
  sofort wieder in den Arme-hoch-Gang und schliesst schneller (Spur L: Erholung F1208, Griff F1214 bei 1173; Basis: Erholung
  F1193 in den langsamen Gang, zweiter Treffer F1201, Griff erst F1224 bei 851). Die Treffer-
  pruefung selbst ist unveraendert (Harness 368 -> 369/373) und das Verhalten ist byte-true RE2 — aber "unveraendert wirksam"
  stimmt fuer das, was der Nutzer an der exe erlebt, nicht. Korrektur: Satz an die exe-Messung anpassen (Messer trifft wie
  vorher; weil die Zombies mit erhobenen Armen schneller heranruecken, bleiben je Anmarsch weniger Schlaege, gemessen 6 statt 10
  in vier Laeufen — wie in RE2). Nachpruefbar: Laeufe `k_L_20`/`k_B_20`, `k_L_21.3`/`k_B_21.3`, `a5_1130_messer`/`b5_1130_messer` (W22), `k_L_23.7`/`k_B_23.7`
  (Abschnitt 3.6), je state.log Platz 5 HP.

## Hinweise (kein Mangel dieser Spur)

* **H1 — Navigator-Tasche ROOM1030 (vorbestehend):** Slot 1 (Typ 0x16) geht im Fall 1030<-1050 "hin" ab F780 bis Laufende F1200 im Gang
  auf der Stelle an der Wand (-6144..-6222,-24838), Steuerziel (-4984,-23860); Zensus-Mass "still": Basis 600, Spur L 330. Der Riegel
  `unit_r36_zombieki_eintritt` zaehlt den Fall jetzt per Gegenlauf (Maske 0) als "vorbestehend" statt als KLEMME — das ist
  differenziell sauber, laesst die sichtbare Passivitaet aber offen. Gehoert in eine eigene Spur (Navigator, nicht Pfad L).
* **H2 — `probe_r36_blut` laeuft seit Spur L im Strom bis Runde 37 (Maske 0):** die neun Blut-Pins pruefen damit nicht mehr die
  ausgelieferte KI. Der dabei gefundene Anker-Versatz im Aufsteh-Treffer (46/67 neben dem Kopf, Port-Konvention "Pose des
  laufenden Bilds" gegen Original Record+72 des Vorbilds, Blend-Byte `sb 15,334` @0x80107B3C selbst bestaetigt) ist vorbestehend
  und im Dossier OFFEN — fuer Spur C/G.
* **H3 — PIPE (Waffe 2, Nahkampf):** im Harness 322 -> 258 Treffer (~1,6 sigma); im Spiel nicht erreichbar (kein Item_aot_set mit
  0x02 in allen RDTs, nicht im Start-Inventar).
* **H4 — `PL+0x8 == 15`-Ausgang von DECISION[12]** (@0x8010468c-9c) ist im Port weggelassen mit derselben Begruendung wie
  fill_gates (Spieler-Id nie 15) — nicht selbst gemessen.
* **H5 — der historische Zusammenbruch 3523 -> 0 an 187fd244** ist nicht selbst nachgebaut; selbst bestaetigt sind 182db277
  (Modal-Leerfahren in `test_re2_zombie_abc` frame()) und dass der heutige Stand mit allen Produzenten nicht einbricht.

## 0. Bau und Grundlage

* `bash re15_port/tools/local_build.sh configure` + `... build` -> `=== LOCAL-BUILD-OK (build)` ("ninja: no work to do";
  Zeitstempel geprueft: `enemy_ai_re2_zombie.c` 2026-10-12 15:11:22, `.obj` 15:22:04, `re15_pc.exe` 15:22:05 — die exe
  enthaelt den letzten Code-Stand; nach 6c1ec5aa gab es keine Code-Aenderung an der KI mehr, nur Tests/Dossier).
* Code-Diff `git diff e399ee3f -- re15_port/engine re15_port/include` gelesen (402 Zeilen): nur
  `engine/src/enemy_ai_re2_zombie.c` (Produzenten (1)(2)(3), (b), (c), DECISION[12], Sonden-Maske, Kommentar-Zensus) und
  NEU `include/re15_re2zombie_r37.h` (Sonden-Schalter). Keine weitere Engine-/Plattform-Datei.
* Maske 0 = Basis: selbst am Code geprueft — Produzent (1) wuerfelt mit und ohne Maske gleich (nur der Commit haengt am Bit),
  (b) ohne Bit = Commit bei `== 0`, (c) ohne Bit = `if (sub_state_2 == 1)` mit identischem Rumpf wie e399ee3f (auch der Fall
  `t158 == 0 && !(dist && los)` endet in beiden mit `return`), D12 ohne Bit = `break`. Die Zaehler/`s_r37_letzter` wirken nicht
  auf Verhalten. Die Gegenproben "Maske 0" der Sonden sind also echte Basis-Gegenproben.

## 1. RE-Gate — selbst disassembliert (re2_disasm.py, RE2 `info/re2leon/COMMON/BIN/EMZ0.BIN` bzw. `info/re2leon/PSX.EXE`)

| Stelle im Diff | selbst gelesen | Urteil |
|---|---|---|
| (1) Timer @0x80101D68-DBC | `80101d68 lhu v0,346(s1)` / `addiu -1` / `sh` / `sll 16` / `bne -> 0x80101dc0` / `jal 0x80015fe8` / `sra 3` / `addiu 100` / `jal 0x80015fe8` (Delay `sh v0,346`) / `andi 0x3` / `bne` / `lw v0,496` / `sltiu 0x1388` / `beq` (Delay `addiu v0,zero,513`) / `80101dbc sw v0,4(s1)` | stimmt mit `re2z_exec_walk` (Reseed-Draw, 2. Draw `&3 == 0`, `ai_dist < 0x1388`, 0x201) |
| (2) Nahbereich @0x80101EC4-F10 | `lw 496` / `sltiu 0x7d0` / `beq -> 0x80101f14` / a1,a2 = 0x800cfc30/0x800cfc38 / `jal 0x80015614` (a3 = 512) / `bne` / `lbu 0x800cfdcb` / `andi 0x80` / `bne` / `addiu 513` / `80101f10 sw v0,4(s1)` | stimmt (Kegel 512, unbeansprucht = `!re15_player_is_grabbed()`, wie die vorhandenen Block-G-Zwillinge) |
| (3) Rennen @0x80101F14-F60 | `lhu 0x800cfbf6` / `andi 0x4` / `beq -> 0x80101f64` / `sltiu 0x9c4` / `jal 0x80015614` (a3 = 1324) / `bne` / `addiu 513` / `80101f60 sw` | stimmt; Reihenfolge (1) vor DoT, (2)(3) danach ohne Ruecksprung — wie Port |
| (b) DECISION[0] Block 3 @0x801013c8 | `801013bc jal 0x80015fe8` / `andi 0x1` / `801013c8 beq v0,zero,0x801013d4` / `addiu 3073` / `sw` — Block 2 dagegen `8010136c bne v0,zero,0x80101380` | stimmt: Block 3 Commit bei `== 1`, Block 2 bei `== 0`; Block 2 kehrt nicht zurueck, Block 3 laeuft auch nach einem Block-2-Commit (Port: zwei aufeinanderfolgende `if`) |
| (c) EXEC[0] P1 @0x80101494-5A0 | Moan-Zweig `801014b4 bne v1,zero,0x80101500` (Delay `sh` = Dekrement), Reseed `801014f8-fc`, `80101500-18` (+0x14E-Gate `jal 0x80016200`), `8010151c lh 346` / `bne -> 0x80101544` / Wurf `andi 1` / `bne` / `80101540 sb v0,6(s0)` **ohne Sprung** -> `80101544 lh 344` (Wecker) / `sltiu 0x1d4c` / `andi 0x800` / `8010157c sw 257` / `80101588 beq v0,zero,0x801016ac` / `8010159c j 0x801016ac` (Delay `sh` Dekrement); Phasen-Dispatch `80101410-50` (+0x6 == 2 -> 0x801015a4) | stimmt: Wecker im Wurf-Takt, P2 erst ueber den Dispatch im naechsten Takt; Port-Rumpf deckt alle Wege |
| DECISION[12] @0x8010460C | `table 0x8010c88c 16`: [12] = 0x8010460c (die anderen wie im Dossier); Rumpf: `a1 = 0` (Delay @0x80104638), `bne +0x6,3`, `sltiu a0,0x514`, `lbu 0x800cfdcb & 0x80`, `lbu 333 < 0xb`, `lbu 262` == `lbu 0x800cfcfe`, `8010469c sltu a1,zero,(PL+0x8 ^ 0xf)`, `801046a0 beq a1,zero -> Epilog`; Sektor 1 `lhu 538 & 0x20` / `jal 0x80015758(self+0x38, PL+0x38, +0x76+256, 256)` / `sw 769` / `PL+0x1D3 |= 0x80`; Sektor 2 `& 0x40` / `+0x76-256` / `sw 769` / `|= 0x80` | stimmt; der `PL+0x8 == 15`-Ausgang ist im Port weggelassen (Begruendung "Port-Spieler traegt nie 15", wie fill_gates — uebernommen, nicht selbst gemessen) |
| Test-Zitate | PSX.EXE `80015f04 lbu v0,333(t0)` / `bne` / `80015f14 sw zero,524(t0)` / `80015f1c sh zero,528(t0)`; EMZ0 `80107b38 addiu v0,zero,15` / `80107b3c sb v0,334(s1)`; `80108cf4/cfc jal rand` x2, `80108d08 srav` (Wort 0x00508007), `andi 3`, `80108d10 beq s0,zero`; `80101b20 sh v0,346(s1)` -> `80101b24` ohne Sprung (EXEC[1] P0 faellt nach P1) | alle stimmen |

Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|vorerst|ungefaehr|geschaetzt|approx`): keine
Treffer in hinzugefuegten Zeilen. Env-Schalter: nur `RE15_ASSET_DIR` in der Testquelle (Asset-Pfad), keiner in der Engine.
Die Maske in `re15_re2zombie_r37.h` ist ein Sonden-Schalter ohne Env-Weg (Standard = alles an); ihre Bits sind
PORT-WAHL (Testinfrastruktur), die Bedeutungen tragen @0x.

## 2. RE2-Original-Messung des Bau-Agenten — Rohdaten selbst ausgewertet

`L_belege/re2_original_room20a0_jeder_takt.txt.gz` (5400 Proben, 6 Zombies x 900 Takte) mit eigenem Zaehler gelesen:
Uebergaenge `(1,1)->(1,9)` 5x, `(1,9)->(1,5)` 5x, `(1,1)->(1,2)` 3x, `(1,5)->(1,1)` 3x, `(1,2)->(1,3)` 2x, `(1,3)->(1,5)` 2x,
`(1,2)->(1,1)` 1x. Die drei Wechsel in den Arme-hoch-Gang: Takt 421 `8014777c` +0x1F0 1999, Takt 696 `8016634c` 3565
(+0x15A danach 125 = Reseed, Produzent (1)), Takt 773 `80156d64` 1987; Griffe aus 0x201 bei 1180 (Takt 437) und 1152 (Takt
776); der Rueckfall 1/2 -> 1/1 (Takt 777) bei `0x800CFDCB = 0x80` (Spieler beansprucht). Deckt sich mit dem Dossier M2.

## 3. Messungen an der echten exe (eigener Treiber, eigene Messkopie `re15_pc_abnL_<lauf>.exe`, nur eigene PID)

Treiber `abn_lauf.sh` (Scratch): Karte per `probe_r36_gegnerzahl_karte` bzw. `probe_r36_elza_karte`, `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1`,
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60`, `RE15_STATE_LOG=state.log`, `RE15_EXIT_AT`. Auswertung mit
EIGENEM Auswerter `abn_state.py` (je Platz: Weg, Abstand zu Leon Start/Ende/min, Zustandsanteile, jeder Wechsel st/ss1 mit
+0x1F0). Montage-Karte (wie Spur H/L): `1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca`, Weg in
den Gang = ECHTE Tuer (`W1,A0.2,W2` = Aktionstaste vor der Tuer 1150 -> 1130, debug.log `Nachspawn 1130 (Programm 10)`).
Gegenprobe "Basis" = exe des abgenommenen Spur-H-Baums (`r37_zombieaktiv`, gebaut 2026-10-12 14:31:49; Code = e399ee3f bis auf
Kommentare in irons_tod_1150.c / re15_zombieaktiv_r37.h, `git diff e399ee3f d1b4b4f2 -- re15_port`), als Messkopie im Ordner
der L-exe (gleiche Assets).

### 3.1 Nutzer-Beispiel: ROOM1130 nach Irons' Tod (Montage), echte Tuer

| Lauf | Leon | Spur L: Wechsel nach 1/2 (+0x1F0) | Spur L: Griffe / Biss | Basis (H) |
|---|---|---|---|---|
| I (ctest `integration_r37_re2zombie`, selbst gefahren, 51,5 s) | steht 1100 Bilder | Platz 5 (Typ 0x11) F869 bei **1991** -> F888 Griff (1/3, 1108) | Leon-HP 100 -> 70 (F909) -> 40 (F947) | — |
| a1 `W1,A0.2,W2,R0.33,U25` | geht den Gang hinunter | Platz 5 F458 **1936** -> Griff F474 (1193); Platz 4 F772 **1978** -> Griff F793 (1180); Timer-Faelle Platz 3 F707 **4692**, Platz 2 F818 **4283** (Produzent (1), < 0x1388) | HP 100 -> 70 -> 40 -> 10 -> -20 (Tod F852) | b1: KEIN 1/2; Griffe direkt aus 1/1 bei 1173/1193/1187; Tod F1043 |
| a2 `...,R0.33,UX25` | RENNT | Platz 5 F343 / Platz 4 F346 / Platz 1 F474 je **2346** (< 0x9C4, Leon rennt = Produzent (3)); Platz 3 F461 2442 | Ausfall aus dem Arme-hoch-Gang mit **Griff aus dem Stoss** (DECISION[12]): Platz 3 F471 1/2 -> 1/12, F480 Phase 3 Clip 27 (0x1B) Bild 5, F481 +0x1F0 1280 < 0x514 -> 1/3 (Griff) | b2: kein 1/2, kein 1/12->1/3; Griffe aus 1/1 |

Alle fuenf Montage-Zombies gehen in beiden Staenden ab Bild 6 auf Leon zu (Weg 18000-45000 in 1100 Bildern; das ist Spur H).
Spur L fuegt den RE2-Arme-hoch-Gang (1/2) mit Griff aus 0x201 und den Griff aus dem Ausfall hinzu — an der Basis-exe tritt
beides nie auf. Bild der exe: `L_belege/exe_1130_arme_hoch_F864_F872_F880.png` (Bau-Agent) angesehen: nicht Teil meiner Wertung.

### 3.2 "generell": Normalspiel Leon ROOM1180 und Elza ROOM1181

| Lauf | Ergebnis (Spur L) |
|---|---|
| a4 Leon ROOM1180 (-4729,-16018), steht 900 Bilder | Platz 3 (Typ 0x10): F283 1/1 -> 1/2 bei **1995**, F311 Griff (1193), F414 1/5 (Abwurf), F728 zurueck in den Gang, F802 wieder 1/2 bei **1991**, F836 Griff (1180); Leon-HP 100 -> 80 -> 60 -> 40 -> 20 |
| a3 **Elza** ROOM1181 (debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`), steht 900 Bilder | identisch zu a4 (gleiche Raumdaten 1180/1181, gleicher Strom): 1/2 bei 1995 / 1991, Griffe bei 1193 / 1180, Elza-HP 100 -> 20 |

### 3.3 Vergleich mit dem RE2-Original (Bau-Agent-Rohdaten, selbst ausgewertet, Abschnitt 2)

| | RE2-Original ROOM20A0 | Port Spur L (exe) |
|---|---|---|
| Wechsel 1/1 -> 1/2 durch (2) | +0x1F0 1999 / 1987 | 1991 / 1936 / 1978 / 1995 / 1991 |
| Wechsel durch (1) Timer | 3565 | 4692 / 4283 |
| Griff aus 1/2 | 1180 / 1152 | 1193 / 1180 / 1108 / 1193 / 1180 |
| Rueckfall 1/2 -> 1/1 bei beanspruchtem Spieler | 1x (Takt 777, 0x800CFDCB = 0x80) | a1: Platz 2 F818 -> F819 waehrend Platz 4 Leon haelt (F793-F852) |

### 3.4 Weitere Raeume im Harness (`test_r37_re2zombie gang <maske> <raum> <x> <z> <gier> 900 <montage> 0`, echter Raumeintritt + SCD)

| Raum | Maske 0 (Stand bis Runde 37) | Maske 63 (Spur L) |
|---|---|---|
| 1010 Leon (Eintritt aus 1020, (3650,6900) Gier 2048, Normalspiel) | 2 Zombies, kein 0x201, Weg/Takt 0x101 25,1, Spieler 4x gegriffen | 4 Wechsel nach 0x201 (alle (2), mittl. 1704), Weg/Takt 0x201 39,4, 2 Griffe aus 0x201, Spieler 3x gegriffen ab Bild 94 |
| 1040 nach der Montage ((-20083,-3559), Montage-Bits) | 5 Zombies, kein 0x201, Spieler 2x gegriffen ab Bild 527 | 2 Wechsel ((1) 1, (2) 1), Weg/Takt 0x201 34,1, 2 Griffe aus 0x201, Spieler 2x gegriffen ab Bild 513 |

### 3.5 Bild: hebt der Zombie die Arme? (exe, BESCHLEUNIGTER Renderer, `RE15_FRAMEDUMP=856-888/8:fd_`, `RE15_WINDOW_SCALE=1`)

Lauf v1 = Integrationsweg (Montage-Karte, echte Tuer, Leon steht), OHNE `RE15_SOFTWARE_RENDER`. state.log: Platz 5 (Typ 0x11)
F869 1/1 -> 1/2 bei 1991, F888 Griff (1108) — bitgleich zu Lauf I. Ausschnitt (Scratch der Abnahme, nicht eingecheckt — nur dieser Bericht wird committet) `laeufe/v1_1130_bild/fd_zoom.png`
(Bilder 856 / 864 / 872 / 880 / 888): 856 und 864 Arme haengend (Gang), 872 Arme gehen nach vorn, 880 Arme waagerecht nach
vorn auf Leon gerichtet, 888 Griff an Leon. Der Arme-hoch-Gang ist im gezeichneten Bild sichtbar.

### 3.6 Messer an der exe, Paare Spur L / Basis (gleiche Eingabe, R1 + Schlag alle 0,5 s, Schlagphase verschoben)

Leon steht an der Tuer 1130 (`RE15_EQUIP=1`, debug.log `[equip] RE15_EQUIP -> item 1`), erster anrueckender Zombie Platz 5
(Typ 0x11, HP 250, Schaden je Messertreffer 3):

| Skript-Versatz | Spur L: HP Platz 5 / Treffer vor dem Griff | Basis: HP / Treffer |
|---|---|---|
| W20 | 247 / 1 | 244 / 2 |
| W21.3 | 244 / 2 | 244 / 2 |
| W22 (a5/b5) | 247 / 1 | 244 / 2 |
| W23.7 | 244 / 2 | 238 / 4 |
| Summe | **6** | **10** |

Mechanismus (state.log k_L_20 / k_B_20 Bilder 1150-1240): Spur L — F1180 Arme hoch (1995), F1186 Messertreffer (2/1), nach der
Treffer-Erholung F1208 SOFORT wieder 1/2 (Produzent (2), +0x1F0 1487 < 0x7D0, im Kegel), Griff F1214 bei 1173 — der naechste
Schlag (F1222) kommt zu spaet. Basis — nach der Erholung F1193 1/1 (langsamer Gang), zweiter Treffer F1201 bei 1419, Griff erst
F1224 bei 851. Die Trefferpruefung des Messers selbst ist unveraendert (Harness 368 -> 369/373, Abschnitt 4); weniger Treffer
je Anmarsch, weil der Zombie im Arme-hoch-Gang schneller schliesst (RE2-Original 44,1 gegen 32,2 je Takt, Abschnitt 2/M2).

## 4. Nahkampf ("der Nahkampf wird nicht schlechter")

* `ctest -R "^unit_r37_re2zombie_nahkampf$|^unit_re2_zombie_abc$" -V`: `test_re2_zombie_abc: OK (Pistole: 1775 Treffer / 90 Tode /
  DOWN 94..323 ueber 254 Fenster; Messer: 373 Treffer)`; `ok: Nahkampf nicht schlechter als der Stand bis Runde 37 (Maske 0): 373
  gegen 368 (>= 95 %)`.
* Sonde selbst gefahren (`test_r37_re2zombie mess <maske> 64 <waffe>`, Aufbau = test_re2_zombie_abc):
  Messer (1) Maske 0 = 368 / Maske 63 = 369 Treffer (je Seed: Mittel der Differenz +0,02, sd 5,64, 24 besser / 7 gleich / 33
  schlechter); Pistole (3) 1795 -> 1775, Tode 96 -> 90; Item-Modal oeffnet sich mit Spur L nur 0x (Messer) — kein Einfrieren.
  Mit dem heutigen Modal-Leerfahren (182db277, selbst gelesen: `frame()` faehrt `re15_item_modal_active()` leer) gibt es
  keinen Zusammenbruch. Den historischen Zusammenbruch an 187fd244 habe ich NICHT selbst nachgebaut (Dossier M1).
* Nebenbefund: Waffe 2 = PIPE (Nahkampf, Bank W02, enemy_ai_re2_zombie.c Tabelle Z.4010) 322 -> 258 Treffer (Mittel -1,00 je Seed,
  se 0,63, ~1,6 sigma). Die PIPE liegt in KEINEM RDT (Item_aot_set-Zensus `bank9_zensus.py original`: Items 0x04..0x47, kein
  0x01/0x02) und ist nicht im Start-Inventar (`re15_inv_load_briefing`: Messer, Browning HP, Munition) — im Spiel nicht
  erreichbar; Hinweis, kein Mangel.
* exe (a5/b5, Leon an der Tuer 1130 mit `RE15_EQUIP=1`, R1 + Schlag alle 0,5 s): Spur L 1 Messertreffer am ersten Zombie
  (HP 250 -> 247) vor dem Griff, Basis 2 (250 -> 244). Vier Paare: Abschnitt 3.6.

## 5. Zensus der OFFEN-Marken (Auftrag L) — Stichproben selbst gelesen

* Tabelle 0x8010C88C: [3..11], [13..15] je `jr ra` / `nop` (alle zwoelf Adressen 0x801025e4 ... 0x80104e4c einzeln
  disassembliert) — nur [0], [1], [2], [12] sind echte Entscheidungen. Der Zensus-Fund DECISION[12] ist richtig.
* ACTIVE-Wurzel `801011a8 lbu v0,5(s0)` -> `jalr` DECISION (Tabelle 0x8010c88c), dann `801011d0 lbu v0,5(s0)` NEU gelesen ->
  `jalr` EXECUTOR (0x8010c8cc): ein in DECISION[12] geschriebenes 0x301 laeuft im selben Takt in EXEC[3] — wie der Port
  (`re2z_active`: switch, dann `re2z_exec_dispatch`).
* Fresser-Wecker RE1.5 `STAGE1.BIN 80103990 lw v0,464(v1)` / `80103998 sltiu v0,v0,0xfa0` und RE2 `EMZ0 80103900 addiu
  v1,zero,257` / `8010390c sw v1,4(s0)`: Zitate stimmen.
* Navigator-Tasche ROOM1030 (Zensus "wirkt ja", nicht geschlossen, eigene Spur vorgeschlagen) selbst nachgefahren:
  `test_r36_zombieki_eintritt spur 1030 1050 2 1 1 1200 30` -> Slot 1 ab F780 bis F1200 im Gang 1/1/1 (Clip 0) auf der Stelle
  bei (-6144..-6222, -24838), Leon (-6171..-6219, -22303) auf der anderen Seite der Tresen-/Wandzelle, `steer(-4984,-23860)`,
  +0x1F0 ~2530. `ctest unit_r36_zombieki_eintritt -V`: `[vorbestehend] Stand bis Runde 37: 600 Bilder aufrecht still
  (Gegenlauf Maske 0)`, mit Spur L `still 330 ab F780`. Vorbestehend und mit Spur L kuerzer; Navigator = enemy_ai_common.c /
  nav_zone_common.c, laut VERTRAG §1.4 nicht der Pfad der Spur L. -> Hinweis (eigene Spur), kein Mangel dieser Spur.

## 6. Vertrag (VERTRAG.md) und Tests

* Pfade `git diff e399ee3f --name-only`: `analysis/befunde_runde37/L_*`, `engine/src/enemy_ai_re2_zombie.c` (VERTRAG §1.4
  Ausnahme Spur L), NEU `include/re15_re2zombie_r37.h`, Tests (`tests/unit/test_r37_re2zombie.c`, `tests/unit/probes/
  r37_re2zombie.cmake`, `tests/integration/test_r37_re2zombie.cmake`, angepasst: `test_r36_zombieki_eintritt.c`,
  `probe_r36_blut.c`, `probe_10d0_situp_re2.c`, `test_r34_granaten.cmake`, `test_r35_granate.cmake`). Keine release/,
  platform/android/, shared_assets/PSX/, keine Test-CMakeLists, keine neue .c in engine/src (Android-GLOB unberuehrt).
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine (Engine-Diff schreibt keine Flags, keine Texte).
* Griff-/Biss-Code (Spur J) und `re2z_init` (Spur H) nicht angefasst (Diff-Hunks gelesen: nur DECISION[0]/[12], EXEC[0], EXEC[1]
  und Kommentare in EXEC[2]/EXEC[5]/EXEC[11]-Koepfen).
* Portabilitaet: `test_r37_re2zombie.c` ohne POSIX-Aufrufe; `test_r37_re2zombie.cmake` mit `cmake_minimum_required(VERSION 3.16)`,
  Messkopie `re15_pc_r37l_haken_<id><ext>`, Arbeitsordner `r37_re2zombie_wd` (kein Target-Name).
* Verschobene Riegel mit alt/neu im Dossier (Schritt 5 + 5b + Granaten): vorhanden. Selbst gelesen und bewertet:
  `probe_10d0_situp_re2` (s1 in {1,2}; Mechanismus `80101b20 -> 80101b24` selbst bestaetigt), `test_r34_granaten`/`test_r35_granate`
  (Fenster fuer den Zerreiss-Fall; `80108d08 srav` / `80108d10 beq s0,zero` selbst bestaetigt), `test_r36_zombieki_eintritt`
  (Gegenlauf "vorbestehend" mit Maske 0 — differenziell, s. Abschnitt 5) und `probe_r36_blut` (laeuft jetzt im Strom bis Runde 37,
  Maske 0 — s. Hinweis H2).

### Testlaeufe (Baum r37_re2zombie, Stand ab2bf2b9; keine volle Suite)

1. `ctest -R "r37_re2zombie" -E "^integration_" -j 4` -> `100% tests passed, 0 tests failed out of 9`; verbose gelesen
   (prod1 4/4, prod2 5/5, prod3 5/5, b 3/3, c 6/6, d12 6/6, nahkampf 2/2, raum1130 4/4, raum1180 4/4 — die Pins pruefen
   Grenzwerte (0x1387/0x1388, 0x7CF/0x7D0, 0x9C3/0x9C4, 0x513/0x514, Bild 11), Wurfzahlen und Gegenproben mit Maske 0).
2. `ctest -R "^integration_r37_re2zombie$"` -> Passed 51,50 s (`ROOM1130 Wechsel in den Arme-hoch-Gang 1 (... < 0x7D0: 1;
   Abstaende 1991), Griffe aus dem Arme-hoch-Gang 1`).
3. Unit-Auswahl aller Module/Raeume, die der Diff beruehrt, plus RE2-Strom-Nutzer:
   `-R "re2|zomb|zgirl|granat|lyer|messer|crawl|kriech|gegnerzahl|1140|1130|1180|enemy|biss|griff|grab|bump|kfjump|teardeath|blut|bandlock|rise|r37_|irons|1150|dog|crow|spider|affen|situp|10d0|hund|kraehe|gore|wecker|birkin|elza" -E "^integration_" -j 4`
   -> **`100% tests passed, 0 tests failed out of 231`**.
4. Integration seriell, alle Skripte mit Gegnern/Zombies/Granaten/Irons/Elza:
   `-R "^integration_.*(re2|zomb|zgirl|granat|messer|gegnerzahl|1140|1130|1180|enemy|biss|griff|irons|1150|dog|crow|spider|affen|r37_|kraehe|hund|gore|birkin|r36_schuss|r36_elza|r36_szenenleon|r35_werfer|r35_redhawk)"`
   -> **`100% tests passed, 0 tests failed out of 28`** (2798 s; u.a. integration_r34_granaten 226,9 s, integration_r35_granate
   160,9 s, integration_r35_zgirl/_5090, integration_r36_elza_*, integration_r36_gegnerzahl, integration_r36_szene1150,
   integration_r36_szenenleon, integration_r37_re2zombie 51,6 s, integration_r37_zombieaktiv 25,1 s). Kein Wiederholungslauf noetig.

