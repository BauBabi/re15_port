# Runde 36 — Spur C "schuss": Abnahme 3 (unabhaengig, 2026-10-11)

Baum `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, geprueft am Commit 5d30fd64 (Nachbesserung 3 nach Abnahme 2,
`C_abnahme_2.md` §6 M1). Nur gelesen/gemessen, kein Code im Baum geaendert. Eigene Sonden/Skripte/Logs im Scratchpad
dieser Abnahme (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn3C_r36/`
(Kurzform `abn3C/...`).

(Bericht abgeschlossen 2026-10-11. Ergebnis §8: bestanden = true — alle drei Punkte erfuellt, Gates halten; Empfehlung §7 B1.)

## 0. Gelesen / gebaut
* AUFTRAG.md (Wortlaut der drei Punkte), VERTRAG.md (Spur C, §1.4, §2), Dossier C_schuss.md §5 (OFFEN), §8.8 (Zensus),
  §9.7, §10 (Nachbesserung 3), C_abnahme_2.md (M1), `git log master..HEAD` (48 Commits; Basis 78a4a748, master steht auf
  836023f5 = ein Doku-Commit weiter), `git diff master --stat` (112 Dateien, +7589/-146; gegen die Basis 111 Dateien), den
  Code-Diff `git diff master -- re15_port/engine re15_port/include` vollstaendig und `git diff 4adb4890 HEAD -- re15_port`
  (Nachbesserung 3) gelesen.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
  (`abn3C/configure.log`, `abn3C/build.log`). re15_pc.exe / libre15_engine.a 2026-10-11 00:02:32 / 00:02:30;
  `git status --short re15_port` leer (nur `mess_r36c/` unversioniert, ausserhalb re15_port).

## 1. Punkt 2 — "Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht, wenn sie getroffen werden"

### 1.1 Code der Nachbesserung 3 gelesen (`git diff 4adb4890 HEAD -- re15_port`)
* enemy_ai_re2_zombie.c, Kriecher-Zweig P5 (Leon-Weiche): Kopfabriss jetzt `re2z_gore_fx_a3(e, 0, 0x08001000u, rot_y, {0,-600,0})`,
  Hals-Stumpf `re2z_gore_fx_a3(e, 8, 4096u, ang, {0,100,0})` (vorher `re2z_gore_fx_ex(..., 0x00801000u, ..., 0,-600,0)` bzw.
  `(..., 0,100,0)` = Welt-Versatz). Dazu je 1 Haken-Zeile `re15_schuss_r36_fx_nach(e)` hinter dem ESP-Spawn in
  re2z_blood_fx_scaled / re2z_gore_fx_core und `re15_schuss_r36_fx_id(packed)` (Mess-Ring).
* enemy_ai_common.c (+3 Zeilen): include, `re15_schuss_r36_nachfuehr_marke(e)` direkt nach `re15_enemy_ai_live_step(s)` im
  Zombie-Zweig (Typen 0x10/0x11/0x16/0x12/0x18) von run_all, `re15_schuss_r36_nachfuehren_alle()` als letzte Zeile von run_all.
* schuss_r36.c: `fx_merke` schnappt die belegten ESP-Plaetze unmittelbar vor dem Spawn, `fx_nach` merkt {Aktor, Bild, Lage, NEUE
  Plaetze, Ring-Nr., eigene Matrix?}, `nachfuehr_marke` setzt die Bezugslage auf das Handler-Ende, `nachfuehren_alle` verschiebt am
  Ende von run_all die neuen Plaetze (x/z) und den Ring um die Schwanz-Differenz (Schub + SCA-Klemme), nicht bei Ankern mit eigener
  Matrix. run_all (Zeilen 14460-14845) hat keinen vorzeitigen `return`; Eintraege anderer Bilder werden ueber den Bild-Vergleich
  uebersprungen. Puffer NF_MAX = RE15_ESP_FX_MAX = 96 (PORT-PUFFER = Groesse des ESP-Pools, `addiu t2,zero,96` @0x8001BF10 selbst
  gelesen). r36/zombieki setzt `re15_r36_wurf_klemme` hinter re15_enemy_sca_clamp in denselben Zweig — liegt vor dem Ende von
  run_all, wird also mit nachgefuehrt (`git diff master r36/zombieki` gelesen).
* Selbst disassembliert (§4): Id/a3/Anker beider Emitter und die Belege der Nachfuehrung (FUN_80015cb8 absolute Lage,
  RE2-Root-Schwanz `jal 0x8003567c`).

