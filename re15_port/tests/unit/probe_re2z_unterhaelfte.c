/* DIAGNOSE (Runde 11, Auftrag 4): Was tut die ABGETRENNTE UNTERHAELFTE?
 *
 * Original: FUN_8010B7D4 (EMZ0.BIN, Ladebasis 0x80100000), Sprungtabelle @0x8010027C,
 * 9 Zustaende. Sie posiert die Haelfte ueber einen ZWEITEN Aufruf von FUN_80029E10
 * (@0x8010B904 / @0x8010B9EC / @0x8010BB30 / @0x8010BBC0 / @0x8010BC04) mit einem
 * EIGENEN Clip-Cursor (+0x21C/+0x21D/+0x21E) und einer BONE-MASKE im Arbeitswort
 * work+4 (`lw s4,4(s5)` @0x80029F04; Maske aus der Schablone @0x80100248+4 = 0x7E
 * -> nur Bones 1..6).
 *
 * Der Port friert stattdessen EINE Pose ein (re2z_low_kf). Diese Sonde misst, wie weit
 * die beiden auseinanderlaufen: Clip-Laengen, Bone-Wege, und die Zustandsfolge.
 *
 * KEIN Engine-Code geaendert - reine Messung.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void dump_anim(const char *name, const re15_emd_animation_t *a, int ok)
{
    if (!ok) { printf("  %-8s: NICHT geladen\n", name); return; }
    printf("  %-8s: %d Clips, %d Frame-Eintraege, max kf=%d\n",
           name, a->clip_count, a->frame_count, a->last_keyframe_index);
    for (int c = 0; c < a->clip_count; c++) {
        if (c != 0 && c != 1 && c != 22 && c > 24) continue;
        printf("      Clip %2d: first=%4d n=%3d  kf[0..7]=",
               c, a->clips[c].first_frame, a->clips[c].frame_count);
        for (int f = 0; f < a->clips[c].frame_count && f < 8; f++)
            printf(" %u", a->frames[a->clips[c].first_frame + f] & 0xFFFu);
        printf("\n");
    }
}

/* Weltlage eines Bones aus einer AUSDRUECKLICH gegebenen Pose (kein Actor-Zustand). */
static void bone_world(const re15_emd_skeleton_t *sk, int kf, int bone,
                       int16_t rot_y, int32_t x, int32_t y, int32_t z, int32_t out[3])
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    out[0] = x; out[1] = y; out[2] = z;
    if (re15_skel_compute_pose(sk, kf, poses) != 0) return;
    re15_skel_bone_to_world(poses[bone].trans, rot_y, x, y, z, out);
}

