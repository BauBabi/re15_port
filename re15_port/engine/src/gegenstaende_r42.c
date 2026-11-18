/*
 * RE1.5 Rebuilt — Runde 42 Strang "gegenstaende" (v0.8.31), Stufe 1. Herleitung, Belege und alle Konstanten:
 * include/re15_gegenstaende_r42.h; Texte tools/r42_gegenstaende/texte.py -> gen/gegenstaende_r42_texte.inc.
 */
#include "re15_gegenstaende_r42.h"

#include <stddef.h>
#include <string.h>

#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_msg.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/gegenstaende_r42_texte.inc"
#include "gen/gegenstaende_r42_bilder.inc"
#include "gen/gegenstaende_r42_karte.inc"   /* Stufe 2: Blue Master Keycard 0x47 mit "MICHAEL" (tools/r42_gegenstaende/michaelkarte.py) */

/* Bruecken-Bits (re15_ventilkette_r41.h RE15_R41C_BIT_DRUCK80 / _BRUECKE_UNTEN, Bank 9) und Pult-Ereignis 24
 * (RE15_R41C_EV_PULT): hier als Zahl, damit der Strang keinen Kopf der Ventilkette einbinden muss. */
#define BIT_DRUCK80        101
#define BIT_BRUECKE_UNTEN  102
#define EV_PULT            24

/* ---- Die Fundstellen ------------------------------------------------------------------------------------------------------------
 * raum = genau dieser Raum (Leon xxx0 / Elza xxx1). vorrang = Slot des Original-Satzes an der Stelle (-1 = neuer Slot); art/kennung =
 * was dort stehen muss, sonst wird nichts angefasst. Rechteck = das des Original-Satzes (bzw. die belegte Stelle).
 * ZENSUS der Slot-Bezuege (alle Aot_reset/Aot_on der Raeume, werkzeug dis.py ueber main+sub): 2010/2011 nur Aot_reset 4 (@0x008CE /
 * @0x008EC); 11F0/11F1 Aot_reset 1, Aot_on 14 (@0x017E2); 1191 Aot_reset 0..3, 5; 30E0 Aot_reset 0, 3; 30E1 Aot_reset 0; 40B1, 4050,
 * 4051 keine; 3000/3001 Aot_on 0; 5021 Aot_reset 3/7/8, Aot_on 5; 1071 Aot_reset 14. Keiner trifft einen Vorrang-Slot. */
enum { ART_NEU = 0, ART_ITEM, ART_TEXT, ART_EREIGNIS };

typedef struct {
    uint16_t raum;
    uint8_t  item, menge, bit;
    int8_t   vorrang;
    uint8_t  art;          /* was im Vorrang-Slot erwartet wird                       */
    uint8_t  kennung;      /* ART_ITEM: Item-Id; ART_TEXT: msg; ART_EREIGNIS: Ereignis  */
    int16_t  x, z, w, d;
    uint8_t  band;
    uint8_t  cut_a, cut_b; /* nur bei diesem Eintritts-Cut (work_vars[10], scd_room_setup.c @0x8001d948); 0xFF = immer */
} stelle_t;
#define IMMER 0xFF, 0xFF

