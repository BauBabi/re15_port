/* probe_verify_step_tunnel.c — NACHMESSUNG (kein Fix) der Behauptung:
 *   "Groesster Tick-Schritt ueber alle sechs ROOM1030-Zombies x 3000 Bilder: 443"
 *   "duennste solide Band-0-Zelle = #19, 358x1093 -> mit 2r=800 aufgeblasen 1158"
 *   "443 < 1158 -> kein Ueberspringen einer Zelle"
 *
 * Statt die Folgerung zu glauben, misst diese Sonde DIREKT, ob eine Bewegungs-STRECKE
 * (prev -> now) die um r aufgeblasene Box einer band-/masken-passenden Zelle durchquert,
 * OHNE dass einer der beiden Endpunkte darin liegt (= genau der Fall, den der Punkt-Test
 * verfehlt).
 *
 * Broad-Phase-Box (byte-true @0x8003b41c-478, PUSH-Zweig):
 *   x in [cell.x - r, cell.x + width + r)   z in [cell.z - r, cell.z + density + r)
 * (Der in mehreren Sonden zitierte Bereich @0x8003b284-94 ist der NICHT-soliden 0x20-Flag-
 *  Zweig; `bne v1,zero,LAB_8003b2e0` @0x8003b258 springt bei (maske & u0) != 0 dort weg.)
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define MAXF 3000

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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e) { return e->active && re15_re2z_owns_type(e->type); }

/* Segment [p0,p1] gegen halb-offenes AABB [lo,hi) — Slab-Verfahren. */
static int seg_hits_box(double x0, double z0, double x1, double z1,
                        double lox, double hix, double loz, double hiz,
                        double *t_in, double *t_out)
{
    double t0 = 0.0, t1 = 1.0;
    double d[2]; double p[2]; double lo[2]; double hi[2];
    d[0] = x1 - x0; d[1] = z1 - z0;
    p[0] = x0;      p[1] = z0;
    lo[0] = lox;    lo[1] = loz;
    hi[0] = hix;    hi[1] = hiz;
    for (int a = 0; a < 2; a++) {
        if (d[a] == 0.0) { if (p[a] < lo[a] || p[a] >= hi[a]) return 0; continue; }
        double ta = (lo[a] - p[a]) / d[a], tb = (hi[a] - p[a]) / d[a];
        if (ta > tb) { double t = ta; ta = tb; tb = t; }
        if (ta > t0) t0 = ta;
        if (tb < t1) t1 = tb;
        if (t0 > t1) return 0;
    }
    if (t_in) *t_in = t0;
    if (t_out) *t_out = t1;
    return 1;
}

static int in_box(double x, double z, double lox, double hix, double loz, double hiz)
{ return (x >= lox && x < hix && z >= loz && z < hiz); }

typedef struct { int32_t x, z, y, r; uint8_t mask; } rec_t;
static rec_t s_rec[RE15_ACTOR_MAX][MAXF];
static int   s_live[RE15_ACTOR_MAX];

