/*
 * RE1.5 Rebuilt — Irons' Tod in ROOM1150, die Knall-Montage (1130 / 1040 / 1030) und der Schluss-
 * Schnitt ROOM11C0 Cut 13 (Ada/Marvin). Runde 35, Spur L, Punkte 2-4.
 *
 * NUTZER-VORGABE (woertlich, analysis/befunde_runde35/AUFTRAG.md Z. 69-91): "Betritt man ROOM 1150
 * kommt eine weitere Cutscene: Leon: Sir! (Arm Streck Animation) / Leon rennt zu Irons zur Liege /
 * Leon: Sir, the communication system can't be fixed! ... / Irons: Leon... I... I'm proud ... /
 * Irons: But... I... I'm not going to make it... I'm sorry... / Irons: Please... one last favor...
 * Be a hero... Leon... (Arm ausgestreckt wie in der 1. Cutscene, dann abrupt fallen lassen, tot) /
 * Leon: SIR, Sir?! / Leon bleibt kniend - schuettelt mit leicht geneigten Kopf den Kopf langsam und
 * steht dann langsam wieder auf / Kurze Pause / Dann gibt es einen Knall - ... ROOM 1140 ... Anzahl an
 * Zombies die noch leben in ROOM 1130 spawnen bei Cut0 ... weiterer lauter Knall - so wie bei der
 * Cutscene von ROOM 1030 - ... ROOM 1040 ... Rolltor hoch ... 5 Zombies ... Cut1 ... Room 1070 ...
 * Zombies in ROOM 1030 spawnen in Cut7 ... noch ein Knall und die Zombies kriechen noch einmal durch
 * das Tor in Cut6 ... Wechsel zu Room 11C0 Parking Lot Cut 13 ... Ada: What was this noise? ...
 * Marvin: Yes!.... / Marvin: Oh, no, Leon! / Marvin dreht sich zu Ada / Marvin: I have to help him,
 * sorry! (Arm streck) / Marvin rennt aus dem CUT raus Richtung Gebaeude / Ada: Marvin!... (Arm streck)
 * / Ende der Cutscene." Dossier mit allen Belegen: analysis/befunde_runde35/L_cut1150.md.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. RE1.5 hat keine zweite Irons-Szene; die Choreografie ist die des
 * Nutzers, ihre FORM ist belegt: jede Zeile der Programme traegt ihr Vorbild im Auslieferungsstand
 * (irons_tod_1150.c), ausgefuehrt von der vorhandenen VM. Kein Asset-Patch.
 *
 * MECHANISMUS DER MONTAGE (Raumwechsel innerhalb einer Szene): das Original kennt dafuer genau
 * einen Weg — Door_aot_set (0x3B) + Aot_on (0x47, "fire now", LAB_800407bc: jalr Handler
 * @0x8004082c -> sce-2-Handler @0x800430bc -> Warp FUN_8001d600). So wechselt ROOM1240 sub02/sub03
 * (Slot 0) ins Intro ROOM1170 und ROOM1080 sub07..10 die Fahrstuhl-Etagen. Der Spieler wird dabei
 * ausserhalb des Bildes "geparkt" (Spawn des Tuersatzes), jeder Zielraum traegt sein eigenes
 * Programm, das der Installer nach dem Init-Lauf von main00 startet (scd_event_fire(21)). Waehrend
 * eines Montage-Schritts unterbleibt der Reseed des raumeigenen sub01 (FUN_8003f038 @0x8003f064-84),
 * weil ROOM11C0 sub01 @0x01824 `04 0a 18 02` sonst die spaetere Ada-Szene verbrauchen wuerde.
 * Kette: 1150 (Szene) -> [1130 Cut 0, nur wenn in 1140 noch Zombies leben] -> 1040 Cut 1 -> 1030
 * (Cut 7 nur wenn in 1070 noch Zombies leben; Cut 6 immer) -> 11C0 Cut 13 -> 1150 (Rueckkehr).
 */
#ifndef RE15_IRONS_TOD_H
#define RE15_IRONS_TOD_H

#include <stdint.h>

/* Raeume der Kette (Leon-Variante; Elza hat weder (9,71) noch (3,94), s. re15_tuer1120.h). */
#define RE15_IT_RAUM_1150   0x1150
#define RE15_IT_RAUM_1130   0x1130
#define RE15_IT_RAUM_1040   0x1040
#define RE15_IT_RAUM_1030   0x1030
#define RE15_IT_RAUM_11C0   0x11C0

/* Freigabe: (9,71) = 10F0-Szene gesehen (Spur K, VERTRAG §1.1) UND (3,94) = erste Irons-Szene
 * (ROOM1150 sub08 @0x01110 `22 03 5e 01`). Einmal-Riegel (9,73) = Todesszene gesehen; erstes Opcode
 * des Programms wie ROOM11B0 sub06 @0x01478 `22 03 83 01`. Bits 74/75/76 = Spawns 1130/1040/1030
 * verbraucht (VERTRAG §1.1 Spur L). Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9]), liegt in
 * g_game.flags und wird mit dem Spielstand gespeichert. */
