/*
 * RE1.5 Rebuilt — Runde 37 Spur D "itemband": Band-Gate fuer Item-Saetze + Member 0x12 = Kollisionsband.
 * Belege und Zusammenhang: include/re15_itemband_r37.h, Dossier analysis/befunde_runde37/D_itemband.md.
 *
 * Haken (je 1 Aufruf + Kommentar "Runde 37 Spur D"):
 *   engine/src/aot_common.c   re15_aot_scan    -> re15_itemband_r37_gate()          (@0x80042cac-ccc)
 *   engine/src/actor_common.c re15_actor_set_member case 18 -> re15_itemband_r37_member_band() (@0x80041228)
 */
#include <stdio.h>
#include <stdlib.h>

#include "re15_itemband_r37.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_engine.h"
#include "re15_room.h"

static int   s_log_init = 0;
static FILE *s_log = NULL;
static int   s_gesperrt_zahl = 0;
static int   s_member_zahl = 0;

static FILE *itemband_log(void)
{
    if (!s_log_init) {
        s_log_init = 1;
        const char *p = getenv("RE15_ITEMBAND_LOG");
        if (p && *p) s_log = fopen(p, "w");
    }
    return s_log;
}

int re15_itemband_r37_gate(int slot, int *reach, int *inside)
{
    if (slot < 0 || slot >= RE15_AOT_MAX || !reach || !inside) return 0;
    const re15_aot_t *a = &g_aot.slots[slot];
    if (a->type != RE15_AOT_TYPE_ITEM) return 0;
    /* @0x80042cc0 `lbu v1,130(s1)`: das Band des Spielers = 0x800ACAD6 = s_coll_band. Wie bei den uebrigen
     * Typen (aot_common.c, GENERIC-Gate) nur bei etabliertem Band — im Spiel setzt jeder Raumaufbau es aus der
     * Ankunfts-Y (room_common.c Schritt 13), es ist dort nie -1. */
    const int pb = re15_collision_debug_band();
    const int gesperrt = (pb >= 0) &&
                         !(a->band & RE15_R37D_BAND_EGAL) &&      /* @0x80042cb4-cb8 */
                         ((int)a->band != pb);                     /* @0x80042cc4-ccc */
    FILE *f = itemband_log();
    if (f && g_aot_action_pressed && *reach) {
        const re15_aot_item_params_t *ip = &g_aot.item_params[slot];
        fprintf(f, "F%u room=%04X ITEM slot %d floor %u item 0x%02X tk %u spieler-band %d spieler=(%d,%d,%d) -> %s\n",
                (unsigned)g_engine.frame_count, g_current_room_id, slot, (unsigned)a->band,
                (unsigned)ip->item_type, (unsigned)ip->taken_bit, pb,
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y,
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                gesperrt ? "GESPERRT (@0x80042ccc bne Band)" : "FREI");
        fflush(f);
    }
    if (gesperrt) {
        if (g_aot_action_pressed && *reach) s_gesperrt_zahl++;
        *reach = 0;
        *inside = 0;
    }
    return gesperrt;
}

void re15_itemband_r37_member_band(int slot, int32_t value)
{
    if (slot != RE15_ACTOR_SLOT_PLAYER) return;            /* nur die Spieler-Entity 0x800ACA54 traegt 0x800ACAD6 */
    const int alt = re15_collision_debug_band();
    const int neu = (int)(uint8_t)value;                    /* `sb a2,130(a0)` @0x80041228: das untere Byte */
    re15_collision_set_band(neu);                           /* s_coll_band UND g_actors[0].floor */
    s_member_zahl++;
    FILE *f = itemband_log();
    if (f) {
        fprintf(f, "F%u room=%04X MEMBER 0x12 Spieler: Kollisionsband %d -> %d (@0x80041228) y=%d\n",
                (unsigned)g_engine.frame_count, g_current_room_id, alt, neu,
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y);
        fflush(f);
    }
}

int re15_itemband_r37_gesperrt_zahl(void) { return s_gesperrt_zahl; }
int re15_itemband_r37_member_zahl(void)   { return s_member_zahl; }
