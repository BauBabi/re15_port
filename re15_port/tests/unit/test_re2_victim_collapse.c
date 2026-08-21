/* test_re2_victim_collapse.c — PIN `unit_re2_victim_collapse`.
 *
 * NUTZER-REPORT 2026-08-21 (RE2-KI-Modus): "Wenn Leon gefressen wird, steht er noch komisch."
 *
 * IST (gemessen, probe_re2_victim_pose): der Port fuhr fuer die Zombie-Familie auch unter dem
 * RE2-Flavor die RE1.5-Victim-Bank-Belegung `collapse = Variante + 6`. Gegen die RE2-Victim-Bank
 * (EM010 EMD-Paar 3, 17 Clips) sind 6 und 7 aber STEHENDE Struggle-Clips (10 bzw. 30 Frames) —
 * Leon blieb also beim Gefressenwerden aufrecht stehen.
 *
 * SOLL (byte-gelesen aus EMOVL10_S0.BIN, RAW @0x80100000):
 *   Das RE2-Zombie-Overlay installiert die beiden Spieler-Handler SELBST:
 *     @0x801010EC-104   0x800CE300[kind] = 0x8010A924   (cmd 5, Griff/Struggle)
 *     @0x80101108-120   0x800CE400[kind] = 0x8010B3C0   (cmd 6, Gefressen/Kollaps)
 *   STRUGGLE 0x8010A9B8 (Tab @0x8010CF2C, Phasen @0x801001DC): Basis-Paar {0,3}
 *     (`sb zero,24(sp)` @0x8010A9E4 / `sb 3,25(sp)` @0x8010A9E8), P0 Clip = Basis @0x8010AA5C,
 *     P2 Clip = Basis+1 @0x8010AB88, P4 Clip = Basis+2 @0x8010ACA0-A8
 *     => intro/hold/release identisch zur RE1.5-Belegung (v*3 / +1 / +2).
 *   KOLLAPS 0x8010B464 (Tab @0x8010CF3C = {0x8010B464,0x8010B464}, Phasen @0x8010022C):
 *     Basis-Paar {13,15} (`addiu v0,zero,13` @0x8010B484 + `sb v0,24(sp)` @0x8010B488;
 *     `addiu v0,zero,15` @0x8010B48C + `sb v0,25(sp)` @0x8010B490),
 *     P0 @0x8010B4C4 Clip = Paar[PL+0x5] (`lbu v1,24(v0)` @0x8010B4D0, Rate 0x0F
 *     `lui a0,0xf` @0x8010B4C8, Clip-Wort `sw v1,332(s2)` @0x8010B4F4)
 *     => COLLAPSE = 13 (vorn) / 15 (hinten).
 *   TOD: `sh -32768,342(s2)` @0x8010B738 in PHASE 2 = dem Tick NACH dem Clip-Ende,
 *     NICHT bei Clip-Frame 0x23 (das ist der RE1.5-Zwilling FUN_8010a6f8 @0x8010a7e8/@0x8010a80c).
 *
 * Geprueft wird die GERENDERTE Pose (Knochen-Weltposition ueber genau den Renderpfad
 * platform/pc/main.c:5460-5478), nicht nur der Clip-Index — Zustandswerte allein haetten den
 * Fehler nicht gezeigt (der Clip-Index 6 sah "plausibel" aus).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fail = 0;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  OK   " __VA_ARGS__); printf("\n"); } \
    else      { printf("  FAIL " __VA_ARGS__); printf("\n"); s_fail++; } \
} while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- PL00-Rig (Leons eigene Knochen + Bind-Pose; der Renderer nimmt genau dieses) --------- */
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int s_pl00_ok = 0;
static void load_pl00(void)
{
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    s_pl00_ok = (edd && emr &&
                 re15_emd_parse_animation(edd, esz, &s_pl00_anim) == 0 &&
                 re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) == 0);
}

/* Kopf-Weltposition (PL00-Bone 8) fuer (Bank, Clip, Frame) — der Renderpfad aus main.c. */
static int head_y(const re15_enemy_bank_t *vb, int clip, int frame)
{
    if (!s_pl00_ok || !vb->victim_ok || clip < 0 || clip >= vb->anim_victim.clip_count) return 1 << 30;
    int fc = vb->anim_victim.clips[clip].frame_count;
    if (fc <= 0) return 1 << 30;
    re15_emd_skeleton_t vs = s_pl00_skel;
    vs.keyframe_data       = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    re15_actor_t p; memset(&p, 0, sizeof p);
    p.active = 1; p.motion = (uint8_t)clip; p.anim_frame = (uint16_t)(frame % fc);
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, &p, clip, p.anim_frame);
    if (kf < 0) return 1 << 30;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(&vs, kf, poses) != 0) return 1 << 30;
    int32_t w[3]; re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, w);
    return w[1];
}

