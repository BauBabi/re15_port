/* FUN_8002ea40 @ 0x8002ea40  (Ghidra headless, raw MIPS overlay) */

bool FUN_8002ea40(int param_1,uint param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  param_2 = param_2 & 0x7f;
  uVar5 = *(int *)(param_3 + param_2 * -8 + 8) - (int)DAT_800cfb7e;
  uVar4 = *(int *)(param_1 + param_2 * -8 + 0xc) - (int)DAT_800cfb7e;
  if ((int)(uVar5 ^ uVar4) < 0) {
    bVar1 = false;
  }
  else {
    iVar3 = *(int *)(param_2 * 8 + param_1 + 4);
    iVar6 = (*(int *)(param_2 * 8 + param_3) - iVar3) * uVar4;
    iVar2 = uVar5 + uVar4;
    if (iVar2 == 0) {
      trap(0x1c00);
    }
    if ((iVar2 == -1) && (iVar6 == -0x80000000)) {
      trap(0x1800);
    }
    bVar1 = (uint)((iVar6 / iVar2 + iVar3) - (int)DAT_800cfb82) <
            (uint)((int)DAT_800cfb80 - (int)DAT_800cfb82);
  }
  return bVar1;
}


