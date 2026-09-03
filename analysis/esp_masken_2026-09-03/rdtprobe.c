#include <stdio.h>
#include <stdlib.h>
#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_aot.h"
re15_aot_state_t g_aot;
int re15_aot_set_cam_switch(int slot, int32_t cx, int32_t cz, int32_t hw, int32_t hh,
                            uint8_t a, uint8_t b)
{ (void)slot;(void)cx;(void)cz;(void)hw;(void)hh;(void)a;(void)b; return 0; }

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *b = malloc((size_t)sz);
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) return 1;
    fclose(f);
    re15_rdt_t r;
    int rc = re15_rdt_parse(b, (size_t)sz, &r);
    printf("parse rc=%d  nCut=%d nOmodel=%d prop_count=%d zones=%d sca=%d subs=%d\n",
           rc, r.cut_count, (int)r.nOmodel, r.prop_count, r.zone_count, r.sca_count, r.sub_scd_count);
    for (int i = 0; i < r.prop_count; i++) {
        printf("  prop[%d]: md1 file-offset=0x%lX size=%d   tim file-offset=0x%lX size=%d\n", i,
               (unsigned long)(r.prop_md1[i]-b), r.prop_md1_size[i],
               (unsigned long)(r.prop_tim[i]-b), r.prop_tim_size[i]);
        re15_md1_t m;
        int mr = re15_md1_parse(r.prop_md1[i], r.prop_md1_size[i], &m);
        printf("           md1_parse=%d meshes=%d\n", mr, mr==0?m.mesh_count:-1);
        if (mr == 0) for (int k = 0; k < m.mesh_count; k++)
            printf("             mesh%d tris=%d quads=%d qverts=%d\n", k,
                   m.meshes[k].triangle_count, m.meshes[k].quad_count, m.meshes[k].quad_vertex_count);
        re15_tim_t t;
        int tr = re15_tim_parse(r.prop_tim[i], r.prop_tim_size[i], &t);
        printf("           tim_parse=%d  %dx%d\n", tr, tr==0?t.width:0, tr==0?t.height:0);
    }
    for (int c = 0; c < r.cut_count; c++) {
        re15_pri_cut_t pc;
        int n = re15_pri_parse_section(b, (size_t)sz, r.cuts[c].pri_offset, &pc);
        unsigned int first = 0;
        if (r.cuts[c].pri_offset + 4 <= (unsigned)sz)
            first = (unsigned)b[r.cuts[c].pri_offset] | ((unsigned)b[r.cuts[c].pri_offset+1]<<8)
                  | ((unsigned)b[r.cuts[c].pri_offset+2]<<16) | ((unsigned)b[r.cuts[c].pri_offset+3]<<24);
        printf("  cut%-2d pri_offset=0x%08X  first_u32=0x%08X  parse=%d masks=%d\n",
               c, r.cuts[c].pri_offset, first, n, pc.mask_count);
    }
    printf("  hdr[7] (per-room mask capacity) = %d\n", b[7]);
    return 0;
}
