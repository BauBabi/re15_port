/*
 * re15_savepoint.c — the phone save-point registry + pending signal. See
 * re15_savepoint.h. The SCD Message_on handler (scd_vm.c) calls
 * re15_savepoint_is() and, on a match, sets the pending signal the platform
 * consumes to open the save flow.
 */
#include "re15_savepoint.h"

/* The 14 phone save-points: {room, main<NN> message id}. RE-verified as the set
 * of rooms whose message data carries "You can save your progress"; each has a
 * Leon room (even id) and an Elza mirror (odd id). */
static const struct { unsigned room; uint8_t msg; } s_savepoints[] = {
    { 0x1070, 0x14 }, { 0x1071, 0x14 },   /* STAGE1 main20 */
    { 0x1120, 0x06 }, { 0x1121, 0x06 },   /* STAGE1 main06 */
    { 0x1150, 0x01 }, { 0x1151, 0x01 },   /* STAGE1 main01 */
    { 0x2010, 0x03 }, { 0x2011, 0x03 },   /* STAGE2 main03 */
    { 0x30A0, 0x01 }, { 0x30A1, 0x01 },   /* STAGE3 main01 */
    { 0x30B0, 0x01 }, { 0x30B1, 0x01 },   /* STAGE3 main01 */
    { 0x4010, 0x2B },                     /* STAGE4 main43 */
    { 0x4011, 0x07 },                     /* STAGE4 main07 */
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
