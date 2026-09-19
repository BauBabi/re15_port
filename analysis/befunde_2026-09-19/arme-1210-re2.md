# ROOM1210 Gitterhaende nach RE2 ROOM2050 (EM2D "Zombie-Arme") — Runde 16, 2026-09-19

Thema-Kuerzel `arme-1210-re2`. Sonde: `re15_port/tests/unit/probe_r16_arme_1210_re2.c`
(Registrierung `re15_port/tests/unit/probes/r16_arme-1210-re2.cmake`, Build-Verzeichnis
`re15_port/build_r16_arme`). Sondenlog: `arme-1210-re2_sonde.log`. Volle Disassembly des RE2-Arm-
Overlays: `arme-1210-re2_em2d_ai1.dis` (1354 Instruktionen, `CDEMD0_EM2D_ai1.BIN` @0x80100000).

## 0. Kurzfassung

Der Port faehrt in ROOM1210 einen Zwitter aus der RE1.5-Stumpf-Maschine EM01A (4 Bones, Hand-Auslenkung 142) und
Bruchstuecken des RE2-Zombie-Griffs: gemessen reagiert beim normalen Gang durch die Flurmitte KEINER der zehn
Arme (0/10, Tor 4541 um den Ursprung geht nur 163 bzw. 377 Einheiten vor der Wand auf), an der Wand springt der
ganze Armkoerper in 4 Bildern 2420 Einheiten durch die Mauer, greift nie (0 Griffe in drei Bahnen) und faehrt
nach 44 Bildern wieder zurueck; vier der zehn Arme (Slots 1-4) sind konstruktionsbedingt tot.
Das RE2-Original ist ein EIGENER Gegnertyp 0x2D (CDEMD0.EMS Sektor 0x9B0..0x9C7: KI 0x1528 B, TIM 0x8220 B,
EMD 0x6020 B: 15 Bones = 2 Arme x 7, 6 Clips + 2 Opfer-Clips), dessen Ursprung IM Fenster sitzt (RE2: 300-520
hinter der Wandflaeche, 1930-2700 ueber dem Boden), der bei Abstand < 2500 im Frontkegel ausfaehrt (A0
@0x80100564-70), den Spieler mit 900/600-Radius um die HAND (Bone 3/10) greift (A1 @0x801007C4, A3 @0x801009BC),
ihn AN DIE HAND teleportiert (B4 @0x80100C18-38), 150 Bilder minus Mash haelt (@0x80100C8C, @0x80100D28-34)
und danach fuer immer ruht (0x701 @0x80100E40).
Der Port muss diesen Typ 1:1 als Modul `enemy_ai_re2_zellenarm.c` mit dem RE2-Modell EM2D bauen, die zehn
ROOM1210-Records als Anker (z aus dem RDT, x an die Wandflaeche -21090/-16676, y = -2513 wie EM01A) verwenden
und den RE1.5-Skript-Ausloeser `grid_id==1` (sub02 @0x1EDA) als RE2-"Weckruf" (member 7 &= 0x7FFF) abbilden.

## 1. Reproduktion / Messung

Sonde `probe_r16_arme_1210_re2` (RE2-Flavor gesetzt, ROOM1210.RDT geladen, SCD main+sub laufen, Spieler
laeuft ab z=-3500 mit 75/Bild ueber `re15_collision_constrain` bis z<-25500; drei Bahnen: Flurmitte
x=-19500, Westwand (Ziel -23000, Klemme bei -20622), Ostwand (Ziel -16000, Klemme bei -18164)).

### 1.1 Geometrie ROOM1210 (Teil B der Sonde)

```
slot  Spawn(x,z)         yaw   Flurkante(x) Ursprung->Kante  Lunge-Ende->Kante  Hand(+1671)->Kante
 1    (-25000, -8847)     0    -20622        4378              1958               287
 2    (-25000,-10247)     0    -20622        4378              1958               287
 3    (-14000, -5897)  2048    -18164        4164              1744                73
 4    (-14000, -7297)  2048    -18164        4164              1744                73
 5    (-25000,-15747)     0    -20622        4378              1958               287
 6    (-25000,-17130)     0    -20622        4378              1958               287
 7    (-14000,-12797)  2048    -18164        4164              1744                73
 8    (-14000,-14197)  2048    -18164        4164              1744                73
 9    (-14000,-21158)  2048    -18164        4164              1744                73
10    (-14000,-22365)  2048    -18164        4164              1744                73
SCA-Zellen: [6] x -22150..-21090 (Westwand, 1060 dick), [11] x -17696..-16676 (Ostwand, 1020 dick)
```
Begehbarer Flur x -20622..-18164 (Spieler-Mittelpunkt), Breite 2458. EM01A: 4 Meshes, 4 Bones
(Eltern 0:-1 1:0 2:0 3:0), Clips 30/39/30/54 Bilder, groesste Bone-Auslenkung 142 (Clip 2, Bild 4), Bone 1
steht bei Spawn auf Welt-y = -2513 (Zeile `f 0 arm 1 ... Bone1-Welt=(-25035,-2513,-8847)`).

### 1.2 Lauf Flurmitte (Teil A0)

```
f131 pl=(-19500,-13400) sub02: grid_id=1 auf allen Armen
Zusammenfassung Lauf: Griff-Ereignisse=0, Bilder im Griff=0
slot 1..10: aktive Bilder 0
```
**0 von 10 Armen reagieren.** Das Port-Tor `RE15_WRITHER_ERREICHT = 4091+450 = 4541` um den Arm-URSPRUNG
(enemy_ai_common.c:12154) ist in der Flurmitte (5500 zu beiden Reihen) nie erfuellt; es geht erst auf, wenn
der Spieler-Mittelpunkt naeher als 4541 an -25000 bzw. -14000 steht, also x <= -20459 (163 vor der Westklemme
-20622) bzw. x >= -18541 (377 vor der Ostklemme -18164).

### 1.3 Lauf an der Westwand (Teil A1, x auf -20622 geklemmt)

```
f147 arm 5 sub=1(LUNGE) ph=0 clip=0 pos=(-25000,-15747)            pl=(-20622,-14600) d=4521
f151 arm 5 sub=1(LUNGE) ph=2 clip=2 pos=(-22580,-15747) drift=2420 pl=(-20622,-14900) d=2126
f182 arm 5 sub=1(LUNGE) ph=3 clip=0 pos=(-23380,-15747) drift=1620
f186 arm 5 sub=2(GREIFEN) clip=0 pos=(-22580,-15747)               pl=(-20622,-17525) d=2684
f187 arm 5 sub=3(ZURUECK) ph=0 clip=1                              (GREIFEN dauerte 1 Bild)
f191 arm 5 sub=0(RUHE)  pos=(-25000,-15747)
(arm 6 identisch f165..f209; Arme 1,2 NIE: sub02 kommt erst bei z=-13400, sie liegen bei -8847/-10247)
Griff-Ereignisse=0 | slot 5/6: 44 aktive Bilder, max. x-Drift 2420
```

