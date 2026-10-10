# Runde 36 — Spur B "kleidung": UNABHAENGIGE ABNAHME 0 (2026-10-10)

Baum `.claude/worktrees/r36_kleidung`, Zweig `r36/kleidung`, HEAD 866af8e3 (Basis master 78a4a748).
Pruefer: Abnahme 0 (glaubt nichts, was nicht selbst am gebauten Stand gemessen wurde).
Bericht wird fortlaufend geschrieben (Sitzungslimit).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Z.32)

> Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.

## 1. Diff und Bau

* `git log master..HEAD`: 12 Spur-Commits (d0b7d321 .. 866af8e3) + 2 Merges von master. Diff 18 Dateien
  (+1393/-26): neu `engine/src/kleidung_r36.c`, `include/re15_kleidung_r36.h`, Tests
  `tests/unit/test_r36_kleidung.c` + `_teile.inc`, `tests/unit/probes/r36_kleidung.cmake`,
  `tests/integration/test_r36_kleidung_{bild,spielweg}.cmake`; Haken in enemy_ai_common.c (2 Ersetzungen
  + 1 neue Zeile + Accessor), enemy_ai_re2_{zombie,dog,crow}.c (je Include + 1 Zeile), re15_damage.c
  (Byte-Vergleich + Accessor), re15_damage.h / re15_enemy_ai.h (je 1 Deklaration), main.c (LUT-Tabelle
  ersetzt durch `re15_kleidung_panel_ziel`). Code-Diff vollstaendig gelesen.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`;
  `local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)`. exe
  `re15_port/build/platform/pc/re15_pc.exe` 2026-10-10 16:13:32 (neuer als jede Quelle; Baum sauber
  ausser diesem Bericht) -> der gebaute Stand ist HEAD 866af8e3.

## 2. Spur-Tests (selbst gefahren)

`ctest --test-dir re15_port/build -R r36_kleidung --timeout 900 --output-on-failure -V` -> 7/7 gruen (100 s):

| Test | Ergebnis | Kernzeilen |
|---|---|---|
| unit_r36_kleidung_griff_re2 | Passed | Abwurf 1/2 alle L0, Abwurf 3 (F492) p4=L1 p7=L1 gen 3 |
| unit_r36_kleidung_griff_re15 | Passed | Abwurf 3 (F416) p4=L1 p7=L1 (unveraendert ggue. Dossier) |
| unit_r36_kleidung_hund | Passed | 4 Bisse F30/F78/F111/F116, hp 100->80->60->40->20, Panels **4,5,6,7 je +50 — kein Stempel** |
| unit_r36_kleidung_kraehe | Passed | Griffe F348/F1278 je +130 Wundpunkte |
| unit_r36_kleidung_lut | Passed | — |
| integration_r36_kleidung_bild | Passed (67,5 s) | LA/LA2 0 px; LA/LB 737; LA/LC 263; EA/EB 462, alles im Kasten |
| integration_r36_kleidung_spielweg | Passed (32,5 s) | 3 Abwuerfe, Spielstand 0/30 0/0 0/0 0/0 0/50 0/100 0/0 1/0 hp 20, 131 px in (176,103)-(190,125) |

## 3. RE-Gate (selbst disassembliert, `re15_disasm.py` / `re2_disasm.py` im Baum)

| # | Zitat im Diff | Binaerdatei | Gelesen | Urteil |
|---|---|---|---|---|
| 1 | Phase 4 @0x8010a4e8, aca5a:=5, Blut 0x1500 Part 8/3 @0x8010a518-574, Dispatch @0x8010a580-5b0 | STAGE1.BIN | `ori v0,zero,0x5`/`sb v0,-13734(at)` (aca5a) @0x8010a4f0/f8; `sltiu v0,v0,0x2` @0x8010a518; `addiu a2,a2,580` @0x8010a54c / `addiu a2,a2,1440` @0x8010a570; `jal 0x80019700` @0x8010a574; `lbu aca59` @0x8010a580, `addiu at,at,440` (0x801201b8) @0x8010a5a0, `jalr v0` @0x8010a5b0 | bestaetigt |
| 2 | Tabelle 0x801201b8 + Helper | STAGE1.BIN | [0]0x8010a1cc (0,0xa)(5,0x32)(7,0x32); [1]0x8010a208 (0,0xa)(4,0x32)(7,0x32); [2]0x8010a244 (1,0x32); [3]0x8010a268 (2,0x32) — alle `jal 0x80037edc` | bestaetigt |
| 3 | FUN_80037edc Byte-Vergleich @0x80037f0c | PSX.EXE | `sb v0,1(t0)` @0x80037f08, `lbu v0,1(t0)` @0x80037f0c, `sltiu v0,v0,0x78` @0x80037f14, `sb zero,1(t0)` @0x80037f24, Clamp `sltiu v1,v1,0x3`/`ori v0,zero,0x2` @0x80037f40/4c | bestaetigt (`(sum & 0xff) < 0x78` richtig) |
| 4 | Hundebiss @0x8010f33c-64, Bytes @0x80121014 | STAGE1.BIN / PSX.EXE | `jal 0x8001a780` @0x8010f310, `addiu v0,v0,2`/`sb aca59` @0x8010f31c/24, rng @0x8010f334, `andi v0,v0,0x1` @0x8010f34c, `jal 0x80037edc`+`ori a1,zero,0x32` @0x8010f360/64; Bytes 0x80121010 = `e8 08 11 80 05 07 04 06`; 0x8001a780 = ((a0.+0x6a - cur.+0x6a + 0x400) & 0xfff) < 0x800 | bestaetigt |
| 5 | Kraehe 0x801161e8, +0x1ec | STAGE1.BIN | Baender `slti 1500/3000/3600` @0x80116200/20/54 mit (1,a)(2,a) / (3,a)(4,0x28)(6,0x28)(5,0x28) / (0,8)(7,0x28); +0x1ec = `lhu 0x800aca8c` (Spieler.y) − `lhu 56(a0)` -> `sh v0,492(a0)` @0x801124cc und @0x80114f40 | bestaetigt |
| 6 | LUT @0x80074208, Index aca5c @0x80037c8c/94 | PSX.EXE | 128 Bytes identisch mit `s_panel_lut`; s3 = 0x800b10ec − 0x4618 = 0x800acad4 (@0x80037c2c-38), `lbu v0,-120(s3)` = 0x800aca5c @0x80037c8c, `sll v0,v0,4` @0x80037c94 | bestaetigt |
| 7 | Hook-A-Typen FUN_8011e864 @0x8011e9c0-ac8 | STAGE1.BIN | v0 = 0x8010a28c (@0x8011e9a4) nach 0x800ac758+4·{0x10,0x11,0x12,0x16,0x18,0x1c,0x1d,0x1e,0x1f}; v1 = 0x8010c16c (@0x8011e9b4) nach Typ 0x13; 0x20 -> 0x80111944, 0x21 -> 0x8011597c, 0x27 -> 0x8011c118 | bestaetigt |
| 8 | RE2-Hakenorte: Zombie P4 @0x80102968, Hund Boden-Biss @0x80104F7C-88 | EMZ0.BIN / EMD0G_MOD0.BIN | Mash-Freigabe `sb 4,6(s1)`+`sb 4,6(s3)` @0x80102890/94, P4 `sb 5,6(s1)` @0x8010298c; Hund `sh 3,6(s0)` @0x80104f7c, `bltz` (tot) @0x80104f90, sonst `jal 0x801051bc` @0x80104fa4 | Hakenort bestaetigt (PORT-MAPPING, RE2 hat kein Wundsystem) |

Rate-Woerter im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx`): 0 Treffer.
Env-Schalter: nur `RE15_KLEIDUNG_LOG` (Messhaken, schreibt nur stderr, kein Verhalten).