static const stelle_t k_stellen[] = {
    /* Mutter (V-03): Pausenraum 2010/2011, beim Erste-Hilfe-Spray main00 @0x00792 `50 03 09 31 00 00 c4 09 5c 12 20 03 20 03 22 00 ..`
     * (Slot 3, Item 0x22, Modell 4). PORT-WAHL Stelle: K-10 "Fund im Pausenraum 2010 bei der Kurbel"; die Kurbel-Zone Slot 4 @0x00880
     * wird nach der Aufnahme von sub02 @0x008CE `46 04 00 ..` geloescht -> dort kein Vorrang moeglich; das Spray ist der naechste
     * sichtbare Gegenstand im Raum. */
    { 0x2010, RE15_G42_MUTTER, 1, RE15_G42_BIT_MUTTER, 3, ART_ITEM, 0x22, 2500, 4700, 800, 800, 0, IMMER },
    { 0x2011, RE15_G42_MUTTER, 1, RE15_G42_BIT_MUTTER, 3, ART_ITEM, 0x22, 2500, 4700, 800, 800, 0, IMMER },
    /* Rohr (V-04): Kesselraum 11F0/11F1, am Schrank main00 @0x00CE6 `2c 0d 03 31 00 00 d8 27 b2 c1 dc 05 e8 03 ff 00 18 13` (Slot 13,
     * sce 3 -> sub19 oeffnet die Schranktuer, Aot_on 14 = Munition). */
    { 0x11F0, RE15_G42_ROHR, RE15_G42_MENGE_ROHR, RE15_G42_BIT_ROHR, 13, ART_EREIGNIS, 19, 10200, -15950, 1500, 1000, 0, IMMER },
    { 0x11F1, RE15_G42_ROHR, RE15_G42_MENGE_ROHR, RE15_G42_BIT_ROHR, 13, ART_EREIGNIS, 19, 10200, -15950, 1500, 1000, 0, IMMER },
    /* Granatwerfer (V-05), Elza: der tote Polizist em4 in 1071 sub00 @0x0161A `44 04 11 87 .. ca 4c 03 00 00 2c 37` (Typ 0x11 bei
     * (844,14124), Text 19 "A police officer. He's dead..." ohne Aufruf); Rechteck um den Koerper wie die Leichen-Zone ROOM1110 Platz 5
     * @0x00AEE (analysis/untersuchungstexte_2026-10-23/README.md, Vorschlag (344,13624,1000,1000)). */
    { 0x1071, RE15_G42_WERFER, RE15_G42_MENGE_WERFER, RE15_G42_BIT_WERFER, -1, ART_NEU, 0, 344, 13624, 1000, 1000, 0, IMMER },
    /* Leere Granathuelsen (V-06), Elza: Waffenkammer 1191 bei den Schrotpatronen sub14 @0x02BBA `50 0b 09 31 00 00 f4 4c 6a b4 ..
     * 16 00 07 00 96 00 0b 00` (Slot 11, Modell 11) und 30E1 bei der Magnum-Munition main00 @0x00A8C `50 05 09 31 00 00 0c 62 20 35 ..
     * 17 00 06 00 c3 00 02 00` (Slot 5, Modell 2; GEGENSTAENDE.md "neben der Magnum"). 1191 ist zweigeteilt: main00 Switch(V[0x0a] =
     * Eintritts-Cut) @0x022CC/@0x022DC/@0x022EC, Case 0 -> Gosub 13 (Schiessstand), Case 0x0c/0x0d -> Gosub 14 (Waffenkammer mit den
     * Patronen) -> die Huelsen nur beim Eintritt in die Waffenkammer. */
    { 0x1191, RE15_G42_HUELSEN, RE15_G42_MENGE_TEIL, RE15_G42_BIT_HUELSEN_1191, 11, ART_ITEM, 0x16, 19700, -19350, 1000, 1000, 0, 0x0C, 0x0D },
    { 0x30E1, RE15_G42_HUELSEN, RE15_G42_MENGE_TEIL, RE15_G42_BIT_HUELSEN_30E1, 5, ART_ITEM, 0x17, 25100, 13600, 1000, 1000, 0, IMMER },
    /* Zuender (V-06), beide, EIN Stueck: die toten RPD-Polizisten 3000/3001 main00 @0x0151A `44 04 11 84 .. 3d 54 c0 00 00 c4 be`
     * (Typ 0x11 bei (-16300,-16700)); Rechteck 1000 x 1000 um den Koerper (Form wie oben). */
    { 0x3000, RE15_G42_ZUENDER, RE15_G42_MENGE_ZUENDER, RE15_G42_BIT_ZUENDER, -1, ART_NEU, 0, -16800, -17200, 1000, 1000, 0, IMMER },
    { 0x3001, RE15_G42_ZUENDER, RE15_G42_MENGE_ZUENDER, RE15_G42_BIT_ZUENDER, -1, ART_NEU, 0, -16800, -17200, 1000, 1000, 0, IMMER },
    /* Nitro-Kapsel (V-06), beide: PORT-WAHL Ort 30E0/30E1 statt 3070/3071 (3070/3071 hat keinen Gegenstand und keinen Untersuchen-
     * Platz, an den sie sich legen liesse); beim Kraut 30E0 main00 @0x00AFA `50 08 09 31 00 00 bc 66 4c 36 .. 24 00 01 00 c2 00 01 00`
     * (Slot 8) / 30E1 @0x00A76 `50 04 09 31 ..` (Slot 4). Leon 1 Stueck (kein Granatwerfer), Elza 6. */
    { 0x30E0, RE15_G42_NITRO, RE15_G42_MENGE_NITRO_LEON, RE15_G42_BIT_NITRO, 8, ART_ITEM, 0x24, 26300, 13900, 1000, 1000, 0, IMMER },
    { 0x30E1, RE15_G42_NITRO, RE15_G42_MENGE_TEIL, RE15_G42_BIT_NITRO, 4, ART_ITEM, 0x24, 26300, 13900, 1000, 1000, 0, IMMER },
    /* Saeure-Kapsel (V-06), Elza: 40B1 bei der Magnum-Munition main00 @0x00724 `50 05 09 31 00 00 44 b2 b0 d2 .. 17 00 06 00 bc 00 00 00`
     * (Slot 5). */
    { 0x40B1, RE15_G42_SAEURE, RE15_G42_MENGE_TEIL, RE15_G42_BIT_SAEURE_40B1, 5, ART_ITEM, 0x17, -19900, -11600, 1000, 1000, 0, IMMER },
    /* Spind 4050/4051 (V-07): main00 @0x01D16 `2c 0e 01 31 00 00 0c e5 2e be e8 03 b0 04 00 00 ff ff` (Slot 14, msg 0 "There's nothing
     * useful in this locker."). Leon Brennstoff, Elza Saeure-Kapsel (GEGENSTAENDE.md V-07 "In Elzas 4051 liegt im selben Spind eine
     * Acid Capsule"). */
    { 0x4050, RE15_G42_BRENNSTOFF, RE15_G42_MENGE_BRENNSTOFF, RE15_G42_BIT_SPIND_4050, 14, ART_TEXT, 0, -6900, -16850, 1000, 1200, 0, IMMER },
    { 0x4051, RE15_G42_SAEURE, RE15_G42_MENGE_TEIL, RE15_G42_BIT_SPIND_4050, 14, ART_TEXT, 0, -6900, -16850, 1000, 1200, 0, IMMER },
    /* Brand-Kapsel (V-06), Elza: 5021 an der Stelle der Magnum-Munition von 5020 (main00 @0x012C0 `50 03 09 31 00 00 36 f7 5c c7 84 03
     * 84 03 17 00 0c 00 87 00 ff 00`, ohne Modell); 5021 hat dort keinen Satz (BSS geteilt). */
    { 0x5021, RE15_G42_BRAND, RE15_G42_MENGE_TEIL, RE15_G42_BIT_BRAND_5021, -1, ART_NEU, 0, -2250, -14500, 900, 900, 0, IMMER },
};
#define N_STELLEN ((int)(sizeof k_stellen / sizeof k_stellen[0]))

