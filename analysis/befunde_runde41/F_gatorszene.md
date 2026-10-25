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
- [ ] OFFEN (§O): Spur-B-Riegel integration_r41_re2gator braucht die Bits 110/113 + neue Bildfenster (Nutzer-Aenderungen)

## 0. Protokoll

* 2026-10-24 19:4x Start: Baum sauber, HEAD aece7457. Messlaeufe immer mit eigener exe-Kopie `re15_pc_r41f_<lauf>.exe`
  (scratchpad-Helfer `F41/lauf.sh`, Elza `F41/lauf_elza.sh`), RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP.
* Commits: 467c766e (Dossier), 07950791/a11460cd (Punkt 2), f4b17b5f (Szene), e0f0635a (Water Key), 6abb940b (Belege),
  898ae6a8 (Cut 1/Licht, Knall, 2080 oben, Tor, Musik), ef13fe31 (Riegel), 73bf903f (r26-Riegel), Schluss-Commit.

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
  (`addiu a1,zero,4` = FRESSEN P1 Clip 4), @0x80101188 (Clip 5 P2), @0x801011d8-e0 (Clip 11 P3), @0x80102c48 (Clip 10,
  `addiu a2,zero,3` = Blend 3, R4), Intro Clip 6 @0x801008ac-b8, Schwimmen Clip 0 Blend 32 @0x80100c38. Wurzelbahnen (EMR
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

Start 2026-10-25, HEAD 76ad856d, Baum sauber. Messordner (scratchpad, nicht im Repo): `F41n1/` (exe-Kopien
`re15_pc_f41n1_<lauf>.exe`, RE15_WINDOW_SCALE=1, Bilder nur per RE15_FRAMEDUMP).

Plan (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher):
- M1 Kampfbeginn ohne Spielfigur im Bild (Kamera bleibt nach der Szene auf P3s Leichen-Cut 1).
- M2 Water Key nach dem Tod unsichtbar (beide Todeswege).
- M3 doppelter Verschlossen-Ton am Tor 20A0/20A1.
- M4 rote Fremd-Riegel (unit_r36_tueren_schott, unit_r38_spinnen, unit_r30_tuer_verschlossen, integration_r41_re2gator).
- M5 Schluessel-Puls in der Ankunftseinstellung gesaettigt.
- M6 RE-Gate: `s_ga.t > 60` ohne Beleg; Clip-11-Zitat (RE2 @0x801011e0 `jal 0x8001a240`).