## 4. Verursacher-Commit (Gegenprobe Historie)

`git grep re15_wound_(add|release_stamp)(` an e11962ec~1, e11962ec, v0.8.21, v0.8.23, master: Aufrufe NUR in
enemy_ai_common.c (RE1.5-Gehirne) bzw. affen_11c0.c — in enemy_ai_re2_{zombie,dog,crow}.c an KEINEM Stand.
e11962ec (2026-08-22) aendert nur `s_flavor = RE15_AI_FLAVOR_RE2` (+ Test-Kontext). Historie davor:
a26fbe9d (2026-08-02) stempelte in `re15_player_take_damage` bei JEDEM Treffer (HP-korreliert), b26fe943
(2026-08-03) verlegte den Stempel an den Griff-Abwurf (Phase 4). Damit ist "seit e11962ec stempelt unter dem
Standardgehirn nichts" belegt.

## 5. Messung an der exe (HEAD 866af8e3, Standardgehirn RE2, beschleunigter Renderer)

Umgebung: die Maschine lief waehrend der Abnahme in einer RDP-Sitzung (ein Bildschirm `\\.\DISPLAY513`
1122x540); jede exe blieb an der SDL-Debug-Assertion `WIN_AddDisplay ... index == *display_index`
(SDL_windowsmodes.c:380) stehen. Fuer alle exe-Laeufe deshalb `SDL_ASSERT=always_ignore` (SDL-eigener
Assertion-Handler; keine Spiellogik). Messkopie `re15_pc_abn0b_hund.exe` neben der exe, Arbeitsordner im
Scratchpad. Ohne `RE15_TITLE_SHOT(_AF)` steht die exe im Titel (kein Auto-New-Game) — zwei erste Laeufe
deshalb ohne Spielbild, nicht gewertet.

