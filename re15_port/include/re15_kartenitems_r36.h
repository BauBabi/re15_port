/*
 * RE1.5 Rebuilt — LAGEPLAENE ZUM AUFSAMMELN (Runde 36 Spur N, Runde 42 Strang lageplaene): EIN Revierplan und
 * EIN Kellerplan an der Wand; Aufnehmen schaltet ALLE Kartenblaetter des Bereichs frei wie in Resident Evil 2.
 * Dossier mit allen Messungen: analysis/befunde_runde36/N_kartenitems.md (Runde 36: fuenf Etagenplaene).
 *
 * RUNDE 42 — RE2-FORM (Strang lageplaene, Stufe 1):
 *   NUTZERWUNSCH 18.11.2026 woertlich: "Dann will ich das nicht so. Ich will das es mehr funktioniert wie in
 *   Resident Evil 2." und "nicht benötigte maps kommen weg."
 *   RE2 room20B0 sub10 @0x037BA..0x0381A: EIN Plan ("A police station map. / Will you take it?" RDT @0x3B92,
 *   "You've taken a / police station map." @0x3DB8) setzt in EINEM Ereignis Set(33,2) Set(33,3) Set(33,4)
 *   @0x037DA/@0x037DE/@0x037E2 (Besitz 1F/2F/3F) und Set(35,2..4) @0x037E6-EE (Etagenwahl). Eigener B1-Plan
 *   room2130 (@0x1B32 / @0x1D57). Port: Revierplan = 1F+2F+3F (Blatt 2/3/4, Bits 89/88/87), Kellerplan =
 *   B2+B1 (Blatt 1/0, Bits 77/78; Name "police basement map" = PORT-WAHL, RE2 hat nur einen Kellerstock).
 *   Das Port-Programm setzt je Blatt ein Bank-9-Bit hintereinander wie RE2 die drei Set(33,n).
 *   ORT (PORT-WAHL nach Nutzer-Regel "die Stelle, die beide auf dem Pflichtweg zuerst erreichen"):
 *     Revierplan ROOM1030/1031 (Lobby, Stelle der Runde 36 unveraendert): Elza STARTET in 1031 (PSX.EXE
 *       @0x8001d324 Raum 0x03), Leon kommt auf dem Abstieg hindurch (1040 Slot 1 -> 1030 @0x010B8, weiter
 *       1030 Slot 0 -> 1050 @0x01C6A); die anderen Stellen 1140 (Leon Nebenweg; Elza erst E16) und 10D0
 *       (Leon L13, Elza Kette G2) liegen spaeter.
 *     Kellerplan ROOM1200/1201 (Leichenhalle, Stelle der Runde 36 unveraendert): Pflicht fuer beide
 *       (Minidisc Player 1200 sub00 @0x00932 -> Stimmschloss 1100 -> Blue Keycard -> Kartenleser 10D0 ->
 *       10F0/10F1; Leon L12-L14 -> P5, Elza D1_elza_pflichtweg.md G1..G6); 11D0 ist fuer keinen Pflicht
 *       (Tueren nur 1180/1230 @0x011E2/@0x01206 und 1260 @0x01228; beide verlassen STAGE1 ueber
 *       11B0 Slot 5 @0x01034 bzw. 10B1 Slot 1 @0x016F2 -> 1260, nicht ueber 11D0).
 *   ENTFALLEN: die Wandplaene 1140 (3F), 10D0 (2F), 11D0 (B1) — kein Prop, keine Zone, keine Texte mehr;
 *   die Waende dort sehen wieder aus wie ausgeliefert. Ihre Bits 87/88/78 bleiben die Besitz-Bits ihrer
 *   Blaetter und werden vom zustaendigen Plan mitgesetzt.
 *   ALTE SPIELSTAENDE: Besitz je Blatt = sein Bit (unveraendert). Ein Plan haengt, solange EINES seiner Bits
 *   fehlt (wer frueher nur den 3F-Plan nahm, findet den Revierplan in der Lobby und bekommt 1F/2F dazu).
 *
 * RUNDE 42 STUFE 2 — EIN PLAN JE GEBIET FUER STAGE2..6 (wie RE2: "sewer map" room4040, "factory map" room5040):
 *   NUTZERWUNSCH 18.11.2026 (Auftrag): je Gebiet ein Plan, gut sichtbar wie ein Plakat, frueh auf dem Pflichtweg beider
 *   Figuren; Kanalisation "der Wandplan im ersten Kanalraum 2000/2001" (Konzept F-25/K-04). NICHT hier: Forschungszentrum
 *   (Capcoms Wandplan 4020, baut r42/rest als Blatt 8) und Labor (Original ROOM5030 sub02, Flag (3,115)).
 *   Kanalisation ROOM2000/2001, Blatt 6, Bit 34 (re15_story_r42.h RE15_R42_BIT_PLAN_KANAL), Nordwand des Nordgangs z = -19800
 *     (SCA-Zelle 7 @0x012A4 z[-19800..-17800] floor 19 = Band 1) links neben der Ostleiter, Mitte x 1500. GEMESSEN: der
 *     Raum hat ZWEI getrennte Gehwege (Band 1: Kanal-Zellen 12/13 @0x012E0/@0x012EC sind fuer den Spieler fest, u0 = 01) -
 *     Nord/Ost mit Tuer 2070 und Ostleiter, Sued/West mit 2010/2050 und Westleiter. Leon bekommt die Steuerung im Nordgang:
 *     2000 sub02 Pos_set @0x0180C (4600,-1800,-21150), Szene in Cut 11 @0x017FE. Elza steht nach ihrer ersten Szene am
 *     Westgang (2001 sub02 Pos_set @0x0188E, Cut 4), muss aber fuer John (sub00 Slot 7 @0x017EA x 2200..6100 -> sub04
 *     Cut_chg 11 @0x01966) und Sherry (sub05 Cut_chg 11 @0x01A9E, Pos_set @0x01AA4 (597,-1800,-20962)) an diese Wand.
 *     Nur das Ostende ist fuer BEIDE sicher frueh; Cut 11 ist dort die Szenenkamera beider.
 *   Fabrik NUR ROOM30C1 (Elza), Blatt 7, Bit 35 (dasselbe Bit wie Leons Uebergabe in 3060, RE15_R42_BIT_PLAN_FABRIK),
 *     Kamera 1 = Ankunft aus 3091 (Slot 0 @0x01332 -> Cut 1; Pflicht: Marvin-Szene E31 30C1 sub00 @0x00BEE): Nordwand
 *     z = 1100 (SCA-Zelle 1 @0x007F8 x[-27200..-5300] z[1100..1600]) zwischen Treppenfuss (Zelle 10..13 x <= -9400)
 *     und Ostwand (Zelle 0 x = -5800). Konzept 6b woertlich: "Elza: an der Wand im Tresenbereich 30C1".
 *   U-Bahn ROOM6000/6001, Blatt 12, Bit 37 (RE15_R42_BIT_PLAN_UBAHN), Kamera 0 = Ankunft aus dem Hangar (50B0/50B1 Slot 1
 *     @0x004AE/@0x004B6 -> Cut 0, y -3600; Konzept L-24/E-18: beide gehen 50B0 -> 6000 -> 6030 Slot 3), Rueckwand des
 *     Treppenabsatzes x = 2025 (SCA-Zelle 17 @0x00828 x[25..2025] z[-2400..3725]). Capcoms gruene Tafel (Kamera 8) haengt
 *     bei z -22000 am Tor und kommt erst nach dem Generator (L-25/E-19) ins Bild — deshalb die Ankunftswand.
 *   GROESSE = RE2-Original: das karte01-MD1 UNSKALIERT (s = 1: 1080 x 720, MD1 @0x44), PORT-WAHL "wie ein Plakat".
 *   HOEHE: Mitte 2000 ueber dem Boden des Standorts (2000 Gehweg y -1800 aus 1260 Slot 1/4 @0x0114A/@0x011AA `f8 f8`; 30C1 y 0 aus
 *     3091 @0x01332; 6000 y -3600 aus 50B0 @0x004AE `f0 f1`) = PORT-WAHL (Augenhoehe-Plakat).
 *   TEXT im Rahmen RE2 room4040 @0x1B9B "There's a / sewer map. / Will you take it?" und @0x1BCB "You've taken / the sewer
 *     map."; "factory map" = RE2 room5040 @0x2341/@0x2376; "subway map" = PORT-WAHL (RE2 hat keinen U-Bahn-Plan).
 *   PROP-BAND = Band des Standorts + 1 = PORT-WAHL AUS MESSUNG (keine Disc-Regel). Mess-Beleg: die Klettersonde
 *     FUN_8002d474 laesst ein Objekt nur zu, wenn Objekt-Band == Spieler-Band (Objekt-Test FUN_8002d2c0 @0x8002d364-70,
 *     `lbu v0,130(s0)` / `bne v1,v0,0x8002d3a4` @0x8002d370), und der Kasten-Test FUN_8002da4c (@0x8002da8c-a4, Rand -112)
 *     trifft das flache Plakat fast ueberall. GEGENPROBE B (18.11., Prop-Band 1 im Gehweg-Band 1 von 2000): unit kanal rot
 *     (2000/2001, Klettersonde an 4 von 5 Standorten), integration_r42_lageplaene2_weg rot (Leon kletterte auf y -3600,
 *     F 16222) - mit +1 gruen. ⛔ Die frueher hier zitierten Wandobjekte der Disc zeigen KEINE +1-Regel (Pruefer s2 gelb 1,
 *     selbst nachgelesen, Obj_model_set pc[4] / pc[12..13] = y): ROOM2000 obj 0 @0x016E6 Band 2 bei y -900 (Gehweg -1800,
 *     Band 1), ROOM6000 obj 0 @0x00E16 Band 3 bei y 0, ROOM1200 obj 0 @0x007DA Band 1 bei y -2858 (= sein eigenes Band
 *     -(y/0x708)). Dieselbe FORM "eine Etage ueber dem Boden" haben die Null-Box-Requisiten ROOM1091 obj 2 @0x02202 (Band 2
 *     bei y -2653, Boden -1800 = Band 1) und ROOM1010 obj 0 @0x00930 (Band 1 bei y -1600, Boden 0) - Formbeispiele, keine Regel.
 *   SLOT 20 und Ereignis 28 in allen drei Raeumen frei (RDT-Saetze nur Slots 0..7, Kamerazonen ab 26/42/44; Subs < 28).
 *   NACHRICHTEN 30/31: RDT-Block 2000 12, 2001 15, 30C1 11, 6000 8, 6001 3 Eintraege; Port belegt dort 40 (2000/2001),
 *     21..26 (30C1), 0/3 als Neutext (6000) — 30/31 frei (analysis/befunde_runde42/belege/zensus_r42.txt).
 *
 * RUNDE 42 NACHARBEIT (Strang lageplaene_nach, nach den Pruefberichten s1/s2):
 *   DACH: der Revierplan schaltet zusaetzlich Blatt 5 frei (Hubschrauberlandeplatz ROOM1170/1171 = einziger Raum des Blatts,
 *     re15_map_zones.h Zeilen 0x1170/0x1171 Spalte Blatt 5). Orchestrator-Entscheid 18.11.2026: "das Dach gehoert zum Revier".
 *     Bank-9-Bit 42 = PORT-WAHL aus dem Zensus: frei im Mod-RDT-Zensus (bank9_zensus.py original, 206 RDTs), im Port-Zensus
 *     (bank9_zensus.py port), auf der 40-%-Disc (Version_40%/PSX, 96 RDTs) und in keinem r42-Zweig/keinem Tag v0.8.19..30 belegt.
 *     RE2-Form unveraendert: ein weiteres Set hinter den drei Set(33,2..4) @0x037DA-E2. Alte Spielstaende mit 87/88/89: der
 *     Plan haengt wieder (das Dach-Bit fehlt) und gibt beim Nehmen das Dach dazu.
 *   ORT REVIERPLAN NEU (Pruefer s1 gelb 6: an der Marke map1F.bmp nur ~11 x 8 Bildpunkte im 320er Bild): Ostwand der Lobby
 *     x = 2398 (ROOM1030 SCA-Zelle 1 @0x01A1C x[2398..4198]) neben der Ausgangstuer nach 1050 (Door_aot_set Slot 0 @0x01C6A
 *     x 1500..3000 z -13800..-11600), ueber der Notfall-Tafel (Aot_set Slot 9 @0x01D42 msg 4 @0x297F "Some sort of reference
 *     chart for / identifying emergency severity."). WARUM DORT: beide Figuren verlassen die Lobby durch diese Tuer (Leon L6
 *     ROOM1030 Slot 0 @0x01C6A -> 1050; Elza E2 -> E3 ROOM1031 Slot 0 @0x01CD8 -> 1051), und in den Tuerbereich fuehren nur
 *     die RVD-Zonen 1->2 (1030 @0x00294 / 1031 @0x002B4, x -2900..1800 z -15400..-10500) und 3->2 (@0x0030C / @0x0032C,
 *     x -3899..2907 z -15799..-10299) - es gibt keine Zone 0->2 und keine aus 4..8 nach 2 (sub00 Cut_replace @0x01FF4-FD
 *     tauscht nur 0/3/4/6 gegen 9..12, Kamera 2 bleibt). Kamera 2 (Cut @0x000A0 pos (-7668,-3114,-11934)) ist damit die
 *     Tuerkamera BEIDER Figuren (gemessen am echten Weg: Leon 4 -> 1 -> 2, Elza 6 -> 1 -> 2, integration_r42_lageplaene3_weg). Mitte (2398,-4150,-14900), Groesse s = 2 (2160 x 1440): im 320er Bild der Kamera 2
 *     x 117..161 y 41..74 (vorher Kamera 3 ~11 x 8). Mitte/Hoehe = PORT-WAHL am Hintergrundbild gemessen (ueber den Aushaengen,
 *     unter dem Deckenbalken). Aufhebe-Rechteck x 1598..2598 z -14800..-13800 im freien Streifen zwischen Bank/Tischreihe
 *     (SCA-Zelle 14 @0x01AB8 x[952..2752] z[-23204..-14744]) und Tuer; die Tafel (Slot 9, Rechteck 2384 x 1631) zieht dafuer
 *     unveraendert nach Slot 39 um (Aktions-Scan FUN_80042bac nimmt je Druck den KLEINSTEN treffenden Slot; 39 frei in
 *     1030/1031, Sonde probe_r34n_e_dokumente aots) - nach der Aufnahme antwortet dort wieder die Tafel. Das Muenztelefon
 *     (Slot 8 @0x01D2E) bleibt jetzt unberuehrt in Slot 8. Standort zum Nehmen z -14000 (gemessen: bei z -14300 haelt die
 *     Bank den Spieler auf).
 *   FABRIKPLAN 30C1 (Pruefer s2 gelb 2: ~35 x 25 Bildpunkte bei 640 x 480): gleiche Stelle, s = 2; Kamera 1 x 188..229
 *     y 108..136 (320er Bild, vorher 198..218 x 115..129).
 *   GROESSE s = 2 = PORT-WAHL (NUTZERWUNSCH 18.11. "gut SICHTBAR"). Massstab-Bezug: RE2s Wandplaene karte01/karte02 sind
 *     1080 x 720 (room3060/4040 bzw. room5040/5060, MD1 @0x44); RE2s Polizeiplaene room20B0 obj 8 und room2130 obj 0 sind
 *     KEINE Wandplaene, sondern das gefaltete Faltblatt karte00 (room20B0 MD1 @0x004FDC, 1428 B, md5 735097e1, Ecken
 *     x -56..56, y -96..1, z -269..269 = 112 x 538) - als Plakat-Massstab nicht brauchbar.
 *
 * STRANG LAGEPLAENE_NACH2 (18.11.2026) - REVIERPLAN AUF AUGENHOEHE:
 *   NUTZERWUNSCH 18.11.2026 woertlich: "Bei 1030 und 1031 passt das nicht. Das der Revierplan so weit über den Spieler
 *     hängt, das geht so nicht." (Stand 10287b077: Mitte y -4150, Unterkante -3430 = 410 ueber Leons Scheitel -3020.)
 *   KOPFHOEHE GEMESSEN aus den Knochenlagen (Riegel unit_r42_lageplaene4_hoehe, Formel skeleton_common.c Plc_neck-Ursprung,
 *     neck_bone 8 @0x80031938; Clip 0 Bild 0 = RE15_R36_KOPF_CLIP): Kopfband = Ursprung Knochen 8 .. hoechste Ecke des Kopf-
 *     Meshs. Leon PLD/PL00.PLD -2513..-3009, Elza PLD/PL04.PLD -2332..-2778 (Boden 0). Zum Vergleich (Hintergrund Kamera 2,
 *     Bildpunkte auf die Wand x 2398 zurueckgerechnet): WANTED-Aushang rechts der Tuer Bild x 128..149 y 78..103 -> z -14260..
 *     -15391, y -1950..-3230 (Mitte -2590); zweiter Aushang y -2147..-3135 (Mitte -2641). Die Plaene 2000/30C1/6000 haengen
 *     2000 ueber dem Boden (unter beiden Hals-Ursprungen), der Kellerplan 1200 bei y -2573.
 *   ORT: Ostwand x 2398 LINKS neben der Ausgangstuer - das freie Wandstueck zwischen Door_aot_set Slot 0 (@0x01C6A / 1031
 *     @0x01CD8, z bis -11600) und dem Ende der SCA-Zelle 1 (@0x01A1C / @0x01A80, z -9632), Mitte z -10616. Rechts der Tuer
 *     ist auf Augenhoehe kein Platz (Aushaenge ab z -14260, davor Bank/Tischreihe SCA-Zelle 14). Kamera 2 bleibt die
 *     Tuerkamera beider Figuren (RVD 1->2 / 3->2, Abschnitt NACHARBEIT); der Fernseher am Pfeiler steht im Bild rechts,
 *     weit weg von dieser Stelle.
 *   MITTE y -2658 = Mittel der beiden Kopfband-Mitten (-2761, -2555). GROESSE s = 1.5 (1620 x 1080) = PORT-WAHL: passt mit
 *     je 174 Rand zwischen Tuerkante und Wandende und deckt beide Kopfbaender ganz (y -2118..-3198).
 *   AUFHEBEN: Rechteck x 1598..2598 z -12010..-11010 (Slot 21, frei in 1030/1031). Der Tresen (SCA-Zelle 0 @0x01A10,
 *     z >= -10568) haelt den Spieler bei z -11036 an (gemessen), sein Nachrichtensatz Slot 10 (@0x01D56, z -11000..-7839)
 *     wird nicht beruehrt; ueber der Tuer (z <= -11600) gewinnt Slot 0 den Aktions-Scan (kleinster Slot, FUN_80042bac).
 *     Die Notfall-Tafel bleibt in Slot 9 (kein Umzug mehr). Klettersperre unveraendert: Prop-Band = Standort-Band + 1.
 *
 * NUTZER-VORGABE (AUFTRAG.md Z. 73-77, woertlich): "Extrahiere mir Karte01 oder Karte02 ... Haenge sie
 *   mir beispielhaft an die Stelle von map3F.bmp, und schalte damit die Map fuer Etage 3F frei, wie das
 *   Kartensystem in Resident Evil 2 auch funktioniert. Auch mit den Text." + 2F ROOM10D0 (map2F.bmp),
 *   1F ROOM1030 (map1F.bmp), B2 ROOM1200 (mapB2.bmp), B1 ROOM11D0 (mapB1.bmp).
 *   ⛔ map3F.bmp ist ROOM1140 Cut 5 (Bildvergleich 8,5 gegen >= 113,8 fuer alle ROOM1150-Cuts), nicht
 *   ROOM1150 wie AUFTRAG.md annahm; Blatt 4 = 3F (re15_map_zones.h Zeile 0x1140).
 *
 * WAS DAS ORIGINAL VORGIBT (selbst disassembliert / aus den RDT-Bytes):
 *   FORM DER AUFNAHME — RE1.5 kennt sie selbst: ROOM5030 sub02 (Laborplan) @0x010AC
 *     `2b 00 ff ff` Frage, `02 00`, `06 00 1e 00` Ifel_ck, `21 0c 1f 00` Ck(12,31,0) = Ja,
 *     `46 05 ..` Aot_reset(5,0), `09 0a 01 00` Sleep 1, `22 03 73 01` Set, `2b 01 ff ff` "You've taken
 *     the laboratory map.", `02 00`, `08 00`, `01 00`; Ausloeser sub00 @0x0108A Aot_set Slot 5 sce 3
 *     sat 0x31 p0 0x00FF p1 0x0218.
 *   RE2 (Ziel, wo RE1.5 nichts hat): room20B0 sub10 @0x037BA..0x0381A — dieselbe Folge plus Besitz-
 *     Bits Set(33,2..4) @0x037DA-E2, Etagen-Wahl Set(35,2..4) @0x037E6-EE und das Wegraeumen des
 *     Weltmodells `2e 04 08` Work_set(Objekt 8) @0x037F2 + `32 00 20 4e 20 4e 20 4e` Pos_set
 *     (20000,20000,20000) @0x037F6, dann `02 00` @0x037FE und die Meldung @0x03800.
 *   ZEICHNER — RE2 FUN_8006E120: Bank 33 (0x800D4924, Zeigertabelle 0x800A78C8 + 33*4 = 0x800A794C)
 *     `jal 0x80077360` @0x8006E66C; mit Plan unbesuchte Raeume in CLUT-Zeile 498 (`li s5,0x1f2`
 *     @0x8006E71C), ohne Plan uebersprungen (`beq v0,zero,0x8006e768` @0x8006E744). Der Port hat das
 *     Gatter schon (re15_map_owned_page); diese Datei liefert die Fundstellen der Polizei-Blaetter.
 *   ETAGENWAHL — RE2 Bank 35 (0x800D4908): `jal 0x80077360` @0x8006D934 / @0x8006D998; die Aufnahme
 *     setzt sie (room20B0 @0x037E6-EE). Port: re15_map_blatt_waehlbar fragt den Besitz mit ab.
 *   TEXT — RE2 room2130 sub00 / sub12 (Bytes Dossier 2.4), RE1.5-Rahmen ROOM2030 msg 2/3.
 *   LICHT — RE1.5 FUN_8002c18c (Objekt-Zeichner): `lw v0,0xc(s1)` @0x8002c2ec, `andi v0,v0,0x10`
 *     @0x8002c2f4, `beq` @0x8002c2f8: Attribut-Bit 0x10 (Pool+0x0c = Obj_model_set pc[8..9],
 *     LAB_80040914 @0x800409a8/@0x800409b0) -> FLACH ohne Raumlicht ueber FUN_80022f0c @0x8002c320
 *     (a2 = obj+0x70 = 0x808080 aus FUN_8002b898.c:16 = Textur unveraendert), sonst NCCT. RE2 zeichnet
 *     seine Wandplaene genau so: room20B0 obj 8 @0x03158 und room2130 obj 0 @0x00FDC tragen das
 *     Attributwort 0x0010 (FUN_800363f0.c:41-54 `puVar4[4] & 0x10` -> FUN_8002d0e8/FUN_8002d244
 *     unbeleuchtet; FUN_80035710.c:16 0x808080). In RE1.5 tragen 260 Objekte in 46 Raeumen dasselbe
 *     Bit, darunter ROOM1200 obj 0/1 (@0x007DA/@0x00804) an derselben Wand wie die B2-Karte. Die
 *     Karte erhaelt deshalb attr = RE15_R36K_PROP_ATTR; der Prop-Zeichner (platform/pc/main.c) fuehrt
 *     den Flach-Zweig fuer JEDES Objekt mit dem Bit (Nachbesserung 1, Dossier Abschnitt 8).
 *
 * PORT-WAHL (Messung, Dossier 3): Ebene, Mitte, Massstab (Karte innerhalb der Marke, 3:2), Drehung,
 *   Aufhebe-Rechteck 1000 x 1000 um Mitte + 300 Einheiten zur Raumseite — alles erzeugt von
 *   re15_port/tools/r36_kartenitems/karten_export.py -> engine/src/gen/kartenitems_r36.inc.
 *
 * BANK-9-BITS "Plan genommen" (Nachtrag kartenbits, Dossier Abschnitt 9): 3F = 87, 2F = 88, 1F = 89,
 *   B2 = 77, B1 = 78. Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9] @0x80074688) ist im Original die Zone
 *   der Item-TAKEN-Bits: Item_aot_set @0x80040644 liest das Bit als u16 pc[18] (@0x80040680, Langform pc[26]
 *   @0x80040670) und legt den Satz still, wenn es steht (`jal 0x8004efe4` @0x800406dc, `sb zero,0(v0)`
 *   @0x800406f4); Ck @0x8003fcf4 / Set @0x8003fdd0 treffen ueber dieselbe Tabelle dasselbe Bit. Ein
 *   Lageplan-Bit darf deshalb KEIN Original-Bit sein. Zensus (analysis/befunde_runde36/N_belege/
 *   bank9_zensus.txt): 85 von 256 Bits belegt, frei u.a. 53..84 und 87..89. B2/B1 lagen bis zum Nachtrag
 *   auf 90/91 = ROOM2090/2091 main00 @0x00A12 Item 0x3F (Bit 0x5a) und ROOM2040/2041 main00 @0x01364
 *   Item 0x32 (Bit 0x5b) — wer den Plan nahm, fand diese Items in Stage 2 nicht. 77/78: im Original frei,
 *   in keiner Port-Version je benutzt (git log --all, Tags v0.8.21-23). NICHT frei, obwohl im VERTRAG 1.1
 *   als Reserve gefuehrt: 85 (Green Keycard ROOM3040 @0x014B2), 86 (Spark Plug ROOM4040 @0x00F66),
 *   92 (Super Redhawk ROOM3050 @0x01EC6). Riegel: unit_r36_kartenbits.
 */
