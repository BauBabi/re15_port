/*
 * RE1.5 Rebuilt — Runde 37 Spur D "itemband": Band-Gate fuer Item-Saetze + Member 0x12 = Kollisionsband.
 * Belege und Zusammenhang: include/re15_itemband_r37.h, Dossier analysis/befunde_runde37/D_itemband.md.
 *
 * Haken (je 1 Aufruf + Kommentar "Runde 37 Spur D"):
 *   engine/src/aot_common.c   re15_aot_scan    -> re15_itemband_r37_gate()          (@0x80042cac-ccc)
 *   engine/src/actor_common.c re15_actor_set_member case 18 -> re15_itemband_r37_member_band() (@0x80041228)
 *   engine/src/aot_common.c   re15_aot_scan, GENERIC-Feuer -> re15_itemband_r37_liftsperre() (Nachbesserung 1,
 *                             PORT-WAHL nach RE2 ROOM6030 sub10 @0x02F70-7A)
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

/* ---------------------------------------------------------------------------------------------------------------
 * Nachbesserung 1: Plattform-Sperre Lift A (PORT-WAHL nach RE2 ROOM6030, Belege re15_itemband_r37.h (3)).
 * Die Ausloeser-Saetze, wie sie ausgeliefert sind (tools/scd_dump_room.py, ROOM2031 an denselben Offsets byte-gleich):
 *   sub00 @0x01CE6 `2c 05 03 31 03 00 da 16 56 2c dc 05 dc 05 ff 00 18 02 00 00` slot 5, floor 3 (Kabine unten, auf)
 *   sub00 @0x01D20 `2c 04 03 31 06 00 da 16 56 2c dc 05 dc 05 ff 00 18 02 00 00` slot 4, floor 6 (Kabine oben, ab;
 *         nach jeder Fahrt neu scharf ueber sub02 @0x01E6E / @0x01EB0 `46 0? 03 31 ff 00 18 02 00 00`, Rechteck bleibt)
 * Rechteck = Satzbytes +6..+13 (x 5850, z 11350, w 1500, d 1500), Unterprogramm = Satzbyte +17 (2). Gesperrt wird nur
 * dieser Satz: Raum, Slot, Unterprogramm UND Rechteck muessen stimmen. */
typedef struct { uint16_t raum; uint8_t slot; uint8_t sub; int16_t x, z, w, d; } liftsperre_t;
static const liftsperre_t k_liftsperre[] = {
    { 0x2030, 5, 2, 5850, 11350, 1500, 1500 },   /* sub00 @0x01CE6 */
    { 0x2030, 4, 2, 5850, 11350, 1500, 1500 },   /* sub00 @0x01D20 */
    { 0x2031, 5, 2, 5850, 11350, 1500, 1500 },   /* ROOM2031 (Elza) sub00 @0x01CE6 */
    { 0x2031, 4, 2, 5850, 11350, 1500, 1500 },   /* ROOM2031 (Elza) sub00 @0x01D20 */
};
static int s_liftsperre_zahl = 0;

static const liftsperre_t *liftsperre_satz(unsigned raum, int slot)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return NULL;
    const re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_GENERIC || a->has_quad) return NULL;
    for (size_t k = 0; k < sizeof k_liftsperre / sizeof k_liftsperre[0]; k++) {
        const liftsperre_t *L = &k_liftsperre[k];
        if (raum != L->raum || slot != L->slot || a->event_id != L->sub) continue;
        /* op_aot_set speichert Mitte + halbe Ausdehnung (scd_vm.c: cx = x + w/2, hw = w/2). */
        if (a->x != (int32_t)L->x + L->w / 2 || a->z != (int32_t)L->z + L->d / 2 ||
            a->half_w != L->w / 2 || a->half_h != L->d / 2) continue;
        return L;
    }
    return NULL;
}

int re15_itemband_r37_ist_liftsperre(unsigned raum, int slot) { return liftsperre_satz(raum, slot) != NULL; }

int re15_itemband_r37_liftsperre(int slot, int mitte)
{
    const liftsperre_t *L = liftsperre_satz((unsigned)g_current_room_id, slot);
    if (!L) return 0;
    const int gesperrt = !mitte;               /* RE2 6030 sub10 @0x02F74 `21 05 22 00` -> @0x02F78 Evt_end */
    if (gesperrt) s_liftsperre_zahl++;
    FILE *f = itemband_log();
    if (f) {
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        fprintf(f, "F%u room=%04X LIFT slot %d sub %u spieler=(%d,%d,%d) band %d mitte im Ausloeser %s -> %s\n",
                (unsigned)g_engine.frame_count, g_current_room_id, slot, (unsigned)L->sub, (int)pl->x, (int)pl->y,
                (int)pl->z, re15_collision_debug_band(), mitte ? "ja" : "nein",
                gesperrt ? "GESPERRT (RE2 6030 sub10 @0x02F78)" : "FAEHRT");
        fflush(f);
    }
    return gesperrt;
}

int re15_itemband_r37_gesperrt_zahl(void)   { return s_gesperrt_zahl; }
int re15_itemband_r37_member_zahl(void)     { return s_member_zahl; }
int re15_itemband_r37_liftsperre_zahl(void) { return s_liftsperre_zahl; }
