/* probe_1030_grenzzyklus.c — WO GENAU entsteht der Grenzzyklus des ROOM1030-Steckenbleibers,
 * seit der RE2-Init-Setzer B (@0x801008D8-0x80100950) jedem dritten Zombie das Tempo-Bit
 * +0x21A|0x8000 gibt?
 *
 * MESSAUFTRAG (Runde 12):
 *   1. Bild-fuer-Bild-Protokoll des Steckenbleibers: Position bei Bildbeginn (= das `old` der
 *      Klemme, enemy_ai_common.c:13657 sweep_ox/sweep_oz), Position bei Bildende (= nach der
 *      Klemme), Schrittweite, Yaw, Keyframe, Steer-Ziel, Kontakt-Byte +0x90, Zell-Attribut +0x1b4.
 *   2. Welche SCA-Zelle hat geschoben — Index, Typ, Rechteck. Zwei unabhaengige Wege:
 *      (a) Beobachtung: Broadphase-Ueberdeckung an der ENDposition (derselbe Filter wie
 *          collision_constrain_impl, re15_collision.c:726-730).
 *      (b) Grundwahrheit: RE15_COLL_TRACE=1, der Motor druckt (vorher)->(nachher) je Zelle
 *          (re15_collision.c:731-742). Das Fenster ist eng, weil der Zaehler dort bei 200 deckelt.
 *   3. A/B mit und ohne Tempo-Bit an derselben Stelle.
 *   4. Streckentest: klemmt der Port nur den ZIELPUNKT? Dann muss die duennste Zelle der Bahn
 *      gegen die groesste gemessene Schrittweite gehalten werden.
 *
 * Diese Sonde AENDERT KEINEN Motorcode. Das Tempo-Bit wird ausschliesslich von aussen am
 * Aktor-Feld gesetzt/geloescht (e->re2z_flags21a), die Zufallsfolge bleibt damit identisch.
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

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define GATE_Z_NORTH (-22771)
#define MAXF 3200

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
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

/* ---- Bild-Protokoll --------------------------------------------------------------------- */
typedef struct {
    int32_t  sx, sz;      /* Position bei BILDBEGINN  = das old der Klemme (:13657)          */
    int32_t  ex, ez;      /* Position bei BILDENDE    = nach der Klemme                      */
    int16_t  ry;          /* +0x6A Yaw                                                        */
    int16_t  stx, stz;    /* +0x1BC/+0x1BE Steer-Ziel                                         */
    int16_t  mot;         /* +0x94 Clip                                                       */
    int32_t  kf;          /* +0x14D Keyframe                                                  */
    uint8_t  st, s1, s2;
    uint8_t  contact;     /* +0x90                                                            */
    uint16_t cellattr;    /* +0x1B4 Attributwort der zuletzt treffenden Zelle                 */
    uint8_t  wallhit;     /* Rueckgabe FUN_8003b0a4                                           */
    uint8_t  sca;         /* +0x1D7                                                           */
    uint16_t f21a;        /* +0x21A                                                           */
    uint16_t f10e;
    uint32_t dist;
    uint16_t rr;          /* +0x78[6] hit_radius_min = das r der Klemme                     */
    uint16_t aflags;      /* +0x1C4                                                          */
    uint8_t  m0b;         /* +0x0B AOT-Zonenstempel                                          */
    int16_t  pkf;         /* root_prev_kf = der Anker, gegen den move_root die Differenz bildet */
    int16_t  pmot;        /* root_prev_motion                                                  */
} fr_t;

static fr_t s_fr[RE15_ACTOR_MAX][MAXF];
static fr_t s_ref[RE15_ACTOR_MAX][MAXF];    /* Kopie von LAUF A fuer den A/B-Vergleich */
static int  s_live[RE15_ACTOR_MAX];

/* Steuerung von aussen */
static int  s_nudge_slot = -1, s_nudge_frame = -1; static int32_t s_nudge_x = 0;
static int  s_force_bit_slot = -1;   /* diesem Slot das Tempo-Bit jeden Frame AUFZWINGEN */
static int  s_kill_speedbit = 0;       /* jeden Frame +0x21A &= ~0x8000 bei ALLEN Zombies */
static int  s_trace_from = -1, s_trace_to = -1, s_trace_slot = -1;
static uint32_t s_seed = 0x2545f491u;
static int  s_quiet = 0;
static int  s_pre_draws = 0;   /* zusaetzliche Wuerfe am RE2-Generator nach dem Raumladen */
static int  s_stamp_dump = 0;  /* Bilder 460..490 aller Slots ausdrucken                  */
static int  s_f_arm[RE15_ACTOR_MAX];    /* erstes Bild mit +0x1C4 & 0x1000        */
static int  s_f_crawl[RE15_ACTOR_MAX];  /* erstes Bild mit +0x1D7 == 8 (Kriecher) */

typedef struct {
    int spawned, armed, crossed, stuck_slot;
    double stuck_path, stuck_net;
    int frames;
} out_t;