#ifndef RE15_KARTENITEMS_R36_H
#define RE15_KARTENITEMS_R36_H

#include <stdint.h>

#define RE15_R36K_MAX_BLAETTER 4   /* Revierplan: 1F/2F/3F + Dach (Nacharbeit, Orchestrator-Entscheid 18.11.) */

typedef struct {
    uint16_t raum;             /* Leon-Variante; Elza = raum | 1 (derselbe Ort)                        */
    uint8_t  blatt;            /* Kartenblatt der FUNDSTELLE (re15_map_zones.h) = blaetter[0]          */
    uint8_t  bit;              /* Bank-9-Bit der Fundstelle = bits[0] (Kopf BANK-9-BITS)               */
    uint8_t  aot_slot;         /* Slot des Aufhebe-Ereignisses                                         */
    uint8_t  verdraengt_nach;  /* 0xFF = keiner; sonst: der Satz in aot_slot zieht dorthin um (Vorrang) */
    int32_t  x, y, z;          /* Prop-Ursprung (Welt)                                                  */
    int16_t  rot_x, rot_y, rot_z;   /* Pool +0x68/+0x6A/+0x6C (scd_vm.c Member 3..5)                    */
    int32_t  rect_x, rect_z;   /* Ecke des Aufhebe-Rechtecks, Groesse RE15_R36K_RECT                    */
    uint8_t  n_blaetter;       /* Runde 42: Blaetter dieses Plans (Revier 4, Keller 2). 0 = Zeile im alten 13-Felder-
                                * Format: gilt als Ein-Blatt-Plan mit blatt/bit (kartenitems_r36.c n_von, seit bb87d614c) */
    uint8_t  blaetter[RE15_R36K_MAX_BLAETTER];  /* 5 = Dach, 4 = 3F, 3 = 2F, 2 = 1F, 1 = B2, 0 = B1      */
    uint8_t  bits[RE15_R36K_MAX_BLAETTER];      /* je Blatt sein Bank-9-Bit: 89/88/87/42 bzw. 77/78     */
    uint8_t  variante;         /* Runde 42 Stufe 2: RE15_R36K_VARIANTE_* (0 = beide Fassungen des Raums)  */
    uint8_t  band;             /* Runde 42 Stufe 2: Aot_set pc[4] der Zone = Boden-Band des Standorts -(y/0x708)
                                * (re15_collision.c:186; Scan-Gatter @0x80042cac); Stufe 1: 0 wie ROOM5030       */
} re15_kartenitem_r36_t;

