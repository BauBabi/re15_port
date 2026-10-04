# Runde 35 Spur M — ROOM1120 Cut 1: Fenster zerbricht, Kraehe fliegt herein

Baum `.claude/worktrees/r35_cut11c0_fenster`, Zweig `r35/cut11c0_fenster`, Basis master 154a73c1.

## Auftrag (woertlich, AUFTRAG.md Z. 93)
> Wenn Leon dann in ROOM 1120 Cut1 Richtung dem Fenster hinten zulaeuft, sollen die Scheiben
> zerbrechen - so wie in Resident Evil 2 - und eine Kraehe "rein fliegen". Die Glas Zersplitter
> Effekt musst du aus Resident Evil 2 extrahieren, sowie der Knall Sound. Es waere super, wenn du
> dann auch im Background das Hintere Fenster etwas "beschaedigen" koenntest.

Zuteilung (VERTRAG.md): Bank-9-Bit 79 = "1120-Fenster zerbrochen"; Nachrichten 1120: 9..15 (nicht
gebraucht); Ereignis 24 (1120).

## 1. Messung vorher (am gebauten Stand 154a73c1)

* ROOM1120 (RE1.5, `shared_assets/PSX/STAGE1/ROOM1120.RDT`) hat KEIN Fenster-Ereignis. SCD-Vollwalk
  (`re15_port/tools/scd_dump_room.py`): main00 = Door 0 / Aot-oder-Door 1 / Door 2 / Aot 3 + drei
  Kraehen `44 00|01|02 21 ...` (@0x00D0C/@0x00D20/@0x00D34, am Boden y=0); sub00 = `01 00`; sub01 =
  nur Flag-Buchhaltung (3,54..56). Kein Sce_espr_on, kein Se_on, kein Obj_model_set. -> RE1.5 ist an
  dieser Stelle unfertig (fehlendes System), Ziel = RE2 Retail (Beta -> Retail, CLAUDE.md).
* Port-Zensus ROOM1120: kein Port-Installer (grep 0x1120 in engine/src: nur Karte/Speicherpunkt/
  Kraehen-KI-Kommentare). AOT-Slots 0..3 = main00, 48..63 = Kamerazonen -> Slot 4 frei.
* Kamera Cut 1 (RDT @0x60+0x20): fov 26684, P (5919,-2928,-1645), T (4280,-1744,5199).
  Rueckprojektion (tools/maske/geom.build_view, H = 208) des Fensters im Cut-1-Bild
  (`build/bg_ppm/ROOM11201.ppm`, Fensterrahmen x 180..210 / y 68..103, Scheiben x 182..208 / y 83..101)
  auf die Rueckwand z = 11200 (SCA-Zelle (-2700,11200,11350,2000) = Wandflaeche Sued):
  Rahmen x 4084..5929, y -3942..-1827; Scheiben x 4221..5808, y -3049..-1953; Mitte (5021,-2511).
  Gegenprobe: Wandfuss (x,0,11200) projiziert auf Bildzeile 130..132 = sichtbare Bodenkante (~y 128).
* Wege im Raum: Tuer 2 (-8950,-3900) -> ROOM1130 Cut 2; Tuer 0 (-1200,8550) -> ROOM1060; Tuer 1
  (300,5900) -> ROOM1080 nur mit (4,243). RVD: Cut 1 = Gang x 1000..8500, z -2000..13000
  (Eintrag 2 @0x128); Cut 0 <-> 1 an x 2400..4400 im hinteren Quergang (Eintraege 1/3). Der Weg
  1130 -> 1060/1080 fuehrt in Cut 1 NACH NORDEN (+z) auf das Fenster zu und biegt bei z ~6400 nach
  Westen ab (SCA-Block (-10500,1650,14000,4750) sperrt x < 3500 fuer z 1650..6400).

## 2. RE-Belege

