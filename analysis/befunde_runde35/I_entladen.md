# Runde 35 — Spur I "entladen" (Dossier, fortlaufend)

Zweig `r35/entladen`, Baum `.claude/worktrees/r35_entladen`, Basis master 154a73c1.

## Auftrag (woertlich, AUFTRAG.md Z. 24)
> Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen Spielstand
> davor über die angezeigt werden …. Wenn man tot ist, aber auch wenn man den Raum wechselt sollen
> sämtliche Assets von den Räumen davor entladen sein.

Punkte:
1. Nach Tod + New Game bleiben PRIs des alten Spielstands sichtbar.
2. Beim Tod UND beim Raumwechsel saemtliche Assets der vorherigen Raeume entladen.

## Messung vorher

### Messschiene (gebaut, Commit c21e38f8 + Folgecommit)
`RE15_ENTLADEN_LOG=<datei>` (platform/pc/src/entladen_pc.c): jede Grenze ("raum", "spielstart",
"spielende") zaehlt die Generation `g_re15_entladen_gen` hoch; jeder Cache vermerkt beim FUELLEN die
Generation (render_pc: Maskenliste, Atlas, jeder TIM-Slot; bg_pc: SLD-Auszug; MSK-Container;
enemy_common: Gegnerbank; Raum-ESP-Bank; room_pc: RDT-Bytes). Belegt + aelter = FREMD.
Zeilen: `VORHER <grenze>` (Bilanz vor dem Wechsel), `SUMME` (Bilder seit letzter Grenze: mit Masken,
mit fremder Belegung, mit FREMD GEZEICHNETEN Masken), `EREIGNIS <grenze>` (Belegung direkt nach der
Grenze), `BILD` (gedrosselt, wenn fremd). Zusaetzlich `RE15_ENTLADEN_SHOT=<praefix>` +
`RE15_ENTLADEN_SHOT_BILD=<n,...>`: komplett komponiertes Bild (Rueckleser vor SDL_RenderPresent,
wie FRAMEDUMP) — auch in Titel/Charakterwahl, wo FRAMEDUMP nicht hinkommt.
⚠ gdigrab liefert in dieser Sitzung weisse Bilder (Auftrag) -> Bildbelege per Rueckleser.

### M1 Tod -> Titel -> NEW GAME (exe-Kopie re15_pc_entladen.exe, Stand vor dem Entladen)
Lauf a: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=40
RE15_BOOT_EXIT_AT=3`. ROOM1020 Cut 0 hat 24 sprite.pri-Masken (debug.log `[pri] cut=0
pri_offset=0x778 masks=24 fg_atlas=1`). Log: `I_entladen_bilder/vorher_lauf_a_entladen.log`.
```
VORHER spielende gen=3 raum=1020 | belegt pri_masken=24 pri_atlas=1 sld=1 tim=14 gegner=2 esp_bank=1 rdt=1
SUMME seit=spielende bilder=23 bilder_mit_masken=23 ... bilder_fremde_masken_gezeichnet=23 fremde_masken_max=24
EREIGNIS spielstart gen=5 raum=1020 | belegt pri_masken=24 pri_atlas=1 sld=1 tim=14 gegner=2 esp_bank=1 rdt=1
BILD 1 gen=5 raum=1240 ... | fremd tim=12 rdt=1          (zweites Spiel, ROOM1240)
SUMME seit=spielstart bilder=301 ... bilder_fremd_belegt=301 fremd_belegt_max=13
```
=> In JEDEM Bild zwischen Tod und neuem Spiel werden die 24 Masken des Todesraums GEZEICHNET
(render_pc end_frame zeichnet die Maskenliste in jedem Modus). Im zweiten Spiel bleiben 12 Raum-
TIM-Slots (Gegner/Props/Raum-ESP des Todesraums) und die ROOM1020-RDT-Bytes resident (301/301 Bilder).

Lauf d (echter Weg ueber das Titelmenue in die Charakterwahl): zusaetzlich
`RE15_TITLE_SHOT_AF=60 RE15_TITLE_CONFIRM_MS=8000 RE15_PSELECT_AUTO=1 RE15_BOOT_EXIT_AT=2
RE15_ENTLADEN_SHOT_BILD=250,...,590` -> `SUMME seit=spielende bilder=599 bilder_mit_masken=599
bilder_fremde_masken_gezeichnet=599 fremde_masken_max=24`.
Bild 500 nach dem Tod (`I_entladen_bilder/vorher_auswahl_b500.png`, Charakterwahl "Confirm-Zoom",
schwarzer Hintergrund): rechts steht ein grau-weisser Raum-Ausschnitt (Gelaender/Treppe) — ein
ROOM1020-Masken-Stueck UEBER der Charakterwahl. Das ist der Nutzerbefund.
WARUM gerade dort: die Charakterwahl malt ihren Hintergrund (SELECTH.TIM) VOR dem Masken-Pass
(render_pc.c "PLAYER-SELECT backdrop ... BEFORE the 3D models", danach Step 2 Tri+Masken); Titel und
Film liegen NACH dem Masken-Pass und decken sie zu (Titelbild gen1 vs gen4 bitgleich, 0 Pixel Diff).
Das Original zeichnet in keinem dieser Module eine Raum-Maske (R4).

### M2 Raumwechsel ROOM1020 -> ROOM1030 (Stand vor dem Entladen)
Lauf e: `RE15_DEBUG_JUMP=1020@5 RE15_GOTO_ROOM=1030 RE15_EXIT_AT=150#1030` (GOTO feuert 30 Bilder
nach dem Eintritt in 1020, beide Wechsel ueber re15_room_apply_pending). Ergebnis in ROOM1030,
alle 150 Bilder gleich:
```
BILD 1 gen=4 raum=1030 ... | fremd tim=4 | tim_slots 4 11 12f 26f 27f 36 37 46 47f
```
=> 4 TIM-Slots aus ROOM1020 bleiben gueltig: 12 + 47 = Gegnerbank 1 + ihr Gore-Slot (Typ, den es
in 1030 nicht gibt; re15_enemy_reset gibt die Bank frei, aber nicht ihre Textur-Slots), 26 + 27 =
Raum-Props obj 6/7 (der Teardown in room_pc.c invalidiert nur 4..9, nicht 26..35/45). Masken,
Atlas, SLD, ESP-Bank, Gegnerbanken, RDT sind beim Raumwechsel bereits sauber (fremd 0).

## RE-Belege (Adressen, Bytes)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis|bytes` auf `info/Re1.5/PSX.EXE`
+ eigener jal-Wort-Scan ueber die ganze EXE (jal-Wort = 0x0C000000|(ziel>>2)).

### R1 Der Raumlader FUN_800396fc gibt die Raum-Arena frei — bei JEDEM Aufruf
```
80039700: 0b 80 04 3c  lui a0,0x800b
80039704: 80 c7 84 8c  lw  a0,-0x3880(a0)      ; a0 = *0x800ac780 = Arena-BASIS
80039738: 7c c7 24 ac  sw  a0,-0x3884(at)      ; 0x800ac77c (Bump-Kopf) = Basis
80039740: 78 c7 24 ac  sw  a0,-0x3888(at)      ; 0x800ac778 (RDT-Zeiger) = Basis
80039748: b0 be 24 ac  sw  a0,-0x4150(at)      ; 0x800bbeb0 = Basis
800397e8: jal 0x80013b60                       ; neue RDT AB DER BASIS laden (ueberschreibt alles Alte)
8003996c: jal 0x80019354                       ; Effekt-Slots/ESP-Bank des Raums neu (96x0x84 nullen)
800399cc: jal 0x80039270                       ; sprite.pri-Tabelle NEU in der Arena anlegen
```
FUN_80039270 (`RE_15_Quellcode_V2/FUN_80039270.c`): `DAT_800b2584 = DAT_800ac77c` (Masken-Tabelle =
Arena-Kopf), Groessen aus RDT-Kopfbyte 7 — die Maskentabelle IST Raum-Arena.

### R2 Zwei (und nur zwei) Aufrufer des Raumladers — Tuer UND Spielstart
jal-Wort 0x0C00E5BF gescannt: genau `@0x8001d5ac` und `@0x8001d988`.
* `@0x8001d988` in FUN_8001d600 (Tuer-/Raumwechsel, XREF ghidra1_V2.txt:137568).
* `@0x8001d5ac` in FUN_8001d22c = Init des Spielmoduls, gerufen `jal 0x8001d22c` @0x8001c96c als
  erste Tat von FUN_8001c958 (Spielmodul-Schleife). Diese Init laeuft bei JEDEM Eintritt ins Spiel
  (NEW GAME, LOAD/CONTINUE — der Tod verlaesst das Modul, s. R4). Sie setzt die Arena ZUSAETZLICH
  selbst zurueck, bevor sie Spieler und Raum laedt:
```
8001d248: ori a0,zero,0x2 ; 8001d24c: jal 0x80021634 ; 8001d250: addu a1,zero,zero  ; Modus-2-Schwarz
8001d580: jal 0x8001923c                       ; globale Effektbank CORE00
8001d590: 0b 80 02 3c  lui v0,0x800b
8001d594: 80 c7 42 8c  lw  v0,-0x3880(v0)      ; Arena-Basis 0x800ac780
8001d5a0: 7c c7 22 ac  sw  v0,-0x3884(at)      ; Bump-Kopf 0x800ac77c = Basis
8001d5a4: jal 0x800314b0                       ; Spieler laden
8001d5ac: jal 0x800396fc                       ; Raumlader (R1)
8001d5cc: jal 0x80021634 (a0=0,a1=0)           ; BG frei
8001d5c8: sb 1,0x800b5457                      ; BG/Masken-Aufbau beim Present
```
=> Im Original ist "Tod -> NEW GAME" fuer die Raumdaten dasselbe wie ein Raumwechsel: die Arena wird
ab der Basis neu belegt; nichts aus dem alten Spielstand ist danach adressierbar.

### R3 Die Masken-Zeichenzahl steht IN der aktuellen RDT
FUN_80039590 (Zeichner):
```
80039590: lui v0,0x800b ; 80039594: lw v0,-0x3888(v0)   ; v0 = RDT-Basis 0x800ac778
800395c0: lbu s4,0(v0)                                  ; Zeichenzahl = RDT-Byte 0
800395c8: lw  s3,0x2584(s3)                             ; Tabelle 0x800b2584 (Arena, R1)
800395cc: beq s4,zero,0x80039678                        ; 0 -> nichts zeichnen
```
FUN_800392d4 (Aufbau, einziger Aufrufer `jal` @0x80021c28 im Cut-BG-Lader FUN_80021bbc):
`@0x80039334 j 0x8003955c / @0x80039338 sb zero,0(a0)` (NULL-Sektion -> Zahl 0) bzw.
`@0x80039358 sb t2,0(a0)` (deklarierte Zahl). Die Zahl lebt also in den Bytes des RESIDENTEN Raums;
mit der neuen RDT (R1/R2) ist sie neu.

