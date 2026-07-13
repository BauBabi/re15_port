/*
 * re15_savedata.h — the game-state save block (FE-4 memory-card save/load).
 *
 * RE1.5's own memory-card system is fully coded but DORMANT (dev-only overlay,
 * 8 MB-devkit buffer, its enable gate bit is never set) — so per the port
 * mandate we port RE2's (retail, fully wired) save system, adapted to RE1.5:
 *   - save-point   = the in-room PHONE (RE1.5's "You can save your progress with
 *                    this" object; RE2's typewriter role),
 *   - save item    = "MEMORY CARD" (item id 0x21; RE1.5's ink-ribbon role),
 *   - on-card file = "BISLPS-0022200"+slot (RE1.5's own SLPS-00222 product code),
 *   - card format  = byte-true PSX memory card (see re15_memcard.h).
 *
 * This struct is the SAVE BLOCK CONTENT: the port's live game-state captured for
 * a save and restored on load. RE2 keeps a ~0x800-byte save region live in RAM
 * and writes it wholesale; the port captures the equivalent state on demand. The
 * field order mirrors RE1.5's GSB where known (save-count, character, player X/Y/Z
 * — DAT_800b0fbd/DAT_800aca5c/DAT_800aca88.. per the RE), plus inventory + the
 * 16-zone story-flag array (g_game.flags) + room.
 */
#ifndef RE15_SAVEDATA_H
#define RE15_SAVEDATA_H

#include <stdint.h>
#include "re15_scd.h"        /* RE15_FLAG_ZONES / RE15_FLAG_WORDS_ZONE + g_game  */
#include "re15_inventory.h"  /* re15_inv_slot_t / RE15_INV_MAX_SLOTS             */

#define RE15_SAVE_MAGIC    0x35314552u   /* "RE15" little-endian                 */
#define RE15_SAVE_VERSION  1

/* The captured game-state. Fields are ordered u32 → u16 → u8 → arrays to avoid
 * implicit padding, so the layout (and thus the checksum) is deterministic. */
typedef struct {
    uint32_t magic;            /* RE15_SAVE_MAGIC — used to detect a valid slot   */
    uint32_t version;          /* RE15_SAVE_VERSION                               */
    uint32_t playtime;         /* engine frame count at save time                 */
    int32_t  player_x;         /* g_actors[0].x  (RE1.5 +0x34)                    */
    int32_t  player_y;         /* g_actors[0].y  (RE1.5 +0x38)                    */
    int32_t  player_z;         /* g_actors[0].z  (RE1.5 +0x3c)                    */
    uint16_t room;             /* g_current_room_id                               */
    uint16_t save_count;       /* increments per save (RE1.5 DAT_800b0fbd)        */
    int16_t  player_rot;       /* g_actors[0].rot_y (RE1.5 +0x6a)                 */
    int16_t  player_hp;        /* g_actors[0].hp   (RE1.5 +0x1ba)                 */
    uint16_t player_status;    /* g_actors[0].status_flags (RE1.5 +0x98)          */
    uint8_t  character;        /* g_gameflow.character (RE1.5 DAT_800aca5c bit0)  */
    uint8_t  equipped_slot;    /* re15_inv_equipped_slot()                        */
    re15_inv_slot_t inv[RE15_INV_MAX_SLOTS];                     /* 11 × 4 bytes  */
    uint32_t flags[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE];       /* g_game.flags  */
    uint32_t checksum;         /* additive checksum over all preceding bytes      */
} re15_savedata_t;

/* Additive checksum over the whole struct except the trailing checksum word. */
uint32_t re15_savedata_checksum(const re15_savedata_t *sd);

/* Capture the live game-state (g_game / g_actors[0] / g_inv / g_current_room_id /
 * g_gameflow) into *out, stamping magic/version/checksum. `playtime` = the frame
 * count to record. `save_count` = the new save number to store. */
void re15_savedata_capture(re15_savedata_t *out, uint32_t playtime, uint16_t save_count);

/* Restore a captured block into the live game-state. Validates magic + checksum
 * first. Returns 0 on success, -1 if the block is invalid. On success the caller
 * still needs to (re)load `room` (the resume room is left in *loaded_room). */
int re15_savedata_restore(const re15_savedata_t *in, uint16_t *loaded_room);

#endif /* RE15_SAVEDATA_H */
