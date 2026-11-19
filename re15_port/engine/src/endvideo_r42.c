/*
 * endvideo_r42.c — Runde 42 Strang finale, Stufe 5: Abspann-Vorspann von ROOM5090/5091 ohne "THANK YOU FOR PLAYING", mit Platz fuer
 * das Endvideo. Herleitung, RE2-Belege (ROOM4090 @0x027F4..@0x0281C) und RE1.5-Opcodes: include/re15_endvideo_r42.h.
 */
#include "re15_endvideo_r42.h"
#include "re15_story_r42.h"   /* RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE */

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_scd.h"   /* re15_game_flag_get */

/* Original sub02, 5090 @0x012C2..@0x012D9 = 5091 @0x012A6..@0x012BD (xxd, beide gleich). */
const uint8_t g_re15_ev_sub02_original[RE15_EV_SUB02_LAENGE] = {
    0x22, 0x02, 0x07, 0x01,   0x22, 0x01, 0x1b, 0x01,   0x09, 0x0a, 0x14, 0x00,   0x2b, 0x00, 0x00, 0x00,
    0x09, 0x0a, 0x64, 0x00,   0x47, 0x05,               0x01, 0x00 };

#define LO(v) ((uint8_t)((unsigned)(v) & 0xFFu))
#define HI(v) ((uint8_t)(((unsigned)(v) >> 8) & 0xFFu))

const uint8_t g_re15_ev_programm[] = {
    /* Original sub02 bis vor die Meldung */
    0x22, 0x02, 0x07, 0x01,                               /* Set(2,7,1)  Pad-Sperre   5090 @0x012C2 / 5091 @0x012A6 */
    0x22, 0x01, 0x1b, 0x01,                               /* Set(1,27,1) Kinobalken   @0x012C6 / @0x012AA          */
    0x54, 0x00, 0x04, 0x00, 0x00, 0x00,                   /* MAIN anhalten: RE2 @0x02804 Fall 4; Form 5090 @0x012B4.
                                                           * PORT-WAHL Lage: schon hier statt nach Sleep 20 + Sleep 1
                                                           * (Kopf re15_endvideo_r42.h "MUSIK")                      */
    0x09, 0x0a, 0x14, 0x00,                               /* Sleep 20                 @0x012CA / @0x012AE          */
    /* Message_on 0 "THANK YOU FOR PLAYING !!!" (@0x012CE / @0x012B2) und Sleep 100 (@0x012D2 / @0x012B6) entfallen
     * (NUTZERWUNSCH 18.11.2026). An ihrer Stelle die RE2-Form ROOM4090 (Kopf re15_endvideo_r42.h): */
    0x56, RE15_EV_BLENDE_KANAL, RE15_EV_BLENDE_ABR, RE15_EV_BLENDE_RGB,
          LO(RE15_EV_ABBLENDEN), HI(RE15_EV_ABBLENDEN),  /* Abblenden: RE2 @0x027F4 `53 02 02 07 00 02`            */
    0x57, RE15_EV_BLENDE_KANAL, 0x00, 0x00,               /* Start, Pegel 0 (Schritt > 0, @0x80021710-20)          */
    0x09, 0x0a, RE15_EV_SCHLAF_1, 0x00,                   /* Sleep 1: RE2 @0x02800 (MAIN ist schon angehalten)     */
    0x09, 0x0a, RE15_EV_SCHLAF_2, 0x00,                   /* Sleep 62: RE2 @0x0280A                                */
    0x02,                                                 /* Evt_next: RE2 @0x02810                                */
    0x56, RE15_EV_BLENDE_KANAL, RE15_EV_BLENDE_ABR, RE15_EV_BLENDE_RGB,
          LO(RE15_EV_EINBLENDEN), HI(RE15_EV_EINBLENDEN),/* Schwarz halten: RE2 @0x02812 `53 02 02 07 00 fe`      */
    0x57, RE15_EV_BLENDE_KANAL, 0x00, 0x00,               /* Start, Pegel 0x7FFF (Schritt <= 0)                    */
    0x2b, RE15_EV_MSG_FILM, 0x00, 0x00,                   /* Film (Port-Zeile): RE2 Movie_on @0x02818 `6f 0b`      */
    0x02,                                                 /* Evt_next: RE2 @0x0281A                                */
    0x57, RE15_EV_BLENDE_KANAL, 0x00, 0x00,               /* Pegel 0x7FFF: RE2 Sce_fade_adjust @0x0281C `74 02 ff 7f`*/
    /* Original sub02 ab Aot_on */
    0x47, 0x05,                                           /* Aot_on(5) -> ROOM6040/6041   @0x012D6 / @0x012BA       */
    0x01, 0x00,                                           /* Evt_end                       @0x012D8 / @0x012BC       */
};
const unsigned g_re15_ev_programm_laenge = (unsigned)sizeof g_re15_ev_programm;

unsigned g_re15_ev_starts = 0;

const uint8_t *re15_endvideo_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (sub != RE15_EV_SUB || !pc) return pc;
    if (room_id != RE15_EV_RAUM_LEON && room_id != RE15_EV_RAUM_ELZA) return pc;
    if (!re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE)) return pc;
    if (memcmp(pc, g_re15_ev_sub02_original, RE15_EV_SUB02_LAENGE) != 0) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[endvideo] ROOM%04X sub02 weicht vom Original ab -> Original-sub02 laeuft\n", (unsigned)room_id);
#endif
        return pc;                                        /* anderer Stand: lieber Original */
    }
    g_re15_ev_starts++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[endvideo] ROOM%04X sub02 -> Port-Programm ohne THANK YOU: Abblenden (RE2 ROOM4090 @0x027F4), Film-Zeile %d, "
                    "Aot_on(5)\n", (unsigned)room_id, RE15_EV_MSG_FILM);
#endif
    return g_re15_ev_programm;
}
