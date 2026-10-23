/* giftkugel_r41.c — Runde 41 Spur D: Giftkugel der Spinne trifft den Spieler wie RE2 (Schaden 20 @0x800200dc,
 * Kommando 2/3 FUN_80047664 @0x800477b8/@0x8004780c, Todeszweig FUN_800401d4). Belege: include/re15_giftkugel_r41.h. */
#include "re15_giftkugel_r41.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_engine.h"
#include "re15_ai_flavor.h"   /* re15_ai_re2_for_type */
#include "re15_enemy_ai.h"    /* re15_player_is_grabbed */

static unsigned s_treffer, s_tode;
static int      s_env_init;
static long     s_hp_bild = -1;
static int      s_hp_wert;

void re15_audio_room_se(int idx);   /* Plattform (audio_pc.c) / Test-Stub (tests/test_support.c) */

unsigned re15_giftkugel_r41_treffer_zaehler(void) { return s_treffer; }
unsigned re15_giftkugel_r41_tode(void)            { return s_tode; }
void     re15_giftkugel_r41_reset(void)           { s_treffer = s_tode = 0; }

int re15_giftkugel_r41_treffer(re15_actor_t *pl, const re15_attack_box_t *box, int besitzer)
{
    if (!pl || !box) return 0;
    if (besitzer <= 0 || besitzer >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *sp = &g_actors[besitzer];
    if (sp->type != 0x25u || !re15_ai_re2_for_type(0x25u)) return 0;   /* nur die Spinne unter dem RE2-Gehirn */
    if (!re15_hitbox_test(pl, box)) return 0;                           /* RE1.5-Effektkasten (Radius 500) */
    if (pl->hit_react & 0x1u) return 0;            /* hit-once +0x93 @0x80012e24-30 (RE2 +0x1d3 @0x800476ec-f4) */
    if (re15_player_is_grabbed()) return 0;        /* +0x1D3 & 0x80 */
    if (pl->hp < 0) return 0;                      /* `lh v0,0x156(s0) / bltz` @0x800476fc-704 */

    const int vor = (int)pl->hp;
    pl->hp = (int16_t)(pl->hp - RE15_GIFTKUGEL_R41_SCHADEN);            /* FUN_800401d4: HP -= 20 */
    re15_audio_room_se(10);                                             /* Treffer-SE wie @0x80012e70 */
    /* Kommando 2 (@0x800477b8) in der Port-Form des Spielerzweigs (re15_damage.c @0x80012ebc-eec). */
    pl->state       = 2;
    pl->sub_state_1 = (uint8_t)(re15_player_hit_from_front(pl, box->x, box->z) + 2);
    pl->sub_state_2 = 0;
    pl->hit_react  |= 0x1u;
    s_treffer++;
    if (pl->hp < 0) {
        /* FUN_800401d4 Todeszweig (ret 2): DAT_800cfdcb |= 0x80 (@0x800402c0-d0) -> Kommando 3 (@0x8004780c). */
        pl->re2z_self1d3 |= 0x80u;
        pl->state = 3; pl->sub_state_1 = 0; pl->sub_state_2 = 0;
        s_tode++;
        fprintf(stderr, "[giftkugel] F%u RE2-Treffer (Spinne Slot %d): HP %d -> %d = TOD (Kommando 3)\n",
                (unsigned)g_engine.frame_count, besitzer, vor, (int)pl->hp);
        return 2;
    }
    fprintf(stderr, "[giftkugel] F%u RE2-Treffer (Spinne Slot %d): HP %d -> %d (Kommando 2)\n",
            (unsigned)g_engine.frame_count, besitzer, vor, (int)pl->hp);
    return 1;
}

int re15_giftkugel_r41_messhaken(re15_actor_t *pl)
{
    if (!s_env_init) {
        s_env_init = 1;
        const char *e = getenv("RE15_R41_HP_AT");
        if (e && *e) {
            long b = atol(e);
            const char *c = strchr(e, ':');
            if (c) { s_hp_bild = b; s_hp_wert = atoi(c + 1); }
        }
    }
    if (!pl || s_hp_bild < 0) return 0;
    if ((long)g_engine.frame_count != s_hp_bild) return 0;
    pl->hp = (int16_t)s_hp_wert;
    fprintf(stderr, "[giftkugel] F%u Messhaken: HP := %d\n", (unsigned)g_engine.frame_count, s_hp_wert);
    return 1;
}
