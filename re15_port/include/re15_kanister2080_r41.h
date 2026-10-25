/*
 * RE1.5 Rebuilt — Runde 41 Spur G "kanister2080": ROOM2080/2081 nach der 2090-Szene der Spur F (Bit 110), der RE2-Gaskanister
 * an der Reling und die Marvin/John-Szene, die ihn ins Alligator-Becken ROOM2090/2091 wirft.
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde41/G_kanister2080.md.
 *
 * NUTZER-VORGABEN (woertlich, analysis/befunde_runde41/AUFTRAG.md):
 *   P1 "Nach dem Start der Cutscene, wo der Aligator den Zombie mit dem Schluessel frisst, soll oben im ROOM 2080 im
 *       Hauptbereich Erst ein Sound kommen der schliessendem Metall Tuer. Dann sollen oben im Hauptbereich 8 Zombies
 *       stehen die gespawned sind."
 *   P2 "Ich moechte das im ROOM 2080 am Rand der Reling der Gaskanister steht, der in Resident Evil 2 dafuer sorgt, das man
 *       den Aligator besiegt. Siehe Gas_Can.bmp."
 *   P3 "Hat man den Aligator oft genug angeschossen, soll eine Cutscene Triggern. Es gibt erneut die Geraeusche eine
 *       schliessenden Tuer. Marvin (Bei Leons Szenario), John (Bei Elzas Szenario) ist im ROOM 2080. Toetet alle 8 Zombies
 *       mit Kopfschuss - ein Schuss - Kopf platzt - tot. Rennt zu den Gas Kanister nimmt ihn auf in beide Haende und sagt so
 *       etwas wie - "Hey, Leon, try with this!" Dann schmeisst er den Gaskanister die Reling runter. Der liegt dann in 2090
 *       an der Stelle im Wasser. Wenn der Aligator dran vorbei kommt, soll er ihn aufnehmen, GENAUSO wie im original
 *       Resident Evil 2. Sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen, und die Sterbe Animation mit
 *       geplatzen Kiefer etc. vom Aligator soll genauso kommen wie im Original."
 *   Nachtrag 5: "Auch beim runterschmeissen des Gaskanisters - es gibt eine Szene in Resident Evil 2, wo Ada den Raketenwerfer
 *       runter wirft. GENAUSO wuerde ich das auch haben wollen von der Kamera."
 *   Nachtrag 17 (zu P1): "Muss natuerlich ein knallen sein, als ob die Zombies rein brechen, so wie bei STAGE1 von Elza/Leon
 *       am Ende in der Cutscene nach Irons."
 *   Nachtrag 20: "In 2080, nachdem die Zombies durch die Tuer gebrochen sind, sollen sie oberhalb von der Treppe laufen,
 *       nicht unten."
 *
 * FORMEN (Belege im Dossier §1):
 *   - Raumwechsel in der Szene = Door_aot_set (0x3B) + Aot_on (0x47) "fire now" (Port-Montage irons_tod_1150.c /
 *     elzazombies_r39.c; Vorbilder ROOM1240 sub02/sub03, ROOM1080 sub07..10). 2080 und 2090 teilen x/z (RVD-Vierecke
 *     ROOM2080 @0x188 Cut 5/6/8 = ROOM2090 @0x220; Tuer 2080 Slot 1 @0x0BC2 (-9650,-26150) -> 2090 (-8200,0,-25550)); die
 *     Bahn endet sichtbar im 2080-Bild, in 2090 liegt der Kanister an seiner Spielstelle (RE2 ROOM7020 sub13 Pos_set nach dem
 *     Aufschlag @0x08CA, RE15_K2080_LIEGE_X/Z, Nachbesserung 1).
 *   - Zombies = Sce_em_set-Saetze (Form ROOM2020 sub00 @0x008F6 `44 00 16 02 01 00 00 bc ..`: Typ 0x16 "Steher" grid 0x02),
 *     Etage 8 (y = -8*1800, @0x800421d4), Tot-Bits Zone 7 (Gate @0x80042128-38).
 *   - KNALL (P1, Nachtrag 17) = derselbe Tuerknall wie nach Irons (irons_tod_1150.c:1357 / elzazombies_r39.c:1123):
 *     Knall-Bank gen/knall_bank.inc Satz 0 = RE2 ROOM1060 sub05 Se_on @0x0F48 `36 02 0a 01 ..`, EDT[0x0a] @0x3F80
 *     `00 00 85 20`, VAG 5 @0x066B8 (8768 B, "Zombies breaking in", RE2_SOUND_LISTE.md ROOM106).
 *   - Tuer-Ton (P3 "erneut die Geraeusche einer schliessenden Tuer", Marvin/John kommt herein) = RE2-Tuerbank Satz 1
 *     (Door_exit @0x800141f0 `jal 0x8005ba28`, `lui a0,0x1` @0x800141f4) des Tonteils DOOR06 (Stahltuer mit X-Kreuz; Tuer
 *     2080 Slot 0 <-> 2030, gen/tuer_zuordnung.inc S161 T075; Tabelle @0x8009A568).
 *   - EBENE (Nachtrag 20): die Zombies stehen auf Band 8 (oberhalb der Treppe Aot_set Slot 2 @0x0BE2, Band 5 -> 8); die
 *     Band-8-Zellen der SCA @0x4BC umschliessen Laufgang/Absatz/Balkon, die Absatz-Westwand @0x05C4 endet bei x -16650 ->
 *     Gegner (FUN_8003b0a4 Band-Vergleich @0x8003b228-3c) kommen nicht auf die Treppe.
 *   - NPC-Schuss = ROOM1021 sub02 (John 0x49): Plc_motion(2,6) @0x020C6, Sleep 110 @0x020CA, Plc_motion(2,7) @0x020CE,
 *     Sleep 30 @0x020D2, Senken Plc_motion(2,6)+Plc_flg 0x80 @0x020EA/@0x020EE, Sleep 20 @0x020F2; zwischen zwei Zielen wartet
 *     das Programm, bis das Nachdrehen (0xC8/Bild @0x80033f90) am Ziel ist (Signal AUSGERICHTET, kein festes Sleep).
 *   - Aufnehmen (Nachtrag 24b) = Marvins Hinknien/Aufstehen ROOM11B0 sub06 @0x01650-0x01660 (Loco c11 vor/rueckwaerts).
 *   - Wurf = RE2 ROOM7020 sub12/sub13 (Adas Raketenwerfer, Zensus Item_aot_set 0x11): Cut_chg 7 @0x0726, Plc_motion
 *     @0x0750 (Adas Clip `3f 0a 12 40` -> PORT-WAHL Marvins/Johns Eigen c6, sie haben keinen Wurf-Clip, Dossier 8.5),
 *     Message_on @0x0764, Sleep 30 @0x076A, Plc_motion @0x0782, Sleep 20 @0x0786, Super_reset @0x078E, Fall
 *     Speed_set 2/1/0 = 10/20/25 + 8/7/6 = 0/20/5 @0x088E-08A2 mit Add_speed/Add_aspeed (@0x80055ab0/@0x80055b2c) bis
 *     y >= Boden - 800 (Member_cmp 0x0c Op 3 @0x08B0 -> @0x80056008/@0x80055f10), Aufschlag-Se_on @0x08BA, Sleep 15
 *     @0x07A8, Schnitt auf den Fangenden (Cut_chg 11 @0x07D4), Ende Plc_ret/Set(2,7,0)/Set(1,27,0)/Cut_auto (sub18
 *     @0x0A2C-0A38).
 *   - Aufschlag im Wasser = RE2 ROOM40A0 sub10 @SCD 0x0686 `36 02 0b 01 04 01 ..` (Satz 0x0b der Raumbank, der Kanister
 *     faellt ins Wasser) -> Mini-Bank shared_assets/RE2/KANISTER2080.VBS.
 *   - Kopfschuss = Treffer ueber den Anwender des Spielerschusses (re15_player_weapon_fire) mit der Magnum-Kritklasse w7
 *     (@0x80012380-88 -> hp = -1 fuer Typ < 0x20 @0x800124fc-51c) -> RE2-Zeile 5 -> FUN_801092C4 "der Kopf platzt ab"
 *     (enemy_ai_re2_zombie.c re2z_death_magnum). PORT-WAHL: Schuetzenlage = NPC (das Original kennt keinen NPC-Schaden).
 */
