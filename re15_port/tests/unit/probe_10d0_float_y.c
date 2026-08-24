/* probe_10d0_float_y.c — MESSSONDE (kein ctest-Assert), Nutzer-Report 2026-08-25 (FINDING 2):
 *   "der zombie in 10d0 schwebt nach dem aufstehen leicht in der Luft" (RE2-KI).
 *
 * Misst die HOEHE — Tick fuer Tick, ueber die ganze Sequenz Sitzen -> Wecken -> Aufstehen ->
 * erste RE2-Ticks, und zwar GENAU so, wie der Renderer sie rechnet:
 *     bone_world = rotate(pose[bone].trans, yaw) + (e->x, e->y, e->z)      (main.c:6975-6979)
 * Ausgegeben werden deshalb:
 *   e->y / e->floor            — die Aktor-Wurzel (Boden-/Band-Wert)
 *   MINY                       — der TIEFSTE Bone in Weltkoordinaten = die Fusssohle;
 *                                MINY == 0 hiesse "steht exakt auf der Bodenebene Y=0"
 *   b0y (Wurzel-Bone) / b8y    — Hueft- und Brustbone in Weltkoordinaten
 *   Bank                       — 0 Aktions-, 1 RE1.5-Sitz-Pose-Bank, 2 Loco-Bank
 *
 * Die Bank wird EXAKT wie in platform/pc/main.c gewaehlt (Sitz-Import-Zweig main.c:6817-6830,
 * Loco-Zweig main.c:6752-6759) und — anders als probe_10d0_situp_re2 — wird der WELLE-G-HYBRID
 * angewandt (pc_enemy_hybrid_re15_models -> re2_hybrid_apply): der laeuft im RE2-Flavor
 * BEDINGUNGSLOS (main.c:668-676) und ersetzt die RE2-Bind-Laengen durch die RE1.5-Laengen.
 * Ohne ihn misst man ein Skelett, das im Spiel nicht existiert.
 *
 * Aufruf: probe_10d0_float_y [mode]
 *   0 = ROOM10D0, RE2-Flavor  (Sitz-Import -> Aufstehen -> RE2-WALK)   <- der Report
 *   1 = ROOM10D0, RE1.5-Flavor (Referenz: derselbe Sitzer ohne RE2-Uebergabe)
 *   2 = ROOM1140, RE2-Flavor  (stehende Zombies, NIE durch den Sitz-Import) <- Gegenprobe
 *   3 = ROOM1140, RE1.5-Flavor (Gegenprobe der Gegenprobe)
 *   4 = wie 0, aber OHNE Hybrid (misst den Beitrag des Hybrid-Rigs)
 *   5 = Bind-Offsets (bone_relative_pos) RE2 vs. RE1.5, vor/nach dem Hybrid
 *   6 = Ketten-Delta Wurzel->Bone fuer alle Hybrid-Typen (Skopus des Fehlers)
 *   7 = Mesh-Tiefe (tiefster Vertex je Bone) RE2 vs. RE1.5
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
#include "re15_esp.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_actor_uses_loco_bank(const re15_actor_t *a);
extern int  re15_compute_actor_kf(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
                                  const re15_actor_t *a, int clip_override, uint32_t frame);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_render_semantics = 1;   /* Renderer-Semantik (FRAC-Crossfade) */

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- RE1.5-EM10 (Geometrie + Sitz-Pose-Bank + RE1.5-Flavor-Bank) ------------------------- */
static uint8_t  s_em15[0x80000];
static size_t   s_em15_len = 0;
static re15_emd_skeleton_t  s_sk15;
static re15_emd_animation_t s_an15;
static re15_md1_t           s_md15;
static int load_re15_emd(uint8_t type)
{
    size_t ems_n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems_n);
    if (!ems) { fprintf(stderr, "WARN: PSX/EMD/CDEMD0.EMS fehlt\n"); return -1; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int rc = -1;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_n, idx, &off, &len) == 0 && len <= sizeof s_em15) {
        memcpy(s_em15, ems + off, len); s_em15_len = len;
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        rc = re15_emd_parse_container(s_em15, len, &s_md15, &s_sk15, &s_an15, &tim);
    }
    free(ems);
    return rc;
}

