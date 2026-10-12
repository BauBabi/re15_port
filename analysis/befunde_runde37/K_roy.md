# Runde 37 — Spur K roy (Dossier, fortlaufend)

Wortlaut (AUFTRAG.md Nachtrag 7, 2026-10-12):
> Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine
> Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben.....

Teilpunkte:
- K1 MODELL: Roy (Typ 0x47) in Elzas Raeumen (ROOM1011 verwundet/bewusstlos, ROOM1021 Zombie) mit dem Original-Modell.
- K2 ANIMATIONEN: jeder Plc_motion/Plc_flg/Plc_dest-Clip in 1011/1021 gegen die Bank des richtigen Modells.
- K3 DAUERSCHLEIFE: kein Clip laeuft ueber sein Ende hinaus in Schleife, wo das Original haelt.
- Riegel: Modell-Kennung je Raum, Clip je Szenenabschnitt, keine Schleife, Elliot bei Leon (1170) unveraendert.

Baum: .claude/worktrees/r37_roy (Zweig r37/roy, Basis master 3feb3a78).

## 0. Stand / Fortschritt
- [ ] Messung vorher (Port)
- [ ] RE-Belege Modell (Lader @0x80022300, Tabellen, EMS-Index)
- [ ] RE-Belege Animationen (Clip-Bank je Plc_motion, Halten vs. Schleife)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## 1. Messung vorher (Port, Stand master 3feb3a78, eigener Bau im Spurbaum)

Laeufe (Messkopie der exe, Arbeitsordner scratchpad/K_roy/<lauf>):
* v1011 = ECHTER Weg: Elza-Spielstand ROOM1021 (-4000,-18000) Gier 2695 (probe_r36_elza_karte), CONTINUE,
  RE15_FIRE_AOT=1@40#1021 (Tuer -> ROOM1011, Roy-Szene sub02 laeuft von selbst), RE15_ANIM_TRACE + RE15_STATE_LOG,
  Ende RE15_EXIT_AT=2400#1011.
* v1021 = Elza Neuspiel, RE15_SET_FLAG=4:0x60,3:0xC9, RE15_GOTO_ROOM=1021, RE15_FORCE_EVENT=2@40 (Szene sub02),
  Ende 1400#1021.

debug.log (v1011 Zeilen 91-95, v1021 Zeilen 74-79) — Roy bekommt in BEIDEN Elza-Raeumen Elliots Modell:
    [rbj] room 1011 cinematic overlay: 10 clips, 100 kf
    [enemy] EM47 loaded: 15 meshes, 15 bones, 24 clips -> slot 11        <- CDEMD0.EMS (Leon-Archiv)
    [elliot] PL05 loaded: 15 meshes, 15 bones, 24 clips (Raum 1011, Spawn 0x47)
    [elliot] Raum-RBJ-Overlay: 10 clips                                  <- RBJ-Record 0 = ELZAS Record
    [tim] elliot TIM in slot 1: 256x256
(ELLIOT.MD1/.EDD/.EMR/.TIM sind bytegleich CDEMD0.EMS Typ 0x47 dir[7]/dir[1]/dir[2]/dir[8], Blob @Sektor 1644 —
gemessen mit K_belege/cmp47.py-Logik: MD1 @0xa9a8, EDD @0xc, EMR @0x620, TIM @0xee54, alle eq=True. Der Kommentar
"PL05.PLD" in elliot_pc.c/main.c ist falsch: in PL05.PLD steht keine der vier Dateien.)

RE15_ANIM_TRACE Roy (Aktor 1, Typ 71) v1011 — Clipfolge (Bild: Motion/Clip/Bilder):
    0: 5/5/20 (Schleife, Plc_flg 4) | 299: 7/7/20 | 399: 5 | 502: 6/6/25 | 602: 5 | 705: 7 | 806: 6 |
    906: 10 -> Clip 0 (10 % 10, 1 Bild) | 1006: 5 | 1310: 7 | 1411: 6 | 1511: 5 | 1614: 10 -> Clip 0 | 1715: 7 |
    1816: 9/9/20 -> haelt Bild 19 bis Laufende (2400)
  = Roy spielt Elzas Record 0 (10 Clips {1,1,1,1,1,20,25,20,18,20}) statt seines Records 1 (14 Clips
  {1,1,1,1,1,28,78,30,80,100,60,22,35,2}). Clip 10 und 13 gibt es in Record 0 nicht (anim_select klemmt per
  Modulo: Clip 10 -> 0, Clip 13 -> 3). Die "Ohnmacht" Clip 9 ist Elzas 20-Bilder-Geste (aufrecht, Arm vor) —
  das ist der Befund der Runde-36-Abnahme (B2 "richtet sich auf und bleibt aufrecht").
  Bild K_belege/k3_port_vorher_rec0_auf_elliot.png: Record 0 Clip 5 auf Elliots Modell = KNIEN + Arm
  ausstrecken (Elzas Geste neben dem Verwundeten), Clip 9 = stehend Arm ausstrecken. Clip 5 ist Roys Ruhe-Clip
  mit Plc_flg(0,4) (Schleife, sub03 @0x00DE0/@0x00DE4 und nach jeder Geste) -> der Port laesst Roy minutenlang
  im 20-Bilder-Takt knien und den Arm ausstrecken = die gemeldete DAUERSCHLEIFE ("wiederholen sich staendig").

