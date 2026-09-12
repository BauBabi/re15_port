# BEFUND gator-vollausbau — "der Aligator hat im richtigen RE2 viel mehr Animation" + "Rumschleudern von Leon beim Finisher klappt nicht"

Nutzer 2026-09-12. Analyse 2026-09-12, Runde 6. NUR statische Analyse.
Quellen: info/re2leon/ (PSX.EXE t_addr 0x80010000, Datei-Off = 0x800+(addr-0x80010000));
EM23-Overlay + EMD aus CDEMD0.EMS ueber den Gegner-TOC der EXE; RE2_Quellcode_V2/;
ROOM40A0.RDT. Werkzeug: .claude/skills/re15-psx-disasm/scripts/re2_disasm.py
(Voll-Disasm 4666 Instr. des Overlays selbst gezogen). Vorbefunde: runde3
gator-biss-sound.md (Dispatch-Geruest, SE-Zensus), runde4 gator-se-korrektur.md
(Bank 17), analysis/re2_alligator_2026-09-09/DOSSIER.md (Beschaffung).

## 0. KURZFAZIT

Die RE2-Gator-KI (EM23_OVL_0000.BIN, 18664 B, gelinkt @0x80100000) nutzt **11 der 12
Clips** der EM23-Bank (alle ausser dem leeren Paar 2) plus **2 Opfer-Clips** (Paar 3,
Leon-Rig) plus **6 Leon-Clips aus der ROOM40A0-RDT-Raumanimation**. Der Port nutzt 4.
Der echte Fress-Finisher ist AUTHORED, nicht prozedural: Gator Clip 4 (Schnapp, Maul
zu bei f13) -> Clip 5 (120 F Schuetteln) -> Clip 11 (Kau-Loop); Leon wird bei f13 auf
das Anker-Paar 0x80015B94/0x80015CB8 gekoppelt und spielt das **Opfer-Paar-3**:
Clip 1 (120 F) = das RUMSCHLEUDERN — die Bahn (10,3k Einheiten vorm Anker, y bis
-9177 hoch, seitlich +-2648) steckt KOMPLETT in den Keyframes, es gibt KEINE
prozedurale Wirbelbahn und KEINEN Bone-Attach fuer Leon (Bone-Attach existiert nur
fuer den GASKANISTER). Drei Kill-Varianten + zwei ueberlebbare Biss-Reaktionen + vier
Todes-Praesentationen, alles unten mit @0x.

## 1. QUELLEN-IDENTIFIKATION (Frage "aus WELCHEM BIN?")

Gegner-TOC der EXE @0x8009ADF4 (Datei 0x8B5F4), Index (kind-0x10)*4+rec, 8 B je
Record {u32 Sektor, u32 Groesse}; Zugriffs-Semantik = Binder FUN_8001aaa8
@0x8001AB4C-50/-7C-80 (re15_port/engine/src/re2_ems.c:21-34). Fuer kind 0x23:

| rec | Inhalt | EMS-Offset | Groesse | identisch mit |
|---|---|---|---|---|
| 0 | KI-Overlay, gelinkt @0x8010D000 | Sektor 1512 | 18664 | EM23_OVL_D000.BIN |
| 1 | **KI-Overlay, gelinkt @0x80100000** | 0x2F9000 (Sektor 1522) | 18664 | **info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN — byte-identisch (selbst geprueft)** |
| 3 | EMD | 0x31F000 | 0x2E318 | re15_port/shared_assets/RE2/EM23.EMD, md5 9673f75fb713 — byte-identisch |

**Alle Runde-3/4-Adressen (@0x80100D64/84, @0x80102C10, @0x801020C4) stammen aus
EM23_OVL_0000.BIN** (= CDEMD0.EMS kind 0x23 rec 1); Datei-Offset = Adresse-0x80100000.

## 2. EM23-BANK — KOMPLETTKATALOG (selbst geparst, EMD md5 9673f75fb713)

dir @0x2E2F8, 8 Eintraege. Binder-Zuordnung dir->em-Feld (RE2_Quellcode_V2/
FUN_8001aaa8.c:41-48): dir[0]->+0x1BC, dir[1]=EDD1->+0x17C, dir[2]=EMR1->+0x108,
dir[3]=EDD2->+0x184, dir[4]=EMR2->+0x180, **dir[5]=EDD3->+0x18C, dir[6]=EMR3->+0x188**,
dir[7]=MD1->+0x14.

### 2.1 Paar 1 (Gator-Rig): EDD @0xC (12 Clips), EMR @0x127C (22 Bones, kf 112 B, 1153 kf)

Frame-Wort: Bits 0-11 = kf-Index; Bit 0x08000000 = SE spielen, SE-Id = Wort>>28
(FUN_80016028 @0x80016034-54); Top-5-Bits 0x28 -> +0x227=1 (Event @0x801003FC-0C),
0x48 -> +0x232=55 (Bruell-Cooldown @0x80100424-34). Keyframe: Bytes 0-5 = Pose-Offset
(y = Hoehe), Bytes 6-11 = kumulativer Lauf-Versatz (dx = vorwaerts, Modell schaut +X)
— den macht FUN_80015E7C (Delta -> Speed +0x144/146/148, Latch +0x20E) + FUN_800152C8
(Yaw-Rotation auf die Position) zur echten Bewegung; beides steckt im Clip-Treiber
FUN_8001A330 (§4).