/* ---- Baenke -------------------------------------------------------------------------------- */
static int load_re15_into(uint8_t type)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        uint8_t *blob = (uint8_t *)malloc(len);          /* resident: die Bank aliast hinein */
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        re15_tim_t tim = (re15_tim_t){0};
        if (eb && re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
            eb->ok = 1; eb->buf = NULL;
            eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim,
                                                        &eb->anim_victim) == 0);
            ok = eb->victim_ok;
        }
    }
    free(ems);
    return ok;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_re2_into(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    return eb->victim_ok;
}

/* Einen kompletten Kollaps fahren; out_kill_tick = der CLIP-FRAME, in dem hp erstmals < 0 wird. */
static void run_collapse(uint8_t type, int behind, int max_ticks,
                         int *out_clip, int *out_fc, int *out_kill_tick, int *out_end_head_y)
{
    re15_actor_init();
    re15_player_victim_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *z  = &g_actors[1];
    pl->active = 1; pl->hp = 40; pl->x = 0; pl->y = 0; pl->z = 0;
    z->active = 1; z->type = type; z->hp = 100; z->state = 1;
    z->sub_state_1 = (uint8_t)(behind ? 4 : 3);        /* RE1.5-Ableiter: >=4 = hinten */
    z->rot_y = (int16_t)(behind ? 0 : 0x800);
    pl->rot_y = 0;

    re15_re2z_victim_devour(z, behind);                /* der RE2-Kill-Tick (@0x80102928-50) */
    *out_kill_tick = -1;
    for (int t = 1; t <= max_ticks; t++) {
        re15_player_victim_tick();
        if (*out_kill_tick < 0 && pl->hp < 0) *out_kill_tick = (int)pl->anim_frame;
    }
    *out_clip = (int)pl->motion;
    re15_enemy_bank_t *vb = re15_enemy_find(type);
    *out_fc = (vb && vb->victim_ok && *out_clip < vb->anim_victim.clip_count)
            ? vb->anim_victim.clips[*out_clip].frame_count : -1;
    *out_end_head_y = (vb) ? head_y(vb, *out_clip, (*out_fc > 0) ? *out_fc - 1 : 0) : (1 << 30);
}