RE15_ANIM_TRACE Roy (Aktor 2, Typ 71) v1021:
    0..547: Clip 23/30 Bilder Schleife (sub03 Plc_motion(1,23)+Plc_flg 4), 548..727: Clip 5/30 (Plc_dest-Gang,
    halbe Rate wegen +0x1c4 0x10), 728..: Clip 7 = 1 Bild -> steht. Roy faellt nicht (= Runde 36 B1).

## 1a. Wortlaut -> Messgroessen
| Nutzer | Mechanik | gemessen vorher |
|---|---|---|
| "weder das Modell von Roy" | Modell des Typs 0x47 bei Elza | Elliot (CDEMD0 Typ 0x47) statt CDEMD1 Typ 0x47 |
| "noch all seine Animationen" | Kanal je Executor-Sub (RBJ-Record / Loco- / eigene Bank) | 1011: Elzas Record 0; 1021: Elliots Baenke (Clip 7 = 1 Bild) |
| "wiederholen sich ... staendig" | Ruhe-Clip 5 mit Schleifen-Bit | Schleife zeigt Elzas Knie-Greif-Geste (Record 0 Clip 5) |

## 2. RE-Belege (selbst disassembliert aus info/Re1.5/PSX.EXE, re15_disasm.py)

### 2.1 K1 — welches Modell laedt das Original fuer Typ 0x47 bei Elza
Sce_em_set (0x44, Handler @0x800420a0) -> `jal 0x80022300` @0x80042328 (Runde 35 Spur I). FUN_80022300:
    80022324  lbu  v0,8(s2)            ; Typ der Entity
    8002232c  lw   v1,-13764(v1)       ; DAT_800aca3c
    80022334  bltz v1,0x800223ac       ; Bit 31 = ELZA-BIT (TITLE.BIN @0x801024a8/@0x801024e4, re15_gameflow.h)
    80022338  addiu a0,v0,-16          ; Index = Typ - 0x10
  Leon:
    8002233c  ori  v0,zero,0x26        ; Datei 0x26
    80022354  addiu at,at,12092        ; 0x80072f3c (Laenge),  80022370 addiu at,at,12088 ; 0x80072f38 (Sektor)
  Elza:
    800223ac  ori  v0,zero,0x27        ; Datei 0x27
    800223c4  addiu at,at,12668        ; 0x8007317c (Laenge),  800223e0 addiu at,at,12664 ; 0x80073178 (Sektor)
Datei-Ids -> CD (Dateitabelle @0x8006f440, 8 B je Datei; ISO-Verzeichnis der MZD-Disc, K_belege/isolba.py):
    Datei 0x26: Tabellenbytes 24 05 00 .. -> LBA 1316 = PSX/EMD/CDEMD0.EMS (4732928 B)
    Datei 0x27: Tabellenbytes 2b 0e 00 .. -> LBA 3627 = PSX/EMD/CDEMD1.EMS (4739072 B)
TOC-Eintrag Typ 0x47 (Index 0x37, K_belege/emstab.py):
    CDEMD0 @0x800730f0: Sektor 1644, 127640 B (Byte @0x8007316f = a3)  -> Elliot (Leon)
    CDEMD1 @0x80073330: Sektor 1639, 142272 B (Byte @0x800733af = 2e)  -> ROY (Elza)
Kanal-Zeiger, die FUN_80022300 aus dem Blob-Verzeichnis setzt (positionsfest):
    80022488 sw +0x1b0 = dir[0] | 80022498 sw +0x16c = dir[1] (EDD) | 800224a8 sw +0x84 = dir[2] (EMR)
    800224b8 sw +0x174 = dir[3] | 800224c8 sw +0x170 = dir[4] | 800224d8 sw +0x17c = dir[5] | 800224e8 sw +0x178 = dir[6]
    800224f8 sw +0x10 = dir[7] (MD1), TIM = dir[8]
Unterschied der beiden Blobs (K_belege/emsdiff.py):
    Typ 0x47: edd1 1556/2008 B (Clip 7: 1 -> 113 Bilder, Clip 8: 1 -> 2), emr2 29296/34096, edd3 712/836 (Clip 5:
    30 -> 61), emr4 11848/13048, md1 17580/25636, tim 66592/66592 (anderer Inhalt).
Bild K_belege/k1_em047_leon_cdemd0_vs_elza_cdemd1.png (Leser = re15_port/tools/emd_ansichtsblatt.py):
    CDEMD0 Typ 0x47 = hellblaues Hemd, graue Hose (Elliot); CDEMD1 Typ 0x47 = RPD-Uniform (Weste, Abzeichen),
    Blut am Bein (der verwundete Roy). CDEMD1 Clip 7 (113 Bilder) = sackt auf die Knie und faellt aufs Gesicht.

