/*
 * Runde 36 Spur F "zombieki", Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir
 * irgendwie verbessern." Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 2.
 *
 * MESS-SONDE (echte Spielschleife, ROOM11C0, beide Gorillas in der Jagd ab der Original-Lage F195 wie
 * test_r35_affen takt/schrot): Leon laeuft per deterministischem Zufallsgang (alle 90 Bilder neue
 * Blickrichtung, Rennen = UP+CROSS, HP wird je Bild auf 100 gehalten, damit die Jagd nicht endet) N Bilder
 * durch den Parkplatz. Gemessen wird je Gorilla jede Strecke, in der er LEBT, nicht beisst/greift und sich
 * >= 90 Bilder lang um weniger als 40 Einheiten bewegt ("blockiert"): Lage, Abstand zu Leon, Sub-Zustaende,
 * Sicht-Latch (+0x1d0 & 1), Wandklemme (+0x1d6), Zonensprung-Zaehler, naechste SCA-Zellen und ob Leon fuer
 * ihn sichtbar ist (Zellstrahl). Aufruf: test_r36_zombieki_affen_block [bilder] [seed]
 */
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
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_zombieki_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static uint8_t          *s_raw = NULL; static size_t s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static uint8_t            s_blob27[600000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int bank_laden_27(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x27);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x27);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob27);
    if (ok) { memcpy(s_blob27, ems + off, len); if (!eb) eb = re15_enemy_alloc(0x27); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob27, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob27, len, &eb->skel_victim, &eb->anim_victim) == 0); }
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int bilder)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

static int32_t isqrt64(long long v) { long long r = 0; while ((r + 1) * (r + 1) <= v) r++; return (int32_t)r; }

static void zellen_nahe(const re15_actor_t *e, char *out, size_t n)
{
    out[0] = 0;
    int band = (int)e->floor;
    int32_t r = (int32_t)e->hit_radius_min + 60;
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        if (band != (c->floor >> 4)) continue;
        if ((unsigned)(e->x - ((int32_t)c->x - r)) < (unsigned)((int32_t)c->width + r * 2) &&
            (unsigned)(e->z - ((int32_t)c->z - r)) < (unsigned)((int32_t)c->density + r * 2)) {
            size_t len = strlen(out);
            if (len + 60 < n)
                snprintf(out + len, n - len, "%s#%d t%d u0=%02x u1=%02x (%d,%d %dx%d)", len ? " " : "", i, c->type,
                         c->u0, c->u1, (int)c->x, (int)c->z, (int)c->width, (int)c->density);
        }
    }
}

typedef struct { int f0, f1; int32_t x, z; int dist; int subs[32]; int los1; int klemme; int spruenge; int leon_sicht; char zellen[300]; } seg_t;

/* Raum + Baenke + beide Gorillas in der Original-Lage F195 (test_r35_affen takt): beide in der Jagd (Sub 3). */
static int aufbau(re15_actor_t **gout, re15_actor_t **plout)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) { printf("ROOM11C0 nicht ladbar\n"); return 0; }
    if (!bank_laden_27()) { printf("EM027-Bank fehlt\n"); return 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *g[2] = { aktor_vom_typ(0x27, 0), aktor_vom_typ(0x27, 1) };
    if (!g[0] || !g[1]) { printf("zwei Gorillas fehlen\n"); return 0; }
    g[0]->x = -5525; g[0]->z = -14883; g[0]->y = 0; g[0]->rot_y = 2731; g[0]->grid_id = 0x10; g[0]->floor = 0;
    g[0]->state = 1; g[0]->sub_state_1 = 3; g[0]->sub_state_2 = 1; g[0]->sub_state_3 = 0; g[0]->motion = 5; g[0]->anim_frame = 10;
    g[0]->anim_frac = 0; g[0]->dog_blocked_ctr = 9; g[0]->hit_react = 0; g[0]->dog_flags = 1; g[0]->mag_boost = 4;
    g[1]->x = -8915; g[1]->z = -12487; g[1]->y = 0; g[1]->rot_y = 93; g[1]->grid_id = 0x10; g[1]->floor = 0;
    g[1]->state = 1; g[1]->sub_state_1 = 3; g[1]->sub_state_2 = 1; g[1]->sub_state_3 = 0; g[1]->motion = 5; g[1]->anim_frame = 16;
    g[1]->anim_frac = 0; g[1]->dog_blocked_ctr = 15; g[1]->hit_react = 0; g[1]->dog_flags = 1; g[1]->mag_boost = 4;
    pl->hp = 100; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    gout[0] = g[0]; gout[1] = g[1]; *plout = pl;
    return 1;
}