/* Runde 42 Stufe 2: Fassung des Raums, in der der Plan haengt. Der Fabrikplan haengt NUR in Elzas 30C1: Leon spielt 30C0
 * als Marvin (Konzept L-06) und bekommt den Plan erst danach in 3060 von Marvin (L-09, story3a_r42.c, Bit 35). */
#define RE15_R36K_VARIANTE_BEIDE 0   /* raum und raum|1                                                            */
#define RE15_R36K_VARIANTE_LEON  1   /* nur raum (gerade Id)                                                       */
#define RE15_R36K_VARIANTE_ELZA  2   /* nur raum|1 (ungerade Id)                                                   */

#define RE15_R36K_ANZAHL       5   /* Runde 42: Revierplan 1030, Kellerplan 1200 (Stufe 1); Kanalisation 2000, Fabrik 30C1 (Elza),
                                    * U-Bahn 6000 (Stufe 2). Forschungszentrum 4020 baut r42/rest, Labor 5030 ist Original. */
#define RE15_R36K_BANK         9         /* Bank-9-Bits (gespeichert mit g_game.flags)                   */
#define RE15_R36K_EREIGNIS     28        /* VERTRAG.md 1.3 Spur N                                       */
#define RE15_R36K_OBJ          15        /* Prop-Pool-Id; TIM-Slot RE15_TIM_SLOT_PROP(15) = 35; kein Raum
                                          * der fuenf (Runde 36) hat nOmodel > 8, kein Port-Prop nutzt 8..16 */
