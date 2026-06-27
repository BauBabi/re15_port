/* FUN_80047858 @ 0x80047858  (Ghidra headless, raw MIPS overlay) */

uint FUN_80047858(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  long lVar3;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint unaff_s7;
  
  uVar8 = 0x7fffffff;
  if (*(byte *)(param_1 + 8) < 0x10) {
    unaff_s7 = 0;
  }
  else {
    pbVar5 = (byte *)(param_3 + (uint)*(byte *)(param_1 + 8) * 8 + -0x80);
    iVar7 = 4;
    iVar6 = 0;
    do {
      iVar4 = *(int *)(param_1 + 0x198) + (uint)*pbVar5 * 0xac;
      iVar1 = *(int *)(iVar4 + 0x5c) - *(int *)(param_2 + 0x14);
      iVar2 = *(int *)(iVar4 + 100) - *(int *)(param_2 + 0x1c);
      iVar4 = *(int *)(iVar4 + 0x60) - *(int *)(param_2 + 0x18);
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      lVar3 = SquareRoot0(iVar1 * iVar1 + iVar2 * iVar2);
      if ((uint)(iVar4 + lVar3) < uVar8) {
        unaff_s7 = (uint)*pbVar5 | iVar6 << 0x10;
        uVar8 = iVar4 + lVar3;
      }
      pbVar5 = pbVar5 + 2;
      iVar7 = iVar7 + -1;
      iVar6 = iVar6 + 1;
    } while (iVar7 != 0);
  }
  return unaff_s7;
}