### 1.4 Lauf an der Ostwand (Teil A2, x auf -18164 geklemmt)

```
f131 arm 7 + arm 8 sub=1 im SELBEN Bild wie sub02 (pl z=-13400; Arm-z -12797/-14197)
f135 arm 7/8 ph=2 clip=2 pos=(-16420,..) drift=-2420    pl d=1962/1810
f170 arm 7/8 sub=2(GREIFEN)                             pl d=3932/2745   -> f171 ZURUECK (1 Bild)
f211/227 arm 9/10 LUNGE, f250/266 GREIFEN (8 Bilder, Clip 1), f259/275 ZURUECK, f263/279 RUHE
Griff-Ereignisse=0 | slot 7/8: 44 Bilder, slot 9/10: 52 Bilder, Drift 2420; Arme 3,4 NIE
```
Beim Arm 9 stand der Spieler im GREIFEN-Fenster bei d(Ursprung)=2126: Hand = Ursprung -1671 = -18091,
Spieler (-18164,-22400) -> Hand-Abstand sqrt(73^2+1242^2) = 1244 > Griff-Tor 1200 (@0x801018F4) — knapp
daneben, danach ist er vorbei. In keiner Bahn kommt ein Griff zustande.

### 1.5 Was "scheisse aussieht" (aus den Messwerten, nicht gedeutet)

1. **Nichts passiert** beim normalen Gang (Flurmitte): 0/10 Arme (1.2).
2. **Der ganze Arm-Koerper springt 2420 Einheiten in 4 Bildern durch die Mauer** (800/Bild, f147->f151), steht
   30 Bilder mit Clip 2 vor der Wand (Bone-Auslenkung 142, das Mesh ist ein Stumpf), zuckt und faehrt in 4
   Bildern zurueck — ein Koerper-Teleport, keine Arm-Bewegung. RE2 bewegt den Ursprung nie mehr als 400+500
   (B0 P0 @0x801006F8-718, P3 @0x80100780-8C) und blendet den Arm dabei ein/aus (0x80101164).
3. **Kein Griff, kein Zupack-Laut**: GREIFEN dauert 1 Bild (Arme 5-8) bzw. 8 Bilder (9/10) und wird durch
   `!reach` sofort abgebrochen; das Griff-Tor 1200 an der Hand geht nie auf.
4. **Vier tote Arme** (Slots 1-4 bei z -5897..-10247): sub02 setzt `grid_id=1` erst im Rechteck
   z -15000..-13300 (@0x1EAE), der Port-Einmalriegel + Reichweite lassen sie danach nie mehr ziehen.
5. **Falsches Modell**: EM01A ist ein 4-Bone-Stumpf ohne Opfer-Bank (victim_ok=0, Leihgabe vom Zombie 0x10);
   RE2 EM2D ist ein 15-Bone-Doppelarm mit eigener Opfer-Bank (2 Clips).

Nicht messbar in dieser Sonde: der Kamera-Schnitt (`g_scd.cam_id` blieb in allen Bahnen 0, die RVD-Zonen werden
im Test-Harness nicht getrieben); die Sichtbarkeitsfrage ist in `probe_1210_cutcheck` bereits gemessen
(Spawn-Punkte in 0 von 9 Cut-Vierecken, Lunge-Ende in 1-3), s. Kommentarblock enemy_ai_common.c:11996-12010.

## 2. Original-Mechanismus (RE2 Retail Leon, Typ 0x2D)

### 2.1 Identitaet des Typs

* ROOM2050.RDT main00 @0x1970..0x1A36: zehn `sce_em_set` (`44 00 NN 2d ...`), Typ **0x2D**, em-Id 0x11,
  kill 0xFF, floor 0. Dekodiert (pc[4..5]=Spawn-Flags -> +0x10E, pc[10/12/14]=x/y/z, pc[16]=dirY):
  ```
  @01970 slot0 flags 0010 pos (-27150,-2580,-12350) dir 0      @019DE slot5 flags 0010 pos (-12100,-2480,-7000) dir 1024
  @01986 slot1 flags 0001 pos (-27200,-2430,-13820) dir 0      @019F4 slot6 flags 0001 pos (-12500,-2000,-7000) dir 1024
  @0199C slot2 flags 0000 pos (-27150,-2180,-12050) dir 0      @01A0A slot7 flags 0000 pos (-12440,-2700,-7000) dir 1024
  @019B2 slot3 flags 0001 pos (-27200,-1930,-13320) dir 0      @01A20 slot8 flags 0001 pos (-12230,-2540,-7000) dir 1024
  @019C8 slot4 flags 0000 pos (-27130,-2200,-14000) dir 0      @01A36 slot9 flags 0000 pos (-12450,-2160,-7000) dir 1024
  ```
  Flag-Bit 0 = welcher der beiden Arme des Modells (INIT @0x80100190), Bit 4 (0x10) = Zeitgeber-Master der
  Gruppe (Root @0x80100068-80: nur ein Entity mit +0x10E&0x10 zaehlt den globalen Griff-Cooldown 0x800CFBF4
  herunter, @0x80100098-B0 ebenso den Overlay-SE-Cooldown 0x80101524).
* EXE-TOC @0x8009ADF4 (Datei 0x8B5F4), Typ 0x2D = Index (0x2D-0x10)*4: KI-Overlay Sektor 0x9B0/0x9B3
  (0x1528 B, Kopien gegen 0x8010D000/0x80100000), TIM Sektor 0x9B6 (0x8220 B, 128x256), EMD Sektor 0x9C7
  (0x6020 B). Geschnitten via `re15_port/tools/re2_ems_cut.py` -> `build/extracted/re2_ems/CDEMD0_EM2D_*`;
  CDEMD0- und CDEMD1-Kopie sind byte-identisch (`cmp`).
* Die Typen 0x2E (2x @0x17E0/0x17F6, EMD 0x34ECC B) und 0x2F (1x @0x17CA, EMD 0x1578 B) liegen im
  `ifel_ck Ck(1,6)==1`-Zweig (@0x17C2-0x180E) — eine spaetere Raumvariante (Ivy-Klasse), NICHT die Arme.

### 2.2 Modell EM2D (EMD @EMS 0x4E3800, dir @+0x6000, 8 Eintraege)

```
dir = [0x8, 0xC, 0x204, 0x27E4, 0x27E8, 0x27EC, 0x2894, 0x34CC]
Paar 1  EDD @0xC: 6 Clips 20/25/30/15/10/19 Bilder (Keyframes 0..118), EMR @0x204 {100,176,15 Bones,80}
Paar 2  EDD @0x27E4: leer
Paar 3  EDD @0x27EC: 2 OPFER-Clips 19/20 Bilder (Leon), EMR @0x2894 {0,8,15,80}
MD1 @0x34CC: 7764 B, 30 Objekte = 15 Meshes; Geometrie nur in Mesh 1,2,3 (Arm A) und 8,9,10 (Arm B)
Bone-Eltern: 0:-1 1:0 2:1 3:2 4:3 5:3 6:3 7:3 | 8:0 9:8 10:9 11:10 12:10 13:10 14:10
```
Hand-Reichweite (Sonde Teil C, Modellraum, x = vorwaerts, y PSX-abwaerts): Bone 3 (Hand A) max. x 1082-1088,
Fingerspitze Bone 7 max. 1300-1307 bei y -376..+306; Ruhe (Clip 0 Bild 0): Bone 7 bei (320,264,34).
Clip-Rollen aus der KI: 0/1 = Ausfahren/Tasten (B1 @0x80100878-88 `rng&1`), 3 = Zugriff (B3 @0x80100AEC),
5 = Halten (B4 @0x80100BC8) und Warten (B6 @0x80100EE8), 4 = Loslassen/Rueckzug (B5 @0x80100DD0-D4), 2 = nur
ueber den Hook-Zweig 0x80101338 (Rate 7).

