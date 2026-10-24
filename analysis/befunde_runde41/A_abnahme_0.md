# Runde 41 — Spur A "wasser20b0": ABNAHME 0 (unabhaengig, 2026-10-23)

Baum `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, HEAD a46ea1ee (Basis master 116e8b0f).
Abnahme-Stand: fortlaufend geschrieben (nach jedem gemessenen Punkt gespeichert; Abschnitte zum Schluss numerisch geordnet). Der Bau-Agent wird nicht geglaubt,
nur das selbst Gemessene zaehlt. Massstab: AUFTRAG.md (Wortlaut + Nachtraege 1-3 + Entscheide E1/E2), VERTRAG.md §2.

## 0. Baustand (selbst gebaut)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
  `=== LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` mtime 2026-10-23 18:36:03 ist juenger als
  jede Quelldatei des Diffs (main.c 18:35:36, enemy_ai_common.c 18:05:56, wassersprung_r41.c 18:05:15, wasser20b0_r41.c
  17:35:41, aot_common.c 17:42:11, re15_actor.h 17:51:41) -> die exe ist der HEAD-Stand.
- Letzter Code-Commit ee935239 (18:36:19). Die danach liegenden Commits (e4612ae2 Video/Belege, 63dd4a87 Entscheide E1/E2,
  9726bd01/a46ea1ee Nachtrag 3) aendern KEINEN Code. => Nachtrag 3 (19:12) und Entscheid E1 (18:5x) sind im Code NICHT
  umgesetzt (siehe Punkt 2b).

## 1. Diff-Umfang (git diff master --name-only, ohne 605 Bild-/Belegdateien)
Code: aot_common.c (+3), climb_common.c (+6), enemy_ai_common.c (+8), scd_vm.c (+2), wasser_r38.c (+3), re15_actor.h
(RE15_ACTOR_MAX 21 -> 36), re15_wasser_r38.h (+2), platform/pc/main.c (+17: RE15_PLAYER_POS 4. Wert, Schatten-Bedingung
Typ 0x24), neu: wasser20b0_r41.c/.h, wassersprung_r41.c/.h; Tests: test_r41_wasser20b0.c (neu, 6 Teile),
probes/r41_wasser20b0.cmake (neu), integration/test_r41_wasser20b0.cmake (neu), test_r36_gegnerzahl.c/.cmake (angepasst).
Guess-Tell-Suche im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/getenv): nur Kontextzeilen des
Altbestands (aot_common.c Z.7 "deferred grant", scd_vm.c Z.106 "deferred until", main.c getenv RE15_PLAYER_POS) —
KEINE neuen Treffer.

## 2. Gezielte Tests (selbst gefahren)
- `ctest -R "^unit_r41_wasser20b0_"` -> `100% tests passed, 0 tests failed out of 6` (0.19 s).
- (weitere Laeufe: Abschnitte 7 und 9)

## 3. RE-Gate (selbst disassembliert, re15_disasm.py; STAGE2.BIN bzw. PSX.EXE)
Stichproben (alle zitierten Stellen enthalten das Behauptete):
- EM024 INIT Zeichenmaske: `lw a0,392(v0)` @0x8010efc8 (= *(+0x188)), `addiu v1,zero,-2` @0x8010efd4, `and v0,v0,v1`
  @0x8010efe0, `sw v0,0(a0)` @0x8010efe4 -> Bit 0 des Part-Worts geloescht. `ori v0,zero,0x1`/`sw v0,4(v1)` @0x8010eff0-f4
  (+0x4 = 1), `lbu v0,9(v1)`/`sb v0,5(v1)` @0x8010f004-0c (+0x5 := grid). BELEGT.
- EM024 ACTIVE: `jalr v0` @0x8010f0a8 ueber Tabelle 0x80118d58 (`addiu at,at,-29352` @0x8010f098 = 0x80118d58); +0x8c++
  @0x8010f0c4-c8; `subu v0,v0,a0`/`sw v0,52(v1)` @0x8010f0e4-e8 (x -= +0x8c); `slti v0,v0,-25000` @0x8010f100;
  `ori v0,zero,0x4e20` @0x8010f108; `ori v0,zero,0xe1` @0x8010f118. BELEGT.
- Tabellen @0x80118d50: [0]=0x8010ef1c [1]=0x8010f020; @0x80118d58: 0x8010f130, 0x8010f3fc, 0x8010f910, 0x8010fe24,
  0x80110338, 0x8011084c x3; @0x80118d88: 0x8010f178, 0x8010f2c0; @0x80118d90: 0x8010f444, 0x8010f64c, 0x8010f6b0,
  0x8010f8b8. BELEGT (table-Dump).
- grid 0 Phase 0 @0x8010f178: `lbu v0,464(v0)`/`bne` @0x8010f188-90 (Takt 0), Spawns `addiu a0,zero,-1000`/`ori a1,zero,0xa8c`
  /`jal 0x801108cc`/`addiu a2,zero,-1200` @0x8010f194-a0 dann `ori a0,zero,0x4000`/`addu a1,zero,zero`/`jal 0x80110988`/
  `ori a2,zero,0x7` @0x8010f1a4-b0; zweiter Satz (-2000,0xa8c,0x4b0)/0x4000; dritter (-1000,0xa8c,0)/0x3000. Takt-Ende
  `addiu v1,v1,1`/`sb v1,464(v0)` @0x8010f268-6c, `sltiu v0,v0,0xb` @0x8010f284, `sb zero,464` @0x8010f290, +0x6++ @0x8010f2ac.
  BELEGT = Port-Tabelle s_g0_p0[0..2] und RE15_WASSER20B0_R41_TAKTE_KURZ.
- Helfer 0x80110988: `andi a2,a2,0xff`/`sll a2,a2,16` @0x8011098c-90, `andi a0,a0,0xffff` @0x80110994, a1 s16 @0x80110998-9c,
  `lw v1,392(v0)` @0x801109b4, `lui v0,0xf00` @0x801109b8, `or a0,a0,v0`/`or a0,a2,a0` @0x801109bc-c0, a3 = 0x8011931c
  @0x801109a8-ac, `jal 0x80019700` @0x801109cc mit `addiu a2,v1,64` (= *(+0x188)+0x40). BELEGT.
- Landung PSX.EXE: `lh v0,442(v1)`/`sw v0,56(v1)` @0x80038724-2c; `lh v1,-13604(v1)` @0x8003873c (0x800acadc = Spieler
  0x800aca54+0x88); `lw a0,-13684(a0)` @0x80038744 (0x800aca8c = +0x38); `slt v1,v1,a0` @0x8003874c; `beq v1,zero,0x800387d4`
  @0x80038758; `ori s1,zero,0x4` @0x8003875c; `lui a0,0x603`/`ori a0,a0,0x3000` @0x8003876c-70; a3 = 0x8007428c (`read` =
  0,0,0,0); `addiu s0,s0,-2`/`sll s0,s0,8` @0x80038788-90; `jal 0x80019700` @0x80038794 (a1 = +0x6a + s0); zweiter
  `jal 0x80019700` @0x800387bc mit `addiu a1,a1,2048` @0x800387c0; Schleife `bne ... 0x8003876c`/`addiu s1,s1,-1`
  @0x800387cc-d0. BELEGT (8 Spawns, Gier-Faecher, Gate Stempel < y).
- FUN_80045630: `lbu v0,8(v1)`/`sltiu v0,v0,0x40` @0x80045728-30 (Typ >= 0x40), NPC-Zweig `j 0x80045794`/`addiu s3,a0,-8`
  @0x80045758-5c (ueberspringt das Objekt-Override @0x80045770-90). BELEGT.
- NPC-Geh-Sub: `lw v0,360(v0)`/`lw v1,0(v0)`/`andi v0,v1,0x4000` @0x80051278-88, `addu a0,zero,zero` @0x80051290,
  Fuss 7 - 3*((Wort>>12)&1) @0x80051294-ac, `jal 0x80045630` @0x800512a8; Sub 5 `ori a0,zero,0x1` @0x800515fc,
  `jal 0x80045630` @0x80051614. BELEGT.
- AUTO-Pass: Hauptschleife `jal 0x8001a50c` @0x8001ce04, `jal 0x80031c44` @0x8001ce0c, `jal 0x8002bd44` @0x8001ce14,
  `jal 0x800436a8` @0x8001ce1c — linear, keine Verzweigung dazwischen. FUN_800436a8 selbst: Spieler-Pool
  `ori a1,zero,0x1`/`addu a2,zero,zero`/`jal 0x80042bac` @0x800436bc-cc OHNE Gate auf Spielermodus/Pad-Sperre; Gegner-
  Schleife `lbu v0,-13746(v0)` (g_active_count 0x800aca4e) @0x800436d8 und `sltu v0,s0,v0` @0x80043740 = ALLE aktiven
  Entities (kein 20er-Deckel). BELEGT.
- Aktor-Plaetze (Entscheid E2, Widerspruch zu Runde 36 "20 Plaetze, FUN_80037250 sltiu 0x14" — ERKLAERT):
  (a) Sce_em_set 0x800420a0: Record-Lage s0 = (pc[1]&0x7f)*500 + 0x800acc2c (`andi s4,v0,0x7f` @0x800420f8, `addiu a0,a2,1200`
      @0x800420dc, `addu s0,v0,a0` @0x80042118) ohne Index-Pruefung; der Zaehler wird @0x80042214-3c gefuehrt:
      `lh v0,0(a1)` (Zaehler), `lh v1,4084(v1)` = DAT_800b0ff4 (Deckel), `slt v0,v0,v1` @0x80042224, sonst Wort0 = 0x8000
      (`ori v0,zero,0x8000`/`sw v0,0(s0)` @0x80042230-38, Satz inaktiv), sonst Zaehler+1 `sh v0,0(a1)` @0x8004223c.
      Der Deckel DAT_800b0ff4 wird EINMAL geschrieben: `ori v0,zero,0xff`/`sh v0,4084(at)` @0x8003ed58-60 (Skript-System-
      Init neben allocate_script_slot 0x8003ee3c) = 255. Savestate-RAM (eigene Lesung re15_ss.Ram): DAT_800b0ff4 = 255 in
      ts_10/flut_a/tf_7/tf_5/menu_20b.sav; DAT_800aca4e = 35 in ts_10/flut_a/tf_7/tf_5 (0 im Debug-Menue menu_20b).
      => Die Ablage fuehrt bis 255 Saetze; 35 EM024 sind im Original alle aktiv.
  (b) Entity-Takt FUN_8001a50c: `lbu s1,-13746(s1)` @0x8001a528 (g_active_count), Schleife `sltu v0,v0,s1` @0x8001a5b4 —
      ALLE 35 werden getaktet (kein 20er-Deckel).
  (c) FUN_80037250 (Decompile RE_15_Quellcode_V2/FUN_80037250.c): Rundlauf-Suche "naechste aktive Entity mit +0x9a >= 0 und
      (+0x9 & 0x60) == 0" ab dem Slot von DAT_800acbfc, Umbruch bei Slot 20 (`sltiu v0,v0,0x14` @0x800372b4 -> Slot 0). Der
      20er-Umbruch gehoert zu DIESER Zielwahl-Hilfe (Zielzeiger DAT_800acbfc), nicht zur Ablage, nicht zum Takt, nicht zum
      AOT-Pass. Die Runde-36-Folgerung "Slots 20..34 verworfen" war eine Fehldeutung dieser Schleife. RE15_ACTOR_MAX 36 ist
      damit belegt (Original: Deckel 255, 20B0 fuehrt 35).
- Original-RAM (eigene Lesung): EM024 Slots 0..34 +0x93=1, +0x94=0, +0x95=1 konstant (ts_10/tf_7/flut_a), Slot 8 (Satz 0x08)
  +0x5=1 (grid 1) +0x6=2 (Phase 2) +0x1d0=20 (ts_10) / 11 (tf_7) / 4 (flut_a); Drift ts_10 x=-17166/+0x8c=251 (alle 35),
  tf_7 x=264/168, flut_a x=-3716/313. Stimmt mit den Drift-Pins des Riegels unit_r41_wasser20b0_raum ueberein.
- Schatten: Die Behauptung "kein jal 0x8001b064 in 0x8010ee9c..0x80110a00" ist durch den Unit-Riegel nicht gedeckt; siehe
  Abschnitt 4 (eigene Suche).
Konstanten im Diff ohne @0x-Beleg: KEINE gefunden. Markierte PORT-WAHL: re15_wasser20b0_r41_auto_aot (Begrenzung der
Sperr-Aufhebung auf 20B0/20B1 — die Original-Mechanik kennt keine Sperre, die Begrenzung ist eine Port-Vorsicht, im Code und
Dossier als OFFEN benannt). RE15_PLAYER_POS 4. Wert = Messhaken (env-gegatet).

## 4. Schatten-Behauptung (eigener Scan)
STAGE2.BIN (103216 B, raw @0x80100000) nach dem Wort 0x0c006c19 (`jal 0x8001b064`) durchsucht: 8 Fundstellen 0x80100500,
0x8010a9a4, 0x8010c550, 0x80110b50, 0x80115870, 0x80115e40, 0x80116404, 0x801169cc — KEINE in 0x8010ee9c..0x80110a00.
STAGE1.BIN enthaelt u.a. 0x80116740 (Feuer-Emitter-Muster). Die Behauptung "EM024 ohne Figurenschatten" ist belegt; der
main.c-Haken `npc->type != 0x24u` ist damit gedeckt. Hinweis: Dass der Schatten ueberhaupt aus dem Typ-Code kommt (und
nicht aus dem Zeichner), folgt aus den 8 Aufrufstellen in Typ-Handlern; der Port zeichnet fuer alle anderen Typen
weiterhin pauschal — das ist Altbestand, nicht Teil dieser Spur.

## 5. Punkt 1 — 20B0-Wasserschwall: eigene Messung an der exe
### 5.1 Lauf A (Leon, Debug-Sprung 20B0, Bild-fuer-Bild-Dump)
Befehl (Messkopie re15_pc_abn0_a_leon20b0.exe, cwd = Arbeitsordner, env -i):
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=20b0@250
RE15_FX_LOG=fx.log RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1 RE15_SCHATTEN_LOG=1 RE15_FRAMEDUMP=0-300/1:f RE15_EXIT_AT=320#20b0`
debug.log: `PC loaded room20b0.rdt`, `[scd F250] Cut_chg(11)`, `[scd F105] Cut_chg(7)`, `[scd F146] Cut_chg(15)`,
`[scd F228] Cut_chg(6)`, `[scd F242] letterbox closed -> gameplay (player_mode=0)`, `EXIT_AT: Bild 320`. 35x `Sce_em_set
type=0x24`. fx.log: Partikel id=15 (Effekt 0x0f) sub 0/7 ab F7 (8 Plaetze), je Bild 6..28 gezeichnete Plaetze in F7..F105,
F139..F146, F303.. (Front ausser Sicht dazwischen). enemy_dbg: kein "ZEICHNE Typ 0x24". => (a) Partikel werden gespawnt und
gezeichnet, (b) sub05 feuert (Cut 7 -> 15 -> 6), (c) der Spieler wird frei (F242), (d) die 35 Emitter bleiben unsichtbar.
Vorher (Dossier 1.1, nicht selbst gemessen): kein Partikel, Szene haengt. Die Mechanik-Luecke ist geschlossen.
### 5.2 Integrationsriegel (selbst gefahren)
`ctest -R "^integration_r41_wasser20b0$"` -> `100% tests passed, 0 tests failed out of 1` (100.61 s); Lauf lauf_c53a64c4:
Leon 20B0 fx id=15 2408 Zeilen, Elza 20B1 (PL04, room20b1.rdt) 2408 Zeilen, je 35 Spawns, Cut 7 F105 / Cut 6 F228 / frei
F242 in beiden Szenarien.
### 5.3 Video-Vergleich Original/Port (eigene Zaehlung, nicht die TSV des Bau-Agenten)
Werkzeug scratchpad/abn0/zaehl.py (PIL/numpy): Partikelpixel = G >= 150, G > R+8, G > B+8, R > 60 (dieselbe Definition
wie im Dossier, unabhaengig implementiert) + Schnitt-Erkennung (mittl. |Differenz| zum Vorbild > 25).
Original = A_belege/orig_20b0/oNNN.png (aus dem DuckStation-Video, 30 fps), Port = mein Lauf A fNNNNNN.ppm.
- Schnitte: Original o24 (d=30), o58 (d=35), o121 (d=32), o169 (d=26); Port F22 (d=28), F51 (d=34), F106 (d=30).
  Ladestaus im Original-Video (identische Folgebilder): o17-o23 (7 Bilder, n=288/295 konstant), o52-o57 (6), o62-63, o160-168
  (9). Zieht man die Staus ab, liegen die Schnitte 2 und 3 auf +-2 Bilder (o24-7=17 vs F22; o58-13=45 vs F51 — der Rest
  bleibt eine Dehnung von ~5 Bildern, nicht als Port-Fehler belegbar).
