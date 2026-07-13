/*
 * re15_savepoint.h — the in-room PHONE save-points (FE-4 Phase 3).
 *
 * RE1.5's save-points are telephones: examining one runs a Message_on showing
 * "It's a phone. You can save your progress with this. Save is not available in
 * this preview." (the prototype disabled the actual save). Per the port mandate
 * these become working save-points (RE2's typewriter role). A save-point is
 * identified byte-true by the (room, message-id) pair of that phone message —
 * message-id == the room's main<NN>.msg index (voice/text both key off it).
 *
 * The 14 phone locations (RE-verified: every room whose message data contains
 * "You can save your progress"), Leon room + Elza mirror (odd id):
 *   STAGE1 1070/1071 main20 · 1120/1121 main06 · 1150/1151 main01
 *   STAGE2 2010/2011 main03
 *   STAGE3 30A0/30A1 main01 · 30B0/30B1 main01
 *   STAGE4 4010 main43 · 4011 main07
 */
#ifndef RE15_SAVEPOINT_H
#define RE15_SAVEPOINT_H

#include <stdint.h>

/* 1 if a Message_on of `msg_id` in `room_id` is a phone save-point message. */
int re15_savepoint_is(unsigned room_id, uint8_t msg_id);

/* Set by the SCD Message_on handler when a save-point phone is examined; the
 * platform polls it to open the save flow (gated on the MEMORY CARD item), then
 * clears it. One-shot request, like the boot-movie gate. */
int  re15_savepoint_pending(void);
void re15_savepoint_set_pending(int on);

#endif /* RE15_SAVEPOINT_H */