### 1.2 Eigene Sonde: 320 Laeufe, woertlicher Zeichner-Zwilling (Sonde der Abnahme 2, unveraendert)
`abn3C/abn3_blut.c` = Byte-Kopie von `abn2C/abn2_blut.c` (md5 acd54452...), mit `abn3C/kompiliere.sh` (Flags aus build.ninja) gegen
die AKTUELLE `engine/libre15_engine.a` (00:02:30) + `tests/libre15_test_support.a` gelinkt; Matrix wie Abnahme 2: Waffen
3/4/5/6/7/8/12/13/19/20 x Seite px/pz/nx/nz x eben/tief x Platz 2..5 = 320 Laeufe (`abn3C/batch_blut.sh`, stderr getrennt in
`*.err`, dadurch keine zerschnittenen Zeilen). Klassierung `abn3C/klassen_abn3.py` (eigene) -> `abn3C/klassen_abn3_neu.txt`,
dieselbe Klassierung auf die VORHER-Laeufe der Abnahme 2 (`abn2C/blut`, Stand d5c0c14f) -> `abn3C/klassen_abn3_alt.txt`,
Klassierung der Abnahme 2 auf die neuen Laeufe -> `abn3C/klassen_abn2skript_neu.txt`:

| Klasse | VORHER (Abnahme-2-Laeufe) | NACHHER (dieser Stand) |
|---|---|---|
| K1 Kopfabriss @0x80102B24, Part 0 | 25 Spawns ohne a3 (Welt), d 600 (+ Rest in "part 0 ohne" bis d 709; 10 Zeilen zerschnitten) | **32 Spawns in 32 Laeufen, a3 (0,-600,0), d(gezeichnet T) 0..0** |
| K2 Hals-Stumpf @0x80102BBC, Part 8 | 286 Spawns ohne a3, d 100 | **288 Spawns in 32 Laeufen, a3 (0,100,0), d 0..0** |
| "- part 0 ohne" (darin die alten Rest-Kopfabrisse) | 195, max d 709 | 192, max d 0 |
| Loco-Bank-Treffer w3..w6 TREF part 0/1 bank 1 (Klassierung der Abnahme 2) | bis 30 | **0** |
| Aufschlag beim Zeichnen | 112, d 0 | 112, d 0 |
| TREF part 0 / part 1 a3 (alle Waffen) | max 103 / 97 | max 103 / 97 unveraendert (MG-Trefferbild: Schub nach dem Spawn @0x8010810C-6C, wie Original) |
| uebrige Klassen (TOD, Tropf, RIP, Arm, Glied, Burst) | — | gleiche Maxima wie vorher (Kopf fliegt ab 361, Glied fliegt ab 144-185, Arm 10, AUF Part 8 42-43) |
Summe 4808 Spawns, 320 ERGEBNIS-Zeilen, 0 nicht zerlegbare Zeilen. Lauf-Folgen: Todesbild und Spawn-Zahl in allen 320
ERGEBNIS-Zeilen gleich wie in den VORHER-Laeufen (Vergleich je Lauf, 0 Abweichungen). Die Zahlen des Bau-Agenten (§10.6: K1 32/32 d 0, K2 d 0, Loco
30 -> 0) sind damit unabhaengig bestaetigt (K2 288 statt 282, weil seine Laeufe stderr in die Ausgabe mischten). Woertlich
(`abn3C/blut/w19_pz_eben_p5.txt`, Klemm-Fall): `F266 ... part  0 fx=(733,-117,-17406) a3=(0,-600,0) | d(gezeichnet T)=0 |
d(T-1)=25` (vorher `abn2C/blut/w19_pz_eben_p5.txt`: `ERGEBNIS ... max d(T) 896`).

### 1.3 Wirken Emitter und Nachfuehrung auf die ECHTEN ESP-Plaetze? (eigene Zusatz-Sonde)
Die Sonden messen den Mess-Ring, nicht die gezeichneten Effekt-Plaetze. Deshalb `abn3C/abn3_nachf.c` = abn3_blut.c + (a) ESP-Baenke
wie die exe (Raum-ESP aus ROOM1140.RDT +0x4C..+0x58 via re15_esp_parse, global DATA/CORE00.ESP; Muster main.c pc_load_room_esp;
Schalter ABN3_ESPBANK) + (b) Abgleich: jeder im Bild NEU belegte Slot mit Raum-Id 0/5/7 muss exakt auf einer Ring-Lage desselben
Bilds liegen. Mit Baenken bleiben die Laeufe bitgleich (z.B. `3 nx eben 4`: Tod F378, 21 Spawns, max d 42 — wie ohne).
* Alle 320 Laeufe mit Baenken (`abn3C/blut_esp/`): **16886 neue Blut/Gore-Slots, 16838 exakt auf einer Ring-Lage des Spielschritts,
  die uebrigen 48 exakt auf einer Ring-Lage des Zeichnens (Aufschlag-Blut fliegender Glieder, `Z`-Zeilen, z.B. w12 nx eben 2 F88
  `anker (-1944,0,-19156)` = `Z ... part 10 fliegt fx=(-1944,0,-19156)`) -> 0 Abweichungen.**
* Wand-Fall `3 nx eben 4` F102 (Taumel-Treffer, SCA-Klemme im Schwanz; vorher d 25): Ring `fx=(-715,-2269,-21478)` d 0, die fuenf
  neuen Blut-Slots 91..95 `id 0 sub 0 anker (-715,-2269,-21478)` sind mit verschoben; die gleichzeitig neuen Muendungs-/Rauch-/
  Huelsen-Slots id 2/3/4 bleiben unberuehrt.