| Clip | F | Netto dx/dz (perF) | rootY min..max (erst->letzt) | Frame-SEs/Events | Nutzung (Beleg §5) |
|---|---|---|---|---|---|
| 0 | 163 | +5990 / 0 (36,7/F) | -1404..-1055 (-1064->-1055) | f76 SE2, f162 SE2 (0x28-Event) | Schwimmen: ACTIVE sub1 (frac 32 @0x80100C38), sub7-P0 (@0x80101270) |
| 1 | 26 | +5798 / 0 (223/F) | -1369..-1139 | f12 SE2, f25 SE2 | Paddel-Schub: sub2 (frac 4 @0x80100CF8-CFC), Tot-Treiben (R2-P3 frac 7 @0x80102C40-44 — s. R2; HP-Tod P2 @0x80102EB8) |
| 2 | 150 | +3535 / -29 | -1465..-1026 (-1053->-1199) | f50 SE2, f110 SE2 | Lunge-ANLAUF: sub3-P1 (frac 16 @0x80100DAC-B4); HP-Tod-Zappeln: DEATH-P1 (Wort 0x30002 @0x80102E80-A0) |
| 3 | 150 | +2884 / **-4045** | -1614..-1053 | f50 SE4, f77 SE4 (0x48) | Lunge-BISS: sub3-P2 (@0x80100E20-28); Intro-Schnapper: sub0-P2 (@0x801008E4-EC); Maul-offen-Fenster f>=114 (@0x80100E74-A4) |
| 4 | 45 | +2491 / 0 (55/F) | -1800..-1040 | keine (stumm) | **FRESS-SCHNAPP: sub4-P1 (@0x801010EC-F4); Maul zu bei f13 (@0x801010FC-1104)** |
| 5 | 120 | +329 / 0 (2,7/F) | -2011..-1034 (-1800->-1134) | f2 SE1, f80 SE3 | **FRESS-SCHUETTELN: sub4-P2 (@0x80101180-88) — Gegenstueck zum Opfer-Clip 1 (beide 120 F)** |
| 6 | 125 | **+9498** / -23 (76/F) | -1333..+1489 (1489->-1169) | f6 SE4, f30/f34 SE2 | INTRO-Durchbruch: sub0-P1 (@0x801008AC-B4) |
| 7 | 165 | **-6134 / +4072** | -2244..-1053 | f4/f45/f115 SE4, f25/f36/f44 SE2 | TODESROLLE: HURT-R2-P2 (@0x801027E0-E8) |
| 8 | 97 | +1243 / 0 | -1502..-779 (->-805) | f8 SE4, f63 SE4, f90 SE2 | Sink-Tod: R2-P4 (@0x8010289C+@0x80102908), R3-P2 (@0x80102AC8-D0) |
| 9 | 37 | +1243 / 0 | -1934..-785 | keine | KANISTER-EXPLOSION-Aufbaeumen: R1-P1 (@0x8010248C-94) |
| 10 | 30 | -1025 / 0 | -2033..-1053 | keine | FLINCH: R4-P1 (frac 3 @0x80102C40-48) |
| 11 | 59 | 0 / -14 | -1165..-935 | keine | KAU-LOOP nach dem Verschlingen: sub4-P3 (FUN_8001A240 ohne Bewegung @0x801011D8-E0) |

### 2.2 Paar 2: LEER (dir[3]/dir[4] je 4 B) — wie bei anderen Bossen.

### 2.3 Paar 3 (OPFER, Leon-Rig): EDD @0x20BF0 (2 Clips), EMR @0x20E54

**EMR3-Header: bto=0x0, kfo=0x8, bones=15, kf 80 B, 150 kf — KEINE relpos-Tabelle,
KEIN Armature: nur Keyframes.** Die Pose laeuft auf dem SKELETT DES SPIELERS
(PL00.PLD EMR: 15 Bones, kf 80 B — identisches Layout, selbst geparst @PL00.PLD
dir[1]=0x59C). Das bestaetigt: Paar 3 = Opfer-Animation fuers Spieler-Rig.

| Clip | F | kf | Verlauf (Offset y / Versatz dx,dz) | Bedeutung |
|---|---|---|---|---|
| 0 | 30 | 0-29 | y -1779 -> +85 (hoch!), dx 2->2114 | **GEPACKT/HOCHGEHOBEN** (Proximity-Kill-Variante, ab Frame 3 gestartet @0x80102FBC) |
| 1 | 120 | 30-149 | dx 7541..10527 (≈10,3k VOR dem Anker!), dz -2648..+1254, y -1642 -> Spitze **-9177** (f60-f75) -> -2403 | **DAS RUMSCHLEUDERN**: Leon haengt ~10,3k Einheiten in Blickrichtung vorm gemeinsamen Anker (= im Maul), wird bis 9,2k hochgerissen und seitlich geschleudert — synchron zu Gator-Clip 5 (beide 120 F) |

Platzierung: x/z = Anker + RotY(Gator-Yaw)*(dx,dz) ueber 0x80015B94/0x80015CB8
(Mechanik vollstaendig zitiert in enemy_ai_re2_zombie.c:2255-2270 +
enemy_ai_re2_dog.c:355-370); y = Pose-Kanal (Bytes 2-3 des kf).

### 2.4 ROOM40A0-RDT-Raumanimation (Sektion 22 @0x65E8) — die "Chomp"-Opferclips

Binder FUN_80018e08 (RE2_Quellcode_V2, Maske bit0=Spieler -> pl+0x190/+0x194 =
0x800CFD88/8C): rec0 mask=1: EMR @RDT+0x65F4 (15 Bones, kf 80 = Leon-Rig), EDD
@RDT+0x8F8C, **21 Clips**: 0-14 je 1 F (Halteposen), 15 (20 F), 16 (24 F), 17 (16 F),
18 (10 F), 19 (30 F), 20 (21 F). Clips 17-20 + Pose 6 sind die ueberlebbaren
Biss-Reaktionen (§6.3). (rec1 mask=2 = NPC em[0], ebenfalls Menschen-Rig 15/80.)

## 3. STRUKTUR-LEGENDE (alles selbst aus dem Disasm belegt)

