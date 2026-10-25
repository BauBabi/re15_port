# Runde 41 — Spur F "gatorszene" (Dossier)

Baum: `.claude/worktrees/r41_gatorszene`, Zweig `r41/gatorszene`, Basis `aece7457` (= r41/integration 9f10bdb0
+ AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1); Nachtraege 17, 20, 21 vom Orchestrator in den Zweig gelegt (94e0a71a,
4f405376, e8cb2403).

## Punkte (Wortlaut AUFTRAG.md, Zuteilung VERTRAG §0 + Entscheid FG-E1)

1. "Ich moechte eine neue Cutscene: Wenn man Room 2090 betritt, soll der Charakter einen Zombie am Boden liegen sehen
   mit einen blinkenden Schluessel - Blinkend so - wie bei der anderen Implementierung weiter oben der Feuerloescher.
   Es schwimmt bereits ein Zombie im Wasser, nimm vielleicht diesen. Der Aligator soll dann aus dem Wasser springen
   und den Zombie fressen inklusive dem Schluessel. Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe
   seine Leiche sein und blinken."
   + Nachtrag 6: "Leon kommt im Raum an, sieht die Zombie Leiche mit dem Schlüssel in der Cutscene. Dann kommen oben in
   2080 die Zombies rein. Dann erneut Kamera auf Zombie mit Schlüssel. Aligator kommt frisst den Zombie samt Schlüssel.
   Macht danach Richtung Leon eine schrei und Schüttel Animation. Danach geht der Kampf los, mit der Kampfmusik von
   Resident Evil 2." / "Betreff Aligator darfst du alles machen was du willst. Du darfst auch die Startposition des
   Aligators und die Position der schwimmenden Zombie Leiche, die sichtbar den Schlüssel hat ändern".
   + Nachtrag 17: "Muss natürlich ein knallen sein, als ob die Zombies rein brechen, so wie bei STAGE1 von Elza/Leon am
   Ende in der Cutscene nach Irons."
   + Nachtrag 20: "In 2080, nachdem die Zombies durch die Tür gebrochen sind, sollen sie oberhalb von der Treppe laufen,
   nicht unten." (F zeigt in der 2080-Einstellung dieselbe Lage wie Spur G.)
   + Nachtrag 21: "man kommt über die Leiter zum Aligator, nicht über das Tor. Das Tor soll verschlossen sein, und dann
   mit den Schlüssel den man vom Aligator bekommt geöffnet werden!"
2. "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

Ressourcen (VERTRAG 1.1/1.2): Bits (9,110..113) = 110 Szene gesehen, 111 Schluessel liegt, 112 Schluessel genommen,
113 Tor aufgeschlossen; Nachrichten 2090/2091 Ids 42 (Tor zu), 43 (Tor auf), 44-46 frei.

## Stand

- [x] Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE, Dossiers B/D), Nachtraege 17/20/21
- [x] Punkt 2 Spinnen: Messung vorher / Beleg / Umsetzung / Messung nachher
- [x] Punkt 1 Szene + Schluessel + Musik + Knall + 2080 oben + Tor: Messung vorher / Beleg / Umsetzung / Messung nachher
- [x] Riegel: 8 Unit-Teile + 2 exe-Laeufe (Leon, Elza), gezielte Gruppen gruen (§T)
- [x] Spur-B-Riegel integration_r41_re2gator mit belegtem Querbezug gruen (Nachbesserung 1, M4)
- [x] Nachbesserung 1: M1-M6 der Abnahme 0 behoben und gemessen (Abschnitt unten), Zusatzbefund N1 (B-Kamera-Programme)
- [x] Nachbesserung 2: M1 (drei Fremd-Riegel mit Querbezug, gruen) und M2 (Einstellung auf den Schluessel nach dem Tod) der
      Abnahme 1 behoben und gemessen (Abschnitt "Nachbesserung 2")
- [x] Nachbesserung 3: M1 der Abnahme 2 (Zwischenbild der Zonenkamera zwischen Explosionsschnitt und Schluessel-Einstellung)
      behoben und gemessen, Nebenbefund (R1-Tod aus frueherem Raumbesuch) behoben (Abschnitt "Nachbesserung 3")
- [x] Nachbesserung 4: M1 der Abnahme 3 (Nachtrag 29: Leiche samt Kette weiter weg von der Leiter, Ausweichen am Kampfbeginn)
      behoben und gemessen (Abschnitt "Nachbesserung 4")
- [ ] Nachbesserung 5: M1-M4 der Abnahme 4 (= Entscheid F-E2: Kampfbeginn beim RE2-Sollwert, Schluessel nicht dunkler als
      abgenommen, Fressen nicht von hinten, Riegel mit oberer und unterer Grenze) (Abschnitt "Nachbesserung 5")

## 0. Protokoll

* 2026-10-24 19:4x Start: Baum sauber, HEAD aece7457. Messlaeufe immer mit eigener exe-Kopie `re15_pc_r41f_<lauf>.exe`
  (scratchpad-Helfer `F41/lauf.sh`, Elza `F41/lauf_elza.sh`), RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP.
* Commits: 467c766e (Dossier), 07950791/a11460cd (Punkt 2), f4b17b5f (Szene), e0f0635a (Water Key), 6abb940b (Belege),
  898ae6a8 (Cut 1/Licht, Knall, 2080 oben, Tor, Musik), ef13fe31 (Riegel), 73bf903f (r26-Riegel), Schluss-Commit.
* Nachbesserung 1 (2026-10-25): 6e9a7b90 (Plan), f478ece5 (M1/M3/M6/N1), 5b08e96e (M4 + M5), ff5bdf11 (M2 + Riegel),
  e01ea055 (M2 groesstes Bild), d224c392 (exe-Riegel + Bilder), 9d36260c (Dossier), Schluss-Commit fix(r41-gatorszene).
* Nachbesserung 2 (2026-10-25): 58d88629 (Plan), 709c0ceb + 4d013992 (M1), a6e29e7d + f38cbc4d (M2 Code), 1023f54e + be1977be
  (M2 Riegel + Bilder), 0c73197e (Dossier M2), Schluss-Commit fix(r41-gatorszene): Nachbesserung 2.
* Nachbesserung 3 (2026-10-25): 6c8956ec (Plan), 71641acd (Code M1), f7090113 (Dossier vorher/RE, exe-Riegel), f314cb63
  (Unit-Riegel), 4b22cd32 (Messung nachher + Bilder), 9e0347c8 (Nebenbefund), a7e00b75 (KN/KQ), b0192d5b (Spielfigur im
  Riegel), 298d2c5d (P0-Beleg), 36539858 (Unit gruen), Schluss-Commit fix(r41-gatorszene): Nachbesserung 3.
* Nachbesserung 4 (2026-10-25): aa7b3e8a (Plan), 0f990c9f (Messung vorher), f5be4f3d (RE2-Sollwert + Probe), 2b8f3739
  (Lagesuche), 5702190a (Code obj 3 + Unit-Riegel), ec012418 (exe-Riegel + Querbezuege), fb64e68c (beste Lage), 23cad995
  (Dossier Wahl), d9357d86 (Messung nachher + Bilder), Schluss-Commit fix(r41-gatorszene): Nachbesserung 4.
* Nachbesserung 5 (2026-10-25): Commits im Abschnitt "Nachbesserung 5".

## Punkt 2 — Spinnen aus ROOM2090 entfernen

### 2.1 Messung vorher (Stand aece7457, Lauf `v20a0`: RE15_DEBUG_JUMP=20A0@250 + RE15_FIRE_AOT=0@40#20A0)
- state.log F10: Aktor 1 `t=25 @(-100,-8296)` hp 111, Aktor 2 `t=25 @(-2300,-23896)` hp 110, Aktor 35 `t=23` (Boss).
- F70..F550: beide Spinnen laufen auf Leon zu (Aktor 1 bis (6197,-13482), Aktor 2 bis (6194,-19118)), Angriffsclips 4/7/11.
- Cut 0 und Cut 8 zeigen die Spinne neben Leon in der Leiter-Nische (Spawn von 2080 aus).

### 2.2 RE-Beleg
- Zwei RDT-Saetze in main00 (ROOM2090.RDT und ROOM2091.RDT bytegleich):
  `@0x00AB0 44 00 25 00 00 00 00 73 9c ff 00 00 98 df 00 00 00 00 00 00` (Slot 0, Typ 0x25, Kill-Flag 0x73, (-100,0,-8296)),
  `@0x00AC4 44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2 00 00 00 00 00 00` (Slot 1, Typ 0x25, Kill-Flag 0x74, (-2300,0,-23896)).
- FORM im Original: ein Sce_em_set-Satz erscheint nicht, wenn das Kill-Flag-Tor ihn verwirft (FUN_800420a0 @0x80042120-38;
  `ori v0,zero,0x8000` / `sw v0,0(s0)` @0x80042230-38). Der Port verwirft genau diese zwei Saetze beim Lesen (Schluessel
  Raum-Basis + Typ + Original-Lage, Muster zombie_abstand_r35.c), kein RDT-Patch, kein Kill-Flag gesetzt.
- "TEMPORAERER Spinnen-Freeze": existiert im Stand aece7457 nicht mehr (`grep -rn TEMPOR` ohne Treffer).

### 2.3 Umsetzung
- `engine/src/gatorszene_r41.c` / `include/re15_gatorszene_r41.h`: `re15_gatorszene_r41_spawn_weg` (Haken scd_vm.c
  op_sce_em_set direkt nach dem Kill-Flag-Tor), Riegel-Schalter `re15_gatorszene_r41_spinnen_weg_setzen` (kein Spielschalter,
  kein getenv), Zaehler `re15_gatorszene_r41_spinnen_verworfen`.
- Riegel mit Beleg angepasst: `tests/unit/probe_sca_wall_hit.c` Teil C2 misst die SCA-Klemme am ORIGINAL-Satz @0x00AB0 ->
  Verwerfen fuer die Messung aus, danach zurueck.

### 2.4 Messung nachher
- Leon (Lauf `n20a0`) und Elza (Lauf `n2091`, RE15_PSELECT_AUTO=1 + RE15_PSELECT_AUTO_SWITCH=1, `Spieler-Familie PL04`):
  debug.log je zwei Zeilen `[gatorszene] ROOM2090|2091: Spinne 0x25 bei (...) verworfen`, state.log ab der Ankunft 0 Zeilen `t=25`.
- Riegel `unit_r41_gatorszene_spinnen`: 0 Spinnen in 2090/2091, Gegenprobe ohne Verwerfen 2; exe-Riegel zaehlt die Zeilen.
- In 20A0/20A1 bleiben die Schachtspinnen (nicht verlangt).

## Punkt 1 — Ankunftsszene 2090

### 1.1 Messung vorher (Stand aece7457)
- Kein Szenen-Code fuer 2090: Tuerweg 20A0 -> 2090 zeigt Cut 2, der Boss lauert untergetaucht am Lauerplatz (-7200,-9200).
- Kameras ROOM2090 (RID @0x060, 13 Cuts, 32 B je Cut, Bildabstand H >> 7 = 208): Cut 0 zeigt die treibende Leiche obj 2
  (Obj_model_set @0x00A6C, (-1725,-1740,-21723) Gier 685), Cut 5 die zweite Leiche obj 3 (@0x00A8E (-25,-1740,-7423)); beide
  wippen (sub02/sub03 `2e 03 02` / `2e 03 03` + Speed/Add_speed @0x00AF0/@0x00B5E). Leichen-Modell MD1 @0x2054 (Modelltabelle
  @0x200 Eintrag 2): x -444..2626 (Kopf .. Beine; Beine y +700 haengen ins Wasser), Ruecken y -341 bei x 800..1100, z -100.
- 2080 ist aus 2090 unsichtbar (alle Kameras y -2131..-5634 blicken ins Becken; 2080 liegt auf -14400 = ROOM2090 Tuer 0 @0x00996
  Nutzlast-y `c0 c7`). Kameras ROOM2080 (`cut2080_<c>`): Cut 0 Treppe, Cut 2 oberer Hauptbereich mit Rechner, Cut 5 Reling.
- Sichtbarkeit des Schluessels (erste Fassung, Laeufe `lc_*`, `k7b`): das RE2-Bodenmodell Platform Key (ROOM7040.RDT MD1 @0x354C)
  ist ein FLACHES Quadrat 180 x 180 (Ring + Bartschluessel, schwarzer Grund transparent). Aus Cut 0 an der ersten Szenen-Lage
  (-5300,-21000) nur ein Strich (5,6 x 0,6 Bildpunkte gerechnet); in Cut 7 lag er im Umgebungslicht (Lichtsatz Cut 7: Licht 0/1
  bei x -29490/-24699, Reichweite 20000/30000 -> Lichtwert 78), Bildwerte des Rings 33..47 (Lauf k7b, 24 Bilder ueber eine
  Blinkperiode).

