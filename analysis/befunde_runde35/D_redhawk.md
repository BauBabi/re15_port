# Runde 35 Spur D "redhawk" — Dossier

Baum `.claude/worktrees/r35_redhawk`, Zweig `r35/redhawk`, Basis master 154a73c1.

## Auftrag (woertlich, AUFTRAG.md Z.17)
> Wenn ich mit der Super Redhawk auf die Hunde schieße bleiben die Fleisch Effekte die sich rauslösen permanent da in loop.

Zuteilung (VERTRAG.md): Spur D hat KEINE Bank-9-Bits, keine Nachrichten-IDs, keine AOT-Slots, kein
Ereignis. Nur Code + Tests.

## Fortschritt (fortlaufend)
- 2026-10-04: Baum geprueft (status leer, HEAD 154a73c1). AUFTRAG.md + VERTRAG.md gelesen.

## Messung vorher

### M1 Zensus der Raum-ESP-Baenke (probe_r35_redhawk zensus, alle 94 Raeume mit ESP)
ROOM1190 und ROOM11D0 (die Hunde-Raeume) tragen beide NUR Raum-Id 7:
```
R1190 id=7 count_a=11 count_b=9 anim: [0 d0100 p2003] [1 d0101 p2003] [2 d0102 p2003] [3 d0103 p2003]
      [4 d0104 p2003] [5 d0100 p20ff] [6 d0105 p2003] [7 d0106 p2003] [8 d0107 p1003] [9 d0108 p1003] [10 d0000 p0000]
R1190 id=7 sub=0 st=0/1 row=0/2 A=0 B=36 acc=(0,10,0) f0e=13 vel=(150,-80,0) p16=6 p1e=0 g26=1
R1190 id=7 sub=0 st=0/1 row=1/2 A=0 B=0  acc=(0,0,0)  f0e=00 vel=(0,0,0)     p16=0 p1e=0 g26=0
  (Subs 1..5 gleich gebaut, nur vel: (50,-160) (180,-120) (80,-30) (20,-20) (110,-100); Subs 6/7 = Fehlparse
   hinter dem Ende, A=92/50 > 47 = ausserhalb der 48er-Tabelle)
```
Anim: Records 0..4 je 3 Bilder, Record 5 = Schleifenmarke (Dauer 0xFF) zurueck auf Record 0 ->
der FLUG-Zyklus 0..4 laeuft EWIG. Erst Routine 37 setzt den Anim-Index auf row[0x16] = 6 ->
Records 7,8,9 und Record 10 = Terminator 0/0 -> Flags := 0 (@0x8001a40c) = Platz frei.
Ohne Routine 36/37 erreicht ein Brocken den Terminator NIE.

### M2 Laufzeit-Messung am gebauten Stand 154a73c1 (probe_r35_redhawk messung, ROOM11D0)
Echter Spielschritt (re15_game_step, RE2-KI, Bank EM020) + ESP-Takt dahinter wie die Plattform
(fx_plattform_pc.c), Raum-Bank ROOM11D0 gebunden, Schuss = re15_player_weapon_fire(7):
```
ROOM11D0 Schuss Waffe 7: Treffer=2 hp 83 -> -117 st=3 +5=7
Bild    1: id7 lebend 1 ... Bild 10: id7 lebend 6 (sichtbar 6), id0 52
Bild   30: id7 lebend 6 (sichtbar 6), id0 0, Hund st=7/1/0
     slot=12 id=7 sub=1 A=0 B=36 fl=03 anim=0 row=0/2 ...
Bild  900: id7 lebend 6 (sichtbar 6), id0 0
     slot=12 id=7 sub=1 A=0 B=36 fl=03 anim=0 timer=0 row=0/2 y=-528 xlat_y=506 vel=(0,4,0) wpos=(-4029,-16,-17955)
     slot=39 id=7 sub=1 A=0 B=36 fl=03 anim=4 timer=0 row=0/2 ... vel=(0,-6,0) wpos=(-3739,0,-17933)
ERGEBNIS ROOM11D0: Spitze id7 6, letztes Bild mit id7 900 von 900
```
=> BEFUND REPRODUZIERT: die 6 Fleisch-Brocken (Raum-Id 7, je einer pro Router-Bild FX(2,1|2))
liegen nach 30 s noch da, stehen alle noch auf Routine B = 36 (nie 37), Zeile 0/2, Anim im
Flug-Zyklus 0..4 (Schleife) und zittern auf der Port-Bodenklemme (vel.y +-13). Die Id-0-Tropfen
(globale Bank) enden dagegen sauber (id0 52 -> 0).
Erklaerung: Routine B 36/37 fehlen im Port (esp_fx_dispatch_b kennt nur 12/29) -> der Brocken
landet nie, der Anim-Index wird nie auf 6 gesetzt, der Terminator nie erreicht.

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

