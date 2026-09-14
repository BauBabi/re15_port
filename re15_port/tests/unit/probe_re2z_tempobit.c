/* probe_re2z_tempobit.c - MESSUNG: was macht das RE2-Tempo-Bit (+0x21A & 0x8000) mit der
 * Vorwaertsstrecke, und rechnet der Port dabei wie das Original?
 *
 * ORIGINAL (info/re2leon, selbst disassembliert):
 *   EXEC[1]-Tick @0x80101CB0-D60 (EMZ0.BIN, RAW @0x80100000):
 *     80101cbc  jal 0x80015e7c   (a1=+0x108, a2=+0x17C, a3=0)   <- DELTA -> +0x144
 *     80101cd0  jal 0x8002959c   (a3=0x100)                     <- Pose + NORMAL-Advance
 *     80101d00-24  +0x14D % 3  (0xAAAAAAAB-Magie), ==2 ?
 *     80101d54  jal 0x8002a9c8                                  <- EXTRA-Advance
 *     80101d60  jal 0x800152c8   (a1=0)                         <- +0x144 rotiert aufaddieren
 *   FUN_8002A9C8 @0x8002A9C8-AA24 (PSX.EXE, t_addr 0x80010000, Datei-Offset 0x800+a-t_addr):
 *     8002a9cc lbu v0,332(a3)   ; +0x14C = Clip
 *     8002a9d0 lbu a0,333(a3)   ; +0x14D = Keyframe (PRE)
 *     8002a9e4 lw  a2,0(v1)     ; Deskriptor = *(a1 + clip*4)
 *     8002a9ec srl v1,a2,16     ; HIGH16 = Byte-Offset der Keyframe-Liste
 *     8002a9f8 andi a2,a2,0xffff; LOW16  = Keyframe-ANZAHL  (= der Wrap-Wert)
 *     8002a9fc sb  v0,333(a3)   ; +0x14D = kf+1
 *     8002aa0c sw  v1,376(a3)   ; +0x178 = &kf[PRE]  (Delay-Slot, IMMER)
 *     8002aa04 sltu v0,(kf+1)&0xff,a2 ; 8002aa08 beq -> Wrap
 *     8002aa18 sb  zero,333(a3) ; Wrap: +0x14D = 0, Rueckgabe 1; sonst Rueckgabe 0
 *   FUN_80015E7C @0x80015E7C-FE4: dx/dy/dz = root(kf_jetzt) - GESPEICHERTER Vorstand
 *   (+0x20C/+0x20E/+0x210, subu @0x80015FCC-D4, sh nach +0x144/146/148 @0x80015FD8-E4);
 *   der Vorstand wird @0x80015FC4/C8 auf root(kf_jetzt) gesetzt. Er merkt sich also eine
 *   POSITION, keine Bildnummer -> ein uebersprungener Keyframe ergibt beim naechsten Aufruf
 *   die Differenz ueber ZWEI Keyframes (DOPPELSCHRITT), nichts geht verloren.
 *
 * PORT: re15_clip_root_motion_delta (enemy_ai_common.c:691) verlangt
 *   if (fr_prev < 0 || fr_now != fr_prev + 1) return;    (Zeile 699)
 *   if (s_now != s_prev + 1) return;                     (Zeile 705)
 * -> beim Sprung um ZWEI Bilder wird gar nichts addiert. Diese Sonde misst genau das.
 *
 * Sie fuehrt den ECHTEN Port-Bewegungscode (re15_re2z_move_root) ueber die ECHTE RE2-Bank
 * (CDEMD0.EMS, EM010 Paar-1/Loco) und stellt ihm die Original-Arithmetik gegenueber, die aus
 * DENSELBEN Keyframe-Feldern (re15_emd_get_keyframe_speed = Bytes 6..11) gebildet wird.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2 = NULL; static size_t s_re2n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2) s_re2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2n);
    if (!s_re2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2, s_re2n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* Der Aktor im WALK-Zustand: state 1 / +0x5 = 1 -> re15_re2z_poses_loco_bank == 1 (Paar 1). */
