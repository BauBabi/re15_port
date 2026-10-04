/*
 * RE1.5 Rebuilt — ROOM1120 Cut 1: das hintere Fenster zerbricht, eine Kraehe fliegt herein.
 *
 * Runde 35 Spur M. NUTZER-VORGABE (woertlich, analysis/befunde_runde35/AUFTRAG.md Z. 93):
 *   "Wenn Leon dann in ROOM 1120 Cut1 Richtung dem Fenster hinten zulaeuft, sollen die Scheiben
 *    zerbrechen - so wie in Resident Evil 2 - und eine Kraehe "rein fliegen". Die Glas Zersplitter
 *    Effekt musst du aus Resident Evil 2 extrahieren, sowie der Knall Sound. Es waere super, wenn
 *    du dann auch im Background das Hintere Fenster etwas "beschaedigen" koenntest."
 * Dossier mit allen Belegen: analysis/befunde_runde35/M_cut11c0_fenster.md.
 *
 * RE1.5 hat an dieser Stelle KEIN Ereignis (ROOM1120 main00/sub00/sub01 vollstaendig gelaufen: kein
 * Sce_espr_on, kein Se_on) -> Beta -> Retail: das Vorbild ist RE2 Leon room1090 (Gang mit Fenstern):
 *   sub03 @0x0014 Aot_set Slot 6 sce 5 sat 0x41 -> Gosub sub15 ; @0x0028 Gosub sub09
 *   sub09 @0x0000 `44 00 00 21 02 40 00 0d 00 00 ac f4 3c f6 74 c3 18 0c ...` (Fensterkraehe 0)
 *   sub15 = das Ereignis (Set, Aot_reset, Member_set(0x17,4), Sleep 2, Rumble, 27 Splitter,
 *           Sleep 3, Se_on, Sleep 5, Se_on, ...).
 * Die FORM ist belegt (Opcodes, ESP-Banken/Ops, SE-Satz, Kraehen-State 4); die LAGE im RE1.5-Raum
 * ist PORT-WAHL aus der Rueckprojektion (Dossier §1, §3).
 *
 * ABLAUF:
 *   Raumaufbau (scd_room_setup.c, nach dem Init-Lauf): re15_fenster1120_install — nur ROOM1120,
 *   (9,73)=1 (Irons-Todesszene gesehen, Spur L) und (9,79)=0: Ereignis 24 einmal "Spawn" (Port-
 *   Programm = EIN RE1.5-Sce_em_set der Fensterkraehe) und AOT-Slot 4 als AUTO-Ereignis 24.
 *   Spieler im Band -> AOT-Scan -> scd_event_fire(24) -> Port-Programm Set(9,79,1) + Aot_reset(4).
 *   re15_fenster1120_tick sieht die Kante von (9,79) und faehrt die sub15-Zeitlinie in C:
 *   T+0 Kraehe frei (+0x1D4 = 4), T+2 Rumble + 13 Splitter (RE2-FX-Maschine, Raum-ESP),
 *   T+5 Knall, T+10 Knall. Das beschaedigte Fenster (Hintergrund-Ueberlagerung Cut 1) steht ab T+2
 *   und bei jedem spaeteren Betreten (9,79)=1.
 */
#ifndef RE15_FENSTER1120_H
#define RE15_FENSTER1120_H

#include <stdint.h>

#define RE15_FENSTER_RAUM          0x1120   /* nur Leon: ROOM1121 (Elza) hat keine Irons-Montage   */

/* Zustandsbits (VERTRAG Runde 35 §1.1): (9,79) = "1120-Fenster zerbrochen" (Spur M);
 * (9,73) = "Irons-Todesszene gesehen" (Spur L, nur GELESEN). Bank 9 = 0x800b1078, im Speicherstand. */
#define RE15_FENSTER_BANK          9
#define RE15_FENSTER_BIT           79
#define RE15_FENSTER_TOR_BIT       73

/* Ereignis (VERTRAG §1.3: M = 24 in 1120; < RE15_RDT_MAX_SUB_SCD, ROOM1120 hat nur sub00/sub01). */
#define RE15_FENSTER_EREIGNIS      24

