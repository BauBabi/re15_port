# Runde 41 — Spur B "re2gator" — Abnahme 6 (unabhängig)

Prüfer: Abnahme-6-Agent, 2026-10-24 (ab 18:00). Baum `.claude/worktrees/r41_re2gator`, HEAD ee006df1 (Zweig r41/re2gator,
merge-base mit master 116e8b0f). Geprüft wird der Stand der Nachbesserung 6 (16 Commits 27b7c1d1 … ee006df1, Code-Diff
`git diff 9de8e9f4..HEAD -- re15_port`, 9 Dateien +1116/−117). Aus Dossier und Bau-Meldung wird nichts ungemessen übernommen.
Messdateien: Prüfer-Scratchpad
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/B41abn6/`
(Läufe unter `mess/<Lauf>/`, eigene exe-Kopien `re15_pc_b41a6_<Lauf>.exe` bzw. `re15_pc_b41a6z_<Lauf>.exe` für Elza, je Lauf
`lauf.txt` mit allen Variablen). "Vorher" = Läufe der Abnahme 5 (`.../scratchpad/B41abn5/mess/<Lauf>/`).

(Fortlaufend geschrieben; Gesamturteil am Ende.)

## 1. Lektüre + Bau

- Gelesen: AUFTRAG.md (Spurbaum-Stand bis Nachtrag 11; die neueren master-Einträge I-E1/I-E2, Nachtrag 12-14, E-E4, H-E1 betreffen
  nur C/E/H/I — per grep auf "Spur B|gator|finisher" geprüft, 0 Treffer), VERTRAG.md, B_abnahme_5.md vollständig (M1–M3, H1–H4),
  Dossier B_re2gator.md §14.0–§14.9, `git log --oneline master..HEAD`, `git diff master...HEAD --stat` (Zweig gesamt 73 Dateien,
  +12000/−68) und den Code-Diff der Nachbesserung 6 vollständig (`B41abn6/nb6.diff`, 1400 Zeilen): enemy_ai_boss_gator.c (+41/−?:
  P3 ohne Ende, `pl->y = e->y` in P0, Feld p3_kreise), gator_re2_r41.c (+380: gr2_fin_laufend, gr2_raum_objekte/gr2_obj_im_strahl/
  gr2_obj_in_cut/re15_gr2_raum_verdeckt, gr2_fin_unverdeckt, gr2_fin_fenster grob→fein mit Massstab-Ordnung, re15_gr2_zopf_massstab,
  re15_gr2_leon_knochen, s_wahl_ohne_laufend im Rückfall), gator_re2_pc.c (+81: Messzeile Maul-Sitz mit "Opfer gehalten/frei", Summe im
  Game-Over-Übergabebild, Zopf-Abstand), re15_gator_re2_r41.h (+16), platform/pc/main.c (+1 Zopf-Haken), Riegel test_r41_re2gator.c
  (+322), probes/r41_re2gator.cmake (+8 Kommentar), integration/test_r41_re2gator.cmake (+102).
- Bau: `bash re15_port/tools/local_build.sh configure` → `LOCAL-BUILD-OK (configure)` (18:03); alle 15 .c/.h/.inc-Dateien des Zweigs
  (`git diff --name-only master...HEAD -- re15_port`, `B41abn6/touched_files.txt`) per `touch` (nur mtime), dann
  `local_build.sh build` → 798 Schritte, `LOCAL-BUILD-OK (build)`, re15_pc.exe 18:03:33 (`B41abn6/build.log`). Warnungen nur
  main.c 8803/10134/10514 (`-Wdiscarded-qualifiers`, Altbestand), keine in Spur-B-Dateien.
- Gezielte Unit-Riegel: Regex des Bau-Agenten **erweitert** um `r38_zopf_.*` (der neue main.c-Haken liegt im Zopf-Zweig des
  Spieler-Zeichners, den der Bau-Agent nicht mitgefahren hat) sowie `re2_victim_collapse|dog_grab_anchor|re2_dog_grab_anchor`
  (Opfer-Modus-Nachbarn; `B41abn6/unit_regex.txt`): `ctest -R "<regex>" --timeout 600 -j 6 --output-on-failure` → **100% tests
  passed, 0 tests failed out of 131** (18:05:34–18:07:32, `B41abn6/ctest_unit.log`; unit_gator_sweep 116,7 s, unit_r41_re2gator_buehne
  35,3 s; unit_r38_zopf_libgte/init/ruhe/drehung/laufen/freeze/original grün).
  Einzeln mit -V (`B41abn6/unit_maul_V.txt`):
  - `unit_r41_re2gator_maul`: Gegenproben `NB5-Buehne E1 (5638,-12546) Gier 2323 Cut 4 … von Raum-Objekten verdeckt 196` und
    `NB5-Hoehe (Leon P1 auf y 0): 36 Promille … ueber der Wasserlinie y -1620`; sechs Lagen je `Finisher 434 Bilder (P1 14 P2 120 P3
    300)`, Bühne `Cut 6 (laufend 0/2/2/3/3)` bzw. `Cut 1 (laufend 6)` (Plattformblock), `Maul >= 50 %: P1 0 P2 106 P3 300`, `Kiefer-Abstand
    max 2`, `FRESSEN-Ende -1, Opfer nicht gehalten 0, P3 nicht geparkt 0, Massstab falsch 0, von Raum-Objekten verdeckt 0 Bilder (max 0
    Vertices, Koerper im Bild min 0), P1 ueber der Wasserlinie y -1620 min 762 Promille` → `maul: OK (0 Fehler)`. ("Koerper im Bild min
    0" = Wurf-Bilder, Leon oben hinaus wie RE2; "oben hinaus 16".)
  - `unit_r41_re2gator_buehne`: `280 Bisslagen (Raster 2000 x 4 Gier) x laufende Zonenkamera = 2240 Suchen, ohne Buehne 0, Buehne =
    laufende Kamera 0, Buehne falsch 0, angezeigt/angefordert-Paar falsch 0, Versatz max 19428; … Buehnen-Kameras 0..7: 0 280 0 0 0 0
    1960 0` — die Bühne ist ortsfest (Cut 6, bei laufender 6 Cut 1); Gator-Versatz bis 19428 (Dossier §14.5 nennt den Wert nicht).
  - `unit_r41_re2gator_kamera`: `FINISHER: … cut 5 … Kamera im Biss-Bild 0/0`, `FINISHER-Halt: 400/400 Takte Cut 5 gesperrt`.

## 2. RE-Stichproben (selbst disassembliert; `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` bzw. `re15_disasm.py`)

RE2-Overlay `info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN`, RE2-EXE `info/re2leon/PSX.EXE`, RE1.5-EXE `info/Re1.5/PSX.EXE`.
Ausgaben `B41abn6/re2_dispatch4.dis`, `re2_p3.dis`, `re2_leon_handler.dis`, `re2_p0.dis`, `re2_80015d60.dis`, `re15_80021bbc.dis`,
`re15_800142f4.dis`, `re15_8002c18c.dis`, `re15_8001e8fc.dis`, `re15_8001e9f0.dis`.
- **RE2 FRESSEN-Dispatch / P3 (M1):** `lbu v1,6(s0)` @0x80100f74; `addiu v0,zero,3 / beq v1,v0,0x801011c4` @0x80100fa8-ac. P3
  @0x801011c4-e4 = `addu a1,s1,zero / addu a2,s2,zero / jal 0x80015cb8 / addu a3,zero,zero / addu a0,s0,zero / addiu a1,zero,11 /
  addu a2,zero,zero / jal 0x8001a240 / addu a3,zero,zero`, danach Epilog `lw ra,28(sp)` @0x801011e8 … `jr ra` @0x801011fc. Das
  `sb v0,6(s0)` @0x801011c0 liegt im Delay-Slot des `j 0x801011e8` @0x801011bc und gehört zum P2-Ende. **Kein Phasen-Schreiber in P3 ✓.**
- **RE2 Leon-Handler @0x80102d44 (M1):** `lbu v1,6(s0)` @0x80102d60; `addiu v0,zero,3 / beq v1,v0,0x80102f24 / addiu v0,zero,128`
  @0x80102d94-9c; Phase 3 @0x80102f24-28 = `lui at,0x800d / sb v0,-565(at)` → 0x800cfdcb (= Basis 0x800cfbf8 + 0x1D3) und Epilog.
  Animation `jal 0x8002959c` nur in Phase 2 @0x80102f08, Platzierung `jal 0x80015cb8` @0x80102ed4 — in Phase 3 keine. ✓ (Kleinigkeit:
  `sb` speichert 0x80, der Code-Kommentar schreibt "|= 0x80" — für den Port ohne Wirkung, er führt nur das Gehalten-Bit.)
- **RE2 FRESSEN P0 (M2/M3):** Sperre `ori v0,v0,0x100 / sw v0,0(a0)` @0x80100fcc-d0 (a0 = 0x800cfbd8), Cut 14/7 `addiu a0,zero,14`
  @0x80100fec / `addiu a0,zero,7` @0x80100ff0 / `jal 0x8002c7ac` @0x80100ff4; Kanister aus `sw zero,1308(at)` @0x8010100c (0x800d051c);
  Gator-x fest `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c; Leon `addiu v1,zero,-915 … sw v1,-968(at)` @0x80101070-78
  (z = −915 absolut) und `addiu v0,v0,10643 … sw v0,-976(at)` @0x8010107c-84 (x = Gator-x + 10643) — kein Schreiber auf Leons y. ✓
