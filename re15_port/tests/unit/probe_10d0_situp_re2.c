/* probe_10d0_situp_re2.c — MESSSONDE (kein ctest-Assert), Nutzer-Report 2026-08-24 (FINDING 3):
 *   "stimmt der Zombie jetzt fast, nur beim aufstehen wiederholt sich die Eine Animation noch
 *    einmal kurz, bis er dann letztlich steht. Ausserdem wurde mein Schuss bei ihn auf den
 *    Boden wieder nicht mit Blut quittiert."
 *
 * Gemessen wird der 10D0-SITZER (Sce_em_set slot=1 type=0x10 behavior=0x0e, RDT sub00 +0x5e)
 * im RE2-KI-Modus, also ueber den SITZ-IMPORT (enemy_ai_common.c re15_enemy_ai_live_tick:
 * re2z_re15_pose -> RE1.5-Maschinen 0x12 SLEEPING / 0x0d STANDUP, Handoff 0x101).
 *
 * A) AUFSTEH-SEQUENZ: jeder Tick mit st/s1/s2/grid/clip/anim_frame/clip_len/RENDER-SLOT
 *    (anim_frame %% clip_len — genau das, was der Renderer per clip_override posiert).
 *    Ein "Wiederholen" ist damit als Zahl sichtbar: Slot springt zurueck, oder ein Clip wird
 *    zweimal von 0 gestartet.
 * B) BODENTREFFER: Schuss auf den SITZENDEN Zombie (Zustand 1/0x12) — Bandtor, Trefferannahme,
 *    Zeile +0x5, Blut-Spawns (re15_esp_fx_count-Delta).
 *
 * Aufruf: probe_10d0_situp_re2 [mode]
 *   mode 0 = A (Aufstehen)   mode 1 = B (Schuss sitzend, Aim LEVEL)
 *   mode 2 = B mit Aim DOWN  mode 3 = B mit Aim UP
 *   mode 4 = A im RE1.5-Flavor (Referenz)
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

extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_actor_clip_len_legacy(const re15_actor_t *a);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_player_set_aim_elevation_for_test(int elev);
extern int  re15_player_aim_ready(void);
extern void re15_player_aim_reset(void);
extern uint32_t re15_re2_rand(void);
extern int  re15_re2z_last_hit_handler(void);

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

/* --- RE2-Bank (wie platform/pc/main.c pc_enemy_load im RE2-Flavor) ------------------------ */
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static void load_bank_re2(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) { fprintf(stderr, "WARN: RE2/CDEMD0.EMS fehlt\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

/* --- RE1.5-Bank (RE15-Flavor-Referenz + die POSE-BANK des Sitz-Imports) ------------------- */
static uint8_t  s_em10[0x80000];
static size_t   s_em10_len = 0;
static re15_emd_skeleton_t  s_sk15;
static re15_emd_animation_t s_an15;
static re15_md1_t           s_md15;
static int load_re15_em10(void)
{
    size_t ems_n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems_n);
    if (!ems) { fprintf(stderr, "WARN: PSX/EMD/CDEMD0.EMS fehlt\n"); return -1; }
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0;
    int rc = -1;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_n, idx, &off, &len) == 0 && len <= sizeof s_em10) {
        memcpy(s_em10, ems + off, len); s_em10_len = len;
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        rc = re15_emd_parse_container(s_em10, len, &s_md15, &s_sk15, &s_an15, &tim);
    }
    free(ems);
    return rc;
}
static void load_bank_re15(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb || s_em10_len == 0) return;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_em10, s_em10_len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_em10, s_em10_len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_em10, s_em10_len, &eb->skel_own,  &eb->anim_own)  == 0);
    }
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static re15_actor_t *find_z(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) return &g_actors[s];
    return NULL;
}

/* Die POSE aus GENAU der Bank, aus der platform/pc/main.c posiert (Sitz-Import-Zweig
 * main.c:6804-6816 -> RE1.5-Pose-Bank, sonst Aktions-/Loco-Bank). Rueckgabe: Keyframe-Index
 * + Weltposition des Bones. Nur so ist ein "wiederholt sich" als Zahl sichtbar. */
