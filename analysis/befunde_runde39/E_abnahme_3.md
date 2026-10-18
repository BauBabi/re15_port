# Runde 39 — Spur E "gegner": UNABHAENGIGE ABNAHME 3 (2026-10-18, nach Nachbesserung 3)

Pruefer: Abnahme-Agent (unabhaengig vom Bau-Agenten). Baum `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`, HEAD 0ed55507,
Merge-Basis mit master 22abaed9 (master = 2ce58566; `git diff master --stat` zeigt ausserdem nur master-seitige Doku/
Orchestrator-Dateien und test_r38_itemorte.c (96ab793c, master) als "geloescht" — nicht von dieser Spur).
Gegenstand: die vier Nutzer-Punkte am gebauten Stand + die beiden Maengel der Nachbesserung 3 (Befund E (a)/(b), args_e_nach3.json).

**Kurzurteil**: Punkte 1-4 **erfuellt** (alle selbst an der gebauten exe gemessen; jeder Lauf mit gleichem Aufbau wie in Abnahme 2
bytegleich, vorher-Laeufe mit v0.8.26 selbst wiederholt); die beiden Maengel der Nachbesserung 3 (Befund E (a) Gorilla durch die
Westwand, Befund E (b) Elza im Wurf durch die Suedwand) **behoben** — gegen eine SELBST gebaute vorher-exe (0a9076e9) bildgleich bis
zum Befund-Bild, dort korrigiert, danach 0 Bilder ausserhalb; Wache greift sonst in keinem Lauf ein (11C0, 3040, 11C1, Kriecher,
1141 ohne Querung bytegleich). Gates Vertrag/Tests/RE-Gate halten (175 Unit + 17 Integration gruen, 12 RE-Stichproben selbst
disassembliert, eigene Mutationsprobe rot ohne Wache). Maengel: keine; Hinweise H1-H7 (§9). **bestanden = true.**

Bericht wurde fortlaufend geschrieben und nach den Messungen nach Themen sortiert.

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 19:59 (juenger als die letzte Quelle gegner_r39.c
  19:39; 0ed55507 aendert nur das Dossier). Die exe traegt die neuen Logzeilen `querung F%u gorilla slot=...` /
  `querung F%u spieler zelle=...` / `stationen F%u (Vorbild) ...` und den Schalter `RE15_R39_GEGNER_LOG` (strings).

## 1. Code-Diff (gelesen) — was Nachbesserung 3 aendert
`git diff 98822214 HEAD -- re15_port` (Abnahme-2-Stand -> HEAD): engine/src/gegner_r39.c (+207: Mess-Schiene "Stationen",
Teil (5) "Querungs-Wache"), include/re15_gegner_r39.h (+8), enemy_ai_common.c (+1 Zeile im 0x27-Zweig nach
`re15_enemy_sca_clamp_band(e, mag_ox, mag_oz, 4u)`), game_step_common.c (+1 include, die wurf-Zeile mit Klammer + 1 Aufruf
`re15_r39_wurf_querung(pl, wurf_alt_x, wurf_alt_z)` nach `re15_player_body_and_walls`), Logschalter `RE15_R39_LOG` ->
`RE15_R39_GEGNER_LOG` (gegner_r39.c + 3 Integrations-cmake), Tests: test_r39_gegner_querung.c (neu), probes/r39_gegner.cmake
(+22), test_r38_griff_alle.c (+4, ein Tabellenfall). Kern `r39_querung`: nach der RE1.5-Klemme nur, wenn die Strecke
Vorbild -> Klemm-Ergebnis das OFFENE Innere einer soliden Rechteckzelle schneidet (Filter wie collision_constrain_impl:
Typ 1, Band == floor>>4, Maske & u0), RE2-Code aus der Vorbild-Lage und Seite nach dem Vorzeichen (Lage - Vorbild); bis zu
4 Durchgaenge; bleibt eine Querung (auch eine Zelle mit Vorbild-Code 0), gilt die Vorbild-Lage. Parameter: Gorilla
r = hit_radius_min, Band = e->floor, Maske 4 (= dieselben Argumente wie `re15_enemy_sca_clamp_band`, enemy_ai_common.c:14142);
Spielerin r = 450, Band = `re15_collision_debug_band()` (= s_coll_band), Maske 1 (= `re15_collision_constrain`,
re15_collision.c:781). Wurf-Pfad = `re15_player_victim_gorilla()` = Greifer 0x27 ODER 0x29 (enemy_ai_common.c:1520) — die
Wache gilt also auch fuer den Griff der kleinen Affen (Stage 3-5).

## 2. RE-Gate (Nachbesserung 3) — Stichproben SELBST disassembliert
Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (info/Re1.5/PSX.EXE, STAGE1.BIN) und `re2_disasm.py`
(info/re2leon/PSX.EXE, md5 09a9b642... = Hauptbaum).

