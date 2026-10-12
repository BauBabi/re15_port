/*
 * RE1.5 Rebuilt — Feuerloescher (Runde 37 Spur I). Herleitung, Belege und alle Konstanten:
 * include/re15_feuerloescher_r37.h und analysis/befunde_runde37/I_feuerloescher.md.
 * Vorbild der Datei-Form: inhalt_r35.c (Item-Prop + Aufnahme-Zone beim Raumstart), kartenitems_r36.c
 * (eingebackenes MD1/TIM), story_r36.c (Untersuchen-Text-Platz mit eigener Nachricht).
 */
#include "re15_feuerloescher_r37.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_inventory.h"   /* re15_inv_find_item (FUN_8004dfec) */
#include "re15_room.h"        /* g_current_room_id */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/feuerloescher_r37.inc"

/* ---- Die Lagen (PORT-WAHL, Messungen im Dossier Abschnitt "Lage") -------------------------------
 * ROOM1050 (Leon): Wand rechts der Notausgangs-Tuer zu ROOM1090 (Door_aot_set Slot 5 @0x00B7A,
 *   Rechteck x 18100..21600 z -23900..-22900), Cut 6. Wandebene z ~ -23900 (Tuerunterkante im
 *   Hintergrund ROOM1050 Cut 6 deckt sich mit der Projektion bei z -23900; die Kollisionszelle
 *   x 12700..26100 z -24350..-23377 liegt VOR der gemalten Wand). Unterkante 1150 = 0,68 m,
 *   Oberkante 1942 = 1,14 m ueber dem Boden (NFPA 10: Oberkante <= 1,53 m bei < 40 lb).
 *   Aufhebe-Rechteck x 16700..17700 z -23900..-22900: dieselbe z-Spanne wie die Tuer daneben (der
 *   Spieler steht vor der Kollisionskante z -23377, der Vorwaerts-Punkt 620 @0x80042bd0 liegt dann
 *   in z -23520..-23000); kein Ueberlapp mit Slot 5 (x >= 18100) und Slot 6 (x <= 16650).
 *   Slot 13: in ROOM1050 belegt 0..10 (Original), 15 (Dokument, re15_dokumente.h), 52.. Kamerazonen.
 * ROOM1090 (Leon, nach dem Loeschen UND weggeworfen) / ROOM1091 (Elza, immer): liegend auf dem Boden
 *   (Band 1 = y -1800, Feuer Sce_em_set @0x02214.. y 0xf8f8) vor der Seitentuer des Wagens, aus der die
 *   Frau in der Rettungsszene steigt (sub03 Ada-Start (2602,-2233) @0x02554), zwischen den Feuern
 *   (2052,-1334) @0x02214 und (2375,-2333) @0x02228 — dort stand Leon beim Loeschen (Viereck Slot 2
 *   @0x02336). (2500,-1300) liegt in Cut 4/5/6/7 im Bild und unter KEINER Maske (probe_r34n_e_dokumente
 *   `sicht`); verworfen: (700,-1300) Cut 4 Maske 21, (2300,0) Cut 5 Maske 15, (2100,600) in Cut 7 unter
 *   dem Bildrand, (2700,-1000)/(2900,-1200) Cut 5 Maske 14. Cut 7 = Hauptbild der Rettungsszene
 *   (Cut_chg F985 im Lauf), Cut 6 = Wagentuer ("There! Are you okay?"). rot_y 3250 = diagonal zu den
 *   Blickachsen von Cut 5 (Lot 3760) und Cut 6 (Lot 2733). Untersuchen-Platz 1000 x 1000 um das Prop;
 *   ROOM1091 Slot 3 (< Slot 5 = Schuppen-Text der Runde 36, Vorrang des kleinsten Slots FUN_80042bac;
 *   der Messpunkt der Runde 36 (620,250) liegt NICHT darin), ROOM1090 Slot 5 (nach dem Loeschen legt
 *   sub00 keinen Slot 2/3-Satz an). */
static const re15_fl37_lage_t k_lagen[] = {
    /* raum    art                tor                        slot zone band   x      y       z    rot_y  rect_x rect_z  w     d   */
    { 0x1050, RE15_FL37_STEHEND, RE15_FL37_NICHT_GENOMMEN,   13,  1,   0, 17200, -1150, -23775,    0, 16700, -23900, 1000, 1000 },
    { 0x1090, RE15_FL37_LIEGEND, RE15_FL37_FEUER_AUS,         5,  0,   1,  2500, -1800,  -1300, 3250,  2000,  -1800, 1000, 1000 },
    { 0x1091, RE15_FL37_LIEGEND, RE15_FL37_IMMER,             3,  0,   1,  2500, -1800,  -1300, 3250,  2000,  -1800, 1000, 1000 },
};
#define N_LAGEN ((int)(sizeof k_lagen / sizeof k_lagen[0]))

static int index_des_raums(uint16_t room_id)
{
    for (int k = 0; k < N_LAGEN; k++)
        if (room_id == k_lagen[k].raum) return k;
    return -1;
}

int re15_feuerloescher_r37_anzahl(void) { return N_LAGEN; }