static void run(const char *name, int frames, out_t *out)
{
    if (out) { memset(out, 0, sizeof *out); out->stuck_slot = -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(s_seed);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    /* PHASEN-VERSCHIEBUNG des RE2-Generators (@0x80015FE8, Zustand 0x800CE318, Saat
     * 0xD2706CA4 beim Raumladen): genau der Effekt, den die zwei/drei Extra-Wuerfe des
     * Setzers B haben. Damit laesst sich messen, ob die Scharfschalt-Zahl ueberhaupt an
     * der Wurf-Phase haengt. */
    for (int d = 0; d < s_pre_draws; d++) (void)re15_re2_rand();

    memset(s_live, 0, sizeof s_live);
    int32_t z0[RE15_ACTOR_MAX], x0[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        s_live[s] = 1; z0[s] = g_actors[s].z; x0[s] = g_actors[s].x;
    }
    if (out) for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s_live[s]) out->spawned++;

    if (!s_quiet) printf("\n=== %s ===\n", name);
    if (frames > MAXF) frames = MAXF;
    if (out) out->frames = frames;

    int armed_seen[RE15_ACTOR_MAX]; memset(armed_seen, 0, sizeof armed_seen);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) { s_f_arm[s] = -1; s_f_crawl[s] = -1; }

    for (int f = 0; f < frames; f++) {
        if (s_nudge_slot > 0 && f == s_nudge_frame && s_live[s_nudge_slot])
            g_actors[s_nudge_slot].x = s_nudge_x;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            if (s_kill_speedbit)  g_actors[s].re2z_flags21a &= (uint16_t)~0x8000u;
            if (s == s_force_bit_slot) g_actors[s].re2z_flags21a |= 0x8000u;
            s_fr[s][f].sx = g_actors[s].x; s_fr[s][f].sz = g_actors[s].z;
        }
        if (s_trace_slot > 0 && f == s_trace_from) putenv((char *)"RE15_COLL_TRACE=1");
        if (s_trace_slot > 0 && f >= s_trace_from && f <= s_trace_to) {
            fflush(stdout);
            fprintf(stderr, "[F%d] slot%d START (%ld,%ld) yaw=%d kf=%ld clip=%d\n",
                    f, s_trace_slot, (long)g_actors[s_trace_slot].x,
                    (long)g_actors[s_trace_slot].z, (int)g_actors[s_trace_slot].rot_y,
                    (long)g_actors[s_trace_slot].anim_frame, (int)g_actors[s_trace_slot].motion);
            fflush(stderr);
        }
        frame_step();
        if (s_trace_slot > 0 && f == s_trace_to) { putenv((char *)"RE15_COLL_TRACE="); fflush(stderr); }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            const re15_actor_t *e = &g_actors[s];
            fr_t *r = &s_fr[s][f];
            r->ex = e->x; r->ez = e->z; r->ry = e->rot_y;
            r->stx = e->steer_x; r->stz = e->steer_z;
            r->mot = e->motion; r->kf = e->anim_frame;
            r->st = e->state; r->s1 = e->sub_state_1; r->s2 = e->sub_state_2;
            r->contact = e->ai_contact; r->cellattr = e->coll_cell_attr;
            r->wallhit = e->sca_wall_hit; r->sca = e->sca_mask;
            r->f21a = e->re2z_flags21a; r->f10e = e->re2z_f10e; r->dist = e->ai_dist;
            r->rr = e->hit_radius_min; r->aflags = e->anim_flags; r->m0b = e->member_0b;
            r->pkf = e->root_prev_kf; r->pmot = e->root_prev_motion;
            if (e->anim_flags & 0x1000u) { armed_seen[s] = 1; if (s_f_arm[s] < 0) s_f_arm[s] = f; }
            if (e->sca_mask == 8u && s_f_crawl[s] < 0) s_f_crawl[s] = f;
        }
    }

    if (s_stamp_dump) {
        printf("\n  --- Scharfschalt-Fenster: Bilder 470..480, alle Slots ---\n");
        printf("  %-5s", "f");
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s_live[s]) printf("  z%-30d", s);
        printf("\n");
        for (int f = 468; f <= 480 && f < frames; f++) {
            printf("  %-5d", f);
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                if (!s_live[s]) continue;
                const fr_t *r = &s_fr[s][f];
                printf("  (%7ld,%8ld)m0b=%-2u 0x%04x", (long)r->ex, (long)r->ez,
                       r->m0b, r->aflags);
            }
            printf("\n");
        }
    }
    if (!s_quiet) printf("  %-5s %-6s %-20s %-20s %-9s %-9s %-6s %-7s %s\n",
           "slot", "typ", "start", "ende", "Weg", "Netto", "0x21A", "scharf", "Riegel");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        double path = 0.0; int32_t lx = x0[s], lz = z0[s];
        int32_t maxstep = 0; double sumstep = 0; int nstep = 0; int maxstep_f = -1;
        for (int f = 0; f < frames; f++) {
            double dx = (double)(s_fr[s][f].ex - lx), dz = (double)(s_fr[s][f].ez - lz);
            path += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            lx = s_fr[s][f].ex; lz = s_fr[s][f].ez;
            /* SCHRITTWEITE DES TICKS = Bildende minus Bildbeginn (die Klemme ist drin) */
            int32_t tx = s_fr[s][f].ex - s_fr[s][f].sx, tz = s_fr[s][f].ez - s_fr[s][f].sz;
            int32_t ad = (tx < 0 ? -tx : tx) + (tz < 0 ? -tz : tz);
            if (ad > maxstep) { maxstep = ad; maxstep_f = f; }
            if (ad) { sumstep += ad; nstep++; }
        }
        int32_t ex = s_fr[s][frames-1].ex, ez = s_fr[s][frames-1].ez;
        double nx = (double)(ex - x0[s]); if (nx < 0) nx = -nx;
        double nz = (double)(ez - z0[s]); if (nz < 0) nz = -nz;
        int crossed = (z0[s] <= GATE_Z_NORTH && ez > GATE_Z_NORTH);
        if (out) {
            out->armed += armed_seen[s]; out->crossed += crossed;
            if (!crossed && z0[s] <= GATE_Z_NORTH && out->stuck_slot < 0) {
                out->stuck_slot = s; out->stuck_path = path; out->stuck_net = nx + nz;
            }
        }
        if (!s_quiet)
            printf("  z%-4d 0x%02x   (%7ld,%8ld) (%7ld,%8ld) %9.0f %9.0f 0x%04x %-7d %-20s "
                   "scharf@%-5d kriech@%-5d maske=%u  Schritt max=%ld@f%d mittel=%.1f\n",
                   s, g_actors[s].type, (long)x0[s], (long)z0[s], (long)ex, (long)ez,
                   path, nx + nz, s_fr[s][frames-1].f21a, armed_seen[s],
                   crossed ? "DURCH" : (z0[s] > GATE_Z_NORTH ? "(startete noerdlich)"
                                                            : "*** STECKT ***"),
                   s_f_arm[s], s_f_crawl[s], s_fr[s][frames-1].sca,
                   (long)maxstep, maxstep_f, nstep ? sumstep / nstep : 0.0);
    }
}

