/*
 * RE1.5 Rebuilt — Runde 41 Spur C "ventilkette": die STAGE2-Kette Leiter 2000 / Bruecke + Ventile 2030 /
 * Flut -> 20B0 / Schaltpult 2050 / Rechner 2080.
 *
 * NUTZER-VORGABE (AUFTRAG.md "Vorab vereinbart", 2026-10-23, woertlich):
 *   "in ROOM 2000 die Leiter zu ROOM 2050 mit herunterlaufenden Wasserstrom zu blockieren mit einer Message wie
 *    'I cannot climb up the ladder because of the water pressure'. Dann wollte ich das man das Valve Raetsel in
 *    2030 loest. Damit sollte dann eine Warnung kommen, das das Wasser in einer Minute steigt, und man den Raum
 *    verlassen soll. Verlaesst man den Raum fuehrt das zu ROOM 20B0. spaeter sollte der Raum wieder frei werden
 *    ohne Wasser. Nach dem Steigen des Wassers dort, sollte die Leiter frei sein, also kein Wasser mehr runter
 *    laufen. ... das man das Wasserlevel vielleicht im Raum [2050] nicht steigen lassen kann, bevor man das
 *    Raetsel loest. Und die Herunterfahrbare Plattform, bei der ich auch toll finde, wenn die vorher oben waere
 *    und man etwas machen muss, damit sie runter faehrt ... eher das Pult auf der linken Seite ...
 *    Bruecken-Pult: Irgendein kleines Raetsel waere gut ... Wasser in 2030: Ueber die Ringebene ...
 *    Ventilraetsel: einverstanden. Zeit abgelaufen und noch im Raum: Tot mit ertrink Effekt. Ventil 13 als
 *    Bruecken-Raetsel: Einverstanden."
 *   Punkt 6 (VERTRAG): "ROOM 2080: der Rechner, der das Wasser in 2080 senkt, entleert danach auch 2030" —
 *   ERSETZT durch die Nachtraege 7-10 (AUFTRAG.md 2026-10-24, Wortlaut):
 *   Nachtrag 7: "Nach der Aktivierung des Stroms ist 2030 leer gelaufen." (Rechner 2080 nur Original)
 *   Nachtrag 8: "der Wasser pegel in 2030 kann dann noch auf reling hoehe sein. also quasi nur noch der untere
 *                bereich geflutet nach strom aktivierung ueber generator."
 *   Nachtrag 9: "und die bruecke in 2030 muss man entsprechd vorher runtergelassen haben, damit man das valve
 *                raetsel vorher loesen kann."
 *   Nachtrag 10 (Auswahl): "Flut erst mit Sicherung" — die Ventile lassen sich erst drehen, wenn die Sicherung
 *                aufgehoben ist (vorher ein kurzer Text am Ventil).
 *   Nachtrag 12 (k): "Wasserstand nach dem Strom: unterhalb der Absperrung, sodass es nicht drueber 'rein' laufen wuerde."
 *   Nachtrag 13: das echte Ventilraetsel — Dokument mit dem Nutzer-Text ("These amateurs ... They mixed up black, green,
 *                and red, and swapped the numbers for black! ..."), ein Plakat mit der Anleitung, "nur eben genau falsch
 *                entsprechend dem Dokument" (Nachbesserung 4, Dossier §0e).
 *   Nachtrag 16 (ersetzt Nachtrag 10): "einen zweiten Sicherungskasten ... ausserhalb des Raumes, wo man die Sicherung
 *                zuerst einsetzen muss. Macht man das geht die Kamera auf die Ventile und man hoert ein unlock geraeusch
 *                ... Ist sie raus, sollen die valves wieder blockiert werden ... im ROOM 2010" (Nachbesserung 5, §0f).
 *   Nachtrag 18: Plakat im Stil des Spiels; 2050-Hebel gehen beim Umlegen runter; Ventil-Dreh-Animation aus RE2;
 *                Timer der 60 s + Alarm + Sprachausgabe beim Warntext.
 *   Nachtrag 19: Text 46 = "WARNING: The water will rise in one minute. Please leave the room immediately!"
 *
 * Dossier: analysis/befunde_runde41/C_ventilkette.md (RE-Belege §1, Umsetzung §3). Kein Asset-Patch: die RDTs
 * bleiben byte-true; alles hier wird nach dem Init-Lauf des Raums angelegt (scd_room_setup.c / main.c Boot-Weg).
 *
 * ============================================================================================================
 * ORIGINAL-BELEGE (Bytes selbst gelesen, tools/scd_dump_room.py; Datei-Offsets in shared_assets/PSX/STAGE2/)
 * ============================================================================================================
 * ROOM2000 Leiter = main00 @0x0169E Door_aot_set Slot 4 `3b 04 02 31 01 00 30 94 48 26 e8 03 e8 03 8c 23 e8 ea 04 10
 *   00 04 01 05 00 03 ...` (Rechteck (-27600,9800,1000,1000) Band 1 -> ROOM2050 Cut 0). Die Wasserstrahlen an der
 *   Leiterwand: main00 @0x0174C/@0x0175C/@0x0176C `3a 00 0b ...` (immer an). Kein Skript liest (3,235); keine Tuer
 *   fuehrt nach ROOM20B0 (Tuertabellen gen/tuer_zuordnung.inc + gen/re15_tuer_eigen.inc: 0 Treffer).
 * FORM "Tuer wird Sperrtext": ROOM2030 main00 @0x01B0A Ifel_ck / @0x01B0E Ck(3,219)=0 -> @0x01B12 Aot_set Slot 2
 *   sce 1 msg 0 im Tuer-Rechteck, Else @0x01B2A Door_aot_set. (= Aot_reset-Form `46 ss 01 31 mm 00 ff ff 00 00`,
 *   ROOM2030 sub01 @0x01E18.)
 * ROOM20B0 = geflutete 2000-Variante: sub00 @0x01AAA Ck(3,235)=0 -> 35 Emitter EM024 @0x01AAE.. (Wasserwand
 *   x 7200 vor der Ost-Tuer), Zone Slot 7 @0x01DF2 -> sub05 Auswasch-Szene, @0x01E06 Evt_exec sub02 (Eintritt:
 *   Pos_set (2380,-1800,-6355) @0x01F08, Cut 11 @0x01F00); sub05 setzt @0x01FBC `22 03 eb 01` Set(3,235)=1.
 *   Else-Zweig @0x01E0A: die drei Strahlen @0x01E96/@0x01EA6/@0x01EB6 (gleiche Bytes wie 2000). Tuer Slot 3
 *   @0x019C2 fuehrt direkt nach ROOM2030 Cut 0 (6550,-5400,-100) Richtung 2048 — in 2000 fuehrt Slot 3 nach 2070.
 *   Kameras: 20B0 Cut 0..4 = 2000 Cut 0..4, 20B0 5/6/7/8/9 = 2000 6/7/8/9/10, 20B0 10 = 2000 14 (Tabellen
 *   RDT+0x60, 32 B je Cut, selbst verglichen). Tuern nach 2000 tragen Cut 0 (1260 @0x114A), 4 (1260 @0x11AA),
 *   10 (2050 @0x1C4A), 13 (2070 @0xA3A/@0xA9A), 14 (2010 @0x746).
 * ROOM2030 Bruecke = main00 @0x01B6C Obj_model_set obj 0 `2d 00 00 00 00 00 0a 00 00 00 cd fb f8 f8 ed fe ...`
 *   (Typ 0, Band 0, Lage (-1075,-1800,-275); MD1 @0x0020CC 636 B: 18 Dreiecke, x -899..900, z -5685..5686, y 0).
 *   Von keinem Skript bewegt. Ebenen: Ring Band 3 y -5400, Mittelsteg Band 1 y -1800 (PSX-y: kleiner = hoeher).
 *   Begehbare Steg-Zelle: SCA Region 0..4 Zelle 12 (-2050,-5950,2000,11400) u0 00 Band 1 (in allen 5 Regionen
 *   gleich); Sperren per Sca_id_set-Form ROOM2050 sub04 @0x02346 `37 0r 32 ff` (Region r, Zelle, u0 := ff).
 * FORM Objekt-Fahrt (derselbe Raum): sub03 @0x01ED2 `2e 03 01` Work_set, @0x01ED6 `2f 01 88 ff` Speed_set(1,-120),
 *   @0x01EDA `0d 00 06 00 2d 00` For 45 { `30` Add_speed `02` `02` Evt_next x2 `00` }, @0x01EE6 `34 01 d0 d5`
 *   Member_set(1,Endlage); 45 x 120 = 5400 = Hoehendifferenz Ring/Buero. Pad-Sperre `22 02 07 01` @0x01E30,
 *   Cut_auto 0 @0x01E34, Fahrt-Kamera `29 0a` @0x01E60, Rueckweg `3c 01` @0x01EC2, `22 02 07 00` @0x01EC4.
 * ROOM2030 Stelle Pult: Nordwand z -10350 (SCA Zelle 24 Band 3 z -12350..-10350), Rueckprojektion des roten
 *   Markers in schaltschrank.png (Cut 2, Bild (45,128)) auf z = -10350 -> Welt (-89,-7079); Pultkanten Bild
 *   (30,60)/(20,180) -> x 125..706, y -8506..-6242. Nordstreifen des Rings z -10350..-9000 (Gelaender Zelle 20).
 * ROOM2030 Stelle Ventil 13: der Tank rechts der Treppe (schaltschrank.png) = SCA Zelle 2 Band 1 (-3050,-6900,
 *   1000,500) am Fuss der Treppe (Zone Slot 8 sce 12 @0x01B8E (-6050,-8400,3700,1400)); Landeplatz Band 1
 *   x -2550..-50, z -8500..-6900 (Zellen 1/6/11 ringsum).
 * ROOM2030 Stelle Ventile (Nachbesserung 3, Nachtrag 9): die nummerierten Ventile liegen ALLE im unteren Bereich
 *   (BSS-Cuts mit projizierten SCA-Bodenzellen, Dossier §0d B3, C_belege/nb3_ventile_lage_sheet.png): 01/02/03 an der
 *   Nordwand ueber dem Nord-Landeplatz (Zelle 13, Cut 3/14), 04/05/06 an der Ostwand unter der Ring-Reling (Cut 0/15),
 *   07/08/09 an der Suedwand ueber dem Sued-Landeplatz (Zelle 16 x -5600..4800 z 6350..7900, Cut 5/6/7), 13 am Tank.
 * ROOM2030 Ventil-RAEDER (Nachbesserung 4, Dossier §0e B4-2, C_belege/nb4_ventile_raeder_sheet.png): Rueckprojektion der
 *   Radmitten aus den BSS-Bildern auf die Wandflaeche (Nordwand z -8500 = Zelle 6 @0x008E0 Innenkante, Suedwand z 7900 =
 *   Zelle 7 @0x008EC Innenkante): Nord (Cut 3/14) rot x 2367, gruen x 171, schwarz x -1360 zwischen den Schildern 03 x 4422 /
 *   02 x 1413 / 01 x -2584; Sued (Cut 7, Cut 5/6 +- 110) rot x -2637, gruen x 453, schwarz x 2675 zwischen 09 x -2465 /
 *   08 x 1358 / 07 x 4335. Zuordnung PORT-WAHL "ein Rad je Schild in Wandreihenfolge": 03 rot, 02 gruen, 01 schwarz;
 *   09 rot, 08 gruen, 07 schwarz (das naechste Schild ist fuer zwei Raeder nicht eindeutig). Ostwand 04-06: keine Raeder.
 *   Plaetze 1000 x 1000 (Groesse der Original-Plaetze, re15_dokumente.h "69 von 162") um die Radmitte (x auf 50 gerundet)
 *   und die Wandflaeche +- 500 (Form des Pult-Platzes), Band 1; frei von Slot 8/9/10 und voneinander.
 * ROOM2030 Ventilraetsel (Nachbesserung 4, Nachtrag 13, Dossier §0e B4-3): Form "nacheinander stellen, am Ende pruefen,
 *   falsch -> alles zurueck" = RE2 ROOM2130 sub04 (Pruefung erst @0x01752 `23 00 05 00 50 00`, sonst Meldung @0x017D6/@0x017E8
 *   + Fehlerton `36 02 0b 01` @0x01800; Neuanfang mit Zuruecksetzen @0x01138/@0x01144..@0x01188) und RE1.5 ROOM11F0 sub17
 *   (Hebel in die Grundstellung @0x0160E..@0x0167A, Schalterbits (5,13..22) := 0 @0x016C0..@0x016E4). Zustand im Raum-
 *   Scratch Bank 5 wie die Schalter ROOM2060 (5,6..9) @0x01296..@0x012A6 (Raumladen wischt Wort 0 @0x8003ed74).
 * ROOM2030 Sicherung aufgehoben = (3,108): Setzer NUR ROOM2030/2031 sub06 @0x01FF2 `22 03 6c 01` (Ja auf msg 2
 *   "Will you take the Fuse?"), Leser main00 @0x01BA6 `21 03 6c 00` (Platz Slot 9 nur bei 0) und ROOM2060 sub00
 *   @0x010A2 (Sicherungskasten); kein Loescher in irgendeinem RDT (Zensus, Dossier §0d B3). Taken-Bit, kein Inventar.
 * ROOM2060 Strom = (3,0x7E): Setzer NUR ROOM2060/2061 sub01 @0x012AE `22 03 7e 01` (Loesung @0x01296..@0x012A6
 *   Schalter (5,6..9) = 1,0,1,0); kein Loescher (Zensus aller RDTs). Leser ROOM2030/2031 sub01 @0x01E14 `21 03 7e 00`
 *   -> @0x01E18/@0x01E22 untere Lift-Rufzonen Slot 5/7 = Text msg 1 ohne Strom (sub01 laeuft jedes Bild, FUN_8003f038).
 * ROOM2030 Treppe = main00 @0x01B8E Aot_set Slot 8 sce 12 `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00 02 00 00 00`
 *   (-6050,-8400,3700,1400), Band 0x80 = jedes Band (FUN_80042cac); der EINZIGE Weg vom Ring (Band 3) hinab zum
 *   Nord-Landeplatz (Band 1): Lift A faehrt Ring <-> Buero (Band 6, sub00 @0x01CD2/@0x01CE6), Lift B Ring <->
 *   Suedgalerie (Band 9, @0x01D74/@0x01D88).
 * ROOM2030 Relinghoehe (Nachtrag 8): SCA @0x880 (12 B je Zelle w,d,x,z,typ,u0,u1,fl; Band = fl >> 4). Untere Reling =
 *   Wandzellen Band 1 fl 0x11 (Steg 3/4 @0x008BC/@0x008C8, Landeplaetze 5/10/11 @0x008D4/@0x00910/@0x0091C); ihr oberer
 *   Handlauf liegt im BSS-Bild (Cut 3 Zelle 11, Cut 5 Zelle 10, Cut 6 Zelle 5) 1800 ueber dem Landeplatz y -1800 =
 *   y -3600 = Band 2 (C_belege/nb3_reling_hoehe_sheet.png); die Ring-Bodenzellen 36-41 @0x00A48..@0x00A84 tragen fl 0x23
 *   (Band 2..3); ROOM2050 haelt sein Wasser auf demselben Mass y -3600 @0x01F88 (1800 unter der Laufebene -5400).
 *   Schachtoeffnung (Innenkanten der Ring-Reling Band 3 fl 0x33): Nord z -8500 (Zelle 20 @0x00988), Ost x 5400 (Zelle 19
 *   @0x0097C), Sued z 8150 (Zelle 21 @0x00994), SW x -5850 (Zelle 22 @0x009A0), NW-Balkon x -5400 / z -6400 (Zellen 32/31
 *   @0x00A18/@0x00A0C), West bis zur Aussenwand x -7700 (Zellen 26/27 @0x009D0/@0x009DC) zwischen z -6400 und 2450
 *   (Zelle 23 @0x009AC).
 * ROOM2030 Pegel nach dem Strom (Nachbesserung 4, Nachtrag 12 (k), Dossier §0e B4-1, C_belege/nb4_absperrung_sheet.png): die
 *   Absperrung am inneren Ringrand = die Wandzellen 19/20/21/22/31/32 fl 0x33 (nur Band 3, keine Hoehe) = im Bild ein
 *   DURCHSICHTIGES Stabgelaender mit Handlauf -> Grenze = Ringboden y -5400 (Bodenzellen 36-41 fl 0x23 u1 01, Ankunft der
 *   Ring-Tueren nfloor 3); Pegel = -5400 + 50 = -5350 (ein Schritt der Wasserrate ROOM2080 sub02 @0x00CC8 `2f 01 32 00`,
 *   PORT-WAHL "knapp darunter"). Die Treppe (Slot 8, von -5400 bei x -6050 auf -1800 bei x -2350) liegt damit bis auf die
 *   oberste Stufe UNTER dem Pegel -> der Treppenschacht x -5400..-4200 z -8500..-6900 bekommt Flaeche (bei -3600 ausgespart).
 * ROOM2030 Kameras: Cut 4 @0x000E0 (-4253,-9340,7013)->(-1841,-4606,2549) = Draufsicht auf Becken + Steg
 *   (Projektion des Stegs: Bild x 100..210, y 73..221) = Fahrt-Kamera der Bruecke und der Flut (PORT-WAHL).
 * ROOM2050 Hebel = Slot 6 sce 3 Ereignis 4 (sub00 @0x01E28), sub04 @0x022E6 (Frage msg 0, Set(3,104),
 *   Speed_set(1,-20)/For 180). Schaltpult = main00 @0x01D30 Ifel_ck/@0x01D34 Ck(5,7)=1 -> @0x01D38 Slot 8 sce 3
 *   Ereignis 9 (4000,-8450,1800,800) Band 3 + @0x01D4C Cursor obj 8 (Typ 4, (-19554,0,22684)); (5,7) wird nirgends
 *   gesetzt (tot). sub09 @0x023F2 `29 0f` Cut 15, Slot 8 aus, `22 05 04 01` `22 05 05 01` `22 05 06 01`; sub01
 *   @0x02182..@0x021E0 (5,4)=1 & Sce_key_ck 0x01/0x04/0x02/0x08 -> sub05..08 (Cursor +-200 z/x), @0x021E2..@0x021F8
 *   (5,5)=1 & 0x40 -> sub10 (Ausstieg @0x0240C: Cursor parken, Slot 8 wieder sce 3 Ereignis 9, (5,4..6)=0,
 *   (2,0)/(2,2)=0, Cut_old, Cut_auto 1), @0x021FA (5,6)=1 -> Set(2,0)=1 Set(2,2)=1. KEINE Schalterzonen.
 *   Vorbild Schalterlogik ROOM2060 sub01 @0x011BC.. (Cursor-Zone Member_cmp 0x0F + Sce_key_ck 0x40 -> Kippen),
 *   Loesung @0x01296..@0x012AE (Bits -> Set(3,126)). Vorbild Zielwert 80: include/re15_panel_zeiger.h (RE2
 *   ROOM2130 sub04 @0x01752 cmp(var5 == 80), Ruhe 30 Bilder @0x0171C, Bestaetigung se 0x0C @0x01762).
 *   Cut 15 (@0x00240, (-19628,-17442,22616) Nadir): Cursor (-19554,-900,22684) -> Bild (160.9,119.1), 200 Einheiten
 *   = 2.515 px (H 208, vz 16542). Hebel im BSS-Bild ROOM20515: Mitten x 88/133/178 (gross), 238 (klein), y 125;
 *   Skala "0..100": Ziffern-Spalten 0 @165.5 ... 90 @248.5, 100 @256.5 (0.91 px je Punkt), rotes Band 80..100
 *   x 239..256 (y 74), Laufschiene y 70..73 (Pixel selbst gemessen, Dossier §1.4).
 * ROOM2080 Rechner = sub00 @0x00C1A Slot 4 sce 3 Ereignis 2 (nur (3,245)=0), sub02 @0x00C86..@0x00CF1:
 *   Frage msg 0, Ja -> @0x00C94 Set(3,245)=1, Slot 4/3 aus, Tuer 1 an, Pad-Sperre, Sleep 20, Cut 7,
 *   @0x00CC4 Work_set(3,0) Wasser, @0x00CC8 `2f 01 32 00` Speed_set(1,+50), @0x00CCC For 148 (148 x 50 = 7400),
 *   Sleep 20, Cut 3, Cut_auto 1, Freigabe, Plc_ret, Endif @0x00CEE, Evt_end @0x00CF0. Ifel-Laenge @0x00C8E 0x60.
 * Wasserflaeche (PORT-PROP fuer 2030, das keine hat): ROOM2080 Prop 0 (MD1 @0x000D98 4572 B, 108 Vierecke
 *   x 0..21600 z 0..16200, TIM @0x026408 16928 B; Obj_model_set @0x00C2E Typ 3, Flags 0x000a|1, Attribut 0x001a),
 *   eingebacken in gen/ventilkette_r41_wasser.inc (tools/r41_ventilkette/gen_r41_ventilkette.py). Zwei Flaechen
 *   decken den Schacht (Aussenwaende x -9700..9250, z -12350..12000; Buero bis z 20000).
 *
 * ============================================================================================================
 * PORT-WAHLEN (benannt; Form jeweils belegt)
 * ============================================================================================================
 * - Bruecke OBEN = Ringebene y -5400 (die naechste Ebene ueber Band 1 in diesem Raum; Fahrt 30 x 120 = 3600 in der
 *   Lift-Form sub03). Solange oben: Steg-Zelle 12 gesperrt (Sca_id_set-Form), Pult meldet msg 40 (Druck null,
 *   Ventil 13 zu); Ventil 13 (am Tank) oeffnen -> Pult fragt "Lower the bridge?" -> Fahrt unter Cut 4.
 * - Ventilraetsel (Nachbesserung 4, Nachtrag 13; ersetzt den Platzhalter): sechs Ventil-Plaetze an den Raedern, erst mit
 *   Bit 102 UND (3,108) (Nachtraege 9/10; vorher Satz 49 "I should look around here first."); je Platz Frage 50..55
 *   (Nummer + Farbe) -> Ja -> Drehung in (5,0x10..0x13) + Klick 0x0A; nach der VIERTEN: richtige Folge 09, 07, 02, 01
 *   (PORT-WAHL, Ableitung im Dossier §7) -> Ereignis 26 = Warnung -> Sleep 1800 (= 60 s bei 30 Takten) im selben Faden (Form ROOM2000 sub07 @0x01B36: For/Evt_next-Schleife eines
 *   Hintergrundfadens) -> Pad-Sperre, Cut 4, Wasser steigt 324 x 50 (Rate des Rechners @0x00CC8) vom Beckengrund
 *   y 0 bis zur Tuerebene y -16200; der C-Takt ertraenkt den Spieler, sobald die Flaeche 900 ueber seinen Fuessen
 *   steht (hp := -1 wie RE15_KILL_AT, main.c; blaue Blende Kanal 0 in der Form ROOM11F0 @0x174a `56 00 02 07 00 00`
 *   + `57`: ABR 2 (subtraktiv, der einzige Blendmodus der ausgelieferten Skripte) mit Maske Rot+Gruen (Bits 2+1) -
 *   die Kachel zieht Rot und Gruen ab, Blau bleibt = blauer Schleier; Spritzer = Wasser-Schritt-Effekt FUN_80045630
 *   Code 0x06050D00). Gemessen 2026-10-23 22:05 (Flut-Lauf, Bildmitte): die erste Fassung "ABR 0 + Maske Blau"
 *   zeichnete der PC-Renderer als B-F (render_pc.c kennt nur ABR 1/2) -> (29,36,29) wurde (30,37,0) = Blau WEG.
 *   Falsche Folge -> Ereignis 27 = (5,0x10..0x13) := 0 + Text 45 "The valves turned back to their original position." +
 *   Fehlerton 0x0B (Form oben).
 * - Dokument "Technician's Note" (Nachtrag 13 Folge 2): Dokument 6 = Item 0x4E (re15_files.c, Bildsatz FILE31, Satz
 *   tools/r41_ventilkette/dokument_satz.py aus dem Nutzer-Text), Blatt am Boden neben der Sicherung (in 2030 gibt es keine
 *   Leiche), Taken-Bit (9,107) (Zensus: frei im Original und im Port). Weltmodell seit Nachbesserung 5 (Abnahme 4 Mangel 1:
 *   das RE2-Blatt mesh01 flach am Boden war in 15 von 16 Kameras unsichtbar, in Cut 5 39 Pixel) ein Zettel AUFRECHT an der
 *   Suedwand neben dem Schrank mit der Sicherung (Schrank = SCA Zelle 8 x -6100..-5600, Sicherung Slot 9 @0x01BAA x -5900..
 *   -5100): Form des Aushangs (ein Viereck, Normale zur Kamera), Lage per Projektion in Cut 5 gemessen (Dossier §0f B5-6).
 * - Plakat (Nachtrag 13 Folge 3): Aushang an der Suedwand (Weltmodell obj 7) + Nahaufnahme = Port-Bild
 *   (gen/ventilkette_r41_plakat.inc, tools/r41_ventilkette/plakat.py) ueber die Vollbild-Ebene (main.c); Form der
 *   Nahaufnahmen der Runde 40 (Pad-Sperre ROOM1051 sub03 @0x00DA4 .. @0x00DD2), schliessen mit der Bestaetigen-/
 *   Abbrechen-Flanke (virtuell 0xC000 wie das Wegdruecken einer Nachricht, andi 0xc000 @0x80028698).
 * - Verlassen waehrend des Zaehlers (jede Tuer) = Flut gelaufen: Bits 103 + 100. Ost-Tuer Slot 0 (-> 2070) wird
 *   dabei auf ROOM20B0 Cut 11 umgelenkt (Ziel der Szene @0x01F00/@0x01F08) — der Gegenweg der 20B0-Tuer Slot 3
 *   @0x019C2, die 2070 ebenfalls ueberspringt. Danach dauerhaft (Bit 103).
 * - Nach der Flut OHNE Strom (Bit 103, (3,126)=0; "geflutet"): 2030 steht bis 1800 ueber dem Ring unter Wasser
 *   (y -7200; Abstand wie ROOM2050 Wasser -3600 zum Steg -5400 @0x01F88), Lift-Rufzonen oben (Slots 4/6) = Text msg 1
 *   (Original-Satz "The elevator is not responding...", Form sub01 @0x01E18), Ring-Tueren 20B0/2070 gesperrt (C-E2).
 * - Nach der Flut MIT Strom (Bit 103, (3,126)=1; Nachtraege 7/8/12): EINE Wasserflaeche (obj 4) nur ueber der
 *   Schachtoeffnung auf y -5350 = knapp unter der Absperrung (Nachbesserung 4; Netz im Format von ROOM2080 Prop 0,
 *   Generator), Ring trocken (Tueren, Lifte = Original), Treppe Slot 8 = Sperrtext 48 im Band 3 (Form der Leiter 2000),
 *   Ventile, Dokument und Plakat entfallen (unter Wasser). Der Zustand haengt DIREKT an (3,126) (nie geloescht, s. o.).
 * - 2000 -> 20B0 fuer jede Tuer nach 2000 (Cut-Zuordnung oben), in 20B0 die drei Strahlen @0x01E96.. unterdrueckt
 *   (Nutzer: "kein Wasser mehr runter laufen").
 * - 2050: Hebel (Ereignis 4) vor der Loesung = Sperrsatz msg 40 (Form ROOM2060 sub12 @0x015D0). Pult: Slot 8 +
 *   Cursor wie @0x01D38/@0x01D4C angelegt, Einstieg = Beschreibung + "Will you push them? " (ROOM11F0 msg 1
 *   Bytes) + sub09-Bytes ohne (5,5) (sonst beendet SQUARE das Pult, sub01 @0x021E2); Kippen = SQUARE auf einer
 *   Hebelzone (2060-Form), Abbruch = CROSS (Ausstieg sub10 @0x0240C), Gewichte 30/20/40 (gross) + 10 (klein),
 *   einzige Loesung 30+40+10 = 80 (alle 16 Kombinationen gezaehlt, Riegel). Loesung -> 30 Ruhebilder (RE2
 *   @0x0171C) -> Bit 106, se 0x0C (RE2 @0x01762), "Power supply OK." (ROOM11F0 msg 2 Bytes), Ausstieg sub10.
 *   Zeiger = roter Laeufer auf der Schiene y 70..73 (RE2 ROOM2130: Zeiger unter der waagerechten Skala).
 * - 2080: Rechner = nur Original (Nachtrag 7; Port-Programme und Text 2080 Id 40 entfernt, Nachbesserung 3).
 */
