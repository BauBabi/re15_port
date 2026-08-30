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
#include "re15_savepoint.h"   /* re15_savepoint_loc — Ortsnamen-Index (Patch-Analog 0x800B0FBF) */
#include "re15_scd.h"         /* Weste: Flag(3,0x75) -> work_vars[0x10]-Rekonstruktion */
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

/* ---- ALT-STAENDE (bis v6) -------------------------------------------------------
 * Bis v6 hatte die ITEM BOX 32 Plaetze; mit der Umstellung auf RESIDENT EVIL 2s
 * vollstaendigen Box-Mechanismus (64-Platz-Ring, Nutzer-Auftrag 2026-08-30) waechst
 * box[] MITTEN im Datensatz. Alle aelteren Staende werden deshalb ZUERST im ALTEN
 * Layout (re15_savedata_v6_t) validiert und auf v6 gehoben, und danach feldweise in
 * das v7-Layout uebernommen (Plaetze 0..31 bleiben erhalten, 32..63 starten leer). */
static uint32_t v6_sum(const re15_savedata_v6_t *sd, size_t n)
{
    const uint8_t *p = (const uint8_t *)sd;
    uint32_t sum = 0;
    for (size_t i = 0; i < n; i++) sum += p[i];
    return sum;
}

static int v6_validate_and_lift(re15_savedata_v6_t *sd)
{
    /* Dieselbe Kette wie frueher, nur auf der ALT-Struktur: jede Vorversion traegt
     * ihr Checksum-Wort dort, wo das jeweils NEUE Feld beginnt. */
    if (sd->version == 6) {
        uint32_t ck = v6_sum(sd, offsetof(re15_savedata_v6_t, checksum));
        return (ck == sd->checksum) ? 0 : -1;
    }
    if (sd->version == 5) {                    /* v5 = v6 ohne visited[] */
        size_t off = offsetof(re15_savedata_v6_t, visited);
        uint32_t old_ck;
        memcpy(&old_ck, (const uint8_t *)sd + off, sizeof old_ck);
        if (v6_sum(sd, off) != old_ck) return -1;
        memset(sd->visited, 0, sizeof sd->visited);
        return 0;
    }
    if (sd->version == 4) {                    /* v4 = ohne wounds[] + visited[] */
        size_t off = offsetof(re15_savedata_v6_t, wounds);
        uint32_t old_ck;
        memcpy(&old_ck, (const uint8_t *)sd + off, sizeof old_ck);
        if (v6_sum(sd, off) != old_ck) return -1;
        memset(sd->wounds, 0, sizeof sd->wounds);
        memset(sd->visited, 0, sizeof sd->visited);
        return 0;
    }
    if (sd->version == 2 || sd->version == 3) {  /* v2/v3 = zusaetzlich ohne box[] */
        size_t off = offsetof(re15_savedata_v6_t, box);
        uint32_t old_ck;
        memcpy(&old_ck, (const uint8_t *)sd + off, sizeof old_ck);
        if (v6_sum(sd, off) != old_ck) return -1;
        memset(sd->box, 0, sizeof sd->box);
        memset(sd->wounds, 0, sizeof sd->wounds);
        memset(sd->visited, 0, sizeof sd->visited);
        return 0;
    }
    return -1;
}