Spieler-Basis 0x800CFBF8 (Beleg-Kette enemy_ai_re2_dog.c:358): +0x04 Routine-Wort
(0x800CFBFC, ghidra DAT_800cfbfc), +0x0E Lauf-Versatz-Latch (0x800CFC06, Schreiber
FUN_80015e7c.c:34-42), +0x38/+0x40 x/z (0x800CFC30/38), +0x76 Yaw (0x800CFC6E),
+0x156 HP (0x800CFD4E), +0x158 Opfer-Sync-Latch (0x800CFD50), +0x188/+0x18C
Opfer-Anim-Paar EMR/EDD (0x800CFD80/84), +0x190/+0x194 Raum-Anim-Paar (0x800CFD88/8C),
+0x1B4 Greifer-Zeiger (0x800CFDAC), +0x1D3 Status (0x800CFDCB, Bit 0x80 = gepackt).
Gator (em): +0x4/5/6 routine0/1/2; +0x14C/14D/14E Clip/Frame/Crossfade; +0x144/146/148
Speed; +0x156 HP; +0x158 Easing-Zaehler; +0x16B Kanister-Abraeum-Zaehler; +0x170/172/174
Zielpunkt; +0x178 Frame-Wort-Zeiger; +0x18C/+0x188 Opfer-Paar; +0x198 Modellwerk;
+0x1B4 = Spieler-Zeiger (@0x80100218-21C); +0x1C0 Bit4 Ziel-Modus; +0x1D2 Hurt-Reaktion;
+0x1D3 Bit7 unverwundbar + Low-7-Timer (@0x801001C8-DC); +0x1F0 Distanz (EXE-gepflegt,
im Overlay nur gelesen); +0x1FC gerettetes Routine-Wort; +0x218 Flags (0x2 Fang aus,
0x10 MAUL OFFEN, 0x20 HP<25000, 0x40 Nah-Cam); +0x21A Zone 0/1/2; +0x21C/+0x21E
Lunge-Ueberschuss; +0x224 T1-Stufe; +0x225 Lunge-Latch; +0x226 T2-Kanister-Stufe
(0..7, 13=explodiert); +0x227 Event; +0x228/+0x22A Nacken-Winkel-Offsets (Parts 7/6
.vz); +0x22C Fang-Cooldown; +0x22D R1-Latch; +0x22E Bruell-/sub7-Timer; +0x230
Vor-Hurt-Sub; +0x232 55er-Bruell-CD; +0x238 Biss-CD; +0x234 Per-Frame-Callback
(jalr @0x80100478!).
Globals: 0x800CFBD8/DC Spielzustand-Bits, 0x800CFBF2 Kamera-Cut (FUN_8002c7ac),
0x800CE330 aktuell getickte Entity, 0x800CE334 Listen-Ende, 0x800CE38C = vom Gator
registrierter Handler-Slot (§6.1), 0x800D0324+idx*504 = obj_model-Array (Beleg
analysis/nutzer_batch_2026-08-27/re2-fenstergreifer-skript.md:590) -> **obj[1]
@0x800D051C = der GASKANISTER**, 0x800D4874 Raum-Flagwort (Bit-Test FUN_80077360),
0x800D4820 Waffen-/Quellen-Id (s16, ==8-Gate; OFFEN welche Waffe).

## 4. EXE-HELFER (Clip-Treiber — selbst disassembliert)

**FUN_8001A330(em, clip, frac, hi) @0x8001A330-A3C8** = DER Clip-Treiber:
Clip-Wechsel nur wenn +0x14C!=clip (Frame=0, +0x14E=frac @0x8001A34C-64); dann
FUN_80015E7C (Root-Versatz->Speed, @0x8001A368-78), Advance 0x8002959C mit
a3=(4096/(frac+1))|(hi<<16) (@0x8001A37C-AC), und wenn nicht fertig
FUN_800152C8(em,0) = Bewegung (@0x8001A3B8-C4). Rueckgabe 1 am Clip-Ende.
**FUN_8001A240** = dasselbe OHNE Bewegung/15E7C (Kau-Loop). **FUN_8001A308(em,prev)**
= Clip-Reset (+0x14C=255) bei Routinenwechsel (@0x8001A31C-24). FUN_8001A1E0(ptr,ziel,
schritt) = Wert-Annaeherung. 0x80101BAC/0x80101C74 = quadratische Ease-in/out-Deltas
(Overlay-lokal). FUN_8002C7AC(n) = Kamera-Cut (DAT_800cfbf2). FUN_80077360/0x8007730C
= Flag-Test/-Set. FUN_800401D4(dmg,0) = Spieler-Schaden.

## 5. DIE KOMPLETTE STATE-MACHINE (Tabelle State->Clip->Zweck)

Wurzel @0x80100190: Freeze-Gate 0x800CFBD8&0x20000000 (@0x801001B4-C0); Dispatch
+0x4 ueber @0x8010461C: [0] INIT 0x801004E0, [1] ACTIVE 0x80100764, [2] HURT
0x80101FF0, [3] DEATH 0x80102C84, [7] 0x80102D00 (leer). Nach dem Dispatch:
Entity-Push-Kollision (Zeigerliste ab 0x800CFE14, 0x80034D0C @0x80100304),
T2-Kanister-Maschine 0x801017C4 (@0x80100320), T1-Anlauf 0x80101388 (@0x80100328),
Wellen-Emitter (Nacken-Offsets +0x22A/+0x228 auf die Winkel-SVECTORs von Modell-Part 6
@mw+1136 / Part 7 @mw+1308, RotMatrix 0x8008E1F4 @0x8010038C/@0x801003B8),
**Fang-Check 0x801016D8 wenn !(+0x218&2)** (@0x801003D4-E8), Frame-Flag-Verarbeitung
(§2.1) + Frame-SE 0x80016028 (@0x80100438), Callback +0x234 (@0x80100468-7C), Timer.

INIT @0x801004E0: **0x800CE38C = 0x80102D08** (@0x801004F4-500); Routine=1; **HP =
25000 + Tabelle[Flags 0x800CFB74 Bits 0x40/0x20]** -> 25300/25300/25240/25240
(@0x80100518-580, Tabelle @0x801045BC/BE/EC/EE = 300/300/240/240); Clip-Wort=1
(@0x8010060C); 22 Part-Flags |=0x9000 (Schleife @0x801005C8-E0, Stride 0xAC);
**Opfer-Paar +0x188/+0x18C -> Spieler +0x188/+0x18C** (@0x801005E4-600); Hitbox-Batterie
(+0xB0..+0xFE, Werte 450/1450/2000/2800/-2200/1900/950/3000/600/1100 @0x80100628-738).

ACTIVE @0x80100764: Pre-Pass +0x5 ueber @0x8010463C, Haupt-Pass ueber @0x8010465C.
Routine-Woerter little-endian: 0x101=sub1, 0x201=sub2, 0x301=sub3, 0x401=sub4,
0x601=sub6, 0x701=sub7.

