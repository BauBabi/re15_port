/* =============================================================================
 * test_ems — the EMS enemy-model archive index (re15_ems.c), against the REAL
 * EMD/CDEMD0.EMS. Verifies the byte-true blob offsets (matching the Java
 * RE15MasterExtractor split), the type->index order table, and that an extracted
 * zombie EMD (em16, type 0x16) parses cleanly through re15_emd_parse_container —
 * i.e. the whole "load an enemy model out of the CD archive" chain works. This
 * is the foundation for rendering the briefing zombies (they ship ONLY inside
 * CDEMD0/1.EMS; there are no per-type EM<NN>.EMD files).
 * ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "re15_ems.h"
#include "re15_emd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return NULL; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (out_sz) *out_sz = sz;
    return b;
}

int main(void)
{
    int fail = 0;
    const char *path = RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS";
    long sz = 0;
    uint8_t *ems = slurp(path, &sz);
    if (!ems) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }
    printf("=== EMS enemy-model archive index (CDEMD0.EMS, %ld bytes) ===\n", sz);

    /* (1) the type -> blob-index order table (DEFAULT_EMS_ORDER). */
    struct { uint8_t type; int idx; } map[] = {
        {0x10, 0}, {0x11, 1}, {0x16, 4}, {0x47, 21}, {0x4D, 24}, {0xFF, -1}, {0x00, -1}
    };
    for (int i = 0; i < (int)(sizeof map / sizeof map[0]); i++) {
        int got = re15_ems_index_for_type(map[i].type);
        if (got != map[i].idx) {
            fprintf(stderr, "FAIL: index_for_type(0x%02X) = %d, expected %d\n",
                    map[i].type, got, map[i].idx); fail = 1; }
    }
    if (!fail) printf("  (1) type->index: 0x10=0 0x11=1 0x16=4 0x47=21 0x4D=24 unknown=-1\n");

    /* (2) the byte-true blob offsets + lengths (the three zombie models). */
    struct { int idx; size_t off; size_t len; } ent[] = {
        {0, 0,      214496},   /* em10 (type 0x10) */
        {1, 215040, 214272},   /* em11 (type 0x11) */
        {4, 874496, 213584},   /* em16 (type 0x16) */
    };
    for (int i = 0; i < (int)(sizeof ent / sizeof ent[0]); i++) {
        size_t off = 0, len = 0;
        if (re15_ems_get_entry(ems, (size_t)sz, ent[i].idx, &off, &len) != 0) {
            fprintf(stderr, "FAIL: get_entry(%d) not found\n", ent[i].idx); fail = 1; continue; }
        if (off != ent[i].off || len != ent[i].len) {
            fprintf(stderr, "FAIL: entry %d off/len = %zu/%zu, expected %zu/%zu\n",
                    ent[i].idx, off, len, ent[i].off, ent[i].len); fail = 1; }
    }
    if (!fail) printf("  (2) blob offsets byte-true: em10@0 em11@215040 em16@874496\n");

    /* a past-the-end index must fail cleanly. */
    { size_t o, l; if (re15_ems_get_entry(ems, (size_t)sz, 9999, &o, &l) == 0) {
        fprintf(stderr, "FAIL: get_entry(9999) must fail\n"); fail = 1; } }

    /* (3) the full chain: extract the em16 zombie blob and parse it as an EMD —
     * meshes + skeleton + animation, including the death clip 0x1f (FUN_80107cb0). */
    {
        int idx = re15_ems_index_for_type(0x16);
        size_t off = 0, len = 0;
        if (re15_ems_get_entry(ems, (size_t)sz, idx, &off, &len) != 0) {
            fprintf(stderr, "FAIL: em16 blob lookup\n"); fail = 1;
        } else {
            re15_md1_t md1 = {0};
            re15_emd_skeleton_t skel = {0};
            re15_emd_animation_t anim = {0};
            re15_tim_t tim = {0};
            int rc = re15_emd_parse_container(ems + off, len, &md1, &skel, &anim, &tim);
            if (rc != 0) {
                fprintf(stderr, "FAIL: em16 EMD parse rc=%d\n", rc); fail = 1;
            } else if (md1.mesh_count <= 0 || skel.bone_count <= 0 || anim.clip_count <= 0) {
                fprintf(stderr, "FAIL: em16 empty: %d meshes / %d bones / %d clips\n",
                        md1.mesh_count, skel.bone_count, anim.clip_count); fail = 1;
            } else if (anim.clip_count <= 0x1f || anim.clips[0x1f].frame_count <= 0) {
                fprintf(stderr, "FAIL: em16 has no death clip 0x1f (clip_count=%d)\n",
                        anim.clip_count); fail = 1;
            } else {
                printf("  (3) em16 zombie EMD parsed: %d meshes, %d bones, %d clips; "
                       "death clip 0x1f = %d frames\n",
                       md1.mesh_count, skel.bone_count, anim.clip_count,
                       anim.clips[0x1f].frame_count);
            }
        }
    }

    free(ems);
    if (fail) { fprintf(stderr, "\nEMS ARCHIVE-INDEX TEST FAILED\n"); return 1; }
    printf("\nPASS: EMS enemy-model archive index (type->index; byte-true offsets; em16 EMD parse)\n");
    return 0;
}