* Kriecher `12 px eben 2` F192: `ESP-neu F192 slot 14 id 5 sub 0 anker (568,-114,-19589)` = Ring (Raum-Id 5 = Fleischbrocken);
  Hals-Stumpf F193 Slots 15..17 `id 0` bei (547,-159,-19613) = Ring.
* Kriecher-Klemmfall `19 pz eben 5` F266: der ESP-Pool ist dort durch das MC51-Dauerfeuer VOLL (`vorher aktiv 96, nachher aktiv 96`)
  — der Kopfabriss belegt keinen Slot, sichtbar ist dort nichts; verschoben wird nur der Ring (s. §7 B1).

### 1.4 exe (echter Renderer), eigene Messkopie `re15_pc_abn3c.exe`, `abn3C/lauf.sh` + `abn3C/p_batch.sh`
Aufbau E2f der Abnahme 2 (Kriecher-Kopfabriss): `RE15_DEBUG_JUMP=1140@250`, `RE15_PLAYER_POS=-3600,-19600,0`, `RE15_AI_FLAVOR=re2`,
`RE15_GIVE=12:250`, `RE15_EQUIP=12`, `RE15_INPUT_SCRIPT=W1.2,M0.6,MA3.0,M0.3,W16` (BASIS spiel), `RE15_EXIT_AT=700#1140`,
`RE15_FRAMEDUMP=200-260/1`, `RE15_WINDOW_SCALE=1`, fx/state-Log; exit=0.
* state.log F226: Kriecher `[5 t=11 st=1 ss1=1 ss2=5 ... mo=16 af=22 ... @(-2728,-19774,r2199)] hp=226`, Leon
  `PL(-3812,-19450,rot=151,hp=70)`. **Zustandsfolge bitgleich** mit dem VORHER-Lauf der Abnahme 2 (`abn2C/p2/e2f_ingram/state.log`;
  `diff` nach Ausblenden des Feldes `fx=` (Zahl aktiver Effekte) = 0 Zeilen, 951 Bilder).
* fx.log (`frame=1`): **`id=5 sub=0 eidx=0 frame=1 w(-3302,-117,-19659) ... F=226`** (vorher drei Zeilen `id=0 sub=128 ...
  w(-2732,-849,-19781) ... F=226`), Hals-Stumpf **`id=0 sub=0 ... w(-3277,-162,-19642)`** in F227, 229, ..., 243 (vorher
  `w(-3376,-51,-19641)`). Die Brocken liegen 117 ueber dem Boden, 585 von der Kriecher-Wurzel Richtung Leon (Hals/Kopf des liegenden
  Kriechers) statt 849 senkrecht ueber der Wurzel. Alle uebrigen `frame=1`-Zeilen des Laufs unveraendert (diff mit ausgeblendeten
  Feldern `slot=`/`q=`; Unterschiede nur F226..F243 an diesen zwei Emittern).
* Bild: `abn3C/p2_e2f_crop_wie_abn2.png` (F222-F233, derselbe Ausschnitt x0-130/y60-160, 4x wie `abn2C/p2_e2f_crop.png`): F222-F225
  Leon tritt zu; ab F226 rote Brocken/Spritzer UNTEN am Boden bei Leons Fuss = am Kopf des liegenden Kriechers; der rote Ballen in
  der Luft auf Kniehoehe (Abnahme-2-Bild F226-F232) ist weg. Gesamtbild `abn3C/p2_e2f_full.png`.
* E1 Ingram-Treffer (Aufbau des Bau-Agenten: `-5000,-19600,0`, `W1.2,M0.6,MA9.0,M0.5,W2`, Exit 300): fx.log (`frame=1`-Zeilen ohne
  `slot=`/`q=`, je 10816 Zeilen) und state.log **gleich** dem VORHER-Lauf der Abnahme 2 (`abn2C/p2/e1_ingram`) — die Nachfuehrung
  veraendert gewoehnliche Treffer im freien Raum nicht.
* E3 Pistole (Browning, derselbe Aufbau): Treffer F55/F77/F99, Tod F121 (hp 50->34->18->2->-14), Blut `id=0` je Treffer auf
  Brusthoehe am Zombie (`F55 w(-1781,-2163,-19518)`; `F77 w(-1535,-2144,-19756)` bei Wurzel (-1644,-19665); `F99 w(-1731,-2147,-20309)`
  bei Wurzel (-1767,-20277); `F121 w(-1862,-2176,-20446)`), Tropfblut F126-F136 sinkend (y -1934 -> -1199). Bild `abn3C/p2_e3_f76.png`
  (F76-F81: Blut am Oberkoerper des Zombies am linken Bildrand).

### 1.5 Urteil Punkt 2
**erfuellt.** Der einzige offene Mangel der Abnahme 2 (M1 Kriecher-Kopfabriss, Welt- statt Anker-Versatz) ist behoben und
unabhaengig bestaetigt: Sonde 320 Laeufe (K1 32/32 und K2 288/288 an der gezeichneten Matrix), echte ESP-Slots (16886/16886 auf ihrer
Ring-Lage), exe F226 (Brocken am Hals am Boden statt Ballen in der Luft, Zustandsfolge bitgleich). Die Id-Berichtigung des
Bau-Agenten (0x08001000 = Id 8, nicht 0x00801000) ist richtig (§4, `srl t6,a0,24` @0x8001BF14). Kein gemessener Blut-Emitter am
getroffenen Zombie liegt mehr neben dem gezeichneten Koerper, ausser den mechanisch wie im Original begruendeten Klassen (Schub nach
dem Spawn, Glied fliegt im Spawn-Bild ab).