/* Leiche 1050: ROOM1051 sub03 @0x00DA4..@0x00DD7 bytegleich (Riegel unit_r42_gegenstaende_programme vergleicht mit der RDT). */
static const uint8_t k_prg_leiche[52] = {
    0x22, 0x02, 0x07, 0x01,  /* @0x00DA4 Set(2,7)=1                     */
    0x2e, 0x01, 0x00, 0x00,  /* @0x00DA8 Work_set + Nop                 */
    0x3f, 0x01, 0x0b, 0x00,  /* @0x00DAC Plc_motion(1,11,0) Hinbeugen   */
    0x09, 0x0a, 0x1e, 0x00,  /* @0x00DB0 Sleep 30                       */
    0x29, 0x09,              /* @0x00DB4 Cut_chg 9                      */
    0x2b, 0x05, 0xff, 0xff,  /* @0x00DB6 Message_on 5                   */
    0x02, 0x00,              /* @0x00DBA Evt_next                       */
    0x47, 0x0c,              /* @0x00DBC Aot_on 12 = Aufnahme           */
    0x02, 0x00,              /* @0x00DBE Evt_next                       */
    0x29, 0x03,              /* @0x00DC0 Cut_chg 3                      */
    0x3c, 0x01,              /* @0x00DC2 Cut_auto 1                     */
    0x3f, 0x01, 0x0b, 0x00,  /* @0x00DC4 Plc_motion                     */
    0x43, 0x00, 0x80, 0x00,  /* @0x00DC8 Plc_flg                        */
    0x09, 0x0a, 0x1e, 0x00,  /* @0x00DCC Sleep 30                       */
    0x42, 0x00,              /* @0x00DD0 Plc_ret                        */
    0x22, 0x02, 0x07, 0x00,  /* @0x00DD2 Set(2,7)=0                     */
    0x01, 0x00,              /* @0x00DD6 Evt_end                        */
};