#ifndef RE15_VENTILKETTE_R41_H
#define RE15_VENTILKETTE_R41_H

#include <stdint.h>
#include "re15_rdt.h"

/* ---- Bank-9-Bits (VERTRAG §1.1: C = 100-103, 106, 107; Zensus unit_r36_kartenbits) ------------------------- */
#define RE15_R41C_BANK               9
#define RE15_R41C_BIT_LEITER_FREI    100   /* 2000/20B0: Leiter nach 2050 frei (nach der Flut)              */
#define RE15_R41C_BIT_VENTIL13       101   /* 2030: Zulaufventil 13 geoeffnet                               */
#define RE15_R41C_BIT_BRUECKE_UNTEN  102   /* 2030: Bruecke gesenkt (y -1800, Zelle 12 offen)               */
#define RE15_R41C_BIT_FLUT_GELAUFEN  103   /* 2030: Ventile geoeffnet und Raum verlassen -> 2000 wird 20B0  */
#define RE15_R41C_BIT_PULT2050       106   /* 2050: Schaltpult auf 80 V                                     */
#define RE15_R41C_BIT_DOKUMENT       107   /* 2030: "Technician's Note" genommen (Taken-Bit, Nachbesserung 4; bis Nachtrag 7
                                            * "2080-Rechner -> 2030 leer", seitdem frei; Zensus bank9_zensus.py original+port) */
