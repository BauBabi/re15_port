/* FUN_80014b40 @ 0x80014b40  (Ghidra headless, raw MIPS overlay) */

void FUN_80014b40(int param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  
  puVar1 = DAT_800c3a80;
  iVar3 = DAT_800c3a80[1];
  *(undefined4 *)(param_1 + 0x7c) = 0x808080;
  iVar3 = iVar3 + param_2 * 0x38;
  *(int *)(param_1 + 0x14) = iVar3;
  *(int *)(param_1 + 0x1c) = iVar3 + 0x1c;
  uVar2 = FUN_8002cbc4(param_1 + 0x10,*puVar1,0);
  *DAT_800c3a80 = uVar2;
  return;
}


