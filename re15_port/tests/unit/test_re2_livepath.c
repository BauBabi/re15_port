/**
 * @file test_re2_livepath.c
 * @brief PIN — der ECHTE RE2-Modus-Spielweg (Asset-Kette + Anim + Treffer) in echten Raeumen.
 *
 * Anlass: Nutzer-Report 2026-08-17 zum RE2-KI-Modus — u.a. "Hunde sind durch Zombies ersetzt",
 * "keine Animationen", "reagieren nicht auf Schuesse". Die bestehenden RE2-Tests pruefen
 * FELDWERTE mit synthetischen Bank-Stubs; dieser Test faehrt stattdessen den Weg, den
 * platform/pc/main.c im Spiel geht:
 *     RDT laden -> raum-eigenes SCD hochfahren (Sce_em_set-Spawns)
 *     -> pc_enemy_load-SPIEGEL (RE2-Zweig: re2_ems_load_bank aus shared_assets/RE2/CDEMD0.EMS)
 *     -> re15_enemy_ai_run_all + re15_actors_anim_advance (game_step-Reihenfolge)
 *     -> re15_actor_anim_select + die main.c-Bank-Overrides = was der RENDERER wirklich posiert.
 *
 * Gepinnt wird deshalb genau das, was die Reports betrifft:
 *   (1) IDENTITAET: der Hund laedt ein VIERBEINER-Skelett, der Zombie ein aufrechtes — die
 *       Silhouette (Bounding-Box der Ruhepose) unterscheidet beide eindeutig, ein vertauschter
 *       TOC-Record faellt sofort auf. Plus Negativprobe gegen den jeweils anderen Typ.
 *   (2) BANK DA: Mesh/Bone/Clip-Zahlen der RE2-Baenke + Negativprobe (kind 0x14 hat KEINEN
 *       EMD-Record in der TOC @0x8009adf4 -> muss fehlschlagen).
 *   (3) ANIMATION: ueber 200 Frames aendert sich die GERENDERTE (clip,keyframe)-Paarung, und der
 *       gerenderte Clip liegt in JEDEM Frame im Bereich der gewaehlten Bank.
 *   (4) TREFFER: ein Schuss senkt HP und schaltet auf HURT/DEATH.
 * `probe_re2_livepath census` (argv) listet zusaetzlich die Gegner-Typen aller Raeume.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_anim_select.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* ---- residente Archive (wie die main.c-Caches pc_cdemd / pc_re2_cdemd) ---- */
static const uint8_t *re15_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}
static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}

/* Silhouette der Ruhepose (kf 0): Bounding-Box der Bone-Translationen.
 * Vierbeiner = LANG (dx) und FLACH (dy), aufrechter Zombie = umgekehrt. */
static void skel_bbox(const re15_emd_skeleton_t *sk, int32_t out_d[3])
{
    out_d[0] = out_d[1] = out_d[2] = -1;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (sk->bone_count <= 0 || re15_skel_compute_pose(sk, 0, poses) != 0) return;
    int32_t mn[3] = { 1<<30, 1<<30, 1<<30 }, mx[3] = { -(1<<30), -(1<<30), -(1<<30) };
    for (int b = 0; b < sk->bone_count; b++)
        for (int k = 0; k < 3; k++) {
            int32_t v = poses[b].trans[k];
            if (v < mn[k]) mn[k] = v;
            if (v > mx[k]) mx[k] = v;
        }
    for (int k = 0; k < 3; k++) out_d[k] = mx[k] - mn[k];
}