#ifndef RE15_KANISTER2080_R41_H
#define RE15_KANISTER2080_R41_H

#include <stdint.h>

/* ---- Raeume ---------------------------------------------------------------------------------------------------------- */
#define RE15_K2080_RAUM_2080       0x2080u
#define RE15_K2080_RAUM_2081       0x2081u
#define RE15_K2080_RAUM_2090       0x2090u
#define RE15_K2080_RAUM_2091       0x2091u
#define RE15_K2080_STAGE_BYTE      0x01    /* Door_aot_set +22 = Stage-Index (ROOM2080 @0x0BC2 +22 `01` -> STAGE2) */
#define RE15_K2080_RAUM_BYTE_2080  0x08    /* ROOM2030 Tuer @0x1AEA +23 = `08` (Elza-Variante aus dem Elza-Bit, @0x800397ec) */
#define RE15_K2080_RAUM_BYTE_2090  0x09    /* ROOM2080 Tuer Slot 1 @0x0BC2 +23 = `09` */

/* ---- Bits (VERTRAG 1.1: G = Bank 9 Bits 114-119; Spur F = 110-113) ------------------------------------------------------ */
#define RE15_K2080_BANK            9
#define RE15_K2080_BIT_F_SZENE     110   /* Spur F: 2090-Ankunftsszene gesehen (nur gelesen) */
#define RE15_K2080_BIT_ANKUNFT     114   /* P1: Knall beim ersten Betreten von 2080/2081 nach Bit 110 gespielt */
#define RE15_K2080_BIT_SZENE       115   /* P3: Marvin/John-Szene gelaufen (Kanister geworfen, Zombies tot, kein Prop mehr) */
#define RE15_K2080_BIT_KANISTER    116   /* P3: Kanister liegt im Becken 2090/2091, bis der Alligator an ihm stirbt */