| Behauptung (Code/Dossier) | selbst gelesen | Ergebnis |
|---|---|---|
| RE2 FUN_8004c900: t2 = 2r+w @0x8004c91c-20, t1 = 2r+d @0x8004c928-2c, a0 = Vorbild.x + r - cell.x @0x8004c930-3c, Bit 2 = (a0 >=u t2) @0x8004c940-48, Z Bit 1 @0x8004c94c-64, Randfaelle -1 / t+1 -> Code := 2 bzw. 1 @0x8004c968-a8, Code 0 -> `jal 0x8004c858` @0x8004c9b8 | RE2 `dis 0x8004c900 92`: `lhu a0,144(a3)` (r) / `lhu v1,4(a2)` (w) / `sll v0,a0,1` / `addu t2,v0,v1`; `lh v0,280(a3)` (+0x118) / `lh v1,0(a2)` / `addu` / `subu a0` / `sltu v0,a0,t2` / `xori 1` / `sll t0,v0,1`; Z analog mit `lh v0,282(a3)` (+0x11a); `beq a0,-1` -> `addiu t0,zero,2` (ueberschreibt), `bne a0,t2+1`; `beq a1,-1` / `bne a1,t1+1` -> `addiu t0,zero,1`; `bne t0,zero` sonst `jal 0x8004c858` @0x8004c9b8 | stimmt mit `r39_re2_code` (gegner_r39.c) ueberein, inkl. Ueberschreib-Semantik der Randfaelle |
| RE2 Seite: Mitte cell.x + w/2, t = r + w/2 + 1, Minus wenn x - Vorbild >= 0 (`bltz a1` @0x8004c9f4), `sw v0,132(a3)` @0x8004ca08; Z @0x8004ca18-50 | `lw v0,132(a3)` / `subu a1,v0,v1` (x - Vorbild) / `srl v1,v1,1` / `addiu t2,v0,1` / `bltz a1,0x8004ca04` / `subu v0,v0,t2` @0x8004ca00 bzw. `addu v0,v0,t2` @0x8004ca04 / `sw v0,132(a3)`; Z: `lw v0,140(a3)` ... `sw v0,140(a3)` @0x8004ca50 | stimmt mit `r39_re2_seite` |
| RE2 FUN_8003567c ruft die Klemme `jal 0x8004c1bc` @0x800356d0 und schreibt danach IMMER die Vorbild-Lage +0x118/+0x11a @0x800356ec-f8 | `dis 0x8003567c 40`: `lhu a1,144(s0)` (r) / `jal 0x8004c1bc` @0x800356d0; beide Ueberspring-Zweige (`bne` @0x80035698 / @0x800356a8) landen auf 0x800356ec: `lw v0,132(s0)` / `lw v1,140(s0)` / `sh v0,280(s0)` / `sh v1,282(s0)` @0x800356ec-f8 | stimmt |
| Tabelle @0x800a737c[0] = FUN_8004c900, Auswahl `lhu v0,8(s0)` / `andi 0xf` / `lw v0,29564(at)` @0x8004c418-34, `jalr v0` @0x8004c448 | `dis 0x8004c400 20` + `table 0x800a737c 10`: [0] = 0x8004c900 (ebenso [9]); Zugriff auf Zelle +0/+2/+4/+6 = x/z/w/d (Rechteck) | stimmt |
| RE1.5 Wurf-Platzierung: `sltiu v0,v0,0x25` @0x8011c278 (Clip-Bild des Spielers 0x800acae9), `jal 0x8001ad68` @0x8011c294 mit a0 = Spieler | STAGE1 `dis 0x8011c264 20`: `addiu a0,a0,-13591` (0x800acae9) / `lbu v0,0(a0)` / `sltiu v0,v0,0x25` @0x8011c278 / `beq` @0x8011c27c / `lw a1,0x800acbcc` / `lw a2,0x800acbd0` / `jal 0x8001ad68` @0x8011c294 / Delay `addiu a0,a0,-149` (= 0x800aca54 Spieler) | stimmt |
| Spieler-Klemme @0x80031d70, Radius `lhu a1,6(v0)` @0x80031d6c = DAT_80073e94[+6] = 0x01c2 = 450, Maske `ori a2,zero,0x1` @0x80031d74 | PSX.EXE `dis 0x80031d60 8`: `lw v0,0x800acacc` (Spieler +0x78) / `lhu a1,6(v0)` @0x80031d6c / `jal 0x8003b0a4` @0x80031d70 / `ori a2,zero,0x1` @0x80031d74; `bytes 0x80073e94 12` = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` (+6 = 0x01c2) | stimmt (RE15_R39_SPIELER_R 450, Maske 1) |
| Gorilla-Klemme Maske `ori a2,zero,0x4` @0x80116e68 | STAGE1 `dis 0x80116e60 6`: `lw v0,120(a0)` / `ori a2,zero,0x4` @0x80116e68 / `lhu a1,6(v0)` / `jal 0x8003b0a4` @0x80116e70 | stimmt (RE15_R39_MASKE_GEGNER 4) |
| RE1.5 Koerper-Stoss FUN_8002aec4: Querungs-Zweig nur, wenn die Vorbild-Hoehe +0x42 ausserhalb des Bandes lag; je Achse nur bei Ueberquerung; sonst normaler Stoss | PSX.EXE `dis 0x8002b1d0 110` + `dis 0x8002b384 60`: `lh v0,66(s2)` (+0x42) @0x8002b268, `slt v0,t1,a1` / `beq` @0x8002b2a4-a8 / `bne v0,zero,0x8002b414` @0x8002b2b0 (alte Hoehe im Band -> normaler Stoss); X-Tests `lh a1,64(s2)` (+0x40) gegen `lh v1,52(s3)` @0x8002b2b8-300, Z-Tests `lhu t2,68(s2)` (+0x44) @0x8002b2cc / @0x8002b378-3a8; ohne Ueberquerung `ori t4,zero,0x1` @0x8002b410 -> Stoss mit den normalen Versaetzen t0/a3 (`addu v0,t0,v0` / `sw v0,52(s2)` @0x8002b420-428) | stimmt — der Zweig verhindert einen Durchstoss ohne Ueberquerung NICHT (Begruendung des Bau-Agenten, warum er ihn nicht portiert, haelt) |

- Konstanten in Teil (5): 450 (@0x80031d6c / Bytes oben), Maske 1 (@0x80031d74), Maske 4 (@0x80116e68), RE2-Rechnung
  (+1, w/2, 2r+w, Randfaelle) je mit Adresse im Kommentar; keine neue Zahl ohne Beleg. Die FORM (RE1.5-Klemme bleibt, RE2-
  Regel nur fuer eine durchquerte Zelle, Rest-Querung -> Vorbild) ist als Beta -> Retail + Runde-38-T-Form begruendet
  (Kommentar gegner_r39.c (5), Dossier "Umsetzung (Nachbesserung 3)") — eine Port-Form, kein Byte-Port von RE2 (RE2 rechnet die
  Seitenwahl IMMER, nicht nur bei Querung); das ist im Code-Kommentar so gesagt ("FORM (kleinste belegte)").
- Feinheit (Hinweis, kein Mangel): Fuer eine Zelle mit Vorbild-Code 0 (Vorbild in der aufgeblasenen, aber nicht in der
  echten Zelle) rechnet RE2 die naehere Kante (FUN_8004c858); der Port haelt bei einer Querung in diesem Fall die Vorbild-Lage
  (Rest-Querung-Schleife: "auch Zellen, deren Vorbild-Code 0 ist"). Das Dossier formuliert den Code-0-Fall als "-> nichts";
  "nichts" gilt nur, wenn das Vorbild in der ECHTEN Zelle liegt (`r39_quert` liefert dann 0). An den Laeufen gemessen: in keinem
  eigenen Lauf eine Zeile "Rest-Querung" (v5/v6/v11_nachher, 11C0 Kampf/Szene/Sprung, 11C1, Kriecher: 0), alle Korrekturen
  waren der RE2-Zweig (Code 1/2). -> Hinweis H2.

### 2b. RE-Gate Nachtrag — "RE2: dieselbe Klemme fuer Spieler UND Gegner"
- Byte-Suche nach `jal 0x8003567c` (0x0c00d59f) in info/re2leon: PSX.EXE @0x80026634 und @0x8005d0a0; Overlays EMZ0.BIN /
  EMZ0_d1.BIN / EMOVL10_S0/S1.BIN @0x80100638, EMD0G_MOD0.BIN @0x80100088, EMOVL21_S0.BIN @0x80100250, EMS25.BIN @0x80100178,
  EMS26.BIN @0x801000c0.
- RE2 `dis 0x80026620 8`: Spieler-Wurzel `jal 0x8003bfac` @0x80026620 -> Koerper-Stoss `jal 0x800355c4` @0x80026628 -> Klemme
  `jal 0x8003567c` @0x80026634 (`ori a1,zero,0x8000`). EMZ0.BIN `dis 0x80100628 6`: Stoss `jal 0x80035530` @0x8010062c -> Klemme
  `jal 0x8003567c` @0x80100638 (`addiu a1,zero,1024`). -> Behauptung "Spieler UND Gegner, Stoss vor Klemme" stimmt.
- RE1.5 FUN_8001ad68 (Wurf-Platzierung, `dis 0x8001ad68 50`): Wurzel-Versatz `jal 0x8001ae38` @0x8001ad84, RotMatrixY(+0x6a)
  `lh a0,106(s1)` / `jal 0x800659d0` @0x8001add8-dc, ApplyMatrix `jal 0x80067a28` @0x8001adec, x = (s16)+0xa0 + off.x
  `lh v0,160(s1)` / `addu` / `sw v0,52(s1)` @0x8001adf4-ae04, z = (s16)+0xa2 + off.z `sw v0,60(s1)` @0x8001ae08-18, `jr` ohne jede
  Klemme -> die Platzierung setzt die Spielerin absolut (Anker + gedrehter Versatz); die einzige Klemme danach ist die des
  Spieler-Schwanzes @0x80031d70. Behauptung des Dossiers stimmt.
- RE1.5 Spieler-Schwanz FUN_80031c44: `jalr v0` (Kommando-Handler) @0x80031cb4 -> `jal 0x8002b544` (Koerper-Schub) @0x80031cbc ->
  Klemme `jal 0x8003b0a4` @0x80031d70 (s. Tabelle). Port game_step_common.c: `re15_player_victim_tick` (Platzierung) ->
  `re15_player_body_and_walls` (Schub, Klemme) -> `re15_r39_wurf_querung` — dieselbe Reihenfolge, die Wache danach.

## 3. Befund E (b) — Elza im Gorilla-Wurf durch die Suedwand (Mangel 1 der Nachbesserung 3), SELBST gemessen
Vergleichs-exe "vorher" SELBST gebaut: `git archive 0a9076e9 -- re15_port ':(exclude)re15_port/shared_assets'` nach
`scratchpad/E_abn3/vorher_src`, dort `RE15_TESTS=OFF bash re15_port/tools/local_build.sh configure/build` -> LOCAL-BUILD-OK
(exe 20:16, `strings`: kein `querung F`, Schalter `RE15_R39_GEGNER_LOG` vorhanden). 0a9076e9 = Abnahme-2-Stand 98822214 + nur der
Logschalter-Cherry-pick; `git diff 0a9076e9 HEAD -- re15_port/engine re15_port/include re15_port/platform` = genau die
Nachbesserung 3 (gegner_r39.c, re15_gegner_r39.h, +1 Zeile enemy_ai_common.c, +2 game_step_common.c). "nachher" = Baum-exe
19:59 (Kopie `exe_nachher_abn3.exe`, md5 gleich). Laeufe: `scratchpad/E_abn3/lauf1141.sh` (Messkopie `re15_pc_eabn3_<tag>.exe`
neben der Baum-exe, `env -i`, Aufbau = integration_r39_gegner_1141 / Versuch 6 des Bau-Agenten: RE15_SET_FLAG=3:0x5F,4:0xF0,
4:0xF1,4:0xEC,3:0x78, RE15_GIVE=0x43:1,0x04:99,0x15:60, RE15_EQUIP=4, RE15_DEBUG_JUMP=1131@gp, RE15_PLAYER_POS=-1300,-13900,0,
RE15_PSELECT_AUTO(_SWITCH)=1, RE15_AUDIO_CAP_SYNC, Wegplan 1131 Schluessel/Tuer -> 1141 Phase H -> Gemaelde -> Suedflaeche
(`wegplan_sued.txt`), Schuetze `schuss_sued.txt` ab Spielbild 3300, Messhaken RE15_R39_MESS_SPIELER_HP=100; nachher zusaetzlich
RE15_R39_MESS_STATIONEN=1). Auswerter `aussen.py` (eigene Huelle aus der SCA ROOM1141, `sca_dump.py`: Westwand Zelle 0
x -10600..-8600, Suedwand Zelle 1 z -25600..-23600, Ostwand Zelle 4 x 14700..16700; dazu "in einer soliden Rechteckzelle 0-12").

| Lauf | exe | Ergebnis (state.txt / gegner_r39.log) |
|---|---|---|
| `v6_vorher` (EXIT 3700) | 0a9076e9 (selbst gebaut) | exit 0; F3553 PL(-3087,-23132), F3555 PL(-2715,-23132) (Innenkante -23600+450+18), **F3556 PL(-2144,-26068)** (= Aussenkante -25600-450-18), F3557.. PL(-613,-26068); **Elza 145 Bilder ausserhalb (F3556..F3700)**; Gorilla G3 dabei Sub 15/3 mo=28 (Rear-up-Pin), Elza mo=1 (Wurf-Clip) |
| `v6_nachher` (EXIT 4500) | Baum-exe | exit 0; bis F3555 Bild fuer Bild gleich mit vorher (erstes abweichendes Bild **F3556**); **F3556 PL(-2144,-23149)**, F3557.. PL(-613,-23149); `aussen.py` F1..F4500: **Elza 0 / Gorillas 0 Bilder ausserhalb, 0 Bilder in einer Wandzelle** |

- Schreiber-Kette (Mess-Schiene Stationen, Zeile "stationen F<n+1>" = Bild n): Bild 3555 `opfer=(-2715,-24266)` -> `klemme=
  (-2715,-23132)` (Platzierung vor der Wandmitte -24600 -> Innenkante); **Bild 3556 `opfer=(-2144,-24973)` (373 hinter der
  Wandmitte) -> `schub=(-2144,-24973)` (Schub bewegt nichts) -> `klemme=(-2144,-26068)` (naehere = AUSSENkante) -> `ende=(-2144,
  -23149)`**; Bild 3557 `opfer=(-613,-25309)` -> `klemme=(-613,-26068)` -> `ende=(-613,-23149)`; ab Bild 3558 (Clip-Bild 37 >= 0x25)
  keine Platzierung mehr (`opfer` = Vorlage). Anker (1060,-24101) liegt selbst in der Suedwand-Zelle. = genau die Kette des
  Dossiers (Wurf-Platzierung ad68 fuer Clip-Bild < 0x25 @0x8011c278/@0x8011c294 -> Spieler-Klemme @0x80031d70 -> FUN_8003bca8).
- Wache: `querung F3556 spieler zelle=1 code=1 vorbild=(-2715,-23132) re15=(-2144,-26068) -> (-2144,-23149) (RE2 FUN_8004c900)`,
  `querung F3557 ... re15=(-613,-26068) -> (-613,-23149)`; weitere Wuerfe gegen die Suedwand `F3744/F3745` (-26068 -> -23149) und
  ein GORILLA, den derselbe Mechanismus durch die Suedwand gestellt haette: `querung F4030 gorilla slot=3 zelle=1 code=1
  vorbild=(-1799,-21982) re15=(-1326,-27218) -> (-1326,-21999)` (-27218 = -25600-1600-18; -21999 = -24600+1600+1000+1).
  Nachgerechnet (RE2-Formel, Bild 3556): a1 = -23132 + 450 + 25600 = 2918 >= t1 = 2*450 + 2000 = 2900 -> Code 1; z - Vorbild < 0 ->
  z = (-25600 + 1000) + (450 + 1000 + 1) = -23149. Stimmt mit dem Log.
- Urteil Mangel 1 (Befund E (b)): **behoben** — der Fix erklaert den gemessenen Befund (Platzierung jenseits der Wandmitte +
  naehere-Kante-Klemme), die Korrektur greift genau im Befund-Bild, vorher/nachher bis dahin bildgleich.

## 4. Befund E (a) — Gorilla durch die Westwand (Mangel 2 der Nachbesserung 3), SELBST gemessen
Aufbau wie §3, aber Wegplan `wegplan_1141.txt` (Elza bleibt nach dem Gemaelde stehen), Schuetze `schuss_nachher4.txt` (`W10` +
80 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`)) ab Spielbild 2760, `RE15_EXIT_AT=4800`, Messhaken HP 100 = Versuch 5 des Bau-Agenten.