/* ---- 1:1-Spiegel von pc_enemy_load (platform/pc/main.c) ---- */
static void probe_enemy_load(uint8_t type, int verbose)
{
    if (type == 0 || re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;

    if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && re15_re2_owns_type(type)) {
        size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) {
            eb->buf = NULL;
            eb->pc_tex_slot = 11 + (int)(eb - g_enemy);
            eb->ok = 1;
            if (verbose) {
                int32_t d[3]; skel_bbox(&eb->skel, d);
                printf("  [load] RE2 EM0%02X: %d meshes, %d bones, %d clips, loco %d, tim %dx%d,"
                       " skel-bbox %d/%d/%d\n", type, eb->md1.mesh_count, eb->skel.bone_count,
                       eb->anim.clip_count, eb->anim_loco.clip_count, tim.width, tim.height,
                       d[0], d[1], d[2]);
            }
            return;
        }
        printf("  [load] RE2 EM0%02X FEHLGESCHLAGEN -> RE1.5-Fallback\n", type);
    }

    size_t esz = 0; const uint8_t *ems = re15_ems_blob(&esz);
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    uint8_t *bufp = NULL;
    if (ems && idx >= 0 && re15_ems_get_entry(ems, esz, idx, &off, &len) == 0) {
        bufp = (uint8_t *)malloc(len);
        if (bufp) memcpy(bufp, ems + off, len);
    }
    if (!bufp) { eb->type = 0; return; }
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(bufp, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        free(bufp); eb->type = 0; return;
    }
    eb->buf = bufp;
    eb->loco_ok   = (re15_emd_parse_loco_bank(bufp, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(bufp, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(bufp, len, &eb->skel_own, &eb->anim_own) == 0);
    eb->pc_tex_slot = 11 + (int)(eb - g_enemy);
    eb->ok = 1;
    if (verbose)
        printf("  [load] RE1.5 EM%02X: %d meshes, %d bones, %d clips\n", type,
               eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count);
}

/* ---- was der RENDERER posiert (main.c-Auswahl + Loco-Override) ---- */
typedef struct { const re15_emd_animation_t *anim; int clip; int kf; } view_t;

static re15_md1_t           s_def_mesh;
static re15_emd_skeleton_t  s_def_skel;
static re15_emd_animation_t s_def_anim;

static void render_view(const re15_actor_t *npc, view_t *v)
{
    re15_anim_banks_t banks;
    memset(&banks, 0, sizeof banks);
    banks.def_mesh = &s_def_mesh; banks.def_skel = &s_def_skel; banks.def_anim = &s_def_anim;
    re15_anim_view_t av;
    re15_actor_anim_select(npc, 0, &banks, &av);
    const re15_emd_skeleton_t *sk = av.skel;
    v->anim = av.anim;
    int clip_override = av.clip_override;
    if (re15_actor_uses_loco_bank(npc)) {
        re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
        if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
            sk = &lb->skel_loco; v->anim = &lb->anim_loco;
            clip_override = (int)npc->motion;
        }
    }
    v->clip = (clip_override >= 0) ? clip_override : (int)npc->motion;
    v->kf   = re15_compute_actor_kf(v->anim, sk, npc, clip_override, npc->anim_frame);
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* ---- SE-Mitschnitt (der ENEMSE-Hook, den platform/pc/main.c im Spiel registriert) ---- */
static int s_se_log[128], se_n;
static void probe_se(int se_id, int flag2000) { (void)flag2000; if (se_n < 128) s_se_log[se_n++] = se_id; }
static int  se_seen(int id) { for (int i = 0; i < se_n; i++) if (s_se_log[i] == id) return 1; return 0; }

static int bringup(const re15_rdt_t *rdt, int fire_sub, int verbose)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    if (rdt->sub_scd[0]) scd_thread_start(1, rdt->sub_scd[0]);
    if (fire_sub >= 0 && rdt->sub_scd_count > fire_sub && rdt->sub_scd[fire_sub])
        scd_thread_start(2, rdt->sub_scd[fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type) { probe_enemy_load(g_actors[s].type, verbose); n++; }
    /* Skript-Freigabe (Fix 2026-08-23): grid-0x40-Skript-Hunde (ROOM1190) warten byte-true in
     * Zustand 4 auf SCD grid=0x43 (@0x801113e4-ec). Der Test gibt sie frei wie das Raumskript
     * und laesst die Sprungmaschine landen (Exit 0x201 @0x8011162c). */
    {
        int had4 = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (e->active && e->type == 0x20) { frame(); break; }   /* INIT-Tick */
        }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (e->active && e->type == 0x20 && e->state == 4) { e->grid_id = 0x43; had4 = 1; }
        }
        for (int f = 0; had4 && f < 400; f++) {
            int waiting = 0;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x20 && g_actors[s].state == 4)
                    waiting = 1;
            if (!waiting) break;
            frame();
        }
    }
    return n;
}