### R7 Zeilen-Vorschub FUN_800174e4 (Gegenkontrolle, selbst disassembliert)
`800174f0-fc` slot+0x6f++; `8001750c-24` Quelle = *(slot+0x80) + Cursor*40; `80017538-d8` 40-Byte-
Kopie nach slot+0x00 (32 + 8). Zeile 1 der Brocken = Nullen -> Beschl./Geschw. 0, A = B = 0.

### R8 Folgerungen aus der Disassembly (am Port nachgemessen, s. Messung nachher)
* R36 nullt die Geschwindigkeit im Routine-B-Schritt; die Tick-Physik danach
  (@0x8001a2fc-388: xlat += vel, DANN vel += acc) laesst sie am Ende des Landebilds auf der
  Beschleunigung (0,10,0) stehen. Im Folgebild kopiert R37 Zeile 1 (Nullen) -> der Brocken steht.
* R37 vergleicht mit slot+0x1e = dem h, das R36 im LETZTEN NEIN-Zweig gespeichert hat (im
  Landebild schreibt R36 es nicht). Faellt ein Brocken seitlich in eine hohe Zelle
  (room_coll = -1800*(Band+1)), loest R36 sofort aus (vel.x/z = 0), R37 aber erst, wenn der
  Brocken senkrecht bis unter die zuletzt gemessene Bodenhoehe gefallen ist (gemessen: +10 / +13
  Bilder). Kein Port-Einfall — so steht es in den Instruktionen.
* Der Brocken landet OHNE Einrasten: die Weltlage bleibt dort, wo R36 sie zuerst unter h fand
  (gemessen Weltlage-y 12..110 unter Boden 0) — das Original klemmt nicht.

### R9 RE2-Gegenprobe (Beta -> Retail)
RE1.5 R36/R37 sind vollstaendige Routinen (kein Stub, kein jalr 0) -> Ziel ist RE1.5. Die Hunde-KI
ist RE2, ihre Effekte laufen aber im Port ueber die RE1.5-Zeilenmaschine mit RE1.5-Raumdaten (Id 7 ==
RE2-Raum-Id 9, byte-identischer Sprite-Koerper, Dossier analysis/befunde_runde4_2026-09-12/
gore-vollausbau.md §1.2; die Zeilen-Programme hat Capcom fuer RE2 weiterentwickelt). Muster-Suche im
RE2-PSX.EXE nach dem R36/R37-Koerper (Konstanten `ori v1,zero,0x25` + `sh v1,2(v0)`,
`ori a2,zero,8` + `ori a3,zero,0x100`): KEIN Treffer — RE2 hat die Maschine umgebaut. Fuer den
Port-Pfad (RE1.5-Daten in der RE1.5-Maschine) ist RE1.5 massgeblich.

## Umsetzung

| Datei | Art | Inhalt |
|---|---|---|
| `re15_port/engine/src/esp_brocken.c` | NEU | `re15_esp_brocken_b()` = Routine B 36 (@0x800187c4) und 37 (@0x8001885c) + Messschiene |
| `re15_port/include/re15_esp_brocken.h` | NEU | Vertrag + Belege |
| `re15_port/engine/src/re15_esp.c` | 3 Zeilen | `#include`, in `esp_fx_dispatch_b` `if (re15_esp_brocken_b(f)) return;`, Export `re15_esp_fx_zeile_weiter` (= `esp_fx_row_advance`, FUN_800174e4) |

Konstanten (alle mit Adresse im Code):
* room_coll-Argumente r = 0 (`addu a1,zero,zero` @0x800187dc), Startband 8 (`ori a2,zero,0x8`
  @0x800187e8), Maske 0x100 (`ori a3,zero,0x100` @0x800187f4) -> `re15_collision_room_coll`
  (= FUN_8001c6e8, re15_collision.c:314).
* B := 37 (`ori v1,zero,0x25` @0x80018818, `sh v1,2(v0)` @0x80018828).
* slot+0x1e := h (`sh a0,30(v0)` @0x80018848); Flags := row[0x0e] (@0x80018884/8c);
  Anim := row[0x16] (@0x8001889c/a4); Vorschub (`jal 0x800174e4` @0x800188a0).
* PORT-WAHL (keine Original-Adresse, gekennzeichnet): `f->floor_y = INT32_MAX` beim ersten
  R36-Takt = die Port-Sammelklemme in re15_esp_fx_tick Stufe (f) gilt fuer diesen Platz nicht.
  Begruendung: das Original HAT keine Klemme (Physik @0x8001a2fc-388 rein xlat += vel, vel += acc),
  den Boden kennt nur Routine B; mit der Klemme kaeme die Weltlage nie unter h und R36 schluege nie
  an. Gleicher Wert und gleiche Begruendung wie ESP_KEIN_BODEN fuer Granaten (re15_esp.c:557).

Keine Bank-9-Bits, keine Nachrichten, keine AOT-Slots, keine Ereignisse, keine Assets.

## Messung nachher