## 2. Punkt 1 — "Alle Schusswaffen treffen ueber den Konferenztisch und treffen fressende Zombies. Im Original geht beides nicht."
Code des Punkts seit Abnahme 2 unveraendert (`git diff 4adb4890 HEAD` beruehrt re15_damage.c, enemy_ai_re2_crow.c, re15_actor.h,
test_r36_schuss*.c nicht; enemy_ai_common.c nur die zwei Nachfuehr-Haken in run_all, die LOS-Funktion nicht). Trotzdem selbst an der
exe dieses Stands gemessen (Messkopie `re15_pc_abn3c.exe`, Aufbau der Abnahmen 1/2: `RE15_DEBUG_JUMP=1140@250`, `RE15_PLAYER_POS`,
`RE15_AI_FLAVOR`, `RE15_GIVE=<w>:6`, `RE15_EQUIP=<w>`, `RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4`, `RE15_EXIT_AT=200#1140`;
Auswertung `abn3C/hp.py` = HP je Platz/Munition aus state.log/wf.log). Alle Laeufe exit=0:
| Aufstellung | Waffen | Ergebnis (dieser Stand) |
|---|---|---|
| tisch_re15: Leon (200,-10300) Blick -z, RE1.5-KI (dort ist auch der Fresser treffbar) | 3,4,5,6,7,8,12,13,19,20,14,15,16,17,18 | alle 15: Munition faellt (w3 6->3, w5 6->0, w7 6->4, w12 6->0, w14 6->0, w15..18 6->4), **kein Platz verliert HP** (P1 65, P2 75, P3 65, P4 250, P5 250) |
| diag_re15: Leon (6000,-10300) rot 1387 (Diagonale durch die Tischzelle) | 3, 8, 12, 20 | kein HP-Verlust |
| Kontrolle frei_re15: Leon (200,-18200), RE1.5-KI | 3, 8, 12, 20 | Treffer: w3 P5 250->218 (F19); w8 P5/P4 -> -1 (F20/F23); w12 P5/P4 -> 242; w20 P5/P4 -> -1 (F19/F44) |
| fresser_tuer: Leon am Tuerpunkt (-7600,-17600) Blick +x, RE2-KI | alle 15 | alle 15: Munition faellt, **kein Platz verliert HP** (P2..P5 st1/ss1 8 = EXEC[8], P1 st1/7) |
| Kontrolle weck_re2: Leon (200,-18200), Fresser geweckt | 3, 8, 12, 20 | Treffer: w3 P5 -> 202; w8 P2/P3/P4/P5 -> -1/-120/-150/-1; w12 P5 -> 226; w20 P5/P4 -> -1 |
Alle Zahlen bitgleich mit Abnahme 1 (§4.1) und Abnahme 2 (§2). Die Nachbesserung 3 hat den Punkt nicht beruehrt.
Trennschaerfe der Riegel (gelesen): unit_r36_schuss prueft neben dem Tisch / ohne SCA / Band 1 / Zombie vor dem Tisch jeweils "Treffer"
und hinter dem Tisch / Fresser "kein Treffer" fuer alle zehn Hitscan-Waffen; integration_r36_schuss faehrt die exe (s. §5).
Echter Weg: Abnahme 1 hat ROOM10D0 -> Tuer -> ROOM10E0 gegen den Sprung verglichen (gleicher Gegnerzustand); der Mechanismus
(Spieler-Resolver) haengt nicht davon ab, wie der Raum betreten wurde — hier nicht erneut gefahren.
**Urteil Punkt 1: erfuellt.**

## 3. Punkt 3 — "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in Resident Evil 2 auch so? (belegen; wenn RE2 anders ist: RE2-Verhalten bauen)"
* Beleg (selbst gelesen, §4): RE2 EMOVL21_S0.BIN, HURT-Zeilen-Dispatch @0x80104A18 auf +0x5 (RE2-Waffen-Id): Zeile 5 (Magnum) und 6
  (Custom Magnum), 9/10/11 (GL), 17 (Rakete) -> 0x80102CA0 = GIB; der GIB wirft/wirbelt die 13 Teile und laesst sie verschwinden,
  danach Zustand (7,1) GIB-Leiche ohne Lache (Dossier §3.2, Abnahme 1 gelesen). Antwort: **ja, in RE2 zerplatzt die Kraehe beim
  Magnum-Treffer ebenfalls und ist danach komplett weg.** Der Port zeigt jetzt das RE2-Zerplatzen (13 Teile ueber die Part-Bruecke)
  statt des sofortigen Ausblendens.
