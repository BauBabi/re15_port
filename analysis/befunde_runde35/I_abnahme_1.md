# Runde 35 — Spur I "entladen": UNABHAENGIGE ABNAHME 1 (nach Nachbesserung 1)

Baum `.claude/worktrees/r35_entladen`, Zweig `r35/entladen`, Stand `85ab539d` (Code = `86f65b95`,
`git diff 86f65b95 HEAD` aendert nur das Dossier). Basis master `154a73c1` (merge-base; `git diff master`
gegen den heutigen master `87cc8575` zeigt fremde Spuren als "Loeschungen" — gewertet wurde
`git diff master...HEAD`). Abnahme 2026-10-04. Alles unten ist am gebauten Stand SELBST gemessen
(eigene exe-Kopie `re15_pc_abn_i1.exe`, eigenes Arbeitsverzeichnis im Scratchpad, PATH msys64 zuerst,
kein taskkill). Die Messschiene des Bau-Agenten (`RE15_ENTLADEN_LOG`) ist nur EIN Messweg; dazu
unabhaengig: Bytevergleich gegen frische Prozesse, A/B gegen eine selbst gebaute exe des Stands VOR N1,
und RAM-Lesen mit gdb (Symbole der exe), um zu sehen, was der Zensus NICHT zaehlt.

## Ergebnis

| Punkt (Wortlaut AUFTRAG.md Z. 24) | Urteil |
|---|---|
| 1. "Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen Spielstand davor ... angezeigt" | **erfuellt** |
| 2. "Wenn man tot ist, aber auch wenn man den Raum wechselt sollen saemtliche Assets von den Raeumen davor entladen sein" | **teilweise** (Mangel 1) |

Die beiden Maengel der Abnahme 0 (Raum-Stimmen, Elliot/Heli/Pilot) sind behoben und nachgemessen. Neu
gemessen: die Cinematic-Bank ROOM1170 (55 060 Bytes) bleibt nach dem Raumwechsel und ueber den Tod geladen;
der Zensus hat dafuer kein Fach und meldet "0".

Gates: Suite gruen (486/486, Log im eigenen build/, Zeitstempel nach dem exe-Bau; `r35_entladen` 8/8 selbst
gefahren) · @0x-Gate haelt (12 Adressen selbst disassembliert, alle stimmen) · Pfad-/Vertrags-Gate haelt
(main.c-Umfang als Abweichung im Dossier benannt) · Tests vorhanden und messend.
**bestanden = false** (Punkt 2 nur teilweise).

## Bau und Tests

```
cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build
  -> configure OK / ninja: no work to do / === LOCAL-BUILD-OK (build)
     exe 2026-10-04 01:43:15 (main.c 01:42:54, elliot_pc.c 01:28:32) = Code 86f65b95
Dossier: woertlich "=== LOCAL-BUILD-OK (all) — Tests 486/486" (N >= 478) -> Suite nicht selbst neu gefahren.
  Beleg im Baum: re15_port/build/local_build_ctest.log (02:07:44, nach dem exe-Bau):
  "100% tests passed, 0 tests failed out of 486", Total Test time 1291.62 sec; alle fuenf Flatter-Haken Passed.
ctest --test-dir <baum>/re15_port/build -R r35_entladen --output-on-failure   (selbst)
  -> unit_r35_entladen_beleg / _gegner / _n1beleg Passed; integration_r35_entladen_a 21.21 s, _b 15.78 s,
     _c 38.78 s, _d 45.73 s, _e 27.44 s -> 100% tests passed, 0 tests failed out of 8
```
A/B-exe "vor N1": `git archive 20ced3e7 re15_port` (ohne shared_assets) in den Scratchpad, eigener Bau
(`RE15_TESTS=OFF`, `RE15_ASSETS_PATH`/`RE15_CD_ROOT` auf diesen Baum).

## Punkt 1 — PRIs des alten Spielstands nach Tod + NEW GAME