### 2.3 Raum-Skript ROOM2050 (Datei-Offsets, RE2-Opcode-Laengen aus `re2_scd_lens.py`)

```
01940 aot_set  2c 03 05 41 00 00 24 93 44 cb a0 0f 80 0c ff 00 18 03 00 00   Zone 3: x -27868..-23868 z -13500..-10300, EVENT sub03
01954 aot_set  2c 04 05 41 00 00 5c c7 8c d8 e4 0c 48 0d ff 00 18 04 00 00   Zone 4: x -14500..-11200 z -10100..-6700,  EVENT sub04
01968 ifel_ck / 0196C ck 21 01 01 00 (Flag(1,1)==0)  -> 10x sce_em_set (2.1)
01A4C.. 10x { work_set 2e 03 NN ; member_copy 3d 10 07 ; calc 26 00 05 10 00 80 ; member_set2 35 07 10 }
        = member 7 |= 0x8000 fuer jeden Arm. Member 7 = Entity +0x10E (FUN_80055CB0 case 7: param_1[0x87]).
sub03 @01D40: cut_chg 09 ; 8a 00 0a 00 00 00 ; 8b b4 02 00 00 00 ; 8b fa 06 00 02 00 ; aot_reset 3 ;
        set(4,0x2E,1) ; 5x { work_set(3,0..4) ; member_copy 10,7 ; calc 26 00 06 10 ff 7f ; member_set2 7,10 }
        = member 7 &= 0x7FFF (Arme 0-4 WECKEN) ; sleep 1 ; se_on 36 02 0a 00 00 00 1b 28 f8 f8 9a cc ; cut_auto
sub04 @01DD0: dasselbe fuer Arme 5-9 mit cut_chg 0a, aot_reset 4, Flag(4,0x2F).
```
Bit 0x8000 in +0x10E wird vom Scheduler FUN_8004A694 waehrend des Ticks ausgeblendet (`andi v0,v1,0x7fff`
@0x8004A734, Restore @0x8004A798-A4) und von den EXE-Scans getestet (`+0x10E & 0xC000/0xE000 == 0`:
FUN_8003DB38, FUN_80042144, FUN_800410CC (Waffen-Applier), FUN_8003D96C, FUN_8004618C, FUN_80045C10,
FUN_800470C0, FUN_80047664) — der schlafende Arm ist damit weder Ziel noch Kollisionspartner. (Ob eine
Zeichner-Weiche das Bit zusaetzlich liest, ist NICHT belegt, s. §5.)

### 2.4 Zustandsmaschine des Overlays (alle Adressen `CDEMD0_EM2D_ai1.BIN` @0x80100000)

Tabellen: Routinen @0x80101424 = {INIT 0x801000F8, ACTIVE 0x801003C4, HURT 0x80100F50, DEATH 0x80100FE0,
SKRIPT 0x801010DC, 0, 0, 0x80101120}. ACTIVE waehlt ueber +0x10E&1 @0x801003CC-E4 eine von zwei INHALTSGLEICHEN
Varianten (@0x80101444: 0x80100404/0x80100488), jede ein Dual-Dispatch auf +0x5: A-Tabelle (Logik)
@0x8010144C = {0x8010050C, 0x801007A8, 0x80100990(jr), 0x801009A0, 0x80100B60(jr), 0x80100D88(jr),
0x80100E94(jr), 0x80100F40(jr)}, B-Tabelle (Bewegung/Anim) @0x8010146C = {0x80100674, 0x80100830,
0x80100998(jr), 0x80100A90, 0x80100B68, 0x80100D90, 0x80100E9C, 0x80100F48(jr)}.
Zustandswort +0x4 = {routine, sub, phase, -}: 0x101 REACH, 0x301 ZUGRIFF, 0x401 HALTEN, 0x501 RUECKZUG,
0x601 WARTEN, 0x701 ENDE.

**Root @0x80100018**: FUN_8004A808(self, &PL.x, 0, 0) (fuellt +0x1F0 Spielerabstand, EXE-seitig);
Pause bei 0x800CFBDC&0x20000000 @0x8010003C-4C; globaler Cooldown 0x800CFBF4-- und SE-Cooldown
0x80101524-- nur bei +0x10E&0x10 (@0x80100054-B0); Dispatch @0x801000B4-D8; danach FUN_80035530(self).

**INIT @0x801000F8**: +0x4=1; Hook 0x800CE3B4 = 0x8010121C (@0x8010011C-144); +0x1D0/+0x1CC/+0x1C8=0,
+0x1D3=0, +0x144/+0x148/+0x146=0; **+0x158 = rng&0xF** (@0x80100148-64); **+0x156 (HP) = 250** (@0x80100168-6C);
+0x224=0; +0x151/152/153 = 0x82; **Heimat**: +0x218 = yaw, +0x21C = x, +0x220 = z (@0x80100188-194);
Arm-Wahl @0x80100190-2F8: +0x1C0=0, +0x1C1 = Bone-Tabelle[var*7] (0x80101414 = {1..7 | 8..14}), fuer die
7 Bones des aktiven Arms part[k].+0x9C/+0x9E=96, +0xA0=512, +0xA2=312, +0x98/+0x9A=0 (Part-Stride 0xAC ab
+0x198), die 7 Bones des ANDEREN Arms bekommen word0 = 0 (@0x80100240/@0x801002F8); dann
0x80101164(self, var, 0) (Parts-Bit0 loeschen, Entity-word0 |= 2 = AUSGEBLENDET); +0x1E8=1;
+0x9E=500, +0x9A/+0x9C/+0x90/+0x92/+0x1EE = 800 (@0x8010032C-34C); word0 |= 0xC000000 | 0x1404; Schatten
FUN_80016480(+0x16C, 0, 0x02BC0258, 0, part+0x5C) @0x80100378-A4.

