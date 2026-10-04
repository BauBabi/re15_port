# Runde 35 — Spur I "entladen": UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r35_entladen`, Zweig `r35/entladen`, Stand `a485d787` (Basis master `154a73c1`).
Abnahme 2026-10-04. Alles unten ist am gebauten Stand SELBST gemessen; die Messschiene des
Bau-Agenten (`RE15_ENTLADEN_LOG`) wurde nur als EIN Messweg benutzt, der Hauptbeleg fuer Punkt 1 ist
ein davon unabhaengiger Pixelvergleich gegen einen frischen Prozess.

## Ergebnis

| Punkt (Wortlaut AUFTRAG.md Z. 24) | Urteil |
|---|---|
| 1. "Nach dem Sterben und new game ... noch PRIs von meinen Spielstand davor ... angezeigt" | **erfuellt** |
| 2. "Wenn man tot ist, aber auch wenn man den Raum wechselt sollen saemtliche Assets von den Raeumen davor entladen sein" | **teilweise** (M1, M2) |

Gates: Suite gruen (484/484, eigener Lauf) · @0x-Gate haelt (Stichprobe 20+ Adressen, alle stimmen) ·
Pfad-/Vertrags-Gate haelt · Tests vorhanden und messend (6/6 einzeln gruen).
**bestanden = false** (Punkt 2 nur teilweise).

## Bau und Suite (eigener Lauf)

```
cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build
  -> ninja: no work to do / === LOCAL-BUILD-OK (build)       (exe = Stand a485d787; a485d787 aendert nur das Dossier)
bash re15_port/tools/local_build.sh test
  -> 100% tests passed, 0 tests failed out of 484 / Total Test time (real) = 1269.82 sec
  -> === LOCAL-BUILD-OK (test) — Tests 484/484
ctest --test-dir re15_port/build -R r35_entladen --output-on-failure
  -> unit_r35_entladen_beleg/_gegner Passed, integration_r35_entladen_a 21.19 s, _b 15.67 s, _c 38.78 s, _d 45.98 s
  -> 100% tests passed, 0 tests failed out of 6
```
Kein Flatter-Haken rot, nichts nachgefahren.

Vergleichs-exe "vorher": Stand `9b36e714` (Messschiene des Bau-Agenten OHNE Entladen; Code-Diff zu
master = nur Generation/Messschiene/MSK-Cache-Umzug) — aus `git archive master` + die 8 Dateien aus
`9b36e714` im Scratchpad gebaut (`RE15_TESTS=OFF`), zum Laufen als Kopie neben die exe gelegt und
danach geloescht. Zusaetzlich master `154a73c1` gebaut (nicht weiter gebraucht).

Laeufe: `lauf.sh <exe> <arbeitsverz> <timeout> KEY=WERT...` mit sauberem PATH
(`/c/msys64/mingw64/bin` zuerst), exe-KOPIE unter eigenem Namen, kein taskkill.

## Punkt 1 — PRIs des alten Spielstands nach Tod + NEW GAME

### Messung A: Tod -> Titel -> NEW GAME -> Charakterwahl, dicht je Bild
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60
RE15_TITLE_CONFIRM_MS=8000 RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=40
RE15_BOOT_EXIT_AT=2 RE15_ENTLADEN_LOG=entladen.log RE15_ENTLADEN_SHOT=es
RE15_ENTLADEN_SHOT_BILD=300..610 (jedes Bild)` — Spiel 1 springt nach ROOM1020 (Cut 0, 24 Masken),
stirbt bei Bild 40, der zweite Titel bestaetigt NEW GAME ueber den ECHTEN Menueweg in die
Charakterwahl (Bestaetigungs-Zoom), dann Spielstart. Bilder = Rueckleser VOR `SDL_RenderPresent`
(`re15_render_pc_request_readback`). Referenz "frisch": gleiche exe, KEIN Spiel davor
(`RE15_TITLE_CONFIRM_MS=1 RE15_PSELECT_AUTO=1 RE15_BOOT_EXIT_AT=1`, Bilder 300..610).

Auswertung `vergleich.py` (exakter Bytevergleich jedes Bildes gegen ALLE Bilder des frischen Laufs):
```
VORHER (9b36e714):  vorher-Tod Bilder 300: exakt gleich einem Frisch-Bild 29, ohne exakten Treffer 271
                    vor b330 ~ frisch b331: 1934 px verschieden, bbox (191,0)-(319,152)
                    vorher-Bilder mit >500 px Abweichung: 271, erste/letzte b328/b599
                    entladen.log: SUMME seit=spielende bilder=599 bilder_mit_masken=599
                                  bilder_fremd_belegt=599 bilder_fremde_masken_gezeichnet=599 fremde_masken_max=24