### R4 Masken werden NUR im Spielmodul gezeichnet
Einziger Aufrufer des Zeichners: jal-Wort-Scan -> genau `@0x8001ce54` (`64 e5 00 0c jal FUN_80039590`),
im Bildrumpf der Spielmodul-Schleife FUN_8001c958 (Gate `@0x8001ce3c-4c`: `DAT_800aca38 & 0x100000`).
Die Schleife laeuft, solange `DAT_800aca38 & 0x40000000` (`@0x8001d1e8-f8 bne v0,zero,0x8001c97c`);
danach `jal 0x80021eb4` @0x8001d200 + `jal 0x80029a28(a0=0)` @0x8001d208 = Modul verlassen.
Titel, Charakterwahl, Optionen, Speicherkarte, Film sind andere Module -> dort zeichnet das Original
KEINE Raum-Maske.

## Umsetzung (Dateien, Konstanten)

Prinzip: der PC hat keine Arena, sondern Caches mit Prozess-Lebensdauer. EIN Aufruf
`re15_entladen_ereignis(anlass)` ist das Gegenstueck zum Arena-Reset und sitzt an GENAU den Grenzen,
an denen das Original den Raumlader bzw. die Modul-Init laufen laesst (R1/R2/R4):

| Grenze | Port-Stelle | Original |
|---|---|---|
| `raum` | room_pc.c `re15_room_load`: nach erfolgreichem Lesen+Parsen, VOR Installation der neuen RDT (Ladefehler laesst den alten Raum unversehrt) | Arena-Reset @0x80039738 vor RDT-Laden @0x800397e8 (Tuer @0x8001d988) |
| `spielstart` | main.c Boot-Block vor `re15_bg_init()` / Boot-RDT (NEW GAME, LOAD, erster Start) | Spielmodul-Init FUN_8001d22c: Arena @0x8001d590-a0, dann Spieler @0x8001d5a4, Raum @0x8001d5ac |
| `spielende` | main.c vor `goto re_title` (Tod -> Titel) | Modul-Schleife endet @0x8001d1f8, Modul verlassen @0x8001d200/@0x8001d208; Masken-Zeichner nur @0x8001ce54 |

Was faellt (platform/pc/src/entladen_pc.c `alles_entladen`), je mit Original-Beleg:
1. Maskenliste + Vordergrund-Atlas + alle RAUM-TIM-Slots (render_pc.c `re15_render_pc_entladen_raum`):
   Props 4..9/26..35/45, Gegner 10..18 + Gore 46..49, Tuersequenz 24/25, Raum-ESP 36..43.
   Masken-Tabelle = Arena (`jal 0x80039270` @0x800399cc), Objekt-Slots per Flagwort genullt
   (FUN_8003ea7c @0x8003eab0-acc). Texturen werden sofort UNGUELTIG (loaded=0) und am naechsten
   Bildanfang zerstoert (`re15_render_pc_entladen_freigeben` in `re15_render_begin_frame`) — die
   Dreiecksliste des laufenden Bildes kann den Slot noch referenzieren. NICHT angefasst: 0..3
   (Spieler, Elliot, Heli/Pilot — Boot-Lader), 19..23/44/50..55 (globale Effektseiten, Spielstart).
   **Korrigiert in Nachbesserung 1:** 1..3 sind Raum-Slots (Elliot = Sce_em_set-Modell, Heli/Pilot =
   RDT-Objekte); nur 0 (Spieler, fester Puffer 0x801bd814) bleibt.
2. SLD-Atlas-Auszug (bg_pc.c `re15_pri_sld_entladen`) + nachgezeichnete Masken (MSK-Cache aus
   main.c nach entladen_pc.c verlegt: `re15_entladen_msk`).
3. Gegnerbanken `re15_enemy_reset()` (idempotent; Raumwechsel/Boot riefen es schon, jetzt auch Tod).
4. Effekte: `re15_esp_fx_reset`, Raum-Bank NULL, `re15_esp_pool_reset`, `re2fx_reset`
   (FUN_80019354 @0x80019378 / @0x80019388-e4, einziger Aufrufer @0x8003996c).
5. Lichtset (`g_re15_room_lights_ok=0`) + Raum-Nachrichten (`re15_msg_clear_room_block`), beide aus der
   RDT reloziert (@0x800397fc-834).
6. Raum-Tonbaenke: audio_pc.c — Freigabe-Haelfte von `re15_audio_load_room_banks` als eigene
   Funktion `re15_audio_raum_entladen` (Inhalt unveraendert; Original SsVabClose in FUN_80043eac /
   FUN_80043fb0, gerufen @0x80039974/@0x8003997c).
7. RDT-Bytes (room_pc.c `re15_room_pc_entladen`, + `g_room_rdt` genullt). Die Boot-RDT gehoert jetzt
   ebenfalls room_pc (`re15_room_pc_uebernehmen`, main.c Boot) — vorher wurde sie nie freigegeben.

Zwei Riegel in main.c, die einen Raumwechsel ueberlebten:
* PRI-Riegel (s_pri_room/s_pri_cut) vergleicht zusaetzlich die Generation — nach dem Entladen wird
  die Maskenliste im ersten Spielbild neu abgeleitet, AUCH wenn (Raum,Cut) gleich bleibt (Tod ->
  LOAD im Todesraum: sonst fehlten die Masken bis zum naechsten Cut-Wechsel).
* RBJ-Riegel (s_rbj_room): nach jedem Entladen ungueltig. Original bindet die Raum-Animation bei
  JEDEM Raumladen aus der neuen RDT neu (`jal 0x8001b3f8` @0x80039a08, unbedingt; FUN_8001b3f8 liest
  RDT+0x5C `lw a2,92(v0)` @0x8001b404). Vorher: Tod im Raum X -> neues Spiel -> erster Tuer-Raum X
  behielt die Cinematic-Bank des Boot-Raums.

Haken in gemeinsamen Dateien (je 1-5 Zeilen): main.c (include, ESP-Bank-Merker, `spielstart`,
`spielende`, MSK-Aufruf, PRI-Riegel, RBJ-Riegel, Boot-RDT-Uebergabe), render_pc.c (include,
Generation in set_pri_rects/set_pri_atlas/upload_tim_slot, Freigabe am Bildanfang, Messschiene im
Masken-Pass, Block mit Zugriffs-/Entladefunktionen), bg_pc.c (Generation + 2 Funktionen),
room_pc.c (Grenze `raum`, Generation, 3 Funktionen), audio_pc.c (Funktion geteilt + Zensus-Zugriff),
enemy_common.c (Generation je Bank). Neue Dateien: include/re15_entladen.h,
engine/src/entladen_common.c, platform/pc/src/entladen_pc.c.
Keine neue Verhaltens-KONSTANTE (nur Slot-Bereiche, die aus der dokumentierten Slot-Karte
render_pc.c RE15_TIM_SLOT_MAX abgeleitet sind).

## Messung nachher

Gleiche Laeufe, gleiche exe-Kopie, Stand 02397874 + RDT-Schritt (Logs `I_entladen_bilder/nach_*`).
* Lauf a (Tod 1020 -> NEW GAME -> Tod 1240 -> NEW GAME): alle EREIGNIS-Zeilen `belegt` 0 in allen
  12 Faechern; alle SUMME-Zeilen `bilder_fremd_belegt=0 bilder_fremde_masken_gezeichnet=0`; keine
  BILD-Zeile. In ROOM1020 weiterhin `bilder_mit_masken=116` (die eigenen Masken bleiben).
* Lauf d (Tod -> Titelmenue -> Charakterwahl): `SUMME seit=spielende bilder=599 bilder_mit_masken=0
  bilder_fremde_masken_gezeichnet=0` (vorher 599/599). Bild 500: Raum-Stueck rechts WEG
  (`nach_auswahl_b500.png`); Differenz vorher/nachher genau im Maskenbereich: Bild 400 bbox
  (198,0)-(320,153) 1178 px, Bild 500 bbox (187,0)-(320,158) 7233 px. In Bild 400 ist der
  Titelbalken "PLEASE SELECT MAIN CAST" jetzt vollstaendig (vorher von einem Masken-Stueck
  angeschnitten).
* Lauf e (1020 -> 1030): `EREIGNIS raum` belegt 0; keine BILD-Zeile in 150 Bildern ROOM1030
  (vorher 4 fremde TIM-Slots 12/26/27/47 in jedem Bild). Eigene Masken 1030 Cut 6: 22, gezeichnet.

### M3 Cinematic-Bank nach Tod (RBJ-Riegel) — Messung + Gegenprobe
Lauf: `RE15_TITLE_SHOT=t.bmp RE15_GOTO_ROOM=1170 RE15_KILL_AT=1500 RE15_BOOT_EXIT_AT=3`. Spiel 1:
1240 -> (GOTO, F30) 1170 -> Tod in 1170. Spiel 2: NEW GAME -> Montage 1240 -> echte Tuer nach 1170
(F1421, gemessen mit RE15_FADE_LOG `TUER-EINTRITTS-FADE`) -> Tod. debug.log:
* NACHHER (Riegel je Generation): Spiel 2 Zeile 271 `[rbj] room 1170 cinematic overlay: 26 clips, 366 kf`.
* GEGENPROBE (nur die Riegel-Zeile entfernt, exe re15_pc_gegenprobe.exe, danach `git checkout`):
  Spiel 2 betritt 1170 (`[room] PC loaded room1170.rdt`), aber KEINE `[rbj] room 1170`-Zeile —
  die Helipad-Intro-Bank wurde nicht gebunden, Leon/Elliot spielten das Intro mit der Bank des
  Boot-Raums (ROOM1240: keine Datei -> PL00-Basis). Original: `jal 0x8001b3f8` @0x80039a08 bindet
  bei JEDEM Raumladen neu.
* Masken im Spiel 2 ROOM1170 (echte Tuer): `bilder_mit_masken=847`, fremd 0.

## Tests

Registriert NUR in `re15_port/tests/unit/probes/r35_entladen.cmake`:
| Test | misst | Ergebnis (einzeln) |
|---|---|---|
| unit_r35_entladen_beleg | Original-Bytes in info/Re1.5/PSX.EXE: `@0x80039704/38/40` Arena-Reset, `@0x8001d5a0`, `jal 0x80039270` @0x800399cc, `jal 0x8001b3f8` @0x80039a08; Voll-Scan: genau 2x `jal 0x800396fc` (@0x8001d5ac/@0x8001d988), genau 1x `jal 0x80039590` (@0x8001ce54) | Passed |
| unit_r35_entladen_gegner | Generation je Gegnerbank (re15_enemy_alloc), FREMD nach der Grenze, leer nach re15_enemy_reset, Ueberlauf -> 1 | Passed |
| integration_r35_entladen_a | Tod 1020 -> NEW GAME -> Tod 1240 -> NEW GAME: jede EREIGNIS-Zeile belegt 0, jede SUMME fremd 0, keine BILD-Zeile, ROOM1020 zeichnet eigene Masken, Titel nach Tod 0 Masken | Passed 21 s |
| integration_r35_entladen_b | Raumwechsel 1020 -> 1030 -> Tod: dito, ROOM1030 zeichnet eigene Masken | Passed 16 s |
| integration_r35_entladen_c | Tod -> LOAD im Todesraum (Karte ROOM1020 (-26000,0,-8700) Cut 0): beide Spiele zeichnen ihre Masken (137/137 Bilder), fremd 0 | Passed 39 s |
| integration_r35_entladen_d | Tod in 1170 -> NEW GAME -> Montage-Tuer 1170 (240 Bilder/s): Cinematic-Bank 1170 bei JEDEM Betreten gebunden (2/2), fremd 0 | Passed 50 s |

