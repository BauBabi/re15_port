/*
 * RE1.5 Rebuilt — Runde 42 Strang "original", Stufe 1: Leichenhalle ROOM1200/1201, die Kuehlfach-Tuer klappert und der
 * Zombie kriecht heraus — wie auf der 40%-Disc.
 *
 * NUTZERWUNSCH 18.11.2026 (woertlich): "Wo du gerade in der Vorschau in dem Raum bist. Im Original unmodifzierten 40% build
 * klappert dort noch die eine Tür in der Leichenhalle und der Zombie kriecht dann raus. Das ist in der Mod Version aus
 * irgendeinen Grund nicht mehr so.... Das soll wieder rein!"
 *
 * DISC (Version_40%/PSX/STAGE1/ROOM1200.RDT md5 b67f1da9..., ROOM1201.RDT md5 50cb9338...; Dump re15_port/tools/
 * scd_dump_room.py; beide Fassungen gleich bis auf eine Dir_set-Zeile in 1201):
 *   main00 @0x007CA Ifel_ck / @0x007CE Ck(4,17)==0  `21 04 11 00`
 *          @0x007D2 Aot_set `2c 01 05 41 00 00 22 98 e4 bc 82 05 7e 04 00 00 00 00 00 00`
 *                   = Platz 1, sce 5 (Markierung, Handler @0x8004318C = NOP), Flags 0x41 (Mitte 0x40 + Spieler-Pool 0x01,
 *                   Bit 0x10 frei = automatisch), Etage 0, Rechteck x -26590 z -17180 b 1410 t 1150
 *   sub01  @0x008FC `06 00 0c 00` Ifel_ck / @0x00900 `23 00 01 00 01 00` Cmp(work_vars[1] == 1)
 *          @0x00906 `04 ff 18 02` Evt_exec(0xFF, sub02)
 *   sub02  @0x0090E `46 01 00 41 00 00 00 00 00 00` Aot_reset Platz 1 -> sce 0 (Markierung aus)
 *          @0x00918 .. @0x009CB  Work_set(3,1) + Klappern (Pos_set obj 1, z -14402/-14362/-14404/-14360 um die Ruhelage
 *                                -14382, 4+2+3 Takte, Sleep 3/4/7/9/5/2/3/2) + Sleep 5 + erster Tuerschwung
 *                                (Speed_set(4,2), Speed_set(0x0a,2), 15x Add_speed+Add_aspeed, 18x Add_speed)
 *          @0x009CC `2e 02 02 00` Work_set(2,2) / @0x009D0 `34 0c 81 00` Member_set(0x0C = grid, 0x81) = Gegner 2 wach
 *          @0x009D4 .. @0x00A1D  Rest des Tuerschwungs
 *          @0x00A1E `22 04 11 01` Set(4,17,1) / @0x00A22 `01 00`
 *   ROOM1201 sub02: dieselben Zeilen, dazu @0x0091C `33 00 00 00 00 04 00 00` Dir_set(0,1024,0) hinter dem Work_set(3,1).
 *   Gegner 2 (beide Fassungen) @0x008AE `44 02 10 a1 01 01 00 ff d6 9f f8 f8 02 cd 00 00 e8 03 00 00`: Typ 0x10, grid 0xA1
 *   (Bit 0x20 = Zombie-Wurzel ueberspringt den Takt, `andi 0x20` @0x80100450-5c), Etage 1, (-24618,-1800,-13054), Gier 1000.
 *
 * AUSLOESUNG AUF DER DISC (PSX.EXE der Disc, gleiche Bytes wie der Mod-Exe im SCD-/AOT-Bereich 0x8003f000..0x80043300):
 *   Rechteck-Test FUN_80042b64: `subu v1,v1,v0` / `sltu a0,a0,v1` @0x80042b74-78 (x) und @0x80042b90-94 (z) =
 *     (unsigned)(px - x) <= b  und  (unsigned)(pz - z) <= t
 *   Mitte-Treffer -> work_vars[1] = Platz: `addiu v0,s2,-1` @0x80042edc / `sh v0,4050(at)` @0x80042ee4 (0x800B0FD2)
 *   Cmp @0x8003ff68: Index = pc[2] (`andi v0,v1,0xff` @0x8003ff80, Basis 0x800B0FD0 @0x8003ff94), Operator pc[3] = 0 ->
 *     Tabelle @0x80010c3c[0] = 0x8003ffc8 `xor` / `sltiu v0,v0,0x1` = gleich
 *   Evt_exec @0x8003f2a0 -> FUN_8003ee3c: Faden 0xFF = erster freier Ereignis-Faden
 *   VM-Ende wischt work_vars[0..3] JEDES Bild auf -1: `addiu v0,zero,-1` @0x8003ebf4, `sh` @0x8003ebfc/ec04/ec0c/ec14
 *
 * MOD (re15_port/shared_assets/PSX/STAGE1/ROOM1200.RDT = ROOM1201.RDT, md5 cf20cd5d..., beide Dateien gleich):
 *   keine Markierung in main00, sub01 prueft nur Bits; sub02 @0x009A0 (82 B) ist Byte fuer Byte der Disc-Schwanz
 *   @0x009CC..@0x00A1D (ab Member_set 0x81, OHNE Klappern, OHNE ersten Tuerschwung, OHNE Set(4,17)); gerufen wird es nur
 *   von sub03 @0x009F4 nach dem Aufnehmen des Minidisc-Players: @0x00A08 Ck(9,104)==1, @0x00A0C `04 ff 18 02`
 *   Evt_exec(sub02), danach Musik, Wecken der Liegenden (Gegner 0/1/3) und @0x00A36 Set(4,17,1).
 *
 * PORT (Stufe 1):
 *   - Ausloesung (PORT-WAHL der Form): KEIN eigener AOT-Platz — Platz 1 traegt in der Mod den Gegenstand (sub00 @0x00908),
 *     und der Port wischt work_vars nicht je Bild (aot_common.c Glied 1 a). Statt dessen pruefen wir je Spielbild genau den
 *     Disc-Test: Spieler-Mitte im Rechteck nach FUN_80042b64, Etage 0 (Satz @0x007D2 rec[2] = 0, Pool 0x01), nur solange
 *     Ck(4,17)==0 (Gate @0x007CE).
 *   - Ablauf = Disc-sub02 ab @0x00918 Byte fuer Byte (1200: 268 B bis @0x00A23, 1201: 276 B bis @0x00A2B), im ersten freien
 *     Ereignis-Faden (wie Evt_exec 0xFF). Das Aot_reset @0x0090E (Platz 1 -> sce 0) faellt weg (PORT-WAHL: es wuerde in der
 *     Mod den Gegenstand auf Platz 1 loeschen); seine Wirkung "nur einmal" uebernimmt der Merker s_gestartet.
 *   - Mod-sub03 @0x00A0C Evt_exec(sub02): ausgelassen, wenn das Kuehlfach schon offen ist (Disc-Ablauf in diesem Besuch
 *     gestartet ODER Ck(4,17)==1) — sonst schwaenge die Tuer ein zweites Mal und der Kriecher bekaeme 0x81 erneut. Der Rest
 *     von sub03 (Minidisc, Musik, Wecken, Set(4,17)) bleibt unberuehrt. PORT-WAHL.
 */
