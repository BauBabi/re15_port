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
 *
 * FORMEN (Belege im Dossier §1):
 *   - Raumwechsel in der Szene = Door_aot_set (0x3B) + Aot_on (0x47) "fire now" (Port-Montage irons_tod_1150.c /
 *     elzazombies_r39.c; Vorbilder ROOM1240 sub02/sub03, ROOM1080 sub07..10). 2080 und 2090 teilen x/z (RVD-Vierecke
 *     ROOM2080 @0x188 Cut 5/6/8 = ROOM2090 @0x220; Tuer 2080 Slot 1 @0x0BC2 (-9650,-26150) -> 2090 (-8200,0,-25550)), der
 *     Kanister landet in 2090 dort, wo seine Bahn in 2080 endet.
 *   - Zombies = Sce_em_set-Saetze (Form ROOM2020 sub00 @0x008F6 `44 00 16 02 01 00 00 bc ..`: Typ 0x16 "Steher" grid 0x02),
 *     Etage 8 (y = -8*1800, @0x800421d4), Tot-Bits Zone 7 (Gate @0x80042128-38).
 *   - Tuer-Ton = RE2-Tuerbank Satz 1 (Door_exit @0x800141f0 `jal 0x8005ba28`, `lui a0,0x1` @0x800141f4) des Tonteils
 *     DOOR06 (Stahltuer mit X-Kreuz; Tuer 2080 Slot 0 <-> 2030, gen/tuer_zuordnung.inc S161 T075; Tabelle @0x8009A568).
 *   - NPC-Schuss = ROOM1021 sub02 (John 0x49): Plc_motion(2,6) @0x020C6, Sleep 110 @0x020CA, Plc_motion(2,7) @0x020CE,
 *     Sleep 30 @0x020D2, Senken Plc_motion(2,6)+Plc_flg 0x80 @0x020EA/@0x020EE, Sleep 20 @0x020F2.
 *   - Wurf = RE2 ROOM7020 sub12/sub13 (Adas Raketenwerfer, Zensus Item_aot_set 0x11): Cut_chg 7 @0x0726, Plc_motion
 *     @0x0750, Message_on @0x0764, Sleep 30 @0x076A, Plc_motion @0x0782, Sleep 20 @0x0786, Super_reset @0x078E, Fall
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
#define RE15_K2080_BIT_ANKUNFT     114   /* P1: Tuer-Ton beim ersten Betreten von 2080/2081 nach Bit 110 gespielt */
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
#define RE15_K2080_SIG_TUERTON     0x1D  /* Programm -> Takt: Tuer-Ton (DOOR06 Satz 1) */
#define RE15_K2080_SIG_ANKUNFT     0x17  /* Plc_dest-Ankunftsflag (+0x1c3 @0x80041c24; Form ROOM1170 `00 05 21`) */
#define RE15_K2080_TUER_SLOT       21    /* AOT-Slot der Raumwechsel (frei in 2080 0..4 und 2090 0..5; Montage nutzt 20) */