- Schuebe (Bilder mit Partikeln): Original o5-9 (95/314/498/642/747 px), o25-26, o33-37 (168..417), o44-48 (108..463),
  o60-64, o72-75, o82-86, o93-97, o104-108 (187..652); Port F11-15 (33/87/115/145/125), F23-27, F33-37 (36..85), F44-48
  (31..83), F55-59, F67-70, F77-81, F88-92, F99-103 (44..108). Rhythmus ~10-11 Bilder in beiden Reihen; Ort gleich
  (Kontaktbilder scratchpad/abn0/kontakt_8_16.png, kontakt_36_48.png, kontakt_86_105.png: Wolke hinter/neben Leon).
- Dichte: Summe der Partikelpixel Bild 0..110 Original 13301 vs Port 2535 (Faktor 5,2), Maximum 747 vs 145, Bilder mit
  Partikeln 89 vs 69. Der erste Schub an der Osttuer (o5-9, 95..747 px) erscheint im Port erst F11-15 mit 33..145 px.
  Sichtpruefung der Kontaktbilder: o8 zeigt eine dichte Wolke links UND rechts der Tuer bis ueber Leons Schulter, p8 nur
  zwei kleine Flecken; o36/o48 vs p36/p48: gleiche Lage, Port-Wolke duenner und mit mehr Loechern.
  => Der Mangel des Dossiers (1.9) ist bestaetigt und bleibt offen (Orchestrator-Entscheid: MANGEL bis Bild-fuer-Bild gleich).