/* ---- Beobachtung: welche Zellen ueberdecken einen Punkt? ----------------------------------
 * SELBER Filter wie collision_constrain_impl (re15_collision.c:718-730): Quadrantenwahl
 * quadrant_of (FUN_8003b068), Band strikt ==, (mask & u0) != 0, Broadphase um r aufgeblasen. */
static int cells_at(int32_t x, int32_t z, int band, int32_t r, unsigned mask,
                    int *idx, int n_max)
{
    unsigned zb = (unsigned)(z - (int32_t)g_room_rdt.ceiling_z) & 0x80000000u;
    unsigned xb = (unsigned)(x - (int32_t)g_room_rdt.ceiling_x) & 0x80000000u;
    int q = (int)((zb | (xb >> 1)) >> 0x1e);
    int st = 0; for (int i = 0; i < q && i < 5; i++) st += g_room_rdt.sca_rgn[i];
    int en = st + (q < 5 ? g_room_rdt.sca_rgn[q] : 0);
    if (en > g_room_rdt.sca_count) en = g_room_rdt.sca_count;
    int n = 0;
    for (int i = st; i < en; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if (band != (c->floor >> 4)) continue;
        if ((mask & c->u0) == 0) continue;
        if ((unsigned)(x - ((int32_t)c->x - r)) < (unsigned)((int32_t)c->width   + r * 2) &&
            (unsigned)(z - ((int32_t)c->z - r)) < (unsigned)((int32_t)c->density + r * 2)) {
            if (n < n_max) idx[n] = i;
            n++;
        }
    }
    return n;
}

static void dump_cycle(int slot, int f0, int f1, int step, const char *title)
{
    const re15_actor_t *e = &g_actors[slot];
    int band = re15_collision_band_from_y(e->y);
    int32_t r = (int32_t)e->hit_radius_min;
    unsigned mask = e->sca_mask ? e->sca_mask : 4u;
    printf("\n  --- %s: Slot %d, Bilder %d..%d (band=%d r=%ld maske=%u) ---\n",
           title, slot, f0, f1, band, (long)r, mask);
    printf("  Steer-Ziel (+0x1BC/+0x1BE) am Fensteranfang: (%d,%d)\n",
           s_fr[slot][f0].stx, s_fr[slot][f0].stz);
    printf("  %-6s %-9s %-9s %-9s %-9s %-5s %-5s %-4s %-5s %-4s %-4s %-3s %-5s %-9s %-5s %s\n",
           "f", "startX", "startZ", "endeX", "endeZ", "dX", "dZ", "|d|", "yaw", "clip",
           "kf", "hit", "+0x90", "st/s1/s2", "maske",
           "Zellen an STARTpos | #28-Suedkante/Push-Ebene");
    for (int f = f0; f <= f1; f += step) {
        const fr_t *r0 = &s_fr[slot][f];
        int32_t dx = r0->ex - r0->sx, dz = r0->ez - r0->sz;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        int32_t rf = r0->rr ? (int32_t)r0->rr : r;
        unsigned mf = r0->sca ? r0->sca : 4u;
        int idx[8]; int n = cells_at(r0->sx, r0->sz, band, rf, mf, idx, 8);
        printf("  %-6d %-9ld %-9ld %-9ld %-9ld %-5ld %-5ld %-4ld %-5d %-4d %-4ld %-3u 0x%02x  "
               "%u/%u/%-5u %-5u ",
               f, (long)r0->sx, (long)r0->sz, (long)r0->ex, (long)r0->ez,
               (long)dx, (long)dz, (long)ad, r0->ry, r0->mot, (long)r0->kf,
               r0->wallhit, r0->contact, r0->st, r0->s1, r0->s2, mf);
        for (int i = 0; i < n && i < 8; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[idx[i]];
            printf("#%d/t%d ", idx[i], c->type);
        }
        if (n == 0) printf("- ");
        if (28 < g_room_rdt.sca_count) {
            const re15_sca_entry_t *c28 = &g_room_rdt.sca[28];
            printf("| Kante=%ld Ebene=%ld ende-Kante=%ld",
                   (long)((int32_t)c28->z - rf), (long)((int32_t)c28->z - rf - 0x12),
                   (long)(r0->ez - ((int32_t)c28->z - rf)));
        }
        printf("\n");
    }
    {
        int seen[64]; int ns = 0;
        for (int f = f0; f <= f1; f++) {
            const fr_t *r0 = &s_fr[slot][f];
            int idx[8]; int n = cells_at(r0->ex, r0->ez, band, r, mask, idx, 8);
            for (int i = 0; i < n && i < 8; i++) {
                int dup = 0; for (int k = 0; k < ns; k++) if (seen[k] == idx[i]) dup = 1;
                if (!dup && ns < 64) seen[ns++] = idx[i];
            }
        }
        printf("  Beteiligte Zellen:\n");
        for (int k = 0; k < ns; k++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[seen[k]];
            printf("    Zelle#%-3d typ=%-2d band=%d u0=0x%02x u1=0x%02x  x[%d..%d] z[%d..%d]  "
                   "%dx%d  (mit r=%ld: x[%ld..%ld] z[%ld..%ld])\n",
                   seen[k], c->type, (int)(c->floor >> 4), (unsigned)c->u0, (unsigned)c->u1,
                   (int)c->x, (int)c->x + (int)c->width, (int)c->z, (int)c->z + (int)c->density,
                   (int)c->width, (int)c->density, (long)r,
                   (long)((int32_t)c->x - r), (long)((int32_t)c->x + (int32_t)c->width + r),
                   (long)((int32_t)c->z - r), (long)((int32_t)c->z + (int32_t)c->density + r));
        }
    }
}