/* ---- Signale Programm <-> Takt (Bank 5 Wort 0 = Raum-Scratch: beim Raum-Init geloescht, `sw zero` FUN_8003ecec @0x8003ed74,
 * Port scd_room_setup.c; in ROOM2080/2081/2090/2091 ohne Ck/Set Bank 5 und ohne Plc_dest, Zensus scd_dump_room.py) ------ */
#define RE15_K2080_SIG_BANK        5
#define RE15_K2080_SIG_ZIEL        0x18  /* Programm -> Takt: naechsten Zombie anvisieren */
#define RE15_K2080_SIG_SCHUSS      0x19  /* Programm -> Takt: Schuss (Kopfschuss-Treffer + Ton) */
#define RE15_K2080_SIG_AUFHEBEN    0x1A  /* Programm -> Takt: Kanister in die Haende */
#define RE15_K2080_SIG_WURF        0x1B  /* Programm -> Takt: loslassen (Super_reset @0x078E) + Fall */
#define RE15_K2080_SIG_GELANDET    0x1C  /* Takt -> Programm: Aufschlag (ROOM7020 Set(5,0x0e) @0x08B6) */
#define RE15_K2080_SIG_TUERTON     0x1D  /* Programm -> Takt: Tuer-Ton der Szene (DOOR06 Satz 1, Marvin/John kommt herein) */
#define RE15_K2080_SIG_AUSGERICHTET 0x1E /* Takt -> Programm: NPC steht auf dem Ziel (Nachdrehen 0xC8/Bild @0x80033f90 fertig) */
#define RE15_K2080_SIG_ANKUNFT     0x17  /* Plc_dest-Ankunftsflag (+0x1c3 @0x80041c24; Form ROOM1170 `00 05 21`) */
#define RE15_K2080_TUER_SLOT       21    /* AOT-Slot der Raumwechsel (frei in 2080 0..4 und 2090 0..5; Montage nutzt 20) */