/* ---- Zombies (P1) -------------------------------------------------------------------------------------------------- */
#define RE15_K2080_N_ZOMBIES       8
#define RE15_K2080_ZOMBIE_TYP      0x16  /* STAGE2-Zombie (ROOM2020 sub00 @0x008F6, ROOM2000 @0x017CC) */
#define RE15_K2080_ZOMBIE_GRID     0x02  /* Steher (ROOM2020 @0x008F6 pc[3]; ROOM10E1 Else-Zweig @0x00DBE) */
#define RE15_K2080_ETAGE           8     /* oberer Laufgang: Tuer Slot 1 nfloor 8, SCA floor-Byte 0x88 (@0x534..0x5C4) */
#define RE15_K2080_Y               (-14400)  /* = -8 * 1800 (Sce_em_set +0x1ba @0x800421d4-8004220c) */
#define RE15_K2080_TOT0            236   /* Zone-7-Tot-Bits 236..243 (Zensus Dossier 1.6: frei im Original und im Port) */

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
#define RE15_K2080_NPC_STAND_X     (-7245)   /* Aufheben: 500 vor der Markenmitte (Haende Loco c2 Bild 33 bei +149..+-73) */
#define RE15_K2080_NPC_STAND_Z     (-17625)
#define RE15_K2080_LAUF_UEBER      250       /* RUN-Ankunft bei d < 300 (`slti 300` @0x80051764): Ziel 250 hinter dem Stand */
/* Wurf an der Reling: die Reling-Zelle @0x5A0 beginnt bei x -6700; Koerperradius 400 (+0x78-Satz +6) -> Mitte -7100. Die
 * Haende (Eigen c6 Ende +822 vorn) liegen dann jenseits der Relingstange (Bodenlinie x -6350 aus Cut 5, Pfosten (139,151)/
 * (200,151) Rueckprojektion) - die RE2-Bahn faellt frei (Dossier 3.4). Gehen (Sub 4, Ankunft d < 100 @0x800513f8). */
#define RE15_K2080_NPC_WURF_X      (-7100)
/* Clips (Zensus Dossier 1.8, CDEMD0.EMS 0x40/0x49): Eigenbank (Sub 2) c6 Anheben/Zielen 10 B., c7 Feuern 16 B., c0 Tragen
 * 22 B.; Loco (Sub 1) c2 Buecken+Aufheben 50 B. (tiefster Punkt Bild 33). */
#define RE15_K2080_CLIP_ZIELEN     6
#define RE15_K2080_CLIP_FEUER      7
#define RE15_K2080_CLIP_TRAGEN     0
#define RE15_K2080_CLIP_AUFHEBEN   2
#define RE15_K2080_AUFHEBEN_GRIFF  33    /* Loco c2 Bild 33: Wurzel -1005, Hand am Boden (Zensus) */
#define RE15_K2080_AUFHEBEN_BILDER 50
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
extern void (*re15_k2080_tuerton_hook)(void);    /* Tonteil DOOR06 in die Tuerbank + Satz 1 */
extern void (*re15_k2080_platsch_hook)(void);    /* KANISTER2080.VBS Satz 0 (RE2 ROOM40A0 Satz 0x0b) */
extern void (*re15_k2080_schuss_hook)(void);     /* NPC-Schuss = RE15_ELZA_TON_SCHUSS (RE2 ROOM4040 Satz 0x0A) */
/* Zeichner: 1 = der Kanister ist in diesem Bild in 2080/2081 sichtbar; pos = Welt (Fuss-Ursprung des MD1), rot = Q12 3x3. */
int  re15_k2080_kanister_lage(int32_t pos[3], int32_t rot[9]);

/* ---- Pruefhaken ---------------------------------------------------------------------------------------------------- */
int  re15_k2080_phase(void);
int  re15_k2080_zombie_lage(int i, int32_t *x, int32_t *z, int16_t *gier);
int  re15_k2080_landepunkt(int32_t *x, int32_t *z);       /* 1 = Wurfbahn gerechnet (Lage im 2090-Becken) */
void re15_k2080_bahn_rechnen(const int32_t start[3], int16_t gier, int32_t wasser_y, int32_t ende[3], int *bilder);
int  re15_k2080_zaehler(int was);   /* 0 Tuer-Toene, 1 Schuesse, 2 Kopf-Tode, 3 Ausloesungen, 4 Kanister gesetzt (2090) */
uint8_t re15_k2080_npc_typ(void);   /* 0x40 Leon, 0x49 Elza */
const uint8_t *re15_k2080_programm(int nr, int *out_len);   /* 0 Spawn 2080, 1 Szene 2080, 2 Start 2090, 3 Rueckkehr 2090 */
void re15_k2080_reset(void);

#endif /* RE15_KANISTER2080_R41_H */
