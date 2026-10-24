# Runde 41 — Spur C "ventilkette" — Abnahme 3 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD 2fb94b25 (Basis master 116e8b0f;
master selbst steht inzwischen auf 7b59e297 mit Spur A — die Integration ist nicht Gegenstand dieser Abnahme).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Nachtraegen 6-10 und den Entscheiden
C-a..e, C-E1..C-E3, C-E2b (C-E2c widerrufen durch Nachtrag 6). Vorlage: Abnahme 2 (`C_abnahme_2.md`, Maengel N1-N4 + H-a),
Nachbesserung 3 (Dossier §0d).
Begonnen 2026-10-24 11:55. Pruefer: Abnahme 3 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad
`C41abn3/` (unten `<a3>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn3`).

## 0. Stand / Protokoll (neueste zuerst)
* 12:5x FORTSETZUNG (zweite Pruefer-Sitzung, Abnahme 3; die erste endete ~12:23 am Sitzungslimit). HEAD jetzt c8655d74
  (nur `analysis/befunde_runde41/AUFTRAG.md` +27 Zeilen = Nachtrag 11 fuer Spur M, kein Code) -> gepruefter Code-Stand
  unveraendert 2fb94b25. `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `_kette uebersprungen:
  Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` (`<a3>/configure2.txt`, `<a3>/build2.txt`), exe 11:03
  7803557 B. Die Integrationsgruppe der ersten Sitzung (12:00) ist nie ueber den ersten Riegel hinausgekommen
  (`<a3>/ctest_integ.txt` endet bei `Start 574`) -> neu gestartet (`-j 2`, Protokoll `<a3>/ctest_integ2.txt`), Ergebnis §3.
  Rohdaten der ersten Sitzung, die im Bericht noch fehlten (`<a3>/mess/*`: leiter3, kette_b, hebel_vorher, pult2050, r2070,
  generator, sicherung2060, w3 ...), werden ausgewertet, nicht wiederholt.
* 12:05 Integrationsgruppe (16 Riegel, seriell: r41 x5 + Raumgruppe 2030/2050/2000/20B0/2070) im Hintergrund gestartet
  (`<a3>/regex_integ.txt`, Protokoll `<a3>/ctest_integ.txt`); Ergebnis §3.
* 12:03 `test_r41_ventilkette.exe` direkt: `== Leon` / `== Elza`, `J1 Zensus: 206 Raeume, 16 Funde`, `L Zensus: 14 Stellen
  (3,126)/(3,108); Set(3,126) 2 (geloescht 0), Set(3,108) 2 (geloescht 0)`, **`PASS (376 Pruefungen)`**, rc=0
  (`<a3>/unit_r41_v.txt`).
* 12:02 Unit-Gruppe (Regex `<a3>/regex_unit.txt` = Regex der Abnahme 2 + unit_r37_feuerloescher_kiste,
  unit_r35_inventar1050_kiste, unit_r34n_c_generator, unit_r31_generator; 166 Tests):
  `ctest --test-dir re15_port/build -R "<regex>" --timeout 600 --output-on-failure -j 4` ->
  **`100% tests passed, 0 tests failed out of 166`** (18.28 s, `<a3>/ctest_unit.txt`).
* 11:57-12:00 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, Zeile 180 `-- integration_r41_ventilkette_kette
  uebersprungen: Spur A fehlt (engine/src/wasser20b0_r41.c, AUTO-Haken re15_wasser20b0_r41_auto_aot)` (C-E1);
  `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a3>/configure.txt`, `<a3>/build.txt`).
  exe `re15_pc.exe` 11:03 (7803557 B); letzter Commit mit Spiel-Code 07be0846 (10:50), danach nur tests/tools/analysis
  -> die exe ist der Stand HEAD. `git status --porcelain` leer.

## 2. Gates (fortlaufend)

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung; Hauptbaum nur gelesen)
* EXE selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, `info/Re1.5/PSX.EXE`):
  1. **@0x8003ed74** `sw zero,4136(at)` -> 0x800b1028 (Raumladen wischt Bank 5 Wort 0) und **@0x8003ec1c** `sw zero,4140(at)`
     -> 0x800b102c, direkt vor `jr ra` @0x8003ec20 (Wort 1 = Ein-Bild-Handshake am Ende jedes VM-Laufs). Stimmt mit dem Kopf
     (RE15_R41C_FLAG_TIMER/_FLUT in Bank 5 Wort 0) ueberein.
  2. **@0x800456ac** `andi v0,s0,0x80` (Wasserboden), @0x800456b0 `beq v0,zero,0x800456dc`, **@0x800456b8/bc** `lui a0,0x605` /
     `ori a0,a0,0xd00` = Effekt 0x06050D00; Aufrufer **@0x8003253c** `andi v0,v1,0x4000` -> @0x8003255c `jal 0x80045630`.
     Stimmt (Spritzer-Code und Fuss-Aufsetzer-Herleitung im Kopf).
  3. **@0x8002c24c** `addiu v0,v0,-900` nach `lbu v1,8(s1)` / `ori v0,zero,0x4` / `bne v1,v0,0x8002c254` @0x8002c234-3c und
     `lw v0,96(s1)` @0x8002c244 (y-Lage +0x60 von Typ-4-Objekten) — genau das im Kopf zitierte, als ENTLIEHEN und PORT-WAHL
     gekennzeichnete Mass RE15_R41C_ERTRINK_TIEFE. **@0x800218d0** `sra a0,v0,23` nach `sll v0,v0,16` = level >> 7 (Helligkeit
     der Blende). Stimmt.
* RDT-Bytes selbst gelesen (`<a3>/py/bytes.py`, `shared_assets/PSX/STAGE2`):
  ROOM2060/2061 **@0x012AE `22 03 7e 01`** hinter @0x01292 `06 00 1e 00 21 03 7e 00 21 05 06 01 21 05 07 00 21 05 08 01 21 05
  09 00 04 ff 18 0d` (Ifel Ck(3,126)=0, Schalter 1,0,1,0, Evt_exec sub13); ROOM2030 **@0x01FF2 `22 03 6c 01`** in sub06 @0x01FE4
  (`2b 02 ff ff` Frage, `21 0c 1f 00` Ja, Set(3,108), `46 09 00..` Slot 9 aus, `2b 03 ff ff`); ROOM2030 sub01 **@0x01E10**
  `06 00 1a 00 21 03 7e 00 46 05 01 31 01 00 ff ff 00 00 46 07 01 31 01 00 ff ff 00 00 08 00 01 00`; @0x01BA6 `21 03 6c 00 2c 09
  03 31 01 00 f4 e8 fa 19 20 03 e8 03 ff 00 18 06 ..` (Sicherung Slot 9, Band 1) und `50 0a 09 31 01 00 fe 10 fa 19 20 03 e8 03
  16 00 0e 00 6c 00 ff 00` (Slot 10 Item 0x16 x14, Band 1); Treppe **@0x01B8E** `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00
  02 00 00 00` (x -6050..-2350, z -8400..-7000, Band 0x80); ROOM2050 **@0x01F88** `2d 00 00 00 05 00 0a 00 00 00 1f e6 f0 f1 ..`
  = Obj_model_set obj 0 bei (-6625, **-3600**, -1850); ROOM2080 sub02 @0x00C86 (108 B, `22 03 f5 01` @0x00C94, `2f 01 32 00`
  @0x00CC8, For `94 00` @0x00CCC) unveraendert; 20B0 @0x019C2 / 2070 @0x00A5A/@0x00A7A Tuersaetze nach Stage 1 Raum 3 Ankunft
  (6550,-5400,-100) wie im Kopf. ROOM2030 = ROOM2031, 20B0 = 20B1, 2060 = 2061, 2080 = 2081 Byte fuer Byte (`cmp`).
* SCA ROOM2030 @0x880 (Kopf `50 da 0e cb`, Zellen selbst geparst als w,d,x,z,typ,u0,u1,fl): Zelle 3/4 (Steg-Gelaender) und 5/10/11
  (Landeplatz-Gelaender) fl **0x11**; Bodenzellen 13 (Nord-Landeplatz x -2550..5000 z -8500..-6900) und 16 (Sued-Landeplatz
  x -5600..4800 z 6350..7900) u1 01 fl 0x01; Ring-Boden 36-41 @0x00A48..@0x00A84 fl **0x23**; Ring-Reling 19 (x 5400..5900),
  20 (z -9000..-8500), 21 (z 8150..8650), 22 (x -6350..-5850), 31/32 (NW-Balkon), 23 (z 2450..2950), 26/27 (Aussenwand
  x -9700..-7700) fl **0x33** — alles wie im Kopf von re15_ventilkette_r41.h zitiert.
* Bild-Beleg der Relinghoehe (C_belege/nb3_reling_hoehe_sheet.png, Ausschnitt Cut 5 vergroessert `<a3>/reling_cut5_zoom.png`):
  die oberste Projektionslinie (+1800 ueber dem Landeplatz) liegt auf dem oberen Handlauf der unteren Reling, die Null-Linie
  am Fuss der Pfosten. Damit ist y -3600 als "Oberkante der unteren Reling" belegt; die Lesart "Oberkante der Ring-Reling"
  waere y -7200 = der alte geflutete Zustand (Ring unter Wasser) und widerspraeche "nur noch der untere Bereich geflutet".
* Netz der Relinghoehe (gen/ventilkette_r41_wasser.inc `re15_r41c_wasser_reling_md1`, 4052 B) selbst geparst: 117 Ecken alle
  y 0 (Prop auf y -3600), 95 Vierecke, groesste Kante 1770, Flaeche 200 820 000 = Summe der vier Rechtecke
  RE15_R41C_RELING_RECHTECKE (exakt, keine Ueberdeckung ausserhalb). Der Ostrand x 5400 / Nordrand z -8500 / Suedrand z 8150 =
  Innenkanten der Ring-Reling-Zellen 19/20/21.
* Zensus (eigener ROH-Scan aller 240 ROOM*.RDT nach `22 03 7e`/`22 03 6c` in jeder Byte-Lage, Ueber-Approximation des
  Walkers): (3,126) nur ROOM2060/2061 @0x012AE op 1; (3,108) nur ROOM2030/2031 @0x01FF2 op 1; kein op 0 / op 7 irgendwo
  -> "kein Loescher" haelt.
* Diff-Suche (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
  TODO/FIXME/approx/ungefaehr/geschaetzt/vermutlich/wahrscheinlich/sieht richtig: **0 Treffer**; "Platzhalter" nur als Nutzer-
  Wortlaut (Ventilraetsel als Platzhalter). Kein neues `getenv` in engine/include/platform.

* Zusaetzliche Stichprobe der Fortsetzung (eigene Byte-Lesung `shared_assets/PSX/STAGE2`, Python): ROOM2000 @0x01B36 `0d 00 04 00
  b4 00 02 00 0e 00` (For 180 { Evt_next } Next = Hintergrundfaden-Form des Zaehlers), @0x0169E `3b 04 02 31 01 00 30 94 48 26 e8 03
  e8 03 ...` (Leiter-Tuer Slot 4, x -27600 z 9800 1000 x 1000), @0x0174C `3a 00 0b 00 ...` = 20B0 @0x01E96 (gleiche Strahl-Bytes);
  20B0 @0x01FB2 `09 0a 3c 00` (Sleep 60), @0x01FBC `22 03 eb 01` (Set(3,235)), @0x019C2 `3b 03 02 31 01 00 4a 15 24 e1 e8 03 b8 0b
  96 19 e8 ea 9c ff 00 08 01 03 ...` (Tuer -> Stage-Index 1 Raum 3, Ankunft (6550,-5400,-100)); ROOM2030 @0x01B6C `2d 00 00 00 00
  00 0a 00 ...` (obj 0), @0x01ED2 `2e 03 01 00 2f 01 88 ff 0d 00 06 00 2d 00 30 02 02 00 0e 00 34 01 d0 d5` (Lift-Fahrt-Form),
  @0x01EF2 `2f 01 78 00`, @0x01F02 `34 01 e8 ea` (Endlage -5400); ROOM2050 @0x01D38 `2c 08 03 31 03 00 a0 0f fe de 08 07 20 03 ff
  00` (Pult-Platz Band 3), @0x022E6 `2b 00 ff ff 02 00 06 00 d0 00 21 0c 1f 00` (Hebel sub04: Frage, Ja), @0x0201A `46 06 03 31 ff
  00 18 02 00 00` (Kisten -> Ereignis 2); ROOM2080 @0x00CC8 `2f 01 32 00 0d 00 06 00 94 00` (Rate +50, For 148). **Alles wie im
  Kopf zitiert.**
* **Erklaert der Fix den Befund?** (Abnahme 2 L11): N2 — `ring_sperren()` prueft jetzt `zustand_2030() == GEFLUTET` (Bit 103 UND
  (3,126)=0, ventilkette_r41.c), vorher nur Bit 103 -> erklaert `ring_strom` (Flut + Strom -> Sperrtext 41); gemessen L6/L9 (mit
  Strom `DOOR FIRE`, ohne Strom Sperrtext). N3 — dritter Zustand RELING (Flaeche obj 4 y -3600 nur ueber der Schachtoeffnung,
  Treppe Slot 8 -> Text 48 Band 3) erklaert "nur zwei Zustaende"; gemessen L6 (Text 48, Leon bleibt auf y -5400). N4 —
  Ereignis 26 waehlt `2030_ventile` nur bei Bit 102 UND (3,108), sonst `2030_ventile_vorher` (Text 49), und der Platz Slot 14
  liegt Band 1 an der Suedwand (vorher Band 3 am Ring) -> erklaert `ventile_ohne_102`; gemessen L2 (49 vor der Sicherung, 45
  danach). Rechner 2080: `install_2080` und die Ereignis-Abzweigung fuer 2080 sind entfernt (kein 2080-Zweig mehr in
  `re15_ventilkette_r41_ereignis`), Text 2080/40 nicht mehr in der Tabelle -> L6 `room=2080 id=0` Original, kein Port-Programm.

### G2 Vertrag (VERTRAG.md §1/§2)
* Pfade (`git diff master...HEAD --name-only` ohne `analysis/`): engine/src/{ventilkette_r41.c, gen/ventilkette_r41_*.inc (4),
  game_step_common.c, scd_room_setup.c, scd_vm.c}, include/re15_ventilkette_r41.h, platform/pc/main.c, tests/integration/
  {test_r41_ventilkette*.cmake (5), test_r36_dachkiste.cmake, test_r37_itemband.cmake}, tests/unit/{probes/r41_ventilkette.cmake,
  test_r41_ventilkette.c, test_r36_kartenbits.c}, tools/r41_ventilkette/ (3). Suche nach `release/`, `platform/android/`,
  `shared_assets/PSX/`, `pri/`, `synchro/`, `CMakeLists.txt`: **0 Treffer**.
* Gemeinsame Dateien = kleine markierte Haken ("Runde 41 Spur C"): game_step_common.c +1 (Takt), scd_room_setup.c +2 (include +
  Installer am Ende des Blocks), scd_vm.c +4 (include, Ereignis-Kette, Sce_espr_on-Haken), main.c +11 an 4 Stellen (include,
  Wasserflaechen-Lader, CONTINUE-Install, Pult-Zeichner) — angenommen durch C-E3; test_r36_dachkiste.cmake +2 / test_r37_
  itemband.cmake +3/-1 (Querbezugs-Flags 9:100 bzw. 9:101,9:102, C-E3).
* Bank-9-Bits: gesetzt werden nur 100/103 (ventilkette_r41.c `bit_setzen`), 101/102 (Programme `22 09 65 01`/`22 09 66 01`), 106
  (`panel_tick`) = Zuteilung C (100-103, 106; 107 seit Nachtrag 7 frei und nirgends gesetzt — Unit G prueft `bit(107) == 0`).
  k_port[] in test_r36_kartenbits.c um genau diese 5 Zeilen ergaenzt; `unit_r36_kartenbits` gruen (Unit-Gruppe §3).
* Text-Ids (gen/ventilkette_r41_texte.inc Tabelle): 2000/40, 2030/40-49, 2050/40,43,44 (+41/42 = Original-Bytes ROOM11F0 msg 1/2),
  20B0/41 (+ Leitersatz 40 ueber die Gruppe 2000), 2070/40; **keine 2080-Ids** mehr (VERTRAG 1.2: entfallen). Alle innerhalb der
  Zuteilung.
* Plaetze/Ereignisse: 2030 Slots 12/13/14 (frei laut Kopf-Zensus) mit Port-Ereignissen 24/25/26 (> sub06); 2050 Slot 8 + obj 8 =
  die Original-Saetze @0x01D38/@0x01D4C; umgetypt nur 2000 Slot 4, 20B0 Slot 3, 2070 Slots 1/2, 2030 Slot 8 und die Rufzonen 4/6
  (nur GEFLUTET). Bank 5 0x1D/0x1E nur in 2030/2031 (M3, Unit H).
* Riegel je Punkt (probes/r41_ventilkette.cmake): unit_r41_ventilkette A-L (376 Pruefungen, Leon + Elza); integration
  `_` (Laeufe Ventile/Flucht/Leiter/Tod mit Blendenfarbe), `_b` (Pult 2050, Hebel, Leitern nach der Flut, Elza 2001/2051),
  `_weg`/`_weg_elza` (v1/v2/g1/g2/w1/w3/w4 zu Fuss), `_sperre` (11A0-Sperre, Nachtrag 6), `_kette` (nur mit Spur A, C-E1).
  Jeder Lauf mit eigener exe-Kopie (Praefixe `re15_pc_r41c_haken_`, `_r41cb_`, `_r41ck_`, `_r41cs_`, `_r41cw<figur>_`, alte
  Kopien vorher geloescht), eigenem Arbeitsordner, `cmake_minimum_required(VERSION 3.21)`, `.exe` nur per `MATCHES`,
  `RUN_SERIAL`. Messend: Log-Saetze woertlich (`DOOR FIRE` mit Rechteck/Ziel, `[msgtext]`), Wegplan-FLAG-Folgen, Zellen.
* Hinweis (Doku, kein Verhalten): Kopfkommentar von `tests/unit/probes/r41_ventilkette.cmake` Z. 2 ("Rechner 2080 entleert
  2030") und Z. 30 ("2030 mit Bit 107 ohne Wasser (Gegenprobe ohne 107)") beschreiben noch den Stand vor Nachtrag 7; der Code
  der Riegel prueft den neuen Stand (`_b` I2/L2/M2 mit (3,126), Unit G `bit(107) == 0`).

## 1. Messungen an der exe (eigene Laeufe)
Runner `<a3>/lauf.sh` (eigene exe-Kopie `re15_pc_abn3c_<lauf>.exe` je Lauf, danach geloescht; erbt keine RE15_-Variablen;
Arbeitsordner `<a3>/mess/<lauf>/` mit debug.log, state.txt, wegplan.txt, bilder/), Serien `<a3>/serie1.sh` .. `serie4.sh`, eigene
Wegplaene `<a3>/plaene/*.txt` (Wegplan-Treiber RE15_WEGPLAN: echte Pad-Bits aus Spieler-/Ziellage, Aktionstaste, kein
RE15_FIRE_AOT). Renderer: beschleunigt (`SDL-Renderer direct3d`), RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP. Leon ueber den Titel-
Haken, Elza mit `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1` (debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`).
Spinnen in 2030 als getoetet (Kill-Bits 7:155..158), sonst kein Haken im Ablauf.
Messfehler, die ich selbst gemacht und verworfen habe (kein Befund gegen den Bau): Serie 1 `kette` lief mit einem Wegpunkt
quer gegen die gesperrte Steg-Zelle und rutschte endlos an z -6418 entlang (state.txt F4221..4223 `PL(-923..-929,-6418)`
pad=0010 — zugleich ein Beleg, dass der Steg mit der Bruecke oben nicht begehbar ist); Serie 2 `kette` wurde vom Treiber
nach 96 Befehlen abgeschnitten (`WP_MAX 96`, dachkiste_r36.c:165) — Teil 2 als eigener Lauf (`kette_b`, Serie 4). Serie 1/2
`leiter`/`leiter2` standen 12..30 Einheiten zu weit oestlich (Vorwaertspunkt ausserhalb des Leiter-Rechtecks x -27600..-26600)
-> `leiter3` (Serie 4).

### L2 — Punkt 2 + Nachtraege 9/10, ROOM2030 vor der Flut zu Fuss (Leon `kette`, Elza `kette_elza`)
Aufbau: `RE15_DEBUG_JUMP=2030@250`, Start an der Ost-Tuer `RE15_PLAYER_POS=6550,-100,2048,3` (Ring), keine Kettenbits vorgesetzt
(Wegplan F61-F64 `FLAG (9,101) = 0`, `FLAG (9,102) = 0`, `FLAG (3,108) = 0`, `FLAG (9,103) = 0`), Plan `<a3>/plaene/kette.txt`
(Ring Ost -> Nord -> Pult -> NW-Balkon -> Treppe -> Nord-Landeplatz -> Tank -> zurueck -> Pult -> Treppe -> Steg -> Sued-Landeplatz).
* AOTS F65: `slot 12 typ 0 ev 24 band 3 mitte (350,-10350)` (Pult), `slot 13 typ 0 ev 25 band 1 mitte (-2400,-6400)` (Ventil 13),
  `slot 14 typ 0 ev 26 band 1 mitte (-900,7900) halb (2500,500)` (Ventile, Sued-Landeplatz).
* Leon debug.log (Reihenfolge woertlich): `ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt` -> `Ereignis 24 -> Port-Programm
  2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the bridge. The pressure gauge reads zero.Intake valve 13 is
  closed."` -> (Treppe hinab, Wegplan F1284 `ZELLE r0[12] LAUFZEIT: x -2050..-50 z -5950..5450 typ 1 u0 0xff`) -> `Ereignis 25 ->
  2030_v13_frage` -> `id=41 "Intake valve 13. Will you open it?"` -> `id=42 "You've opened valve 13. The pressure is rising."`
  (F1740 `FLAG (9,101) = 1`) -> Treppe hinauf -> `Ereignis 24 -> 2030_pult_senken` -> `id=43 "The pressure gauge reads 80. Lower the
  bridge?"` -> `[scd F2496] Cut_chg(4)` -> `[prop-member F2556] prop[0] obj=0x00 m=1 → val=-1800` -> `[scd F2590] letterbox
  closed` (F2830 `FLAG (9,102) = 1`) -> `Ereignis 24 -> 2030_pult_unten` -> `id=44 "The bridge is down."` -> Treppe hinab, F3549
  `ZELLE r0[12] ... u0 0x00` -> ueber den Steg (F3776 bei (-1140,5956)) -> Sued-Landeplatz.
* Kamera (state.txt): Text 40 in cam 1 (F388, Leon bei (432,-9650) vor dem Pult), F751 cam 2 (Nordwand, Pult links im Bild) —
  Bildbogen `<a3>/kette_leon_teil1_a.png`: F370/F400/F520 Leon am Ring vor dem Schaltschrank (Manometer, blauer Kasten) mit
  Text 40, F760 Cut 2 = Pult links (Nutzerbild schaltschrank.png), F1000 Treppe, F1390/F1540 Ventil 13 am Tank (Text 41/42),
  F2350 Frage 43, F2500/F2530/F2560 Draufsicht Cut 4 mit der sinkenden Bruecke, F2860 Text 44.
* Elza (`kette_elza`): `Spieler-Familie PL04` -> `loaded room2031.rdt` -> dieselben Saetze mit `room=2031` (40, 41, 42, 43,
  `[scd F2490] Cut_chg(4)`, `[prop-member F2550] prop[0] obj=0x00 m=1 → val=-1800`, 44), ZELLE F1285 u0 0xff / F3544 u0 0x00,
  `FLAG (9,101)` 0 -> 1 (F1735), `FLAG (9,102)` 0 -> 1 (F2824).
* **Nachtraege 9/10 im selben Lauf** (Sued-Landeplatz, Bruecke unten, Sicherung noch nicht genommen, F4171 `FLAG (3,108) = 0`):
  `Ereignis 26 -> Port-Programm 2030_ventile_vorher` -> `[msgtext] room=2030 id=49 "Numbered valves. If I turn them, this whole
  level will be flooded.I should look around here first."` (kein Zaehler) -> Sicherung am Westende (Original Slot 9):
  `[msgtext] room=2030 id=2 "Will you take the Fuse?"` -> `id=3 "You've taken the Fuse."`, F4641 `FLAG (3,108) = 1` -> erst
  jetzt `Ereignis 26 -> Port-Programm 2030_ventile` -> `id=45 "Numbered valves. They control the flood gates of the treatment
  pool.Will you turn them?"`. Elza identisch (`room=2031`, F4175 `(3,108) = 0`, F4645 `(3,108) = 1`).
  Bildbogen `<a3>/kette_leon_teil1_b.png`: F3610 Cut 4 auf dem Steg, F3760 Cut 5 Leon vor dem Schild 09 (rotes und gruenes
  Ventilrad), F3850/F4030 Text 49, F4300/F4450 Sicherung (Frage, genommen), F4780/F4900 Frage 45.

### L3 — Punkt 3: Zaehler, Flut, Tod mit Ertrink-Effekt (Leon `flut`; Bildbogen `<a3>/flut_sheet.png`)
Aufbau: Bits 101/102/(3,108) gesetzt (Bruecke unten, Sicherung genommen — Vorbedingung nach Nachtraegen 9/10), Spinnen-Kill-Bits,
Start vor den Ventilen am Sued-Landeplatz `RE15_PLAYER_POS=-600,7150,3072,1`, Plan `flut.txt` (Aktion, Ja, Warnung zu, stehen).
* debug.log: `Ereignis 26 -> Port-Programm 2030_ventile` -> `id=45 "Numbered valves. They control the flood gates of the treatment
  pool.Will you turn them?"` -> `id=46 "WARNING: The water will rise in one minute. Leave the room!"` -> `Tuer Slot 0 -> ROOM20B0
  Cut 11 (Zaehler laeuft)` / `Zaehler laeuft: 1800 Takte` (F537) -> `[scd F2337] Cut_chg(4)` / `FLUT: Wasser steigt` -> `SPRITZER:
  Wasser y -1800 erreicht die Fuesse (Spieler y -1800), Effekt 0x06050D00` (genau einmal) -> `[fade-log] config ch=0 abr=2 mask=6
  step=0 bucket=7`, `kick ch=0 value=0x5000` -> `ERTRUNKEN: Wasser y -2700, Spieler y -1800 -> hp -1, blaue Blende`.
* state.txt: Zaehler-Start F537 -> Flut F2337 = **1800 Bilder (60 s bei 30 Bildern/s)**; Spritzer F2372 (`fx=1`); Tod F2390
  (`hp=-1`, 53 Bilder nach Flutbeginn, Spieler unten auf y -1800). Danach Todesfolge des Ports (Weissblende, YOU DIED,
  Neustart ab ROOM1240 — Port-Ablauf, nicht Gegenstand der Spur).
* Bild (`<a3>/flut_sheet.png`): F2340-F2380 Draufsicht Cut 4, Wasser steigt, die gesenkte Bruecke taucht ein; F2390-F2420
  blauer Schleier (Blende Kanal 0); F2450 Weissblende; F2500 YOU DIED.
* Flucht statt Tod: `kette_b` (§ L3b, Serie 4) — Ventile -> Warnung -> zu Fuss zur Ost-Tuer -> 20B0.

### L6 — Nachtraege 7/8 (Strom -> Relinghoehe, Ring offen, Rechner Original), Leon `strom`, Elza `strom_elza`
Aufbau: Zustand "Flut gelaufen + Strom": `RE15_SET_FLAG=3:235,9:103,9:100,9:101,9:102,3:108,9:106,3:109,3:114,3:113,3:219,7:155..158,
3:144,3:126`, Start in 20B0 vor der Ost-Tuer `RE15_PLAYER_POS=4950,-6400,0,1`, Plan `strom.txt` (Ost-Tuer -> Ring Ost -> Nord ->
NW-Balkon, Aktion an der Treppe -> Ring Ost -> Sued -> SW -> West-Gang -> Lift B -> Suedgalerie -> 2080 -> Treppe -> Rechner -> 2090),
alles zu Fuss mit der Aktionstaste.
* Leon debug.log: `loaded room20b0.rdt` -> (kein "Ring-Tuer ... gesperrt") -> `[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,
  hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` -> **`ROOM2030: Strom (3,126) -> Wasser auf Relinghoehe
  y -3600 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48, Ring frei`** -> `[prop-render] pi=4 oid=0x04 pos=(0,-3600,0)` (kein
  obj 5, kein "unter Wasser (y -7200)") -> Aktion am NW-Balkon Richtung Treppe: **`[msgtext] room=2030 id=48 "I cannot go down
  the stairs because of the water."`**, `[walk] F660/F690/F720 ... pl pos=(-6605,-5400,-7722)` = Leon bleibt auf dem Ring ->
  Lift B: Wegplan F1665 `FLAG (3,220) = 0` -> `[scd F1847] Cut_chg(9)` / `WRITE Set zone=3 idx=220 op=1` -> F2249 `FLAG (3,220) = 1`
  -> `[aot] DOOR FIRE slot=1 rect=(-7700,6400,hw=500,hh=900) target_cut=0 spawn=(-23200,-9000,-16700)` -> `loaded room2080.rdt`
  -> **`[msgtext] room=2080 id=0 "From here maybe I can control the water level.Will you operate the computer?"`** -> `WRITE Set
  zone=3 idx=245 op=1` -> `[scd F883] Cut_chg(7)` -> `[scd F1199] Cut_chg(3)` (Wegplan F64 `FLAG (3,245) = 0`, F1649 `= 1`) ->
  `[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500) target_cut=0 spawn=(-8200,0,-25550)` -> `loaded room2090.rdt`.
  Kein `Port-Programm 2080`, kein `room=2080 id=40`, kein Set auf Bank 9 Bit 107.
* Bild (`<a3>/strom_2030_leon.png`, Bilder des 2030-Abschnitts vor dem Ueberschreiben gesichert): Ring trocken (Leon geht auf
  dem Ring, kein Wasser ueber dem Ringboden), die Wasserflaeche fuellt nur den Schacht unterhalb der Ring-Reling — cam 1 (F270/
  F910): unten im Bild Wasser, Schilder 04/02 darueber; cam 2 (F410/F570/F650): obere Treppenstufen und Tank 13 ragen heraus,
  Text 48 an der Treppe; cam 0 (F1110) Ost-Tuer ohne Wasser im Bild; cam 7 (F1250/F1350): Wasser vor den Schildern 09/08/07/06,
  die untere Reling ist untergetaucht; cam 8 (F1450/F1590) West-Gang, Lift B; F1750 Fahrt; F1870 cam 9 Suedgalerie.
  2080 (`<a3>/strom_2080_leon.png`): Frage msg 0 am Rechner, Cut 7 der Pegel in 2080 sinkt (Wand/Gitter tauchen auf), Cut 3.
* Elza (`strom_elza`): `Spieler-Familie PL04` -> `loaded room20b1.rdt` -> `DOOR FIRE slot=3 ...` -> `loaded room2031.rdt` ->
  `ROOM2031: Strom (3,126) -> Wasser auf Relinghoehe y -3600 ...` -> `[msgtext] room=2031 id=48 "I cannot go down the stairs
  because of the water."` -> `[scd F1847] Cut_chg(9)` + `room=2031 WRITE Set zone=3 idx=220 op=1` -> `DOOR FIRE slot=1 rect=(-7700,
  6400,...)` -> `loaded room2081.rdt` -> `[msgtext] room=2081 id=0 "From here maybe I can control the water level..."` ->
  `room=2081 WRITE Set zone=3 idx=245 op=1` -> `Cut_chg(7)` -> `Cut_chg(3)`.

### L1 — Punkt 1: Leiter 2000 vor der Flut (Leon `leiter3`, Elza `leiter3_elza`; Rohdaten der ersten Sitzung, Serie 4)
Aufbau: `RE15_DEBUG_JUMP=2000@250`, `RE15_PLAYER_POS=-25500,10300,2048,1`, keine Kettenbits (Wegplan F61 `FLAG (9,100) = 0`),
Plan `leiter3.txt` (zu Fuss an die Leiter (-26150,10300) Blick 2048, dreimal Aktion).
* Leon debug.log: `[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40 (Form ROOM2030 @0x01B12)` -> Aktion F104
  -> **`[msgtext] room=2000 id=40 "I cannot climb up the ladder because of the water pressure."`** -> Aktion F257 schliesst ->
  Aktion F350 -> derselbe Satz erneut; kein `DOOR FIRE`, kein `loaded room2050` bis `EXIT_AT` F1000; Wegplan F413 `FLAG (9,100)
  = 0`. state.txt: Strahlen-Effekte aktiv in allen Spielbildern (`fx=21` 338 Bilder, `fx=23` 662 Bilder).
* Elza: `Spieler-Familie PL04` -> `loaded room2001.rdt` -> `ROOM2001: Leiter Slot 4 ... -> Sperrtext 40` -> `[msgtext] room=2001
  id=40 "I cannot climb up the ladder because of the water pressure."` (2x), kein Raumwechsel, `fx=21/23` (338/762 Bilder).
* Bild: der Lauf stand in Cut 0 (Leiter links im Bild, Satz zweizeilig, `<a3>/leiter3_leon.png`, `<a3>/leiter3_f600_x2.png`);
  den Leiter-Schnitt mit dem Strahl misst L1b (unten).

### L1b — Punkt 1/4 im Bild: Leiter-Schnitt vor und nach der Flut (Leon `leiter4`, `leiter4_nach`; Serie 9)
`RE15_FORCE_CUT` nur zur Bildwahl (2000 Cut 10 = 20B0 Cut 9, Kameratabellen-Zwilling laut Kopf), sonst wie L1; nach der Flut
`RE15_SET_FLAG=3:235,9:103,9:100`, Sprung nach 20B0 an dieselbe Stelle (-25500,10300). Bildbogen **`<a3>/leiter_vorher_nachher.png`**.
* 2000: `ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40` -> 2x `[msgtext] room=2000 id=40 "I cannot climb up the ladder because
  of the water pressure."`, kein Raumwechsel; `fx=21` 338 / `fx=23` 262 Bilder. Bild: der Wasserstrahl faellt aus dem Gitterfenster
  neben der Leiter, Leon steht an der Leiter, der Satz zweizeilig.
* 20B0 (nach der Flut): `fx=0` in allen 551 Zeilen, kein Satz; Bild: dasselbe Gitterfenster ohne Strahl (nur die gemalten Spuren
  des 20B0-Hintergrunds), Leon an der Leiter. Die Leiter selbst ist danach eine Tuer nach 2050 (L4, L7).

### L3b — Punkt 3 Flucht: Sicherung -> Ventile -> Warnung -> zu Fuss zur Ost-Tuer -> 20B0 (Leon `kette_b`, Elza `kette_b_elza`)
Aufbau: Bruecke unten vorgesetzt (`RE15_SET_FLAG=7:155..158,9:101,9:102`; die Bruecke selbst misst L2), Start am Suedende des
Stegs `RE15_PLAYER_POS=-1100,5900,3072,1`, Plan `kette_b.txt` (Sicherung am Westende, Ventile, dann gehend Steg -> Nord-
Landeplatz -> Treppe -> Ring Nord -> Ring Ost -> Ost-Tuer).
* Leon debug.log (woertlich, Reihenfolge): `id=2 "Will you take the Fuse?"` -> `id=3 "You've taken the Fuse."` (Wegplan F61
  `FLAG (3,108) = 0`, F552 `= 1`) -> `ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile` -> `id=45 "Numbered valves. They control
  the flood gates of the treatment pool.Will you turn them?"` -> `id=46 "WARNING: The water will rise in one minute. Leave the
  room!"` -> `[flagcensus] room=2030 WRITE Set zone=5 idx=29 op=1` -> `ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)`
  / `Zaehler laeuft: 1800 Takte` -> `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11 spawn=(5950,-1800,-6400)`
  -> `loaded room20b0.rdt` -> `[scd F2191] Cut_chg(11)` -> **`Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100`** ->
  `ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet)`. Kein `FLUT`, kein `ERTRUNKEN`.
* Zeit: Warnung 46 zu F1122 (state.txt `msg(a=0 ... id=46)`; Set(5,0x1D) im selben Bild) -> Tuer-Aktion F2191 = **1069 Bilder
  (35,6 s) gehend**, Rest 731 Bilder auf die 1800.
* Elza: `PL04`, `room=2031` dieselben Saetze (2, 3, 45, 46), `ROOM2031: Tuer Slot 0 -> ROOM20B0 Cut 11`, `DOOR FIRE slot=0
  rect=(7350,-200,...) target_cut=11` -> **`loaded room20b1.rdt`** -> `Bits 103 + 100`, ebenfalls ohne Flut.
* Bild `<a3>/kette_b_leon.png`: die Kamera blieb in diesem Lauf auf Cut 0 (Start per PLAYER_POS mitten im Raum; die Kamera-Zonen
  schalten erst beim Ueberqueren ihrer Grenzen aus dem passenden Cut) — Satz 46 zweizeilig, F2170 Leon an der Ost-Tuer. Die
  Kameras auf dem Weg zeigt der Bildbogen der Nachbesserung (`C_belege/nb3_ventile_zu_fuss_leon_sheet.png`); fuer das Urteil ohne
  Belang (gemessen sind Weg, Zeit und Raumwechsel).
* Die 20B0-Auswasch-Szene startet im Spurbaum nicht (AUTO-Haken der Spur A fehlt, Entscheid C-E1); `_kette` ist vorhanden und
  sauber uebersprungen (Configure Z. 180), siehe G2.

### L5 — Punkt 5 vor dem Pult: Hebel gesperrt (Leon `hebel_vorher`, Rohdaten der ersten Sitzung, Serie 2)
Aufbau: `RE15_DEBUG_JUMP=2050@250`, `RE15_PLAYER_POS=4900,-7100,1024,3`, Plan `hebel_vorher.txt` (GEGNERWEG, zu Fuss
(1900,-7300) -> (1900,-10300) -> (4300,-11150) Blick 0, Aktion).
* debug.log: `ROOM2050: Pult Slot 8 (@0x01D38) angelegt, Ereignis 9` / `Cursor obj 8 (@0x01D4C) angelegt` -> Aktion am Hebel ->
  **`ROOM2050 Ereignis 4 -> Port-Programm 2050_hebel_sperre`** -> **`[msgtext] room=2050 id=40 "The pump has no power. The
  switchboard must be set first."`**; kein `Cut_chg(4)`, Wegplan F63 `FLAG (3,104) = 0` -> F532 `FLAG (3,104) = 0`, F533
  `FLAG (9,106) = 0` (das Wasser steigt nicht).
* Messfehler der ersten Sitzung (verworfen, kein Befund): `pult2050` (Eingabeskript ohne GEGNERWEG) — der Zombie in 2050 toetete
  Leon vor dem Pult (`mo=7`, danach Neustart ab ROOM1240), rc=124. Neu gemessen in L5b.

### L5b — Punkt 5: Pult loesen (Leon `pult5`, Elza `pult5_elza`; neu in der Fortsetzung, Serie 8 `<a3>/serie8.sh`)
Aufbau: `RE15_DEBUG_JUMP=2050@250`, Wegplan `pult5.txt` (GEGNERWEG, zu Fuss vor das Pult (4900,-7150) Blick 1024, ENDE),
danach Eingabeskript ab Spielbild 300 (`RE15_INPUT_SCRIPT_BASIS=spiel`): Aktion, Aktion, Ja, Steuerkreuz rechts 0,25 s / Viereck /
links 1,2 s / Viereck / rechts 2,0 s / Viereck, warten, Aktion — dieselbe Folge wie Abnahme 2 L5. Bilder `<a3>/pult5_leon.png`.
* Leon: Wegplan F62 `FLAG (9,106) = 0`, F63 `FLAG (3,104) = 0`, AOTS F64 `AOT slot 8 typ 0 ev 9 flags 0x31 band 3 mitte
  (4900,-8050) halb (900,400)` (= main00 @0x01D38 (4000,-8450,1800,800)). debug.log: `ROOM2050 Ereignis 9 -> Port-Programm
  2050_pult_ein` -> `[msgtext] room=2050 id=43 "A high voltage switchboard. The pump needs 80 volts."` -> `id=41 "Will you push
  them?"` -> `[scd F504] Cut_chg(15)` -> `Pult 2050: Hebel 3 -> EIN, Schalter 0x4, 40 V` -> `Hebel 1 -> EIN, Schalter 0x5, 70 V`
  -> `Hebel 4 -> EIN, Schalter 0xD, 80 V` -> (30 Ruhebilder) **`[msgtext] room=2050 id=42 "Power supply OK."`** / **`Pult 2050
  GELOEST: 80 V -> Bit 106`** -> `Pult 2050 verlassen (Schalter 0xD, 80 V)`; erneute Aktion F992 -> `2050_pult_zustand` -> `id=44
  "The switchboard is set to 80 volts. The pump is powered."`.
* Bild: Cut 15 = Original-Hintergrund des Schaltpults (DANGER High voltage, Skala mit roter 80), roter Zeiger F525 auf 0, F600 40,
  F690 70, F735 80 (im roten Feld), gruene Marken unter den EIN-Hebeln, F765 "Power supply OK.", F1035 Raumbild mit Satz 44.

### L5c — Punkt 5: Hebel nach dem Pult = Original (Leon `hebel5`, Elza `hebel5_elza`; Serie 8)
Aufbau: wie `hebel_vorher`, aber `RE15_SET_FLAG=9:106` (= das Bit, das L5b am Spiel setzt), Plan `hebel5.txt` (GEGNERWEG, zu Fuss
an den Hebel, Aktion, Ja).
* Leon: Wegplan F62 `FLAG (9,106) = 1`, F63 `FLAG (3,104) = 0` -> Aktion F286 -> KEIN Port-Programm -> Original sub04 `[msgtext]
  room=2050 id=0 "A lever to fill and drain water. Will you move it?"` -> Ja F439 -> `[flagcensus] room=2050 WRITE Set zone=3
  idx=104 op=1` -> `[prop-member F443] prop[5] obj=0x04 m=1 -> val=-1400` -> `[scd F463] Cut_chg(4)` -> `[prop-member F643]
  prop[6] obj=0x05 m=1 -> val=-5000` -> `Cut_chg(7)`; Wegplan F1257 **`FLAG (3,104) = 1`** (das Wasser ist gestiegen). Bilder
  `<a3>/hebel5_leon.png` (Frage, Cut 4 Becken, Cut 7 Leon am Hebel).
* Elza `pult5_elza`: `Spieler-Familie PL04` -> `loaded room2051.rdt` -> `ROOM2051: Pult Slot 8 ... angelegt` -> `ROOM2051 Ereignis
  9 -> Port-Programm 2050_pult_ein` -> `id=43` -> `id=41` -> `Cut_chg(15)` -> 40 / 70 / 80 V -> `[msgtext] room=2051 id=42 "Power
  supply OK."` / `GELOEST: 80 V -> Bit 106` -> `verlassen` -> `id=44`. `hebel5_elza`: `FLAG (9,106) = 1` -> `[msgtext] room=2051
  id=0 "A lever to fill and drain water. Will you move it?"` -> `WRITE Set zone=3 idx=104 op=1` -> `Cut_chg(4)` -> `Cut_chg(7)`.

### L4 — Punkt 4: nach der Flut 20B0 statt 2000, Leiter frei (Leon `nachflut4`, Elza `nachflut4_elza`; Serie 8)
Aufbau: Zustand "Szene gelaufen" `RE15_SET_FLAG=3:235,9:103,9:100` (die Szene selbst gehoert Spur A, C-E1), Sprung nach 2010,
danach alles zu Fuss mit der Aktionstaste (Plan `nachflut4.txt`: 2010-Tuer nach "2000" -> Leiter -> 2050 -> 2050-Leiter).
* Leon: Wegplan F61/F62 `FLAG (9,103) = 1`, `FLAG (9,100) = 1` -> `ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` ->
  `DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=10 spawn=(-26450,-1800,-2700)` -> **`loaded room20b0.rdt`** (nicht
  2000) -> AOTS `slot 4 typ 1 ev 0 ... mitte (-27100,10300)` (Leiter = Tuer, kein Sperrtext) -> Aktion -> **`DOOR FIRE slot=4
  rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)`** -> `loaded room2050.rdt` -> `ROOM2050: Tuer Slot 0 ->
  ROOM20B0 Cut 9 (Ziel ROOM2000)` -> Aktion an der 2050-Leiter -> `DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000)
  target_cut=9 spawn=(-26450,-1800,10250)` -> `loaded room20b0.rdt` -> `EXIT_AT: Bild 700 in Raum 20b0`. state.txt **`fx=0` in
  allen 1047 Spielbildern** (in 2000 vor der Flut `fx=21/23`, L1) = kein Wasserstrahl mehr an der Leiter.
* Elza: `PL04` -> `loaded room2011.rdt` -> `ROOM2011: Tuer Slot 0 -> ROOM20B0 Cut 10` -> **`loaded room20b1.rdt`** -> `DOOR FIRE
  slot=4 ...` -> `loaded room2051.rdt` -> `ROOM2051: Tuer Slot 0 -> ROOM20B0 Cut 9` -> `DOOR FIRE slot=0 ... target_cut=9` ->
  `loaded room20b1.rdt`; `fx=0` in allen 812 Bildern.

### L8 — Weg nach der Flut, Glieder 2060 (Original-Raetsel) und 2060 -> 2030 Nordgalerie -> 2020 (Rohdaten der ersten Sitzung)
* `w1` (Leon, NEU in der Fortsetzung, Serie 9; Zustand "Flut gelaufen", Original-Raetsel ausserhalb der Kette (Kisten (3,109),
  Kurbel (3,219), Pult 106) gesetzt, OHNE Strom; Start an der 2050-Leiter-Ankunft (9100,4100), Plan `w1.txt` zu Fuss): Wegplan F62
  `FLAG (3,126) = 0`, `ZELLE r0[51] LAUFZEIT: x -7550..-6150 z -4500..850 typ 1 u0 0x00` -> `DOOR FIRE slot=1 rect=(-5150,4450,
  hw=1000,hh=500) target_cut=0 spawn=(11350,-1800,-3150)` -> `loaded room2020.rdt` -> `DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,
  hh=500) target_cut=13 spawn=(-450,-16200,-9700)` -> `loaded room2030.rdt` -> **`ROOM2030: unter Wasser (y -7200), Lift-Rufzonen
  4/6 -> msg 1`** (Nordgalerie auf der Tuerebene, ueber dem Wasser) -> zu Fuss zur 2060-Tuer -> `DOOR FIRE slot=3 rect=(-7700,
  -7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)` -> `loaded room2060.rdt`. Elza `w1_elza`: `PL04` -> `room2051` ->
  `room2021` -> `room2031` (`ROOM2031: unter Wasser (y -7200) ...`) -> `room2061`, dieselben Tuersaetze.
* `sicherung2060` (Leon, Zustand "Flut gelaufen", Start vor dem Sicherungskasten, Eingabeskript): `[msgtext] room=2060 id=3
  "One of the fuses is missing..."` -> `id=4 "Will you use the Fuse?"` -> `[scd F519] Cut_chg(8)` -> Cursor -> **`WRITE Set zone=3
  idx=144 op=1`** -> `Cut_chg(9)` -> `id=5 "You've used the Fuse."` (Original sub18/sub19 @0x0169E, kein Port-Programm).
* `generator` (Leon, (3,144) gesetzt): `id=0 "The generator does not appear to be on.There are four switches...Will you push
  them?"` -> `Cut_chg(7)` -> Schalter 1 (`zone=5 idx=6 op=1`), Schalter 3 (`zone=5 idx=8 op=1`) -> **`WRITE Set zone=3 idx=126
  op=1`** -> `Cut_chg(10)` -> `id=2 "It seems that the generator is working now."` (Original sub01 @0x012AE).
* `w3` (Leon, Flut + (3,144) + (3,126), Start in 2060, Plan `w3.txt` zu Fuss): `DOOR FIRE slot=0 rect=(8450,3950,hw=500,hh=1000)
  target_cut=13 spawn=(-6950,-16200,-7200)` -> `loaded room2030.rdt` -> **`ROOM2030: Strom (3,126) -> Wasser auf Relinghoehe y
  -3600 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48, Ring frei`** / `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der
  Flut)` -> Nordgalerie zu Fuss -> `DOOR FIRE slot=2 rect=(-450,-10400,hw=900,hh=500) target_cut=5 spawn=(-14200,-1800,2450)` ->
  `loaded room2020.rdt`. Elza `w3_elza`: `PL04`, `loaded room2061.rdt` -> `loaded room2031.rdt` -> `ROOM2031: Strom (3,126) ->
  ... Relinghoehe ...` -> `loaded room2021.rdt`.

### L9 — Ring-Zugaenge aus 2070 (Leon `r2070` ohne Strom, `r2070s` mit Strom; Zustand "Flut gelaufen")
* `r2070` (`FLAG (9,103) = 1`, `FLAG (3,126) = 0`): `ROOM2070: Tuer Slot 0/3 -> ROOM20B0 Cut 7 (Ziel ROOM2000)`, `Ring-Tuer Slot
  1/2 nach 2030 gesperrt (geflutet) -> Sperrtext 40`, AOTS `slot 1 typ 5 ev 40` / `slot 2 typ 5 ev 40` -> Aktion ->
  `[msgtext] room=2070 id=40 "I cannot open the door because of the water pressure."`, kein Raumwechsel.
* `r2070s` (`FLAG (3,126) = 1`): AOTS `slot 1 typ 1` / `slot 2 typ 1` (Tueren) -> Aktion -> `DOOR FIRE slot=1 rect=(5500,-6400,
  hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` -> `Strom (3,126) -> Wasser auf Relinghoehe`.
  => Ring-Sperre C-E2 nur "geflutet UND Strom aus" (Nachtrag 7), mit Strom offen.

### L6b — Nachtrag 8 im Bild: 2030 vorher / geflutet / Relinghoehe (Serie 9, je ein Lauf, `RE15_FORCE_CUT` nur zur Bildwahl, F120)
Drei Zustaende: vor der Flut (`9:101,9:102` + Spinnen), Flut ohne Strom (`3:235,9:103,9:100,...`), Flut + Strom (`... ,3:144,3:126`);
Start an der Ost-Tuer auf dem Ring. Bildbogen **`<a3>/cuts_vorher_nachher.png`** (Cut 4 / 1 / 7). Log je Lauf: geflutet `ROOM2030:
unter Wasser (y -7200) ...`, mit Strom `ROOM2030: Strom (3,126) -> Wasser auf Relinghoehe y -3600 ...`, vorher keiner der Saetze.
* Mit Strom: Cut 4 (Draufsicht) Becken, Steg und Landeplaetze unter der Flaeche, die Waende mit 05/01/02 darueber; Cut 1 Ring-Reling
  und Schilder 04/02 ueber dem Wasser, Tank und Schachtgrund darunter; Cut 7 Schilder 09/08/07/06 ueber dem Wasser, Ring trocken.
  Deckt sich mit dem Bildbogen der Nachbesserung (`C_belege/nb3_pegel_vorher_nachher_sheet.png`, exe 10:27) am heutigen Stand.
* Ohne Strom (nur der Vollstaendigkeit halber; mit diesem Zustand ist der Ring gesperrt und nicht betretbar, L7/L9): die Flaeche
  y -7200 liegt im Bild ueber der Reling; in Cut 1 bleibt der untere Bildrand (Tank, 04/02) unbedeckt — der Ausschnitt der
  Flaeche (x -10800..10800, z -12400..20000) reicht vor der Kamera nicht bis an den Bildrand. Im Spiel nicht sichtbar (Ring-Kameras
  sind in diesem Zustand nicht erreichbar), kein Befund.

### L7 — Nach der Flut OHNE Strom: Ring-Sperre und Weg zur Leiter (Leon `ohne_strom`)
Aufbau wie L6, aber ohne 3:144/3:126 (Wegplan F62 `FLAG (3,126) = 0`), Start vor der 20B0-Ost-Tuer, Plan `ohne_strom.txt`.
* AOTS F63: `slot 3 typ 5 ev 41 flags 0x31 band 1 mitte (5950,-6400)` (Tuer -> Text-Platz), `slot 4 typ 1` (Leiter = Tuer).
* debug.log: `ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet) -> Sperrtext 41` -> Aktion -> **`[msgtext] room=20b0 id=41
  "I cannot open the door because of the water pressure."`** (kein Raumwechsel) -> Mittelgang nach Westen, Sprung in den Kanal
  (Wegplan F774 Aktion, mo 230), Aufstieg (F935), Westgang -> Leiter: **`[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500)
  target_cut=0 spawn=(9100,-5400,4100)`** (kein Sperrtext) -> `loaded room2050.rdt` -> `ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9
  (Ziel ROOM2000)` -> Aktion an der 2050-Leiter -> `DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000) target_cut=9 spawn=(-26450,
  -1800,10250)` -> `loaded room20b0.rdt`.
* state.txt: `fx=0` in 3937 Zeilen, `fx=1` in 104 Zeilen (der Kanal-Sprung; keine 18 Strahlen-Effekte wie in 2000 vor der Flut,
  dort fx 21..33, L1) -> nach der Flut laeuft an der Leiter kein Wasser mehr herunter.