extern int re15_compute_actor_kf(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
                                 const re15_actor_t *a, int clip_override, uint32_t frame);
extern int re15_actor_uses_loco_bank(const re15_actor_t *a);
/* 1 = die Pose wird wie im RENDERER berechnet (g_anim_pose_actor gesetzt -> der FRAC-Crossfade
 * FUN_8001f3bc mischt die zuletzt GERENDERTE Pose ein, rekursiv). 0 = reine QUERY. */
static int s_render_semantics = 0;
static int pose_bone(const re15_actor_t *e, int bone, int32_t out[3], int *out_kf, int *bank)
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z; if (out_kf) *out_kf = -1; if (bank) *bank = -1;
    const re15_emd_skeleton_t  *sk = NULL;
    const re15_emd_animation_t *an = NULL;
    int clip_override = -1, which = 0;
    if (e->re2z_re15_pose && re15_re2z_re15_pose_anim() &&
        (int)e->motion < re15_re2z_re15_pose_anim()->clip_count) {
        sk = re15_re2z_re15_pose_skel(); an = re15_re2z_re15_pose_anim();
        clip_override = (int)e->motion; which = 1;
    } else {
        re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (!b) return 0;
        sk = &b->skel; an = &b->anim; which = 0;
        if (re15_actor_uses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
            sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion; which = 2;
        }
    }
    if (bank) *bank = which;
    if (!sk || !an || sk->bone_count <= 0 || bone < 0 || bone >= sk->bone_count) return 0;
    if (an->clip_count <= 0 || sk->keyframe_count <= 0) return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    if (out_kf) *out_kf = kf;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = s_render_semantics ? (void *)(uintptr_t)e : NULL; /* QUERY vs RENDERER */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[bone].trans, e->rot_y, e->x, e->y, e->z, out);
    return 1;
}

static void row(int t, const re15_actor_t *e, int fx)
{
    int clen = re15_actor_clip_len(e);
    int cleg = re15_actor_clip_len_legacy(e);
    int slot = (clen > 0) ? (int)((uint32_t)e->anim_frame % (uint32_t)clen) : -1;
    int32_t b8[3], b0[3]; int kf = -1, bk = -1;
    pose_bone(e, 8, b8, &kf, &bk);                 /* GENAU EIN Render-Aufruf pro Tick */
    { int sv = s_render_semantics; s_render_semantics = 0;
      pose_bone(e, 0, b0, NULL, NULL); s_render_semantics = sv; }
    printf("t%-4d st=%u s1=0x%02x s2=%u s3=%u grid=0x%02x pose15=%u | clip=%-3d af=%-3u "
           "len=%-3d (act-bank %-3d) SLOT=%-3d kf=%-4d bank=%d | b8dy=%-6ld b0dy=%-6ld | "
           "hp=%d frac=%u rate=%u af_flags=%04X 21A=%04X 1D2=%u 93=%02X dist=%u fx=%d\n",
           t, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, e->grid_id,
           e->re2z_re15_pose, (int)e->motion, e->anim_frame, clen, cleg, slot, kf, bk,
           (long)(b8[1] - e->y), (long)(b0[1] - e->y),
           (int)e->hp, e->anim_frac, e->anim_blend_rate, e->anim_flags,
           e->re2z_flags21a, e->re2z_hits1d2, e->hit_react,
           (unsigned)e->ai_dist, fx);
}