- Zusaetzlich gefunden (Letterbox, scratchpad/abn0: Balkenhoehe je Bild): Original o1..o100 OHNE Balken, o140..o250 Balken
  16/17 px; Port F5..F10 Balken 24/24 px, F11-12 24/0, F13..F100 keine, F140..F200 24/24, F240.. keine. Der Port zeigt also
  am Szenenanfang Balken, die das Original nicht zeigt, und die Balken sind 24 statt 16/17 px hoch. Nicht Gegenstand des
  Nutzer-Punkts, aber eine Bild-fuer-Bild-Abweichung dieser Szene (ohne @0x-Beleg fuer beide Seiten -> an den Orchestrator).

### 5.4 Reproduzierbarkeit
Die Port-Bildreihe des Bau-Agenten (A_belege/port_20b0/pNNN.png) und mein Lauf A (fNNNNNN.ppm, eigene Messkopie, env -i)
sind pixelgleich (Bilder 8/16/26/36/48/64/86/105/150/240: mittlere |Differenz| 0.00, 100 % gleiche Pixel) — der Port-Lauf ist
deterministisch, die TSV-Zahlen des Dossiers sind damit nachvollziehbar; meine Zaehlung (5.3) bestaetigt sie unabhaengig.

### 5.5 Zeitachse exakt (Nachtrag zu 5.3; exakt identische Folgebilder = max |Diff| = 0)
Original: identische Folgebilder bei o20-23, o53-57, o113-115, o117-120, o161-168, o187, o223, o251-254, o257-259 (33 Staus
des DuckStation-Videos). Schnitte 24/58/121/169 -> ohne Staus 20/49/105/145.
Port: keine Staus bis F191 (identische Bilder erst ab F192 in der Standphase). Schnitte 22/51/106; Cut_chg(15) bei F146
(debug.log; Bilddifferenz unter der Schwelle). => Schnittfolge Port 22/51/106/146 vs Original 20/49/105/145: +-2 Bilder.
Die Zeitachse der Szene (Cut-Folge 11 -> RVD -> 7 -> 15) stimmt damit innerhalb von 2 Bildern; der Unterschied bleibt die
DICHTE der Spritzerwolke (Faktor ~5 in Pixeln) und der fehlende erste Schub o5-9.

