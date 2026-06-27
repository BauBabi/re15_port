/* FUN_8004ed84 @ 0x8004ed84  (Ghidra headless, raw MIPS overlay) */

bool FUN_8004ed84(int *param_1,int param_2,short *param_3)

{
  int iVar1;
  int iVar2;
  long lVar3;
  uint uVar4;
  
  uVar4 = (uint)((ushort)param_3[2] >> 1);
  iVar1 = *param_1 - ((int)*param_3 + uVar4);
  iVar2 = param_1[2] - ((int)param_3[1] + uVar4);
  lVar3 = SquareRoot0(iVar1 * iVar1 + iVar2 * iVar2);
  return 0 < (int)((uVar4 + param_2) - lVar3);
}


