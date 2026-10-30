/* probe_ems_bloecke.c — MESSSONDE (kein ctest): jeden EMD-Block eines EMS-Archivs parsen, Kennzahlen drucken und die
 * Pose von Keyframe 0 als OBJ schreiben (Alligator-Suche 2026-10-30: 29 Bloecke im Archiv, 25 mit Port-Typ).
 * Aufruf: probe_ems_bloecke <EMS> <ausgabe-ordner> */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"

int main(int argc, char **argv)
{
    if (argc < 3) return 2;
    FILE *f = fopen(argv[1], "rb"); if (!f) return 1;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = malloc((size_t)n); if (fread(d, 1, (size_t)n, f) != (size_t)n) return 1; fclose(f);
    for (int i = 0; i < 64; i++) {
        size_t off = 0, len = 0;
        if (re15_ems_get_entry(d, (size_t)n, i, &off, &len) != 0) break;
        static re15_md1_t md1; static re15_emd_skeleton_t sk; static re15_emd_animation_t an; re15_tim_t tim;
        memset(&md1, 0, sizeof md1); memset(&sk, 0, sizeof sk); memset(&an, 0, sizeof an); memset(&tim, 0, sizeof tim);
        uint8_t *blob = malloc(len); memcpy(blob, d + off, len);
        int rc = re15_emd_parse_container(blob, len, &md1, &sk, &an, &tim);
        printf("Block %2d Sektor %5zu Laenge %7zu rc=%d Knochen %2d Meshes %2d Clips %2d TIM %dx%d\n",
               i, off / 2048, len, rc, sk.bone_count, md1.mesh_count, an.clip_count, tim.width, tim.height);
        if (rc != 0) continue;
        re15_skel_pose_t P[RE15_EMD_MAX_BONES];
        if (re15_skel_compute_pose(&sk, 0, P) != 0) continue;
        char p[512]; snprintf(p, sizeof p, "%s/block%02d.obj", argv[2], i);
        FILE *o = fopen(p, "w"); if (!o) continue;
        int base = 1;
        for (int b = 0; b < md1.mesh_count && b < RE15_EMD_MAX_BONES; b++) {
            int bi = (b < sk.bone_count) ? b : 0;
            const re15_md1_mesh_t *m = &md1.meshes[b];
            for (int k = 0; k < m->tri_vertex_count; k++) {
                const re15_md1_vertex_t *v = &m->tri_vertices[k]; int32_t w[3];
                for (int r = 0; r < 3; r++) w[r] = (int32_t)(((int64_t)P[bi].rot[r*3]*v->x + (int64_t)P[bi].rot[r*3+1]*v->y + (int64_t)P[bi].rot[r*3+2]*v->z) >> 12) + P[bi].trans[r];
                fprintf(o, "v %d %d %d\n", w[0], -w[1], w[2]);
            }
            for (int k = 0; k < m->triangle_count; k++) {
                const re15_md1_tri_uv_t *u = &m->triangle_uvs[k];
                fprintf(o, "f %d %d %d\nt %d %d %d %d %d %d %d %d\n", base + m->triangles[k].v0, base + m->triangles[k].v1, base + m->triangles[k].v2,
                        u->u0, u->v0, u->u1, u->v1, u->u2, u->v2, u->clut, u->page);
            }
            base += m->tri_vertex_count;
            for (int k = 0; k < m->quad_vertex_count; k++) {
                const re15_md1_vertex_t *v = &m->quad_vertices[k]; int32_t w[3];
                for (int r = 0; r < 3; r++) w[r] = (int32_t)(((int64_t)P[bi].rot[r*3]*v->x + (int64_t)P[bi].rot[r*3+1]*v->y + (int64_t)P[bi].rot[r*3+2]*v->z) >> 12) + P[bi].trans[r];
                fprintf(o, "v %d %d %d\n", w[0], -w[1], w[2]);
            }
            for (int k = 0; k < m->quad_count; k++) {
                const re15_md1_quad_uv_t *u = &m->quad_uvs[k];
                fprintf(o, "f %d %d %d %d\nt %d %d %d %d %d %d %d %d %d %d\n", base + m->quads[k].v0, base + m->quads[k].v1, base + m->quads[k].v3, base + m->quads[k].v2,
                        u->u0, u->v0, u->u1, u->v1, u->u3, u->v3, u->u2, u->v2, u->clut, u->page);
            }
            base += m->quad_vertex_count;
        }
        fclose(o);
        if (tim.pixels && tim.clut) {   /* TIM roh: Kopf (bpp w h clut_x clut_y n) + CLUT + Pixel */
            snprintf(p, sizeof p, "%s/block%02d.tim.bin", argv[2], i);
            FILE *t = fopen(p, "wb");
            if (t) {
                int32_t hdr[6] = { tim.bpp, tim.width, tim.height, tim.clut_x, tim.clut_y, tim.clut_entries };
                fwrite(hdr, 4, 6, t); fwrite(tim.clut, 2, (size_t)tim.clut_entries, t);
                int bytes = tim.width * tim.height * tim.bpp / 8;
                fwrite(tim.pixels, 1, (size_t)bytes, t); fclose(t);
            }
        }
    }
    return 0;
}