### 1.2 RE-Belege
- **Szenenform**: SCD-Bytecode aus Original-Opcodes (Port-Muster cut_10f0.c, irons_tod_1150.c): Set `22 bb ii vv` (ROOM1150
  sub08 @0x01110), Sleep (@0x01162), Cut_chg (@0x01178), Riegel Set(2,7)/(1,27) (@0x01114/@0x01118), Plc_ret + Cut_auto
  (@0x012E0/@0x012E2), Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00` (@0x0112A-@0x01132), Sce_em_set (ROOM2020 main00
  @0x008F6), Door_aot_set-Nutzlast (ROOM2090 main00 @0x00996), Aot_on (ROOM1240 sub02 / ROOM1080 sub07). Montage = Raumwechsel
  in der Szene ueber Door_aot_set + Aot_on (gemessen: keine Tuersequenz, direkte Raumladung wie die Irons-Montage).
- **Ereignisse**: 18 = Szene (2090-Sub-Tabelle @0x0ADC `08 00` = 4 Subs, 2080 @0x0BF8 `06 00` = 3 Subs), 19 = Tor 2090,
  20 = Tor 20A0 (Sub-Tabelle @0x1A24 `16 00` = 11 Subs); kein anderer Port-Hook nutzt 18..20 in diesen Raeumen.
- **Knall (Nachtrag 17)**: der Tuerknall der Irons-Montage: irons_tod_1150.c k_p_szene `OP_KNALL(RE15_IT_SIG_KNALL_TUER),
  OP_SLEEP(45)` vor dem Schnitt, Takt `re15_audio_re2_tuer_se(KNALL_SE_TUER)` auf der Knall-Bank gen/knall_bank.inc Satz 0 =
  RE2 ROOM1060 sub05 Se_on 0x0a @0x00F48 (EDT 0x0a `00 00 85 20` -> VAG 5 @Datei 0x066B8, 8768 B, sha1 192e09df...). Derselbe
  Satz wie Spur G (G-Commit f78be367 "Nachtrag 17 Knall = Knall-Bank Satz 0"). Reihenfolge "erst Knall, dann die Zombies, dann
  der Schnitt" = Runde 36 Spur K (K3) der Irons-Montage.
- **2080 oben (Nachtrag 20)**: Band 8 = oberer Laufgang (ROOM2080 Tuer Slot 1 @0x00BC2 Band 8; Treppe Aot_set Slot 2 sce 12
  @0x0BE2 Band 5 -> 8; Band-8-Absatzwand SCA @0x05C4 endet bei x -16650 - Belege Spur G). Lagen, Typ 0x16, Band 8, y -14400,
  Byte 6 = 0x01, Gier = die acht Saetze von Spur G (re15_kanister2080_r41.h k_zombies, Stand f78be367, "alle mit Fuss UND Kopf im
  Bild von Cut 2"). Abweichend von G: grid 0x0d = ENGAGE ab INIT (STAGE1.BIN `ori v0,zero,0x201` / `sw v0,4(v1)` @0x80100f40-44,
  Raum-Records ROOM1040 main00 @0x011FC, Port-Vorbild EM_1130_DURCH) - sie LAUFEN; Ziel = der geparkte Spieler an der
  Leiter-Ankunft = Nutzlast von ROOM2090 Tuer 0 @0x00996 `22 d9 c0 c7 9c 9b 00 08` (-9950,-14400,-25700) Gier 2048, in BLK-Zone 0
  von ROOM2080 (BLK @0x0B68: Zone 0 x -14500..-8984, z -27100..-6500; Navigator-Ziel nur in einer BLK-Zone, re15_montage_r38.h
  Baustein 2 / FUN_80039e7c). Kamera 2080 Cut 2 (RID @0x0A0, = Spur G RE15_K2080_CUT_SCHUSS); der Parkplatz liegt 65 Grad neben
  der Blickachse (halber Bildwinkel 37,6) -> nicht im Bild. Halt 150 Bilder = Irons-Montage 1130-Schritt ("15 + 10 + 150").
- **Tor (Nachtrag 21)**: 2090/2091 Slot 1 = Door_aot_set @0x009B6 `3b 01 02 31 01 ..` (Rechteck x 6750..7750, z -18100..-14600,
  Band 1, -> 20A0 (4950,-3600,4100) Cut 13); 20A0/20A1 Slot 0 = @0x01746 `3b 00 02 31 02 ..` (Rechteck x 3450..6450,
  z 4500..5500, Band 2, -> 2090 (6300,-1800,-16250) Cut 2). FORM = RE2-Tuer-Handler LAB_80051514 wie Runde 36 Q / 38 F: Satz zum
  sce-3-Ereignis umgewidmet (re15_aot_retype = Aot_reset @0x80040760-a8), Besitz im Inventar (`jal FUN_800696cc` @0x80051628,
  `bltz` @0x80051634): mit Schluessel Text (@0x8005164c), Aufschliess-Ton Se_on(2,0x25) (@0x80051654/58 = RE2_DOOR_SE_AUF_KEY),
  Wegwerf-Fortsetzung (@0x80051670), Schlossbit (@0x80051690), KEIN Durchgang bei diesem Druck (@0x80051698); ohne Schluessel
  Verschlossen-Ton (@0x800516a0/a4, Art M) + Text (@0x800516b8). Texte = ROOM20A0 Nachrichtenblock @0x2008: msg 1 @0x203C
  `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00` "It's locked.", msg 2 @0x204C "You've opened the lock." (ROOM2090 hat
  keinen Nachrichtenblock: RDT +0x3C = 0 -> Port-Texte 42/43 bytegleich zu diesen Saetzen).
- **Gator-Clips (RE2 EM23, selbst disassembliert `re2_disasm.py --bin EM23_OVL_0000.BIN`)**: FUN_8001a330-Rufe @0x801010f4
  (`addiu a1,zero,4` = FRESSEN P1 Clip 4), @0x80101188 (Clip 5 P2), @0x80102c48 (Clip 10,
  `addiu a2,zero,3` = Blend 3, R4), Intro Clip 6 @0x801008ac-b8, Schwimmen Clip 0 Blend 32 @0x80100c38. BERICHTIGT
  (Nachbesserung 1, M6): Clip 11 P3 ist KEIN FUN_8001a330-Ruf - `addiu a1,zero,11` @0x801011d8 + `jal 0x8001a240` @0x801011e0;
  FUN_8001a240 setzt nur Clip/Blend (@0x8001a258-60) und schaltet das Bild (FUN_8002959c @0x8001a28c), ohne den Wurzelschritt
  FUN_80015e7c/FUN_800152c8 von FUN_8001a330 (@0x8001a374/@0x8001a3c0); die Lage setzt davor FUN_80015cb8 vom Anker (@0x801011cc). Wurzelbahnen (EMR
  Keyframe-Bytes 6..11, `F41/em23_bahn.py`): Clip 6 125 Bilder off -7040 -> +2458 (Auftauchen Pose-y +1489 -> -1210 in Bild
  0..30), Clip 4 off 810 -> 3301, Clip 5 2803 -> 3834 -> 3132, Clip 11 3132, Clip 10 0 -> -1025.
- **FRESSEN-Anker** (RE2): Koppelbild 13 @0x80101104-34, Anker FUN_80015b94 (@0x80101110-34), absolute Lage FUN_80015cb8
  (`sw v0,56(s1)` @0x80015d68, `sw v0,64(s1)` @0x80015d90), P2/P3 @0x80101174/@0x801011cc.
- **Leiche im Maul** = RE2-Kanister-Form: Kind der Part-6-Weltmatrix (`addiu a0,s0,1104` @0x801018d0 / `sw a0,128(s2)`
  @0x801018e0), Relation im Koppelbild eingefroren wie FUN_8002d030 @0x801018f0 (rel = R6^T*Ro, lokal = R6^T*(o-p6)).
- **Wende auf Leon** 75/Bild (RE2 R1 FUN_8001569c @0x801025f8-608). **Schrei + Schuetteln** = RE2-Grossreaktion R4: SE 0
  (`addu a0,zero,zero` @0x80102c0c + `jal 0x8005bd6c` @0x80102c10, ENEMSE Bank 17), Clip 10 Blend 3 (@0x80102c40-48), Stoss-
  Ereignis 1 (Bildwort 0x28 -> +0x227 = 1 @0x801003f0-40c; Ereignis-Maschine @0x80101d44 Ruetteln + Bildschirm-Schuetteln).
  Biss-Sperre danach 110 (+0x238 @0x8010091c-20).
- **Kameras im Gator-Teil**: Spur-B-API re15_gr2_kamera_schnitt (RE2-Schnitte, Zonenkameras, Massstab-Grenze): DURCHBRUCH ohne
  Opfer-Bedingung (RE2 Cut 10, soll frontal +4092), FINISHER mit der Leiche als Opfer (RE2 Cut 7/14). Fuer den Schrei bleibt die
  laufende Kamera, solange sie den Kopf zeigt (re15_gr2_im_bild_cut): die Wahl ANGRIFF nahm in Lauf s3 Cut 3, dort liegt der Kopf
  (Bild (266,106)) hinter dem Vordergrund des Kessels (die Wahl prueft keine PRI-Masken).
- **RE2-Kampfmusik** (ROOM40A0): INIT_TBL.DAT (Szenarioblock 0x16C8 B, Tabelle @0x22C, Stage-Versatz @0x800a80b0
  `00 1f 3d 4d 60 6c 86` -> Stage 4 = 0x4d; Eintrag 0x4d + 0x0A @0x2DA = `5a ff` in BEIDEN Bloecken) = MAIN 0x1A mit
  Manuell-Start-Bit 0x40, kein SUB; Gegenprobe der Tabellenlage ueber Sce_bgmtbl_set (0x57, Handler @0x80057e98) in ROOM4090/
  4100/2020. Gestartet von sub11 `51 00 01 00 78 40` @SCD 0x071F (Handler @0x80057e20, FUN_8005b6f0 Fall 1 = SsSeqPlay),
  ausgeblendet `51 00 05 00 00 00` @0x0762/@0x078A. Datei COMMON/SOUND/MAIN1A.BGM (228152 B, md5 01835748815e388e2519d4977d48aab5):
  Anhang u32 @size-12 = 0xE88 VB, @size-8 = 0x64 VH, @size-4 = 0 SEQ; SEQ 100 B `pQES`, VAB `pBAV` 2 Programme, 8 Toene, 1 VAG
  224416 B (= ein langer Sample-Loop, vom SEQ gespielt).
- **Schluessel**: der nie platzierte RE1.5-"Water Key" (Namenstabelle DEBUG.BIN @0x800c495c Eintrag 0x45, CHECK 'The label says:
  "Sewer Mainteinance Room."' @0x800c50de, Icon ITEMALL.PIX Tile 0x45 = Schluesselbund, ITPS.ITP Block 0x45) unter der nie
  platzierten Id 0x34 (0x45 traegt seit Runde 36 Q den Parking Lot Key); Weltmodell RE2 Item 0x63 Platform Key (ROOM7040.RDT MD1
  @0x00354C 140 B, TIM @0x033844), Ring + Bartschluessel = naechstes RE2-Modell zum RE1.5-Schluesselbund
  (`tools/r41_gatorszene/schluessel_export.py --pruefen` -> "bytegleich"). Blinken = re15_blink_r41 (Spur D, Form TITLE.BIN
  FUN_801028ec). Tod des Alligators = R1-Endzustand (re15_gr2_tot, +0x10E |= 0x4000 @0x80102654-64) ODER Todesrolle Clip 7 mit
  CORPSE-Zustand 7 (enemy_ai_boss_gator.c GBP_DIE -> GBP_DEAD `e->state = 7`). Item-Zone = Item_aot_set-Form (Rechteck 1000 x 1000
  wie ROOM2090 main00 @0x00A12), Taken-Bit (9,112).

### 1.3 Umsetzung
- **Programme** (engine/src/gatorszene_r41.c, Ereignis 18):
  P1 2090 Ankunft: Abschnitt-Ende, Riegel, Cut 1, Sleep 90 (1,5 Blinkperioden 0x3c), Set(5,29) KNALL, Sleep 45, Door_aot_set ->
  2080 (-9950,-14400,-25700) Gier 2048 Cut 2, Aot_on. Der Port-Takt laedt die Knall-Bank und spielt Satz 0.
  P2 2080 Montage: acht Sce_em_set (oben, s. 1.2) vor dem ersten Bild, Riegel, Cut 2, Sleep 150, Door_aot_set -> 2090 an Leons
  Eintrittslage, Cut 1.
  P3 2090 Rueckkehr: Riegel, Cut 1, Sleep 45, Set(5,28) Gator, Poll (5,30), Set(9,110,1), Plc_ret, Cut_auto, Riegel auf.
- **Leiche + Kamera** (PORT-WAHL nach Nachtrag 6, `tools/r41_gatorszene/leiche_lage.py`): je Zonenkamera Cut 0..7 die Lage, in der
  die ganze Leiche mit Rand im Bild liegt und die Gator-Buehne (Kopf im Koppelbild 13 auf der Rumpfmitte, Start S aus dem Norden
  durch den Westkanal) ganz in den sce-8-Wasserzonen (main00 @0x009D6/@0x009EA/@0x009FE) verlaeuft, mit dem groessten Wert
  Bildgroesse^2 x Licht des Schluessels (Lichtsatz des Cuts @0x468 + 40*Cut, NCCT-Modell re15_light.h). Ergebnis: Cut 1 (Wert
  23412; Cut 7 7209, Cut 0 5026; Lichtsatz @0x490, Licht 0 bei (-4790,-4000,-24387)), Leiche (-5500,-1740,-23700) Gier 384,
  Rumpfmitte (-4594,-24306), Start S (-4623,-13591) im Nordbecken. Schluessel: Kind der Leiche, lokal (1090,-420,-100) (hoechste
  Stelle des Rueckens + halbe Kippung), Neigung rx 608 / rz -160 (54,7 Grad zur Kamera, hoechstens 60): Bildflaeche cos 0,270 ->
  0,925 bei Licht 216 -> 210, ~10,4 x 9,6 Bildpunkte.
- **Gator-Ablauf** (Haken enemy_ai_boss_gator.c nach re15_gr2_takt / nach re15_gr2_nachtakt): untergetaucht halten bis Signal 28,
  dann Buehne + Clip 6 (Durchbruch) -> Clip 4 Bild 1..14 (Koppelbild 13: Leiche haengt am Kopf) -> Clip 5 (Schuetteln) -> Clip 11
  (ein Kau-Kreis, Leiche + Schluessel weg) -> Clip 0 Wende auf Leon (75/Bild) -> Clip 10 Blend 3 + SE 0 + Stoss-Ereignis 1 ->
  Clip 0, Biss-Sperre 110, Kamera frei, Kampf (Boss in die Verfolgung), Signal 30.
- **Musik**: Weiche 0xFF3C (Muster cut_10f0.c 0xFF01) in audio_pc.c ss_bgm_entry; MAIN-Platz 0x3C (im RE1.5-Zensus unbenutzt,
  SOUND/MAIN3C.BGM = 4-Byte-Platzhalter) wird aus `shared_assets/RE2/SOUND/MAIN1A.BGM` geladen (Haken re15_bgm_load_track).
  An: Bit 110 gesetzt, Alligator aufgetaucht und lebend; aus: Alligator tot (dann die RE1.5-Raummusik von 2090) oder Raum verlassen.
- **Schluessel nach dem Tod**: Kopf (Part 6) des toten Alligators, in eine Wasserzone gezogen (re15_aot_water_at), auf der
  Wasseroberflaeche y -1620 (sce-8-Zone p0 @0x009D6 `ac f9`), Item-Zone Slot 15 (frei in 2090), Bits 111/112; nach dem Laden eines
  Spielstands (Todeslage nur im RAM) die Rumpfmitte der gefressenen Leiche.
- **Tor**: s. 1.2; Wegwerf-Stelle (2090/2091, msg 43) in item_discard_common.c. 20A0 wird nur an der 2090-Seite aufgeschlossen
  (dort bekommt man den Schluessel); von 20A0 aus "It's locked." (Original-msg 1) bis Bit 113.
- **Haken in gemeinsamen Dateien** (je 1-5 Zeilen, "Runde 41 Spur F"): scd_vm.c (Spawn-Verwerfen, Ereignis, Takt),
  scd_room_setup.c (Installer), enemy_ai_boss_gator.c (Boss-Takt, Nachtakt), platform/pc/main.c (Prop-Lader, Icon/Bild, PC-Init),
  audio_pc.c (BGM-Weiche, RE2-Datei), item_prompt_common.c / re15_inv_screen.c / itps_common.c / item_icon_common.c /
  inv_render_pc.c (Name/Text/Bild/Icon 0x34), item_discard_common.c (Wegwerf-Stelle); Riegel: probe_sca_wall_hit.c (C2),
  test_r36_kartenbits.c (k_port 110-113), probe_r26_inventar.c (B3/B4: 10 'key'-Namen, 0x34 im Port-Namen-Zweig).

### 1.4 Messung nachher (exe-Kopien, gebauter Stand 898ae6a8 ff.)
- **Leiter-Weg Leon** (`lad1`..`lad4`: RE15_SET_FLAG=3:245, RE15_DEBUG_JUMP=2080@250, RE15_FIRE_AOT=1@40#2080): Tor gesperrt
  (`ROOM2090 Slot 1 (Tor) gesperrt: Ereignis 19`), Folge A (-8200,-25550) -> P1 -> T (Bild 98) -> P2/M 8 Zombies (2080) -> P3 ->
  S -> Buehne `Leiche (-4595,-24305) Kopf13-Versatz (45,-3055) Weg4 (0,-1328) Weg6 (-16,-6332) -> A (-4640,-19922)
  S (-4624,-13590) Gier 1024` -> G1 Cut 7 -> G2 Cut 0 -> K13 -> G3 -> G4 -> V (Bild 369) -> G5 -> G6 (Cut 0 bleibt) -> G7 (Bild 405)
  -> B 1. Elza (`el1`, 2081/2091): dieselbe Folge.
- **Bild Leiche** (`F_belege/f1_leiche_cut1.png`, lad3 Bild 45): Leiche gross in Cut 1, der Schluesselring ueber dem Ruecken
  sichtbar; Leon (Leiter-Ankunft) nicht im Bild. Puls des Rings (k1, 24 Bilder): hellster Bildpunkt 93..112.
- **2080** (`F_belege/f2_2080_oben.png`, lad1 Bilder 12/56/100/133, Cut 2): acht Zombies auf dem oberen Laufgang, sie laufen nach
  links Richtung Leiter; state.log (lad4): Bild 1 `[1 t=16 .. @(-12800,-19600,r2698)]` .. `[8 .. @(-9900,-14700,r1819)]`, letztes
  Bild z.B. `[1 .. @(-13892,-19575,r1578)]`, `[8 .. @(-10732,-18661,r949)]`: 8 von 8 gelaufen, alle x > -16650, y -14400.
- **Gator** (`F_belege/f3_gator_ablauf.png`, s4): Bild 73/145 Cut 7 - der Gator taucht im Westkanal auf und kommt frontal mit
  offenem Maul; Bild 181/265 Cut 0 - Biss und Schuetteln (Leiche in der Luft); Bild 397 Schrei (Cut 0, Kopf rechts im Bild);
  Bild 421 Kampf (Cut 1 frei, Leon im Hintergrund).
- **Knall** (`ton1`, SDL_AUDIODRIVER=dummy statt RE15_NOAUDIO): `Folge T ROOM2090 F98 wert=0/1` = Knall-Bank geladen, Satz 0
  gespielt. **Musik**: `[bgm] stage=1 room=09 entry=FF3C -> MAIN3C(flag 0) SUB--(flags 1/1)` und `[bgm] loaded: 1 VAGs, SEQ 100B,
  PPQN=480 tempo=372670us` (= der MAIN1A-Container: 1 VAG, SEQ 100 B) mit Folge B 1 bei Bild 407.
- **Tod -> Schluessel** (`tod1`: RE15_SET_FLAG=9:110, RE15_GB_TEST=3 = Testhaken-Tod Clip 7): `Water Key (Item 0x34, Bits
  (9,111)/(9,112)) bei der Leiche (-7277,-1620,-25029), Zone Slot 15`, Folge L + B 0 (Musik aus) bei Bild 470.
- **Tor + Aufnahme**: Riegel unit_r41_gatorszene_tor/_schluessel (s. §T).

## §T Tests

Neu: `tests/unit/probes/r41_gatorszene.cmake`, `tests/unit/test_r41_gatorszene.c` (unit_r41_gatorszene_spinnen | _programme |
_leiche | _texte | _tor | _schluessel | _szene | _szene_elza), `tests/integration/test_r41_gatorszene.cmake`
(integration_r41_gatorszene_leon | _elza: echte exe ueber die Leiter, Folge-Zeilen in Reihenfolge, Tor gesperrt, Spinnen
verworfen, 2080-Zombies am state.log oben und laufend). Plattformneutral (keine Pfade ausser CMAKE_SOURCE_DIR, eigene exe-Kopie
je Lauf mit Zufalls-Id, eigener Arbeitsordner).

Gezielte Laeufe (keine volle Suite), wörtlich:
`ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_discard$|unit_item_modal$|unit_item_icon$|unit_sca_wall_hit$|unit_re2_spider_water_2090$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_opcodes$|unit_scd_aot_opcodes$|unit_door_seq$|unit_r38_wasser_aotbreite$|unit_r41_kleinkram_a|r31_trefferboxen|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$)" -j3 --timeout 900 --output-on-failure`
-> `100% tests passed, 0 tests failed out of 81` (Total Test time (real) = 263.64 sec).

Gesondert (Spur B, NICHT gruen, s. §O): `ctest ... -R "^integration_r41_re2gator$"` -> `0% tests passed, 1 tests failed out of 1`
("RE2-Intro ... nicht mit gewaehlter ..."): der Debug-Sprung nach 2090 startet jetzt die Ankunftsszene.

## §O OFFEN