/* Mutter am Pult (Form ROOM2060 sub18 @0x01678..@0x0169C, Ifel-Laenge = ab dem Satz hinter Ifel_ck bis hinter Endif wie
 * `06 00 14 00` @0x01684 -> Endif @0x0169A). */
static const uint8_t k_prg_mutter[34] = {
    0x2b, RE15_G42_MSG_OHNE, 0xff, 0xff,    /* +00 Message_on 56 (@0x01678 `2b 03 ff ff`)            */
    0x02, 0x00,                             /* +04 Evt_next (@0x0167C)                               */
    0x2b, RE15_G42_MSG_FRAGE, 0xff, 0xff,   /* +06 Message_on 57 Frage (@0x0167E `2b 04 ff ff`)      */
    0x02, 0x00,                             /* +0A Evt_next (@0x01682)                               */
    0x06, 0x00, 0x10, 0x00,                 /* +0C Ifel_ck, Laenge +10..+20 = hinter Endif (@0x01684 `06 00 14 00`) */
    0x21, 0x0c, 0x1f, 0x00,                 /* +10 Ck(12,31)=0 = Ja (@0x01688)                       */
    0x22, RE15_G42_BANK, RE15_G42_BIT_MUTTER_DRIN, 0x01, /* +14 Set(9,43)=1 (Form ROOM2060 sub19 @0x0169E `22 03 90 01`) */
    0x2b, RE15_G42_MSG_DANACH, 0xff, 0xff,  /* +18 Message_on 58 (Form sub19 @0x016C8 "You've used the Fuse.") */
    0x02, 0x00,                             /* +1C Evt_next                                          */
    0x08, 0x00,                             /* +1E Endif (@0x0169A)                                  */
    0x01, 0x00,                             /* +20 Evt_end (@0x0169C)                                */
};

static int bit9(uint8_t b) { return re15_game_flag_get(RE15_G42_BANK, b); }

static int slot_frei_von_oben(void)
{
    for (int s = RE15_AOT_MAX - 1; s >= 0; s--)
        if (!g_aot.slots[s].active) return s;
    return -1;
}

/* Satz unveraendert umziehen (alle parallelen Felder; Form itemorte_r38.c satz_umziehen). */
static void satz_umziehen(int von, int nach)
{
    g_aot.slots[nach]        = g_aot.slots[von];
    g_aot.door_params[nach]  = g_aot.door_params[von];
    g_aot.item_params[nach]  = g_aot.item_params[von];
    g_aot.flag_params[nach]  = g_aot.flag_params[von];
    g_aot.env_params[nach]   = g_aot.env_params[von];
    g_aot.stair_params[nach] = g_aot.stair_params[von];
    g_aot.slots[nach].was_inside = 0;
    g_aot.slots[von].active = 0;
    g_aot.slots[von].was_inside = 0;
}