#define RE15_R36K_MSG_FRAGE    30        /* VERTRAG.md 1.2 (ROOM1150-Liste: 28/29 belegt -> 30/31)      */
#define RE15_R36K_MSG_GENOMMEN 31
#define RE15_R36K_RECT         1000      /* r35-Muster: haeufigste Item-Groesse (69 von 162)            */
#define RE15_R36K_AOT_SCE      3         /* EVENT wie ROOM5030 sub00 @0x0108A (`2c 05 03 ..`)            */
#define RE15_R36K_AOT_SAT      0x31      /* Aktion+vorne+Spieler wie ROOM5030 (0xB1 = 0x31|Viereck)      */
#define RE15_R36K_AOT_P0       0x00FFu   /* ROOM5030 sub00 Payload +0 `ff 00`                            */
#define RE15_R36K_PROP_FLAGS   0x000B    /* 0x000A | 1 (@0x80040998), wie r35 RE15_R35I_PROP_FLAGS       */
#define RE15_R36K_PROP_ATTR    0x0010    /* Pool+0x0c Flach-Bit (Obj_model_set pc[8..9] @0x800409a8/b0):
                                          * RE2 room20B0 obj 8 @0x03158 / room2130 obj 0 @0x00FDC = 0x0010;
                                          * RE1.5 ROOM1200 obj 0/1 @0x007DA/@0x00804 = 0x0010; Zeichner
                                          * FUN_8002c18c @0x8002c2f4 -> FUN_80022f0c flach, 0x808080      */