### Messung r3: Tod -> echter Titelmenue-Weg -> Charakterwahl (jedes Bild)
```
nach : RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_TITLE_CONFIRM_MS=8000
       RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=40 RE15_BOOT_EXIT_AT=2
       RE15_ENTLADEN_LOG=entladen.log RE15_ENTLADEN_SHOT=es RE15_ENTLADEN_SHOT_BILD=300..610
frisch: dieselbe exe ohne Spiel davor (RE15_TITLE_CONFIRM_MS=1 RE15_BOOT_EXIT_AT=1), Bilder 300..610
vergleich.py (SHA1 jedes Bildes gegen ALLE Frisch-Bilder):
  nach gen4 Bilder: 300 | exakt gleich einem Frisch-Bild: 300 | ohne Treffer: 0
entladen.log: VORHER spielende gen=3 raum=1020 | belegt pri_masken=24 pri_atlas=1 sld=1 tim=14 gegner=2 ...
              EREIGNIS spielende gen=4 | belegt (alle 15 Faecher) 0
              SUMME seit=spielende bilder=599 bilder_mit_masken=0 bilder_fremde_masken_gezeichnet=0
```
Bilder b400/b500/b590 angesehen (Scratchpad `abn_i1/n_strip.png`): "PLEASE SELECT MAIN CAST" vollstaendig,
Leon-Zoom auf schwarzem Grund, kein ROOM1020-Maskenstueck (Abnahme 0 vorher: rechts ein grau-weisses
Raumstueck in (191,0)-(319,152)).

### Messung r4: das neue Spiel selbst (ROOM1240 + ROOM1170) nach dem Tod, gegen frisch
```
frisch: RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_EXIT_AT=1100#1170
        RE15_ENTLADEN_SHOT_BILD=100,200,...,1400
nach  : dasselbe + RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=1500 (Spiel 1 stirbt in ROOM1020, Spiel 2 laeuft
        die Montage 1240 -> Tuer -> 1170)
ROOM1240 (gen2 frisch / gen5 nach): 14 Bilder, 13 identisch, 1 verschieden (b100: frisch Mittel 0.8,
        nach 0.0, Maximaldifferenz 1 — eine Blendstufe im fast schwarzen Montage-Einstieg; das Bild nach
        dem Tod ist DUNKLER, nichts Zusaetzliches)
ROOM1170 (gen3 frisch / gen6 nach): 11 Bilder, 11 identisch
entladen.log nach: alle EREIGNIS belegt 0; SUMME seit=spielstart bilder=1421 fremd 0
```

### Messung r8/r8c: Tod in einem ANDEREN Raum -> LOAD (Dossier O3, dort nicht gefahren)
```
Karte: probe_r35_entladen_karte re15_card.mcr 1020 -26000 0 -8700 0
RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_GOTO_ROOM=1030 RE15_KILL_AT=60 RE15_BOOT_EXIT_AT=3
  Spiel 1: LOAD 1020 -> GOTO 1030 -> Tod in 1030; Spiel 2: LOAD 1020 -> Tod in 1020
entladen.log: VORHER spielende gen=3 raum=1030 | belegt pri_masken=22 ... | EREIGNIS spielende gen=4 alle 0
              SUMME seit=spielstart (Spiel 2, ROOM1020) bilder=321 bilder_mit_masken=137 fremd 0
Bilder ROOM1020 nach dem Laden: Spiel 2 (nach Tod in 1030) vs Spiel 1 (erstes Laden): 5/5 identisch
Titel nach dem Tod (r8c, jedes Bild b31..b361): gen4 (nach Tod in 1030) 331/331 und gen6 (nach Tod in 1020)
  331/331 exakt gleich einem Bild des ersten Titels.
```
(Im ersten Durchlauf r8b wichen die Titel-Einblendbilder b17..b30 um hoechstens 8 Stufen ab — fast schwarze
Bilder, die Stufen kamen in Paaren statt Bild fuer Bild, also Takt der Einblendung; ab b31 alles gleich.)

### Mechanismus erklaert den Befund
Vorbedingung im Protokoll: `VORHER spielende ... pri_masken=24` (die Maskenliste des Todesraums lebt bis zum
Ende des Spiels), danach `EREIGNIS spielende ... 0` und 0 Maskenbilder in Titel/Charakterwahl.
Fix: `re15_entladen_ereignis("spielende")` vor `goto re_title` (main.c:11425-11428).

**Urteil Punkt 1: erfuellt.**

## Punkt 2 — saemtliche Assets der Raeume davor entladen (Tod UND Raumwechsel)