/* ---- Zombies (P1) -------------------------------------------------------------------------------------------------- */
#define RE15_K2080_N_ZOMBIES       8
#define RE15_K2080_ZOMBIE_TYP      0x16  /* STAGE2-Zombie (ROOM2020 sub00 @0x008F6, ROOM2000 @0x017CC) */
#define RE15_K2080_ZOMBIE_GRID     0x02  /* Steher (ROOM2020 @0x008F6 pc[3]; ROOM10E1 Else-Zweig @0x00DBE) */
#define RE15_K2080_ETAGE           8     /* oberer Laufgang: Tuer Slot 1 nfloor 8, SCA floor-Byte 0x88 (@0x534..0x5C4) */
#define RE15_K2080_Y               (-14400)  /* = -8 * 1800 (Sce_em_set +0x1ba @0x800421d4-8004220c) */
#define RE15_K2080_TOT0            236   /* Zone-7-Tot-Bits 236..243 (Zensus Dossier 1.6: frei im Original und im Port) */
/* Ebene (Nachtrag 20, Dossier 1.11): die Treppe = Aot_set Slot 2 sce 12 @0x0BE2 (Band-Byte 0x80, side 0, Bandsprung 3:
 * unten Band 5 an der 2030-Tuer, oben Band 8), Rechteck x -22700..-16400 z -17850..-15550; die Band-8-Absatzwand SCA
 * @0x05C4 (x -17650..-16650, z -19300..-13750) schliesst die obere Ebene nach Westen ab. */
#define RE15_K2080_TREPPE_X0       (-22700)
#define RE15_K2080_TREPPE_X1       (-16400)
#define RE15_K2080_TREPPE_Z0       (-17850)
#define RE15_K2080_TREPPE_Z1       (-15550)
#define RE15_K2080_ABSATZ_WAND_X   (-16650)  /* Ostkante der Zelle @0x05C4: westlich davon ist kein Band-8-Boden */

/* ---- Kanister (P2) ------------------------------------------------------------------------------------------------- */
/* Rueckprojektion der Markenmitte (167,156) aus Gas_Can.bmp (= Cut 5, Bildvergleich 0,00) auf y -14400 (Dossier 1.5). */
#define RE15_K2080_KAN_X           (-6745)
#define RE15_K2080_KAN_Z           (-17625)
#define RE15_K2080_KAN_LAENGE      2784  /* KANISTER40A0.MD1 y -2784..0 (Fuss im Ursprung), r 267 */
#define RE15_K2080_KAN_RADIUS      267

/* ---- Marvin / John (P3) -------------------------------------------------------------------------------------------- */
#define RE15_K2080_NPC_LEON        0x40  /* Marvin (ROOM2000 sub @0x017B4 `44 01 40 40 ..`) */
#define RE15_K2080_NPC_ELZA        0x49  /* John (ROOM2001 @0x0179E `44 00 49 40 ..`) */
#define RE15_K2080_NPC_SLOT        8     /* Skript-Slot (Aktor 9; Zombies 0..7) */
#define RE15_K2080_NPC_GRID        0x40  /* stehender NPC (ROOM2000/2001 pc[3] = 0x40 -> Sub 6) */
/* Lagen (PORT-WAHL, gemessen mit tools/r41_kanister2080/probe_lagen.c, Dossier 3.2): Start = Treppenabsatz Band 8
 * (SCA @0x51C x -16650..-14300, z -18300..-14750), Blick nach Osten auf den Laufgang (Gier 0 = +x, atan2(-dz,dx)); Stand am
 * Kanister = 700 vor der Markenmitte (Koerper 400 + Kanister 267, SCA-Balkon @0x528). */