/* Ein Raum unter RE2-Flavor: Spawns + Baenke + 200 Frames + ein Schuss. */
/* Silhouetten-Klassen (Ruhepose, Bone-Translations-Box): so ist ein vertauschter EMD-Record
 * nicht verhandelbar erkennbar. Gemessen 2026-08-17 an shared_assets/RE2/CDEMD0.EMS:
 *   Hund   EM020: 1922/946/468   (LANG + FLACH)
 *   Zombie EM010: 418/2546/955   (SCHMAL + HOCH)
 *   Kraehe EM021: 643/300/240    (KLEIN in allen Achsen) */
enum { SHAPE_UPRIGHT = 0, SHAPE_QUADRUPED = 1, SHAPE_BIRD = 2 };

static void run_room(const char *tag, const char *rdtpath, int fire_sub, uint8_t want_type,
                     int want_min_actors, int shape)
{
    long sz = 0;
    uint8_t *buf = slurp(rdtpath, &sz);
    if (!buf) { CHECK(0, "%s: RDT fehlt (%s)", tag, rdtpath); return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { CHECK(0, "%s: RDT-Parse", tag); free(buf); return; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("== %s (RE2-Flavor)\n", tag);
    int n = bringup(&rdt, fire_sub, 1);
    int ntyped = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == want_type) ntyped++;
    CHECK(ntyped >= want_min_actors, "%s: >=%d Aktoren vom Typ 0x%02X erwartet, %d von %d aktiv",
          tag, want_min_actors, want_type, ntyped, n);
    if (ntyped == 0) { free(buf); return; }

    /* (1)+(2) Bank da + IDENTITAET ueber die Skelett-Silhouette */
    re15_enemy_bank_t *eb = re15_enemy_find(want_type);
    CHECK(eb != NULL, "%s: KEINE Bank fuer 0x%02X -> der Renderer faellt auf Leon zurueck",
          tag, want_type);
    if (!eb) { free(buf); return; }
    CHECK(eb->anim.clip_count > 0 && eb->md1.mesh_count > 0 && eb->skel.bone_count > 0,
          "%s: leere Bank (clips=%d meshes=%d bones=%d)", tag, eb->anim.clip_count,
          eb->md1.mesh_count, eb->skel.bone_count);
    int32_t d[3];
    skel_bbox(&eb->skel, d);
    if (shape == SHAPE_QUADRUPED) {
        CHECK(d[0] > 2 * d[1], "%s: Hund muss ein VIERBEINER sein (dx=%d muss >> dy=%d) — "
              "ein aufrechtes Skelett hier hiesse: falscher EMD-Record geladen", tag, d[0], d[1]);
        CHECK(d[1] < 1500 && d[0] > 1500,
              "%s: Hunde-Silhouette verfehlt (dx=%d dy=%d)", tag, d[0], d[1]);
    } else if (shape == SHAPE_BIRD) {
        CHECK(d[0] < 1200 && d[1] < 800 && d[2] < 800,
              "%s: Kraehe muss KLEIN sein (%d/%d/%d) — Zombie/Hund-Silhouette?", tag, d[0], d[1], d[2]);
        CHECK(eb->skel.bone_count == 13, "%s: Kraehe hat 13 Bones, gelesen %d", tag, eb->skel.bone_count);
    } else {
        CHECK(d[1] > 2 * d[0], "%s: Zombie muss AUFRECHT sein (dy=%d muss >> dx=%d)", tag, d[1], d[0]);
        CHECK(d[1] > 2000, "%s: Zombie-Skelett zu flach (dy=%d) — Vierbeiner-Silhouette?", tag, d[1]);
    }

    /* (3) ANIMATION auf dem echten Weg: gerenderte (clip,kf)-Paarung muss sich bewegen,
     *     und der gerenderte Clip muss in JEDEM Frame in der gewaehlten Bank existieren. */
    int first = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == want_type) { first = s; break; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint32_t last[RE15_ACTOR_MAX]; int changes[RE15_ACTOR_MAX], oob[RE15_ACTOR_MAX];
    memset(last, 0xff, sizeof last); memset(changes, 0, sizeof changes); memset(oob, 0, sizeof oob);
    for (int f = 0; f < 200; f++) {
        pl->x = g_actors[first].x + 1200; pl->z = g_actors[first].z; pl->motion = 100;
        frame();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != want_type) continue;
            view_t v; render_view(e, &v);
            if (!v.anim || v.clip < 0 || v.clip >= v.anim->clip_count) { oob[s]++; continue; }
            uint32_t key = ((uint32_t)v.clip << 16) | (uint32_t)(v.kf & 0xffff);
            if (last[s] != key) { if (last[s] != 0xffffffffu) changes[s]++; last[s] = key; }
        }
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != want_type) continue;
        CHECK(oob[s] == 0, "%s: actor%d rendert in %d Frames einen Clip AUSSERHALB der Bank",
              tag, s, oob[s]);
        CHECK(changes[s] >= 20, "%s: actor%d zeigt nur %d Pose-Wechsel in 200 Frames "
              "(= sichtbar keine Animation)", tag, s, changes[s]);
    }

    /* (4) TREFFER: der Schuss muss HP senken und HURT/DEATH setzen. */
    {
        re15_actor_t *e = &g_actors[first];
        /* FIXTURE-ANKER (2026-09-05, Welle B): der freie 300-Frame-Vorlauf kann den Probanden
         * jetzt mitten in Knockdown/Fall/Liege stellen (gemessen: st=1.5.1, grid 0x80,
         * +0x21A=0x0202, +0x10E=0x2004 — ein Level-Schuss verfehlt den Fallenden BYTE-TRUE).
         * Der Pin misst den Schadens-DURCHGRIFF am stehenden Zombie; deshalb warten, bis er
         * wieder aufrecht ist (state 1, sub<=2), und die Down-Marker sind dann von den
         * Aufsteh-Ketten selbst geloescht. */
        if (want_type == 0x10) {   /* nur die Zombie-Familie kennt Down-Zustaende; die
                                    * Kraehe lebt in den Flug-Subs (13 = Hover) */
            for (int w = 0; w < 600 && !(e->state == 1 && e->sub_state_1 <= 2); w++) {
                pl->x = e->x - 3000; pl->z = e->z; pl->motion = 0;  /* ausser Grab-Reichweite */
                frame();
            }
            CHECK(e->state == 1 && e->sub_state_1 <= 2,
                  "%s: Proband kommt nicht in den Stand zurueck (st=%u.%u)", tag,
                  e->state, e->sub_state_1);
        }
        int hp0 = e->hp;
        pl->x = e->x - 800; pl->z = e->z; pl->rot_y = 0; e->hit_react = 0;
        re15_player_set_equipped_weapon(3);
        int r = re15_player_weapon_fire(3);
        CHECK(r != 0, "%s: der Schuss trifft den Gegner gar nicht (weapon_fire == 0)", tag);
        CHECK(e->hp < hp0, "%s: HP unveraendert (%d) — kein Schadens-Durchgriff", tag, e->hp);
        CHECK(e->state == 2 || e->state == 3,
              "%s: state %d nach dem Treffer (2 HURT / 3 DEATH erwartet)", tag, e->state);
        printf("  [schuss] hp %d -> %d, state %d\n", hp0, e->hp, e->state);
    }

    /* (5) TREFFERREAKTION (Nutzer-Report 2026-08-17 "Sie reagieren nicht auf die Schuesse"):
     *     der Schuss muss den Zombie SICHTBAR und HOERBAR stoppen. Gemessen wird genau das,
     *     was der Nutzer wahrnimmt — Zustand, gerenderte Pose, SE-Id.
     *     Belege (enemy_ai_re2_zombie.c): Dispatch @0x801053E0-410, Haupt-Handler 0x80105438,
     *     Grunzer 11/12 @0x801054E8-500, Cooldown 150 @0x80105508-0C, Kosten-Abzug
     *     @0x801055D8-EC, Phasen @0x801055B4 / @0x80105850-6C / @0x80105970-7C. */
    if (want_type == 0x10) {
        re15_actor_t *e = &g_actors[first];
        /* frisch aufsetzen: lebendiger, gehender Zombie neben dem Spieler. Seit Welle B
         * (2026-09-05) auch die DOWN-Marker loeschen — der Vorlauf kann sie gesetzt haben,
         * und der Kandidatenfilter schloesse den "Liegenden" trotz state=1.1 aus. */
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hp = 60;                       /* < 81 -> kein Resistenz-Nachladen @0x80105604 */
        e->re2z_res223 = 20; e->re2z_flag222 = 0; e->re2z_cd239 = 0;
        e->hit_react = 0;
        e->grid_id &= (uint8_t)~0x80u;
        e->re2z_flags21a &= (uint16_t)~(0x2u | 0x10u | 0x200u);
        e->re2z_f10e &= (uint16_t)~0x2000u;
        e->y = 0;
        pl->x = e->x - 800; pl->z = e->z; pl->rot_y = 0;
        re15_player_set_equipped_weapon(3);

        se_n = 0;
        int16_t res_before = e->re2z_res223;
        int fired = re15_player_weapon_fire(3);
        CHECK(fired != 0, "%s: Reaktions-Probe — der Schuss trifft nicht", tag);
        printf("  [reaktion] nach dem Schuss: state=%d row(+0x5)=%d phase(+0x6)=%d col(+0x1D2)=%d\n",
               e->state, e->sub_state_1, e->sub_state_2, e->re2z_hits1d2);
        CHECK(e->sub_state_2 == 0,
              "%s: +0x6 muss nach dem Treffer 0 sein (`sw` @0x80047288 nullt +0x5..+0x7), ist %d",
              tag, e->sub_state_2);
        CHECK(e->re2z_hits1d2 == 1,
              "%s: +0x1D2 = Basis-Zone 1 (@0x80047294-98), ist %d", tag, e->re2z_hits1d2);

        view_t v0; render_view(e, &v0);
        uint32_t pose0 = ((uint32_t)v0.clip << 16) | (uint32_t)(v0.kf & 0xffff);
        int hurt_frames = 0, pose_changes = 0, se_grunt = -1;
        uint32_t lastp = pose0;
        for (int f = 0; f < 60; f++) {
            frame();
            if (e->state == 2) hurt_frames++;
            if (se_grunt < 0) { if (se_seen(11)) se_grunt = 11; else if (se_seen(12)) se_grunt = 12; }
            view_t v; render_view(e, &v);
            uint32_t p = ((uint32_t)v.clip << 16) | (uint32_t)(v.kf & 0xffff);
            if (p != lastp) { pose_changes++; lastp = p; }
            if (f == 0)
                printf("  [reaktion] frame1: state=%d phase=%d res223=%d cd239=%d clip=%d kf=%d SE=%d\n",
                       e->state, e->sub_state_2, e->re2z_res223, e->re2z_cd239, v.clip, v.kf, se_grunt);
        }
        printf("  [reaktion] HURT-Frames=%d, Pose-Wechsel=%d, Grunzer-SE=%d, res223 %d -> %d\n",
               hurt_frames, pose_changes, se_grunt, res_before, e->re2z_res223);
        CHECK(se_grunt == 11 || se_grunt == 12,
              "%s: KEIN Treffer-Grunzer — SE 11/12 @0x801054E8-500 fehlt (Nutzer-Report: kein Laut)",
              tag);
        CHECK(e->re2z_cd239 > 0,
              "%s: der Grunzer-Cooldown 150 @0x80105508-0C wurde nicht gestellt", tag);
        CHECK(hurt_frames >= 15,
              "%s: die Reaktion dauert nur %d Frames — das Original haelt state 2 ueber P0(1) + "
              "P1(3, @0x80105850-6C) + P2(17, +0x158 16->0 @0x80105970-7C)", tag, hurt_frames);
        CHECK(pose_changes >= 5,
              "%s: die gerenderte Pose aendert sich waehrend der Reaktion nur %dx", tag, pose_changes);
        CHECK(e->re2z_res223 == (int8_t)(res_before - 15),
              "%s: +0x223 -= cost[Zeile 3] = 15 (Tabelle @0x8010CC33, Abzug @0x801055D8-EC), "
              "%d -> %d", tag, res_before, e->re2z_res223);

        /* Schnellfeuer -> Knockdown 0x501: der Zwischen-Handler 0x80105BC0 laedt die Resistenz
         * in seiner Endphase wieder auf (@0x80106028-34), also muss nachgesetzt werden. */
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0;
        /* HP 120 statt 60 (Harness-Konstante, 2026-08-19). Mit dem vollstaendigen RE2-Schadens-
         * modell macht die Pistole 16 statt 5 Schaden (RE2-Zeile 0x800A412C, Waffe 3 -> Zeile 3,
         * Zone 0 = 16 @0x800A4154). GEMESSEN mit den alten 60 HP: die Resistenz laeuft wie
         * vorgesehen leer (20 -> 5 -> -10) und +0x222 kippt im dritten Schuss auf 1 — aber genau
         * dieser dritte Schuss toetet den Zombie (60 - 3*16 = 12, vierter Schuss -4), also kann
         * der Flinch @0x801050A4 nicht mehr feuern. 120 HP geben der Resistenz-Kette (das, was
         * dieser Block misst) die noetigen Treffer; der Test ist damit in BEIDEN Modi gruen. */
        e->hp = 120; e->re2z_res223 = 20; e->re2z_flag222 = 0;
        /* ⛔ 2026-08-27 FIXTURE-ISOLATION (keine Abschwaechung der Zusicherung).
         * Dieser Block behauptet "Schnellfeuer auf DIESEN Zombie loest den Niederschlag aus".
         * Er feuerte aber in einen Raum mit fuenf Zombies und verliess sich darauf, dass der
         * Aufloeser immer denselben trifft. Seit der RE2-Boden-Aufsteher treffbar ist
         * (re15_damage.c, Rise-Marker +0x21A & 0x10 @0x80103588-8c), ist ein niedergeschlagener
         * Nachbar ab seiner Aufsteh-Phase ebenfalls Kandidat und gewinnt die Auswahl.
         * GEMESSEN: ab Schuss 6 ging der Schaden an Slot 4 (st=1.5, 21A=8012 = Aufsteher,
         * hp 71->55->39->23->7), waehrend das Testziel bei hp=40 stehen blieb.
         * Das ist byte-true richtig (RE2s Aufloeser FUN_800470C0 @0x80047124-64 kennt kein
         * Band-Gate) — also muss der Test sein Ziel isolieren, statt die Auswahl zu raten. */
        for (int q = 1; q < RE15_ACTOR_MAX; q++)
            if (&g_actors[q] != e && g_actors[q].type >= 0x10 && g_actors[q].type <= 0x18)
                g_actors[q].active = 0;
        int knocked = 0;
        for (int shot = 0; shot < 10 && !knocked; shot++) {
            e->hit_react = 0;
            re15_player_weapon_fire(3);
            frame();
            if (e->state == 1 && e->sub_state_1 == 5) knocked = 1;
        }
        printf("  [reaktion] Schnellfeuer -> Knockdown=%d (state=%d sub=%d)\n",
               knocked, e->state, e->sub_state_1);
        CHECK(knocked, "%s: Schnellfeuer loest keinen Knockdown 0x501 aus (@0x801050A4-AC)", tag);
    }
    free(buf);
}

