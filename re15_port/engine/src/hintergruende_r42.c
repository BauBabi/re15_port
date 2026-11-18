/*
 * RE1.5 Rebuilt — Runde 42 "rest", Stufe 2: nie gezeigte Kameras beim Untersuchen (10F Funkkonsole Cut 10, Regal Cut 9;
 * 1010/1011 Memory Card Cut 7; 1090 Ende des Loeschens Cut 14).
 *
 * Herleitung, Zensus, Bild-gegen-Stelle und alle Vorbilder: include/re15_hintergruende_r42.h. Kein Asset-Patch: die RDTs
 * bleiben byte-true, die Programme bestehen nur aus Original-Befehlen.
 */
#include "re15_hintergruende_r42.h"

#include "re15_aot.h"          /* g_aot, re15_aot_retype, re15_aot_set */
#include "re15_inhalt_r35.h"   /* RE15_R35I_KARTE_SLOT / _ITEM / _BIT */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ---- 10F0/10F1: Funkkonsole (Cut 10), msg 0 "... It's out of commission. ..." -------------------------------------- */
static const uint8_t k_funk[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_HG42_FUNK_CUT,            /* Cut_chg 10 Konsole    Form ROOM1051 sub03 @0x00DB4                  */
    0x2b, RE15_HG42_FUNK_MSG, 0xff, 0xff,/* Message_on 0          = Platz 2/3 ROOM10F0 main00 @0x00F72/@0x00F86 */
    0x02, 0x00,                          /* Evt_next              = ROOM1051 sub03 @0x00DBA                     */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1            = ROOM11D0 sub02 @0x01886                     */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)            = ROOM11D0 sub02 @0x01888                     */
    0x01, 0x00,                          /* Evt_end                                                             */
};

/* ---- 10F0/10F1: Regal (Cut 9), msg 2 "There isnt anything useful on this shelf." ------------------------------------ */
static const uint8_t k_regal[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_HG42_REGAL_CUT,           /* Cut_chg 9 Regal       Form ROOM1051 sub03 @0x00DB4                  */
    0x2b, RE15_HG42_REGAL_MSG, 0xff, 0xff,/* Message_on 2         = Platz 6 ROOM10F0 main00 @0x00FC2            */
    0x02, 0x00,                          /* Evt_next                                                            */
    0x2a, 0x00,                          /* Cut_old                                                             */
    0x3c, 0x01,                          /* Cut_auto 1                                                          */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)                                                          */
    0x01, 0x00,
};

/* ---- 1010/1011: Memory Card nehmen mit Nahbild (Cut 7), Form der Leiche ROOM1051 ------------------------------------ */
static const uint8_t k_karte[] = {
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)            = ROOM1051 sub03 @0x00DA4                     */
    0x29, RE15_HG42_KARTE_CUT,           /* Cut_chg 7 Regal-Ecke  Form ROOM1051 sub03 @0x00DB4                  */
    0x47, RE15_R35I_KARTE_SLOT,          /* Aot_on 10 Aufnahme    Form ROOM1051 sub03 @0x00DBC `47 0c`          */
    0x02, 0x00,                          /* Evt_next              = ROOM1051 sub03 @0x00DBE                     */
    0x06, 0x00, 0x10, 0x00,              /* Ifel_ck               = ROOM1051 sub01 @0x00CB2                     */
    0x21, 0x09, RE15_R35I_KARTE_BIT, 0x01,/* Ck(9,80)=1 genommen  Form ROOM1051 sub01 @0x00CB6 `21 09 a5 01`    */
    0x46, RE15_HG42_KARTE_EVT_SLOT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         /* Aot_reset 11 sce 0    = ROOM1051 sub01 @0x00CBA `46 0b 00 ..`       */
    0x08, 0x00,                          /* Endif                 = ROOM1051 sub01 @0x00CC4                     */
    0x2a, 0x00,                          /* Cut_old               = ROOM11D0 sub02 @0x01884                     */
    0x3c, 0x01,                          /* Cut_auto 1            = ROOM11D0 sub02 @0x01886                     */
    0x22, 0x02, 0x07, 0x00,              /* Set(2,7,0)            = ROOM11D0 sub02 @0x01888                     */
    0x01, 0x00,                          /* Evt_end                                                             */
};

