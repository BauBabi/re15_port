/* probe_1210_material.c — MESSUNG (keine Behauptung): was steht fuer ROOM1210 zur Verfuegung?
 *
 * NUTZER-WUNSCH: "im room 1210 strecken ab einem gewissen punkt alle Zombies die Haende aus dem
 * gitter ... in Resident Evil 2 gibt es einen Flur, wo man durchlaeuft, und kommt man am Fenster
 * vorbei, kommen die Haende der Zombies mit Stoehnen und greifen einen. das wuerde ich gerne auch
 * hier so umgesetzt haben wollen."
 *
 * Bevor irgendetwas gebaut wird, muss gemessen sein, WAS es gibt:
 *   1. Die Anim-Bank von Typ 0x1A (EM01A, EMS-Index 6): wie viele Clips, wie lang, welche Baenke
 *      (Haupt / loco / own / victim)? Eine OPFER-Bank entscheidet, ob ein Griff ueberhaupt ohne
 *      geliehene Animation moeglich ist.
 *   2. Die zehn Kreaturen in ROOM1210: Position, Deskriptor, Ausrichtung — daraus ergibt sich die
 *      Fenster-Linie und damit ein Naeherungs-Radius, der nicht geraten ist.
 *   3. Die Sound-Baenke des Raums: welche SE-Ids gibt es ueberhaupt fuer ein Stoehnen?
 *
 * KEIN add_test — reine Messsonde. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_math.h"   /* re15_squareroot0 */

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

static uint8_t s_blob[0x80000];

static void dump_anim(const char *tag, const re15_emd_animation_t *an,
                      const re15_emd_skeleton_t *sk)
{
    if (!an || an->clip_count <= 0) { printf("   %-8s (leer)\n", tag); return; }
    printf("   %-8s clips=%d frames=%d bones=%d kf=%d kfsize=%d\n",
           tag, an->clip_count, an->frame_count, sk ? sk->bone_count : -1,
           sk ? sk->keyframe_count : -1, sk ? sk->keyframe_size_bytes : -1);
    for (int c = 0; c < an->clip_count && c < 24; c++)
        printf("      clip %2d: %3d Frames (ab %d)\n", c,
               an->clips[c].frame_count, an->clips[c].first_frame);
}

