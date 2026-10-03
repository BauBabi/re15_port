# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 1

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, gepruefter Stand HEAD 6690f0de (Basis master 154a73c1,
44 Commits). Abnahme 2026-10-03 nach Nachbesserung 1 (Maengel M1-M5 aus J_abnahme_0.md). Massstab: Wortlaut
AUFTRAG.md, Regeln VERTRAG.md / CLAUDE.md. Alle Messlaeufe selbst gefahren (Scratch `scratchpad/jabn1/`, exe-Kopie
`re15_port/build/platform/pc/re15_pc_jabn1.exe`, md5 49b2e558... = re15_pc.exe), nur eigene PIDs beendet.

**Ergebnis: NICHT BESTANDEN** — Punkte 1, 2, 3, 5, 6 erfuellt; Punkt 4 teilweise (zwei gemessene, ungeklaerte
Abweichungen im Affenkampf, vom Bau-Agenten selbst unter OFFEN gefuehrt, Status trotzdem "fertig").

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. exe 22:20; letzte Code-Aenderung c51ac684 (Pin-Variante);
  danach nur Kommentar im probes-Kopf (39995920) und Dossier (`git diff 0bb44d65 HEAD --stat -- re15_port`).
- Dossier: woertlich `=== LOCAL-BUILD-OK (all) — Tests 493/493`.
- Selbst gefahren: `ctest -R "r35_affen|^unit_member$|^unit_maggot_ai$"` -> **17/17 Passed** (15 Riegel + 2 Alt-Riegel).
- Selbst gefahren: `bash re15_port/tools/local_build.sh all` -> `test OK — 493/493 bestanden` / **`=== LOCAL-BUILD-OK (all) — Tests 493/493`** (1129,6 s, kein Fenster-Haken rot; Log scratchpad `jabn1/suite_all.log`).

## 1. Echter Weg
Alle Raum-Messungen ueber die TUER von ROOM11B0 (wie Abnahme 0, Ursache dort belegt): `RE15_SET_FLAG=4:243,3:130`
(Generator an, 11B0-Szene gesehen) `RE15_DEBUG_JUMP=11B0@240 RE15_PLAYER_POS=-25500,-28200,1024
RE15_PRESS=square@300,330,...,900` + `RE15_STATE_LOG RE15_EVT_TRACE RE15_ENEMY_DBG`. debug.log in jedem Lauf:
`[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)` -> `[room] PC loaded room11c0.rdt` -> `[evt] F6 room=11c0
Evt_exec sub=2` -> `[evt] F1088 room=11c0 Evt_exec sub=7`. Bilder nur ueber RE15_FRAMEDUMP.
Laeufe: n2/n2b (ohne Eingabe), n3 (Handfeuerwaffe Item 3), n4 (wie n2, `RE15_FORCE_CUT=4`), n5 (Item 7 = SUPER
REDHAWK), n6/n6b (`RE15_INPUT_SCRIPT=W34.5,U2.5,W1`, Leon im Freien), n8 (Item 8 = REMINGTON M870), n9 (Item 9 =
Handgranate), n10 (`RE15_DEBUG_JUMP=11C0@240`, ohne Eingabe = dieselbe Lage wie die Original-Aufnahme r3).
Feuerskript n3/n5/n8/n9: `RE15_INPUT_SCRIPT=W40,(MA0.1,M0.6)x150 BASIS=spiel START=60` (wie Abnahme 0 t3/t5).

Original-Referenz geprueft: die r3-Savestates (MZD-Disc) laufen mit **byte-gleichem Code** — Savestate s035 RAM
0x80100000..+0x219b0 gegen `info/Re1.5/PSX/BIN/STAGE1.BIN`: 0 Abweichungen; PSX.EXE 0x8001a780-ae40,
0x8001c2dc-c400, 0x8003bc2c-bca8, 0x800360e8-366b0: je 0 Abweichungen (Skript `jabn1/`, re15_ss.Ram).

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**
- Verstecken, n2 state.log (11C0-Segment): `F1088 Ada st=4/5/2` (sub07 `Plc_dest(slot=1 mode=0x05
  dest=(-18214,-7229))`), `F1145 st=4/6/0 @(-18025,-7379)` (Original-Savestate t=47.02: (-18000,20000,-7403)).
