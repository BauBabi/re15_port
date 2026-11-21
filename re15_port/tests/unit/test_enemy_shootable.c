/* test_enemy_shootable.c — the player auto-aim + PER-TYPE damage across the roster.
 *
 * Byte-true: FUN_80011f50 iterates all entities and tests each one's DAMAGE HITBOX (+0x78) — no per-type
 * whitelist. And the damage is per-type (@0x8006e0d0: dmg = u16[type*0x58 + weapon*4]) — a crow takes
 * 2/shot, a tyrant 16, an alligator/birkin 30, while the spider-baby/birkin-5 rows are all zero =
 * WEAPON-IMMUNE (targetable, but no gun damage). Asserts:
 *   (1) ALLIGATOR (0x23): shootable, pistol -30.
 *   (2) TYRANT (0x2b): shootable, pistol -16.
 *   (3) IVY (0x2d): NOT targetable — the ivy INIT installs NO +0x78 box (the {450,1530} box @0x8011a2c8
 *       belongs to the neighbouring type-0x40 NPC, audit wf_efd92a2c ivy #77), so auto-aim skips it.
 *   (4) WRITHER (0x1a): keeps its 300-radius +0x78 box (explosions via re15_re2_gl_apply still hit it), but is
 *       NO gun / auto-aim target: aim band 0 like the disc (Runde 42 gore U8, be62608c0).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

/* returns the fire result (slot+1 or 0) and reports the damage dealt via *dmg_out */
static int fire_at(uint8_t type, int32_t dist, int16_t *dmg_out)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
    re15_player_death_reset();
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = type; e->state = 1; e->x = 0; e->z = dist; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    int16_t hp0 = e->hp = 300;
    int ret = re15_player_weapon_fire(2);   /* pistol (weapon 2) */
    *dmg_out = (int16_t)(hp0 - e->hp);
    return ret;
}

int main(void)
{
    int fail = 0;
    int16_t dmg;
    printf("=== player auto-aim (hitbox-gated) + PER-TYPE weapon damage ===\n");

    /* (1) ALLIGATOR: shootable, pistol damage = 30 (@0x8006e0d0 + 0x23*0x58) */
    if (fire_at(0x23, 2000, &dmg) != 2 || dmg != 30) {
        fprintf(stderr, "FAIL(1): ALLIGATOR (0x23) pistol must deal 30, dealt %d\n", dmg); fail = 1; }
    else printf("  (1) ALLIGATOR (0x23): shootable, pistol -%d\n", dmg);

    /* (2) TYRANT: shootable, pistol damage = 16 */
    if (fire_at(0x2b, 1500, &dmg) != 2 || dmg != 16) {
        fprintf(stderr, "FAIL(2): TYRANT (0x2b) pistol must deal 16, dealt %d\n", dmg); fail = 1; }
    else printf("  (2) TYRANT (0x2b): shootable, pistol -%d\n", dmg);

    /* (3) IVY: NOT targetable — the ivy has NO +0x78 damage box (dormant weapon-immune prop). The old
     * test pinned the inverse ("targetable but immune") which mis-attributed the type-0x40 NPC's
     * {450,1530} box to the ivy (audit wf_efd92a2c ivy #77). apply_hitbox(0x2d) leaves radius 0, so
     * auto-aim skips it (re15_damage.c: hit_radius_min<=0 -> not a valid target) -> fire selects nothing. */
    {
        int ret = fire_at(0x2d, 1200, &dmg);
        if (ret != 0 || g_actors[1].hit_radius_min != 0) {
            fprintf(stderr, "FAIL(3): IVY (0x2d) must be non-targetable (no box), ret=%d box=%d\n", ret, g_actors[1].hit_radius_min); fail = 1; }
        else printf("  (3) IVY (0x2d): NO box -> NOT a weapon/auto-aim target (dormant immune prop)\n");
    }

    /* (4) WRITHER (0x1a) — Gitterhand. ⛔ RUNDE 42 STRANG GORE U8 (be62608c0, BAUPLAN gore_matrix U8; Nutzerregel 16.11.2026
     * woertlich "80% build ist ausschlaggebend, dann 40% build und wenn ... dort Dinge noch fehlen dann kommt Resident Evil 2
     * zum Einsatz", "anschiessbare Gitterhaende" ist dort ausdruecklich ein aufgehobener Entscheid): wie auf der Disc KEIN
     * Zielband - kein `jal 0x80012aa4` / `jal 0x80012974` (Band-Stempel) im Arm-Code 0x8010c1ec-0x8010d7a0, die Arme kommen
     * ohne HP (ROOM1210 Sce_em_set @0x1d86..0x1e4e). re15_damage.c setzt fuer 0x1a eband = 0 -> der Schuss waehlt den Arm
     * nicht. Die +0x78-Box (300, INIT @0x8012091c) BLEIBT: Explosionen (re15_re2_gl_apply) wirken weiter.
     * Strang riegel 21.11.2026: dieser Riegel pinnte noch den aufgehobenen Stand ("targetable+killable") und war seit
     * be62608c0 rot; umgestellt wie test_writher_hit_feedback im selben Commit. */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
        re15_player_death_reset();
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x1a; e->state = 1; e->x = 0; e->z = 800; e->rot_y = 0; e->hp = 0;
        re15_enemy_apply_hitbox(e, 0x1a);
        int ret = re15_player_weapon_fire(2);
        if (ret != 0 || e->hit_radius_min != 300 || e->state != 1 || e->hp != 0) {
            fprintf(stderr, "FAIL(4): WRITHER (0x1a) must keep its 300 box but be NO gun target (U8), ret=%d box=%d hp=%d state=%d\n",
                    ret, e->hit_radius_min, e->hp, e->state); fail = 1; }
        else printf("  (4) WRITHER (0x1a): 300-box kept (explosions), aim band 0 -> no gun target (hp=%d state=%d)\n", e->hp, e->state);
    }

    if (fail) { printf("ENEMY-SHOOTABLE: FAIL\n"); return 1; }
    printf("ENEMY-SHOOTABLE: all checks passed\n");
    return 0;
}