- **RE2 FUN_80015cb8 (EXE):** `lh v0,358(s1) / lh v1,50(sp) / addu / sw v0,60(s1)` @0x80015d6c-7c (y = Anker +358 + Versatz). ✓
- **RE1.5 laufende Kamera (M2):** FUN_80021bbc: `lbu v1,-1099(v1)` (0x800afbb5) @0x80021be0, `sh v0,4072(at)` (0x800b0fe8, voriger
  Cut) @0x80021bf4, **`sh v1,4068(at)` (0x800b0fe4) @0x80021bfc**, danach `jal 0x80014324` + `sw v0,-14448(at)` (0x800ac790,
  Regionszeiger des angezeigten Cuts) @0x80021c00-0c. FUN_800142f4: **`sb a0,-1099(at)` (0x800afbb5) @0x80014300** (Cut-Anforderung;
  das Dossier nennt die Funktion "RVD-Scan" — sie ist der Anforderungs-Setzer, den RVD-Scan und Cut_chg rufen; Adresse/Wirkung stimmen). ✓
- **RE1.5 Objekt-Zeichner FUN_8002c18c (M3):** Pool 0x800b3f98 (`addiu v0,v0,16280` @0x8002c1f0), Schritt `addiu s6,s6,148` @0x8002c1f8,
  aktiv `andi v0,v0,0x1` @0x8002c204; Typ 4 `ori v0,zero,0x4 / bne` + `addiu v0,v0,-900 / sw v0,96(s1)` @0x8002c234-50; **Typ 3
  `ori v0,zero,0x3 / bne v1,v0,0x8002c2d8` @0x8002c2bc-c0 → `jal 0x8003e64c` @0x8002c2c8 → `j 0x8002c3bc`**; sonst `lw a1,0(s5)` (s5 =
  0x800aca38 − 680 = 0x800ac790) / `jal 0x80014368` @0x8002c2d8-dc / `beq v0,zero,0x8002c3bc` @0x8002c2e4 (Regions-Cull). ✓