| Lauf | exe | Ergebnis |
|---|---|---|
| `v5_vorher` | 0a9076e9 (selbst gebaut) | exit 0; F4622 G3 (-7368,-9243) Sub 7/2 (Vergeltungs-Sprung), F4623 G3 (-6982,-9649), G2 (-6632,-9948); **F4624 G3 (-12218,-9837)** (= Westkante -10600-1600-18), danach an der Aussenkante entlang ((-12218,-10051) F4625 ...); **Gorilla 177 Bilder ausserhalb (F4624..F4800)** |
| `v5_nachher` | Baum-exe | exit 0; bis F4623 Bild fuer Bild gleich (erstes abweichendes Bild **F4624**); `querung F4624 gorilla slot=3 zelle=0 code=2 vorbild=(-6982,-9649) re15=(-12218,-9837) -> (-6999,-9837) (RE2 FUN_8004c900)`; state F4624 G3 (-6999,-9837); `aussen.py` F1..F4800: **Elza 0 / Gorillas 0 Bilder ausserhalb, 0 in einer Wandzelle** |

- Nachgerechnet (RE2-Formel, Bild 4624): a0 = -6982 + 1600 + 10600 = 5218 >= t2 = 2*1600 + 2000 = 5200 -> Code 2; x - Vorbild < 0 ->
  x = (-10600 + 1000) + (1600 + 1000 + 1) = -6999. Stimmt mit dem Log.