### Was nachweislich faellt (selbst gemessen)
| Lauf | Grenze | VORHER (Auszug) | EREIGNIS |
|---|---|---|---|
| r1 Normaltempo, Ton (SDL_AUDIODRIVER=dummy), Montage-Tuer 1240->1170 | raum | `stimme=6 ... stimme_laeuft=0`, `figur=0`, tim_slots 36 37 | alle 15 Faecher 0 |
| r6 echte Tuer DOOR13 1020->1040 (`RE15_FIRE_AOT=2@40#1020`) | raum | `pri_masken=24 tim=15 gegner=2 ...`, tim_slots ... 24 ... | alle 0; 0 BILD-Zeilen in 1040 |
| r7 240 Bilder/s, Intro -> 1170 -> echte Tuer 4 nach 1130 (`RE15_FIRE_AOT=4@3000#1170`) | raum | `figur=1 stimme=4 stimme_laeuft=1`, tim_slots **1** 4 5 6 7 8 9 11 | alle 0; 0 BILD-Zeilen in 1130 |
| r3/r4/r8 | spielende / spielstart | s.o. | alle 0 |

M1 Stimmen (Abnahme 0): behoben. r1 debug.log: main00..05 aus ROOM1240 geladen (Z. 58..119), danach
`EREIGNIS raum gen=3 ... stimme=0`; in ROOM1170 werden die 1170-Stimmen frisch geladen (Z. 191..222).
M2 Elliot (Abnahme 0): behoben. r1/r7: in ROOM1240 `figur=0`, kein Slot 1; geladen erst nach dem Raum:
`[room] PC loaded room1170.rdt` (Z. 132) -> `[elliot] PL05 loaded ... (Raum 1170, Spawn 0x47)` (Z. 174),
`[tim] elliot TIM in slot 1`, `[enemy-diag] actor1 type=0x47 model=elliot`; am Raumwechsel 1170->1130
`figur=1` + Slot 1 -> EREIGNIS 0. Vorher (exe 20ced3e7, ab_vor_lauf/debug.log Z. 26): `[elliot] PL05 loaded`
schon beim Boot in ROOM1240. Slots 2/3 (Boot-RDT-Objekte 2/5): r8 `VORHER raum gen=2 raum=1020 ... tim_slots 2 3 4 ...`
-> EREIGNIS 0.

A/B Aussehen (eigene Messung): exe N1 gegen exe 20ced3e7, `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_FPS=240
RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_EXIT_AT=1700#1170 RE15_FRAMEDUMP=880-1600/40:fd_` ->
**19/19 Bilder bytegleich** (Helipad-Szene mit Elliot, beide `[F950-elliot-root] screen=(102.2,108.4)`).