### N1 probe_r35_redhawk messung (gleicher Lauf wie M2, nach dem Fix)
```
Bild   10: id7 lebend 6 (sichtbar 6), id0 52
Bild   30: id7 lebend 6 ... slot=30 id=7 sub=0 A=0 B=0 fl=13 anim=7 row=1/2 wpos=(-4083,85,-16764)
Bild   60: id7 lebend 0 (sichtbar 0)
Bild  900: id7 lebend 0
ERGEBNIS ROOM11D0: Spitze id7 6, letztes Bild mit id7 57 von 900      (vorher: 900 von 900)
```
### N2 Spurverfolgung je Brocken (probe_r35_redhawk pin, Pin 1) gegen R3/R4/R6
```
Platz 30 sub 0: Landung Bild 18, h=-1800 Weltlage-y -365 (Vorbild -425 bei h 0), vel=(0,10,0) = acc ok
Platz 30: Abschluss Bild 28 (+10) B=0 Flags 13 Zeile 1/2 Anim 6, slot+0x1e 0 < Weltlage-y 85 ok
Platz 12 sub 1: Landung Bild 37, h=0 Weltlage-y 12 (Vorbild -178 bei h 0), vel=(0,10,0) = acc ok
Platz 12: Abschluss Bild 38 (+1) B=0 Flags 13 Zeile 1/2 Anim 6, slot+0x1e 0 < Weltlage-y 12 ok
Platz 21: Abschluss Bild 38 (+1) B=0 Flags 13 Zeile 1/2 Anim 7 ...
Platz 39: Abschluss Bild 47 (+13) ... (seitlich in hohe Zelle, senkrechter Fall wie R8)
```
Alle 6: Landebild = erstes Bild mit Weltlage-y > room_coll (Vorbild <= h), Geschwindigkeit genullt
(nach dem Takt = acc), Abschluss mit Flags 0x13 / Zeile 1 / Anim 6|7, Ende am Terminator
spaetestens 13 Bilder nach dem Abschluss.

### N3 Haenger-Zensus der Raum-ESP-Daten (probe_r35_redhawk haenger [vorher])
"Haenger" = gueltiger Stream (alle A,B < 48), dessen Anim ab Record 0 vor dem Terminator auf eine
Schleifenmarke laeuft UND der eine im Port unbekannte Routine traegt.
```
vorher : 94 Raeume, 1953 gueltige Streams, 673 Haenger = 504 x Id 7 (B 36) + 120 x Id 13 (A 24/25) + 49 x Id 1 (A 2)
nachher: 94 Raeume, 1953 gueltige Streams, 169 Haenger = 120 x Id 13 (A 24/25) + 49 x Id 1 (A 2)
```
Die 504 Brocken-Streams (84 Raeume x Subs 0..5) sind geschlossen. Id 13 / Id 1 -> OFFEN (nicht
Hund/Waffe, s.u.).

### N4 Wer wirft Id-7-Brocken im Port (alle betroffen, alle durch denselben Fix geheilt)
* Hund RE2 (enemy_ai_re2_dog.c re2d_fx, FX-Tabelle @0x801056AC Art 9 -> Raum-Id 7):
  Tod Zeilen {5,6,9,17,19} ueber Router 0x80104610 (Waffe 7 Redhawk = Zeile 5; Waffe 9/15 HE = Zeile 9;
  Waffe 18 = Zeile 17), HURT-Bild-Bits FX 1/2 (@0x80102838-48, Zufall rand&3), Knockdown aus dem Sprung
  FX 2/3 (@0x801039B8-D0).
* Zombie RE2 (enemy_ai_re2_zombie.c:5531, 0x09020000 -> Raum-Id 7 Sub 2, Brocken-Tod @0x8010973C-B8).
* Raum-Skripte (op 0x3A) mit Id 7 laufen ueber dieselbe Maschine.

## Tests
* `unit_r35_redhawk` (probes/r35_redhawk.cmake, test_r35_redhawk.c `pin`):
  - Pin 1 (10-14): Redhawk-Schuss (re15_player_weapon_fire(7)) toetet den Hund (ROOM11D0, RE2-KI,
    echter Spielschritt + ESP-Takt wie die Plattform): 6 Brocken, Mechanik je Brocken gegen R3/R4/R6,
    Messschiene 6 Landungen / 6 Abschluesse, **90 Bilder nach dem Schuss 0 lebende Fleisch-Plaetze**
    (vorher 6, M2), nach 900 Bildern 0.
  - Pin 2a (20-21): HE-Granate (Resolver-Art 2) -> Zeile 9 -> derselbe Router: 6 Brocken, alle enden.
  - Pin 2b (22-23): Pistole (Waffe 3) bis zum Tod: 5 Treffer, 0 Brocken entstanden (Zeile 3 hat keinen
    Brocken-Router; HURT-Bits nur per Zufall) -> 0 lebend.
  - Pin 3 (30-31): alle 84 Raeume mit Raum-Id 7, je Brocken-Sub (504) gespawnt und 300 Bilder getaktet:
    0 Haenger, 0 Mechanik-Fehler.
  Ergebnis lokal: GRUEN (0 Fehler).
* Messwerkzeuge (kein Test): `probe_r35_redhawk zensus | haenger [vorher] | messung [bilder]`.

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)