#define RE15_R41C_BIT_KASTEN        125   /* 2010: Sicherung steckt im Ventil-Sicherungskasten (Nachtrag 16 Folge 5: 107 ist
                                            * belegt -> naechstes freies Bit nach Zensus: bank9_zensus.py original "FREI im
                                            * Original ... 110..133", port ohne Zugriff auf 125; 110-124 sind F/G/E/J/K
                                            * zugeteilt; VERTRAG 1.1 nachgetragen, k_port[] in test_r36_kartenbits.c) */
/* Original-Bits, die die Kette liest (Belege im Kopf): */
#define RE15_R41C_STROM_BANK         3
#define RE15_R41C_STROM_BIT          126   /* (3,0x7E) Generator 2060: Set nur sub01 @0x012AE, Leser 2030 sub01 @0x01E14 */
#define RE15_R41C_SICHERUNG_BANK     3
#define RE15_R41C_SICHERUNG_BIT      108   /* (3,0x6C) Sicherung aufgehoben: Set nur 2030 sub06 @0x01FF2            */
#define RE15_R41C_GENERATOR_BIT      144   /* (3,0x90) Sicherung im Generator-Kasten 2060: Set nur sub19 @0x0169E    */
/* Zustand von 2030 nach der Flut (Messhaken re15_ventilkette_r41_zustand_2030) */
#define RE15_R41C_2030_TROCKEN       0     /* Bit 103 = 0                                                    */
#define RE15_R41C_2030_GEFLUTET      1     /* Bit 103 = 1, (3,126) = 0: Wasser y -7200, Ring gesperrt         */
#define RE15_R41C_2030_RELING        2     /* Bit 103 = 1, (3,126) = 1: Wasser y -5350 in der Schachtoeffnung */