### 2.2 K2 — welche Animationen Roy spielt
Executor-Subs (Dispatch @0x80076ca0: [0]=0x80050cb8 [1]=0x80050ddc ... [9]=0x80051cf8):
    Sub 0: 80050d40 lw a0,384(v0) (+0x180) / 80050d48 lw a1,388(v0) (+0x184) / 80050d4c jal 0x8001f314
    Sub 1: 80050e64 lw a0,132(v0) (+0x84)  / 80050e6c lw a1,364(v0) (+0x16c) / 80050e70 jal 0x8001f314
Plc_motion-Handler @0x80041b90: 80041ba8 sb a1,148(v0) (+0x94 = pc[2] Clip), 80041bc4 sb a2,5(v0) (+0x5 = pc[1]
= Sub), 80041bc8 sh a1,452(v0) (+0x1c4 = pc[3] — Schleifen-/Rueckwaerts-Bits werden mit JEDEM Plc_motion ersetzt).
RBJ-Marker-Binder FUN_8001b3f8 (Raumladen `jal` @0x80039a08):
    8001b404 lw a2,92(v0)           ; RDT +0x5C = Raum-Animation
    8001b438 lw a0,0(v1)            ; Marker-Wort des Records
    8001b440 andi v0,a0,0x1 -> 8001b450/8001b460 sw -> 0x800acbd4/0x800acbd8 (Spieler +0x180/+0x184)
    8001b470 andi v0,a0,0x1 (je weiteres Bit, a3 = Gegner-Index) -> 8001b498 0x800acdac + a3*0x1F4 (+0x180),
    8001b4b4 0x800acdb0 + a3*0x1F4 (+0x184); Gegner-Entity i = 0x800acc2c + i*0x1F4.
Raum-Animation (RDT +0x5C) — K_belege/rbj.py:
    ROOM1011 @0x1384: rec0 Marker 0x1 (Spieler Elza) 10 Clips {1,1,1,1,1,20,25,20,18,20};
                      rec1 Marker 0x2 (Gegner 0 = Roy, Sce_em_set Slot 0 @0x009F6/@0x00A3E) 14 Clips
                      {1,1,1,1,1,28,78,30,80,100,60,22,35,2}
    ROOM1021 @0x25ec: rec0 Marker 0x1 (Elza) 11 Clips; rec1 Marker 0x2 (Gegner 0 = John 0x49 @0x01DF6) 10 Clips.
                      Roy ist dort Gegner 1 (Sce_em_set Slot 1 @0x01E0A) -> Bit 2 -> KEIN Record.
Folge je Skriptstelle (Roy):
    ROOM1011 sub03 @0x00DE0 Plc_motion(0,5)+Plc_flg(0,4): Sub 0 -> rec1 Clip 5 (28 B., Schleife) = sitzend, Hand
      an der Wunde (Bild K_belege/k2_room1011_rbj_rec1_auf_roy.png Zeile 1).
    sub02 Gesten (0,6) 78 B. / (0,7) 30 B. / (0,10) 60 B.: sitzend, Arm-/Handgesten (Zeilen 2,3,5);
      jeweils vorwaerts, dann derselbe Clip mit Plc_flg(0,0x80) rueckwaerts, dann Clip 5 + Plc_flg(0,4).
    sub02 @0x00D54 Plc_motion(0,9): rec1 Clip 9 = 100 Bilder, sinkt aus dem Sitzen flach auf den Boden (Zeile 4).
    sub05 @0x00E00 Plc_motion(0,13)+Plc_flg(0,4): rec1 Clip 13 = 2 Bilder, liegend bewusstlos (Zeile 6).
    ROOM1021 sub03 @0x021C2 Plc_motion(1,23)+Plc_flg 4: Sub 1 -> CDEMD1 dir[1] Clip 23 (30 B.), Zombie-Stand.
    sub02 @0x020B2 Plc_dest Mode 4 -> eigene Bank (+0x170/+0x174 = CDEMD1 dir[4]/dir[3]) Clip 5 = 61 Bilder;
      @0x020BA Plc_flg(1,0x14) = Halbtakt (0x10, Gate @0x80050c00-c34) + Schleife.
    sub02 @0x020DE Plc_motion(1,7,0): Sub 1 -> CDEMD1 dir[1] Clip 7 = 113 Bilder, +0x1c4 = 0 -> einmal + Halten.

### 2.3 K3 — Dauerschleife
Kein Schleifen-Defekt des Abspielers: die Skript-Schleifen (Plc_flg 4) sind original. Defekt = falscher INHALT
der Schleife: Roys Ruhe-Clip 5 lief aus Elzas Record 0 (Knien + Arm ausstrecken, 20 Bilder) statt aus Roys
Record 1 (sitzend, Hand an der Wunde, 28 Bilder). Ursache = dieselbe wie K2 (Elliot-Ausnahme fuer Typ 0x47).

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