#define RE15_IT_K_BANK      9
#define RE15_IT_K_BIT       71
#define RE15_IT_IRONS_BANK  3
#define RE15_IT_IRONS_BIT   94
#define RE15_IT_BANK        9
#define RE15_IT_BIT_GESEHEN 73
#define RE15_IT_BIT_1130    74
#define RE15_IT_BIT_1040    75
#define RE15_IT_BIT_1030    76

/* Ereignis-Nummer der Port-Programme (VERTRAG §1.3: L = 21; in jedem Raum der Kette dieselbe, die
 * Raeume 1130/1040/1030 haben keine andere Spur). < RE15_RDT_MAX_SUB_SCD, kein Sub eines dieser
 * Raeume (1150: 9 Subs, 1130: 3, 1040: 11, 1030: 12, 11C0: 8). */
#define RE15_IT_EREIGNIS    21

/* Tuer-Slot der Montage-Schnitte in jedem Raum: frei in allen fuenf Raeumen (1150 nutzt 0..6 +
 * 11/12 Hebetisch; 1130 0..8; 1040 0..7; 1030 0..17; 11C0 0; Kamerazonen 48..63). */
#define RE15_IT_TUER_SLOT   20

/* Signal-Bit in Bank 5, WORT 0 (Bits 0..31 = Raum-Scratch, geloescht nur beim Raum-Init
 * FUN_8003ecec @0x8003ed74): das Programm setzt es mit dem Original-Opcode Set, der Port-Takt
 * reagiert und loescht es. 12 = Irons' Arm faellt (Clip 2 ab Bild 72, s.u.). ROOM1150 selbst
 * nutzt in Wort 0 nur Bit 0 (sub04 @0x00F96). ⛔ NICHT Wort 1 (Bits 32..63): das ist das
 * Ein-Bild-Handshake-Wort (Plc_dest-Ankunft 0x20 u.a.), das FUN_8003ebf4 nach JEDEM VM-Takt
 * wischt (scd_vm.c `g_game.flags[5][1] = 0`) — gemessen: ein Signal auf Bit 44 kam nie an. */
#define RE15_IT_SIG_ARM     12

/* Irons = Sce_em_set main00 @0x00E88 Slot 0 (Aktor-Slot 1), Typ 0x45, Pos (-21150,-720,-26131).
 * Seine Szenen-Clips liegen im Raum-RBJ (RDT+0x5C @0x1774) Record 1 (Marker 2 = Gegner 0):
 * [36,60,90,35,30,102,34]. Erste Szene nutzt 3 (Liege-Loop), 4, 5 (Arm ausgestreckt, 102 Bilder,
 * Halt bei msg 12/13 @0x0125A/@0x0125E), 6 (sub03). Clips 0/1/2 ruft kein Sub: 0 = Hand an der
 * Stirn, 1 = Hand an der Stirn -> Arm faellt herab, 2 = Arm haengend -> hebt sich -> ausgestreckt
 * (Bild 60-72) -> faellt abrupt (72 -> 84) und bleibt haengen (89). Der abrupte Fall = Clip 2 ab
 * Bild 72 (gerendert: tools/.. irons_streifen, Dossier §2.2); das Original kann einen Clip nur ab
 * Bild 0 starten (Plc_motion @0x80041b90 -> Phase 0 `sb zero,149` @0x80050d0c), darum setzt der
 * Port-Takt auf das Signal (5,44) hin Bild 72 direkt (Phase 1 des Motion-Subs 0x80050cb8, Halt
 * bei Clip-Ende @0x80050da4). Die Totenpose = Bild 89 gehalten (Phase 2). */
#define RE15_IT_IRONS_SLOT      1
#define RE15_IT_IRONS_TYP       0x45
#define RE15_IT_IRONS_CLIP_TOT  2
#define RE15_IT_IRONS_BILD_FALL 72
#define RE15_IT_IRONS_BILD_TOT  89

/* Parkplaetze des Spielers je Zielraum (Spawn der Montage-Tuersaetze) — PORT-WAHL, gemessen am
 * Bild (Dossier §4): jeweils HINTER der Kamera des Montage-Cuts auf einem Tuer-Rechteck (= begehbar):
 *   1130 Cut 0 (Kamera (-2386,-3801,-7451) -> (876,303,-18973)): Tuer-Rechteck Slot 1 @0x008AE
 *        (-3550,-3150,1000,2000) -> (-3050,-2150), 5300 hinter der Kamera.
 *   1040 Cut 1 (Kamera (-26262,-3114,-10692) -> (-25020,-1746,-1818)): Schalter-Rechteck Slot 4
 *        @0x01118 (-26000,-16000,2100,2200) -> (-24950,-14900); gemessen unsichtbar, die Zombies
 *        laufen durchs offene Tor zur Kamera (Lauf m7, Dossier §4).
 *   1030 Cut 7 (Kamera (-26820,-3114,-2574) -> (-15858,-936,-5040)) und Cut 6 (Kamera
 *        (-11790,-3870,-15282) -> (-8640,-684,-26478)): Tuer-Rechteck Slot 2 @0x01CAA
 *        (-27500,-4700,1200,2100) -> (-27300,-3650): hinter beiden Kameras.
 *   11C0 Cut 13 (Kamera (-11300,-3788,-8584) -> (-1688,-832,-21500)): Tuer-Rechteck Slot 0 @0x01712
 *        (-27100,15900,4000,2700) -> (-25100,17250), Gierung 3067 = die der Tuer.
 *   1150 Rueckkehr: Couchplatz der ersten Szene (-20538,-25147) (Plc_dest @0x01136), Gierung 1500
 *        (Member_set @0x0115E), Cut 7 (@0x01178). */