/* Riegel-Szene: Leon fest auf (lx,lz), Gorilla 1 auf (gx,gz) in der Jagd mit Sicht-Latch, Gorilla 2 geparkt. */
typedef struct { int erst, dmin, sub3, spr; } szene_t;
static szene_t szene(int32_t lx, int32_t lz, int32_t gx, int32_t gz, int bilder)
{
    szene_t r = { 0, 1 << 30, 0, 0 };
    re15_actor_t *g[2]; re15_actor_t *pl;
    if (!aufbau(g, &pl)) return r;
    pl->x = lx; pl->z = lz; pl->y = 0; pl->floor = 0;
    g[0]->x = gx; g[0]->z = gz; g[1]->x = 20000; g[1]->z = -20000;
    g[0]->dog_flags = 0; g[0]->sub_state_3 = 1; g[0]->motion = 4;   /* BLINDE Jagd (+0x7 = 1, Latch 0) — der Befund-Zustand, unabhaengig von der Phase des 16-Takt-Sensors */
    pl->rot_y = (int16_t)((((int)re15_atan2_q12(g[0]->z - pl->z, g[0]->x - pl->x) - 0x400)) & 0xfff);
    int prev7 = 0;
    for (int f = 1; f <= bilder; f++) {
        frame(0, 0);
        if (pl->hp < 100) pl->hp = 100;
        re15_actor_t *e = g[0]; int sub = e->sub_state_1 & 31;
        long long dx = e->x - pl->x, dz = e->z - pl->z; int d = isqrt64(dx * dx + dz * dz);
        if (d < r.dmin) r.dmin = d;
        if (sub == 3) r.sub3++;
        if (!r.erst && (sub == 5 || sub == 6 || sub == 8 || sub == 15)) r.erst = f;
        if (sub == 7 && !prev7) r.spr++;
        prev7 = (sub == 7);
    }
    printf("    [szene Ende: Gorilla st=%d/%d/%d/%d @(%d,%d) hp %d los=%d klemme=%d aktiv=%d]\n", g[0]->state, g[0]->sub_state_1, g[0]->sub_state_2,
           g[0]->sub_state_3, (int)g[0]->x, (int)g[0]->z, (int)g[0]->hp, g[0]->dog_flags & 1, g[0]->sca_wall_hit != 0, re15_r36_affen_padsuche_aktiv((int)(g[0] - g_actors)));
    return r;
}

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

