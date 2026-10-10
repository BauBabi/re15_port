# Runde 36 — Spur A: klettern (BLOCKER)

Zweig `r36/klettern`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Runde-36-Auftrag). Dossier wird
fortlaufend geschrieben.

## Auftrag (Wortlaut)

> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern -
> z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

## Plan

1. REPRODUZIEREN am gebauten Stand (eigener Baum, ROOM10F0, Tritt verschieben + hochklettern).
2. BISECT v0.8.21 (9362dd7e) -> v0.8.22 (87cc8575) -> v0.8.23 (3ec5b1f8), dann die Runde-35-Commits.
3. ORIGINAL-Mechanismus (climb_common.c, FUN_... Adressen) gegen den Port stellen; Fix muss den Befund
   ERKLAEREN.
4. Pin (Mechanik: Tritt verschieben + hochklettern + oben ankommen, ueber den echten Weg) + Suite >= 592.

## Zensus ROOM10F0 (RDT selbst gelesen, tools/scd_dump_room.py)

- Der verschiebbare Tritt ist Obj_model_set **obj 0, Typ 4** in sub00:
  - @0x010F4 `2d 00 04 00 00 00 00 00 00 00 5e e8 00 00 56 13 00 00 00 04 00 00 32 00 7c fc 00 00 a3 02 84 03 58 02`
    = Startlage (-6050,0,4950), Band 0, Box c(50,-900,0) h(675,900,600), wenn Flag (3,102)==0 (Ck @0x010DC);
    dazu Aot_set Slot 10 sce 5 (Schiebezone) @0x010E0.
  - @0x0111A dieselben Bytes mit pos (-4850,0,-850) = Endlage, wenn (3,102)==1 (Else_ck @0x01116).
  - sub01 @0x01140: Work_set(3,0) + Member_cmp(15 == 10) -> Set (3,102)=1 (der Tritt ist angekommen).
  - Door_aot_set Slot 1 @0x00F52 -> ROOM1090, **floor 1**, Rechteck (-5350,-2250,1200,1000) = noerdlich der
    Endlage. Oberkante des Tritts = obj.y - 2*box_hy = -1800 = Band 1 -> man klettert auf den Tritt und
    erreicht von dort die Tuer nach ROOM1090. Ohne das Klettern ist ROOM1090 von hier nicht erreichbar.
- **Flags-Wort des Tritts: pc[6..7] = `00 00` -> `0x0000 | 1 = 0x0001`** (LAB_80040914 @0x80040990-a4
  `lhu v0,6(a2); ori v0,v0,0x1; sw v0,0(a1)`). Die Kisten in ROOM1090 tragen dagegen pc[6..7] = `00 01`
  -> 0x0101 (ROOM1090 @0x0216E/@0x02190).
- Zensus ueber alle 240 RDTs (scratch zensus_obj.py, Ausgabe im Dossier-Anhang unten): Typ-4-Objekte OHNE
  Bit 0x100 in ROOM10F0/10F1, 11A0/11A1, 2040/2041 (obj 1/2), 2060/2061 (obj 2), 20A0/20A1, 3091 (obj 1,
  0x0081), 5020/5021. Alle uebrigen Typ-4-Objekte (1080, 1090, 10D0, 1100, 11E0, 11F0, 1230, 2040 obj 0,
  2050, 2060 obj 3/4, 3050, 3090, 30E0, 4020, 5050, 50D0) tragen 0x0101. Typ-0-Objekte tragen ebenfalls
  Flag-Bits (10F0 obj 1..3: 0x000B; 11E0 obj 0: 0x03FD) — das Wort ist ein allgemeines Attributwort.

## Messung vorher (gebauter Stand master 5a9dbb7b, exe-Kopie re15_pc_r36a.exe)

