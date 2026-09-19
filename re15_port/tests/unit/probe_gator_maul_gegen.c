/* probe_gator_maul_gegen.c - GEGENPRUEFUNG zur Sonde probe_gator_maul.c.
 * Frage: ist der XZ-Abstand 5596 (P3-Park) EINDEUTIG, oder treffen auch
 * P2-Frames (sf 0..119) denselben Wert? Rein datengetrieben aus der
 * EM23-Bank (Gator-Clip 5 vs Opfer-Clip 1, gemeinsamer Anker).
 */
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

static void clip_off(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                     int clip, int frame, int32_t out[3])
{
    out[0] = out[1] = out[2] = 0;
    if (!sk || !an || clip < 0 || clip >= an->clip_count) return;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (c->frame_count <= 0) return;
    int slot = frame % c->frame_count;
    int fi = c->first_frame + slot, fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    int kf = (int)(an->frames[fi] & 0xFFFu);
    int16_t sx = 0, sy = 0, sz = 0;
    re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz);
    out[0] = sx; out[1] = sy; out[2] = sz;
}

int main(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("FAIL: EMS\n"); return 1; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x23u);
    if (!eb || re2_ems_load_bank(ems, n, 0x23, eb, NULL) != 0) { printf("FAIL: bank\n"); return 1; }
    eb->buf = NULL; eb->ok = 1;
    printf("victim_ok=%d gclips=%d vclips=%d\n", (int)eb->victim_ok,
           eb->anim.clip_count, eb->anim_victim.clip_count);

    int sf, treffer = 0, mn_i = -1; double mn_d = 1e9, mx_d = -1e9;
    printf("sf  gator5=(  x,   z)  opfer1=(  x,   z)  lokal-d=(  dx,  dz)  |XZ|\n");
    for (sf = 0; sf < 120; sf++) {
        int32_t g[3], v[3];
        clip_off(&eb->skel, &eb->anim, 5, sf, g);
        clip_off(&eb->skel_victim, &eb->anim_victim, 1, sf, v);
        double dx = (double)(v[0] - g[0]), dz = (double)(v[2] - g[2]);
        double d = sqrt(dx*dx + dz*dz);
        if (d < mn_d) mn_d = d;
        if (d > mx_d) { mx_d = d; }
        if (fabs(d-5596.5) < 60.0) { treffer++; mn_i = sf; }
        if ((sf % 6) == 0 || fabs(d-5596.5) < 60.0)
            printf("%3d  (%6d,%6d)  (%6d,%6d)  (%6.0f,%6.0f)  %8.2f%s\n",
                   sf, g[0], g[2], v[0], v[2], dx, dz, d,
                   (fabs(d-5596.5) < 60.0) ? "   <== nahe 5596" : "");
    }
    /* P3-Park: Gator bleibt auf Clip-5-f0-Versatz (sf=120 -> 120%120=0), Leon auf Opfer-1 f119 */
    {   int32_t g[3], v[3];
        clip_off(&eb->skel, &eb->anim, 5, 0, g);
        clip_off(&eb->skel_victim, &eb->anim_victim, 1, 119, v);
        double dx = (double)(v[0] - g[0]), dz = (double)(v[2] - g[2]);
        printf("P3-PARK: gator5_f0=(%d,%d) opfer1_f119=(%d,%d) -> d=(%.0f,%.0f) |XZ|=%.2f\n",
               g[0], g[2], v[0], v[2], dx, dz, sqrt(dx*dx + dz*dz));
        clip_off(&eb->skel, &eb->anim, 4, 13, g);
        printf("  (Vergleich Gator-C4-f13 = %d,%d -> |XZ|=%.2f)\n", g[0], g[2],
               sqrt(pow((double)(v[0]-g[0]),2) + pow((double)(v[2]-g[2]),2)));
    }
    printf("P2-Spanne |XZ| = %.2f .. %.2f ; Frames im 5596+-60-Fenster: %d (zuletzt sf=%d)\n",
           mn_d, mx_d, treffer, mn_i);
    return 0;
}
