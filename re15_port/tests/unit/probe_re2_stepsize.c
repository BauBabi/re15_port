/* probe_re2_stepsize.c — MESSSONDE (kein Fix): wie weit traegt EIN Tick Wurzelbewegung des
 * RE2-Zombies, und wie weit ein Tick mit UEBERSPRUNGENEM Keyframe?
 *
 * Grundlage (Original, selbst disassembliert, info/re2leon/PSX.EXE):
 *   FUN_80015E7C @0x80015FCC-FE4:  +0x144 = root(kf_jetzt) - *(s16*)(self+0x20C)
 *                                  *(self+0x20C) = root(kf_jetzt)   (@0x80015FC4)
 *   -> der Bezugspunkt ist der ZULETZT GESPEICHERTE Wurzelstand, NICHT "ein Keyframe zurueck".
 *      Wird zwischen zwei e7c-Aufrufen ZWEIMAL vorgerueckt (FUN_8002A9C8 @0x8002A9FC),
 *      ist das Delta die Summe zweier Keyframe-Differenzen.
 *   FUN_800152C8 @0x800152E4-80015334: dreht +0x144 um Yaw und addiert auf +0x38/+0x40.
 *
 * Die Sonde liest die ECHTEN Clip-Wurzelfelder der geladenen RE2-Bank (CDEMD0.EMS) und
 * gibt pro Clip die Ein-Keyframe- und die Zwei-Keyframe-Differenzen aus.
 */
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re2_ems.h"
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

static uint8_t s_blob[0x80000];
static uint8_t *s_r15 = NULL; static size_t s_r15n = 0;
static int load_bank_re15(uint8_t type)
{
    if (!s_r15) s_r15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_r15n);
    if (!s_r15) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_r15, s_r15n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_r15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static int kf_of(const re15_emd_animation_t *A, int clip, int slot)
{
    const re15_emd_clip_t *c = &A->clips[clip];
    int fi = c->first_frame + slot, fend = c->first_frame + c->frame_count - 1;
    while ((A->frames[fi] & 0x8000u) && fi < fend) fi++;    /* 0x8000-Skip @0x80015e18-38 */
    return (int)(A->frames[fi] & 0xFFFu);
}

static void dump_clip(const char *tag, const re15_emd_skeleton_t *S,
                      const re15_emd_animation_t *A, int clip)
{
    if (!A || clip < 0 || clip >= A->clip_count) { printf("  %s clip %d: FEHLT\n", tag, clip); return; }
    const re15_emd_clip_t *c = &A->clips[clip];
    if (c->frame_count <= 0) { printf("  %s clip %d: leer\n", tag, clip); return; }
    printf("  %s clip %d : %d Bilder\n", tag, clip, c->frame_count);
    int n = c->frame_count;
    int maxd1 = 0, maxd2 = 0; long sum1 = 0;
    printf("    slot  kf    sx     sz  |  d1(sx) d1(sz) |len1|  d2(sx) d2(sz) |len2|\n");
    for (int s = 0; s < n; s++) {
        int kfn = kf_of(A, clip, s);
        int kp1 = kf_of(A, clip, (s + n - 1) % n);
        int kp2 = kf_of(A, clip, (s + n - 2) % n);
        int16_t sx, sy, sz, ax, ay, az, bx, by, bz;
        if (!re15_emd_get_keyframe_speed(S, kfn, &sx, &sy, &sz)) continue;
        if (!re15_emd_get_keyframe_speed(S, kp1, &ax, &ay, &az)) continue;
        if (!re15_emd_get_keyframe_speed(S, kp2, &bx, &by, &bz)) continue;
        int d1x = sx - ax, d1z = sz - az;
        int d2x = sx - bx, d2z = sz - bz;
        int l1 = (int)(0.5 + __builtin_sqrt((double)(d1x*d1x + d1z*d1z)));
        int l2 = (int)(0.5 + __builtin_sqrt((double)(d2x*d2x + d2z*d2z)));
        if (s > 0) { if (l1 > maxd1) maxd1 = l1; sum1 += l1; }
        if (s > 1 && l2 > maxd2) maxd2 = l2;
        printf("    %3d  %4d %6d %6d |  %6d %6d %5d | %6d %6d %5d\n",
               s, kfn, (int)sx, (int)sz, d1x, d1z, l1, d2x, d2z, l2);
    }
    printf("    => max 1-Keyframe-Schritt %d, max 2-Keyframe-Schritt %d, Summe(1kf, ohne Wrap) %ld\n",
           maxd1, maxd2, sum1);
}

int main(void)
{
    const uint8_t types[3] = { 0x10, 0x11, 0x16 };
    for (int t = 0; t < 3; t++) {
        uint8_t ty = types[t];
        re15_enemy_reset();
        if (!load_bank_re2(ty)) { printf("RE2 Bank Typ 0x%02x: NICHT geladen\n", ty); continue; }
        re15_enemy_bank_t *b = re15_enemy_find(ty);
        printf("\n########## RE2-Bank Typ 0x%02x (CDEMD0.EMS) ##########\n", ty);
        printf("  Haupt-Bank: %d Clips, Loco-Bank: %s (%d Clips)\n",
               b->anim.clip_count, b->loco_ok ? "ja" : "nein",
               b->loco_ok ? b->anim_loco.clip_count : 0);
        for (int cl = 0; cl <= 2; cl++) {
            dump_clip("PAIR2/main", &b->skel, &b->anim, cl);
            if (b->loco_ok) dump_clip("PAIR1/loco", &b->skel_loco, &b->anim_loco, cl);
        }
    }
    /* Gegenprobe mit der RE1.5-Bank, damit der Vergleich Flavor-sauber ist. */
    re15_enemy_reset();
    if (load_bank_re15(0x10)) {
        re15_enemy_bank_t *b = re15_enemy_find(0x10);
        printf("\n########## RE1.5-Bank Typ 0x10 (EMD/CDEMD0.EMS) ##########\n");
        printf("  Haupt-Bank: %d Clips, Loco-Bank: %s (%d Clips)\n",
               b->anim.clip_count, b->loco_ok ? "ja" : "nein",
               b->loco_ok ? b->anim_loco.clip_count : 0);
        for (int cl = 0; cl <= 2; cl++) {
            dump_clip("PAIR2/main", &b->skel, &b->anim, cl);
            if (b->loco_ok) dump_clip("PAIR1/loco", &b->skel_loco, &b->anim_loco, cl);
        }
    }
    printf("\nSONDE-OK\n");
    return 0;
}