int main(void)
{
    printf("=== ROOM1210: was ist an Material da? ===\n");

    /* ---- 1. Die Anim-Bank von Typ 0x1A ------------------------------------------------ */
    {   size_t n = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
        if (!ems) { printf("FAIL: CDEMD0.EMS nicht lesbar\n"); return 1; }
        int idx = re15_ems_index_for_type(0x1A);
        size_t off = 0, len = 0;
        printf("\n-- EM01A (Typ 0x1A), EMS-Index %d --\n", idx);
        if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) {
            printf("   FAIL: kein EMS-Eintrag\n");
        } else if (len > sizeof s_blob) {
            printf("   FAIL: Eintrag %zu B zu gross\n", len);
        } else {
            memcpy(s_blob, ems + off, len);
            re15_md1_t md; re15_emd_skeleton_t sk; re15_emd_animation_t an;
            re15_tim_t tim = (re15_tim_t){0};
            memset(&md,0,sizeof md); memset(&sk,0,sizeof sk); memset(&an,0,sizeof an);
            printf("   EMS-Eintrag: Offset 0x%zx, %zu Byte\n", off, len);
            if (re15_emd_parse_container(s_blob, len, &md, &sk, &an, &tim) != 0) {
                printf("   FAIL: EMD-Parse\n");
            } else {
                printf("   Meshes=%d  TIM=%dx%d\n", md.mesh_count, tim.width, tim.height);
                dump_anim("HAUPT", &an, &sk);
                re15_emd_skeleton_t sko; re15_emd_animation_t ano;
                memset(&sko,0,sizeof sko); memset(&ano,0,sizeof ano);
                if (re15_emd_parse_own_bank(s_blob, len, &sko, &ano) == 0 && ano.clip_count > 0)
                    dump_anim("OWN", &ano, &sko);
                else printf("   OWN      (keine)\n");
                re15_emd_skeleton_t skl; re15_emd_animation_t anl;
                memset(&skl,0,sizeof skl); memset(&anl,0,sizeof anl);
                if (re15_emd_parse_loco_bank(s_blob, len, &skl, &anl) == 0 && anl.clip_count > 0)
                    dump_anim("LOCO", &anl, &skl);
                else printf("   LOCO     (keine)\n");
                printf("   -> OPFER-Bank (fuer einen Griff noetig): im RE1.5-Container gibt es\n"
                       "      nur die drei obigen Paare; eine eigene Victim-Bank hat der\n"
                       "      Container nicht (RE2-EMDs haben dafuer dir[5]/dir[6]).\n");
            }
        }
        free(ems);
    }

    /* ---- 2. Die zehn Kreaturen in ROOM1210 --------------------------------------------- */
    {   char path[600];
        snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
        size_t sz = 0; uint8_t *buf = slurp(path, &sz);
        if (!buf) { printf("\nFAIL: ROOM1210.RDT nicht lesbar\n"); return 1; }
        static re15_rdt_t rdt;
        if (re15_rdt_parse(buf, sz, &rdt) != 0) { printf("\nFAIL: RDT-Parse\n"); return 1; }

        static re15_camera_view_t cam; static re15_game_ctx_t ctx;
        memset(&cam,0,sizeof cam); memset(&ctx,0,sizeof ctx);
        ctx.rdt = &rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;

        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        g_current_room_id = 0x1210;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        for (int f = 0; f < 8; f++) { const unsigned char *r; int l, i;
            re15_msg_tick(&r,&l,&i); ctx.pad_current = 0; ctx.pad_pressed = 0;
            scd_vm_tick(); re15_game_step(&ctx); }

        printf("\n-- ROOM1210: Kreaturen nach dem Raum-Aufbau --\n");
        int32_t minx = 0x7fffffff, maxx = -0x7fffffff, minz = 0x7fffffff, maxz = -0x7fffffff;
        int n1a = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active) continue;
            printf("   slot %2d typ=0x%02X grid=0x%02X emflag=0x%02X hp=%d pos=(%6ld,%6ld,%6ld) yaw=%4d clip=%u\n",
                   s, e->type, e->grid_id, e->em_flag_id, (int)e->hp,
                   (long)e->x, (long)e->y, (long)e->z, (int)e->rot_y, e->motion);
            if (e->type == 0x1A) {
                n1a++;
                if (e->x < minx) minx = e->x;  if (e->x > maxx) maxx = e->x;
                if (e->z < minz) minz = e->z;  if (e->z > maxz) maxz = e->z;
            }
        }
        printf("   -> %d Kreaturen vom Typ 0x1A; Spanne x %ld..%ld (%ld), z %ld..%ld (%ld)\n",
               n1a, (long)minx, (long)maxx, (long)(maxx-minx),
               (long)minz, (long)maxz, (long)(maxz-minz));

        /* Abstand zum jeweils naechsten Nachbarn — das ist der Massstab fuer einen
         * Fenster-Radius, der aus den Daten kommt statt aus dem Gefuehl. */
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *a = &g_actors[s];
            if (!a->active || a->type != 0x1A) continue;
            int32_t best = 0x7fffffff; int bs = -1;
            for (int t = 1; t < RE15_ACTOR_MAX; t++) {
                re15_actor_t *b = &g_actors[t];
                if (t == s || !b->active || b->type != 0x1A) continue;
                int32_t dx = b->x - a->x, dz = b->z - a->z;
                int32_t d = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
                if (d < best) { best = d; bs = t; }
            }
            if (bs >= 0) printf("   slot %2d -> naechster Nachbar slot %2d, Abstand %ld\n",
                                s, bs, (long)best);
        }

        printf("\n-- ROOM1210: Sound-Baenke im RDT --\n");
        printf("   snd0 vh=%p (%d B) vb=%p (%d B) edt=%p (%d B)\n",
               (void*)rdt.snd_vh[0], rdt.snd_vh_size[0],
               (void*)rdt.snd_vb[0], rdt.snd_vb_size[0],
               (void*)rdt.snd_edt[0], rdt.snd_edt_size[0]);
        printf("   snd1 vh=%p (%d B) vb=%p (%d B) edt=%p (%d B)\n",
               (void*)rdt.snd_vh[1], rdt.snd_vh_size[1],
               (void*)rdt.snd_vb[1], rdt.snd_vb_size[1],
               (void*)rdt.snd_edt[1], rdt.snd_edt_size[1]);

        free(buf);
    }

    printf("\nprobe_1210_material: MESSUNG FERTIG (reine Diagnose, kein Pass/Fail)\n");
    return 0;
}