- Beobachtung (kein Mangel): im Bild nach der Korrektur ueberlappen G3 (-6999) und G2 (-6611) weiter; G2 wird F4625 vom normalen
  RE1.5-Koerper-Stoss auf (-3806,-10818) geschoben (Stoss pen ~2800, Klemme an Zelle 9 z -9200-1600-18). Grosse Stoss-Spruenge
  zwischen den Gorillas gibt es auch im vorher-Lauf (Original-Stoss FUN_8002aec4, §2); beide bleiben im Raum.
- Nebenbefund im `v5_vorher` (Stand OHNE Wache): F4681 wirft G2 (Sub 15/3, mo=28) Elza von (-4146,-9668) nach (-3728,-7332) =
  durch die 1400 dicke Zelle 9 (z -9200..-7800) hindurch in die anliegende Zelle 3 (z -7800..1750; -7332 = Nordkante von Zelle 9
  + 450 + 18): Elza steht danach 120 Bilder IN Zelle 3. Derselbe Klassenfall (Wurf-Platzierung + naehere Kante, hier zwei
  anliegende Zellen nacheinander geklemmt). Im nachher-Lauf kommt die Lage nicht vor (Kampf ab F4624 anders); die Wache deckt
  ihn rechnerisch ab (Strecke (-4146,-9668) -> (-3728,-7332) quert Zelle 9, Vorbild-Code 1 -> z = -8500 - 1151 = -9651).
- Urteil Mangel 2 (Befund E (a)): **behoben** (Korrektur genau im Befund-Bild, vorher/nachher bis dahin bildgleich).

## 5. Nutzer-Punkte an der gebauten exe (Regression nach Nachbesserung 3 + Wortlaut)
Laeufe mit `scratchpad/E_abn3/run.sh` (Messkopie neben der Baum-exe, `env -i`). Wo ein Lauf derselbe Aufbau wie ein Lauf der
Abnahme 2 ist (exe b963e6bb, Ordner `scratchpad/E_abn2/<tag>`), vergleiche ich das Zustands-Log BYTEWEISE (`cmp`) — gleich heisst:
die Wache hat in diesem Lauf nichts veraendert, und die Abnahme-2-Auswertung gilt Bild fuer Bild weiter.

### 5.1 Punkt 1 — Gorillas: doppelte Energie, Flinch erst nach 3 Handfeuerwaffen-Treffern (alle 0x27, Nachtrag 3)
| Lauf | Aufbau | gemessen |
|---|---|---|
| `p1_11c0_kampf` (Leon) | `RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3`, Skript `M1.5` + 14 x `MA0.1,M0.6` ab Spielbild 290, `RE15_EXIT_AT=900`, Software-Renderer | exit 0; state.log **bytegleich** mit `E_abn2/p1_11c0_kampf`; gegner_r39.log: `affen init slot=2/3 raum=11c0 typ=27 hp=360 (tabelle 180 @0x8011f514, regel 1)`, `F335 hp=348 akku=12 -> OHNE ZUCKEN`, `F379 hp=336 akku=24 -> OHNE ZUCKEN`, **`F440 hp=324 akku=36 -> FLINCH`**, `F462 hp=312 akku=12 -> OHNE`; `gor.py`: einziger Flinch-Beginn (Zustand 2 + Clip 7) F440 Platz 2; 0 Zeilen `querung` |
| `p1_11c0_szene` (Leon, Szenen-Weg) | Debug-Sprung 11C0@240, Leon (-22604,14455), Pistole ab Spielbild 1090 (`M0.5` + 18 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`)), `RE15_EXIT_AT=2290` | exit 0; state.log = Anfang von `E_abn2/p1_11c0_szene` (2531 Zeilen bytegleich bis F2290); 12 Treffer-Zeilen, FLINCH 4 (F1174/F1429/F1837/F1894, je akku 36), OHNE 8, regelwidrig 0; Flinch-Beginne im state.log genau F1174 (P2), F1429 (P2), F1837 (P3), F1894 (P2); 0 `querung` |
| `n1141_ohne` (Elza, ROOM1141 ueber 1131 -> Tuer, OHNE Messhaken) | Aufbau §4 ohne RE15_R39_MESS_SPIELER_HP, `RE15_EXIT_AT=4500` | exit 0; state.txt **bytegleich** mit `E_abn2/n1141_ohne` (7220 Zeilen); 19 Treffer-Zeilen, FLINCH 6 (F3129/3466/3732/3780/4059/4233, je akku 36), OHNE 13, regelwidrig 0; Flinch-Beginne im state.txt = genau diese Bilder; Elza (steht, kein Messhaken) stirbt F4439 (hp 4 -> -2); 0 `querung`, 0 Bilder ausserhalb — die Regel haengt nicht am Messhaken |
| `v5_nachher` / `v6_nachher` (Elza, ROOM1141 ueber 1131 -> Tuer) | §3/§4 | `affen init ... raum=1141 typ=27 hp=360 (... regel 1)` (Raumaufbau + BEFREIUNG); v5: 25 Treffer-Zeilen, FLINCH 8 (alle akku 36), OHNE 17, regelwidrig 0; Flinch-Beginne F3129/3466/3732/3780/4059/4233/4581/4791 = die FLINCH-Zeilen |

- Wortlaut: "doppelt so viel Energie" -> INIT 360 = 2 x 180 (Tabelle @0x8011f514); "erst die flinch Animation ..., wenn sie von
  der Energie her 3x mit der Handfeuerwaffe getroffen wurden" -> Clip 7 nur bei Akku >= 36 = 3 x 12 (Pistole @0x8006ee44, P228
  @0x8006ee48); dazwischen kein Clip 7. Seit Nachtrag 3 fuer jeden 0x27 (11C0 und 1141 gemessen). Vorher-Laeufe (v0.8.26 bzw.
  master-exe: 180 + Clip 7 je Treffer) hat Abnahme 2 gemessen; der Code dieses Punkts ist seit Abnahme 2 unveraendert
  (`git diff 98822214 HEAD` beruehrt Teil (1) nicht), die exe-Laeufe sind bytegleich -> **Punkt 1 erfuellt**.

### 5.1b Voll-Ablauf ROOM1141 mit Wache (Versuch 7 v11 selbst nachgefahren)
`v11_nachher`: Aufbau §4, Schuetze `schuss_v11.txt` (`W11` + 80 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`), bytegleich mit dem Skript der
Abnahme 2), Messhaken HP 100, `RE15_EXIT_AT=11500` -> exit 0 (551 s). **state.txt bytegleich mit `E_abn2/n1141_v11`** (14220 Zeilen,
`cmp`); 0 Zeilen `querung`; `aussen.py` F1..F11500: 0 / 0 ausserhalb, 0 in einer Wandzelle; `gor.py`: 60 Treffer-Zeilen (FLINCH 20,
OHNE 40, regelwidrig 0), **Tode G3 F5739, G2 F6576** (Zustand 3 nach dem 31. Treffer); John (Platz 1, Typ 0x49) setzt sich F6697 in
Bewegung ((-7531,-21363) -> ...) und steht ab **F6815 geparkt (-30000,-30000)**. Der durchlaufende Ablauf "Kampf mit 2 x 360 ->
Tod beider Gorillas -> John geht" ist von der Wache unberuehrt.

