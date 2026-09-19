/* ============================================================================
 * probe_p3_clip11_check - UNABHAENGIGE GEGENPRUEFUNG des Befundes "anker-y /
 * zweiter Taeter" (2026-09-14). Kein add_test; standalone gegen libre15_engine.
 *
 * Gemessen wird die ORIGINAL-Tickfolge des RE2-EM23-Fress-Finishers, so wie sie
 * aus der Disassembly folgt:
 *   Phase 2 @0x80101168: FUN_80015CB8 (Platzierung mit dem AKTUELLEN +0x14C/+0x14D)
 *                        DANN FUN_8001A330(a1=5) = Pose + Frame-Advance.
 *   Frame-Advance @0x80029B28-B4C (in FUN_80029614, gerufen aus FUN_8002959C):
 *        lbu v0,333(s2) / addiu v0,v0,1 / sb v0,333(s2)
 *        sltu v0,v0,s3  (s3 = frame_count des AKTUELLEN Clips, lhu @0x80029680)
 *        bne  -> return 0 ; sonst  sb zero,333(s2) / v0=1  (WRAP auf 0!)
 *   Phase 3 @0x801011C4: FUN_80015CB8 (a0=s0 aus dem Delay-Slot @0x80100FB0)
 *                        DANN FUN_8001A240(a1=11) @0x801011E0 = Clipwechsel
 *                        (@0x8001A258-5C sb a1,332 / sb zero,333) + Pose/Advance.
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_actor.h"
#include "re15_skeleton.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); long s; uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); s = ftell(f); fseek(f, 0, SEEK_SET);
    b = malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); fclose(f); return NULL; }
    fclose(f); if (n) *n = (size_t)s; return b;
}

static re15_enemy_bank_t B;

/* Wurzel-Versatz (sx,sz) eines Clip-Frames, exakt wie re15_clip_root_motion_abs
 * (0x8000-Skip + keyframe speed-Feld). */
static void off_of(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                   int clip, int fr, int *ox, int *oy, int *oz)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int slot = fr % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    int16_t sx = 0, sy = 0, sz = 0;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    re15_emd_get_keyframe_speed(sk, (int)(an->frames[fi] & 0xFFFu), &sx, &sy, &sz);
    *ox = sx; *oy = sy; *oz = sz;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    int i, f, ox, oy, oz;
    if (!emd) { printf("EM23.EMD fehlt\n"); return 1; }
    memset(&B, 0, sizeof B);
    if (re2_emd_parse_bank(emd, sz, &B) != 0) { printf("parse fail\n"); return 1; }

    printf("EM23 clips=%d\n", B.anim.clip_count);
    for (i = 0; i < B.anim.clip_count; i++)
        printf("  clip %2d: frames=%d first=%d\n", i,
               B.anim.clips[i].frame_count, B.anim.clips[i].first_frame);

    printf("\nClip 5 (Schuetteln) Wurzelversatz, Randframes:\n");
    for (i = 0; i < 4; i++) { off_of(&B.skel,&B.anim,5,i,&ox,&oy,&oz);
        printf("   f%-3d (sx,sy,sz)=(%d,%d,%d)\n", i, ox, oy, oz); }
    for (i = B.anim.clips[5].frame_count - 3; i < B.anim.clips[5].frame_count; i++) {
        off_of(&B.skel,&B.anim,5,i,&ox,&oy,&oz);
        printf("   f%-3d (sx,sy,sz)=(%d,%d,%d)\n", i, ox, oy, oz); }

    printf("\nClip 11 (Kau-Loop) Wurzelversatz, ALLE Frames:\n");
    {
        int n = B.anim.clips[11].frame_count, mnx=1<<30, mxx=-(1<<30), mnz=1<<30, mxz=-(1<<30);
        for (f = 0; f < n; f++) {
            off_of(&B.skel,&B.anim,11,f,&ox,&oy,&oz);
            if (ox<mnx) mnx=ox; if (ox>mxx) mxx=ox;
            if (oz<mnz) mnz=oz; if (oz>mxz) mxz=oz;
            if (f < 6 || f >= n-3 || (f%10)==0)
                printf("   f%-3d (sx,sy,sz)=(%d,%d,%d)\n", f, ox, oy, oz);
        }
        printf("   -> sx in [%d..%d], sz in [%d..%d] ueber %d Frames\n", mnx,mxx,mnz,mxz,n);
    }

    printf("\nClip 4 f13 (Anker-Frame): ");
    off_of(&B.skel,&B.anim,4,13,&ox,&oy,&oz); printf("(sx,sy,sz)=(%d,%d,%d)\n", ox,oy,oz);

    /* ---- ORIGINAL-Tickfolge nachgefahren (yaw=0, Anker aus Clip4 f13) ---- */
    {
        int clip = 4, frame = 13;          /* Zustand beim Eintritt in Phase 2 */
        int anchor_x, anchor_z;
        int px, pz, t;
        off_of(&B.skel,&B.anim,4,13,&ox,&oy,&oz);
        anchor_x = 0 - ox; anchor_z = 0 - oz;      /* Anker = pos - off, pos=(0,0) */
        printf("\nORIGINAL-Tickfolge (Anker=(%d,%d)):\n", anchor_x, anchor_z);
        for (t = 1; t <= 126; t++) {
            int phase3 = (t > 120);
            off_of(&B.skel,&B.anim,clip,frame,&ox,&oy,&oz);   /* FUN_80015CB8 */
            px = anchor_x + ox; pz = anchor_z + oz;
            if (t <= 3 || (t >= 118 && t <= 126))
                printf("   P%d tick %3d: platziert clip=%2d f=%3d -> pos=(%d,%d)\n",
                       phase3 ? 3 : 2, t, clip, frame, px, pz);
            if (!phase3) {                                     /* FUN_8001A330(5) */
                if (clip != 5) { clip = 5; frame = 0; }
            } else {                                           /* FUN_8001A240(11) */
                if (clip != 11) { clip = 11; frame = 0; }
            }
            frame++;                                           /* @0x80029B30 */
            if (frame >= B.anim.clips[clip].frame_count) frame = 0;  /* @0x80029B48 */
        }
    }

    /* ---- PORT-Tickfolge (enemy_ai_boss_gator.c:1958-1997) ---- */
    {
        int anchor_x, anchor_z, px = 0, pz = 0, sf, t;
        off_of(&B.skel,&B.anim,4,13,&ox,&oy,&oz);
        anchor_x = 0 - ox; anchor_z = 0 - oz;
        printf("\nPORT-Tickfolge (g->timer 101..340):\n");
        for (t = 101; t <= 346; t++) {
            if (t <= 220) {
                sf = t - 100;
                off_of(&B.skel,&B.anim,5,sf,&ox,&oy,&oz);
                px = anchor_x + ox; pz = anchor_z + oz;
                if (t <= 103 || t >= 218)
                    printf("   P2 timer %3d: sf=%3d clip=5 f=%3d -> pos=(%d,%d)\n",
                           t, sf, sf % B.anim.clips[5].frame_count, px, pz);
            } else if (t <= 340) {
                if (t <= 223 || t >= 338)
                    printf("   P3 timer %3d: clip=11 f=%d  KEINE Platzierung -> pos=(%d,%d)\n",
                           t, (t - 221) % 30, px, pz);
            }
        }
    }
    free(emd);
    return 0;
}