### 5.6 "Duenner" oder "dunkler"? (Nachtrag zu 5.3, Hinweis fuer die Nachbesserung)
Gruenliche Pixel (G > R+8, G > B+8) bei abgesenkter Schwelle G >= 100 (Hintergrund-Grundrauschen: Original 3..363 px in
Bildern ohne Wolke, Port 0..6): Original o8 3166, o16 2461, o36 2037, o48 1900, o86 1273, o105 2395; Port p8 9, p16 8,
p36 90, p48 85, p86 4, p105 1. Mittlere Farbe der Wolkenpixel: Original (118,128,109), 90 %-Perzentil G 155 — eine grosse,
mittelhelle, halbdurchsichtige Flaeche; Port (154..160,170..176,147..152), 90 %-Perzentil G 176 — wenige, kleine, HELLERE
Flecken. Die Port-Sprites sind also nicht zu dunkel, sondern decken 15-30x weniger Flaeche. Pruefrichtung fuer die
Nachbesserung: Sprite-Groesse je Platz (S/Skala/defW aus den Zeilen des Effekts 0x0f, main.c step16/w16) und Zahl der
gleichzeitig gezeichneten Plaetze (Port F36: 6, F48: 12 gezeichnete Zeilen je Bild) gegen das Original (ESP-Pool 3..14
aktive 0x0f-Plaetze, Skalen 0x2000..0x4000) — nicht die Mischung allein.