### Mangel 1 — Cinematic-Bank ROOM1170 (RBJ/ROOM1170.RBJ, 55 060 B) bleibt geladen (NEU, gemessen)
Der Zensus zaehlt sie nicht; deshalb melden alle EREIGNIS-Zeilen "0". Gelesen mit gdb ueber die
Symbole der exe (`nm`: `s_room_rbj.64` = main.c:8106 `static uint8_t *s_room_rbj`, `s_room_rbj` =
Bindung in enemy_common.c:53), Haltepunkt `re15_testhaken_ende` (RE15_EXIT_AT):
```
r9  (RE15_FPS=240 ... RE15_FIRE_AOT=4@3000#1170 RE15_EXIT_AT=100#1130; echte Tuer 1170 -> 1130,
     debug.log "[rbj] room 1130 has no RBJ ... Leon auf PL00-Basis zurueckgesetzt"):
  === am Ende (Bild 100 in ROOM1130) ===
  main.c s_room_rbj.64 (ROOM1170.RBJ-Dateipuffer) = 0x3eea290
  main.c s_rbj_room.65 (Riegel-Raum)             = 0x1130
  enemy_common s_room_rbj (Bindung)             = (nil)
  0x3eea290: 0x04 0xd7 0x00 0x00 0x02 0x00 0x00 0x00     == xxd ROOM1170.RBJ: 04d7 0000 0200 0000
  entladen.log: VORHER raum gen=3 raum=1170 ... figur=1 -> EREIGNIS raum gen=4 ... alle 0
r10 (RE15_FPS=240 RE15_GOTO_ROOM=1170 RE15_KILL_AT=3200 RE15_EXIT_AT=200#1240; Tod in 1170, NEW GAME):
  === Bild 200 in ROOM1240 des ZWEITEN Spiels ===
  main.c s_room_rbj.64 = 0x3efb030, Bytes 04 d7 00 00 02 00 00 00, s_rbj_room.65 = 0x1170
  entladen.log: EREIGNIS spielende gen=4 alle 0, EREIGNIS spielstart gen=5 alle 0
```
=> Nach dem Raumwechsel 1170->1130 UND nach dem Tod in 1170 (bis in ROOM1240 des neuen Spiels) liegt die
Animationsbank von ROOM1170 noch im Speicher. Sie ist nicht mehr gebunden (enemy_common = NULL), aber
geladen — das widerspricht "saemtliche Assets ... entladen".
Ursache im Code: main.c:8131-8134 gibt den alten Puffer nur frei, wenn der NAECHSTE Raum eine Animation hat
(`if (rbuf && rsz > 0) { if (s_room_rbj) free(s_room_rbj); ...`); der Zweig "has no RBJ" (main.c:8148-8189)
gibt nichts frei; `alles_entladen` (entladen_pc.c:264-312) fasst ihn nicht an; der neue RBJ-Riegel
(main.c:8112-8113) setzt nur den Raum-Schluessel zurueck.
Zweiter Weg derselben Datei: Boot-Pfad `uint8_t *rbj_buf = pc_read_shared(rbj_path, ...)` (main.c:4196). Bei
einem Spielstand in ROOM1170 (r11: Karte `probe_r35_entladen_karte re15_card.mcr 1170`, CONTINUE) steht bei
JEDEM Boot `[rbj] loading cinematic bank: RBJ/ROOM1170.RBJ (55060 bytes)` (debug.log Z. 32 und Z. 137);
`grep "free(rbj_buf" main.c` = kein Treffer -> der Puffer faellt weder am Raumwechsel noch am Tod und wird
beim naechsten Spielstart neu angelegt (der alte ist verloren).
Original: der Animationsblock ist RDT+0x5C (FUN_8001b3f8 `lw a2,92(v0)` @0x8001b404), die RDT liegt ab der
Arena-Basis (`jal 0x80013b60` @0x800397e8), Arena-Reset @0x80039738 — dieselbe Kette, die das Dossier fuer
den RBJ-Riegel zitiert. Nicht im Dossier unter O2/OFFEN.
Naechster Schritt: beide Puffer in `alles_entladen` freigeben (Statics aus main.c heraus wie beim MSK-Cache),
Fach "rbj" in den Zensus, Pin: Lauf r9 (Tuer 1170->1130) und r10 (Tod in 1170) muessen danach `rbj=0` zeigen.

**Urteil Punkt 2: teilweise** — Masken, Texturen, Modelle (inkl. Elliot), Effekte, Tonbaenke, Stimmen, RDT
und BG fallen nachweislich; die Cinematic-Bank ROOM1170 bleibt messbar geladen.

## Gate 4 — RE-Gate

Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`):
```
RE1.5 PSX.EXE (0x800420a0 = Sce_em_set 0x44 laut RE15_FUN_CATALOG.md:125, ghidra1:152496)
  800422c0: lui s1,0x800b / 800422c4: lw s1,-14468(s1)  0x800ac77c   ; Arena-Kopf
  800422dc: sw s1,124(s0) / 800422f4: addiu s1,s1,12
  80042328: jal 0x80022300 / 8004232c: addu a3,s1,zero                ; EMD nach a3 = Arena
  80042554: sw s1,-14468(at)  0x800ac77c                              ; Kopf hinter das Modell
  (800422f0 beq Typ == 0x800b3f78 -> 0x80042478: gleicher Typ wie zuvor -> Zeiger vom Vorgaenger
   0x800b3f74 uebernommen, ebenfalls Arena; kein Sonderweg fuer Typ 0x47 im Handler)
  800314b8/bc + 800314c8/cc: lui ...,0x801b / ori ...,0xd814           ; Spieler fest 0x801bd814
  80039a44: jal 0x800299a4 / 80039a4c: lui v1,0x8010 / 80039a50: addu / 80039a58: sw v1,-14464(at) 0x800ac780
  8001d580: jal 0x8001923c / 8001d588: jal 0x80039a30 / 8001d590-a0: lw 0x800ac780 -> sw 0x800ac77c
  8001d5a4: jal 0x800314b0 / 8001d5ac: jal 0x800396fc