**Sub 0 — RUHE. B0 @0x80100674** (Phasen +0x6):
P0 @0x801006C8: Clip-Wort +0x14C = 0x000F0000 (Clip 0, Rate 15), Advance FUN_8002959C(…,256); Pose = Heimat
(+0x76/+0x38/+0x40 := +0x218/+0x21C/+0x220 @0x8010070C-714), **+0x144 = 400, FUN_800152C8(self, 2048)** =
400 rueckwaerts hinter die Heimat (@0x801006F8-718), +0x1D3 |= 0x80, Phase 1.
P1 @0x80100720: +0x158-- ; bei 0 Phase 2. P2: nichts. P3 @0x80100740 (vom A0 gesetzt): Advance; fertig ->
**0x101** + Pose = Heimat (@0x80100750-64); sonst bei Bild +0x14D < 11: +0x144 = 50, FUN_800152C8(self, 0)
(50/Bild vorwaerts, @0x80100768-90).
**A0 @0x8010050C**: a = FUN_80015614(self, PL.x, PL.z, 1024), b = FUN_80015614(…, 760); s2 = +0x1F0.
FUN_80015614 (EXE @0x80015614-98): d = (Peilung - yaw + half)&0xFFF; d < 2*half -> 0 ("im Kegel"), sonst
+-half. Zweig 1 @0x80100564-5CC: `s2 < 0x9C4 (2500)` && a==0 && Phase >= 2 -> **0x101**, 0x80101164(self,var,1)
(Parts sichtbar, word0 &= ~2), Pose = Heimat, +0x1D3 = 0; Zweig 2 @0x801005D4-618: `s2 < 0xBB8 (3000)` &&
b==0 && Phase == 2 -> Phase 3 (Kriechen), Parts sichtbar, +0x1D3 = 0. Beide Zweige: erstes Mal +0x224 = 1
(@0x80100654), ab dem zweiten Mal SE 4 ueber FUN_8005BD6C, wenn 0x80101524 == 0, danach 0x80101524 = 60
(@0x80100620-644). (Der zweite Ausloeser ist erreichbar: Zweig 2 bei 2500..3000, danach Zweig 1 bei < 2500
waehrend Phase 3.)

**Sub 1 — REACH. B1 @0x80100830**: P0 @0x80100870-D0: +0x14C = 0xF0000 | (rng&1) (Clip 0 oder 1), Phase 1,
**+0x158 = (rng&0x1F)+60**, mit 50 %: SE (rng&1 ? 0 : 1). P1 @0x801008D8: +0x76 += FUN_80015614(…, 32)
(Nachfuehren 32/Bild), Klemme auf Heimat-Yaw +-384 (@0x80100900-940), Advance(256); +0x158-- ; bei 0 ->
**0x501** (@0x80100958-70).
**A1 @0x801007A8**: FUN_800157D4(&PL, part[Bone-Tabelle[var*7+2]]+0x5C, 900) (Abstand Spieler <-> Hand-Bone
3 bzw. 10 STRIKT < 900, EXE @0x800157D4-81C) -> **0x301** (@0x8010080C-18).

**Sub 3 — ZUGRIFF. B3 @0x80100A90**: P0: +0x14C = 0xF0003 (Clip 3), Phase 1; P1: FUN_80015558(self, PL.x,
PL.z, 48) (Eindrehen), Advance -> Phase += fertig; P2 -> **0x501** (verfehlt, @0x80100AE0/B40).
**A3 @0x801009A0**: Hand-Test Radius 600 (@0x801009BC-A08) && +0x14D >= 5 && 0x800CFBF4 == 0 &&
PL+0x1D3&0x80 == 0 -> **0x401** + PL+0x1D3 |= 0x80 (@0x80100A50-6C); Spieler bereits beansprucht -> **0x601**.

**Sub 4 — HALTEN. B4 @0x80100B68**: P0 @0x80100BC4-CB0: +0x14C = 0xF0005 (Clip 5), Phase 1,
**PL.x/PL.z := part[Hand].+0x5C/+0x64** (0x800CFC30/0x800CFC38 @0x80100C18-38 — der Spieler wird an die Hand
gesetzt), 0x800CFDAC = self, 0x800CFD80/84 = +0x188/+0x18C, **PL+0x4 = 5** (Spieler-Routine 5,
@0x80100C4C-54), Rumble FUN_8003947C(4,0)/80039514(4,250,0)/800395B8(100,50,150,0), **+0x15A = 150**
(@0x80100C8C-90). P1 @0x80100CB4: bei Bild 0: Rumble + SE (rng&1 ? 5 : 7) (@0x80100CC0-D08); Advance;
**+0x15A -= 1 + 2*FUN_8001598C()** (Mash, @0x80100D20-38); < 0 -> Phase 2, **PL+0x6 = 2** (@0x80100D48-50).
P2 @0x80100D5C: **0x800CFBF4 = 120** (globaler Griff-Cooldown), **0x501**.

**Sub 5 — RUECKZUG. B5 @0x80100D90**: P0: +0x14C = 0xF0004 (Clip 4, 10 Bilder), +0x1D3 |= 0x80, SE 6
(@0x80100DD0-F8). P1: Advance; jedes Bild +0x144 = 30, FUN_800152C8(self, 2048) (30/Bild zurueck,
@0x80100E60-74); fertig -> +0x158 = (rng&0x1F)+30, 0x80101164(self,var,0) (ausblenden), **0x701**,
**+0x156 = -1**, +0x1D3 |= 0x80 (@0x80100E18-58).
**Sub 6 — WARTEN. B6 @0x80100E9C**: Clip 5; sobald PL+0x1D3&0x80 == 0 -> Phase 2 -> **0x101** (erneuter Griff).
**Sub 7 — ENDE**: A7/B7 = `jr ra` (@0x80100F40/48). Kein Setzer fuehrt aus 7 heraus: **jeder Arm greift
genau einmal**, danach ist er unsichtbar und mit HP -1 kein Ziel.

**HURT @0x80100F50** -> Tabelle @0x801014CC[+0x1D2] -> 0x80100F8C: **0x501** + SE (rng&1 ? 2 : 3).
**DEATH @0x80100FE0** -> @0x8010101C: FUN_8001BF10(0x21000, yaw, part[Bone-Tabelle[var*7]]+0x48, {0,0,0})
(Blut), **+0x156 = 250** (Arm ist unsterblich), **0x501**, SE 2/3 (@0x80101090-C4). Schuss auf den Arm =
Rueckzug.
**Hook 0x800CE3B4 = 0x8010121C** -> 0x80101258 (Phasen ueber +0x6 via Tabelle @0x80100004): Clip 0 + FX
FUN_8005BA28(0x4000001 | (1|2)<<16, self+0x38), Eindrehen 2048, Clip 1 Rate 7, Ende: +0x4 = 1 und
PL+0x1D3 &= 0x7F (@0x80101374-390). Aufrufer im EXE NICHT gefunden (§5).

### 2.5 SE-Bank

ENEMSE-Paar-Tabelle @0x800A7400 ist nach EM-ID indiziert: em-Id 0x11 steht in Zeile 42 = {0x05, 0x11}
(zweite Haelfte, flag2000 -> +0x10 im Map). Vom Arm benutzte Ids: 0/1 Ausfahren (50 %), 2/3 Treffer/Tod,
4 Ausloeser (Cooldown 60, overlay-global), 5/7 Zupacken, 6 Loslassen.

