/*
 * re15_savedata.c — capture/restore the port's game-state save block.
 * See re15_savedata.h. Pure engine (target-agnostic); the on-card framing lives
 * in re15_memcard.c, the phone trigger + card screen in the platform layer.
 */
#include "re15_savedata.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_gameflow.h"
#include "re15_collision.h"   /* re15_collision_set_band / band_from_y — resume floor band */
#include "re15_damage.h"      /* re15_player_equipped_weapon / _set_equipped_weapon (DAT_800aca5d) */
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

int re15_savedata_validate(re15_savedata_t *sd)
{
    if (!sd) return -1;
    if (sd->magic != RE15_SAVE_MAGIC) return -1;
    if (sd->version >= 5) {
        return (sd->checksum == re15_savedata_checksum(sd)) ? 0 : -1;
    }
    if (sd->version == 4) {
        /* v4 layout = the v5 struct WITHOUT wounds[]: its checksum word sits at
         * offsetof(wounds). Validate the old extent, upgrade: zero wounds (older
         * saves load clean), version 5, fresh checksum. */
        const uint8_t *p = (const uint8_t *)sd;
        size_t old_ck_off = offsetof(re15_savedata_t, wounds);
        uint32_t sum = 0, old_ck;
        for (size_t i = 0; i < old_ck_off; i++) sum += p[i];
        memcpy(&old_ck, p + old_ck_off, sizeof old_ck);
        if (sum != old_ck) return -1;
        memset(sd->wounds, 0, sizeof sd->wounds);
        sd->version  = RE15_SAVE_VERSION;
        sd->checksum = re15_savedata_checksum(sd);
        return 0;
    }
    if (sd->version == 2 || sd->version == 3) {
        /* v2/v3 layout = the struct WITHOUT box[] and wounds[]: its checksum word
         * sits where box[] now starts. Validate the old extent, then upgrade in
         * place: empty box + clean wounds, current version, fresh checksum. */
        const uint8_t *p = (const uint8_t *)sd;
        size_t old_ck_off = offsetof(re15_savedata_t, box);
        uint32_t sum = 0, old_ck;
        for (size_t i = 0; i < old_ck_off; i++) sum += p[i];
        memcpy(&old_ck, p + old_ck_off, sizeof old_ck);
        if (sum != old_ck) return -1;
        memset(sd->box, 0, sizeof sd->box);
        memset(sd->wounds, 0, sizeof sd->wounds);
        sd->version  = RE15_SAVE_VERSION;
        sd->checksum = re15_savedata_checksum(sd);
        return 0;
    }
    return -1;
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
    out->weapon_id     = (uint8_t)re15_player_equipped_weapon();   /* DAT_800aca5d */
    out->camera_cut    = (uint8_t)g_scd.cam_id;                    /* active fixed-camera cut */

    memcpy(out->inv,   g_inv.slots, sizeof(out->inv));
    memcpy(out->flags, g_game.flags, sizeof(out->flags));
    re15_itembox_export(out->box);       /* v4 ITEM BOX contents (page*8+i) */
    re15_wound_save(out->wounds);        /* v5 Blut-Decal-Wunden (GSB+0x130-Analog @0x800b10ec —
                                          * im Original-Save-memcpy @0x800261c4-d8 enthalten) */

    out->checksum = re15_savedata_checksum(out);
}

int re15_savedata_restore(const re15_savedata_t *in, uint16_t *loaded_room)
{
    re15_savedata_t up;
    if (!in) return -1;
    /* validate + v2/v3 -> v4 upgrade on a copy (v2/v3 load with an empty box) */
    up = *in;
    if (re15_savedata_validate(&up) != 0) return -1;
    in = &up;

    re15_actor_t *pl = &g_actors[0];
    pl->x            = in->player_x;
    pl->y            = in->player_y;
    pl->z            = in->player_z;
    pl->rot_y        = in->player_rot;
    pl->hp           = in->player_hp;
    pl->status_flags = in->player_status;

    /* The global collision band is a pure function of the player Y (band_from_y),
     * set once from the room's default-spawn Y on entry and NOT recomputed per
     * frame — so after overwriting Y with the saved value, re-derive the band (and
     * the actor's +0x82 floor) or a save on a non-entry floor resumes clamping /
     * cutting the camera against the wrong floor band. */
    {
        int band = re15_collision_band_from_y(pl->y);
        re15_collision_set_band(band);
        pl->floor = (uint8_t)band;
    }

    g_gameflow.character = in->character;

    memcpy(g_inv.slots, in->inv, sizeof(g_inv.slots));
    /* Restore BOTH the active weapon id (DAT_800aca5d — drives the in-hand mesh,
     * weapon fire, aim-class split and SE bank) and the exact equipped slot. Set
     * the weapon first (it also derives a slot), then pin the saved slot. */
    re15_player_set_equipped_weapon(in->weapon_id);
    re15_inv_set_equipped_slot(in->equipped_slot);
    memcpy(g_game.flags, in->flags, sizeof(g_game.flags));
    re15_itembox_import(in->box);        /* v4 ITEM BOX contents survive load */
    /* v5 Wund-Restore — setzt IMMER alle 8 Panels (aeltere Saves laden Nullen aus dem
     * Upgrade-Pfad): behebt zugleich den Stale-Blut-Bug (analysis/save_injured_state.md
     * SI-3 — CONTINUE liess das Blut des gestorbenen Runs stehen, weil kein Spiel-Caller
     * ausser new_game re15_wound_reset rief) und stellt die gespeicherten Blut-Level
     * wieder her (SI-1; der Generation-Bump laesst den Platform-Wound-Sync nach dem
     * naechsten TIM-Upload automatisch re-stempeln). */
    re15_wound_load(in->wounds);

    if (loaded_room) *loaded_room = in->room;
    return 0;
}
