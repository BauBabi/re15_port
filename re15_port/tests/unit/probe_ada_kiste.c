/* probe_ada_kiste.c — MESSSONDE: laeuft ein Typ-0x42-NPC ("Ada") in ROOM1090 durch die KISTEN?
 *
 * Original-Referenz (selbst disassembliert, siehe Bericht):
 *   FUN_8002bd44 @0x8002be0c-4c = die OBJEKT-Schleife des Main-Ticks (@0x8001ce14). Sie
 *   iteriert enemy_array (0x800acc2c, stride 0x1f4, count 0x800aca4e) und ruft fuer JEDES
 *   aktive Entity FUN_8002cabc(entity, obj, 0) @0x8002be40 -> a2=0 = "schiebe das ENTITY".
 *   Die NPC-Wurzel 0x8011cb70 selbst ruft KEINE Objekt-Kollision — die Ausschiebung kommt
 *   von der Objektseite.
 * Sonde ohne add_test. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob42[0x80000];
static int                s_scd_off = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim; memset(&tim, 0, sizeof tim);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank  (blob, len, &eb->skel_loco,   &eb->anim_loco)   == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
                eb->own_ok    = (re15_emd_parse_own_bank   (blob, len, &eb->skel_own,    &eb->anim_own)    == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    if (!s_scd_off) scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Ist (x,z) INNERHALB des Objekt-Kastens von prop p (Aktor-Radius r addiert)? */