### 2.1 RE2-Vorbild: ROOM1090 (RE2 Leon, Stage 1 Raum 09 = Gang mit Fenstern + Kraehen)
Zensus aller 495 RE2-RDTs (`Sce_em_set` Typ 0x21): Kraehen nur in room1090 (sub07..sub11) und
room2110. room1090 sub15 ist das Fenster-Ereignis (opcode-exakter Walk mit
`tools/re2_sicherung/re2_scd_walk.py`, 0 Desyncs in sub15):
```
sub03 @0x0010 Ck(4,0x31)==0 -> @0x0014 Aot_set 2c 06 05 41 00 00 b9 df 2d c3 34 08 7c 15 ff 00 18 0f 00 00
        (Slot 6, sat 0x41, Rechteck x -8263 w 2100, z -15571 d 5500, Gosub 0x0f = sub15)
        @0x0028 Gosub 9 (sub09 = vier versteckte Fensterkraehen + zwei weitere)
sub09 @0x0000 44 00 00 21 02 40 00 0d 00 00 ac f4 3c f6 74 c3 18 0c 00 00 00 00
        Kraehe 0: +4/+5 = 02 40 -> +0x10E = 0x4002, Lage (-2900,-2500,-15500), dir 0x0c18
        (Kraehen 1..3 ebenso, x -6900/-9800/-10300; Kraehen 4/5 ohne 0x4000 im Gang)
sub15 @0x0000 22 04 31 01            Set(4,0x31) = "Fenster zerbrochen"
      @0x0004 46 06 00 ..            Aot_reset(6) = Ausloeser aus
      @0x000E..0x002A 2e 03 0n 00 / 34 17 04 00   Work_set(Kraehe n) + Member_set(0x17,4) n=0..3
      @0x002E 09 / 0a 02 00          Sleep 2
      @0x0032/36 Set(14,4,0) Set(15,5,0)
      @0x003A 8a 00 03 00 03 00 / @0x0040 8a 00 03 00 07 00 / @0x0046 8b fa 08 00 00 00  (Rumble)
      @0x004C/50 Sca_id_set(15|20, 0x80de)
      @0x0054..0x01F4 27x 3a 00 <bank> <sub> 00 00 <skala u16> <x> <y> <z> 00 0c  (Glassplitter)
      @0x0204 Sleep 3   @0x0208 36 02 21 01 00 00 9c f0 00 00 50 c9  Se_on
      @0x0214 Sleep 5   @0x0218 36 02 21 01 00 00 9c f0 00 00 50 c9  Se_on (zweiter Knall)
      @0x0224/2A Sce_bgm_control, @0x0230 Gosub 0x25, @0x0232 Sleep 13,
      @0x0236.. Sca_id_set(15|20,0x80fe), Flr_set(1,1), Set(14,3,1), Set(15,4,1), Evt_end
```
Rueckprojektion der 27 Splitter- und 6 Kraehenlagen in die RE2-Kameras 2/3/4/7
(`camera.rid`, gleiche build_view-Formel): alle Splitter liegen auf den Scheiben der drei Fenster
(x ~ -2500 / -6300 / -9900, Glasebene z ~ -14600), die Kraehen dicht dahinter.

### 2.2 Opcodes (RE2 PSX.EXE `info/re2leon/PSX.EXE`, Dispatch 0x800A74C8)
* Sleep 0x09 -> 0x800539dc (PC+1, legt u16 der Folgezeile +2 als Zaehler ab); Sleeping 0x0A ->
  0x80053a24: Zaehler-- ; != 0 -> `jr ra / addiu v0,zero,2` @0x80053a84 (Takt abgeben); == 0 ->
  PC += 3 @0x80053a6c, Rueckgabe ebenfalls 2. => "Sleep n" laesst die Folgezeilen n Bilder spaeter laufen.
* Member_set 0x34 -> 0x80055c00 -> Setter 0x80055cb0, Feld 0x17 = Tabelle 0x80011228[23] = 0x80055d8c
  `sh a2,468(a0)` = Entity+0x1D4 := 4.
* Sce_espr_on 0x3A -> 0x800565a4: a0 = rec[2]<<24 | rec[3]<<16 | u16 rec+6 (@0x800565cc-0x80056608),
  Matrix = 0x80056a38(rec[4]=0,rec[5]=0) -> Sprungtabelle 0x80011470[0] = 0x80056a78 -> 0x8009DB44
  (Einheitsmatrix), SVECTOR = rec+8/+10/+12 (@0x800565ec-0x8005660c), a1 = lh rec+14 = Gier
  (@0x80056610), `jal 0x8001bf10` @0x80056614 (= sofort lebendig, Status 0xA003).
* Se_on 0x36 -> 0x80056428: a0 = rec[1]<<24 | rec[2]<<16 | rec[3] (@0x80056518-34), Lage = rec+6/8/10
  + Bezug 0 (rec+4 = 0 -> Fall 0 @0x80056478), `jal 0x8005ba28` @0x80056530. Fuer
  `36 02 21 01 ...` -> Code 0x02210001: Bank 2 (`srl t1,a0,24` @0x8005ba30) = RAUMBANK
  (DAT_800dbb80 = [RDT+8], s. tools/re2_door_se_cut.py), Satz 0x21 (@0x8005ba7c-80).
* 0x8A -> 0x80059348 -> `jal 0x8003947c`, 0x8B -> 0x80059394 -> `jal 0x80039514` = die zwei
  Rumble-Ringe (include/re15_rumble.h: Controller-Vibration, kein Kamera-Shake).