### 5.2 Punkt 2 — kein paralleler Hundebiss (RE2-Hundemodul)
Code seit Abnahme 2 unveraendert (`git diff 98822214 HEAD` beruehrt enemy_ai_re2_dog.c und Teil (2) nicht; der Kopf von
`re15_r39_hund_riegel_tick` ruft zusaetzlich nur die env-gegatete Mess-Schiene `r39_mess_stationen`, ohne Env sofort return).
Spielszene ROOM11C1 (Elza, fuenf Hunde 0x20; Karte SELBST erzeugt: `probe_r36_elza_karte re15_card.mcr 11C1 -22604 14455 0` ->
"Raum 0x11C1, Figur 4 (Elza=4)", bytegleich mit der Karte der Abnahme 2), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
RE15_EXIT_AT=1150`, debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`, Auswerter `hunde.py` (HP-Abzuege + Hunde in Sub 3/3):

| Lauf | Bisse (Bild, HP, Abstand) |
|---|---|
| `p2_11c1_v0826` (VORHER, v0.8.26-exe) | F779 100->80, F862 (83), F906 (44), F947 (41), F1008 20->0 (61), **F1019 0->-20 nach 11 Bildern, Hunde 4 UND 5 in Sub 3/3** = paralleler Biss in der Treffer-Reaktion (Nutzer-Befund als Spielszene) |
| `p2_11c1_nachher` (Baum-exe) | F779, F862 (83), F906 (44), F947 (41), F1008 (61), **F1040 (32)** — kein Biss innerhalb der Reaktion; gegner_r39.log `hund riegel GESETZT F779 / FREI F802`, `F862/F885`, `F906/F929`, `F947/F970`, `F1008/F1031` (je 23 Bilder = 22 Bilder Flinch + Exit-Bild, Freigabe wie @0x8003f8b4); state.log **bytegleich** mit `E_abn2/p2_11c1_nachher` |

- Echte exe ROOM1190 (Hunde-Luke, die Zuordnung "Room 1090" -> 1190 hat Abnahme 2 per Zensus belegt): `integration_r39_gegner_hunde1190`
  selbst gefahren -> Passed 43,1 s; Laufordner `build/tests/integration/r39_gegner_hunde1190_wd/lauf_6de7681d/gegner_r39.log`:
  `hund riegel GESETZT F685 / FREI F708`, `F720 / F743`, `F753 / F776`, `F833 / F856` (4 Bisse, je 23 Bilder Riegel).
- Wortlaut "nicht parallel noch von anderen Hunden angegriffen/gebissen ..., wenn ich mich nicht wehren kann": waehrend der eigenen
  Treffer-Reaktion beisst/greift kein zweiter Hund an (Kontakt-Tor @0x80104e5c-68 + Flug-Tor @0x80104d84-94 am Byte PL+0x1D3, Setzer
  @0x801051f8-0x80105200, Freigabe wie @0x8003f8b4 — in Abnahme 1/2 selbst disassembliert); vorher (v0.8.26) als Spielszene
  reproduziert (F1019), nachher weg. **Punkt 2 erfuellt.**

### 5.3 Punkt 3 — liegende Zombies kriechen (ROOM1010/1011, ROOM1220/1221)
Code seit Abnahme 2 unveraendert (Teil (3) und enemy_ai_re2_zombie.c nicht im Diff 98822214..HEAD). Alle Laeufe ueber den ECHTEN
Tuer-Weg: Tuer-AOT des Nachbarraums (`RE15_FIRE_AOT`, debug.log `[fire-aot] slot=0 at F60 (Raum 1020)` bzw. `(Raum 1021)`), danach
Wegplan `RAUM <r> / WARTE 60 / GEHE 2500 -2700 150` (1010/1011) bzw. `GEHE -23500 -7700 150` (1220/1221) `/ WARTE 400`,
`RE15_EXIT_AT=600#<r>`; Leon mit Titel-Autostart (`RE15_TITLE_SHOT`, `RE15_DEBUG_JUMP=<Nachbar>@240`), Elza mit
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO(_SWITCH)=1 RE15_DEBUG_JUMP=<Nachbar>@gp` (debug.log `Spieler-Familie PL04`,
`[elzaems] Typ 0x10 aus EMD/CDEMD1.EMS`). Auswerter `kriech.py` (je g=81-Zombie: Wecken aus `ss1=2 mo=23`, Weg bis zum Griff `ss1=1`).

| Lauf | gemessen |
|---|---|
| `p3_1010_leon` | state.log **bytegleich** mit `E_abn2/p3_1010_leon`; `kriecher weckt slot=3 typ=10 dist=2985` / `slot=4 dist=2996`; Platz 3 wach F104 (mo=5), kriecht 745 Einheiten bis zum Griff F126; Platz 4 wach F111, 3048 Einheiten bis zum Griff F270; Leon HP 100 -> 95 F137 -> 90 -> 85 ... |
| `p3_1220_leon` | state.log **bytegleich** mit `E_abn2/p3_1220_leon`; `slot=1 typ=16 dist=2941` / `slot=2 dist=2996`; Platz 2 wach F103 -> Griff F133 (830), Platz 1 wach F98 -> Griff F276 (2233); Leon HP 95 F144 ... |
| `p3_1011_elza` | Raum-1011-Segment (600 Zeilen) **gleich** dem der Abnahme 2; `slot=4 typ=10 dist=2985` / `slot=5 dist=2996`; wach F104/F111, Griff F126/F270; Elza HP 95 F137 |
| `p3_1221_elza` | Raum-1221-Segment (600 Zeilen) **gleich** dem der Abnahme 2; `[fire-aot] slot=1 at F60 (Raum 1211)`, `[elzaems] Typ 0x16 aus EMD/CDEMD1.EMS`; `slot=1 typ=16 dist=2941` / `slot=2 dist=2996`; wach F98/F103, Griff F276/F133; Elza HP 95 F144 |
| `p3_1010_v0826` (VORHER, v0.8.26) | beide 0x10 g=81 liegen 600 Bilder (`ss1=2 mo=23`, Weg 0), Leon HP 100 — der Nutzer-Befund; bytegleich mit `E_abn2/p3_1010_v0826` |

- Wortlaut "die am Boden liegen kriechen im Original. Das sollen sie bei uns auch": in allen vier Raeumen, beide Figuren, kriechen
  die liegenden 0x81-Zombies los (RE1.5 Block B: dist < 0xbb8 @0x80104718, arc 0x200 @0x8010472c-34, Wort 1 @0x80104738/48 — in
  Abnahme 2 selbst disassembliert) und greifen; vorher (v0.8.26) lagen sie 600 Bilder. **Punkt 3 erfuellt.**

### 5.4 Punkt 4 — Gorilla-Schatten beim Sprung am Boden
- `p4_sprung` (nachher) und `p4_sprung_v0826` (vorher, v0.8.26-exe aus `E_abn2/exe`, selbst gestartet), beide BESCHLEUNIGTER
  Renderer (debug.log `SDL-Renderer direct3d`), `RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-22604,14455,0
  RE15_FRAMEDUMP=1350-1395/5:fd_ RE15_EXIT_AT=1500#11C0`.
