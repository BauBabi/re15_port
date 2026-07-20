/*
 * re15_savepoint.c — the phone save-point registry + pending signal. See
 * re15_savepoint.h. The SCD Message_on handler (scd_vm.c) calls
 * re15_savepoint_is() and, on a match, sets the pending signal the platform
 * consumes to open the save flow.
 */
#include "re15_savepoint.h"

/* The 16 phone/computer save-points: {room, main<NN> message id}. This is the
 * COMPLETE set - every room + message index game-wide whose .msg body decodes to
 * "You can save your progress with this. Save is not available in this preview"
 * (exhaustive scan of all room RDT message blocks: RDT+0x3c table, atlas decode
 * char = byte + 0x24). Each has a Leon room (even id) + an Elza mirror (odd id).
 * (Was 14 - STAGE5 5010/5011 were missing, so their computers showed the dormant
 * flavor message instead of the save menu; verified byte-true 2026-07-20.) */
static const struct { unsigned room; uint8_t msg; } s_savepoints[] = {
    { 0x1070, 0x14 }, { 0x1071, 0x14 },   /* STAGE1 main20 */
    { 0x1120, 0x06 }, { 0x1121, 0x06 },   /* STAGE1 main06 */
    { 0x1150, 0x01 }, { 0x1151, 0x01 },   /* STAGE1 main01 */
    { 0x2010, 0x03 }, { 0x2011, 0x03 },   /* STAGE2 main03 */
    { 0x30A0, 0x01 }, { 0x30A1, 0x01 },   /* STAGE3 main01 */
    { 0x30B0, 0x01 }, { 0x30B1, 0x01 },   /* STAGE3 main01 (examine routes via sce=0 work-var AOT - deferred) */
    { 0x4010, 0x2B },                     /* STAGE4 main43 */
    { 0x4011, 0x07 },                     /* STAGE4 main07 */
    { 0x5010, 0x06 }, { 0x5011, 0x2D },   /* STAGE5 main06 / main45 */
};

int re15_savepoint_is(unsigned room_id, uint8_t msg_id)
{
    for (unsigned i = 0; i < sizeof(s_savepoints) / sizeof(s_savepoints[0]); i++)
        if (s_savepoints[i].room == room_id && s_savepoints[i].msg == msg_id)
            return 1;
    return 0;
}

static int s_savepoint_pending = 0;
int  re15_savepoint_pending(void)        { return s_savepoint_pending; }
void re15_savepoint_set_pending(int on)  { s_savepoint_pending = on ? 1 : 0; }

/* Re-examine debounce (see the header). Reinstates the original's msg_block-based
 * re-examine block for the port's menu-instead-of-message save-point. */
static int s_savepoint_cooldown = 0;
void re15_savepoint_set_cooldown(int frames) { s_savepoint_cooldown = frames > 0 ? frames : 0; }
void re15_savepoint_cooldown_tick(void)      { if (s_savepoint_cooldown > 0) s_savepoint_cooldown--; }
int  re15_savepoint_cooling(void)            { return s_savepoint_cooldown > 0; }

/* Gameplay cut latched at the examine action (see the header). */
static int s_savepoint_cut = -1;
void re15_savepoint_set_cut(int cut)  { s_savepoint_cut = cut; }
int  re15_savepoint_saved_cut(void)   { return s_savepoint_cut; }

void re15_savepoint_reset(void) { s_savepoint_cooldown = 0; s_savepoint_pending = 0; s_savepoint_cut = -1; }