| Sub | Pre / Main | Clip (frac) | Bewegung/Zweck | Uebergaenge |
|---|---|---|---|---|
| 0 INTRO | — / 0x80100804 | P1: 6 (0), P2: 3 (0) | P0 wartet +0x1D4!=0 (extern/SCD, @0x80100854-68); P1: Freeze 0x110, Cut 0 bei Frame 70 (@0x80100894-A4), Clip 6 fertig -> Cut 1, z=-21212 (@0x801008C4-D8); P2: Clip 3 fertig -> Freeze frei, +0x238=110, **Yaw += 0x400** (@0x80100938-48), +0x218|=4 | -> 0x101 (@0x8010092C) |
| 1 SCHWIMMEN | 0x80100964 / 0x80100C20 | 0 (32) | Pre = Zonen-/Trigger-Logik: x<-17877 -> Zone1+LUNGE (+0x21C=-17877-x, @0x80100990-C4), z<-7322 -> Zone2+LUNGE (+0x21E=z+7323, @0x801009D8-A08); Kanister-Logik (§5b); Distanz-Hysterese +0x1F0>=0x65 -> sub2 (@0x80100B6C-BB4) | 0x301 / 0x201 |
| 2 PADDELN | 0x80100964 / 0x80100CDC | 1 (4) | Clip-Ende -> Selbst-Reset 0x201 (@0x80100D08-10); +0x1F0<0x5A -> sub1 (@0x80100BDC-F0); Kanister-z-Naehe <9500 -> sub1 (@0x80100C78-98) | 0x101/0x201 |
| 3 LUNGE | 0x80100D28(leer) / 0x80100D30 | P0: SE4 (@0x80100D84); P1: 2 (16); P2: 3 (0) | Doppel-Advance wenn +0x22C!=0 (@0x80100DAC-D8 bzw. E20-48); **Maul-offen +0x218\|=0x10 wenn Zone2 & Clip3 & f>=114** (@0x80100E64-A4); +0x225=1 waehrend P1 (@0x80100E08-0C); Ende: Yaw+=0x400 (@0x80100EB0-C0); Gleit-Korrektur +6/F auf x bzw. -6/F auf z (@0x80100EE8-F34) | Zone2 -> **0x701** +0x22E=255 (@0x80100ED0-E0), sonst 0x101 |
| 4 FRESSEN | 0x80100F50(leer) / 0x80100F58 | P1: 4 (0); P2: 5 (0); P3: 11 (Loop) | **DER FINISHER — Details §6.1** | terminal (P3 Loop) |
| 5 | 0x80101204/0x8010120C (leer) | — | unbenutzt | — |
| 6 PARKEN | 0x8010121C (leer) | — | tot/Leiche (nach R3) bzw. Spieler schon tot | — |
| 7 MAUL-OFFEN-JAGD | 0x80101224(leer) / 0x8010122C | 0 (32) | P0: +0x218\|=0x10 JEDEN Frame (Delay-Slot @0x80101294!), +0x22E=255 (@0x80101278-7C); Frame>=0x43 -> P1 (@0x8010128C-A8) | P1: Spieler-HP<0 -> **0x601**, sonst -> **0x401 FRESSEN** (@0x801012AC-E4) |

**5b. T2-KANISTER-MASCHINE** (+0x226 ueber Tabelle @0x80100024, Treiber 0x801017C4,
GTE-Register-Sicherung @0x801017E8-804): Raum-Flag 0x800D4874 Bit1 -> Ziel-Modus
+0x1C0|=4, Ziel=(eigene x, y-800, **Kanister-z [0x800D055C]**) (@0x80100A34-70);
Bit0 + Fenster (Kanister-z - eigene z - 8601 in [0..0x18F)) -> **+0x226=1**
(@0x80100AB8-B08). Stufen: [1] Raum-Flag **Bit 29 SETZEN** (0x8007730C @0x80101848),
[2] Ease (13 Schritte, Amplituden +240/-320 auf +0x22A/+0x228), Spieler-Abstand
z>=8100 erzwungen (@0x801018A4-C0), bei t=13: **Kanister obj[1]+0x80 =
&Modellwerk-Part-6-Weltmatrix (mw+0x450) @0x801018D0/E0** — DER Bone-Attach (Maul),
Position/Matrix uebernommen (0x8002D030+0x8007746C @0x801018F0/0x8010195C); [3]-[6]
Ease-Stufen (6/6/7/7 Schritte, -140/-60 bzw. +75/+100), Kanister-Lokalpos ->
(1750,-210) (0x8001A1E0 @0x801019C8/@0x80101A30-54/@0x80101ABC-DC); [7] Haltezustand
"traegt Kanister". +0x226==7 gilt ueberall als "traegt".

**5c. T1-CHOMP (Steg-Biss)** — Anlauf 0x801012FC: armt +0x224=1 wenn Spieler-Latch
+0x0E!=0 (=Bewegungs-Anim mit Root-Versatz laeuft) & 0x800CFDCB==0 & +0x224==0 &
!(+0x1C0&4) & +0x226 in {0,7} (@0x801012FC-137C). Stufen (Tabelle @0x80100004):
Ease-Zyklen auf +0x22A/+0x228 (Kopf hebt sich: 4x(+20,-100) ein/aus, 3x(-20,+100)
zurueck, 0x801013C4-0x80101554), Stufen 3-5 rufen je Frame **den COMMIT 0x80101578**:
Spieler-Routine = **0x205** (@0x801015B4-C4), bei gleicher Blickrichtung
(FUN_80015910) **+1 -> 0x305** (@0x801015C8-EC), Spieler-Yaw-Snap auf 0/0x400/0x800/
0xC00 je Lage (FUN_8001569C(pl, yaw, 1536) @0x801015F0-8C), **Schaden 60 (bzw. 30
wenn +0x218&0x20) via 0x801038E4 -> FUN_800401D4** (@0x8010169C-A4; Kill-Bit ->
Spieler-Routine 3, Knockdown-Bit -> Spieler+0x06=4 @0x80103914-48), Spritzer-Callback
0x80103D80 (4 Ticks ueber +0x234, Effekte 0x8001BF10 an der Spielerposition
@0x80103DD4-E74), +0x238=70. **Der Gator wechselt dabei KEINEN State** — die
Chomp-Optik ist das Nacken-Easing + Spritzer.