- gegner_r39.log nachher: 428 Schatten-Zeilen, **quad_y != boden_y: 0**; 24 Sprung-Zeilen F1362-F1385 Platz 3 `st=1/7`,
  koerper_y -720 .. -4680 .. -720, `boden_y=0 quad_y=0`.
- Framedumps (md5): F1350/1355/1360/1390/1395 vorher == nachher; **F1365-F1385 verschieden** (nur die Sprungbilder); meine
  v0.8.26-Bilder == die der Abnahme 2, meine nachher-Bilder == die der Abnahme 2 (b963e6bb). Bild
  `scratchpad/E_abn3/p4_sprung_vergleich_abn3.png` (Ausschnitt x40-280/y0-200, 3x): **vorher F1370/F1375 haengt eine dunkle
  Schatten-Ellipse in der Luft unter dem springenden Gorilla; nachher liegt der Blob auf dem Asphalt, in der Luft keiner.**
- **Punkt 4 erfuellt.**

### 5.5 Nebenwirkungen der Wache (vorher = 0a9076e9 selbst gebaut, nachher = Baum-exe, gleicher Aufbau)
| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `k11c0_lang_vorher` / `_nachher` (Leon, 11C0 Kampf-Layout, Gorillas greifen 3000 Bilder an) | `RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486`, keine Eingabe, Messhaken HP 100, `RE15_EXIT_AT=3000` | state.log vorher/nachher **bytegleich** (F1..F3000); Gorilla-Unterzustaende im Log: Sub 0 252, Sub 3 3086, Sub 4 65, Sub 5 2558, Sub 7 (Sprung) 39 Bild-Eintraege; 55 Angriffe (Messhaken-Ausgleiche); 0 `querung` — die Wache greift in 11C0 (Autos, Saeulen) nicht ein |
| `affen3_3040_vorher` / `_nachher` (Leon, ROOM3040, kleine Affen 0x29) | Aufbau integration_r38_affen3 (`RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-8800,-2400,1024,1 RE15_INPUT_SCRIPT=W5,U25`, `RE15_EXIT_AT=1000#3040`) | state.log vorher/nachher **bytegleich**; Leon HP 100 -> 55 (Bisse), kein Griff (`gr=1` 0 Bilder) — der 0x29-Griff-Pfad der Wache ist hier nicht ausgeloest; er ist ueber unit_r38_griff_affe3_hinten (T-Sonde, gruen) und die Integration r38_affen3* (§7) abgedeckt |
| 11C0 Kampf/Szene/Sprung, 11C1, Kriecher (§5.1-5.4) | — | alle bytegleich mit den Laeufen der Abnahme 2 (b963e6bb, ohne Wache), 0 `querung` |
| `n1141_ohne`, `v11_nachher` (1141) | §5.1 / §5.1b | bytegleich mit Abnahme 2, 0 `querung` |

Die Wache aendert also nur die Bilder, in denen eine Querung vorliegt (v5: F4624; v6: F3556/3557/3744/3745/4030/4228/4229);
in allen anderen gemessenen Laeufen ist das Spiel Byte fuer Byte das der Abnahme 2.

## 6. Riegel der Nachbesserung 3 — gelesen, gefahren, EIGENE Mutationsprobe
- `ctest -V -R "^unit_r39_gegner_(querung_.*|t_gorilla1141)$"` (Ausgabe gelesen): wand1141 vorher (Haken aus) `(-12218,-10292)`,
  90/90 Bilder hinter der Westwand; nachher `(-6999,-10292)`, Kontakt +0x90 0x88, 90/90 im Raum; Sonde Versuch 5 (-12218,-9848) ->
  (-6999,-9848). frei1141: Stoss findet statt (1129 -> -1675), 60 Bilder Haken aus/an gleich. wurf1141: vorher Bild 24/25 Elza
  (-2144,-26068)/(-613,-26068), 50 Bilder ausserhalb; nachher 0 ausserhalb, kleinstes z -23150. t_gorilla1141 (T-Sonde
  test_r38_griff_alle, Fall gorilla1141_sued): Latch Bild 15, Anker (1058,-24107), 74 Griffbilder, 0 ausserhalb, 0 hinter einer Wand.
  -> `100% tests passed, 0 tests failed out of 4`.
- **Eigene Mutationsprobe** (nichts im Baum geaendert): `scratchpad/E_abn3/mutation/wache_aus.c` definiert
  `re15_r39_wurf_querung` / `re15_r39_affen_querung` als No-op und wird VOR `libre15_engine.a` mit `-Wl,--allow-multiple-definition`
  gegen die unveraenderten Objekt-Dateien des Baum-Builds gelinkt (msys64-gcc):
  - `t_griff_mut.exe gorilla1141_sued` -> `FAIL: Spieler in jedem Griffbild in der Raumhuelle: 50 ausserhalb`, `FAIL: kein
    Griffbild hinter einer Wand: 50` (Bild 39 Spieler (-2143,-26068)) — die T-Sonde wird ohne Wache rot (Angabe des Bau-Agenten
    "50 ausserhalb" bestaetigt).
  - `t_quer_mut.exe`: wand1141 `FEHLER: [nachher] ... gemessen 0 drinnen, x -12218..-12218` (1 Fehler), wurf1141 3 Fehler (50
    ausserhalb, 50 hinter einer Wand, kleinstes z -26068), frei1141 0 Fehler (die Gegenprobe haengt nicht an der Wache).
  -> Die Riegel messen die Wache und nichts anderes.