static void run(re15_ai_flavor_t fl, const char *name, int frames)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    if (fl == RE15_AI_FLAVOR_RE2) { (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16); }
    else { (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); (void)load_bank_re15(0x16); }

    memset(s_live, 0, sizeof s_live);
    int32_t x0[RE15_ACTOR_MAX], z0[RE15_ACTOR_MAX];
    int nzc = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        s_live[s] = 1; x0[s] = g_actors[s].x; z0[s] = g_actors[s].z; nzc++;
    }
    printf("\n=== %s === %d Zombies gespawnt, %d Bilder\n", name, nzc, frames);
    printf("  Spawn:");
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s_live[s]) printf("  z%d=(%ld,%ld)", s, (long)x0[s], (long)z0[s]);
    printf("\n");

    if (frames > MAXF) frames = MAXF;
    for (int f = 0; f < frames; f++) {
        frame_step();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            const re15_actor_t *e = &g_actors[s];
            s_rec[s][f].x = e->x; s_rec[s][f].z = e->z; s_rec[s][f].y = e->y;
            s_rec[s][f].mask = (uint8_t)(e->sca_mask ? e->sca_mask : 4u);
            s_rec[s][f].r = (int32_t)e->hit_radius_min;
        }
    }

    /* ---- (1) Schrittgroessen -------------------------------------------------------------- */
    printf("  %-6s %-6s %-22s %-22s %s\n", "slot", "typ", "max |dx|+|dz|", "max euklid", "max euklid ab Bild 1");
    long gm = -1, ge2 = -1, ge2_1 = -1; int gmf = -1, gms = -1, gef = -1, ges = -1, gef1 = -1, ges1 = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        long bm = -1, be2 = -1, be2_1 = -1; long bmf = -1, bef = -1, bef1 = -1;
        int32_t lx = x0[s], lz = z0[s];
        for (int f = 0; f < frames; f++) {
            long dx = (long)s_rec[s][f].x - lx, dz = (long)s_rec[s][f].z - lz;
            long m = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            long e2 = dx * dx + dz * dz;
            if (m > bm) { bm = m; bmf = f; }
            if (e2 > be2) { be2 = e2; bef = f; }
            if (f >= 1 && e2 > be2_1) { be2_1 = e2; bef1 = f; }
            lx = s_rec[s][f].x; lz = s_rec[s][f].z;
        }
        printf("  z%-5d 0x%02x   %6ld @f%-12ld %6.0f @f%-12ld %6.0f @f%ld\n",
               s, g_actors[s].type, bm, bmf, sqrt((double)be2), bef, sqrt((double)be2_1), bef1);
        if (bm > gm)    { gm = bm; gmf = (int)bmf; gms = s; }
        if (be2 > ge2)  { ge2 = be2; gef = (int)bef; ges = s; }
        if (be2_1 > ge2_1) { ge2_1 = be2_1; gef1 = (int)bef1; ges1 = s; }
    }
    printf("  -> GLOBAL max |dx|+|dz|        = %ld (Slot %d, Bild %d)\n", gm, gms, gmf);
    printf("  -> GLOBAL max euklid           = %.1f (Slot %d, Bild %d)\n", sqrt((double)ge2), ges, gef);
    printf("  -> GLOBAL max euklid ab Bild 1 = %.1f (Slot %d, Bild %d)\n", sqrt((double)ge2_1), ges1, gef1);

    /* ---- (2) DIREKTER Tunnel-Test ---------------------------------------------------------- */
    long tunnels = 0, corner_only = 0; double worst_chord = 0.0;
    int t_f = -1, t_s = -1, t_c = -1; double t_chord = 0.0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int32_t lx = x0[s], lz = z0[s];
        for (int f = 0; f < frames; f++) {
            int32_t nx = s_rec[s][f].x, nzz = s_rec[s][f].z;
            int32_t r = s_rec[s][f].r; unsigned mask = s_rec[s][f].mask;
            int band = re15_collision_band_from_y(s_rec[s][f].y);
            for (int i = 0; i < g_room_rdt.sca_count; i++) {
                const re15_sca_entry_t *c = &g_room_rdt.sca[i];
                double t_in = 0.0, t_out = 1.0;
                if (band != (c->floor >> 4)) continue;
                if ((mask & c->u0) == 0) continue;
                double lox = (double)c->x - r, hix = (double)c->x + (double)c->width + r;
                double loz = (double)c->z - r, hiz = (double)c->z + (double)c->density + r;
                if (in_box(lx, lz, lox, hix, loz, hiz)) continue;
                if (in_box(nx, nzz, lox, hix, loz, hiz)) continue;
                if (!seg_hits_box(lx, lz, nx, nzz, lox, hix, loz, hiz, &t_in, &t_out)) continue;
                tunnels++;
                double dx = (double)nx - lx, dz = (double)nzz - lz;
                double chord = (t_out - t_in) * sqrt(dx * dx + dz * dz);
                if (chord > worst_chord) worst_chord = chord;
                if (chord < 50.0) corner_only++;
                if (t_f < 0) { t_f = f; t_s = s; t_c = i; t_chord = chord; }
            }
            lx = nx; lz = nzz;
        }
    }
    printf("  -> TUNNEL-Ereignisse (Strecke quert Zelle, BEIDE Endpunkte draussen): %ld\n", tunnels);
    if (tunnels)
        printf("     erstes: Bild %d Slot %d Zelle #%d Sehne %.1f | laengste verpasste Sehne = %.1f "
               "| davon Eck-Streifer (<50) = %ld\n", t_f, t_s, t_c, t_chord, worst_chord, corner_only);

    /* ---- (2b) Detail zu jedem Ereignis mit Sehne >= 50, plus PUSH-SIGNATUR-Test.
     * push_rect legt die Endposition EXAKT 0x12 (=18) ausserhalb einer Kante der
     * aufgeblasenen Box ab (re15_collision.c:57-64). Liegt das Ende NICHT auf einer
     * solchen Kante, war es kein Push dieser Zelle. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int32_t lx = x0[s], lz = z0[s];
        for (int f = 0; f < frames; f++) {
            int32_t nx = s_rec[s][f].x, nzz = s_rec[s][f].z;
            int32_t r = s_rec[s][f].r; unsigned mask = s_rec[s][f].mask;
            int band = re15_collision_band_from_y(s_rec[s][f].y);
            for (int i = 0; i < g_room_rdt.sca_count; i++) {
                const re15_sca_entry_t *c = &g_room_rdt.sca[i];
                double t_in = 0.0, t_out = 1.0;
                if (band != (c->floor >> 4)) continue;
                if ((mask & c->u0) == 0) continue;
                double lox = (double)c->x - r, hix = (double)c->x + (double)c->width + r;
                double loz = (double)c->z - r, hiz = (double)c->z + (double)c->density + r;
                if (in_box(lx, lz, lox, hix, loz, hiz)) continue;
                if (in_box(nx, nzz, lox, hix, loz, hiz)) continue;
                if (!seg_hits_box(lx, lz, nx, nzz, lox, hix, loz, hiz, &t_in, &t_out)) continue;
                double dx = (double)nx - lx, dz = (double)nzz - lz;
                double chord = (t_out - t_in) * sqrt(dx * dx + dz * dz);
                if (chord < 50.0) continue;
                int pushsig = (nx == (int32_t)lox - 18) || (nx == (int32_t)hix + 18)
                           || (nzz == (int32_t)loz - 18) || (nzz == (int32_t)hiz + 18);
                printf("     [Detail] f%-5d z%d Zelle#%-3d (%ux%u) r=%d maske=%u: "
                       "(%ld,%ld)->(%ld,%ld) d=(%ld,%ld) Sehne=%.1f  Push-Signatur=%s\n",
                       f, s, i, c->width, c->density, r, mask,
                       (long)lx, (long)lz, (long)nx, (long)nzz, (long)dx, (long)dz, chord,
                       pushsig ? "JA (Push dieser Zelle)" : "NEIN (Zelle nie gesehen)");
            }
            lx = nx; lz = nzz;
        }
    }

    /* ---- (2c) Kontext um den groessten |dx|+|dz|-Schritt --------------------------------- */
    if (gms > 0) {
        printf("     [Kontext] groesster |dx|+|dz|-Schritt: Slot %d Bild %d\n", gms, gmf);
        for (int f = (gmf > 3 ? gmf - 3 : 0); f <= gmf + 3 && f < frames; f++)
            printf("        f%-5d (%ld,%ld) r=%d maske=%u y=%ld\n", f,
                   (long)s_rec[gms][f].x, (long)s_rec[gms][f].z,
                   s_rec[gms][f].r, s_rec[gms][f].mask, (long)s_rec[gms][f].y);
    }

    /* ---- (3) duennste solide Band-0-Zelle --------------------------------------------------- */
    {
        int best = -1, bestv = 1 << 30;
        for (int i = 0; i < g_room_rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[i];
            if ((c->floor >> 4) != 0) continue;
            if ((4u & c->u0) == 0 && (8u & c->u0) == 0) continue;
            int mn = c->width < c->density ? (int)c->width : (int)c->density;
            if (mn < bestv) { bestv = mn; best = i; }
        }
        if (best >= 0) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[best];
            printf("  -> duennste solide Band-0-Zelle: #%d typ=%d %ux%u u0=0x%02x -> schmale Achse %d; "
                   "mit r=400 aufgeblasen %d\n", best, c->type, c->width, c->density, c->u0,
                   bestv, bestv + 800);
            for (int i = 0; i < g_room_rdt.sca_count; i++) {
                const re15_sca_entry_t *cc = &g_room_rdt.sca[i];
                int mn = cc->width < cc->density ? (int)cc->width : (int)cc->density;
                if (mn == bestv && i != best) printf("     (gleichauf: #%d %ux%u)\n", i, cc->width, cc->density);
            }
        }
    }
}

int main(void)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    run(RE15_AI_FLAVOR_RE15, "LAUF RE1.5", 3000);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "LAUF RE2",   3000);
    free(buf); g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== MESSUNG FERTIG ==\n");
    return 0;
}