**5d. PROXIMITY-FANG 0x801016D8** (je Frame, wenn !(+0x218&2)): Abstand
sqrt((Spieler-x - Part0-Welt-x)^2 + (Spieler-z - Part0-Welt-z)^2) (mw+92/+100,
0x8008D2F4 @0x80101704-40) **< 5200** -> +0x22C=8, +0x218|=2, Spieler: +0x1B4=Gator,
**Routine-Wort = 0x105**, word0|=0x100A/0x1008, **HP = -1 (Instant-Kill)**,
+0x1D3|=0x80 (@0x80101744-A8).

**5e. HURT** @0x80101FF0 (Eintritt: +0x230=alter Sub, +0x5=255, Frame -2
@0x80102030-48). Selektor 0x80101EAC: HP<25000 -> +0x218|=0x20 (@0x80101EBC-DC);
Reaktion = 1 wenn +0x226==7 & +0x230!=1 (@0x80101EF4-F18); 2/3 wenn Maul offen
(+0x218&0x10) & [0x800D4820]==8 & HP<25000 (3 bei 0x800CFBD8&0x40000000, sonst 2,
@0x80101F1C-F88); 4 wenn +0x225==0 & +0x22E==0 & rng&3==0 (+0x22E=(rng&0x3F)+120,
@0x80101FA0-D4); sonst 0. Dispatch ueber Tabelle @0x8010467C.

| R | Handler | Clips | Inhalt |
|---|---|---|---|
| 0 | 0x80102090 | — | SE5 (Gate +0x232/+0x233==0, @0x801020AC-C8), Hit-VFX 0x801039EC (Tabelle @0x80100144 nach +0x230), **Routine-Restore aus +0x1FC** (@0x801020D4-DC) + Inline-Weiterlauf des ACTIVE-Subs (@0x801020E0-2138) |
| 1 | 0x80102158 | 9 | **KANISTER-EXPLOSION = TOD**: SE4 (@0x801021C4-EC), Nah-Cam 5 wenn [0x800D4820] in {6,13} & z<-13300 (@0x80102204-6C), +0x226=13 (@0x801022B8-BC), Kanister abloesen (+0x80=0x8009DB44 @0x80102300-14) und 9 F nach Clip-Start entfernen (word0=0 @0x8010255C-78, Zaehler +0x16B=9 @0x8010235C-94), Explosion 0x8003947C(8,3)+Blitz 0x800395B8(50,255) (@0x80102328-44), Splash-Ring um den Kanister 0x8010338C (SVECTOR-Tabelle @0x801000A8), Fontaenen-Callback 0x80103EB4 (64 Ticks, Effekte an mw+1104 = Maul, @0x80102368/@0x80103F58), Blutlache 0x8005BA28(0x20F0001) (@0x8010237C-88), Rueckstoss +0x144=-500 +40/F abklingend rueckwaerts (0x800152C8(em,0x800) @0x801025A4-A8), Achsen-Recentering (x->-25800 bzw. z->600 @0x801025B0-F4), Clip 9, bei f31 Event (@0x8010252C-58); P3: **+0x10E\|=0x4000 + Raum-Flag 0x800D4874 Bit 30 SETZEN** (@0x80102654-64) — Endzustand |
| 2 | 0x801026B8 (Phasen @0x8010005C) | 7 -> 1 (x5) -> 8 | **MAUL-SCHUSS-TOD A**: Freeze (@0x801026F4-724), Cut 9 (@0x80102784-88), Snap x=-20125/z=-904/Yaw 3072 wenn mitten im Lunge (+0x225, @0x80102798-C8), **Todesrolle Clip 7**, Blutlache 0x20E0001 (@0x801027F4-810), 5x Clip 1 treiben (+0x16B-Zaehler @0x80102830-6C), dann Cut 8 + **+0x10E\|=0x4000** + Clip 8 (@0x80102874-908) |
| 3 | 0x80102924 | 8 | **MAUL-SCHUSS-TOD B** (0x40000000-Szenario): Ziel (-9248,-1800,970) (@0x801029B0-C4), Cut 14/7 (@0x80102A54-7C), z->0 (@0x80102AB0-B8), Clip 8, dann +0x10E\|=0x4000, Cut 8, **-> 0x601 geparkt** + Leichen-Hitbox (7150/5500/4800/2000/-2000/7000/-150 @0x80102B60-B8) |
| 4 | 0x80102BD0 | 10 (3) | Grossreaktion: SE0 (Gate +0x232==0 @0x80102C00-10), +0x1D3\|=0x80, Clip 10, fertig -> 0x101 + Bit7 frei (@0x80102C50-64) |

**5f. DEATH (Routine 3, HP<=0)** @0x80102C84: +0x1D2 wird geloescht (0x80102C7C
@0x80102CB4) -> immer Variante [0] der Tabelle @0x80104690 = 0x80102D44: P0 Wasser-/
Blut-Salve (9 Effekt-Aufrufe 0x800395B8/0x8003947C mit Zeit-Parametern 11/21/86/106/
108/128/148 @0x80102DA8-E50); P1 **Clip 2** (Wort 0x30002 @0x80102E80-A0); P2
**0x80015CB8 je Frame** (haelt ein gepacktes Opfer synchron! @0x80102EC8-D8) +
Blutlache 0x4020001 bei f3 (@0x80102EDC-F8) + Advance a3=4096 -> P3; P3 setzt
Spieler+0x1D3=0x80 je Frame (@0x80102F24-28). Treiben mit Clip 1 (@0x80102EB8).

**5g. Frame-Events** (+0x227 via Tabelle @0x80100044): 1: Splash 0x800395B8(10,120,50)
(@0x80101D80-8C); 3: 0x800CFBD8|=4; 4: nach 5 F Splash(8,85,40) + Bits 4->8; 5: nach
10 F Bit 8 loeschen (@0x80101DCC-E94).