- Bild `jabn1/n4_ada.png` (Cut 4, F1075-F1300): F1105 Ada laeuft zu den Wagen, F1120/F1135 klein am Wagen, **ab F1150
  nicht mehr im Bild** (F1150/F1180/F1300 leer). y = 20000 (@0x1C74) pinnt Riegel `ada` (gruen).
- Zurueckkommen, n5 (Item 7, beide Gorillas WIRKLICH getoetet, kein gesetztes Kill-Bit): Slot 2 `F1449 st=3 hp=-20`,
  Slot 3 `F1976 st=3 hp=-20`; debug.log `[evt] F2018 room=11c0 Evt_exec sub=3`, `[scd F2048] Cut_chg(4)`, `F2606
  Evt_exec sub=4`, danach `PC loaded room11b0.rdt`. Bild `jabn1/n5_ada_zurueck.png`: F2300 Ada steht wieder neben
  Leon, F2380/F2400 "Ada: ...There are some outrageous monsters out there.", F2420-F2500 "Leon: Yeah... Anyway,
  let's get out of here." Ebenso n8 (M870): `F2310 Evt_exec sub=3`.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- n2 state.log: beide Gorillas ab F1 `st=1/0/0 hp=180` (INIT im Spawn-Bild; Original t=6.11: st=1, hp 180);
  `F816 cam=12 S2 st=1/0/1 g=10 mo=22 @(-3617,-17798)` = Austritt (Original t=36.41: (-3617,0,-17798) g=10 c=22).
- Bild `jabn1/vgl_wagen.png` (oben Original s020 t=34.89 / s021 t=36.41 / s022 t=37.92, unten Port n2b F800/F840/
  F870): Gorilla sitzt in der offenen Heckklappe, steigt an derselben Stelle gross vor dem Pfeiler aus; Lage und Groesse
  decken sich (Versatz nur durch die Szenenbalken). `jabn1/n2b_wagen.png` F790-F870 durchgehend.
- Riegel `wagen` (M2 aus Abnahme 0) pinnt jetzt die Ursache: Spawn-INIT Zustand 1/HP 180/Scale 0x1b33, y = -2500 in
  36 Bildern, Austritt (-3617,0,-17798) — selbst gefahren, alle Pruefungen ok.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper" — **erfuellt**
- Bild `jabn1/vgl_brust_zoom.png` (3,5x, Original s021/s022 gegen Port n2b F840/F870): Brust in beiden geschlossen
  braunes Fell, keine weisse/geaderte Platte, kein abstehendes Teil.
- In Bewegung: `jabn1/n6b_brust_zoom.png` (F1256-F1304, Gorilla aufrecht beim Brustschlag, Nachbar gebeugt) und
  `jabn1/n2b_ada.png` (F1070-F1250, zwei laufende Gorillas): kein mitschwingendes Fremdteil am Rumpf.
- Riegel `teile` gruen; Beleg @0x80117200-3c in Abnahme 0 selbst disassembliert (unveraendert).

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **teilweise**
Was jetzt stimmt (selbst gemessen):
- Zielstrebig/aggressiv: n2 (Tuerweg, Leon ohne Eingabe) Heavy F1477, danach 15 Bisse F1583..F2079, Tod F2079. Die
  Gorillas laufen auf Leon zu (keine Rueckwaerts-Kriecherei mehr, vgl. Abnahme 0).