#define RE15_K2080_NPC_X           (-14800)
#define RE15_K2080_NPC_Z           (-16500)
#define RE15_K2080_NPC_STAND_X     (-7245)   /* Aufheben: 500 vor der Markenmitte (Haende Loco c11 Bild 24 bei +249..+593, Dossier 8.1) */
#define RE15_K2080_NPC_STAND_Z     (-17625)
#define RE15_K2080_LAUF_UEBER      250       /* RUN-Ankunft bei d < 300 (`slti 300` @0x80051764): Ziel 250 hinter dem Stand */
/* Wurf an der Reling: die Reling-Zelle @0x5A0 beginnt bei x -6700; Koerperradius 400 (+0x78-Satz +6) -> Mitte -7100 = Nennlage des
 * Werfers fuer die Bahn-Nachrechnung (Riegel unit_r41_kanister2080_bahn). Seit Nachbesserung 1 wirft er dort, wo er den Kanister
 * aufgehoben hat (Ankunft RUN d < 300 @0x80051764 vor STAND_X + LAUF_UEBER, der Kanister steht schon an der Reling): die Haende
 * (Eigen c6 Ende +840 vorn) liegen dann 1450 ueber der Relingstange (Bodenlinie x -6350 aus Cut 5, Pfosten (139,151)/(200,151)
 * Rueckprojektion, Oberkante ~ -15443), die RE2-Bahn faellt frei darueber (Dossier 3.4/8.1). */
#define RE15_K2080_NPC_WURF_X      (-7100)
/* Clips (Nachbesserung 1, Dossier 8.1: JEDES Bild mit tools/r41_kanister2080/probe_npc_clip_bilder.c, G_belege/nb1_clipbilder_40_49.txt;
 * Original-Verwendung tools/r41_kanister2080/plc_motion_zensus.py, G_belege/nb1_plc_motion_zensus.txt):
 *   Eigenbank (Sub 2) c6 = beidhaendiges Anheben/Zielen (10 B.), c7 = Feuern (16 B.) - ROOM1021 sub02 @0x020C6/@0x020CE;
 *   Loco (Sub 1) c11 = HINKNIEN (25 B., Wurzel -1810 -> -761, beide Haende vorn unten), rueckwaerts (Plc_flg 0x80) = AUFSTEHEN -
 *   Marvin ROOM11B0 sub06 `3f 01 0b 00` @0x01650, Sleep 60 @0x01654, `3f 01 0b 00` + `43 00 80 00` @0x01658/@0x0165C, Sleep 30
 *   @0x01660 (John ROOM1021 sub02 @0x02102). NICHT mehr: Loco c2 = PL00-KLETTERN (Aufstieg, `ori v0,zero,0x2` @0x80038134 -
 *   Nachtrag 24b "Kletter-Animation") und Eigen c0 = RUN-Gait (Sub 5 `sb zero,148` @0x8005157c, kein "Tragen"). */
#define RE15_K2080_CLIP_ZIELEN     6
#define RE15_K2080_CLIP_FEUER      7
#define RE15_K2080_CLIP_KNIEN      11    /* Loco c11 = Plc_motion(1,11) ROOM11B0 sub06 @0x01650 */
#define RE15_K2080_KNIEN_BILDER    25    /* Bildzahl Loco c11 (EDD 0x40 und 0x49, Schluesselbilder kf 236..260) */
#define RE15_K2080_KNIEN_HALT      60    /* Sleep 60 `09 0a 3c 00` ROOM11B0 sub06 @0x01654 (Knien bis zum Aufstehen) */
#define RE15_K2080_AUFSTEHEN_HALT  30    /* Sleep 30 `09 0a 1e 00` ROOM11B0 sub06 @0x01660 (nach dem Aufstehen) */
/* Griff = letztes Bild des Hinkniens (Bild 24): beide Haende liegen seitlich am Kanister (HR (249,-668,-509), HL (593,-312,318) zur
 * Wurzel; Kanister-Achse 500..550 vor der Wurzel, Radius 267) - von dort traegt der Rueckwaerts-Clip ihn mit hoch. */
#define RE15_K2080_KNIEN_GRIFF     (RE15_K2080_KNIEN_BILDER - 1)
#define RE15_K2080_NACHFUEHREN     0xC8  /* Gier je Bild beim Anvisieren = Gun-FSM Sub 5 `ori a1,zero,0xc8` @0x80033f90 */