/* ---- Raeume (Leon; Elza = |1) -------------------------------------------------------------------------------- */
#define RE15_R41C_RAUM_2000   0x2000u
#define RE15_R41C_RAUM_2010   0x2010u
#define RE15_R41C_RAUM_2030   0x2030u
#define RE15_R41C_RAUM_2050   0x2050u
#define RE15_R41C_RAUM_2060   0x2060u
#define RE15_R41C_RAUM_2070   0x2070u
#define RE15_R41C_RAUM_20B0   0x20B0u
#define RE15_R41C_RAUM_1260   0x1260u

/* ---- Text-Ids (VERTRAG §1.2) ---------------------------------------------------------------------------------- */
#define RE15_R41C_MSG_LEITER          40   /* 2000/20B0 */
#define RE15_R41C_MSG_PULT_OHNE       40   /* 2030 */
#define RE15_R41C_MSG_V13_FRAGE       41
#define RE15_R41C_MSG_V13_OFFEN       42
#define RE15_R41C_MSG_BRUECKE_FRAGE   43
#define RE15_R41C_MSG_BRUECKE_UNTEN   44
#define RE15_R41C_MSG_VENTILE_ZURUECK 45   /* Nachbesserung 4: falsche Folge -> zurueckgesetzt (frueher Platzhalter-Frage) */
#define RE15_R41C_MSG_WARNUNG         46
#define RE15_R41C_MSG_V13_ZUSTAND     47
/* Nachbesserung 3: Id 48 (frueher "valves are open ... flooded") und 49 (frueher "... drained", Bit 107) neu belegt —
 * nach der Flut liegen die Ventile unter Wasser (Nachtrag 9), den Zustand waehrend des Zaehlers zeigt die Warnung 46. */
#define RE15_R41C_MSG_TREPPE_FLUT     48   /* Treppe Slot 8 bei "auf Relinghoehe" (Nachtrag 8) */
#define RE15_R41C_MSG_VENTILE_VORHER  49   /* Ventile ohne Bruecke (Nachtrag 9) oder ohne Kasten-Sicherung (Nachtrag 16): seit
                                            * Nachbesserung 5 = Original "It's locked." (ROOM20A0 msg 1 @0x203C, Nachtrag 16
                                            * Folge 4 "neutraler Text ohne look around") */
#define RE15_R41C_MSG_VENTIL0         50   /* 50..55 = Frage an Ventil 01/02/03/07/08/09 (VERTRAG 1.2, Nachtrag 13 Folge 6) */
/* 2010/2011 Ventil-Sicherungskasten (Nachtrag 16, VERTRAG 1.2 Ids 40-44): 41/42/44 sind Original-Saetze (Scheiben). */
#define RE15_R41C_MSG_KASTEN_LEER     40   /* "A fuse box for the valve controls." | ROOM2060 msg 3 @0x1833        */
#define RE15_R41C_MSG_KASTEN_FRAGE    41   /* = ROOM2060 msg 4 @0x1855 "Will you use the Fuse? "                    */
#define RE15_R41C_MSG_KASTEN_BENUTZT  42   /* = ROOM2060 msg 5 @0x1875 "You've used the Fuse."                      */
#define RE15_R41C_MSG_KASTEN_DRIN     43   /* ROOM2060 msg 6 @0x1892 | ROOM2030 msg 2 @0x208B (Frage)              */
#define RE15_R41C_MSG_KASTEN_GENOMMEN 44   /* = ROOM2030 msg 3 @0x20AC "You've taken the Fuse."                     */
#define RE15_R41C_MSG_HEBEL_OHNE      40   /* 2050 */
#define RE15_R41C_MSG_PULT_FRAGE      41   /* = ROOM11F0 msg 1 "Will you push them? " @0x18D8 */
#define RE15_R41C_MSG_PULT_OK         42   /* = ROOM11F0 msg 2 "Power supply OK." @0x18F1     */
#define RE15_R41C_MSG_PULT_BESCHR     43
#define RE15_R41C_MSG_PULT_ZUSTAND    44
/* Orchestrator-Entscheid C-E2 (AUFTRAG.md 2026-10-24 05:25, Abnahme 0 M5), seit Nachtrag 7 nur "geflutet" (Bit 103 ohne
 * Strom (3,126)): die Zugaenge in den gefluteten Ring von 2030 sind gesperrt (Aktionstaste -> Port-Text, kein
 * Raumwechsel). Ids laut VERTRAG §1.2 (ergaenzt). */
