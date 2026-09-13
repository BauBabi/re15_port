/* DIAGNOSE: die Knochen-Hierarchie des RE2-Zombies aus der GELADENEN Bank.
 *
 * ⛔ WARUM: zwei Recherchen behaupteten, die Part-Benennung im Port sei vertauscht
 * (2..7 = Beine statt Arme), mit Belegen auf CDEMD0.EMS-Offsets 0x2A800 bzw. 0x2AE9C.
 * Beide Male lieferte ein roher Byte-Griff dort etwas anderes als behauptet - das EMR
 * traegt die Hierarchie nicht als flaches parent[]. Der Port-Parser (re2_ems.c) baut sie
 * beim Laden auf; DIESE Ausgabe ist damit die belastbare Quelle.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
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
    printf("=== RE2-Zombie EM010: %d Bones ===\n", eb->skel.bone_count);
    for (int i = 0; i < eb->skel.bone_count; i++)
        printf("  Bone %2d: Eltern %2d  relpos (%6d,%6d,%6d)\n",
               i, eb->skel.bone_parent[i],
               eb->skel.bone_relative_pos[i][0], eb->skel.bone_relative_pos[i][1], eb->skel.bone_relative_pos[i][2]);
    printf("--- Kinder je Bone ---\n");
    for (int p = -1; p < eb->skel.bone_count; p++) {
        int erst = 1;
        for (int i = 0; i < eb->skel.bone_count; i++) {
            if (eb->skel.bone_parent[i] != p) continue;
            if (erst) { printf("  %2d ->", p); erst = 0; }
            printf(" %d", i);
        }
        if (!erst) printf("\n");
    }
    return 0;
}