int re15_savedata_validate(re15_savedata_t *sd)
{
    if (!sd) return -1;
    if (sd->magic != RE15_SAVE_MAGIC) return -1;

    if (sd->version >= 7) {
        return (sd->checksum == re15_savedata_checksum(sd)) ? 0 : -1;
    }
    if (sd->version >= 2 && sd->version <= 6) {
        /* Alt-Stand: im v6-Layout pruefen/heben, dann ins v7-Layout uebernehmen.
         * Der uebergebene Puffer ist v7-gross (>= v6), das Lesen der Alt-Form ist
         * also gedeckt; die Uebernahme laeuft ueber eine Kopie, weil sich alle
         * Felder HINTER box[] verschieben. */
        re15_savedata_v6_t old;
        memcpy(&old, sd, sizeof old);
        if (v6_validate_and_lift(&old) != 0) return -1;

        memset(sd, 0, sizeof *sd);
        sd->magic         = old.magic;
        sd->version       = RE15_SAVE_VERSION;
        sd->playtime      = old.playtime;
        sd->player_x      = old.player_x;
        sd->player_y      = old.player_y;
        sd->player_z      = old.player_z;
        sd->room          = old.room;
        sd->save_count    = old.save_count;
        sd->player_rot    = old.player_rot;
        sd->player_hp     = old.player_hp;
        sd->player_status = old.player_status;
        sd->character     = old.character;
        sd->equipped_slot = old.equipped_slot;
        sd->weapon_id     = old.weapon_id;
        sd->camera_cut    = old.camera_cut;
        sd->loc_idx       = old.loc_idx;
        sd->reserved1     = old.reserved1;
        memcpy(sd->inv,     old.inv,     sizeof sd->inv);
        memcpy(sd->flags,   old.flags,   sizeof sd->flags);
        /* Die alten 32 Plaetze behalten ihre Reihenfolge (frueher Seite*8+i, jetzt
         * Ring-Index) — der Inhalt bleibt damit erhalten; 32..63 sind leer. */
        memcpy(sd->box,     old.box,     sizeof old.box);
        memcpy(sd->wounds,  old.wounds,  sizeof sd->wounds);
        memcpy(sd->visited, old.visited, sizeof sd->visited);
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
    out->loc_idx       = re15_savepoint_loc();   /* Ortsnamen-Index, gelatcht am Save-Trigger
                                                  * (Patch-Analog: 0x800B0FBF -> Karte +0x203) */

    memcpy(out->inv,   g_inv.slots, sizeof(out->inv));
    memcpy(out->flags, g_game.flags, sizeof(out->flags));
    re15_itembox_export(out->box);       /* v4 ITEM BOX contents (page*8+i) */
    re15_wound_save(out->wounds);        /* v5 Blut-Decal-Wunden (GSB+0x130-Analog @0x800b10ec —
                                          * im Original-Save-memcpy @0x800261c4-d8 enthalten) */

    re15_map_visited_export(out->visited);   /* v6 RE2-Kartensystem (re15_map_visited.c) */

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
    /* R.P.D.-WESTE: work_vars werden nicht serialisiert (das Original speichert
     * 0x800b0ff0 im Save-Record-memcpy @0x800261c4-d8 mit) — den Modell-Index aus dem
     * save-persistenten Flag(3,0x75) rekonstruieren, sonst laedt der naechste Raum
     * Leon ohne Weste ("Modell-Desync nach Load"). Den Engine-Spiegel mitziehen,
     * damit der Reload-Heiler (re15_vest_hp_on_model_reload) nach dem Load keinen
     * falschen Wechsel sieht und die GESPEICHERTEN HP ueberschreibt. */
    {
        int16_t vm = re15_game_flag_get(3, 0x75) ? 1 : 0;
        g_scd.work_vars[0x10] = vm;
        re15_vest_model_mark(vm);
    }
    re15_itembox_import(in->box);        /* v4 ITEM BOX contents survive load */
    /* v5 Wund-Restore — setzt IMMER alle 8 Panels (aeltere Saves laden Nullen aus dem
     * Upgrade-Pfad): behebt zugleich den Stale-Blut-Bug (analysis/save_injured_state.md
     * SI-3 — CONTINUE liess das Blut des gestorbenen Runs stehen, weil kein Spiel-Caller
     * ausser new_game re15_wound_reset rief) und stellt die gespeicherten Blut-Level
     * wieder her (SI-1; der Generation-Bump laesst den Platform-Wound-Sync nach dem
     * naechsten TIM-Upload automatisch re-stempeln). */
    re15_wound_load(in->wounds);
    /* v6 RE2-Kartensystem: Besucht-Bits laden; der geladene Raum wird beim folgenden
     * scd_room_reenter ohnehin markiert (Choke-Point), das Import genuegt hier. */
    re15_map_visited_import(in->visited);

    if (loaded_room) *loaded_room = in->room;
    return 0;
}