#define RE15_R41C_MSG_RING_20B0       41   /* 20B0/20B1 */
#define RE15_R41C_MSG_RING_2070       40   /* 2070/2071 */
#define RE15_R41C_MSG_LIFT_ORIGINAL    1   /* 2030 msg 1 "The elevator is not responding..." @0x2038 */

/* ---- Ereignisse (Port-Ids < RE15_RDT_MAX_SUB_SCD, ausserhalb der Raum-Subs: 2030 hat sub00..06) ------------- */
#define RE15_R41C_EV_PULT      24
#define RE15_R41C_EV_VENTIL13  25
#define RE15_R41C_EV_FLUT      26   /* vom C-Takt nach der richtigen Folge gefeuert: Warnung 46 -> Zaehler -> Flut        */
#define RE15_R41C_EV_FALSCH    27   /* vom C-Takt nach vier Drehungen in falscher Folge: zuruecksetzen + Text 45          */
#define RE15_R41C_EV_VENTIL0   17   /* 17..22 = Ventil 01/02/03/07/08/09 (Nachbesserung 4)                              */
#define RE15_R41C_EV_PLAKAT    23   /* Plakat-Nahaufnahme                                                                */
#define RE15_R41C_EV_2050_HEBEL 4   /* Original-Ereignis des Hebels (sub00 @0x01E28)  */
/* Nachbesserung 1 (Abnahme 0, M1): DERSELBE Hebel-Platz Slot 6 fuehrt Ereignis 2, sobald die drei Kisten im Becken
 * liegen: ROOM2050/2051 sub01 @0x02006 `06 00 1c 00` Ifel_ck, @0x0200A `21 03 6d 00` Ck(3,109)=0, @0x0200E/@0x02012/
 * @0x02016 `21 05 00 01`/`21 05 01 01`/`21 05 02 01` Ck(5,0..2)=1 (gesetzt @0x0203C ff. bei Member_cmp 0x0F == 3/4/5),
 * @0x0201A `46 06 03 31 ff 00 18 02 00 00` Aot_reset Slot 6 sce 3 sat 0x31 -> Ereignis 2 = sub02 @0x0220E: Frage msg 0
 * `2b 00 ff ff`, Ja `21 0c 1f 00` @0x02218, Set(3,109) `22 03 6d 01` @0x0221C, Wasser obj 0 y 0 `34 01 00 00` @0x02268,
 * Cut 4 `29 04` @0x02270, Speed_set(1,-20) `2f 01 ec ff` @0x0227C + For 180 `0d 00 06 00 b4 00` @0x02280 = das Wasser
 * STEIGT. Einzige Ereignis-2-Stelle in 2050/2051 (scd_dump_room.py: Aot_set/Aot_reset mit `18 02` nur @0x0201A). */
#define RE15_R41C_EV_2050_KISTEN 2  /* sub01 @0x0201A -> sub02 @0x0220E (Hebel mit den Kisten im Becken) */
#define RE15_R41C_EV_2050_PULT  9   /* Original-Ereignis des Pults (main00 @0x01D38)  */
#define RE15_R41C_EV_2050_AUS  10   /* Original sub10 = Ausstieg aus dem Pult          */
#define RE15_R41C_EV_KASTEN    20   /* 2010: Ventil-Sicherungskasten (2010 hat nur sub00..sub03) */

/* ---- Plaetze 2030 (frei: 0-3 Tueren, 4-7 Lifte, 8 Treppe, 9 Sicherung, 10/11 Items, 51-63 RVD) --------------- */
#define RE15_R41C_SLOT_PULT      12
#define RE15_R41C_SLOT_VENTIL13  13
#define RE15_R41C_SLOT_VENTIL0   14   /* 14..19 = Ventil 01/02/03/07/08/09 (Nachbesserung 4)          */
#define RE15_R41C_SLOT_DOKUMENT  20   /* Item-Zone "Technician's Note"                                 */
#define RE15_R41C_SLOT_PLAKAT    21   /* Untersuchen des Plakats                                       */
#define RE15_R41C_SLOT_2000_LEITER 4   /* main00 @0x0169E */
/* 2010 Ventil-Sicherungskasten (Nachbesserung 5, Nachtrag 16, Dossier §0f B5-1): Lage = der Schaltschrank mit dem kleinen
 * Anzeigefeld (3 x 3 Tasten) an der Ostwand im Hintergrund ROOM2010 Cut 3 (BSS-Bild x 229..252 y 66..86, Rueckprojektion auf
 * die Ostwand x 5400 = Innenkante SCA Zelle 3 (5400,-9550,2000,16650) -> Mitte z -2770, y -2650..-2970). Platz in der Groesse
 * des Original-Kastens ROOM2060 Slot 9 @0x010A6 (1400 x 1000, die lange Seite an der Wand) und in der Lage "Wandflaeche +- 500"
 * wie Pult- und Ventil-Plaetze dieses Moduls (der Vorwaertspunkt eines Spielers an der Wand liegt in der Wand): x 4900..5900,
 * z -3470..-2070, Band 0 (alle Plaetze von 2010 tragen Band 0, main00 @0x00746..@0x007D0). Platz 8 = erster freier (0-7). */
#define RE15_R41C_SLOT_KASTEN      8
#define RE15_R41C_KASTEN_RECT      4900, -3470, 1000, 1400
#define RE15_R41C_KASTEN_BAND      0
#define RE15_R41C_SLOT_2060_KASTEN 9   /* ROOM2060 sub00 @0x010A6/@0x010C6/@0x010DE (Sicherungskasten des Generators) */
#define RE15_R41C_MSG_2060_FEHLT   3   /* ROOM2060 msg 3 "One of the fuses is missing..." (Else-Zweig @0x010A6) */
#define RE15_R41C_SLOT_2030_OSTTUER 0  /* main00 @0x01ACA */
#define RE15_R41C_SLOT_2050_PULT  8    /* main00 @0x01D38 */
#define RE15_R41C_SLOT_2030_TREPPE 8   /* main00 @0x01B8E sce 12 (Band 0x80) */
#define RE15_R41C_TREPPE_BAND     3    /* Sperre nur vom Ring aus (Ring = Band 3, Zellen 36-41 fl 0x23) */
/* Zugaenge in den Ring von 2030 (Tuer-Zensus aller ROOM*.RDT, Ziel Stage 1 Raum 3 mit Ankunft auf dem Ring
 * (6550,-5400,-100) nfloor 3): 20B0/20B1 Slot 3 @0x019C2 `3b 03 02 31 01 00 4a 15 24 e1 ... 00 08 01 03 00 03`,
 * 2070/2071 Slot 1 @0x00A5A `3b 01 02 31 00 00 88 13 24 e1 ... 00 08 01 03 00 03` und Slot 2 @0x00A7A
 * `3b 02 02 31 00 00 12 b2 24 13 ... 00 08 01 03 00 03`. Alle uebrigen Tueren nach 2030 (2020/2021 Slot 1 @0x00866/
 * @0x0088A, 2060/2061 Slot 0 @0x00D9A, 2080/2081 Slot 0 @0x00BA2) kommen auf der Tuerebene y -16200 (nfloor 9) an,
 * also ueber dem Wasser y -7200 -> bleiben offen. */
#define RE15_R41C_SLOT_20B0_RING  3
#define RE15_R41C_SLOT_2070_RING_A 1
#define RE15_R41C_SLOT_2070_RING_B 2
#define RE15_R41C_RAUM_2030_ID    0x03   /* Door_aot_set pc[23] (Stage 1 = STAGE2) */
#define RE15_R41C_SAT            0x31  /* Vorwaertspunkt-Test wie alle Original-Plaetze dieser Raeume */
#define RE15_R41C_PAUSE          0xFFFFu
/* Rechtecke (x, z, Breite, Tiefe) + Band — Herleitung im Kopf. */
#define RE15_R41C_PULT_RECT      -300, -10850, 1300, 1000
#define RE15_R41C_PULT_BAND      3
#define RE15_R41C_VENTIL13_RECT  -3050, -6900, 1300, 1000
#define RE15_R41C_VENTIL13_BAND  1
/* Ventile (Nachbesserung 4, Kopf "Ventil-RAEDER"): Nummer, Radmitte x (auf 50 gerundet), Wandflaeche z, Farbe 0 rot / 1 gruen /
 * 2 schwarz; Platz = x +- 500, Wand +- 500, Band 1. Reihenfolge = Ereignis 17.. / Slot 14.. / Text 50... */
