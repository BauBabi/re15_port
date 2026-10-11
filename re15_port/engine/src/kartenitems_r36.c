/*
 * RE1.5 Rebuilt — Lageplaene zum Aufsammeln (Runde 36 Spur N). Herleitung, Belege und alle
 * Konstanten: include/re15_kartenitems_r36.h und analysis/befunde_runde36/N_kartenitems.md.
 * Vorbild der Datei-Form: inhalt_r35.c (Prop + Zone beim Raumstart), rolltor_1050.c (Port-Programm
 * mit Ja/Nein-Frage, Texte beim Ausloesen).
 */
#include "re15_kartenitems_r36.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/kartenitems_r36.inc"

#define N_ZEILEN ((int)(sizeof k_r36k_lage / sizeof k_r36k_lage[0]))

static int index_des_raums(uint16_t room_id)
{
    for (int k = 0; k < N_ZEILEN; k++)
        if ((uint16_t)(room_id & 0xFFFEu) == k_r36k_lage[k].raum) return k;
    return -1;
}

int re15_kartenitems_r36_anzahl(void) { return N_ZEILEN; }

const re15_kartenitem_r36_t *re15_kartenitems_r36_zeile(int i)
{
    return (i >= 0 && i < N_ZEILEN) ? &k_r36k_lage[i] : NULL;
}

const re15_kartenitem_r36_t *re15_kartenitems_r36_des_raums(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    return k >= 0 ? &k_r36k_lage[k] : NULL;
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* Ist der Satz in `slot` schon unsere Aufhebe-Zone? (Idempotenz: beide Raumstart-Wege koennen
 * hintereinander laufen; ein zweites Verdraengen wuerde den verdraengten Satz ueberschreiben.) */
static int ist_unsere_zone(int slot)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    return a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R36K_EREIGNIS;
}

/* Den Satz aus `von` unveraendert nach `nach` tragen (alle parallelen Felder, wie sie der Scan liest). */
static void satz_umziehen(int von, int nach)
{
    g_aot.slots[nach]       = g_aot.slots[von];
    g_aot.door_params[nach] = g_aot.door_params[von];
    g_aot.item_params[nach] = g_aot.item_params[von];
    g_aot.flag_params[nach] = g_aot.flag_params[von];
    g_aot.env_params[nach]  = g_aot.env_params[von];
    g_aot.stair_params[nach] = g_aot.stair_params[von];
    g_aot.slots[nach].was_inside = 0;
}

void re15_kartenitems_r36_install(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    if (k < 0) return;
    const re15_kartenitem_r36_t *g = &k_r36k_lage[k];
    /* schon genommen -> weder Prop noch Zone (RE2 room20B0: das Weltobjekt bleibt bei (20000,..)
     * geparkt, RE1.5 ROOM5030 sub00 @0x01086 Ck(3,115,0) legt den Ausloeser nur ohne Bit an). */
    if (re15_game_flag_get(RE15_R36K_BANK, g->bit)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914), Muster inhalt_r35.c */
    if (slot_von_obj_id(RE15_R36K_OBJ) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = RE15_R36K_OBJ;
        g_scd.props[i].obj_type   = 0;                     /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_R36K_PROP_BAND;   /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                    /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = g->x;
        g_scd.props[i].y = g->y;
        g_scd.props[i].z = g->z;
        g_scd.props[i].rot_x = g->rot_x;
        g_scd.props[i].rot_y = g->rot_y;
        g_scd.props[i].rot_z = g->rot_z;
        g_scd.props[i].flags = RE15_R36K_PROP_FLAGS;       /* 0x000A | 1 @0x80040998         */
    }

    /* ZONE — EVENT-Satz wie ROOM5030 sub00 @0x0108A (sce 3, sat 0x31, p0 0x00FF, p1 = Sub<<8|0x18). */
    if (g->aot_slot >= RE15_AOT_MAX || ist_unsere_zone(g->aot_slot)) return;
    if (g->verdraengt_nach < RE15_AOT_MAX) {
        /* ROOM1030/1031: Slot 8 ist der Muenztelefon-Satz (main00 Aot_set @0x01D2E bzw. @0x01D9C,
         * sce 1, Rechteck 4096 x 4096 ueber der ganzen Rueckwand). Der Aktions-Scan nimmt je Druck den
         * KLEINSTEN treffenden Slot (FUN_80042bac, aot_common.c "one action handler per press") — die
         * Karte in einem hohen Slot waere dort nie erreichbar. Der Satz zieht deshalb unveraendert in
         * den freien Slot verdraengt_nach um, die Karte nimmt seinen Platz; ausserhalb ihres Rechtecks
         * antwortet das Telefon wie bisher, nach der Aufnahme (Aot_reset(8,0)) ueberall. */
        if (g_aot.slots[g->aot_slot].active) {
            if (g_aot.slots[g->verdraengt_nach].active) return;   /* kein freier Platz: nichts anlegen */
            satz_umziehen(g->aot_slot, g->verdraengt_nach);
        }
    } else if (g_aot.slots[g->aot_slot].active) {
        return;                                              /* fremder Satz im Slot: nichts anlegen */
    }
    re15_aot_set(g->aot_slot, RE15_AOT_TYPE_GENERIC, RE15_R36K_EREIGNIS,
                 g->rect_x + RE15_R36K_RECT / 2, g->rect_z + RE15_R36K_RECT / 2,
                 RE15_R36K_RECT / 2, RE15_R36K_RECT / 2);
    g_aot.slots[g->aot_slot].sce_flags = 0;
    g_aot.slots[g->aot_slot].band      = 0;                 /* Aot_set pc[4] = 0 wie ROOM5030       */
    re15_aot_retype(g->aot_slot, RE15_R36K_AOT_SCE, RE15_R36K_AOT_SAT, RE15_R36K_AOT_P0,
                    (uint16_t)((RE15_R36K_EREIGNIS << 8) | 0x18), 0);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[kartenitems] ROOM%04X: Karte Blatt %d (Bit 9,%d) Prop obj %d bei (%ld,%ld,%ld), "
                    "Zone Slot %d%s\n", (unsigned)room_id, g->blatt, g->bit, RE15_R36K_OBJ,
            (long)g->x, (long)g->y, (long)g->z, g->aot_slot,
            g->verdraengt_nach < RE15_AOT_MAX ? " (Muenztelefon -> Slot 39)" : "");
