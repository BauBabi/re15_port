# Runde 35 Spur D "redhawk" — Dossier

Baum `.claude/worktrees/r35_redhawk`, Zweig `r35/redhawk`, Basis master 154a73c1.

## Auftrag (woertlich, AUFTRAG.md Z.17)
> Wenn ich mit der Super Redhawk auf die Hunde schieße bleiben die Fleisch Effekte die sich rauslösen permanent da in loop.

Zuteilung (VERTRAG.md): Spur D hat KEINE Bank-9-Bits, keine Nachrichten-IDs, keine AOT-Slots, kein
Ereignis. Nur Code + Tests.

## Fortschritt (fortlaufend)
- 2026-10-04: Baum geprueft (status leer, HEAD 154a73c1). AUFTRAG.md + VERTRAG.md gelesen.

## Messung vorher
(folgt)

## RE-Belege

### R1 Port-Pfad Redhawk -> Hund (Code gelesen, Stand 154a73c1)
* Redhawk = Waffe 7; `s_re2d_row_von_waffe[7] = 5` (enemy_ai_re2_dog.c:2036) -> Schadenszeile 5.
* Zeile 5 gehoert zum Router 0x80104610 (Zeilen {5,6,9,17,19}, Tabelle @0x801055CC): Kern,
  Teile-Wurf 0x80104440, +0x21F := 18 (@0x80104758), danach JEDES Bild FX(3,0), FX(2,0),
  FX(2,1+(rand&1)) (@0x80104644-7C) bis das Budget leer ist (18 = 6 Bilder).
* FX-Tabelle @0x801056AC: FX 1/2/3 = Art 9 (Spritzer) Sub 0/1/2; der Port-Dekoder
  (re2d_fx, = re2z_gore_fx_ex) macht daraus RAUM-Id 7 Sub 0/1/2. FX 7/8 = Art 0x85 -> Raum-Id 5.
* ROOM1190 traegt laut Kommentar enemy_ai_re2_zombie.c:1091 nur Raum-Id {7} -> genau die
  Fleisch-Brocken (RE15_ESP_ROWMACHINE.md Korrektur 6: "id 7 Chunks (B=36 Floor-Test -> 37
  Land-Splat)").
* Port-Zeilen-VM (re15_esp.c esp_fx_dispatch / esp_fx_dispatch_b): Routine A 19 und Routine B
  36/37 sind NICHT implementiert (A-Faelle 0,3,4,5,8,9,10,11,15,16,17,18,30,31,38,41,42; B nur 12/29).
  Ein Row-VM-Platz stirbt im Port nur durch Flags-Bit 0 = 0 (Routine) oder Anim-Terminator 0/0.

### R2 Routine-Tabelle @0x80071d40 (re15_disasm.py table 0x80071d40 48)
`[19] -> 0x80017d08`, `[36] -> 0x800187c4`, `[37] -> 0x8001885c`.
0x800b52c4 ist im ESP-Tick der ZEIGER AUF DEN AKTUELLEN PLATZ (das Werkzeug annotiert
"attack_workstruct" falsch): `80019e64 lw v1,21188(v1)` / `80019e84 lhu v0,0(v1)` / `jalr`;
`80019eb0 addiu v0,v0,132` / `80019eb8 sw v0,21188(at)` (Schritt 0x84 = ein Platz).

### R3 Routine 36 @0x800187c4 (Routine B, Bodentest) — selbst disassembliert
```
800187cc lw   v1,0x52c4(v1)      ; slot
800187d8 lh   v0,40(v1) -> sp+16 ; P.x = slot+0x28 (Weltlage)
800187e4 lh   v0,42(v1) -> sp+20 ; P.y = slot+0x2a
800187f0 lh   v0,44(v1) -> sp+24 ; P.z = slot+0x2c
800187dc addu a1,zero,zero / 800187e8 ori a2,zero,0x8 / 800187f4 ori a3,zero,0x100
800187f8 jal  0x8001c6e8          ; room_coll(&P, 0, 8, 0x100) -> v0 = Bodenhoehe
80018804 sll/sra v0 (s16) ; 80018808 lw v1,20(sp) ; 80018810 slt v0,v0,v1 ; Boden < P.y ?
80018814 beq  v0,zero,0x8001883c
  ja:   80018828 sh v1(=0x25),2(v0)   ; Routine B := 37
        8001882c/30/38 sh zero,16/18/20(v0) ; Geschwindigkeit slot+0x10/12/14 := 0
  nein: 80018848 sh a0,30(v0)         ; slot+0x1e := Bodenhoehe
```
### R4 Routine 37 @0x8001885c (Routine B, Landung) — selbst disassembliert
```
8001886c lh  v1,42(a0)  ; slot+0x2a Welt-y
80018870 lh  v0,30(a0)  ; slot+0x1e Bodenhoehe (von R36)
80018878 slt v0,v0,v1 / 8001887c beq v0,zero,0x800188a8 ; nur wenn Boden < Welt-y
80018884 lbu v0,14(a0) / 8001888c sb v0,108(a0)   ; Flags slot+0x6c := row[0x0e]
8001889c lbu v0,22(v1) / 800188a4 sb v0,110(v1)   ; Anim-Index slot+0x6e := row[0x16] (Delay-Slot)
800188a0 jal 0x800174e4                           ; Zeilen-Vorschub
```
### R5 Routine 19 @0x80017d08 (Routine A, Brocken-Werfer der Id 5) — selbst disassembliert
```
80017d1c lhu v0,14(v1) / beq zero -> 80017d40 ; row[0x0e] != 0: row[0x0e]-- (80017d3c), fertig
80017d40 jal 0x8001af20 (rand) / 80017d50 addiu v0,v0,3072        ; scale = rand + 0xC00
80017d54 lhu s0,22(v1) / srl 8 / sll 24 ; 80017d58 lbu v1,22(v1) / sll 16 ; Code = (row16>>8)<<24 | (row16&0xff)<<16 | scale
80017d6c jal rand ; 80017d74..98 a1 = rand*682 (5,85,340,341,*2)    ; Gier
80017d9c lw a2,116(a3) (slot+0x74) / 80017da4 addiu a3,a3,64 (slot+0x40)
80017da0 jal 0x800199d4                                             ; Kind-Spawn (Start-Flags 0x0a)
80017db4 lhu v0,38(v0) / beq zero / 80017dc4 jal 0x800174e4        ; row[0x26] != 0 -> Vorschub
```
### R6 Tick-Anim-Stufe @0x8001a38c (Bestaetigung der Felder)
`8001a398 lbu v0,109(v1)` Timer slot+0x6d; `8001a3bc-c8` slot+0x6e++ (ausser Flags&0x40);
`8001a3dc lw v1,120(a1)` Records slot+0x78; Terminator rec[2]==0 && rec[0]==0 ->
`8001a40c sb zero,108(a1)` (Flags := 0 = Platz frei); rec[2]==0xff -> slot+0x6e := rec[0] (Schleife).
=> slot+0x6e IST der Anim-Record-Index (Port: f->frame), slot+0x6d der Timer (Port: f->timer).

## Umsetzung
(folgt)

## Messung nachher
(folgt)

## Tests
(folgt)

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)