int main(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("SKIP: CDEMD0.EMS fehlt\n"); return 0; }
    re15_actor_init();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
    if (!eb || re2_ems_load_bank(ems, n, 0x10, eb, NULL) != 0) {
        printf("FAIL: EM010 laedt nicht\n"); return 1;
    }
    eb->buf = NULL; eb->ok = 1;

    printf("=== 1) DIE BAENKE DES RE2-ZOMBIES (EM010) ===\n");
    printf("  Bones: skel=%d loco=%d own=%d victim=%d\n",
           eb->skel.bone_count, eb->skel_loco.bone_count,
           eb->skel_own.bone_count, eb->skel_victim.bone_count);
    dump_anim("HAUPT",  &eb->anim,        1);
    dump_anim("loco",   &eb->anim_loco,   eb->loco_ok);   /* Entity+0x17C/+0x108 = Zustand 1 */
    dump_anim("own",    &eb->anim_own,    eb->own_ok);    /* Entity+0x184/+0x180 = Zustand 3/5/7/8 */
    dump_anim("victim", &eb->anim_victim, eb->victim_ok);

    printf("\n=== 2) DIE CLIPS, DIE FUN_8010B7D4 SPIELT ===\n");
    {
        const re15_emd_animation_t *A = eb->own_ok ? &eb->anim_own : &eb->anim;
        const char *q = eb->own_ok ? "own (Entity+0x184/+0x180)" : "HAUPT";
        printf("  Bank fuer Zustand 3/5/7/8 = %s\n", q);
        for (int c = 1; c <= 22; c += 21) {
            if (c >= A->clip_count) { printf("  Clip %2d: EXISTIERT NICHT (nur %d Clips)\n", c, A->clip_count); continue; }
            printf("  Clip %2d: %d Bilder, kf-Folge:", c, A->clips[c].frame_count);
            for (int f = 0; f < A->clips[c].frame_count; f++)
                printf(" %u", A->frames[A->clips[c].first_frame + f] & 0xFFFu);
            printf("\n");
        }
    }

    printf("\n=== 3) BEWEGT SICH DIE HAELFTE? Bone 1..6 ueber Clip 22 ===\n");
    {
        const re15_emd_animation_t *A = eb->own_ok ? &eb->anim_own : &eb->anim;
        const re15_emd_skeleton_t  *S = eb->own_ok ? &eb->skel_own : &eb->skel;
        int c = 22;
        if (c < A->clip_count && S->bone_count > 6) {
            int nf = A->clips[c].frame_count;
            for (int f = 0; f < nf; f++) {
                int kf = (int)(A->frames[A->clips[c].first_frame + f] & 0xFFFu);
                int32_t w1[3], w3[3], w6[3];
                bone_world(S, kf, 1, 0, 0, 0, 0, w1);
                bone_world(S, kf, 3, 0, 0, 0, 0, w3);
                bone_world(S, kf, 6, 0, 0, 0, 0, w6);
                printf("   Bild %2d kf=%3d  Bone1(%5d,%5d,%5d) Bone3(%5d,%5d,%5d) Bone6(%5d,%5d,%5d)\n",
                       f, kf, w1[0],w1[1],w1[2], w3[0],w3[1],w3[2], w6[0],w6[1],w6[2]);
            }
        } else printf("   (Clip 22 fehlt)\n");
    }

    printf("\n=== 3b) CLIP 1 (Zustand 3): bewegen sich die Beine? ===\n");
    {
        const re15_emd_animation_t *A = eb->own_ok ? &eb->anim_own : &eb->anim;
        const re15_emd_skeleton_t  *S = eb->own_ok ? &eb->skel_own : &eb->skel;
        int c = 1, nf = A->clips[c].frame_count;
        int32_t mn[3][3], mx[3][3];
        const int bl[3] = {1,3,6};
        for (int b = 0; b < 3; b++) for (int k2 = 0; k2 < 3; k2++) { mn[b][k2]=1<<30; mx[b][k2]=-(1<<30); }
        for (int f = 0; f < nf; f++) {
            int kf = (int)(A->frames[A->clips[c].first_frame + f] & 0xFFFu);
            for (int b = 0; b < 3; b++) {
                int32_t w[3]; bone_world(S, kf, bl[b], 0, 0, 0, 0, w);
                for (int k2 = 0; k2 < 3; k2++) { if (w[k2]<mn[b][k2]) mn[b][k2]=w[k2]; if (w[k2]>mx[b][k2]) mx[b][k2]=w[k2]; }
                if (f % 10 == 0 && b == 1)
                    printf("   Bild %2d kf=%3d Bone3(%6d,%6d,%6d)\n", f, kf, w[0],w[1],w[2]);
            }
        }
        for (int b = 0; b < 3; b++)
            printf("   Bone%d Spannweite ueber Clip 1: dx=%d dy=%d dz=%d\n", bl[b],
                   mx[b][0]-mn[b][0], mx[b][1]-mn[b][1], mx[b][2]-mn[b][2]);
    }

    printf("\n=== 3c) ORIGINAL-LAENGE gegen PORT-LAENGE ===\n");
    {
        const re15_emd_animation_t *A = eb->own_ok ? &eb->anim_own : &eb->anim;
        printf("   Original Zustand 3 = Clip 1 = %d Bilder x 30 = %d Einheiten Rutsch\n",
               A->clips[1].frame_count, A->clips[1].frame_count * 30);
        printf("   Original Zustand 5 = Clip 22 = %d Bilder\n", A->clips[22].frame_count);
        printf("   Einfrieren nach 1+T+1+%d+1 Bildern, T=10..41 -> %d..%d Bilder\n",
               A->clips[1].frame_count, A->clips[1].frame_count + 13, A->clips[1].frame_count + 44);
    }

    printf("\n=== 4) DIE ZUSTANDSFOLGE, WIE DER PORT SIE HEUTE FAEHRT ===\n");
    {
        re15_actor_t *e = &g_actors[1];
        memset(e, 0, sizeof(*e));
        e->active = 1; e->type = 0x10;
        e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
        e->re2z_rag_anchor_on = 1u;
        e->re2z_rag_anchor_x = 0; e->re2z_rag_anchor_z = 0;
        e->re2z_flags21a |= 1u;          /* +0x21A |= 1 @0x80106AD0 */
        e->re2z_low_state = 0u;
        e->re2z_low_kf    = 0;
        int last = -1;
        for (int f = 0; f < 260; f++) {
            re15_re2z_lower_body_tick(e);
            if (f < 12 || e->re2z_low_state != last || f % 40 == 0)
                printf("   f=%3d st=%u clip=%u c1=%u c2=%u timer=%3u x=%6d z=%6d frozen=%u kf=%d\n",
                       f, e->re2z_low_state, e->re2z_low_clip, e->re2z_low_c1,
                       e->re2z_low_c2, e->re2z_low_timer,
                       e->re2z_low_x, e->re2z_low_z, e->re2z_low_frozen, (int)e->re2z_low_kf);
            last = (int)e->re2z_low_state;
        }
        printf("   ENDZUSTAND: st=%u clip=%u kf(eingefroren)=%d  -> die Pose steht seit Bild 0.\n",
               e->re2z_low_state, e->re2z_low_clip, (int)e->re2z_low_kf);
    }
    return 0;
}
