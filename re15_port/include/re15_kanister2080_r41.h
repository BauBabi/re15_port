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
 *     ROOM2080 @0x188 Cut 5/6/8 = ROOM2090 @0x220; Tuer 2080 Slot 1 @0x0BC2 (-9650,-26150) -> 2090 (-8200,0,-25550)), der
 *     Kanister faellt nach dem Loslassen in 2090 (Cut 12, FG-E4) und liegt dort, wo er aufschlaegt (FG-E3).
 *   - Derselbe Alligator nach der Szene (FG-E3): Aktor-Kopie beim Ausloesen, beim Rueckweg in denselben Slot zurueck.
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
 *   - Nachbesserung 2: Dir_set der Wurfrichtung erst mit dem Schnitt auf Cut 7 (RE2 @0x0726 -> @0x0748); Wsleep/Wsleeping
 *     (RE2 @0x079A/@0x079B: Zeile + Stimme zu Ende) VOR dem Loslassen, weil der Raumwechsel nach 2090 sie beenden wuerde;
 *     Wurfrichtung Nord/Sued nach der Lage des Alligators; Anschwimmen = RE2-Zielmodus (RE15_K2080_ANSCHWIMM_AB).
 *   - Aufschlag im Wasser = RE2 ROOM40A0 sub10 @SCD 0x0686 `36 02 0b 01 04 01 ..` (Satz 0x0b der Raumbank, der Kanister
 *     faellt ins Wasser) -> Mini-Bank shared_assets/RE2/KANISTER2080.VBS.
 *   - Kopfschuss = Treffer ueber den Anwender des Spielerschusses (re15_player_weapon_fire) mit der Magnum-Kritklasse w7
 *     (@0x80012380-88 -> hp = -1 fuer Typ < 0x20 @0x800124fc-51c) -> RE2-Zeile 5 -> FUN_801092C4 "der Kopf platzt ab"
 *     (enemy_ai_re2_zombie.c re2z_death_magnum). PORT-WAHL: Schuetzenlage = NPC (das Original kennt keinen NPC-Schaden).
 */
#ifndef RE15_KANISTER2080_R41_H
#define RE15_KANISTER2080_R41_H

#include <stdint.h>
#include "re15_actor.h"   /* re15_actor_t (Nachbesserung 2: Haken im Boss-Takt) */

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
#define RE15_K2080_BIT_SUEDWURF    117   /* Nachbesserung 2 (N3): der Wurf ging ins Suedbecken (Gier RE15_K2080_WURF_GIER_SUED),
                                          * sonst ins Nordbecken - fuer den Raumaufbau ohne Laufzeit-Bahn (Spielstand/Continue) */

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
/* Nachbesserung 2 (N1): Takt -> Programm "Wsleeping erfuellt" = keine Nachricht steht mehr und keine Sprachaufnahme laeuft mehr.
 * RE2 ROOM7020 sub12 Wsleep `0b` @0x079A / Wsleeping `0c` @0x079B; Wsleeping (Tafel @0x800a74c8[12] = 0x80053ac0) gibt ab,
 * solange 0x800cfbd8 & 0x20 (`andi v0,v0,0x20` @0x80053acc; Bit gesetzt vom Xa_on-Handler `ori v0,v0,0x20` @0x80058060, d.h. die
 * XA-Stimme laeuft) oder 0x800e873c & 0x80 (`andi v0,v0,0x80` @0x80053ae4; gesetzt vom Nachrichten-Start FUN_8002fe38
 * @0x8002fe80/88) steht. Port: g_scd.message_active (inkl. Nutzer-Nachhall) / g_re15_voice_laeuft + g_re15_voice_restbilder. */
#define RE15_K2080_SIG_WSLEEP      0x1F
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
/* FG-E4 (Orchestrator-Entscheid 2026-10-25 00:56, Abnahme G 0 M3): der Fall wird "von unten" in ROOM2090/2091 Cut 12 gezeigt
 * (RID @0x60 + 12*32: pos (-1601,-2131,-11018) -> (-5184,-3926,-13767), flag 1; 59,6 Grad unter dem Werfer-Kopf, die
 * naechstliegende vorhandene Kamera zu RE2 ROOM7020 Cut 7 von unten 43,6 Grad, Dossier 8.4/8.9). Landeflaeche dort = die
 * Wasseroberflaeche des Beckens y -1620 (ROOM2090 main00 Aot_set sce 8 Slot 2/3/4 @0x009D6/@0x009EA/@0x009FE p1 `ac f9`;
 * Obj_model_set Typ 3 Wasser y -1620); RE2 sub13 stoppt 800 darueber (Member_cmp y < -800 @0x08B0). Danach liegt der Kanister
 * an der Aufschlagstelle auf der Becken-Hoehe der Aufnahme (RE15_K2080_GATOR_WASSER_Y, FG-E3 "der Kanister landet so"). */
