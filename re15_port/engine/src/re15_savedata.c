/*
 * re15_savedata.c — capture/restore the port's game-state save block.
 * See re15_savedata.h. Pure engine (target-agnostic); the on-card framing lives
 * in re15_memcard.c, the phone trigger + card screen in the platform layer.
 */
#include "re15_savedata.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_gameflow.h"
#include <string.h>
#include <stddef.h>

uint32_t re15_savedata_checksum(const re15_savedata_t *sd)
{
    /* additive sum over every byte before the trailing checksum word */
    const uint8_t *p = (const uint8_t *)sd;
    size_t n = offsetof(re15_savedata_t, checksum);
    uint32_t sum = 0;
    for (size_t i = 0; i < n; i++) sum += p[i];
    return sum;
}

void re15_savedata_capture(re15_savedata_t *out, uint32_t playtime, uint16_t save_count)
{
    if (!out) return;
    memset(out, 0, sizeof(*out));

    const re15_actor_t *pl = &g_actors[0];
    out->magic         = RE15_SAVE_MAGIC;
    out->version       = RE15_SAVE_VERSION;
    out->playtime      = playtime;
    out->player_x      = pl->x;
    out->player_y      = pl->y;
    out->player_z      = pl->z;
    out->room          = (uint16_t)g_current_room_id;
    out->save_count    = save_count;
    out->player_rot    = pl->rot_y;
    out->player_hp     = pl->hp;
    out->player_status = pl->status_flags;
    out->character     = (uint8_t)g_gameflow.character;
    out->equipped_slot = (uint8_t)re15_inv_equipped_slot();

    memcpy(out->inv,   g_inv.slots, sizeof(out->inv));
    memcpy(out->flags, g_game.flags, sizeof(out->flags));

    out->checksum = re15_savedata_checksum(out);
}

int re15_savedata_restore(const re15_savedata_t *in, uint16_t *loaded_room)
{
    if (!in) return -1;
    if (in->magic != RE15_SAVE_MAGIC) return -1;
    if (in->checksum != re15_savedata_checksum(in)) return -1;

    re15_actor_t *pl = &g_actors[0];
    pl->x            = in->player_x;
    pl->y            = in->player_y;
    pl->z            = in->player_z;
    pl->rot_y        = in->player_rot;
    pl->hp           = in->player_hp;
    pl->status_flags = in->player_status;

    g_gameflow.character = in->character;

    memcpy(g_inv.slots, in->inv, sizeof(g_inv.slots));
    re15_inv_set_equipped_slot(in->equipped_slot);
    memcpy(g_game.flags, in->flags, sizeof(g_game.flags));

    if (loaded_room) *loaded_room = in->room;
    return 0;
}