#define RE15_R41C_VENTILE_N       6
#define RE15_R41C_VENTIL_NUMMERN  { 1, 2, 3, 7, 8, 9 }
#define RE15_R41C_VENTIL_X        { -1350, 150, 2350, 2600, 400, -2650 }
#define RE15_R41C_VENTIL_WAND_Z   { -8500, -8500, -8500, 7900, 7900, 7900 }
#define RE15_R41C_VENTIL_FARBE    { 2, 1, 0, 2, 1, 0 }
#define RE15_R41C_VENTILE_BAND    1                          /* Nord-Landeplatz Zelle 13 / Sued-Landeplatz Zelle 16 */
/* Richtige Folge (PORT-WAHL, Vorschlag zur Freigabe; Plakat druckt sie falsch, Dossier §7): 09 rot, 07 schwarz, 02 gruen,
 * 01 schwarz = Indizes in RE15_R41C_VENTIL_NUMMERN. */
#define RE15_R41C_DREHUNGEN       4
#define RE15_R41C_LOESUNG         { 5, 3, 1, 0 }
/* Bank 5 (Raum-Scratch) der Ventile/des Plakats (2030 benutzt Bank 5 sonst nicht; 0x1D/0x1E s. u.). */
#define RE15_R41C_FLAG_DREHUNG0   0x10   /* 0x10..0x13 = Drehung 1..4 (Form ROOM11F0 Schalterbits (5,13..22)) */
#define RE15_R41C_FLAG_GEDREHT    0x1B   /* Handshake Programm -> C-Takt (Ja auf die Ventil-Frage)              */
#define RE15_R41C_FLAG_PLAKAT     0x1C   /* Plakat-Nahaufnahme offen                                           */
#define RE15_R41C_SE_FEHLER       0x0B   /* RE2 ROOM2130 sub04 @0x01800 `36 02 0b 01` (Bank PANEL2130, Satz 0x0B) */
#define RE15_R41C_FLAG_TON        0x1A   /* 2030: Ventil-Ton jetzt (Programm statt RE2 se_on(2,5) @0x02168..)    */
#define RE15_R41C_FLAG_STANDBILD  0x1F   /* 2010: Standbild der Ventile offen (2010/2011 benutzen Bank 5 nicht)   */
/* Ventil-Dreh-Animation (Nachbesserung 5, Nachtrag 18 Punkt 3, Dossier §0f B5-3): RE2 ROOM1080 (Hubschrauber-Landeplatz,
 * msg 0 @0x267A "A water pressure valve") sub08 @0x0210A..@0x0221E, Clips 15/16/17 aus dem RBJ @0x39AC derselben RDT
 * (13128 B, 1 Record, EMR 15 Knochen x 80 B, EDD 19 Clips: 15 = 30, 16 = 50, 17 = 40 Bilder) = shared_assets/RE2/
 * VENTIL1080.RBJ; Ton = RE2 ROOM1080 snd0 Satz 5 (EDT-Eintrag `00 00 54 16`) = shared_assets/RE2/VENTIL1080.EDT/.VH/.VB
 * (tools/r41_ventilkette/re2_ventil_export.py, bytegleich mit der RDT). Abstand Spieler - Wandflaeche 816 = RE2 pos_set
 * @0x02116 (z -21250) bis zur SCA-Flaeche in Blickrichtung (Element 12, z -22066). */
#define RE15_R41C_VENTIL_RBJ_RAUM 0x1080u   /* RE2-Quellraum (nur Protokoll/Riegel)                                 */
#define RE15_R41C_DREH_ABSTAND    816
#define RE15_R41C_DREH_CLIP0      15       /* RBJ Clip 15 Hand zum Rad, 16 fassen, 17 Drehtakt                    */
#define RE15_R41C_VENTIL_SE       5        /* RE2 ROOM1080 snd0 Satz 5 (se_on `36 02 05 01 ...` @0x02168)          */
/* Toene, die der C-Takt ueber den Plattform-Haken spielt (PC: platform/pc/src/ventilkette_r41_pc.c). */
#define RE15_R41C_TON_VENTIL      1        /* RE2 ROOM1080 Satz 5 (Bank VENTIL1080)                                */
#define RE15_R41C_TON_AUF         2        /* RE2 "You unlocked it." Satz 0x26 (EXE @0x800515f8; KARTSE AUF_HIER)  */
/* Standbild beim Einsetzen (Nachtrag 16 Folge 2 "Kamera auf die Ventile"): die Ventil-Kamera der Suedwand ROOM2030 Cut 7
 * (Raeder 09/08/07 + Plakat, Dossier §0e B4-2), BSS/ROOM2030/BG07.BSS (= ROOM2031/BG07.BSS bytegleich). */
#define RE15_R41C_STANDBILD_RAUM  0x2030u
#define RE15_R41C_STANDBILD_CUT   7
/* Countdown der 60 s (Nachbesserung 5, Nachtrag 18 Punkt 4, Dossier §0f B5-4): die verbleibenden Bilder des Sleep 1800
 * (RE15_R41C_ZAEHLER_BILDER, 30 Takte je Sekunde), angezeigt als MM:SS:HH in der Nachrichtenschrift des Spiels (TEX.TIM,
 * Glyphen 0x0C..0x15 = 0..9, 0x16 = ':', Vorschub DEBUG.BIN @0x4416: Ziffer 8 px ('1' 7), ':' 4 px) in Farbe 2 - die Farbe,
 * mit der das Original eine ablaufende Frist hervorhebt (ROOM2040 msg 4 @0x19A6 "Bomb activated, I have `05 02`10
 * seconds`05 00` before it detonates!", die Zeitbombe). Ein Countdown-Bild hat weder RE1.5 noch die vorliegenden RE2-Daten
 * (Suche im Dossier §0f B5-4) -> Format und Lage PORT-WAHL: oben rechts, rechter Rand wie der Rand der Nachrichtenbox
 * links (x 34, re15_render_pc_msg_text(34, 180, ...) in main.c) -> x 286 - 56 = 230, y 20. */
#define RE15_R41C_COUNTDOWN_X     230
#define RE15_R41C_COUNTDOWN_Y     20
#define RE15_R41C_COUNTDOWN_FARBE 2
/* Dokument (Nachbesserung 4, Nachtrag 13 Folge 2; Dossier §0e B4-4). */
#define RE15_R41C_DOK_NR          6      /* re15_files.c s_docs[6]                                           */
#define RE15_R41C_DOK_ITEM        0x4E   /* 0x48 + 6 (RE2 @0x80071d04, RE1.5-Basis u8 @0x800c7370)          */
#define RE15_R41C_DOK_OBJ         6      /* nOmodel 4, obj 4/5 = Wasser                                      */
/* Nachbesserung 5 (Mangel 1): Zettel 500 x 700 an der Suedwand (Wandflaeche z 7900 = Innenkante SCA Zelle 7 @0x008EC) zwischen
 * dem Schrank mit der Sicherung (x <= -5600) und dem Warnpfosten (x ~-4400), Mitte (-4850,-3950,7880): ueber dem Handlauf der
 * unteren Reling wie der Aushang (Mitte y -3900); projiziert in Cut 5 (Kamera des Sued-Landeplatzes, RID @0x00100)
 * Bild x 105..118 y 73..93, ueber dem Handlauf (Messung §0f B5-6). Platz = Wandflaeche +- 500 (Form der Ventil-/Pult-
 * Plaetze), x -5100..-4600: grenzt an die Sicherung (x <= -5100) und laesst den Plakat-Platz (x >= -4100) frei. PORT-WAHL. */
#define RE15_R41C_DOK_X           (-4850)
#define RE15_R41C_DOK_Y           (-3950)
#define RE15_R41C_DOK_Z           7880
#define RE15_R41C_DOK_RECT        -5100, 7400, 500, 1000    /* Wandflaeche z 7900 +- 500, x -5100..-4600 */
/* Plakat (Nachbesserung 4, Nachtrag 13 Folge 3; Dossier §0e B4-5). */
#define RE15_R41C_PLAKAT_OBJ      7
#define RE15_R41C_PLAKAT_X        (-3650)  /* Suedwand zwischen dem gelb-schwarzen Pfosten (x ~-4400) und Ventil 09 */
#define RE15_R41C_PLAKAT_Y        (-3900)  /* Mitte 2100 ueber dem Landeplatz, ueber dem Handlauf der unteren Reling */
#define RE15_R41C_PLAKAT_Z        7880     /* 20 vor der Wandflaeche z 7900                                       */
#define RE15_R41C_PLAKAT_RECT     -4100, 7380, 900, 1000   /* Wand +- 500, buendig zwischen Dokument und Ventil 09 */
/* Objekt-Band der zwei Blaetter (Obj_model_set pc[4] -> pool+0x82): 2 = weder das Spielerband am Landeplatz (1; Kletter-
 * Sonde FUN_8002d2c0 @0x8002d370 obj[0x82] == Spielerband) noch eines darunter (0; Standtest @0x80031cfc obj[0x82] + 1 ==
 * Spielerband). PORT-WAHL nach Messung (Lauf v2_leon F1858: mit Band 1 kletterte Leon auf das Plakat, ventilkette_r41.c). */