- **RE1.5 Wurzel-Skala / Zopf-Flag (Nebenbefund Elza):** `andi v0,v0,0x800 / beq` @0x8001e904-08, Skala `lh v0,358(v1)` dreimal und
  `jal 0x80065ff0` (ScaleMatrix) @0x8001e940; FUN_8001e9ec: `andi v0,v0,0x40 / bne v0,zero,0x8001ea2c` @0x8001ea10-14 überspringt die
  Ketten-Komposition `jal 0x80022da0` @0x8001ea24 (Teile mit eigener Weltmatrix). ✓
- Schluss: **keine falsche Adresse**; alle stichprobenhaft geprüften Stellen (8 Funktionen, 2 Binärdateien je Spiel) enthalten das
  Behauptete.

## 3. exe-Läufe (eigene Kopien `re15_pc_b41a6(z)_<Lauf>.exe`, Ordner `B41abn6/mess/<Lauf>/`)

Basis (`B41abn6/lauf.sh`, = Rezepte der Abnahme 5): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_STATE_LOG=state.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=audio.raw`, `timeout`,
Umgebung ohne RE15_*-Testhebel (`env -u …`; Aufrufer-Umgebung geprüft: keine RE15_/SDL_-Variable); Elza (`lauf_elza.sh`) mit
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`. Neu: `EXIT_AT=251#1240` = durch das Game Over bis in
den neu gestarteten Raum 1240 (Bildzähler-Reset trennt die Räume). Auswertung `grlauf.py` (state.log je Raum-Block: gr-/cam-Läufe,
HP-Wechsel, letzte Zeile), `bisse.py`, `sheet.py`/`crop.py` (Bildbögen/Ausschnitte; Aufhellung nur zur Sichtung), `schuettel.py`.