/* ---- 1090 (nur Leon): Ende des Loeschens im Feuerschein (Cut 14), A3 ------------------------------------------------
 * Ausgelieferter sub06 @0x02702..@0x0272B Byte fuer Byte; nach "Ja" (Ck(12,31)=0) zusaetzlich Cut_chg 14 vor der Wartezeit,
 * dann wie ausgeliefert Set(3,129)/Set(3,132)/Aot_on 3 (Selbst-Tuer -> Cut 6 ohne Feuer, gleiche Kamera). */
static const uint8_t k_feuer[] = {
    0x2b, 0x07, 0xff, 0xff,              /* Message_on 7 "I must hurry up ..."   = ROOM1090 sub06 @0x02702          */
    0x02, 0x00,                          /* Evt_next                             = @0x02706                         */
    0x2b, 0x08, 0xff, 0xff,              /* Message_on 8 "Will you use the Fire Extinguisher?" = @0x02708           */
    0x02, 0x00,                          /* Evt_next                             = @0x0270C                         */
    0x06, 0x00, 0x1a, 0x00,              /* Ifel_ck (Block 0x18 @0x0270E + 2 Byte Einschub)                         */
    0x21, 0x0c, 0x1f, 0x00,              /* Ck(12,31)=0 = "Ja"                   = @0x02712                         */
    0x22, 0x02, 0x07, 0x01,              /* Set(2,7,1)                           = @0x02716                         */
    0x29, RE15_HG42_FEUER_CUT,           /* Cut_chg 14 Feuerschein = Disc ROOM1090 sub11 @0x02472 (Auto-Kamera aus @0x800402d4) */
    0x09, 0x0a, 0x0a, 0x00,              /* Sleep 10                             = @0x0271A                         */
    0x22, 0x03, 0x81, 0x01,              /* Set(3,129,1) Feuer aus               = @0x0271E                         */
    0x22, 0x03, 0x84, 0x01,              /* Set(3,132,1)                         = @0x02722                         */
    0x47, 0x03,                          /* Aot_on 3 Selbst-Tuer (Cut 6)         = @0x02726                         */
    0x08, 0x00,                          /* Endif                                = @0x02728                         */
    0x01, 0x00,                          /* Evt_end                              = @0x0272A                         */
};

#define N(a) ((int)sizeof(a))

static int raum(uint16_t r, uint16_t leon) { return r == leon || r == (uint16_t)(leon | 1u); }

const uint8_t *re15_hintergruende_r42_programm(const char *name, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if      (!strcmp(name, "funk"))  { p = k_funk;  n = N(k_funk); }
    else if (!strcmp(name, "regal")) { p = k_regal; n = N(k_regal); }
    else if (!strcmp(name, "karte")) { p = k_karte; n = N(k_karte); }
    else if (!strcmp(name, "feuer")) { p = k_feuer; n = N(k_feuer); }
    if (out_len) *out_len = n;
    return p;
}

/* Text-Platz -> Ereignis-Platz in der Form ROOM1110 sub02 @0x00D18 `46 ss 03 31 ff 00 18 ee 00 00` (Rechteck bleibt),
 * wie nahaufnahmen_r40.c umwidmen(). Nur der unveraenderte Original-Satz wird umgewidmet. */
static void umwidmen(uint16_t room_id, int slot, uint8_t msg, uint8_t ereignis)
{
    re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_MESSAGE || a->event_id != msg) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[hintergruende42] ROOM%04X: Platz %d nicht msg %d (aktiv %d Typ %d ereignis %d) -> nichts\n",
                room_id, slot, msg, a->active, a->type, a->event_id);
#endif
        return;
    }
    re15_aot_retype(slot, 3, 0x31, 0x00FF, (uint16_t)((ereignis << 8) | 0x18), 0);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[hintergruende42] ROOM%04X: Platz %d (msg %d) -> Ereignis %d\n", room_id, slot, msg, ereignis);
#endif
}

