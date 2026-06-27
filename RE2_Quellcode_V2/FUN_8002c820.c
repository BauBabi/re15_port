/* FUN_8002c820 @ 0x8002c820  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8002c820(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)*(short *)(param_2 + 4);
  iVar5 = (int)*(short *)(param_2 + 6);
  iVar6 = param_1[2] - iVar5;
  iVar7 = *(short *)(param_2 + 8) - iVar8;
  iVar1 = *param_1 - iVar8;
  iVar2 = *(short *)(param_2 + 10) - iVar5;
  iVar3 = *(short *)(param_2 + 0x10) - iVar8;
  iVar4 = *(short *)(param_2 + 0x12) - iVar5;
  if (iVar7 * iVar6 <= iVar2 * iVar1) {
    if (iVar3 * iVar6 < iVar4 * iVar1) {
      return 0;
    }
    iVar5 = *(short *)(param_2 + 0xe) - iVar5;
    iVar8 = *(short *)(param_2 + 0xc) - iVar8;
    if (((iVar2 - iVar5) * (iVar1 - iVar8) <= (iVar7 - iVar8) * (iVar6 - iVar5)) &&
       ((iVar3 - iVar8) * (iVar6 - iVar5) <= (iVar4 - iVar5) * (iVar1 - iVar8))) {
      return 1;
    }
  }
  return 0;
}