## 6. DER FRESS-/TODES-PFAD IM DETAIL (Frage 3)

### 6.1 Variante 0 — das ZONEN-FRESSEN (ACTIVE sub4, Einstieg aus sub7-P1 @0x801012E0)

P0 @0x80100FBC-10E4 (einmalig): 0x800CFBD8|=0x100; **Kamera-Cut 7 bzw. 14**
(Flag16-abhaengig @0x80100FD4-F8); 0x800DFC1B=1; **Kanister obj[1] word0=0**
(@0x80101008-0C); Spieler-Routine0=5/Routine1=0 (@0x80101024-38), **Spieler-HP=-1**
(@0x80101030-40); eigener x-Snap -23078 (@0x80101048-4C); **Spieler-Teleport
x = eigene x + 10643, z = -915** (@0x8010106C-84 — Korridor laeuft entlang X;
generisch: Gator-Pos + RotY(Yaw)*(10643,0)); Spieler+0x1B4=Gator (@0x80101068);
**Opfer-Paar erneut in Spieler +0x188/+0x18C kopiert** (@0x80101088-9C);
word0|=0x100A, Spieler-word0|=0x1008, **Spieler+0x1D3|=0x80** (@0x801010A0-D4).

P1 @0x801010E8: **Gator-Clip 4** (0x8001A330(em,4,0,0) @0x801010EC-F4). **Bei
Frame-Byte==13** (@0x801010FC-1104): 0x800CFB74|=0x4000100 (@0x80101108-34),
**0x80015B94(SPIELER, EMR1, EDD1, 0)** = gemeinsamer Anker aus dem AKTUELLEN
Gator-Frame, Kopie in den Spieler (@0x80101110-34; Mechanik: enemy_ai_re2_zombie.c:
2261-2268), **Spieler+0x158 = 1** (@0x80101148-50 — das Sync-Signal an die
Opfer-Maschine!), **Spieler-Yaw = Gator-Yaw** (@0x80101154-5C), Phase 2.

P2 @0x80101168: je Frame **0x80015CB8(GATOR,...)** (Platzierung Anker+Root-Offset,
@0x80101168-78) + **Gator-Clip 5** (120 F Schuetteln, @0x80101180-88); fertig ->
0x800CFBF8|=0x80000 (@0x80101198-AC), Phase 3.
P3 @0x801011C4: 0x80015CB8 weiter + **Clip 11 als Loop OHNE Bewegung**
(FUN_8001A240 @0x801011D8-E0). Terminal.

**Leon-Seite (Handler [0] = 0x80102D44 mit a0=Spieler; Dispatch nach Spieler-Routine1
ueber 0x80102D08 / Tabelle @0x80104690 — derselbe Code dient als Gator-DEATH-[0],
§5f):** P0 Wasser-/Blut-Salve; P1: Leon spielt SEIN PLD-Paar-1-**Clip 2** (113 F
Strampeln; PL00.PLD Paar 1 = [30,34,113,22,22,20,25,10,25,45] Frames, selbst geparst;
Advance explizit ueber s0+0x108/+0x17C @0x80102E88-90) — **bis Spieler+0x158!=0**
(das Signal aus Gator-f13!): dann **Clip-Wort=1 -> OPFER-PAAR-3 CLIP 1 = DAS
RUMSCHLEUDERN** (@0x80102EA4-C0; der P2-Advance laeuft ueber die vom Aufrufer
gereichten a1/a2 = Spieler+0x188/+0x18C = EM23-EMR3/EDD3); P2: je Frame
0x80015CB8(SPIELER) = Platzierung Anker + Opfer-Root-Offset (dx≈10,3k gedreht um den
Yaw; y aus dem Pose-Kanal — die Schleuderbahn), Blutlache bei f3, Advance a3=4096;
P3 gepackt parken. — **Kein Bone-Attach fuer Leon; die "Befestigung" ist das
gemeinsame Anker-Paar + die authored Keyframes.**
(OFFEN: der EXE-Aufrufer, der 0x800CE38C laedt, ist statisch nicht auffindbar —
kein lw/addiu/gp-Zugriff mit imm 0xE38C in EXE, COMMON/BIN-Overlays oder allen
CDEMD0-Overlays ausser dem sw des Gators @0x80100500; die Zuordnung
Handler<->Spieler ist ueber die Feldnutzung bewiesen: +0x158-Latch @0x80102EA4,
+0x1B4-Greifer @0x80102F70, Raum-Paar +0x190/+0x194 @0x8010312C-30, Routine-Restore
auf 1 @0x801031C8-D0. Der generische EXE-Zwilling der Opfer-Maschine liegt
@0x8003F8E4, 12 Phasen, Tabelle @0x80010F98, nutzt explizit Spieler+0x188/+0x18C
@0x8003F9A4-A8 — fuer Gegner OHNE eigenen Handler.)

### 6.2 Variante 1 — PROXIMITY-KILL (Fang 0x801016D8, Routine 0x105)

Leon-Handler [1] @0x80102F48: P0: Blutlache 0x4030001 (@0x80102F8C-98) + Blitz
(53,240,40) (@0x80102F9C-A8), **Clip-Wort 0x300 = OPFER-PAAR-3 CLIP 0 AB FRAME 3**
(das Hochheben, @0x80102FB0-BC), 0x800CFB74|=0x4000000 (@0x80102FC0-CC); P1: 3x
Spritzer 0x801034E8 (@0x80102FE0-3008), **Drehung zum Greifer** (+0x1B4! atan2
0x800154AC + FUN_8001569C(pl,winkel,128) @0x8010300C-34), Advance (a1/a2 =
Opfer-Paar) bis Clip-Ende (@0x80103038-58); P2: haengt im Maul (parken). Der Gator
selbst wechselt KEINEN State (+0x22C=8, +0x218|=2) — er schwimmt mit Leon im Maul
weiter; Game-Over kommt ueber HP=-1.

### 6.3 Varianten 2/3 — UEBERLEBBARER CHOMP (T1-Commit, Routine 0x205/0x305)