#define RE15_R41C_BLATT_BAND      2

/* ---- Bruecke (obj 0 @0x01B6C) ------------------------------------------------------------------------------- */
#define RE15_R41C_BRUECKE_OBJ      0
#define RE15_R41C_BRUECKE_Y_UNTEN  (-1800)   /* @0x01B6C Lage y                                 */
#define RE15_R41C_BRUECKE_Y_OBEN   (-5400)   /* Ringebene (Lift-Endlage @0x01F02 `34 01 e8 ea`) */
#define RE15_R41C_BRUECKE_SCHRITT  120       /* @0x01EF2 `2f 01 78 00`                           */
#define RE15_R41C_BRUECKE_SCHRITTE 30        /* 30 x 120 = 3600                                  */
#define RE15_R41C_STEG_ZELLE       12        /* SCA Zelle 12 je Region (u0 00 = begehbar)        */
#define RE15_R41C_FAHRT_CUT        4         /* Draufsicht @0x000E0                              */

/* ---- Wasser (Port-Props obj 4/5 in 2030, ROOM2080 Prop 0) --------------------------------------------------- */
#define RE15_R41C_WASSER_OBJ_A     4
#define RE15_R41C_WASSER_OBJ_B     5
#define RE15_R41C_WASSER_X         (-10800)  /* Flaeche 0..21600 -> x -10800..10800            */
#define RE15_R41C_WASSER_ZA        (-12400)  /* Flaeche A 0..16200 -> z -12400..3800            */
#define RE15_R41C_WASSER_ZB        3800      /* Flaeche B -> z 3800..20000                      */
#define RE15_R41C_WASSER_PARK_Z    60000     /* ausserhalb jeder Kamera, bis die Flut laeuft: PORT-WAHL des Parkplatzes, */
#define RE15_R41C_WASSER_PARK_Y    1800      /* Form = ROOM2050 Wasser obj 0 "ausser Sicht" bei y -16200 @0x01D92       */
#define RE15_R41C_WASSER_Y_BODEN   0         /* Beckengrund Band 0                              */
#define RE15_R41C_WASSER_Y_RUHE    (-7200)   /* nach der Flut ohne Strom: 1800 ueber dem Ring (-5400) */
#define RE15_R41C_RINGBODEN_Y      (-5400)   /* Grenze: Ringboden (Bodenzellen 36-41 fl 0x23, Ankunft nfloor 3; Kopf)   */
#define RE15_R41C_PEGEL_ABSTAND    50        /* PORT-WAHL "knapp darunter" = ein Schritt der Rate @0x00CC8 (Kopf)       */
#define RE15_R41C_WASSER_Y_RELING  (RE15_R41C_RINGBODEN_Y + RE15_R41C_PEGEL_ABSTAND)   /* = -5350 (Nachtrag 12 (k)) */
/* Schachtoeffnung fuer den Pegel mit Strom (x0, x1, z0, z1; Innenkanten der Absperrung, Kopf) — das Netz erzeugt
 * tools/r41_ventilkette/gen_r41_ventilkette.py (gen/ventilkette_r41_wasser.inc re15_r41c_wasser_reling_md1) im Format
 * von ROOM2080 Prop 0 (Vierecke <= 1800 x 1800, je eine Kachel mit dem UV-Satz des Originals); Ursprung = Weltursprung. */
#define RE15_R41C_RELING_N         3
#define RE15_R41C_RELING_RECHTECKE { { -5400, 5400, -8500, -6400 }, { -7700, 5400, -6400, 2450 }, { -5850, 5400, 2450, 8150 } }
#define RE15_R41C_WASSER_Y_ENDE    (-16200)  /* Tuerebene Band 9                                 */
#define RE15_R41C_WASSER_SCHRITT   50        /* ROOM2080 sub02 @0x00CC8 `2f 01 32 00`           */
#define RE15_R41C_WASSER_SCHRITTE  324       /* 324 x 50 = 16200                                 */
#define RE15_R41C_WASSER_TYP       3         /* @0x00C2E pc[2]                                   */
#define RE15_R41C_WASSER_FLAGS     0x000Bu   /* @0x00C2E pc[6..7] 0x000a | 1 (op_obj_model_set)  */
#define RE15_R41C_WASSER_ATTR      0x001Au   /* @0x00C2E pc[8..9]                                */
#define RE15_R41C_ERTRINK_TIEFE    900       /* PORT-WAHL (Ertrinken gibt es im Original nicht): Wasser 900 ueber
                                              * den Fuessen. ENTLIEHENES Mass, KEINE Koerperhoehe: @0x8002c24c
                                              * `addiu v0,v0,-900` hebt in der Objekt-Schleife (Schritt 148
                                              * `addiu s6,s6,148` @0x8002c1f8) die y-Lage (+0x60) von Typ-4-Objekten
                                              * an (`lbu v1,8(s1)` / `ori v0,zero,0x4` / `bne` @0x8002c234-3c). */
/* Spritzer: KEINE Rate — ein Ereignis, wenn die Flaeche die Fuesse erreicht (ventilkette_r41.c flut_tick; Original
 * nur je Fuss-Aufsetzer @0x8003253c -> FUN_80045630 @0x800456b8). Die fruehere Rate "alle 6 Takte" war ohne Beleg. */
#define RE15_R41C_ZAEHLER_BILDER   1800      /* 60 s bei 30 SCD-Takten (Sleep `09 0a 08 07`)     */
/* Marker des Programms fuer den C-Takt: Bank 5 WORT 0 (Raum-Scratch, beim Raumladen gewischt FUN_8003ecec
 * @0x8003ed74; Wort 1 taugt NICHT — es ist der Ein-Bild-Handshake, den FUN_8003ebf4 @0x8003ec1c am Ende JEDES
 * VM-Laufs loescht). 2030 benutzt Bank 5 nicht (0 Treffer `21 05`/`22 05` in ROOM2030.RDT); der Installer loescht
 * beide Bits NUR in 2030/2031 (Nachbesserung 1, M3: in fremden Raeumen sind es Original-Bits, z. B. ROOM1171 sub02
 * @0x013B2 `22 05 1e 01` im Init-Takt = Kraehen-Flag DIVE-ARM @0x801155e8; das Raumladen wischt Wort 0 ohnehin,
 * scd_room_setup.c @0x8003ed74). */
#define RE15_R41C_FLAG_TIMER       0x1D
#define RE15_R41C_FLAG_FLUT        0x1E
/* Blaue Blende: Kanal 0, ABR 2 (B-F subtraktiv wie @0x174a), Maske Rot+Gruen (Config-Bits 2+1: Blau bleibt stehen),
 * statisch (Form ROOM11F0 @0x174a `56 00 02 07 00 00`
 * + `57 00 <level>`; Helligkeit = level >> 7, re15_fade.h). */
#define RE15_R41C_FADE_KANAL   0
#define RE15_R41C_FADE_ABR     2            /* @0x174a `56 00 02 ..` (FUN_800217b0: abr = Byte 2)                */
#define RE15_R41C_FADE_MASKE   0x6          /* Rot (Bit 2) + Gruen (Bit 1) abziehen, Blau (Bit 0) bleibt          */
#define RE15_R41C_FADE_LEVEL   0x5000u      /* -> Helligkeit 160 (level >> 7 @0x800218d0); PORT-WAHL: tiefer Schleier */
#define RE15_R41C_FADE_BUCKET  7            /* OT-Fach der SCD-Blende: Handler 0x56 @0x80042a58 `ori a3,7` (scd_vm.c op_fade_config) */

/* ---- 2050 Schaltpult ----------------------------------------------------------------------------------------- */
#define RE15_R41C_PANEL_CUT        15
#define RE15_R41C_PANEL_N          4
#define RE15_R41C_PANEL_GEWICHTE   { 30, 20, 40, 10 }     /* PORT-WAHL, Loesung 30+40+10 = Maske 0xD */
#define RE15_R41C_PANEL_LOESUNG    0xDu
#define RE15_R41C_PANEL_ZIEL       80                     /* RE2 ROOM2130 sub04 @0x01752 */
#define RE15_R41C_PANEL_RUHE       30                     /* RE2 @0x0171C sleep 0x1E      */
/* Cursor-Welt <-> Bild unter Cut 15: Bild = (160.9 + (x+19554)*2.515/200, 119.1 - (z-22684)*2.515/200).
 * Hebelmitten im Bild x 88/133/178/238, y 125 -> Welt x -25350 / -21772 / -18193 / -13422, z 22215.
 * Zonen 2400 x 4400 (Hebelkoerper y 85..165 = z 19000..25400), Form der 2060-Zonen 2050 x 2050 @0x00F34. */
