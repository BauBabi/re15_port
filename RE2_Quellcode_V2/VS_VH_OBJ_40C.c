/* VS_VH_OBJ_40C @ 0x80085744  (Ghidra headless, raw MIPS overlay) */

void VS_VH_OBJ_40C(int *param_1,uint param_2,int param_3,int param_4)

{
  int unaff_s0;
  short unaff_s1;
  int unaff_s2;
  
  while( true ) {
    param_2 = param_2 + 1;
    param_1 = param_1 + 1;
    if (param_3 < (int)param_2) break;
    unaff_s0 = unaff_s0 + *param_1;
    if ((param_2 & 1) == 0) {
      *(short *)(((int)param_2 / 2) * 0x10 + unaff_s2 + 0xc) =
           (short)((uint)(param_4 + unaff_s0) >> 3);
    }
    else {
      *(short *)(((int)param_2 / 2) * 0x10 + unaff_s2 + 0xe) =
           (short)((uint)(param_4 + unaff_s0) >> 3);
    }
  }
  *(int *)(&DAT_800eada0 + unaff_s1 * 4) = unaff_s0;
  (&DAT_800dcc68)[unaff_s1] = 2;
  return;
}


