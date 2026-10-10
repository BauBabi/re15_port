import sys
src = open(sys.argv[1], encoding="utf-8").read()
needle = "    re15_schuss_r36_fx_merke(part, p, a3);                   /* Mess-Ring (Sonden), Runde 36 Spur C */\n    re15_esp_fx_spawn_rows("
assert needle in src, "needle"
dbg = r'''    if (getenv("DBG_FX") && a3) { int32_t R_[9], T_[3]; int rv_ = re15_schuss_r36_gezeichnet(e, re2z_part_to_bone(e, part), R_, T_);
        printf("  FXDBG part %d a3 (%d,%d,%d) p (%d,%d,%d) | e (%d,%d,%d) r%d mo %d af %d st %u/%u/%u | anker rv %d T (%d,%d,%d) Rcol1 (%d,%d,%d)\n", part, a3[0], a3[1], a3[2], p[0], p[1], p[2],
               e->x, e->y, e->z, e->rot_y, (int)e->motion, (int)e->anim_frame, e->state, e->sub_state_1, e->sub_state_2, rv_, T_[0], T_[1], T_[2], R_[1], R_[4], R_[7]); }
'''
src = src.replace(needle, dbg + needle, 1)
src = "#include <stdlib.h>\n#include <stdio.h>\n" + src
open(sys.argv[2], "w", encoding="utf-8").write(src)