static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static void load_bank_re2(uint8_t type, int hybrid)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) { fprintf(stderr, "WARN: RE2/CDEMD0.EMS fehlt\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return; }
    eb->buf = NULL; eb->ok = 1;
    if (hybrid && s_em15_len > 0) {                  /* main.c:675 — laeuft im Spiel IMMER */
        int um = -1;
        int rc = re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &um);
        fprintf(stderr, "[probe] hybrid EM%02X rc=%d unmapped=%d\n", type, rc, um);
    }
}
static void load_bank_re15(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb || s_em15_len == 0) return;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_em15, s_em15_len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_em15, s_em15_len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_em15, s_em15_len, &eb->skel_own,  &eb->anim_own)  == 0);
    }
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Die Bankwahl des RENDERERS (main.c) + die volle Pose. Liefert MINY/b0y/b8y in WELT-Y. */
typedef struct { int32_t miny; int miny_bone; int32_t b0y, b8y; int kf, bank; } posey_t;
static int pose_all(const re15_actor_t *e, posey_t *out)
{
    memset(out, 0, sizeof *out); out->kf = -1; out->bank = -1;
    const re15_emd_skeleton_t  *sk = NULL;
    const re15_emd_animation_t *an = NULL;
    int clip_override = -1, which = 0;
    if (e->re2z_re15_pose && re15_re2z_re15_pose_anim() &&
        (int)e->motion < re15_re2z_re15_pose_anim()->clip_count) {
        sk = re15_re2z_re15_pose_skel(); an = re15_re2z_re15_pose_anim();
        clip_override = (int)e->motion; which = 1;                 /* main.c:6817-6830 */
    } else {
        re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (!b) return 0;
        sk = &b->skel; an = &b->anim; which = 0;
        if (re15_actor_uses_loco_bank(e) && b->loco_ok &&
            (int)e->motion < b->anim_loco.clip_count) {            /* main.c:6752-6759 */
            sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion; which = 2;
        }
    }
    out->bank = which;
    if (!sk || !an || sk->bone_count <= 0 || an->clip_count <= 0 || sk->keyframe_count <= 0)
        return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    out->kf = kf;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = s_render_semantics ? (void *)(uintptr_t)e : NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t w[3];
    out->miny = 0x7fffffff; out->miny_bone = -1;
    for (int b = 0; b < sk->bone_count; b++) {
        re15_skel_bone_to_world(poses[b].trans, e->rot_y, e->x, e->y, e->z, w);
        if (w[1] < out->miny) { out->miny = w[1]; out->miny_bone = b; }   /* PSX-Y: kleiner = HOEHER */
        if (b == 0) out->b0y = w[1];
        if (b == 8) out->b8y = w[1];
    }
    /* MINY = kleinster Y = HOECHSTER Punkt. Fuer die Fusssohle brauchen wir das MAXIMUM. */
    return 1;
}
/* PSX-Y zeigt nach UNTEN: grosses Y = tief. Die Sohle ist also der GROESSTE Bone-Y-Wert. */
static int pose_sole(const re15_actor_t *e, posey_t *out)
{
    if (!pose_all(e, out)) return 0;
    /* pose_all hat das Minimum gesucht; hier das Maximum nachziehen. */
    const re15_emd_skeleton_t  *sk = NULL;
    const re15_emd_animation_t *an = NULL;
    int clip_override = -1;
    if (e->re2z_re15_pose && re15_re2z_re15_pose_anim() &&
        (int)e->motion < re15_re2z_re15_pose_anim()->clip_count) {
        sk = re15_re2z_re15_pose_skel(); an = re15_re2z_re15_pose_anim();
        clip_override = (int)e->motion;
    } else {
        re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (!b) return 0;
        sk = &b->skel; an = &b->anim;
        if (re15_actor_uses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count)
        { sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion; }
    }
    if (!sk || !an) return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;   /* der Crossfade lief schon in pose_all — hier nur Geometrie */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t w[3]; int32_t maxy = -0x7fffffff; int maxb = -1;
    for (int b = 0; b < sk->bone_count; b++) {
        re15_skel_bone_to_world(poses[b].trans, e->rot_y, e->x, e->y, e->z, w);
        if (w[1] > maxy) { maxy = w[1]; maxb = b; }
    }
    out->miny = maxy; out->miny_bone = maxb;   /* Feld wird als SOHLE weiterverwendet */
    return 1;
}