static void banks_dump(void)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    printf("== BANKS ==\n");
    if (b && b->ok) {
        printf("  AKTIONS-BANK (die der Flavor geladen hat): clips=%d bones=%d\n",
               b->anim.clip_count, b->skel.bone_count);
        printf("    len:");
        for (int c = 0; c < b->anim.clip_count; c++) printf(" [%d]=%d", c, b->anim.clips[c].frame_count);
        printf("\n");
        printf("  LOCO ok=%d clips=%d | OWN ok=%d clips=%d\n",
               b->loco_ok, b->anim_loco.clip_count, b->own_ok, b->anim_own.clip_count);
    } else printf("  AKTIONS-BANK FEHLT\n");
    const re15_emd_animation_t *pa = re15_re2z_re15_pose_anim();
    if (pa) {
        printf("  RE1.5-POSE-BANK (Sitz-Import): clips=%d  [0x29]=%d [0x2A]=%d\n",
               pa->clip_count,
               (0x29 < pa->clip_count) ? pa->clips[0x29].frame_count : -1,
               (0x2A < pa->clip_count) ? pa->clips[0x2A].frame_count : -1);
    } else printf("  RE1.5-POSE-BANK NICHT REGISTRIERT\n");
}

static void setup_room(int flavor_re2)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(flavor_re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x10D0;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (flavor_re2) { load_bank_re2(0x10); load_bank_re2(0x40); }
    else            { load_bank_re15(0x10); }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    /* weit weg: der Sitzer darf beim Hochfahren des Raums nicht wecken (Tor dist < 0xBB8) */
    pl->x = 5878; pl->z = 11400; pl->rot_y = 2048;
    for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }
}