static int erwartet(const stelle_t *st, const re15_aot_t *a)
{
    switch (st->art) {
    case ART_ITEM:     return a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[st->vorrang].item_type == st->kennung;
    case ART_TEXT:     return a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == st->kennung;
    case ART_EREIGNIS: return a->type == RE15_AOT_TYPE_GENERIC && a->event_id == st->kennung;
    default:           return 0;
    }
}

static void zone_anlegen(int slot, const stelle_t *st)
{
    re15_aot_set_item_tk_prop(slot, st->x + st->w / 2, st->z + st->d / 2, st->w / 2, st->d / 2,
                              st->item, st->menge, st->bit, 0xFFu);
    g_aot.slots[slot].sce_flags = RE15_G42_SAT;   /* Item_aot_set +3 */
    g_aot.slots[slot].band      = st->band;       /* Item_aot_set +4 */
}

static void stelle_anlegen(const stelle_t *st, uint16_t room_id)
{
    if (bit9(st->bit)) return;                        /* genommen -> keine Zone (@0x800406dc) */
    if (st->cut_a != 0xFF && g_scd.work_vars[10] != st->cut_a && g_scd.work_vars[10] != st->cut_b) return;
    int ziel;
    if (st->vorrang >= 0) {
        re15_aot_t *a = &g_aot.slots[st->vorrang];
        if (a->active) {
            if (!erwartet(st, a)) {
#ifdef RE15_PLATFORM_PC
                fprintf(stderr, "[gegenstaende42] ROOM%04X Slot %d: unerwarteter Satz (Typ %d) -> Item 0x%02X nicht angelegt\n",
                        (unsigned)room_id, (int)st->vorrang, (int)a->type, (unsigned)st->item);
#endif
                return;
            }
            const int frei = slot_frei_von_oben();
            if (frei < 0 || frei == st->vorrang) return;
            satz_umziehen(st->vorrang, frei);
        }
        ziel = st->vorrang;
    } else {
        ziel = slot_frei_von_oben();
        if (ziel < 0) return;
    }
    zone_anlegen(ziel, st);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gegenstaende42] ROOM%04X Slot %d: Item 0x%02X x%u Bit (9,%u)\n", (unsigned)room_id, ziel,
            (unsigned)st->item, (unsigned)st->menge, (unsigned)st->bit);
#endif
}

void re15_gegenstaende_r42_install(uint16_t room_id)
{
    for (int i = 0; i < N_STELLEN; i++)
        if (k_stellen[i].raum == room_id) stelle_anlegen(&k_stellen[i], room_id);

    /* Colt Python statt Head of Akuma (K-10 (3), V-02): ROOM2040/2041 main00 @0x01364 `50 07 09 31 03 00 a4 06 62 c6 e8 03 e8 03 32 00
     * 01 00 5b 00 ff 00` -> Item 0x14, Menge 6 (@0x80074E98); Slot, Rechteck, Band 3 und Bit 91 bleiben. */
    if ((room_id & 0xFFFEu) == 0x2040u) {
        for (int s = 0; s < RE15_AOT_MAX; s++) {
            re15_aot_t *a = &g_aot.slots[s];
            if (a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[s].item_type == RE15_G42_AKUMA &&
                g_aot.item_params[s].taken_bit == RE15_G42_BIT_PYTHON) {
                g_aot.item_params[s].item_type = RE15_G42_PYTHON;
                g_aot.item_params[s].amount    = RE15_G42_MENGE_PYTHON;
            }
        }
    }

    /* Leiche 1050 (Glock, V-01): Form 1051 main00 @0x00C0E (Ereignis-Platz) + @0x00C22 (Item, Rechteck 0, nur ohne Bit). */
    if (room_id == RE15_G42_RAUM_LEICHE) {
        const int s = RE15_G42_LEICHE_SLOT, si = RE15_G42_LEICHE_ITEM_SLOT;
        if (!g_aot.slots[s].active) {
            re15_aot_set(s, RE15_AOT_TYPE_GENERIC, RE15_G42_LEICHE_EREIGNIS,
                         RE15_G42_LEICHE_X + RE15_G42_LEICHE_W / 2, RE15_G42_LEICHE_Z + RE15_G42_LEICHE_D / 2,
                         RE15_G42_LEICHE_W / 2, RE15_G42_LEICHE_D / 2);
            g_aot.slots[s].sce_flags = RE15_G42_SAT;
            g_aot.slots[s].band      = 0;
            re15_aot_retype(s, 3, RE15_G42_SAT, 0x00FF, (uint16_t)((RE15_G42_LEICHE_EREIGNIS << 8) | 0x18), 0);   /* `ff 00 18 05` */
        }
        if (!g_aot.slots[si].active && !bit9(RE15_G42_BIT_GLOCK)) {
            re15_aot_set_item_tk_prop(si, 0, 0, 0, 0, RE15_G42_GLOCK, RE15_G42_MENGE_GLOCK, RE15_G42_BIT_GLOCK, 0xFFu);
            g_aot.slots[si].sce_flags = RE15_G42_SAT;
            g_aot.slots[si].band      = 0;
        }
    }

    /* Texte am Bruecken-Panel 2030/2031. */
    if ((room_id & 0xFFFEu) == RE15_G42_RAUM_PULT) {
        re15_msg_install_text(RE15_G42_MSG_OHNE,   k_g42_msg_ohne,   sizeof k_g42_msg_ohne);
        re15_msg_install_text(RE15_G42_MSG_FRAGE,  k_g42_msg_frage,  sizeof k_g42_msg_frage);
        re15_msg_install_text(RE15_G42_MSG_DANACH, k_g42_msg_danach, sizeof k_g42_msg_danach);
    }
}