* exe dieses Stands (`RE15_DEBUG_JUMP=10C0@250`, `RE15_PLAYER_POS=3522,5556,581`, `RE15_FORCE_CUT=1`, RE2-KI,
  `RE15_INPUT_SCRIPT=MD0.6,MDA0.4,MD1.0,W3`, `RE15_FRAMEDUMP=14-70/1`, `RE15_WINDOW_SCALE=1`, `RE15_EXIT_AT=110#10C0`), Platz 3:
  * w7 Super Redhawk: `F7 st=1 ss1=0 ss2=7 hp=10` -> **`F19 st=7 ss1=1 ss2=0 hp=-1`** (GIB-Leiche, Zeile [1] der Leichen-Tabelle).
    Bild `abn3C/p3_w7_crop.png` (F17-F40, x100-300/y170-240, 2x) und `abn3C/p3_w7_zoom.png` (F18-F23, 5x): F18 Kraehe sitzt mit
    Schatten, F19 Treffer (rote/dunkle Teile am Koerper), F20-F28 Teile und Spritzer fliegen auseinander, F29-F31 verblassend, ab F32
    nichts mehr — kein Kadaver, keine Lache.
  * Gegenprobe w3 Browning: `F19 st=3 ss1=3 ss2=5 hp=-5` -> `F31 st=7 ss1=0` (normale Leiche); Bild `abn3C/p3_w3_crop.png`.
  * w20 Colt Python: `F19 st=3 ss1=20 ss2=5 hp=-190` -> `F31 st=7 ss1=0` (normale Leiche, kein GIB; nicht Teil des Wortlauts, §7 B4).
  Zustandsfolgen bitgleich mit Abnahme 2 §3.
**Urteil Punkt 3: erfuellt** (Frage beantwortet und am RE2-Dispatch selbst belegt; der Port zeigt das RE2-Verhalten).

## 4. RE-Gate (selbst disassembliert mit dem Hauptbaum-Skill `re2_disasm.py` / `re15_disasm.py`)
| Stelle (Binaerdatei) | Behauptung (Commit/Dossier) | Selbst gelesen | Urteil |
|---|---|---|---|
| EMZ0.BIN 0x80102AD4-B28 | Kopfabriss: a0 = 0x08001000, a3 = Zeiger auf (0,-600,0), a2 = Part 0 | `lui a0,0x800` / `ori a0,a0,0x1000` @0x80102AD4/ADC; `addiu a3,sp,24` @0x80102AEC; `addiu v0,zero,-600` / `sh zero,24(sp)` / `sh v0,26(sp)` / `sh zero,28(sp)` @0x80102B10-1C; `lh a1,118(s1)` @0x80102B20; `jal 0x8001bf10` / `addiu a2,a2,72` (a2 = `lw a2,408(s1)`) @0x80102B24-28 | wie behauptet |
| EMZ0.BIN 0x80102B8C-BC0 | Hals-Stumpf: a0 = 4096, a3 = (0,100,0), a2 = Part 8 | `addiu v0,zero,100` @0x80102B8C; `sh zero,24` / `sh v0,26` / `sh zero,28(sp)` @0x80102B94-A0 (letzte im Delay-Slot von `jal 0x80015fe8`); `addiu a0,zero,4096` / `addiu a3,sp,24` @0x80102BA4-A8; `jal 0x8001bf10` / `addiu a2,s0,1448` @0x80102BBC-C0 | wie behauptet |
| RE2 PSX.EXE 0x8001BF10-C00C | Id = a0>>24, Sub = (a0>>16)&0xff, Skala = a0&0xffff; a3 = Zeiger (gelesen); Pool 96 | `addiu t2,zero,96` @0x8001BF10 (Slot-Suche, 96 Plaetze a 124 Byte); `srl t6,a0,24` @0x8001BF14; `srl v0,a0,16` / `andi t5,v0,0xff` @0x8001BF1C-20; `beq a3,zero,0x8001c000` / `lw v0,0(a3)` / `sw v0,44(t0)` / `lw v0,4(a3)` / `sw v0,48(t0)` @0x8001BFE0-FC; `sll v0,a0,16` / `sw v0,56(t0)` @0x8001C00C-10 | wie behauptet: 0x08001000 = Id 8 / Sub 0 / Skala 0x1000; die Lesart der Abnahme 2 (0x00801000 = Id 0) war falsch |
| RE2 PSX.EXE 0x80015CB8-D68 | Griff setzt die Lage ABSOLUT (+0x38 = +0x164 + R*Wurzelversatz) | `jal 0x80015db0` @0x80015CE8; `lh a0,118(s1)` / `jal 0x8008e8b4` @0x80015D3C-40; `jal 0x8008dba4` @0x80015D50; `lh v0,356(s1)` / `lh v1,48(sp)` / `addu` / `sw v0,56(s1)` @0x80015D58-68 | wie behauptet |
| EMZ0.BIN 0x801004E8 / 0x8010062C-3C | Root: Zustands-Handler, dann Schwanz mit SCA-Klemme | `jalr v0` @0x801004E8 (Tabelle @0x8010C830); `jal 0x80035530` @0x8010062C; `jal 0x8003567c` / `addiu a1,zero,1024` @0x80100638-3C | wie behauptet |
| RE2 PSX.EXE 0x8003567C-D0 | Klemme ueberspringt nur bei word0 & 8 oder +0x1E4 != 0 | `andi v0,v1,0x8` / `bne` @0x80035694-98; `lw v0,484(s0)` / `bne` @0x800356A0-A8; `jal 0x8004c1bc` @0x800356D0 | wie behauptet |
| RE1.5 PSX.EXE 0x80012160-74 (Punkt 1) | Sichtlinie auf den Gewinner vor Typ-Pruefung/Latch | `beq s1,zero,0x8001253c` / `addiu s0,s1,52` / `jal 0x8001b9b4` / `addu a0,s0,zero` / `bne v0,zero,0x80012540` / `addu v0,zero,zero` | wie behauptet |
| RE1.5 PSX.EXE 0x8001BA08-38 (Punkt 1) | Regionen 3..0, Klasse 0x300, ODER | `ori s0,zero,0x3`; `ori a2,zero,0xf00` / `jal 0x8003dcc4` / `ori a3,zero,0x300` @0x8001BA1C-24; `or s1,s1,v0` @0x8001BA28; `bne v0,zero,0x8001ba14` / `addiu s0,s0,-1` @0x8001BA34-38 | wie behauptet |
| RE2 PSX.EXE 0x80041270-9C (Punkt 1, Fresser) | Kandidat raus bei +0x1D3 != 0 bzw. +0x10E & 0xC000 | `lbu v0,467(s2)` / `bne v0,zero,0x80041300` @0x80041270-78; `lhu v0,270(s2)` / `andi v0,v0,0xc000` / `bne` @0x80041290-9C | wie behauptet |
| EMOVL21_S0.BIN Tabelle 0x80104A18 (Punkt 3) | Zeilen 5/6/9/10/11/17 -> GIB 0x80102CA0 | `table 0x80104a18 18`: [5] [6] [9] [10] [11] [17] -> 0x80102CA0; [0..4] [13..16] -> 0x80102934; [7] [8] -> 0x8010327C; [12] -> 0x80102FB4 | wie behauptet |
Ergebnis: alle 10 Stichproben (davon 6 zur Nachbesserung 3) enthalten genau das Behauptete, jeweils in der richtigen Binaerdatei
(RE2 EMZ0.BIN fuer den Zombie, RE2 PSX.EXE fuer FUN_8001BF10/FUN_80015CB8/FUN_8003567C, RE1.5 PSX.EXE fuer den Spieler-Schuss).
* Rate-Woerter in den hinzugefuegten re15_port-Zeilen (`git diff master -- re15_port`, deferred/tunable/interim/for now/faithful/
  plausib/TODO/approx/Platzhalter): **keine**. Neue getenv: nur `RE15_ASSET_DIR` in der Sonde probe_r36_blut.c.
