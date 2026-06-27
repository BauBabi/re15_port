/* FUN_8011a5c0 @ 0x8011a5c0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5c0(void)

{
  undefined2 uVar1;
  int iVar2;
  int in_v1;
  int iVar3;
  int unaff_s3;
  ushort in_stack_0000002c;
  
  iVar3 = (_DAT_800acaf0 - 4) + in_v1 * -2;
  _DAT_800acaf0 = (ushort)iVar3;
  if (iVar3 * 0x10000 < 0) {
    DAT_800aca5a = 4;
    iVar3 = (in_v1 * -2 + 4) * 0x10000 >> 0x10;
    iVar2 = (int)(((uint)in_stack_0000002c - (uint)*(ushort *)(unaff_s3 + 0x5c)) * 0x10000) >> 0x10;
    uVar1 = func_0x80065f60(iVar3 * iVar3 + iVar2 * iVar2);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
    return;
  }
  func_0x800245d8(0);
  return;
}