#define RE15_K2080_CUT_FALL        12
#define RE15_K2080_WASSER_2090     (-1620)
/* Wurfrichtung (Gier des Werfers beim Anheben; die RE2-Bahn v (25,20,10) a (5,20,0) @0x088E-08A2 wird um sie gedreht, wie
 * seit Abnahme 0 - RE2 wirft in Weltachsen nach +x (Add_speed Weltaddition @0x80055abc-e0), die Reling-Richtung des Ports ist
 * frei). PORT-WAHL, GEMESSEN Dossier 8.9 (exe-Laeufe der ganzen Folge ohne Testhaken, Messhaken RE15_K2080_WURF_GIER):
 *   3072 (Blick +z, Nordbecken): Aufschlag (-7515,-12304) nach 38 Bildern, 14 von 39 Fall-Bildern in 2090 Cut 12 (24..37);
 *        derselbe Alligator nimmt ihn in 5 von 6 Verlaeufen auf (Aufnahme/R1 nach der Rueckkehr F104/F570, F619/F833,
 *        F3675/F3882, F3130/F3454, Elza F619/F833; ein Verlauf ohne Aufnahme in 4200 Bildern, Kampf in der SW-Ecke).
 *   1024 (Blick -z, Suedbecken, Stand vor FG-E3): 0 von 2 Aufnahmen; 0 (+x): 0 von 1; 640: 0 von 1; 3200: 3 von 4;
 *   3328: 1 von 1; 2816: Aufschlag x -9586 ausserhalb des Beckens (x > -8900). */
#define RE15_K2080_WURF_GIER       3072
/* Nachbesserung 2 (N3, Dossier 9.3): zwei gemessene Wurfrichtungen (Tabelle 8.9), die Wahl faellt beim Bau des Szenen-Programms
 * nach der Lage des Alligators beim Ausloesen (FG-E3 "der Kanister landet so, dass der Alligator ihn ... aufnimmt"): der
 * Landepunkt, der WEITER vom Alligator liegt - RE2 braucht den Kanister mindestens 8880 voraus (Zielmodus @0x80100a88-8c) und
 * holt ihn ueber das Fenster 8601..9000 (@0x80100ad8-ec), also aus dem Anlauf.
 *   NORD 3072: Aufschlag (-7515,-12304), 14 von 39 Fall-Bildern in 2090 Cut 12;
 *   SUED 1024: Aufschlag (-6821,-22914), Fall samt Aufschlag in Cut 12 (17-19 Bilder, Dossier 8.9). */
#define RE15_K2080_WURF_GIER_NORD  RE15_K2080_WURF_GIER
#define RE15_K2080_WURF_GIER_SUED  1024
/* Nachbesserung 2 (N3): ANSCHWIMMEN = RE2-Zielmodus +0x1C0 & 4 in der Beckengeometrie des Ports. RE2 (EM23_OVL_0000.BIN,
 * selbst gelesen): Bank 5 Bit 1 (Kanister liegt) und Stufe < 2 -> Zielmodus an (@0x80100a34-5c), Ziel = der Kanister
 * (x, y - 800, obj[1].z) @0x80100a4c-70; aus, sobald obj[1].z - z < 9000 - 120 = 8880 (`addiu v0,v0,-9000 / slti v0,v0,-120`
 * @0x80100a88-8c) und Stufe 0; solange er an ist, schaerft FUN_801012fc den Biss nicht (@0x8010134c-58). RE2s Gator lenkt den
 * Koerper nie - er schwimmt seine Korridorbahn (x -23078 `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c), und der
 * Kanister liegt AUF dieser Bahn: Fall v_x -40 `2f 00 d8 ff` @0x0672 x 23 Bilder @0x0676, Treiben v_x -36 `2f 00 dc ff`
 * @0x06A2 x 12 + 4 Bilder @0x06AA/@0x06BC (ROOM40A0 sub10), Liegestelle (-22816,-1984,-9500) sub05 Pos_set @0x0484 = 262 neben
 * der Bahn. Im offenen 2090-Becken hat der Port-Gator keine feste Bahn (Verfolgung = Nutzer-Design der Spur B); die Bahn,
 * die ueber den Kanister fuehrt, ist hier das Steuerziel: solange der Kanister liegt (Stufe 0) und >= 8880 x Zeichen-Skala
 * entfernt ist, schwimmt der Alligator ihn an (Kanten-Umweg/Klemme/Wende wie die Verfolgung) und beisst nicht. */
