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

/* Stufe 7: Abspann-Diashow ROOM6040/6041 sub02, Original @0x0052A..@0x00597 (110 Bytes, beide Raeume gleich, xxd). */
const uint8_t g_re15_ev_dia_original[RE15_EV_DIA_LAENGE] = {
    0x54, 0x00, 0x01, 0x00, 0x00, 0x00,   0x22, 0x02, 0x07, 0x01,   0x29, 0x00,   0x09, 0x0a, 0xc8, 0x00,   /* @0x0052A..@0x00539 */
    0x29, 0x01,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                 /* @0x0053A..@0x00547 */
    0x29, 0x02,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                                           /* @0x00548 */
    0x29, 0x03,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                                           /* @0x00552 */
    0x29, 0x04,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                                           /* @0x0055C */
    0x29, 0x05,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                                           /* @0x00566 */
    0x29, 0x06,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,                                           /* @0x00570 */
    0x29, 0x07,   0x09, 0x0a, 0xfa, 0x00,                                                                     /* @0x0057A */
    0x29, 0x08,   0x09, 0x0a, 0xfa, 0x00,                                                                     /* @0x00580 */
    0x54, 0x00, 0x02, 0x00, 0x00, 0x00,   0x09, 0x0a, 0x02, 0x00,   0x22, 0x00, 0x01, 0x00,   0x01, 0x00 };  /* @0x00586..@0x00597 */

/* Die Diashow ohne Tafel 1: das Original-sub02 ohne die 14 Bytes @0x0053A..@0x00547 (Cut_chg 1 + 3 x Sleep 250). Alle uebrigen
 * Befehle in Original-Reihenfolge und mit Original-Werten (NUTZERWUNSCH 18.11.2026, Kopf re15_endvideo_r42.h "STUFE 7"). */
const uint8_t g_re15_ev_dia_programm[RE15_EV_DIA_LAENGE - RE15_EV_DIA_TAFEL1_LAENGE] = {
    0x54, 0x00, 0x01, 0x00, 0x00, 0x00,   /* MAIN an        @0x0052A                                   */
    0x22, 0x02, 0x07, 0x01,               /* Set(2,7,1)     @0x00530                                   */
    0x29, 0x00,   0x09, 0x0a, 0xc8, 0x00, /* Cut 0 (schwarz), Sleep 200   @0x00534 / @0x00536          */
    /* [Cut_chg 1 @0x0053A + Sleep 250 @0x0053C/@0x00540/@0x00544 entfallen: Tafel "Thank you for playing this reconstruction"] */
    0x29, 0x02,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   /* Cut 2, 500 Bilder   @0x00548..@0x00551 */
    0x29, 0x03,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   /* Cut 3, 500 Bilder   @0x00552..@0x0055B */
    0x29, 0x04,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   /* Cut 4, 500 Bilder   @0x0055C..@0x00565 */
    0x29, 0x05,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   /* Cut 5, 500 Bilder   @0x00566..@0x0056F */
    0x29, 0x06,   0x09, 0x0a, 0xfa, 0x00,   0x09, 0x0a, 0xfa, 0x00,   /* Cut 6, 500 Bilder   @0x00570..@0x00579 */
    0x29, 0x07,   0x09, 0x0a, 0xfa, 0x00,                             /* Cut 7, 250 Bilder   @0x0057A..@0x0057F */
    0x29, 0x08,   0x09, 0x0a, 0xfa, 0x00,                             /* Cut 8, 250 Bilder   @0x00580..@0x00585 */
    0x54, 0x00, 0x02, 0x00, 0x00, 0x00,   /* MAIN aus       @0x00586                                   */
    0x09, 0x0a, 0x02, 0x00,               /* Sleep 2        @0x0058C                                   */
    0x22, 0x00, 0x01, 0x00,               /* Set(0,1,0) = INGAME aus -> Titel   @0x00590                */
    0x01, 0x00 };                         /* Evt_end        @0x00594                                   */

unsigned g_re15_ev_dia_starts = 0;

const uint8_t *re15_endvideo_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (sub != RE15_EV_SUB || !pc) return pc;
    /* Stufe 7: Abspann-Diashow ohne die Tafel "Thank you for playing this reconstruction" (sub00 @0x00522 `04 ff 18 02`). Ohne
     * Bedingung an Bit 129: die Tafel soll nie erscheinen (der Raum ist ohnehin nur nach dem Finale erreichbar, aot_common.c). */
    if (room_id == RE15_EV_DIA_LEON || room_id == RE15_EV_DIA_ELZA) {
        if (memcmp(pc, g_re15_ev_dia_original, RE15_EV_DIA_LAENGE) != 0) {
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[abspann] ROOM%04X sub02 weicht vom Original ab -> Original-sub02 laeuft\n", (unsigned)room_id);
#endif
            return pc;
        }
        g_re15_ev_dia_starts++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[abspann] ROOM%04X sub02 -> Diashow ohne Tafel 1 (Cut_chg 1 @0x0053A + 3 x Sleep 250 entfallen), "
                        "Cut 0,2..8 wie im Original\n", (unsigned)room_id);
#endif
        return g_re15_ev_dia_programm;
    }
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
