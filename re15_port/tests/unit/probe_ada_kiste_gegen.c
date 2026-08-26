/* probe_ada_kiste_gegen.c — GEGENSONDE zum Befund "ada-kiste" (Linse VAKUANZ).
 *
 * Prueft drei Dinge, die die Originalsonde probe_ada_kiste.c NICHT geprueft hat:
 *  (A) Ist der Spieler-Teil der Messung (3) ueberhaupt eine PROP-Messung? -> derselbe
 *      Anlauf einmal MIT und einmal OHNE re15_collision_objects; plus Wand-Audit
 *      jedes Schritts (Punkt in der Wand?).
 *  (B) Ist Eskorte-State 1 in ROOM1090 NATUERLICH erreichbar? -> echter Bild-Ablauf
 *      (main.c-Reihenfolge, Tuerspawn, Tank-Pad) mit gesetzten Fortschritts-Flags
 *      3/0x81 + 3/0x84 (= Zustand nach dem Feuerloescher, genau das Gate von sub00
 *      @Datei 0x22A6) und LANG genug: sub03 enthaelt >=577 Frames reiner Sleeps.
 *  (C) Laeuft die NPC im NATUERLICHEN Zustand (SCD AN, Plc_ret aus dem Skript,
 *      Spieler per Pad) durch die Kiste prop[0]?
 * Kein add_test, kein Produktivcode geaendert. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static uint8_t            s_blob42[0x80000];

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

/* Ein Bild in der EXAKTEN Reihenfolge des PC-Ports (uebernommen aus
 * probe_1090_trigger_walk.c, das die main.c-Reihenfolge dokumentiert). */
static int s_cine_was_active = 0;
static int s_last_msg_id = -1;
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    s_last_msg_id = id;
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int cine_on(void) { return re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7); }

static void room_boot(int set_progress_flags)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was_active = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    if (set_progress_flags) {          /* Zustand NACH dem Feuerloescher (sub00-Gate 0x22A6) */
        re15_game_flag_set(3, 0x81, 1);
        re15_game_flag_set(3, 0x84, 1);
    }
    scd_room_reenter(&s_rdt, pl->x, pl->z, 3);
    g_scd.cut_auto_enabled = 1;
    for (int f = 0; f < 30; f++) frame(0, 0);
}

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

static int find42(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x42) return s;
    return -1;
}

static int s_npc_slot = -1;
static int s_inside[16];
static int32_t s_mindist[16];
static void npc_sample(int f, int verbose)
{
    if (s_npc_slot < 0) return;
    re15_actor_t *n = &g_actors[s_npc_slot];
    for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++) {
        if (!g_scd.props[p].active) continue;
        if (g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0) continue;
        int32_t bx = (int32_t)g_scd.props[p].box_cx + g_scd.props[p].x;
        int32_t bz = (int32_t)g_scd.props[p].box_cz + g_scd.props[p].z;
        int32_t dx = n->x - bx, dz = n->z - bz;
        int32_t ax = dx < 0 ? -dx : dx, az = dz < 0 ? -dz : dz;
        int32_t d = ax > az ? ax : az;          /* Chebyshev zum Kastenmittelpunkt */
        if (d < s_mindist[p]) s_mindist[p] = d;
        if (in_box(p, n->x, n->z, 0)) {
            if (s_inside[p] == 0 && verbose)
                printf("     >>> F%-4d NPC IN prop[%d]-KASTEN @(%ld,%ld) st=%d ss1=%d\n",
                       f, p, (long)n->x, (long)n->z, n->state, n->sub_state_1);
            s_inside[p]++;
        }
    }
}

static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol, int *fcount)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < (long long)tol*tol) return 1;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        int want = ((int)(ang + 0.5)) & 0x0FFF;
        int err  = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        uint16_t bits = (err > 60) ? RE15_PAD_BIT_RIGHT
                      : (err < -60) ? RE15_PAD_BIT_LEFT
                                    : RE15_PAD_BIT_UP;
        int32_t bx = pl->x, bz = pl->z; int br = pl->rot_y;
        frame(bits, 0);
        if (fcount) (*fcount)++;
        npc_sample(fcount ? *fcount : i, 1);
        if (pl->x == bx && pl->z == bz && pl->rot_y == br) { if (++stuck > 40) return 0; }
        else stuck = 0;
    }
    return 0;
}