- Regel-Gleichheit, selbst nachgerechnet aus den ROH-Spuren des Bau-Agenten (`jnb1/g_orig.txt`, `g_desync.txt`,
  eigener Dekoder `jabn1/hits.py`, Spieler +0x9a je Bild): Original Gleichtakt Treffer F62(-12), F166, 219, 269, 322,
  372, 425, 476, 528, 580, 631, 684, 734, 788, 837, 892 = Abstaende **53,50,53,50,53,51,52,52,51,53,50,54,49,55**;
  Original nach GDB-Desync (e2 +0x1dc := 30) **36,36,36,36,35,36,...**, Leon (-6891,-12548) -> (-8006,-11268) nach
  NW. Port n2: Abstaende **36,36,36,35,35,35,35,35,36,35,36,35,36,35**, Leon (-7083,-12346) -> (-7923,-11343) nach NW
  = der Wechseltakt des Originals. Riegel `takt` (Port ab Original-Bild F195): 218 270 321 374 424 478 527 582 gegen
  Original 218/268/321/371/424/475/527/579 (max. 3 Bilder).
- Griff-Variante (Abnahme-0-Befund "Sprung beim Zupacken"): exe n6b Pin F1203 (gr=1, mo=1), Leon steht F1203-F1214 auf
  (-6908,-14468), erste Platzierung **F1215 = Opfer-Clip-Bild 0x0c** (vorher Abnahme 0 t6: Sprung 1600 in F1203).
  Original-GDB: stehen bis F265, Platzierung ab F266 = Bild 0x0c. Fix erklaert den Befund.