const re15_fl37_lage_t *re15_feuerloescher_r37_zeile(int i)
{
    return (i >= 0 && i < N_LAGEN) ? &k_lagen[i] : NULL;
}

const re15_fl37_lage_t *re15_feuerloescher_r37_des_raums(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    return k >= 0 ? &k_lagen[k] : NULL;
}

static int tor_offen(const re15_fl37_lage_t *g)
{
    switch (g->tor) {
    case RE15_FL37_IMMER:          return 1;
    case RE15_FL37_FEUER_AUS:
        /* Feuer aus (3,0x81) UND Leon traegt den Loescher nicht mehr (Wegwerf-Abfrage "Ja" nach
         * "You've used the Fire Extinguisher." ROOM1090 msg 9, gen/discard_sites.inc) — sonst laege
         * derselbe Loescher zweimal da. PORT-WAHL, Dossier Abschnitt 3.3. */
        return re15_game_flag_get(RE15_FL37_STORY_BANK, RE15_FL37_FEUER_AUS_BIT) &&
               re15_inv_find_item(RE15_FL37_ITEM) < 0;
    case RE15_FL37_NICHT_GENOMMEN: return re15_game_flag_get(9, RE15_FL37_TAKEN_BIT) ? 0 : 1;
    default:                       return 0;
    }
}

int re15_feuerloescher_r37_liegt(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    return k < 0 ? -1 : tor_offen(&k_lagen[k]);
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

static void text_ablegen(void)
{
    re15_msg_install_text(RE15_FL37_MSG_LEER, k_fl_msg_leer, sizeof k_fl_msg_leer);
    /* Dauer-Eintrag wie fuer jede RDT-Nachricht (re15_msg_load_room_block; Muster story_r36.c). */
    int d = re15_msg_compute_duration(k_fl_msg_leer, sizeof k_fl_msg_leer, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations(RE15_FL37_MSG_LEER, d);
}

/* SPIND ROOM1000 (nur Leon): die Original-Aufnahme Slot 3 (@0x00C24) wird zum Raumtext msg 0
 * "Nothing unusual." — Satzform wie Slot 6 @0x00CAA `2c 06 01 31 00 00 .. 00 00 ff ff 00 00`
 * (sce 1, sat 0x31, Band 0, msg 0, Pause 0xFFFF), Rechteck der Original-Aufnahme (20500,-800) 800 x 800.
 * Gilt unabhaengig vom Taken-Bit: der Loescher liegt jetzt in ROOM1050. */
static void spind_umwidmen(void)
{
    const int s = RE15_FL37_SPIND_SLOT;
    /* Der Original-Satz ist ITEM — oder NONE, wenn das Taken-Bit schon steht (Port-Gegenstueck zu
     * `sb zero,0(v0)` @0x800406f4, scd_vm.c op_item_aot_set); in beiden Faellen traegt er Item 0x31. */
    if (g_aot.slots[s].active && !((g_aot.slots[s].type == RE15_AOT_TYPE_ITEM ||
                                    g_aot.slots[s].type == RE15_AOT_TYPE_NONE) &&
                                   g_aot.item_params[s].item_type == RE15_FL37_ITEM))
        return;                                   /* fremder Satz im Slot: nichts anfassen */
    re15_aot_set(s, RE15_AOT_TYPE_MESSAGE, RE15_FL37_SPIND_MSG, 20500 + 400, -800 + 400, 400, 400);
    g_aot.slots[s].sce_flags    = RE15_FL37_AOT_SAT;
    g_aot.slots[s].band         = 0;
    g_aot.slots[s].pause_mask16 = (uint16_t)RE15_FL37_PAUSE;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0\n");
#endif
}

void re15_feuerloescher_r37_install(uint16_t room_id)
{
    if (room_id == RE15_FL37_SPIND_RAUM) { spind_umwidmen(); return; }
    int k = index_des_raums(room_id);
    if (k < 0) return;
    const re15_fl37_lage_t *g = &k_lagen[k];
    if (!tor_offen(g)) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[feuerloescher] ROOM%04X: kein Loescher (%s)\n", (unsigned)room_id,
                g->tor != RE15_FL37_FEUER_AUS ? "schon genommen, (9,0x86)=1"
                : !re15_game_flag_get(RE15_FL37_STORY_BANK, RE15_FL37_FEUER_AUS_BIT) ? "Feuer brennt noch, (3,0x81)=0"
                : "Leon traegt ihn noch (Item 0x31 im Inventar)");
#endif
        return;
    }

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914), Item-Prop-Form ROOM1010 @0x00930 */
    if (slot_von_obj_id(RE15_FL37_OBJ) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = RE15_FL37_OBJ;
        g_scd.props[i].obj_type   = 0;                    /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_FL37_PROP_BAND;  /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                   /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = g->x;
        g_scd.props[i].y = g->y;
        g_scd.props[i].z = g->z;
        g_scd.props[i].rot_y = g->rot_y;
        g_scd.props[i].flags = RE15_FL37_PROP_FLAGS;      /* 0x000A | 1 @0x80040998         */
        g_scd.props[i].attr  = RE15_FL37_PROP_ATTR;       /* 0 = NCCT (@0x8002c2f4 Bit 0x10 aus) */
    }

    /* ZONE — nur in einen freien Slot (fremde Saetze bleiben unberuehrt). */
    if (g->slot >= RE15_AOT_MAX || g_aot.slots[g->slot].active) return;
    const int32_t cx = g->rect_x + g->rect_w / 2, cz = g->rect_z + g->rect_d / 2;
    if (g->zone_item) {
        /* AUFNAHME wie der Original-Satz @0x00C24 — Item 0x31 x1, Taken-Bit 0x86 — nur mit dem
         * Modell als tk_prop (@0x80040684), damit die Aufnahme es wegraeumt (@0x80021fc8). */
        re15_aot_set_item_tk_prop((int)g->slot, cx, cz, g->rect_w / 2, g->rect_d / 2,
                                  RE15_FL37_ITEM, RE15_FL37_MENGE, RE15_FL37_TAKEN_BIT, RE15_FL37_OBJ);
        g_aot.slots[g->slot].sce_flags = RE15_FL37_AOT_SAT;
        g_aot.slots[g->slot].band      = g->band;
    } else {
        /* UNTERSUCHEN: sce-1-Platz mit eigener Nachricht (Form story_r36.c text_platz). */
        text_ablegen();
        re15_aot_set((int)g->slot, RE15_AOT_TYPE_MESSAGE, RE15_FL37_MSG_LEER, cx, cz, g->rect_w / 2, g->rect_d / 2);
        g_aot.slots[g->slot].sce_flags    = RE15_FL37_AOT_SAT;
        g_aot.slots[g->slot].band         = g->band;
        g_aot.slots[g->slot].pause_mask16 = (uint16_t)RE15_FL37_PAUSE;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[feuerloescher] ROOM%04X: Loescher %s obj %d bei (%ld,%ld,%ld) rot_y %d, Zone Slot %d %s\n",
            (unsigned)room_id, g->art == RE15_FL37_STEHEND ? "an der Wand" : "liegt leer am Boden",
            RE15_FL37_OBJ, (long)g->x, (long)g->y, (long)g->z, (int)g->rot_y, g->slot,
            g->zone_item ? "Aufnahme Item 0x31 Bit (9,0x86)" : "Untersuchen msg 29");