#define RE15_R36K_PROP_BAND    1         /* wie r35 RE15_R35I_PROP_BAND (100 von 121 Item-Props); Runde 42: Abstand
                                          * ueber dem Band des Standorts (Prop-Band = band + 1, kartenitems_r36.c) */

/* Prop + Aufhebe-Zone beim Raumstart anlegen (nur solange EINES der Bank-9-Bits des Plans fehlt). Gerufen an BEIDEN
 * Raumstart-Wegen (scd_room_setup.c, platform/pc/main.c Boot/CONTINUE) nach dem Init-Lauf von main00.
 * Idempotent; tut in jedem anderen Raum nichts. */
void re15_kartenitems_r36_install(uint16_t room_id);

/* HAKEN scd_event_fire: das Port-Programm fuer (Kartenraum, Ereignis 28), solange die Karte haengt;
 * setzt dabei die Nachrichten 30/31 des Raums. Sonst NULL. */
const uint8_t *re15_kartenitems_r36_ereignis(uint16_t room_id, uint8_t event_id);

/* Plattform-Lader: obj_id/MD1/TIM des Karten-Props in diesem Raum (beide Varianten), sonst -1/NULL. */
int            re15_kartenitems_r36_obj_id(uint16_t room_id);
const uint8_t *re15_kartenitems_r36_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_kartenitems_r36_tim_bytes(uint16_t room_id, int *out_size);

/* HAKEN re15_map_owned.c: Besitz eines Kartenblatts aus den Bank-9-Bits 87/88/89/77/78 (Gegenstueck zu RE2
 * Bank 33 @0x8006E66C). Runde 42: je Blatt sein Bit (blaetter[]/bits[] aller Plaene). */
int      re15_kartenitems_r36_blatt_besitz(unsigned page);
uint32_t re15_kartenitems_r36_besitz_bits(void);

/* Sonden/Riegel. */
int                          re15_kartenitems_r36_anzahl(void);
const re15_kartenitem_r36_t *re15_kartenitems_r36_zeile(int i);
const re15_kartenitem_r36_t *re15_kartenitems_r36_des_raums(uint16_t room_id);
const uint8_t *re15_kartenitems_r36_programm(uint16_t room_id, int *out_len);
/* welche: 0 = Frage (Nachricht 30), 1 = "You've taken ..." (31). */
/* Runde 42: 1 = alle Bits des Plans gesetzt (dann haengt er nicht mehr). */
int re15_kartenitems_r36_genommen(const re15_kartenitem_r36_t *z);
const uint8_t *re15_kartenitems_r36_meldung(uint16_t room_id, int welche, int *out_len);

#endif /* RE15_KARTENITEMS_R36_H */