### 2.6 RE2-Geometrie (ROOM2050 collision.sca, RE2-Format 16 B/Eintrag)

Westwand [3] x -28972..-26852 (Arme bei -27150/-27200 = 298/348 HINTER der Wandflaeche), Block [0]
x -24802..-19532 -> Flur 2050 breit; Ostwand [4] z -7523..-5413 (Arme bei -7000 = 523 hinter der Flaeche),
Block [10]/[11] ab z -9563/-11363 -> Flur 2040 breit. Die Hand (1082..1300 vor dem Ursprung) liegt damit
730..1000 IM Flur; die A0-Tore 2500/3000 decken den ganzen Flur neben dem Fenster ab. Arm-y -1930..-2700
ueber dem Boden (Tuer-Record @0x171E: next_y = 0).

## 3. Port-Ist (Datei:Zeile)

* `re15_port/engine/src/enemy_ai_common.c:14038` dispatcht Typ 0x1A auf `re15_writher_ai_tick` (:11845) —
  ein Zwitter: RE1.5-EM01A-Lunge (Sub 1, :12180-12233, 3x800 + 20 vor, 30x20 zurueck, 4x200 vor) +
  RE2-Zombie-Griffbausteine (Sub 2 GREIFEN :12235-12310 mit Griff-Tor 1200 an der Hand, Sub 4 HALTEN
  :12312-12470 mit Budget 148, Sub 5 ABWERFEN, Sub 3 ZURUECK :12475-12510) + Einmalriegel
  `s_writher_verbraucht` (:11789) + Reichweiten-Tor 4541 um den Ursprung (:12154-12158).
* Warum falsch (gemessen §1): das Tor 4541 misst gegen einen Ursprung, der 4164/4378 vom Laufraum entfernt
  liegt (Flurmitte 5500) -> 0/10 in der Mitte; die Lunge verschiebt den Koerper 2420 (RE2: Ursprung fest,
  Arm wird ein-/ausgeblendet); GREIFEN endet nach 1-8 Bildern an `!reach`; Griff-Tor 1200 an einer
  Stumpf-Hand (Auslenkung 142, "MESH_REACH 1671" ist Mesh-, nicht Bone-Reichweite); vier Arme koennen wegen
  sub02-Zeitpunkt + Einmalriegel nie feuern; Modell EM01A ohne Opfer-Bank (Leihgabe Zombie 0x10).
* Modell-Lader `re15_port/platform/pc/main.c:746 pc_enemy_load_ex` laedt fuer 0x1A die RE1.5-Bank; der
  RE2-Zweig (:832) greift nur fuer Typen mit `re15_re2_owns_type` (Zombie-Familie/Hund/Kraehe/Spinne) und
  tauscht danach das Mesh gegen RE1.5 (`pc_enemy_hybrid_re15_models`). Praezedenz fuer ein RAUMGEBUNDENES
  reines RE2-Modell: Birkin EM36 (:753-820).
* Pins, die den Ist-Stand festschreiben und beim Fix fallen/neu verankert werden muessen:
  `unit_1210_gitterhaende` (CMakeLists.txt:3230), `unit_1210_arme` (:3628).

## 4. Fix-Plan (Phase 2)

1. **Neues Modul `re15_port/engine/src/enemy_ai_re2_zellenarm.c`** nach dem Muster von enemy_ai_re2_dog.c
   (Root/A-B-Dual-Dispatch, `re15_ai_set_state_word`), Einstieg `re15_re2arm_tick(slot)` aus dem 0x1A-Zweig
   (enemy_ai_common.c:14038) wenn `re15_ai_re2_for_type(0x1A)`; RE1.5-Writher bleibt der RE1.5-Default.
   Zustaende/Konstanten 1:1 aus §2.4 (jede mit @0x): Sub 0 P0 400 zurueck @0x801006F8, P3 50/Bild fuer Bild<11
   @0x80100770-84, A0 2500/@0x80100564 + Kegel 1024, 3000/@0x801005D4 + Kegel 760, +0x224-Latch + SE 4 cd 60
   @0x80100620-654; Sub 1 Clip rng&1 @0x80100878, Timer (rng&0x1F)+60 @0x80100898-9C, Nachfuehren 32 mit
   Klemme +-384 @0x801008E8-940, A1 Hand-Radius 900 @0x801007C4; Sub 3 Clip 3, Slew 48 @0x80100B14, A3 Radius
   600 @0x801009BC, Bild>=5 @0x80100A18, Cooldown 0x800CFBF4 @0x80100A28, Ein-Angreifer-Riegel PL+0x1D3&0x80
   @0x80100A3C-48; Sub 4 Clip 5, Teleport an die Hand @0x80100C18-38, Budget 150 @0x80100C8C, -1-2*mash
   @0x80100D28-34, Release PL+0x6=2 @0x80100D50, Cooldown 120 @0x80100D5C; Sub 5 Clip 4, 30/Bild zurueck
   @0x80100E64, Ende 0x701 + HP -1 @0x80100E40-50; Sub 6 Warten @0x80100F08-24; HURT/DEATH -> 0x501 + HP 250
   @0x80101090-A0. Port-Felder: `re2z_t158` (+0x158), `re2z_t15a` (+0x15A), `re2z_self1d3`, `re2z_f10e`
   (+0x10E: Arm-Wahl Bit0, Master Bit4, Schlaf 0x8000), neue Felder fuer +0x218/+0x21C/+0x220 (Heimat),
   +0x224 (Erst-Latch), +0x1C1 (aktiver Arm). Globale: Griff-Cooldown (0x800CFBF4-Analog, raumweit, wie
   `s_re2d_gflags` beim Hund) und SE-Cooldown 0x80101524.
   Helfer, die der Port schon hat: `re15_re2_advance_959c` (FUN_8002959C), `re2d_move`-Form (FUN_800152C8:
   x += cos*spd, z -= sin*spd), `re2z_sector`/FUN_80015614-Kegel (Rueckgabe 0/+-half), FUN_800157D4-Form
   (dist2D(PL, Bone) < r, dog :530), `re15_enemy_bone_world_pos` fuer Bone 3/10, Victim-Shim
   `re15_player_victim_latch_ex` + `re15_re2z_player_pin` + `re15_re2z_mash` + `re15_player_victim_throwoff`.
2. **Modell**: `pc_enemy_load_ex` bekommt fuer Typ 0x1A im RE2-Flavor den Zweig "RE2 EM2D (kind 0x2D) OHNE
   Hybrid" (wie Birkin EM36, main.c:753): `re2_ems_load_bank(ems, sz, 0x2D, eb, &tim)`; Opfer-Bank = Paar 3
   des EM2D (2 Clips) -> keine Leihgabe mehr (`re15_victim_donor_set(0x1A,…)` entfaellt). Zeichnen: nur die
   7 Bones des aktiven Arms (INIT word0=0 fuer den anderen Arm @0x80100240/@0x801002F8), ausgeblendet
   solange Entity-Bit 2 gesetzt (0x80101164 a2=0). Renderer-Seite: Part-Sichtbarkeitsmaske wie beim
   Kraehen-GIB (`crow_hide`-Muster).
