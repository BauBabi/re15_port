/* Anker-Messung II ROOM1210: wo liegt der RE2-Arm (EM2D) im BILD?
 * Projiziert die Bone-Weltpunkte des aktiven Arms mit der byte-true Kamera (cut 4)
 * auf den Original-Hintergrund ROOM12104.bmp und schreibt ein PPM mit Markierungen. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_damage.h"
#include "re2_ems.h"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)sz);
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static void view_apply(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z, int32_t o[3])
{
    o[0] = (int32_t)(((int64_t)v->rot[0]*x + (int64_t)v->rot[1]*y + (int64_t)v->rot[2]*z) >> 12) + v->trans[0];
    o[1] = (int32_t)(((int64_t)v->rot[3]*x + (int64_t)v->rot[4]*y + (int64_t)v->rot[5]*z) >> 12) + v->trans[1];
    o[2] = (int32_t)(((int64_t)v->rot[6]*x + (int64_t)v->rot[7]*y + (int64_t)v->rot[8]*z) >> 12) + v->trans[2];
}
static int proj(const re15_camera_view_t *v, const int32_t p[3], int *su, int *sv)
{
    int32_t c[3]; view_apply(v, p[0], p[1], p[2], c);
    if (c[2] <= 1) return 0;
    *su = 160 + (int)((int64_t)c[0] * v->fov_screen_dist / c[2]);
    *sv = 120 + (int)((int64_t)c[1] * v->fov_screen_dist / c[2]);
    return 1;
}
int main(int argc, char **argv)
{
    int32_t anker_y = (argc > 1) ? atoi(argv[1]) : -2500;
    int32_t hinter  = (argc > 2) ? atoi(argv[2]) : 400;
    int32_t armz    = (argc > 3) ? atoi(argv[3]) : -17130;
    const char *bgp = (argc > 4) ? argv[4] : NULL;
    const char *outp= (argc > 5) ? argv[5] : NULL;

    size_t n = 0; uint8_t *buf = slurp("shared_assets/PSX/STAGE1/ROOM1210.RDT", &n);
    re15_rdt_t rdt; re15_rdt_parse(buf, n, &rdt);
    re15_camera_view_t vw; re15_camera_build_view(&rdt.cuts[4], &vw);

    size_t es = 0; uint8_t *ems = slurp("shared_assets/RE2/CDEMD0.EMS", &es);
    if (!ems) { printf("CDEMD0.EMS fehlt\n"); return 1; }
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x1A);
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(ems, es, 0x2D, eb, &tim) != 0) { printf("EM2D-Parse fehlgeschlagen\n"); return 1; }
    eb->ok = 1; eb->buf = NULL;
    printf("EM2D: %d Bones, %d Clips\n", eb->skel.bone_count, eb->anim.clip_count);

    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x1A; e->hp = 250;
    e->x = -21090 - hinter; e->y = anker_y; e->z = armz; e->rot_y = 0;   /* Westreihe, Blick +x */
    printf("Arm-Ursprung=(%d,%d,%d) yaw=0\n", (int)e->x, (int)e->y, (int)e->z);

    int32_t ymin = 1 << 30, ymax = -(1 << 30), xmax = -(1 << 30);
    int umin = 999, umax = -999, vmin = 999, vmax = -999;
    for (int clip = 0; clip < eb->anim.clip_count; clip++) {
        e->motion = (int16_t)clip;
        int fc = eb->anim.clips[clip].frame_count;
        int cu0 = 999, cu1 = -999, cv0 = 999, cv1 = -999; int32_t cxmax = -(1<<30);
        for (int f = 0; f < fc; f++) {
            e->anim_frame = (uint16_t)f;
            for (int b = 1; b <= 7; b++) {
                int32_t p[3]; re15_enemy_bone_world_pos(e, b, p);
                if (p[1] < ymin) ymin = p[1];
                if (p[1] > ymax) ymax = p[1];
                if (p[0] > xmax) xmax = p[0];
                if (p[0] > cxmax) cxmax = p[0];
                int u, v;
                if (proj(&vw, p, &u, &v)) {
                    if (u < umin) umin = u; if (u > umax) umax = u;
                    if (v < vmin) vmin = v; if (v > vmax) vmax = v;
                    if (u < cu0) cu0 = u; if (u > cu1) cu1 = u;
                    if (v < cv0) cv0 = v; if (v > cv1) cv1 = v;
                }
            }
        }
        printf("  Clip %d (%2d Bilder): Bild-Rechteck u %3d..%3d  v %3d..%3d ; Hand-x max %d (%d vor der Flaeche)\n",
               clip, fc, cu0, cu1, cv0, cv1, (int)cxmax, (int)(cxmax - (-21090)));
    }
    printf("Bone-Welt-y gesamt: %d .. %d (Ursprung %d)  -> %d ueber / %d unter dem Ursprung\n",
           (int)ymin, (int)ymax, (int)anker_y, (int)(anker_y - ymin), (int)(ymax - anker_y));
    printf("Vorderster Bone-x: %d (Wandflaeche -21090 -> %d im Flur)\n", (int)xmax, (int)(xmax + 21090));
    printf("Bild-Rechteck aller Bones: u %d..%d  v %d..%d\n", umin, umax, vmin, vmax);

    if (bgp && outp) {
        size_t bs = 0; uint8_t *bg = slurp(bgp, &bs);
        if (!bg) { printf("BG fehlt\n"); return 1; }
        /* P6-Kopf ueberspringen: drei Felder */
        size_t o = 2; int fields = 0;
        while (o < bs && fields < 3) { while (o < bs && (bg[o]==' '||bg[o]=='\n'||bg[o]=='\t')) o++;
            while (o < bs && bg[o] > ' ') o++; fields++; }
        o++;
        uint8_t *px = bg + o;
        for (int clip = 0; clip < eb->anim.clip_count; clip++) {
            e->motion = (int16_t)clip;
            int fc = eb->anim.clips[clip].frame_count;
            for (int f = 0; f < fc; f += 4) {
                e->anim_frame = (uint16_t)f;
                for (int b = 1; b <= 7; b++) {
                    int32_t p[3]; re15_enemy_bone_world_pos(e, b, p);
                    int u, v; if (!proj(&vw, p, &u, &v)) continue;
                    if (u < 0 || u >= 320 || v < 0 || v >= 240) continue;
                    size_t i = ((size_t)v*320 + u)*3;
                    px[i] = (b == 3) ? 255 : 0; px[i+1] = (b == 3) ? 64 : 255; px[i+2] = 0;
                }
            }
        }
        FILE *fo = fopen(outp, "wb");
        fwrite(bg, 1, bs, fo); fclose(fo);
        printf("Bild mit Markierungen: %s (gruen = Bones 1..7, rot = Hand-Bone 3)\n", outp);
    }
    return 0;
}