1. **Spur-B-Riegel integration_r41_re2gator** (B's Datei, nicht von F geaendert): alle fuenf Laeufe springen ohne Bit 110 nach
   2090 (die Ankunftsszene uebernimmt) und Lauf "belagerung" geht durchs Tor 20A0 -> 2090 (jetzt verschlossen, Nachtrag 21).
   Gegenprobe mit `RE15_SET_FLAG=9:110,9:113` in allen Laeufen (Kopie im scratchpad, `F41/btest`): Lauf "nische" kommt durch die
   Kamera-Folge, scheitert dann an "Finisher-Halt 7 Zeilen (erwartet >= 10 bis EXIT_AT 650)": ohne die Spinnen faellt Leons HP
   spaeter (state.log `F0 hp=100, F290 hp=50, F397 hp=0, F520 hp=-50` -> FRESSEN erst beim dritten Biss F520 statt F403; FRESSEN bei
   hp < 0). Noetig (Orchestrator/Spur B): die Bits in den Laeufen + neue Bildfenster (z.B. EXIT_AT spaeter) oder Leons HP per
   vorhandenem Messhaken RE15_R39_MESS_SPIELER_HP.
2. **Abgleich mit Spur G**: die acht 2080-Lagen sind von G kopiert (k_zombies, Stand f78be367) - bei der Integration auf G's
   `re15_k2080_zombie_lage()` umstellen oder gleich halten. Knall = derselbe Satz 0. G's Montage-/Ankunftslogik laeuft erst nach
   Bit 110 (F's Montage laeuft davor) - keine Ueberschneidung gemessen, aber nicht zusammen gebaut.
3. Schluessel nach dem Tod liegt FLACH auf dem Wasser: seine Bildgroesse haengt an der gerade gezeigten Zonenkamera (aus Cut 0 in
   Lauf tod1 nicht zu erkennen; beim Hingehen groesser). Blinken + Aufnahme-Zone (1000 x 1000) helfen beim Finden.
4. Nach dem Tod spielt wieder die RE1.5-Raummusik von 2090 (MAIN2E); RE2 blendet in ROOM40A0 aus (`51 00 05` @0x0762) und
   bleibt still - nicht nachgebaut, weil der Nutzer nur den Kampf mit RE2-Musik verlangt hat.
5. Nachrichten-Ids 44-46 (2090/2091) unbenutzt.
6. PSX-Bau nicht geprueft (kein PSn00bSDK im Baum); Datenmenge: Knall-Bank (20 KB, dritte Kopie der .inc), Schluessel-TIM 34 KB,
   ITPS-Block 12 KB.
7. In 20A0/20A1 bleiben die Schachtspinnen (nicht verlangt).

## Für den Nutzer

- **Neue Assets (Paket-Gate)**: `re15_port/shared_assets/RE2/SOUND/MAIN1A.BGM` (RE2-Kampfmusik des Alligators, bytegleich aus
  info/re2leon/COMMON/SOUND/MAIN1A.BGM, 228152 B, md5 01835748815e388e2519d4977d48aab5). Der Water Key steckt als eingebackene
  Daten in `re15_port/engine/src/gen/gatorszene_r41_schluessel.inc` (keine Datei im Paket noetig). Keine Sprachdateien.
- **Ablauf zum Ausprobieren**: STAGE2 bis ROOM2080 (Rechner (3,245) gesetzt), die Leiter hinunter nach 2090: Leiche mit
  blinkendem Schluessel (Kamera 1) -> Knall von oben -> Schnitt nach 2080: acht Zombies laufen oben auf dem Laufgang -> zurueck zur
  Leiche -> der Alligator taucht im Westkanal auf, frisst die Leiche samt Schluessel, dreht sich zu Leon und schreit (Bild wackelt)
  -> Kampf mit der RE2-Musik. Nach dem Tod des Alligators liegt der Schluessel blinkend bei seinem Kopf im Wasser; aufheben
  (Aktionstaste) -> "Water Key" im Inventar ("The label says: Sewer Mainteinance Room."). Das Tor 2090 <-> 20A0 (Osten, Steg) ist
  verschlossen ("It's locked."); mit dem Water Key: "You've opened the lock." + Wegwerf-Frage, danach ist das Tor offen.
  Messhilfe ohne Spielweg: RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080.
- **Hinweis**: Der Schluessel ist das RE2-Bodenmodell "Platform Key" (Ring mit Schluessel, flach); in der Leichen-Einstellung
  ~10 Bildpunkte gross, leicht zur Kamera gekippt.

## Nachbesserung 1 (nach Abnahme 0, F_abnahme_0.md: M1-M6)

Start 2026-10-25, HEAD 76ad856d, Baum sauber. Messordner (scratchpad, nicht im Repo): `F41n1/` (`runs/<lauf>/debug.log|state.log|
gator_boss.log|fd_*.ppm`, Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41n1_<lauf>.exe`, timeout, danach geloescht;
`tools/puls_diff.py` = Bildvergleich mit/ohne Schluessel). Alle Laeufe RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP.
Belegbilder: `F_belege/n1_*.png`.

### N1.M1 Kampfbeginn ohne Spielfigur im Bild (Leon und Elza)

- **Ursache.** Am Szenenende ruft der Gator-Ablauf `re15_gr2_kamera_frei` (Spur B). Dessen Cut_old-Zwilling kehrt auf den Cut
  VOR dem Kamera-Programm zurueck = P3s Leichen-Cut 1 (gator_boss.log "zurueck -> Cut 1 F405"). Die Zonen-Zustandsmaschine
  FUN_80014230 prueft nur Zonen mit cam_from = angezeigter Cut (`lbu v1,22(a0)` @0x80014254, Memory reai-v2-camera-zone-
  statemachine). Die Gruppe von Cut 1 (RVD ROOM2090 @0x0025C Anker, @0x00270 1->0 bei x -1500..-500, @0x00284 1->2) enthaelt
  Leons Lage (-8392,-25550) nicht. Die Kamera blieb deshalb bis zum Ende auf Cut 1, Leon hinter der Kamera.
- **Messung vorher** (Abnahme-Lauf L1, state.log 5. Raum selbst ausgewertet): F370-F404 cam=0 (Schrei im Finisher-Cut 0), ab F405
  cam=1 bis F699, Leon (-8392,-25550), F466 erster Treffer.
- **Beleg (Original-Form des Szenenendes).** ROOM1150 sub08 (eigener SCD-Dump): `29 05` Cut_chg(5) @0x012C4 auf den Cut der
  Spielerlage, `42` Plc_ret @0x012E0, `3c 01` Cut_auto @0x012E2. Zensus aller 240 RDTs: vor 115 von 201 Cut_auto(1) steht als
  letzter Kamera-Befehl ein Cut_chg, vor 56 ein Cut_old. Cut_chg = LAB_800402a0 (alter Cut `lbu a1` @0x800402c0 / `sb` @0x800402e4,
  work_vars[0x0C] @0x800402ec, work_vars[0x0A] @0x800402fc, Bit 0x100 @0x800402d4). Ziel = Eintritts-Cut, den der Raumlader aus
  der Tuer-Nutzlast setzt (@0x8001d940; Leiter ROOM2080 Slot 1 @0x00BC2 -> Cut 0).
- **Aenderung** (gatorszene_r41.c): `s_leon_cut` = g_scd.cam_id beim Ankunfts-Installer (scd_room_setup.c setzt ihn vor den
  Installern). `kam_uebergabe()` = Cut_chg-Zwilling auf diesen Cut, im SELBEN Bild wie `re15_gr2_kamera_frei` (der SCD-Takt
  laeuft vor dem Spielschritt, main.c; im naechsten Bild waere Cut 1 ein Bild lang sichtbar). Programm 3 macht danach wie
  ROOM1150 Plc_ret + Cut_auto 1. Folge-Zeile 'C' (Cut, Spielfigur im Bild), Messzeile
  `[gatorszene] Kamera-Uebergabe: ...`, Pruefhaken `re15_gatorszene_r41_spieler_im_bild` (Fuss und Kopf = eine Bandhoehe 0x708
  hoeher im Bild, Projektion wie der Zeichner).
- **Zusatzbefund N1 (vom Bau-Agenten verursacht, von der Abnahme nicht gemeldet):** `ga_start` schaltete mit
  `re15_gr2_kamera_auto(0)` Spur B's Kamera-Programme ab und niemand schaltete sie wieder an. Nach der Ankunftsszene liefen deshalb
  fuer den Rest der Sitzung weder der R1-Explosionsschnitt (gr2_kam_r1_start, RE2 @0x801021ec-6c) noch der Finisher-Schnitt
  (re15_gr2_kamera_finisher, RE2 @0x80100fec-ff4) noch der Intro-Schnitt. Jetzt am Szenenende `re15_gr2_kamera_auto(1)`.
- **Messung nachher.** L1 (Leon) und E1 (Elza, `Spieler-Familie PL04`), Leiterweg wie Abnahme L1: debug.log `Folge C ROOM2090|2091
  F405 wert=0/1`, `Kamera-Uebergabe: Cut 0 (Eintritts-Cut), Spielfigur im Bild (Fuss-Bildpunkt 88,126)`; state.log 5. Raum
  F400-F499 in jedem Bild `cam=0` (F405 kein Cut-1-Bild; F406 pm=1, F420 pm=0). Bildboegen `F_belege/n1_m1_kampfstart_leon.png`
  (F400-F470) und `n1_m1_kampfstart_elza.png` (F404-F470): Spielfigur links an der Leiter, der Alligator davor, Angriff F470.

### N1.M2 Water Key nach dem Tod unsichtbar

- **Ursache.** Die erste Fassung legte das FLACHE Quadrat (MD1-Normale (0,-4096,0) @0x354C+0x60) an den Kopf des Alligators.
  Alle Zonenkameras von 2090 stehen auf y -3132, also 1512 ueber dem Wasser, an den Beckenenden (RID @0x060). Von dort ist ein
  flacher Schluessel 180 x sin(Blickhoehe) x H / Tiefe hoch: Gegenprobe im Riegel unit_r41_gatorszene_tod 0,8-0,9 Bildpunkte
  (Todesrolle) bzw. 1,2 (R1). Die zwei Kopf-Lagen der Abnahme lagen zudem unter bzw. hinter Cut 0/1.
- **Messung vorher:** Abnahme 0 §3.3 (R1 in allen 13 Kameras bitgleich, Todesrolle Cut 0/1 bitgleich).
- **Beleg/Form.** PORT-WAHL der Lage (Nutzer: "soll der Schluessel in der Naehe seine Leiche sein und blinken"), dieselbe Form wie
  der Schluessel der Ankunft (leiche_lage.py). Alle Groessen kommen aus den Daten: Kamera-RID @0x060 (re15_camera_build_view
  wie der Zeichner), Wasserzonen sce 8 (re15_aot_water_at, ROOM2090 @0x009D6/@0x009EA/@0x009FE, p0 = Wasserhoehe -1620),
  Quadrat +-90 (MD1 @0x354C), Alligator-Kasten {1000,-720,0,2200,720,800} @0x80118b98 (halbe Breite 800), Knochen der
  gezeichneten Leichen-Pose (re15_gr2_bone_welt_pub), Blink-Spitze 0xBE und Kappung 0x80 (s. M5).
- **Aenderung** (gatorszene_r41.c `key_lage_bei_tod`, `key_suche`, `key_neigung`):
  1. Kamera = die laufende (R1 P3 setzt das Tot-Flag @0x80102654-64 und gibt die Szenen-Kamera im selben Schritt frei
     @0x80102688-94). Hat sie keine Lage, gilt die Zonenkamera (RVD-Gruppe mit Wechselsatz, Cut 0..7) mit dem groessten Bild.
  2. Lage = Rasterpunkt (100) im Wasser (Mitte und vier Ecken) im Bild der Kamera (x 24..296, y 40..205 wie leiche_lage.py), vor
     JEDEM Knochen der Leiche um 800, hoechstens so weit vom naechsten Knochen wie der Kopf der Leiche von ihrer Wurzel (Knochen 6
     zu 0). Darunter wird die Lage mit dem groessten Bild gewaehlt. Die Zwischenfassung "der Leiche am naechsten" (Lauf T1n:
     Tiefe 6948) zeigte nur 4 Pixel von 43 bis 78 und wurde deshalb verworfen.
  3. Neigung zur Kamera hoechstens 60 Grad mit dem groessten cos(Blick) x Helligkeit, deren Blink-Spitze die Primitivfarbe 0x80
     nicht ueberschreitet. Das Licht wird wie im Zeichner gerechnet (re15_light_setup_actor / _ctx_rotate_for_bone /
     _shade_vertex, Lichtsatz des Cuts).
  4. Hoehe: die tiefste Ecke liegt auf der Wasseroberflaeche.
  5. Nach dem Laden eines Spielstands (Ersatzlage = Rumpfmitte der Leiche) folgt die Neigung im ersten Takt, weil
     room_common.c Schritt 10 den Lichtsatz erst nach den Installern parst.
- **Messung nachher** (Differenzbild = derselbe Lauf mit (9,112) vorab, Bild fuer Bild, `tools/puls_diff.py`):
  | Lauf | Kamera | Lage (Messzeile) | Bilder mit Unterschied | Kasten | hellster Pixel | Bilder am Max |
  |---|---|---|---|---|---|---|
  | T2n/T2x Leon Todesrolle (GB_TEST=3) | Cut 0 laufend | (-1300,-1729,-24000), Tiefe 4835, 3061 von der Leiche, rx -416 rz 576, Prim 86 -> 128 | 64 / 64 (F472-F598) | (175,124)-(178,129), 4 px | 75 -> 111 (x1,48) | 4 |
  | K1n/K1x Leon R1-Kanistertod (Rezept K, RE15_FORCE_CUT=1 wie Abnahme B 0) | Cut 1 laufend | (-1600,-1717,-26900), Tiefe 5766, 2985 von der Leiche, rx -608 rz -320, Prim 86 -> 128 | 66 / 66 (F200-F330) | (257,124)-(262,127), 6 px | 75 -> 110 (x1,467) | 5 |
  | integration_r41_gatorszene_tod (Leon) | Cut 0 | wie T2n | 60 / 60 (F480-F598) | 4 px | 75 -> 111 (x1,48) | 4 |
  | integration_r41_gatorszene_tod_elza (Elza 2091) | Cut 0 | wie T2n | 60 / 60 | 4 px | 75 -> 111 (x1,48) | 4 |
  Der Puls ist ein Dreieck mit der Periode 60 (Spitzen F508-514 / F568-574). Riegel unit_r41_gatorszene_tod: Todesrolle Cut 0
  9,7 Bildpunkte breit, R1 Cut 1 7,5, R1 mit Sprung-Cut 0 -> Ersatz Cut 1, Elza wie Leon. Bilder:
  `F_belege/n1_m2_todesrolle_cut0.png`, `n1_m2_todesrolle_zoom.png`, `n1_m2_r1_cut1.png`.
- **Grenze (ehrlich):** Der Schluessel ist das flache RE2-Bodenmodell Platform Key (duenner Ring mit Bart auf 180 x 180,
  sonst transparent), und die Zonenkameras stehen an den Beckenenden. In der Naehe der Leiche bleiben 4-6 sichtbare Pixel, die
  im Dreieck pulsieren. Ein groesserer oder massiverer Schluessel braeuchte ein anderes Modell -> Frage an den Nutzer (unten).

### N1.M3 Doppelter Verschlossen-Ton am Tor 20A0/20A1

- **Ursache.** `tor_druck` spielte Art M selbst. Die Original-msg 1 von 20A0 steht als Text-Platz-Stelle in
  gen/lock_se_sites.inc (Zeile 49/50), deshalb spielte `re15_scd_show_message` -> `re15_lock_se_notice` den Ton ein zweites Mal.
- **Beleg (selbst disassembliert, re2_disasm.py PSX.EXE):** RE2 LAB_80051514 Nicht-Treffer-Zweig: `lui a0,0x216` @0x800516a0,
  `jal 0x8005ba28` @0x800516a4, `addu a1,zero,zero` @0x800516a8, danach Text `jal 0x8002fe38` @0x800516b8. Das ist GENAU EIN
  Se_on (Treffer-Zweig: Besitz `jal 0x800696cc` @0x80051628 / `bltz` @0x80051634, Text @0x8005164c, `lui a0,0x225` @0x80051654).
- **Aenderung:** Der Tor-Handler spielt den Ton nur, wenn (Raum, Nachricht) keine Text-Platz-Stelle der Tabelle ist
  (`lock_stelle`, re15_lock_se_site). In 2090/2091 (Port-msg 42) spielt ihn also der Handler, in 20A0/20A1 (msg 1) die Tabelle.
- **Messung nachher:** unit_r41_gatorszene_tor `2090 ohne Schluessel: 1 Verschlossen-Ton(e), Satz 0 (soll 1 x 0)` und
  `20A0 ohne (9,113): 1 Verschlossen-Ton(e), Satz 0`; unit_r30_tuer_verschlossen: ROOM20A0/20A1 Platz 0 msg 1 Art M `1 x 0`.

### N1.M4 Rote Fremd-Riegel (belegte Querbezuege, je gezielt gefahren)

| Riegel | Ursache | Querbezug / Aenderung | Ergebnis |
|---|---|---|---|
| unit_r36_tueren_schott | Nachtrag 21: 2090 Slot 1 @0x009B6 bis Bit 113 sce-3-Ereignis | nach Orchestrator-Entscheid F-E1: ohne (9,113) keine begehbare Tuer nach 20A0 (`ROOM2090 ohne (9,113): 0 begehbare Tuer(en)`), mit (9,110)+(9,113) Durchgang DOOR26 V0 wie bisher | gruen |
| unit_r38_spinnen (Teil D) | Nutzer-Punkt 2: Saetze @0x00AB0/@0x00AC4 verworfen | nach F-E1: 2090 aus der Spinnen-Erwartung, die Zeile verlangt jetzt 0 Spinnen (`RE1.5/RE2 ROOM2090: 0 Spinnen`), uebrige Raeume unveraendert (erste Fassung dieser Nachbesserung mass 2090 mit abgeschaltetem Verwerfen - ersetzt) | gruen |
| unit_r30_tuer_verschlossen | 20A0/20A1 Slot 0 @0x01746 zeigt jetzt msg 1 (Stelle von Platz 8) + M3 | 54 Plaetze (K 16 / M 38 / S 0), weiterhin 47 Stellen; je Platz 1 Ton | gruen |
| integration_r41_re2gator (Spur B) | (a) ohne Bit 110 startet die Ankunftsszene, Tor 20A0 -> 2090 bis Bit 113 zu; (b) ohne Spinnen trafen Leon in "nische" zwei Spinnen-Treffer weniger (Abnahme B 6 Lauf A2: 100 -> 80 F57 -> 60 F175, Gator 10 F290 -> -40 F397); (c) "kanister": Aufnahme F1227 statt F1119 | die fuenf Kampf-Laeufe RE15_SET_FLAG=9:110,9:113 (B's RE2-Intro beim WIEDER-Betreten nach der Szene); nische RE15_R41_HP_AT=175:60 (Messhaken Spur D) = dieselbe HP wie mit den Spinnen; kanister RE15_EXIT_AT 1400 -> 1500 (Schranke >= 200 Bilder bleibt); nach F-E1 SECHSTER Lauf "szene" (Leiter, ohne Bit 110): Auftritt ueber F's Szene. Querbezug im Kopf der Datei | gruen: nische hp 100 -> 60 F175 -> 10 F290 -> -40 F397, FINISHER Cut 6 ab F397; kanister angehaengt F1227, 270 Bilder, Abweichung 0; szene: kein RE2-Intro, Schnitte DURCHBRUCH Cut 7 -> FINISHER Cut 0 -> Ende, Clip-Folge 6/4/5/11/0/10, kein Gier-Sprung |

### N1.M5 Blinken in der Ankunftseinstellung gesaettigt

- **Ursache (gemessen am Code, nicht vermutet).** Die NCCT-Ausgabe ist Licht/2 (CODE 0x80 der Flaechenfarbe, light_common.c
  Stufe 3+4 `byte_out = sat_u8(IR_c >> 5)`). Der PC-Zeichner bildet die Primitivfarbe mit x0xFF/0x80 auf SDL ab und kappt bei
  0xFF (render_pc.c `psx_prim_to_sdl_vert`: ueber 0x80 bleibt Texel x 1,0). Ab Licht 256 wird der Schluessel also nicht mehr
  heller. Die erste Neigung (rx 608 / rz -160) hatte Licht 211,5. Die Blink-Spitze 211,5 x 0xBE/0x80 = 314 wurde auf 256 gekappt
  (x1,21). Das ergab den gemessenen Puls 92 -> 112 mit einem Plateau ueber 34 von 60 Bildern.
- **Beleg:** Blink-Folge TITLE.BIN FUN_801028ec (`sltiu v0,v1,0x1f` @0x801028fc, `addiu a0,a0,2` @0x8010290c, `-2` @0x80102910,
  Reset `0x3c` @0x80102918-28; re15_blink_r41.h), Kappung render_pc.c psx_prim_to_sdl_vert.
- **Aenderung:** leiche_lage.py waehlt die Neigung jetzt unter der Grenze Licht x 0xBE / 0x80 <= 255 in jedem Kanal (Raster 16):
  rx 544 / rz -464 (59,4 Grad), cos 0,917 (Bildgroesse wie vorher, ~10,4 x 9,5), Licht 169,8 -> Spitze 252.
  GSZ_KEY_AUF_RX/RZ in gatorszene_r41.c.
- **Messung nachher:** unit_r41_gatorszene_leiche `Cut 1: Primitivfarbe Ruhe 84 -> Blink-Spitze 125 (<= 0x80)`; Gegenprobe
  erste Neigung `Ruhe 104 -> Spitze 154 > 0x80`. exe L5 (Leon) und E5 (Elza), Ankunft Cut 1, hellster Pixel im Schluessel-
  Ausschnitt (112..146 x 112..146): F60 72, F66 79, F72 87, F78 94, F84 101, F90 108, F96 103, F102 95, F108 88, F114 81, F120 72.
  Das sind min 72, max 108 (x1,50), 3 von 61 Bildern am Hoechstwert, ein Dreieck ohne Plateau (vorher 92 -> 112, x1,22,
  Plateau 34/60). Folge: In Ruhe ist der Schluessel etwas dunkler (72 statt 92), weil die Kappung oben Platz fuer den vollen Puls
  verlangt. Bild `F_belege/n1_m5_puls_ankunft_zoom.png`.

### N1.M6 RE-Gate

- `s_ga.t > 60` ersetzt durch `GSZ_WENDE_MAX = ceil(0x800 / 75) = 28` (PORT-WAHL der Notbremse, aus den Konstanten hergeleitet):
  Rate 75 `addiu a2,zero,75` @0x80102608, groesste Wende 0x800 (Halbkreis-Weiche `< 0x801` in FUN_8001569c). Die Wende ist jetzt
  FUN_8001569c exakt (`ga_drehen`, RE2_Quellcode_V2/FUN_8001569c.c: Einrasten bei `(rate + ziel - gier) & 0xfff < 2*rate`,
  Richtung bei `< 0x801`). Die Szene nennt sie als Nutzer-Inszenierung; RE2 R4 @0x80102bd0 selbst dreht nicht.
- Clip-11-Zitat berichtigt (Code und Dossier §1.2): `addiu a1,zero,11` @0x801011d8, `jal 0x8001a240` @0x801011e0. FUN_8001a240
  setzt Clip und Blend (@0x8001a258-60) und schaltet das Bild (FUN_8002959c @0x8001a28c). Den Wurzelschritt von FUN_8001a330
  (FUN_80015e7c @0x8001a374, FUN_800152c8 @0x8001a3c0) hat es nicht. Die Lage setzt vorher FUN_80015cb8 @0x801011cc = ga_anker_lage.

### N1 Tests

Neu/erweitert: unit_r41_gatorszene_tod (neu, M2), _leiche (M5), _tor (M3), _szene/_szene_elza (M1);
integration_r41_gatorszene_tod / _tod_elza (neu, M2: zwei exe-Laeufe mit/ohne Schluessel + Auswerter `test_r41_gatorszene
bilddiff`), _leon / _elza (M1: Folge C 0, Kamera-Uebergabe-Zeile, state.log F405-F479 cam=0).
Gezielt (keine volle Suite), Regex wie Abnahme 0 plus `integration_r41_re2gator$` (scratchpad `F41n1/final_regex.txt`):
`ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|integration_r41_re2gator$|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_|integration_r39_inventar_discard$|unit_item_|unit_inv_|unit_sca_wall_hit$|unit_re2_spider_|unit_spider_ai$|unit_adult_spider_ai$|unit_r38_spinnen$|integration_r38_spinnen_echtlauf$|unit_writher_ai$|unit_enemy_shootable$|unit_collision_layers$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|integration_r36_schluessel|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|integration_r37_feuerloescher$|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_|unit_door_seq$|unit_r38_wasser_aotbreite$|unit_r41_kleinkram_a$|integration_r41_kleinkram_a_blink$|r31_trefferboxen|unit_rotor_bgm_pin$|unit_r35_cut10f0_bgm$|unit_r39_johnruf1041_musik$|unit_r35_cut1150_knallbank$|unit_r36_szene1150_knall_re2$|integration_r39_elzazombies_knall$|unit_r36_inventar$|unit_r36_kartenitems$|unit_r37_itemband$|unit_r36_tueren_|unit_r33_tueren_|unit_r30_tuer|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$|prop_spawn_entered$)" -j3 --timeout 900 --output-on-failure`
-> `100% tests passed, 0 tests failed out of 133` (Total Test time (real) = 657.29 sec).
Nach der letzten Code-Aenderung (key_suche: Reihenfolge der Pruefungen nach Kosten, Ergebnis gleich; r_nah aus Knochen 6/0)
erneut die betroffene Gruppe: `ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|integration_r41_re2gator$)" -j3 --timeout 900 -V`
-> `100% tests passed, 0 tests failed out of 14` (Total Test time (real) = 242.85 sec; tod/tod_elza wieder 60/60 Bilder,
75 -> 111, gleiche Lage (-1300,-1729,-24000)).
Nach dem Orchestrator-Entscheid F-E1 (AUFTRAG.md 2026-10-25 02:51, waehrend dieser Nachbesserung in den Zweig gelegt) die drei
angepassten Fremd-Riegel erneut: `ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r36_tueren_schott|unit_r38_spinnen|
integration_r41_re2gator)$" -j3 --timeout 1000 -V` -> `100% tests passed, 0 tests failed out of 3` (Total Test time (real) = 293.18 sec).

### N1 OFFEN

1. Sichtbarkeit nach dem Tod: 4-6 pulsierende Pixel (Modell und Kameraabstand, s. M2). Ein anderes Bodenmodell waere eine
   Nutzer-Entscheidung.
2. Abgleich mit Spur G (unveraendert aus §O 2): 2080-Lagen gleich halten. Neu: Spur B's Kamera-Programme laufen nach der Szene
   wieder (N1). Bei der Integration mit G die Kanister-/Explosionsschnitte nach der Ankunftsszene messen.
3. Unveraendert aus §O: Musik nach dem Tod (MAIN2E statt RE2-Ausblenden), Nachrichten-Ids 44-46 frei, PSX-Bau ungeprueft,
   Schachtspinnen 20A0.
4. §O 1 (Spur-B-Riegel) und §O 3 (flacher Schluessel) sind mit dieser Nachbesserung erledigt (M4, M2).
5. Ersatzlage nach dem Laden eines Spielstands in einem frischen Prozess (Todeslage nicht im RAM): Lauf P3/P3x (Bits 110+111,
   RE15_KEIN_GATOR=1, Sprung-Cut 0): Neigung im ersten Takt zur Kamera 0 (rx -320 rz 576), 31 von 31 Bildern mit Unterschied,
   aber nur eine Pixelspalte (127,102)-(127,105), 32 -> 63 - klein. Nicht geprueft (nicht F's Modul): ob der Alligator nach so
   einem Laden wieder lebend gesetzt wird.
6. PSX-Laufzeit von key_suche (einmal beim Tod; Rasterpruefung nur im Ring um die Leiche) ungemessen - PSX-Bau ungeprueft.

### N1 Fuer den Nutzer

- Keine neuen Assets und keine Sprachdateien in dieser Nachbesserung.
- Ablauf: Nach dem Schrei des Alligators zeigt die Kamera wieder den Leiter-Eingang mit Leon bzw. Elza (Cut 0). Der Kampf
  beginnt im Bild.
- Nach dem Tod des Alligators liegt der Water Key im Wasser vor der Leiche, im Bild der Kamera, in der man gerade steht, und
  zur Kamera gekippt. Er pulsiert alle 2 s. Er ist klein: das RE2-Bodenmodell "Platform Key" ist ein duenner Ring, aus den
  Beckenkameras sind das 4-6 Pixel.
- **Frage:** Soll der Schluessel nach dem Tod groesser bzw. als massiveres Modell erscheinen (z. B. ein anderes RE2-Schluessel-
  modell oder ein vergroessertes Modell)? Oder soll die Kamera nach dem Tod kurz auf den Schluessel schneiden?

## Nachbesserung 2 (nach Abnahme 1, F_abnahme_1.md: M1, M2)

Start 2026-10-25, HEAD a9472f02, Baum sauber. Messordner (scratchpad, nicht im Repo): `F41n2/` (`runs/<lauf>/...`, eigene
exe-Kopie je Lauf `re15_pc_f41n2_<lauf>.exe`, timeout, danach geloescht). Alle Laeufe RE15_WINDOW_SCALE=1, Bilder nur per
RE15_FRAMEDUMP.

Plan:
- N2.M1 Drei Fremd-Riegel mit belegtem Querbezug nachziehen (Muster F-E1): integration_r38_wasser (leon2090),
  integration_r37_itemband (weg2090), integration_r41_wasser20b0_npc (landung_leon/landung_elza); je Ursache, Messung vorher,
  Aenderung, gezielt gruen.
- N2.M2 Water Key nach dem Tod nur 4-6 Pixel: Form im Original/RE2 suchen (wie zeigen die Spiele einen Gegenstand, der erst
  nach einem Kampf aufnehmbar wird), umsetzen, neu messen (Differenzbild, Kasten, Puls, R1 mit natuerlicher Kamera, Leon + Elza).

### N2.M1 Drei Fremd-Riegel durch F rot (Muster F-E1)

- **Ursache.** Alle drei Laeufe stellen einen Zustand VOR den Nutzer-Vorgaben der Runde 41 nach: (1) `integration_r38_wasser`
  Teil leon2090 springt ohne Bit 110 nach 2090 -> das erste Betreten startet die Ankunftsszene (Nutzer-Punkt "neue Cutscene"),
  Programm 1 setzt den Riegel Set(2,7,1)/Set(1,27,1) (ROOM1150 sub08 @0x01114/@0x01118) und haelt Leon; (2)
  `integration_r37_itemband` Teil weg2090 und (3) `integration_r41_wasser20b0_npc` Teile landung_leon/landung_elza gehen durch
  das Tor 20A0/20A1 slot 0 @0x01746 nach 2090/2091, das seit Nachtrag 21 bis Bit (9,113) verschlossen ist (sce-3-Ereignis 20,
  "It's locked.").
- **Messung vorher** (exe am Stand a9472f02, eigene Kopien, scratchpad `F41n2/runs`):
  | Lauf | Umgebung wie der Riegel | Ergebnis |
  |---|---|---|
  | W0 | leon2090 (RE15_DEBUG_JUMP=2090@250, Lage (-8438,-25550), "U1.6,W1" ab Bild 30, EXIT_AT 80) | `Folge A ROOM2090 F250`, `Folge P ROOM2090 F250 wert=1/0`; state.log F30.. `pad=0010 PL(-8438,-25550) ... pm=2` (Leon steht trotz Taste), kein wasser.log |
  | I0 | weg2090 (Wegplan des Riegels, RE15_DEBUG_JUMP=20A0@250, Lage (4950,4100) Blick 3072) | `ROOM20A0 Slot 0 (Tor) gesperrt: Ereignis 20, Bit (9,113) = 0`, `Folge Z ROOM20A0 F84`, kein DOOR FIRE, Zeitgrenze |
  | L0 | landung_leon (Eingabeskript des Riegels, RE15_DEBUG_JUMP=20a0@240) | `ROOM20A0 Slot 0 (Tor) gesperrt`, `Folge Z ROOM20A0 F27/F215/F245`, 0 Landungen, 0 Zeilen id=6 sub=3 |
- **Beleg / Querbezug** ("neue Regel widerlegt alten Riegel", Entscheid F-E1, Memory reai-v2-gezielte-integration-querbezug): Die
  Laeufe messen Wasser-Schritte, die Item-Band-Regel und die Landung, nicht die Szene/das Tor. Der Zustand, in dem ihr Weg im
  Spiel besteht, ist "Szene gesehen" (Bit 110) bzw. "Szene gesehen + Tor aufgeschlossen" (Bits 110 + 113, VERTRAG 1.1) - das
  Tor oeffnet erst mit dem Schluessel nach dem Tod des Alligators, die Szene laeuft nur beim ersten Betreten.
- **Aenderung** (nur Messhaken RE15_SET_FLAG, Kommentar mit Nutzer-Wortlaut im Riegel):
  `tests/integration/test_r38_wasser.cmake` (r38n_exe nimmt weitere Umgebung, leon2090 `RE15_SET_FLAG=9:110`),
  `tests/integration/test_r37_itemband.cmake` (weg2090 `RE15_SET_FLAG=9:110,9:113`),
  `tests/integration/test_r41_wasser20b0_npc.cmake` (landung_leon/landung_elza `RE15_SET_FLAG=9:110,9:113`).
- **Messung nachher** (dieselben Laeufe mit den Bits): W1 `[setflag] flag(9,110/0x6e) = 1`, wasser.log F38/F53/F68
  `room=2090 ... stempel=-1620 ... zweig=stempel code=0x06032000 part=6 ... edt=0x1a`, debug.log `[foot] SE foot=7 type=26 vag=3`;
  I1 `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2 spawn=(6300,-1800,-16250)`, `PC loaded room2090.rdt`,
  itemband.txt `F87 room=2090 ITEM slot 5 floor 1 item 0x3F tk 90 spieler-band 1 ... -> FREI`, wegplan.txt FLAG (9,90) 0 -> 1;
  L1 (Leon) / E1 (Elza, `Spieler-Familie PL04`) je DOOR FIRE slot=0, `PC loaded room2090.rdt` bzw. `room2091.rdt`, 1 Landung,
  104 Zeilen id=6 sub=3 (wie Abnahme A 2).
- **Riegel gezielt:** `ctest --test-dir "<baum>/re15_port/build" -R "^(integration_r38_wasser|integration_r37_itemband|integration_r41_wasser20b0_npc)$" -j3 --timeout 1200 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 3` (Total Test time (real) = 858.32 sec; r38_wasser 188.84 s, r41_wasser20b0_npc
  227.65 s, r37_itemband 858.25 s).

### N2.M2 Water Key nach dem Tod nur 4-6 Pixel (Abnahme 1: "als Schluessel nicht erkennbar")

- **Entscheid.** Kein Orchestrator- oder Nutzer-Entscheid lag vor (AUFTRAG.md bis F-E1 02:51, master 645fa45f). Von den drei
  Wegen der offenen Frage (groesseres Modell / massiveres Modell / Kameraschnitt nach dem Tod) ist der Kameraschnitt gebaut,
  als PORT-WAHL mit belegter Form: (a) das Modell bleibt das RE2-Bodenmodell bytegleich (ROOM7040 MD1 @0x354C, keine erfundene
  Groesse), (b) RE1.5 zeigt Gegenstaende genau so: Schnitt auf eine Kamera, Halt, Cut_old + Cut_auto (ROOM2010 sub03 /
  ROOM1180 sub02, s. Belege), (c) der Nutzer hat dieselbe Inszenierung fuer denselben Schluessel vorgegeben (Nachtrag 6 "sieht
  die Zombie Leiche mit dem Schluessel in der Cutscene"), (d) RE2-artige Kameraschnitte sind Nutzer-Vorgabe (Nachtrag 4). Ein
  groesseres Modell braeuchte eine Zahl ohne Beleg; das RE1.5-Item-Bild 0x45 (ITPS.ITP Block 0x45) zeigt einen Schluesselbund
  in Normalgroesse.
- **Ursache (gemessen).** Zwei Teile: (1) Die Lage galt der LAUFENDEN Kamera. Alle Zonenkameras von 2090 stehen an den
  Beckenenden auf y -3132 (RID @0x060); die laufende Kamera sieht die Leiche von fern (Abnahme 1: Tiefe 4835 bzw. 5766 = Quadrat
  7 bzw. 6 Bildpunkte, sichtbar 4-6). (2) Die Verdeckungs-Regel "vor JEDEM Knochen - 800" schloss jede Kamera aus, hinter der
  ein Teil der Leiche liegt oder vor der die Leiche naeher als ihr Bildrand liegt. Messzeile "Schluessel-Kandidat" (neu) mit der
  alten Regel, Lauf T1m (Todesrolle): nur Cut 0 (laufende, Tiefe 4835, 7 Bildpunkte), Cut 3 (8989), Cut 6 (9571).
- **Geometrie (Rechnung, scratchpad F41n2/py/kam2090.py mit der Projektion von camera_common.c, H = 26684 >> 7 = 208):** das
  naechste Wasser im Bild liegt je Zonenkamera bei Tiefe 2174-2519 (Quadrat 14,9-17,2 Bildpunkte, am unteren Bildrand); Cut 8-12
  sind schlechter (9,7 / 7,7 / 6,7 / 13,8 / kein Wasser). Mehr als ~17 Bildpunkte gibt keine Kamera von 2090 her.
- **RE-Belege (selbst gelesen):**
  - ROOM2010.RDT @0x008EE `29 05 2b 02 ff ff 02 00 2a 00 3c 01 01 00` = Cut_chg 5, Message_on 2, Evt_next, Nop, Cut_old, Nop,
    Cut_auto 1, Evt_end; ROOM1180.RDT @0x00B3C `29 09 2b 01 ff ff 02 00 2a 00 3c 01 01 00` dieselbe Form (xxd; shared_assets ==
    info/Re1.5, cmp). Zensus aller Leon-RDTs (scd_dump_room.py): 16 Stellen Cut_chg -> Message_on -> Evt_next -> Cut_old (10C0
    sub02, 1150 sub06/07, 1180 sub02/03, 1230 sub22, 2010 sub03, 30A0 sub02, 30B0 sub02, 4010 sub07/08, 4020 sub11, 5010
    sub02/03, 6020 sub02, 6030 sub04).
  - Cut_old FUN_8004032c (re15_disasm.py, info/Re1.5/PSX.EXE): `8004033c lbu a0,16251(a0)` (DAT_800b3f7b = der bei Cut_chg
    gemerkte ANGEZEIGTE Cut), `80040344 lhu v1,4068(v1)` (work_vars[0x0A]), `80040354 sb v0,21591(at)` (= 1), `8004035c sh
    v1,4072(at)` (work_vars[0x0C]), `80040364 sh a0,4068(at)` (work_vars[0x0A] = gemerkter Cut), `80040368 jal 0x800142f4`,
    `80040374 lw v0,-13764(v0)` / `80040378 addiu v1,zero,-257` / `8004037c and` / `80040384 sw` (0x800aca3c &= ~0x100 = Zonen-
    kamera an), `80040390 addiu v1,v1,1` (1 Byte). Port scd_vm.c op_cut_old setzt dasselbe.
  - Cut_chg-Zwilling LAB_800402a0 (`lbu a1,4068(a1)` @0x800402c0, `sb a1,16251(at)` @0x800402e4, `sh a2,4072(at)` @0x800402ec,
    `sh a0,4068(at)` @0x800402fc, `ori v0,v0,0x100` @0x800402d4) - wie die Kamera-Uebergabe (Nachbesserung 1, M1).
  - Riegel Set(2,7,1)/Set(1,27,1) ROOM1150 sub08 @0x01114/@0x01118, Plc_ret @0x012E0, Riegel auf @0x012E4/@0x012E8 (wie P3).
  - Halt = GSZ_LEICHE_BILDER 90 (1,5 Blinkperioden 0x3c, TITLE.BIN FUN_801028ec `ori v0,zero,0x3c` @0x80102918) = die
    Leichen-Einstellung der Ankunft.
  - Verdeckung: Alligator-Kasten {1000,-720,0,2200,720,800} @0x80118b98 (STAGE2.BIN, halbe Breite 800 = Kugelradius je Knochen).
  - Der Tod kommt im Spielschritt (Spur B R1 P3: Tot-Flag @0x80102654-64, Kamera frei @0x80102688-94); der SCD-Takt laeuft
    VOR dem Spielschritt (main.c) -> Erkennung im Nachtakt-Haken (enemy_ai_boss_gator.c ruft re15_gatorszene_r41_nachtakt nach
    re15_gr2_nachtakt), damit der Schnitt im selben Bild wie der Rueckschnitt der Spur B kommt (kein Zwischenbild) - [berichtigt in
    Nachbesserung 3: gilt nur fuer "Sperre ohne Schnitt"; mit Explosionsschnitt kam B's Rueckschnitt zwei Takte VOR dem Tot-Flag, N3.M1].
- **Aenderung** (engine/src/gatorszene_r41.c, include/re15_gatorszene_r41.h; keine gemeinsame Datei):
  - key_lage_bei_tod: die Zonenkamera (RVD-Gruppe mit Wechselsatz, Cut 0..7) mit der kleinsten Tiefe (groesstes Bild), bei
    Gleichstand die laufende; Messzeile je Kandidat.
  - leiche_verdeckt: je Knochen eine Kugel 800; verdeckt, wenn sie nicht ganz hinter dem Schluessel liegt und ihr Bildkreis
    (800 x H / Tiefe) den Schluessel (+ halbe Quadrat-Bildgroesse) trifft; Kugel durch die Bildebene = verdeckt.
  - key_tod_pruefen (Nachtakt im Bild des Todes, Rueckfall SCD-Takt) -> Lage, (9,111), Schluessel + Zone Slot 15, dann
    key_einstellung: Cut_chg-Zwilling auf die Schluessel-Kamera + Programm 4 (`22 02 07 01 22 01 1b 01 09 0a 5a 00 2a 00 2e 01
    00 00 42 00 3c 01 22 02 07 00 22 01 1b 00 01 00`); key_einstellung_takt meldet das Ende. Zustand RE15_GSZ_SCHLUESSEL nur
    fuer re15_gatorszene_r41_ereignis. Pruefhaken re15_gatorszene_r41_key_einstellung. Folge-Zeilen E (Schnitt) / O (zurueck).
  - Die Einstellung kommt auch im Messzwilling mit (9,112) vorab (gleiche Kamera, gleiche Bilder; nur der Schluessel fehlt) -
    im Spiel gibt es (9,112) vor (9,111) nicht.
- **Messung nachher** (eigene exe-Kopien, Zwilling mit (9,112) vorab, Bild fuer Bild, `F41n2/tools/puls_diff.py`):

  | Lauf | Weg | Kamera (laufende) | Lage / Tiefe / Abstand | Einstellung | Bilder mit Schluessel | Kasten | hellster Pixel |
  |---|---|---|---|---|---|---|---|
  | T2m/T2o Leon | Todesrolle GB_TEST=3, Bit 110 | Cut 1 (0) | (-5200,-1689,-26800), 2441 (Quadrat 15,4), 1698 von der Leiche | F469-F561; state.log cam=1 F469-F559, pm=2 F470-F559, cam=0 ab F560 | 46 / 46 (F470-F560), je 37 Pixel | (286,195)-(302,206) = 17 x 12 | 72 -> 106 (x1,47), 2 Bilder am Max |
  | TEm/TEo Elza (2091) | dto. | Cut 1 (0) | dto. | F469-F561 | 44 / 44 | (286,195)-(302,206) | 72 -> 106 (x1,47) |
  | K4m/K4o Leon | R1-Kanistertod, natuerliche Kamera (Rezept K3: (-1000,-25550) Blick Ost, `U1.2,(M0.3,MA0.1)x12`, GB_TEST=5, Kanister (4430,-26160)) | Cut 2 (1) | (1400,-1704,-23300), 2643 (Quadrat 14,1), 1001 | F84-F176 (`R1 frei ... Cut 1 F84` im selben Bild) | 45 / 45 (F86-F174), je 29 Pixel | (32,192)-(44,206) = 13 x 15 | 70 -> 102 (x1,46) |
  | KEm/KEo Elza (2091) | dto. | Cut 2 (1) | dto. | F84-F176 | 44 / 44 | (32,192)-(44,206) | 70 -> 102 (x1,46) |

  Vorher (Abnahme 1): 4 Pixel (175,124)-(178,129) bzw. 6 Pixel (257,124)-(262,127). Bilder `F_belege/n2_m2_todesrolle_
  einstellung.png`, `n2_m2_todesrolle_zoom.png` (6-fach: Ring mit Bart in F500/F530/F554, im Zwilling nichts),
  `n2_m2_r1_einstellung.png`, `n2_m2_r1_zoom.png`, `n2_m2_elza_einstellung.png`, `n2_m2_nach_einstellung.png`.
- **Nach der Einstellung** (gemessen, `n2_m2_nach_einstellung.png`): R1 - in der laufenden Cut 1 weiter sichtbar, Kasten
  (164,105)-(166,106), 151 / 151 Bilder, 70 -> 102. Todesrolle - in Cut 0 0 Pixel (F564-F660): der Schluessel ist zur
  Einstellungs-Kamera Cut 1 (Westende) gekippt und zeigt Cut 0 (Ostende) die Rueckseite; Objekt-Flaechen mit NCLIP < 0 verwirft
  das Original (re15_nclip_r38.h: FUN_800254a0 @0x800255a8 `bgez`, FUN_800256b0 @0x800257c0). Die Aufnahme-Zone (1000 x 1000,
  Slot 15) liegt an der gezeigten Stelle.

### N2 Tests

Geaendert/neu (nur F-eigene Riegel + die drei Fremd-Riegel von N2.M1, je mit Querbezug):
- `unit_r41_gatorszene_tod`: Kamera = Zonenkamera (>= 2 RVD-Saetze), Quadrat >= Wert der Nachbesserung 1 in der laufenden
  Kamera (Todesrolle 15,4 >= 9,7; R1 7,5 >= 7,5 mit Ersatz-Pose), nicht verdeckt (Knochen-Kugeln 800), Einstellung: im Bild des
  Todes Cut K + Zonenkamera aus + Riegel (2,7)/(1,27), nach 91 Bildern Cut_old auf den Cut davor, Zonenkamera an, Riegel auf,
  Folge E/O.
- `unit_r41_gatorszene_schluessel`: Mitte hoechstens halbe Diagonale (127) ueber dem Wasser; Aufnahme erst nach dem Ende der
  Einstellung (Pad-Sperre).
- `integration_r41_gatorszene_tod` / `_tod_elza` (Todesrolle) und NEU `_tod_r1` / `_tod_r1_elza` (R1-Kanistertod, natuerliche
  Kamera): Lage-Zeile "groesstes Bild", Quadrat >= 10,4 Bildpunkte (Ankunfts-Schluessel), Einstellung in beiden Zwillingen
  gleich und >= 90 Bilder, state.log cam=K in der Einstellung und danach wieder der Cut davor (nur der letzte Raum-Abschnitt),
  bilddiff in der Einstellung mit Kasten >= 10 x 9 (Auswerter `test_r41_gatorszene bilddiff ... <min_breite> <min_hoehe>`).
- Fremd: `integration_r38_wasser` (leon2090 9:110), `integration_r37_itemband` (weg2090 9:110,9:113),
  `integration_r41_wasser20b0_npc` (landung_leon/_elza 9:110,9:113) - s. N2.M1.

Gezielt (keine volle Suite):
- F allein: `ctest --test-dir "<baum>/re15_port/build" -R "^integration_r41_gatorszene_tod" -j4 --timeout 900 -V`
  -> `100% tests passed, 0 tests failed out of 4` (Total Test time (real) = 118.61 sec; tod/tod_elza Quadrat 153/10, Kasten 17 x 12,
  72 -> 106; tod_r1/tod_r1_elza Quadrat 141/10, Kasten 13 x 15, 70 -> 102).
- Gesamt-Regex (F + B + alle Riegel der Abnahme 1 (Kamera/Zonen, Wasser, Tueren, 2090/2091-Besucher) + Fremd-Riegel N2.M1,
  scratchpad `F41n2/final_regex.txt`):
  `ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|integration_r41_re2gator$|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_|integration_r39_inventar_discard$|unit_item_|unit_inv_|unit_sca_wall_hit$|unit_re2_spider_|unit_spider_ai$|unit_adult_spider_ai$|unit_r38_spinnen$|integration_r38_spinnen_echtlauf$|unit_writher_ai$|unit_enemy_shootable$|unit_collision_layers$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|integration_r36_schluessel|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|integration_r37_feuerloescher$|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_|unit_door_seq$|unit_r38_wasser_|integration_r38_wasser$|unit_r41_kleinkram_a$|integration_r41_kleinkram_a_blink$|r31_trefferboxen|unit_rotor_bgm_pin$|unit_r35_cut10f0_bgm$|unit_r39_johnruf1041_musik$|unit_r35_cut1150_knallbank$|unit_r36_szene1150_knall_re2$|integration_r39_elzazombies_knall$|unit_r36_inventar$|unit_r36_kartenitems$|unit_r37_itemband$|integration_r37_itemband$|unit_r36_tueren_|unit_r33_tueren_|unit_r30_tuer|unit_r31_tuer_ton$|unit_r35_cut10f0_tuerton$|unit_r39_karten_tueren$|integration_r36_tueren$|unit_camera_view$|unit_cut_old_displayed$|unit_r34n_b_cut_old$|prop_zone_dispatch$|prop_scd_pc_advance$|unit_r38_montage_kameras$|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$|prop_spawn_entered$|integration_r35_granate$|integration_r41_wasser20b0_npc$|integration_r38_wasserzombie$|unit_r35_granate$|unit_irons_mittelmodell$|unit_r34_reaktion$|unit_1210_arme$|unit_r30_irons_tisch_licht$|unit_r35_affen_|unit_r36_szenenleon_|unit_r36_ziel1190$|unit_r37_pri1100_|unit_r38_npckante_|unit_r38_wasserzombie$|unit_r41_wasser20b0_)" -j4 --timeout 1200 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 213` (Total Test time (real) = 958.10 sec; darin integration_r37_itemband 840.53 s,
  integration_r41_re2gator 301.78 s, integration_r41_wasser20b0_npc 215.33 s, integration_r35_granate 169.30 s,
  integration_r38_wasser 166.37 s, integration_r38_wasserzombie 142.79 s, integration_r41_gatorszene_leon/elza 49.70/66.76 s,
  _tod/_tod_elza 65.68/98.52 s, _tod_r1/_tod_r1_elza 44.01/76.30 s).

### N2 OFFEN

1. **Nach der Einstellung (Todesrolle):** in der laufenden Kamera Cut 0 ist der Schluessel nicht zu sehen (0 Pixel F564-F660):
   er ist zur Einstellungs-Kamera Cut 1 gekippt und zeigt Cut 0 die Rueckseite (NCLIP-Verwurf wie das Original, re15_nclip_r38.h).
   Beim R1-Tod bleibt er in Cut 1 sichtbar (3 x 2 Bildpunkte). Gezeigt und erkennbar ist er in der Einstellung; die Aufnahme-Zone
   liegt an der gezeigten Stelle. Ein von beiden Seiten sichtbarer Schluessel braeuchte eine zweite, gespiegelte Flaeche, die das
   RE2-Modell nicht hat - nicht gebaut.
2. **Leiche fern von allen Kameras:** stirbt der Alligator mitten im Becken (z. B. Westkanal z -17000), bleibt auch die beste
   Zonenkamera bei ~6 Bildpunkten (Rechnung kam2090.py: Cut 2 Tiefe 6053). Gemessene R1-Enden liegen an den Beckenenden
   ((3413,-25001) Spur B/Abnahme, (2619,-8209) und (-2856,-6884) Spur G); die Mitte ist nicht gemessen.
3. **Integration mit Spur G:** nach dem R1-Tod folgt jetzt die Einstellung (90 Bilder, Riegel). G's Kampf-Riegel
   (integration_r41_kanister2080_kampf*) pruefen nach dem Tod u. a. "KAMERA R1 zurueck -> Cut 0" und "R1 ende (Leiche)" - beide
   Zeilen kommen weiter (der Schnitt folgt im selben Bild danach); Pruefungen G's auf cam/Spielersteuerung NACH dem Tod bei der
   Integration nachmessen. Die 2080-Lagen weiter gleich halten (re15_k2080_zombie_lage).
4. Unveraendert aus N1: Musik nach dem Tod (MAIN2E statt RE2-Ausblenden), Nachrichten-Ids 44-46 frei, Schachtspinnen 20A0,
   Ersatzlage nach dem Laden eines Spielstands (keine Einstellung, Neigung im ersten Takt), PSX-Bau ungeprueft (key_suche laeuft
   beim Tod jetzt fuer alle acht Zonenkameras statt fuer eine - PSX-Laufzeit ungemessen).

### N2 Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien.
- Nach dem Tod des Alligators schneidet das Bild fuer 1,5 Sekunden (90 Bilder) auf die Kamera, in der der Water Key neben der
  Leiche am groessten liegt (Balken oben und unten wie in der Ankunftsszene, Leon/Elza stehen still). Der Schluessel blinkt dort als
  Ring mit Bart, danach geht die Kamera zurueck. Aufheben an der gezeigten Stelle (Aktionstaste).
- Am Tor bleibt alles wie in Nachbesserung 1.
- Frage (Beobachtung): Ist der Water Key in dieser Einstellung nach dem Tod fuer dich gut zu erkennen?

## Nachbesserung 3 (nach Abnahme 2, F_abnahme_2.md: M1)

Start 2026-10-25, HEAD d5321663, Baum sauber. Messordner (scratchpad, nicht im Repo): `F41n3/` (`runs/<lauf>/...`, eigene
exe-Kopie je Lauf `re15_pc_f41n3_<lauf>.exe`, timeout, danach geloescht). Alle Laeufe RE15_WINDOW_SCALE=1, Bilder nur per
RE15_FRAMEDUMP.

Plan:
- N3.M1 Zwischenbild der Zonenkamera zwischen Explosionsschnitt (Spur B, R1 P0) und Schluessel-Einstellung: Messung vorher
  (Rezept KW/KWE der Abnahme: Leiter-Spawn, natuerliche Cut 0, RE15_GB_TEST=5, Kanister (4430,-26160), Schuesse ab F151),
  RE2-R1 P1-Ende/P2/P3 selbst disassemblieren (EM23_OVL_0000.BIN @0x80102488-0x801026a0), Schnitt auf den Schluessel dort,
  wo B auf die Zonenkamera zurueckschneiden wuerde, Messung nachher Bild fuer Bild (Leon + Elza), Riegel fuer den Fall mit
  Explosionsschnitt (state.log + Bildklassen je Bild).

### N3.M1 Zwischenbild der Zonenkamera zwischen Explosionsschnitt und Schluessel-Einstellung (Leon und Elza)

- **Ursache.** Spur B's R1 (gator_re2_r41.c re15_gr2_r1_tick) bildet RE2 ab: P0 schneidet auf die Explosions-Kamera E, wenn der
  laufende Cut den Kopf nicht zeigt (r1_geschnitten), P1 laeuft Clip 9 bis zum letzten Bild und schneidet dann auf den Ausgangs-Cut
  X zurueck (gr2_kam_zurueck), P2 ist ein Takt, P3 setzt das Tot-Flag und gibt die Sperre frei (gr2_kam_frei). F schnitt erst mit
  dem Tot-Flag (key_tod_pruefen -> gator_tot -> re15_gr2_tot), zwei Takte nach B's Rueckschnitt - X stand zwei Bilder lang da. Die
  Behauptung "kein Zwischenbild" der Nachbesserung 2 galt nur fuer "Sperre ohne Schnitt" (Lauf K4m, Riegel _tod_r1).
- **Messung vorher** (exe am Stand d5321663 = 5c99bba8 + Doku, `ninja: no work to do`; Laeufe V_KW Leon / V_KWE Elza mit dem Rezept
  KW/KWE der Abnahme: `RE15_DEBUG_JUMP=2090@250` (2091 + `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500`),
  `RE15_SET_FLAG=9:110 RE15_R39_MESS_SPIELER_HP=100 RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W5,(M0.3,MA0.1 zwoelfmal ausgeschrieben)
  RE15_EXIT_AT=330 RE15_FRAMEDUMP=150-330/1`):
  - gator_boss.log: `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160 clip=9 bild=0`, `KAMERA R1 zurueck -> Cut 0 F196 clip=9 bild=36`,
    `R1 ende (Leiche) pos=(3413,-25001)`, `R1 VSync 2 F198`, `KAMERA R1 frei (Zonenkamera an) Cut 0 F198`; debug.log `Schluessel-
    Einstellung: Cut 2 (vorher Cut 0) F198`, `... Ende: Cut 0 F290`; state.log cam=1 F160-F195, **cam=0 F196-F197**, cam=2 F198-F288,
    cam=0 ab F289 - Leon (`Spieler-Familie PL00`) und Elza (`PL04`) gleich. Gator: F195 (3431,-24952) af 36, F196-F198 (3413,-25001)
    af 36 = Endpose schon im P1-Ende-Takt (P2/P3 bewegen nichts).
  - Bilder (scratchpad `F41n3/tools/kamklasse.py`: mittlere Abweichung je Kanal zu drei Referenzbildern desselben Laufs, E = fd 185,
    K = fd 250, X = fd 310): fd 186-196 E (zu E 6,6-9,6 / zu X 11,0-15,8), **fd 197 X (X 3,31 / E 14,34 / K 11,64), fd 198 X (0,19)**,
    fd 199-205 K (0,0-2,43) -> Folge `E+X+K+`. Die Zahl der Pixel mit |Delta| > 40 zum Vorbild liegt INNERHALB der Explosions-
    Einstellung bei bis zu 5420 (Effekte), beim Schnitt E -> X bei 2682 - eine Schwelle auf Wechsel-Pixel trennt Schnitte also nicht;
    darum klassifiziert der Riegel je Bild ueber Referenzbilder (keine Schwelle).
- **RE-Belege (selbst disassembliert, `re2_disasm.py --bin EM23_OVL_0000.BIN`, RE2 PSX.EXE):**
  - P0 (zum Vergleich, B's Zitat nachgelesen): `8010220c ori v0,v0,0x100` / `80102214 sw v0,-1064(at)` (0x800cfbd8 Sperre), `80102218-24` laufender Cut (0x800d4820) 6 oder 13, `80102234 slti v0,v0,-13300` (z), `80102248 ori v0,v0,0x40` / `8010224c sh v0,536(s1)` (+0x218 |= 0x40 = "geschnitten"), `80102268 jal 0x8002c7ac` mit `8010226c addiu a0,zero,5` (Cut 5).
  - P1 Clip-9-Ende: `80102488 addu a0,s1,zero / 8010248c addiu a1,zero,9 / 80102494 jal 0x8001a330`, `8010249c beq v0,zero,0x8010252c`
    (nicht am Ende -> weiter mit Clip 9).
  - Rueckschnitt am Ende: `801024a4 lhu v1,536(s1)` (+0x218), `801024ac andi v0,v1,0x40`, `801024b0 beq v0,zero,0x8010251c` (ohne
    P0-Schnitt kein Rueckschnitt), `801024b4 andi v0,v1,0xffbf` / `801024c0 sh v0,536(s1)` (0x40 loeschen), `801024c4-d0` 0x800cfbd8 &
    0x40000000, `801024d8 jal 0x80077360` (a1 16), `801024e4/e8` a0 = 13 bzw. 6, **`801024ec jal 0x8002c7ac`** = Cut setzen (RE2 PSX.EXE
    `8002c7b8 sb a0,-1038(at)` = 0x800cfbf2, `8002c7bc jal 0x8002c7dc` RVD-Satz des Cuts -> `8002c7c8 sw v0,-7364(at)` 0x800ce33c);
    `801024f4-80102518` 0x800cfbf8 &= 0xfff7fbff, 0x800dfc1b = 1; `8010251c-28` +0x6 += 1.
  - P2: `80102638 lbu v1,6(s1) / 8010263c lhu v0,536(s1) / 80102640 addiu v1,v1,1 / 80102644 andi v0,v0,0xfffb / 80102648 sh v0,536(s1)
    / 8010264c j 0x80102698 / 80102650 sb v1,6(s1)`.
  - P3: `80102654 lhu v0,270(s1) / 80102658 addiu a1,zero,30 / 8010265c ori v0,v0,0x4000 / 80102660 jal 0x8007730c / 80102664 sh
    v0,270(s1)` (Tot-Flag +0x10E, Raumflag 30), `80102668 addiu v0,zero,2` / `8010267c sb v0,-998(at)` (VSync 2), **`80102684 lw
    v0,-1064(v0) / 80102688 addiu v1,zero,-257 / 8010268c and v0,v0,v1 / 80102694 sw v0,-1064(at)`** = 0x800cfbd8 &= ~0x100 (Sperre frei).
  - Folgerung: RE2 schneidet am Clip-9-Ende zurueck und haelt die Sperre bis P3; zwischen Rueckschnitt und Tot-Flag liegen genau
    P2 und P3. Den Schluessel hat RE2 nicht (Port-Ergaenzung) - die Einstellung auf ihn steht jetzt an der Stelle des RE2-Rueckschnitts.
  - RE1.5-Form der Einstellung unveraendert (Nachbesserung 2): Cut_chg-Zwilling LAB_800402a0 (`sh a0,4068(at)` @0x800402fc = work_vars
    [0x0A]), Cut_old FUN_8004032c (`lbu a0,16251(a0)` @0x8004033c, Zonenkamera an @0x80040374-84). key_einstellung nimmt vorher =
    work_vars[0x0A] = der von B's Rueckschnitt gesetzte Ausgangs-Cut X -> Cut_old kehrt auf X zurueck wie RE2.
  - ROOM2090/2091-SCD (`scd_dump_room.py`): 0 Cut_chg/Cut_old/Cut_auto - kein SCD-Faden aendert den Cut; die Zonenkamera ist ab P0
    gesperrt -> zwischen P0 und P3 aendert nur B's Rueckschnitt den Cut (Erkennungsmerkmal).
- **Aenderung** (engine/src/gatorszene_r41.c, keine gemeinsame Datei, keine Aenderung in Spur B's Modul):
  - `key_rueckschnitt_pruefen(slot)` im Nachtakt (vor key_tod_pruefen): R1 laeuft (`re15_gr2_r1_laeuft`), B haelt die Kamera
    (`re15_gr2_kamera_gesperrt`) und der Cut ist ein anderer als im ersten R1-Takt (`s_r1_cut`, nach B's P0-Schnitt) = B hat in diesem
    Takt zurueckgeschnitten. Dann, nur bei offenem Schluessel (2090/2091, (9,110), nicht (9,111)): `re15_gr2_kamera_frei(NULL)` (B's
    Programm endet, Cut bleibt X, P3 gibt nichts mehr frei), Folge-Zeile `R E/X`, Logzeile `Schluessel-Schnitt statt R1-Rueckschnitt`,
    dann `key_tod_setzen` (Lage, (9,111), Schluessel + Zone, Einstellung) - im selben Takt, vor dem Zeichnen.
  - `key_tod_pruefen` = `key_tod_offen` + `gator_tot` -> `key_tod_setzen` (unveraendert fuer "Sperre ohne Schnitt" und Todesrolle).
  - PORT-WAHL der Inszenierung (Kommentar im Code): die Explosions-Einstellung behaelt ihre RE2-Laenge (Clip 9 bis zum letzten Bild),
    der Schluessel erscheint zwei Takte vor dem Tot-Flag (die Leiche liegt dann schon in ihrer Endpose).
  - H4 der Abnahme: Zitat "Zonenkamera an" jetzt @0x80040374-84 (statt Funktionsanfang @0x8004032c).
- **Messung nachher** (exe mit 71641acd, `platform\pc\re15_pc.exe` neu gelinkt; Laeufe N_KW Leon / N_KWE Elza mit demselben Rezept,
  Zwillinge N_KWo / N_KWEo mit `RE15_SET_FLAG=9:110,9:112` = ohne Schluessel):
  - gator_boss.log: `KAMERA R1 EXPLOSION RE2-Cut 5 -> Cut 1 F160`, `KAMERA R1 zurueck -> Cut 0 F196 clip=9 bild=36`, **`KAMERA R1 frei
    (Zonenkamera an) Cut 0 F196 clip=-1 bild=-1`** (Programm-Ende durch F), `R1 ende (Leiche) pos=(3413,-25001)`, `R1 VSync 2 F198` -
    keine "KAMERA R1"-Zeile nach F196 (P3 gibt nichts mehr frei). debug.log: `Folge R ROOM2090 F196 wert=1/0`, `Schluessel-Schnitt statt
    R1-Rueckschnitt: Explosions-Cut 1 -> Ausgangs-Cut 0 F196`, `Schluessel-Lage: Kamera 2 (groesstes Bild, laufende 0), Lage
    (1400,-1704,-23300) Tiefe 2643, 1001 von der Leiche (22 Punkte, Naehe <= 3072), Neigung rx 640 rz -128, Primitivfarbe Ruhe 80 /
    Blink-Spitze 118` (= bitgleich zur Lage vorher im Tot-Bild F198), `Schluessel-Einstellung: Cut 2 (vorher Cut 0) F196`, `... Ende:
    Cut 0 F288 (Einstellung Cut 2 ab F196, vorher Cut 0), Zonenkamera an`. Elza (`PL04`, ROOM2091) Zeile fuer Zeile gleich.
  - state.log 2090/2091-Abschnitt: cam=0 F1-F159, cam=1 F160-F195, **cam=2 F196-F286**, cam=0 ab F287 - kein cam=0 zwischen Explosion
    und Schluessel (vorher F196-F197). Leon = Elza; die Zwillinge ohne Schluessel schneiden in denselben Bildern.
  - Bilder (kamklasse.py, Referenzen E = fd 178, K = fd 241, X = fd 303): fd 165-196 E (zu E 3,4-8,1), **fd 197 K (K 2,43 / X 11,52 / E
    12,22)**, fd 198-206 K (0,0-1,5) -> Folge `E+K+` (Leon und Elza). Bildbogen `F_belege/n3_m1_wechsel_leon.png` und
    `n3_m1_wechsel_elza.png` (obere Reihe vorher fd 195-200: Explosion, Explosion, Cut 0 mit der Spielfigur an der Leiter, Cut 0,
    Schluessel, Schluessel; untere Reihe nachher: Explosion, Explosion, Schluessel x 4).
  - Schluessel in der Einstellung (diffserie.py N_KW gegen N_KWo, jede Abweichung zaehlt): fd 197-287 91 / 91 Bilder, je 29 Pixel, Kasten
    (32,192)-(44,206) = 13 x 15, hellster 70 -> 103 (x1,47) - wie der Fall ohne Explosionsschnitt (Abnahme 2 KLm). Elza gleich. Im
    letzten Explosionsbild fd 196 liegen 2 Pixel des Schluessels schon in Cut 1 (164,105)-(166,106): er wird im Takt des Schnitts gelegt,
    angezeigt wird der neue Cut ab dem naechsten Bildanfang (re15_cam_present_tick) - wie im Fall ohne Explosionsschnitt (Abnahme 2: "KL
    F84 schon mit den 2 Schluessel-Pixeln der Cut-1-Lage").
  - Weitere Lagen der Abnahme (Rezepte KN/KQ, Leon, exe mit 9e0347c8): **KN** (Teleport Nord (2000,-9000), Schuesse ab F151):
    `Schluessel-Schnitt statt R1-Rueckschnitt: Explosions-Cut 1 -> Ausgangs-Cut 0 F196`, state.log cam=1 F160-F195, cam=2 F196-F286,
    cam=0 ab F287, Bilder fd 165-206 `E+K+` (vorher Abnahme 2: Cut 1 -> Cut 0 F196/F197 -> Cut 2). **KQ** (Schuesse ab F991, Leiche
    (4528,-12323) Ostbecken-Mitte): Explosions-Cut 3 = Schluessel-Cut 3 (`Schluessel-Lage: Kamera 3 ... Tiefe 2667, 2346 von der Leiche`),
    `Explosions-Cut 3 -> Ausgangs-Cut 0 F1036`, Einstellung `Cut 3 (vorher Cut 0) F1036` bis `Ende: Cut 0 F1128`; state.log cam=3
    durchgehend F1000-F1126, cam=0 ab F1127 (vorher Abnahme 2: Cut 3 -> Cut 0 F1036/F1037 -> Cut 3); Bilder fd 1005-1046: kein Bild der
    Referenz X (Cut 0, Abweichung 12,2-12,7 gegen 0,5-6,0 zu den beiden Cut-3-Referenzen).
- **Nebenbefund (beim Bau des Unit-Riegels gemessen, gleiche Funktion):** Im zweiten Fall desselben Prozesses (ROOM2091 nach dem
  2090-Fall) lief die Einstellung schon im ersten Bild ("Einstellung Cut 2 ab Bild 0"), ohne Rueckschnitt-Ersatz, danach stand Cut 0
  13 Bilder lang. Ursache: `re15_gr2_tot()` fragt ALLE Slots ab (gator_re2_r41.c `for (...) if (s_g[i].tot) return 1`), `s_g` eines
  Slots loescht erst der Boss-INIT des neuen Aktors (`re15_gr2_reset`, enemy_ai_boss_gator.c INIT), und der SCD-Takt-Rueckfall
  `key_tod_pruefen` (re15_gatorszene_r41_tick) laeuft im ersten Bild VOR dem ersten Boss-Takt -> der R1-Tod eines frueheren Alligators
  derselben Sitzung legte den Schluessel sofort. Im Spiel moeglich, wenn in einer Sitzung nach dem Tod ein Zustand davor betreten wird
  (Spielstand geladen / Continue mit (9,110) und ohne (9,111)) - an der exe nicht nachgestellt (RE15_SET_FLAG(_AT) kann (9,111) nicht
  loeschen). Aenderung: der R1-Endzustand zaehlt in `key_tod_pruefen` nur, wenn R1 in DIESEM Raumbesuch lief (`s_r1_lief`, gesetzt in
  key_rueckschnitt_pruefen bei `re15_gr2_r1_laeuft`, geloescht im Installer); die Todesrolle (Zustand 7 des aktuellen Aktors) und
  `musik_soll` bleiben unveraendert. Nachher: der Fall ROOM2091 im selben Prozess verhaelt sich wie der erste (s. Tests).
- **Unit-Riegel nachher** (`test_r41_gatorszene tod_explosion`, Harness mit echtem Spielschritt, Lage KW = test_r41_re2gator Fall (b2)):
  ROOM2090 und ROOM2091 je `angezeigter Cut je Bild ab dem Treffer: 1 x 36, 2 x 90, 0 x 2` - "Folge R 1/0", "Einstellung Cut 2 ab
  Bild 35, Tot-Flag Bild 37", "kein angezeigtes Bild mit Cut 0 vor dem Tot-Flag (0)", "nach dem Tot-Flag haelt die Einstellung
  (Zonenkamera gesperrt 1, B's Programm frei 1)", "nach dem Halt: Cut_old auf Cut 0, Zonenkamera an, (9,111) = 1"; Gegenprobe
  (9,111) vorab: `1 x 36, 0 x 23` - "B schneidet zwei Bilder vor dem Tot-Flag (Bild 37) auf Cut 0 zurueck (2 Bilder Cut 0)", keine
  Folge-Zeile R -> `tod_explosion: 0 Fehler`.

### N3 Tests

Neu (nur F-eigene Riegel, registriert in tests/unit/probes/r41_gatorszene.cmake; keine Fremd-Riegel geaendert):
- `unit_r41_gatorszene_tod_explosion` (test_r41_gatorszene.c Teil tod_explosion): R1 MIT Explosionsschnitt der Spur B in der Lage KW
  (ROOM2090 und ROOM2091 im selben Prozess, dazu Gegenprobe (9,111) vorab) - angezeigter Cut je Bild (re15_cam_present_tick), Folge R,
  Einstellung zwei Takte vor dem Tot-Flag, kein Cut 0 vor dem Tot-Flag, Laeufe E -> K (>= 90) -> Cut_old, Sperre nach dem Tot-Flag,
  Cut_old + Zonenkamera an; Gegenprobe: B's Rueckschnitt unberuehrt (zwei Bilder Cut 0 wie RE2). Der zweite Fall misst zugleich den
  Nebenbefund (frueherer R1-Tod im selben Prozess).
- `integration_r41_gatorszene_tod_explosion` / `_tod_explosion_elza` (test_r41_gatorszene.cmake Teil tod_explosion*): Rezept KW/KWE der
  Abnahme an der exe - Explosionsschnitt erreicht, Rueckschnitt-Ersatz-Zeile, Einstellung ab dem Rueckschnitt-Takt mit Rueckkehr auf
  den Ausgangs-Cut (90..94 Bilder), E/K/X verschieden, keine B-Kamerazeile danach, Spielfigur PL00/PL04, state.log cam=E bis F<r-1>
  und cam=K ab F<r>, Bilder je Bild ueber den neuen Auswerter `test_r41_gatorszene kamfolge` (Klasse = naechstes Referenzbild, keine
  Schwelle) mit Folge E+K+. Gegenprobe des Auswerters an den Bildern des alten Stands (Laeufe V_KW/V_KWE): `Folge
  EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEXXKKKKKKKKKK -> FEHLER`; am neuen Stand (N_KWE) `... X 0, 1 Wechsel ... -> OK`.

Gezielt (keine volle Suite), Regex = die der Abnahme 2 (F + B + Kamera/Zonen + Wasser + Tueren + Inventar + Spinnen + alle Laeufe durch
2090/2091/20A0 + die drei Fremd-Riegel von N2.M1; scratchpad `F41n3/regex.txt` = `F41a2/regex.txt`), sie schliesst die neuen Riegel ueber
`unit_r41_gatorszene_` / `integration_r41_gatorszene_` ein (`ctest -N`: `Total Tests: 216` = 213 + 3):
`ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|integration_r41_re2gator$|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_|integration_r39_inventar_discard$|unit_item_|unit_inv_|unit_sca_wall_hit$|unit_re2_spider_|unit_spider_ai$|unit_adult_spider_ai$|unit_r38_spinnen$|integration_r38_spinnen_echtlauf$|unit_writher_ai$|unit_enemy_shootable$|unit_collision_layers$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|integration_r36_schluessel|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|integration_r37_feuerloescher$|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_|unit_door_seq$|unit_r38_wasser_|integration_r38_wasser$|unit_r41_kleinkram_a$|integration_r41_kleinkram_a_blink$|r31_trefferboxen|unit_rotor_bgm_pin$|unit_r35_cut10f0_bgm$|unit_r39_johnruf1041_musik$|unit_r35_cut1150_knallbank$|unit_r36_szene1150_knall_re2$|integration_r39_elzazombies_knall$|unit_r36_inventar$|unit_r36_kartenitems$|unit_r37_itemband$|integration_r37_itemband$|unit_r36_tueren_|unit_r33_tueren_|unit_r30_tuer|unit_r31_tuer_ton$|unit_r35_cut10f0_tuerton$|unit_r39_karten_tueren$|integration_r36_tueren$|unit_camera_view$|unit_cut_old_displayed$|unit_r34n_b_cut_old$|prop_zone_dispatch$|prop_scd_pc_advance$|unit_r38_montage_kameras$|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$|prop_spawn_entered$|integration_r35_granate$|integration_r41_wasser20b0_npc$|integration_r38_wasserzombie$|unit_r35_granate$|unit_irons_mittelmodell$|unit_r34_reaktion$|unit_1210_arme$|unit_r30_irons_tisch_licht$|unit_r35_affen_|unit_r36_szenenleon_|unit_r36_ziel1190$|unit_r37_pri1100_|unit_r38_npckante_|unit_r38_wasserzombie$|unit_r41_wasser20b0_)" -j4 --timeout 1200 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 216`** (`Total Test time (real) = 1077.76 sec`; darin integration_r37_itemband 837.31 s,
integration_r41_re2gator 268.77 s, integration_r41_wasser20b0_npc 223.17 s, integration_r38_wasser 183.64 s,
integration_r41_gatorszene_leon/_elza 61.93/80.08 s, _tod/_tod_elza 75.29/123.83 s, _tod_r1/_tod_r1_elza 43.96/96.08 s,
_tod_explosion/_tod_explosion_elza 28.66/45.14 s, unit_r41_gatorszene_tod_explosion 0.37 s). Spur B's Riegel (unit_r41_re2gator_*,
integration_r41_re2gator) laufen ohne (9,110) bzw. ohne R1 und sind damit von key_rueckschnitt_pruefen unberuehrt - gruen.

### N3 OFFEN

1. **Nebenbefund an der exe nicht nachgestellt:** "frueherer R1-Tod in derselben Sitzung, dann Zustand davor betreten" (Spielstand mit
   (9,110) ohne (9,111) laden oder Continue) - gemessen nur im Unit-Riegel (zweiter Fall im selben Prozess). `musik_soll` nutzt weiter
   `gator_tot()` (re15_gr2_tot() ueber alle Slots): in diesem Ablauf koennte die Kampfmusik-Weiche im ersten Bild den alten Tod sehen -
   nicht gemessen, nicht geaendert (sonst aendert sich die Musik beim Wiederbetreten NACH dem Tod).
2. **Integration mit Spur G:** im Fall mit Explosionsschnitt stehen B's Zeilen jetzt `KAMERA R1 zurueck -> Cut X F<r>` und `KAMERA R1
   frei ... F<r>` im Rueckschnitt-Takt (vorher frei erst in P3), `R1 ende (Leiche)` unveraendert in P3. G's Kampf-Riegel
   (test_r41_kanister2080_kampf.cmake, im G-Baum nur gelesen) prueft die Reihenfolge `KANISTER gesetzt -> Stufe 1 -> angehaengt -> R1
   start -> R1 ende (Leiche)` - davon unberuehrt; im Integrationsbaum neu messen. Die 2080-Lagen weiter gleich halten.
3. Unveraendert aus N2: Form "Kameraschnitt nach dem Tod" = PORT-WAHL ohne Bestaetigung des Nutzers; Todesrolle nach der Einstellung 0
   Bildpunkte in Cut 0 (Rueckseite, NCLIP @0x800255a8); Leiche mitten im Becken ~6 Bildpunkte (gerechnet, nicht gemessen); MAIN2E nach dem
   Tod statt RE2-Ausblenden; Nachrichten-Ids 44-46 frei; Schachtspinnen 20A0; nach dem Laden eines Spielstands keine Einstellung
   (Ersatzlage); PSX-Bau ungeprueft (key_suche beim Tod fuer acht Zonenkameras).

### N3 Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien.
- Stirbt der Alligator am Kanister und hat das Spiel dafuer auf eine Explosions-Kamera geschnitten, folgt jetzt direkt die Einstellung auf
  den blinkenden Water Key - der kurze Blick (zwei Bilder) auf die normale Raumkamera dazwischen ist weg. Nach 1,5 Sekunden geht die Kamera
  wie bisher auf die Raumkamera zurueck. Ohne Explosionsschnitt und bei Elza gleich.
- Frage (aus Nachbesserung 2, weiter offen): Ist der Water Key in dieser Einstellung nach dem Tod fuer dich gut zu erkennen, und passt die
  Form "Kamera springt nach dem Tod kurz auf den Schluessel"?

## Nachbesserung 4 (nach Abnahme 3, F_abnahme_3.md: M1 = Nachtrag 29)

Start 2026-10-25, HEAD cc4a4646 (Code-Stand da156ce6 + Doku 0a1625af/cc4a4646), Baum sauber. Messordner (scratchpad, nicht im
Repo): `F41n4/` (`runs/<lauf>/...`, eigene exe-Kopie je Lauf `re15_pc_f41n4_<lauf>.exe`, timeout, danach geloescht). Alle Laeufe
RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP.

Nutzer-Wortlaut (Nachtrag 29, 2026-10-25 12:08): "Die Zombie Leiche mit dem Schluessel ist zu nah an der Leiter. Das Krokodil frisst
ihn, und ist damit direkt beim Start des Kampfes quasi unmittelbar an der Leiter. Man hat keine Chance direkt am Anfang vom Kampf den
Aligator auszuweichen. Die Zombie Leiche inklusive der Kette - Schluessel - Aligator Fressen etc. muss weiter weg sein, damit man am
Start des Kampfes eine Chance hat."

Plan:
- N4.M1 (1) Messung vorher an der exe (Leiterweg, Leon + Elza, stehend und sofort weglaufend): Abstand Gator <-> Spielfigur beim
  Kampfbeginn, Bild des ersten Angriffs (Clip 4 Bild 0) und des ersten Treffers. (2) RE2-Sollwert: Ende des Alligator-Intros in
  ROOM40A0 (Pos_set @SCD 0x0736, Intro-Clips 6 -> 3, Gier-Knick @0x80100938-48, Biss-Sperre +0x238 = 110 @0x8010091c-20) gegen
  Leons Lage am Intro-Ende (SCD-Ausloeser/Pos_set), Abstand und Bilder bis zum ersten Angriff aus den Dash-/Biss-Schwellen des
  RE2-Moduls (selbst disassembliert). (3) Geometrie 2090: Wasserzonen (sce 8 @0x009D6/@0x009EA/@0x009FE), Becken/Plattform,
  groesster erreichbarer Abstand; neue Lage der Leiche + Buehne + Szenenkamera + Schluessel-Neigung (leiche_lage.py erweitert um
  den Abstand zur Leiter-Ankunft). (4) Umsetzung, (5) Messung nachher (Leon + Elza, stehend/weglaufend, Ausweichen gelingt),
  (6) Riegel fuer Abstand und Bilder bis zum ersten Angriff; bestehende F-Riegel mit Beleg (Nachtrag 29) nachziehen.

### N4.M1 Leiche samt Kette weiter weg von der Leiter (Nachtrag 29, Leon und Elza)

- **Messung vorher** (exe am Stand cc4a4646 = Code da156ce6, gebaut 10:37, Laeufe `F41n4/runs/V_*`, Rezept der Abnahme 3:
  `RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080|2081 RE15_EXIT_AT=700#2090|2091`, weglaufend zusaetzlich
  `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W13.5,UX4`; Auswertung `F41n4/py/kampf.py`: letzter
  Raum-Abschnitt von state.log, Kampfbeginn = `Folge C`):

  | Lauf | Figur | Kampfbeginn | Spieler | Gator-Wurzel (Gier) | Abstand | erster Angriff (Clip 4) | erster Treffer | HP 0 |
  |---|---|---|---|---|---|---|---|---|
  | V_L stehend | PL00 | F405 | (-8392,-25550) | (-4254,-20984) (1505) | 6162 | F459 (+54) | F466 (+61) | F573 |
  | V_LW weglaufend (Osten) | PL00 | F405 | (-8392,-25550) | (-4254,-20984) | 6162 | F431 (+26) | F438 (+33) | F545 |
  | V_E stehend | PL04 | F405 | (-8392,-25550) | (-4254,-20984) | 6162 | F459 (+54) | F466 (+61) | F573 |
  | V_EW weglaufend | PL04 | F405 | (-8392,-25550) | (-4254,-20984) | 6162 | F431 (+26) | F438 (+33) | F545 |

  = Abnahme 3 Abschnitt 2 (Bild fuer Bild gleich). Ursache: die Kette endet an der Leiche (-5500,-23700) / Rumpfmitte (-4594,-24306)
  (GSZ_LEICHE_*, Fassung 1 von leiche_lage.py: Anlauf fest Gier 1024 aus dem Norden durch den Westkanal, Wert nur Bildgroesse^2 x
  Licht des Schluessels in Cut 1 - der Abstand zur Leiter war kein Kriterium). Der einzige freie Weg von der Leiter ist Osten, also
  am Maul vorbei.
- **RE2-Sollwert (selbst gelesen und gerechnet).** Probe `re15_port/tools/r41_gatorszene/probe_re2_intro_ende.c` (gegen
  libre15_engine.a, RE2-EM23-Bank aus shared_assets/RE2/CDEMD0.EMS, Massstab 1 wie RE2), Ausgabe `F41n4/probe_re2_intro_ende.txt`.
  Belege (SCD-Walker `tools/r41_re2gator/re2_scd_walk.py` auf info/re2leon/PL0/RDT/ROOM40A0.RDT, `re2_disasm.py --bin
  EM23_OVL_0000.BIN` bzw. info/re2leon/PSX.EXE):
  - Ausloeser sub02 (Durchbruch-Szene) `Evt_exec 04 ff 18 0b` @SCD-main 0x00EE startet sub11; sub11 `Member_set2 35 17 10` @0x0730
    (+0x1D4 -> Intro P1), `Evt_next` @0x0734, `Pos_set 32 00 cd 11 f8 f8 c3 b8` @0x0736 = Gator (4557,-1800,-18237), Gier 0x400 aus
    Sce_em_set `44 00 00 23 ...` @RDT 0x25A0 (SCD-init sub00 @0x00A0).
  - Intro Sub 0 (EM23 @0x80100804): P0 `lh v0,468(s0)` / `beq` @0x80100854-5c; P1 `ori v0,v0,0x110` @0x8010087c, Clip 6
    `addiu a1,zero,6` / `jal 0x8001a330` @0x801008ac-b4, Ende -> Cut 1 `jal 0x8002c7ac` / `addiu a0,zero,1` @0x801008c4-c8, z = -21212
    `addiu v1,zero,-21212` / `sw v1,64(s0)` @0x801008d0-d4; P2 Clip 3 `addiu a1,zero,3` / `jal 0x8001a330` @0x801008e4-ec, Ende:
    0x800cfbd8 &= ~0x10 & ~0x100 @0x801008f8-918, `addiu v0,zero,110` / `sb v0,568(s0)` @0x8010091c-20 (+0x238 = 110),
    `addiu v1,zero,257` / `sw v1,4(s0)` @0x80100928-2c (Sub 1), `addiu v0,v0,1024` / `sh v0,118(s0)` @0x80100940-48 (Gier += 1024).
  - Wurzelschritt FUN_8001a330 (RE2 PSX.EXE): Clip-Wechsel setzt Bild 0 (`sb zero,333(s0)` @0x8001a360), Geschwindigkeit
    `jal 0x80015e7c` @0x8001a374 (RE2_Quellcode_V2/FUN_80015e7c.c: bei Bild 0 Vorgaenger-Versatz +0x20C/+0x210 = 0 -> erster Schritt
    = off[0]), Bildschritt `jal 0x8002959c` @0x8001a3a8, am Clip-Ende kein Schritt (`bne v0,zero,0x8001a3cc` @0x8001a3b0), sonst
    `jal 0x800152c8` @0x8001a3c0 (FUN_800152c8: RotMatrix Y = Gier, x += c*vx + s*vz, z += -s*vx + c*vz). Versaetze = EMR-
    Keyframe-Bytes 6..11 (Clip 6 off -7040 -> 2458, Clip 3 (0,0,3) -> (2884,0,-4042), Clip 0 106 .. 6096 mit Rueckgleiten in den
    Bildern 6..22 und 82..99, Clip 1 311 -> 6109 in 26 Bildern).
  - Nach dem Intro: Sub 1 Main `FUN_8001a330(em,0,32,0)` @0x80100c34-40; Sub 1 -> Sub 2 nur bei +0x238 == 0 (`lbu v0,568(s0)` /
    `bne` @0x80100b5c-64) und +0x1F0 >= 0x65 (`sltiu v0,v0,0x65` @0x80100b88/@0x80100ba4; +0x1F0 = SquareRoot0(dx*dx + dz*dz)
    `jal 0x8008d2f4` / `sw v0,496(s0)` @0x800265a4-e0, Welteinheiten); +0x238 zaehlt am Ende des Root-Takts ab (`lbu v0,568(s0)` /
    `addiu v0,v0,-1` / `sb v0,568(s0)` @0x801004a8-b8); Sub 2 = Clip 1 (@0x80100cf8-d04). Der Koerper wird nie gelenkt (Root nur
    Hals FUN_80017d98 @0x8010036c-74) -> geradeaus mit Gier 2048 (Westen).
  - Leon: sub03 `Pos_set 32 00 80 da f8 f8 38 9b` @0x0250 = (-9600,-1800,-25800), Warten auf Cut 1 `While ... Cmp 23 00 1a 05 01 00`
    @0x025E-6A, `Sleep 0x3c` @0x0276, `Sleep 0x32` @0x027E, `Plc_dest 40 00 08 0a d0 d5 38 9b` @0x0286 -> (-10800,-25800),
    `Plc_dest 40 00 15 0a 00 08 80 00` @0x029E (Drehung), `Plc_ret` @0x02B6, `Set 22 02 07 00` @0x02B7 (Riegel auf). Leon bekommt
    die Steuerung also >= 110 Bilder + Gehweg nach Cut 1, der Gator braucht nach Cut 1 noch Clip 3 = 150 Bilder: beide Enden
    liegen im selben Fenster (Leons Gehzeit fuer 1200 Einheiten ist hier nicht gemessen - Bezug ist das Intro-Ende des Gators).
  - **Ergebnis RE2:** Intro-Ende Wurzel (503,-24100) Gier 2048; Leon (-10800,-25800): **Abstand Wurzel <-> Leon 11430** (laengs
    11303, quer 1700), Kopf (Bone 6) <-> Leon im ersten Bild danach 6871. Leon steht: 110 Bilder Schwimmen (Clip 0, Wurzel 503 ->
    -2557), dann Paddeln (Clip 1); der vorderste Knochen erreicht Leons x **nach 121 Bildern** (Kopf dabei 1647 seitlich), die
    Wurzel nach 143. Sollwerte fuer den Port: Abstand Wurzel <-> Spielfigur am Kampfbeginn >= 11430, erster Angriff (Leon steht)
    >= 121 Bilder nach dem Kampfbeginn. Gleiche Einheiten (Leon ist in beiden Spielen gleich gross); der Port-Gator ist 2/3 so gross
    (GB_SCALE_Q12 2731) und entsprechend langsamer - ein gleicher Wurzelabstand ist im Port also eher grosszuegiger.
- **Geometrie 2090 und Wahl der Lage (PORT-WAHL nach Nachtrag 6/29, Rechnung `tools/r41_gatorszene/leiche_lage_n4.py`, Ausgabe
  `F41n4/leiche_lage_n4.txt`).** Wasser = Becken des Boss-Moduls (gb_becken_draussen: SCA-Aussenwaende x[-8900..7200]
  z[-27000..-5400] ohne Plattform x[-1700..1850] z[-20000..-12450] und Rampe x[1850..7200] z[-18100..-14500]); Kette wie
  ga_buehne (Kopf im Koppelbild 13 auf der Rumpfmitte, S = Mitte - Kopf13 - Weg4 - Weg6, zusammen 10715 laengs), Weg der Wurzel
  S -> Kau-Lage mit Rand 900 und des Kopfes mit Rand 500 im Wasser; Abstand Kau-Lage <-> Leiter-Ankunft (-8200,-25550) minus 700
  (Wende + Schrei) >= 11430; Leiche ganz im Bild des Cuts (Huelle des MD1 @0x2054), Wert = Bildgroesse^2 x Licht des Schluessels
  (Fassung 1). Ergebnis:
  - Die alte Lage (-5500,-23700) und die Original-Lage von obj 2 (-1725,-21723) @0x00A6C haben KEINE Kette mit dem RE2-Abstand
    im Wasser (Suedbecken zu nah an der Leiter).
  - Bester Wert ueber alle Lagen: obj 2 nach (700,-8200) in Cut 5 (Schluessel 11,5 Bildpunkte) - das liegt 1063 neben der zweiten
    treibenden Leiche obj 3 (Obj_model_set `2d 03 00 00 02 00 0a 00 00 00 e7 ff 34 f9 01 e3 00 00 78 06 ...` @0x00A8E =
    (-25,-1740,-7423) Gier 1656), also zwei Leichen nebeneinander - verworfen.
  - Erste Wahl (verworfen nach Messung): obj 3 an ihrer Original-Lage (-25,-7423) Gier 1656 - Kette mit Anlauf-Gier 3488 im
    Wasser, Abstand 16213 - 700, Cut 5, Neigung rx 144 / rz -672, ~9,0 x 8,2 Bildpunkte gerechnet. Gemessen an der exe (Lauf
    `F41n4/runs/N_L1a`, Ankunftseinstellung Cut 5 F40-F135, Ausschnitt (150..169, 106..121)): nur 2-9 Pixel ueber Helligkeit 25,
    hellster Pixel 48 -> 70 (x1,46) - der Ring ist kaum zu sehen. Ursache: das Nordbecken liegt in allen Kameras nur im
    Umgebungslicht (Lichtsaetze RDT @0x468 + 40 * Cut: Cut 3/4 Lichter bei (-11890,-4000,-24387)/(-14200,-4000,-30252), Cut 5/6/7
    bei x -29490/-24699, Umgebung 63/63/63 bzw. 63/73/63; flaches Licht im Nordbecken 63..81, im Suedbecken unter Cut 1/2 bis 195),
    also zaehlt die Groesse.
  - **Gewaehlt: obj 3 an der Lage mit dem besten Wert** = die Fassung-1-Regel unter dem RE2-Abstand, Cut 5 (700,-1740,-8200)
    Gier 2432; da sie 1063 neben obj 3 liegt, ist die Szenen-Leiche obj 3 selbst (die treibende Leiche des Nordbeckens, Nutzer
    "nimm vielleicht diesen", Nachtrag 6: Lage darf sich aendern), obj 2 bleibt an seiner Original-Lage @0x00A6C (13739 entfernt,
    Suedbecken, Bild Cut 0, ohne Schluessel). Anlauf-Gier 3552 (groesster kleinster Rand: Wurzelweg 1496 / Kopfweg 1534; Nachbarn
    3536: 1422/1460, 3568: 1326/1611), S (-7381,-15553) im Westkanal, der Gator schwimmt nach Nordosten ins Nordbecken; Kau-Lage
    (-2077,-9725), Abstand zur Leiter-Ankunft 16968 (- 700 = 16268 >= 11430). Ganze Leiche im Bild (62,132)-(261,205);
    Schluessel-Neigung lokal rx 672 / rz -144 (59,9 Grad), Licht 103,8 (Spitze 154 < 172), ~11,5 x 10,6 Bildpunkte (Fassung 1:
    10,4 x 9,5 bei Licht 170). `leiche_lage_n4.py` (ohne Schalter) rechnet genau diese Wahl nach, `--suche` die freie Suche,
    `--original` den Vergleich. ROOM2091.RDT ist bytegleich zu ROOM2090.RDT (cmp) -> Elza gleich.
- **Umsetzung** (engine/src/gatorszene_r41.c, include/re15_gatorszene_r41.h; keine gemeinsame Datei, kein Eingriff in Spur B's
  Modul; Form der Szene unveraendert - nur Lage/Kamera/Anlauf/Neigung, alles PORT-WAHL der Inszenierung nach Nachtrag 6/29):
  - `RE15_GSZ_LEICHE_OBJ` 2 -> 3 (Obj_model_set obj 3 @0x00A8E), `GSZ_LEICHE_X/_Z/_ROT_Y` (-5500,-23700,384) -> (700,-8200,2432),
    `GSZ_CUT_LEICHE` 1 -> 5 (RID @0x060 + 5 * 32), `GSZ_GATOR_GIER` 1024 -> 3552, `GSZ_KEY_AUF_RX/_RZ` 544/-464 -> 672/-144 - je mit
    Herleitung im Kommentar (leiche_lage_n4.py; Lichtsatz RDT @0x468 + 40 * 5; Puls-Grenze @0x801028fc/@0x8010290c).
  - Der Rueckkehr-Tuersatz von Programm 2 nimmt `GSZ_CUT_LEICHE` (2080 -> 2090 jetzt in Cut 5), ga_buehne rechnet die Buehne aus der
    Prop-Lage (unveraendert), die Kamera-Uebergabe am Ende bleibt der Eintritts-Cut 0 der Leiter.
  - Werkzeuge: `tools/r41_gatorszene/probe_re2_intro_ende.c` (RE2-Sollwert), `tools/r41_gatorszene/leiche_lage_n4.py` (Wahl).
- **Messung nachher** (exe mit fb64e68c, eigene Kopien `re15_pc_f41n4_<lauf>.exe`, Rezept wie vorher; Laeufe `F41n4/runs/M_*`):

  | Lauf | Figur | Kampfbeginn | Spieler | Gator-Wurzel (Gier) | Abstand | erster Angriff (Clip 4) | erster Treffer |
  |---|---|---|---|---|---|---|---|
  | M_L stehend | PL00 | F422 | (-8200,-25550) | (-2178,-9077) (1253) | 17539 | F892 (+470) | F899 (+477) |
  | M_E stehend | PL04 | F422 | (-8200,-25550) | (-2178,-9077) (1253) | 17539 | F892 (+470) | F899 (+477) |
  | M_LW weglaufend (UX ab F406 gehalten, rennt ab dem Kampfbeginn) | PL00 | F422 | (-8200,-25550) -> (5948,-26532) | (-2178,-9077) | 17539 | F947 (+525) | F954 (+532) |
  | M_EW weglaufend | PL04 | F422 | wie M_LW | (-2178,-9077) | 17539 | F947 (+525) | F954 (+532) |

  Vorher 6162 / +54 / +61 (stehend) bzw. +26 / +33 (weglaufend), RE2-Soll 11430 / 121. Folge Zeile fuer Zeile wie vorher bis auf
  die Bilder: A F40, P 1, T F98, P 2 + M 8/2 (2080 F142), P 3 F156, S F53, Buehne `Leiche (-206,-7594) ... S (-7379,-15540) Gier
  3552`, G 1 F53 (Cut 5), G 2 F176 (Cut 4), K 13 F189, G 3 F190, G 4 F310, V F369, G 5 F369, G 6 F393 (Cut 4), C F422 (Cut 0,
  Spielfigur im Bild, Fuss-Bildpunkt 88,127), G 7 F422, B 1 F424 (MAIN3C = RE2 MAIN1A). Die Wende auf Leon dauert 24 Bilder
  (vorher 7; G 5 -> G 6), deshalb kommt der Kampfbeginn 17 Bilder spaeter. Kameras der Spur-B-API: DURCHBRUCH -> Cut 5 (frontal 4085,
  Soll 4092), FINISHER -> Cut 4 (frontal -1625; Cut 5 frontal 3933 liegt ueber der Massstab-Grenze 1892 von B8). Spinnen: je
  2090-Besuch zwei Zeilen "verworfen". Der Weg des Gators (state.log M_L): 110 Bilder Schwimmen (Clip 0, Sperre 110), Paddeln durch
  den Westkanal, ab F583 der RE2-Dash beim Wechsel in Leons Zone (Clip 2 + 3, 300 Bilder), Biss F892.
  Bilder: `F_belege/n4_gator_szene.png` (F60/F120/F170 Cut 5: der Gator taucht im Westkanal auf und kommt frontal auf die Leiche
  zu; F185-F400 Cut 4: Biss, Schuetteln, Kauen, Wende, Schrei; F425 Cut 0: Kampfbeginn, Leon an der Leiter, der Gator weit weg im
  Nordbecken; F550/F700/F890: Anlauf und Biss), `F_belege/n4_flucht_leon.png` (M_LW: Leon rennt ab F423 nach Osten, steht ab ~F498
  an der Ostwand, der Gator kommt nach dem Dash, Biss F947).
- **Schluessel in der Ankunftseinstellung** (Cut 5, Laeufe M_L1a / M_E1a, Bilder F40-F136 je Bild, `F41n4/py/keypuls.py`,
  Ausschnitt (155..184, 114..133) ueber dem Ruecken - die Leiche liegt darunter): hellster Pixel 47 -> 66 (x1,40), Kasten aller
  Bilder (160,122)-(168,133) = 9 x 12, 1-16 Pixel ueber 25 je Bild (das Wippen der Leiche, sub03 @0x00B5E, dreht den Ring);
  Spitzen um F76-F100. Zoom `F_belege/n4_ankunft_cut5.png` (oben fd 60/90 doppelt, unten fd 52/76/100/118 sechsfach: Ring mit Bart
  in fd 76/100). Leon = Elza. Unit leiche: Primitivfarbe Ruhe 51 -> Spitze 76 (<= 0x80), flach 38. Vergleich: Original-Lage obj 3
  48 -> 70 bei 2-9 Pixeln, Fassung 1 (Cut 1, Suedbecken) 72 -> 108 - der Schluessel ist jetzt etwa so gross wie in Fassung 1,
  aber dunkler (Nordbecken nur im Umgebungslicht, s. oben).

### N4 Tests

Neu (nur F-eigene Riegel, registriert in tests/unit/probes/r41_gatorszene.cmake):
- `unit_r41_gatorszene_abstand` / `_abstand_elza` (test_r41_gatorszene.c Teil abstand): die ganze Szene ueber die Leiter mit dem
  echten Boss-Takt bis zum Kampfbeginn; RE2-Sollwert IM Riegel aus den RE2-Daten gerechnet (Probe-Rechnung: Abstand 11402 mit
  SquareRoot0 wie RE2 +0x1F0 `jal 0x8008d2f4` @0x800265d4, exakt 11430; 121 Bilder); verlangt Abstand Gator-Wurzel <-> Spielfigur
  >= RE2 (gemessen 17496), Boss ab Bild 1 in der Verfolgung, erster Biss/Treffer (Leon steht) nicht vor 121 Bildern (gemessen
  470/477); Gegenprobe: dieselbe Szene, Gator am Kampfbeginn an die Fassung-1-Lage (-4254,-20984) Gier 1505 -> erster Biss nach 48
  Bildern < 121 (der Riegel faengt den Befund der Abnahme 3; exe vorher: 54).
- `integration_r41_gatorszene_flucht` / `_flucht_elza` (test_r41_gatorszene.cmake): Leiterweg, Spielfigur rennt ab dem Kampfbeginn
  nach Osten (`RE15_INPUT_SCRIPT=W13.5,UX4.6`, gehalten ab F406, die Szene haelt die Figur bis zum Kampfbeginn; Lauf N_LW2 = N_LW
  mit W14.1 Bild fuer Bild gleich); verlangt Abstandsquadrat am Kampfbeginn (Folge C) >= RE2 (Auswerter `test_r41_gatorszene
  re2soll`: `abstand=11402 abstand2=130647809 bilder=121`), kein Biss/Treffer bis F<c>+121, Spielfigur >= 5000 nach Osten
  (gemessen 14932, x -8200 -> 6732).
- Auswerter `test_r41_gatorszene re2soll` (Werkzeug-Modus fuer die exe-Riegel).

Nachgezogen mit Beleg (Querbezug Nachtrag 29):
- `integration_r41_gatorszene_leon` / `_elza`: Kampfbeginn = Folge C statt fest F405 (cam=0 in F<c>..F<c+74>, jetzt F422-F496),
  dazu Abstand + kein Biss/Treffer in den RE2-Bildern (Funktion r41f_kampf; Gegenprobe des Auswerters an den Logs der Laeufe V_L /
  V_LW vor der Aenderung: FEHLER "Abstandsquadrat ... < RE2", an N_L / N_LW: OK).
- `unit_r41_gatorszene_leiche` (obj 3 bei (700,-8200) Gier 2432, Neigung 672/-144, Cut 5: Primitivfarbe Ruhe 51 -> Spitze 76 <= 0x80;
  Gegenprobe jetzt "flach dunkler" (38 < 51), weil in Cut 5 keine Neigung die Kappung erreicht), `_programme` (P1 Cut 5),
  `_szene` (Rueckkehr-Cut 5).
- Spur-B-Riegel `integration_r41_re2gator` Lauf szene (Datei der Spur B, Querbezug (e) im Kopf): RE15_EXIT_AT 420 -> 440, weil das
  Szenenende ("KAMERA SZENE ... Ende") jetzt in F422 liegt (Wende 24 statt 7 Bilder); Pruefungen unveraendert.

Gezielt (keine volle Suite), Regex = die der Abnahme 3 (`F41a3/regex2.txt` = `F41n4/regex.txt`), sie schliesst die neuen Riegel
ueber `unit_r41_gatorszene_` / `integration_r41_gatorszene_` ein (`ctest -N`: `Total Tests: 221` = 217 + 4):
`ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|integration_r41_re2gator$|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_|integration_r39_inventar_discard$|unit_item_|unit_inv_|unit_sca_wall_hit$|unit_re2_spider_|unit_spider_ai$|unit_adult_spider_ai$|unit_r38_spinnen$|integration_r38_spinnen_echtlauf$|unit_writher_ai$|unit_enemy_shootable$|unit_collision_layers$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|integration_r36_schluessel|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|integration_r37_feuerloescher$|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_|unit_door_seq$|unit_r38_wasser_|integration_r38_wasser$|unit_r41_kleinkram_a$|integration_r41_kleinkram_a_blink$|r31_trefferboxen|unit_rotor_bgm_pin$|unit_r35_cut10f0_bgm$|unit_r39_johnruf1041_musik$|unit_r35_cut1150_knallbank$|unit_r36_szene1150_knall_re2$|integration_r39_elzazombies_knall$|unit_r36_inventar$|unit_r36_kartenitems$|unit_r37_itemband$|integration_r37_itemband$|unit_r36_tueren_|unit_r33_tueren_|unit_r30_tuer|unit_r31_tuer_ton$|unit_r35_cut10f0_tuerton$|unit_r39_karten_tueren$|integration_r36_tueren$|unit_camera_view$|unit_cut_old_displayed$|unit_r34n_b_cut_old$|prop_zone_dispatch$|prop_scd_pc_advance$|unit_r38_montage_kameras$|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$|prop_spawn_entered$|integration_r35_granate$|integration_r41_wasser20b0_npc$|integration_r38_wasserzombie$|unit_r35_granate$|unit_irons_mittelmodell$|unit_r34_reaktion$|unit_1210_arme$|unit_r30_irons_tisch_licht$|unit_r35_affen_|unit_r36_szenenleon_|unit_r36_ziel1190$|unit_r37_pri1100_|unit_r38_npckante_|unit_r38_wasserzombie$|unit_r41_wasser20b0_|integration_r36_kartenitems_bild$)" -j4 --timeout 1200 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 221`** (`Total Test time (real) = 1101.44 sec`; darin integration_r37_itemband 830.27 s,
integration_r36_kartenitems_bild 465.79 s, integration_r38_rolltor20a0 282.65 s, integration_r41_re2gator 269.20 s,
integration_r41_wasser20b0_npc 219.66 s, integration_r38_wasser 185.88 s, integration_r41_gatorszene_leon/_elza 54.21/74.70 s,
_flucht/_flucht_elza 53.98/74.43 s, _tod/_tod_elza 64.11/104.40 s, _tod_r1/_tod_r1_elza 42.27/82.47 s, _tod_explosion/_elza
27.80/47.88 s, unit_r41_gatorszene_abstand/_elza 0.11/0.11 s).

### N4 OFFEN

1. **Schluessel im Nordbecken dunkler:** Ankunftseinstellung Cut 5 hellster Pixel 47 -> 66 (Fassung 1 im Suedbecken unter Cut 1:
   72 -> 108), Kasten 9 x 12. Ursache sind die Lichtsaetze des Original-RDT (@0x468 + 40 * Cut): kein Licht reicht ins Nordbecken,
   dort gilt nur das Umgebungslicht 63..73; der Wert Bildgroesse^2 x Licht ist dort bestmoeglich gewaehlt. Ein helleres Licht waere
   erfunden - nicht gebaut.
2. **Fressen von hinten:** die Kamerawahl FINISHER der Spur B nimmt Cut 4 (frontal -1625), weil Cut 5 (frontal 3933) ueber der
   Massstab-Grenze 1892 liegt (Entscheid B8, Modul der Spur B). Biss, Schuetteln und Kauen sind in Cut 4 zu sehen
   (n4_gator_szene.png F185-F400), die Leiche im Maul nur teilweise.
3. **Anlauf lang:** Leon steht -> erster Biss nach 470 Bildern (15,7 s), RE2-Soll 121. Davon 300 Bilder RE2-Dash (Clip 2 + 3) beim
   Wechsel in Leons Zone (Spur B, Zonen-Dash @0x801009a8/@0x801009ec) - "mindestens wie RE2" ist erfuellt, der Nutzer kann das
   Tempo im Spiel beurteilen.
4. RE2-Sollwert: Leons Plc_ret liegt im Fenster des Clip-3-Endes (>= 110 Bilder Sleep + Gehweg 1200 nach Cut 1, Clip 3 = 150 Bilder);
   Leons Gehzeit in RE2 ist nicht gemessen - Bezug ist das Intro-Ende des Gators (@0x801008f4-948).
5. **Integration mit Spur G:** der Kampf beginnt jetzt in F422 (statt F405) mit dem Gator im Nordbecken (-2178,-9077) statt bei
   (-4254,-20984); die Kampf-/Kanister-Riegel der Spur G (nur im G-Baum) im Integrationsbaum neu messen ("Querbezug B/G: nur die
   Startlage des Kampfes aendert sich", Nachtrag 29).
6. Unveraendert aus N3: Nebenbefund R1-Tod aus frueherem Raumbesuch nur im Unit-Riegel; musik_soll ueber re15_gr2_tot(); Form
   "Kameraschnitt nach dem Tod" ohne Bestaetigung; MAIN2E nach dem Tod; Nachrichten-Ids 44-46 frei; Schachtspinnen 20A0; nach dem Laden
   keine Einstellung (Ersatzlage jetzt die Rumpfmitte von obj 3 (-206,-7594)); PSX-Bau ungeprueft.

### N4 Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien.
- Die Leiche mit dem blinkenden Schluessel ist jetzt die treibende Zombie-Leiche im hinteren (noerdlichen) Becken - die zweite, die
  schon im Original dort schwimmt (Kamera vom Nordost-Ende). Leon kommt wie bisher ueber die Leiter an: Leiche mit Schluessel ->
  Knall -> 2080 oben -> erneut die Leiche -> der Alligator taucht im Westkanal auf, schwimmt durchs hintere Becken zur Leiche und
  frisst sie samt Schluessel -> dreht sich zu Leon und schreit -> die Kamera geht an die Leiter, der Kampf beginnt mit der RE2-Musik.
  Beim Kampfbeginn ist der Alligator jetzt rund 17500 Einheiten entfernt (vorher 6200); steht man still, beisst er nach etwa 15 s
  (vorher 2 s), rennt man sofort nach Osten, nach etwa 17 s (vorher 1 s). Die erste Leiche (vorne, vom Eingang aus sichtbar) treibt
  wieder an ihrer Original-Stelle und hat keinen Schluessel.
- Hinweis: Im hinteren Becken ist es dunkler (das Original beleuchtet es kaum), der Schluessel ist dort etwa so gross wie vorher,
  aber dunkler.
- Frage (Beobachtung): Passt dir die Entfernung beim Kampfbeginn so, oder ist der Anlauf des Alligators jetzt zu lang? Ist der
  blinkende Schluessel im hinteren Becken fuer dich gut zu erkennen?

## Nachbesserung 5 (nach Abnahme 4, F_abnahme_4.md: M1-M4 = Orchestrator-Entscheid F-E2)

Start 2026-10-25, HEAD 4b875538 (Code-Stand fb64e68c + Doku 877919e2/e96d076e/3b2340c7/4b875538), Baum sauber, exe = Code-Stand
(re15_pc.exe 13:49 nach gatorszene_r41.c 13:49). Messordner (scratchpad, nicht im Repo): `F41n5/` (`runs/<lauf>/...`, eigene exe-Kopie
je Lauf `re15_pc_f41n5_<lauf>.exe`, timeout, danach geloescht). Alle Laeufe RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP.

Massgeblich: Entscheid F-E2 (AUFTRAG.md 2026-10-25 14:21): (1) Kampfbeginn = RE2 (Abstand 11430 / erster Kontakt 121 Bilder), so nah wie
moeglich, nicht darunter; liegt es am Zonen-Dash, Lage mit der RE2-Zonenbeziehung am Intro-Ende (in RE2 belegt); (2) Schluessel in der
Ankunftseinstellung mindestens wie abgenommen (hellster Pixel Ruhe >= 72, Spitze >= 108, Kasten >= 10 x 9); (3) Fressen mit Frontalwert
> 0, Leiche im Maul sichtbar, B8 bleibt, Freiheitsgrad = Lage der Leiche; unter 1-3 der beste Schluessel-Wert, Rangfolge 1 > 2 > 3;
Riegel mit oberer UND unterer Grenze aus dem RE2-Beleg, auf beiden Seiten dasselbe Mass.

Plan:
- (a) RE2 belegen: Zonenlinien des Gators (+0x21A, Sub-1-Pre @0x80100964-a08) und Lage Gator/Leon am Intro-Ende -> Zonenbeziehung;
  Ende der Zwischensequenz fuer Leon (sub03 Plc_ret @SCD 0x02B6 / Set(2,7,0) @0x02B7) gegen das Intro-Ende (Clip-3-Ende) -> Band
  des RE2-Sollwerts (untere und obere Grenze) im selben Mass wie der Port (Kontakt = vorderster Knochen erreicht Leon laengs der
  Schwimmrichtung).
- (b) Lagesuche mit dem ECHTEN Engine-Code (C-Sonde gegen libre15_engine.a: Buehne, Durchbruch, Fressen, Wende, Schrei, Kamerawahl der
  Spur B, Kampfbeginn, erster Kontakt) statt der Python-Naeherung: Gator am Kampfbeginn in Leons Zone 0 (Suedbecken), Abstand und
  Kontaktbilder im RE2-Band, FINISHER-Kamera mit frontal > 0 unter der Massstab-Grenze, Leiche und Kiefer im Bild, Schluessel in der
  Ankunftskamera mit dem besten Wert.
- (c) Umsetzung (Konstanten in gatorszene_r41.c, Herleitung im Kommentar), (d) Messung an der exe (Leon + Elza, stehend + weglaufend,
  Ankunftseinstellung keypix wie Abnahme 4, Fress-Einstellung), (e) Riegel mit oberer + unterer Grenze (abstand/_elza, leon/_elza,
  flucht/_elza) und Untergrenze fuer den Schluessel (leiche), (f) gezielte Tests, Dossier, Abschluss-Commit.
