/* FUN_8011b824 @ 0x8011b824  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b824(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_30 [20];
  int local_1c;
  int local_14;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar1 + 0x18,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0xc4,auStack_30);
  iVar1 = iVar1 + param_2 * 0x2b0 + 0x408;
  func_0x80022da0(auStack_30,iVar1 + -0x140,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + -0x94,auStack_30);
  func_0x80022da0(auStack_30,iVar1 + 0x18,auStack_30);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (local_1c - *(int *)(iVar1 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (local_14 - *(int *)(iVar1 + 0x5c));
  return;
}