#define RE15_R41C_PANEL_ZONE_W     2400
#define RE15_R41C_PANEL_ZONE_D     4400
#define RE15_R41C_PANEL_ZONE_Z0    19800
#define RE15_R41C_PANEL_X0         { -26550, -22972, -19393, -14622 }
#define RE15_R41C_CURSOR_OBJ       8
/* Zeiger auf der Skala (320x240): x = 165.5 + 0.91 * Wert (Ziffern 0 @165.5 .. 100 @256.5 = 91 px je 100 Punkte),
 * Schiene y 70..73, rotes Band 239..256. Ganzzahlig: x = (16550 + 91 * Wert + 50) / 100. */
#define RE15_R41C_SKALA_X0_100     16550    /* x 0 in Hundertstelpixeln */
#define RE15_R41C_SKALA_PX_100     91       /* Hundertstelpixel je Punkt (= 91 px je 100 Punkte) */
#define RE15_R41C_SKALA_Y          70
#define RE15_R41C_SKALA_H          4
#define RE15_R41C_LAMPE_Y          168      /* gruene Marke unter einem EIN-Hebel (PORT-WAHL) */
/* Hebel gehen sichtbar runter (Nachbesserung 5, Nachtrag 18 Punkt 2, Dossier §0f B5-2): Form der Original-Hebel ROOM11F0
 * sub06 @0x01322.. (`2f 05 40 00` Speed_set(5,+0x40) @0x01332, For 16 `0d 00 04 00 10 00` @0x01336, Add_speed + Evt_next):
 * 16 Bilder je Umlegen, 64 je Bild = 1024 = 90 Grad aus der Grundstellung rot_z 512 (Obj_model_set @0x00E76). Die 2050-Hebel
 * sind gemalt -> Griff + Klingen als Bildteil (gen/ventilkette_r41_hebel.inc, tools/r41_ventilkette/hebel2050.py), senkrecht
 * skaliert um den Drehpunkt mit cos(45 + 90 t/16 Grad) / cos(45 Grad). */
#define RE15_R41C_HEBEL_TAKTE      16       /* @0x01336 For 16  */
#define RE15_R41C_HEBEL_SCHRITT    64       /* @0x01332 +0x40   */
#define RE15_R41C_HEBEL_GRUND      512      /* @0x00E76 rot_z   */

/* ---- API ------------------------------------------------------------------------------------------------------ */
/* Nach dem Init-Lauf (scd_room_setup.c Installer-Block; main.c Boot-/CONTINUE-Weg). rdt darf NULL sein. */
void re15_ventilkette_r41_install(uint16_t room_id, const re15_rdt_t *rdt);
/* scd_event_fire-Kette: Port-Programm fuer (Raum, Ereignis) oder NULL (= Original). */
const uint8_t *re15_ventilkette_r41_ereignis(uint16_t room_id, uint8_t event_id);
/* op_sce_espr_on: 1 = diesen Sce_espr_on ueberspringen (20B0/20B1 Strahlen @0x01E96/@0x01EA6/@0x01EB6 bei Bit 100). */
int re15_ventilkette_r41_espr_unterdrueckt(uint16_t room_id, const uint8_t *pc, const uint8_t *raw);
/* Spiel-Takt (game_step_common.c, hinter den anderen Runden-Ticks). */
void re15_ventilkette_r41_tick(void);
/* Zuruecksetzen (Testaufbau). */
void re15_ventilkette_r41_reset(void);
/* Eingebackene Port-Props fuer main.c (PC): k = 0/1 Wasserflaechen obj 4/5, k = 2 Dokument-Blatt obj 6, k = 3 Plakat-Aushang
 * obj 7 (Nachbesserung 4); obj_id im Raum oder -1, MD1/TIM-Bytes je k. */
#define RE15_R41C_PROPS_N 4
int re15_ventilkette_r41_obj_id(uint16_t room_id, int k);
const uint8_t *re15_ventilkette_r41_md1_bytes(int *out_size);
const uint8_t *re15_ventilkette_r41_tim_bytes(int *out_size);
const uint8_t *re15_ventilkette_r41_md1_bytes_k(int k, int *out_size);
const uint8_t *re15_ventilkette_r41_tim_bytes_k(int k, int *out_size);
/* Plakat-Nahaufnahme (main.c zeigt sie ueber re15_render_pc_show_fmv): 320 x 240 RGBA8888 (0xRRGGBBAA), solange sie offen
 * ist, sonst NULL (PC; PSX NULL). */
const uint32_t *re15_ventilkette_r41_plakat_rgba(void);
/* Zeichner-Abfrage (main.c zeichnet, nach dem 11F0-Zeiger): nr 0 = Zeiger, nr 1..4 = gruene Marke unter dem
 * EIN-Hebel nr; 1 = zeichnen, Rechteck in *x0/*y0/*w/*h (320x240). Nur solange das 2050-Pult aktiv ist. */
int re15_ventilkette_r41_sicht(int nr, int *x0, int *y0, int *w, int *h);

/* ---- Messhaken (Riegel) --------------------------------------------------------------------------------------- */
int      re15_ventilkette_r41_timer_laeuft(void);
int      re15_ventilkette_r41_flut_laeuft(void);
int      re15_ventilkette_r41_ertrunken(void);
int      re15_ventilkette_r41_spritzer(void);     /* Spritzer-Ereignisse seit dem Raumstart (Riegel) */
unsigned re15_ventilkette_r41_schalter(void);      /* Maske der EIN-Hebel (Bit 0..3) */
int      re15_ventilkette_r41_spannung(void);      /* Summe der Gewichte der EIN-Hebel */
int      re15_ventilkette_r41_spannung_aus_maske(unsigned maske);
int      re15_ventilkette_r41_panel_aktiv(void);
int      re15_ventilkette_r41_panel_zone(int32_t x, int32_t z);   /* -1 oder Hebel 0..3 */
int      re15_ventilkette_r41_zeiger_x(int wert);                 /* Bild-x des Zeigers */
int      re15_ventilkette_r41_cut_20b0(int cut_2000);             /* Cut-Zuordnung 2000 -> 20B0 */
const uint8_t *re15_ventilkette_r41_programm(const char *name, int *out_len);
const uint8_t *re15_ventilkette_r41_text(uint16_t room_id, uint8_t id, int *out_len);
int      re15_ventilkette_r41_wasser_prop(int k);                 /* Pool-Index der Flaeche k oder -1 */
int      re15_ventilkette_r41_zustand_2030(void);                 /* RE15_R41C_2030_TROCKEN/_GEFLUTET/_RELING */
const uint8_t *re15_ventilkette_r41_md1_bytes_fuer(int reling, int *out_size);  /* Riegel: Netz je Zustand */
int      re15_ventilkette_r41_drehungen(void);                    /* vermerkte Drehungen (5,0x10..) 0..4              */
int      re15_ventilkette_r41_folge(int i);                       /* Ventil-Index der Drehung i oder -1               */
int      re15_ventilkette_r41_plakat_offen(void);                 /* 1 = Nahaufnahme offen                            */
int      re15_ventilkette_r41_ventil_nummer(int i);               /* 1, 2, 3, 7, 8, 9                                 */
/* Nachbesserung 5 */
int      re15_ventilkette_r41_standbild(unsigned *raum, int *cut);  /* 1 = Standbild der Ventile offen (2010), Quelle   */
unsigned re15_ventilkette_r41_ton_zaehler(int ton);               /* gespielte Toene je RE15_R41C_TON_* (Riegel)      */
int      re15_ventilkette_r41_ventil_rbj(unsigned room_id);       /* 1 = Raum bindet das RE2-RBJ VENTIL1080 (2030/2031) */
int      re15_ventilkette_r41_countdown(int *rest_bilder);        /* 1 = Countdown sichtbar (2030, Zaehler laeuft)      */
int      re15_ventilkette_r41_hebel_stellung(int i);              /* 0 (oben) .. 16 (unten), Hebel 0..3 im Pult 2050    */
/* Countdown als Nachrichten-Bytes (`05 02` Ziffern/':' `05 00` `01 00`) fuer re15_render_pc_msg_text; Rueckgabe Laenge. */
int      re15_ventilkette_r41_countdown_text(int rest_bilder, uint8_t *out, int cap);
/* Plattform-Haken fuer die Toene (PC setzt ihn; ohne Haken nur gezaehlt). */
extern void (*re15_ventilkette_r41_ton_hook)(int ton);

#endif /* RE15_VENTILKETTE_R41_H */