static void setup(re15_actor_t *e, uint8_t clip)
{
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->hp = 100;
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1;
    e->re2z_f10e = 0; e->motion = clip; e->anim_frame = 0;
    e->rot_y = 0;                       /* yaw 0 -> +X ist die Vorwaertsachse */
    e->x = 0; e->z = 0; e->anchor_x = 0; e->anchor_z = 0;
    e->root_prev_kf = -1; e->root_prev_motion = clip;
    e->anim_flags = 0x0004;             /* LOOP */
}

/* ============================================================================================
 * LIVE-GEGENPROBE: derselbe Effekt im ECHTEN Spielschritt (ROOM1030, RE2-Geschmack, echte
 * Sce_em_set-Spawns, echte SCD-VM, echtes re15_game_step). Gemessen wird pro Zombie und Tick:
 *   - Bildsprung  = anim_frame(jetzt) - anim_frame(vorher)   (Wrap herausgerechnet)
 *   - Versatz     = |dx| + |dz|
 * Erwartung, wenn der Befund stimmt: JEDER Tick mit Bildsprung 2 hat Versatz 0.
 * (Die Klemme kann einen Versatz zusaetzlich fressen, deshalb wird der Umkehrschluss
 *  NICHT gepinnt - nur der eine Richtungssatz "Sprung 2 -> kein Wurzelschritt".)
 * ============================================================================================ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static void live_1030(void)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("\nLIVE: ROOM1030.RDT nicht lesbar - uebersprungen\n"); return; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) {
        printf("\nLIVE: RDT-Parse fehlgeschlagen\n"); free(buf); return;
    }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
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
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);

    static uint16_t prev_af[RE15_ACTOR_MAX];
    static int32_t  prev_x[RE15_ACTOR_MAX], prev_z[RE15_ACTOR_MAX];
    static uint16_t prev_mo[RE15_ACTOR_MAX];
    long jump2_total = 0, jump2_zero = 0, jump1_total = 0, jump1_zero = 0;
    long jump2_bit = 0, jump2_nobit = 0;
    int  bit_seen = 0, nobit_seen = 0;
    long long path_bit = 0, path_nobit = 0; long ticks_bit = 0, ticks_nobit = 0;

    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        prev_af[s] = g_actors[s].anim_frame; prev_mo[s] = g_actors[s].motion;
        prev_x[s] = g_actors[s].x; prev_z[s] = g_actors[s].z;
    }
    for (int f = 0; f < 3000; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || !re15_re2z_owns_type(e->type)) continue;
            int32_t d = (e->x - prev_x[s]); if (d < 0) d = -d;
            int32_t dz = (e->z - prev_z[s]); if (dz < 0) dz = -dz;
            int32_t mv = d + dz;
            int same_clip = (e->motion == prev_mo[s]);
            int jump = (int)e->anim_frame - (int)prev_af[s];
            int walking = (e->state == 1 && e->sub_state_1 == 1);
            int has_bit = (e->re2z_flags21a & 0x8000u) != 0;
            if (has_bit) bit_seen = 1; else nobit_seen = 1;
            if (walking && same_clip) {
                if (has_bit) { path_bit += mv; ticks_bit++; }
                else         { path_nobit += mv; ticks_nobit++; }
                if (jump == 2) {
                    jump2_total++; if (mv == 0) jump2_zero++;
                    if (has_bit) jump2_bit++; else jump2_nobit++;
                } else if (jump == 1) {
                    jump1_total++; if (mv == 0) jump1_zero++;
                }
            }
            prev_af[s] = e->anim_frame; prev_mo[s] = e->motion;
            prev_x[s] = e->x; prev_z[s] = e->z;
        }
    }
    printf("\n================ LIVE ROOM1030 (RE2, 3000 Bilder, echtes game_step) ================\n");
    printf("  Bildsprung 1 im Gang: %6ld Ticks, davon OHNE Versatz %6ld (%.1f%%)\n",
           jump1_total, jump1_zero, jump1_total ? 100.0 * jump1_zero / jump1_total : 0.0);
    printf("  Bildsprung 2 im Gang: %6ld Ticks, davon OHNE Versatz %6ld (%.1f%%)   "
           "[mit Bit %ld / ohne Bit %ld]\n",
           jump2_total, jump2_zero, jump2_total ? 100.0 * jump2_zero / jump2_total : 0.0,
           jump2_bit, jump2_nobit);
    printf("  Versatz je Gang-Tick: mit Bit %.2f (%ld Ticks) | ohne Bit %.2f (%ld Ticks)\n",
           ticks_bit ? (double)path_bit / (double)ticks_bit : 0.0, ticks_bit,
           ticks_nobit ? (double)path_nobit / (double)ticks_nobit : 0.0, ticks_nobit);
    printf("  (Bit-Traeger gesehen: %d, Nicht-Traeger gesehen: %d)\n", bit_seen, nobit_seen);

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset();
    if (!load_bank_re2(0x10)) { printf("SKIP: RE2-Bank EM010 nicht ladbar\n"); return 0; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok || !b->loco_ok) { printf("SKIP: keine Loco-Bank\n"); return 0; }
    const re15_emd_animation_t *A = &b->anim_loco;
    const re15_emd_skeleton_t  *S = &b->skel_loco;

    printf("RE2 EM010 Paar-1 (Loco): clips=%d\n", A->clip_count);
    /* Die beiden Original-Gangclips aus dem Param-Block @0x80100860-8C sind 0 und 2. */
    for (int clip = 0; clip <= 2; clip += 2) {
        if (clip >= A->clip_count) continue;
        const re15_emd_clip_t *c = &A->clips[clip];
        printf("\n================ CLIP %d : frame_count=%d first_frame=%d ================\n",
               clip, c->frame_count, c->first_frame);
        printf("  Keyframe-Wurzel sx je Bild: ");
        for (int f = 0; f < c->frame_count && f < 24; f++) {
            int kf = (int)(A->frames[c->first_frame + f] & 0xFFFu);
            int16_t sx = 0, sy = 0, sz = 0;
            re15_emd_get_keyframe_speed(S, kf, &sx, &sy, &sz);
            printf("%d ", (int)sx);
        }
        printf("\n");

        for (int bit = 0; bit <= 1; bit++) {
            /* ---- (A) PORT: echter Code (re15_re2z_move_root) ------------------------------- */
            re15_actor_t e; setup(&e, (uint8_t)clip);
            /* ---- (B) ORIGINAL: FUN_80015E7C-Arithmetik ueber DENSELBEN Keyframe-Feldern ---- */
            int32_t org_x = 0; int have_prev = 0; int16_t prev_sx = 0;
            uint32_t org_frame = 0;

            long long port_path = 0; int32_t port_last = 0;
            int32_t port_max_tick = 0, org_max_tick = 0;
            int port_zero_ticks = 0, skips = 0;

            for (int t = 0; t < 100; t++) {
                /* --- NORMAL-Advance (Port: re15_actors_anim_advance, player_common.c:1252-58;
                 *     Original: Schwanz von FUN_80029614 @0x80029B28-4C) --- */
                e.anim_frame = (uint16_t)(((uint32_t)e.anim_frame + 1u) % (uint32_t)c->frame_count);
                org_frame    = (org_frame + 1u) % (uint32_t)c->frame_count;

                /* --- Tempo-Weiche: (+0x14D % 3)==2 POST-Advance (@0x80101D00-2C) --- */
                int extra = bit && ((e.anim_frame % 3u) == 2u);
                if (extra) {
                    /* re2z_fat_cadence_tick, enemy_ai_re2_zombie.c:1374 (== FUN_8002A9C8) */
                    e.anim_frame = (uint16_t)(((uint32_t)e.anim_frame + 1u) % (uint32_t)c->frame_count);
                    org_frame    = (org_frame + 1u) % (uint32_t)c->frame_count;
                    skips++;
                }

                /* --- PORT: echte Bewegung --- */
                int32_t before = e.x;
                re15_re2z_move_root(&e);
                int32_t d = e.x - before;
                if (d == 0) port_zero_ticks++;
                if (d > port_max_tick) port_max_tick = d;
                port_path += (d < 0 ? -d : d);
                port_last = e.x;

                /* --- ORIGINAL: dx = root(kf_jetzt) - gespeicherter Vorstand (@0x80015FCC) ---
                 *     Der Wrap auf Bild 0 setzt den Vorstand auf 0 (@0x80015F14, a3==0). */
                {
                    int kf = (int)(A->frames[c->first_frame + (int)org_frame] & 0xFFFu);
                    int16_t sx = 0, sy = 0, sz = 0;
                    re15_emd_get_keyframe_speed(S, kf, &sx, &sy, &sz);
                    int16_t base = (org_frame == 0u) ? (int16_t)0 : (have_prev ? prev_sx : sx);
                    int32_t dx = (int32_t)sx - (int32_t)base;
                    org_x += dx;
                    if (dx > org_max_tick) org_max_tick = dx;
                    prev_sx = sx; have_prev = 1;
                }
            }
            printf("  Bit %s : PORT  x=%6ld (Weg %6lld, Null-Ticks %2d, groesster Tick %4ld)\n",
                   bit ? "AN " : "AUS", (long)port_last, port_path,
                   port_zero_ticks, (long)port_max_tick);
            printf("           ORIG  x=%6ld (Extra-Advances %d, groesster Tick %4ld)\n",
                   (long)org_x, skips, (long)org_max_tick);
        }
    }

    /* ---- Der eigentliche Nachweis: ein EINZELNER Doppelsprung -------------------------------
     * Gleicher Aktor, gleiche Bank: einmal Bild n->n+1, einmal n->n+2. Im Original ist der
     * zweite Fall die SUMME der beiden Einzelschritte; im Port ist er 0. */
    {
        int clip = 0;
        const re15_emd_clip_t *c = &A->clips[clip];
        printf("\n================ EINZELSPRUNG-NACHWEIS (Clip 0) ================\n");
        printf("  %-6s %-12s %-12s %-12s %-12s\n", "n", "PORT n->n+1", "PORT n->n+2",
               "Summe 1+1", "ORIG n->n+2");
        for (int n = 1; n + 2 < c->frame_count && n < 10; n++) {
            re15_actor_t e; setup(&e, (uint8_t)clip);
            e.root_prev_kf = (int16_t)n; e.root_prev_motion = (uint16_t)clip;
            e.anim_frame = (uint16_t)(n + 1);
            int32_t x0 = e.x; re15_re2z_move_root(&e); int32_t d1 = e.x - x0;

            re15_actor_t e2; setup(&e2, (uint8_t)clip);
            e2.root_prev_kf = (int16_t)(n + 1); e2.root_prev_motion = (uint16_t)clip;
            e2.anim_frame = (uint16_t)(n + 2);
            int32_t y0 = e2.x; re15_re2z_move_root(&e2); int32_t d1b = e2.x - y0;

            re15_actor_t e3; setup(&e3, (uint8_t)clip);
            e3.root_prev_kf = (int16_t)n; e3.root_prev_motion = (uint16_t)clip;
            e3.anim_frame = (uint16_t)(n + 2);
            int32_t z0 = e3.x; re15_re2z_move_root(&e3); int32_t d2 = e3.x - z0;

            int kf_a = (int)(A->frames[c->first_frame + n]     & 0xFFFu);
            int kf_c = (int)(A->frames[c->first_frame + n + 2] & 0xFFFu);
            int16_t sa = 0, sc = 0, dummy = 0;
            re15_emd_get_keyframe_speed(S, kf_a, &sa, &dummy, &dummy);
            re15_emd_get_keyframe_speed(S, kf_c, &sc, &dummy, &dummy);
            printf("  %-6d %-12ld %-12ld %-12ld %-12ld\n",
                   n, (long)d1, (long)d2, (long)(d1 + d1b), (long)((int32_t)sc - (int32_t)sa));
        }
    }
    live_1030();

    printf("\nPROBE-OK\n");
    return 0;
}
