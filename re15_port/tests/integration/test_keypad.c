/* ============================================================================
 *  STAGE1 keycard keypad (dial-combination lock) FSM — S1-4 / PROG-3 deep verify.
 *
 *  The card reader (sub20) fires Evt_exec(0x1811)->sub17 which starts a 4-digit
 *  DIAL combination lock. RE'd byte-true from room1230 (Red Keycard, code 5632):
 *    sub17  init:  Cut_chg(9) + Message_on(0) + Set(5,0/1/2,1) = keypad active.
 *    sub01  poll:  dpad rotates the dial (Member[15] via sub02 Add_speed); confirm
 *                  (Sce_key_ck 0x4000=SQUARE) at Member[15]==<value> -> Evt_exec the
 *                  matching digit sub. Also the WIN block:
 *                  If Ck(3,137,0)&Ck(5,13,1)&Ck(5,14,1)&Ck(5,15,1)&Ck(5,16,1)
 *                     -> Evt_exec(0x1813=sub19) + Set(3,137,1).
 *    digit subs (slot progression z5/7->8->5->4, each sets a correct-flag z5/13..16):
 *       sub10 (Ck 5,7,0 -> Set 5,7 5,8 5,13)   slot1 correct-flag z5/13
 *       sub11 (Ck 5,8,1 -> Set 5,5 5,14)        slot2 correct-flag z5/14
 *       sub08 (Ck 5,5,1 -> Set 5,4 5,15)        slot3 correct-flag z5/15
 *       sub07 (Ck 5,4,1 -> Set 5,16 5,20)       slot4 correct-flag z5/16
 *    sub19  win:   Aot_reset(6,sce=2 DOOR) installs the door live + Message_on(5) "used".
 *    sub16  reset: clears all z5 flags on a wrong entry.
 *
 *  This drives the REAL digit-sub + win bytecode (via scd_event_fire) in the correct
 *  slot sequence and asserts the FSM logic byte-true: each digit sub sets its
 *  correct-flag, and once all four are set the poll unlocks the door (Set(3,137,1)),
 *  after which main00 installs the gated slot as a real DOOR. (The dpad DIAL input
 *  layer -- Member[15] rotation + confirm -- is the input feeding these subs; its
 *  opcodes op_sce_key_ck/op_member_cmp are byte-true, see notes in scd_vm.c.)
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define RDT_MIN_SIZE 0x60
#define GET(z,i) re15_game_flag_get((z),(i))

static const char *ASSETS;
static re15_rdt_t g_rdt;
static uint8_t *g_buf;

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

/* enter room1230 with the keycard-layout reached (z4/243=1) + Red Keycard held (z9/136=1) */
static void enter_1230(void)
{
    scd_vm_init();
    g_current_room_id = 0x1230;
    re15_game_flag_set(4, 243, 1);   /* outer room-state gate -> keycard layout installs */
    re15_game_flag_set(9, 136, 1);   /* Red Keycard possession (reader Ck(9,136,1)) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
}

/* fire sub_scd[ev] and tick it to completion, clearing event slots afterwards */
static void fire(int ev, int ticks)
{
    scd_event_fire((uint8_t)ev);
    for (int f = 0; f < ticks; f++) scd_vm_tick();
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) g_scd.threads[s].active = 0;
}