#define RE15_K2080_ANSCHWIMM_AB    (9000 - 120)
/* Nachtrag 26 (Nutzer-Auswahl "Ja, RE2-Lenkung": "im Kanister-Bereich lenkt er Hals und Schwimmweg zum Kanister") + FG-E5: die
 * RE2-HALSFUEHRUNG im Zielmodus = FUN_800177c0 (RE2_Quellcode_V2/FUN_800177c0.c, Zweig +0x1C0 & 4): Ziel (+0x170/+0x172/+0x174),
 * Gier-Residual zum Koerper auf +-Klemme begrenzt, Akku-Ziel = Ziel - Keyframe-Winkel des Parts, Akku slewt mit festem Schritt
 * und wird zum Bauen der Part-Matrix auf den Keyframe ADDIERT (danach wieder abgezogen). Werte des Gators (EM23_OVL_0000.BIN,
 * selbst gelesen): Hals-Part +0x1C1 = 5 (`addiu v0,zero,5` / `sb v0,449(s0)` @0x801001c4-cc, INIT @0x80100590), Schritt
 * +0x9C/+0x9E = 20 (`addiu v0,zero,20` / `sh v0,1016(v1)` / `sh v0,1018(v1)` @0x801005a0-ac), Klemmen +0xA0 = 560 / +0xA2 = 312
 * (@0x801005b0-b8, `sh v0,1020(v1)` / `sh v0,1022(v1)` @0x801005b4/@0x801005c4), Ziel y = eigene y - 800 (`addiu v1,v1,-800`
 * @0x80100a58). Ausserhalb des Zielmodus: Akku -> 0 (FUN_800177c0 SELBST-Zweig: Ziel = Keyframe; der normale RE2-Halsmodus
 * FUN_80017d98 bleibt wie bei Spur B OFFEN, B §11.3). */
#define RE15_K2080_HALS_PART       5
#define RE15_K2080_HALS_SCHRITT    20
#define RE15_K2080_HALS_KLEMME_GIER    560
#define RE15_K2080_HALS_KLEMME_NEIGUNG 312
#define RE15_K2080_HALS_ZIEL_DY    (-800)
/* Nachtrag 26 (Nutzer-Auswahl "Ja, RE2-Kippen"): der Kanister KIPPT beim Greifen sichtbar in die Haende statt in einem Bild quer
 * zu liegen. Form = RE2 ROOM40A0 sub10 (der Kanister kippt von der Wand ins Wasser, selbst gelesen re2_scd_walk.py):
 * Member_set rz = 0 `34 10 00 00` @0x0666, Dreh-Beschleunigung a[5] = -4 `2f 0b fc ff` @0x066E, For 23 { Add_speed, Add_aspeed,
 * Evt_next } `0d 00 06 00 17 00` @0x0676 (Winkel += v, dann v += a: Winkel(k) = -2 k (k - 1), nach 23 Bildern -1012), dann
 * Member_set rz = 0xfc00 = -1024 `34 10 00 fc` @0x0682 (genau 90 Grad). PORT-WAHL der Achse: der Bruch Winkel/1024 fuehrt
 * Drehung (Hochkant -> quer in den Haenden) und Mitte (Marke -> Haende) zugleich. */
#define RE15_K2080_KIPP_BILDER     23
#define RE15_K2080_KIPP_BESCHL     4
#define RE15_K2080_KIPP_ENDE       1024
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
/* HAKEN enemy_ai_boss_gator.c (Nachbesserung 2, N3): GBP_CHASE nach der Aufnahme-Lenksperre - 1 = der Alligator schwimmt in
 * diesem Bild den geworfenen Kanister an (gesteuert + Vortrieb hier, die Verfolgung entfaellt fuer dieses Bild);
 * GBP_GUARD - aktiv = zurueck in die Verfolgung. Nur fuer den Kanister DIESER Szene (Bit 116, in 2090/2091 gesetzt). */
int  re15_k2080_r41_anschwimmen(int slot, re15_actor_t *e, int32_t dist);
int  re15_k2080_r41_anschwimmen_aktiv(const re15_actor_t *e);
/* Pruefhaken (Riegel landung): Wahl der Wurfrichtung fuer einen Alligator bei (gx,gz) (gslot < 0: kein Alligator gesichert). */
int  re15_k2080_wurf_gier_wahl(int gslot, int32_t gx, int32_t gz);
/* Nachtrag 26: Hals-Akku (Gier, Neigung) des Alligators (Part 5) und ein Schritt der RE2-Halsfuehrung; Pruefhaken fuer die Riegel.
 * re15_k2080_hals_schritt: Akku-Ziel (zy, zp) -> Akku slewt um hoechstens RE15_K2080_HALS_SCHRITT (FUN_800177c0). */
void re15_k2080_hals_lesen(int16_t *gier, int16_t *neigung);
void re15_k2080_hals_schritt(int zy, int zp);
int  re15_k2080_hals_ziel(int32_t dx, int32_t dy, int32_t dz, int rot_y, int kf_ay0, int kf_ay5, int kf_az5, int *zy, int *zp);
/* Nachtrag 26: Kipp-Bruch im Bild k (1..) des Greifens, Q12 (0..4096); RE2 ROOM40A0 sub10 @0x066E/@0x0676/@0x0682. */
int  re15_k2080_kipp_bruch(int k);
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