Was NICHT stimmt (gemessen, Mechanismus offen):
- **(a) Gleicher Weg, anderes Ergebnis.** n10 (`RE15_DEBUG_JUMP=11C0@240`, Leon ohne Eingabe; Spawn (-22604,14455) =
  r3-Spawn): Szenen-Endlage Leon **(-7153,-12351)** gegen Original r3 **(-7138,-12372)** (26 Einheiten); danach Bisse
  F1565..F2069 im **36er-Wechseltakt** (14 Abstaende je 36), Leon tot **F2069** (Freigabe n10 debug.log `[evt] F1070
  room=11c0 Evt_exec sub=7` -> 999 Bilder = 33,3 s). Original r3 im
  selben Szenario: **Gleichtakt ~51,6**, Tod **39,4 s** nach der Freigabe. Der Bau-Agent fuehrt das als OFFEN ("Szenen-
  Endlage ~25 Einheiten, entscheidet welcher Takt entsteht"), Ursache der 26 Einheiten nicht gefunden.
- **(b) Wurf-Bahn des Rear-up-Griffs ab Opfer-Clip-Bild ~0x22 nicht wie das Original.** Original (`jnb1/g_griff.txt`,
  selbst dekodiert `jabn1/g_griff_dec_mine.txt`): F288-F289 Leon (-3964,-12506) Bild 34/35, **F290 (-4588,-11243)
  Bild 0x24, F291 (-5381,-10551) Bild 0x25**, danach weiter ~100/Bild bis (-5381,-10936) F301, Zustand 5 bis F378 (Clip
  0x10 F340-F352, Clip 0xb F355-F376), **frei F379 bei (-4759,-10633)**. Port-Riegel `griff` aus derselben Lage:
  F287 (-3965,-12507) = Original F288, dann **bleibt Leon dort stehen** (F288-F336 unveraendert), frei F339 bei
  (-3685,-12254) -> Endlagen ~1950 auseinander. Der Riegel `griff` prueft nur bis zur ersten Platzierung (Bild 0x0c).
  In der exe (n6b) springt Leon bei Bild 0x24 (F1239 1493 Einheiten) — Harness und exe verhalten sich hier also
  verschieden, und keines von beiden ist ab Bild 0x22 gegen das Original belegt. Vom Bau-Agenten als OFFEN gefuehrt
  ("Messweg: 0x8001ad68 ... +0xa0/+0xa2").
- Beides sind sichtbare Abweichungen des Affenkampfs vom Original ("da stimmt noch irgendwas bei der Uebernahme
  nicht"); CLAUDE.md: "Mechanismus nicht gefunden = NICHT fertig" -> Maengel A1/A2.

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- n6 (Tuerweg, Leon im Freien, ohne Eingabe nach 2,5 s Gehen): Rear-up + Griff F1198/F1548/F2093 (ein vierter F1896
  verbindet nicht), danach jedes Mal **Clip 3**: `F1251 S3 Clip 3 st=1/15/5 af=23`, `F1601`, `F2146` (n6b
  bildgleich bis F2100).
- Bild `jabn1/n6b_brust_zoom.png` (F1256-F1304, 4er-Schritt): der Gorilla steht aufrecht und fuehrt die Arme
  abwechselnd an die Brust (F1276, F1296/F1300 Arm an der Brust; F1264/F1284 aussen), danach auf allen vieren
  (`n6b_brust.png` F1316). Am Szenen-Endpunkt (n2) kein Rear-up — wie im Original r3 (40 s dort, keiner).
- Clip-3-Zeitpunkte gegen das Original (GDB-Griff): Original e1 `1/15/5 c3/24` F304, `1/2/1 c3/30` F331; Port-Riegel
  `griff` `1/15/5 c3/27` F306, `1/2/1 c3/30` F330.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**
Auswertung `jabn1/p6.py` (Flinch-Eintritt st 1->2, Exit-Sub beim Austritt, sub-7-Eintritte):
| Lauf | Waffe (inventory_common.c s_item_names) | Spur | Slot 2 Exit-Subs | Slot 3 Exit-Subs | Spruenge nach Treffer |
|---|---|---|---|---|---|
| n3 | 3 BROWNING HP | 0 | 3,3,7,3,3,7,3,3,7,3,3,7,3 (13 Treffer) | 3,3,7,3,3 (5) | 4 + 1 (je der 3.) |
| n5 | 7 SUPER REDHAWK | 1 (Zeile 7) | 3,3,7 | 3,3,7 | 1 + 1 |
| n8 | 8 REMINGTON M870 | 1 (Zeile 8) | 3,3,7,3 | 3,3,7,3 | 1 + 1 |
| n9 | 9 HAND GRENADE | 2 | kein Treffer | kein Treffer | — |
- Vorher (Abnahme 0 t5, Item 7): 6 Treffer = 6 Spruenge (7,7,7 / 7,7,7). Zaehler setzt sich nach dem Sprung zurueck
  (n8: 4. Treffer -> 3).
- Spur 2 (Granaten/Werfer, Zeilen 9..11/15..18) ist nur im Riegel `sprung` gemessen (Zeile 9 und gemischt 3,7,9,9,3,7
  -> 3,3,7,3,3,7; selbst gefahren, 25 ok-Zeilen); im Spiel traf die Handgranate in n9 nicht (Granaten-Reichweite =
  Spur A). Gleiche Funktion `re15_affen_sprung_oder_jagd` in allen drei Exits.
- Nicht-Treffer-Spruenge: je Lauf ein Fernsprung des Selektors aus sub 4 (n3 F1380, n5 F1395, n8 F1312; Path B
  @0x80118028-8100), ohne Treffer, wie im Original r3 (t=54.55). Siehe Hinweis H1.

## 3. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)
Diff `git diff master -- re15_port` vollstaendig gelesen. Jede verhaltensrelevante Konstante traegt @0x... oder ist
gekennzeichnet (RE15_AFFEN_TREFFER_BIS_SPRUNG = 3 NUTZER-VORGABE; Ersatz-Sub 3 in re15_affen_sprung_oder_jagd =
PORT-WAHL; mag_hit_ctr = PORT-FELD). Stichproben, SELBST disassembliert (`.claude/skills/re15-psx-disasm/scripts/
re15_disasm.py`, richtige Binaerdatei, Sprungziele mit):
1. **STAGE1 0x8011abe8-acb4 (Pin-Latch, neue Reihenfolge):** `jal 0x8001ac38` @0x8011ac18 (a0 = s1 = 0x800aca54
   Spieler), `sb v1(5),-13736(at)` @0x8011ac48 (aca58), **`jal 0x8001a780` @0x8011ac50** (a0 = Spieler),
   `sb v0,-13735(at)` @0x8011ac68 (aca59), ..., **`jal 0x8001a8f8` @0x8011acac** mit `ori a1,zero,0x800` @0x8011acb0,
   a0 = s0-372 = 0x800aca88 = Spieler+0x34. Sprungziel 0x8001a780: `lw v1,g_entity(cur)`, `lh v0,106(a0)` -
   `lh v1,106(v1)`, `+1024`, `andi 0xfff`, `slti 2048`. **Bestaetigt** (a780 VOR dem Yaw-Latch). Port-Fix
   `re15_player_victim_latch_ex(e, pl, re15_maggot_a780(e, pl))` wertet das Argument vor dem Latch aus. Siehe H2.
2. **STAGE1 Spur-1/2-Exits und -Eintritte:** `ori v0,zero,0x7` / `sb v0,5(v1)` @0x8011b3c8-cc (vorher `sb zero,147(a0)`
   @0x8011b3ac, `sb v0,4(v1)` @0x8011b3bc; danach `sb zero,6/7` @0x8011b3dc/ec); `ori v0,zero,0x7` / `sb v0,5(v1)`
   @0x8011b6c4-c8 (mit `sb zero,147(v0)` @0x8011b6a8, `+0x4 = 1` @0x8011b6b4-b8); Eintritte `lbu/ori 0x2/sb 147`
   + `+0x7 = 1` @0x8011b238-54 und @0x8011b44c-68. **Bestaetigt.**
3. **PSX.EXE FUN_8001c2dc + Handler [5] (Knockdown-Sonde):** Band `lui 0x91a2 / ori 0xb3c5 / mult / sra 10 / subu`
   @0x8001c2e8-2c, `jal 0x8003b7f0` @0x8001c340, Flag-Clear im Delay-Slot `sb zero,0(s2)` @0x8001c354, `jal 0x8003bc2c`
   @0x8001c358, `andi 0x1` @0x8001c37c -> `sb s4,0(s2)` @0x8001c3c0, `andi 0x2` @0x8001c390 -> Flag 0, `andi 0x600`
   @0x8001c3b0, Band-Schleife `bne ... 0x8001c330` / `addiu s0,s0,-1` @0x8001c3f0-f4; Sprungziel 0x8003bc2c: vier
   Quadranten-Listen, `lhu v0,10(v1)`/`sll 16`/`sra 28` == Band. Handler [5]: Abbau `subu`/`sh -13600` @0x80036584-8c,
   `lhu a1,6(a1)` @0x80036590, `jal 0x8001c2dc` @0x80036594, `sh zero,-13600` @0x800365b0, `jal 0x800245d8`
   @0x800365b4 — Reihenfolge Abbau -> Sonde -> Vorschub wie der Port. Box[6]: `read 0x80073e94` u16 = [0,64006,0,
   **450**,...]. **Bestaetigt** (re15_affen_kd_sonde inkl. affen_band_hat_zelle).
- Guess-Tell-Suche im hinzugefuegten Code (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|
  ungefaehr|geschaetzt|vermutlich`): **0 Treffer** (der Abnahme-0-Treffer "Plausibilitaet" im Testkopf ist weg).
  Env-Schalter: nur `RE15_AFFEN_FUSS` (Mess-Log, kein Spielverhalten).
- Fix erklaert Befund: M3 — Vorbedingung (t5: Zaehler nur in Spur 0, 6 Treffer = 6 Spruenge) im Protokoll, nachher
  3,3,7 (n5/n8); Griff — Vorbedingung (t6: Sprung 1600 im Latch-Bild, a780 nach dem Yaw-Latch = immer Rueck-
  Variante) im Protokoll, nachher Stehen bis Bild 0x0b (n6b). M1-Takt — kein Code-Fix, Widerlegung per Original-
  Rohspur selbst nachgerechnet (Abschnitt 2, Punkt 4).

## 4. Vertrags-/Pfad-Gate, Tests
- `git diff master --name-only`: keine Pfade unter `release/`, `platform/android/`, `shared_assets/PSX/`; keine Edits an
  `tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`. Neue Dateien affen_11c0.c, re15_affen.h,
  test_r35_affen.c, probes/r35_affen.cmake (Vertrag 1.4).
- Hakengroesse (M5 aus Abnahme 0), `git diff master -U0` je Hunk gezaehlt: enemy_ai_common.c 19 Hunks, **max. 2 Zeilen**
  (+22/-13); game_step_common.c 4 Hunks max. 2 (+4/-2); main.c 3 Hunks max. 2 (+5); scd_vm.c +1; actor_common.c +2/-2;
  emd_common.c +2. **M5 behoben.**
- Bank-9-Bit 82, Nachrichten-IDs 20..23, Ereignis 25: nicht belegt; keine Assets.
- Tests: 15 Riegel `unit_r35_affen_*` + unit_member/unit_maggot_ai: 17/17 gruen (selbst). Messend je Punkt: P1 `band`/
  `ada`; P2 `wagen`; P3 `teile`; P4 `flug`/`kdsonde`/`biss`/`frac`/`takt`/`griff`; P5 `brust`/`anker`/`griff`;
  P6 `sprung` (24 Pruefungen, Spur 0/1/2/gemischt)/`schrot`; M4 `npcband`. Luecke: `griff` prueft die Wurf-Bahn nach
  Bild 0x0c nicht (A2); kein Riegel faehrt die Szene von ihrem Anfang bis zum ersten Biss gegen r3 (A1).

## 5. Maengel (nummeriert, nachpruefbar)
- **A1 (Punkt 4): Im selben Szenario wie das Original toetet der Port Leon 6 s frueher.** Messung n10
  (`RE15_DEBUG_JUMP=11C0@240`, keine Eingabe, Spawn (-22604,14455) wie r3): Leon-Endlage nach sub02 (-7153,-12351)
  gegen r3 (-7138,-12372) (Tuerweg n2: (-7164,-12350)); Bisse F1565..F2069 im 36er-Takt, Tod 33,3 s nach der
  Freigabe; Original r3: Gleichtakt ~51,6 Bilder (Rohspur g_orig.txt), Tod 39,4 s. Die Regeln sind belegt gleich
  (Riegel `takt`), aber die Ursache der 26 Einheiten (und damit des anderen Takts) ist nicht gefunden. Naechster
  Messweg (vom Bau-Agenten selbst genannt): GDB-Einzelbild-Spur des Originals ueber sub02 (Leons Plc_dest-Gang, Haltepunkt
  auf die Spieler-Wurzel) gegen den Port-Gang Bild fuer Bild; Gegenprobe: Port-Harness mit Leon exakt auf (-7138,-12372)
  am Szenenende -> entsteht dann der Gleichtakt?
- **A2 (Punkt 4/5): Wurf-Bahn des Rear-up-Griffs ab Opfer-Clip-Bild ~0x22 nicht original.** Original (GDB g_griff.txt):
  Bild 0x24/0x25 Platzierung nach (-4588,-11243)/(-5381,-10551), danach Weiterwandern ~100/Bild, Zustand 5 bis F378,
  frei F379 bei (-4759,-10633). Port-Riegel `griff` (gleiche Startlage): bleibt ab Bild 33 bei (-3965,-12507), frei F339
  bei (-3685,-12254) — ~1950 auseinander; exe n6b springt dagegen bei Bild 0x24 (F1239, 1493 Einheiten). Fehlt: der
  Mechanismus (Platzierung 0x8001ad68 Bild 0x22-0x24, Gorilla-Anker +0xa0/+0xa2, Wurzelbewegung des Opfer-Clips 1 ab
  Bild 0x25) und ein Riegel, der die Bahn bis zur Freigabe gegen g_griff.txt prueft; dazu klaeren, warum Harness und exe
  bei Bild 0x24 verschieden sind. Dazu misst der Harness P3-P6 nicht: frei F339 = 2 Bilder nach dem Clip-1-Ende
  (Original 41 Bilder Clip 0x10/0xb), weil enemy_ai_common.c:1650-1651 ohne PL00-Bank `if (fc <= 0) fc = 1; /* Bank
  fehlt (Unit-Kontext) */` setzt — der Riegel braucht die PL00-Bank, um die Freigabe (Original F379) zu pruefen.
- **A3 (Doku, re15_affen.h Kopf (3)):** der Kopfkommentar sagt weiterhin "Luft-/Sturz-Spuren (1/2) bleiben byte-true",
  waehrend der Code seit 30484afa auch in Spur 1/2 zaehlt (Funktionskommentar weiter unten sagt das Gegenteil). Eine Zeile
  korrigieren.

### Hinweise (keine Maengel)
- H1 (Auslegung Punkt 6): Der Selektor-Fernsprung (sub 4 -> sub 7, Path B @0x80118028-8100) kommt ohne Treffer (n3
  F1380, n5 F1395, n8 F1312), wie im Original. Der Wortlaut "nicht nach jeden Schuss" zielt auf den Vergeltungssprung;
  wenn der Nutzer "erst springen, wenn 3x getroffen" woertlich fuer JEDEN Sprung meint, waere auch dieser zu gaten.
- H2: `re15_maggot_a780` (enemy_ai_common.c:8707, vor Runde 35) rechnet `(e->rot_y - pl->rot_y + 0x400)`, das Original
  `(pl.rot_y - e.rot_y + 0x400)` (@0x8001a788-a4) — gleich bis auf die Grenzwerte Differenz 0x400/0xc00. Der Griff-Fix
  haengt jetzt daran; re15_affen_biss_clip hat die richtige Reihenfolge.
- H3: Dossier und Abnahme 0 nennen Item 7 "Schrotflinte"; Item 7 ist die SUPER REDHAWK (inventory_common.c:401), die
  Schrotflinte ist Item 8 (M870). Die M870 ist hier zusaetzlich gemessen (n8: 3,3,7,3 / 3,3,7,3) — Ergebnis haelt.

## 6. Ergebnis
| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (Tuerweg, echte Kills, Bilder n4/n5) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (Bildvergleich mit r3, Riegel `wagen` pinnt die Ursache) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (Zoom-Vergleich mit r3, Bewegungsbilder) |
| 4 KI zielstrebiger/aggressiver wie im Original | teilweise (Regeln belegt gleich; A1 anderer Takt im selben Szenario, A2 Wurf-Bahn) |
| 5 Brust-schlagen-Animation | erfuellt (3x in ~35 s im Freien, Bild) |
| 6 Sprung erst nach 3 Treffern | erfuellt (Spur 0 und 1 in der exe, Spur 2 im Riegel) |

Gates: Bau gruen; Suite 493/493 (selbst gefahren); r35-Riegel 17/17; @0x-Gate gehalten (3 Stichproben bestaetigt, 0 Guess-Tells);
Pfad-/Vertrags-Gate gehalten (M5 behoben). **Abnahme 1: NICHT BESTANDEN** (Punkt 4 teilweise; Maengel A1-A3).
Messlaeufe/Bilder: `scratchpad/jabn1/` (n2..n10, *.png, hits.py, p6.py, griffe.py, grifftrace.py, g_griff_dec_mine.txt).