### 2.3 Der Glas-Effekt (RE2-Raum-ESP von room1090)
`info/re2leon/PL0/RDT/room1090/effect.esp` (3092 B), Ids `29 10 11 12 13 14 0C 19`, Bank-Offsets
rueckwaerts ab dem letzten Wort (FUN_8001bca0, wie re2fx_register_core): 0x10 @0x1A0, 0x11 @0x374,
0x12 @0x548, 0x13 @0x71C, 0x14 @0x8F0. Banken 0x10..0x13 = Splitter (je 6 Skripte, Sub 0..5,
identische Schritte, nur Textur/Anim verschieden), 0x14 = Aufprall-Glitzern. Je Skript 1 Teil,
2 Schritte (24 B):
```
Schritt 0: 01 00 00 00 00 10 00 10 00 0c 00 01 20 00 00 00 00 00 03 b0 20 00 00 00
           Op A 1, Op B 0, Aspekt 0x1000/0x1000, Beschl. (0,12,0), +0xB 1, v = (32|64|96|128, 0, 0),
           Status 0xB003, TPage-OR 0x20
Schritt 1: 00 10 27 27 00 10 00 10 00 0c 00 00 <vx> 00 <vy> 00 ...  Op A 0, Op B 16, step[2]=step[3]=39
           (Sub 1/2/3/4: vy = -100/-60/-100/-30 = Wurf nach oben; Sub 5: Beschl. 0x26 = 38)
Bank 0x14 Sub 0: Schritt 0 Op A 1 (Status 0xB003, Aspekt 0x1800), Schritt 1 Op 0/0; Anim 6 Bilder
           a 1, Anim[6] = {0,0,0,0} -> ENDE (Platz frei).
```
Neue Ops (RE2 Op-Tabelle 0x8009D868, selbst disassembliert):
* Op 16 = 0x8001f128: Boden = FUN_8004fba0({+0x34,+0x36,+0x38}, 2, 0x2000, 0) (@0x8001f13c-0x8001f15c),
  `sw v0,20(v1)` @0x8001f174 (+0x14 := Boden), Op A := +0x0B (`lbu a0,11 / sb a0,0` @0x8001f170/78),
  Op B := 5 (`addiu v0,zero,5 / sb v0,1` @0x8001f184/8c).
* Op 5 = 0x8001df90: w = Wasser(x,z) 0x800527b4 (@0x8001dfa8); w != 0 && w < y -> Spritzer
  0x1A01<<16|Skala (`lui a0,0x1a01` @0x8001dfec, `jal 0x8001cbe8` @0x8001dff0), Platz frei; sonst
  f = FUN_8004fba0(P,2,0x2000,0) (@0x8001e060); f < y -> Op[step+2] (@0x8001e070-90); sonst Kontakt
  (0x800dcbc8) != 0: f == +0x14 -> Op[step+3], sonst Op[step+2] (@0x8001e094-d8); kein Kontakt ->
  +0x14 := f (`sw a0,20(v0)` @0x8001e108). Aufruf `jalr` ueber 0x8009D868 (@0x8001e0dc-ec).
* Op 39 = 0x800206fc: Op A := 84 (`addiu v0,zero,84 / sb v0,0` @0x80020700/14), Op B := 0 (@0x80020724),
  aufgeschobener Spawn 0x14000000 | Skala(+0x3A) mit Einheitsmatrix an +0x34 (`lui a0,0x1400`
  @0x8002070c, `jal 0x8001cbe8` @0x80020740).
* Op 84 = 0x80025348: Op A/B := 0, Status := 0 (`sh zero,24(v0)` @0x80025378), nochmals Spawn
  0x14000000 | Skala an +0x34 (`jal 0x8001cbe8` @0x80025384).
* Texturen: `esp10.tim`..`esp14.tim` (4 bpp, CLUT 16x1, Bild 64 hw = 256 Texel breit, 32/32/64/40/32
  Zeilen); Bankkopf CLUT 0x7840 / TPage 0 (Lader legt die Seite um) + TPage-OR 0x20 = Mischmodus 1.

### 2.4 Der Knall (RE2-Raumbank von room1090)
`snd0.edt` (192 B) == RDT room1090 @[RDT+8]=0x75FC, `snd0.vh` == @0x76BC, `snd0.vb` == @0x84DC
(bytegleich geprueft). EDT-Satz 0x21 @0x84 = `00 00 7c 60`: Prog 0, Ton 7, Prio 0xC, Byte3 0x60 ->
Kanal 0, 3 Zusatzlagen (FUN_8005ba28: `lbu v1,3 / andi s1,v1,0x1f / srl s4,v1,5` @0x8005bacc-e0,
`srl s2,a0,4` Ton @0x8005badc, `andi fp,a0,0xf` Prio @0x8005bb04). Prog 0 Ton 7..10 = VAG 5, vol 127,
Mitte 80/80/73/80 (VH @0x820+k*32) = geschichteter Glasbruch.

