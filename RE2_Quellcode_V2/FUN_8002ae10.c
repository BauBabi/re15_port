/* FUN_8002ae10 @ 0x8002ae10  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ae10(int param_1,int param_2,undefined4 param_3,uint *param_4)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)DAT_800ce5e0;
  if (param_1 == 1) {
    iVar1 = uVar2 * 0x1000 + 0x44;
  }
  else if (param_1 == 0) {
    iVar1 = uVar2 << 5;
  }
  else {
    if (param_1 != 2) goto LAB_8002ae74;
    iVar1 = uVar2 * 0x40 + 0x2048;
  }
  param_4 = (uint *)(&DAT_800cc1e8 + param_2 * 4 + iVar1);
LAB_8002ae74:
  DAT_800dfd70 = &DAT_800e2a78 + uVar2;
                    /* Probable PsyQ macro: addPrim(). */
  *DAT_800dfd70 = *DAT_800dfd70 & 0xff000000 | *param_4 & 0xffffff;
  *param_4 = *param_4 & 0xff000000 | (uint)DAT_800dfd70 & 0xffffff;
  DAT_800dfd74 = param_4;
  DAT_800dfd6c = *DAT_800dfd70;
  return;
}