int main(void)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", RE15_ASSET_PSX_DIR);
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    printf("=== GEGENSONDE ada-kiste (Linse VAKUANZ) ===\n");
    printf("  EM042-Bank: %s\n", load_bank(0x42, s_blob42, sizeof s_blob42) ? "ok" : "FEHLT");
    room_boot(1);

    for (int p = 0; p < 16; p++) { s_inside[p] = 0; s_mindist[p] = 0x7fffffff; }

    /* ---------------- (A) IST MESSUNG (3) UEBERHAUPT EINE PROP-MESSUNG? ------------- */
    printf("\n-- (A) Spieler-Anlauf MIT vs OHNE re15_collision_objects + Wand-Audit --\n");
    for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++) {
        if (!g_scd.props[p].active) continue;
        if (g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0) continue;
        int32_t bx = (int32_t)g_scd.props[p].box_cx + g_scd.props[p].x;
        int32_t bz = (int32_t)g_scd.props[p].box_cz + g_scd.props[p].z;
        int32_t hx = (int32_t)(uint16_t)g_scd.props[p].box_hx;
        int32_t span = hx + 1600;
        int band = g_scd.props[p].band;
        int32_t py = g_actors[RE15_ACTOR_SLOT_PLAYER].y;
        printf("  == prop[%d] Mitte(%ld,%ld) band=%d objY=%ld | Anlauf y=%ld (Spieler-Y) ==\n",
               p, (long)bx, (long)bz, band, (long)g_scd.props[p].y, (long)py);

        re15_collision_set_band(band);
        int32_t a_x = bx - span, a_z = bz;   /* MIT objects  */
        int32_t b_x = bx - span, b_z = bz;   /* OHNE objects */
        int a_in = 0, b_in = 0, walled = 0;
        for (int step = 0; step < 40; step++) {
            int32_t nx = a_x + 100, nz = a_z;
            re15_collision_constrain(&s_rdt, a_x, a_z, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            a_x = nx; a_z = nz;

            int32_t mx = b_x + 100, mz = b_z;
            re15_collision_constrain(&s_rdt, b_x, b_z, &mx, &mz);
            b_x = mx; b_z = mz;

            /* Wand-Audit: liegt der FREIE Punkt der Bahn in einer Wandzelle? */
            int32_t fx = (bx - span) + 100 * (step + 1), fz = bz;
            int32_t cx = fx, cz = fz;
            re15_collision_constrain(&s_rdt, fx, fz, &cx, &cz);
            if (cx != fx || cz != fz) walled++;

            if (in_box(p, a_x, a_z, 0)) a_in = 1;
            if (in_box(p, b_x, b_z, 0)) b_in = 1;
        }
        printf("     Spieler MIT  objects: Ende x=%ld  jemals im Kasten=%d\n", (long)a_x, a_in);
        printf("     Spieler OHNE objects: Ende x=%ld  jemals im Kasten=%d\n", (long)b_x, b_in);
        printf("     Wand-Audit: %d von 40 Bahnpunkten liegen in einer SCA-Wandzelle\n", walled);
        printf("     -> Prop-Anteil am Spieler-Ergebnis: %s\n",
               (a_x == b_x && a_in == b_in) ? "KEINER (Wand allein erklaert alles)"
                                            : "vorhanden (MIT/OHNE unterscheiden sich)");
    }
    re15_collision_set_band(re15_collision_band_from_y(g_actors[RE15_ACTOR_SLOT_PLAYER].y));

    /* ---------------- (B) NATUERLICHE ERREICHBARKEIT VON ESKORTE-STATE 1 ------------ */
    printf("\n-- (B) NATUERLICHER Ablauf: sub03-Cutscene bis Plc_ret, KEINE State-Erzwingung --\n");
    printf("   (Pad-Confirm nur waehrend der Cutscene-Flags; sub03-Sleeps summieren >=577 Frames)\n");
    int first_state1 = -1, f = 0;
    int last_st = -1, last_ss = -1;
    int cine_seen = 0, cine_end = -1;
    for (; f < 2400; f++) {
        /* Confirm-Edge alle 6 Bilder — der Spieler drueckt sich durch die Dialoge.
         * ⚠️ NICHT an cine_on() gekoppelt: Message_on 0x09 @0x2502 laeuft VOR den
         * Cutscene-Flags @0x2508/0x250C; mit dem Gate blockierte die Sonde ewig. */
        uint16_t edge = ((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        if (cine_on()) cine_seen = 1;
        else if (cine_seen && cine_end < 0) cine_end = f;
        if ((f % 200) == 0)
            printf("   [lauf] F%-4d msg=%d cine=%d pmode=%d Spieler(%ld,%ld)\n",
                   f, s_last_msg_id, cine_on(), (int)g_scd.player_mode,
                   (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                   (long)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        frame(0, edge);
        if (s_npc_slot < 0) s_npc_slot = find42();
        npc_sample(f, 0);
        if (s_npc_slot >= 0) {
            re15_actor_t *n = &g_actors[s_npc_slot];
            if (n->state != last_st || n->sub_state_1 != last_ss) {
                printf("   F%-4d st=%d ss1=%d ss2=%d grid=0x%02X mo=%d @(%ld,%ld,%ld) cine=%d\n",
                       f, n->state, n->sub_state_1, n->sub_state_2, n->grid_id, n->motion,
                       (long)n->x, (long)n->y, (long)n->z, cine_on());
                last_st = n->state; last_ss = n->sub_state_1;
            }
            if (n->state == 1 && first_state1 < 0) first_state1 = f;
        }
        if (first_state1 >= 0 && f > first_state1 + 60) break;
    }
    printf("   Cutscene-Flags gesehen=%d, Ende bei F%d\n", cine_seen, cine_end);
    printf("   ERGEBNIS: Eskorte-State 1 NATUERLICH erreicht? %s",
           first_state1 >= 0 ? "JA" : "NEIN");
    if (first_state1 >= 0) printf("  (erstes Frame F%d)", first_state1);
    printf("\n");
    if (s_npc_slot < 0) { printf("   KEIN 0x42-Aktor.\n"); free(s_raw); return 0; }

    /* ---------------- (C) NATUERLICHE FAHRT AN DER KISTE VORBEI --------------------- */
    printf("\n-- (C) Spieler laeuft per Pad an prop[0] vorbei; folgt die NPC durch die Kiste? --\n");
    re15_actor_t *n = &g_actors[s_npc_slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("   Start: Spieler @(%ld,%ld) NPC @(%ld,%ld) st=%d ss1=%d pmode=%d\n",
           (long)pl->x, (long)pl->z, (long)n->x, (long)n->z, n->state, n->sub_state_1,
           (int)g_scd.player_mode);
    /* Erst die Dialogreste wegdruecken, dann 20 Bilder Diagnose warum/ob der Spieler geht. */
    for (int i = 0; i < 600 && g_re15_pauseflags != 0; i++)
        frame(0, (i % 4) == 0 ? RE15_PAD_BIT_SQUARE : 0);
    printf("   nach Dialog-Wegdruecken: pause=%08lX msg=%d\n",
           (unsigned long)g_re15_pauseflags, s_last_msg_id);
    for (int i = 0; i < 20; i++) {
        int32_t px = pl->x, pz = pl->z; int pr = pl->rot_y;
        frame(RE15_PAD_BIT_UP, 0);
        if (i < 6 || i == 19)
            printf("   [diag] i%-2d msg=%d pause=%08lX pmode=%d wact=%d st=%d mo=%d "
                   "pos=(%ld,%ld) d=(%ld,%ld,%d)\n",
                   i, s_last_msg_id, (unsigned long)g_re15_pauseflags, (int)g_scd.player_mode,
                   (int)pl->walk_active, (int)pl->state, (int)pl->motion,
                   (long)pl->x, (long)pl->z, (long)(pl->x - px), (long)(pl->z - pz),
                   (int)pl->rot_y - pr);
    }
    int32_t bx0 = (int32_t)g_scd.props[0].box_cx + g_scd.props[0].x;
    int32_t bz0 = (int32_t)g_scd.props[0].box_cz + g_scd.props[0].z;
    int fc = 0;
    /* Rund um die Kiste herum — der Verfolger schneidet die Ecke. */
    const int32_t wp[][2] = {
        { -6500, 7000 },        { bx0 - 1700, bz0 + 2400 }, { bx0 - 1700, bz0 - 1800 },
        { bx0 + 1700, bz0 - 1800 }, { bx0 + 1700, bz0 + 2400 }, { bx0 - 1700, bz0 + 2400 },
        { bx0 - 1700, bz0 - 1800 }, { bx0 + 1700, bz0 + 2400 }, { bx0 - 1700, bz0 - 1800 },
        { -3500, 6500 },
    };
    for (unsigned k = 0; k < sizeof wp / sizeof wp[0]; k++) {
        int r = drive_to(wp[k][0], wp[k][1], 500, 500, &fc);
        printf("   wp%u (%ld,%ld): %s | Spieler(%ld,%ld) NPC(%ld,%ld) st=%d ss1=%d\n",
               k, (long)wp[k][0], (long)wp[k][1], r ? "erreicht" : "STUCK",
               (long)pl->x, (long)pl->z, (long)n->x, (long)n->z, n->state, n->sub_state_1);
    }
    printf("\n   === ERGEBNIS (C) nach %d Bildern natuerlicher Fahrt ===\n", fc);
    for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++) {
        if (!g_scd.props[p].active) continue;
        if (g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0) continue;
        printf("   prop[%d]: NPC-Frames IM Kasten = %d | minimale Chebyshev-Distanz zur Mitte = %ld"
               " (Kasten-Halbmass %u)\n",
               p, s_inside[p], (long)s_mindist[p], (unsigned)g_scd.props[p].box_hx);
    }
    /* ---------------- (D) ERREICHBARKEIT DER KISTE — FLUTFUELLUNG ------------------- */
    /* ⛔ CLAUDE.md: fuer Erreichbarkeit IMMER re15_collision_constrain, nie on_floor.
     * Frei(p) := constrain(p -> p) verschiebt p nicht. Uebergang from->to gilt, wenn
     * constrain(from -> to) tatsaechlich auf to landet (das ist die Spieler-Bewegung).
     * KEINE Zustands-Erzwingung: reine Geometrie auf Band 1 (= band_from_y(-1800)). */
#define GW 160
#define GH 200
#define GSTEP 150
    static unsigned char reach[GW * GH];
    printf("\n-- (D) FLUTFUELLUNG vom Tuerspawn: ist die Kiste ueberhaupt begehbarer Boden? --\n");
    {
        const int32_t ORGX = -13500, ORGZ = -18000;
        memset(reach, 0, sizeof reach);
        re15_collision_set_band(1);
        int32_t sx = -10100, sz = 4200;
        int gi = (int)((sx - ORGX) / GSTEP), gj = (int)((sz - ORGZ) / GSTEP);
        static int qx[GW * GH], qz[GW * GH];
        int qh = 0, qt = 0;
        qx[qt] = gi; qz[qt] = gj; qt++;
        reach[gj * GW + gi] = 1;
        while (qh < qt) {
            int ci = qx[qh], cj = qz[qh]; qh++;
            int32_t cxw = ORGX + ci * GSTEP, czw = ORGZ + cj * GSTEP;
            static const int dq[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
            for (int d = 0; d < 4; d++) {
                int ni = ci + dq[d][0], nj = cj + dq[d][1];
                if (ni < 0 || nj < 0 || ni >= GW || nj >= GH) continue;
                if (reach[nj * GW + ni]) continue;
                int32_t txw = ORGX + ni * GSTEP, tzw = ORGZ + nj * GSTEP;
                int32_t rx = txw, rz = tzw;
                re15_collision_constrain(&s_rdt, cxw, czw, &rx, &rz);
                int32_t ex = rx - txw, ez = rz - tzw;
                if (ex < 0) ex = -ex; if (ez < 0) ez = -ez;
                if (ex > 20 || ez > 20) continue;
                reach[nj * GW + ni] = 1;
                qx[qt] = ni; qz[qt] = nj; qt++;
            }
        }
        printf("   erreichbare Zellen ab Tuerspawn (Band 1, Schritt %d): %d\n", GSTEP, qt);
        for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++) {
            if (!g_scd.props[p].active) continue;
            if (g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0) continue;
            int32_t bx = (int32_t)g_scd.props[p].box_cx + g_scd.props[p].x;
            int32_t bz = (int32_t)g_scd.props[p].box_cz + g_scd.props[p].z;
            int32_t hx = (int32_t)(uint16_t)g_scd.props[p].box_hx;
            int32_t hz = (int32_t)(uint16_t)g_scd.props[p].box_hz;
            int in_cells = 0, ring_cells = 0;
            int32_t best = 0x7fffffff;
            for (int j = 0; j < GH; j++) for (int i = 0; i < GW; i++) {
                if (!reach[j * GW + i]) continue;
                int32_t wx = ORGX + i * GSTEP, wz = ORGZ + j * GSTEP;
                int32_t dx = wx - bx, dz = wz - bz;
                int32_t ax = dx < 0 ? -dx : dx, az = dz < 0 ? -dz : dz;
                int32_t cheb = ax > az ? ax : az;
                if (cheb < best) best = cheb;
                if (ax <= hx && az <= hz) in_cells++;
                else if (ax <= hx + 450 && az <= hz + 450) ring_cells++;
            }
            printf("   prop[%d] Mitte(%ld,%ld) h=(%ld,%ld) band=%d:"
                   " erreichbare Zellen IM Kasten=%d, im 450-Ring=%d, naechste Zelle Cheb=%ld\n",
                   p, (long)bx, (long)bz, (long)hx, (long)hz, g_scd.props[p].band,
                   in_cells, ring_cells, (long)best);
        }
    }

    free(s_raw);
    return 0;
}