#endif
}

/* ---- Das Port-Programm. Jede Zeile mit ihrem Vorbild. ----------------------------------------------
 * Ifel_ck-Laenge nach ROOM5030 sub02 @0x010B2 `06 00 1e 00`: Blockende = Ifel_ck + 4 + Laenge = hinter
 * dem Endif (0x010B2 + 4 + 0x1E = 0x010D4). Hier: +0x06 + 4 + 0x2C = +0x36. */
static const uint8_t k_programm[] = {
    /* +00 */ 0x2b, RE15_R36K_MSG_FRAGE, 0xff, 0xff,   /* Message_on 30 (Frage), Maske 0xFFFF  = ROOM5030 sub02 @0x010AC */
    /* +04 */ 0x02, 0x00,                              /* Evt_next + Nop                       = @0x010B0                 */
    /* +06 */ 0x06, 0x00, 0x2c, 0x00,                  /* Ifel_ck, Blockende +0x36             = @0x010B2                 */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,                  /* Ck(12,31,0) = Ja                     = @0x010B6                 */
    /* +0E */ 0x46, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                       /* Aot_reset(Slot,0) Zone weg           = @0x010BA (Slot = Zeile)  */
    /* +18 */ 0x09, 0x0a, 0x01, 0x00,                  /* Sleep 1                              = @0x010C4                 */
    /* +1C */ 0x22, RE15_R36K_BANK, 0xEE, 0x01,        /* Set(9,Bit,1) Besitz + Etagenwahl     = Rolle @0x010C8; RE2
                                                        * Set(33,n) @0x037DA + Set(35,n) @0x037E6 in EINEM Bit   */
    /* +20 */ 0x2e, 0x03, RE15_R36K_OBJ,               /* Work_set(3 = Objekt, obj 15)         = RE2 @0x037F2 `2e 04 08`
                                                        * (RE1.5 Art 3 = Objekt: scd_vm.c scd_work_bind)          */
    /* +23 */ 0x00,                                    /* Nop                                  = RE2 @0x037F5             */
    /* +24 */ 0x32, 0x00, 0x20, 0x4e, 0x20, 0x4e, 0x20, 0x4e,
                                                       /* Pos_set(20000,20000,20000) Modell weg = RE2 @0x037F6           */
    /* +2C */ 0x02, 0x00,                              /* Evt_next + Nop                       = RE2 @0x037FE             */
    /* +2E */ 0x2b, RE15_R36K_MSG_GENOMMEN, 0xff, 0xff,/* Message_on 31 "You've taken the ..." = @0x010CC / RE2 @0x03800  */
    /* +32 */ 0x02, 0x00,                              /* Evt_next + Nop                       = @0x010D0                 */
    /* +34 */ 0x08, 0x00,                              /* Endif                                = @0x010D2                 */
    /* +36 */ 0x01, 0x00,                              /* Evt_end                              = @0x010D4                 */
};
#define PRG_SLOT 0x0F   /* Offset des Slot-Bytes im Aot_reset */
#define PRG_BIT  0x1E   /* Offset des Bit-Bytes im Set        */

