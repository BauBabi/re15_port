/*
 * RE1.5 Rebuilt — Runde 40 "nahaufnahmen": nie gezeigte STAGE1-Nahaufnahmen beim Untersuchen zeigen.
 *
 * Herleitung, Zensus und alle Vorbilder: include/re15_nahaufnahmen_r40.h. Kein Asset-Patch: die RDTs bleiben byte-true.
 */
#include "re15_nahaufnahmen_r40.h"

#include "re15_aot.h"        /* g_aot, re15_aot_retype */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ---- 11D0/11D1: Zwinger-Pult (Cut 12) ------------------------------------------------------------------------- */
static const uint8_t k_pult[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_N40_PULT_CUT,             /* Cut_chg 12 Pult       Form ROOM1051 sub03 @0x00DB4                  */
    0x2b, RE15_N40_PULT_MSG, 0xff, 0xff, /* Message_on 0          = Platz 2 ROOM11D0 main00 @0x01248 (msg 0)    */
    0x02, 0x00,                          /* Evt_next              = ROOM1051 sub03 @0x00DBA                     */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1            = ROOM11D0 sub02 @0x01886                     */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)            = ROOM11D0 sub02 @0x01888                     */
    0x01, 0x00,                          /* Evt_end                                                             */
};

/* ---- 1110/1111: Waffenspinde (Cut 11) ------------------------------------------------------------------------- */
static const uint8_t k_spind[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_N40_SPIND_CUT,            /* Cut_chg 11 Spinde                                                   */
    0x2b, RE15_N40_SPIND_MSG, 0xff, 0xff,/* Message_on 4          = Platz 16 ROOM1110 main00 @0x00C70 (msg 4)   */
    0x02, 0x00,                          /* Evt_next                                                            */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1                                                          */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)                                                          */
    0x01, 0x00,
};

/* ---- 1110/1111: Zeitbombe im Spind "EXPLOSIVE" (Cut 12), Ereignis 3 = sub03 umhuellt -------------------------- */
static const uint8_t k_bombe[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_N40_BOMBE_CUT,            /* Cut_chg 12 "EXPLOSIVE"                                              */
    /* ROOM1110 sub03 @0x00D24..@0x00D47 Byte fuer Byte (ohne das Evt_end @0x00D48): */
    0x2b, 0x08, 0xff, 0xff,              /* @0x00D24 Message_on 8 "Will you take the Time Bomb?"                */
    0x02, 0x00,                          /* @0x00D28 Evt_next                                                   */
    0x06, 0x00, 0x1a, 0x00,              /* @0x00D2A Ifel_ck (Laenge unveraendert: Block liegt gleich)          */
    0x21, 0x0c, 0x1f, 0x00,              /* @0x00D2E Ck(12,0x1F)=0 (Ja)                                         */
    0x22, 0x03, 0x92, 0x01,              /* @0x00D32 Set(3,0x92)=1                                              */
    0x46, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* @0x00D36 Aot_reset Platz 20 sce 0           */
    0x2b, 0x09, 0xff, 0xff,              /* @0x00D40 Message_on 9 "You've taken the Time Bomb."                 */
    0x02, 0x00,                          /* @0x00D44 Evt_next                                                   */
    0x08, 0x00,                          /* @0x00D46 Endif                                                      */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1                                                          */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)                                                          */
    0x01, 0x00,                          /* = @0x00D48 Evt_end                                                  */
};

/* ---- 1100/1101: Stimmschloss ohne Geraet (Cut 7 -> 8 -> 9 -> 10) --------------------------------------------- */
static const uint8_t k_vus[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1100 sub02 @0x00C8C                     */
    0x29, 0x07,                          /* Cut_chg 7 SYSTEM READY = ROOM1100 sub08 @0x00D86                    */
    0x09, 0x0a, 0x1e, 0x00,              /* Sleep 30              = ROOM1100 sub08 @0x00D88                     */
    0x29, 0x08,                          /* Cut_chg 8 SAY HELLO   = ROOM1100 sub08 @0x00D90                     */
    0x09, 0x0a, 0x1e, 0x00,              /* Sleep 30              (Takt wie @0x00D88)                           */
    0x29, 0x09,                          /* Cut_chg 9 INVALID     (Bild ROOM110.BSS Scheibe 9, nie geschaltet)  */
    0x09, 0x0a, 0x1e, 0x00,              /* Sleep 30                                                            */
    0x29, 0x0a,                          /* Cut_chg 10 TRY AGAIN  (Scheibe 10, nie geschaltet)                  */
    0x2b, RE15_N40_VUS_MSG, 0xff, 0xff,  /* Message_on 1          = Platz 2 ROOM1100 sub00 @0x00AD6 (msg 1)     */
    0x02, 0x00,                          /* Evt_next                                                            */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)            = ROOM1100 sub02 @0x00CC0                     */
    0x29, 0x06,                          /* Cut_chg 6             = ROOM1100 sub02 @0x00CC4                     */
    0x3c, 0x01,                          /* Cut_auto 1            = ROOM1100 sub02 @0x00CC6                     */
    0x01, 0x00,
};

