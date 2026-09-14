/* probe_abzug_takt.c — MESSSONDE (reiner Befund, kein Fix).
 *
 * Frage: Wie viele Bilder liegen im PORT zwischen dem ersten Bild mit gehaltenem
 * Abzug (SQUARE) im Zielzustand und dem ersten HP-Abzug am Gegner?
 *
 * Original-Gegenstueck (selbst disassembliert): Sub 1 (HALTEN) @0x80033180
 *   @0x80033300 lw DAT_800ac768 / @0x80033308 andi 0x40 (SQUARE, virtuelles Bit 6)
 *   @0x80033314 jal FUN_8004ea6c (Magazin)
 *   @0x80033320 ori v0,zero,2 / @0x80033328 sh v0,DAT_800aca5a   <- HALBWORT: setzt Sub=2
 *                                                                    UND loescht den Riegel 0x800aca5b
 *   @0x8003332c j 0x80033450 -> @0x80033450-58 lw ra/addiu sp/jr ra = RUECKKEHR
 * Der Entlade-Sub 2 @0x80033460 laeuft erst im FOLGEBILD, weil die Wurzel
 * @0x80032e9c-c4 je Bild genau EINMAL auf DAT_800aca5a dispatcht
 * (Kette: @0x8001ce0c jal 0x80031c44 -> @0x80031efc lbu 0x800aca59 / @0x80031f0c
 *  Tabelle 0x80073ff0[7]=0x80032e44 -> @0x80032e74 Tabelle 0x80074030[Waffe]=0x80032e9c).
 * Erwartung fuer das Original also: Abzugsbild N -> Schaden in N+1.
 *
 * Diese Sonde misst NUR die Portseite.
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_desc[RE15_ACTOR_MAX];

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

static void bringup(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    memset(s_desc, 0, sizeof s_desc);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) s_desc[s] = g_actors[s].grid_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
}

static int find_desc(uint8_t d)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && s_desc[s] == d) return s;
    return -1;
}

static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    bringup();
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
        frame(0, 0);
    }
    int slot = find_desc(0x86);
    if (slot < 0) { printf("kein Fresser 0x86\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(slot, 6000);

    /* R1 halten bis ZIELBEREIT; Bilder zaehlen */
    int raise = 0;
    for (; raise < 60 && !re15_player_aim_ready(); raise++) {
        pl->hp = 100; face(slot, 6000); frame(RE15_PAD_BIT_R1, 0);
    }
    printf("ZIELBEREIT nach %d R1-Bildern (aim_ready=%d)\n", raise, re15_player_aim_ready());

    /* Ab jetzt Abzug HALTEN; Bild-Index des ersten HP-Abzugs messen */
    int hp_last = e->hp;
    int erster = -1;
    printf("Bild | pad            | aim_ready | HP\n");
    for (int f = 0; f < 12; f++) {
        pl->hp = 100; face(slot, 6000);
        int rdy_vor = re15_player_aim_ready();
        int hp_vor  = e->hp;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        printf("  %2d | R1+SQUARE      |   %d       | %d -> %d%s\n",
               f, rdy_vor, hp_vor, e->hp, (e->hp < hp_vor) ? "   <== SCHADEN" : "");
        if (erster < 0 && e->hp < hp_last) erster = f;
        hp_last = e->hp;
    }
    printf("\nERGEBNIS: erster HP-Abzug im Bild-Index %d (0 = IM Abzugsbild, 1 = Folgebild)\n", erster);
    return 0;
}