### 2.5 Die Fensterkraehe (RE2-Kraehenmodul EMOVL21_S0.BIN, Ladeadresse 0x80100000)
* Spawn-Wort: RE2 Sce_em_set `lhu v0,4(v1)` @0x8005734c -> `sh v0,270(s0)` @0x80057354 (+0x10E = rec+4).
* INIT @0x80100428-34: +0x10E & 0x4000 -> @0x80100444-74: +4 := 4, +0x10E &= 0xbfff, +0x22A |= 1,
  +5 := +0x10E & 0xf (= 2), sofort `jalr` Zustandstabelle 0x80104908[4] (@0x80100480-88).
* State 4 = 0x801034DC (Navigator 0x8004a808, dann Tabelle 0x80104A64[+5]); Sub 2 = 0x801037f8:
  - P0 @0x80103854-74: +6 := 1, word0 |= 0x80008, +0x10E |= 0x4000.
  - P1 @0x80103878-d4: wartet auf +0x1D4 & 4; dann Anim-Wort 0x00070004 (Clip 4, Bild 0),
    +0x144 (Tempo) := 300, +0x219 := 6, +6 := 2, +0x1D4 := 0, +0x146 (vy) := 0,
    word0 &= 0xfff7ffff, +0x10E &= 0xbfff.
  - P2 @0x801038d8-0x80103930: 0x80015350(e,0,0) (Bewegung entlang der Blickrichtung), Anim 0x8002959c
    (a3 = 512), +0x144 -= 10, +0x219--; war der Zaehler 0: word0 &= ~8, 0x80104078(e,1,4) = ACTIVE Sub 4.
  -> 7 Bewegungsbilder mit 300,290,...,240 = 1890 Einheiten.
* word0-Bits: 0x80000 = NICHT ZEICHNEN (RE2 Zeichner pruefen `(*p & 0x80000) == 0`: FUN_80019b3c,
  FUN_80019cd0, FUN_800363f0, FUN_80027160); 0x8 = WAND-KOLLISION AUS (FUN_8003567c @0x80035694
  `andi v0,v1,0x8` / `bne v0,zero,0x800356ec` ueberspringt den Wandpass). Also: versteckt hinter dem
  Fenster, bei +0x1D4&4 sichtbar, fliegt 7 Bilder OHNE Wandklemme durch die Scheibe, danach normal.
* Sub 0 (0x80103554) / Sub 1 (0x8010363c, Wegpunkte (-12500,-12852)/(-3919,-12288) RE2-raumfest)
  werden von keinem Fenster-Spawn erreicht (rec+4 = 2) -> nicht Teil dieses Auftrags (OFFEN unten).

## 3. Entscheidungen (Port-Abbildung, NUTZER-VORGABE / PORT-WAHL gekennzeichnet)

* Ausloeser: AOT Slot 4, sce 3, sat 0x41 (AUTO, Form von RE2 sub03 @0x0014 `... 05 41 ...`), Ereignis 24
  (VERTRAG). Rechteck = RE2-Form "Band quer ueber den ganzen Gang, 2100 tief" (RE2 w 2100 / d 5500):
  x 3500..6650 (Gangbreite in Cut 1, SCA-Zellen x 3500 / 6650), z 4300..6400 (2100 tief, Nordkante =
  Ende des SCA-Blocks z 6400) -> PORT-WAHL der Lage.
* Story-Tor: (9,73) = "Irons-Todesszene gesehen" (Spur L) muss 1 sein — NUTZER-VORGABE "Wenn Leon DANN
  in ROOM 1120 ..." steht im Ablauf nach der 1150-Montage. Einmalig: (9,79).
* EIN Fenster, EINE Kraehe (NUTZER-VORGABE "eine Kraehe"): RE2-Fenster 1 (Splitter mit x in
  [-2800,-2200] = 13 der 27 Saetze) + RE2-Kraehe 0. Abbildung RE2 -> RE1.5 als Drehung um 180 Grad
  um die Hochachse (RE2 "in den Raum" = +z, RE1.5 = -z): X = 5000 - (x2 + 2500), Z = 11200 - (z2 + 14600),
  Y = y2; Gier 3072 -> 1024, Kraehen-dir 0x0c18 -> 0x0418. Bezugspunkte: RE1.5-Fenstermitte x 5000
  (Rueckprojektion 4221..5808), Rueckwand z 11200 (SCA); RE2-Fenster-1-Mitte x -2500 (Splitter
  -2200..-2800), Glasebene z -14600 (aeusserste Splitterreihe, sub15 @0x0144 / @0x01D4) -> PORT-WAHL.
* Zeitlinie = RE2 sub15 woertlich (Sleep 2 / 3 / 5): T+0 Kraehe frei, T+2 Rumble + Splitter, T+5 Knall,
  T+10 zweiter Knall.

## OFFEN
(wird gefuellt)

## Fuer den Nutzer
(wird gefuellt)