/* ---- Wurf (RE2 ROOM7020 sub12/sub13) ------------------------------------------------------------------------------- */
#define RE15_K2080_WURF_VX         25    /* Speed_set(0, 25) @0x0896 */
#define RE15_K2080_WURF_VY         20    /* Speed_set(1, 20) @0x0892 */
#define RE15_K2080_WURF_VZ         10    /* Speed_set(2, 10) @0x088E */
#define RE15_K2080_WURF_AX         5     /* Speed_set(6, 5)  @0x08A2 */
#define RE15_K2080_WURF_AY         20    /* Speed_set(7, 20) @0x089E */
#define RE15_K2080_WURF_AZ         0     /* Speed_set(8, 0)  @0x089A */
#define RE15_K2080_WURF_STOP       800   /* Member_cmp(y < -800) @0x08B0: Ende 800 ueber der Landeflaeche */
#define RE15_K2080_WASSER_HOCH     (-10800)  /* ROOM2080 sub00 @0x0C2E Obj_model_set y, solange (3,245) = 0 */
#define RE15_K2080_WASSER_TIEF     (-3420)   /* ROOM2080 sub00 @0x0C5E Obj_model_set y nach dem Rechner (3,245) = 1 */
#define RE15_K2080_GATOR_WASSER_Y  (-1200)   /* B: Kanister-Hoehe im Becken = GB_WATER_Y (0x23-KI @0x8010c65c) */
/* Liegestelle im 2090-Becken (Nachbesserung 1, M2 / Nachtrag 23 "Aligator nimmt irgendwann in dem weiteren Rundlauf den Kanister
 * auf"). FORM RE2: nach dem Aufschlag (Se_on 0x0e @0x08BA) legt ROOM7020 sub13 den Raketenwerfer per Pos_set an seine SPIELSTELLE
 * `32 00 b6 9f 00 00 10 aa` @0x08CA = (-24650,0,-21998) im Item-AOT 5 (@0x00DA: x -26200..-22800, z -23200..-20700), nicht ans
 * Bahnende: die Bahn bewegt z nur um v_z 10/Bild (Speed_set(2,10) @0x088E, Add_speed reine Weltaddition @0x80055abc-e0), Ada steht
 * bei z -17900 (Pos_set @0x0740) -> der Pos_set versetzt um mehrere tausend in z. PORT-WAHL der Spielstelle, GEMESSEN (Dossier
 * 8.2): nach der Szene taucht der Alligator nur auf, wenn Leon naeher als 9000 an seinen Lauerplatz (-7200,-9200) kommt (Spur B
 * GB_AGGRO/GB_START, Nutzer-Entscheid N22.2 "Erst beim Annaehern") - der weitere Kampf laeuft also im Nordbecken. Sonde
 * tools/r41_kanister2080/probe_liegestelle.c (echter Boss-Tick, 50 Leon-Lagen < 9000 vom Lauerplatz): Plateau 25/50 Aufnahmen um
 * (-6500..-5500, -11000..-9500), Kanalmitte (-5300,-16300) 15/50; echte exe (natuerlicher Gator, Leon/Elza schiessen, 7 Lagen):
 * hier 7/7 Aufnahmen und 7/7 R1-Tode, Kanalmitte 1/4 ohne Tod. */
#define RE15_K2080_LIEGE_X         (-6500)
#define RE15_K2080_LIEGE_Z         (-11000)
/* Haende am Ende von Eigen c6 (Mitte der Bones 11/14, Zensus Dossier 1.8): Fallback, wenn die NPC-Bank nicht geladen ist. */
#define RE15_K2080_HAND_VOR        840
#define RE15_K2080_HAND_HOCH       (-2496)

/* ---- Kameras ------------------------------------------------------------------------------------------------------- */
#define RE15_K2080_CUT_SCHUSS      2     /* RID @0x0A0: Treppenabsatz + Laufgang von Osten (Probe: alle 8 Lagen + NPC im Bild) */
#define RE15_K2080_CUT_KANISTER    5     /* RID @0x0100: Balkon/Kanister (= Gas_Can.bmp) */
#define RE15_K2080_CUT_WURF        7     /* RID @0x0140: Totale von der Grubenseite = RE2 ROOM7020 Cut 7 (Werfer von der Fang-Seite) */
/* Parkplatz des Spielers waehrend des 2080-Teils: Treppenfuss an der 2030-Tuer (Band 5, Eintritt ROOM2030 -> 2080
 * (-23200,-9000,-16700), re15_room_spawns.h) - in keiner der drei Szenen-Kameras im Bild (Probe, Dossier 3.2). */