Leon-Handler [2] @0x8010307C (Phasen-Tabelle @0x80100074) / [3] @0x801031F8
(Phasen-Tabelle @0x8010008C) — Clips aus dem **ROOM40A0-Raum-Paar** (Spieler
+0x190/+0x194, §2.4):
[2] Rueckwaerts-Schleudern: Blutlache 0x4010001 (@0x801030BC-C8), **Clip 18**
(Wort 0x30012 @0x801030CC-D4), Speed -800, +150/F abklingend (0x800152C8(pl,0)
@0x80103120-24, @0x8010313C-4C), dann **Clip 19** (0x30013 @0x80103150-58) mit
Doppel-Advance (@0x80103164-9C); Ende: **Routine-Wort=1** (zurueck ins Spiel),
+0x1D3=0, +0x1C0&=~0x12 (@0x801031C8-DC).
[3] Vorwaerts-Wurf (von hinten gepackt): **Clip 20** (0x30014 @0x80103248-50),
Speed **+800** (@0x8010325C-60), dann **Clip 17** (0x30011 @0x801032DC-E4,
Doppel-Advance), **Liege-Pose 6** (0x30006 @0x8010332C-3C), Aufstehen ueber
PLD-Paar-1 (a1/a2=+0x108/+0x17C, a3=0x10400 @0x80103340-54), Ende Routine 1
(@0x80103358-70).

### 6.4 Todes-Synchronisation

Stirbt der Gator WAEHREND er Leon haelt, laeuft DEATH-P2 mit 0x80015CB8 je Frame
(@0x80102EC8-D8) — Leiche + Opfer bleiben gekoppelt. Kanister-Tod (R1) und
Maul-Tode (R2/R3) setzen +0x10E|=0x4000; R1 zusaetzlich Raum-Flag Bit 30.

## 7. UMSETZUNGS-SPEZIFIKATION fuer enemy_ai_boss_gator.c

Der Port-Boss lebt im RE1.5-ROOM2090-Pool (anderes Layout als der RE2-Korridor) —
die ZONEN-Trigger (x-17877/z-7322) sind raumgebunden und werden NICHT uebernommen;
alles Verhaltens-/Anim-/Praesentationsmaterial unten schon.

### 7a. Fehlende Clips/States nachruesten (alle Werte §2.1/§5)

1. **LURK/CHASE**: Distanz-Hysterese sub1<->sub2: fern Clip 0 (frac 32), nah Clip 1
   (frac 4) — Schwellen 0x65/0x5A auf die +0x1F0-Einheiten des Ports mappen
   (@0x80100B6C-BB4/@0x80100BDC-F0). Heute laeuft IMMER Clip 0.
2. **LUNGE = Clip 2 (frac 16) + Clip 3 (frac 0)** statt nur Clip 4; Maul-offen-
   Hit-Fenster Clip 3 f>=114 (@0x80100E74-A4); nach Clip 3 **Yaw+=0x400**
   (@0x80100EB4-C0) — die Clips drehen den Koerper um 90 Grad ein, der Ausgleich
   gehoert dazu (auch Intro @0x80100938-48). SE 4 am LUNGE-START (@0x80100D64-84),
   nicht am Schnapp (runde-4 bestaetigt).
3. **GUARD-Hochbiss -> T1-Chomp-Modell**: Kopf-Heben als Ease auf die Nacken-Bones
   (Ease-Deltas 0x80101BAC/0x80101C74; Zyklen 4x(+20,-100), 4x zurueck ease-out,
   3x(-20,+100), 3x zurueck — @0x801013C4-0x80101554; der Port hat pitch_vz/arc_vz
   bereits — Werte ersetzen), Schaden 60/30 (@0x8010169C-A4), Spieler-Yaw-Snap
   (1536/F @0x80101684-88) + Knockback-Reaktion (7b.6).
4. **DIE — vier Praesentationen**: HP-Tod Clip 2->1 (@0x80102E80-EB8), Maul-Tod A
   Clip 7 -> 5x Clip 1 -> Clip 8 (@0x801027E0-2908), Maul-Tod B Clip 8
   (@0x80102AC8-D0) + Leichen-Hitbox (@0x80102B60-B8). Heute nur Clip 7. Blutlachen
   0x20E0001/0x20F0001/0x4020001 an den zitierten Phasen.
5. **FLINCH**: Clip 10 mit frac 3 (@0x80102C40-48); Reaktions-Gates: Grossreaktion
   nur rng&3==0 + Cooldown 120+(rng&0x3F) (@0x80101FA0-D4), Standard-Hit = NUR SE +
   Routine-Restore aus +0x1FC OHNE Anim-Abbruch (@0x801020D4-DC). Der 10%-Schwellen-
   Flinch des Ports ist grosszuegiger als RE2 (als Abweichung dokumentieren).
6. **Frame-Flag-Vollausbau**: 0x48-Woerter armen den 55er-Cooldown (@0x80100424-34),
   0x28-Woerter starten die Splash-Kette (§5g) — Port spielt bisher nur SEs.
7. **Root-Motion**: die Clips TRAGEN ihre Bewegung (Netto-Spalten §2.1; Treiber
   FUN_8001A330 §4). Port-Advance sollte fuer 0/1/2/3/4/5/6/7 die kf-Versatz-Deltas
   fahren (re15_emd_get_keyframe_speed existiert) statt GB_SWIM_SPEED/GB_LUNGE_SPEED.

### 7b. DER ECHTE FINISHER (ersetzt die prozedurale Wirbelbahn in GBP_FRESSEN)

Ablauf (alle Belege §6.1):
1. Eintritt (heutiger FRESSEN-Trigger): Spieler hp=-1 (@0x80101030-40), **Teleport
   auf Gator-Pos + RotY(yaw)*(10643, 0), Spieler-Yaw = Gator-Yaw** (@0x8010106C-84/
   @0x80101154-5C). Leon spielt STRAMPELN (RE2: PLD-Paar-1 Clip 2, 113 F;
   Port-MAPPING: naechstliegende RE1.5-Struggle-Anim, als Mapping kommentieren).
   Kamera: RE2 Cut 7/14 — Port-Kamera-Business beibehalten.