#define RE15_IT_PARK_1130_X   (-3050)
#define RE15_IT_PARK_1130_Z   (-2150)
#define RE15_IT_PARK_1040_X   (-24950)
#define RE15_IT_PARK_1040_Z   (-14900)
#define RE15_IT_PARK_1030_X   (-27300)
#define RE15_IT_PARK_1030_Z   (-3650)
#define RE15_IT_PARK_11C0_X   (-25100)
#define RE15_IT_PARK_11C0_Z   (17250)
#define RE15_IT_PARK_11C0_YAW 3067
#define RE15_IT_COUCH_X       (-20538)
#define RE15_IT_COUCH_Z       (-25147)
#define RE15_IT_COUCH_YAW     1500
#define RE15_IT_CUT_1130      0
#define RE15_IT_CUT_1040      1
#define RE15_IT_CUT_1030_TUER 7
#define RE15_IT_CUT_1030_TOR  6
#define RE15_IT_CUT_11C0      13
#define RE15_IT_CUT_1150      7

/* Zombie-Records, die die Montage umzieht. Tot-Bits (Sce_em_set pc[7], Zone 7 = re15_em_status_zone
 * fuer Stage 1-3, Gate @0x80042128-38): ROOM1140 sub00 @0x00BAA.. 0xd3..0xd7 (Typen 0x16,0x10,0x10,
 * 0x11,0x11), ROOM1070 sub00 @0x015CA.. 0xc6..0xca (0x10,0x10,0x10,0x11,0x11). Neue Bits aus dem
 * freien Bereich (Zensus aller Sce_em_set in STAGE1-3, Dossier §2.3): 1130-Kopien 40,41,42,43,46;
 * 1030-Kopien 47,48,49,75,76; Kriecher 77,78,79. */
#define RE15_IT_N_1140        5
#define RE15_IT_N_1070        5
#define RE15_IT_N_KRIECHER    3
#define RE15_IT_BIT_1140_0    0xd3
#define RE15_IT_BIT_1070_0    0xc6

/* Zustand der Kette (nur RAM; waehrend der Szene kann weder gespeichert noch gestorben werden). */
#define RE15_IT_AUS        0
#define RE15_IT_SZENE      1   /* 1150: Szene laeuft, naechster Raum 1130 oder 1040 */
#define RE15_IT_S1130      2   /* 1130-Programm laeuft, naechster Raum 1040 */
#define RE15_IT_S1040      3   /* 1040-Programm laeuft, naechster Raum 1030 */
#define RE15_IT_S1030      4   /* 1030-Programm laeuft, naechster Raum 11C0 */
#define RE15_IT_S11C0      5   /* 11C0-Programm laeuft, naechster Raum 1150 */
#define RE15_IT_RUECKKEHR  6   /* 1150: Rueckkehr-Programm laeuft */

/* HAKEN scd_room_setup.c (nach dem Init-Lauf von main00) + main.c (Boot-/CONTINUE-Weg): startet je
 * nach Raum und Zustand die Szene, den Montage-Schritt, die Rueckkehr, die Nachspawns (1130/1030 bei
 * spaeterem Betreten) oder setzt Irons' Totenpose. Tut in anderen Raeumen nichts. */
void re15_irons_tod_install(uint16_t room_id);

/* HAKEN scd_event_fire: Port-Programm fuer (Raum, Ereignis 21, Zustand) oder NULL. */
const uint8_t *re15_irons_tod_ereignis(uint16_t room_id, uint8_t event_id);

/* HAKEN scd_vm_tick: 1 = der Reseed von sub01 unterbleibt (Montage-Schritt in einem fremden Raum). */
int re15_irons_tod_sub01_gesperrt(void);

/* HAKEN scd_vm_tick (jedes Bild): Signal (5,12) -> Irons' Arm faellt. */
void re15_irons_tod_tick(void);

/* Pruefhaken (kein Spielverhalten). */
int            re15_irons_tod_zustand(void);
void           re15_irons_tod_zustand_setzen(int z);      /* Riegel: Schritt vorgeben */
const uint8_t *re15_irons_tod_programm(int welches, int *out_len);   /* Vorlagen 0..6 (s. .c) */
const uint8_t *re15_irons_tod_laufprogramm(int *out_len);            /* RAM-Kopie der VM */
const uint8_t *re15_irons_tod_meldung(uint16_t room_id, int msg_id, int *out_len);
int            re15_irons_tod_lebend(int welche /* 0 = 1140, 1 = 1070 */);   /* Zaehlung ueber die Tot-Bits */

#endif /* RE15_IRONS_TOD_H */