## 6. Punkt 2a — Wassereffekt beim Sprung von der Kante ins Wasser (eigene Messung)
### 6.1 Lauf B (Leon, 2090, Steg Band 1, Aktion an der Kante)
`RE15_DEBUG_JUMP=2090@240 RE15_PLAYER_POS=4500,-14700,3072,1 RE15_PRESS=square@60 RE15_CLIMB_TRACE=climb.log RE15_FX_LOG=fx.log
RE15_KEIN_GATOR=1 RE15_FRAMEDUMP=50-150/1:f RE15_EXIT_AT=200#2090` (+ NO_INTRO/NOAUDIO/WINDOW_SCALE=1/TITLE_SHOT).
climb.log: T301 sub 9 kind 0x02 (Kante), T302 sub 10, T316 Band 0 y=-1210, T340 Landung (4500,0,-14032).
fx.log: 8 Zeilen `id=6 sub=3` ab F79 bei w(4500,-640,-14032) (Effekt 6 = Wasser-Spritzer der Raum-Bank, Sub 3, Skala 0x3000 —
Code 0x06033000 @0x8003876c-70), 104 gezeichnete Zeilen ueber F79..F91 (8 Plaetze x 13 Bilder), S 8 -> 16 -> 24 (Anim).
Vorher (Dossier 2.1): kein `id=6`. => die Landungs-Spawns sind im echten Lauf da, fuer Leon.
### 6.2 Lauf D (Elza, 2091, gleiche Lage)
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=2091@240 ...` -> debug.log
"Spieler-Familie PL04 (character=4, Elza-Bit=1)", climb.log identisch (sub 9 -> 10 -> Landung T340), fx.log 104 Zeilen
`id=6 sub=3`, 8 Plaetze ab F79 bei w(4500,-640,-14032). => beide Szenarien.
### 6.3 Sichtbarkeit (Einschraenkung meiner Messung)
In beiden Laeufen stehen die Spritzer-Sprites bei sx=819..851 (ausserhalb der 320 px), weil Kamera-Cut 0 von 2090 den
Teleport-Standplatz (4500,-14700) und die Landestelle NICHT zeigt (debug.log "proj sx=1035 sy=25" fuer den Spieler; die
Bilder f000050..f000140 zeigen nur den treibenden Zombie und die Spinne, Leon selbst ist nicht im Bild). Das ist ein
Artefakt des Debug-Sprungs (Cut 0 ohne RVD-Umschaltung), nicht des Effekts — der Spritzer folgt derselben Projektion wie der
Spieler. Eine Sichtpruefung ueber den ECHTEN Weg (Tuer 20A0 -> 2090 auf den Steg, Spawn (6300,-16250), zur Kante, Aktion)
folgt in 6.4.
Unit-Riegel unit_r41_wasser20b0_landung (8 Plaetze, Gier-Faecher +512/+2560/+256/+2304/0/+2048/-256/+1792, Anker = Lage vor
dem Bodenschreiben, trocken 0) gruen (eigener Lauf).

### 6.4 Lauf E2 — der ECHTE Weg (Tuer 20A0 -> 2090, Steg, Kante, Aktion), Leon
Kalibrierlaeufe E0/E1: Eingabeskript-Tick = 270 + Raumbild (2090); LEFT dreht -96/Bild; rot 2048 (Tuer-Spawn) laeuft nach -x,
das Becken liegt bei +z = rot 3072. Lauf E2:
`RE15_DEBUG_JUMP=20a0@240 RE15_PLAYER_POS=4950,5000,2048,2 RE15_INPUT_SCRIPT="W9,A0.2,W4,R0.37,W0.3,U0.7,W0.2,A0.2,W0.3,A0.2,
W0.3,A0.2,W0.3,A0.2,W4" RE15_INPUT_SCRIPT_START=0 RE15_CLIMB_TRACE=climb.log RE15_FX_LOG=fx.log RE15_KEIN_GATOR=1
RE15_FRAMEDUMP=150-330/3:f RE15_EXIT_AT=360#2090`.
debug.log: `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2 spawn=(6300,-1800,-16250)`, `PC loaded
room2090.rdt`, Spawn (6300,-1800,-16250) rot 2048 Cut 2 (= Door_aot_set 20A0 Slot 0: Ziel 0x189c/0xf8f8/0xc086, rot 0x800,
Stage 1 Raum 9, Cut 2, Band 1), Drehung auf rot 3104 -> 3072, Lauf bis (6363,-1800,-14968); climb.log T441 sub 9 kind 0x02,
T442 sub 10, T456 Band 0 y=-1210, T480 Landung (6363,0,-14032); Kamera schaltet auf Cut 3 (RVD).
fx.log: 8 Plaetze `id=6 sub=3` bei F192, gezeichnet bei sx 93..96 / sy 146..148 (IM Bild), 104 Zeilen ueber F192..F204,
S 8 -> 16 -> 24. Sichtpruefung (scratchpad/abn0/landung_e2_streifen.png, landung_e2_zoom.png): Raumbild 195-204 zeigen
kleine weiss-gruenliche Spritzer um Leons Fuesse im Becken (wenige Pixel, kurz). => Der Effekt "fehlt" nicht mehr und ist
ueber den echten Weg sichtbar. VORBEHALT: kein Original-Bild/-Video der 2090-Landung in den Belegen (die Spur hat nur
die Disassembly als Original-Beleg); derselbe Sprite-Zeichner liefert in 20B0 15-30x weniger Flaeche als das Original
(5.6) — ob die Landungs-Spritzer in Groesse/Dichte dem Original entsprechen, ist NICHT gemessen.

## 7. Vertrag (VERTRAG.md)
- Pfade: alle Aenderungen unter re15_port/{engine/src,include,platform/pc/main.c,tests/...} und analysis/befunde_runde41/.
  KEINE release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists (die neuen Tests haengen
  ueber tests/unit/probes/r41_wasser20b0.cmake). OK.
- Bank-9-Bits / Nachrichten-Ids: Spur A hat keine Zuteilung und belegt keine (Diff: kein re15_game_flag_set/MSG-Id neu). OK.
- Gemeinsame Dateien (nur kleine Haken, "Runde 41 Spur A"): aot_common.c 1 Bedingung + Include; climb_common.c 1 Aufruf +
  2 Includes; enemy_ai_common.c 2 NPC-Haken + 2 EM024-Haken (Init/Sub/Bildvorschub entfernt) + Includes; scd_vm.c 1 Zeile;
  wasser_r38.c Getter + 1 Aufruf. OK (jeweils markiert).
- ABWEICHUNG: platform/pc/main.c ist laut VERTRAG §1.3 fuer D (Lautstaerke) und F/G (Szenen-Haken) vorgesehen, Spur A
  aendert dort zwei Stellen (RE15_PLAYER_POS 4. Wert = Messhaken; Schatten-Bedingung `npc->type != 0x24u`). Beide klein und
  markiert, aber nicht im Vertrag; Konfliktrisiko mit D bei der Integration (andere Zeilen, geringes Risiko). Zur Kenntnis.
