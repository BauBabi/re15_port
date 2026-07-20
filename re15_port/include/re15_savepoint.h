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

/* Re-examine debounce. BYTE-TRUE rationale: in the original, examining the phone
 * shows Message_on(msg) → message_display_frames>0 → msg_block (player_common.c:306)
 * → the examine AOT is gated on !msg_block (aot_common.c:573), so the phone cannot
 * re-fire while its message is on screen (the default message duration is 90 frames,
 * scd_vm.c msg_show `dur = 90`). The port shows the save MENU instead of that message,
 * so it carries no message_display_frames — WITHOUT this cooldown every fresh action
 * press while standing on the phone re-opens the menu (the "menu↔room flicker" bug).
 * The cooldown reinstates exactly that debounce window, but scoped to the re-examine
 * (it does NOT gate movement — the modal menu already served as the interaction, and
 * the user explicitly rejected a post-save movement freeze). Set on menu close, ticked
 * once per game frame, checked by the save-point Message_on paths. */
void re15_savepoint_set_cooldown(int frames);   /* arm the debounce (frames > 0) */
void re15_savepoint_cooldown_tick(void);        /* decrement once per game frame */
int  re15_savepoint_cooling(void);              /* 1 while the debounce is active */

/* Clear the pending request + cooldown. Called on a room change so a save in the
 * previous room can't suppress (cooldown) or spuriously fire (stale pending) a
 * save-point in the newly-entered room. */
void re15_savepoint_reset(void);

#endif /* RE15_SAVEPOINT_H */
