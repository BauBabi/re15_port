/* FUN_80110128 @ 0x80110128  (Ghidra headless overlay RE) */

void FUN_80110128(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = func_0x8001f314(param_1,*(undefined4 *)(param_2 + -0x3430),param_3,0x200);
  uVar2 = (uint)DAT_800aca5a;
  DAT_800aca5a = (byte)(uVar2 + iVar1);
  iVar1 = *(int *)(uVar2 + iVar1 + -0x387c);
  if (*(short *)(iVar1 + 0x1d6) != 0) {
    *(short *)(iVar1 + 0x1de) = *(short *)(iVar1 + 0x1de) + 1;
    FUN_8011120c();
    return;
  }
  *(undefined2 *)(iVar1 + 0x1de) = 0;
  return;
}