### 5.1 Der dokumentierte Weg des Nutzers: Zwingerhunde ROOM1190

Beleg fuer den Weg: befund.log des Nutzers (Hauptbaum, nur gelesen) — einziger HP-Abstieg der Sitzung
in ROOM1190: `F2002 R1190 C11 hp=80`, `F2047 R1190 C2 hp=80 -> hp=60` (−20-Schritte), danach 65; Karte
`re15_card.mcr` (Kopie, Werkzeug `nullen` auf der KOPIE): Slots 0..4 alle `wunden 0/0 x8`, darunter
Slot 1/2 (ROOM1150, hp 65) und Slot 4 (ROOM1070, hp 20).

Befehl (Arbeitsordner `scratchpad/hund1190`):
```
SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_KLEIDUNG_LOG=1 RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450
RE15_DEBUG_DOGWAKE=500 RE15_FRAMEDUMP=500-1500/20:h_ RE15_EXIT_AT=1500  re15_pc_abn0b_hund.exe   -> RC=0
```
(Haken RE15_DEBUG_SUB/DOGWAKE = Sce_em_set der Hunde in sub 13, wie analysis/befunde_2026-09-27/
r34_sichtpruefung.sh; Leon steht ohne Eingabe.) debug.log:
```
[debug-menu] AUTO-JUMP -> ROOM1190 (Frame 240)
[kleidung] F582 hundebiss typ=0x20 dir=2 hp=80 gen=1 panels=0/0,0/0,0/0,0/0,0/50,0/0,0/0,0/0
[kleidung] F585 hundebiss typ=0x20 dir=2 hp=60 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/0,0/0,0/0
[kleidung] F587 hundebiss typ=0x20 dir=1 hp=40 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/0,0/0,0/50
[kleidung] F673 hundebiss typ=0x20 dir=0 hp=20 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/50,0/0,0/50
[kleidung] F708 hundebiss typ=0x20 dir=0 hp=0 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/100,0/0,0/50
```
Ergebnis: Leon geht ueber Caution (80/60/40) und Danger (20) bis in den Tod (0) — fuenf Bisse, 250
Wundpunkte auf Panel 4/5/7 verteilt, **kein Panel erreicht 0x78, gen bleibt 1, es wird NIE ein Decal
geblittet**. Bilder h_000560/600/680/700 (scratchpad/hund1190/hund_reihe.png) zeigen den Ablauf (Hunde an
Leon, Blutfahnen am Biss), aber keine Wunde auf der Kleidung. Der Unit-Riegel unit_r36_kleidung_hund zeigt
dasselbe (4 Bisse hp 100->20 auf Panels 4/5/6/7, kein Stempel) und prueft nur die FORM je Biss.

Original zum Vergleich (selbst disassembliert, STAGE1.BIN): der RE1.5-Hundebiss kostet **10 HP**
(`lhu player.hp` @0x8010f2bc, `addiu a0,v0,-10` @0x8010f2c8, `sh a0,player.hp` @0x8010f2d0) und stempelt
+0x32 (@0x8010f360/64) auf eines von ZWEI Panels je Blickrichtung (05/07 bzw. 04/06). Von 100 bis 20 HP
sind das 8 Bisse = 400 Punkte auf 2 Panels -> mindestens ein Panel >= 150 (Schubfach) -> Decal sicher vor
Danger. Die PORT-MAPPING der Spur haengt dieselben +0x32 an den RE2-Biss, der im Port **20 HP** kostet
(Dossier §1.2 "@0x80104EB8") — das halbiert das Verhaeltnis Wundpunkte:HP-Verlust; auf dem Hundeweg
entsteht die vom Nutzer beschriebene Kopplung "Warning/Danger -> Blut/Risse" damit nicht.

(wird fortgesetzt: 5.2 Zombie-Weg selbst gemessen)