NACHHER (a485d787): nachher-Tod Bilder 300: exakt gleich einem Frisch-Bild 300, ohne exakten Treffer 0
                    entladen.log: SUMME seit=spielende bilder=599 bilder_mit_masken=0 ... bilder_fremde_masken_gezeichnet=0
```
Bild b500 (Charakterwahl, Zoom auf Leon, Hintergrund schwarz) nebeneinander
(`scratchpad/abn_i0/vergleich_b500.png`): vorher steht rechts ein grau-weisses Raumstueck
(ROOM1020-Masken), nachher nicht. Die Abweichung vorher liegt GENAU im Maskenbereich
(191,0)-(319,152) — derselbe Befund wie das Dossier (`vorher_auswahl_b500.png`).
=> Nachher ist JEDES der 300 Bilder von Titel/Charakterwahl nach dem Tod byte-gleich mit einem
Bild eines Prozesses, der nie gespielt hat. Kein Rest des alten Spielstands sichtbar.

### Messung B: das neue Spiel selbst (ROOM1240) nach dem Tod
`RE15_FRAMEDUMP=180-399/5:fd_`; frisch: `RE15_TITLE_SHOT_AF=60 RE15_EXIT_AT=399`; nach Tod:
`RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=400 RE15_BOOT_EXIT_AT=3` (Spiel 2 laeuft in ROOM1240 bis Bild 399).
```
Spielbilder 180..399 (je 5): 44 identisch 44 verschieden 0   (mittl. Helligkeit Bild 300: 32.5, also Inhalt)
entladen.log nach Tod: EREIGNIS spielstart gen=5 ... belegt alle 13 Faecher 0;
                       SUMME seit=spielstart bilder=661 bilder_fremd_belegt=0 bilder_fremde_masken_gezeichnet=0