#define RE15_K2080_PARK_X          (-23200)
#define RE15_K2080_PARK_Y          (-9000)
#define RE15_K2080_PARK_Z          (-16700)
#define RE15_K2080_PARK_ETAGE      5

/* ---- Ablauf (nur RAM) ---------------------------------------------------------------------------------------------- */
#define RE15_K2080_AUS             0
#define RE15_K2080_NACH_2080       1     /* 2090: Szene ausgeloest, Raumwechsel nach 2080 laeuft */
#define RE15_K2080_IN_2080         2     /* 2080: Marvin/John-Teil */
#define RE15_K2080_RUECK_2090      3     /* 2080 -> 2090: Rueckkehr laeuft */

/* ---- Haken ----------------------------------------------------------------------------------------------------------- */
/* HAKEN scd_room_setup.c (Ende des Installer-Blocks): Raum-Aufbau 2080/2081/2090/2091. */
void re15_kanister2080_r41_install(uint16_t room_id);
/* HAKEN scd_vm.c Bildtakt (neben re15_elzazombies_r39_tick): Ausloeser, Signale, Wurfbahn. */
void re15_kanister2080_r41_tick(void);
/* Plattform-Haken (PC: platform/pc/src/kanister2080_pc.c). NULL = stumm. */
extern void (*re15_k2080_knall_hook)(void);      /* P1: Knall-Bank (gen/knall_bank.inc) in die Tuerbank + Satz 0 */
extern void (*re15_k2080_tuerton_hook)(void);    /* P3: Tonteil DOOR06 in die Tuerbank + Satz 1 */
extern void (*re15_k2080_platsch_hook)(void);    /* KANISTER2080.VBS Satz 0 (RE2 ROOM40A0 Satz 0x0b) */
extern void (*re15_k2080_schuss_hook)(void);     /* NPC-Schuss = RE15_ELZA_TON_SCHUSS (RE2 ROOM4040 Satz 0x0A) */
/* Zeichner: 1 = der Kanister ist in diesem Bild in 2080/2081 sichtbar; pos = Welt (Fuss-Ursprung des MD1), rot = Q12 3x3. */
int  re15_k2080_kanister_lage(int32_t pos[3], int32_t rot[9]);

/* ---- Pruefhaken ---------------------------------------------------------------------------------------------------- */
int  re15_k2080_phase(void);
int  re15_k2080_zombie_lage(int i, int32_t *x, int32_t *z, int16_t *gier);
int  re15_k2080_landepunkt(int32_t *x, int32_t *z);       /* 1 = Wurfbahn gerechnet (Lage im 2090-Becken) */
void re15_k2080_bahn_rechnen(const int32_t start[3], int16_t gier, int32_t wasser_y, int32_t ende[3], int *bilder);
int  re15_k2080_zaehler(int was);   /* 0 Knall/Tuer-Toene, 1 Schuesse, 2 Kopf-Tode, 3 Ausloesungen, 4 Kanister gesetzt (2090) */
/* Messhaken Ebene: Zahl der aktiven 2080-Zombies (Typ 0x16) und davon westlich der Absatzwand bzw. nicht auf y -14400. */
int  re15_k2080_zombies_oben(int *aktiv, int *auf_treppe, int *falsche_hoehe);
uint8_t re15_k2080_npc_typ(void);   /* 0x40 Leon, 0x49 Elza */
const uint8_t *re15_k2080_programm(int nr, int *out_len);   /* 0 Spawn 2080, 1 Szene 2080, 2 Start 2090, 3 Rueckkehr 2090 */
void re15_k2080_reset(void);

#endif /* RE15_KANISTER2080_R41_H */