* Zahlen im Code-Diff ohne `@0x`/PORT im Umfeld (eigenes Skript ueber alle `+`-Zeilen in engine/include): 5 Treffer, alle belegt
  oder ohne Verhaltenswirkung: 800 und `100 * seite` (re2z_blood_a3, Belege im Funktionskopf @0x801056DC-F4 / @0x80105680-B4),
  `0x1000` (Einheitsmatrix Q12), `0xFFFFFFFF` (Ring-Marke "keine Id"), `RE15_SCHUSS_R36_FX_RING 64` (Mess-Ring nur fuer Sonden).
  Nachbesserung 3 fuehrt nur -600/100/0x08001000/4096 (zitiert) und NF_MAX = 96 (PORT-PUFFER, Groesse des Pools) ein.
* Erklaert der Fix den Befund? Ja: Welt-Versatz (0,-600,0) zeigte am liegenden Kriecher senkrecht nach oben (Abnahme 2: 849 ueber der
  Wurzel), a3 im Anker-Rahmen zeigt die Wirbelsaeule entlang zum Hals; gemessen jetzt d 0 und exe am Boden (§1). Die Rest-Abstaende
  41..666 bei 5 Kopfabrissen erklaert die absolute Lage-Setzung des Griffs + Klemme im Schwanz (selbst gelesen, s. Tabelle);
  die Nachfuehrung beseitigt genau diese Klasse (Wand-Fall F102 25 -> 0, Klemm-Fall F266 896 -> 0).

## 5. Vertrag / Tests
* Pfade (`git diff master --name-only` ohne analysis/): engine/src enemy_ai_common.c, enemy_ai_re2_crow.c, enemy_ai_re2_zombie.c,
  re15_damage.c, schuss_r36.c (neu); include re15_actor.h, re15_schuss_r36.h (neu); tests integration/test_r36_schuss.cmake,
  unit/probes/r36_schuss.cmake, probe_r36_blut.c, test_r36_schuss(_kraehe).c + 7 angepasste Riegel/Sonden. Keine Treffer fuer
  release/, platform/android/, shared_assets/PSX/, RELEASE_NOTES, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt.