static uint8_t s_programm[sizeof k_programm];

const uint8_t *re15_kartenitems_r36_programm(uint16_t room_id, int *out_len)
{
    int k = index_des_raums(room_id);
    if (out_len) *out_len = 0;
    if (k < 0) return NULL;
    memcpy(s_programm, k_programm, sizeof k_programm);
    s_programm[PRG_SLOT] = k_r36k_lage[k].aot_slot;
    s_programm[PRG_BIT]  = k_r36k_lage[k].bit;
    if (out_len) *out_len = (int)sizeof s_programm;
    return s_programm;
}

const uint8_t *re15_kartenitems_r36_meldung(uint16_t room_id, int welche, int *out_len)
{
    int k = index_des_raums(room_id);
    if (out_len) *out_len = 0;
    if (k < 0 || welche < 0 || welche > 1) return NULL;
    if (out_len) *out_len = welche ? k_r36k_genommen_len[k] : k_r36k_frage_len[k];
    return welche ? k_r36k_genommen[k] : k_r36k_frage[k];
}

static void text_einsetzen(unsigned char id, const uint8_t *m, int len)
{
    re15_msg_install_text(id, m, (size_t)len);
    /* Dauer-Eintrag wie fuer jede RDT-Nachricht (re15_msg_load_room_block; Muster rolltor_1050.c). */
    int d = re15_msg_compute_duration(m, (size_t)len, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

const uint8_t *re15_kartenitems_r36_ereignis(uint16_t room_id, uint8_t event_id)
{
    int k = index_des_raums(room_id);
    if (k < 0 || event_id != RE15_R36K_EREIGNIS) return NULL;
    if (re15_game_flag_get(RE15_R36K_BANK, k_r36k_lage[k].bit)) return NULL;
    /* Texte beim Ausloesen einsetzen (Muster rolltor_1050.c): gilt an jedem Raumstart-Weg, der
     * Raum-Teardown loescht sie wieder (re15_msg_clear_room_block). */
    text_einsetzen(RE15_R36K_MSG_FRAGE, k_r36k_frage[k], k_r36k_frage_len[k]);
    text_einsetzen(RE15_R36K_MSG_GENOMMEN, k_r36k_genommen[k], k_r36k_genommen_len[k]);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[kartenitems] ROOM%04X Ereignis %u -> Port-Programm Karte Blatt %d\n",
            (unsigned)room_id, (unsigned)event_id, k_r36k_lage[k].blatt);
#endif
    return re15_kartenitems_r36_programm(room_id, NULL);
}

int re15_kartenitems_r36_obj_id(uint16_t room_id)
{
    return index_des_raums(room_id) >= 0 ? RE15_R36K_OBJ : -1;
}

const uint8_t *re15_kartenitems_r36_md1_bytes(uint16_t room_id, int *out_size)
{
    int k = index_des_raums(room_id);
    if (out_size) *out_size = 0;
    if (k < 0) return NULL;
    if (out_size) *out_size = (int)sizeof k_r36k_md1_0;
    return k_r36k_md1[k];
}

const uint8_t *re15_kartenitems_r36_tim_bytes(uint16_t room_id, int *out_size)
{
    if (out_size) *out_size = 0;
    if (index_des_raums(room_id) < 0) return NULL;
    if (out_size) *out_size = (int)sizeof k_r36k_tim;
    return k_r36k_tim;
}

int re15_kartenitems_r36_blatt_besitz(unsigned page)
{
    for (int k = 0; k < N_ZEILEN; k++)
        if (k_r36k_lage[k].blatt == page && re15_game_flag_get(RE15_R36K_BANK, k_r36k_lage[k].bit))
            return 1;
    return 0;
}

uint32_t re15_kartenitems_r36_besitz_bits(void)
{
    uint32_t m = 0;
    for (int k = 0; k < N_ZEILEN; k++)
        if (k_r36k_lage[k].blatt < 32 && re15_game_flag_get(RE15_R36K_BANK, k_r36k_lage[k].bit))
            m |= (uint32_t)1u << k_r36k_lage[k].blatt;
    return m;
}