int main(void)
{
    ASSETS = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== PROG-3 keypad FSM (room1230 Red Keycard dial lock) ===\n");
    g_buf = read_room(0x1230, &g_rdt);
    if (!g_buf) { fprintf(stderr, "FAIL: cannot load ROOM1230\n"); return 1; }

    int fail = 0;
    enter_1230();

    /* init: the reader fires sub17 (keypad start). */
    fire(17, 6);
    printf("  sub17 init:   z5/0=%d z5/1=%d z5/2=%d (keypad active)\n", GET(5,0), GET(5,1), GET(5,2));
    if (!(GET(5,0) && GET(5,1) && GET(5,2))) { fprintf(stderr, "FAIL: sub17 did not activate the keypad\n"); fail = 1; }

    /* drive the 4 correct digit confirms in slot order (each sub's precondition is set
     * by the previous): sub10 -> sub11 -> sub08 -> sub07 sets z5/13,14,15,16.
     * Tick only 2 frames per confirm: each digit sub sets its flags in the FIRST burst,
     * then Sleep(2)s before its goto Sub16 reset tail (which the real game skips because
     * the concurrent sub01 win-poll unlocks the door during that sleep). Killing the sub
     * mid-sleep models "the win fires before the reset" for this isolated drive. */
    re15_game_flag_set(5, 7, 0);   /* slot-1 initial state (sub10 needs Ck(5,7,0)) */
    fire(10, 2); printf("  sub10 slot1:  z5/13=%d (need 1)  [z5/7=%d z5/8=%d]\n", GET(5,13), GET(5,7), GET(5,8));
    fire(11, 2); printf("  sub11 slot2:  z5/14=%d (need 1)  [z5/5=%d]\n", GET(5,14), GET(5,5));
    fire( 8, 2); printf("  sub08 slot3:  z5/15=%d (need 1)  [z5/4=%d]\n", GET(5,15), GET(5,4));
    fire( 7, 2); printf("  sub07 slot4:  z5/16=%d (need 1)  [z5/20=%d]\n", GET(5,16), GET(5,20));
    if (!(GET(5,13) && GET(5,14) && GET(5,15) && GET(5,16))) {
        fprintf(stderr, "FAIL: the 4 digit subs did not set all correct-flags z5/13..16\n"); fail = 1;
    }

    /* win: the poll sub01 sees all 4 correct-flags + door still locked -> Set(3,137,1). */
    printf("  before win:   z3/137=%d (door unlock flag, should be 0)\n", GET(3,137));
    fire(1, 4);
    printf("  sub01 win:    z3/137=%d (should be 1 = UNLOCKED)\n", GET(3,137));
    if (!GET(3,137)) { fprintf(stderr, "FAIL: keypad win did not set the unlock flag z3/137\n"); fail = 1; }

    /* the door: re-run main00 with the unlock flag set -> gated slot installs as a real DOOR. */
    enter_1230();                       /* z3/137 persists in g_game across the reenter */
    /* re-apply the unlock we just proved (enter_1230 does not clear it -- scd_vm_init does,
     * so set it again to model "the flag is saved" and re-run main00) */
    re15_game_flag_set(3, 137, 1);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    int door_ty = g_aot.slots[4].active ? (int)g_aot.slots[4].type : -2;
    printf("  main00 door:  slot4 type=%d (%s)\n", door_ty,
           door_ty == RE15_AOT_TYPE_DOOR ? "DOOR open" : door_ty == RE15_AOT_TYPE_MESSAGE ? "MESSAGE locked" : "?");
    if (door_ty != RE15_AOT_TYPE_DOOR) { fprintf(stderr, "FAIL: unlocked door did not install as DOOR\n"); fail = 1; }

    /* ---- Part 2: the dial INPUT read path (cursor over grid cell -> notch -> prop member -> confirm) ----
     * The dpad moves the cursor OBJECT; each frame its member+0xb (notch) = the sce=5 grid-cell slot
     * it is over (byte-true FUN_80042bac @0x80042f5c). The confirm reads it via Work_set(3,0) +
     * Member_cmp(15==notch). Pre-fix the port returned 0 for a prop work entity, so no confirm fired. */
    printf("\n  -- dial input read path (grid-cell notch -> prop member_0b -> Member_cmp) --\n");
    enter_1230();                       /* fresh (z3/137=0 locked) -> sce=5 grid cells + dial prop install */

    int cell_slot = -1; int32_t cx = 0, cz = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_EXAMINE_WORKVAR) {
            cell_slot = i; cx = g_aot.slots[i].x; cz = g_aot.slots[i].z; break;
        }
    printf("  grid cells: dial prop_count=%d, first sce=5 cell = slot %d @ (%d,%d)\n",
           g_scd.prop_count, cell_slot, cx, cz);
    if (cell_slot < 0) { fprintf(stderr, "FAIL: no sce=5 grid cell installed for the keypad\n"); fail = 1; }
    else {
        /* (a) the AOT scan's notch = the grid-cell slot the cursor is over */
        int notch = re15_aot_object_notch(cx, cz);
        printf("  re15_aot_object_notch(cell) = %d (expect slot %d)\n", notch, cell_slot);
        if (notch != cell_slot) { fprintf(stderr, "FAIL: notch != the grid-cell slot the cursor is over\n"); fail = 1; }
        /* off the grid -> no notch */
        if (re15_aot_object_notch(1<<20, 1<<20) != -1) { fprintf(stderr, "FAIL: notch set while off the grid\n"); fail = 1; }

        /* (b) the per-frame object pass sets the cursor prop's member_0b = the cell it's over
         * (this is what re15_game_step calls each frame so the dpad-moved cursor's notch is live) */
        g_scd.props[0].member_0b = 0;
        g_scd.props[0].x = cx; g_scd.props[0].z = cz;   /* place the dial cursor on the cell */
        re15_object_notch_update();
        printf("  re15_object_notch_update: prop0.member_0b = %d (expect %d)\n", g_scd.props[0].member_0b, cell_slot);
        if (g_scd.props[0].member_0b != (uint8_t)cell_slot) { fprintf(stderr, "FAIL: per-frame notch pass did not set the cursor notch\n"); fail = 1; }
        if (re15_prop_get_member(0, 15) != notch) { fprintf(stderr, "FAIL: prop member 15 (notch) round-trip\n"); fail = 1; }

        /* (c) the confirm reads it: Work_set(3,0) binds the dial prop, Member_cmp(15==notch) is TRUE
         * (was always false pre-fix). Fragment: Work_set; If Member_cmp(15==notch){ Set(6,1,1) } EndIf. */
        const uint8_t NN = (uint8_t)notch;
        const uint8_t frag_hit[] = {
            0x2E, 0x03, 0x00,               /* Work_set(3,0) -> bind dial prop 0 */
            0x06, 0x00, 12, 0x00,           /* If (block_length 12 -> past EndIf)  */
            0x3E, 0x00, 0x0F, 0x00, NN, 0x00, /* Member_cmp(member15 == notch)     */
            0x22, 0x06, 0x01, 0x01,         /* Set(6,1,1) witness                  */
            0x08, 0x00,                     /* EndIf                               */
            0x02,                           /* Evt_next                            */
        };
        re15_game_flag_set(6, 1, 0);
        g_scd.threads[SCD_EVENT_SLOT_FIRST].active = 0;
        scd_thread_start(SCD_EVENT_SLOT_FIRST, frag_hit);
        scd_vm_tick();
        int hit = re15_game_flag_get(6, 1);
        printf("  Work_set(3,0)+Member_cmp(15==%d) -> witness=%d (expect 1 = confirm accepts)\n", notch, hit);
        if (!hit) { fprintf(stderr, "FAIL: dial confirm Member_cmp did not read the prop notch\n"); fail = 1; }

        /* control: a WRONG notch must NOT confirm */
        const uint8_t WN = (uint8_t)(notch == 5 ? 6 : 5);
        uint8_t frag_miss[sizeof frag_hit];
        memcpy(frag_miss, frag_hit, sizeof frag_hit);
        frag_miss[11] = WN;                 /* Member_cmp value = a different notch */
        re15_game_flag_set(6, 1, 0);
        g_scd.threads[SCD_EVENT_SLOT_FIRST].active = 0;
        scd_thread_start(SCD_EVENT_SLOT_FIRST, frag_miss);
        scd_vm_tick();
        if (re15_game_flag_get(6, 1)) { fprintf(stderr, "FAIL: confirm accepted a WRONG notch\n"); fail = 1; }
    }

    free(g_buf);
    if (fail) { printf("KEYPAD: FAIL\n"); return 1; }
    printf("KEYPAD: dial FSM + INPUT byte-true — cursor notch=grid slot -> prop member_0b -> confirm accepts;\n"
           "        4 confirms set z5/13..16, poll unlocks z3/137, door opens\n");
    return 0;
}
