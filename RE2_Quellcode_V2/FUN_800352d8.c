/* FUN_800352d8 @ 0x800352d8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800352d8(int *param_1,uint *param_2,uint *param_3)

{
  undefined4 uVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  uVar1 = 0;
  if ((((*param_2 | *param_3) & 2) == 0) && (uVar1 = 0, (*param_2 & *param_3 & 0x1000) == 0)) {
    iVar5 = (uint)*(ushort *)((int)param_2 + 0x9a) + (uint)*(ushort *)((int)param_3 + 0x9a);
    iVar4 = param_3[0x21] - (*param_1 + (int)(short)param_2[0x28]);
    iVar3 = param_3[0x23] - (param_1[2] + (int)*(short *)((int)param_2 + 0xa2));
    uVar1 = 0;
    if (((uint)(iVar4 + iVar5) <= (uint)(iVar5 * 2)) &&
       (uVar1 = 0, (uint)(iVar3 + iVar5) <= (uint)(iVar5 * 2))) {
      lVar2 = SquareRoot0(iVar4 * iVar4 + iVar3 * iVar3);
      uVar1 = 0;
      if ((0 < iVar5 - lVar2) &&
         ((iVar4 = (uint)*(ushort *)((int)param_3 + 0x9e) + (uint)*(ushort *)((int)param_2 + 0x9e),
          iVar3 = param_3[0x22] - (param_1[1] + (int)(short)param_2[0x26]), iVar3 <= -iVar4 ||
          (uVar1 = 1, iVar4 <= iVar3)))) {
        uVar1 = 0;
      }
    }
  }
  return uVar1;
}