RE2 info/re2leon/PSX.EXE
  8004a1bc/c0: a3 = 0x800110c8 / 8004a1c4: jal 0x80012fb8   (RE2_Quellcode_V2/FUN_80049e48.c:
               FUN_80012fb8(uVar9,DAT_800ce324,1,&DAT_800110c8) = RDT lesen, danach Relokation +8..+100)
  800130d4: addiu a0,zero,9  / 800130e0: jal 0x8008a380
  800130f0: addiu a0,zero,14 / 800130f4/f8: a1 = 0x8009a429 / 80013100: jal 0x8008a380
  80013110: addiu a0,zero,21 / 80013140: addiu a0,zero,6
  bytes 0x8009a410: 00 00 00 00 80 c8 ... ; 0x8009a420: 70 2e 01 80 48 2f 01 80 00 a0 a0 00
                    -> 0x8009a415 = 0xC8 (Bit 6 XA an), 0x8009a429 = 0xA0 (Bit 6 aus)
```
Jede zitierte Stelle enthaelt das Behauptete. Einschraenkung: dass 0x8008a380 DsCommand ist, folgt aus dem
Aufrufmuster (a0 = Kommando 9/14/21/6, a1 = Parameterzeiger, a3 = -1), nicht aus einer Signatur.
Der Unit-Pin `unit_r35_entladen_n1beleg` prueft genau diese Worte/Bytes (test_r35_entladen.c:103-127).

Diff-Suche (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt|approx|hack`
in `git diff master...HEAD -- re15_port`): kein Treffer. Neue getenv nur `RE15_ENTLADEN_LOG/_SHOT/_SHOT_BILD`
(Messhaken). Neue Zahlen: Slot-Bereiche (1..18 usw., mit @0x-Beleg im Kommentar render_pc.c), keine
Verhaltenskonstante. Fix erklaert den Befund: ja (VORHER `stimme=6` / `figur=1` + Slot 1 im Protokoll,
EREIGNIS 0; Elliot im Boot-Raum vorher geladen, nachher nicht).

## Gate 5 — Vertrag, Pfade, Tests

`git diff master...HEAD --name-only`: nur `analysis/befunde_runde35/I_*`, `engine/src/{enemy_common,entladen_common}.c`,
`include/{re15_enemy,re15_entladen}.h`, `platform/pc/{main.c,src/audio_pc.c,src/bg_pc.c,src/elliot_pc.c,src/entladen_pc.c,src/render_pc.c,src/room_pc.c}`,
`tests/{integration/test_r35_entladen.cmake,unit/probe_r35_entladen_karte.c,unit/probes/r35_entladen.cmake,unit/test_r35_entladen.c}`.
Kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`, keine Edits an den beiden CMakeLists
(Treffer: 0). Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots oder Ereignisse belegt.
main.c: +35/-48 in 13 Hunks — der Elliot-Boot-Lader (31 Zeilen) wandert nach elliot_pc.c, dazu Haken-Zeilen;
das sprengt "1-5 Zeilen", ist aber im Dossier ("main.c-Umfang") offen benannt -> Hinweis, kein Gate-Bruch.
Tests: `unit_r35_entladen_{beleg,gegner,n1beleg}` (Original-Bytes + Voll-Scans), `integration_r35_entladen_{a..e}`
(echte exe-Kopie). Pins c/d/e pruefen genau die N1-Faecher (`tim_slots 2 3`, `figur=1` + Slot 1 am Tod,
`stimme>=1` vor / 0 nach der Grenze). Einen Pin fuer Mangel 1 gibt es nicht (kein Fach).

## Hinweise (keine Maengel)

* H1 Montage-Schnappschuss `s_bg_prev` (bg_pc.c:527-537) bleibt beim Raumwechsel gueltig: gdb r14
  (`RE15_FPS=240 ... RE15_EXIT_AT=100#1170`) -> `s_bg_prev_ok = 1` in ROOM1170 (ein ROOM1240-Bild,
  statisches Feld 307 200 B). Die Begruendung in entladen_pc.c:298-302 ("gehoert zu einer laufenden
  Ueberblendung") trifft nicht zu: die Montage laeuft nur in ROOM1240 (main.c:5426
  `re15_montage_fx_set_active(RE15_ROOM_BASE(...) == 0x1240)`), nach der Tuer wird das Bild nie mehr gezeichnet.
  Kein sichtbarer Effekt; gehoert entweder an "raum" mit verworfen oder mit Beleg ins Dossier (O2).