/* ---- Zensus (nur diagnostisch, argv "census") ---- */
static void census(int stage)
{
    printf("== ZENSUS STAGE%d ==\n", stage);
    for (int r = 0; r < 0x40; r++) {
        char path[300];
        snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%X%02X0.RDT", stage, stage, r);
        long sz = 0; uint8_t *buf = slurp(path, &sz);
        if (!buf) continue;
        re15_rdt_t rdt;
        if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { free(buf); continue; }
        uint8_t types[32]; int nt = 0;
        for (int sub = -1; sub < 32; sub++) {
            re15_actor_init(); re15_aot_init(); scd_vm_init();
            re15_damage_seed_rng(0x0badf00du);
            if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
            if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
            if (sub >= 0) {
                if (rdt.sub_scd_count <= sub || !rdt.sub_scd[sub]) continue;
                scd_thread_start(2, rdt.sub_scd[sub]);
            }
            g_scd.work_vars[10] = 0;
            for (int i = 0; i < 120; i++) scd_vm_tick();
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                if (!g_actors[s].active || !g_actors[s].type) continue;
                uint8_t t = g_actors[s].type;
                int dup = 0; for (int k = 0; k < nt; k++) if (types[k] == t) dup = 1;
                if (!dup && nt < 32) types[nt++] = t;
            }
        }
        if (nt) {
            printf("  ROOM%X%02X0:", stage, r);
            for (int k = 0; k < nt; k++)
                printf(" 0x%02X%s", types[k], re15_re2_owns_type(types[k]) ? "(RE2)" : "");
            printf("\n");
        }
        free(buf);
    }
}