3. **Anker in ROOM1210** (Nachruestung, jede Zahl belegt): z und Reihe aus den zehn Sce_em_set-Records
   (@0x1D86..0x1E3A); x = Wandflaeche der SCA-Zelle: West `-21090` ([6] x -22150..-21090), Ost `-16676`
   ([11] x -17696..-16676) — RE2 setzt den Ursprung 298..523 HINTER die Flaeche (§2.6); ob die Gitterstaebe von
   ROOM1210 vor oder in der Zelle stehen, muss am Bild gemessen werden (§5.3), Startwert = Flaeche;
   Yaw 0 (West) / 2048 (Ost) wie die Records; y = -2513 (gemessene Welt-y von EM01A-Bone 1 in ROOM1210 =
   die Hoehe, fuer die die Raumkunst gebaut ist; RE2 liegt bei -1930..-2700). Hand-Reichweite 1082/1300 ->
   Hand bei -20008..-19790 (West) / -17758..-17976 (Ost) im Laufraum -20622..-18164; Griff-Radius 600 deckt
   damit die westliche/oestliche Flurhaelfte, Flurmitte -19393 liegt 97 ausserhalb (RE2-Verhaeltnis: Hand
   730..1000 im 2050er-Flur) — Feinlage ueber §5.3, nicht raten.
4. **Ausloeser**: RE2 weckt je Gruppe ueber AOT + `member 7 &= 0x7FFF`. ROOM1210s eigener Ausloeser ist
   sub02 `Member_set(12,1)` @0x1EDA (grid_id = 1, AOT @0x1EAE). Abbildung: Arm schlaeft (+0x10E|0x8000-Analog:
   kein A0, kein Ziel, unsichtbar) bis `grid_id&0x1F == 1`; danach laeuft A0 pro Arm mit 2500/3000 + Kegel —
   das ergibt RE2s gestaffeltes Reagieren ohne erfundene Zonen. Arme hinter dem Spieler (Slots 1-4) reagieren
   erst, wenn er zurueckgeht (RE2-Verhalten: A0 kennt keine Richtung ausser dem Frontkegel).
   Der Cut-Wechsel (RE2 `cut_chg 9/10` + `cut_auto`) hat in ROOM1210 kein Datenvorbild -> nicht bauen.
5. **SE**: ENEMSE Bank 42, zweite Haelfte (flag2000=1), Ids 0/1/2/3/4/5/6/7 wie §2.5 ueber
   `re15_audio_re2_enemy_bank(42)` + `re15_audio_re2_enemy_se(id, 1)` (Audio-Hook wie
   `re15_re2dog_audio_hook`). Alternative RE1.5-Raumbank-Mapping nur auf Nutzerentscheid (Zombie-Mandat
   2026-08-23 galt der Zombie-Familie).
6. **Reihenfolge**: (a) Modul + Modell-Lader + Sonde `probe_r16_arme_1210_re2` erweitern (RE2-Flavor-Lauf
   muss zeigen: Flurmitte -> Arme 5-10 reagieren gestaffelt, Griff-Ereignisse > 0, Ursprung-Drift <= 400+500,
   Ende 0x701); (b) Pins `unit_1210_gitterhaende`/`unit_1210_arme` auf die RE2-Maschine neu verankern
   (Zustandsfolge 0x101->0x301->0x401->0x501->0x701, Teleport-Punkt = Hand, Cooldown 120 raumweit);
   (c) Sichtpruefung per gdigrab (Skill re15-port-visual-verify) fuer Hoehe/x-Lage der Anker;
   (d) Paketieren.
7. **Risiken**: Teleport des Spielers an die Hand muss durch `re15_collision_constrain` abgesichert werden
   (RE2 braucht das nicht, die Hand liegt dort im Laufraum; in ROOM1210 nach Punkt 3 ebenfalls, Messung
   pflicht); Kamera-Vierecke: Ursprung an der Wandflaeche liegt naeher am Laufraum als die alten Spawns
   (probe_1210_cutcheck erneut fahren); die RE2-Scheduler-Frage zu +0x158 (§5.1) ist fuer das Modul
   irrelevant (der Port tickt ohne Scheduler), aber dokumentiert.

## 5. Offen / nicht belegt

1. **Scheduler-Widerspruch**: FUN_8004A694 (@0x8004A740 `lh s1,0x158(s0)`, @0x8004A75C `beq s1,zero`)
   liest vor dem Tick +0x158 und erzwingt bei != 0 nach dem Tick `+0x4 = ((v&0xFF00)-0x100)|4`
   (@0x8004A760-74) — das kollidiert mit +0x158 als Timer (Arm @0x80100164, RE2-Zombie @0x8010282C).
   Enemies stehen nachweislich in derselben Liste (FUN_80052B38 prueft +0x1FA). Nicht aufgeloest; das
   Port-Modul ist davon unabhaengig.
2. **Spieler-Routine 5** (PL+0x4 = 5, @0x80100C4C-54): der EXE-Handler (Opferanimation aus Paar 3, Schaden,
   Mash-Signal an FUN_8001598C) ist nicht RE'd — im Port uebernimmt der Victim-Shim (wie beim Hund, Griff-Art 6).
   Ob RE2 waehrend des Arm-Griffs HP abzieht, ist damit NICHT belegt (der Arm selbst ruft FUN_800401D4 nie).
3. **Bildlage der Gitterstaebe in ROOM1210** (x-Ebene relativ zur SCA-Zelle, Hoehe): nur per Screenshot/PRI
   messbar; Startwerte in §4.3 sind Datenwerte, keine Sichtpruefung.
4. **Hook 0x800CE3B4** (Arm-Routine 0x8010121C): Aufrufer im EXE nicht gefunden (nur die Definition
   ghidra_re2_Leon.txt:489304); Zweck (Freikommen/Skript) unbelegt.
5. Opcodes 0x8A/0x8B in sub03/04 (`8a 00 0a 00 00 00`, `8b b4 02 00 00 00`, `8b fa 06 00 02 00`) nicht
   dekodiert; Kamera-Cut 9/10 hat in ROOM1210 kein Gegenstueck.
6. Ob eine RE2-Zeichner-Weiche +0x10E&0x8000 liest (schlafender Arm unsichtbar allein ueber word0-Bit 2
   oder zusaetzlich ueber das Bit) — die Decompile-Treffer (§2.3) sind Scans, kein Zeichner.
7. Sonde: der Kamera-Cut wird im Harness nicht getrieben (cam_id 0 in allen Bahnen); Sichtbarkeitsaussagen
   stuetzen sich auf probe_1210_cutcheck (aelter).

## 6. Umsetzung (Phase 2)

Stand 2026-09-19, Worktree `.claude/worktrees/wf_074e2f88-24e-5` (Branch `worktree-wf_074e2f88-24e-5`),
Build `re15_port/build_p2`. Der Fix-Plan (§4) ist gebaut, die in §5.3 offene Anker-Messung ist
nachgeholt — nicht per Screenshot-Augenmass, sondern durch Rueckprojektion des EXTRAHIERTEN
Original-Hintergrunds durch die byte-true Kamera.