```
(Bilder 1..39 waren in beiden Laeufen ganz schwarz — als Beleg verworfen, deshalb 180..399.)

### Mechanismus erklaert den Befund
Vorbedingung des Fixes im Vorher-Protokoll vorhanden: `EREIGNIS spielende gen=4 ... belegt
pri_masken=24 ... fremd pri_masken=24` — die Maskenliste des Todesraums lebte ueber den Tod, und
`render_pc.c` zeichnet sie in jedem Modus (Masken-Pass in `re15_render_end_frame`). Die Charakterwahl
malt ihren Hintergrund VOR dem Masken-Pass, Titel/Film decken sie zu — deshalb nur dort sichtbar.
Fix: `re15_entladen_ereignis("spielende")` vor `goto re_title` (main.c) leert Liste/Atlas/SLD/MSK.

### RE-Beleg (selbst disassembliert, `re15_disasm.py` auf info/Re1.5/PSX.EXE)
```
8001ce3c: lui v0,0x800b / 8001ce40: lw v0,-13768(v0) 0x800aca38 / 8001ce48: and v0,v0,v1(0x100000)
8001ce4c: bne v0,zero,0x8001ce5c / 8001ce54: jal 0x80039590        ; Masken-Zeichner, Spielmodul-Schleife
8001d1f8: bne v0,zero,0x8001c97c (aca38 & 0x40000000) / 8001d200: jal 0x80021eb4 / 8001d208: jal 0x80029a28
```
Eigener jal-Wort-Voll-Scan der EXE: `jal 0x80039590` genau 1x (`0x8001ce54`); in
`info/Re1.5/PSX/BIN/*.BIN` (alle Overlays) 0x. => Raum-Masken zeichnet nur das Spielmodul.

**Urteil Punkt 1: erfuellt.**

## Punkt 2 — saemtliche Assets der Raeume davor entladen (Tod UND Raumwechsel)

### Messung C: Raumwechsel ueber eine ECHTE Tuer mit Tuersequenz (O3 des Dossiers, dort nicht gefahren)
`RE15_AOT_DUMP=1` in ROOM1020: `[aot]  2 DOOR Mitte(-27150,-8200) ... -> ROOM1040 cut=6`.
Lauf: `RE15_DEBUG_JUMP=1020@5 RE15_FIRE_AOT=2@40#1020 RE15_EXIT_AT=150#1040 RE15_ENTLADEN_LOG=...`
```
debug.log: [fire-aot] slot=2 at F40 (Raum 1020)
           [tuer] Sequenz Archiv 2 DOOR13 Variante 0 ... (9 Skripte) / Sequenz fertig: 286 Bilder
           [room] PC loaded room1040.rdt / [flow] EXIT_AT: Bild 150 in Raum 1040
NACHHER: VORHER raum gen=3 raum=1020 | belegt pri_masken=24 ... tim=15 ... | tim_slots 4 5 6 7 8 9 11 12 24 26 27 36 37 46 47
         EREIGNIS raum gen=4 raum=1020 | belegt (alle 13 Faecher) 0 ; BILD-Zeilen in ROOM1040: 0
VORHER : EREIGNIS raum gen=4 | fremd pri_masken=24 pri_atlas=1 sld=1 tim=15 gegner=2 esp_bank=1 rdt=1
         BILD 1/30/60/90 gen=4 raum=1040 ... fremd ... tim=5   (5 TIM-Slots aus ROOM1020 bleiben in 1040)
```
Der Tuer-Slot 24 (DOOR13) faellt mit. Der echte Tuerweg ist sauber.

### Messung D: Tod (Lauf A/B oben)
`EREIGNIS spielende gen=4 raum=1020 | belegt ... (alle 13 Faecher) 0`; vorher alle Faecher fremd belegt.

### Mangel M1 — Raum-Stimmen (synchro/STAGEn/room<id>/mainNN.wav) werden NICHT entladen
Lauf: `SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60
RE15_EXIT_AT=300#1170 RE15_ENTLADEN_LOG=entladen.log` (ohne Dummy-Treiber ist hier kein Audiogeraet:
`SDL_OpenAudioDevice failed: WASAPI can't find requested audio endpoint`, dann laeuft der Cache gar nicht).
```
debug.log  67: [voice] clip loaded: main00.wav (48000 Hz x2 -> 248062 @44100 Hz)
          104: main01 (235200) / 137: main02 (259087) / 174: main03 (288487) / 215: main04 (308700) / 255: main05 (242550)
          277: [room] PC loaded room1170.rdt          ; Montage-Tuer 1240 -> 1170
          301: [flow] EXIT_AT: Bild 300 in Raum 1170   ; keine [voice]-Zeile in ROOM1170
entladen.log: EREIGNIS raum gen=3 raum=1240 | belegt ... ton=0 ...   (Zensus kennt den Stimmen-Cache nicht)
```
6 dekodierte ROOM1240-Clips = 1 582 086 Samples int16 ~ 3,2 MB PCM. Code: `s_voice_clip[VOICE_MAX_MSG]`
(`platform/pc/src/audio_pc.c:277`), Schluessel `s_voice_room` (`:1992`). EINZIGE Freigabe:
`audio_pc.c:2052-2053` innerhalb `re15_voice_load_clip`, und nur wenn eine Stimme in einem ANDEREN Raum
angefordert wird (`:2205` aus `re15_voice_play`). Weder `alles_entladen` (entladen_pc.c) noch
`re15_audio_raum_entladen` (audio_pc.c:3642ff) fassen `s_voice_clip`/`s_voice_room`/`s_xa` an.
=> Nach Raumwechsel UND nach dem Tod bleiben die Stimmen des Raums davor geladen, bis irgendwann in
einem anderen Raum eine Zeile gesprochen wird. Das widerspricht dem Wortlaut "saemtliche Assets" und
der Abschlussbehauptung "An allen Grenzen ist nichts mehr belegt" — der Zensus hat fuer diese Klasse
kein Fach, die Pins koennen sie nicht sehen. Nicht im Dossier unter O2 erklaert.
Naechster Schritt: Cache in `alles_entladen` verwerfen (mit dem Stream-Loesen unter
`SDL_LockAudioDevice` wie `audio_pc.c:2031-2048`), Fach "stimme" in den Zensus, Pin mit
`SDL_AUDIODRIVER=dummy` (Lauf wie oben: nach `EREIGNIS raum` muss das Fach 0 sein).

### Mangel M2 — Ausnahme "TIM 0..3 / Elliot / Heli+Pilot" ohne Original-Beleg
Dossier O2: "Bewusst NICHT entladen (keine Raum-Assets ...): TIM 0..3 (Spieler, Elliot, Heli/Pilot:
Boot-Lader, bei jedem Spielstart neu)". Das ist ein Port-Strukturargument, kein Original-Beleg — und
der Port widerspricht sich selbst:
* `platform/psx/src/asset_psx.c:484-505` (PSX-Pfad): Elliot ist "one special NPC per room, freed on
  room change", Manifest `{ 0x1170, "ELLIOT" }`, Modell in die Raum-Arena.
* `engine/src/room_common.c:314-318`: "(4b) ARCH: load the dest room's CINEMATIC overlay (rbj) +
  special NPC (Elliot) into the per-room arena ... (PSX; PC keeps its boot-loaded copies)";
  `platform/pc/main.c:8003-8004`: `rc.load_cinematic = 0; /* PC keeps its boot-loaded Elliot/rbj resident */`.
* Heli/Pilot (`main.c:4011-4032`) werden aus `rdt.prop_md1[2]/[5]` + `prop_tim[2]/[5]` der BOOT-RDT
  in TIM-Slot 2/3 geschnitten — also Bytes eines Raums — und bleiben ueber jeden Raumwechsel.
Gemessen: in den Laeufen oben (Boot ROOM1240) `[elliot] PL05 loaded ... [tim] elliot TIM in slot 1`
bei jedem Spielstart; der Zensus zaehlt Slot 0..3 per Definition nicht
(`re15_render_pc_tim_slot_raum`, render_pc.c). Heli/Pilot fallen bei Boot 1240 nicht an (keine
`[tim] heli`-Zeile) — der Fall Boot in ROOM1170 (LOAD) ist nicht gefahren.
Gefordert: entweder Original-Beleg, dass RE1.5 diese Modelle ausserhalb der Raum-Arena haelt, oder
Entladen/Neuladen je Raum wie der eigene PSX-Pfad.

### Was nachweislich faellt (bestaetigt)
Alle 13 Faecher des Zensus (pri_masken, pri_atlas, sld, msk, tim 4..18/24..43/45..49, gegner,
esp_bank, esp_fx, esp_pool, re2fx, rdt inkl. Boot-RDT, ton snd0/snd1, bg) sind an jeder gefahrenen
Grenze (raum per Sprung, raum per echter Tuer mit Sequenz, raum per Montage-Tuer [Pin d], spielende,
spielstart, LOAD [Pin c]) 0, und in keinem Bild danach fremd belegt.

**Urteil Punkt 2: teilweise** — Masken/Texturen/Modelle/Effekte/Tonbaenke/RDT/BG fallen
nachweislich; die Raum-Stimmen (M1) bleiben messbar geladen, die Elliot/Heli-Ausnahme (M2) ist
unbelegt.

## Gate 4 — RE-Gate

Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, PSX.EXE):
```
80039704: lw a0,-14464(a0)   0x800ac780   ; Arena-Basis
80039738: sw a0,-14468(at)   0x800ac77c   ; Bump-Kopf = Basis
80039740: sw a0,-14472(at)   0x800ac778   ; RDT-Zeiger = Basis
80039748: sw a0,-16720(at)   0x800bbeb0
800397c0: lw a1,-14472(a1)   0x800ac778 ; 800397e8: jal 0x80013b60   ; neue RDT AB der Arena-Basis
8003996c: jal 0x80019354 ; 80039974: jal 0x80043eac ; 8003997c: jal 0x80043fb0
800399cc: jal 0x80039270 ; 80039270..88: lw v1,0(0x800ac77c) / sw v1 -> 0x800b2584   ; Maskentabelle = Arena-Kopf
80039a08: jal 0x8001b3f8 ; 8001b404: lw a2,92(v0) (RDT+0x5C)
8001d590..a0: lw v0,0x800ac780 / sw v0 -> 0x800ac77c ; 8001d5a4: jal 0x800314b0 ; 8001d5ac: jal 0x800396fc
8001d988: jal 0x800396fc
8001d248/4c: ori a0,zero,2 / jal 0x80021634 ; 8001d620/24 dto. ; 8001d82c/30 dto. ; 8001dadc jal 0x80021634 (a0=0), 8001daec sb 1 -> 0x800b5457
80019378: sb zero,0(at) Schleife Stride 132 (0x84) ; 8003eab0-acc: sw zero, 32 Plaetze Stride 148 ab 0x800b3f98
800397fc-834: Relokation der RDT-Kopfzeiger +8..+0x60
```
jal-Voll-Scan (eigenes Skript, EXE ab Datei-Offset 0x800, t_addr 0x80010000):
`jal 0x800396fc` -> genau `0x8001d5ac`, `0x8001d988`; `jal 0x80039590` -> genau `0x8001ce54`;
`jal 0x80039270` -> `0x800399cc`; `jal 0x8001b3f8` -> `0x80039a08`; `jal 0x8001d22c` -> `0x8001c96c`.
In allen `info/Re1.5/PSX/BIN/*.BIN`: keiner dieser jal. Jede zitierte Stelle enthaelt das Behauptete.

Diff-Suche (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt|approx`):
kein Treffer. Neue Zahlen im Code: nur Slot-Bereiche (Port-Slot-Topologie, render_pc.c Slot-Karte),
Log-Drossel (jedes 30. Bild) und Vorgabebild 5 der Messschiene — keine Verhaltenskonstante. Kein
Env-Schalter als Abschluss (RE15_ENTLADEN_LOG/SHOT sind reine Messhaken).
Fix erklaert den Befund: ja (Vorbedingung `pri_masken=24 fremd` am Spielende im Vorher-Protokoll).

## Gate 5 — Vertrag, Pfade, Tests

`git diff master --name-only`: nur `analysis/befunde_runde35/I_entladen*`, `re15_port/engine/src/{enemy_common,entladen_common}.c`,
`re15_port/include/re15_entladen.h`, `re15_port/platform/pc/{main.c,src/audio_pc.c,src/bg_pc.c,src/entladen_pc.c,src/render_pc.c,src/room_pc.c}`,
`re15_port/tests/{integration/test_r35_entladen.cmake,unit/probe_r35_entladen_karte.c,unit/probes/r35_entladen.cmake,unit/test_r35_entladen.c}`.
Kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`, keine Edits an
`tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`. Keine Bank-9-Bits, keine
Nachrichten-IDs, keine AOT-Slots/Ereignisse belegt (nicht noetig). main.c-Haken je 1-6 Zeilen.
Tests: `unit_r35_entladen_beleg` (Original-Bytes + jal-Voll-Scan), `_gegner`, `integration_r35_entladen_a..d`
(echte exe-Kopie). Gegenprobe selbst: das Vorher-Protokoll (9b36e714) haette Pin a an
`EREIGNIS spielende ... belegt pri_masken=24` und an `SUMME ... bilder_fremde_masken_gezeichnet=599`
scheitern lassen — die Pins messen.

## Hinweise (keine Maengel)

* H1 Pin b faehrt den Raumwechsel ueber `RE15_GOTO_ROOM`, nicht ueber eine Tuer; der echte
  Tuerweg ist durch Pin d (Montage-Tuer) und diese Abnahme (Messung C, DOOR13) abgedeckt.
* H2 Pin a kommt ueber `RE15_TITLE_SHOT` am Titel vorbei ohne Charakterwahl; er faengt den
  Mechanismus trotzdem (Titelbilder zeichneten vorher die Masken). Die Charakterwahl selbst ist
  hier per Pixelvergleich belegt (Messung A).
* H3 Die Slot-Karte in `render_pc.c:220` sagt "24/25 = weapon-in-hand model TIMs"; tatsaechlich
  belegen `door_scene_pc.c:44/140` 24/25 (Tuer/Spender), `main.c:166-167` nennt sie "RESERVED/unused".
  Das Dossier ("Tuersequenz 24/25") stimmt; der Kommentar in render_pc.c ist alt (nicht von dieser Spur).
* H4 O1 PSX nicht gebaut/gemessen (vom Bau-Agenten offen gefuehrt) — nicht pruefbar in dieser Abnahme.
* H5 Neue .c-Dateien: Android-Bau neu konfigurieren (GLOB-Cache).
* H6 Neuer 58-Zeilen-Block in `render_pc.c` (Zugriff auf statische Render-Caches) — nicht in der
  VERTRAG-Liste der gemeinsamen Dateien, aber Merge-Konflikte mit anderen Spuren moeglich.

## Maengel (nummeriert)

1. **M1 Raum-Stimmen-Cache bleibt geladen.** `s_voice_clip` (audio_pc.c:277, Schluessel
   `s_voice_room` :1992) wird an keiner Grenze (raum/spielstart/spielende) verworfen; einzige Freigabe
   audio_pc.c:2052-2053 erst bei der naechsten Stimme in einem anderen Raum. Gemessen: 6 ROOM1240-Clips
   (main00..05, ~3,2 MB PCM) geladen, `EREIGNIS raum gen=3` 1240->1170 meldet "ton=0 ... belegt 0",
   300 Bilder in ROOM1170 ohne neue Stimme -> Clips weiter resident. Kein Fach im Zensus, nicht in O2.
2. **M2 Ausnahme Elliot/Heli/Pilot (TIM 0..3) ohne Original-Beleg.** O2 begruendet sie mit
   "Boot-Lader"; der eigene PSX-Pfad (asset_psx.c:484-505, room_common.c:314-318) behandelt Elliot als
   Raum-NPC in der Raum-Arena, der PC haelt ihn dauerhaft (main.c:8003-8004); Heli/Pilot stammen aus
   der Boot-RDT (main.c:4011-4032, Slots 2/3). Original-Beleg fehlt oder Entladen je Raum fehlt.

Artefakte der Messung (Scratchpad, fluechtig): `abn_i0/r1_frisch`, `r2_tod_nach`, `r3_tod_vorher`,
`r4_spiel_frisch`, `r5_spiel_nach_tod`, `r7_tuer_nach`, `r7_tuer_vorher`, `r8_stimme`,
`vergleich.py`, `vergleich_b500.png`, `suite_abnahme.log`.