int main(int argc, char **argv)
{
    memset(&s_def_mesh, 0, sizeof s_def_mesh);
    memset(&s_def_skel, 0, sizeof s_def_skel);
    memset(&s_def_anim, 0, sizeof s_def_anim);
    re15_re2z_audio_hook(probe_se, NULL);      /* wie platform/pc/main.c den ENEMSE-Hook setzt */

    if (argc > 1 && strcmp(argv[1], "census") == 0) {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        for (int st = 1; st <= 6; st++) census(st);
        return 0;
    }

    /* NEGATIV-Probe der TOC-Kette: kind 0x14 hat KEINEN EMD-Record (@0x8009adf4 Groesse 0) —
     * das Gate muss greifen, sonst wuerde irgendein Nachbar-Blob als Gegner geladen. */
    {
        size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
        CHECK(ems != NULL, "shared_assets/RE2/CDEMD0.EMS fehlt — der RE2-Modus laeuft dann auf "
                           "RE1.5-Modellen mit RE2-Clip-Nummern");
        if (ems) {
            re15_enemy_bank_t tmp; memset(&tmp, 0, sizeof tmp);
            CHECK(re2_ems_load_bank(ems, sz, 0x14, &tmp, NULL) != 0,
                  "kind 0x14 hat keinen EMD-Record -> load_bank MUSS fehlschlagen");
            memset(&tmp, 0, sizeof tmp);
            CHECK(re2_ems_load_bank(ems, sz, 0x10, &tmp, NULL) == 0,
                  "kind 0x10 MUSS laden (Gegenprobe zur Negativ-Probe)");
        }
    }

    run_room("ROOM1190 Hunde", RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", 13, 0x20, 1, SHAPE_QUADRUPED);
    run_room("ROOM1140 Zombies", RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", -1, 0x10, 1, SHAPE_UPRIGHT);
    run_room("ROOM10C0 Kraehen", RE15_ASSET_PSX_DIR "/STAGE1/ROOM10C0.RDT", -1, 0x21, 1, SHAPE_BIRD);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(fails ? "test_re2_livepath: %d FAIL\n" : "test_re2_livepath: OK\n", fails);
    return fails ? 1 : 0;
}