/* ---- 11F0/11F1: Notstrom-Pult (Cut 12) vor der Raetselbuehne, Ereignis 16 = sub16 umhuellt ------------------- */
static const uint8_t k_panel[] = {
    0x29, RE15_N40_PANEL_CUT,            /* Cut_chg 12 Pult an der Wand                                        */
    0x2b, 0x00, 0xff, 0xff,              /* = sub16 @0x015A2 Message_on 0                                       */
    0x02, 0x00,                          /* = @0x015A6                                                          */
    0x2b, 0x01, 0xff, 0xff,              /* = @0x015A8 Message_on 1 "Will you push them?"                       */
    0x02, 0x00,                          /* = @0x015AC                                                          */
    0x06, 0x00, 0x48, 0x00,              /* = @0x015AE Ifel_ck, Laenge 0x46 + 2 (eingefuegtes Cut_old)          */
    0x21, 0x0c, 0x1f, 0x00,              /* = @0x015B2 Ck(12,0x1F)=0 (Ja)                                       */
    0x46, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* = @0x015B6 Aot_reset Platz 1 sce 0          */
    0x2a, 0x00,                          /* Cut_old: Raum-Cut wieder angezeigt, damit Cut_chg 10 IHN merkt und
                                          * sub17 @0x016F0 Cut_old wie im Original dorthin zurueckkehrt
                                          * (Cut_chg merkt den ANGEZEIGTEN Cut, @0x800402c0/@0x800402e4)        */
    0x29, 0x0a,                          /* = @0x015C0 Cut_chg 10 Raetselbuehne                                 */
    0x22, 0x05, 0x00, 0x01,  0x22, 0x05, 0x01, 0x01,  0x22, 0x05, 0x02, 0x01,  0x22, 0x05, 0x03, 0x01,
    0x22, 0x05, 0x04, 0x01,  0x22, 0x05, 0x05, 0x01,  0x22, 0x05, 0x06, 0x01,  0x22, 0x05, 0x07, 0x01,
    0x22, 0x05, 0x08, 0x01,  0x22, 0x05, 0x09, 0x01,  0x22, 0x05, 0x0a, 0x01,  0x22, 0x05, 0x0b, 0x01,
    0x22, 0x05, 0x0c, 0x01,              /* = @0x015C2..@0x015F2 Set(5,0..12)=1                                 */
    0x08, 0x00,                          /* = @0x015F6 Endif                                                    */
    0x06, 0x00, 0x0a, 0x00,              /* Ifel_ck "Nein" (Form @0x015AE)                                      */
    0x21, 0x0c, 0x1f, 0x01,              /* Ck(12,0x1F)=1                                                       */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1            = ROOM11D0 sub02 @0x01886                     */
    0x08, 0x00,                          /* Endif                                                               */
    0x01, 0x00,                          /* = @0x015F8 Evt_end                                                  */
};

#define N(a) ((int)sizeof(a))

static int raum(uint16_t r, uint16_t leon) { return r == leon || r == (uint16_t)(leon | 1u); }

const uint8_t *re15_nahaufnahmen_r40_programm(const char *name, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if      (!strcmp(name, "pult"))  { p = k_pult;  n = N(k_pult); }
    else if (!strcmp(name, "spind")) { p = k_spind; n = N(k_spind); }
    else if (!strcmp(name, "bombe")) { p = k_bombe; n = N(k_bombe); }
    else if (!strcmp(name, "vus"))   { p = k_vus;   n = N(k_vus); }
    else if (!strcmp(name, "panel")) { p = k_panel; n = N(k_panel); }
    if (out_len) *out_len = n;
    return p;
}

/* Text-Platz -> Ereignis-Platz in der Form ROOM1110 sub02 @0x00D18 `46 ss 03 31 ff 00 18 ee 00 00`. */
static void umwidmen(uint16_t room_id, int slot, uint8_t msg)
{
    re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_MESSAGE || a->event_id != msg) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[nahaufnahmen40] ROOM%04X: Platz %d nicht msg %d (aktiv %d Typ %d ereignis %d) -> nichts\n",
                room_id, slot, msg, a->active, a->type, a->event_id);
#endif
        return;
    }
    re15_aot_retype(slot, 3, 0x31, 0x00FF, (uint16_t)((RE15_N40_EREIGNIS << 8) | 0x18), 0);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[nahaufnahmen40] ROOM%04X: Platz %d (msg %d) -> Ereignis %d\n", room_id, slot, msg, RE15_N40_EREIGNIS);
#endif
}

void re15_nahaufnahmen_r40_install(uint16_t room_id)
{
    if (raum(room_id, 0x11D0))      umwidmen(room_id, RE15_N40_PULT_SLOT, RE15_N40_PULT_MSG);
    else if (raum(room_id, 0x1110)) umwidmen(room_id, RE15_N40_SPIND_SLOT, RE15_N40_SPIND_MSG);
    else if (raum(room_id, 0x1100)) umwidmen(room_id, RE15_N40_VUS_SLOT, RE15_N40_VUS_MSG);
}

const uint8_t *re15_nahaufnahmen_r40_ereignis(uint16_t room_id, uint8_t event_id)
{
    const uint8_t *p = NULL;
    const char *was = NULL;
    if (event_id == RE15_N40_EREIGNIS) {
        if (raum(room_id, 0x11D0))      { p = k_pult;  was = "Zwinger-Pult Cut 12"; }
        else if (raum(room_id, 0x1110)) { p = k_spind; was = "Waffenspinde Cut 11"; }
        else if (raum(room_id, 0x1100)) { p = k_vus;   was = "VUS 7/8/9/10"; }
    } else if (event_id == RE15_N40_BOMBE_EREIGNIS && raum(room_id, 0x1110)) {
        p = k_bombe; was = "Zeitbombe Cut 12 + sub03";
    } else if (event_id == RE15_N40_PANEL_EREIGNIS && raum(room_id, 0x11F0)) {
        p = k_panel; was = "Notstrom-Pult Cut 12 + sub16";
    }
#ifdef RE15_PLATFORM_PC
    if (p) fprintf(stderr, "[nahaufnahmen40] ROOM%04X Ereignis %u -> %s\n", room_id, (unsigned)event_id, was);
#else
    (void)was;
#endif
    return p;
}
