/* FUN_8002ac94 @ 0x8002ac94  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ac94(int param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  DR_MOVE *p;
  RECT local_18;
  
  if (param_2 == 0 && param_3 == 0) {
    return;
  }
  local_18.x = -(short)param_2;
  if (0 < param_2) {
    return;
  }
  local_18.w = (short)param_2 + 0x140;
  local_18.h = 1;
  p = (DR_MOVE *)(&UNK_800dfd78 + (uint)DAT_800ce5e0 * 0x1680 + param_1 * 0x18);
  local_18.y = (short)param_4;
  if (param_1 + param_3 < 0xf0) {
    sVar1 = (short)(param_1 + param_4) + (short)param_3;
    if (param_1 + param_3 < 0) goto LAB_8002ad3c;
  }
  else {
    sVar1 = local_18.y + 0xef;
  }
  local_18.y = sVar1;
LAB_8002ad3c:
  SetDrawMove(p,&local_18,0,param_1 + param_4);
  if (param_3 < 0) {
                    /* Probable PsyQ macro: addPrim(). */
    p->tag = p->tag & 0xff000000 | *DAT_800dfd74 & 0xffffff;
    *DAT_800dfd74 = *DAT_800dfd74 & 0xff000000 | (uint)p & 0xffffff;
  }
  else {
    DAT_800dfd70->tag = DAT_800dfd70->tag & 0xff000000 | (uint)p & 0xffffff;
    DAT_800dfd70 = p;
  }
  return;
}