Nachtrag Hintergrundbild (Commit 5955ae71): Fach `bg` im Zensus (bg_pc Generation beim Dekodieren);
Lauf a danach: alle 6 EREIGNIS-Zeilen `belegt ... ton=0 bg=0`, alle SUMME fremd 0, ROOM1020 116
Bilder mit eigenen Masken.

SUITE (Stand 5955ae71, eigener Bau, `bash re15_port/tools/local_build.sh all`, 00:26-00:47):
`=== LOCAL-BUILD-OK (all) — Tests 484/484` (Beleg im eigenen Baum:
re15_port/build/local_build_ctest.log `100% tests passed, 0 tests failed out of 484`,
`Total Test time (real) = 1237.52 sec`, Exit 0). Kein Flatter-Haken rot.
⚠ Sitzungsnotiz: der Scratchpad ist zwischen den Spuren GETEILT — die Spur "karte" schrieb in
dieselben Dateinamen (suite*.log, commit_final.txt). Ein frueheres "482/482" im geteilten
suite1.log war NICHT mein Lauf; massgeblich ist nur das ctest-Log im eigenen build/.

Gegenproben (Fix-Zeile temporaer entfernt, gebaut, gemessen, `git checkout` zurueck):
* PRI-Riegel ohne Generation -> integration_r35_entladen_c FAILED: "Masken nach dem Laden fehlen
  (Spiel 1: 137, Spiel 2 nach Tod im selben Raum/Cut: 0 Bilder)".
* RBJ-Riegel ohne Generation -> Spiel 2 ohne `[rbj] room 1170`-Zeile (M3) = Bedingung von Lauf d.
* Ohne Entladen (Messstand c21e38f8/9b36e714) liefern dieselben Laeufe die Vorher-Zahlen aus M1/M2
  (fremde Masken in 23/599 Bildern, fremde TIM-Slots) -> a und b schlagen dort an.

## OFFEN

* O1 PSX-Port: der PSX-Teil (platform/psx) hat seinen eigenen Raumlader (asset_psx.c/room_common.c
  reset_render) und die echte Arena; entladen_common.c (Generation) baut dort mit, der PC-Teardown
  entladen_pc.c nicht. Nicht gebaut/gemessen (PSn00bSDK-Bau ist hier nicht Teil der Suite).
  Naechster Messweg: PSX-Build + gleiche Laeufe ueber DuckStation-Savestate-RAM (0x800ac77c nach
  Tuer == 0x800ac780 + RDT-Groesse).
* O2 (KORRIGIERT in Nachbesserung 1 — Elliot/Heli/Pilot fallen jetzt, s. dort; die Begruendung
  "Boot-Lader" war ohne Beleg und fuer 1..3 falsch). Bewusst NICHT entladen bleiben: TIM 0 (Spieler:
  fester Puffer 0x801bd814 @0x800314c8/cc, geladen nur am Spielstart @0x8001d5a4 bzw. bei
  Figurwechsel im Raumlader), 19..23/
  44/50..55 (globale Effektseiten CORE00/TEX.TIM, Spielmodul-Init `jal 0x8001923c` @0x8001d580),
  CDEMD0.EMS-Archive (Prozess-Cache einer CD-Datei, wie der CD-Inhalt selbst), STAGE%u.BIN-Tabelle
  (bg_pc, je Stage), Tuer-TONBANK (RE2: Key-Off nur fuer Raum-SPU-Bereich @0x800597a4-0x80059810,
  Tuerbank 0x3DC50 ueberlebt den Raumwechsel — unveraendert aus Runde 31).
* O3 Messung "Tod -> LOAD in einem ANDEREN Raum" und "Raumwechsel ueber eine echte Tuer mit
  Tuersequenz (TIM 24/25)" nicht einzeln gefahren; beide laufen durch dieselben Grenzen
  (spielstart bzw. raum), die Faecher sind dieselben. Naechster Messweg: Lauf C mit Karte in Raum X
  und Tod in Raum Y; Tuersequenz per RE15_FIRE_AOT an einer Kreuz-Raum-Tuer + RE15_ENTLADEN_LOG.

## Fuer den Nutzer

* Keine Sprachdateien, keine neuen Assets (Paket-/Android-Gate unveraendert).
* Neue Quelldatei platform/pc/src/entladen_pc.c + engine/src/entladen_common.c: der Android-Bau
  cacht die GLOB-Liste (Memory reai-v2-android-glob-cache) -> dort einmal neu konfigurieren.
* Bedienung: nichts. Wirkung: nach dem Tod sind Titel, Charakterwahl und das neue Spiel frei von
  Masken/Texturen/Modellen/Effekten/Tonbaenken/RDT des alten Spielstands; nach jedem Raumwechsel
  dito fuer den Raum davor.
* Messschiene fuer eigene Befunde: `RE15_ENTLADEN_LOG=entladen.log` (Zeilen EREIGNIS/SUMME/BILD,
  s. Messung vorher), Bildbeleg `RE15_ENTLADEN_SHOT=<praefix>` + `RE15_ENTLADEN_SHOT_BILD=5,40,...`.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-04)

Abnahmebericht `I_abnahme_0.md`: Punkt 1 erfuellt, Punkt 2 teilweise. Zwei Maengel:
M1 Raum-Stimmen-Cache (`s_voice_clip`/`s_voice_room`, audio_pc.c) faellt an keiner Grenze;
M2 Ausnahme TIM 0..3 (Elliot, Heli/Pilot) ohne Original-Beleg.
Stand zu Beginn: HEAD 20ced3e7, Baum sauber.

### N1-M1 Raum-Stimmen

**Ursache.** Der Port dekodiert jede Raum-Stimme (`synchro/STAGEn/room<id>/mainNN.wav`) in den Cache
`s_voice_clip[64]` (audio_pc.c), Schluessel `s_voice_room`. Freigegeben wurde er nur in
`re15_voice_load_clip`, wenn eine Zeile in einem ANDEREN Raum angefordert wird. Die Grenzen
(raum/spielstart/spielende) kannten den Cache nicht; der Zensus hatte kein Fach dafuer.

**Messung vorher** (Abnahme 0, nicht wiederholt): `SDL_AUDIODRIVER=dummy` Intro 1240 -> 1170:
main00..05 aus ROOM1240 geladen (debug.log 67..255, ~3,2 MB PCM), `EREIGNIS raum gen=3` meldet
`ton=0`, 300 Bilder ROOM1170 ohne neue Zeile -> die sechs Clips bleiben resident.

**Beleg (Ton = RE2, RE1.5 hat keine Stimme: SCD 0x59 ist dort ein Flag-Opcode @0x8003fe90).**
Werkzeug `re2_disasm.py` + eigener jal-/Speicher-Scan ueber `info/re2leon/PSX.EXE`.
```
RE2 Abspielen FUN_800129b4 (Aufrufer jal @0x800338a4, @0x80058048; Overlays OPENING/DIEDEMO/STAGE6)
  DsCommand(9 Pause) / (0x0D Setfilter, &0x800d5b48) / (0x0E Setmode, Byte @0x8009a415 = 0xC8)
  / (0x15 SeekL) / (0x1B ReadS)
  0x8009a410: 00 00 00 00 80 c8 00 00 ...  -> 0x8009a414 = 0x80 (Init), 0x8009a415 = 0xC8
  0x8009a428: 00 a0 a0 00                  -> 0x8009a429 = 0xA0 (Datei-Lesen), 0x8009a42a = 0xA0
RE2 CD-Datei-Leser FUN_80012fb8 (41 Aufrufer)
  800130d4: addiu a0,zero,9   / 800130e0: jal 0x8008a380 (DsCommand)   ; Pause
  800130f0: addiu a0,zero,14  / 80013100: jal 0x8008a380               ; Setmode 0xA0
  80013110: addiu a0,zero,21  / 80013120: jal 0x8008a380               ; SeekL
  80013140: addiu a0,zero,6   / 80013150: jal 0x8008a380               ; ReadN
RE2 Raumlader FUN_80049e48 (RE2_Quellcode_V2/FUN_80049e48.c:98)
  8004a1ac: lw v0,0x7210(at) 0x800a7210 (Datei-Index je Stage/Raum) / 8004a1c4: jal 0x80012fb8   ; RDT lesen
  8004a2f4: jal 0x80059e54 (Raum-Ton-Init) / 8004a33c: jal 0x8005a09c
psx-spx cdromdrive.md "Setmode": Bit 6 XA-ADPCM (0=Off, 1=Send XA-ADPCM sectors to SPU Audio Input).
RE2 Stimm-Zustand: Tabelle @0x8009a418 = {0x80012ca4 (0 Ruhe: sb zero 0x800d5334/5301/5335,
  0x800cfbd8 &= ~0x20), 0x80012cd8, 0x80012e70, 0x80012f48 (3 Stopp: DsCommand 9, Zustand 0)}.
```
=> In RE2 liegt nie eine dekodierte Stimme im RAM (CD-XA geht vom Laufwerk in den SPU-Eingang), und
der Raumlader programmiert das Laufwerk fuer die neue RDT um (Pause, Setmode ohne Bit 6, ReadN):
ab dem Raumladen erreicht keine Stimme des alten Raums mehr die SPU. Eine explizite Stopp-
Anforderung (FUN_80012c2c, setzt Zustand 3) ruft der Raumlader NICHT — es ist das Umprogrammieren
des Laufwerks, das die Stimme beendet. Port-Gegenstueck: an der Grenze Clips frei + Strom loesen.

**Aenderung.** audio_pc.c `re15_audio_stimmen_entladen()` (unter `SDL_LockAudioDevice`: Strom
loesen, falls er aus einem Clip liest; alle 64 Clips frei, `tried` 0, `s_voice_room` 0) und
`re15_audio_stimmen_belegt(&gen,&laeuft)`; Generation beim Clip-Laden (`re15_audio_stimme_gen_setzen`).
entladen_pc.c Schritt (9) ruft das Entladen; Zensus-Fach `stimme` (mit Generation); VORHER-Zeile
zusaetzlich `stimme_laeuft=0/1` (lief an der Grenze gerade eine Zeile?).

### N1-M2 Elliot / Heli / Pilot (TIM 0..3)

**Ursache.** O2 nahm TIM-Slots 0..3 aus ("Boot-Lader"). Der PC lud Elliot (PLD/ELLIOT.*, Slot 1)
und Heli/Pilot (Objekte 2/5 der Boot-RDT, Slots 2/3) einmal je Spielstart und hielt sie ueber
jeden Raumwechsel; der Zensus zaehlte 0..3 per Definition nicht.