/* Je Bild (scd_vm_tick, neben dem Raum-sub01): 1050 hat kein Gegenstueck zu 1051 sub01 @0x00CB2
 * `06 00 10 00 | 21 09 a5 01 | 46 0b 00 00 00 00 00 00 00 00 | 08 00` (Ck(9,165) -> Aot_reset(11, sce 0, sat 0)); 1050 sub01 @0x00CAA
 * ist nur Evt_end. Port: dieselbe Regel mit Bit (9,40). Danach greift das Tagebuch (Dokument 1, Slot 15) auf derselben Leiche. */
void re15_gegenstaende_r42_tick(void)
{
    extern unsigned g_current_room_id;
    if (g_current_room_id != RE15_G42_RAUM_LEICHE || !bit9(RE15_G42_BIT_GLOCK)) return;
    const re15_aot_t *a = &g_aot.slots[RE15_G42_LEICHE_SLOT];
    if (a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_G42_LEICHE_EREIGNIS)
        re15_aot_retype(RE15_G42_LEICHE_SLOT, 0, 0, 0, 0, 0);   /* @0x00CBA `46 0b 00 00 00 00 00 00 00 00` */
}

const uint8_t *re15_gegenstaende_r42_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id == RE15_G42_RAUM_LEICHE && event_id == RE15_G42_LEICHE_EREIGNIS) return k_prg_leiche;
    if ((room_id & 0xFFFEu) == RE15_G42_RAUM_PULT && event_id == EV_PULT && !bit9(RE15_G42_BIT_MUTTER_DRIN) &&
        !bit9(BIT_DRUCK80) && !bit9(BIT_BRUECKE_UNTEN))
        return k_prg_mutter;
    return NULL;
}

int re15_gegenstaende_r42_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item)
{
    if (((uint16_t)room_id & 0xFFFEu) != RE15_G42_RAUM_PULT || msg_id != RE15_G42_MSG_DANACH) return 0;
    if (out_item) *out_item = RE15_G42_MUTTER;
    return 1;
}

const uint8_t *re15_gegenstaende_r42_desc(uint8_t item_id)
{
    switch (item_id) {
    case RE15_G42_MUTTER:  return k_g42_desc_mutter;
    case RE15_G42_ZUENDER: return k_g42_desc_zuender;
    case RE15_G42_HUELSEN: return k_g42_desc_huelsen;
    case RE15_G42_NITRO:   return k_g42_desc_nitro;
    case RE15_G42_SAEURE:  return k_g42_desc_saeure;
    case RE15_G42_BRAND:   return k_g42_desc_brand;
    default:               return NULL;
    }
}

