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
(laufend)

## Messung nachher
(laufend)

## Tests
(laufend)

## OFFEN
(laufend)

## Fuer den Nutzer
(laufend)
