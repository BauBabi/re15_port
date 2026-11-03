/* leiche_wand.c - Abnahme 3 Spur AN: Gelenke der liegenden Leiche (Clip 24 Bild 0) gegen die SCA-Zellen von ROOM5090.
 * Nur Messwerkzeug. Lage/Gier/Clip aus dem eigenen Fenster-state.log (kampf: @(4856,-22748,r1170), y 900, mo 24 af 0).
 * Gerippe = ANNETTE_4D.EMD ueber re15_emd_parse_container wie test_r41_annette bank_aus. Ausgabe: je Gelenk Weltlage und
 * kleinster Abstand zu einer SCA-Zelle des Bandes 0 (negativ = im Kasten). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_damage.h"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)l); if (fread(b, 1, (size_t)l, f) != (size_t)l) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)l; return b;
}

int main(void)
{
    size_t n = 0, m = 0;
    uint8_t *r = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &n);
    static re15_rdt_t rdt;
    if (!r || re15_rdt_parse(r, n, &rdt) != 0) { printf("RDT fehlt\n"); return 1; }
    uint8_t *e = slurp(RE15_ASSET_PSX_DIR "/../RE2/ANNETTE_4D.EMD", &m);
    if (!e) { printf("EMD fehlt\n"); return 1; }
    re15_enemy_reset();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x4Du);
    if (!eb || re15_emd_parse_container(e, m, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) { printf("EMD-Parse\n"); return 1; }
    eb->ok = 1; eb->buf = NULL;
    re15_actor_init();
    re15_actor_t *a = &g_actors[1];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0x4D; a->x = 4856; a->y = 900; a->z = -22748; a->rot_y = 1170; a->motion = 24; a->anim_frame = 0;
    printf("SCA-Zellen: %d, Gelenke: %d\n", rdt.sca_count, eb->skel.bone_count);
    int32_t gmin = 1 << 30;
    for (int b = 0; b < eb->skel.bone_count; b++) {
        int32_t w[3]; re15_enemy_bone_world_pos(a, b, w);
        int32_t best = 1 << 30; int bi = -1;
        for (int i = 0; i < rdt.sca_count; i++) {
            const re15_sca_entry_t *s = &rdt.sca[i];
            if ((s->floor >> 4) != 0) continue;
            int32_t x0 = s->x, x1 = s->x + s->width, z0 = s->z, z1 = s->z + s->density;
            int32_t dx = (w[0] < x0) ? x0 - w[0] : (w[0] > x1) ? w[0] - x1 : 0;
            int32_t dz = (w[2] < z0) ? z0 - w[2] : (w[2] > z1) ? w[2] - z1 : 0;
            int32_t d;
            if (dx == 0 && dz == 0) {                       /* innen: negativer Abstand zum naechsten Rand */
                int32_t in = w[0] - x0; if (x1 - w[0] < in) in = x1 - w[0];
                if (w[2] - z0 < in) in = w[2] - z0; if (z1 - w[2] < in) in = z1 - w[2];
                d = -in;
            } else d = (dx > dz) ? dx : dz;                 /* Chebyshev genuegt als Grobmass */
            if (d < best) { best = d; bi = i; }
        }
        if (best < gmin) gmin = best;
        const re15_sca_entry_t *s = &rdt.sca[bi];
        printf("Gelenk %2d (%6d,%5d,%6d)  naechste Zelle #%d typ %d x %d..%d z %d..%d  Abstand %d\n", b, w[0], w[1], w[2], bi,
               s->type, s->x, s->x + s->width, s->z, s->z + s->density, best);
    }
    printf("kleinster Abstand Gelenk-Zelle: %d\n", gmin);
    return 0;
}
