# Runde 39 — Spur E "gegner": UNABHAENGIGE ABNAHME 3 (2026-10-18, nach Nachbesserung 3)

Pruefer: Abnahme-Agent (unabhaengig vom Bau-Agenten). Baum `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`, HEAD 0ed55507,
Merge-Basis mit master 22abaed9 (master = 2ce58566; `git diff master --stat` zeigt ausserdem nur master-seitige Doku/
Orchestrator-Dateien und test_r38_itemorte.c (96ab793c, master) als "geloescht" — nicht von dieser Spur).
Gegenstand: die vier Nutzer-Punkte am gebauten Stand + die beiden Maengel der Nachbesserung 3 (Befund E (a)/(b), args_e_nach3.json).

Bericht wird fortlaufend geschrieben (Abschnitte in Messreihenfolge).

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
  "nichts" gilt nur, wenn das Vorbild in der ECHTEN Zelle liegt (`r39_quert` liefert dann 0). Ob der Fall im Spiel auftritt,
  pruefe ich an den Laeufen (Zeilen "Rest-Querung" in gegner_r39.log).

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
| `v5_nachher` / `v6_nachher` (Elza, ROOM1141 ueber 1131 -> Tuer) | §3/§4 | `affen init ... raum=1141 typ=27 hp=360 (... regel 1)` (Raumaufbau + BEFREIUNG); v5: 25 Treffer-Zeilen, FLINCH 8 (alle akku 36), OHNE 17, regelwidrig 0; Flinch-Beginne F3129/3466/3732/3780/4059/4233/4581/4791 = die FLINCH-Zeilen |

- Wortlaut: "doppelt so viel Energie" -> INIT 360 = 2 x 180 (Tabelle @0x8011f514); "erst die flinch Animation ..., wenn sie von
  der Energie her 3x mit der Handfeuerwaffe getroffen wurden" -> Clip 7 nur bei Akku >= 36 = 3 x 12 (Pistole @0x8006ee44, P228
  @0x8006ee48); dazwischen kein Clip 7. Seit Nachtrag 3 fuer jeden 0x27 (11C0 und 1141 gemessen). Vorher-Laeufe (v0.8.26 bzw.
  master-exe: 180 + Clip 7 je Treffer) hat Abnahme 2 gemessen; der Code dieses Punkts ist seit Abnahme 2 unveraendert
  (`git diff 98822214 HEAD` beruehrt Teil (1) nicht), die exe-Laeufe sind bytegleich -> **Punkt 1 erfuellt**.

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

### 5.1b Voll-Ablauf ROOM1141 mit Wache (Versuch 7 v11 selbst nachgefahren)
`v11_nachher`: Aufbau §4, Schuetze `schuss_v11.txt` (`W11` + 80 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`), bytegleich mit dem Skript der
Abnahme 2), Messhaken HP 100, `RE15_EXIT_AT=11500` -> exit 0 (551 s). **state.txt bytegleich mit `E_abn2/n1141_v11`** (14220 Zeilen,
`cmp`); 0 Zeilen `querung`; `aussen.py` F1..F11500: 0 / 0 ausserhalb, 0 in einer Wandzelle; `gor.py`: 60 Treffer-Zeilen (FLINCH 20,
OHNE 40, regelwidrig 0), **Tode G3 F5739, G2 F6576** (Zustand 3 nach dem 31. Treffer); John (Platz 1, Typ 0x49) setzt sich F6697 in
Bewegung ((-7531,-21363) -> ...) und steht ab **F6815 geparkt (-30000,-30000)**. Der durchlaufende Ablauf "Kampf mit 2 x 360 ->
Tod beider Gorillas -> John geht" ist von der Wache unberuehrt.

### 2b. RE-Gate Nachtrag — "RE2: dieselbe Klemme fuer Spieler UND Gegner"
- Byte-Suche nach `jal 0x8003567c` (0x0c00d59f) in info/re2leon: PSX.EXE @0x80026634 und @0x8005d0a0; Overlays EMZ0.BIN /
  EMZ0_d1.BIN / EMOVL10_S0/S1.BIN @0x80100638, EMD0G_MOD0.BIN @0x80100088, EMOVL21_S0.BIN @0x80100250, EMS25.BIN @0x80100178,
  EMS26.BIN @0x801000c0.
- RE2 `dis 0x80026620 8`: Spieler-Wurzel `jal 0x8003bfac` @0x80026620 -> Koerper-Stoss `jal 0x800355c4` @0x80026628 -> Klemme
  `jal 0x8003567c` @0x80026634 (`ori a1,zero,0x8000`). EMZ0.BIN `dis 0x80100628 6`: Stoss `jal 0x80035530` @0x8010062c -> Klemme
  `jal 0x8003567c` @0x80100638 (`addiu a1,zero,1024`). -> Behauptung "Spieler UND Gegner, Stoss vor Klemme" stimmt.

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