### 3.1 Regression Punkte 2/3 (NB6 ändert nur FRESSEN/Bühne/Messzeilen + Zopf-Haken)
- **state.log bitgleich mit Abnahme 5** (`cmp`): D2 (1351 Zeilen), C3 (1651), K (511), N (1351). D2-debug.log weicht nur in der
  Titel-Restzeile `[tfade] … (Rest vom Titel: 239/231)` ab (Wanduhr), sonst gleich.
- D2: `RE2 DASH Planung: an (-6052,-18113) yaw 1139 -> Start-Gier 624 (Drehung 515)`, `Vordrehung … F481`, `RE2 PUTZ runde 1/6 … 6/6
  spawns=9 gesamt=54`, `DASH ende … wand_draussen_max=0`, 25 `Putz-Sicht`-Zeilen (debug.log), `ENEMSE Bank 17 geladen: 7 VAGs`, 27
  `bank=17`-SE; Schütteln `schuettel.py mess/D2/fd d_ 536 560 529`: F542 (0,+2) F543 (0,0) F544 (0,+2) F545 (0,+3) F546 (+1,0) F547 (0,0)
  F548 (0,+1) F549–F551 (0,0) F552 (0,+1) F553 (0,+1) F554–F555 (0,0) F556 (+1,+1) — Bild für Bild wie Abnahme 5.
- C3: `CROSS start=(4450,-11900) ende=(3350,-20700) frames=240` (Plattform-Überquerung), `KANISTER Zielmodus 1 … F1107`, `angehaengt
  lokal=(4614,-253,616) F1119`, Kanister-Sitz je 30 Bilder `Abweichung max 0`, `GIER-WACHE bis F1302: … Spruenge > 0x60: 0, Befreiungen 0`.
- K: `KANISTER angehaengt lokal=(4373,-161,11) F18`, `R1 VSync 4 (Zeitlupe) F70`, `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F70`, `R1 ende
  (Leiche)`, `VSync 2 F108`; Bild `B41abn6/K_kiefer.png`: F64–F70 Gator mit Kanister, Schuss F70, F72–F80 Blitz/Splitter am Kopf, ab F82
  Leiche mit geplatztem Unterkiefer.
- N: `Sicht F1 untergetaucht pos=(-5300,0,-16300)`, `Sicht F201 aufgetaucht pos=(5085,-1200,-19880) clip=6 bild=0`.

### 3.2 Punkt 1 — Finisher (Mängel M1–M3 der Abnahme 5), gemessen