#endif
}

void re15_feuerloescher_r37_genommen(uint8_t item_type, uint8_t taken_bit)
{
    /* ROOM1000 sub01 @0x00CFC-0x00D0C: Ck(9,0x86)==1 und Ck(3,0x85)==0 -> Set(3,0x85)=1. */
    if (item_type != RE15_FL37_ITEM || taken_bit != RE15_FL37_TAKEN_BIT) return;
    if (!re15_game_flag_get(9, RE15_FL37_TAKEN_BIT)) return;
    if (re15_game_flag_get(RE15_FL37_STORY_BANK, RE15_FL37_STORY_BIT)) return;
    re15_game_flag_set(RE15_FL37_STORY_BANK, RE15_FL37_STORY_BIT, 1);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C\n");
#endif
}

void re15_feuerloescher_r37_weggeworfen(uint8_t item_type)
{
    /* Wegwerf-Abfrage "Ja" (item_discard_common.c, re15_inv_remove_slot): der leere Loescher faellt
     * in ROOM1090 an seinen Platz — derselbe Installer wie beim Raumstart (Tor jetzt offen). */
    if (item_type != RE15_FL37_ITEM || g_current_room_id != 0x1090) return;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[feuerloescher] Loescher weggeworfen -> liegt in ROOM1090\n");
#endif
    re15_feuerloescher_r37_install((uint16_t)g_current_room_id);
}

int re15_feuerloescher_r37_obj_id(uint16_t room_id)
{
    return index_des_raums(room_id) >= 0 ? RE15_FL37_OBJ : -1;
}

const uint8_t *re15_feuerloescher_r37_md1(int art, int *out_size)
{
    if (art == RE15_FL37_STEHEND) { if (out_size) *out_size = (int)sizeof k_fl_md1_stehend; return k_fl_md1_stehend; }
    if (art == RE15_FL37_LIEGEND) { if (out_size) *out_size = (int)sizeof k_fl_md1_liegend; return k_fl_md1_liegend; }
    if (out_size) *out_size = 0;
    return NULL;
}

const uint8_t *re15_feuerloescher_r37_md1_bytes(uint16_t room_id, int *out_size)
{
    int k = index_des_raums(room_id);
    if (out_size) *out_size = 0;
    if (k < 0) return NULL;
    return re15_feuerloescher_r37_md1(k_lagen[k].art, out_size);
}

const uint8_t *re15_feuerloescher_r37_tim_bytes(uint16_t room_id, int *out_size)
{
    if (out_size) *out_size = 0;
    if (index_des_raums(room_id) < 0) return NULL;
    if (out_size) *out_size = (int)sizeof k_fl_tim;
    return k_fl_tim;
}

const uint8_t *re15_feuerloescher_r37_text(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_fl_msg_leer;
    return k_fl_msg_leer;
}
