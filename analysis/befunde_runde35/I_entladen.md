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

## Tests
(laufend)

## OFFEN
(laufend)

## Fuer den Nutzer
(laufend)
