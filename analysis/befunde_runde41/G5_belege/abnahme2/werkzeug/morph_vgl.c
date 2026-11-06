/* Abnahme 2: Port-Morph (re15_emd_morph_apply, Mesh 2 EM36) gegen RE2-RAM-Vertexpuffer (0x80132be0, 635 Vertices) bei den RE2-Gewichten */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_enemy.h"
#include "re2_ems.h"
#include "re15_math.h"
static uint8_t *slurp(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); if (!f) return 0; fseek(f, 0, 2); long l = ftell(f); fseek(f, 0, 0);
    uint8_t *b = malloc(l); fread(b, 1, l, f); fclose(f); *n = (size_t)l; return b; }
int main(int argc, char **argv)
{
    size_t n = 0; uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    re15_enemy_bank_t *eb = re15_enemy_find(0x36); if (!eb) eb = re15_enemy_alloc(0x36);
    if (!ems || !eb || re2_ems_load_bank(ems, n, 0x36, eb, NULL) != 0) { printf("Bank fehlt\n"); return 1; }
    eb->buf = NULL; eb->ok = 1;
    if (!eb->morph_ok) { printf("kein Morph\n"); return 1; }
    for (int a = 1; a < argc; a++) {
        FILE *f = fopen(argv[a], "r"); int nv, w[4]; if (fscanf(f, "%d %d %d %d %d", &nv, &w[0], &w[1], &w[2], &w[3]) != 5) return 1;
        static re15_md1_vertex_t pv[1024]; int16_t ww[4] = { (int16_t)w[0], (int16_t)w[1], (int16_t)w[2], (int16_t)w[3] };
        re15_emd_morph_apply(&eb->morph[2], ww, pv);
        int dmax = 0, dmaxk = -1, gleich = 0; long dsum = 0;
        for (int k = 0; k < nv; k++) { int x, y, z; fscanf(f, "%d %d %d", &x, &y, &z);
            int dx = pv[k].x - x, dy = pv[k].y - y, dz = pv[k].z - z; int d = (int)re15_squareroot0((uint32_t)(dx*dx + dy*dy + dz*dz));
            if (d == 0) gleich++; dsum += d; if (d > dmax) { dmax = d; dmaxk = k; } }
        fclose(f);
        printf("%s: Gewichte (%d,%d,%d,%d) nv %d (Port-Morph %d): bitgleich %d, mittel %ld, max %d (Vertex %d)\n", argv[a], w[0], w[1], w[2], w[3], nv,
               eb->morph[2].nverts, gleich, dsum / nv, dmax, dmaxk);
    }
    return 0;
}