/* AOT-Slot 4 (Zensus: main00 belegt 0..3, Kamerazonen 48..63, kein Port-Installer in 1120).
 * Form = RE2 room1090 sub03 @0x0014 `2c 06 05 41 ...`: AUTO-Ausloeser sat 0x41 (Bit 0x10 frei =
 * AUTO-Pass FUN_80042bac @0x80042ca4, 0x40 = CENTRE-Test). RE1.5-Form des Ereignis-Platzes =
 * sce 3 (Typtabelle @0x8007469c[3] = 0x800430f0, Ereignis in Nutzlast-Byte 3 @0x80043100). */
#define RE15_FENSTER_SLOT          4
#define RE15_FENSTER_SCE           3
#define RE15_FENSTER_SAT           0x41
#define RE15_FENSTER_P0            0x00FF
#define RE15_FENSTER_P1            ((uint16_t)((RE15_FENSTER_EREIGNIS << 8) | 0x18))

/* Band quer ueber den Gang (RE2-Form: 2100 tief, ganze Gangbreite — sub03 @0x0014 w 0x0834 /
 * d 0x157c). PORT-WAHL der Lage: x 3500..6650 = Gang in Cut 1 (SCA-Block (-10500,1650,14000,4750)
 * endet bei x 3500, rechte Wand (6650,...)), z 4300..6400 (Nordkante = Ende des SCA-Blocks). */
#define RE15_FENSTER_ZONE_X0       3500
#define RE15_FENSTER_ZONE_X1       6650
#define RE15_FENSTER_ZONE_Z0       4300
#define RE15_FENSTER_ZONE_Z1       6400

/* Abbildung RE2 -> RE1.5 (PORT-WAHL, Dossier §3): Drehung um 180 Grad um die Hochachse.
 *   X = FENSTER_X - (x2 - RE2_X), Z = WAND_Z - (z2 - RE2_Z), Y = y2, Gier/dir -= 0x800.
 * RE1.5: Fenstermitte x 5000 (Rueckprojektion Cut 1: Scheiben x 4221..5808), Rueckwand z 11200
 * (SCA-Zelle (-2700,11200,11350,2000)). RE2: Fenster 1 Mitte x -2500 (Splitter -2200..-2800),
 * Glasebene z -14600 (aeusserste Splitter sub15 @0x0144 / @0x01D4). */
#define RE15_FENSTER_X             5000
#define RE15_FENSTER_WAND_Z        11200
#define RE15_FENSTER_RE2_X         (-2500)
#define RE15_FENSTER_RE2_Z         (-14600)

/* Die Fensterkraehe = RE2 sub09 @0x0000 Kraehe 0: Lage (-2900,-2500,-15500) dir 0x0c18,
 * +0x10E = rec+4 = 0x4002 (RE2 `lhu v0,4(v1)` @0x8005734c / `sh v0,270(s0)` @0x80057354).
 * Im Port Gegner-Slot 3 (= Aktor 4; main00 spawnt 0..2). */
#define RE15_FENSTER_KRAEHE_EM     3
#define RE15_FENSTER_KRAEHE_F10E   0x4002u
#define RE15_FENSTER_KRAEHE_RE2_X  (-2900)
#define RE15_FENSTER_KRAEHE_RE2_Y  (-2500)
#define RE15_FENSTER_KRAEHE_RE2_Z  (-15500)
#define RE15_FENSTER_KRAEHE_RE2_DIR 0x0c18

/* Zeitlinie = RE2 sub15 (Sleep n -> Folgezeilen n Bilder spaeter, Sleeping 0x80053a24). */
#define RE15_FENSTER_T_SPLITTER    2        /* sub15 @0x002E `09` / `0a 02 00`              */
#define RE15_FENSTER_T_KNALL1      5        /* + @0x0204 `0a 03 00`                          */
#define RE15_FENSTER_T_KNALL2      10       /* + @0x0214 `0a 05 00`                          */
#define RE15_FENSTER_T_ENDE        23       /* + @0x0232 `0a 0d 00` -> Evt_end @0x024A       */