int main(int argc, char **argv)
{
    int mode = (argc > 1) ? atoi(argv[1]) : 0;
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int flavor_re2 = (mode != 4 && mode != 5);
    if (mode == 6 || mode == 7 || mode == 8) s_render_semantics = 1;  /* 6 = RE2 + Crossfade,
                                                                       * 7 = RE1.5, 8 = Regression */
    if (mode == 7) flavor_re2 = 0;
    if (flavor_re2) {
        /* Der Sitz-Import braucht die RE1.5-Pose-Bank, die die Plattform im Hybrid-Lauf
         * registriert (platform/pc/main.c:539-548). Ohne sie faellt sel 0x0e in den
         * RE2-Liege-Fallback — dann misst die Sonde einen anderen Pfad. */
        if (load_re15_em10() == 0) re15_re2z_set_re15_pose_bank(&s_sk15, &s_an15);
        else fprintf(stderr, "WARN: RE1.5-EM10 nicht ladbar -> Sitz-Import inaktiv\n");
    } else {
        (void)load_re15_em10();
    }

    setup_room(flavor_re2);
    banks_dump();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = find_z();
    if (!e) { printf("FAIL: kein Zombie 0x10 gespawnt\n"); return 1; }
    printf("== SPAWN == slot=%d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u pose15=%u "
           "clip=%d pos=(%ld,%ld,%ld)\n",
           (int)(e - g_actors), e->type, e->grid_id, e->state, e->sub_state_1, e->sub_state_2,
           e->re2z_re15_pose, (int)e->motion, (long)e->x, (long)e->y, (long)e->z);

    /* --- MODUS 8: REGRESSION (kein Diagnose-Dump, Rueckgabewert != 0 bei Divergenz) ----------
     * Prueft GENAU den Uebergabe-Tick RE1.5-Aufsteher -> RE2-Gehirn, mit RENDERER-Semantik
     * (FRAC-Crossfade), also so, wie der Nutzer es sieht.
     *
     * SOLLSEITE (selbst disassembliert, EMOVL10_S0.BIN): der ACTIVE-Root laesst DECISION und
     * EXECUTOR im SELBEN Tick laufen und liest +0x5 dazwischen NEU —
     *   801011c4: jalr v0             ; DECISION 0x8010C88C[+0x5]
     *   801011d0: lbu  v0,5(s0)       ; +0x5 ERNEUT
     *   801011e4: lw   v0,-14132(at)  ; EXECUTOR 0x8010C8CC[+0x5]
     *   801011ec: jalr v0
     * und EXEC[1] P0 schreibt sein Clip-Wort in demselben Tick:
     *   80101a7c: lbu v0,536(s1) / 80101a80-84: lui v1,0xf / addu v0,v0,v1
     *   80101a8c: sw  v0,332(s1)      ; +0x14C = Clip | FRAC-Saat 0xF
     * Es gibt also KEINEN Tick "neuer Zustand, alter Clip".
     *
     * ZWEI ASSERTS, beide ohne geratene Konstante:
     *  (1) im ersten Tick mit +0x5 == 1 muss +0x94 ein GUELTIGER Index der RE2-Aktions-Bank
     *      sein. Vorher stand dort der RE1.5-Clip 41 (0x29) gegen 31 Clips -> 41 % 31 = 10.
     *  (2) ab diesem Tick darf keine gerenderte Brust-Hoehe unter die SITZ-Pose fallen (die
     *      tiefste legitime Pose der ganzen Sequenz, aus DIESEM Lauf gemessen, nicht geraten).
     *      Der Defekt mass hier Brust 179 statt 2266 = flach am Boden. */
    if (mode == 8) {
        int fail = 0;
        printf("== REGRESSION: 10D0-Sitzer, Uebergabe-Tick Aufsteher -> RE2-Gehirn ==\n");
        re15_enemy_bank_t *ab = re15_enemy_find(0x10);
        if (!ab || !ab->ok) { printf("SKIP: RE2-Aktions-Bank fehlt\n"); return 77; }
        if (!re15_re2z_re15_pose_anim()) { printf("SKIP: RE1.5-Pose-Bank fehlt\n"); return 77; }
        int32_t b[3]; int kf = -1, bk = -1;
        for (int t = 0; t < 40; t++) { pl->hp = 100; frame(0, 0); }
        pose_bone(e, 8, b, &kf, &bk);
        long sit_dy = (long)(b[1] - e->y);            /* Referenz = die SITZ-Pose selbst */
        printf("   Sitz-Referenz: b8dy=%ld (clip=%d af=%u)\n", sit_dy, (int)e->motion, e->anim_frame);
        if (!(e->state == 1 && e->sub_state_1 == 0x12)) {
            printf("FAIL: Sitzer nicht in SLEEPING 0x12 (st=%u s1=0x%02x)\n",
                   e->state, e->sub_state_1);
            return 1;
        }
        pl->x = 5878; pl->z = 24000;                  /* dist < 0xBB8 -> Wecken */
        int handoff = -1, worst_t = -1; long worst_dy = -0x7fffffffL;
        int handoff_clip = -1;
        for (int t = 0; t < 220 && e->active; t++) {
            pl->hp = 100; frame(0, 0);
            pose_bone(e, 8, b, &kf, &bk);             /* genau EIN Render-Aufruf pro Tick */
            long dy = (long)(b[1] - e->y);
            if (handoff < 0 && e->state == 1 && e->sub_state_1 == 1) {
                handoff = t; handoff_clip = (int)e->motion;
                printf("   Uebergabe-Tick t%d: clip=%d (RE2-Bank hat %d Clips) af=%u frac=%u "
                       "b8dy=%ld\n", t, handoff_clip, ab->anim.clip_count, e->anim_frame,
                       e->anim_frac, dy);
            }
            if (handoff >= 0 && dy > worst_dy) { worst_dy = dy; worst_t = t; }
            if (handoff >= 0 && t - handoff > 30) break;
        }
        if (handoff < 0) { printf("FAIL: der Sitzer erreicht den RE2-WALK nie\n"); return 1; }
        if (handoff_clip < 0 || handoff_clip >= ab->anim.clip_count) {
            printf("FAIL: der Uebergabe-Tick traegt Clip %d, die RE2-Bank hat nur %d Clips — "
                   "das Original schreibt das Clip-Wort im SELBEN Tick (@0x80101a8c)\n",
                   handoff_clip, ab->anim.clip_count);
            fail++;
        }
        /* PSX-Y zeigt nach unten: "tiefer" = groesserer (weniger negativer) Wert. */
        if (worst_dy > sit_dy) {
            printf("FAIL: gerenderte Brust faellt bei t%d auf b8dy=%ld — tiefer als die SITZ-Pose "
                   "(%ld). Das ist der Zusammenbruch aus dem Nutzer-Report\n",
                   worst_t, worst_dy, sit_dy);
            fail++;
        } else {
            printf("   tiefste Brust nach der Uebergabe: b8dy=%ld bei t%d (Sitz-Referenz %ld)\n",
                   worst_dy, worst_t, sit_dy);
        }
        printf("probe_10d0_situp_re2: %s\n", fail ? "FAILURES" : "OK");
        return fail ? 1 : 0;
    }

    if (mode == 0 || mode == 4 || mode == 6 || mode == 7) {
        printf("== A: 40 Ticks FERN (sitzen halten) ==\n");
        for (int t = 0; t < 40; t++) { pl->hp = 100; frame(0, 0); if (t % 10 == 0) row(t, e, re15_esp_fx_count()); }
        printf("== A: Spieler NAH (dist<0xBB8) -> Wecken + Aufstehen, JEDER Tick ==\n");
        pl->x = 5878; pl->z = 24000;
        for (int t = 0; t < 200; t++) {
            pl->hp = 100; frame(0, 0);
            row(t, e, re15_esp_fx_count());
            if (!e->active) break;
        }
        return 0;
    }

    /* --- B: Schuss auf den SITZENDEN Zombie ------------------------------------------------- */
    int elev = (mode == 2) ? -1 : (mode == 3) ? 1 : 0;
    printf("== B: Schuss auf den Sitzer, Aim-Elevation %d ==\n", elev);
    /* Nah genug zum Zielen, aber Wecken passiert bei dist<0xBB8 — deshalb ZWEI Laeufe:
     * erst weit (kein Wecken, dist 3694), dann nah. */
    for (int pass = 0; pass < 2; pass++) {
        setup_room(flavor_re2);
        if (flavor_re2) re15_re2z_set_re15_pose_bank(&s_sk15, &s_an15);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        e  = find_z();
        if (!e) { printf("FAIL pass %d: kein Zombie\n", pass); return 1; }
        pl->x = e->x; pl->z = e->z - (pass == 0 ? 3694 : 1694);
        pl->rot_y = 3072;                       /* Blickrichtung +z (zum Zombie) */
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
        re15_player_set_aim_elevation_for_test(elev);
        printf("-- pass %d: dist=%u  st=%u s1=0x%02x s2=%u clip=%d aim_ready=%d\n",
               pass, (unsigned)e->ai_dist, e->state, e->sub_state_1, e->sub_state_2,
               (int)e->motion, re15_player_aim_ready());
        int hp0 = e->hp, fx0 = re15_esp_fx_count();
        for (int t = 0; t < 60; t++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (t == 5) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
            re15_player_set_aim_elevation_for_test(elev);
            frame(cur, edge);
            row(t, e, re15_esp_fx_count());
            if (!e->active) break;
        }
        printf("-- pass %d ERGEBNIS: hp %d -> %d (Treffer=%s), fx-Spawns=%d, letzter Handler=%d\n",
               pass, hp0, e->hp, (e->hp < hp0) ? "JA" : "NEIN",
               re15_esp_fx_count() - fx0, re15_re2z_last_hit_handler());
        {   extern int  re15_re2z_last_fx_part(void);
            extern void re15_re2z_last_fx_pos(int32_t out[3]);
            int32_t fp[3]; re15_re2z_last_fx_pos(fp);
            printf("   FX-Anker: part=%d pos=(%ld,%ld,%ld) | Aktor=(%ld,%ld,%ld) dy=%ld\n",
                   re15_re2z_last_fx_part(), (long)fp[0], (long)fp[1], (long)fp[2],
                   (long)e->x, (long)e->y, (long)e->z, (long)(fp[1] - e->y));
            for (int i = 0; i < 8; i++) {
                const re15_esp_fx_t *f = re15_esp_fx_get(i);
                if (!f || !f->active) continue;
                printf("   FX[%d] id=%u sub=%u eff_idx=%d bank=%s pos=(%ld,%ld,%ld) vis=%d\n",
                       i, f->effect_id, f->sub_index, (int)f->eff_idx,
                       f->bank ? "JA" : "NEIN(unaufgeloest)",
                       (long)f->x, (long)f->y, (long)f->z, re15_esp_fx_visible(f));
            }
        }
    }
    return 0;
}