/* Memory Card: Platz 10 (r35) -> Rechteck 0 (Form ROOM1051 main00 @0x00C22), Platz 11 = Ereignis mit dem alten Rechteck
 * (Form ROOM1051 main00 @0x00C0E). Nur wenn Platz 10 noch die r35-Karte traegt und Platz 11 frei ist. */
static void karte(uint16_t room_id)
{
    re15_aot_t *it = &g_aot.slots[RE15_R35I_KARTE_SLOT];
    re15_aot_t *ev = &g_aot.slots[RE15_HG42_KARTE_EVT_SLOT];
    if (!it->active || it->type != RE15_AOT_TYPE_ITEM ||
        g_aot.item_params[RE15_R35I_KARTE_SLOT].item_type != RE15_R35I_KARTE_ITEM || ev->active) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[hintergruende42] ROOM%04X: Karte nicht in Platz %d oder Platz %d belegt -> nichts\n",
                room_id, RE15_R35I_KARTE_SLOT, RE15_HG42_KARTE_EVT_SLOT);
#endif
        return;
    }
    re15_aot_set(RE15_HG42_KARTE_EVT_SLOT, RE15_AOT_TYPE_GENERIC, RE15_HG42_EREIGNIS, it->x, it->z, it->half_w, it->half_h);
    ev->sce_flags = 0;
    ev->band      = it->band;                       /* Etage der Karte (Aot_set pc[4]) */
    re15_aot_retype(RE15_HG42_KARTE_EVT_SLOT, 3, 0x31, 0x00FF, (uint16_t)((RE15_HG42_EREIGNIS << 8) | 0x18), 0);
    it->x = 0; it->z = 0; it->half_w = 0; it->half_h = 0;   /* @0x00C22: Rechteck (0,0,0,0) */
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[hintergruende42] ROOM%04X: Karte Platz %d Rechteck 0, Ereignis-Platz %d -> Ereignis %d (Cut %d)\n",
            room_id, RE15_R35I_KARTE_SLOT, RE15_HG42_KARTE_EVT_SLOT, RE15_HG42_EREIGNIS, RE15_HG42_KARTE_CUT);
#endif
}

void re15_hintergruende_r42_install(uint16_t room_id)
{
    if (raum(room_id, 0x10F0)) {
        umwidmen(room_id, RE15_HG42_FUNK_SLOT_OST,  RE15_HG42_FUNK_MSG,  RE15_HG42_EREIGNIS);
        umwidmen(room_id, RE15_HG42_FUNK_SLOT_NORD, RE15_HG42_FUNK_MSG,  RE15_HG42_EREIGNIS);
        umwidmen(room_id, RE15_HG42_REGAL_SLOT,     RE15_HG42_REGAL_MSG, RE15_HG42_EREIGNIS_REGAL);
    } else if (raum(room_id, 0x1010)) {
        karte(room_id);
    }
}

const uint8_t *re15_hintergruende_r42_ereignis(uint16_t room_id, uint8_t event_id)
{
    const uint8_t *p = NULL;
    const char *was = NULL;
    if (raum(room_id, 0x10F0)) {
        if (event_id == RE15_HG42_EREIGNIS)            { p = k_funk;  was = "Funkkonsole Cut 10 + msg 0"; }
        else if (event_id == RE15_HG42_EREIGNIS_REGAL) { p = k_regal; was = "Regal Cut 9 + msg 2"; }
    } else if (raum(room_id, 0x1010) && event_id == RE15_HG42_EREIGNIS) {
        p = k_karte; was = "Memory Card Cut 7 + Aot_on 10";
    } else if (room_id == RE15_HG42_FEUER_RAUM && event_id == RE15_HG42_FEUER_EREIGNIS) {
        p = k_feuer; was = "Loeschen: nach Ja Cut 14 (Feuerschein), dann Selbst-Tuer";
    }
#ifdef RE15_PLATFORM_PC
    if (p) fprintf(stderr, "[hintergruende42] ROOM%04X Ereignis %u -> %s\n", room_id, (unsigned)event_id, was);
#else
    (void)was;
#endif
    return p;
}
