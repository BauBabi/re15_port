/* ============================================================================
 *  STAGE1 keycard door: unlock flag flips the gated slot MESSAGE<->DOOR — S1-4 / PROG-4.
 *
 *  The byte-true chain (RE'd + adversarially verified, wf_c78a98ec):
 *    pick up a keycard  -> the pickup modal sets the ZONE-9 possession flag
 *                          (item_modal_common.c:241  re15_game_flag_set(9, taken_bit, 1))
 *    examine the reader -> sub20:  Message_on(prompt)
 *                          -> Ck(12,31,0)   (the Yes/No answer latch)
 *                          -> Ck(9, <card_bit>, 1)   (0x21 op_ck — the "do you have the card?" test)
 *                             has card -> "You've used the X" + Evt_exec(0x1811) keypad -> Set(3, <door>, 1)
 *                             no card  -> "You have not the X"
 *    the door           -> main00:  if(Ck(3,<door>,0)) Aot_set(slot,sce=1)=MESSAGE(locked)
 *                                    else               Door_aot_set(slot,sce=2)=DOOR(open)
 *
 *  Every op on that path is already byte-true in the port (op_ck 0x21 @scd_vm.c:1568,
 *  op_set 0x22, the pickup-modal zone-9 write) — there is NO scd_vm.c gap for the
 *  Blue/Yellow/Red keycard doors (0x5E op_keep_item_ck is a stub but is NOT on this path).
 *  So this pins the LIVE behaviour on the REAL room RDTs: the gated slot installs as
 *  MESSAGE while the unlock flag is clear and as DOOR once it is set — i.e. "using the
 *  correct key (which sets the flag) makes the door open".
 *
 *  Table (verified against the disassembled main00 + raw main00.scd + the runtime census):
 *    room10D0  Blue   Keycard  door flag z3/50   gated slot 0
 *    room11E0  Yellow Keycard  door flag z3/139  gated slot 3
 *    room1230  Red    Keycard  door flag z3/137  gated slot 4   (behind outer Ck(4,243) room-state gate)
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"        /* scd_vm_*, re15_game_flag_set/get */
#include "re15_aot.h"        /* g_aot, RE15_AOT_TYPE_DOOR(1) / _MESSAGE(5) */
#include "re15_actor.h"
#include "re15_room.h"       /* g_current_room_id */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define RDT_MIN_SIZE 0x60

static const char *ASSETS;

static uint8_t *read_room(unsigned rid, re15_rdt_t *rdt)
{
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", ASSETS, rid);
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz < RDT_MIN_SIZE) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f);
    if (re15_rdt_parse(buf, (size_t)sz, rdt) != 0) { free(buf); return NULL; }
    return buf;
}

/* Load a room and run main00 with the door unlock flag preset to `uval` (and, for
 * ROOM1230, its outer room-state gate preset). Return the gated AOT slot's type. */
static int install_with_flag(unsigned rid, int gated_slot,
                             uint8_t uz, uint8_t ui, int uval,
                             uint8_t pz, uint8_t pi, int pval)
{
    re15_rdt_t rdt;
    uint8_t *buf = read_room(rid, &rdt);
    if (!buf) { fprintf(stderr, "FAIL: cannot load ROOM%04X\n", rid); return -1; }
    scd_vm_init();                                /* resets g_game.flags + AOTs + VM */
    g_current_room_id = rid;
    if (pz != 0xFF) re15_game_flag_set(pz, pi, pval);   /* outer room-state gate (1230) */
    re15_game_flag_set(uz, ui, uval);                   /* the door unlock flag */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, /*entry_scenario=*/0);  /* runs main00 -> installs the gated AOT */
    int ty = (gated_slot >= 0 && gated_slot < RE15_AOT_MAX && g_aot.slots[gated_slot].active)
             ? (int)g_aot.slots[gated_slot].type : -2;
    free(buf);
    return ty;
}

static const char *tyname(int t)
{
    return t == RE15_AOT_TYPE_DOOR ? "DOOR(open)" :
           t == RE15_AOT_TYPE_MESSAGE ? "MESSAGE(locked)" :
           t == -2 ? "inactive" : "other";
}