### 6.1 Was gebaut wurde

* **NEU `re15_port/engine/src/enemy_ai_re2_zellenarm.c`** (+ `include/re15_enemy_ai_re2_zellenarm.h`):
  der RE2-Typ 0x2D als eigenes Gehirn — Root @0x80100018, Routinen-Tabelle @0x80101424,
  A-/B-Dual-Dispatch (@0x8010144C / @0x8010146C), Sub 0..7 mit JEDER Konstante am `@0x` der
  Disassembly `CDEMD0_EM2D_ai1.BIN` (Weckruf-Tor, 2500/3000 + Kegel 1024/760, Hand-Radien 900/600,
  Teleport an die Hand @0x80100C18-38, Halte-Budget 150 @0x80100C8C, Mash -1-2x @0x80100D28-34,
  raumweiter Cooldown 120 @0x80100D5C, Rueckzug 30/Bild @0x80100E64, Ende 0x701 + hp -1
  @0x80100E40-50, HURT/DEATH -> 0x501). Der Spieler-Hook 0x8010121C (Spieler-Routine 5 ueber
  PTR_LAB_800A4030[5] = 0x8004006C -> 0x800CE300[0x2D] = 0x800CE3B4) ist mitportiert; SE laufen
  ueber ENEMSE-Bank 42, zweite Haelfte (Paar-Zeile {0x05,0x11} @0x800A7400 = EXE-Datei 0x97C54).
  Zwei Skeptiker-Luecken sind dabei geschlossen: der Scheduler-Widerspruch (§5.1 — FUN_8004A694
  ist der RAUMLADE-Tick, einziger Aufrufer FUN_80049E48 @0x8004A35C, also ist +0x158 danach ein
  freier Zeitgeber) und der Griff-Schaden (§5.2 — weder Hook noch Overlay rufen FUN_800401D4,
  der Griff kostet keine HP).
* **`engine/src/enemy_ai_common.c`**: im 0x1A-Zweig (:14041) uebernimmt unter dem RE2-Flavor
  `re15_re2arm_tick(slot)` den GANZEN Dispatch; der RE1.5-Writher bleibt der RE1.5-Default.
* **`platform/pc/main.c`**: Lader `pc_enemy_load_re2_kind(type, kind, eb)` (der RE2-kind ist jetzt
  vom RE1.5-Typ getrennt) laedt fuer 0x1A im RE2-Flavor das Modell EM2D (kind 0x2D) REIN, ohne
  Hybrid — 15 Bones, 6 Clips, EIGENE Opferbank (Paar 3, 2 Clips). Zeichner: verborgener Arm
  (Entity-Bit 2, 0x80101164 mit a2=0) ohne Mesh UND ohne Schatten; die sieben Bones des inaktiven
  Arms fallen ueber eine flache Part-Maske weg (INIT `sw zero,0(part)` @0x80100240 / @0x801002F8).
* **Pins**: neu `tests/unit/test_p2_1210_arme_re2.c` (Registrierung `probes/p2_arme-1210-re2.cmake`,
  Test `unit_1210_arme_re2`), 22 Pruefungen, alle gruen. Die beiden alten Pins
  `unit_1210_gitterhaende` / `unit_1210_arme` sind auf den RE1.5-Flavor neu verankert
  (`re15_ai_flavor_set(RE15_AI_FLAVOR_RE15)`): sie beschreiben die RE1.5-Nachruestung, die dort
  unveraendert weiterlaeuft.

### 6.2 Anker-Messung am Original-Hintergrund (die offene §5.3)

Werkzeuge + Rohausgaben: `analysis/befunde_2026-09-19/phase2_arme-1210-re2/anker_mess.c`,
`anker_mess2.c`, `re2_mess.c`, Log `anker_mess.log`. Alle drei rechnen mit der Engine-Kamera
(`re15_camera_build_view` = FUN_80053ca4) und der byte-true Projektion `sx = 160 + H*x/z`,
`H = fov>>7 = 208`; gemessen wird auf den extrahierten Original-Hintergruenden
(`extracted/PSX/STAGE1/ROOM121/ROOM1210{3,4}.bmp`, RE2: `info/re2leon/COMMON/BSS/ROOM205/ROOM20509.bmp`).

| Messung | Ergebnis |
|---|---|
| Fensterbank des westlichen Gitterfensters, Cut 4 (pos -18522,-1962,-24048), Pixel v=104 bei u=55/75/95 auf der Ebene x=-21090 | y = **-2040 / -2057 / -2082** — eine waagerechte Kante. Bank **y ~ -2060**; zugleich der Beleg, dass die Kunst die SCA-Flaeche -21090 meint |
| z-Ausdehnung desselben Fensters (u 95 -> u 55) | z **-16862 .. -19428** |
| Noerdliches Westfenster, Cut 3 (pos -19008,-3132,-14274), Bankkante v~114 bei u=78 | y ~ **-2310** (zwischen den Vorwaertsproben y-2000 -> v128 und y-2513 -> v105) |
| RE2-Gegenprobe: die zehn ROOM2050-Ursprünge (@0x1970..0x1A36) in den RE2-Hintergrund Cut 9 projiziert | alle fuenf Westarme liegen IN den dunklen Oeffnungen der vernagelten Fensterwand (`re2_2050_cut9_arme.png`) — RE2s Regel ist "Ursprung im Fensterloch", keine feste Hoehe |
| EM2D-Bone-Wolke (alle 6 Clips, Bones 1..7) am ALTEN Anker y=-2000 | Bone-Welt-y **-2460..-1365**, also bis **695 unter** der gemessenen Bank -2060: der halbe Arm steckte im Mauerwerk (Bildrechteck Cut 4: v 93..125 gegen Bankzeile v=104) |
| Dieselbe Wolke am NEUEN Anker y=-2500 | Bone-Welt-y **-2960..-1865**, Bildrechteck u 68..120 / v 77..111, Hand 907 vor der Wandflaeche (`anker_1210_cut4_arme_y2500.png`) |

Daraus die beiden Portwerte in `enemy_ai_re2_zellenarm.c`:
* `RE2ARM_1210_HINTER = 400` — im gemessenen RE2-Band 298..523 hinter der Wandflaeche (§2.6).
* `RE2ARM_1210_Y = -2500` — im gemessenen RE2-Band -1930..-2700 UND ueber beiden gemessenen
  ROOM1210-Fensterbaenken (-2060 / -2310). Der Vorgaengerwert -2000 ist damit **gemessen widerlegt**
  und ersetzt.

### 6.3 Was die Messung zusaetzlich zeigt (offen, NICHT weggeraten)