/* Knall = Se_on `36 02 21 01 ...` (sub15 @0x0208 / @0x0218) -> Code 0x02210001: Bank 2 = Raumbank,
 * Satz 0x21 (EDT @0x84 `00 00 7c 60`). Port: shared_assets/RE2/GLAS1090.EDT/.VH/.VB. */
#define RE15_FENSTER_SE_SATZ       0x21

/* Splitter: 13 der 27 sub15-Saetze (Fenster 1, x in [-2800,-2200]) — Rohfelder des RE2-Satzes. */
typedef struct {
    uint16_t sub15_off;     /* Lage des Satzes in sub15 (Datei room1090/scd/sub15.scd)            */
    uint8_t  bank, sub;     /* rec[2], rec[3]                                                     */
    uint16_t skala;         /* u16 rec+6                                                          */
    int16_t  x, y, z;       /* rec+8/+10/+12 (RE2-Welt)                                           */
} re15_fenster_splitter_t;
#define RE15_FENSTER_SPLITTER_N    13
const re15_fenster_splitter_t *re15_fenster1120_splitter(int i);
/* RE2-Gier aller sub15-Splitter (rec+14 = `00 0c`) -> RE1.5 0x0400. */
#define RE15_FENSTER_RE2_GIER      0x0c00

/* Phasen (Pruefhaken). */
#define RE15_FENSTER_AUS           0        /* falscher Raum / Tor zu / schon zerbrochen      */
#define RE15_FENSTER_SCHARF        1        /* Kraehe versteckt, Slot 4 scharf                */
#define RE15_FENSTER_LAEUFT        2        /* Zeitlinie laeuft                               */
#define RE15_FENSTER_FERTIG        3        /* Zeitlinie durch                                */

/* HAKEN scd_room_setup.c (nach dem Init-Lauf, am Ende des Installer-Blocks). */
void re15_fenster1120_install(uint16_t room_id);
/* HAKEN scd_event_fire: Port-Programm fuer ROOM1120 Ereignis 24, sonst NULL. */
const uint8_t *re15_fenster1120_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN game_step_common.c (vor re15_enemy_ai_run_all, hinter dem KI-Freeze-Tor). */
void re15_fenster1120_tick(void);

/* Hintergrund-Ueberlagerung: 1 = das beschaedigte Fenster gehoert jetzt ueber das Bild von `cut`. */
int  re15_fenster1120_schaden_sichtbar(int cut);
/* Plattform-Haken fuer den Knall (PC: re15_audio_re2_glas_se). NULL = stumm. */
extern void (*re15_fenster1120_se_hook)(int satz);

/* Pruefhaken. */
int      re15_fenster1120_phase(void);
int      re15_fenster1120_takt(void);            /* Bilder seit T+0 (-1 vor dem Ausloesen)          */
int      re15_fenster1120_kraehe_slot(void);     /* Aktor-Slot der Fensterkraehe (-1 = keiner)      */
int      re15_fenster1120_splitter_gespawnt(void);
int      re15_fenster1120_knalle(void);
const uint8_t *re15_fenster1120_programm(int welches, int *len);   /* 0 Spawn, 1 Ausloeser */
/* Abbildung einer RE2-Lage (Pruefhaken der Riegel). */
void     re15_fenster1120_abbilden(int16_t x2, int16_t y2, int16_t z2, int32_t out[3]);

/* ---- RE2-Kraehe State 4 (enemy_ai_re2_crow.c, Runde 35 Spur M) ------------------------------ */
void     re15_re2crow_befehl(int slot, uint16_t wert);   /* Member_set(0x17, wert) = +0x1D4         */
uint16_t re15_re2crow_befehl_lesen(int slot);
void     re15_re2crow_zwang(int slot, int an);           /* Skript-Kraehe ueber das RE2-Brain       */
int      re15_re2crow_zwang_ist(int slot);

#endif /* RE15_FENSTER1120_H */