static void row(int t, const re15_actor_t *e)
{
    posey_t p;
    if (!pose_sole(e, &p)) { printf("t%-4d (keine Pose)\n", t); return; }
    printf("t%-4d st=%u s1=0x%02x s2=%u grid=0x%02x p15=%u | clip=%-3d af=%-3u len=%-3d kf=%-4d "
           "bank=%d | e->y=%-6ld floor=%-3u | SOHLE(b%-2d)=%-7ld b0y=%-7ld b8y=%-7ld | "
           "frac=%-2u rate=%u\n",
           t, e->state, e->sub_state_1, e->sub_state_2, e->grid_id, e->re2z_re15_pose,
           (int)e->motion, e->anim_frame, re15_actor_clip_len(e), p.kf, p.bank,
           (long)e->y, e->floor,
           p.miny_bone, (long)p.miny, (long)p.b0y, (long)p.b8y,
           e->anim_frac, e->anim_blend_rate);
}

static re15_actor_t *find_type(uint8_t type, int nth)
{
    int seen = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == type) { if (seen++ == nth) return &g_actors[s]; }
    return NULL;
}

int main(int argc, char **argv)
{
    int mode = (argc > 1) ? atoi(argv[1]) : 0;
    int flavor_re2 = (mode == 0 || mode == 2 || mode == 4 || mode == 5 || mode == 6 || mode == 7);
    int hybrid     = (mode != 4 && mode != 5 && mode != 6 && mode != 7);
    int room1140   = (mode == 2 || mode == 3);

    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%s.RDT", base, room1140 ? "1140" : "10D0");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    if (load_re15_emd(0x10) != 0) { printf("FAIL: RE1.5 EM10 nicht ladbar\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(flavor_re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = room1140 ? 0x1140 : 0x10D0;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();

    if (flavor_re2) {
        load_bank_re2(0x10, hybrid); load_bank_re2(0x11, hybrid);
        load_bank_re2(0x16, hybrid); load_bank_re2(0x40, hybrid);
        /* Sitz-Pose-Bank wie main.c:539-548 (dedizierte Kopie der RE1.5-Bank). */
        static re15_emd_skeleton_t  sit_sk; static re15_emd_animation_t sit_an;
        sit_sk = s_sk15; sit_an = s_an15;
        re15_re2z_set_re15_pose_bank(&sit_sk, &sit_an);
    } else {
        load_bank_re15(0x10); load_bank_re15(0x11); load_bank_re15(0x16);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);

    /* ---- BANK-GEOMETRIE: die BIND-Pose-Hoehen, aus denen die Sohle faellt ---- */
    {
        re15_enemy_bank_t *b = re15_enemy_find(0x10);
        printf("== BANKS (flavor=%s hybrid=%d) ==\n", flavor_re2 ? "RE2" : "RE15", hybrid);
        if (b && b->ok) {
            printf("  Aktion: clips=%d bones=%d | Loco ok=%d clips=%d bones=%d | Own ok=%d clips=%d\n",
                   b->anim.clip_count, b->skel.bone_count, b->loco_ok, b->anim_loco.clip_count,
                   b->skel_loco.bone_count, b->own_ok, b->anim_own.clip_count);
        }
        const re15_emd_animation_t *pa = re15_re2z_re15_pose_anim();
        if (pa) printf("  Sitz-Pose-Bank: clips=%d [0x29]=%d [0x2A]=%d\n", pa->clip_count,
                       (0x29 < pa->clip_count) ? pa->clips[0x29].frame_count : -1,
                       (0x2A < pa->clip_count) ? pa->clips[0x2A].frame_count : -1);
    }

    if (mode == 7) {
        /* Wie tief reicht das FUSS-MESH unter seinen Bone? RE2-Original vs. RE1.5 (das der
         * Hybrid einsetzt). Erklaert den Rest-Versatz, den die Ketten-Differenz nicht deckt. */
        re15_enemy_bank_t *b = re15_enemy_find(0x10);   /* RE2, OHNE Hybrid (mode 7) */
        const int8_t *perm = NULL; (void)re2_hybrid_perm(0x10, &perm);
        for (int rb = 0; rb < 15; rb++) {
            int mi = perm[rb];
            const re15_md1_mesh_t *m2 = &b->md1.meshes[rb];        /* RE2-Mesh, RE2-Bone-Index  */
            const re15_md1_mesh_t *m5 = &s_md15.meshes[mi];        /* RE1.5-Mesh, RE1.5-Index   */
            int32_t y2 = -0x7fffffff, y5 = -0x7fffffff;
            for (int i = 0; i < m2->tri_vertex_count; i++) if (m2->tri_vertices[i].y > y2) y2 = m2->tri_vertices[i].y;
            for (int i = 0; i < m2->quad_vertex_count; i++) if (m2->quad_vertices[i].y > y2) y2 = m2->quad_vertices[i].y;
            for (int i = 0; i < m5->tri_vertex_count; i++) if (m5->tri_vertices[i].y > y5) y5 = m5->tri_vertices[i].y;
            for (int i = 0; i < m5->quad_vertex_count; i++) if (m5->quad_vertices[i].y > y5) y5 = m5->quad_vertices[i].y;
            printf("  RE2-bone%-2d (RE15-mesh %-2d): tiefster Vertex RE2=%-6d RE15=%-6d  D=%+d\n",
                   rb, mi, y2, y5, y5 - y2);
        }
        return 0;
    }

    if (mode == 6) {
        /* SKOPUS: welche RE2-Hybrid-Typen verlieren/gewinnen Ketten-Laenge?
         * Pro Typ: die aufsummierte Y-Kette Wurzel->Bone VOR und NACH dem Hybrid. */
        static const uint8_t types[] = { 0x10, 0x11, 0x16, 0x13, 0x20, 0x21 };
        for (unsigned k = 0; k < sizeof types; k++) {
            uint8_t ty = types[k];
            if (load_re15_emd(ty) != 0) { printf("EM%02X: kein RE1.5-EMD\n", ty); continue; }
            re15_enemy_reset();
            load_bank_re2(ty, 0);
            re15_enemy_bank_t *b = re15_enemy_find(ty);
            if (!b || !b->ok || !b->loco_ok) { printf("EM%02X: keine RE2-Loco-Bank\n", ty); continue; }
            int n = b->skel_loco.bone_count;
            int32_t before[RE15_EMD_MAX_BONES];
            for (int i = 0; i < n; i++) {
                int32_t s = 0, j = i;
                while (j >= 0 && b->skel_loco.bone_parent[j] >= 0)
                { s += b->skel_loco.bone_relative_pos[j][1]; j = b->skel_loco.bone_parent[j]; }
                before[i] = s;
            }
            int um = -1; int rc = re2_hybrid_apply(b, (int)ty, &s_md15, &s_sk15, &um);
            printf("EM%02X (rc=%d unmapped=%d, %d Bones): Kette Wurzel->Bone, Y vor/nach Hybrid\n",
                   ty, rc, um, n);
            for (int i = 0; i < n; i++) {
                int32_t s = 0, j = i;
                while (j >= 0 && b->skel_loco.bone_parent[j] >= 0)
                { s += b->skel_loco.bone_relative_pos[j][1]; j = b->skel_loco.bone_parent[j]; }
                if (s != before[i])
                    printf("    bone%-2d  %6d -> %6d   (D = %+d)\n", i, before[i], s, s - before[i]);
            }
        }
        return 0;
    }

    if (mode == 5) {
        /* BIND-GEOMETRIE: die Bein-Kette in beiden Rigs, Bone fuer Bone.
         * perm[i] (re2_ems.c:160) = RE1.5-Index des RE2-Bones i. */
        re15_enemy_bank_t *b = re15_enemy_find(0x10);
        const int8_t *perm = NULL; int n = re2_hybrid_perm(0x10, &perm);
        printf("== BIND-OFFSETS (bone_relative_pos), RE2-Reihenfolge ==\n");
        printf("  i  parent  perm[i]=RE15  | RE2 (x,y,z)            | RE15 (x,y,z)\n");
        for (int i = 0; i < n; i++) {
            int mi = perm[i];
            printf("  %-2d  %-4d   %-3d          | (%6d,%6d,%6d) | (%6d,%6d,%6d)\n",
                   i, (int)b->skel_loco.bone_parent[i], mi,
                   b->skel_loco.bone_relative_pos[i][0], b->skel_loco.bone_relative_pos[i][1],
                   b->skel_loco.bone_relative_pos[i][2],
                   s_sk15.bone_relative_pos[mi][0], s_sk15.bone_relative_pos[mi][1],
                   s_sk15.bone_relative_pos[mi][2]);
        }
        printf("  (RE15-parent:");
        for (int i = 0; i < s_sk15.bone_count; i++) printf(" [%d]=%d", i, (int)s_sk15.bone_parent[i]);
        printf(")\n");
        printf("  (RE15-relpos:");
        for (int i = 0; i < s_sk15.bone_count; i++)
            printf(" [%d]=(%d,%d,%d)", i, s_sk15.bone_relative_pos[i][0],
                   s_sk15.bone_relative_pos[i][1], s_sk15.bone_relative_pos[i][2]);
        printf(")\n");
        /* Jetzt den Hybrid anwenden und DIESELBE Kette noch einmal zeigen. */
        { int um = -1; re2_hybrid_apply(b, 0x10, &s_md15, &s_sk15, &um);
          printf("== NACH HYBRID (re2_hybrid_apply, unmapped=%d) ==\n", um);
          int sum2 = 0;
          for (int i = 0; i < n; i++)
              printf("  %-2d parent=%-3d y=%d\n", i, (int)b->skel_loco.bone_parent[i],
                     b->skel_loco.bone_relative_pos[i][1]);
          for (int i = 1; i <= 4; i++) sum2 += b->skel_loco.bone_relative_pos[i][1];
          printf("  Bein-Kette (Bone 1->2->3->4) Y-Summe NACH Hybrid = %d\n", sum2);
        }
        return 0;
    }

    if (room1140) {
        /* Gegenprobe: STEHENDE Zombies, nie durch den Sitz-Import. Spieler weit weg lassen,
         * dann herholen; gemessen wird der ERSTE 0x10. */
        pl->x = 0; pl->z = 0;
        re15_actor_t *e = find_type(0x10, 0);
        if (!e) { printf("FAIL: kein 0x10 in ROOM1140\n"); return 1; }
        printf("== SPAWN == slot=%d grid=0x%02X st=%u s1=0x%02x pos=(%ld,%ld,%ld) floor=%u\n",
               (int)(e - g_actors), e->grid_id, e->state, e->sub_state_1,
               (long)e->x, (long)e->y, (long)e->z, e->floor);
        for (int t = 0; t < 160 && e->active; t++) {
            pl->hp = 100;
            pl->x = e->x + 1200; pl->z = e->z;      /* nah -> wecken/engagieren */
            frame(0, 0);
            if (t % 4 == 0 || t < 8) row(t, e);
        }
        return 0;
    }

    /* ROOM10D0 — die volle Sitz-Sequenz. */
    pl->x = 5878; pl->z = 11400; pl->rot_y = 2048;
    for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }
    re15_actor_t *e = find_type(0x10, 0);
    if (!e) { printf("FAIL: kein Zombie 0x10 gespawnt\n"); return 1; }
    printf("== SPAWN == slot=%d grid=0x%02X st=%u s1=0x%02x p15=%u clip=%d pos=(%ld,%ld,%ld) floor=%u\n",
           (int)(e - g_actors), e->grid_id, e->state, e->sub_state_1, e->re2z_re15_pose,
           (int)e->motion, (long)e->x, (long)e->y, (long)e->z, e->floor);
    printf("== SITZEN (fern) ==\n");
    for (int t = 0; t < 20; t++) { pl->hp = 100; frame(0, 0); if (t % 10 == 0) row(t, e); }
    printf("== WECKEN -> AUFSTEHEN -> RE2 ==\n");
    pl->x = 5878; pl->z = 24000;
    for (int t = 0; t < 200 && e->active; t++) { pl->hp = 100; frame(0, 0); row(t, e); }
    return 0;
}