Lauf K1: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=3:102,9:71 RE15_PLAYER_POS=-4800,400,1024
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W13,A0.1,W3 RE15_CLIMB_TRACE=climb.txt
RE15_EXIT_AT=560#10f0` (Tritt in der Endlage, Leon suedlich davor, Blick -Z auf den Tritt, Aktionstaste F391):
```
T642 room=10F0 pm=0 pad=8000 pos=(-4800,0,400) rot=1024 band=0 sub=0 kind=0x00 ph=0 obj=-1 stand=-1 ...
T643 room=10F0 pm=0 pad=8000 ... sub=0
T644 room=10F0 pm=0 pad=8000 ... sub=0
T645 room=10F0 pm=0 pad=0000 ... sub=0     (kein Substate 9 in 771 Zeilen: grep -c "sub=9" = 0)
```
-> **REPRODUZIERT: die Aktionstaste vor dem Tritt startet kein Klettern.**

Sonde `probe_r36_klettern_10f0 diag` (echte VM, ROOM10F0.RDT, beide Lagen, alle vier Anlaufseiten):
```
prop[3] id=0 type=4 active=1 flags=0x0001 band=0 pos=(-4850,0,-850)
[f1 yaw1024] spieler=(-4800,0,200) rot=1024 band=0 sonde=(-4800,-600) s6(typeword)=0x0000
             obj3 flags=0x0001 band=0 ... reach=1 boxhit(-112)=1 stand=-1
    try_start=0 sub=0 kind=0x00
```
(identisch fuer yaw 0/2048/3072 und fuer die Startlage): Reichweite (FUN_8002d1e8) = 1, Box-Treffer
(FUN_8002da4c, -112) = 1, Band gleich — **einzig das Flags-Gate der Port-Sonde faellt durch**, und das
SCA-Attributwort am Sondenpunkt ist 0 (kein `s6 & 1`-Weg, kein `s6 & 2`-Weg).

## Bisect-Tabelle

| Stand | Commit | Lauf K1 (Endlage, Aktionstaste vor dem Tritt) |
|---|---|---|
| v0.8.21 | 9362dd7e (build_bisect) | sub bleibt 0, kein Klettern (`grep -c "sub=9" = 0`) |
| v0.8.23 / master | 3ec5b1f8 / 5a9dbb7b | sub bleibt 0, kein Klettern |

`git log 9362dd7e..3ec5b1f8 -- engine/src/climb_common.c engine/src/re15_collision.c` ist LEER; das
Flags-Gate stammt aus b86957e3 (2026-08-21, "Klettern auf die Kiste (ROOM1090)"), die Kletterlogik ist
seit 92c17762 (2026-08-23) unveraendert. **Es ist KEINE Regression der Runde 35**: der Port konnte in
ROOM10F0 seit der Einfuehrung des byte-true Kletterns nie auf diesen Tritt klettern, weil die Sonde das
Flags-Wort falsch liest (unten). Der Nutzer-Befund "nicht mehr" erklaert sich daraus, dass die Runde 35
(Spur K) die Szene in ROOM10F0 einbaute und der Raum seither auf dem Spielweg liegt (Parkplatz-Schluessel
etc.) — vorher wurde der Tritt in 10F0 im Port nicht gebraucht/gespielt (Runde 22 mass 10F0 nur als PRI-
Standplatz-Zensus, Memory reai-v2-schiene-einseitig-und-synthetisch).

## RE-Belege