2. **Gator Clip 4** (45 F). Bei **anim_frame==13** (@0x801010FC-1104):
   re15_re2z_grab_anchor(e, pl, ...) am AKTUELLEN Frame (@0x80101110-34) +
   Opfer-Start-Signal (Analogon zu Spieler+0x158=1 @0x80101148-50).
3. **Gator Clip 5 (120 F) + Leon = EM23-PAAR-3 CLIP 1 (120 F)** ueber das vorhandene
   Victim-System (re15_victim_bank_resolve/donor: EM23 als Donor-Paar-3
   registrieren); je Frame beide ueber grab_rootmotion platzieren (@0x80101168-78 /
   §6.1-Leon-P2); **Leons y MUSS aus dem Pose-Kanal kommen** (kf-Bytes 2-3;
   Schleuderspitze -9177 bei f60-75) — Achtung: emd_common.c nullt kf-Translation
   fuer normale Clips (Kommentar emd_common.c:245-249), fuer die Opferbank muss der
   y-Kanal AN sein. SEs kommen als Frame-Flags von Gator-Clip 5 (f2 SE1, f80 SE3) —
   Clip 4 ist in RE2 datenseitig STUMM (SE-3-Mapping aus runde-4 bleibt separat).
4. **Gator Clip 11 als Standloop** (FUN_8001A240-Aequivalent: Advance ohne
   Bewegung @0x801011D8-E0); Leon ab hier im letzten Opfer-Frame geparkt/verdeckt —
   der heutige fress_skip_mask-Trick (Meshes 1-7 wirbeln) kann raus, weil die
   Opfer-Anim den Koerper selbst ins Maul zieht.
5. Optional (RE2-Variante 1): Mid-Wasser-Fang bei Distanz<5200 vom Koerper-Part
   (@0x80101744-50) mit Opfer-Clip 0 ab Frame 3 (@0x80102FBC) + Drehung zum Gator
   (Schritt 128 @0x8010300C-34) — ergaenzt den heutigen gb_biss_abschluss-Kill.
6. Ueberlebbarer Chomp (Var. 2/3): Knockback-Clips 18/19 bzw. 20/17/6 liegen im
   ROOM40A0-RDT (EMR @RDT+0x65F4, EDD @RDT+0x8F8C, §2.4) — als Port-Asset
   extrahieren ODER auf RE1.5-Knockdown mappen (kennzeichnen; Speed -800/+800 mit
   150/F Abbau @0x801030E0-4C/@0x8010325C-60 uebernehmen).

### 7c. OFFEN (benannt, nicht geraten)

- Konsument des Handler-Slots 0x800CE38C (statisch kein Leser auffindbar; §6.1).
- [0x800D4820]==8/6/13-Gates: welche Waffen-Ids das sind.
- +0x1D4-Intro-Trigger (extern, vermutlich SCD-member_set von ROOM40A0).
- Part-6/7 <-> EMR-Bone-Indizes (Attach/Effekte adressieren MESH-Parts; visuell
  Maul/Nacken — fuer den Port reicht der eigene Kiefer-Bone).
- 0x800CFB74-Bits 0x40/0x20 (HP-Zeilenwahl 300/240): Spielstand-/Szenario-Bits.

## FAZIT

1. Das RE2-Gator-Overlay ist EM23_OVL_0000.BIN = CDEMD0.EMS kind 0x23 rec 1
   (Sektor 1522), gelinkt @0x80100000 — byte-identisch mit info/re2leon/COMMON/BIN/.
2. Die EM23-Bank hat 12 Gator-Clips; RE2 nutzt 11 (alles ausser dem leeren Paar 2),
   der Port heute 4 (0/4/7/10). Fehlend: Paddeln 1, Lunge-Anlauf 2, Lunge-Biss 3,
   Schuetteln 5, Intro/Wende 6, Sink-Tod 8, Explosions-Aufbaeumen 9, Kau-Loop 11 —
   jeder mit authored Root-Motion (Netto-Werte §2.1) statt Speed-Proxies.
3. Der Fress-Finisher ist AUTHORED: Gator 4 (Maul zu bei f13) -> 5 (120 F) -> 11
   (Loop). Leon: Strampeln bis f13, dann OPFER-PAAR-3 CLIP 1 (120 F; Schleuderbahn
   in den Keyframes: 10,3k vorm Anker, y bis -9177, seitlich +-2648) — synchron,
   gekoppelt NUR ueber das Anker-Paar 0x80015B94/0x80015CB8 + Teleport (+10643,-915)
   + Yaw-Kopie. Kein Bone-Attach, keine Prozeduralbahn: darum "klappt das
   Rumschleudern nicht wie geplant" — der Port simuliert, RE2 spielt Daten ab.
4. Das Opfer-EMR (Paar 3) hat KEIN Skelett (bto=0, kfo=8): 15-Bone-Keyframes fuers
   Spieler-Rig; Leons y kommt aus dem Pose-Kanal — der Port muss den y-Kanal fuer
   die Opferbank aktivieren (heute genullt, emd_common.c:245-249).
5. DREI Kill-Varianten (Zonen-Fressen 0x401; Proximity <5200 mit Opfer-Clip 0 ab f3;
   Chomp 0x205/0x305 mit 60/30 Schaden — UEBERLEBBAR, Knockback ueber ROOM40A0-RDT-
   Clips 18/19/20/17/6) und VIER Todes-Praesentationen (HP 2->1; Maulschuss
   7->1x5->8; Maulschuss-B 8; Kanister 9 + Explosion). HP = 25000+300/240 —
   Maul-Tod erst unter 25000: der Kill ist ueber Maul-Fenster (~300 Schaden) oder
   Kanister designt.
6. Bonus komplett dekodiert: Gaskanister obj[1]@0x800D051C wird bei T2-t13 an die
   Part-6-Weltmatrix (Maul, mw+0x450) geparented, Lokalpos auf (1750,-210) geeast,
   Raum-Flags Bit 29 (getragen) / Bit 30 (Explosionstod) — falls der Port die
   Kanister-Route je will, steht alles in §5b/R1.