static int in_box(int p, int32_t x, int32_t z, int32_t r)
{
    int32_t bx = (int32_t)g_scd.props[p].box_cx + g_scd.props[p].x;
    int32_t bz = (int32_t)g_scd.props[p].box_cz + g_scd.props[p].z;
    int32_t sx = (int32_t)(uint16_t)g_scd.props[p].box_hx + r;
    int32_t sz = (int32_t)(uint16_t)g_scd.props[p].box_hz + r;
    int32_t dx = x - bx, dz = z - bz;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return (dx <= sx && dz <= sz);
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1090 — laeuft die 0x42-NPC durch die Kisten? ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1090;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    re15_collision_ensure_band(pl->y);

    printf("  EM042-Bank: %s\n", load_bank(0x42, s_blob42, sizeof s_blob42) ? "ok" : "FEHLT");

    re15_game_flag_set(3, 0x81, 1);
    re15_game_flag_set(3, 0x84, 1);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    /* ---------- (1) PROP-BESTAND ---------- */
    printf("\n-- (1) Obj_model_set-Props nach main00+sub00 --\n");
    for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++) {
        int solid = !(g_scd.props[i].box_hx == 0 && g_scd.props[i].box_hy == 0 &&
                      g_scd.props[i].box_hz == 0);
        printf("  prop[%d] aktiv=%u id=0x%02X typ=%u band=%u flags=0x%04X pos=(%ld,%ld,%ld)"
               "  box c=(%d,%d,%d) h=(%d,%d,%d)  SOLID=%d\n",
               i, g_scd.props[i].active, g_scd.props[i].obj_id, g_scd.props[i].obj_type,
               g_scd.props[i].band, g_scd.props[i].flags,
               (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z,
               g_scd.props[i].box_cx, g_scd.props[i].box_cy, g_scd.props[i].box_cz,
               g_scd.props[i].box_hx, g_scd.props[i].box_hy, g_scd.props[i].box_hz, solid);
    }

    /* ---------- (2) VAKUANZ-PRUEFUNG: welchen State nimmt der 0x42 natuerlich an? ------ */
    printf("\n-- (2) VAKUANZ-PRUEFUNG: natuerlicher Verlauf, SCD AN, 240 Frames --\n");
    s_scd_off = 0;
    int slot = -1;
    int seen_state1 = 0, first_state1 = -1;
    int last_st = -1, last_ss = -1, last_grid = -1;
    for (int f = 0; f < 240; f++) {
        frame_step();
        if (slot < 0)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x42) { slot = s; break; }
        if (slot < 0) continue;
        re15_actor_t *n = &g_actors[slot];
        if (n->state == 1 && !seen_state1) { seen_state1 = 1; first_state1 = f; }
        if (n->state != last_st || n->sub_state_1 != last_ss || n->grid_id != last_grid) {
            printf("   F%-3d st=%d ss1=%d ss2=%d grid=0x%02X mo=%d @(%ld,%ld,%ld)\n",
                   f, n->state, n->sub_state_1, n->sub_state_2, n->grid_id, n->motion,
                   (long)n->x, (long)n->y, (long)n->z);
            last_st = n->state; last_ss = n->sub_state_1; last_grid = n->grid_id;
        }
    }
    if (slot < 0) {
        printf("   KEIN 0x42-Aktor gespawnt.\n");
        free(buf);
        return 0;
    }
    printf("   ERGEBNIS: State 1 natuerlich erreicht? %s\n",
           seen_state1 ? "JA" : "NEIN (bleibt State 4/Sub 6)");
    if (seen_state1) printf("   erstes State-1-Frame: F%d\n", first_state1);

    re15_actor_t *n = &g_actors[slot];
    printf("   NPC-Radius hit_radius_min = %u (Original +0x78[6] @0x80121658 = 450)\n",
           n->hit_radius_min);

    /* ---------- (3) REINE CONTAINMENT-MESSUNG (kein erzwungener State) --------------- */
    printf("\n-- (3) CONTAINMENT-VERGLEICH NPC-Pfad vs. Spieler-Pfad (keine State-Erzwingung) --\n");
    for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++) {
        if (!g_scd.props[p].active) continue;
        if (g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0) continue;
        int32_t bx = (int32_t)g_scd.props[p].box_cx + g_scd.props[p].x;
        int32_t bz = (int32_t)g_scd.props[p].box_cz + g_scd.props[p].z;
        int32_t hx = (int32_t)(uint16_t)g_scd.props[p].box_hx;
        int32_t span = hx + 1600;
        printf("  == prop[%d] Mitte(%ld,%ld) hx=%ld hz=%ld — Anlauf in +X-Richtung ==\n",
               p, (long)bx, (long)bz, (long)hx,
               (long)(int32_t)(uint16_t)g_scd.props[p].box_hz);
        re15_collision_set_band(g_scd.props[p].band);
        int32_t npc_x = bx - span, npc_z = bz;
        int32_t pl_x  = bx - span, pl_z  = bz;
        int npc_through = 0, pl_through = 0;
        for (int step = 0; step < 40; step++) {
            int32_t ox = npc_x, oz = npc_z;
            int32_t nx = npc_x + 100, nz = npc_z;
            re15_collision_constrain_enemy(&s_rdt, ox, oz, &nx, &nz, 450, n->y, 4u);
            npc_x = nx; npc_z = nz;

            int32_t px = pl_x + 100, pz = pl_z;
            re15_collision_constrain(&s_rdt, pl_x, pl_z, &px, &pz);
            re15_collision_objects(&px, &pz);
            pl_x = px; pl_z = pz;

            if (in_box(p, npc_x, npc_z, 0)) npc_through = 1;
            if (in_box(p, pl_x, pl_z, 0))   pl_through  = 1;
        }
        printf("     nach 40x100: NPC bei x=%ld (Start %ld, freies Ziel %ld) | Spieler bei x=%ld\n",
               (long)npc_x, (long)(bx - span), (long)(bx - span + 4000), (long)pl_x);
        printf("     JEMALS IM KASTEN?  NPC=%d  Spieler=%d\n", npc_through, pl_through);
    }

    /* ---------- (4) ERZWUNGENE ESKORTE ---------------------------------------------- */
    printf("\n-- (4) ERZWUNGEN (Plc_ret-Handoff, SCD AUS): Eskorte-State 1 gegen prop --\n");
    printf("   ACHTUNG: State/Position werden hier GESETZT. Erreichbarkeit siehe (2).\n");
    int target = -1;
    for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++)
        if (g_scd.props[p].active &&
            !(g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0)) { target = p; break; }
    if (target < 0) { printf("   keine solide Kiste gefunden.\n"); free(buf); return 0; }

    int32_t bx = (int32_t)g_scd.props[target].box_cx + g_scd.props[target].x;
    int32_t bz = (int32_t)g_scd.props[target].box_cz + g_scd.props[target].z;
    int32_t hx = (int32_t)(uint16_t)g_scd.props[target].box_hx;

    s_scd_off = 1;
    /* Alle SCD-Threads stilllegen — sonst haelt die Work_set-Bindung aus main00/sub00 den
     * Slot fest (re15_scd_slot_event_controlled -> die NPC-Wurzel yieldet, enemy_ai_common.c
     * :9503) und der Aktor bewegt sich ueberhaupt nicht. ERZWUNGEN, klar benannt. */
    for (int t = 0; t < SCD_THREAD_COUNT; t++) g_scd.threads[t].active = 0;
    n->walk_active = 0; n->anim_flags = 0;
    n->state = 1; n->sub_state_1 = 1; n->sub_state_2 = 0; n->sub_state_3 = 0;
    n->grid_id = 0; n->walk_active = 0;
    n->x = bx - (hx + 2600); n->z = bz; n->y = g_scd.props[target].y; n->rot_y = 1024;
    pl->x = bx + (hx + 2600); pl->z = bz; pl->y = n->y;
    re15_collision_ensure_band(n->y);
    printf("   Ziel-prop[%d] Mitte=(%ld,%ld) hx=%ld | NPC start (%ld,%ld) | Spieler (%ld,%ld)\n",
           target, (long)bx, (long)bz, (long)hx,
           (long)n->x, (long)n->z, (long)pl->x, (long)pl->z);
    int inside_frames = 0;
    for (int f = 0; f < 200; f++) {
        frame_step();
        int inb = in_box(target, n->x, n->z, 0);
        if (inb) inside_frames++;
        if (f < 6 || (f % 10) == 0 || inb)
            printf("   F%-3d st=%d ss1=%d mo=%d @(%ld,%ld) IN_KISTE=%d dx_zur_Mitte=%ld\n",
                   f, n->state, n->sub_state_1, n->motion,
                   (long)n->x, (long)n->z, inb, (long)(n->x - bx));
    }
    printf("   ERGEBNIS: %d von 200 Frames INNERHALB des Kastens von prop[%d].\n",
           inside_frames, target);

    free(buf);
    return 0;
}
