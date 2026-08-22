/* probe_re2_fatz_walkspot.c — MESSSONDE zum Nutzer-Befund (1), 2026-08-22.
 *
 * "RE2-Modus, fetter Zombie (Typ 0x11) im Dining Room (ROOM1140): manchmal wechselt er kurz zu
 *  einer Animation mit GEHOBENEN ARMEN und laeuft dann. Immer wenn diese Animation kommt,
 *  LAEUFT ER AUF DER STELLE statt Richtung Spieler."
 *
 * Gemessen wird pro Bild: Zustand/Substates, der GESPIELTE Clip + die BANK
 * (re15_re2z_poses_loco_bank + die Renderer-Auswahl — ohne die misst man Phantome),
 * Weltposition und der tatsaechliche VORTRIEB (dx,dz) sowie ai_dist. Frame-Treiber = die
 * game_step-Reihenfolge (re15_enemy_ai_run_all + re15_actors_anim_advance).
 * Die RE2-Bank wird wie in platform/pc/main.c (pc_enemy_load) geladen.
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
#include "re15_tim.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_re2z_poses_loco_bank(const re15_actor_t *a);
static uint32_t g_min_dist = 0xffffffffu;

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = sz; return b;
}
static const uint8_t *re15_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}
static const uint8_t *re2_ems_blob_(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}

/* 1:1-Spiegel von pc_enemy_load (platform/pc/main.c) */
static void probe_enemy_load(uint8_t type)
{
    if (type == 0 || re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && re15_re2_owns_type(type)) {
        size_t sz = 0; const uint8_t *ems = re2_ems_blob_(&sz);
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) {
            eb->buf = NULL; eb->pc_tex_slot = 11 + (int)(eb - g_enemy); eb->ok = 1;
            printf("  [load] RE2 EM0%02X: %d Meshes, %d Bones, %d Clips, loco %d\n", type,
                   eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count,
                   eb->anim_loco.clip_count);
            return;
        }
    }
    size_t esz = 0; const uint8_t *ems = re15_ems_blob(&esz);
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; uint8_t *bufp = NULL;
    if (ems && idx >= 0 && re15_ems_get_entry(ems, esz, idx, &off, &len) == 0) {
        bufp = (uint8_t *)malloc(len);
        if (bufp) memcpy(bufp, ems + off, len);
    }
    if (!bufp) { eb->type = 0; return; }
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(bufp, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        free(bufp); eb->type = 0; return; }
    eb->buf = bufp;
    eb->loco_ok   = (re15_emd_parse_loco_bank(bufp, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(bufp, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(bufp, len, &eb->skel_own, &eb->anim_own) == 0);
    eb->pc_tex_slot = 11 + (int)(eb - g_enemy); eb->ok = 1;
    printf("  [load] RE1.5 EM%02X: %d Meshes, %d Bones, %d Clips\n", type,
           eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count);
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

int main(int argc, char **argv)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf) { printf("SKIP: ROOM1140.RDT fehlt\n"); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        printf("FAIL: RDT-Parse\n"); return 1; }

    int nframes = (argc > 1) ? atoi(argv[1]) : 900;
    uint32_t seed = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0x0badf00du;
    int quiet = (argc > 3) ? atoi(argv[3]) : 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(seed);
    re15_re2z_rng_reset();
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int target = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x11) { target = s; break; }
    if (target < 0) { printf("FAIL: kein Typ 0x11 im Raum\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != target) g_actors[s].active = 0;
    probe_enemy_load(g_actors[target].type);

    re15_actor_t *e = &g_actors[target];
    printf("Ziel slot=%d typ=0x%02X grid=0x%02X pos(%d,%d,%d) rot=%d  seed=0x%08x\n",
           target, e->type, e->grid_id, (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y,
           (unsigned)seed);

    /* ---- WURZELBEWEGUNG JE LOCO-CLIP (Pair-1-Bank, die der Gang fahrt) ------------------- */
    {
        re15_enemy_bank_t *bk = re15_enemy_find(e->type);
        if (bk && bk->loco_ok) {
            printf("--- Loco-Bank (Pair 1): %d Clips ---\n", bk->anim_loco.clip_count);
            for (int c = 0; c < bk->anim_loco.clip_count; c++) {
                const re15_emd_clip_t *cl = &bk->anim_loco.clips[c];
                int n = cl->frame_count;
                long total = 0; int16_t sx0 = 0, sz0 = 0, sxN = 0, szN = 0;
                printf("  clip %d: %2d Bilder  sx:", c, n);
                for (int i = 0; i < n; i++) {
                    int kf = (int)(bk->anim_loco.frames[cl->first_frame + i] & 0xFFFu);
                    int16_t sx, sy, sz;
                    re15_emd_get_keyframe_speed(&bk->skel_loco, kf, &sx, &sy, &sz);
                    if (i == 0) { sx0 = sx; sz0 = sz; }
                    sxN = sx; szN = sz;
                    if (i < 12) printf(" %d", (int)sx);
                    if (i > 0) total += (sx > sx0) ? 0 : 0;
                }
                printf(" ...  Spanne sx %d->%d (%d), sz %d->%d (%d)\n",
                       (int)sx0, (int)sxN, (int)(sxN - sx0), (int)sz0, (int)szN, (int)(szN - sz0));
                (void)total;
            }
            printf("--- Aktions-Bank (Pair 2): %d Clips ---\n", bk->anim.clip_count);
            for (int c = 0; c < bk->anim.clip_count; c++) {
                const re15_emd_clip_t *cl = &bk->anim.clips[c];
                int n = cl->frame_count; if (n <= 0) { printf("  clip %2d: leer\n", c); continue; }
                int16_t sx0 = 0, sz0 = 0, sxN = 0, szN = 0;
                for (int i = 0; i < n; i++) {
                    int kf = (int)(bk->anim.frames[cl->first_frame + i] & 0xFFFu);
                    int16_t sx, sy, sz;
                    re15_emd_get_keyframe_speed(&bk->skel, kf, &sx, &sy, &sz);
                    if (i == 0) { sx0 = sx; sz0 = sz; }
                    sxN = sx; szN = sz;
                }
                printf("  clip %2d: %3d Bilder  sx %6d->%6d (%6d)  sz %6d->%6d (%6d)\n",
                       c, n, (int)sx0, (int)sxN, (int)(sxN - sx0),
                       (int)sz0, (int)szN, (int)(szN - sz0));
            }
        }
    }
    /* ---- HANDHOEHE je Loco-Clip: welcher Gang hat die GEHOBENEN ARME? ------------------- */
    {
        re15_enemy_bank_t *bk = re15_enemy_find(e->type);
        if (bk && bk->loco_ok) {
            printf("--- Handhoehe (Bone-Y, kleiner = hoeher) je Loco-Clip ---\n");
            for (int c = 0; c < bk->anim_loco.clip_count; c++) {
                const re15_emd_clip_t *cl = &bk->anim_loco.clips[c];
                long long sum[RE15_EMD_MAX_BONES]; int nb = bk->skel_loco.bone_count;
                for (int i = 0; i < nb; i++) sum[i] = 0;
                int n = cl->frame_count; if (n <= 0) continue;
                for (int i = 0; i < n; i++) {
                    int kf = (int)(bk->anim_loco.frames[cl->first_frame + i] & 0xFFFu);
                    re15_skel_pose_t po[RE15_EMD_MAX_BONES];
                    if (re15_skel_compute_pose(&bk->skel_loco, kf, po) != 0) continue;
                    for (int j = 0; j < nb; j++) sum[j] += po[j].trans[1];
                }
                printf("  clip %d:", c);
                for (int j = 0; j < nb; j++) printf(" b%d=%lld", j, sum[j] / n);
                printf("\n");
            }
        }
    }

    /* Erzwungener Gang-Clip fuer die A/B-Messung: RE15_PROBE_WALKCLIP=0|2 (jedes Bild neu
     * gesetzt, weil INIT/EXEC[13] ihn sonst neu wuerfeln). */
    int force_wc = -1;
    { const char *wc = getenv("RE15_PROBE_WALKCLIP");
      if (wc && *wc) { force_wc = atoi(wc); printf("  [forciert] re2z_walkclip = %d\n", force_wc); } }

    /* Spieler in Gehweite: 3000 in -X, damit dist zwischen 2500 und 3500 startet
     * (Bloecke D/E) und der Zombie zu Fuss anruecken muss. */
    pl->x = e->x - 3000; pl->z = e->z; pl->y = e->y; pl->motion = 105;

    int32_t px = e->x, pz = e->z;
    int last_s1 = -1, last_s2 = -1, last_clip = -1;
    static long long mv[16][40]; static long long cnt[16][40];
    /* Spieler UMKREIST den Zombie (RE15_PROBE_ORBIT=Radius): nur so wird der Spieler regelmaessig
     * AUSSERHALB des +-1024-Kegels und die Bloecke D/E der Gang-Leiter koennen ueberhaupt
     * feuern (arc1024 != 0 @0x80101868/@0x801018b0). */
    int orbit = 0; { const char *o = getenv("RE15_PROBE_ORBIT"); if (o && *o) orbit = atoi(o); }
    int32_t ocx = e->x, ocz = e->z;
    for (int f = 0; f < nframes; f++) {
        { static int pm = -2; if (pm == -2) { const char *v = getenv("RE15_PROBE_PLMOTION");
                                             pm = (v && *v) ? atoi(v) : -1; }
          pl->hp = 100;
          pl->motion = (int16_t)((pm >= 0) ? pm : (orbit ? 100 : 105)); }
        if (force_wc >= 0) e->re2z_walkclip = (uint8_t)force_wc;
        /* RE15_PROBE_FORCE2=<Bild>: an diesem Bild den Zustand 0x201 (EXEC[2]) committen —
         * das ist genau das Wort, das die Original-Gang-Leiter @0x80101db8/@0x80101f0c/
         * @0x80101f5c und die Treffer-Erholungen @0x801060F4/@0x80105B24/@0x80107418 schreiben. */
        { static int f2 = -2;
          if (f2 == -2) { const char *v = getenv("RE15_PROBE_FORCE2"); f2 = (v && *v) ? atoi(v) : -1; }
          if (f2 >= 0 && f == f2) { re15_ai_set_state_word(e, 0x201); e->sub_state_2 = 0;
                                    printf("  [forciert] +0x4 = 0x201 (EXEC[2]) in Bild %d\n", f); } }
        if (orbit) {
            int ang = (f * 24) & 0xfff;                 /* ~1 Umlauf je 170 Bilder */
            pl->x = ocx + (int32_t)(((int32_t)re15_cos_q12(ang) * orbit) >> 12);
            pl->z = ocz - (int32_t)(((int32_t)re15_sin_q12(ang) * orbit) >> 12);
        }
        frame();
        int32_t dx = e->x - px, dz = e->z - pz; px = e->x; pz = e->z;
        int adx = dx < 0 ? -dx : dx, adz = dz < 0 ? -dz : dz;
        int fc = re15_actor_clip_len(e);
        int lb = re15_re2z_poses_loco_bank(e);
        int chg = (e->sub_state_1 != last_s1) || (e->sub_state_2 != last_s2) ||
                  ((int)e->motion != last_clip);
        if (e->state == 1 && e->sub_state_1 < 16 && (int)e->motion < 40 && e->motion >= 0) {
            mv[e->sub_state_1][e->motion] += adx + adz;
            cnt[e->sub_state_1][e->motion]++;
        }
        if (!quiet && (chg || (f % 25) == 0))
            printf("f%04d st=%d s1=%2d s2=%d clip=%2d af=%3d fc=%2d LOCO=%d "
                   "pos(%6d,%7d) d(%4d,%4d) |d|=%4d rot=%4d dist=%5u pl(%6d,%7d) %s\n",
                   f, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                   (int)e->anim_frame, fc, lb, (int)e->x, (int)e->z, (int)dx, (int)dz,
                   adx + adz, (int)e->rot_y, e->ai_dist, (int)pl->x, (int)pl->z,
                   chg ? "<<< WECHSEL" : "");
        if (e->ai_dist < g_min_dist) g_min_dist = e->ai_dist;
        last_s1 = e->sub_state_1; last_s2 = e->sub_state_2; last_clip = (int)e->motion;
        if (e->hp < 0) break;
    }

    printf("\n--- min dist = %u (Ende %u), hp=%d st=%d s1=%d 93=%02X 1D3=%02X pl1D3=%02X ---\n",
           g_min_dist, e->ai_dist, e->hp, e->state, e->sub_state_1, e->hit_react,
           e->re2z_self1d3, pl->re2z_self1d3);
    printf("\n--- Vortrieb je (sub_state_1, clip) im ACTIVE-Zustand ---\n");
    for (int s = 0; s < 16; s++)
        for (int c = 0; c < 40; c++)
            if (cnt[s][c])
                printf("  s1=%2d clip=%2d : %5lld Bilder, Summe|d|=%7lld, Schnitt=%6.2f\n",
                       s, c, cnt[s][c], mv[s][c], (double)mv[s][c] / (double)cnt[s][c]);
    return 0;
}