* H2 RE2-ENEMSE-Bankcache `s_re2se_cache[3]` (audio_pc.c:1184) wird an keiner Grenze geleert; der eigene
  Kommentar (audio_pc.c:1155-1160) sagt, RE2 bestimmt die Bank PRO RAUM (FUN_80052b38) und laedt sie nach.
  Nicht gemessen: in r12 (Tod 1020) und r13 (Tod 1190, 900 Bilder) wurde keine Bank geladen (gdb: alle
  `loaded=0`). Naechster Messweg: Raum mit Hund/Spinne/Gitterarm bis zum ersten ENEMSE-Ruf
  (`RE15_RE2SE_LOG`), dann Tuer/Tod und `s_re2se_cache[k].loaded` lesen.
* H3 RE2-Mini-Tonbaenke ELEVSE/HINTSE/TUERSE/PANEL2130/TORSE (audio_pc.c:996/1223/1302/1376/3754) leben
  Prozess-lang und stehen weder im Zensus noch in O2. Im Port dienen sie mehreren Raeumen (TORSE gehoert zur
  Tuerbank, die O2 belegt); fuer ELEVSE (nur 1080/1081/4020/4021, Quelle RE2-Raumbank ROOM21B0 SND0) fehlt
  eine begruendete Entscheidung im Dossier.
* H4 O4 bestaetigt: `heli_md1`/`pilot_md1` (main.c:4011-4033) werden nur geparst, nirgends gelesen (grep);
  ihre TIM-Slots 2/3 bindet niemand (`bind_tim_slot`: nur 0, 21, Bank-, Prop-, Gore-Slots und 1 fuer Elliot).
* H5 `re15_rbj_room()` (enemy_common.c:70, neu) ist nach jeder Grenze NULL (re15_enemy_reset ->
  `re15_rbj_bind_room(NULL,0)`, enemy_common.c:139) — kein haengender Zeiger in elliot_pc.c.
* H6 Integration: Spur L wechselt in ihrer Knall-Montage den Raum mitten in einer Szene; spricht dort an
  einer Grenze noch eine Zeile, schneidet Schritt (9) sie ab (so belegt fuer RE2). In r7 (erzwungene Tuer
  mitten im Intro) stand `stimme_laeuft=1`. Beim Zusammenfuehren mit L nachsehen.
* H7 Neue Quelldateien elliot_pc.c/entladen_pc.c/entladen_common.c: Android-Bau neu konfigurieren (GLOB-Cache).
* H8 O1 (PSX nicht gebaut/gemessen) — nicht pruefbar in dieser Abnahme.

## Maengel (nummeriert)

1. **Cinematic-Bank ROOM1170 bleibt nach Raumwechsel und Tod geladen.** Der Dateipuffer
   `RBJ/ROOM1170.RBJ` (55 060 B) in main.c:8106 (`static uint8_t *s_room_rbj`) wird nur freigegeben, wenn der
   naechste Raum eine Animation hat (main.c:8131-8134); bei einem Raum ohne Animationsblock (Zweig
   main.c:8148ff) und am Tod bleibt er liegen. Gemessen mit gdb: in ROOM1130 Bild 100 nach der echten Tuer
   1170->1130 und in ROOM1240 Bild 200 des neuen Spiels nach dem Tod in 1170 zeigt `s_room_rbj.64` auf einen
   Puffer mit dem Dateikopf `04 d7 00 00 02 00 00 00`; die EREIGNIS-Zeilen derselben Laeufe melden alle
   Faecher 0. Dazu der Boot-Pfad main.c:4196 (`rbj_buf`, Spielstand in ROOM1170): bei jedem Boot
   `[rbj] loading cinematic bank: RBJ/ROOM1170.RBJ (55060 bytes)`, kein `free(rbj_buf)` in main.c. Original:
   Animationsblock RDT+0x5C (@0x8001b404) in der Arena (@0x800397e8, Reset @0x80039738). Kein Fach im
   Zensus, nicht in O2/OFFEN, kein Pin.

Artefakte (Scratchpad, fluechtig): `abn_i1/r1_stimme`, `r3_nach`, `r3_frisch`, `r4_frisch`, `r4_nach`,
`r5_aot1170`, `r6_tuer`, `r7_elliot_tuer`, `r8_load`, `r8b`, `r8c`, `r9_gdb`, `r10_gdb`, `r11_boot1170`,
`r12_gdb`, `r13_gdb`, `r14_gdb`, `ab_n1`, `ab_vor_lauf`, `vergleich.py`, `vergl2.py`, `n_strip.png`.