## 7. Tests (Spurbaum, gezielt — keine volle Suite)
- Unit: `ctest --test-dir re15_port/build -j3 -R "<Ausdruck>" --timeout 600 --output-on-failure`, Ausdruck =
  `scratchpad/E_abn3/regex_unit.txt` (die Liste des Bau-Agenten + `sca_|wall_clamp|quer`, `r38_spinnen`, `1090_push_pin`,
  `1090_room_pin`):
  `^(unit_.*(maggot|affen|gorilla|1141|11c0|r39_gegner|dining|elzaems|szene11c0|griff|hund|dog|kriech|crawl|schatten|body_push|victim|bissarm|wurf|wand|spiderman|sca_|wall_clamp|quer).*|unit_(r37_beretta|r38_szenenschutz_alle|r34_reaktion|r36_tueren|r37_fahrstuhl|1090_fire_pin|adult_spider_ai|ai_flavor_mixed|cockroach_ai|collision_layers|enemy_attack_reach|irons_neck_1150|item_use|member|r35_werfer|r37_sperre1050|re15_re2z_gore_se|re2_hybrid_rig|re2_spider_ai|re2_zombie_ai|writher_kill_flag|r35_granate|slope_collision|r38_spinnen|1090_push_pin|1090_room_pin).*|prop_sca_collision|r31_trefferboxen|r30_hund_hitbox_stauchung)$`
  -> **`100% tests passed, 0 tests failed out of 175`** (13,0 s). Darin u. a. unit_r39_gegner_* (18), unit_r35_affen_* (Original-
  Riegel, Haken aus), unit_r38_affen3_*, unit_r38_griff_* (T-Invariante, 11 Faelle + gorilla1141), unit_maggot_ai*, unit_enemy_body_push,
  unit_sca_contact90, unit_sca_wall_hit, unit_npc_wall_clamp, unit_collision_layers, unit_slope_collision, prop_sca_collision,
  unit_r37_bissarm_*, unit_r34_wurf, alle *dog*/*hund*/*kriech*/*crawl*.
- Integration: `ctest --test-dir re15_port/build -j2 -R "<Ausdruck>" --timeout 3600 --output-on-failure`, Ausdruck
  `^integration_(r39_gegner_1141|r39_gegner_schatten|r39_gegner_hunde1190|r37_elzaems_elza1141|r37_elzaems_elza1181griff|r36_kleidung_hundeweg|r37_szene11c0|r38_affen3|r38_affen3_decke|r38_affen3_tor|r38_dining1141_.*)$`
  -> **`100% tests passed, 0 tests failed out of 17`** (1389,7 s; parallel lief ein fremder Agent mit eigenen exe-Kopien —
  kein Fenster-Haken musste nachgefahren werden): dining1141_a 311,2 s, _s 580,3, _d 297,5, _r 231,1, _w 225,0, _z 129,0, _l 59,6;
  r37_szene11c0 130,2; r39_gegner_1141 293,5 (Laufordner lauf_bcf8210e: 4 x `affen init ... hp=360`, 20 Treffer-Zeilen, 0 `querung`);
  r36_kleidung_hundeweg 61,1; r38_affen3_decke 102,1; r38_affen3_tor 106,5; r38_affen3 86,2; r37_elzaems_elza1181griff 16,5;
  r39_gegner_schatten 60,0 (lauf_47844444: 428 Schatten-Zeilen, 0 `querung`); r37_elzaems_elza1141 14,6; r39_gegner_hunde1190 43,1.
- Zusammen 175 Unit + 17 Integration gezielt gruen + 4 Riegel-Laeufe mit -V gelesen + eigene Mutationsprobe (§6). Die volle Suite
  faehrt der Orchestrator im Integrationsbaum.

## 8. Vertrag, Pfade
- `git diff master...HEAD --name-only` (eigene Aenderungen seit der Merge-Basis 22abaed9): analysis/befunde_runde39/E_abnahme_2.md,
  E_gegner.md; re15_port/engine/src/{enemy_ai_common.c, game_step_common.c, gegner_r39.c}; include/re15_gegner_r39.h;
  tests/integration/test_r39_gegner_{1141,hunde1190,schatten}.cmake; tests/unit/probes/r39_gegner.cmake; tests/unit/
  {test_maggot_ai.c, test_r35_affen.c, test_r38_griff_alle.c, test_r39_gegner.c, test_r39_gegner_1141.c,
  test_r39_gegner_querung.c}. Keine release/, platform/android/, shared_assets/PSX/, keine CMakeLists.txt. (`git diff master`
  zeigt zusaetzlich test_r38_itemorte.c — das ist der spaetere master-Fix 96ab793c, den der Zweig nicht beruehrt.)
- Gemeinsame Dateien: enemy_ai_common.c (VERTRAG §1.3 fuer D, E) — include @46 + Haken @8893 (INIT), @9428 (HURT), @9596 (Tick-Ende),
  @14483 (Hunde-Riegel-Tick), @14746 (Querungs-Wache, neu) + Blut-Zwilling `re15_r39_affen_blut` @8742 (1 Zeile + Kommentar), je "Runde 39 Spur E";
  game_step_common.c (NICHT in §1.3) — +1 include, +1 Aufruf in der vorhandenen wurf-Zeile (Klammer), Kommentar "Runde 39
  Spur E (Nachbesserung 3)". Selbst geprueft: in Runde 39 haengen auch J (r39/dining1141, +1 Zeile @2493) und K
  (r39/heliport1171, include @38 + Hunks @387/@449) Haken in diese Datei; keiner ueberlappt die E-Hunks (@46-49, @2509-2510),
  r39/integration und master haben die Datei seit 22abaed9 nicht geaendert -> kleiner Haken, konfliktfrei; Hinweis H1.
- test_r38_griff_alle.c (Runde-38-T-Datei) +4 Zeilen = ein Tabellenfall, vom Orchestrator verlangt ("die vorhandene
  T-Invarianten-Sonde um den Gorilla erweitern", args_e_nach3.json); registriert aus probes/r39_gegner.cmake.
- Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine belegt (grep im Diff). Neue Env-Schalter: nur Mess-Schienen
  `RE15_R39_GEGNER_LOG` (Log), `RE15_R39_MESS_STATIONEN` (Log), `RE15_R39_MESS_SPIELER_HP` (Messhaken, Nachbesserung 2) — ohne
  Env wirkungslos; keiner ist Abschluss eines Punkts (die Regeln sind ohne sie gemessen, §5).
- Tabu-Woerter im Code-Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/hack/approx/vermutlich): keine.
- Neue Tests parallel-/Linux-fest: test_r39_gegner_querung.c laeuft als reiner Unit-Test (keine exe, kein Laufordner),
  `m` fuer Nicht-WIN32 in probes/r39_gegner.cmake, Asset-Pfad ueber RE15_ASSET_PSX_DIR (kein Backslash/.exe).

## 9. Hinweise (keine Maengel gegen den Wortlaut bzw. die Maengel der Nachbesserung 3)
- **H1 (Integration)**: game_step_common.c steht nicht in VERTRAG §1.3; der E-Haken ist klein (+1 include, +1 Aufruf), konfliktfrei
  gegen J/K (eigene Hunks an anderen Stellen, §8). Bei der Integration in die Liste aufnehmen oder bewusst so lassen.
- **H2 (Doku-Genauigkeit)**: Dossier "Umsetzung (Nachbesserung 3)" nennt fuer Vorbild-Code 0 "-> nichts"; der Code haelt bei
  verbleibender Querung auch dann die Vorbild-Lage (Rest-Querung-Schleife), RE2 wuerde dort die naehere Kante rechnen
  (FUN_8004c858 @0x8004c9b8). In keinem Lauf aufgetreten (0 Zeilen "Rest-Querung"), Port-Form so im Code-Kommentar benannt.
- **H3 (Geltungsbereich, Entscheidung des Orchestrators)**: die Wache sitzt genau auf den beiden Befund-E-Pfaden (Klemme des
  Gorillas 0x27; Klemme der Spielerin im Opfer-Haken der Greifer 0x27/0x29). Die Spielerin im Normal-/Treffer-Zweig (Koerper-Stoss
  eines Gegners, Gorilla gegen Spielerin R = 1600 + 450 = 2050) und alle anderen Gegner-Klemmen bleiben RE1.5 — dieselbe
  Mechanik-Klasse ist dort rechnerisch moeglich, in keinem meiner Laeufe beobachtet (alle Ausserhalb-/In-Wand-Faelle der
  vorher-Laeufe lagen im Wurf-Pfad oder in der Gorilla-Klemme). Der Bau-Agent meldet dasselbe als OFFEN. Code-Lesung dazu: auch
  der Niederschlag-Rutschweg der Spielerin (game_step_common.c Knockdown-Zweig, `re15_player_body_and_walls(c, pl, kx, kz)`,
  Schritte laut Kommentar bis 1000 je Bild) laeuft ueber die RE1.5-Klemme ohne Wache — bei duennen Zellen (z. B. ROOM1141 Zelle 10,
  800 dick) ist ein Schritt ueber die Zellmitte rechnerisch moeglich; nicht gemessen, nicht beobachtet.
- **H4 (Nebenbefund im vorher-Stand, mit der Wache abgedeckt)**: `v5_vorher` F4681: Wurf durch die 1400 dicke Zelle 9 in die
  anliegende Zelle 3 (zwei Zellen nacheinander je zur naeheren Kante geklemmt), Elza 120 Bilder IN Zelle 3. Liegt im Wurf-Pfad;
  die Wache rechnet dafuer die Herkunftsseite (z -9651). Im nachher-Lauf nicht aufgetreten (Kampfverlauf ab F4624 anders).
- **H5 (Beobachtung)**: nach der Gorilla-Korrektur ueberlappen die beiden Gorillas weiter; der Original-Koerper-Stoss
  (FUN_8002aec4, pen bis 3200) versetzt dann den anderen Gorilla in einem Bild um ~2800 (v5_nachher F4624 -> F4625 G2 (-6611,-9865)
  -> (-3806,-10818)). Solche Stoss-Spruenge gibt es auch ohne Wache (vorher-Laeufe, Abnahme 2 §6); beide bleiben im Raum.
- **H6 (aus Abnahme 2, unveraendert)**: Groesse des Gorilla-Schatten-Quads (Original +0xbc/+0xbe = 1700/1800, Port 500/600) — nicht
  Teil des Nutzer-Wortlauts (Lage am Boden ist erfuellt).
- **H7 (Messaufbau)**: Leons Kriecher-Laeufe brauchen den Titel-Autostart (`RE15_TITLE_SHOT`), Elzas den Titel-Bestaetiger
  (`RE15_TITLE_CONFIRM_MS`) + Spielerwahl; ohne beides bleibt die exe im Titel (eigene Fehlstarts, ueber die eigene PID beendet,
  kein `taskkill /IM`).

## 10. Maengel
Keine.

## 11. Ergebnis

| Punkt | Urteil | Kern des Belegs (selbst gemessen) |
|---|---|---|
| 1 Gorillas (alle 0x27, Nachtrag 3): doppelte Energie + Flinch erst nach 3 Handfeuerwaffen-Treffern | **erfuellt** | INIT 360 = 2 x 180 (@0x8011f514) in 11C0 und 1141; Clip 7 nur bei Akku >= 36 (= 3 x 12, @0x8006ee44/@0x8006ee48): 11C0 Kampf F440 (einziger Flinch nach 3 Treffern), 11C0 Szene 12 Treffer / 4 Flinches, 1141 ohne Messhaken 19 / 6, v11 60 / 20, regelwidrig 0; alle 11C0-/1141-Laeufe bytegleich mit Abnahme 2 bzw. Wache greift nur in den Befund-E-Bildern |
| 2 kein paralleler Hundebiss | **erfuellt** | 11C1 Elza (5 Hunde): v0.8.26 Biss F1008 + F1019 (11 Bilder, Hunde 4 und 5 in Sub 3/3), diese exe F1008 -> Riegel FREI F1031 -> naechster Biss F1040; 1190-Riegel (Integration) gruen |
| 3 liegende Zombies kriechen (1010/1011, 1220/1221) | **erfuellt** | alle vier Raeume ueber die Tuer, Leon und Elza: Wecker dist 2985/2996 bzw. 2941/2996, Kriechen bis zum Griff, HP-Abzug; v0.8.26 liegen sie 600 Bilder |
| 4 Gorilla-Schatten beim Sprung am Boden | **erfuellt** | beschleunigter Renderer, F1365-F1385: v0.8.26 Schatten-Ellipse in der Luft, diese exe am Boden (Bild p4_sprung_vergleich_abn3.png); quad_y == boden_y in 428/428 Zeilen |
| Nachbesserung 3, Mangel 1 (Befund E (b), Elza durch die Suedwand) | **behoben** | vorher (0a9076e9 selbst gebaut) F3556 (-2144,-26068), 145 Bilder ausserhalb; nachher F3556 (-2144,-23149), 0 ausserhalb; Schreiber-Kette OPFER -24973 -> KLEMME -26068 -> Wache -23149 |
| Nachbesserung 3, Mangel 2 (Befund E (a), Gorilla durch die Westwand) | **behoben** | vorher F4624 G3 (-12218,-9837), 177 Bilder ausserhalb; nachher `querung F4624 ... -> (-6999,-9837)`, 0 ausserhalb |

Gates: Vertrag/Pfade OK (§8; game_step_common.c als Hinweis H1), Tests OK (175 Unit + 17 Integration selbst gefahren, §7; Riegel
der Nachbesserung 3 messend, eigene Mutationsprobe §6), RE-Gate OK (12 Stichproben selbst disassembliert, §2/§2b — alle stimmen;
jede Konstante der Wache mit @0x...; Form als Beta -> Retail + Runde-38-T-Form benannt), Fix erklaert den Befund (§3/§4: Schreiber-
Kette gemessen, Korrektur genau im Befund-Bild). **bestanden = true.**

## 12. Pruefprotokoll (Werkzeuge, Ablage)
- Alles unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/E_abn3/`:
  Laeufer `run.sh` (Messkopie `re15_pc_eabn3_<tag>.exe` neben der Baum-exe, `env -i`, Kopie + cap.raw danach geloescht),
  `lauf1141.sh`, `lauf_kriech.sh` (Leon), `lauf_kriech_elza.sh` (Elza); Auswerter `aussen.py`, `gor.py`, `hunde.py`, `kriech.py`,
  `sca_dump.py`; exe-Kopien `exe_nachher_abn3.exe` (Baum 19:59), `exe_vorher_abn3_0a9076e9.exe` (selbst gebaut aus `vorher_src/`,
  Log `vorher_build.log`), `exe_v0826.exe` (Kopie aus `E_abn2/exe`); Mutationsprobe `mutation/` (wache_aus.c, t_griff_mut.exe,
  t_quer_mut.exe); Bild `p4_sprung_vergleich_abn3.png`; ctest-Logs `ctest_unit.log`, `ctest_querung_V.log`, `ctest_int.log`.
- Laufordner: v6_vorher, v6_nachher, v5_vorher, v5_nachher, v11_nachher, n1141_ohne, p1_11c0_kampf, p1_11c0_szene, k11c0_lang_vorher,
  k11c0_lang_nachher, p2_11c1_nachher, p2_11c1_v0826, p3_1010_leon, p3_1220_leon, p3_1011_elza, p3_1221_elza, p3_1010_v0826,
  p4_sprung, p4_sprung_v0826, affen3_3040_vorher, affen3_3040_nachher; Karte `karten/11c1/re15_card.mcr` (selbst erzeugt).
- Fehlstarts (eigene Messaufbau-Fehler, verworfen): die ersten Kriecher-Starts ohne Titel-Autostart bzw. mit Titel-Autostart fuer
  Elza (Leon statt Elza geladen) — ueber die eigene PID beendet (`Stop-Process -Id`), danach mit korrektem Aufbau wiederholt.
- Nichts im Baum geaendert ausser diesem Bericht (`git status`: nur analysis/befunde_runde39/E_abnahme_3.md).