* Gemeinsame Dateien nur mit Haken: enemy_ai_common.c +9/-2 (LOS-Export, include, marke, nachfuehren_alle), re15_damage.c +4,
  re15_actor.h +5 (ein Feld; kein Speicherformat betroffen — re15_savedata.c liest/schreibt nur Einzelfelder des Spielers);
  enemy_ai_re2_zombie.c ist laut VERTRAG §1.4 F-Datei mit C-Eigentum "Blut-Anker" — die Aenderungen sind Blut-Emitter, die
  Fresser-Ausnahme im Trefferfilter (Punkt 1) und 2 Zeilen Part-Bruecke (Punkt 3).
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine (`re15_game_flag_set` / `re15_msg_install_text` / `scd_event_fire` in keiner
  `+`-Zeile). Neue getenv nur `RE15_ASSET_DIR` in der Sonde.
* `git merge-tree --write-tree --name-only HEAD <zweig>` gegen alle 17 Zweige r36/* (Stand jetzt: elza 83ba5984, gegnerzahl b235e6a9,
  granate a25cbdcc, integration 916e6fe2, kleidung eb487806, klettern d9f5f7cb, schluessel dc8992df, story d35fcb1f, szenenleon
  bd6e96c2, zombieki b7687fb1, die uebrigen auf 78a4a748): alle rc=0, 0 CONFLICT.
* Tests je Punkt vorhanden und messend: Punkt 1 unit_r36_schuss (Tisch/Band/ohne SCA/vor dem Tisch, Fresser je Gate) +
  integration_r36_schuss (exe); Punkt 2 unit_r36_blut_pin_{eben,tief,mg,rip,arm,burst,glied,kriecher,kriecher5}; Punkt 3
  unit_r36_schuss_kraehe. **Trennschaerfe der neuen Pins selbst nachgefahren** (`abn3C/trenn/`): probe_r36_blut.c.obj dieses Stands
  gelinkt (a) mit enemy_ai_re2_zombie.c + enemy_ai_common.c aus 4adb4890 (vor N3): `pin kriecher` rc 1, 12 FAIL (`F192 Spawn part=0
  ohne a3 liegt 600`, Hals-Stumpf 100), `pin kriecher5` rc 1, 12 FAIL (`F266 ... liegt 896`); (b) nur enemy_ai_common.c aus 4adb4890
  (ohne Nachfuehrung): kriecher rc 0, **kriecher5 rc 1 (`F266 Spawn part=0 mit a3 liegt 666`)**; (c) dieser Stand: beide `OK`.
* Gezielter ctest-Lauf im Spurbaum (keine Voll-Suite): Regex = die 219er-Regex des Bau-Agenten + `dog_ai|maggot|npc_ai|tyrant|ivy|
  dog_devour|affen|hund|messer|sca_wall|attack_reach|npc_wall|room_probe_sweep|savedata|savestate_cmp|spinne|entladen|weapon_select`
  (Spieler-Schuss mit Sichtlinie trifft alle Gegnerarten, run_all-Haken laeuft fuer alle Raeume; `abn3C/ctest_regex.txt`):
  `ctest --test-dir re15_port/build -R "<regex>" --timeout 600 --output-on-failure -j 2` ->
  **`100% tests passed, 0 tests failed out of 275`**, `Total Test time (real) = 1576.50 sec` (`abn3C/ctest_gezielt.log`), ohne
  Wiederholungslauf, bei paralleler Last anderer Spurbaeume. Darin integration_r36_schuss (130.75 s), integration_r35_werfer*,
  integration_r35_entladen_a..l, unit_r36_schuss, unit_r36_schuss_kraehe, alle neun unit_r36_blut_pin_*, unit_re2_zombie_teardeath,
  unit_r26_mg_blut, probe_re2z_stomp, unit_crow_death_corpse, unit_re2_crow_ai, unit_re2_zombie_hitfilter/_abc, unit_r35_affen_*.
* Danach gesondert die Spur-Tests: `ctest --test-dir re15_port/build -R "r36_schuss|r36_blut" --timeout 600 --output-on-failure -j 2`
  -> `100% tests passed, 0 tests failed out of 12`, `Total Test time (real) = 131.96 sec`; `-R unit_r36_blut_pin_kriecher -V` zeigt
  die echte Simulation (`F192 KOPFABRISS Id 0x08001000 d=0`, `F193 HALS-STUMPF Id 0x00001000 d=0`, ...).
* Messkopien: eigene `re15_pc_abn3c.exe` nach den Laeufen geloescht; nach dem ctest-Lauf keine `re15_pc_r36c_haken_*`/
  `re15_pc_r35_entladen_*`-Kopie mehr im Ausgabeordner (die `re15_pc_r35e/r35k_haken_*` vom 10.10. 15:44/16:49 sind aelter).

## 6. Maengel
Keine.

## 7. Beobachtungen ohne Maengel-Status (fuer Orchestrator/Nutzer)
* B1 (Test-Luecke, Empfehlung) Kein Pin prueft die Nachfuehrung der ECHTEN ESP-Plaetze: probe_r36_blut laedt keine ESP-Baenke, die
  Pins messen den Mess-Ring (der Ring liegt auf derselben Lage `p`, mit der gespawnt wird, und wird von derselben Schleife
  verschoben). Im Klemmfall des Pins kriecher5 ist der ESP-Pool mit Baenken sogar voll (96/96), der Kopfabriss dort also
  unsichtbar. Mein Abgleich (§1.3: 16886/16886 Slots auf ihrer Ring-Lage, Wand-Fall F102 Slots 91..95 mitgeschoben) belegt das
  Verhalten dieses Stands; als Riegel waere das Muster `abn3C/abn3_nachf.c` (Baenke laden, neue Slots gegen den Ring) geeignet.
* B2 OFFEN 10 des Bau-Agenten (16 Emitter ab 0x8010A1EC "nicht bestimmt") selbst eingeordnet (EMZ0.BIN, `table 0x8010cf18` /
  `dis`): 11 liegen im RE2-Zustand 8 (Sub-Tabelle @0x8010CF18: Sub 3 0x8010A05C = @0x8010A1EC; Sub 4 0x8010A240 = @0x8010A318/33C/354;
  Sub 5/6 0x8010A9B8 = @0x8010AC44/D70; Sub 7/8 0x8010AF58 = @0x8010B188/244; Sub 9/10 0x8010B464 = @0x8010B668/6A8/6D0) — den betritt
  der Port nie (re2z_state8 hat keinen Eingang); 4 (@0x8010C1D0/208/240/278) liegen in 0x8010C0F8, das die Root-Prolog-Stellen
  @0x801005C4/5E8/60C nach Anim-Wort-Flags rufen, mit a2 = Weltmatrix 0x8009DB44 und a0 = 0x1A000000|... (Id 0x1A, kein Blut,
  Schritt-Effekt); 1 (@0x8010C56C, Blut Part 0) liegt im Per-Tick-Automaten 0x8010C2A4 auf +0x23C, den nur der Griff setzt
  (`sb v0,572(s1)` @0x80102A04, Zufall bei Griff-Bild 3) — Blut waehrend des Griffs, nicht beim Treffer; der Port fuehrt +0x23C
  nicht (fehlendes Blut, keine falsche Lage). Keiner betrifft den Wortlaut von Punkt 2.
* B3 OFFEN 2 ist veraltet formuliert: der RE1.5-KI-Pfad spawnt nicht an der Wurzel, sondern ueber re15_enemy_hurt_blood am Gore-Bone
  bzw. Part 2 (re15_damage.c:3292-3305, auch fuer die Zombie-Girl 0x13 = Bone 8); es fehlt nur der a3-Versatz (200,0,0) @0x8011FE84.
  Betrifft nur RE15_AI_FLAVOR=re15 (nicht Standard), nicht gemessen.
* B4 Kraehe + Colt Python (Abnahme 1 B2 / Abnahme 2 B4) unveraendert: kein GIB (`st=3 ss1=20`), nicht Teil des Wortlauts.
* B5 PORT-KONVENTION "laufendes Bild" (Abnahme 1/2 B1) jetzt konsequent: das Blut sitzt an der gezeichneten Matrix des Bilds T
  NACH Schub/Klemme; das Original ankert an der Zeichnung T-1 (d(T-1) bis 716 bei Taumel-Treffern, weil die Port-Pose selbst springt —
  Stagger-Wurzelbewegung OFFEN 7, Spur F).
* B6 Hinweis aus der Sitzung: die claude.ai-Connectoren Gmail, Google Calendar und Google Drive sind nicht autorisiert (Autorisierung
  in den claude.ai-Connector-Einstellungen noetig); fuer diese Spur ohne Belang.

## 8. Ergebnis
| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Tisch + Fresser, alle Schusswaffen | **erfuellt** | exe: 15 Waffen ueber den Tisch (RE1.5-KI) + Diagonale ohne Treffer, Kontrolle frei mit Treffern; 15 Waffen gegen Fresser (RE2-KI) ohne Treffer, geweckte treffbar (§2) |
| 2 Blut an der richtigen Stelle | **erfuellt** | M1 der Abnahme 2 behoben und unabhaengig bestaetigt: Sonde 320 Laeufe (Kopfabriss 32/32, Hals-Stumpf 288/288 d 0, Loco 30 -> 0, Lauf-Folgen gleich), echte ESP-Slots 16886/16886 auf ihrer Lage, exe F226 Brocken am Hals am Boden statt Ballen in der Luft, Zustandsfolge bitgleich (§1) |
| 3 Redhawk/Kraehe wie RE2 | **erfuellt** | Antwort am RE2-Dispatch selbst belegt, exe zeigt das RE2-Zerplatzen (§3) |
RE-Gate: haelt (10 Stichproben selbst disassembliert, davon 6 zur Nachbesserung 3; keine Rate-Woerter; jede neue Zahl zitiert oder
PORT-PUFFER). Vertrag: haelt (Pfade, keine Bits/IDs/Ereignisse, merge-tree gegen 17 Zweige konfliktfrei). Tests: 275/275 gezielt,
neue Pins trennscharf (selbst gegen alte Objekte gelinkt).
**bestanden = true** (Empfehlung B1 fuer einen spaeteren Riegel).
