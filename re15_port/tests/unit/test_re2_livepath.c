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