### FUN_8002d2c0 @0x8002d2c0 — "steht ein kletterbares Objekt an der Sonde?" (PSX.EXE, selbst disassembliert)
```
8002d344: lui v0,0x800b
8002d348: addiu v0,v0,16280            ; 0x800b3f98 = Objekt-Pool, Stride 148
8002d34c: addu s0,s2,v0
8002d350: lw v0,0(s0)                  ; Flags-Wort des Objekts (pool+0x00)
8002d358: andi v0,v0,0x101             ; Bytes 01 01 42 30
8002d35c: beq v0,zero,0x8002d3a4       ; NUR wenn (flags & 0x101) == 0 wird das Objekt uebersprungen
8002d364: lbu v1,0(s3)                 ; DAT_800acad6 Spieler-Band
8002d368: lbu v0,130(s0)               ; obj+0x82 Objekt-Band
8002d370: bne v1,v0,0x8002d3a4
8002d37c: jal 0x8002d1e8               ; Reichweite (563 voraus, Radius 900)
8002d394: jal 0x8002da4c               ; Box-Treffer (Rand -112)
8002d39c: bne v0,zero,0x8002d304       ; -> DAT_800ac78c = obj, Rueckgabe 1
```
Weil LAB_80040914 @0x80040990-a4 bei JEDER Installation Bit 0 setzt (`ori v0,v0,0x1`), ist
`(flags & 0x101) != 0` fuer JEDES aktive Objekt wahr — **das Gate prueft nur "aktiv", Bit 0x100 ist fuer
den Kletterstart ohne Bedeutung.** Der Port (climb_common.c:86) verlangte `(flags & 0x101) == 0x101`,
also ZUSAETZLICH Bit 0x100 — ein Lesefehler der Instruktion `beq v0,zero` (Rueckgabe 0 nur bei 0).
Folge: alle Typ-4-Objekte mit pc[7] = 0 (Zensus oben: 10F0, 11A0, 2040, 2060, 20A0, 3091, 5020 und die
Elza-Spiegel) waren im Port nicht kletterbar.

### Was Bit 0x100 wirklich ist — FUN_8002dc48 @0x8002dc48 (Aufrufer FUN_80031c44 @0x80031d38, Spieler-Dispatcher)
```
8002dd80: lw v1,0(s0)                  ; Flags des Objekts
8002dd88: andi v0,v1,0x1 / beq -> weiter   ; aktiv
8002dd94: andi v0,v1,0x100             ; Bytes 00 01 62 30
8002dd98: beq v0,zero,LAB_8002ddc0     ; ohne 0x100: Objekt wird NICHT betrachtet
8002dda4: lbu a3,0x82(s2) ; li a2,-0x104 ; addiu a3,a3,-1
8002ddb0: jal FUN_8002da4c             ; Spieler-Vorpunkt in der Box des Objekts im Band darunter?
8002ddb8: bne v0,zero,LAB_8002dd28 ; _ori v0,zero,0xd -> sb v0,0x5(s2) = Spieler-Substate 0xd
```
Bit 0x100 gehoert zur Absturz-/Herunterfall-Pruefung des Spielers (Substate 0xd, FUN_8002dc48; im Port
nicht portiert, game_step_common.c:1369 nennt sie nur) — nicht zum Kletterstart.

### FUN_8002d474 @0x8002d474 — Aufwaerts-Zweig (zur Bestaetigung, dass der Port sonst stimmt)
```
8002d524: jal 0x8003b7f0   ; s6 = SCA-Attributwort am Sondenpunkt (800 voraus)
8002d530: jal 0x8002d2c0   ; s5 = Objekt an der Sonde
8002d53c: bne s5,zero,0x8002d550
8002d544: andi v0,s0,0x1 / 8002d548: beq v0,zero,0x8002d634   ; sonst nur mit s6&1
8002d5bc/8002d5dc: jal 0x8002da4c (Rand 200, Band+1 / Band-2) -> 0 = blockiert
8002d604: sb 9 -> 0x800aca59 (Substate 9)
8002d610: beq s5,zero,0x8002d61c / 8002d614: ori v0,zero,0x81 / 8002d618: sb v0 -> 0x800aca5a (KIND 0x81)
```
Deckungsgleich mit climb_common.c:238-255. In ROOM10F0 ist s6 = 0x0000 an allen vier Anlaufseiten (Sonde
oben), der Tritt ist also im Original NUR ueber s5 (FUN_8002d2c0) kletterbar — und dort faellt er mit Flags
0x0001 durch das `andi 0x101 / beq zero`-Gate NICHT durch.

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