/* Part 2 — the ACTUAL PROG-3 gap: the card reader lives in sub20 (event id 20 from the
 * examine Aot_set(_,3,..,20,..)). RE15_RDT_MAX_SUB_SCD was 16, so the sub table was
 * TRUNCATED and scd_event_fire(20)/Evt_exec(20) returned -1 — the reader never ran, so the
 * unlock flag (Set(3,idx,1)) never fired and the door could NOT be opened. Raised to 32
 * (max STAGE1-6 sub count = 28). This asserts the reader sub is now stored + dispatchable. */
static int reader_reachable(unsigned rid, int reader_event, int min_subs)
{
    re15_rdt_t rdt;
    uint8_t *buf = read_room(rid, &rdt);
    if (!buf) { fprintf(stderr, "FAIL: cannot load ROOM%04X\n", rid); return 1; }
    scd_vm_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);                 /* sets s_current_rdt for scd_event_fire */
    int stored = (reader_event < RE15_RDT_MAX_SUB_SCD) && (rdt.sub_scd[reader_event] != NULL);
    int slot   = scd_event_fire((uint8_t)reader_event);   /* >=0 = dispatchable, -1 = out of range/NULL */
    printf("  ROOM%04X sub_scd_count=%2d  sub[%d]=%-7s  scd_event_fire(%d)=%s\n",
           rid, rdt.sub_scd_count, reader_event, stored ? "present" : "NULL",
           reader_event, slot >= 0 ? "dispatched" : "-1 (UNREACHABLE)");
    int bad = (rdt.sub_scd_count < min_subs) || !stored || slot < 0;
    free(buf);
    return bad ? 1 : 0;
}

struct kd { const char *name; unsigned rid; int slot; uint8_t z, i; uint8_t pz, pi; int pv; };

int main(void)
{
    ASSETS = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== PROG-4 keycard doors: unlock flag flips gated slot MESSAGE<->DOOR (real RDTs) ===\n");

    /* pz=0xFF -> no outer gate; ROOM1230's keycard layout lives in the else of Ck(4,243),
     * i.e. it installs when z4/243 == 1 (the room-state that exposes the weapon-storage door). */
    struct kd doors[] = {
        { "ROOM10D0 Blue Keycard  (z3/50)",  0x10D0, 0, 3, 50,  0xFF, 0, 0 },
        { "ROOM11E0 Yellow Keycard(z3/139)", 0x11E0, 3, 3, 139, 0xFF, 0, 0 },
        { "ROOM1230 Red Keycard   (z3/137)", 0x1230, 4, 3, 137, 4,   243, 1 },
    };

    int fail = 0;
    for (unsigned d = 0; d < sizeof doors / sizeof doors[0]; d++) {
        struct kd *k = &doors[d];
        int locked = install_with_flag(k->rid, k->slot, k->z, k->i, 0, k->pz, k->pi, k->pv);
        int opened = install_with_flag(k->rid, k->slot, k->z, k->i, 1, k->pz, k->pi, k->pv);
        printf("  %-34s flag=0 -> slot%d %-16s | flag=1 -> slot%d %s\n",
               k->name, k->slot, tyname(locked), k->slot, tyname(opened));
        if (locked != RE15_AOT_TYPE_MESSAGE) {
            fprintf(stderr, "FAIL: %s: unlock flag CLEAR should install MESSAGE (locked), got %s\n",
                    k->name, tyname(locked)); fail = 1;
        }
        if (opened != RE15_AOT_TYPE_DOOR) {
            fprintf(stderr, "FAIL: %s: unlock flag SET should install DOOR (open), got %s\n",
                    k->name, tyname(opened)); fail = 1;
        }
    }

    /* Part 2 — reader-sub reachability (the RE15_RDT_MAX_SUB_SCD 16->32 gap fix). */
    printf("\n  -- card reader (sub20) reachability (was truncated by the 16-sub cap) --\n");
    fail |= reader_reachable(0x10D0, 20, 21);   /* Blue   reader = event 20, room has 23 subs */
    fail |= reader_reachable(0x11E0, 20, 21);   /* Yellow reader = event 20, room has 28 subs */
    fail |= reader_reachable(0x1230, 20, 21);   /* Red    reader = event 20, room has 23 subs */

    if (fail) { printf("KEYDOOR: FAIL\n"); return 1; }
    printf("KEYDOOR: 3 keycard doors flip MESSAGE->DOOR on the unlock flag; card readers (sub20) reachable\n");
    return 0;
}