| Lauf | Rezept (zusätzlich zur Basis) | Tod (bisse.py) | Kamera vor dem Biss → Bühne (gator_boss.log) | state.log 2090-Block | Maul-Sitz Ende (debug.log) |
|---|---|---|---|---|---|
| E1 | `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A0 RE15_GB_TEST=1 RE15_EXIT_AT=251#1240 RE15_FRAMEDUMP=590-910:fd/e_` (echter Türweg: `DOOR FIRE slot=0 … spawn=(6300,-1800,-16250)`, `Sequenz fertig: 286 Bilder`) | F627 GUARD, `hp 0 -> -50`, 64° neben Leon | cam=2 F76–F626; `von (5763,-12762) Gier 115 -> (-5650,-16000) Gier 2929 Cut 6, Versatz 11852 … laufende Kamera 2/2 ausgeschlossen, Raum-Objekte 3, Leon-Stichpunkte 15, wegen Verdeckung verworfen 24 …, Massstab Koppelbild 1357` | gr=0 F1–F626, **gr=1 F627–F902**, cam=6 F627–F902; danach Block 1240 F0–F251 gr=0 hp=100 pst=0 mo=200 | `F902: Bilder 275 (P1 14 P2 120 P3 141), … Kiefer-Abstand max 2, Kopf 275/275, Leon 259/275 + oben 16 (Cut 6), Wasser 275/275, Opfer gehalten 275/275, Opfer frei 275/275 …, letztes Finisher-Bild F902 (Game-Over-Uebergabe)` |
| A2 | `RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_EXIT_AT=251#1240 RE15_FRAMEDUMP=380-700:fd/a_` | F397 CHASE, 0° | cam=0 F280–F396 → `(-7407,-22250) Gier 2569 -> (-5650,-16000) Gier 2929 Cut 6, Versatz 6476 … laufende Kamera 0/0 ausgeschlossen` | gr=1 F397–F672; 1240 danach gr=0 hp=100 | `F672: 275 (14/120/141) … gehalten 275/275, frei 275/275 … (Game-Over-Uebergabe)` |
| MN | `… RE15_PLAYER_POS=4000,-15000,2048,1 … EXIT_AT=251#1240 FRAMEDUMP=570-880:fd/m_` | F587 GUARD | cam=0 → Cut 6, Versatz 11852 | gr=1 F587–F862 | `F862: 275 … gehalten/frei 275/275` |
| BL | `… RE15_PLAYER_POS=0,-13200,2048,1 … FRAMEDUMP=510-820/2:fd/b_` | F526 CHASE (Band-Fall) | cam=0 → Cut 6, Versatz 11402 | gr=1 F526–F801 | `F801: 275 … gehalten/frei 275/275` |
| G1 | `… RE15_PLAYER_POS=-7950,-25050,2048 … EXIT_AT=251#1240 FRAMEDUMP=390-700:fd/g_` | F399 CHASE | cam=0 F280–F398 → `(-5734,-21085) Gier 1499 -> (-5650,-16000) Gier 2929 Cut 6 … laufende Kamera 0/0 ausgeschlossen` | gr=1 F399–F674 | `F674: 275 … gehalten/frei 275/275` |
| G2 | `… RE15_PLAYER_POS=-5950,-20550,2048 … EXIT_AT=251#1240 FRAMEDUMP=430-760:fd/h_` | F440 CHASE | cam=6 F432–F439 → `(-7224,-18989) Gier 3209 -> (2100,-24500) Gier 1951 Cut 1, Versatz 10788 … laufende Kamera 6/6 ausgeschlossen …, Massstab Koppelbild 1350` | gr=1 F440–F715 | `F715: 275 … Leon 257/275 + oben 18 (Cut 1) … gehalten/frei 275/275` |
| EZT (Elza) | `lauf_elza.sh`, `RE15_DEBUG_JUMP=20A0@250 RE15_FIRE_AOT=0@40#20A1 RE15_GB_TEST=1 RE15_EXIT_AT=1300 RE15_FRAMEDUMP=600-910:fd/z_` (`Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `JUMP -> … (ROOM20A1)`, Raum 2091) | F627 GUARD | cam=2 → Cut 6 (wie E1) | gr=1 F627–F902 | `F902: 275 … gehalten/frei 275/275, Zopf-Glied 0 zum Kopf-Knochen max 244 (ohne Massstab 3207) … (Game-Over-Uebergabe)` |

**M1 (Opfer bis zum Raumwechsel im Maul) — behoben.** In allen sieben Läufen steht gr=1 lückenlos vom Biss bis zum letzten Bild des
Raums (Bildzähler-Reset); P3 läuft 141 Bilder bis zur Game-Over-Übergabe (Code: P3-Zweig ohne Ende, `if (g->timer >= 221 + 59)
g->timer -= 59` hält den Clip-11-Kreis, enemy_ai_boss_gator.c; `victim_force_end`/`render_scale 0`/`CHASE` nach P3 entfernt). Bilder:
`B41abn6/E1_ende_zoom.png` (×2,5 aufgehellt) F760–F902: Gator aufgerichtet, Leon (blau) im Maul bzw. Beine aus dem Maul (F800/F860, wie
Abnahme 5 in P3), **F902 (letztes 2090-Bild, ungeblendet) Leon im offenen Maul — kein Bild mit stehendem Opfer** (vorher
`B41abn5/E1_ende2.png`: F882–F902 Leon aufrecht); `A2_p3_zoom.png` F540–F672 ebenso (F672 Leon im Maul); `EZT_ende_zoom.png` Elza F800/F860
Beine aus dem Maul, F902 im Maul. Nach dem Raumwechsel ist der Spieler wieder normal (1240-Block: gr=0, hp=100, pst=0, mo=200 in E1, A2,
MN, BL, G1, G2).

**M2 (Bühne nie in der laufenden Kamera) — behoben.** Biss-Bild zeigt die alte Kamera, das nächste die Bühnen-Kamera: E1 F627 Cut 2 →
F628 Cut 6 (`E1_schnitt_p1.png`), A2 F397 Cut 0 → F398 Cut 6 (`A2_schnitt.png`), MN F587 → F588 (`MN_schnitt.png`), BL F526 → F528
(`BL_schnitt.png`), **G1 F399 Cut 0 (Seitenansicht, Biss) → F400 Cut 6** (`G1_schnitt.png`; vorher `B41abn5/G1_gleiche_kamera.png`: kein
Schnitt, Gator dreht 101°), **G2 F440 Cut 6 → F441 Cut 1** (`G2_schnitt.png`; vorher `B41abn5/G2_gleiche_kamera.png`: Leon springt im
selben Bild), Elza F627 Cut 2 → F628 Cut 6 (`EZT_schnitt.png`). In keinem Lauf ist ein Lagesprung in derselben Kamera zu sehen. Der
Kamerawert, den die Bühne ausschließt, ist der angezeigte (`work_vars[0x0A]`, room_common.c re15_cam_present_tick = Apply @0x80021bfc)
und der angeforderte (`cam_id` = 0x800afbb5); state.log `cam=` zeigt den angeforderten Cut (im Biss-Bild schon 6).

**M3 (Opfer in P1 sichtbar) — behoben.** P1 (14 Bilder) in beiden Bühnen gemessen: Cut-6-Bühne E1 F628–F640 (`E1_p1_zoom.png`, ×4,
Aufhellung ×2), A2 F398–F410, G1 F400–F412, Elza F628–F640 (`EZT_zopf_zoom.png`), Cut-1-Bühne G2 F441–F454 (`G2_p1_zoom.png`): das Opfer
steht groß und frei vor dem frontalen Gator, Unterschenkel unter der Wasseroberfläche abgedunkelt (watend), keine Leiche/kein Objekt
davor, keine PRI-Maske über der Figur; F641/F642 (G2: F456) gepackt. Die treibende Leiche obj 2 liegt in der Cut-1-Bühne links hinter Leon
(G2 F456–F715), nicht im Sichtstrahl. Elzas Zopf sitzt in P1 am Hinterkopf (`EZT_zopf_zoom.png`), kein gelber Fetzen (Messzeile 244 statt
3207).
