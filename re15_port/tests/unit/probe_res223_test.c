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

uint32_t re15_re2_rand_draws(void);   /* enemy_ai_re2_zombie.c:334 */

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
static int  s_res223_setzerA = 0;      /* EINMAL bei Bild 0: Setzer-B-Wert 32..47 auf Setzer-A-Band
                                        * 16..31 zuruecksetzen (-16). Testet die ZWEITE Haelfte des
                                        * Trefferzweigs @0x8010093C-4C, die LAUF C unangetastet laesst. */
static int  s_res223_done[RE15_ACTOR_MAX];
static int  s_res223_watch = 0;   /* nur melden, nicht eingreifen */
static int  s_spawn_dump = 0;          /* res223/0x21A je Slot direkt nach dem Spawn ausgeben       */
static uint32_t s_draws_spawn = 0;     /* Wuerfe, die das Raumladen am RE2-Generator verbraucht      */
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
    { uint32_t d0 = re15_re2_rand_draws();
      scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
      s_draws_spawn = re15_re2_rand_draws() - d0; }

    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    /* PHASEN-VERSCHIEBUNG des RE2-Generators (@0x80015FE8, Zustand 0x800CE318, Saat
     * 0xD2706CA4 beim Raumladen): genau der Effekt, den die zwei/drei Extra-Wuerfe des
     * Setzers B haben. Damit laesst sich messen, ob die Scharfschalt-Zahl ueberhaupt an
     * der Wurf-Phase haengt. */
    for (int d = 0; d < s_pre_draws; d++) (void)re15_re2_rand();

    memset(s_res223_done, 0, sizeof s_res223_done);
    memset(s_live, 0, sizeof s_live);
    int32_t z0[RE15_ACTOR_MAX], x0[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        s_live[s] = 1; z0[s] = g_actors[s].z; x0[s] = g_actors[s].x;
    }
    if (out) for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s_live[s]) out->spawned++;
    if (s_spawn_dump) {
        printf("  Wuerfe waehrend des Raumladens (RE2-Generator @0x80015FE8): %lu\n",
               (unsigned long)s_draws_spawn);
        printf("  %-5s %-8s %-8s %s\n", "slot", "+0x21A", "+0x223", "Setzer");
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            int r = (int)g_actors[s].re2z_res223;
            printf("  z%-4d 0x%04x   %-8d %s\n", s, g_actors[s].re2z_flags21a, r,
                   (r >= 32) ? "B (Treffer: Bit + 32..47)" : "A (16..31)");
        }
    }

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
            if (s_res223_setzerA && !s_res223_done[s] && g_actors[s].re2z_res223 >= 32) {
                s_res223_done[s] = 1;
                printf("    [0x223] Slot %d: Setzer-B-Wert %d bei Bild %d -> %d (Setzer-A-Band)\n",
                       s, (int)g_actors[s].re2z_res223, f, (int)g_actors[s].re2z_res223 - 16);
                g_actors[s].re2z_res223 = (int8_t)(g_actors[s].re2z_res223 - 16);
            }
            if (s_res223_watch && !s_res223_done[s] && g_actors[s].re2z_res223 >= 32) {
                s_res223_done[s] = 1;
                printf("    [0x223-beobachtet] Slot %d: %d bei Bild %d (Trefferzweig lief)\n",
                       s, (int)g_actors[s].re2z_res223, f);
            }
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

/* ===========================================================================================
 * GEGENPRUEFUNG des Befunds "die Wurf-PHASE, nicht das Bit".
 *
 * Der Trefferzweig des Setzers B @0x80100930-4C schreibt ZWEI Felder:
 *     80100930/38/40   lhu 538 / ori 0x8000 / sh 538   -> +0x21A |= 0x8000   (Tempo)
 *     8010093C/44/48/4C jal <rng> / andi 0xf / addiu 32 / sb 547 -> +0x223 = 32 + (rand&0xf)
 * LAUF C des Befunds loescht NUR das erste Feld. Das zweite (die Flinch-Reserve, Setzer A
 * gibt 16..31 @0x80100888-9C, Setzer B 32..47) bleibt dort auf dem B-Band stehen.
 * LAUF C isoliert also NICHT "die Wirkung des Setzers B", sondern nur die des Tempo-Bits.
 * Diese Sonde schliesst die Luecke.
 * =========================================================================================== */
static void head(const char *t) { printf("\n############ %s\n", t); }

int main(void)
{
    size_t n = 0;
    char path[512];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *buf = slurp(path, &n);
    if (!buf) { printf("SKIP: %s fehlt\n", path); return 0; }
    if (re15_rdt_parse(buf, n, &s_rdt) != 0) { printf("SKIP: RDT-Parse\n"); return 0; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;

    const int NF = 1400;
    out_t a, c, c2, e;

    head("R1 — Auslieferungsstand (Setzer B voll scharf)  [= LAUF A des Befunds]");
    s_res223_watch = 1;
    run("R1", NF, &a);
    s_res223_watch = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d\n",
           a.spawned, a.armed, a.crossed, a.stuck_slot);

    head("R2 — nur Tempo-Bit geloescht  [= LAUF C des Befunds]");
    s_kill_speedbit = 1;
    run("R2", NF, &c);
    s_kill_speedbit = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d\n",
           c.spawned, c.armed, c.crossed, c.stuck_slot);

    head("R3 — Tempo-Bit geloescht UND +0x223 auf das Setzer-A-Band 16..31 zurueck\n"
         "       (die zweite Haelfte des Trefferzweigs, die LAUF C stehen laesst)");
    s_kill_speedbit = 1; s_res223_setzerA = 1;
    run("R3", NF, &c2);
    s_kill_speedbit = 0; s_res223_setzerA = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d\n",
           c2.spawned, c2.armed, c2.crossed, c2.stuck_slot);

    head("R4 — NUR +0x223 zurueckgesetzt, Tempo-Bit bleibt scharf");
    s_res223_setzerA = 1;
    run("R4", NF, &e);
    s_res223_setzerA = 0;
    printf("  -> %d gespawnt, %d scharf, %d durch, Steckenbleiber Slot %d\n",
           e.spawned, e.armed, e.crossed, e.stuck_slot);

    printf("\n############ ERGEBNIS\n");
    printf("  R1 Auslieferung          : scharf=%d durch=%d steckt=%d\n", a.armed, a.crossed, a.stuck_slot);
    printf("  R2 nur Bit weg (LAUF C)  : scharf=%d durch=%d steckt=%d\n", c.armed, c.crossed, c.stuck_slot);
    printf("  R3 Bit weg + 0x223 weg   : scharf=%d durch=%d steckt=%d\n", c2.armed, c2.crossed, c2.stuck_slot);
    printf("  R4 nur 0x223 weg         : scharf=%d durch=%d steckt=%d\n", e.armed, e.crossed, e.stuck_slot);
    printf("\n  Deutung: weicht R3 von R2 ab, dann traegt die Flinch-Reserve +0x223 zum\n"
           "  Ergebnis bei und LAUF C war KEINE Ein-Variablen-Probe.\n");

    free(buf); g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== FERTIG ==\n");
    return 0;
}