#ifndef RE15_LEICHENHALLE_R42_H
#define RE15_LEICHENHALLE_R42_H

#include <stdint.h>
#include "re15_rdt.h"

#define RE15_R42L_RAUM_LEON        0x1200u
#define RE15_R42L_RAUM_ELZA        0x1201u

/* Gate und Bit: Disc main00 @0x007CE `21 04 11 00` Ck(4,17)==0; Setzer Disc sub02 @0x00A1E `22 04 11 01`. */
#define RE15_R42L_BANK             4
#define RE15_R42L_BIT              17

/* Disc main00 @0x007D2 Aot_set-Satz: rec[2] Etage 0, Rechteck (x, z, b, t). */
#define RE15_R42L_ETAGE            0
#define RE15_R42L_X                (-26590)   /* @0x007D8 `22 98` */
#define RE15_R42L_Z                (-17180)   /* @0x007DA `e4 bc` */
#define RE15_R42L_B                1410       /* @0x007DC `82 05` */
#define RE15_R42L_T                1150       /* @0x007DE `7e 04` */

/* Mod-Anker (shared_assets ROOM1200.RDT = ROOM1201.RDT): sub02 @0x009A0 82 B == Disc @0x009CC..@0x00A1D,
 * sub03 @0x009F4 84 B, Evt_exec in sub03 @0x00A0C `04 ff 18 02`. */
#define RE15_R42L_MOD_SUB02_OFF    0x009A0u
#define RE15_R42L_MOD_SUB02_LEN    82u
#define RE15_R42L_MOD_SUB03_OFF    0x009F4u
#define RE15_R42L_MOD_SUB03_LEN    84u
#define RE15_R42L_MOD_EVT_OFF      0x00A0Cu

/* Raumaufbau (scd_room_setup.c, nach dem Init-Lauf): Zustand loeschen, Anker pruefen, scharf schalten. */
void re15_leichenhalle_r42_install(uint16_t room_id);
/* Je Spielbild (scd_vm_tick, nicht im Raumaufbau): Rechteck-Test, Ablauf starten. */
void re15_leichenhalle_r42_tick(void);
/* op_evt_exec: 1 = diesen Evt_exec (Mod-sub03 @0x00A0C) auslassen. */
int  re15_leichenhalle_r42_evt_exec_gestrichen(uint16_t room_id, const uint8_t *pc, const re15_rdt_t *rdt);

/* Pruefhaken */
const uint8_t *re15_leichenhalle_r42_programm(uint16_t room_id, int *out_len);
int            re15_leichenhalle_r42_scharf(void);      /* 1 = wartet auf den Spieler im Rechteck */
int            re15_leichenhalle_r42_gestartet(void);   /* 1 = Ablauf in diesem Besuch gestartet */
unsigned       re15_leichenhalle_r42_starts(void);
unsigned       re15_leichenhalle_r42_ausgelassen(void);
int            re15_leichenhalle_r42_im_rechteck(int32_t px, int32_t pz);

#endif /* RE15_LEICHENHALLE_R42_H */