int re15_gegenstaende_r42_kein_heilmittel(uint8_t item_id) { return item_id == RE15_G42_MUTTER; }

int re15_gegenstaende_r42_paar(uint8_t id_a, uint8_t id_b, uint8_t *res, uint8_t *pic)
{
    if (id_a == RE15_G42_GLOCK && id_b == RE15_G42_HG_MUNITION) {   /* @0x80074C98 `15 04 02 00` -> `15 06 02 00` */
        if (res) *res = RE15_G42_GLOCK;
        if (pic) *pic = 0;
        return 2;
    }
    if (id_a == RE15_G42_HG_MUNITION && id_b == RE15_G42_GLOCK) {   /* @0x80074CDC `04 04 03 00` -> `06 06 03 00` */
        if (res) *res = RE15_G42_GLOCK;
        if (pic) *pic = 0;
        return 3;
    }
    return 0;
}

int re15_gegenstaende_r42_stellen_anzahl(void) { return N_STELLEN; }

int re15_gegenstaende_r42_stelle(int i, uint16_t *raum, uint8_t *item, uint8_t *menge, uint8_t *bit, int *vorrang_slot)
{
    if (i < 0 || i >= N_STELLEN) return 0;
    if (raum) *raum = k_stellen[i].raum;
    if (item) *item = k_stellen[i].item;
    if (menge) *menge = k_stellen[i].menge;
    if (bit) *bit = k_stellen[i].bit;
    if (vorrang_slot) *vorrang_slot = k_stellen[i].vorrang;
    return 1;
}

const uint8_t *re15_gegenstaende_r42_programm(int welches, int *out_len)
{
    if (welches == 0) { if (out_len) *out_len = (int)sizeof k_prg_leiche; return k_prg_leiche; }
    if (welches == 1) { if (out_len) *out_len = (int)sizeof k_prg_mutter; return k_prg_mutter; }
    if (out_len) *out_len = 0;
    return NULL;
}

/* Item-Bilder der Kapseln (tools/r42_gegenstaende/kapselbilder.py): im GELADENEN ITPS-Puffer CLUT (+0x14, 512 B) und Bild (+0x220,
 * 112x72) der Bloecke 0x1E/0x1F/0x20 ersetzen; Kopf und Rechtecke des Blocks bleiben (Muster re15_ultimate15.h "nur CLUT, Bild").
 * Die Datei bleibt byte-true. */
void re15_gegenstaende_r42_bild_einsetzen(uint8_t *itps, int size)
{
    static const uint8_t *const bild[3] = { k_g42_kapsel_1e, k_g42_kapsel_1f, k_g42_kapsel_20 };
    if (!itps) return;
    for (int k = 0; k < 3; k++) {
        const int id = RE15_G42_NITRO + k;
        if ((id + 1) * 0x3000 > size) continue;
        uint8_t *b = itps + id * 0x3000;
        memcpy(b + 0x14, k_g42_kapsel_clut, sizeof k_g42_kapsel_clut);
        memcpy(b + 0x220, bild[k], 112 * 72);
    }
    /* Stufe 2 (Konzept T-09, Uebergabe 11-17 "Kartenbild Michael"): Block 0x47 @0xD5000 = Blue Master Keycard. Nur das Bild
     * (+0x220, 112x72) wird ersetzt, die CLUT (+0x14) bleibt die des Blocks; geaendert sind nur die Pixel des alten Namens
     * "GORO / MACIDA" (Herleitung und Messung im Kopf von gen/gegenstaende_r42_karte.inc). */
    if ((RE15_G42_KARTE_ID + 1) * 0x3000 <= size)
        memcpy(itps + RE15_G42_KARTE_ID * 0x3000 + 0x220, k_g42_karte47, sizeof k_g42_karte47);
}