**Messung vorher** (Abnahme 0): `[elliot] PL05 loaded ... [tim] elliot TIM in slot 1` bei JEDEM
Spielstart, auch im Boot-Raum ROOM1240 ohne Elliot. Heli/Pilot: Slots 2/3 aus der Boot-RDT
(bei Boot 1240 leer). Eigene Messung mit dem neuen Zensus: s. "Messung nachher (N1)" (VORHER-Zeilen).

**Beleg (RE1.5 PSX.EXE, `re15_disasm.py`, eigener Speicher-/jal-Scan).**
```
Arena-Basis: FUN_80039a30 (jal @0x8001d588 Spielstart, @0x8001d80c, @0x8001d980 Tuer)
  80039a44: jal 0x800299a4 (a0=2, a1=Stage+1: Stage-Overlay laden, v0 = Groesse)
  80039a4c: lui v1,0x8010 / 80039a50: addu v1,v0,v1 / 80039a58: sw v1,-0x3880(at)  0x800ac780
  (einziger Schreiber von 0x800ac780: Speicher-Scan)  -> Arena = 0x80100000 + Overlay-Groesse ...
Sce_em_set (0x44) @0x800420a0 — JEDES gesetzte Modell in die Arena:
  800422c0: lui s1,0x800b / 800422c4: lw s1,-0x3884(s1)   0x800ac77c   ; Arena-Kopf
  800422dc: sw s1,124(s0)                                  ; entity+0x7C = Modellbasis
  800422f4: addiu s1,s1,12
  80042328: jal 0x80022300  (a3 = s1)                      ; EMD laden
  80042554: sw s1,-0x3884(at)  0x800ac77c                  ; Kopf hinter das Modell
FUN_80022300 (RE_15_Quellcode_V2/FUN_80022300.c): Datei 0x26 (CDEMD0.EMS) bzw. 0x27, Eintrag
  (Typ-0x10) aus 0x80072f38/0x80073178, FUN_80013c50(param_4 = Arena) liest, Zeiger +0x1b0/+0x16c/
  +0x84/+0x174/+0x170/+0x17c/+0x178 in DIESEN Puffer, TIM per FUN_80022150 ins VRAM.
Spieler dagegen: FUN_800314b0
  800314c8: lui a1,0x801b / 800314cc: ori a1,a1,0xd814    ; fester Puffer 0x801bd814
  -> jal 0x80013b60 (PLD lesen). Aufrufer: Spielstart @0x8001d5a4 und im Raumlader NUR wenn
  (0x800aca5c & 0xf) != 0 (Figurwechsel, RE_15_Quellcode_V2/FUN_800396fc.c:15-18).
RDT-Objekte (Heli = Objekt 2, Pilot = Objekt 5 von ROOM1170): Teil der RDT, die ab der Arena-Basis
  geladen wird (`lw a1` 0x800ac778 @0x800397c0, `jal 0x80013b60` @0x800397e8).
```
=> Elliot (jedes Typ-0x47-Modell) und Heli/Pilot leben im Original in der Raum-Arena und sind mit
dem Arena-Reset @0x80039738 weg. Die O2-Ausnahme stimmt NUR fuer Slot 0 (Spieler, fester Puffer)
und die globalen Effektseiten (19..23/44/50..55, `jal 0x8001923c` @0x8001d580). O2 ist damit
korrigiert: Slots 1/2/3 sind Raum-Slots.

**Aenderung.** Neue Datei `platform/pc/src/elliot_pc.c`: die Lade-Schritte des frueheren
Boot-Laders (PLD/ELLIOT.MD1/EDD/EMR/TIM, gleiche Parser, TIM -> Slot 1) laufen jetzt beim SPAWN
eines Typ-0x47-Aktors (main.c Roster-Vorladen nach scd_vm_tick, wo jedes Sce_em_set-Modell
nachgeladen wird == `jal 0x80022300` @0x80042328), inkl. Dialog-Gesten-Overlay aus der gebundenen
Raum-RBJ (`re15_rbj_room`, neue 1-Zeilen-Abfrage in enemy_common.c; dieselbe Rechnung wie
re15_apply_room_cinematic Schritt 2) und `re15_npc_set_elliot_anim`. Entladen an jeder Grenze
(entladen_pc.c Schritt 10): Puffer frei, Strukturen genullt, Executor-Zeiger NULL. main.c behaelt
die Strukturen (Zeichen-/Anim-Wege lesen sie per Adresse) und meldet sie an
(`re15_elliot_pc_anmelden`); der Boot-Lader (31 Zeilen) ist entfernt. render_pc.c
`re15_render_pc_tim_slot_raum`: Slots 1..3 jetzt Raum-Slots (entladen + gezaehlt). Zensus-Fach
`figur` (Elliot-Modell, Generation). Aussehen unveraendert (Port-Wahl R23: PL05 statt EM047-Mesh).

### Messung nachher (N1) — Stand 86f65b95, eigener Bau

Laeufe mit eigener exe-Kopie (`scratchpad/i_lauf.sh`, PATH msys64 zuerst, eigenes Arbeitsverzeichnis).
Zeilen gekuerzt (`| fremd ...` und die Masken-Faecher weggelassen, alle dort 0).

**M1 Stimmen** — Lauf r2 in NORMALTEMPO (`SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_TITLE_SHOT=t.bmp
RE15_TITLE_SHOT_AF=60 RE15_EXIT_AT=30#1170`):
```
VORHER raum gen=2 raum=1240 | belegt tim=2 ... rdt=1 ton=2 bg=1 stimme=6 figur=0 | tim_slots 36 37 | stimme_laeuft=0
EREIGNIS raum gen=3 raum=1240 | belegt ... ton=0 bg=0 stimme=0 figur=0
```
Die sechs ROOM1240-Clips (main00..05, Vorbedingung des Mangels) fallen an der Grenze; die letzte
Zeile war vor der Tuer zu Ende (`stimme_laeuft=0`) — im echten Ablauf wird nichts Hoerbares
abgeschnitten. Mit `RE15_FPS=240` (Lauf r1, Pin e) steht an derselben Grenze `stimme_laeuft=1`: das
Spiel laeuft dann viel schneller als das Echtzeit-Tongeraet, main05 (242550 Samples = 5,50 s) ist
noch nicht durch. Dort wird der Strom geloest — das ist die Wirkung des RE2-Raumladers (Laufwerk auf
ReadN ohne XA-Bit). Danach 30 bzw. 300 Bilder ROOM1170: keine BILD-Zeile (nichts Fremdes), `stimme=0`.

**M2 Elliot** — Lauf r3 = Pin d (`RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_GOTO_ROOM=1170
RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3`):
```
VORHER raum gen=5 raum=1240     | belegt tim=2 ... figur=0 | tim_slots 36 37        <- Elliot in 1240 NICHT resident
debug.log 417: [elliot] PL05 loaded: 15 meshes, 15 bones, 24 clips (Raum 1170, Spawn 0x47)
          418: [elliot] Raum-RBJ-Overlay: 26 clips / 419: [tim] elliot TIM in slot 1: 256x256
VORHER spielende gen=6 raum=1170 | belegt tim=8 gegner=1 ... figur=1 | tim_slots 1 4 5 6 7 8 9 11
EREIGNIS spielende gen=7 raum=1170 | belegt (alle 15 Faecher) 0
```
Vorher (Abnahme 0): `[elliot] PL05 loaded` + Slot 1 bei JEDEM Spielstart, auch in ROOM1240.
(Spiel 1 dieses Laufs kommt per RE15_GOTO_ROOM nach 1170; dieser Debug-Sprung faehrt das
Helipad-Intro nicht, Elliot wird bis zum Tod bei Bild 3200 nicht gesetzt -> dort figur=0. Unveraendert.)

**M2 Heli/Pilot-Slots 2/3** — Pin c (Tod -> LOAD im Todesraum ROOM1020):
```
VORHER spielende gen=2 raum=1020 | belegt tim=16 gegner=2 ... | tim_slots 2 3 4 5 6 7 8 9 11 12 26 27 36 37 46 47
EREIGNIS spielende gen=3 raum=1020 | belegt ... tim=0 ...
```
Slots 2/3 tragen hier die Objekte 2/5 der Boot-RDT ROOM1020 (nicht Heli/Pilot — der alte Lader
schneidet blind prop[2]/prop[5] der jeweiligen Boot-RDT). Vorher wurden sie weder gezaehlt noch je
entladen und ueberlebten jeden Raumwechsel.