/* Zyklus-Erkennung: kleinste Periode p, fuer die (ex,ez) im Fenster [f0..f1] p-periodisch ist */
static int find_period(int slot, int f0, int f1)
{
    for (int p = 1; p <= (f1 - f0) / 3; p++) {
        int ok = 1;
        for (int f = f0; f + p <= f1; f++) {
            if (s_fr[slot][f].ex != s_fr[slot][f + p].ex ||
                s_fr[slot][f].ez != s_fr[slot][f + p].ez) { ok = 0; break; }
        }
        if (ok) return p;
    }
    return -1;
}

static void stats(int slot, int f0, int f1, const char *tag)
{
    double path = 0; int32_t mn_x = 0x7fffffff, mx_x = -0x7fffffff;
    int32_t mn_z = 0x7fffffff, mx_z = -0x7fffffff;
    int32_t maxstep = 0; int nmove = 0, nhit = 0;
    for (int f = f0; f <= f1; f++) {
        const fr_t *r = &s_fr[slot][f];
        int32_t dx = r->ex - r->sx, dz = r->ez - r->sz;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        path += ad; if (ad > maxstep) maxstep = ad; if (ad) nmove++;
        if (r->wallhit) nhit++;
        if (r->ex < mn_x) mn_x = r->ex; if (r->ex > mx_x) mx_x = r->ex;
        if (r->ez < mn_z) mn_z = r->ez; if (r->ez > mx_z) mx_z = r->ez;
    }
    int32_t net = (s_fr[slot][f1].ex - s_fr[slot][f0].ex);
    int32_t netz = (s_fr[slot][f1].ez - s_fr[slot][f0].ez);
    printf("  [%s] Slot %d Bilder %d..%d: Weg=%.0f  Netto=(%ld,%ld)  bewegte Bilder=%d/%d  "
           "Wandtreffer=%d  groesster Schritt=%ld  Huelle x[%ld..%ld] z[%ld..%ld] (%ldx%ld)\n",
           tag, slot, f0, f1, path, (long)net, (long)netz, nmove, f1 - f0 + 1, nhit,
           (long)maxstep, (long)mn_x, (long)mx_x, (long)mn_z, (long)mx_z,
           (long)(mx_x - mn_x), (long)(mx_z - mn_z));
}