int main(void)
{
    load_pl00();
    printf("PL00-Rig: %s (%d Knochen)\n", s_pl00_ok ? "geladen" : "FEHLT", s_pl00_skel.bone_count);
    if (!s_pl00_ok) { printf("FAIL: ohne PL00 ist die Posen-Messung wertlos\n"); return 1; }

    /* Referenz: Leon STEHEND aus seinem eigenen Rig — der Massstab fuer "steht". */
    int stand_y = 1 << 30;
    {
        re15_actor_t p; memset(&p, 0, sizeof p); p.active = 1;
        int kf = re15_compute_actor_kf(&s_pl00_anim, &s_pl00_skel, &p, 0, 0);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (kf >= 0 && re15_skel_compute_pose(&s_pl00_skel, kf, poses) == 0) {
            int32_t w[3]; re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, w);
            stand_y = w[1];
        }
    }
    printf("Referenz Leon STEHEND: Kopf y = %d (PSX-Y nach oben negativ)\n\n", stand_y);
    CHECK(stand_y < -2000, "Referenz stehend: Kopf y < -2000 (ist %d)", stand_y);

    /* ================= P1: RE2-Flavor, RE2-Bank — der eigentliche Nutzer-Report ============ */
    printf("== P1: RE2-Flavor, RE2-Bank EM010 — Fress-Kollaps ==\n");
    re15_actor_init();
    re15_enemy_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    int b2 = load_re2_into(0x10);
    CHECK(b2, "RE2-Victim-Bank EM010 geladen (ohne Bank ist die Messung wertlos)");
    if (b2) {
        re15_enemy_bank_t *vb = re15_enemy_find(0x10);
        CHECK(vb->anim_victim.clip_count == 17, "RE2-Victim-Bank hat 17 Clips (ist %d)",
              vb->anim_victim.clip_count);

        int clip, fc, kill, hy;
        run_collapse(0x10, 0, 200, &clip, &fc, &kill, &hy);
        CHECK(clip == 13, "vorn: COLLAPSE-Clip == 13 (@0x8010B484-88 / @0x8010B4D0), ist %d", clip);
        CHECK(fc == 116, "vorn: Clip 13 hat 116 Frames (ist %d)", fc);
        CHECK(hy > -600, "vorn: Kopf am Clip-Ende AM BODEN (y=%d > -600) — Leon LIEGT", hy);

        run_collapse(0x10, 1, 200, &clip, &fc, &kill, &hy);
        CHECK(clip == 15, "hinten: COLLAPSE-Clip == 15 (@0x8010B48C-90 / @0x8010B4D0), ist %d", clip);
        CHECK(fc == 116, "hinten: Clip 15 hat 116 Frames (ist %d)", fc);
        CHECK(hy > -600, "hinten: Kopf am Clip-Ende AM BODEN (y=%d > -600)", hy);

        /* NEGATIV-KONTROLLE: genau die Clips, die der Port VORHER spielte, sind stehend.
         * Ohne diese Zeile waere "Kopf am Boden" kein Nachweis (der Test koennte trivial
         * wahr sein). */
        int y6 = head_y(vb, 6, vb->anim_victim.clips[6].frame_count - 1);
        int y7 = head_y(vb, 7, vb->anim_victim.clips[7].frame_count - 1);
        CHECK(y6 < -1800 && y7 < -1800,
              "NEGATIV: die alten Clips 6/7 sind STEHEND (Kopf y=%d/%d < -1800) — das war der Bug",
              y6, y7);

        /* TOD-ZEITPUNKT: @0x8010B738 liegt in PHASE 2 = dem Tick NACH dem Clip-Ende. */
        run_collapse(0x10, 0, 200, &clip, &fc, &kill, &hy);
        CHECK(kill == fc - 1, "Tod erst am LETZTEN Clip-Frame (@0x8010B738, P2 = Tick nach "
              "Clip-Ende): Frame %d == fc-1 %d", kill, fc - 1);
    }

    /* ================= P2: RE1.5-Regressionswache ========================================= */
    printf("\n== P2: RE1.5-Flavor, RE1.5-Bank EM010 — unveraendert ==\n");
    re15_actor_init();
    re15_enemy_reset();                 /* Bank-Pool leeren: P1 hat den RE2-Blob in Slot 0x10 */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    int b15 = load_re15_into(0x10);
    CHECK(b15, "RE1.5-Victim-Bank EM010 geladen");
    if (b15) {
        re15_enemy_bank_t *vb = re15_enemy_find(0x10);
        CHECK(vb->anim_victim.clip_count == 14, "RE1.5-Victim-Bank hat 14 Clips (ist %d)",
              vb->anim_victim.clip_count);
        int clip, fc, kill, hy;
        run_collapse(0x10, 0, 200, &clip, &fc, &kill, &hy);
        CHECK(clip == 6, "RE1.5 vorn: COLLAPSE-Clip bleibt 6 (@0x8010a28c-Belegung), ist %d", clip);
        CHECK(hy > -600, "RE1.5 vorn: Kopf am Clip-Ende am Boden (y=%d)", hy);
        CHECK(kill == 0x23, "RE1.5: Tod unveraendert bei Clip-Frame 0x23 (FUN_8010a6f8 "
              "@0x8010a7e8/@0x8010a80c), ist %d", kill);
        run_collapse(0x10, 1, 200, &clip, &fc, &kill, &hy);
        CHECK(clip == 7, "RE1.5 hinten: COLLAPSE-Clip bleibt 7, ist %d", clip);
    }

    /* ================= P3: intro/hold/release bleiben unter RE2 unveraendert ============== */
    printf("\n== P3: Struggle-Belegung unter RE2 (v*3 / +1 / +2) ==\n");
    re15_actor_init();
    re15_enemy_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (load_re2_into(0x10)) {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *z  = &g_actors[1];
        pl->active = 1; pl->hp = 100;
        z->active = 1; z->type = 0x10; z->hp = 100; z->state = 1; z->sub_state_1 = 3;
        re15_player_victim_reset();
        re15_re2z_victim_begin(z, pl, 0);
        re15_player_victim_tick();
        CHECK(pl->motion == 0, "vorn: INTRO-Clip 0 (@0x8010A9E4 + @0x8010AA5C), ist %d", pl->motion);
        re15_player_victim_reset();
        re15_re2z_victim_begin(z, pl, 1);
        re15_player_victim_tick();
        CHECK(pl->motion == 3, "hinten: INTRO-Clip 3 (@0x8010A9E0/E8 + @0x8010AA5C), ist %d",
              pl->motion);
    }

    /* ================= P4: der HUND behaelt seinen eigenen RE2-Zweig ====================== */
    printf("\n== P4: Regressionswache Hund 0x20 (WELLE C, eigener RE2-Zweig) ==\n");
    re15_actor_init();
    re15_enemy_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (load_re2_into(0x20)) {
        int clip, fc, kill, hy;
        run_collapse(0x20, 0, 60, &clip, &fc, &kill, &hy);
        CHECK(clip == 0, "Hund: Kollaps bleibt Clip 0 (RE2-Hunde-Victim-Bank, 1 Clip), ist %d", clip);
    } else {
        printf("  (RE2-Hunde-Bank nicht ladbar — Wache uebersprungen)\n");
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n%s (%d Fehler)\n", s_fail ? "FEHLGESCHLAGEN" : "BESTANDEN", s_fail);
    return s_fail ? 1 : 0;
}