**Aussehen unveraendert (A/B)** — exe des Stands VOR N1 (20ced3e7, `git archive` in den Scratchpad,
eigener Bau, `RE15_CD_ROOT` auf diesen Baum) gegen N1, gleicher Lauf (`RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_EXIT_AT=1700#1170
RE15_FRAMEDUMP=880-1600/20:fd_`, Rueckleser vor dem Present): **37/37 Bilder bytegleich**, darunter
die Helipad-Szene mit Elliot im Bild (`I_entladen_bilder/n1_elliot_1170_b960.png`, "Elliot: We must
go! ..."; debug.log `[F950-elliot-root] screen=(102.2,108.4)`).
Erster Wurf: das Laden nur in der Roster-Schleife lieferte EIN Bild mit `[enemy-diag] actor1
type=0x47 model=LEON-FALLBACK` — im Spawn-Bild laeuft der Render-Durchgang vor der Roster-Schleife
(main.c-Kommentar an pc_enemy_load_ex: "DIESE Stelle ist die frueheste"). Zweiter Haken dort
(1 Zeile); danach `[enemy-diag] actor1 type=0x47 model=elliot` schon beim ersten Zeichnen.

### Tests (N1)
| Test | misst | Ergebnis (einzeln) |
|---|---|---|
| unit_r35_entladen_n1beleg (neu) | RE1.5: `8e31c77c` @0x800422c4, `jal 0x80022300` @0x80042328, `ac31c77c` @0x80042554, `3c05801b`/`34a5d814` @0x800314c8/cc, genau ein Arena-Basis-Schreiber `ac23c780` @0x80039a58 (Voll-Scan); RE2: `jal 0x80012fb8` @0x8004a1c4, DsCommand 9/14/21/6 @0x800130d4/f0/80013110/40, Byte 0xA0 @0x8009a429 (Bit 6 = 0), 0xC8 @0x8009a415 (Bit 6 = 1) | Passed |
| integration_r35_entladen_c (erweitert) | + Slots 2/3 am Tod belegt und gezaehlt, EREIGNIS 0 | Passed 38.7 s |
| integration_r35_entladen_d (erweitert) | + Elliot in ROOM1240 nie resident (figur=0, kein Slot 1), geladen nur "(Raum 1170, Spawn 0x47)", am Tod figur=1 + Slot 1 -> EREIGNIS 0 | Passed 45.5 s |
| integration_r35_entladen_e (neu) | Intro 1240 -> 1170 mit Ton (Dummy-Treiber): >= 1 Clip geladen, VORHER raum stimme >= 1, EREIGNIS raum stimme=0, keine BILD-Zeile | Passed 27.4 s |
| unit_r35_entladen_beleg/_gegner, integration_a/_b | unveraendert | Passed |

Gegenproben (N1-Zeilen temporaer entfernt: Schritt (9) und (10) in alles_entladen auskommentiert,
Slot-Bereich zurueck auf 4..18; gebaut, gefahren, `git checkout` zurueck, neu gebaut):
* e FAILED: `EREIGNIS raum gen=3 raum=1240 ... stimme=6` ("nach dem Entladen noch belegt")
* d FAILED: `EREIGNIS spielende gen=7 raum=1170 ... figur=1`
* c FAILED: "Slots 2/3 (Objekte 2/5 der Boot-RDT ROOM1020) nicht als Raum-Slots belegt/gezaehlt"
=> die Pins messen genau die beiden Maengel.

### OFFEN (N1)
* O4 main.c (Boot-Block, ca. Zeile 3995-4020) parst `heli_md1`/`pilot_md1` aus der Boot-RDT; beide
  werden NIRGENDS gelesen (grep: keine weitere Stelle; Slots 2/3 bindet niemand:
  `re15_render_pc_bind_tim_slot` nur mit 0, 21, Bank-, Prop- und Gore-Slots bzw. 1 fuer Elliot).
  Nach der ersten Grenze zeigen ihre Zeiger in die freigegebene Boot-RDT — totes, nie
  dereferenziertes Erbe. Das Entfernen (~28 Zeilen main.c) sprengt die 1-5-Zeilen-Regel fuer
  gemeinsame Dateien; naechster Schritt: in einer Aufraeum-Runde den Block streichen (Verhalten
  unveraendert, da ungenutzt). Die Slots selbst fallen seit N1.
* O5 Typ 0x47 in spaeteren Stages (laut Memory reai-v2-npc-ai = Annette EM047) zeichnet der PC
  weiterhin mit PLD/ELLIOT (Port-Wahl R23, unveraendert); die Lebensdauer folgt jetzt auch dort dem
  Spawn. Nicht Teil dieses Auftrags.
* O1/O3 unveraendert (PSX nicht gebaut; O3 "echte Tuer mit Tuersequenz" hat die Abnahme selbst
  gefahren: DOOR13, alle Faecher 0).
* main.c-Umfang: der Boot-Lader fuer Elliot (31 Zeilen) ist nach elliot_pc.c VERSCHOBEN, dazu
  3 Haken-Zeilen (Anmelden, Roster-Schleife, Render-Durchgang) — mehr als 1-5 Zeilen Diff, aber
  ueberwiegend Loeschung; die Logik liegt in der neuen Datei (VERTRAG 1.4).

### Fuer den Nutzer (N1)
* Keine Sprachdateien, keine neuen Assets (Paket-/Android-Gate unveraendert).
* Neue Quelldatei `platform/pc/src/elliot_pc.c` -> Android-Bau einmal neu konfigurieren (GLOB-Cache).
* Wirkung: nach Raumwechsel und Tod sind auch die gesprochenen Zeilen des Raums davor und Elliot
  (Modell + Textur) nicht mehr geladen; Elliot erscheint unveraendert, sobald er in ROOM1170 gesetzt
  wird. Messschiene: zwei neue Faecher `stimme` und `figur`, VORHER-Zeile mit `stimme_laeuft`.


### Suite (N1)
Eigener Bau, Stand ef842ef1 (Code = 86f65b95), `bash re15_port/tools/local_build.sh all`:
`=== LOCAL-BUILD-OK (all) — Tests 486/486` (Beleg: re15_port/build/local_build_ctest.log `100% tests passed, 0 tests failed out of 486`,
`Total Test time (real) = 1291.62 sec`). Kein Flatter-Haken rot, nichts nachgefahren.
484 -> 486: + unit_r35_entladen_n1beleg, + integration_r35_entladen_e.

## Nachbesserung 2 (nach Abnahme 1, 2026-10-04)

Abnahmebericht `I_abnahme_1.md`: Punkt 1 erfuellt, Punkt 2 teilweise. Mangel M1: die Raum-
Animationsbank (RBJ, z.B. `RBJ/ROOM1170.RBJ`, 55 060 B) bleibt nach Raumwechsel und Tod geladen
(main.c `static uint8_t *s_room_rbj` gibt nur frei, wenn der NAECHSTE Raum eine Animation hat; Boot-Pfad
`rbj_buf` wird nie freigegeben); der Zensus hat kein Fach dafuer. Die Abnahme-Messungen r9 (Tuer
1170->1130, gdb Bild 100) und r10 (Tod in 1170, gdb Bild 200 ROOM1240 des neuen Spiels) werden
NICHT wiederholt, sie sind die Messung vorher.
Stand zu Beginn: HEAD c5ead020, Baum sauber. Schritt 0: `git merge master` (170 Commits, Spuren
A/B/E/K/L) -> Merge 7f547d99; zwei Konflikte, beide "beide Seiten behalten":
`include/re15_enemy.h` (meine `re15_rbj_room` + L `re15_rbj_set_alias`), `engine/src/enemy_common.c`
(meine Include-Zeile re15_entladen.h + K re15_cut10f0.h). main.c/audio_pc.c ohne Konflikt.

### N2 Bestandsaufnahme nach dem Merge (Code gelesen, Zeilen Stand 7f547d99)
Drei Besitzer von Raum-Animationsbloecken:
1. main.c:8121 `static uint8_t *s_room_rbj` (Tuer-Weg, Datei `RBJ/ROOM%04X.RBJ`); frei nur in
   main.c:8151 (naechster Raum MIT Block). Zweig "has no RBJ" (main.c:8167ff) und Tod: liegen lassen.
2. main.c:4197 `uint8_t *rbj_buf = pc_read_shared(rbj_path, ...)` (Boot-Weg, auch `RE15_RBJ`): nie frei.
3. NEU durch Spur K (master): platform/pc/src/cut10f0_pc.c:24 `static uint8_t *s_leih_buf` — die GANZE
   RDT-Datei von ROOM11B0, deren Block @0x5C ROOM10F0 leiht; frei nur bei der naechsten Leihe. Nach
   ROOM10F0 -> anderer Raum bzw. Tod bleibt sie liegen (gleicher Mangel, dritter Weg).
RDT-Alias (`rbj_borrowed`, Block IN der residenten RDT) faellt schon mit Schritt (7) (RDT-Bytes).

### N2 RE-Belege (selbst disassembliert, `re15_disasm.py` / `re2_disasm.py`)
```
RE1.5 PSX.EXE — Raum-Animationsbank = Teil der RDT
  8001b3f8: lui v0,0x800b
  8001b3fc: 78 c7 42 8c  lw v0,-14472(v0)   0x800ac778 = RDT-Zeiger (= Arena-Basis, R1)
  8001b404: 5c 00 46 8c  lw a2,92(v0)       RDT+0x5C = Animationsblock
  8001b40c: 33 00 c0 10  beq a2,zero,...    kein Block -> nichts binden
  80039a08: fe 6c 00 0c  jal 0x8001b3f8     im Raumlader FUN_800396fc, unbedingt (nach jal 0x8003ef6c @0x80039a00)
  800397c0: lw a1,-14472(a1) / 800397e8: jal 0x80013b60   neue RDT ab der Basis; Reset @0x80039738 (R1)
RE2 info/re2leon/PSX.EXE — ENEMSE-Bank je Raum (fuer H2), jal-Wort-Scan selbst:
  8004a334: jal 0x80053528  (einziger Aufrufer)  -> 80053610: jal 0x80052b38  (Bank-Zeile bestimmen,
            RE2_Quellcode_V2/FUN_80052b38.c: DAT_800d424b = Zeile bzw. 0xFF)
  8004a33c: jal 0x8005a09c  (einziger Aufrufer, im Raumlader FUN_80049e48)
  8005a0e0/ec: a0 = Handle 0x800d4c4b ; 8005a100: beq a0,-1,... ; 8005a108: b0 13 02 0c jal 0x80084ec0
  (SsVabClose der alten Bank) ; 8005a114: 00 00 02 a2 sb v0(-1),0(s0) -> Handle = -1
  8005a11c: beq s2(0x800d424b),0xFF,... ; 8005a13c: sw 0x801f8e10 -> 0x800dbb84 (fester Bankpuffer)
RE2 Raumbank-Saetze (H3), Belege der Spuren davor, hier nur gelesen: Kartenhinweis Se(Bank 2 = Raumbank,
  0x2B) @0x8006F234-38; "verschlossen" Se(Bank 2, 0x16) @0x80051610/@0x800516a4, Welle je RAUM
  (Bank 2 = [RDT+0x08] via FUN_80059e54 @0x80059f44, lock_se_common.c); Fahrstuhl ROOM21B0 SND0
  (bank 2) @0x2756/@0x2784; Panel ROOM2130 snd0. Tuersequenz-Bank 0x3DC50 ausserhalb des
  Key-Off-Bereichs 0x14441..0x3DC4F (@0x800597a4ff, Runde 31) -> bleibt (O2).
```
=> Im Original gibt es keine Raum-Animationsbank ausserhalb der RDT; mit dem naechsten Raumladen
(Tuer @0x8001d988 / Spielstart @0x8001d5ac) liegt nur der Block der NEUEN RDT vor. Eine geliehene
Bank eines fremden Raums (Spur K, Port-Wahl) folgt derselben Lebensdauer.

### N2 Umsetzung
* `platform/pc/src/entladen_pc.c`: Besitz des RBJ-Dateipuffers (`re15_entladen_rbj_halten(buf,size,raum)`:
  gibt den vorigen frei, merkt Generation/Raum/Groesse; `re15_entladen_rbj_belegt`). `alles_entladen`
  Schritt (11): eigener Puffer + Leihe K frei; Schritt (12): RE2-Raumbank-Ergaenzungen frei; Schritt (8):
  Montage-Schnappschuss an JEDER Grenze (H1, frueher nur Spielstart/-ende). Drei neue Zensus-Faecher
  `rbj` (Generation, eigener Puffer + Leihe), `bg_prev` (Generation), `re2ton` (wie `ton` nur am
  Ereignis gewertet); VORHER/BILD-Zeilen zusaetzlich `| rbj_datei raum=.. bytes=.. gen=..`.
* `platform/pc/main.c` (4 Zeilen): `static uint8_t *s_room_rbj` entfernt; Tuer-Weg uebergibt den
  Puffer (`re15_entladen_rbj_halten(rbj_borrowed ? NULL : rbuf, rsz, dest_room)` statt eigenem free);
  Zweig "has no RBJ" laesst ausdruecklich los; Boot-Weg uebergibt `rbj_buf` (wenn kein Alias/Leihe).
* `platform/pc/src/cut10f0_pc.c` (Spur K, Haken 9 Zeilen): `s_leih_buf/s_leih_rdt` auf Dateiebene
  + Generation, `re15_cut10f0_pc_rbj_freigeben/_belegt`. Die Leihe selbst unveraendert.
* `platform/pc/src/bg_pc.c` (3 Zeilen): Generation des Schnappschusses + `re15_bg_prev_belegt`.
* `platform/pc/src/audio_pc.c`: `re15_audio_re2_raumbaenke_entladen/_belegt` (unter
  SDL_LockAudioDevice; ENEMSE-Cache ueber das vorhandene `re2se_bank_free`, ELEVSE/HINTSE/TUERSE/
  PANEL2130: PCM frei, Stimmen/Vormerkungen auf diese Puffer geloest, loaded/failed 0 -> der
  vorhandene Lazy-Lader holt sie beim naechsten Ruf). TORSE + Tuersequenz-Bank bleiben (O2).
* `include/re15_entladen.h`: Faecher + Deklarationen. Keine neue Verhaltenskonstante.
Spur L (`re15_rbj_set_alias`, Alias je Takt) und Spur K (Leihe ROOM11B0 -> ROOM10F0) unveraendert
wirksam: die Bindung `re15_rbj_bind_room` haengt am gehaltenen Puffer, der bis zur naechsten Grenze lebt.

### N2 Messung nachher (Stand 4c5cca7f, eigener Bau nach `configure` — der Merge brachte neue .c-Dateien)
Laeufe mit eigener exe-Kopie `re15_pc_i_n2.exe`, Skript `scratchpad/i_n2_lauf.sh` (PATH msys64 zuerst,
eigenes Arbeitsverzeichnis). Logs: `I_entladen_bilder/n2_nach_<lauf>_entladen.log`. Gekuerzt.
```
r1 (= Abnahme r9) RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_FPS=240
   RE15_FIRE_AOT=4@3000#1170 RE15_EXIT_AT=100#1130   (Intro -> 1170 -> echte Tuer 4 -> 1130)
   debug.log: [rbj] room 1170 cinematic overlay: 26 clips / [fire-aot] slot=4 at F3000 / PC loaded room1130
              / [rbj] room 1130 has no RBJ ... PL00-Basis
   VORHER raum gen=3 raum=1170 | belegt ... figur=1 rbj=1 ... | rbj_datei raum=1170 bytes=55060 gen=3
   EREIGNIS raum gen=4 raum=1170 | belegt (alle 18 Faecher) 0 ; 0 BILD-Zeilen in 100 Bildern ROOM1130
r2 (= Abnahme r10) RE15_FPS=240 RE15_GOTO_ROOM=1170 RE15_KILL_AT=3200 RE15_EXIT_AT=200#1240 (Tod in 1170, NEW GAME)
   VORHER spielende gen=3 raum=1170 | belegt ... rbj=1 | rbj_datei raum=1170 bytes=55060 gen=3
   EREIGNIS spielende gen=4 alle 0 ; EREIGNIS spielstart gen=5 alle 0 ; 0 BILD-Zeilen (200 Bilder ROOM1240)
r3 (= Abnahme r11, Boot-Weg) Karte probe_r35_entladen_karte re15_card.mcr 1170 (Cut 3, 4014,-7200,-7436),
   RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_KILL_AT=60 RE15_BOOT_EXIT_AT=3
   debug.log: [rbj] loading cinematic bank: RBJ/ROOM1170.RBJ (55060 bytes) (je Boot, 3x) ; danach
   wechselt der Port aus 1170 nach 1240 (PC loaded room1240 — vorhandenes Verhalten, nicht Teil dieser Spur)
   VORHER raum gen=2 raum=1170 | ... rbj=1 | rbj_datei raum=1170 bytes=55060 gen=2   <- der BOOT-Puffer
   EREIGNIS raum gen=3 alle 0 ; dasselbe in Spiel 2 (gen 5 -> 6); 0 BILD-Zeilen
r4 (Leihe Spur K) RE15_DEBUG_JUMP=10F0@5 RE15_GOTO_ROOM=1030 RE15_EXIT_AT=100#1030
   debug.log: [rbj] Animationsblock von ROOM11B0 geliehen (48168 B, Runde 35 Spur K) / room 10F0 cinematic
   overlay: 25 clips / PC loaded room1030 / room 1030 has no RBJ
   VORHER raum gen=3 raum=10F0 | ... rbj=1 (keine rbj_datei-Angabe = die Leihe) ; EREIGNIS raum gen=4 alle 0
r5 (H3 TUERSE) SDL_AUDIODRIVER=dummy RE15_DEBUG_JUMP=1170@5 RE15_FIRE_AOT=5@10#1170 RE15_GOTO_ROOM=1130
   RE15_EXIT_AT=60#1130 RE15_TUERSE_LOG=tuerse.log
   tuerse.log: F10 raum=1170 nachricht=12 art=M weg=AOT satz=0(ZU_A) nr=1
   VORHER raum gen=3 raum=1170 | ... ton=2 ... rbj=1 bg_prev=0 re2ton=1 ; EREIGNIS raum gen=4 alle 0
H1 bg_prev in jedem Lauf: VORHER raum raum=1240 ... bg_prev=1 -> EREIGNIS raum gen=3 ... bg_prev=0.
```

### N2 Tests
Registriert NUR in `tests/unit/probes/r35_entladen.cmake` (Foreach-Listen erweitert):
| Test | misst | Ergebnis (einzeln, `ctest -R r35_entladen -j3`, 13/13) |
|---|---|---|
| unit_r35_entladen_n2beleg (neu) | RE1.5 `8c42c778` @0x8001b3fc, `8c46005c` @0x8001b404, `10c00033` @0x8001b40c, jal 0x8001b3f8 GENAU EINMAL (@0x80039a08, Voll-Scan); RE2 jal 0x80053528 genau @0x8004a334, jal 0x80052b38 @0x80053610, jal 0x8005a09c genau @0x8004a33c, jal 0x80084ec0 @0x8005a108, `a2020000` @0x8005a114 | Passed |
| integration_r35_entladen_f (neu) | Intro -> 1170 -> echte Tuer 4 -> 1130: Vorbedingung VORHER raum 1170 `rbj=1` + `rbj_datei raum=1170 bytes=55060`, `room 1130 has no RBJ`; VORHER raum 1240 `bg_prev=1`; jede EREIGNIS-Zeile 0, keine BILD-Zeile | Passed 27.5 s |
| integration_r35_entladen_g (neu) | Leihe K: `Animationsblock von ROOM11B0 geliehen`, VORHER raum 10F0 `rbj=1` ohne rbj_datei, EREIGNIS 0 | Passed 5.1 s |
| integration_r35_entladen_h (neu) | Boot-Weg: Karte 1170, `loading cinematic bank: RBJ/ROOM1170.RBJ (55060 bytes)`, VORHER raum 1170 `rbj=1` + rbj_datei, EREIGNIS 0 | Passed 22.2 s |
| integration_r35_entladen_i (neu) | Dummy-Ton, AOT 5 in 1170 (verschlossen): tuerse.log `raum=1170 ... satz=0`, VORHER raum 1170 `re2ton>=1`, EREIGNIS 0 | Passed 3.7 s |
| integration_r35_entladen_d (erweitert) | + Tod in 1170 mit `rbj=1` + `rbj_datei raum=1170 bytes=55060` (Vorbedingung Mangel 1) | Passed 45.8 s |
| unit beleg/gegner/n1beleg, integration a/b/c/e | unveraendert (Zeilen jetzt mit 18 Faechern) | Passed |

**Gegenprobe** (N2-Freigaben temporaer auskommentiert: Schritt (11) beide Aufrufe, Schritt (12), bg_prev
wieder nur an Spielstart/-ende, main.c Zweig "has no RBJ" ohne Loslassen; gebaut als
`re15_pc_gegenprobe_n2.exe`, danach `git checkout` + Neubau). Pins direkt per `cmake -P` mit dieser exe:
D/F/G/H/I alle **FAILED** ("nach dem Entladen noch belegt"). Die EREIGNIS-Zeilen der Gegenprobe zeigen
jeden Mangel einzeln:
```
f: EREIGNIS raum gen=4 raum=1170 rbj=1 bg_prev=1        (Tuer 1170 -> 1130; 105 BILD-Zeilen in 1130)
d: EREIGNIS spielende gen=4 raum=1170 rbj=1 / spielstart gen=5 rbj=1 / raum gen=6 raum=1240 rbj=1 (329 BILD)
g: EREIGNIS raum gen=4 raum=10F0 rbj=1 bg_prev=1        (Leihe K ueberlebt)
h: EREIGNIS raum gen=3 raum=1170 rbj=1 / spielende gen=4 rbj=1 / spielstart gen=5 rbj=1  (Boot-Puffer)
i: EREIGNIS raum gen=4 raum=1170 rbj=1 bg_prev=1 re2ton=1   (TUERSE ueberlebt)
alle: EREIGNIS raum gen=3 raum=1240 bg_prev=1           (Montage-Schnappschuss ueberlebt die Tuer)
```
=> die Gegenprobe reproduziert die gdb-Befunde der Abnahme (r9/r10/r11) mit dem Zensus; die Pins messen
genau Mangel 1 und H1/H3.

**Aussehen unveraendert (A/B, H1 + RBJ)**: N2-exe gegen Gegenprobe-exe, `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60`, Montage 1240 -> Tuer -> 1170:
`RE15_FRAMEDUMP=0-60/1:fd_ RE15_EXIT_AT=61#1170` -> **61/61 Bilder bytegleich** (die ersten 61 Bilder
in 1170 inkl. Tuerbild; Gegenprobe hatte dort bg_prev=1, N2 bg_prev=0), und
`RE15_FRAMEDUMP=200-1600/100:fd_ RE15_EXIT_AT=1601#1170` -> **15/15 bytegleich** (Helipad, Bild
1100/1400 hell, Mittelwert 45/46). Rueckleser vor dem Present (gdigrab liefert in dieser Sitzung Weiss).

**H2 ENEMSE gemessen (Lauf r6 = Pin j)**: `SDL_AUDIODRIVER=dummy RE15_FPS=240 RE15_DEBUG_JUMP=10C0@5
RE15_KILL_AT=1500 RE15_BOOT_EXIT_AT=2 RE15_RE2SE_LOG=re2se.log` (Kraehen-Raum, Memory reai-v2-crow-ai):
`[re2se] ENEMSE Bank 7 geladen: 8 VAGs, Map 32 Eintraege (Cache-Slot 0)`;
`VORHER spielende gen=3 raum=10C0 ... gegner=1 ton=2 ... re2ton=1` -> `EREIGNIS spielende gen=4 ... re2ton=0`.
Gegenprobe-exe: `EREIGNIS spielende raum=10C0 ... re2ton=1` (Bank ueberlebt den Tod) -> Pin j FAILED dort.
| integration_r35_entladen_j (neu) | Dummy-Ton, Kraehe ROOM10C0, Tod: `ENEMSE Bank N geladen`, VORHER spielende 10C0 `re2ton>=1`, EREIGNIS 0 | Passed (einzeln per cmake -P) |
Suite-Zahl nach Merge + N2: `ctest -N` = 531 (master 517 + Spur I 14).

### N2 Hinweise der Abnahme 1 — Entscheidungen
* H1 Montage-Schnappschuss: faellt jetzt an JEDER Grenze (s. Umsetzung, A/B bytegleich).
* H2 ENEMSE-Cache: faellt an jeder Grenze (RE2 @0x8004a33c / @0x8005a108), gemessen Pin j.
* H3 RE2-Mini-Baenke: ELEVSE/HINTSE/TUERSE/PANEL2130 = RE2-Raumbank-Saetze (Bank 2) -> fallen, gemessen
  fuer TUERSE (Pin i); ELEVSE/HINTSE/PANEL2130 laufen durch dieselbe Funktion (gleiche Lazy-Lader-Bauart,
  loaded/failed -> 0), nicht einzeln gemessen. TORSE + Tuersequenz-Bank je Archiv bleiben mit Beleg
  (Tuerbank @0x3DC50 ausserhalb des Key-Off-Bereichs @0x800597a4ff; laufender Schliesston klingt ueber
  die Tuer weiter, Runde 31) — der Ton der TUER, nicht eines Raums.
* H4 heli_md1/pilot_md1: unveraendert O4 (totes Erbe, main.c-Umfang).
* H5 bestaetigt (re15_rbj_room nach jeder Grenze NULL).
* H6 Spur L: deren Sprachzeilen liegen je Raum (synchro/STAGE1/room1150/main22..29, room11C0/main10..12,
  L_cut1150.md:316ff) und laufen innerhalb ihres Raums; eine an der Montage-Grenze noch laufende Zeile
  wuerde Schritt (9) abschneiden — das ist das RE2-Verhalten (N1-Beleg @0x8004a1c4 -> Setmode 0xA0).
  Die L-Integration (integration_r35_cut1150*, _cut10f0) laeuft in der Suite mit.
* H7 Neue Quelldateien seit N1 (elliot_pc.c, entladen_pc.c, entladen_common.c): Android-GLOB neu
  konfigurieren. N2 fuegt KEINE neue .c-Datei hinzu.

### OFFEN (N2)
* O1 (PSX) und O4 (heli/pilot-Parse in main.c) unveraendert.
* O6 ELEVSE/HINTSE/PANEL2130 nicht einzeln mit einem Lauf belegt (s. H3). Naechster Messweg: Fahrstuhl
  ROOM1080 (scd_elev_se.c, Se 0x11/0x12) bzw. Kartenhinweis (map_hint_common.c) bzw. Hebetisch 1150
  (hebetisch_cursor_1150.c) mit SDL_AUDIODRIVER=dummy + RE15_ENTLADEN_LOG, dann Tuer/Tod: re2ton -> 0.

### Fuer den Nutzer (N2)
* Keine Sprachdateien, keine neuen Assets (Paket-/Android-Gate unveraendert), keine neue .c-Datei.
* Wirkung: nach Raumwechsel und Tod ist jetzt auch die Raum-Animationsbank des Raums davor entladen
  (RBJ/ROOM1170.RBJ nach dem Helipad, die von ROOM11B0 geliehene Bank nach ROOM10F0, die Bank eines
  Spielstands in ROOM1170 nach dem ersten Raumwechsel), dazu der Montage-Schnappschuss des Pre-Intros
  und die RE2-Raumtonbaenke (Gegner-Toene, "verschlossen", Fahrstuhl, Kartenhinweis, Panel-Klick). Sie
  werden beim naechsten Bedarf im neuen Raum wieder geladen. Aussehen unveraendert (A/B bytegleich).
* Messschiene: drei neue Faecher `rbj`, `bg_prev`, `re2ton` in `RE15_ENTLADEN_LOG`; VORHER-Zeilen nennen
  eine geladene RBJ-Datei mit Raum und Groesse (`| rbj_datei raum=1170 bytes=55060 gen=3`).

### Suite (N2)
Eigener Bau, Code-Stand e1cee092 (danach nur Dossier), `bash re15_port/tools/local_build.sh all`:
`=== LOCAL-BUILD-OK (all) — Tests 531/531` (Beleg: re15_port/build/local_build_ctest.log 07:03:19, nach
dem exe-Bau 06:16:09: `100% tests passed, 0 tests failed out of 531`, `Total Test time (real) = 2559.82 sec`
— langsamer als N1 durch parallel bauende Baeume). Kein Flatter-Haken rot, nichts nachgefahren; die
K/L-Integration (integration_r35_cut10f0 242.9 s, r35_cut1150*) gruen mit dem gemergten master.
486 (N1) -> 531: + master (Spuren A/B/E/K/L) + unit_r35_entladen_n2beleg + integration_r35_entladen_f/g/h/i/j.

## Nachbesserung 3 (nach Abnahme 2, 2026-10-04)

Abnahmebericht `I_abnahme_2.md`: Punkt 1 erfuellt, Punkt 2 teilweise. Mangel 1: die Lampen-Grafik des
Generator-Bedienfelds ROOM11F0/11F1 Cut 10 (`platform/pc/src/panel_lampen_pc.c`, `static int s_zustand`
1 = geladen, `static uint16_t s_zelle[2][32*32]`, dekodiert aus `shared_assets/RE2/LAMPE2130.TIM` =
ROOM2130.RDT[0x0E398, +4256)) bleibt nach Raumwechsel und Tod geladen; `alles_entladen` ruft
panel_lampen_pc nicht, der Zensus hat kein Fach. Die gdb-Laeufe der Abnahme g6c (Tod in 11F0 ->
NEW GAME 1240: s_zustand=1 an "spielende", "spielstart", im neuen Spiel) und g6d (Tuer AOT 0 11F0 ->
11E0: s_zustand=1 bei Bild 60) sind die Messung vorher und werden NICHT wiederholt.
Stand zu Beginn: HEAD 32239df5, Baum sauber.

### N3 Arbeitsplan
1. Messung vorher mit der Messschiene (neues Fach `lampe`, Vorbedingung in 11F0 Cut 10 = 1).
2. RE-Beleg: die Lampen-Kunst ist RDT-Bestand (RE2 ROOM2130, ESP-TIM der RDT) -> Lebensdauer = Raum.
3. Freigabe in `alles_entladen` (Schritt 13), Fach im Zensus, Pin k (Tod + Tuer), Gegenprobe.
4. Code-Zensus erneut: weitere raumgebundene Caches ohne Freigabe (seit dem Merge)?

### N3 RE-Beleg — die Lampen-Kunst ist ESP-TIM der Raum-RDT, Lebensdauer = ein Raum
Die Datei LAMPE2130.TIM ist der byte-gleiche Schnitt ROOM2130.RDT[0x0E398, +4256) = ESP-TIM-Basis aus
dem RDT-Kopfwort [20] (Datei 0x58), Tabelle bis Kopfwort [21] (0x5C) (`tools/r34n_c/lampe2130_schnitt.py`,
Pruefausgabe `analysis/befunde_runde34_nacht/C_belege/lampe2130_pruefung.txt`). Wer liest diese zwei
Kopfwoerter im RE2-Original und wann? Selbst disassembliert (`re2_disasm.py`, jal-Wort-Scan ueber die
ganze `info/re2leon/PSX.EXE`, Skript scratchpad/jalscan.py):
```
RE2 Raumlader FUN_80049e48 (s0 = 0x800cc1e8 @0x80049e50/54 -> 8508(s0) = 0x800ce324 = RDT-Zeiger)
  8004a178: lw a1,8508(s0)  / 8004a190: lw a1,8508(s0)   Ziel = *(0x800ce324)
  8004a1c4: jal 0x80012fb8                                NEUE RDT in diesen Puffer lesen (N1-Beleg)
  8004a2ec: e9 6e 00 0c  jal 0x8001bba4                   Raum-ESP einrichten (einziger Aufrufer, Scan)
  8004a2f4: jal 0x80059e54                                Raumbank (Bank 2, N2-Beleg)
  8004a33c: jal 0x8005a09c                                ENEMSE-Bank (N2-Beleg)
FUN_8001bba4 (Prolog 8001bba4: addiu sp,sp,-24)
  8001bc78: 24 e3 42 8c  lw v0,-7388(v0)   0x800ce324 = RDT-Zeiger
  8001bc80: 5c 00 44 8c  lw a0,92(v0)      RDT+0x5C = Kopfwort [21] (Ende der ESP-TIM-Tabelle)
  8001bc84: 58 00 45 8c  lw a1,88(v0)      RDT+0x58 = Kopfwort [20] (ESP-TIM-Basis = LAMPE2130-Quelle)
  8001bc88: 4e 6f 00 0c  jal 0x8001bd38    einziger Aufrufer (Scan: genau ['0x8001bc88'])
FUN_8001bd38 (RE2_Quellcode_V2/FUN_8001bd38.c): tim = basis + *(ende-4) je Eintrag, OpenTIM/ReadTIM,
  LoadImage(prect) an x = 0xF*0x40 abwaerts / LoadImage(crect) an (0x120, 0x1E0+) -> VRAM; Ende bei
  0xFF in 0x800eae50.
```
=> Im RE2-Original wird die Lampen-Kunst (ESP-TIM der RDT) bei JEDEM Raumladen aus der NEUEN RDT
hochgeladen (@0x8004a2ec -> @0x8001bc80/84 -> @0x8001bc88); der naechste Raum ueberschreibt RDT-Puffer
und ESP-TIM-Seite. RE1.5 dasselbe ueber die Arena (RDT ab Basis `jal 0x80013b60` @0x800397e8, Reset
@0x80039738, R1). Die dekodierte Kopie im Port (`s_zelle`) hat also die Lebensdauer eines Raums und
faellt an derselben Grenze wie der PANEL2130-Ton desselben Bedienfelds (N2 Schritt 12).

### N3 Code-Zensus erneut (seit dem Merge, alle Lazy-Lader mit Zustandsvariable)
`grep "static (int|uint8_t) s_*(zustand|state|tried|geladen|loaded|ok|init)"` in platform/pc/src +
engine/src und alle `re15_pc_read_{re2,cd,any}/pc_read_shared`-Aufrufer gelesen:
* panel_lampen_pc.c `s_zustand` + `s_zelle` — Mangel 1 (Datei aus shared_assets/RE2, dekodiert).
* hebetisch_cursor_pc.c `s_md1/s_md1_ok/s_licht/s_licht_ok` (ROOM1150-Cursor): Quelle eingebackene
  Bytes (`re15_hebetisch_cursor_md1_bytes/_licht_bytes`, Teil der exe), `re15_md1_parse`/`re15_light_parse`
  allozieren nichts (md1_common.c/light_common.c ohne malloc) -> nur Sichten auf exe-Daten, kein geladenes
  Asset. Die Textur liegt in TIM-Platz 28 (Prop-Platz, faellt in Schritt (1)); neu hochgeladen je
  Cursor-Sitzung (`s_hochgeladen != sitzung`).
* inv_render_pc.c Karte (`s_map_loaded`), RE2-ST0, Box-Panel; item_icon/itps (ITEMALL.PIX/ITPS.ITP);
  audio_pc.c Fuss/SE/Waffe/CORE/TORSE/Tuer; bg_pc.c, elliot_pc.c — global bzw. schon freigegeben (N1/N2).
=> einziger neu gefundener raumgebundener Cache: LAMPE2130 (= Mangel 1).

### N3 Umsetzung (Commit c7d97614, gebaut)
* `platform/pc/src/panel_lampen_pc.c` (Datei der Spur C, Runde 34 Nacht; Haken 11 Zeilen: 1 Include,
  1 Generation `s_gen`, 1 Zeile in `laden()`, 2 Funktionen + 4 Zeilen Kopf): `re15_panel_lampen_pc_entladen()`
  setzt `s_zustand = 0` (= ungeprueft, der Zustand beim Prozessstart) und nullt `s_zelle`;
  `re15_panel_lampen_pc_belegt(&gen)` = `s_zustand == 1`. Zeichnen/Laden selbst unveraendert: der
  naechste sichtbare Lampen-Takt in 11F0/11F1 Cut 10 laedt LAMPE2130.TIM neu.
* `platform/pc/src/entladen_pc.c`: `alles_entladen` Schritt (13) ruft die Freigabe (Kommentar mit RE2
  @0x8004a1c4/@0x8004a2ec/@0x8001bc80/@0x8001bc84/@0x8001bc88, RE1.5 @0x800397e8/@0x80039738);
  19. Zensus-Fach `lampe` (mit Generation -> auch BILD-Zeilen erfassen eine fremde Lampe).
* `include/re15_entladen.h`: `RE15_FACH_LAMPE` + Deklarationen mit Beleg-Kette.
* Keine neue Verhaltenskonstante, keine neue .c-Datei, kein neues Asset, kein main.c-Haken.

### N3 Messung nachher (eigener Bau nach c7d97614, Messschiene RE15_ENTLADEN_LOG)
```
k (Pin, = Abnahme g6c) RE15_FPS=240 RE15_DEBUG_JUMP=11F0@5 RE15_FORCE_CUT=10 RE15_SET_FLAG_AT=5:13,5:15,5:17@100
    RE15_KILL_AT=400 RE15_BOOT_EXIT_AT=2 RE15_PANEL_LOG=panel.log
    panel.log: 561 Bilder "raum=11F0 cut=10 ... maske=015 ... lampe_o=1" (ab F100)
    VORHER   spielende gen=3 raum=11F0 | belegt ... msk=1 tim=18 gegner=2 esp_bank=1 rdt=1 bg=1 ... lampe=1
    EREIGNIS spielende gen=4 raum=11F0 | belegt (alle 19 Faecher) 0
    EREIGNIS spielstart gen=5 | alle 19 Faecher 0 ; keine BILD-Zeile
l (Pin, = Abnahme g6d) dieselbe Lampe + RE15_FIRE_AOT=0@300#11F0 RE15_EXIT_AT=60#11E0
    debug.log "PC loaded room11e0.rdt" ; panel.log 201 Bilder lampe_o=1
    VORHER   raum gen=3 raum=11F0 | ... tim=20 gegner=2 ... lampe=1
    EREIGNIS raum gen=4 raum=11F0 | alle 19 Faecher 0 ; 60 Bilder ROOM11E0 ohne BILD-Zeile
s5 ECHTER WEG (Spur-C-Lauf S5, scratchpad/n3_s5.sh): RE15_DEBUG_JUMP=11F0@240 RE15_SUBSTART=16@40#11F0
    (sub16 = Evt_exec-Ziel des Panel-AOT Slot 1), Meldungen + "Ja" und Schalter 3,1,5 per D-Pad/Quadrat
    (RE15_INPUT_SCRIPT, Basis spiel, Start 320), echte Tuer RE15_FIRE_AOT=0@720#11F0 -> 11E0,
    RE15_EXIT_AT=150#11E0. "[substart] sub_scd[16] at F40", obere Lampe F686..F720 (35 Bilder),
    Bild 700 (Rueckleser vor Present) Lampenrechteck x212..233/y67..85 Mittel RGB (73.6,148.3,72.5)
    gegen Bild 60 vor dem Panel (8.1,8.1,6.0) = gruen gezeichnet;
    VORHER raum gen=3 raum=11F0 ... lampe=1 -> EREIGNIS raum gen=4 alle 19 Faecher 0, 0 BILD-Zeilen
    in 150 Bildern ROOM11E0.
```
**Gegenprobe** (Schritt (13) auskommentiert, gebaut als `re15_pc_gegenprobe_n3.exe`, danach
`git checkout` + Neubau; exe nach der Messung geloescht). Pins K/L per `cmake -P` mit dieser exe:
beide **FAILED** ("nach dem Entladen noch belegt"); die Zeilen reproduzieren die gdb-Werte der Abnahme:
```
k: EREIGNIS spielende gen=4 raum=11F0 lampe=1 ; BILD 1 gen=4 raum=11F0 lampe=1 (fremd 1) ;
   EREIGNIS spielstart gen=5 lampe=1            (= Abnahme g6c: s_zustand=1 nach spielende/spielstart)
l: EREIGNIS raum gen=4 raum=11F0 lampe=1 ; BILD 1/30/60 gen=4 raum=11E0 lampe=1 (fremd 1)
                                                (= Abnahme g6d: s_zustand=1 bei Bild 60 in 11E0)
```
=> Der Fix erklaert den Befund: Vorbedingung (`lampe=1` vor der Grenze, Lampe sichtbar) steht im
Protokoll, mit Freigabe 0, ohne Freigabe 1 — genau das Feld, das gdb in der Abnahme las.

**Aussehen unveraendert / Wiederladen** (A/B neue exe gegen Gegenprobe-exe, scratchpad/n3_ab.sh):
`RE15_DEBUG_JUMP=11F0@5 RE15_FORCE_CUT=10 RE15_SET_FLAG_AT=5:13,5:15,5:17@10 RE15_GOTO_ROOM=11F0
RE15_EXIT_AT=100#11F0` (11F0 -> 11F0 ueber die Raumgrenze), `RE15_ENTLADEN_SHOT_BILD=20,25,60,61,90`:
**8/8 Bilder bytegleich** (SHA1; gen3 b020/b025 mit Lampe: Mittel gruen 148.3 gegen 50.5 ohne).
Neue exe: VORHER raum 11F0 lampe=1 -> EREIGNIS 0; Gegenprobe: EREIGNIS lampe=1 + 4 BILD-Zeilen.
Wiederladen: Bank 5 ist RAUMLOKAL (die Schalter-Bits fallen beim Wiedereintritt, gemessen: mit
RE15_SET_FLAG beim Boot kein lampe_o=1 in 11F0), ein zweites Sichtbarmachen im selben Prozess gibt
die Messschiene nicht her. Belegt ist es trotzdem am Lauf: in JEDEM Lauf lief
`re15_panel_lampen_pc_entladen()` schon an "spielstart" gen 2 und "raum" gen 3 VOR dem ersten Laden,
und der Zustand danach ist unabhaengig vom Zustand davor (`s_zustand = 0`, `s_zelle` = 0, beides fest);
das Laden danach gelingt (lampe=1, Bild gruen, Pins k/l/s5).

### N3 Tests
| Test | misst | Ergebnis |
|---|---|---|
| unit_r35_entladen_n3beleg (neu) | RE2 `3c10800d` @0x80049e50, `2610c1e8` @0x80049e54, `8e05213c` @0x8004a178, jal 0x80012fb8 @0x8004a1c4, jal 0x8001bba4 GENAU @0x8004a2ec (Voll-Scan), `8c42e324` @0x8001bc78, `8c44005c` @0x8001bc80, `8c450058` @0x8001bc84, jal 0x8001bd38 GENAU @0x8001bc88; ROOM2130.RDT [0x58] = 0x0E398 und LAMPE2130.TIM == RDT[0x0E398, +4256) | Passed 0.03 s |
| integration_r35_entladen_k (neu) | Tod in 11F0 mit sichtbarer Lampe: panel.log `raum=11F0 cut=10 ... lampe_o=1`, VORHER spielende 11F0 `lampe=1`, jede EREIGNIS-Zeile 0, keine BILD-Zeile | Passed 3.9 s; Gegenprobe FAILED |
| integration_r35_entladen_l (neu) | Tuer AOT 0 11F0 -> 11E0 mit sichtbarer Lampe: `PC loaded room11e0.rdt`, VORHER raum 11F0 `lampe=1`, EREIGNIS 0, keine BILD-Zeile | Passed 18.7 s; Gegenprobe FAILED |
Registriert nur in `tests/unit/probes/r35_entladen.cmake` (Foreach-Listen um n3beleg / K L erweitert).

### N3 Hinweise der Abnahme 2 — Entscheidungen
* H1 Kinobalken ueber der Charakterwahl nach einem Tod WAEHREND einer Szene (nur per RE15_KILL_AT bei
  offenen Balken erreichbar). Weiter-RE'd, Ergebnis: der Balken-Zaehler laeuft im Original NICHT nur im
  Spielmodul. Die Hauptschleife FUN_80020bb0 (Prolog @0x80020bb0, einziger Aufrufer `jal` @0x800544e8,
  Balken-Init `jal 0x80020f8c` @0x80020be8) ruft je Bild `jal 0x80021a0c` @0x80020f34 und DANACH den
  Modul-Umschalter `jal 0x80010000` @0x80020f3c (FUN_80010000 vergleicht Modul-Ist 0x800a73a4 mit
  -Soll 0x800aca34 @0x8001001c-28) — also fuer Titel, Auswahl und Spiel. FUN_80021a0c rampt
  0x800b5568 nach `0x800aca3c & 0x10` (einziger Schreiber `sb` @0x80021a80, Ghidra-XREF[8]) und
  haengt die Balken an die OT des LAUFENDEN Moduls (`0x800aa6a8 + 0x800aca34*0x20`), unterdrueckt
  nur bei `0x800aca38 & 0x4000`. Das Richtungsbit 0x10 loescht der Raumlader (`sw` @0x80039730,
  Maske der unteren 16 Bit, Kommentar fade_common.c:97ff) — beim Spielstart @0x8001d5ac, also erst bei
  NEW GAME, nicht am Tod. Die drei Schreiber im Spielmodul @0x8001cd48/@0x8001cd90/@0x8001cdc4 setzen
  0x8000/0x40, nicht 0x10. => Ob das Original nach einem Tod bei offenen Balken im Titel Balken zeigt,
  haengt an `0x800aca38 & 0x4000` im Titel-/Auswahlmodul bzw. am Todesweg (YOU-DIED-Sequenz) — nicht
  belegt. Kein Raum-Asset und kein PRI; ohne diesen Beleg aendere ich das Zeichnen NICHT (STOP-GATE).
  Steht unter OFFEN O7 mit Messweg.
* H2 (Titel nach jedem Tod blendet von Weiss ein): kein Raum-Asset, nicht Teil des Auftrags; unveraendert.
* H3 O6: ELEVSE selbst gemessen (Abnahme g4); HINTSE/PANEL2130-Ton laufen durch dieselbe Funktion.
  PANEL2130-Ton jetzt zusammen mit der Lampen-Kunst desselben Bedienfelds an jeder Grenze frei.
* H4 main.c-Zweig "keine PL00-Basis": nur bei nicht ladbarer PL00 erreichbar; unveraendert (main.c-Umfang).
* H5 O1/O4 unveraendert.
