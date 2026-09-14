/* probe_1d3_gate_hang.c - MESSSCHIENE (2026-09-14): WARUM lief das Ausschluss-Gate auf die
 * Trefferpause in TIMEOUTS?
 *
 * BEFUND, den diese Sonde nachweist: NICHT das fehlende Dekrement, sondern die
 * REKURSION DES AUFLOESERS. re15_damage.c hat den byte-true Zweit-Opfer-Rueckgriff
 *     if (g_actors[best].hit_react & 0x1) { g_actors[best].hit_react |= 0x2;
 *                                           goto retry_after_latch; }   (@0x8001240c-18)
 * Seine ABBRUCHBEDINGUNG ist das Kandidaten-Gate eine Zeile weiter oben:
 *     else if ((e->hit_react & 0x3) == 0x3) continue;                   (@0x800120f4-0x80012100)
 * Der Diagnose-Schalter RE15_DIAG_1D3GATE haengt das Trefferpausen-Gate per `else if` an die
 * STELLE dieses Gates - fuer RE2-eigene Typen faellt der +0x93-Ausschluss also WEG. Der
 * Rueckgriff waehlt dann denselben Aktor erneut (Bit 0 steht ja noch), setzt Bit 1, springt
 * zurueck - und das endlos.
 *
 * MESSUNG: ein RE2-Zombie in ROOM1140, EIN Schuss auf dem echten Weg, zwei Varianten:
 *   argv[1] = 0  ->  +0x93 Bit 0 ist VOR dem Schuss GELOESCHT  -> kein Rueckgriff  -> kehrt zurueck
 *   argv[1] = 1  ->  +0x93 Bit 0 ist VOR dem Schuss GESETZT    -> Rueckgriff       -> haengt
 * Beide Varianten mit und ohne RE15_DIAG_1D3GATE fahren (die Sonde liest die Variable nicht
 * selbst - sie kommt aus der Umgebung, genau wie die Engine sie liest).
 *
 * Jede Zeile wird sofort geflusht, damit der letzte Ausdruck vor dem Haenger im Log steht.
 * Reine Messschiene: bestanden = gelaufen.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int     re15_player_aim_ready(void);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

int main(int argc, char **argv)
{
    int want_latch = (argc > 1) ? atoi(argv[1]) : 0;
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("VARIANTE: +0x93 Bit 0 vor dem Schuss = %d, RE15_DIAG_1D3GATE = %s\n",
           want_latch, getenv("RE15_DIAG_1D3GATE") ? getenv("RE15_DIAG_1D3GATE") : "(nicht gesetzt)");

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FEHLT: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse-Fehler\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }

    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) { slot = s; break; }
    if (slot < 0) { printf("kein stehender Zombie gefunden\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *e = &g_actors[slot];
    pl->x = e->x - 2000; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, 2000); frame(RE15_PAD_BIT_R1, 0);
    }
    e->hp = 9000;
    e->re2z_self1d3 = 0;                                   /* Trefferpause abgelaufen */
    e->hit_react = (uint8_t)(want_latch ? 0x01 : 0x00);    /* +0x93 Bit 0 */

    printf("VOR dem Schuss: slot=%d st=%d/%d/%d hp=%d +0x93=0x%02X +0x1D3=0x%02X\n",
           slot, e->state, e->sub_state_1, e->sub_state_2, e->hp, e->hit_react,
           e->re2z_self1d3);
    printf("-> rufe jetzt den Schuss-Frame (re15_game_step -> re15_player_weapon_fire)\n");

    pl->hp = 100; track(slot, 2000);
    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE), RE15_PAD_BIT_SQUARE);

    printf("NACH dem Schuss: hp=%d +0x93=0x%02X +0x1D3=0x%02X st=%d/%d/%d\n",
           e->hp, e->hit_react, e->re2z_self1d3, e->state, e->sub_state_1, e->sub_state_2);
    printf("ZURUECKGEKEHRT - kein Haenger.\n");
    free(buf);
    return 0;
}
