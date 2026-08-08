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
#include "re15_itembox.h"    /* RE15_BOX_SLOTS — ITEM BOX contents (v4)          */

#define RE15_SAVE_MAGIC    0x35314552u   /* "RE15" little-endian                 */
#define RE15_SAVE_VERSION  5             /* v2: + weapon_id; v3: + camera cut (restore the
                                          * save-time framing/background — reuses a reserved
                                          * byte, so v2 saves stay layout+checksum compatible
                                          * and load with cut 0 = the room's default);
                                          * v4: + box[32] (the ITEM BOX — RE1.5's dormant
                                          * save block DOES serialize the four 8-slot arrays
                                          * @0x800b1444/1484/14a4/14c4 wholesale inside the
                                          * 0x1230-byte GSB memcpy @0x800261c4-d4, so box
                                          * persistence is RE1.5-shaped; v2/v3 blocks load
                                          * with an EMPTY box via the validate/upgrade path);
                                          * v5: + wounds[8][2] (die BLUT-DECAL-Wund-Tabelle
                                          * @0x800b10ec liegt bei GSB+0x130 IM 0x1230-Save-
                                          * memcpy @0x800261c4-d8 — das Original speichert
                                          * die Blut-Level nachweislich mit; analysis/
                                          * save_injured_state.md SI-1. Aeltere Blocks
                                          * laden mit Null-Wunden = sauber). */

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
    uint8_t  equipped_slot;    /* re15_inv_equipped_slot()  (DAT_800b25c8)        */
    uint8_t  weapon_id;        /* re15_player_equipped_weapon() (DAT_800aca5d) —
                                * the ACTIVE weapon id; separate global from the
                                * slot index, drives the in-hand mesh/fire/aim/SE */
    uint8_t  camera_cut;       /* g_scd.cam_id at save time — the active fixed-camera cut.
                                * The cut also selects the room's background image, so WITHOUT
                                * restoring it a load frames the saved position under the room's
                                * default cut 0 (wrong camera + wrong background). v3 field. */
    uint8_t  loc_idx;          /* Ortsnamen-Index des Save-Punkts — Patch-Analog game_state[3]
                                * = 0x800B0FBF / Kartenblock +0x203 (reAi patch_save_final.py
                                * Block [Z]; SAVE_LOC_FUNC @0x80070890 liest genau dieses Byte).
                                * 0 = Schreibmaschine/Default ("Irons' Office" sysmes 0x1a =
                                * Stub-Verhalten @0x80026e4c), 1 = Telefon ROOM1070(/1071)
                                * ("Medical Room" sysmes 0x1b). Reserve-Byte rekrutiert wie
                                * camera_cut (v3) — alle aelteren Blocks tragen hier 0 und
                                * zeigen unveraendert sysmes 0x1a; Layout+Checksumme identisch,
                                * kein Versions-Bump noetig. */
    uint8_t  reserved1;        /* keep inv[]/flags[] 4-aligned (deterministic sum) */
    re15_inv_slot_t inv[RE15_INV_MAX_SLOTS];                     /* 11 × 4 bytes  */
    uint32_t flags[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE];       /* g_game.flags  */
    re15_inv_slot_t box[RE15_BOX_SLOTS];   /* v4 ITEM BOX, page*8+i order (the 4
                                            * dormant arrays @0x800b1444/1484/14a4/
                                            * 14c4 flattened; inserted BEFORE the
                                            * checksum, so the v2/v3 checksum word
                                            * sits at offsetof(box) — the upgrade
                                            * path reads it there).              */
    uint8_t  wounds[8][2];     /* v5: die Wund-Tabelle (8 Panels x {level, acc}) — das
                                * GSB+0x130-Analog (@0x800b10ec, Stride 0x1c, im Save-
                                * memcpy enthalten). Vor der checksum eingefuegt; das
                                * v4-Checksum-Wort sitzt bei offsetof(wounds).       */
    uint32_t checksum;         /* additive checksum over all preceding bytes      */
} re15_savedata_t;

/* Additive checksum over the whole struct except the trailing checksum word. */
uint32_t re15_savedata_checksum(const re15_savedata_t *sd);

/* Validate a raw loaded block IN PLACE, upgrading v2/v3 layouts to v4: checks
 * magic; v4+ = the v4 checksum; v2/v3 = the OLD checksum word at offsetof(box)
 * over the old extent, then zeroes box[] (older saves load with an empty box),
 * stamps version 4 and recomputes the checksum. Returns 0 ok, -1 invalid. */
int re15_savedata_validate(re15_savedata_t *sd);

/* Capture the live game-state (g_game / g_actors[0] / g_inv / g_current_room_id /
 * g_gameflow) into *out, stamping magic/version/checksum. `playtime` = the frame
 * count to record. `save_count` = the new save number to store. */
void re15_savedata_capture(re15_savedata_t *out, uint32_t playtime, uint16_t save_count);

/* Restore a captured block into the live game-state. Validates magic + checksum
 * first. Returns 0 on success, -1 if the block is invalid. On success the caller
 * still needs to (re)load `room` (the resume room is left in *loaded_room). */
int re15_savedata_restore(const re15_savedata_t *in, uint16_t *loaded_room);

#endif /* RE15_SAVEDATA_H */