Die z-Werte der zehn ROOM1210-Records treffen die Fensteroeffnungen der Raumkunst nur teilweise:
das suedliche Westfenster reicht z -16862..-19428, die Arme dort stehen bei z -15747 / -17130 (Arm 6
liegt an der rechten Fensterkante, Arm 5 rund 1100 noerdlich DANEBEN, vor geschlossener Wandkunst);
noerdlich dasselbe Bild (Fenster z ~ -9875..-11757 gegen Arme -8847 / -10247). Im RE1.5-Original
faellt das nicht auf, weil dort der Armkoerper IM Mauerwerk steht und nur die Hand ~181 Einheiten
aus der Wandflaeche ragt (Skeptiker-Befund 2). Die Record-z sind Daten und werden nicht verschoben
— wer sie verschiebt, erfindet Raumgeometrie. Naechster Weg, falls das Bild stoert: den PRI-/
Masken-Stand des Cuts pruefen (verdeckt die Vordergrundmaske den Arm neben dem Fenster?) und die
uebrigen sechs Arme derselben Messung unterziehen.

Weiterhin offen aus §5: Hook-Aufrufer-Kette jenseits 0x800CE3B4 (geklaert, s. 6.1), Opcodes
0x8A/0x8B in ROOM2050 sub03/04, die Frage nach einer Zeichner-Weiche auf +0x10E&0x8000, und der
Kamera-Cut-Wechsel (ROOM1210 hat kein `cut_chg`-Gegenstueck zu RE2s 9/10).

### 6.4 Sichtlauf im laufenden Spiel — warum er hier nicht zaehlt

Der Vorgaenger hat zwei FRAMEDUMP-Serien (t0/t1) aufgenommen; beide zeigen Leon **am Raumeingang
stehend** (HUD `R1210 C0 -25784 0 -3131`), kein Arm ist darin zu sehen — als Sichtpruefung der Arme
taugen sie nicht, die Rohbilder sind entfernt (je ein Beispielbild bleibt). Eigene Laeufe
(`RE15_DEBUG_JUMP=1210@gp` + Autopilot + FRAMEDUMP) sind in dieser Sitzung dreimal gescheitert: der
Prozess bleibt nach der Pad-Initialisierung stehen bzw. bricht mit `Assertion failure at
WIN_AddDisplay (SDL_windowsmodes.c:380)` ab — die RDP-Sitzung liefert gerade keine brauchbare
Anzeige. Deshalb ist die Anker-Messung ueber die Kamera-Rueckprojektion gefahren (6.2); sie ist
fuer die Frage "sitzt der Ursprung im Fensterloch" sogar genauer als ein Screenshot, weil sie
Weltkoordinaten statt Pixel-Augenmass liefert. Der Durchlauf-Sichtlauf bleibt nachzuholen, sobald
die Anzeige wieder da ist.

## 7. Nacharbeit (Phase 3, 2026-09-19, Thema re-restposten)

Die beiden §5-Punkte 1 und 2 hat schon Phase 2 geschlossen (§6.1). Dieser Durchgang hat sie
unabhaengig nachgeprueft und um zwei Belege ergaenzt. **Kein Code geaendert.**

### 7.1 §5.1 — der Scheduler-Widerspruch um +0x158

`FUN_8004A694` ist kein Per-Bild-Tick, sondern laeuft genau einmal je Raum-Eintritt. Eigener
`jal`-Zensus ueber die ganze `info/re2leon/PSX.EXE` (Wortscan auf das Sprungziel):

```
0x8004A694  <- genau EIN Aufrufer: @0x8004A35C  (in FUN_80049E48)
FUN_80049E48 <- genau EIN Aufrufer: @0x80026E1C  (in FUN_80026B7C)
FUN_80026B7C <- genau EIN Aufrufer: @0x80025A70  (in FUN_80025794, der Zustandsautomat;
                selbst hat er keinen jal-Aufrufer = Tabellen-Dispatch)
```

Was die Funktion tut, passt dazu: sie laeuft EINMAL ueber die Entity-Liste (s2 = 0x800CFE18 bis
DAT_800CE334), loescht je Entity `+0x10E & 0x8000` (`andi v0,v1,0x7fff / sh v0,270(s0)`
@0x8004A734-38), ruft den Typ-Root, und stellt bei `+0x158 != 0` die gespeicherte Startpose wieder
her (`+0x4 = ((+0x158 & 0xFF00) - 0x100) | 4` @0x8004A75C-74, `sb s1,332` @0x8004A778,
`sh a0,460` @0x8004A784). Das ist die Wiederherstellung nach dem Raumwechsel, kein Zeitgeber-Leser
im Spielbetrieb — **+0x158 ist nach dem INIT frei**, und die drei Lesungen des Zellenarm-Moduls
(rng&0xF im INIT, (rng&0x1F)+60 im REACH, (rng&0x1F)+30 im RUECKZUG) halten.

### 7.2 §5.2 — kostet der Arm-Griff HP?

Nein. Zwei unabhaengige Zensen auf dem ausgeschnittenen Overlay
`build/extracted/re2_ems/CDEMD0_EM2D_ai1.BIN` (5416 Bytes, laedt @0x80100000):

1. **`jal`-Zensus** ueber alle 19 Sprungziele — `0x800401D4` (der Spieler-Schadenseingang; er
   rechnet `HP -= a0` auf 0x800CFD4E, `lhu v0,342(a2) / subu a0,v0,a0 / sh a0,342(a2)`
   @0x80040248-5C) ist **nicht** darunter.
2. **Immediat-Zensus** auf das HP-Offset `0xFD4E`: **genau ein Treffer**, und der ist tot —
   ```
   80100c94  lui  v0,0x800d
   80100c98  lh   v0,-690(v0)        ; 0x800CFD4E = Spieler-HP
   80100ca0  slti v0,v0,101
   80100ca4  beq  v0,zero,0x80100d6c ;  HP >= 101 -> Ende
   80100cac  j    0x80100d6c         ;  sonst AUCH Ende  -> das Ergebnis wird verworfen
   ```
   Beide Zweige laufen auf dieselbe Marke; die Abfrage ist ein Rest, kein Schadenspfad.

Der Griff selbst steht direkt darueber: `addiu v0,zero,5 / sw v0,-1028(at)` @0x80100C4C-54 setzt
PL+0x4 = 5 (Spieler-Basis 0x800CFBF8), und der EXE-Verteiler der Routine 5 (@0x8004006C, Zeile 5
der Tabelle @0x800A4030) springt ueber `0x800CE300[kind]` (`lw v0,-6360(v1)` @0x800400B0 mit
v1 = 0x800CFBD8 + kind*4) — fuer kind 0x2D also nach 0x800CE3B4, und genau dorthin schreibt der
Arm-INIT seinen Hook (`sw v0,-7244(at)` @0x80100144 mit at = 0x800D0000). Der Hook 0x8010121C
dispatcht ueber PL+0x5 und PL+0x6 durch Clip- und Advance-Aufrufe; ein Schadensaufruf ist dort
nicht.

**Befund fuer den Merge:** Modus 5 (Zellenarm) zieht keine HP ab. Der `birkin-rest`-Agent klaert
die Tentakel-Griffmodi getrennt; diese Aussage gilt nur fuer den Arm-Hook 0x8010121C.