int main(void)
{
    printf("== ROOM1030: WO entsteht der Grenzzyklus? ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    out_t a, b, c;
    const int NF = 3000;

    /* ---- LAUF A: der schlichte RE2-Lauf (Auslieferungsstand mit Tempo-Bit) ---------------- */
    s_stamp_dump = 1;
    run("LAUF A — RE2, Auslieferungsstand (Tempo-Bit aktiv)", NF, &a);
    s_stamp_dump = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d (Weg %.0f / Netto %.0f)\n",
           a.spawned, a.armed, a.crossed, a.stuck_slot, a.stuck_path, a.stuck_net);
    if (a.stuck_slot > 0) {
        printf("  -> Periode im Fenster 2000..2999: %d Bilder\n",
               find_period(a.stuck_slot, 2000, NF - 1));
        stats(a.stuck_slot, NF - 400, NF - 1, "A");
        dump_cycle(a.stuck_slot, NF - 400, NF - 1, 1, "LAUF A Grenzzyklus, 400 Bilder");
    }
    memcpy(s_ref, s_fr, sizeof s_ref);
    int a_stuck = a.stuck_slot;

    /* ---- LAUF C: A/B — dieselbe Zufallsfolge, aber Tempo-Bit ab Bild 0 geloescht ----------
     * Der Setzer B zieht seine zwei Testwerte trotzdem (er laeuft im INIT-Tick), die Sonde
     * loescht das Bit erst danach. Damit ist die Zufallsfolge identisch zu LAUF A und der
     * EINZIGE Unterschied ist der Drittel-Takt-Extra-Advance (@0x80101D54). */
    re15_enemy_reset();
    s_kill_speedbit = 1;
    run("LAUF C — dieselbe Zufallsfolge, Tempo-Bit ab Bild 0 geloescht", NF, &c);
    s_kill_speedbit = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d (Weg %.0f / Netto %.0f)\n",
           c.spawned, c.armed, c.crossed, c.stuck_slot, c.stuck_path, c.stuck_net);

    printf("\n  --- Erste Bahn-Abweichung LAUF A gegen LAUF C, je Slot ---\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int fd = -1;
        for (int f = 0; f < NF; f++)
            if (s_ref[s][f].ex != s_fr[s][f].ex || s_ref[s][f].ez != s_fr[s][f].ez) { fd = f; break; }
        if (fd < 0) { printf("    Slot %d: identisch ueber alle %d Bilder\n", s, NF); continue; }
        printf("    Slot %d: erste Abweichung bei Bild %d — A=(%ld,%ld) kf=%ld clip=%d | "
               "C=(%ld,%ld) kf=%ld clip=%d   [0x21A A=0x%04x C=0x%04x]\n",
               s, fd, (long)s_ref[s][fd].ex, (long)s_ref[s][fd].ez, (long)s_ref[s][fd].kf,
               s_ref[s][fd].mot, (long)s_fr[s][fd].ex, (long)s_fr[s][fd].ez,
               (long)s_fr[s][fd].kf, s_fr[s][fd].mot, s_ref[s][fd].f21a, s_fr[s][fd].f21a);
        printf("       Bilder %d..%d: A  ", fd - 3 < 0 ? 0 : fd - 3, fd + 4);
        for (int f = (fd - 3 < 0 ? 0 : fd - 3); f <= fd + 4 && f < NF; f++)
            printf("(%ld,%ld;kf%ld) ", (long)s_ref[s][f].ex, (long)s_ref[s][f].ez,
                   (long)s_ref[s][f].kf);
        printf("\n                        C  ");
        for (int f = (fd - 3 < 0 ? 0 : fd - 3); f <= fd + 4 && f < NF; f++)
            printf("(%ld,%ld;kf%ld) ", (long)s_fr[s][f].ex, (long)s_fr[s][f].ez,
                   (long)s_fr[s][f].kf);
        printf("\n");
    }
    if (a_stuck > 0 && s_live[a_stuck]) {
        printf("\n  --- LAUF C, derselbe Slot %d wie LAUF As Steckenbleiber ---\n", a_stuck);
        stats(a_stuck, NF - 400, NF - 1, "C");
    }
    if (c.stuck_slot > 0) {
        printf("  -> LAUF C Periode 2000..%d: %d Bilder\n", NF - 1,
               find_period(c.stuck_slot, 2000, NF - 1));
        stats(c.stuck_slot, NF - 400, NF - 1, "C-stuck");
        dump_cycle(c.stuck_slot, NF - 400, NF - 1, 1, "LAUF C Grenzzyklus, 400 Bilder");
    }

    /* ---- LAUF B: POSITIV-KONTROLLE mit MOTOR-TRACE ---------------------------------------
     * Der Nutzer-Befund (Weg 91864 / Netto 5910) stammt aus der Positiv-Kontrolle. Hier
     * derselbe Lauf, aber mit RE15_COLL_TRACE=1 in einem engen Fenster: der Motor druckt die
     * schiebende Zelle mit (vorher)->(nachher). */
    re15_enemy_reset();
    s_nudge_slot = a_stuck; s_nudge_frame = 900; s_nudge_x = -12100;
    run("LAUF B — POSITIV-KONTROLLE (Slot einmalig nach x=-12100)", NF, &b);
    s_nudge_slot = -1;
    printf("  -> %d durch, Steckenbleiber Slot %d (Weg %.0f / Netto %.0f)\n",
           b.crossed, b.stuck_slot, b.stuck_path, b.stuck_net);
    if (b.stuck_slot > 0) {
        printf("  -> Periode 2000..%d: %d Bilder\n", NF - 1,
               find_period(b.stuck_slot, 2000, NF - 1));
        stats(b.stuck_slot, NF - 400, NF - 1, "B");
        dump_cycle(b.stuck_slot, NF - 400, NF - 1, 1, "LAUF B Grenzzyklus, 400 Bilder");
    }

    /* ---- Motor-Grundwahrheit: RE15_COLL_TRACE in einem engen Fenster ---------------------- */
    if (b.stuck_slot > 0) {
        printf("\n  --- MOTOR-TRACE (RE15_COLL_TRACE=1), Fenster %d..%d, Slot %d ---\n",
               NF - 60, NF - 1, b.stuck_slot);
        printf("  (Die Zeilen [coll] kommen aus re15_collision.c:731-742 und stehen auf stderr;\n"
               "   [F..]-Marken dieser Sonde ebenfalls, die Reihenfolge stimmt also.)\n");
        fflush(stdout);
        re15_enemy_reset();
        s_nudge_slot = a_stuck; s_nudge_frame = 900; s_nudge_x = -12100;
        s_trace_slot = b.stuck_slot; s_trace_from = NF - 60; s_trace_to = NF - 1;
        out_t d; run("LAUF B2 — Positiv-Kontrolle mit Motor-Trace", NF, &d);
        s_trace_slot = -1; s_nudge_slot = -1;
        fflush(stderr);
    }

    /* ---- PHASEN-FEGER: haengt die Scharfschalt-Zahl an der Wurf-PHASE? --------------------
     * Setzer B zieht IMMER zwei Werte und im Trefferzweig einen dritten. Wer wissen will, ob
     * die 6->5 daher kommt, verschiebt die Phase kuenstlich und schaut, ob die Zahl kippt. */
    printf("\n  --- PHASEN-FEGER: k zusaetzliche Wuerfe am RE2-Generator vor Bild 0 ---\n");
    for (int k = 0; k <= 23; k++) {
        out_t rp;
        s_pre_draws = k;
        re15_enemy_reset(); s_quiet = 1; run("(Phase)", 1400, &rp); s_quiet = 0;
        printf("  k=%-3d scharf=%d durch=%d steckt=%-3d\n", k, rp.armed, rp.crossed, rp.stuck_slot);
    }
    s_pre_draws = 0;

    /* ---- SAAT-FEGER: ist "6 von 6" systematisch oder Zufallsglueck? ------------------------
     * 16 Zufallssaaten, je zwei Varianten: Auslieferungsstand (Tempo-Bit scharf) und
     * "Bit ab Bild 1 geloescht" (gleiche Zufallsfolge). Streut die Scharfschalt-Zahl ueber
     * die Saaten, ist "6 von 6" kein Sollwert, sondern eine Eigenschaft DIESER Saat. */
    printf("\n  --- SAAT-FEGER (16 Saaten x 2 Varianten, je 1400 Bilder) ---\n");
    printf("  %-12s %-30s %s\n", "Saat", "Auslieferung (Bit scharf)", "Bit ab Bild 1 geloescht");
    {
        static const uint32_t seeds[16] = {
            0x2545f491u, 0x00000001u, 0x13579bdfu, 0xdeadbeefu, 0x12345678u, 0xcafebabeu,
            0x0badf00du, 0xa5a5a5a5u, 0x5eed0001u, 0x5eed0002u, 0x5eed0003u, 0x5eed0004u,
            0x5eed0005u, 0x5eed0006u, 0x5eed0007u, 0x5eed0008u };
        int hist_a[8]; int hist_c[8];
        memset(hist_a, 0, sizeof hist_a); memset(hist_c, 0, sizeof hist_c);
        for (int k = 0; k < 16; k++) {
            out_t ra, rc;
            s_seed = seeds[k];
            re15_enemy_reset(); s_quiet = 1; run("(Feger)", 1400, &ra); s_quiet = 0;
            re15_enemy_reset(); s_kill_speedbit = 1; s_quiet = 1;
            run("(Feger)", 1400, &rc); s_quiet = 0; s_kill_speedbit = 0;
            if (ra.armed >= 0 && ra.armed < 8) hist_a[ra.armed]++;
            if (rc.armed >= 0 && rc.armed < 8) hist_c[rc.armed]++;
            printf("  0x%08lx   scharf=%d durch=%d steckt=%-3d          scharf=%d durch=%d steckt=%d\n",
                   (unsigned long)seeds[k], ra.armed, ra.crossed, ra.stuck_slot,
                   rc.armed, rc.crossed, rc.stuck_slot);
        }
        printf("  Verteilung scharfgeschaltet — Auslieferung:");
        for (int i = 0; i < 8; i++) if (hist_a[i]) printf(" %dx%d", hist_a[i], i);
        printf("   | Bit geloescht:");
        for (int i = 0; i < 8; i++) if (hist_c[i]) printf(" %dx%d", hist_c[i], i);
        printf("\n");
        s_seed = 0x2545f491u;
    }

    /* ==========================================================================================
     * DIREKT-PIN AM MOTOR: re15_re2z_move_root mit Sprung +1 gegen Sprung +2.
     * Kein Nachbau — es wird die echte Motorfunktion auf einer KOPIE des lebenden Zombies
     * gerufen, nur mit veraendertem (anim_frame, root_prev_kf)-Paar. */
    printf("\n  --- DIREKT-PIN: re15_re2z_move_root, Anker K gegen Bild K+1 bzw. K+2 ---\n");
    if (a_stuck > 0) {
        re15_actor_t scratch;
        printf("  %-4s %-12s %-12s %-12s %-12s\n", "K", "d(K->K+1)", "d(K->K+2)",
               "+0x144 (+1)", "+0x144 (+2)");
        for (int K = 0; K < 12; K++) {
            int32_t d1x, d1z, d2x, d2z; int16_t r1, r2;
            scratch = g_actors[a_stuck];
            scratch.x = 0; scratch.z = 0; scratch.rot_y = 0;
            scratch.motion = 0; scratch.root_prev_motion = 0;
            scratch.root_prev_kf = (int16_t)K; scratch.anim_frame = K + 1;
            re15_re2z_move_root(&scratch);
            d1x = scratch.x; d1z = scratch.z; r1 = scratch.re2z_root144;
            scratch = g_actors[a_stuck];
            scratch.x = 0; scratch.z = 0; scratch.rot_y = 0;
            scratch.motion = 0; scratch.root_prev_motion = 0;
            scratch.root_prev_kf = (int16_t)K; scratch.anim_frame = K + 2;
            re15_re2z_move_root(&scratch);
            d2x = scratch.x; d2z = scratch.z; r2 = scratch.re2z_root144;
            printf("  %-4d (%5ld,%5ld) (%5ld,%5ld) %-12d %-12d\n", K,
                   (long)d1x, (long)d1z, (long)d2x, (long)d2z, r1, r2);
        }
        printf("  Original FUN_80015E7C @0x80015FCC-FE4 kennt KEINE Bedingung: es zieht die\n"
               "  gespeicherte Wurzel-Momentaufnahme (+0x20C/+0x20E/+0x210, geschrieben\n"
               "  @0x80015FC4/C8) vom Wurzelstand des AKTUELLEN +0x14D ab. Ein uebersprungener\n"
               "  Keyframe liefert dort den DOPPELTEN Vorschub, im Port NULL\n"
               "  (enemy_ai_common.c:700 und :706).\n");
    }

    /* ==========================================================================================
     * DOPPEL-VORSCHUB-ZENSUS: was passiert in einem Bild, in dem der Drittel-Takt einen
     * Keyframe UEBERSPRINGT (kf steigt um 2)?
     *
     * Original FUN_80015E7C @0x80015FCC-FE4:  dx = sx(kf_jetzt) - sx(kf_vorher)  — ohne jede
     * Bedingung. Ein uebersprungener Keyframe gibt also einen DOPPELTEN Vorschub, und genau
     * daraus entsteht RE2s 1,5-fache Gangkadenz (3 Keyframes je 2 Ticks).
     * Port enemy_ai_common.c:697 + :706 haben zwei Riegel:
     *     if (fr_prev < 0 || fr_now != fr_prev + 1) return;
     *     if (s_now != s_prev + 1) return;
     * Ein Sprung um 2 faellt also durch BEIDE und liefert VERSCHIEBUNG NULL.
     * Hier der Zensus ueber LAUF A: wie oft steigt kf um 2, und wie oft ist die Verschiebung
     * in genau diesen Bildern 0? */
    printf("\n  --- DOPPEL-VORSCHUB-ZENSUS (LAUF A, aus s_ref) ---\n");
    printf("  %-5s %-7s %-9s %-9s %-9s %-9s %s\n", "slot", "0x21A", "Bilder dkf=1",
           "davon d=0", "Bilder dkf=2", "davon d=0", "mittl. |d| dkf=1 / dkf=2");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int n1 = 0, n1z = 0, n2 = 0, n2z = 0; double p1 = 0, p2 = 0;
        for (int f = 1; f < NF; f++) {
            const fr_t *r0 = &s_ref[s][f], *rm = &s_ref[s][f - 1];
            if (r0->mot != rm->mot) continue;
            int32_t dkf = r0->kf - rm->kf;
            int32_t dx = r0->ex - r0->sx, dz = r0->ez - r0->sz;
            int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (dkf == 1) { n1++; p1 += ad; if (ad == 0) n1z++; }
            else if (dkf == 2) { n2++; p2 += ad; if (ad == 0) n2z++; }
        }
        printf("  z%-4d 0x%04x  %-9d %-9d %-9d %-9d %.1f / %.1f\n",
               s, s_ref[s][NF-1].f21a, n1, n1z, n2, n2z,
               n1 ? p1 / n1 : 0.0, n2 ? p2 / n2 : 0.0);
    }
    printf("  (Ein Bild mit dkf=2 IST das Drittel-Takt-Bild @0x80101D04-2C+@0x80101D54.\n"
           "   Im Original traegt es den DOPPELTEN Wurzel-Delta, im Port NULL.)\n");

    /* ---- Gangstrecke A gegen C, Bilder 0..400 (freies Gehen, vor dem Tor) ------------------ */
    printf("\n  --- Gangstrecke Bilder 0..400: LAUF A (Bit scharf) gegen LAUF C (Bit aus) ---\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        double pa = 0, pc = 0;
        for (int f = 1; f <= 400; f++) {
            int32_t ax = s_ref[s][f].ex - s_ref[s][f].sx, az = s_ref[s][f].ez - s_ref[s][f].sz;
            int32_t cx = s_fr[s][f].ex  - s_fr[s][f].sx,  cz = s_fr[s][f].ez  - s_fr[s][f].sz;
            pa += (ax < 0 ? -ax : ax) + (az < 0 ? -az : az);
            pc += (cx < 0 ? -cx : cx) + (cz < 0 ? -cz : cz);
        }
        printf("  z%-4d 0x21A(A)=0x%04x  Weg A=%8.0f  Weg C=%8.0f  A/C=%.3f\n",
               s, s_ref[s][NF-1].f21a, pa, pc, pc > 0 ? pa / pc : 0.0);
    }

    /* ---- LAUF D: DEM STECKENBLEIBER das Tempo-Bit AUFZWINGEN -------------------------------
     * Item 4 der Messung: "was macht die Klemme im Tempo-Fall anders?" Slot 3 traegt das Bit im
     * Auslieferungsstand NICHT (0x21A = 0x0000). Hier bekommt er es jeden Frame aufgezwungen —
     * gleiche Zufallsfolge, gleicher Startzustand, nur der Drittel-Takt-Extra-Advance
     * (@0x80101D54) dazu. Unterscheidet sich der Grenzzyklus? */
    if (a_stuck > 0) {
        out_t d2;
        re15_enemy_reset();
        s_force_bit_slot = a_stuck;
        run("LAUF D — Steckenbleiber MIT aufgezwungenem Tempo-Bit", NF, &d2);
        s_force_bit_slot = -1;
        printf("  -> %d scharf, %d durch, Steckenbleiber Slot %d\n",
               d2.armed, d2.crossed, d2.stuck_slot);
        if (d2.stuck_slot > 0) stats(d2.stuck_slot, NF - 400, NF - 1, "D");
        if (a_stuck > 0 && s_live[a_stuck]) {
            printf("  Direktvergleich der letzten 400 Bilder von Slot %d:\n", a_stuck);
            stats(a_stuck, NF - 400, NF - 1, "D/slot3");
            printf("  (LAUF A stand oben: Weg=6058 Netto=(175,17) Wandtreffer=261 "
                   "groesster Schritt=43 Huelle 1319x17)\n");
            printf("  %-6s %-9s %-9s %-5s %-5s %-4s %-4s %-3s %s\n",
                   "f", "startZ", "endeZ", "dX", "dZ", "kf", "clip", "hit", "+0x90");
            printf("  (Spalte kf = +0x14D am BILDENDE, pkf = root_prev_kf = der Anker, gegen\n"
                   "   den re15_re2z_move_root im NAECHSTEN Aufruf die Wurzel-Differenz bildet)\n");
            for (int f = NF - 30; f < NF; f++) {
                const fr_t *r0 = &s_fr[a_stuck][f];
                printf("  %-6d %-9ld %-9ld %-5ld %-5ld %-4ld %-4d %-3u 0x%02x  pkf=%-4d pmot=%d\n",
                       f, (long)r0->sz, (long)r0->ez, (long)(r0->ex - r0->sx),
                       (long)(r0->ez - r0->sz), (long)r0->kf, r0->mot, r0->wallhit, r0->contact,
                       r0->pkf, r0->pmot);
            }
        }
    }

    /* ---- Liegt das Steuerziel IM Hindernis? (Ursache c) ------------------------------------ */
    printf("\n  --- Steuerziel gegen die schiebende Zelle #28 ---\n");
    if (28 < g_room_rdt.sca_count) {
        const re15_sca_entry_t *c28 = &g_room_rdt.sca[28];
        int32_t tx = s_ref[a_stuck > 0 ? a_stuck : 1][NF - 1].stx;
        int32_t tz = s_ref[a_stuck > 0 ? a_stuck : 1][NF - 1].stz;
        int32_t x1 = c28->x, x2 = (int32_t)c28->x + (int32_t)c28->width;
        int32_t z1 = c28->z, z2 = (int32_t)c28->z + (int32_t)c28->density;
        printf("  Zelle#28: x[%ld..%ld] z[%ld..%ld]  u0=0x%02x u1=0x%02x typ=%d band=%d\n",
               (long)x1, (long)x2, (long)z1, (long)z2, (unsigned)c28->u0, (unsigned)c28->u1,
               c28->type, (int)(c28->floor >> 4));
        printf("  Steuerziel (+0x1BC/+0x1BE) = (%ld,%ld) -> %s\n", (long)tx, (long)tz,
               (tx >= x1 && tx < x2 && tz >= z1 && tz < z2)
                   ? "*** LIEGT IM RECHTECK DER ZELLE ***" : "liegt ausserhalb");
        printf("  Tiefe im Rechteck: dz zur Suedkante = %ld, zur um r=400 aufgeblasenen "
               "Suedkante = %ld\n", (long)(tz - z1), (long)(tz - (z1 - 400)));
        printf("  Maske des Steckenbleibers = %u, u0 & Maske = %u -> %s\n",
               s_ref[a_stuck > 0 ? a_stuck : 1][NF - 1].sca,
               (unsigned)(c28->u0 & s_ref[a_stuck > 0 ? a_stuck : 1][NF - 1].sca),
               (c28->u0 & s_ref[a_stuck > 0 ? a_stuck : 1][NF - 1].sca) ? "SOLIDE" : "durchlaessig");
        printf("  (Kriecher-Maske 8: u0 & 8 = %u -> %s)\n", (unsigned)(c28->u0 & 8u),
               (c28->u0 & 8u) ? "SOLIDE" : "durchlaessig — genau dafuer setzt das Skript 0xF7");
    }

    /* ---- Streckentest ---------------------------------------------------------------------- */
    printf("\n  --- Streckentest: klemmt der Port nur den Zielpunkt? ---\n");
    printf("  collision_constrain_impl (re15_collision.c:726-730) testet AUSSCHLIESSLICH die\n"
           "  NEUE Position gegen die um r aufgeblasene Zelle — kein Sweep, kein Segmenttest.\n"
           "  Byte-true zu FUN_8003b0a4 @0x8003b284-94.\n");
    {
        int mind = 0x7fffffff, mini = -1;
        for (int i = 0; i < g_room_rdt.sca_count; i++) {
            const re15_sca_entry_t *e2 = &g_room_rdt.sca[i];
            if ((e2->floor >> 4) != 0) continue;
            if ((e2->u0 & 0xCu) == 0) continue;      /* solide fuer Maske 4 oder 8 */
            int d = e2->width < e2->density ? e2->width : e2->density;
            if (d < mind) { mind = d; mini = i; }
        }
        if (mini >= 0) {
            const re15_sca_entry_t *e2 = &g_room_rdt.sca[mini];
            printf("  Duennste solide Band-0-Zelle: #%d typ=%d %dx%d (u0=0x%02x) -> schmale "
                   "Achse %d;\n  mit r=400 aufgeblasen %d breit. Ein Tick-Schritt muesste "
                   "groesser sein, um sie zu ueberspringen.\n",
                   mini, e2->type, (int)e2->width, (int)e2->density, (unsigned)e2->u0,
                   mind, mind + 800);
        }
    }

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== MESSUNG FERTIG ==\n");
    return 0;   /* Mess-Sonde: meldet, faellt nicht */
}
