/* FUN_8004fb38 @ 0x8004fb38  (Ghidra headless, raw MIPS overlay) */

int FUN_8004fb38(int param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  long lVar2;
  uint uVar3;
  
  uVar3 = (uint)(*(ushort *)(param_4 + 6) >> 1);
  iVar1 = ((int)*(short *)(param_4 + 2) + uVar3) - *(int *)(param_1 + 8);
  lVar2 = SquareRoot0(uVar3 * uVar3 - iVar1 * iVar1);
  return (param_2 - uVar3) + lVar2;
}