int main(int argc, char **argv)
{
    /* "fest <x> <z> [bilder]": Leon steht still auf (x,z) und blickt auf Gorilla 1 — Spur je 30 Bilder + Summe
     * (Abstand min/Ende, erster Angriff, Anteile Sub-Zustaende, Klemme, Sicht, Zonenspruenge). */
    const int fest = (argc > 3 && strcmp(argv[1], "fest") == 0);
    const int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    int bilder = fest ? ((argc > 4) ? atoi(argv[4]) : 1500) : ((argc > 1 && !riegel) ? atoi(argv[1]) : 6000);
    uint32_t seed = (argc > 2 && !fest) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0x1234567u;
    int32_t fest_x = fest ? atoi(argv[2]) : 0, fest_z = fest ? atoi(argv[3]) : 0;


    re15_actor_t *g[2]; re15_actor_t *pl;
    if (!aufbau(g, &pl)) return 1;
    if (fest) {
        pl->x = fest_x; pl->z = fest_z; pl->y = 0; pl->floor = 0;
        if (argc > 6) { g[0]->x = atoi(argv[5]); g[0]->z = atoi(argv[6]); g[1]->x = 20000; g[1]->z = -20000; }   /* Gorilla 1 versetzt, Gorilla 2 geparkt */
        if (argc > 7) { g[0]->sub_state_3 = (uint8_t)atoi(argv[7]); g[0]->dog_flags = g[0]->sub_state_3 ? 0 : 1; g[0]->motion = g[0]->sub_state_3 ? 4 : 5; }   /* +0x7: 0 = Jagd mit Sicht, 1 = blind */
        pl->rot_y = (int16_t)((((int)re15_atan2_q12(g[0]->z - pl->z, g[0]->x - pl->x) - 0x400)) & 0xfff);
        int subs[2][32]; memset(subs, 0, sizeof subs); int klemme[2] = {0,0}, los[2] = {0,0}, spr[2] = {0,0}, sprb[2] = {0,0};
        int dmin[2] = { 1 << 30, 1 << 30 }, erst[2] = {0,0}; int spr_prev2[2] = {0,0};
        for (int f = 1; f <= bilder; f++) {
            frame(0, 0);
            if (pl->hp < 100) pl->hp = 100;
            for (int k = 0; k < 2; k++) {
                re15_actor_t *e = g[k]; int sub = e->sub_state_1 & 31;
                long long dx = e->x - pl->x, dz = e->z - pl->z; int d = isqrt64(dx * dx + dz * dz);
                if (d < dmin[k]) dmin[k] = d;
                subs[k][sub]++; klemme[k] += (e->sca_wall_hit != 0); los[k] += (e->dog_flags & 1);
                if (!erst[k] && (sub == 5 || sub == 6 || sub == 8 || sub == 15)) erst[k] = f;
                if (sub == 7 && !spr_prev2[k]) { spr[k]++; if (e->sub_state_3 == 3) sprb[k]++; }
                spr_prev2[k] = (sub == 7);
            }
            if ((f % 30) == 0) {
                re15_actor_t *e = g[0]; uint8_t zc = 0, zc2 = 0;
                int hit = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc);
                int hit2 = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x20u, &zc2);
                long long dx = e->x - pl->x, dz = e->z - pl->z;
                printf("  F%d G1 st=%d/%d/%d/%d @(%d,%d) rot=%d y=%d los=%d klemme=%d 1dc=%d dist=%d | pad10 %d/%02x pad20 %d/%02x | G2 st=%d/%d/%d/%d @(%d,%d) los=%d klemme=%d\n",
                       f, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y, (int)e->y, e->dog_flags & 1,
                       e->sca_wall_hit != 0, (int)e->dog_blocked_ctr, isqrt64(dx * dx + dz * dz), hit, zc, hit2, zc2,
                       g[1]->state, g[1]->sub_state_1, g[1]->sub_state_2, g[1]->sub_state_3, (int)g[1]->x, (int)g[1]->z, g[1]->dog_flags & 1, g[1]->sca_wall_hit != 0);
            }
        }
        for (int k = 0; k < 2; k++) {
            printf("Gorilla %d: Leon fest @(%d,%d), %d Bilder: Abstand min %d, Ende %d, erster Angriff Bild %d, Klemme %d, Sicht %d, Spruenge %d (blind %d) | Subs:",
                   k + 1, (int)fest_x, (int)fest_z, bilder, dmin[k], (int)isqrt64((long long)(g[k]->x - pl->x) * (g[k]->x - pl->x) + (long long)(g[k]->z - pl->z) * (g[k]->z - pl->z)),
                   erst[k], klemme[k], los[k], spr[k], sprb[k]);
            for (int q = 0; q < 32; q++) if (subs[k][q]) printf(" %d:%d", q, subs[k][q]);
            printf("\n");
        }
        return 0;
    }

    static seg_t segs[2][64]; int nseg[2] = { 0, 0 };
    int32_t ax[2], az[2]; int fa[2]; int subs_akk[2][32]; int los_akk[2], kl_akk[2], spr_akk[2]; int32_t lx[2], lz[2];
    for (int k = 0; k < 2; k++) { ax[k] = g[k]->x; az[k] = g[k]->z; fa[k] = 0; memset(subs_akk[k], 0, sizeof subs_akk[k]); los_akk[k] = kl_akk[k] = spr_akk[k] = 0; lx[k] = g[k]->x; lz[k] = g[k]->z; }
    int ziel_rot = pl->rot_y; int bisse = 0, griffe = 0; int prev_gr = 0; int leon_weg = 0;
    uint32_t rng = seed;
    int spr_prev[2] = { 0, 0 };
    for (int f = 1; f <= bilder; f++) {
        if ((f % 90) == 1) { rng = rng * 1103515245u + 12345u; ziel_rot = (int)((rng >> 8) & 0xfff); }
        uint16_t held = 0;
        int err = ((ziel_rot - (int)(pl->rot_y & 0xfff) + 2048 + 4096) % 4096) - 2048;
        if (err > 48)       held = RE15_PAD_BIT_RIGHT;
        else if (err < -48) held = RE15_PAD_BIT_LEFT;
        else                held = RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS;
        int gr = re15_player_is_grabbed();
        if (gr) held = (uint16_t)((f & 1) ? RE15_PAD_BIT_SQUARE : RE15_PAD_BIT_CROSS);
        int32_t plx0 = pl->x, plz0 = pl->z;
        frame(held, (uint16_t)(gr ? held : 0));
        leon_weg += (int)(labs(pl->x - plx0) + labs(pl->z - plz0));
        if (pl->hp < 100) { if (pl->hp < 40) bisse++; pl->hp = 100; }
        if (gr && !prev_gr) griffe++;
        prev_gr = gr;
        if (argc > 3 && (f % 60) == 0) {          /* trace: Gorilla argv[3] je 60 Bilder */
            int k = atoi(argv[3]); if (k < 0 || k > 1) k = 0;
            re15_actor_t *e = g[k]; uint8_t zc = 0, zc2 = 0;
            int hit = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc);
            int hit2 = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x20u, &zc2);
            int win = hit ? (((((int)(zc & 0xf0u)) << 4) - (int)e->rot_y + 512) & 0xfff) : -1;
            printf("  F%d G%d st=%d/%d/%d/%d @(%d,%d) rot=%d floor=%d los=%d klemme=%d 1dc=%d | pad10 hit=%d zc=%02x win=%d | pad20 hit=%d zc=%02x | Leon (%d,%d) rot=%d dist=%d\n",
                   f, k + 1, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y, (int)e->floor,
                   e->dog_flags & 1, e->sca_wall_hit != 0, (int)e->dog_blocked_ctr, hit, zc, win, hit2, zc2, (int)pl->x, (int)pl->z, (int)pl->rot_y,
                   (int)isqrt64((long long)(e->x - pl->x) * (e->x - pl->x) + (long long)(e->z - pl->z) * (e->z - pl->z)));
        }
        for (int k = 0; k < 2; k++) {
            re15_actor_t *e = g[k];
            int sub = e->sub_state_1 & 31;
            int lebt = (e->state == 1);
            int kampf = (sub == 5 || sub == 6 || sub == 8 || sub == 15);
            int bewegt = (labs(e->x - ax[k]) + labs(e->z - az[k])) >= 40;
            if (e->sub_state_1 == 7 && e->sub_state_3 != 0 && e->sub_state_2 == 0 && !spr_prev[k]) spr_akk[k]++;
            spr_prev[k] = (e->sub_state_1 == 7);
            if (lebt && !kampf && !bewegt) {
                subs_akk[k][sub]++; los_akk[k] += (e->dog_flags & 1) ? 1 : 0; kl_akk[k] += (e->sca_wall_hit != 0) ? 1 : 0;
                if (fa[k] == 0) fa[k] = f;
            } else {
                int dauer = fa[k] ? f - fa[k] : 0;
                if (dauer >= 90 && nseg[k] < 64) {
                    seg_t *s = &segs[k][nseg[k]++];
                    s->f0 = fa[k]; s->f1 = f - 1; s->x = ax[k]; s->z = az[k];
                    long long dx = ax[k] - pl->x, dz = az[k] - pl->z; s->dist = isqrt64(dx * dx + dz * dz);
                    memcpy(s->subs, subs_akk[k], sizeof s->subs); s->los1 = los_akk[k]; s->klemme = kl_akk[k]; s->spruenge = spr_akk[k];
                    s->leon_sicht = !re15_re2_los_cells_blocked(&s_rdt, ax[k], az[k], pl->x, pl->z, (int)e->floor, 4u);
                    zellen_nahe(e, s->zellen, sizeof s->zellen);
                }
                fa[k] = 0; memset(subs_akk[k], 0, sizeof subs_akk[k]); los_akk[k] = kl_akk[k] = spr_akk[k] = 0;
                ax[k] = e->x; az[k] = e->z;
            }
            lx[k] = e->x; lz[k] = e->z;
        }
    }
    printf("ROOM11C0 %d Bilder, Seed 0x%x: Leon-Weg %d, Griffe %d, Bisse(hp<40) %d; Gorillas: (%d,%d) hp %d / (%d,%d) hp %d\n",
           bilder, seed, leon_weg, griffe, bisse, (int)g[0]->x, (int)g[0]->z, (int)g[0]->hp, (int)g[1]->x, (int)g[1]->z, (int)g[1]->hp);
    int gesamt = 0;
    for (int k = 0; k < 2; k++) {
        printf("== Gorilla %d: %d blockierte Strecken >= 90 Bilder\n", k + 1, nseg[k]);
        for (int i = 0; i < nseg[k]; i++) {
            seg_t *s = &segs[k][i]; gesamt += s->f1 - s->f0 + 1;
            printf("  F%d-F%d (%d Bilder) @(%d,%d) Leon-Abstand %d, Leon sichtbar(Zellstrahl) %d | LOS-Latch %d/%d, Klemme %d/%d, Zonenspruenge %d | Subs:",
                   s->f0, s->f1, s->f1 - s->f0 + 1, (int)s->x, (int)s->z, s->dist, s->leon_sicht, s->los1, s->f1 - s->f0 + 1, s->klemme, s->f1 - s->f0 + 1, s->spruenge);
            for (int q = 0; q < 32; q++) if (s->subs[q]) printf(" %d:%d", q, s->subs[q]);
            printf("\n     Zellen: %s\n", s->zellen[0] ? s->zellen : "-");
        }
    }
    printf("blockierte Bilder gesamt: %d von %d (%.1f%%)\n", gesamt, 2 * bilder, 100.0 * gesamt / (2.0 * bilder));
    if (riegel) {
        CHECK(griffe > 0, "Zufallsgang %d Bilder: die Gorillas erreichen Leon (%d Griffe; blockiert %d Bilder)", bilder, griffe, gesamt);
    {
        /* Punkt 2 (PORT-WAHL Pad-Suche, re15_zombieki_r36.h): drei Szenen "Leon noerdlich der Wagenbloecke, Gorilla in
         * der Jagd suedlich/westlich, Sicht faellt". Gegenprobe (Suche aus) = Original-Regel A[3] @0x80117b40-c74:
         * kein Ausgang ohne Sicht -> 1200 Bilder nur Sub 3, kein Angriff, Abstand bleibt. Mit der Suche: Pad ->
         * Sub 4 -> Sprung des Originals -> Angriff. Dazu der Zufallsgang (6000 Bilder, Seed 0x1234567): keine Strecke
         * >= 90 Bilder ohne Bewegung (wie vor der Aenderung). */
        static const int32_t sz[3][4] = { { -7000, 12500, -14000, -3000 }, { -7000, 12500, -5525, -14883 }, { 7000, 4000, -8915, -12487 } };
        /* Leon noerdlich (bzw. oestlich) der Wagenbloecke #2/#7 (Nibble 3), Gorilla suedlich/westlich in der blinden Jagd. */
        for (int i = 0; i < 3; i++) {
            re15_r36_affen_padsuche_enable(0);
            szene_t v = szene(sz[i][0], sz[i][1], sz[i][2], sz[i][3], 1200);
            re15_r36_affen_padsuche_enable(1);
            szene_t n = szene(sz[i][0], sz[i][1], sz[i][2], sz[i][3], 1200);
            printf("Szene %d Leon (%d,%d) Gorilla (%d,%d): VORHER Angriff %d, min %d, Sub3 %d/1200, Spruenge %d | NACHHER Angriff %d, min %d, Sub3 %d/1200, Spruenge %d\n",
                   i + 1, (int)sz[i][0], (int)sz[i][1], (int)sz[i][2], (int)sz[i][3], v.erst, v.dmin, v.sub3, v.spr, n.erst, n.dmin, n.sub3, n.spr);
            CHECK(v.erst == 0 && v.dmin > 6001, "Szene %d Gegenprobe (Original-Regel): 1200 Bilder ohne Angriff, Abstand min %d, Sub 3 %d / Sub 4 %d (Befund)", i + 1, v.dmin, v.sub3, 1200 - v.sub3);
            CHECK(n.erst > 0 && n.erst <= 1200 && n.dmin < 1200, "Szene %d Pad-Suche: Sprung %d, erster Angriff Bild %d (<= 1200), Abstand min %d (< 1200)", i + 1, n.spr, n.erst, n.dmin);
        }
        re15_r36_affen_padsuche_enable(1);
    }
        printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
        return fails ? 1 : 0;
    }
    return 0;
}