- re15_actor.h RE15_ACTOR_MAX 21 -> 36 ist ein Querschnitts-Eingriff (alle Aktor-Schleifen; Entscheid E2 "unter Vorbehalt
  der vollen Suite"). Eigener Scan nach Masken/Feldern mit Slot-Bits: s_dog_noclamp_mask (enemy_ai_common.c:7270) ist uint32_t
  mit `1u << slot` (Z.7368) — bei Slot >= 32 undefiniert; praktisch nur 20B0/20B1 belegen Slots >= 20 (ausschliesslich
  EM024, keine Hunde), also kein Fehlverhalten im Auslieferungsstand, aber eine latente Falle (Hinweis, kein Mangel dieser
  Spur). test_r36_gegnerzahl.c auf uint64_t umgestellt (Static-Assert <= 64). Sonst RE15_ACTOR_MAX-dimensionierte Felder.
- Tests je Punkt: Punkt 1: unit bytes/modell/sequencer/raum + integration (exe, Leon+Elza) — messend. Punkt 2a: unit landung
  (8 Plaetze/Faecher/Gate) — messend; keine exe-Pruefung der Landung im Riegel (nur Dossier-Lauf). Punkt 2b: unit npc —
  prueft die Funktion isoliert (Zweige/Ton), der Takt-Teil laeuft ohne Pruefung ("kein ausgelieferter NPC-Typ traegt im
  Clip 5 Fussaufsetzer: Takt-Teil ohne Pruefung") — d.h. es gibt KEINEN Riegel, der einen NPC-Schritt im Spiel nachweist.
- Parallel-/Linux-Festigkeit: integration_r41_wasser20b0 mit eigener exe-Kopie (Praefix re15_pc_r41a_flut_<id>), eigenem
  Laufordner, cmake_minimum_required vorhanden, kein .exe-/Backslash-Pfad; probes-cmake linkt m unter Nicht-WIN32. OK.
- Gezielte Tests (selbst): `ctest -R "^(unit|probe|prop)_(r41_wasser20b0|r36_gegnerzahl|r38_wasser|...|r35_raeume_hundeschatten)"`
  -> `100% tests passed, 0 tests failed out of 78` (68.11 s). Integrationsriegel: siehe Abschnitt 9.

## 8. Punkt 2b — "Auch die NPCs teilen die gleichen Effekte wenn sie durch das Wasser gehen" (+ Nachtrag 3: Fussschritt-Toene)
### 8.1 Was gebaut ist
wassersprung_r41.c: re15_wassersprung_r41_npc_takt(e, a0) prueft das Bildwort des AKTUELLEN Clips des NPC (re15_actor_footstep
auf Clip -1 = e->motion, Bild e->anim_frame) auf das Aufsetzer-Bit 0x4000 und ruft nur dann npc_schritt (Effekt-Zweig +
Ton a0 + (Boden & 0x7f) - 8). Haken in re15_npc_sub_walk (Phase 1 a0 = sub==5, Phase 2 a0 = 0) VOR re15_npc_anim.
Das ist 1:1 die Original-Routine (@0x80051278-a8 / @0x800515e4-614 -> FUN_80045630, belegt in Abschnitt 3).
### 8.2 Datenlage (eigener Lauf des Riegel-Binaers test_r41_wasser20b0.exe npc, Ausgabe woertlich)
"Typ 0x40: Clip 5 = 20 Bilder, Fussaufsetzer 0 (erstes Bild -1)", "Typ 0x42: Clip 5 = 20 Bilder, Fussaufsetzer 0",
"Typ 0x45: Clip 5 = 20 Bilder, Fussaufsetzer 0", Typen 0x49/0x4b/0x4d: keine Bank (CDEMD1.EMS nicht gezaehlt).
Bildworte Clip 5 (0x40): 20 x 0000; Clip 0: Bit 0x5 bei Bild 8; Clip 1: 0x5 bei Bild 7, 0x4 bei Bild 22.
"(kein ausgelieferter NPC-Typ traegt im Clip 5 Fussaufsetzer: Takt-Teil ohne Pruefung)".
=> Fuer JEDEN ausgelieferten NPC-Typ aus CDEMD0.EMS liefert der Takt-Haken 0: kein Wasser-Spritzer, kein Schritt-Ton — im
Port genau wie im Original. Der Nutzer-Wortlaut ("Auch die NPCs teilen die gleichen Effekte") und Nachtrag 3 ("ja alles:
Effekte wenn sie sich im Wasser bewegen, Fussschritt-Sounds wenn sie laufen. All das muss auch fuer die NPCs gelten") sowie
Entscheid E1 (Aufsetzer-Bilder des Geh-Clips 0/1 auf Clip 5 abbilden, je Typ, beide Szenarien, Pin je Typ) sind NICHT
umgesetzt: kein Code-Commit nach 18:36 (E1 18:5x, Nachtrag 3 19:12), keine Abbildungstabelle, kein Riegel mit einem
NPC-Schritt im Spiel. Das Dossier fuehrt es unter "ENTSCHEIDUNGSFRAGE" und "OFFEN".
### 8.3 Eigene Sonde (scratchpad/abn0/sonde_npc2000.c, gegen libre15_engine.a gelinkt, kein Code im Baum)
ROOM2000 am VM mit Flag (4,244)=1 (sub00 @0x1798 `Ck(4,244)==1` -> Sce_em_set 0x42 Marvin (550,-1800,-20800) + 0x40 Ada
(2450,-1800,-20400), Evt_exec sub02): beide NPCs erscheinen (state 4, Marvin mo 1/2, Ada mo 23), 2500 Bilder:
`npc_schritte=0 npc_boden=0 npc_stempel=0 plaetze=0`. Einschraenkung: in der VM-Sonde laeuft sub02 nicht weiter (der
Spieler bleibt mode=0, Plc_dest wird nur im Hauptschleifen-Pfad gefahren), die Laufphase der NPCs (sub02 @0x0192E
`Plc_dest 40 00 04 01`, @0x0193E `40 00 04 02` = Entity 1/2) wurde hier nicht erreicht -> die exe-Messung folgt in 8.4.

### 8.3a Nebenbefund zur Szenensperre (betrifft E1 "in Szenen UND beim freien Lauf")
AOT-Zensus ROOM20B0/20B1 (scd_dump_room.py): 5 Door_aot_set (sce 2), 2 Aot_set sce 8 (Slots 5/6, Flags 0x47 = WATER-Stempel,
alle Pools), 1 Aot_set sce 3 Flags 0x41 (Slot 7 -> Port-Typ AUTO_EVENT 6, aot_common.c Z.304). Die Sperr-Aufhebung der Spur
(re15_wasser20b0_r41_auto_aot: nur Typ AUTO_EVENT in 20B0/20B1) trifft also genau Satz 7 — korrekt eng.
ABER: aot_common.c Z.1169 `if (in_cinematic && a->type != CAM_SWITCH && !auto_aot(...)) continue;` steht VOR der
WATER/RAMP-Stempelschleife (Z.1347-1366, je Pool-Entity +0x88 := p0). Waehrend einer Szene (player_mode 2) stempelt der
Port damit WEDER Spieler NOCH NPC — im Original laeuft FUN_800436a8 ohne Gate (Abschnitt 3). Fuer NPC-Wasserschritte in
Szenen (E1/Nachtrag 3) muss diese Port-Sperre fuer WATER (und allgemein) fallen; das Dossier fuehrt die allgemeine Aufhebung
als OFFEN. => Zusaetzlicher Mangel-Punkt fuer die Nachbesserung von 2b (ohne ihn bleibt der Stempel-Zweig in Szenen tot).

### 8.4 Lauf F — NPC-Laufszene ROOM2000 an der exe (Leon)
`RE15_SET_FLAG=4:244 RE15_DEBUG_JUMP=2000@240 RE15_PLAYER_POS=4350,-20500,0,1 RE15_FOOT_DEBUG=1 RE15_SE_DEBUG=1 RE15_FX_LOG=fx.log
RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP=0-1500/50:f RE15_EXIT_AT=1500#2000` (Audio AN, damit [foot] Fuss/Typ meldet).
debug.log: `[setflag] flag(4,244/0xf4) = 1`, `Sce_em_set type=0x42 ... pos=(550,-1800,-20800)` (Marvin, Slot 1), `type=0x40
... (2450,-1800,-20400)` (Ada, Slot 2), Cut_chg(11), Spieler-Plc_dest; ab F874 Cut_chg(15) und die NPC-Laeufe:
`Plc_dest(slot=1 mode=0x04 dest=(-6000,-20800)) -> state4/sub4 (Sub-VM-WALK)` (6550 Einheiten), `Plc_dest(slot=2 mode=0x04
dest=(-4400,-20400))` (6850 Einheiten), danach `Plc_dest(slot=1 mode=0x09 dest=(-2450,-21150))`, `slot=2 mode=0x09`.
[foot]-Aufrufe im ganzen Lauf: `[foot] SE foot=7 type=29 vag=1` x4, `foot=4 type=29` x3 — ausschliesslich der Spieler
(Typ 0x1d = Bodencode 29 ohne -8); ein NPC-Schritt haette type = 0 + (0x1d & 0x7f) - 8 = 21 bzw. 22 (a0 = 1, Sub 5)
gemeldet: KEINER. fx.log id=6: nur die zwei Dauer-Emitter des Raums (sub 0 bei (3635,-10000,-24307) und (-5540,-10000,
-21205)), kein Schritt-Platz. => Im gebauten Stand laufen Marvin und Ada ~13 000 Einheiten durch ROOM2000 ohne einen
einzigen Schritt-Ton und ohne Effekt. Nachtrag 3 / E1: NICHT erfuellt (gemessen).
## 9. Integrationsriegel der beruehrten Module (selbst gefahren, sequentiell, keine GUI-Parallelitaet)
- `ctest -R "^integration_r41_wasser20b0$"` -> Passed 100.61 s (5.2).
- `ctest -R "^integration_(r36_gegnerzahl|r36_klettern|r38_wasser)$"` (eine Kette):
  integration_r36_gegnerzahl Passed 96.20 s (Teil e: 35 Emitter / 1 Schatten bei Kapazitaet 36 — die vom Bau-Agenten
  angepasste Erwartung; Teile a-d: 1030/1031/1040/1041 Records 15..19 unveraendert gruen -> RE15_ACTOR_MAX 36 ohne
  Nebenwirkung auf die Save(0x12)-gedeckelten Raeume),
  integration_r36_klettern Passed 524.49 s (Kletter-/Landungs-Haken in climb_common.c: trockene Landungen 10F0 ohne
  Nebenwirkung),
  integration_r38_wasser: (Ergebnis unten, 9.1).

### 9.1 integration_r38_wasser
Passed 206.65 s (Spieler-Wasserschritte, wasser_r38-Haken Getter/Bildanfang ohne Nebenwirkung). Kette gesamt:
`100% tests passed, 0 tests failed out of 3`, 827.46 s.

### 9.2 Kette 2 (Schattenliste / pri-Schatten / Wasserzombie — main.c-Schattenhaken, wasser_r38-Haken)
`ctest -R "^integration_(r39_gegner_schatten|r36_pri_schatten|r38_wasserzombie)$"`: integration_r36_pri_schatten Passed
31.16 s, integration_r38_wasserzombie Passed 170.07 s, integration_r39_gegner_schatten Passed 59.36 s ->
`100% tests passed, 0 tests failed out of 3`. Summe aller eigenen Laeufe: 6 + 78 + 1 + 3 + 3 = 91 Tests gruen, 0 rot
(keine Wiederholung noetig, kein flatternder Fenster-Haken).

## 10. Urteile je Nutzer-Punkt (gegen den Wortlaut)
### Punkt 1 — "Runde 41 startet mit dem Wasserschwall in 20B0 im Original ... in unserem Port noch kaputt" -> TEILWEISE
Erfuellt: Original gemessen (Savestates + Video, eigene RAM-Lesung bestaetigt 35 Emitter, Drift, Part-Wort 0, Phasen/
Takte); Mechanik byte-true portiert (Sequencer grid 0/1 = STAGE2.BIN-Bytes, Zeichenmaske, AUTO-Satz 7 unter Pad-Sperre,
35 Plaetze mit Deckel-Beleg 255 @0x8003ed58-60); Szene laeuft im Port zu Ende (Cut 7/15/6, Spieler frei F242, Leon UND
Elza); Schnittfolge nach Stau-Korrektur +-2 Bilder; Partikel an denselben Orten im selben 10-11-Takt-Rhythmus.
NICHT erfuellt: Bild-fuer-Bild-Gleichheit (Orchestrator-Massstab, Nachtrag 2 des Nutzers "Ist das Video vom Original
identisch deinem Port, dann ist es richtig umgesetzt, sonst nicht"): Wolkenflaeche Faktor ~5 (G>=150) bzw. 15-30 (G>=100)
kleiner, erster Schub o5-9 fehlt (Port erst F11-15), Letterbox-Balken am Szenenanfang und 24 statt 16/17 px.
Der Fix erklaert die drei gemessenen Mechanik-Defekte (keine Partikel / Mesh / sub05), nicht den sichtbaren Dichte-
Unterschied — der bleibt unerklaert (kein Disasm-Beleg fuer Groesse/Zahl/Mischung je Platz im PC-Zeichner).
### Punkt 2 — "Wassereffekt vom rein springen von der Kante in das Wasser. Auch die NPCs teilen die gleichen Effekte" -> TEILWEISE
2a (Sprung): erfuellt — 8 x 0x06033000 @0x8003876c-70/@0x80038794/@0x800387bc, Gate `slt` @0x8003874c, Gier-Faecher; an der
exe fuer Leon (2090) und Elza (2091) gemessen, ueber den echten Weg sichtbar (6.4). Vorbehalt: kein Original-Bild der Landung.
2b (NPCs): NICHT erfuellt — Mechanik portiert, aber im gebauten Stand erzeugt kein ausgelieferter NPC einen Schritt
(Lauf F: Marvin/Ada laufen ~13 000 Einheiten in 2000 ohne [foot]-Aufruf und ohne Effekt; Clip-5-Bildworte ohne 0x4000).
Entscheid E1 + Nachtrag 3 (Aufsetzer aus Clip 0/1 auf Clip 5 abbilden, je Typ, beide Szenarien, Schritt-Ton + Wasser-
Effekt, Pin je Typ) sind nicht gebaut (letzter Code-Commit 18:36 vor E1/Nachtrag 3). Zusaetzlich blockiert die Szenen-
sperre in aot_common.c Z.1169 den WATER-Stempel fuer alle Entities waehrend player_mode 2 (8.3a).

## 11. Gates
- RE-Gate: HAELT. Alle verhaltensrelevanten Konstanten im Diff tragen @0x-Adressen; 12 Stellen selbst disassembliert und
  bestaetigt (Abschnitt 3); Schatten-Scan selbst (4); Savestate-RAM selbst gelesen (3); keine Guess-Tells im Diff.
  Offen/klein: a2 beim Executor-`jalr` @0x80031cb4 (Entity-Matrix +0x20 als Landungs-Anker) ist statisch nicht aus dem
  Prolog belegbar (Registerherkunft vor 0x80031c80) — der Port nimmt die Lage vor dem Bodenschreiben; Wirkung gemessen
  (Spawn bei y=-640 der Fall-Lage), nicht am Original verglichen.
- Vertrag: HAELT mit einer Abweichung (main.c-Haken ausserhalb der Zuteilung, 7).
- Tests: alle gezielten Laeufe gruen (6 + 78 + 1 + 3 [+ Kette 2 in 9.2]); fuer Punkt 2b existiert kein messender Riegel
  im Spiel (nur die isolierte Funktion).
- bestanden = NEIN (Punkt 1 teilweise, Punkt 2 teilweise).

## 12. Maengel (nummeriert, nachpruefbar)
M1 (Punkt 1, Orchestrator-Massstab): Spritzerwolke 20B0 im Port 5x (G>=150) bzw. 15-30x (G>=100) weniger Flaeche als im
   Original-Video, erster Schub an der Osttuer (o5-9, 95..747 px) fehlt (Port F11-15, 33..145 px). Messung: zaehl.py auf
   A_belege/orig_20b0 vs RE15_FRAMEDUMP-Lauf (5.3/5.6). Richtung: Sprite-Groesse/Anzahl je Platz (S, Skala, defW, step16,
   gleichzeitig gezeichnete Plaetze 6..28) gegen Original-VRAM/ESP-Pool belegen, nicht Mischmodus allein.
M2 (Punkt 2b, NUTZER-VORGABE Nachtrag 3 + E1): NPC-Schritte (Wasser-Spritzer/Wellen UND Fussschritt-Toene) fuer die Typen
   0x40..0x4b nicht gebaut: Clip-5-Aufsetzer-Abbildung aus Clip 0/1 fehlt, kein Pin je Typ. Nachweis: Lauf F (8.4), Riegel-
   Ausgabe "Fussaufsetzer 0", Commit-Historie ohne Code nach 18:36.
M3 (Punkt 2b, Szenen): aot_common.c Z.1169 ueberspringt waehrend in_cinematic auch die WATER-Stempel (Typ 9) fuer alle
   Pools — NPC-Wasserschritte in Szenen koennen den Stempel-Zweig (0x06032000) nie erreichen; Original FUN_800436a8 ohne
   Gate (@0x8001ce1c linear, @0x800436bc-cc/@0x80043740). Aufhebung (mindestens fuer WATER, besser allgemein) + Riegel.
M4 (Punkt 1, Bild-fuer-Bild): Letterbox-Balken im Port F5-F12 (24 px), Original o1..o100 ohne Balken; spaeter Port 24 px
   vs Original 16/17 px. Ohne @0x-Beleg auf beiden Seiten — RE'en (Balkenhoehe/Zeitpunkt des Originals) oder als nicht
   Teil der Spur ausweisen (Orchestrator).
M5 (Riegel): kein messender Riegel fuer einen NPC-Schritt im Spiel und keine exe-Pruefung der Landung im Riegel-Satz
   (unit landung prueft die Funktion isoliert; integration prueft nur 20B0/20B1). Fuer die Nachbesserung: Pin je NPC-Typ
   (Schritt-Ton + Wasser-Effekt, z.B. 2000-Szene mit RE15_SET_FLAG=4:244) und ein exe-Riegel der 2090/2091-Landung.
M6 (Vertrag, klein): platform/pc/main.c ist Spur A nicht zugeteilt (VERTRAG §1.3: D, F/G); zwei markierte Haken dort.
M7 (Hinweis, kein Spur-Mangel): s_dog_noclamp_mask uint32_t mit `1u << slot` bei RE15_ACTOR_MAX 36 (enemy_ai_common.c:7270/
   7368) — latent undefiniert fuer Slots >= 32 (nur 20B0/20B1 belegen sie, dort keine Hunde). PSX-BSS +28 KB OFFEN.
