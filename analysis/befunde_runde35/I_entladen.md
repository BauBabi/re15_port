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
(laufend)

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
